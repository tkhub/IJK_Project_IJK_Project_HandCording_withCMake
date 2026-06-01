/**
 * @file i2c_manager.h
 * @brief I2Cマネージャ
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

#ifndef __I2C_MANAGER_H___
#define __I2C_MANAGER_H___

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/

/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
#define GYRO_DATAS_SIZE 14
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/

/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/

/**
 * @brief i2cマネージャの初期化関数
 * @details ジャイロやディスプレイの初期化を行う
 */
void i2cmanagerInit(void);

/**
 * @brief 1ms周期で呼び出される制御関数(A)
 * @details 1msの前半で呼び出される
 */
void i2cmanagerControl_1ms_A(void);

/**
 * @brief 1ms周期で呼び出される制御関数(B)
 * @details 1msの後半で呼び出される
 */
void i2cmanagerControl_1ms_B(void);

/**
 * @brief メインループ処理を行う
 * @details メインループ処理を行う
 */
void i2cmanagerMainloop(void);



/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __I2C_MANAGER_H___*/
