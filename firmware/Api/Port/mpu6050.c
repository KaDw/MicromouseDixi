#include <math.h>
#include "mpu6050.h"
#include "i2c.h"


//uint8_t i2cRxBuffer[6];
uint8_t raw_data[12];
mpu_data a_data;
mpu_data g_data;


uint8_t mpu_selftest(){
	uint8_t i2cTxBuffer[4];

	uint8_t rawData[4];
	uint8_t selfTest[6];
	float factoryTrim[6];

	if(HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)MPU6050_ADDRESS, 5, 100) != HAL_OK)
		return 1;

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_GYRO_CONFIG;
	i2cTxBuffer[1] = 0xE0; // enable self test, range +/- 250deg/s
	i2cTxBuffer[2] = (uint8_t)MPU6050_REG_ACCEL_CONFIG;
	i2cTxBuffer[3] = 0xF0; // enable self test, range+/- 8 g

	if(HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 4, 100)!= HAL_OK)
		return 1;

	HAL_Delay(250);

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_SELF_TEST_X;
	if(HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 1, 100) != HAL_OK)
		return 1;

	if(HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, rawData, 4, 100) != HAL_OK)
		return 1;

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
			return 1;
	}

	return 0;

}

void mpu_init(){
	uint8_t i2cTxBuffer[6];

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_PWR_MGMT_1;
	i2cTxBuffer[1] = 0x03; // PLL with Z axis gyroscope reference


	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 2, 100);

	HAL_Delay(200);

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_CONFIG; //TODO test LPF configrations
	i2cTxBuffer[1] = 0x00; // disable FSYNC, DLPF = 256Hz (default rate)
	i2cTxBuffer[2] = (uint8_t)MPU6050_REG_SMPLRT_DIV;
	i2cTxBuffer[3] = 0x07; // sample rate 1kHz (8 / (1+7))

	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 4, 100);

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_GYRO_CONFIG;
	i2cTxBuffer[1] = 0x00 | (MPU6050_SCALE_250DPS << 3); // MPU6050_SCALE_250DPS
	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_ACCEL_CONFIG;
	i2cTxBuffer[1] = 0x00 | (MPU6050_RANGE_2G << 3);

	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 4, 100);

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_INT_ENABLE;
	i2cTxBuffer[1] = 0x01;
	i2cTxBuffer[2] = (uint8_t)MPU6050_REG_INT_PIN_CFG;
	i2cTxBuffer[3] = 0x08; // MPU6050_SCALE_250DPS

	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 4, 100);

}

void mpu_calibrate(){
	uint8_t i2cTxBuffer[4];
	uint8_t data[8];

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_PWR_MGMT_1; // reset device
	i2cTxBuffer[1] = 0x80;
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 2, 100);

	HAL_Delay(200);

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_PWR_MGMT_1;
	i2cTxBuffer[1] = 0x03; // PLL with z axis gyroscope reference, disable sleep mode
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 2, 100);

	HAL_Delay(200);

