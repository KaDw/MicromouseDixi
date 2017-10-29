#include <math.h>
#include "mpu6050.h"
#include "i2c.h"
#include "utils/logger.h"


uint8_t g_raw_data[14];
int16_t g_imu_data[6];

uint8_t mpu_self_test(){
	uint8_t i2c_tx_buf[4];

	uint8_t raw_data[4];
	uint8_t self_test[6];
	float factory_trim[6];

//	if(HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)MPU6050_ADDRESS, 5, 100) != HAL_OK)
//		return 1;

	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_WHO_AM_I;
	if(HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 1, 100)!= HAL_OK){
		LOG_CRITICAL("failed to transmit");
		return 1;
	}

	if(HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, raw_data, 1, 100) != HAL_OK){
		LOG_CRITICAL("failed to receive");
		return 1;
	}

	if(raw_data[0] != 0x68){ // WHO_I_AM always 0x68
		LOG_CRITICAL("MPU6050 not responding");
		return 1;
	}


	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_GYRO_CONFIG;
	i2c_tx_buf[1] = 0xE0; // enable self test, range +/- 250deg/s
	i2c_tx_buf[2] = (uint8_t)MPU6050_REG_ACCEL_CONFIG;
	i2c_tx_buf[3] = 0xF0; // enable self test, range+/- 8 g

	if(HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 4, 100)!= HAL_OK){
		LOG_CRITICAL("failed to transmit");
		return 1;
	}

	HAL_Delay(250);

	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_SELF_TEST_X;
	if(HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 1, 100) != HAL_OK){
		LOG_CRITICAL("failed to transmit");
		return 1;
	}

	if(HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, raw_data, 4, 100) != HAL_OK){
		LOG_CRITICAL("failed to receive");
		return 1;
	}

	// acc test result (5 bit)
	self_test[0] = (raw_data[0] >> 3) | (raw_data[3] & 0x30) >> 4 ; // X
	self_test[1] = (raw_data[1] >> 3) | (raw_data[3] & 0x0C) >> 4 ; // Y
	self_test[2] = (raw_data[2] >> 3) | (raw_data[3] & 0x03) >> 4 ; // Z

	// gyro test result (5 bit)
	self_test[3] = raw_data[0]  & 0x1F ; // XG_TEST result is a five-bit unsigned integer
	self_test[4] = raw_data[1]  & 0x1F ; // YG_TEST result is a five-bit unsigned integer
	self_test[5] = raw_data[2]  & 0x1F ; // ZG_TEST result is a five-bit unsigned integer


	factory_trim[0] = (4096.0*0.34)*(powf( (0.92/0.34) , (((float)self_test[0] - 1.0)/30.0))); // FT[Xa] factory trim calculation
	factory_trim[1] = (4096.0*0.34)*(powf( (0.92/0.34) , (((float)self_test[1] - 1.0)/30.0))); // FT[Ya] factory trim calculation
	factory_trim[2] = (4096.0*0.34)*(powf( (0.92/0.34) , (((float)self_test[2] - 1.0)/30.0))); // FT[Za] factory trim calculation
	factory_trim[3] =  ( 25.0*131.0)*(powf( 1.046 , ((float)self_test[3] - 1.0) ));             // FT[Xg] factory trim calculation
	factory_trim[4] =  (-25.0*131.0)*(powf( 1.046 , ((float)self_test[4] - 1.0) ));             // FT[Yg] factory trim calculation
	factory_trim[5] =  ( 25.0*131.0)*(powf( 1.046 , ((float)self_test[5] - 1.0) ));             // FT[Zg] factory trim calculation

	for (int i = 0; i < 6; i++) {
		if((100.0 + 100.0*((float)self_test[i] - factory_trim[i])/factory_trim[i]) > 1.0f){
			LOG_CRITICAL("MPU6050 self_test failed");
			return 1;
		}
	}

	LOG_INFO("MPU6050 self_test passed");

	return 0;

}

