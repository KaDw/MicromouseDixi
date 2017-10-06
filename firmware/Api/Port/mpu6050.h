#include "stm32f4xx.h"

#ifndef MPU6050_H_
#define MPU6050_H_

/* Sensitivity Scale Factor 131 LSB/dps (250dps)*/
#define MPU6050_GYRO_SENSITIVITY 131.0f
#define MPU6050_ACCEL_SENSITIVITY 16384.0f

#define MPU6050_ADDRESS 			(0x68<<1)

#define MPU6050_REG_XG_OFFS_TC       0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_REG_YG_OFFS_TC       0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_REG_ZG_OFFS_TC       0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_REG_X_FINE_GAIN      0x03 //[7:0] X_FINE_GAIN
#define MPU6050_REG_Y_FINE_GAIN      0x04 //[7:0] Y_FINE_GAIN
#define MPU6050_REG_Z_FINE_GAIN      0x05 //[7:0] Z_FINE_GAIN
#define MPU6050_REG_XA_OFFS_H        0x06 //[15:0] XA_OFFS
#define MPU6050_REG_XA_OFFS_L_TC     0x07
#define MPU6050_REG_YA_OFFS_H        0x08 //[15:0] YA_OFFS
#define MPU6050_REG_YA_OFFS_L_TC     0x09
#define MPU6050_REG_ZA_OFFS_H        0x0A //[15:0] ZA_OFFS
#define MPU6050_REG_ZA_OFFS_L_TC     0x0B
#define MPU6050_REG_SELF_TEST_X      0x0D //[7:5] XA_TEST[4-2], [4:0] XG_TEST[4-0]
#define MPU6050_REG_SELF_TEST_Y      0x0E //[7:5] YA_TEST[4-2], [4:0] YG_TEST[4-0]
#define MPU6050_REG_SELF_TEST_Z      0x0F //[7:5] ZA_TEST[4-2], [4:0] ZG_TEST[4-0]
#define MPU6050_REG_SELF_TEST_A      0x10 //[5:4] XA_TEST[1-0], [3:2] YA_TEST[1-0], [1:0] ZA_TEST[1-0]
#define MPU6050_REG_XG_OFFS_USRH     0x13 //[15:0] XG_OFFS_USR
#define MPU6050_REG_XG_OFFS_USRL     0x14
#define MPU6050_REG_YG_OFFS_USRH     0x15 //[15:0] YG_OFFS_USR
#define MPU6050_REG_YG_OFFS_USRL     0x16
#define MPU6050_REG_ZG_OFFS_USRH     0x17 //[15:0] ZG_OFFS_USR
#define MPU6050_REG_ZG_OFFS_USRL     0x18
#define MPU6050_REG_SMPLRT_DIV       0x19
#define MPU6050_REG_CONFIG           0x1A
#define MPU6050_REG_GYRO_CONFIG      0x1B
#define MPU6050_REG_ACCEL_CONFIG     0x1C
#define MPU6050_REG_FF_THR           0x1D
#define MPU6050_REG_FF_DUR           0x1E
#define MPU6050_REG_MOT_THR          0x1F
#define MPU6050_REG_MOT_DUR          0x20
#define MPU6050_REG_ZRMOT_THR        0x21
#define MPU6050_REG_ZRMOT_DUR        0x22
#define MPU6050_REG_FIFO_EN          0x23
#define MPU6050_REG_I2C_MST_CTRL     0x24
#define MPU6050_REG_I2C_SLV0_ADDR    0x25
#define MPU6050_REG_I2C_SLV0_REG     0x26
#define MPU6050_REG_I2C_SLV0_CTRL    0x27
#define MPU6050_REG_I2C_SLV1_ADDR    0x28
#define MPU6050_REG_I2C_SLV1_REG     0x29
#define MPU6050_REG_I2C_SLV1_CTRL    0x2A
#define MPU6050_REG_I2C_SLV2_ADDR    0x2B
#define MPU6050_REG_I2C_SLV2_REG     0x2C
#define MPU6050_REG_I2C_SLV2_CTRL    0x2D
#define MPU6050_REG_I2C_SLV3_ADDR    0x2E
#define MPU6050_REG_I2C_SLV3_REG     0x2F
#define MPU6050_REG_I2C_SLV3_CTRL    0x30
#define MPU6050_REG_I2C_SLV4_ADDR    0x31
#define MPU6050_REG_I2C_SLV4_REG     0x32
#define MPU6050_REG_I2C_SLV4_DO      0x33
#define MPU6050_REG_I2C_SLV4_CTRL    0x34
#define MPU6050_REG_I2C_SLV4_DI      0x35
#define MPU6050_REG_I2C_MST_STATUS   0x36
#define MPU6050_REG_INT_PIN_CFG      0x37
#define MPU6050_REG_INT_ENABLE       0x38
#define MPU6050_REG_DMP_INT_STATUS   0x39
#define MPU6050_REG_INT_STATUS       0x3A
#define MPU6050_REG_ACCEL_XOUT_H     0x3B
#define MPU6050_REG_ACCEL_XOUT_L     0x3C
#define MPU6050_REG_ACCEL_YOUT_H     0x3D
#define MPU6050_REG_ACCEL_YOUT_L     0x3E
#define MPU6050_REG_ACCEL_ZOUT_H     0x3F
#define MPU6050_REG_ACCEL_ZOUT_L     0x40
#define MPU6050_REG_TEMP_OUT_H       0x41
#define MPU6050_REG_TEMP_OUT_L       0x42
#define MPU6050_REG_GYRO_XOUT_H      0x43
#define MPU6050_REG_GYRO_XOUT_L      0x44
#define MPU6050_REG_GYRO_YOUT_H      0x45
#define MPU6050_REG_GYRO_YOUT_L      0x46
#define MPU6050_REG_GYRO_ZOUT_H      0x47
#define MPU6050_REG_GYRO_ZOUT_L      0x48
#define MPU6050_REG_EXT_SENS_DATA_00 0x49
#define MPU6050_REG_EXT_SENS_DATA_01 0x4A
#define MPU6050_REG_EXT_SENS_DATA_02 0x4B
#define MPU6050_REG_EXT_SENS_DATA_03 0x4C
#define MPU6050_REG_EXT_SENS_DATA_04 0x4D
#define MPU6050_REG_EXT_SENS_DATA_05 0x4E
#define MPU6050_REG_EXT_SENS_DATA_06 0x4F
#define MPU6050_REG_EXT_SENS_DATA_07 0x50
#define MPU6050_REG_EXT_SENS_DATA_08 0x51
#define MPU6050_REG_EXT_SENS_DATA_09 0x52
#define MPU6050_REG_EXT_SENS_DATA_10 0x53
#define MPU6050_REG_EXT_SENS_DATA_11 0x54
#define MPU6050_REG_EXT_SENS_DATA_12 0x55
#define MPU6050_REG_EXT_SENS_DATA_13 0x56
#define MPU6050_REG_EXT_SENS_DATA_14 0x57
#define MPU6050_REG_EXT_SENS_DATA_15 0x58
#define MPU6050_REG_EXT_SENS_DATA_16 0x59
#define MPU6050_REG_EXT_SENS_DATA_17 0x5A
#define MPU6050_REG_EXT_SENS_DATA_18 0x5B
#define MPU6050_REG_EXT_SENS_DATA_19 0x5C
#define MPU6050_REG_EXT_SENS_DATA_20 0x5D
#define MPU6050_REG_EXT_SENS_DATA_21 0x5E
#define MPU6050_REG_EXT_SENS_DATA_22 0x5F
#define MPU6050_REG_EXT_SENS_DATA_23 0x60
#define MPU6050_REG_MOT_DETECT_STATUS    0x61
#define MPU6050_REG_I2C_SLV0_DO      0x63
#define MPU6050_REG_I2C_SLV1_DO      0x64
#define MPU6050_REG_I2C_SLV2_DO      0x65
#define MPU6050_REG_I2C_SLV3_DO      0x66
#define MPU6050_REG_I2C_MST_DELAY_CTRL   0x67
#define MPU6050_REG_SIGNAL_PATH_RESET    0x68
#define MPU6050_REG_MOT_DETECT_CTRL      0x69
#define MPU6050_REG_USER_CTRL        0x6A
#define MPU6050_REG_PWR_MGMT_1       0x6B
#define MPU6050_REG_PWR_MGMT_2       0x6C
#define MPU6050_REG_BANK_SEL         0x6D
#define MPU6050_REG_MEM_START_ADDR   0x6E
#define MPU6050_REG_MEM_R_W          0x6F
#define MPU6050_REG_DMP_CFG_1        0x70
#define MPU6050_REG_DMP_CFG_2        0x71
#define MPU6050_REG_FIFO_COUNTH      0x72
#define MPU6050_REG_FIFO_COUNTL      0x73
#define MPU6050_REG_FIFO_R_W         0x74
#define MPU6050_REG_WHO_AM_I         0x75 /* always 0x68 */