//	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_PWR_MGMT_1;
//	i2cTxBuffer[1] = 0x00; // turn on internal clock
	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_USER_CTRL;
	i2cTxBuffer[1] = 0x0C; // reset fifo and dmp
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 2, 100);
	HAL_Delay(15);

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_USER_CTRL;
	i2cTxBuffer[1] = 0x40; // enable fifo
	i2cTxBuffer[2] = (uint8_t)MPU6050_REG_FIFO_EN;
	i2cTxBuffer[3] = 0x78; // en z-axis for gyro and acc for fifo
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 4, 100);
	// accumulate 100 samples in FIFO
	HAL_Delay(80);

	// disable fifo
	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_FIFO_EN;
	i2cTxBuffer[1] = 0x00; // en z-axis for gyro and acc for fifo
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 2, 100);

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_FIFO_COUNTH;
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 1, 100);
	// get number of samples in fifo
	HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, data, 2, 100);
	uint16_t fifoCount = (uint16_t)((data[0] << 8) | data[1]);
	fifoCount  = fifoCount / 8; // 2 bytes (gyro) + 6 bytes (acc)

	int32_t accel_offset[3] = {0, 0, 0};
	int32_t gyro_offset[3] = {0, 0, 0};

	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_FIFO_R_W;
	for(uint16_t i = 0; i < fifoCount+1; i++){
		//int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
		// read from fifo
		HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 1, 100);
		HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, data, 12, 100);

	    //accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
	    //accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
	    //accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
	    //gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ; // we need z-axis gyro only
	    accel_offset[0] += (int32_t) ((((int16_t)data[0]) << 8) | data[1] );
	    accel_offset[1] += (int32_t) ((((int16_t)data[2]) << 8) | data[3] ) ;
	    accel_offset[2] += (int32_t) ((((int16_t)data[4]) << 8) | data[5] ) ;
	    gyro_offset[0]  += (int32_t) ((((int16_t)data[6]) << 8) | data[7] ) ; // we need z-axis gyro only

	    //accel_offset[0] += accel_temp[0];
	}
	accel_offset[0] /= fifoCount;
	accel_offset[1] /= fifoCount;
	accel_offset[2] /= fifoCount;
	gyro_offset[0] /= fifoCount;

	// remove gravity from z-axis
	if(accel_offset[2]  > 0L)
		accel_offset[2] -= MPU6050_ACCEL_SENSITIVITY;
	else
		accel_offset[2] +=  MPU6050_ACCEL_SENSITIVITY;
	// offset regs are 16 bits
	//i2cTxBuffer[0] = ((int16_t)(accel_offset[0]) >> 8) & 0x00FF); // H
	//i2cTxBuffer[0] = (int16_t)(accel_offset[0]); // H

	// configure offset registers

}

float* mpu_raw_data(float* data){
	uint8_t i2cTxBuffer[2];
	uint8_t temp[12];
	i2cTxBuffer[0] = (uint8_t)MPU6050_REG_ACCEL_XOUT_H;
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2cTxBuffer, 1, 100);
	HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, temp, 12, 100);

    data[0] = (float)((((int16_t)temp[0]) << 8) | temp[1])/MPU6050_ACCEL_SENSITIVITY;
    data[1] = (float)((((int16_t)temp[2]) << 8) | temp[3])/MPU6050_ACCEL_SENSITIVITY;
    data[2] = (float)((((int16_t)temp[4]) << 8) | temp[5])/MPU6050_ACCEL_SENSITIVITY;
    data[3] = (float)((((int16_t)temp[6]) << 8) | temp[7])/MPU6050_GYRO_SENSITIVITY;
    data[4] = (float)((((int16_t)temp[8]) << 8) | temp[9])/MPU6050_GYRO_SENSITIVITY;
    data[5] = (float)((((int16_t)temp[10]) << 8) | temp[11])/MPU6050_GYRO_SENSITIVITY;

    return data;
}

//float mpu_integrate(float dt){
//	a1 = ((((float)(prev_z-cal_z)+(raw_z-cal_z))*0.5f)*dt*0.0078125f) + a1;
//	return a1;
//}

// interrupt from gyro
void mpu_get_data(){
	uint8_t i2cTxBuffer = (uint8_t)MPU6050_REG_ACCEL_XOUT_H;
	HAL_I2C_Master_Transmit_IT(&hi2c1, MPU6050_ADDRESS, &i2cTxBuffer, 1);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
	HAL_I2C_Master_Receive_DMA(&hi2c1, MPU6050_ADDRESS, raw_data, 12);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle){

	a_data.x = (float)((((int16_t)raw_data[0]) << 8) | raw_data[1])/MPU6050_ACCEL_SENSITIVITY;
	a_data.y = (float)((((int16_t)raw_data[2]) << 8) | raw_data[3])/MPU6050_ACCEL_SENSITIVITY;
	a_data.z = (float)((((int16_t)raw_data[4]) << 8) | raw_data[5])/MPU6050_ACCEL_SENSITIVITY;
    g_data.x = (float)((((int16_t)raw_data[6]) << 8) | raw_data[7])/MPU6050_GYRO_SENSITIVITY;
    g_data.y = (float)((((int16_t)raw_data[8]) << 8) | raw_data[9])/MPU6050_GYRO_SENSITIVITY;
    g_data.z = (float)((((int16_t)raw_data[10]) << 8) | raw_data[11])/MPU6050_GYRO_SENSITIVITY;
	//set new data flag?
	//filter and integrate?
}





