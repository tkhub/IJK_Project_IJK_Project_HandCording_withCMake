/**
 * @file linmkrssr.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>
#include <test_and_debug.h>

/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "linmkrssr.h"
#include "linmkrssr_const.h"
#include "linmkrssr_private.h"

#if SAC_DEBUGMODE == DEBUGMODE_LINEMKR_TEST
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hex2bindec.h>
#endif /* SAC_DEBUGMODE == DEBUGMODE_LINEMKR_TEST */

/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/

//! @brief ラインセンサのAD値がDMAで保存されるアドレス
volatile static uint16_t* linesensorDMARaw[4];
//! @brief マーカーセンサのAD値がDMAで保存されるアドレス
volatile static uint16_t* markersensorDMARaw[2];

//! @brief ラインセンサのAD値を計測タイミングで保存する場所
volatile static uint16_t rawLinesensorValues[4];
//! @brief マーカセンサのAD値を計測タイミングで保存する場所
volatile static uint16_t rawMarkersensorValue[2];

//! @brief 各ラインセンサの正規化後の値(0.0～1.0)
volatile static float nrmLinesensors[4];

//! @brief 各ラインセンサの白黒判定値
volatile static uint8_t binLinesensors;
//! @brief 各マーカセンサの白黒判定値
volatile static uint8_t markerDetected;

//! @brief ラインセンサの設定値を保存
volatile static sensor_param_t lineSensorsParam[4];
//! @brief マーカセンサの設定値を保存
volatile static marker_param_t markerSensorsParam[2];

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/**
 * @brief 正規化ゲイン算出関数
 * @param [in] white 白判定時の値
 * @param [in] black 黒判定時の値
 * @return ゲイン
 * @detail センサのAD値を0.0～1.0に変換するためのゲインを求める。オフセットを求めるnrmOffsetとセットで使う
 */
static float nrmGain(uint16_t white, uint16_t black);

/**
 * @brief 正規化オフセット算出関数
 * @param [in] gain 正規化ゲイン
 * @param [in] black 黒判定時の値
 * @return オフセット
 * @detail センサのAD値を0.0～1.0に変換するためのオフセットを求める。ゲインを求めるnrmGainとセットで使う
 */
static float nrmOffset(float gain, uint16_t black);

/**
 * @brief しきい値算出関数
 * @param [in] white 黒判定時の値
 * @param [in] black 黒判定時の値
 * @param [in] nrmth 正規化されたしきい値
 * @return しきい値
 * @detail 正規化(0.0～1.0)されたしきい値から、各センサの白黒検出値のばらつきに合わせたしきい値を算出する
 */
static uint16_t wb2threshold(uint16_t white, uint16_t black, float nrmth);

/**
 * @brief 正規化関数
 * @param [in] ivalue 生値
 * @param [in] gain 正規化ゲイン
 * @param [in] offset 正規化オフセット
 * @param [in] nmax 正規化値の最大値
 * @param [in] nmin 正規化値の最小値
 * @return 正規化された検出値
 * @detail 正規化された値はnmax～nminの範囲内にリミットされる
 */
static float normalize(const uint16_t ivalue, const float gain, const float offset, const float nmax, const float nmin);

/**
 * @brief ローパスフィルタ関数
 * @param [in] measuredvalue 計測値
 * @param [in] lastvalue 前回の計測値
 * @param [in] gain ローパスゲイン
 * @param [in] nmax 正規化値の最大値
 * @param [in] nmin 正規化値の最小値
 * @return 正規化された検出値
 * @detail ゲインは0.0～1.0の範囲内にリミットされる
 */
static float iirlpf(const float measuredvalue, const float lastvalue, const float gain);

/**
 * @brief 2値判定関数(ヒステリシス付き)
 * @param [in] ivalue 生値
 * @param [in] lastbin 前回の判定値
 * @param [in] thH High側のしきい値
 * @param [in] thL Low側のしきい値
 * @return 判定結果
 */
static bool hysteresis(const uint16_t ivalue, const bool lastbin, const uint16_t thH, const uint16_t thL);



/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
void linesensorsInit(volatile uint16_t* plls, volatile uint16_t* plcs, volatile uint16_t* prcs, volatile uint16_t* prrs) {

    linesensorDMARaw[LL_SSR] = plls;
    linesensorDMARaw[LC_SSR] = plcs;
    linesensorDMARaw[RC_SSR] = prcs;
    linesensorDMARaw[RR_SSR] = prrs;

    lineSensorsParam[LL_SSR].offset     = LLSSR_OFFSET;
    lineSensorsParam[LC_SSR].offset     = LCSSR_OFFSET;
    lineSensorsParam[RC_SSR].offset     = RCSSR_OFFSET;
    lineSensorsParam[RR_SSR].offset     = RRSSR_OFFSET;

    linesensorsCalibration( LLSSR_DEFAULT_WHITE, LLSSR_DEFAULT_BLACK,
                            LCSSR_DEFAULT_WHITE, LCSSR_DEFAULT_BLACK,
                            RCSSR_DEFAULT_WHITE, RCSSR_DEFAULT_BLACK,
                            RRSSR_DEFAULT_WHITE, RRSSR_DEFAULT_BLACK);
}

