/**
 * @file sac.c
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 14, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/

/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include <test_and_debug.h>

#ifdef  DEBUGMODE_NONE_TEST
    #include <xprintf.h>
    #include <string.h>
    #include <stdlib.h>
#endif /* DEBUGMODE_NONE_TEST */

#include "sac.h"
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

#ifdef _ENABLE_SAC_TEST_
static int testcnt;
#endif /* _ENABLE_SAC_TEST_ */
/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/**
 * @fn int testFunc(int* buffer, int size)
 * @brief 内部テスト関数
 * @param [in/out] buffer バッファ
 * @param [in] size バッファのサイズ
 * @return 戻り値の説明
 * @details 詳細な説明
 */
/**
 * static int internalTestFunc(int* buffer, int size)
*/


/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
/* ヘッダファイルで説明済みのためDoxygenのコメントは不要 */

/*
int testFunc(int* buffer, int size) {

}
*/
void sacLoop(void) {

    #ifdef _ENABLE_SAC_TEST_
        static char teststr[200];
        #if SAC_DEBUGMODE == DEBUGMODE_UISW_TEST
            uiswTest(teststr, 160);
        #elif SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
            motorTest(teststr, 160);
        #elif SAC_DEBUGMODE == DEBUGMODE_BUZZER_TEST
            buzzerTest(teststr, 160);
        #elif SAC_DEBUGMODE == DEBUGMODE_LINEMKR_TEST
            linmkrssrTest(teststr, 200);
        #elif SAC_DEBUGMODE == DEBUGMODE_BATTERY_TEST
            batteryTest(teststr, 200);
        #elif SAC_DEBUGMODE == DEBUGMODE_IMU_TEST
            imuTest(teststr, 180);
        #endif /* DEBUGMODE_UISW_TEST */
        printf("%d\t%s\r\n", testcnt, teststr);
        testcnt++;
    #endif /* _ENABLE_SAC_TEST_ */

}

/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
/* プロトタイプ宣言で説明済みのためDoxygenのコメントは不要 */
/*
static int internalTestFunc(int* buffer, int size) {

}
*/

/*========AAAA Private Function Definition END AAAA==========================*/
