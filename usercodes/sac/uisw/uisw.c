/**
 * @file uisw.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 13, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "uisw.h"
#include "uisw_const.h"
#include "gpio.h"

#if SAC_DEBUGMODE == DEBUGMODE_UISW_TEST
// デバッグではターミナルへの文字表示を行う
#include <stdio.h>
#include <string.h>
#endif /* DEBUGMODE_UISW_TEST */

/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/**
 * @brief UISWシーケンスの列挙型
 */
typedef enum {
    UISWSQ_INIT,                //! 初期状態
    UISWSQ_WAIT_PUSH,           //! どちらのスイッチも押されていない状態
    UISW1SQ_WAIT_RELEASE,       //! SW1が押され、離されるのを待っている状態
    UISW2SQ_WAIT_RELEASE,       //! SW2が押され、離されるのを待っている状態
    UISW1SQ_LONG_WAIT_RELEASE,  //! SW1が長し押され、離されるのを待っている状態
    UISW2SQ_LONG_WAIT_RELEASE,  //! SW2が長し押され、離されるのを待っている状態
    UISWSQ_BOTH_WAIT_RELEASE    //! 両方のSWが押され、離されるのを待っている状態
} uiswsq_t;


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

/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/** 
 * @brief マクロ MAX_NANK の説明
 */
/* # define MAX_NANKA 256 */

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/* int global_var; */ /* ヘッダファイルで説明済みのためDoxygenのコメントは不要 */

/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/
/**
 * @brief 内部グローバル変数 internal_var の説明  
*/
/**
 * static int internal_var;
*/
volatile static uiswsq_t uiswsq;
volatile static uiswevent_t uiswevent;
volatile static bool sw1_tmp, sw2_tmp;
volatile static bool sw1_tmp2, sw2_tmp2;
volatile static bool sw1_tmp3, sw2_tmp3;
volatile static uint16_t  sw1_OnCount;
volatile static uint16_t  sw2_OnCount;
volatile static uint16_t  sw1_OffCount;
volatile static uint16_t  sw2_OffCount;
volatile static uint8_t uisw_intvlcnt;

volatile static uiswevent_t* p_uiswBuffer;
static uint8_t      uiswBffSize;
volatile uint8_t    uiswBffHead;
volatile uint8_t    uiswBffTail;

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/

#ifdef _ENABLE_UISW_TEST_

#endif /* _ENABLE_UISW_TEST_ */

/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
/* ヘッダファイルで説明済みのためDoxygenのコメントは不要 */

void uiswInit(volatile uiswevent_t* buffer, uint8_t buffersize) {
    uiswsq = UISWSQ_INIT;
    uiswevent = NON_UISW_EVENT;
    p_uiswBuffer = buffer;
    uiswBffSize = buffersize;
    sw1_tmp3 = false;
    sw2_tmp3 = false;
    sw1_tmp2 = false;
    sw2_tmp2 = false;
    sw1_OnCount = 0;
    sw2_OnCount = 0;

}

