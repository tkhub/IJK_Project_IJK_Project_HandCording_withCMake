/**
 * @file battery.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "battery.h"
#include "battery_const.h"
#include <string.h>
#include "orgtypedef.h"

#if SAC_DEBUGMODE == DEBUGMODE_BATTERY_TEST
#include <stdio.h>
#include <stdlib.h>
#endif /* SAC_DEBUGMODE == DEBUGMODE_BATTERY_TEST */

/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/

/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/

//! @brief バッテリー計測結果が格納されるアドレス
volatile static uint16_t* batteryDMARaw;

//! @brief 現在のバッテリ電圧のADC値
volatile static uint16_t batteryRaw;

//! @brief バッテリーの状態の判定結果
volatile static batterystatus_t batteryState;

//! @brief バッテリー状態サーチ用のインデックス
static int8_t batteryStateLevel;

//! @brief バッテリー電圧
volatile static float batteryVoltage;

//! @brief バッテリー判定のマスク時間
static uint16_t batteryMaskCnt;

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/**
 * @brief バッテリーレベル判定関数
 * @param [in] bVol 現在の電圧
 * @param [in] nowLevel 現在のレベル
 * @return 判定後のレベル
 * @details 配列でサーチするためレベル（数字）で判断する
 */
int8_t batteryLevelDetection(const float bVol, const int8_t nowLevel);

/**
 * @brief バッテリーレベルからステート変換関数
 * @param [in] level 判定したいレベル
 * @return 変換後のステート
 * @details Enum型を直接配列のサーチに使いたくないので変換する
 */
batterystatus_t batteryLevel2batteryState(const int8_t level);

/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
void batteryInit(volatile uint16_t* battaryDMA) {
    batteryState = BATTERY_NON_DETECTED;
    batteryVoltage = BATTERY_INIT_VOL;
    batteryDMARaw = battaryDMA;
    batteryStateLevel = 0;
    batteryMaskCnt = BATTERY_MASK_10MSCNT;
}

void battery_10ms(void) {
    batteryRaw = *batteryDMARaw;
    // ADC生値からバッテリー電圧を求めてローパスフィルタに掛ける
    batteryVoltage = batteryVoltage + ( ( (((float)batteryRaw * BATTERY_GAIN) + BATTERY_OFFSET) - batteryVoltage) * BATTERY_IIRLPF_GAIN);

    //　バッテリー状態判定マスク状態ではない場合、バッテリー状態を判定する。
    if (batteryMaskCnt == 0) {
        batteryMaskCnt = 0;
        batteryStateLevel = batteryLevelDetection(batteryVoltage, batteryStateLevel);
        batteryState = batteryLevel2batteryState(batteryStateLevel);
    }
    else {
        batteryMaskCnt--;
    }
}

float batteryRead(void) {
    return batteryVoltage;
}

batterystatus_t batteryReadStateatus(void) {
    return batteryState;
}


uint8_t batteryState2String(batterystatus_t btst, char * str) {
    switch (btst) {
    case BATTERY_LIPO_FULL:
        strncpy(str, "LIPO-FULL", 10);
        break;
    case BATTERY_LIPO_NORM:
        strncpy(str, "LIPO-NORMAL", 12);
        break;
    case BATTERY_LIPO_WRN:
        strncpy(str, "LIPO-WARNING", 13);
        break;
    case BATTERY_LIPO_EMPTY:
        strncpy(str, "LIPO-EMPTY", 11);
        break;
    case BATTERY_AAA_FULL:
        strncpy(str, "AAA-FULL", 9);
        break;
    case BATTERY_AAA_NORM:
        strncpy(str, "AAA-NORMAL", 11);
        break;
    case BATTERY_AAA_WRN:
        strncpy(str, "AAA-WARNING", 12);
        break;
    case BATTERY_AAA_EMPTY:
        strncpy(str, "AAA-EMPTY", 10);
        break;
    case BATTERY_NON_DETECTED:
            strncpy(str, "?NON_DETECTED?", 15);
            break;
    // エラーにはHI/LO両方あるが、本来は入らないはずの状態なので一律エラーとする。
    default :
        strncpy(str, "!BATTERYERROR!", 15);
        break;
    }
    return strlen(str);
}

#if SAC_DEBUGMODE == DEBUGMODE_BATTERY_TEST
uint8_t batteryTest(char* strBuffer, uint8_t maxBufferSize) {
    char batteryStr[16];
    batterystatus_t btst;
    btst = batteryReadStateatus();
    batteryState2String(btst, batteryStr);
    return snprintf(strBuffer, maxBufferSize, "Vbat = %f(%d),\t\tState = %s(%d,%d)", batteryRead(), batteryRaw, batteryStr, batteryStateLevel, btst);
}
#endif /* SAC_DEBUGMODE == DEBUGMODE_BATTERY_TEST */


/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
int8_t batteryLevelDetection(const float bVol, const int8_t nowLevel) {
    int8_t searchLv;
    // 現在のレベルからサーチを開始

    if ( (nowLevel < 0) || (BATTERY_THRESHOLDS_TABLES_LEN < nowLevel) ) {
        // 不正なインデックスの場合は0に戻してみる
        return 0;
    }
    else {
        searchLv = nowLevel;

        // 現在のレベルから配列の最後までサーチ
        while (searchLv < BATTERY_THRESHOLDS_TABLES_LEN) {
            // 現在のレベルのしきい値より現在の電圧のレベルが高い場合
            if (BATTERY_THRESHOLDS_TABLES.rise_thresholds[searchLv] < bVol) {
                searchLv++;
            }
            // 現在のレベルと現在の電圧が一致しているか小さい場合
            else {
                break;
            }
        }
        while (0 < searchLv) {
            // 現在のレベルより一つ低いレベルよりさらに低い電圧なら
            if (bVol < BATTERY_THRESHOLDS_TABLES.fall_thresholds[searchLv - 1]) {
                searchLv--;
            }
            // 現在のレベルに該当するレベルなら
            else {
                break;
            }
        }
        return searchLv;
    }
}

batterystatus_t batteryLevel2batteryState(const int8_t level) {
    switch (level) {
    case 0:
        return BATTERY_LO_ERROR;
    case 1:
        return BATTERY_AAA_EMPTY;
    case 2:
        return BATTERY_AAA_WRN;
    case 3:
        return BATTERY_AAA_NORM;
    case 4:
        return BATTERY_AAA_FULL;
    case 5:
        return BATTERY_LIPO_EMPTY;
    case 6:
        return BATTERY_LIPO_WRN;
    case 7:
        return BATTERY_LIPO_NORM;
    case 8:
        return BATTERY_LIPO_FULL;
    case 9:
        return BATTERY_HI_ERROR;
    default :
        return BATTERY_NON_DETECTED;
    }
}

/*========AAAA Private Function Definition END AAAA==========================*/
