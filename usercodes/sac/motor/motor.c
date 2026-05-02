/**
 * @file motor.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 14, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "motor.h"
#include "motor_const.h"
#include "motor_private.h"

#include "tim.h"
#include "gpio.h"

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
#include <stdio.h>
#include <string.h>
#endif /* SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST */

/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/**
 * @brief controlstate_t 型の構造体
 */
typedef enum {
    MANUAL,     //! 手動制御
    FEEDBACK    //! F/B制御
} controlstate_t;

/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
#define CH_LEFT     0
#define CH_RIGHT    1


/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/* int global_var; */ /* ヘッダファイルで説明済みのためDoxygenのコメントは不要 */
static const float PPS_TO_RPS = 1.0f; // 仮の値。実際には、エンコーダの分解能やサンプリング周期に基づいて計算する必要がある

/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/
/**
 * @brief 内部グローバル変数 internal_var の説明  
*/
//! モータの制御モード
volatile static controlstate_t  cntrlMode;

//! モータの回転数
volatile static float           motorsRps[2];
//! モータの回転量
volatile static float           motorsRound[2];
//! モータの出力
volatile static float           motorsPower[2];

//! 積算回転量
volatile static int32_t         encoderOdd[2];

//! 左エンコーダのカウント量差分のバッファ
volatile static int32_t         encoderDeltaBuffer[2][MOTOR_RPS_RECORD_SIZE];
//! バッファインデックス
volatile static uint8_t         encoderBufferIdx;

volatile static float           encoderDeltaIIR[2];

//! 最終出力(RPS: 回/秒)

volatile static uint16_t        encoderLast[2];

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
volatile static float testPower;
volatile static int16_t testCnt;
volatile static int8_t testCnt2;
volatile static int16_t testCntIntr;
volatile static bool testUpDown;
#endif /* SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST */

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/**
 * @fn int testFunc(int* buffer, int size)
 * @brief 内部テスト関数
 * @param [in/out] buffer バッファ
 * @param [in] size バッファのサイズ
 * @return 戻り値の説明
 * @detail 詳細な説明
 */
/**
 * static int internalTestFunc(int* buffer, int size)
*/
void motorDrive(const float nrmPwrL, const float nrmPwrR);
int32_t encoderCalcStep(const uint16_t now, const uint16_t last, const uint16_t gapth, const bool invertFlag);


/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
void motorsInit(void) {
    uint8_t i;
    motorsDriveManual(0.0, 0.0);
    
    // バッファ初期化
    for (i = 0; i < MOTOR_RPS_RECORD_SIZE; i++) {
        encoderDeltaBuffer[CH_LEFT][i] = 0;
        encoderDeltaBuffer[CH_RIGHT][i] = 0;
    }
    encoderBufferIdx = 0;
    
    // フィルタ状態初期化
    encoderDeltaIIR[CH_LEFT] = 0.0f;
    encoderDeltaIIR[CH_RIGHT] = 0.0f;
    
#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
    testPower = 0.0;
    testCnt = 0;
    testCnt2 = 0;
    testCntIntr = 0;
    testUpDown = true;
#endif /* SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST */
}

void motorsControl_1ms(void) {
    static uint16_t encL, encR;
    static int32_t deltaL, deltaR;
    uint8_t i;
    int32_t sumL = 0, sumR = 0;
    
    // エンコーダのカウント値を取得
    encL = (uint16_t)__HAL_TIM_GET_COUNTER(&htim1);
    encR = (uint16_t)__HAL_TIM_GET_COUNTER(&htim2);
    
    // 差分計算(オーバーフロー対応)
    deltaL = encoderCalcStep(encL, encoderLast[CH_LEFT], ENCODER_DIR_GAP, ENCODER_DIRECTION_INV_L);
    deltaR = encoderCalcStep(encR, encoderLast[CH_RIGHT], ENCODER_DIR_GAP, ENCODER_DIRECTION_INV_R);
    
    // 最終値を保存
    encoderLast[CH_RIGHT] = encR;
    encoderLast[CH_LEFT] = encL;
    
    // リングバッファに差分を格納
    encoderDeltaBuffer[CH_LEFT][encoderBufferIdx] = deltaL;
    encoderDeltaBuffer[CH_RIGHT][encoderBufferIdx] = deltaR;
    encoderBufferIdx = (encoderBufferIdx + 1) % MOTOR_RPS_RECORD_SIZE;
    
    // ===== 移動平均フィルタ =====
    // 計算コスト: O(N) where N = MOTOR_RPS_RECORD_SIZE
    for (i = 0; i < MOTOR_RPS_RECORD_SIZE; i++) {
        sumL += encoderDeltaBuffer[CH_LEFT][i];
        sumR += encoderDeltaBuffer[CH_RIGHT][i];
    }
    
    encoderDeltaIIR[CH_LEFT] = ((float)sumL - encoderDeltaIIR[CH_LEFT]) * ENCODER_IIR_GAIN + encoderDeltaIIR[CH_LEFT];
    encoderDeltaIIR[CH_RIGHT] = ((float)sumR - encoderDeltaIIR[CH_RIGHT]) * ENCODER_IIR_GAIN + encoderDeltaIIR[CH_RIGHT];
    
    // 積算回転量更新
    encoderOdd[CH_LEFT] += deltaL;
    encoderOdd[CH_RIGHT] += deltaR;

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
    testCntIntr++;
#endif
}