typedef enum
{
    MPU6050_CLOCK_KEEP_RESET      = 0b111,
    MPU6050_CLOCK_EXTERNAL_19MHZ  = 0b101,
    MPU6050_CLOCK_EXTERNAL_32KHZ  = 0b100,
    MPU6050_CLOCK_PLL_ZGYRO       = 0b011,
    MPU6050_CLOCK_PLL_YGYRO       = 0b010,
    MPU6050_CLOCK_PLL_XGYRO       = 0b001,
    MPU6050_CLOCK_INTERNAL_8MHZ   = 0b000
} mpu6050_clockSource_t;

typedef enum
{
    MPU6050_SCALE_2000DPS         = 0b11,
    MPU6050_SCALE_1000DPS         = 0b10,
    MPU6050_SCALE_500DPS          = 0b01,
    MPU6050_SCALE_250DPS          = 0b00
} mpu6050_dps_t;

typedef enum
{
    MPU6050_RANGE_16G             = 0b11,
    MPU6050_RANGE_8G              = 0b10,
    MPU6050_RANGE_4G              = 0b01,
    MPU6050_RANGE_2G              = 0b00,
} mpu6050_range_t;

typedef enum
{
    MPU6050_DELAY_3MS             = 0b11,
    MPU6050_DELAY_2MS             = 0b10,
    MPU6050_DELAY_1MS             = 0b01,
    MPU6050_NO_DELAY              = 0b00,
} mpu6050_onDelay_t;

typedef enum
{
    MPU6050_DHPF_HOLD             = 0b111,
    MPU6050_DHPF_0_63HZ           = 0b100,
    MPU6050_DHPF_1_25HZ           = 0b011,
    MPU6050_DHPF_2_5HZ            = 0b010,
    MPU6050_DHPF_5HZ              = 0b001,
    MPU6050_DHPF_RESET            = 0b000,
} mpu6050_dhpf_t;

typedef enum
{
    MPU6050_DLPF_6                = 0b110,
    MPU6050_DLPF_5                = 0b101,
    MPU6050_DLPF_4                = 0b100,
    MPU6050_DLPF_3                = 0b011,
    MPU6050_DLPF_2                = 0b010,
    MPU6050_DLPF_1                = 0b001,
    MPU6050_DLPF_0                = 0b000,
} mpu6050_dlpf_t;

//

typedef struct
{
	float x;
	float y;
	float z;
}mpu_data;

extern mpu_data a_data;
extern mpu_data g_data;


uint8_t mpu_selftest() __attribute__((optimize(0)));
void mpu_init();
void mpu_calibrate();
float* mpu_raw_data(float* data);


#endif /* MPU6050_H_ */