void uisw_10ms(void) {
    uiswevent_t swstate = NON_UISW_EVENT;

    /// SW1履歴を3個分保持する
    sw1_tmp3 = sw1_tmp2;
    sw1_tmp2 = sw1_tmp;
    sw1_tmp = !HAL_GPIO_ReadPin(SW1_GPIO_Port,SW1_Pin);

    /// SW2履歴を3個分保持する
    sw2_tmp3 = sw2_tmp2;
    sw2_tmp2 = sw2_tmp;
    sw2_tmp = !HAL_GPIO_ReadPin(SW2_GPIO_Port,SW2_Pin);

    switch (uiswsq) {
    default: /// UISWSQ_INIT
        uiswsq = UISWSQ_WAIT_PUSH;
        break;

    case UISWSQ_WAIT_PUSH:
        /// スイッチ押下待ち
        if (sw1_tmp && sw1_tmp2 && !sw1_tmp3) {
            uiswsq = UISW1SQ_WAIT_RELEASE;
            // 長押し判定用カウンタに判定値をセット
            sw1_OnCount = UISW_HOLD_10MS;
        }

        else if (sw2_tmp && sw2_tmp2 && !sw2_tmp3) {
            uiswsq = UISW2SQ_WAIT_RELEASE;
            // 長押し判定用カウンタに判定値をセット
            sw2_OnCount = UISW_HOLD_10MS;
        }
        break;

    case UISW1SQ_WAIT_RELEASE:
        if (sw1_OnCount == 0){
            // 長押し判定。長押し中のイベントも発行
            uiswsq = UISW1SQ_LONG_WAIT_RELEASE;
            swstate = UISW1_LONG_EVENT;
        }
        else {
            sw1_OnCount--;
            if (!sw1_tmp && !sw1_tmp2 && sw1_tmp3) {
                // 直近2回はSW1が押されていない場合、スイッチ押下待ちに遷移するとともSW1押下イベントを発行
                uiswsq = UISWSQ_WAIT_PUSH;
                swstate = UISW1_PUSH_EVENT;
            }
            else if (   (sw1_tmp && sw1_tmp2) &&
                        (sw2_tmp && sw2_tmp2 && sw2_tmp3)) {
                // SW2も同時に押される場合、両押しイベントを発行
                uiswsq = UISWSQ_BOTH_WAIT_RELEASE;
                swstate = UISW_BOTH_EVENT;
            }
            else {
                /* NOP */
            }
        }

        break;

    case UISW2SQ_WAIT_RELEASE:
        if (sw2_OnCount == 0){
            uiswsq = UISW2SQ_LONG_WAIT_RELEASE;
            swstate = UISW2_LONG_EVENT;
        }
        else {
            sw2_OnCount--;
            if (!sw2_tmp && !sw2_tmp2 && sw2_tmp3) {
                // 直近2回はSW2が押されていない場合、スイッチ押下待ちに遷移するとともSW2押下イベントを発行
                uiswsq = UISWSQ_WAIT_PUSH;
                swstate = UISW2_PUSH_EVENT;
            }
            else if (   (sw2_tmp && sw2_tmp2) &&
                        (sw1_tmp && sw1_tmp2 && sw1_tmp3)) {
                // SW1も同時に押される場合、両押しイベントを発行
                uiswsq = UISWSQ_BOTH_WAIT_RELEASE;
                swstate = UISW_BOTH_EVENT;
            }
            else {
                /* NOP */
            }
        }

        break;

    case UISW1SQ_LONG_WAIT_RELEASE:
        if (!sw1_tmp && !sw1_tmp2 && !sw1_tmp3) {
            // SW1長押しが終わり、SW1が３周期分OFFの場合は長押し終わりのイベントを発行
            uiswsq = UISWSQ_WAIT_PUSH;
            swstate = UISW1_LONG_RELEASE_EVENT;
        }
        break;
    case UISW2SQ_LONG_WAIT_RELEASE:
        if (!sw2_tmp && !sw2_tmp2 && !sw2_tmp3) {
            // SW2長押しが終わり、SW2が３周期分OFFの場合は長押し終わりのイベントを発行
            uiswsq = UISWSQ_WAIT_PUSH;
            swstate = UISW2_LONG_RELEASE_EVENT;
        }
        break;
    case UISWSQ_BOTH_WAIT_RELEASE:
        if (    !sw1_tmp && !sw1_tmp2 && !sw1_tmp3 &&
                !sw2_tmp && !sw2_tmp2 && !sw2_tmp3 ) {
            // どのスイッチも押されていない状態が３周期分継続しているならば
            uiswsq = UISWSQ_WAIT_PUSH;
        }
        break;
    }

    // イベントをストアする
    uiswevent = swstate;
    if (swstate != NON_UISW_EVENT) {
        // キューに入れるのはイベントがある場合のみ
        *(p_uiswBuffer + uiswBffTail) = swstate;
        // 格納側はオーバー関係なくガンガン書き込む
        uiswBffTail = (uiswBffTail + 1) % uiswBffSize;
    }
}

