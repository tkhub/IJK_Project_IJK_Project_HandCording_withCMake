/**
 * @file linmkrssr.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

#ifndef __SAC_LINMKRSSR_LINMKRSSR_H___
#define __SAC_LINMKRSSR_LINMKRSSR_H___

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>
#include <test_and_debug.h>

/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/
/**
 * @brief 初期化関数
 * @param [in] plls 左端ラインセンサのDMAデータ格納先バッファ
 * @param [in] plcs 左中央ラインセンサのDMAデータ格納先バッファ
 * @param [in] prcs 右中央ラインセンサのDMAデータ格納先バッファ
 * @param [in] prrs 右端ラインセンサのDMAデータ格納先バッファ
 * @details 各センサのポインタの登録と、内部のセンサキャリブレーションを行う。
 */
void linesensorsInit(volatile uint16_t* plls, volatile uint16_t* plcs, volatile uint16_t* prcs, volatile uint16_t* prrs);

/**
 * @brief ラインセンサキャリブレーション関数
 * 
 * @param llsw :左端ラインセンサの白値
 * @param llsb :左端ラインセンサの黒値
 * @param lcsw :左中央ラインセンサの白値
 * @param lcsb :左中央ラインセンサの黒値
 * @param rcsw :右中央ラインセンサの白値
 * @param rcsb :右中央ラインセンサの黒値
 * @param rrsw :右端ラインセンサの白値
 * @param rrsb :右端ラインセンサの黒値
 */
void linesensorsCalibration(const uint16_t llsw, const uint16_t llsb, const uint16_t lcsw, const uint16_t lcsb, const uint16_t rcsw, const uint16_t rcsb, const uint16_t rrsw, const uint16_t rrsb);

/**
 * @brief ラインセンサの1ms周期関数
 * @brief ラインセンサのAD値をフィルタしたり、白黒判定したりする。
 */
void linesensorsMeasure_1ms(void);

/**
 * @brief ラインセンサの中央からの偏差計測関数
 * 
 * @return float ラインセンサのズレ(左にズレたらマイナス)[mm]
 * @details ラインセンサの正規化した値を元に、重心計算を行いラインセンサのズレ量を算出する。色ムラ等の外乱の影響は受ける。
 */
float linesensorsReadPosition(void);

/**
 * @brief ラインセンサの白黒判定結果読み取り関数
 * 
 * @return uint8_t 4bit目から順に、左端のセンサから右端のセンサの白黒判定値を格納。1なら白。0なら黒。
 */
uint8_t linesensorsReadBin(void);

/**
 * @brief マーカーセンサの初期化関数
 * 
 * @param [in] mkl 左マーカーセンサのDMAデータ格納先のポインタ
 * @param [in] mkr 右マーカーセンサのDMAデータ格納先のポインタ
 */
void markersensorsInit(volatile uint16_t* mkl, volatile uint16_t* mkr);

/**
 * @brief マーカーセンサのキャリブレーション関数
 * 
 * @param mklw 左マーカーセンサの白値
 * @param mklb 左マーカーセンサの黒値
 * @param mkrw 右マーカーセンサの白値
 * @param mkrb 右マーカーセンサの黒値
 */
void markersensorsCalibration(const uint16_t mklw, const uint16_t mklb, const uint16_t mkrw, const uint16_t mkrb);

/**
 * @brief マーカーセンサの1ms周期関数
 * @brief 各マーカーセンサのAD値をフィルタしたり、白黒判定する。
 */
void markersensorsMeasure_1ms(void);

/**
 * @brief マーカーセンサの白黒判定値を読み取る関数
 * 
 * @return uint8_t 左マーカーセンサの判定結果が2bit目。右マーカセンサが1bit目。1なら白。0なら黒。
 */
uint8_t markersensorsRead(void);

#if SAC_DEBUGMODE == DEBUGMODE_LINEMKR_TEST
/**
 * @brief ラインセンサ・マーカセンサのデバッグ用関数
 * @param [out] strBuffer printfデバッグ用の文字列格納先
 * @param [in] maxBufferSize 文字列の最大格納数
 * @return 生成した文字列の長さ
 */
uint8_t linmkrssrTest(char* strBuffer, uint8_t maxBufferSize);
#endif /* SAC_DEBUGMODE == DEBUGMODE_LINEMKR_TEST */

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __SAC_LINMKRSSR_LINMKRSSR_H___ */
