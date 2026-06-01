/**
 * @file buzzer.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 15, 2025
 * @Version 0.00
 */

#ifndef __SAC_BUZZER_BUZZER_H___
#define __SAC_BUZZER_BUZZER_H___

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
 * @brief buzzerSchedule_t 型の構造体
 * @details スケジュールするブザーの鳴動パターンを設定するための型
 */
typedef struct {
    uint16_t oncount10ms;
    uint16_t offcount10ms;
} buzzerSchedule_t;

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
 * @brief ブザーの初期化関数
 * @param [in] bzbfr バッファ
 * @param [in] size バッファのサイズ
 * @details ブザー鳴動用スケジュール用のバッファは初期化元で確保してもらう
 */
void buzzerInit(buzzerSchedule_t* bzbfr, uint8_t bzbfrsize);

/**
 * @brief ブザー鳴動実行関数
 * @details 10msの割り込み内で鳴動スケジュールバッファから鳴動パターンを取り出す
 */
void buzzer_10ms(void);

/**
 * @brief ブザーのスケジュールをセットする
 */
void buzzerSetSchedule(buzzerSchedule_t bzsch);

/**
 * @brief ブザーのスケジュールをms単位でセットする
 */
void buzzerSetScheduleMs(uint16_t onMs, uint16_t offMs);

#if SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST
/**
 * @brief BUZZERのデバッグ用関数
 * @param [out] strBuffer printfデバッグ用の文字列格納先
 * @param [in] maxBufferSize 文字列の最大格納数
 * @return 生成した文字列の長さ
 */
uint8_t buzzerTest(char* strBuffer, uint8_t maxBufferSize);
#endif /* SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST */

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __SAC_BUZZER_BUZZER_H___ */
