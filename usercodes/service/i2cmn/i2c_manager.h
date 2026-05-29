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
/**
 * @brief ABC_t 型の列挙体
 */

typedef struct
{
    uint8_t     target_address; //! ターゲットアドレス
    uint16_t    interval_ms;    //! 通信間隔(ms)
    uint8_t*    send_datas;     //! 送信データ
    uint8_t     send_size;      //! 送信データサイズ
    uint8_t*    receive_datas;  //! 受信データ
    uint8_t     receive_size;   //! 受信データサイズ
}I2C_transaction_t;

/*
メモ
送信はDMAが勝手に行う。DMAが完了したらコールバックされる
1バイト30us程度だとすると、1msで約30バイト。
タイマー割り込み内で、所望の周期で送信依頼されたバイト列をDMAにセットする。
データをセット。
タイマー割り込み内でソートして、最新の送信データを決める。

*/


/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/

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

void i2cmanagerInit(void);
void i2cmanagerControl_1ms_A(void);
void i2cmanagerControl_1ms_B(void);
void i2cmanagerMainloop(void);
void i2cmanagerGyroRead(uint16_t* gyroXYZ[3], uint16_t* accelXYZ[3], uint16_t* temp);
void i2cmanagerDisplay(uint8_t* buffer, uint8_t size);
bool i2cmanagerGyroReady(void);

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __I2C_MANAGER_H___*/
