/**
* @file sac.h
* @brief SACのヘッダファイル
* @author T.Ino
* @date 2025/10/27
*
* @note -
*/
#ifndef SAC_SAC_H_
#define SAC_SAC_H_

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "uisw/uisw.h"
#include "motor/motor.h"
#include "buzzer/buzzer.h"
#include "linmkrssr/linmkrssr.h"
#include "battery/battery.h"
#include "imu/imu.h"

/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/
/**
 * @enum Enum
 * 列挙体の説明
 */

/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/

/*! @var   global_var1
    @brief グローバル変数の説明
*/

/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/
/**
 * @brief SAC用のループ関数
 * @detail
 */
void sacLoop(void);

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#endif /* SAC_SAC_H_ */
