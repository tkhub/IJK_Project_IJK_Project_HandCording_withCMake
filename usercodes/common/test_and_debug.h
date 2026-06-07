/**
 * @file test_and_debug.h
 * @brief  デバッグ制御用のヘッダファイル 
 * @author takap
 * @date Nov 13, 2025
 * @Version 0.00
 */

#ifndef __COMMON_TEST_AND_DEBUG_H___
#define __COMMON_TEST_AND_DEBUG_H___

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV MACRO Definition START VVVV===================================*/

//! SACデバッグ時はコメントアウトを解除。同時に複数は使わないこと
// #define _ENABLE_SAC_TEST_   //! SACレイヤーのテスト・デバッグ用のスイッチ

#ifdef _ENABLE_SAC_TEST_
    #define DEBUGMODE_UISW_TEST         1                       //! UISWのテスト・デバッグ用のスイッチ
    #define DEBUGMODE_MOTOR_TEST        2                       //! MOTORのテスト・デバッグ用のスイッチ
    #define DEBUGMODE_BUZZER_TEST       3                       //! BUZZERのテスト・デバッグ用のスイッチ
    #define DEBUGMODE_LINEMKR_TEST      4                       //! ラインセンサ・マーカセンサのテストデバッグ用のスイッチ
    #define DEBUGMODE_BATTERY_TEST      5                       //! バッテリーのテスト・デバッグ用のスイッチ
    #define DEBUGMODE_IMU_TEST          6                       //! IMUセンサのテスト・デバッグ用のスイッチ
    #define SAC_DEBUGMODE               DEBUGMODE_LINEMKR_TEST  //! 現在のSACレイヤーのテスト・デバッグモード
#else
    #define DEBUGMODE_NONE_TEST         0                       //! テスト・デバッグ用のスイッチ
    #define SAC_DEBUGMODE               DEBUGMODE_NONE_TEST     //! 現在のSACレイヤーのテスト・デバッグモード
#endif /* _ENABLE_SAC_TEST_ */

/*========AAAA MACRO Definition END AAAA=====================================*/

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_TEST_AND_DEBUG_H___ */
