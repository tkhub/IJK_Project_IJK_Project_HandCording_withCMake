/**
 * @file i2c_manager.c
 * @brief I2C通信のマネージャ
 * @author takap
 * @date Nov 30, 2025
 * @Version 0.00
 * @detail 一本のI2C通信バスから複数のデバイスの送受信を管理するモジュール
 */

/*========VVVV Include Standard Header START VVVV============================*/
#include <orgtypedef.h>
/*========AAAA Include Standard Header END AAAA==============================*/

/*========VVVV Include Local Header START VVVV===============================*/
#include "i2c.h"
// #include "stm32f3xx_hal.h"
#include "i2c_manager.h"
#include "i2c_manager_conf.h"
#include "../sac/imu/imu.h"


/*========AAAA Include Local Header END AAAA=================================*/

/*========VVVV Typedef Definition START VVVV=================================*/

/*========AAAA Typedef Definition END AAAA===================================*/

/*========VVVV MACRO Definition START VVVV===================================*/
#define ICM42688P_ADDR_IDX_WHO_AM_I  0
#define ICM42688P_ADDR_IDX_PWR_MGMT0 1
#define ICM42688P_ADDR_IDX_GYRO_CNF0 2
#define ICM42688P_ADDR_IDX_ACCL_CNF0 3
#define ICM42688P_ADDR_IDX_TEMP_DTX1 4
#define ICM42688P_ADDR_IDX_ACCL_DTX1 5
#define ICM42688P_ADDR_IDX_GYRO_DTX1 6

#define ICM42688P_REGDATA_IDX_ACK_OK    0
#define ICM42688P_REGDATA_IDX_LOW_NOIZE 1
#define ICM42688P_REGDATA_IDX_ODR_2KHZ  2

#define ICM42688P_BURST_READ_SIZE 14

/*========AAAA MACRO Definition END AAAA=====================================*/

/*========VVVV GLOBAL Variable Definition START VVVV=========================*/
/* int global_var; */ /* ヘッダファイルで説明済みのためDoxygenのコメントは不要 */

/*========AAAA GLOBAL Variable Definition END AAAA===========================*/

/*========VVVV Private Variable Definition START VVVV========================*/

static uint8_t ICM42688P_ADDR_TBL[] =
{
    0x75,   // ICM42688P_ADDR_IDX_WHO_AM_I
    0x4E,   // ICM42688P_ADDR_IDX_PWR_MGMT0
    0x4F,   // ICM42688P_ADDR_IDX_GYRO_CNF0
    0x50,   // ICM42688P_ADDR_IDX_ACCL_CNF0
    0x1D,   // ICM42688P_ADDR_IDX_TEMP_DTX1
    0x1F,   // ICM42688P_ADDR_IDX_ACCL_DTX1
    0x25    // ICM42688P_ADDR_IDX_GYRO_DTX1
};

static uint8_t ICM42688P_DAT_TBL[3] =
{
  0x47, // ICM42688P_REGDATA_IDX_ACK_OK
  0x0F, // ICM42688P_REGDATA_IDX_LOW_NOIZE
  0x0C  // ICM42688P_REGDATA_IDX_ODR_2KHZ
};

static uint8_t i2cmanager_gyro_datas_dma[ICM42688P_BURST_READ_SIZE];
static bool i2cmanager_gyro_initialized = false;

static uint8_t i2cmanager_display_buffer[1024];

/*========AAAA Private Variable Definition END AAAA==========================*/

/*========VVVV Private Function Prototype Declaration START VVVV=============*/
static bool i2cmanager_gyro_Init(void);
static void i2cmanager_gyro_Request(void);
static bool i2cmanager_display_Init(void);
static void i2cmanager_display_Send(void);


/*========AAAA Private Function Prototype Declaration END AAAA===============*/

/*========VVVV GLOBAL Function Definition START VVVV=========================*/
//! 初期化関数
void i2cmanagerInit(void)
{
    i2cmanager_gyro_initialized =i2cmanager_gyro_Init();
    i2cmanager_display_Init();
    imuInit(i2cmanager_gyro_datas_dma);
}


//! 1ms前半で呼び出される関数
void i2cmanagerControl_1ms_A(void)
{
    i2cmanager_display_Send();
    imuMeasure_ReciveEnd();
}

//! 1ms後半で呼び出される関数
void i2cmanagerControl_1ms_B(void)
{
    i2cmanager_gyro_Request();
}

