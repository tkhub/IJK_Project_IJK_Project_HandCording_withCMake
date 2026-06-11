/**
 * @file uisw.h
 * @brief UISWのAPIを提供する
 * @author takap
 * @date Nov 13, 2025
 * @Version 0.00
 * @details UISWのAPIを提供する。SW入力をバックグラウンドでキャプチャし、イベントをキューイングする。
 */

#ifndef __SAC_UISW_UISW_H___
#define __SAC_UISW_UISW_H___

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
 * @brief スイッチ状態のイベント
 */
typedef enum {
    NON_UISW_EVENT,             //! スイッチイベントなし
    UISW1_PUSH_EVENT,           //! SW1が押されて離されるイベント
    UISW2_PUSH_EVENT,           //! SW2が押されて離されるイベント
    UISW1_LONG_EVENT,           //! SW1が長押し押されてるイベント
    UISW2_LONG_EVENT,           //! SW2が長押し押されてるイベント
    UISW1_LONG_RELEASE_EVENT,   //! SW1が長押し押され離されたイベント
    UISW2_LONG_RELEASE_EVENT,   //! SW2が長押し押され離されたイベント
    UISW_BOTH_EVENT,            //! SWが両押しされたイベント
    UISW_ERROR                  //! 何かしら想定外のケースに陥ったイベント
} uiswevent_t;


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
 * @brief UISW初期化関数
 * @details UISWキューの初期化
 */
void uiswInit(void);

/**
 * @brief UISWポーリング関数
 * @details UISWキューの操作。スイッチ状態は新しい順にキューに格納され、古いものは削除される。
 */
void uisw_10ms(void);

/**
 * @brief UISWキューの読み出し
 * @return UISWのイベント
 * @details NON_UISW_EVENT以外が発生するとキューに保持される。キューから古い順にイベントが読み出され、削除される。
 */
uiswevent_t uiswRead(void);

#if SAC_DEBUGMODE == DEBUGMODE_UISW_TEST
/**
 * @brief UISWのデバッグ用関数
 * @param [out] strBuffer printfデバッグ用の文字列格納先
 * @param [in] maxBufferSize 文字列の最大格納数
 * @return 生成した文字列の長さ
 */
uint8_t uiswTest(char* strBuffer, uint8_t maxBufferSize);
#endif /* DEBUGMODE_UISW_TEST_ */

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __SAC_UISW_UISW_H___ */
