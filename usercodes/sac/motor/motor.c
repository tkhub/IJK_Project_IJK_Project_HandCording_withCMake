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
/** 
 * @brief マクロ MAX_NANK の説明
 */
/* # define MAX_NANKA 256 */

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/* int global_var; */ /* ヘッダファイルで説明済みのためDoxygenのコメントは不要 */

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

//! 左エンコーダのカウント量差分のバッファ
volatile static int16_t         encoderStepBufferL[MOTOR_RPS_RECORD_SIZE];
//! 右エンコーダのカウント量差分のバッファ
volatile static int16_t         encoderStepBufferR[MOTOR_RPS_RECORD_SIZE];

//! 左エンコーダの前回カウント値の保存先
volatile static uint16_t        encoderL_last;
//! 左エンコーダの前回カウント値の保存先
volatile static uint16_t        encoderR_last;

//! エンコーダステップ数のバッファの合計
volatile static int32_t         encoderSum[2];

//! 積算回転量
volatile static int32_t         encoderOddL;
volatile static int32_t         encoderOddR;


#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
static float testPower;
static int8_t testCnt;
static int16_t testCntIntr;
static bool testUpDown;
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
    motorsDriveManual(0.0, 0.0);
    encoderL_last = 0x0000;
    encoderR_last = 0x0000;
#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
    testPower = 0.0;
    testCnt = 0;
    testCntIntr = 0;
    testUpDown = true;
#endif /* SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST */
}

void motorsControl_1ms(void) {
    uint16_t encL, encR, cnt;
    int32_t dencLsum = 0, dencRsum = 0;

    encL = __HAL_TIM_GET_COUNTER(&htim1);
    encR = __HAL_TIM_GET_COUNTER(&htim2);

    for (cnt = 1; cnt < MOTOR_RPS_RECORD_SIZE; cnt++) {
        encoderStepBufferL[cnt] = encoderStepBufferL[cnt - 1];
        dencLsum += encoderStepBufferL[cnt];
        encoderStepBufferR[cnt] = encoderStepBufferR[cnt - 1];
        dencRsum += encoderStepBufferR[cnt];
    }
    encoderStepBufferL[0] = encoderCalcStep(encL, encoderL_last, ENCODER_DIR_GAP, ENCODER_DIRECTION_INV_L);
    encoderStepBufferR[0] = encoderCalcStep(encR, encoderR_last, ENCODER_DIR_GAP, ENCODER_DIRECTION_INV_R);
    encoderL_last = encL;
    encoderR_last = encR;

    dencLsum += encoderStepBufferL[0];
    dencRsum += encoderStepBufferR[0];

    encoderSum[MOTOR_L] = dencLsum;
    encoderSum[MOTOR_R] = dencRsum;

    encoderOddL += encoderStepBufferL[0];
    encoderOddR += encoderStepBufferR[0];

#ifdef _ENABLE_MOTOR_TEST_
    testCntIntr++;
#endif /* _ENABLE_MOTOR_TEST_ */
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
    *rpsL = (float)(1000 * encoderSum[MOTOR_L] / MOTOR_RPS_RECORD_SIZE) / ENCODER_RSOLUTION;
    *rpsR = (float)(1000 * encoderSum[MOTOR_R] / MOTOR_RPS_RECORD_SIZE) / ENCODER_RSOLUTION;
}

void motorsReadPower(float* nrmPwrL, float* nrmPwrR) {
    *nrmPwrL = motorsPower[MOTOR_L];
    *nrmPwrR = motorsPower[MOTOR_R];
}

void motorsReadRound(float* roundL, float* roundR) {
    *roundL = (float)encoderOddL / ENCODER_RSOLUTION;
    *roundR = (float)encoderOddR / ENCODER_RSOLUTION;
}

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
uint8_t motorTest(char* strBuffer, uint8_t maxBufferSize) {
    float pwrL, pwrR;
    float roundL, roundR;
    float rpsL, rpsR;

    if (testCnt == -100) {
        testUpDown = true;
    }
    else if (testCnt == 100) {
        testUpDown = false;
    }
    else {
        /* NOP */
    }
    if (testUpDown == true) {
        testCnt++;
    }
    else {
        testCnt--;
    }
    motorsDriveManual((float)-testCnt / 100.0f, (float)testCnt / 100.0f);
    motorsReadPower(&pwrL, &pwrR);
    motorsReadRound(&roundL, &roundR);
    motorsReadRps(&rpsL, &rpsR);
    return snprintf(strBuffer, maxBufferSize,
            "cnt = %d, pwrL,R = %f,%f, rpsL,R = %f,%f, roundL,R = %f,%f",
            testCnt, pwrL, pwrR, rpsL,rpsR, roundL,roundR);
}
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
