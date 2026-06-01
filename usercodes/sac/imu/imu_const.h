/**
 * @file imu_const.h
 * @brief 簡単な説明(定数宣言)  
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

#ifndef __IMU_CONST_H__
#define __IMU_CONST_H__

#ifdef __cplusplus
extern "C" {
#endif

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/

/**
 * @brief 温度補正用の構造体
 * 
 */
typedef struct
{
    float gain;     //! 温度補正ゲイン
    float offset;   //! 温度補正オフセット
}imu_correction_param_t;

/**
 * @brief 6軸分の補正構造体
 * 
 */
typedef struct
{
    struct
    {
        imu_correction_param_t X;
        imu_correction_param_t Y;
        imu_correction_param_t Z;
    }accel;                                 //! 加速度センサの各軸補正値
    struct
    {
        imu_correction_param_t ROLL;
        imu_correction_param_t PITCH;
        imu_correction_param_t YAW;
    }gyro;                                  //! 角速度センサの各軸補正値
}imu_correction_params_t;
/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Declaration START VVVV========================*/
extern const imu_correction_params_t IMU_CORRECTION_PARAMS;
extern const float IMU_ACCEL_LPF_GAIN;
extern const float IMU_GYRO_LPF_GAIN;
extern const float IMU_TEMP_LPF_GAIN;
extern const float IMU_ACCEL_CONVERSION_GAIN;
extern const float IMU_GYRO_CONVERSION_GAIN;
extern const float IMU_TEMP_CONVERSION_GAIN;
extern const float IMU_TEMP_CONVERSION_OFFSET;

/*========AAAA GLOBAL Variable Declaration END AAAA==========================*/

#ifdef __cplusplus
}
#endif

#endif /*__IMU_CONST_H__*/

