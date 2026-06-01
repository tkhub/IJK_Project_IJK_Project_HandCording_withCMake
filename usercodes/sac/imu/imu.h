/**
 * @file imu.h
 * @brief IMUセンサのSACヘッダファイル
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

#ifndef __IMU_H__
#define __IMU_H__

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
 * @brief 加速度のデータ構造体
 */
typedef struct
{
    float x;        //! X軸         搭載方向は前方を正とする
    float y;        //! Y軸         搭載方向は左側を正とする
    float z;        //! Z軸         搭載方向は上側を正とする
}accel_t;

/**
 * @brief 角速度のデータ構造体
 */
typedef struct
{
    float roll;     //! ロール軸    X軸を時計回りに回転する方向を正とする(右側が下がる方向が正)
    float pitch;    //! ピッチ軸    Y軸を時計回りに回転する方向を正とする(前方が下がる方向が正)
    float yaw;      //! ヨー軸      Z軸を時計回りに回転する方向を正とする(機体が反時計回りに回転する方向が正)
}gyro_t;

/**
 * @brief IMUのデータ構造体(6軸分)
 */
typedef struct
{
    accel_t accel;  //! 加速度(3軸)
    gyro_t gyro;    //! 角速度(3軸)
}imu_t;

/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/

/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

/*========VVVV GLOBAL Function Declaration START VVVV========================*/
/**
 * @brief 初期化関数
 * @param [in] ジャイロセンサの情報をDMAで保存する対象のアドレス
 * @details 初期化。現在のところジャイロセンサそのものの初期化はI2Cマネージャが行うので、その他の初期化処理をここで行う。
 */
void imuInit(uint8_t *datas);

/**
 * @brief imu制御
 * @details I2CマネージャのDMAコールバックなどDMAが終了しているタイミングにて呼び出してもらう。事前に受け取ったDMA用バッファの内容をコピーするなどの処理を行う。
 */
void imuMeasure_ReciveEnd(void);

/**
 * @brief 1ms周期で呼び出される制御関数
 */

void imuMeasure_1ms(void);

/**
 * @brief IMUセンサの各軸周りの角速度を取得
 * 
 * @return gyro_t 角速度[rps]
 */
gyro_t imuGyro(void);

/**
 * @brief IMUセンサの各軸の加速度を取得
 * 
 * @return accel_t 加速度[g]
 */
accel_t imuAccel(void);


/**
 * @brief IMUセンサの全軸分取得
 * 
 * @return imu_t 全軸分のデータ
 */
imu_t imuAll(void);

#if SAC_DEBUGMODE == DEBUGMODE_IMU_TEST
/**
 * @brief IMUのデバッグ用関数
 * @param [out] strBuffer printfデバッグ用の文字列格納先
 * @param [in] maxBufferSize 文字列の最大格納数
 * @return 生成した文字列の長さ
 */
uint8_t imuTest(char* strBuffer, uint8_t maxBufferSize);
#endif /* SAC_DEBUGMODE == DEBUGMODE_IMU_TEST */

/*========AAAA GLOBAL Function Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /* __IMU_H__*/