//! メインループで呼び出される関数
void i2cmanagerMainloop(void)
{
    /* NOP */
}



/*========AAAA GLOBAL Function Definition END AAAA===========================*/

/*========VVVV Private Function Definition START VVVV========================*/
/* プロトタイプ宣言で説明済みのためDoxygenのコメントは不要 */
/*
static int internalTestFunc(int* buffer, int size) {

}
*/
static bool i2cmanager_gyro_Init(void)
{
    HAL_StatusTypeDef status;
    uint8_t readData;
    uint8_t writeReg;
    uint8_t writeValue;

    // WHO AM I
    status = HAL_I2C_Mem_Read(  &hi2c1,
                                (ICM42688P_ADDRESS << 1),
                                ICM42688P_ADDR_TBL[ICM42688P_ADDR_IDX_WHO_AM_I],
                                I2C_MEMADD_SIZE_8BIT,
                                &readData,
                                1,
                                100);
    if (status != HAL_OK || readData != ICM42688P_DAT_TBL[ICM42688P_REGDATA_IDX_ACK_OK])
    {
        return false;
    }

    // SET PWR MODE
    writeReg = ICM42688P_ADDR_TBL[ICM42688P_ADDR_IDX_PWR_MGMT0];
    writeValue = ICM42688P_DAT_TBL[ICM42688P_REGDATA_IDX_LOW_NOIZE];
    status = HAL_I2C_Mem_Write( &hi2c1,
                                (ICM42688P_ADDRESS << 1),
                                writeReg,
                                I2C_MEMADD_SIZE_8BIT,
                                &writeValue,
                                1,
                                100);
    if (status != HAL_OK)
    {
        return false;
    }

    // SET GYRO CONFIG
    writeReg = ICM42688P_ADDR_TBL[ICM42688P_ADDR_IDX_GYRO_CNF0];
    writeValue = ICM42688P_DAT_TBL[ICM42688P_REGDATA_IDX_ODR_2KHZ];
    status = HAL_I2C_Mem_Write(&hi2c1,
                                (ICM42688P_ADDRESS << 1),
                                writeReg,
                                I2C_MEMADD_SIZE_8BIT,
                                &writeValue,
                                1,
                                100);
    if (status != HAL_OK)
    {
        return false;
    }

    // SET ACCL CONFIG
    writeReg = ICM42688P_ADDR_TBL[ICM42688P_ADDR_IDX_ACCL_CNF0];
    writeValue = ICM42688P_DAT_TBL[ICM42688P_REGDATA_IDX_ODR_2KHZ];
    status = HAL_I2C_Mem_Write( &hi2c1,
                                (ICM42688P_ADDRESS << 1),
                                writeReg,
                                I2C_MEMADD_SIZE_8BIT,
                                &writeValue,
                                1,
                                100);
    if (status != HAL_OK)
    {
        return false;
    }

    return true;
}

static bool i2cmanager_display_Init(void)
{
    // ディスプレイの初期化処理をここに記述
    int i;
    for (i = 0; i < 1024; i++)
    {
        i2cmanager_display_buffer[i] = i % 0xFF;
    }
    return true;
}

static void i2cmanager_gyro_Request(void)
{
    HAL_I2C_Mem_Read_DMA(   &hi2c1,                             // I2C Handler Pointer
                            (ICM42688P_ADDRESS << 1),       // Device Address
                            ICM42688P_ADDR_TBL[ICM42688P_ADDR_IDX_TEMP_DTX1],   // Device Register Address
                            1,                                                  // Size of Device Register Address
                            i2cmanager_gyro_datas_dma,     // Pointer to data buffer
                            ICM42688P_BURST_READ_SIZE   // Amount of data to be read
                            );
}

static void i2cmanager_display_Send(void)
{
    // ディスプレイにデータを送信する処理をここに記述
    // 例: HAL_I2C_Mem_Write(&hi2c1, (DISPLAY_ADDRESS << 1), DISPLAY_DATA_REG, I2C_MEMADD_SIZE_8BIT, i2cmanager_display_buffer, sizeof(i2cmanager_display_buffer), 100);
    HAL_I2C_Mem_Write_DMA(&hi2c1, (0x3C << 1), 0x00, 1, i2cmanager_display_buffer, 20);
}

/*========AAAA Private Function Definition END AAAA==========================*/

