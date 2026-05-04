/**
 * @file motor.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 14, 2025
 * @Version 0.00
 */

#ifndef __SAC_MOTOR_MOTOR_H___
#define __SAC_MOTOR_MOTOR_H___

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
 * @brief 初期化関数
 * @detail モータを停止し、エンコーダのカウントをリセットする。
 */
void motorsInit(void);

/**
 * @brief モータ制御
 * @detail エンコーダ処理とモータ出力の更新を行う。
 */
void motorsControl_1ms(void);

/**
 * @brief モータを駆動
 * @param [in] nrmPwrL 左モータへの出力指示(-1.0～+1.0）
 * @param [in] nrmPwrR 右モータへの出力指示(-1.0～+1.0）
 */
void motorsDrive(const float nrmPwrL, const float nrmPwrR);

/**
 * @brief エンコーダが検出した総回転数をリセットする
 */
void motorsResetRound(void);

/**
 * @brief モータへの出力を取得
 * @param [out] nrmPwrL 左モータへの出力指示値の格納先アドレス
 * @param [out] nrmPwrR 右モータへの出力指示値の格納先アドレス
 */
void motorsReadPower(float* nrmPwrL, float* nrmPwrR);

/**
 * @brief モータの回転数を取得
 * @param [out] rpsL 左モータへの回転数[rps]の格納先アドレス
 * @param [out] rpsR 右モータへの回転数[rps]の格納先アドレス
 */
void motorsReadRps(float* rpsL, float* rpsR);

/**
 * @brief モータの回転量を取得
 * @param [out] roundL 左モータへの総回転量の格納先アドレス
 * @param [out] roundR 右モータへの総回転量の格納先アドレス
 */
void motorsReadRound(float* roundL, float* roundR);

#if SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST
/**
 * @brief motorのデバッグ用関数
 * @param [out] strBuffer printfデバッグ用の文字列格納先
 * @param [in] maxBufferSize 文字列の最大格納数
 * @return 生成した文字列の長さ
 */
uint8_t motorTest(char* strBuffer, uint8_t maxBufferSize);
#endif /* SAC_DEBUGMODE == DEBUGMODE_MOTOR_TEST */

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __SAC_MOTOR_MOTOR_H___ */
