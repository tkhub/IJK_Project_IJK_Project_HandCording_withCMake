/**
 * @file battery_const.c
 * @brief 簡単な説明(定数定義)
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "battery_const.h"
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/** 
 * @brief マクロ MAX_NANK の説明
 */
/* # define MAX_NANKA 256 */

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
const float BATTERY_GAIN    = +0.002426f;
const float BATTERY_OFFSET  = +0.167f;
// 実験結果
// 3210 = 7.959V
// 2795 = 6.957V
// 2385 = 5.954V
// 1670 = 4.949V
// その後、微調整

const float BATTERY_IIRLPF_GAIN = 0.25f;


const BatteryThresholdsTables_t BATTERY_THRESHOLDS_TABLES = {
/*                                                              */
.rise_thresholds = {/*ERROR_L*/ 4.4F,/*AAA_EMPTY*/ 1.15F*4.0F,/*AAA_WARNING*/ 1.20F*4.0F,/*AAA_NORMAL*/ 1.25F*4.0F,/*AAA_FULL*/ \
                    /*AAA_FULL*/ 1.600F*4.0f,/*LIPO_EMPTY*/ 3.50F * 2.0f,/*LIPO_WARNING*/ 3.60F*2.0F,/*LIPO_NORMAL*/ 3.70F*2.0F,/*LIPO_FULL*/ 4.45F*2.0F,/*ERROR_Hi*/ 10.0f},
.fall_thresholds = {/*ERROR_L*/ 4.35F,/*AAA_EMPTY*/ 1.10F*4.0f,/*AAA_WARNING*/ 1.15F*4.0f,/*AAA_NORMAL*/ 1.20f*4.0F,/*AAA_FULL*/ \
                    /*AAA_FULL*/ 1.450F*4.0F,/*LIPO_EMPTY*/ 3.325F * 2.0f,/*LIPO_WARNING*/ 3.50F*2.0F,/*LIPO_NORMAL*/ 3.65F*2.0F,/*LIPO_FULL*/ 4.35F*2.0F,/*ERROR_Hi*/ 9.0F}
};

const uint16_t BATTERY_MASK_10MSCNT  = 100;
const float BATTERY_INIT_VOL         = 6.75f;

/*========AAAA GLOBAL Variable Definition END AAAA===========================*/