void motorsDriveManual(const float nrmPwrL, const float nrmPwrR) {
    cntrlMode = MANUAL;
    motorDrive(nrmPwrL, nrmPwrR);
}

void motorsDriveFB(const float rpsL, const float rpsR) {
    cntrlMode = FEEDBACK;
}

void motorsResetRound(void) {

}

void motorsReadRps(float* rpsL, float* rpsR) {
    // フィルタ済みRPSを返す(移動平均+三角窓+IIRフィルタ適用済み)
    *rpsL = encoderDeltaIIR[CH_LEFT];
    *rpsR = encoderDeltaIIR[CH_RIGHT];
}

void motorsReadPower(float* nrmPwrL, float* nrmPwrR) {
    *nrmPwrL = motorsPower[MOTOR_L] * PPS_TO_RPS;
    *nrmPwrR = motorsPower[MOTOR_R] * PPS_TO_RPS;
}

void motorsReadRound(float* roundL, float* roundR) {
    *roundL = (float)encoderOdd[CH_LEFT] / ENCODER_RSOLUTION;
    *roundR = (float)encoderOdd[CH_RIGHT] / ENCODER_RSOLUTION;
}

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
// #define __ENCODER_TEST__
#ifndef __ENCODER_TEST__
uint8_t motorTest(char* strBuffer, uint8_t maxBufferSize) {
    float pwrL, pwrR;
    float roundL, roundR;
    float rpsL, rpsR;
    if (100 < testCnt)
    {
        testCnt = 0;
        if (testCnt2 < -100) {
            testCnt2 = -100;
            testUpDown = true;
        }
        else if (testCnt2 > 100) {
            testCnt2 = 100;
            testUpDown = false;
        }
        if (testCnt2 == -100) {
            testUpDown = true;
        }
        else if (testCnt2 == 100) {
            testUpDown = false;
        }
        else {
            /* NOP */
        }
        if (testUpDown == true) {
            testCnt2 += 10;
        }
        else {
            testCnt2 -= 10;
        }
    }
    else
    {
        testCnt++;
    }
    motorsDriveManual((float)-testCnt2 / 100.0f, (float)testCnt2 / 100.0f);
    motorsReadPower(&pwrL, &pwrR);
    motorsReadRound(&roundL, &roundR);
    motorsReadRps(&rpsL, &rpsR);
    // return snprintf(strBuffer, maxBufferSize,
    //         "cnt = %d, pwrL,R = %f,%f, rpsL,R = %f,%f, roundL,R = %f,%f",
    //         testCnt, pwrL, pwrR, rpsL,rpsR, roundL,roundR);
    return snprintf(strBuffer, maxBufferSize,
            ",%d,%f,%f,%f,%f,%f,%f",
            testCnt, pwrL, pwrR, rpsL,rpsR, roundL,roundR);
}
#else /* __ENCODER_TEST__ */
uint8_t motorTest(char* strBuffer, uint8_t maxBufferSize) {
    int8_t encpwrcnt = 0;
    uint8_t cnt;
    for (encpwrcnt = -100; encpwrcnt <= 100; encpwrcnt += 20)
    {
        printf("[pwr = %d] : Left {\n\r", encpwrcnt);
        motorsDriveManual((float)-encpwrcnt/ 100.0f, (float)encpwrcnt/ 100.0f);
        HAL_Delay(5000);
        for (cnt = 0; cnt < MOTOR_RPS_RECORD_SIZE; cnt++)
        {
            printf("%lu\n\r", encoderStepBuffer[CH_LEFT][cnt]);
        }
        printf("}\n\r : Right {\n\r");
        for (cnt = 0; cnt < MOTOR_RPS_RECORD_SIZE; cnt++)
        {
            printf("%lu\n\r", encoderStepBuffer[CH_RIGHT] [cnt]);
        }
        printf("}\n\r");
    }
    return snprintf(strBuffer, maxBufferSize,"ENCTEST END");
}

