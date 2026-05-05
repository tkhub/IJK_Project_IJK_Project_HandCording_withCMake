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
 * @brief テスト関数
 * @param [in/out] buffer バッファ
 * @param [in] size バッファのサイズ
 * @return 戻り値の説明
 * @detail 詳細な説明
 */
void linesensorsInit(volatile uint16_t* plls, volatile uint16_t* plcs, volatile uint16_t* prcs, volatile uint16_t* prrs);
void linesensorsCalibration(const uint16_t llsw, const uint16_t llsb, const uint16_t lcsw, const uint16_t lcsb, const uint16_t rcsw, const uint16_t rcsb, const uint16_t rrsw, const uint16_t rrsb);
void linesensorsMeasure_1ms(void);
void linesensorsDebug(uint16_t rawVal[4], float nrmVal[4]);
float linesensorsReadPosition(void);
uint8_t linesensorsReadBin(void);

void markersensorsInit(volatile uint16_t* mkl, volatile uint16_t* mkr);
void markersensorsCalibration(const uint16_t mklw, const uint16_t mklb, const uint16_t mkrw, const uint16_t mkrb);
void markersensorsMeasure_1ms(void);
void markersensorsDebug(uint16_t rawVal[2]);
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
