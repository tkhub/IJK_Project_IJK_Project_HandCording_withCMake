/**
 * @file imu.c
 * @brief ジャイロセンサのSAC
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>
#include <test_and_debug.h>

#if SAC_DEBUGMODE == DEBUGMODE_IMU_TEST
#include <xprintf.h>
#include <stdlib.h>
#include <string.h>
#endif /* SAC_DEBUGMODE == DEBUGMODE_LINEMKR_TEST */

/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
// #include "i2c_manager.h"
#include "imu.h"
#include "imu_const.h"
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/

/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/

/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/

/**
 * @brief I2CマネージャのDMA受信バッファのポインタ
*/
static uint8_t* imu_dma_datas;

/**
 * @brief IMUのジャイロデータの生値
 */
static int16_t imu_gyro_rawdatas[3];

/**
 * @brief IMUの加速度データの生値
 */
static int16_t imu_accel_rawdatas[3];

/**
 * @brief IMUの温度データの生値
 */
static uint16_t imu_temp_rawdata;


/**
 * @brief imuのデータ保管場所
 */
static imu_t imu_data;

/**
 * @brief imuの温度保管場所
 */
static float imu_temp_data;

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/

/**
 * @brief ローパスフィルタ関数
 * 
 * @param current_value 現在の値(前周期のこの関数の戻り値)
 * @param new_value 検出した新しい値
 * @param gain 0.0～1.0の値を使うこと
 * @return float 計算後の新しい値
 */
static float imuLPF(float current_value, float new_value, float gain);

/**
 * @brief IMU用の補正関数
 * 
 * @param base_value 補正前の値 
 * @param temp 温度
 * @param gain ゲイン
 * @param offset オフセット
 * @return float 補正後の値
 * @details 温度補正値で補正前の値を差し引く
 */
static float imuCorrection(float base_value, float temp, float gain, float offset);


/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
//! 初期化関数
void imuInit(uint8_t *datas)
{
    imu_dma_datas = datas;
}

//! ジャイロセンサのデータ受信完了時に呼び出される関数
void imuMeasure_ReciveEnd(void)
{
    //! DMAは受信が完了している状態なので取り出しは安全
    //! IMU内蔵の温度センサの値を取り出す。
    imu_temp_rawdata = (uint16_t)( ((uint16_t)imu_dma_datas[0] << 8) | ((uint16_t)imu_dma_datas[1]) );
    
    //! imu_dma_datas[2]＝X軸 15:8, imu_dma_datas[3] = X軸 7:0
    imu_accel_rawdatas[0] = (int16_t)( ((uint16_t)(imu_dma_datas[2] << 8)) | ((uint16_t)imu_dma_datas[3]) );
    //! imu_dma_datas[4]＝Y軸 15:8, imu_dma_datas[5] = Y軸 7:0
    imu_accel_rawdatas[1] = (int16_t)( ((uint16_t)(imu_dma_datas[4] << 8)) | ((uint16_t)imu_dma_datas[5]) );
    //! imu_dma_datas[6]＝Z軸 15:8, imu_dma_datas[7] = Z軸 7:0
    imu_accel_rawdatas[2] = (int16_t)( ((uint16_t)(imu_dma_datas[6] << 8)) | ((uint16_t)imu_dma_datas[7]) );
    //
    //! imu_dma_datas[8]＝ロール軸(X軸周り) 15:8, imu_dma_datas[9] = ロール軸(X軸周り) 7:0
    imu_gyro_rawdatas[0] = (int16_t)( ((uint16_t)(imu_dma_datas[8] << 8)) | ((uint16_t)imu_dma_datas[9]) );
    //! imu_dma_datas[10]＝ピッチ軸(Y軸周り) 15:8, imu_dma_datas[11] = ピッチ軸(Y軸周り) 7:0
    imu_gyro_rawdatas[1] = (int16_t)( ((uint16_t)(imu_dma_datas[10] << 8)) | ((uint16_t)imu_dma_datas[11]) );
    //! imu_dma_datas[12]＝ヨー軸(Z軸周り) 15:8, imu_dma_datas[13] = ヨー軸(Z軸周り) 7:0
    imu_gyro_rawdatas[2] = (int16_t)( ((uint16_t)(imu_dma_datas[12] << 8)) | ((uint16_t)imu_dma_datas[13]) );
}