void mpu_init(){
	uint8_t i2c_tx_buf[6];

	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_PWR_MGMT_1;
	i2c_tx_buf[1] = 0x03; // PLL with Z axis gyroscope reference


	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 2, 100);

	HAL_Delay(200);

	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_CONFIG; //TODO test LPF configrations
	i2c_tx_buf[1] = 0x00; // disable FSYNC, DLPF = 256Hz (default rate)
	i2c_tx_buf[2] = (uint8_t)MPU6050_REG_SMPLRT_DIV;
	i2c_tx_buf[3] = 0x07; // sample rate 1kHz (8 / (1+7))

	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 4, 100);

	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_GYRO_CONFIG;
	i2c_tx_buf[1] = (uint8_t)(MPU6050_SCALE_250DPS << 3); // MPU6050_SCALE_250DPS
	i2c_tx_buf[2] = (uint8_t)MPU6050_REG_ACCEL_CONFIG;
	i2c_tx_buf[3] = (uint8_t)(MPU6050_RANGE_2G << 3);

	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 4, 100);

}

void mpu_calibrate(){

	uint8_t i2c_tx_buf[4];
	uint8_t data[14];

	LOG_INFO("MPU6050 calibration started...");

	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_PWR_MGMT_1; // reset device
	i2c_tx_buf[1] = 0x80;
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 2, 100);

	HAL_Delay(200);

	mpu_init();

	HAL_Delay(200);

	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_USER_CTRL;
	i2c_tx_buf[1] = 0x0C; // reset fifo and dmp
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 2, 100);
	HAL_Delay(150);

//	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_USER_CTRL;
//	i2c_tx_buf[1] = 0x40; // enable fifo
//	i2c_tx_buf[2] = (uint8_t)MPU6050_REG_FIFO_EN;
//	i2c_tx_buf[3] = 0x78; // en z-axis for gyro and acc for fifo
//	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 2, 100);
	// accumulate samples in FIFO
	//HAL_Delay(80);

	// disable fifo
//	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_FIFO_EN;
//	i2c_tx_buf[1] = 0x00; // en z-axis for gyro and acc for fifo
//	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 2, 100);

//	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_FIFO_COUNTH;
//	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 1, 100);
	// get number of samples in fifo
//	HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, data, 2, 100);
//	uint16_t fifoCount = (uint16_t)((data[0] << 8) | data[1]);
//	fifoCount  = fifoCount / 8; // 2 bytes (gyro) + 6 bytes (acc)
	int8_t sample_count = 50;
	int32_t accel_offset[3] = {0, 0, 0};
	int32_t gyro_offset[3] = {0, 0, 0};

	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_ACCEL_XOUT_H;
	//for(uint16_t i = 0; i < fifoCount+1; i++){
	for(uint16_t i = 0; i < sample_count; i++){
		// read from fifo
		HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 1, 100);
		HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, data, 14, 100);

	    accel_offset[0] += (int32_t) ((((int16_t)data[0]) << 8) | data[1] );
	    accel_offset[1] += (int32_t) ((((int16_t)data[2]) << 8) | data[3] ) ;
	    accel_offset[2] += (int32_t) ((((int16_t)data[4]) << 8) | data[5] ) ;
	    gyro_offset[0]  += (int32_t) ((((int16_t)data[8]) << 8) | data[9] ) ;
	    gyro_offset[1]  += (int32_t) ((((int16_t)data[10]) << 8) | data[11] ) ;
	    gyro_offset[2]  += (int32_t) ((((int16_t)data[12]) << 8) | data[13] ) ;
	    HAL_Delay(1);
	    //accel_offset[0] += accel_temp[0];
	}
	accel_offset[0] /= sample_count;
	accel_offset[1] /= sample_count;
	accel_offset[2] /= sample_count;
	gyro_offset[0] /= sample_count;
	gyro_offset[1] /= sample_count;
	gyro_offset[2] /= sample_count;

	// remove gravity from z-axis
	if(accel_offset[2] > 0L)
		accel_offset[2] -= MPU6050_ACCEL_SENSITIVITY;
	else
		accel_offset[2] +=  MPU6050_ACCEL_SENSITIVITY;

	for(uint8_t i=0; i < 3; i++) {
		//accel_offset[i]= (-accel_offset[i]);
		gyro_offset[i] = (-gyro_offset[i]);
	}

	data[0] = (accel_offset[0] >> 8) & 0xff;
	data[1] = accel_offset[0] & 0xff;
	data[2] = (accel_offset[1] >> 8) & 0xff;
	data[3] = accel_offset[1] & 0xff;
	data[4] = (accel_offset[2] >> 8) & 0xff;
	data[5] = accel_offset[2] & 0xff;

	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_XA_OFFS_USRH;
	// transmit to accel offst registers
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 1, 100);
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, data, 6, 100);

	data[0] = (gyro_offset[0] >> 8) & 0xff;
	data[1] = gyro_offset[0] & 0xff;
	data[2] = (gyro_offset[1] >> 8) & 0xff;
	data[3] = gyro_offset[1] & 0xff;
	data[4] = (gyro_offset[2] >> 8) & 0xff;
	data[5] = gyro_offset[2] & 0xff;


	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_XG_OFFS_USRH;
	// transmit to gyro offst registers
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 1, 100);
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, data, 6, 100);



	LOG_INFO("MPU6050 calibration process finished");

}

