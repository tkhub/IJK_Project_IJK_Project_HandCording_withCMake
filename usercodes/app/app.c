/**
 * @file app.c
 * @brief 簡単な説明
 * @author tkp
 * @date 30
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "app.h"
#include <sac.h>
#include <math.h>
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/**
 * @brief ABC_t 型の列挙体
 */
/* typedef enum {
 *  A,  //! Aの説明
 *  B,  //! Bの説明
 *  C   //! Cの説明
 * } ABC_t;
*/

/**
 * @brief DEF_t 型の構造体
 */
/* typedef struct {
 *  int D,  //! Dの説明
 *  int E,  //! Eの説明
 *  int F   //! Fの説明
 * } DEF_t;
*/
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/**
 * @brief マクロ MAX_NANK の説明
 */
 /*#define MAX_NANKA 256 */
 #define MOTOR_BASE_SPEED  (0.25f)  /* モータの基本速度 */
 #define MOTOR_KP          (0.01f) /* 比例制御のゲイン */
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
int global_var; */ /* ヘッダファイルで説明済み */
/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/
/**
 * @brief 内部グローバル変数 internal_var の説明
 */
/*static int internal_var; */
/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
/**
 * @brief 内部テスト関数
 * @param [in/out] buffer バッファ
 * @param [in] size バッファのサイズ
 * @return 戻り値の説明
 * @details 詳細な説明
 */
static int internalTestFunc(int* buffer, int size); 

/*========AAAA Private Function Prototype Declaration END AAAA===============*/

static int run_flag; /* ライントレーサーの動作フラグ */

/*========VVVV GLOBAL Function Definition START VVVV=========================*/

int testFunc(int* buffer, int size) {
 *
 * }

/** APPの初期化関数 */
void appInit(void) {
    run_flag = 0;

    /* モーター停止*/
    motorsDrive(0.0f, 0.0f);

}

/** APPの1msインターバルハンドラ */
void appIntervalHandler_1ms(void) {
    float lineread;
    float motor_speed;
    float motorL;
    float motorR;

    /*停止中*/
    if(run_flag == 0){
        motorsDrive(0.0f, 0.0f);
        return;
    }

    /*ラインからのズレをmm単位で計測する関数*/
    lineread = linesensorReadPosition();

    /*API仕様*/
    /*(左にズレたら- 右に+　中央0)*/
    motor_speed = lineread * MOTOR_KP;

    /*左旋回*/
    motorL = MOTOR_BASE_SPEED - motor_speed;
    /*右旋回*/
    motorR = MOTOR_BASE_SPEED + motor_speed;
    motorsDrive(motorL, motorR);

}

/** APPの10msインターバルハンドラ */
void appIntervalHandler_10ms(void) {
    /*特に使用しない*/

}

/** APPのメインループ */
void appMainLoop(void){
    uiswevent_t sw_status;
    /*スイッチの状態を取得する関数*/
    sw_status = uiswRead();

    /*スイッチが押されたら動作開始*/
    if(sw_status == UISW1_PUSH_EVENT){
        run_flag = !run_flag;

        if(run_flag){
            buzzerSetScheduleMs(100,0);
        }
        else{
            motorsDrive(0.0f, 0.0f);
            buzzerSetScheduleMs(50,0);
        }
    }
}

/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
/* static int internalTestFunc(int* buffer, int size) {
 *
 * }
*/
/*========AAAA Private Function Definition END AAAA==========================*/