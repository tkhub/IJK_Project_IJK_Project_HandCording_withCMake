/**
 * @file motor_const.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 14, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "motor_const.h"
/*========AAAA Include Local Header END AAAA=================================*/


const float STOP_NRMPWR_THRESHOLD   = 0.05F;         //
const uint16_t ENCODER_RSOLUTION    = 28;           // エンコーダ分解能*4で28
const uint16_t ENCODER_DIR_GAP      = ((65535 /2 ) - 32); // サンプリング1ms内で回転し得るカウント数以上に変動があったときはオーバーフローしたと判定する
const bool POWER_DIRECTION_INV_L    = false;
const bool POWER_DIRECTION_INV_R    = false;
const bool ENCODER_DIRECTION_INV_L  = true;
const bool ENCODER_DIRECTION_INV_R  = false;
const uint16_t MOTOR_PWM_MAX        = 3199;
const float ENCODER_IIR_GAIN = 0.5F;
const float MOTOR_INTR_CYCLE_MS    = 1.0F;          // モータ制御周期(ms)