int16_t* mpu_raw_data(int16_t* data){
	uint8_t i2c_tx_buf[2];
	uint8_t temp[14]; // skip temperature
	i2c_tx_buf[0] = (uint8_t)MPU6050_REG_ACCEL_XOUT_H;
	HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDRESS, i2c_tx_buf, 1, 100);
	HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDRESS, temp, 14, 100);

    data[0] = ((((int16_t)temp[0]) << 8) | temp[1]);
    data[1] = ((((int16_t)temp[2]) << 8) | temp[3]);
    data[2] = ((((int16_t)temp[4]) << 8) | temp[5]);
    data[3] = ((((int16_t)temp[8]) << 8) | temp[9]);
    data[4] = ((((int16_t)temp[10]) << 8) | temp[11]);
    data[5] = ((((int16_t)temp[12]) << 8) | temp[13]);

    return data;
}


void mpu_get_data(){
	static uint8_t g_i2c_tx_temp = (uint8_t)MPU6050_REG_ACCEL_XOUT_H;;
	//i2c_tx_buf1[0] = (uint8_t)MPU6050_REG_ACCEL_XOUT_H;
	HAL_I2C_Master_Transmit_IT(&hi2c1, MPU6050_ADDRESS, &g_i2c_tx_temp, 1);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
	HAL_I2C_Master_Receive_DMA(&hi2c1, MPU6050_ADDRESS, g_raw_data, 14);
}

// TODO check if HAL_I2C_MasterRxCpltCallback is called when all data is received
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle){
	g_imu_data[0] = (((int16_t)g_raw_data[0]) << 8) | g_raw_data[1];
	g_imu_data[1] = (((int16_t)g_raw_data[2]) << 8) | g_raw_data[3];
	g_imu_data[2] = (((int16_t)g_raw_data[4]) << 8) | g_raw_data[5];
	g_imu_data[3] = (((int16_t)g_raw_data[8]) << 8) | g_raw_data[9];
	g_imu_data[4] = (((int16_t)g_raw_data[10]) << 8) | g_raw_data[11];
	g_imu_data[5] = (((int16_t)g_raw_data[12]) << 8) | g_raw_data[13];
	// 0.75us
//	a_data.x = (float)((((int16_t)raw_data[0]) << 8) | raw_data[1])/MPU6050_ACCEL_SENSITIVITY;
//	a_data.y = (float)((((int16_t)raw_data[2]) << 8) | raw_data[3])/MPU6050_ACCEL_SENSITIVITY;
//	a_data.z = (float)((((int16_t)raw_data[4]) << 8) | raw_data[5])/MPU6050_ACCEL_SENSITIVITY;
//	g_data.x = (float)((((int16_t)raw_data[6]) << 8) | raw_data[7])/MPU6050_GYRO_SENSITIVITY;
//	g_data.y = (float)((((int16_t)raw_data[8]) << 8) | raw_data[9])/MPU6050_GYRO_SENSITIVITY;
//	g_data.z = (float)((((int16_t)raw_data[10]) << 8) | raw_data[11])/MPU6050_GYRO_SENSITIVITY;
}





