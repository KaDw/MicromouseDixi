/*
 * mpu6050.c
 *
 *  Created on: 21.09.2017
 *      Author: Karol
 */

#include <math.h>
#include "mpu6050.h"
#include "i2c.h"


//uint8_t i2cRxBuffer[6];

uint8_t mpu_selftest(){
	uint8_t i2cTxBuffer[4];

	uint8_t rawData[4];
	uint8_t selfTest[6];
	float factoryTrim[6];

	if(HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)MPU6050_ADDRESS, 5, 100) != HAL_OK)
		return 0;

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_GYRO_CONFIG;
	i2cTxBuffer[1] = 0xE0; // enable self test, range +/- 250deg/s
	i2cTxBuffer[2] = (uint8_t)MPU6050_REG_ACCEL_CONFIG;
	i2cTxBuffer[3] = 0xF0; // enable self test, range+/- 8 g

	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 4, 100);

	HAL_Delay(250);

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_SELF_TEST_X;
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 1, 100);

	HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, rawData, 4, 100);

	// acc test result (5 bit)
	selfTest[0] = (rawData[0] >> 3) | (rawData[3] & 0x30) >> 4 ; // X
	selfTest[1] = (rawData[1] >> 3) | (rawData[3] & 0x0C) >> 4 ; // Y
	selfTest[2] = (rawData[2] >> 3) | (rawData[3] & 0x03) >> 4 ; // Z

	// gyro test result (5 bit)
	selfTest[3] = rawData[0]  & 0x1F ; // XG_TEST result is a five-bit unsigned integer
	selfTest[4] = rawData[1]  & 0x1F ; // YG_TEST result is a five-bit unsigned integer
	selfTest[5] = rawData[2]  & 0x1F ; // ZG_TEST result is a five-bit unsigned integer


	factoryTrim[0] = (4096.0*0.34)*(powf( (0.92/0.34) , (((float)selfTest[0] - 1.0)/30.0))); // FT[Xa] factory trim calculation
	factoryTrim[1] = (4096.0*0.34)*(powf( (0.92/0.34) , (((float)selfTest[1] - 1.0)/30.0))); // FT[Ya] factory trim calculation
	factoryTrim[2] = (4096.0*0.34)*(powf( (0.92/0.34) , (((float)selfTest[2] - 1.0)/30.0))); // FT[Za] factory trim calculation
	factoryTrim[3] =  ( 25.0*131.0)*(powf( 1.046 , ((float)selfTest[3] - 1.0) ));             // FT[Xg] factory trim calculation
	factoryTrim[4] =  (-25.0*131.0)*(powf( 1.046 , ((float)selfTest[4] - 1.0) ));             // FT[Yg] factory trim calculation
	factoryTrim[5] =  ( 25.0*131.0)*(powf( 1.046 , ((float)selfTest[5] - 1.0) ));             // FT[Zg] factory trim calculation

	for (int i = 0; i < 6; i++) {
		if((100.0 + 100.0*((float)selfTest[i] - factoryTrim[i])/factoryTrim[i]) > 1.0f)
			return 0;
	}

	return 1;

}


void mpu_init(){
	uint8_t i2cTxBuffer[6];

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_PWR_MGMT_1;
	i2cTxBuffer[1] = 0x03; // PLL with Z axis gyroscope reference
	i2cTxBuffer[2] = (uint8_t)MPU6050_REG_CONFIG; //TODO test LPF configrations
	i2cTxBuffer[3] = 0x00; // disable FSYNC, DLPF = 256Hz (default rate)
	i2cTxBuffer[4] = (uint8_t)MPU6050_REG_SMPLRT_DIV;
	i2cTxBuffer[5] = 0x07; // sample rate 1kHz (8 / (1+7))

	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 6, 100);

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_GYRO_CONFIG;
	i2cTxBuffer[1] = 0x00 | (MPU6050_SCALE_250DPS << 3); // MPU6050_SCALE_250DPS

	i2cTxBuffer[2] = (uint8_t)MPU6050_REG_ACCEL_CONFIG;
	i2cTxBuffer[3] = 0x00 | (MPU6050_RANGE_2G << 3);

	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 4, 100);


}