void linesensorsCalibration(const uint16_t llsw, const uint16_t llsb, const uint16_t lcsw, const uint16_t lcsb, const uint16_t rcsw, const uint16_t rcsb, const uint16_t rrsw, const uint16_t rrsb) {
    lineSensorsParam[LL_SSR].white          = llsw;
    lineSensorsParam[LL_SSR].black          = llsb;
    lineSensorsParam[LL_SSR].nrm_gain       = nrmGain(llsw, llsb);
    lineSensorsParam[LL_SSR].nrm_offset     = nrmOffset(lineSensorsParam[LL_SSR].nrm_gain, llsb);
    lineSensorsParam[LL_SSR].thresholdH     = wb2threshold(llsw, llsb, LINESENSOR_THRESHOLD);
    lineSensorsParam[LL_SSR].thresholdL     = wb2threshold(llsw, llsb, LINESENSOR_THRESHOLD - LINESENSOR_HYSTERESIS);

    lineSensorsParam[LC_SSR].white          = lcsw;
    lineSensorsParam[LC_SSR].black          = lcsb;
    lineSensorsParam[LC_SSR].nrm_gain       = nrmGain(lcsw, lcsb);
    lineSensorsParam[LC_SSR].nrm_offset     = nrmOffset(lineSensorsParam[LC_SSR].nrm_gain, lcsb);
    lineSensorsParam[LC_SSR].thresholdH     = wb2threshold(lcsw, lcsb, LINESENSOR_THRESHOLD);
    lineSensorsParam[LC_SSR].thresholdL     = wb2threshold(lcsw, lcsb, LINESENSOR_THRESHOLD - LINESENSOR_HYSTERESIS);

    lineSensorsParam[RC_SSR].white          = rcsw;
    lineSensorsParam[RC_SSR].black          = rcsb;
    lineSensorsParam[RC_SSR].nrm_gain       = nrmGain(rcsw, rcsb);
    lineSensorsParam[RC_SSR].nrm_offset     = nrmOffset(lineSensorsParam[RC_SSR].nrm_gain, rcsb);
    lineSensorsParam[RC_SSR].thresholdH     = wb2threshold(rcsw, rcsb, LINESENSOR_THRESHOLD);
    lineSensorsParam[RC_SSR].thresholdL     = wb2threshold(rcsw, rcsb, LINESENSOR_THRESHOLD - LINESENSOR_HYSTERESIS);

    lineSensorsParam[RR_SSR].white          = rrsw;
    lineSensorsParam[RR_SSR].black          = rrsb;
    lineSensorsParam[RR_SSR].nrm_gain       = nrmGain(rrsw, rrsb);
    lineSensorsParam[RR_SSR].nrm_offset     = nrmOffset(lineSensorsParam[RR_SSR].nrm_gain, rrsb);
    lineSensorsParam[RR_SSR].thresholdH     = wb2threshold(rrsw, rrsb, LINESENSOR_THRESHOLD);
    lineSensorsParam[RR_SSR].thresholdL     = wb2threshold(rrsw, rrsb, LINESENSOR_THRESHOLD - LINESENSOR_HYSTERESIS);
}