#endif /* __ENCODER_TEST__ */
#endif /* SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST */

/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
void motorDrive(const float nrmPwrL, const float nrmPwrR) {
    float powerL, powerR;
    if (1.0f < nrmPwrL) {
        powerL = 1.0f;
    }
    else if (nrmPwrL < -1.0) {
        powerL = -1.0f;
    }
    else if ( (-STOP_NRMPWR_THRESHOLD < nrmPwrL) && (nrmPwrL < STOP_NRMPWR_THRESHOLD) ) {
        powerL = 0.0f;
    }
    else {
        powerL = nrmPwrL;
    }



    if (1.0f < nrmPwrR) {
        powerR = 1.0f;
    }
    else if (nrmPwrR < -1.0f) {
        powerR = -1.0f;
    }
    else if ( (-STOP_NRMPWR_THRESHOLD < nrmPwrR) && (nrmPwrR < STOP_NRMPWR_THRESHOLD) ) {
        powerR = 0.0f;
    }
    else {
        powerR = nrmPwrR;
    }

    if (powerL < 0) {
        // 左後進
        powerL = -1.0f * powerL;
        if (!POWER_DIRECTION_INV_L) {
            HAL_GPIO_WritePin(DIR_L_GPIO_Port,DIR_L_Pin, MOTOR_CCW_L);
        }
        else {
            HAL_GPIO_WritePin(DIR_L_GPIO_Port,DIR_L_Pin,MOTOR_CW_L);
        }
        __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, (uint16_t)(3199 * powerL) );
    }
    else {
        // 左前進
        powerL = 1.0f * powerL;
        if (!POWER_DIRECTION_INV_L) {
            HAL_GPIO_WritePin(DIR_L_GPIO_Port,DIR_L_Pin,MOTOR_CW_L);
        }
        else {
            HAL_GPIO_WritePin(DIR_L_GPIO_Port,DIR_L_Pin,MOTOR_CCW_L);
        }
        __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, (uint16_t)(3199 * powerL) );
    }

    if (powerR < 0) {
        // 左後進
        powerR = -1.0f * powerR;
        if (!POWER_DIRECTION_INV_R) {
            HAL_GPIO_WritePin(DIR_R_GPIO_Port,DIR_R_Pin, MOTOR_CCW_R);
        }
        else {
            HAL_GPIO_WritePin(DIR_R_GPIO_Port,DIR_R_Pin, MOTOR_CW_R);
        }
        __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (uint16_t)(3199 * powerR) );
    }
    else {
        // 左前進
        powerR = 1.0 * powerR;
        if (!POWER_DIRECTION_INV_R) {
            HAL_GPIO_WritePin(DIR_R_GPIO_Port,DIR_R_Pin,MOTOR_CW_R);
        }
        else {
            HAL_GPIO_WritePin(DIR_R_GPIO_Port,DIR_R_Pin,MOTOR_CCW_R);
        }
        __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (uint16_t)(3199 * powerR) );
    }
    motorsPower[MOTOR_L] = powerL;
    motorsPower[MOTOR_R] = powerR;
}

int32_t encoderCalcStep(const uint16_t now, const uint16_t last, const uint16_t gapth, const bool invertFlag) {
    uint16_t tmp;
    bool reverse;
    if (now > last) {
        if ((now - last) > gapth) {
            // 1サンプリングでの回転許容量を超えているため、オーバーフローしたと判断
            reverse = true;
            tmp = last - now;
        }
        else {
            // オーバーフローしないときの正転
            reverse = false;
            tmp = now - last;
        }
    }
    else {
        // last > now
        if ((last - now) > gapth) {
            // 1サンプリングでの回転許容量を超えているため、オーバーフローしたと判断
            reverse = false;
            tmp = now - last;
        }
        else {
            reverse = true;
            tmp = last - now;
        }
    }
    if (!invertFlag) {
        if (!reverse) {
            return (int32_t)tmp;
        }
        else {
            return (int32_t)-1 * tmp;
        }
    }
    else {
        if (!reverse) {
            return (int32_t)-1 * tmp;
        }
        else {
            return (int32_t)tmp;
        }
    }
}

/*========AAAA Private Function Definition END AAAA==========================*/
