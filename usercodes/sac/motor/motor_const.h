/**
 * @file motor_const.h
 * @brief 簡単な説明  
 * @author takap
 * @date Nov 14, 2025
 * @Version 0.00
 */

#ifndef __SAC_MOTOR_MOTOR_CONST_H___
#define __SAC_MOTOR_MOTOR_CONST_H___

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

/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/** 
 * @brief マクロ MAX_NANK の説明
 */
/* # define MAX_NANKA 256 */

// #define MOTOR_RPS_RECORD_SIZE 64
#define MOTOR_RPS_RECORD_SIZE 32

/*========VVVV Filter Configuration START VVVV================================*/

/*========AAAA Filter Configuration END AAAA==================================*/



/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
/**
 * @brief グローバル変数 lobal_var の説明  
*/
/**
 * extern int global_var;
*/
extern const float STOP_NRMPWR_THRESHOLD;
extern const uint16_t ENCODER_RSOLUTION;
extern const uint16_t ENCODER_DIR_GAP;
extern const bool POWER_DIRECTION_INV_L;
extern const bool POWER_DIRECTION_INV_R;
extern const bool ENCODER_DIRECTION_INV_L;
extern const bool ENCODER_DIRECTION_INV_R;
extern const uint16_t MOTOR_PWM_MAX;
extern const float ENCODER_IIR_GAIN;
extern const float MOTOR_INTR_CYCLE_MS;

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

#endif /* __SAC_MOTOR_MOTOR_CONST_H___ */