void linesensorsMeasure_1ms(void){

    rawLinesensorValues[LL_SSR] = *linesensorDMARaw[LL_SSR];
    rawLinesensorValues[LC_SSR] = *linesensorDMARaw[LC_SSR];
    rawLinesensorValues[RC_SSR] = *linesensorDMARaw[RC_SSR];
    rawLinesensorValues[RR_SSR] = *linesensorDMARaw[RR_SSR];

    nrmLinesensors[LL_SSR] = iirlpf(normalize(rawLinesensorValues[LL_SSR], lineSensorsParam[LL_SSR].nrm_gain, lineSensorsParam[LL_SSR].nrm_offset, 1.0, 0.0), nrmLinesensors[LL_SSR], LINESENSOR_IIRLPG);
    nrmLinesensors[LC_SSR] = iirlpf(normalize(rawLinesensorValues[LC_SSR], lineSensorsParam[LC_SSR].nrm_gain, lineSensorsParam[LC_SSR].nrm_offset, 1.0, 0.0), nrmLinesensors[LC_SSR], LINESENSOR_IIRLPG);
    nrmLinesensors[RC_SSR] = iirlpf(normalize(rawLinesensorValues[RC_SSR], lineSensorsParam[RC_SSR].nrm_gain, lineSensorsParam[RC_SSR].nrm_offset, 1.0, 0.0), nrmLinesensors[RC_SSR], LINESENSOR_IIRLPG);
    nrmLinesensors[RR_SSR] = iirlpf(normalize(rawLinesensorValues[RR_SSR], lineSensorsParam[RR_SSR].nrm_gain, lineSensorsParam[RR_SSR].nrm_offset, 1.0, 0.0), nrmLinesensors[RR_SSR], LINESENSOR_IIRLPG);
    binLinesensors =    ( ( hysteresis(rawLinesensorValues[LL_SSR], ((binLinesensors & 0x08) >> 3), lineSensorsParam[LL_SSR].thresholdH, lineSensorsParam[LL_SSR].thresholdL) << 3 )    & 0x08) |
                        ( ( hysteresis(rawLinesensorValues[LC_SSR], ((binLinesensors & 0x04) >> 2), lineSensorsParam[LC_SSR].thresholdH, lineSensorsParam[LC_SSR].thresholdL) << 2 )    & 0x04) |
                        ( ( hysteresis(rawLinesensorValues[RC_SSR], ((binLinesensors & 0x02) >> 1), lineSensorsParam[RC_SSR].thresholdH, lineSensorsParam[RC_SSR].thresholdL) << 1 )    & 0x02) |
                        ( ( hysteresis(rawLinesensorValues[RR_SSR], ((binLinesensors & 0x01)),      lineSensorsParam[RR_SSR].thresholdH, lineSensorsParam[RR_SSR].thresholdL))          & 0x01);
}

// 左にズレたらマイナス
float linesensorsReadPosition(void) {
    float linesum = 0;
    int cnt;
    for (cnt = 0; cnt < 4; cnt++) {
        linesum += nrmLinesensors[cnt];
    }
    if ( (-0.01 < linesum ) && (linesum < 0.01)) {
        return 0.0;
    }
    else {
        return (nrmLinesensors[LL_SSR] * lineSensorsParam[LL_SSR].offset +
                nrmLinesensors[LC_SSR] * lineSensorsParam[LC_SSR].offset +
                nrmLinesensors[RC_SSR] * lineSensorsParam[RC_SSR].offset +
                nrmLinesensors[RR_SSR] * lineSensorsParam[RR_SSR].offset ) / linesum;
    }
}

uint8_t linesensorsReadBin(void) {
    return binLinesensors;
}

void linesensorsDebug(uint16_t rawVal[4], float nrmVal[4]) {
    rawVal[LL_SSR] =rawLinesensorValues[LL_SSR];
    rawVal[LC_SSR] =rawLinesensorValues[LC_SSR];
    rawVal[RC_SSR] =rawLinesensorValues[RC_SSR];
    rawVal[RR_SSR] =rawLinesensorValues[RR_SSR];

    nrmVal[LL_SSR] = nrmLinesensors[LL_SSR];
    nrmVal[LC_SSR] = nrmLinesensors[LC_SSR];
    nrmVal[RC_SSR] = nrmLinesensors[RC_SSR];
    nrmVal[RR_SSR] = nrmLinesensors[RR_SSR];
}

void markersensorsInit(volatile uint16_t* mkl, volatile uint16_t* mkr) {
    markersensorDMARaw[MKL_SSR] = mkl;
    markersensorDMARaw[MKR_SSR] = mkr;
    markersensorsCalibration(MKLSSR_DEFAULT_WHITE, MKLSSR_DEFAULT_BLACK, MKRSSR_DEFAULT_WHITE, MKRSSR_DEFAULT_BLACK);
}

void markersensorsCalibration(const uint16_t mklw, const uint16_t mklb, const uint16_t mkrw, const uint16_t mkrb) {
    markerSensorsParam[MKL_SSR].white       = mklw;
    markerSensorsParam[MKL_SSR].black       = mklb;
    markerSensorsParam[MKL_SSR].thresholdH  = wb2threshold(mklw, mklb, MARKERSENSOR_THRESHOLD);
    markerSensorsParam[MKL_SSR].thresholdL  = wb2threshold(mklw, mklb, MARKERSENSOR_THRESHOLD - MARKERSENSOR_HYSTERESIS);

    markerSensorsParam[MKR_SSR].white       = mkrw;
    markerSensorsParam[MKR_SSR].black       = mkrb;
    markerSensorsParam[MKR_SSR].thresholdH  = wb2threshold(mkrw, mkrb, MARKERSENSOR_THRESHOLD);
    markerSensorsParam[MKR_SSR].thresholdL  = wb2threshold(mkrw, mkrb, MARKERSENSOR_THRESHOLD - MARKERSENSOR_HYSTERESIS);
}

