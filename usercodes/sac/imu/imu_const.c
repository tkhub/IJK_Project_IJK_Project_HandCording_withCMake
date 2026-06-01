/**
 * @file imu_const.c
 * @brief 
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 */

/*========VVVV Include Standard Header START VVVV============================*/
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "imu_const.h"
/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV MACRO Definition START VVVV===================================*/

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/* int global_var; */ /* ヘッダファイルで説明済みのためDoxygenのコメントは不要 */
const imu_correction_params_t IMU_CORRECTION_PARAMS = {
    .gyroXYZ = {
        {1.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 0.0f}
    },
    .accelXYZ = {
        {1.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 0.0f}
    }
};
const float IMU_ACCEL_LPF_GAIN = 1.0F;
const float IMU_GYRO_LPF_GAIN = 1.0F;;
const float IMU_TEMP_LPF_GAIN = 0.05F;
const float IMU_ACCEL_CONVERSION_GAIN = 1.0F/2048.0F;
const float IMU_GYRO_CONVERSION_GAIN = 1.04F/16.4F;
const float IMU_TEMP_CONVERSION_GAIN = 1.0F/132.48F;
const float IMU_TEMP_CONVERSION_OFFSET = 25.0F;
/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

