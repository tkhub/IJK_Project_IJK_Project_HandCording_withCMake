/**
 * @file motor.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 14, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>
#include <math.h>
#include <stdint.h>
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "motor.h"
#include "motor_const.h"
#include "motor_private.h"

#include "stm32f303x8.h"
#include "tim.h"
#include "gpio.h"

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
#include <xprintf.h>
#include <string.h>
#endif /* SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST */

/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/

/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
#define CH_LEFT     0
#define CH_RIGHT    1


/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/* int global_var; */ /* ヘッダファイルで説明済みのためDoxygenのコメントは不要 */

/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/
/**
 * @brief 内部グローバル変数 internal_var の説明
*/

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

//! エンコーダのカウント値の差分のIIRフィルタ値
volatile static float           encoderDeltaIIR[2];

volatile static uint16_t        encoderLast[2];

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
volatile static uint16_t testCnt;
volatile static int16_t testModePowerLeft;
volatile static int16_t testModePowerRight;
static const uint16_t TEST_HOLD_TIME_MS = (uint16_t)((float)(5.0F)/(50.0F * 0.001F));
static const uint8_t TEST_STEP = 5;
typedef enum {
    LEFT_FORWARD_RIGHT_BACKWARD_UP,
    LEFT_FORWARD_RIGHT_BACKWARD_DOWN,
    LEFT_BACKWARD_RIGHT_FORWARD_UP,
    LEFT_BACKWARD_RIGHT_FORWARD_DOWN
}testMode_t;

volatile static testMode_t testMode;



#endif /* SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST */

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/**
 * @fn int encoderCalcStep(const uint16_t now, const uint16_t last, const uint16_t gapth, const bool invertFlag)
 * @brief エンコーダのステップ数を計算する関数
 * @param [in] now 現在のエンコーダ値
 * @param [in] last 前回のエンコーダ値
 * @param [in] gapth オーバーフローと判定するギャップ
 * @param [in] invertFlag 反転フラグ
 * @return ステップ数
 * @details 詳細な説明
 */
static int32_t encoderCalcStep(const uint16_t now, const uint16_t last, const uint16_t ovfth, const bool invertFlag);


/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
void motorsInit(void) {
    // モータ停止
    motorsDrive(0.0F, 0.0F);
    // バッファ・オドメトリ初期化
    motorsResetRound();

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
    testCnt = 0;
    testModePowerLeft = 0;
    testModePowerRight = 0;
    testMode = LEFT_FORWARD_RIGHT_BACKWARD_UP;
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
    for (i = 0; i < MOTOR_RPS_RECORD_SIZE; i++) {
        //! バッファの新しい方から重みをつけて加算
        sumL += encoderDeltaBuffer[CH_LEFT][i]  * (((MOTOR_RPS_RECORD_SIZE - 1 - encoderBufferIdx + i) % MOTOR_RPS_RECORD_SIZE) + 1);
        sumR += encoderDeltaBuffer[CH_RIGHT][i] * (((MOTOR_RPS_RECORD_SIZE - 1 - encoderBufferIdx + i) % MOTOR_RPS_RECORD_SIZE) + 1);
    }
    //! 平均結果(重み付き）をIIRフィルタに通す
    //! MOTOR_RPS_RECORD_SIZE = 8で、各カウントが{12,12,12,12,12,12,12,12}のとき、重み付きsumは432。2倍して864で、(8*9)で割ると12となる。
    encoderDeltaIIR[CH_LEFT]    = (((float)sumL * 2.0f / (MOTOR_RPS_RECORD_SIZE * (MOTOR_RPS_RECORD_SIZE + 1))) - encoderDeltaIIR[CH_LEFT])
                                    * ENCODER_IIR_GAIN + encoderDeltaIIR[CH_LEFT];
    encoderDeltaIIR[CH_RIGHT]   = (((float)sumR * 2.0f / (MOTOR_RPS_RECORD_SIZE * (MOTOR_RPS_RECORD_SIZE + 1))) - encoderDeltaIIR[CH_RIGHT])
                                    * ENCODER_IIR_GAIN + encoderDeltaIIR[CH_RIGHT];

    // 積算回転量更新
    encoderOdd[CH_LEFT] += deltaL;
    encoderOdd[CH_RIGHT] += deltaR;

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
#endif
}

