/**
 * @file linmkrssr_private.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

#ifndef __SAC_LINMKRSSR_LINMKRSSR_PRIVATE_H___
#define __SAC_LINMKRSSR_LINMKRSSR_PRIVATE_H___

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/**
 * @brief ABC_t 型の列挙体
 */
/**
 * typedef enum {
 *  A,  //! Aの説明
 *  B,  //! Bの説明
 *  C   //! Cの説明
 * } ABC_t;
*/

/**
 * @brief DEF_t 型の構造体
 */
/**
 * typedef struct {
 *  int D,  //!  Dの説明
 *  int E,  //!  Eの説明
 *  int F   //!  Fの説明
 * } DEF_t;
*/



struct sensor_param_st {
    float       offset;     // センサの中央からの位置(左ズレを負、右ズレを正→左センサが＋位置、右センサが-位置）
    float       nrm_gain;   // 正規化のための傾き
    float       nrm_offset; // 正規化のためのオフセット
    uint16_t    white;      // 白判定時の値
    uint16_t    black;      // 黒判定時の値
    uint16_t    thresholdH;  // 白黒判別のしきい値（生値ベース）
    uint16_t    thresholdL; //
};
typedef struct sensor_param_st sensor_param_t;

struct marker_param_st {
    uint16_t    white;
    uint16_t    black;
    uint16_t    thresholdH;
    uint16_t    thresholdL;
};
typedef struct marker_param_st marker_param_t;


/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
#define LL_SSR 3
#define LC_SSR 2
#define RC_SSR 1
#define RR_SSR 0

#define MKL_SSR 1
#define MKR_SSR 0

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
/**
 * @brief グローバル変数 lobal_var の説明  
*/
/**
 * extern int global_var;
*/

/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/
/**
 * @brief テスト関数
 * @param [in/out] buffer バッファ
 * @param [in] size バッファのサイズ
 * @return 戻り値の説明
 * @details 詳細な説明
 */
/**
 * int testFunc(int* buffer, int size)
*/

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __SAC_LINMKRSSR_LINMKRSSR_PRIVATE_H___ */
