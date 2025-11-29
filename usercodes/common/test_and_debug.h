/**
 * @file test_and_debug.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 13, 2025
 * @Version 0.00
 */

#ifndef __COMMON_TEST_AND_DEBUG_H___
#define __COMMON_TEST_AND_DEBUG_H___

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
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
#define _ENABLE_SAC_TEST_   //! SACレイヤーのテスト・デバッグ用のスイッチ

#ifdef _ENABLE_SAC_TEST_
// #define _ENABLE_UISW_TEST_  //! UISWのテスト・デバッグ用のスイッチ
// #define _ENABLE_MOTOR_TEST_   //! MOTORのテスト・デバッグ用のスイッチ
// #define _ENABLE_BUZZER_TEST_    //! BUZZERのテスト・デバッグ用のスイッチ
// #define _ENABLE_LINEMKR_TEST_   //! ラインセンサ・マーカセンサのテストデバッグ用のスイッチ
// #define _ENABLE_BATTERY_TEST_   //! バッテリーのテスト・デバッグ用のスイッチ
#endif /* _ENABLE_SAC_TEST_ */

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
 * @detail 詳細な説明
 */
/**
 * int testFunc(int* buffer, int size)
*/

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_TEST_AND_DEBUG_H___ */