uiswevent_t uiswRead(void) {
    uiswevent_t event;
    // バッファを頭から読み出す
    __disable_irq();
    event = *(p_uiswBuffer + uiswBffHead);
    // 読み出し終わったバッファは消す
    *(p_uiswBuffer + uiswBffHead) = NON_UISW_EVENT;
    if (uiswBffHead != uiswBffTail) {
        // バッファ読み出しは過剰に行わないようにする（バッファが無駄に進まないように）
        uiswBffHead = (uiswBffHead + 1) % uiswBffSize;
    }
    __enable_irq();
    return event;
}

#if SAC_DEBUGMODE == DEBUGMODE_UISW_TEST
/**
 * @brief UISWのデバッグ用関数
 * @param [out] strBuffer printfデバッグ用の文字列格納先
 * @param [in] maxBufferSize 文字列の最大格納数
 * @return 生成した文字列の長さ
 */
uint8_t uiswTest(char* strBuffer, uint8_t maxBufferSize) {
    uiswevent_t eventTmp = uiswRead();
    uiswsq_t sqTmp = uiswsq;
    uint16_t sw1cnt = sw1_OnCount;
    uint16_t sw2cnt = sw2_OnCount;
    char evstr[12];
    char sqstr[12];

    __disable_irq();
    sw1cnt = sw1_OnCount;
    sw2cnt = sw2_OnCount;
    __enable_irq();

    /// 判定したイベントを文字列に変換する
    switch(eventTmp) {
    case NON_UISW_EVENT:
        strncpy(evstr, "___nonev___", 12);
        break;
    case UISW1_PUSH_EVENT:
        strncpy(evstr, "PUSHSW1", 12);
        break;
    case UISW2_PUSH_EVENT:
        strncpy(evstr, "PUSHSW2", 12);
        break;
    case UISW1_LONG_EVENT:
        strncpy(evstr, "LONGSW1", 12);
        break;
    case UISW2_LONG_EVENT:
        strncpy(evstr, "LONGSW2", 12);
        break;
    case UISW1_LONG_RELEASE_EVENT:
        strncpy(evstr, "LONGSW1RLS", 12);
        break;
    case UISW2_LONG_RELEASE_EVENT:
        strncpy(evstr, "LONGSW2RLS", 12);
        break;
    case UISW_BOTH_EVENT:
        strncpy(evstr, "SW1&SW2", 12);
        break;
    default :
        strncpy(evstr, "!EVENTERR!", 12);
        break;
    }

    switch (sqTmp) {
    case UISWSQ_INIT:
        strncpy(sqstr, "INIT", 12);
        break;
    case UISWSQ_WAIT_PUSH:
        strncpy(sqstr, "WTPUSH", 12);
        break;
    case UISW1SQ_WAIT_RELEASE:
        strncpy(sqstr, "S1WTRLS", 12);
        break;
    case UISW2SQ_WAIT_RELEASE:
        strncpy(sqstr, "S2WTRLS", 12);
        break;
    case UISW1SQ_LONG_WAIT_RELEASE:
        strncpy(sqstr, "S1LNGWTRLS", 12);
        break;
    case UISW2SQ_LONG_WAIT_RELEASE:
        strncpy(sqstr, "S2LNGWTRLS", 12);
        break;
    case UISWSQ_BOTH_WAIT_RELEASE:
        strncpy(sqstr, "SBTHWTRLS", 12);
        break;
    default:
        strncpy(sqstr, "!SQERR!", 12);
        break;
    }
    return snprintf(strBuffer, maxBufferSize,"SQ:%s, EV=%s, SW1CNT=%d, SW2CNT=%d", sqstr, evstr, sw1cnt, sw2cnt);
}

#endif /* SAC_DEBUGMODE == DEBUGMODE_UISW_TEST */

/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
/* プロトタイプ宣言で説明済みのためDoxygenのコメントは不要 */
/*
static int internalTestFunc(int* buffer, int size) {

}
*/

/*========AAAA Private Function Definition END AAAA==========================*/