void markersensorsMeasure_1ms(void) {
    rawMarkersensorValue[MKL_SSR] = *markersensorDMARaw[MKL_SSR];
    rawMarkersensorValue[MKR_SSR] = *markersensorDMARaw[MKR_SSR];

    markerDetected = ( ( hysteresis(rawMarkersensorValue[MKL_SSR], ((markerDetected & 0x02) >> 1),  markerSensorsParam[MKL_SSR].thresholdH, markerSensorsParam[MKL_SSR].thresholdL) << 1 )    & 0x02) |
                     ( ( hysteresis(rawMarkersensorValue[MKR_SSR], ((markerDetected & 0x01)),       markerSensorsParam[MKR_SSR].thresholdH, markerSensorsParam[MKR_SSR].thresholdL))          & 0x01);
}

void markersensorsDebug(uint16_t rawVal[2]) {
    rawVal[MKL_SSR] = rawMarkersensorValue[MKL_SSR];
    rawVal[MKR_SSR] = rawMarkersensorValue[MKR_SSR];
}

uint8_t markersensorsRead(void) {
    return markerDetected;
}

#if SAC_DEBUGMODE == DEBUGMODE_LINEMKR_TEST
uint8_t linmkrssrTest(char* strBuffer, uint8_t maxBufferSize) {
    uint8_t linebin = linesensorsReadBin();
    float linepos = linesensorsReadPosition();
    uint8_t mkr = markersensorsRead();
    return snprintf(strBuffer,maxBufferSize, "L-pos=%-8.4f, L-Bin=0x%02X, mk=0x%01X, raw=%04d(%04d,%04d),%04d(%04d,%04d),%04d(%04d,%04d),%04d(%04d,%04d),%04d(%04d,%04d),%04d(%04d,%04d), nrm = %-6.3f,%-6.3f,%-6.3f,%-6.3f",
                                        //linepos, HEX_TO_BIN_DEC(linebin), HEX_TO_BIN_DEC(mkr),
                                        linepos, linebin, mkr,
                                        rawMarkersensorValue[MKL_SSR], markerSensorsParam[MKL_SSR].thresholdH, markerSensorsParam[MKL_SSR].thresholdL,
                                        rawLinesensorValues[LL_SSR],  lineSensorsParam[LL_SSR].thresholdH, lineSensorsParam[LL_SSR].thresholdL,
                                        rawLinesensorValues[LC_SSR],  lineSensorsParam[LC_SSR].thresholdH, lineSensorsParam[LC_SSR].thresholdL,
                                        rawLinesensorValues[RC_SSR],  lineSensorsParam[RC_SSR].thresholdH, lineSensorsParam[RC_SSR].thresholdL,
                                        rawLinesensorValues[RR_SSR],  lineSensorsParam[RR_SSR].thresholdH, lineSensorsParam[RR_SSR].thresholdL,
                                        rawMarkersensorValue[MKR_SSR], markerSensorsParam[MKR_SSR].thresholdH, markerSensorsParam[MKR_SSR].thresholdL,
                                        nrmLinesensors[LL_SSR], nrmLinesensors[LC_SSR], nrmLinesensors[RC_SSR], nrmLinesensors[RR_SSR]);
}
#endif /* SAC_DEBUGMODE == DEBUGMODE_LINEMKR_TEST */

/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
static float nrmGain(uint16_t white, uint16_t black) {
    return 1.0 / (white - black);
}

static float nrmOffset(float gain, uint16_t black) {
    return -(gain * black);
}

static uint16_t wb2threshold(uint16_t white, uint16_t black, float nrmth) {
    return (uint16_t)( ( (float)(white - black) * nrmth ) + (float)black + 0.5f );
}

static float normalize(const uint16_t ivalue, const float gain, const float offset, const float nmax, const float nmin) {
    float ftmp;
    ftmp = gain * (float)ivalue + offset;
    if (nmax < ftmp) {
        ftmp = nmax;
    }
    if (ftmp < nmin) {
        ftmp = nmin;
    }
    return ftmp;
}

static float iirlpf(const float measuredvalue, const float lastvalue, const float gain) {
    return lastvalue + ( (measuredvalue - lastvalue) * gain);
}

static bool hysteresis(const uint16_t ivalue, const bool lastbin, const uint16_t thH, const uint16_t thL) {
    if (lastbin) {
        // 前回の判定でtrueなら、今回falseの判定は厳しくする
        if (ivalue < thL) {
            return false;
        }
    }
    else {
        // 前回の判定でfalseなら、今回trueの判定は厳しくする
        if (thH < ivalue) {
            return true;
        }
    }
    // 判定に引っかからない場合はそのまま
    return lastbin;
}

/*========AAAA Private Function Definition END AAAA==========================*/
