/**
 * @file battery.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

#ifndef __SAC_BATTERY_BATTERY_H___
#define __SAC_BATTERY_BATTERY_H___

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
/**
 * @brief バッテリー状態を表現するbatterystatus_t 型の列挙体
 *
 */
typedef enum {
    BATTERY_NON_DETECTED,   //! 未判定
    BATTERY_LO_ERROR,       //! エラー状態(Lo)
    BATTERY_AAA_EMPTY,      //! 単3電池過放電警告
    BATTERY_AAA_WRN,        //! 単3電池消耗注意
    BATTERY_AAA_NORM,       //! 単3電池通常状態
    BATTERY_AAA_FULL,       //! 単3電池新品
    BATTERY_LIPO_EMPTY,     //! リポバッテリー過放電警告
    BATTERY_LIPO_WRN,       //! リポバッテリー消耗注意
    BATTERY_LIPO_NORM,      //! リポバッテリー通常状態
    BATTERY_LIPO_FULL,      //! リポバッテリー満充電
    BATTERY_HI_ERROR,       //! エラー状態(Hi)
} batterystatus_t;


/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/** 
 * @brief マクロ MAX_NANK の説明
 */
/* # define MAX_NANKA 256 */

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
 * @brief バッテリー状態検出初期化関数
 * @param [in] batteryVolRAM バッテリ電圧の格納先
 */
void batteryInit(volatile uint16_t* batteryVolRAM);


/**
 * @brief バッテリー状態計測関数(10ms)
 */
void battery_10ms(void);

/**
 * @brief バッテリ電圧の読み出し関数
 * @return バッテリ電圧
 */
float batteryRead(void);

/**
 * @brief バッテリーステータス読み出し関数
 * @return バッテリーステータス
 */
batterystatus_t batteryReadStateatus(void);

/**
 * @brief バッテリーステータス文字列変換関数
 * @detail
 */
uint8_t batteryState2String(batterystatus_t btst, char * str);

#if SAC_DEBUGMODE == DEBUGMODE_BATTERY_TEST
/**
 * @brief バッテリーテスト・デバッグ用関数
 * @param [in/out] 文字列格納先のアドレス
 * @param [in] 文字列格納先に確保済みのサイズ
 * @return 生成した文字列のサイズ
 */
uint8_t batteryTest(char* strBuffer, uint8_t maxBufferSize);
#endif /* SAC_DEBUGMODE == DEBUGMODE_BATTERY_TEST */
/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
uint8_t batteryTest(char* strBuffer, uint8_t maxBufferSize);
#endif

#endif /* __SAC_BATTERY_BATTERY_H___ */