void imuMeasure_1ms(void)
{
    //! TODO:温度補正処理未実装
    imu_temp_data = imuLPF(
                imu_temp_data,
                (((float)imu_temp_rawdata * IMU_TEMP_CONVERSION_GAIN ) + IMU_TEMP_CONVERSION_OFFSET),
                IMU_TEMP_LPF_GAIN);
    
    imu_data.accel.x = imuLPF(
                                imu_data.accel.x,
                                imuCorrection( (float)imu_accel_rawdatas[0] * IMU_ACCEL_CONVERSION_GAIN, imu_temp_data, IMU_CORRECTION_PARAMS.accel.X.gain, IMU_CORRECTION_PARAMS.accel.X.offset),
                                IMU_ACCEL_LPF_GAIN);
    imu_data.accel.y = imuLPF(
                                imu_data.accel.y,
                                imuCorrection((float)imu_accel_rawdatas[1] * IMU_ACCEL_CONVERSION_GAIN, imu_temp_data, IMU_CORRECTION_PARAMS.accel.Y.gain, IMU_CORRECTION_PARAMS.accel.Y.offset),
                                IMU_ACCEL_LPF_GAIN);
    imu_data.accel.z = imuLPF(
                                imu_data.accel.z,
                                imuCorrection((float)imu_accel_rawdatas[2] * IMU_ACCEL_CONVERSION_GAIN, imu_temp_data, IMU_CORRECTION_PARAMS.accel.Z.gain, IMU_CORRECTION_PARAMS.accel.Z.offset),
                                IMU_ACCEL_LPF_GAIN);

    imu_data.gyro.roll = imuLPF(
                                imu_data.gyro.roll,
                                imuCorrection((float)imu_gyro_rawdatas[0] * IMU_GYRO_CONVERSION_GAIN, imu_temp_data, IMU_CORRECTION_PARAMS.gyro.ROLL.gain, IMU_CORRECTION_PARAMS.gyro.ROLL.offset),
                                IMU_GYRO_LPF_GAIN);
    imu_data.gyro.pitch = imuLPF(
                                imu_data.gyro.pitch,
                                imuCorrection((float)imu_gyro_rawdatas[1] * IMU_GYRO_CONVERSION_GAIN, imu_temp_data, IMU_CORRECTION_PARAMS.gyro.PITCH.gain, IMU_CORRECTION_PARAMS.gyro.PITCH.offset),
                                IMU_GYRO_LPF_GAIN);
    imu_data.gyro.yaw = imuLPF(
                                imu_data.gyro.yaw,
                                imuCorrection((float)imu_gyro_rawdatas[2] * IMU_GYRO_CONVERSION_GAIN, imu_temp_data, IMU_CORRECTION_PARAMS.gyro.YAW.gain, IMU_CORRECTION_PARAMS.gyro.YAW.offset),
                                IMU_GYRO_LPF_GAIN);
}

#if SAC_DEBUGMODE == DEBUGMODE_IMU_TEST
uint8_t imuTest(char* strBuffer, uint8_t maxBufferSize) {
    xsnprintf(strBuffer,maxBufferSize, "temp=,%8.4f,X=,%-8.4f,Y=,%-8.4f,Z=,%-8.4f,ROLL=,%-8.4f,PITCH=,%-8.4f,YAW=,%-8.4f",
                            imu_temp_data,
                            imu_data.accel.x, imu_data.accel.y, imu_data.accel.z,
                            imu_data.gyro.roll, imu_data.gyro.pitch, imu_data.gyro.yaw);
    return 0;
}
#endif /* SAC_DEBUGMODE == DEBUGMODE_LINEMKR_TEST */
/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
static float imuLPF(float current_value, float new_value, float gain)
{
    if (gain < 0.0F)
    {
        gain = 0.0F;
    }
    else if (1.0F < gain)
    {
        gain = 1.0F;
    }
    return (new_value - current_value) * gain + current_value;
}

static float imuCorrection(float base_value, float temp, float gain, float offset)
{
    return (base_value - (temp * gain + offset));
}

/*========AAAA Private Function Definition END AAAA==========================*/

