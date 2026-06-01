/**
 * @file linmkrssr_const.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

#ifndef __SAC_LINMKRSSR_LINMKRSSR_CONST_H___
#define __SAC_LINMKRSSR_LINMKRSSR_CONST_H___

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>

/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/** 
 * @brief マクロ MAX_NANK の説明
 */
/* # define MAX_NANKA 256 */

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/

//! @brief 一番左のセンサのオフセット量
extern const float      LLSSR_OFFSET;
//! @brief 一番左のセンサの白検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   LLSSR_DEFAULT_WHITE;
//! @brief 一番左のセンサの黒検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   LLSSR_DEFAULT_BLACK;

//! @brief 左から２番目のセンサのオフセット量
extern const float      LCSSR_OFFSET;
//! @brief 左から２番目のセンサの白検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   LCSSR_DEFAULT_WHITE;
//! @brief 左から２番目のセンサの黒検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   LCSSR_DEFAULT_BLACK;

//! @brief 右から２番目のセンサのオフセット量
extern const float      RCSSR_OFFSET;
//! @brief 右から２番目のセンサの白検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   RCSSR_DEFAULT_WHITE;
//! @brief 右から２番目のセンサの黒検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   RCSSR_DEFAULT_BLACK;

//! @brief 一番右のセンサのオフセット量
extern const float      RRSSR_OFFSET;
//! @brief 一番右のセンサの白検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   RRSSR_DEFAULT_WHITE;
//! @brief 一番右のセンサの黒検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   RRSSR_DEFAULT_BLACK;

/**
 * @brief 正規化されたラインセンサの白判定しきい値。WHITEとBLACKとの差に対する割合で規定
 * @details 生値はセンサごとにばらつくため、正規化した値(黒を0.0、白を1.0とした場合に対する割合）で規定する
 */
extern const float      LINESENSOR_THRESHOLD;

/**
 * @brief 正規化されたラインセンサのヒステリシス。黒判定しきい値は白判定しきい値からこの値を差し引いた値
 * @details 生値はセンサごとにばらつくため、正規化した値(黒を0.0、白を1.0とした場合に対する割合）で規定する
 */
extern const float      LINESENSOR_HYSTERESIS;

//! @brief ラインセンサのIIRローパスフィルタ定数(0より大きく1.0以下)
extern const float      LINESENSOR_IIRLPG;

//! @brief 右マーカーセンサのセンサの白検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   MKLSSR_DEFAULT_WHITE;
//! @brief 右マーカーセンサのセンサの黒検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   MKLSSR_DEFAULT_BLACK;

//! @brief 左マーカーセンサのセンサの白検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   MKRSSR_DEFAULT_WHITE;
//! @brief 左マーカーセンサのセンサの黒検出値。センサのばらつき等を含むので実験で求める。
extern const uint16_t   MKRSSR_DEFAULT_BLACK;

//! @brief マーカセンサの白判定しきい値。WHITEとBLACKとの差に対する割合で規定
extern const float      MARKERSENSOR_THRESHOLD;
//! @brief マーカセンサのヒステリシス。黒判定しきい値は白判定しきい値からこの値を差し引いた値
extern const float      MARKERSENSOR_HYSTERESIS;

/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/


#ifdef __cplusplus
}
#endif

#endif /* __SAC_LINMKRSSR_LINMKRSSR_CONST_H___ */