void motorsDrive(const float nrmPwrL, const float nrmPwrR) {
    float powerL, powerR;
    if (1.0F < nrmPwrL) {
        powerL = 1.0F;
    }
    else if (nrmPwrL < -1.0F) {
        powerL = -1.0F;
    }
    else if ( (-STOP_NRMPWR_THRESHOLD < nrmPwrL) && (nrmPwrL < STOP_NRMPWR_THRESHOLD) ) {
        powerL = 0.0F;
    }
    else {
        powerL = nrmPwrL;
    }
    if (1.0F < nrmPwrR) {
        powerR = 1.0F;
    }
    else if (nrmPwrR < -1.0F) {
        powerR = -1.0F;
    }
    else if ( (-STOP_NRMPWR_THRESHOLD < nrmPwrR) && (nrmPwrR < STOP_NRMPWR_THRESHOLD) ) {
        powerR = 0.0F;
    }
    else {
        powerR = nrmPwrR;
    }

    __disable_irq();
    motorsPower[MOTOR_L] = powerL;
    motorsPower[MOTOR_R] = powerR;
    if (powerL < 0.0F) {
        // 左後進
        powerL = -1.0F * powerL;
        if (!POWER_DIRECTION_INV_L)
        {
            HAL_GPIO_WritePin(DIR_L_GPIO_Port,DIR_L_Pin, MOTOR_CCW_L);
        }
        else
        {
            HAL_GPIO_WritePin(DIR_L_GPIO_Port,DIR_L_Pin,MOTOR_CW_L);
        }
        __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, (uint16_t)(3199 * powerL) );
    }
    else
    {
        // 左前進
        powerL = 1.0F * powerL;
        if (!POWER_DIRECTION_INV_L)
        {
            HAL_GPIO_WritePin(DIR_L_GPIO_Port,DIR_L_Pin,MOTOR_CW_L);
        }
        else
        {
            HAL_GPIO_WritePin(DIR_L_GPIO_Port,DIR_L_Pin,MOTOR_CCW_L);
        }
        __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, (uint16_t)(3199 * powerL) );
    }

    if (powerR < 0.0F) {
        // 左後進
        powerR = -1.0F * powerR;
        if (!POWER_DIRECTION_INV_R)
        {
            HAL_GPIO_WritePin(DIR_R_GPIO_Port,DIR_R_Pin, MOTOR_CCW_R);
        }
        else
        {
            HAL_GPIO_WritePin(DIR_R_GPIO_Port,DIR_R_Pin, MOTOR_CW_R);
        }
        __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (uint16_t)(3199 * powerR) );
    }
    else {
        // 左前進
        powerR = 1.0F * powerR;
        if (!POWER_DIRECTION_INV_R) {
            HAL_GPIO_WritePin(DIR_R_GPIO_Port,DIR_R_Pin,MOTOR_CW_R);
        }
        else {
            HAL_GPIO_WritePin(DIR_R_GPIO_Port,DIR_R_Pin,MOTOR_CCW_R);
        }
        __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (uint16_t)(3199 * powerR) );
    }
    __enable_irq();
}

void motorsResetRound(void)
{
    int i,j;
    __disable_irq();
    encoderOdd[CH_LEFT] = 0;
    encoderOdd[CH_RIGHT] = 0;
    encoderBufferIdx = 0;
    encoderLast[CH_LEFT] = (uint16_t)__HAL_TIM_GET_COUNTER(&htim1);
    encoderLast[CH_RIGHT] = (uint16_t)__HAL_TIM_GET_COUNTER(&htim2);
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < MOTOR_RPS_RECORD_SIZE; j++)
        {
            encoderDeltaBuffer[i][j] = 0;
        }
        encoderDeltaIIR[i] = 0.0f;
    }
    __enable_irq();
}

void motorsReadRps(float* rpsL, float* rpsR)
{
    //! 割り込み周期間でのエンコーダカウント数を割り込み周期で割り、さらにエンコーダ分解能で割ることで回転数を算出
    //! 例：カウントが50で、割り込み周期が1ms、エンコーダ分解能が28の時、
    __disable_irq();
    *rpsL = encoderDeltaIIR[CH_LEFT]    * (float)1.0F  / (MOTOR_INTR_CYCLE_MS * 0.001F) / (float)ENCODER_RSOLUTION;
    *rpsR = encoderDeltaIIR[CH_RIGHT]   * (float)1.0F  / (MOTOR_INTR_CYCLE_MS * 0.001F) / (float)ENCODER_RSOLUTION;
    __enable_irq();
}

void motorsReadPower(float* nrmPwrL, float* nrmPwrR)
{
    __disable_irq();
    *nrmPwrL = motorsPower[MOTOR_L];
    *nrmPwrR = motorsPower[MOTOR_R];
    __enable_irq();
}

void motorsReadRound(float* roundL, float* roundR)
{
    __disable_irq();
    *roundL = (float)encoderOdd[CH_LEFT] / ENCODER_RSOLUTION;
    *roundR = (float)encoderOdd[CH_RIGHT] / ENCODER_RSOLUTION;
    __enable_irq();
}

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
// #define __ENCODER_TEST__
#ifndef __ENCODER_TEST__
uint8_t motorTest(char* strBuffer, uint8_t maxBufferSize)
{
    float pwrL, pwrR;
    float roundL, roundR;
    float rpsL, rpsR;

    if (TEST_HOLD_TIME_MS < testCnt)
    {
        testCnt = 0;
        switch (testMode)
        {
            case LEFT_FORWARD_RIGHT_BACKWARD_UP:
                // Left 0 ~ +100, Right 0 ~ -100
                testModePowerLeft += TEST_STEP;
                testModePowerRight -= TEST_STEP;
                if ( (100 < testModePowerLeft) || (testModePowerRight < -100) )
                {
                    testModePowerLeft = 100;
                    testModePowerRight = -100;
                    testMode = LEFT_FORWARD_RIGHT_BACKWARD_DOWN;
                }
                break;

            case LEFT_FORWARD_RIGHT_BACKWARD_DOWN:
                // Left +100 ~ 0, Right -100 ~ 0
                testModePowerLeft -= TEST_STEP;
                testModePowerRight += TEST_STEP;
                if ( (testModePowerLeft < 0) || (0 < testModePowerRight) )
                {
                    testModePowerLeft = 0;
                    testModePowerRight = 0;
                    testMode = LEFT_BACKWARD_RIGHT_FORWARD_UP;
                }
                break;

            case LEFT_BACKWARD_RIGHT_FORWARD_UP:
                // Left 0 ~ -100, Right 0 ~ +100
                testModePowerLeft -= TEST_STEP;
                testModePowerRight += TEST_STEP;
                if ( (testModePowerLeft < -100) || (100 < testModePowerRight) )
                {
                    testModePowerLeft = -100;
                    testModePowerRight = 100;
                    testMode = LEFT_BACKWARD_RIGHT_FORWARD_DOWN;
                }
                break;

            case LEFT_BACKWARD_RIGHT_FORWARD_DOWN:
                // Left -100 ~ 0, Right +100 ~ 0
                testModePowerLeft += TEST_STEP;
                testModePowerRight -= TEST_STEP;
                if ( (0 < testModePowerLeft) || ( testModePowerRight < 0) )
                {
                    testModePowerLeft = 0;
                    testModePowerRight = 0;
                    testMode = LEFT_FORWARD_RIGHT_BACKWARD_UP;
                }
                break;
            default:
                testMode = LEFT_FORWARD_RIGHT_BACKWARD_UP;
                break;
        }
    }
    else
    {
        testCnt++;
    }
    motorsDrive((float)testModePowerLeft / 100.0F, (float)testModePowerRight/ 100.0F);
    motorsReadPower(&pwrL, &pwrR);
    motorsReadRound(&roundL, &roundR);
    motorsReadRps(&rpsL, &rpsR);
    xsnprintf(strBuffer, maxBufferSize,
        ",%d,%d,%f,%f,%f,%f,%f,%f",
        testMode, testCnt, pwrL, pwrR, rpsL,rpsR, roundL,roundR);
    return 0;
}
#else /* __ENCODER_TEST__ */
uint8_t motorTest(char* strBuffer, uint8_t maxBufferSize)
{
    int8_t encpwrcnt = 0;
    uint8_t cnt;
    for (encpwrcnt = -100; encpwrcnt <= 100; encpwrcnt += 20)
    {
        printf("[pwr = %d] : Left {\n\r", encpwrcnt);
        motorsDrive((float)-encpwrcnt/ 100.0F, (float)encpwrcnt/ 100.0F);
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
    return xsnprintf(strBuffer, maxBufferSize,"ENCTEST END");
}
#endif /* __ENCODER_TEST__ */
#endif /* SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST */

/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/

static int32_t encoderCalcStep(const uint16_t now, const uint16_t last, const uint16_t ovfth, const bool invertFlag)
{
    uint16_t tmp;
    bool reverse;
    if (now > last)
    {
        if ((now - last) > ovfth)
        {
            // 1サンプリングでの回転許容量を超えているため、オーバーフローしたと判断
            reverse = true;
            tmp = last - now;
        }
        else
        {
            // オーバーフローしないときの正転
            reverse = false;
            tmp = now - last;
        }
    }
    else
    {
        // last > now
        if ((last - now) > ovfth)
        {
            // 1サンプリングでの回転許容量を超えているため、オーバーフローしたと判断
            reverse = false;
            tmp = now - last;
        }
        else
        {
            reverse = true;
            tmp = last - now;
        }
    }
    if (!invertFlag)
    {
        if (!reverse)
        {
            return (int32_t)tmp;
        }
        else
        {
            return (int32_t)-1 * tmp;
        }
    }
    else
    {
        if (!reverse)
        {
            return (int32_t)-1 * tmp;
        }
        else
        {
            return (int32_t)tmp;
        }
    }
}

/*========AAAA Private Function Definition END AAAA==========================*/
