/*
 * fusion.c
 *
 *  Created on: 09.10.2017
 *      Author: Karol
 */
#include "fusion.h"
#include "Port/mpu6050.h"
#include <string.h> // memcpy

mpu_data gyro_data;

void gyro_integrate(){

	static int16_t prev_imu_data[6];
	gyro_data.x = ((float)(prev_imu_data[3] + g_imu_data[3]) / MPU6050_GYRO_SENSITIVITY * SAMPLE_TIME) + gyro_data.x;
	gyro_data.y = ((float)(prev_imu_data[4] + g_imu_data[4]) / MPU6050_GYRO_SENSITIVITY * SAMPLE_TIME) + gyro_data.y;
	gyro_data.z = ((float)(prev_imu_data[5] + g_imu_data[5]) / MPU6050_GYRO_SENSITIVITY * SAMPLE_TIME) + gyro_data.z;

	memcpy(prev_imu_data, g_imu_data, sizeof(prev_imu_data)); // previous samples

//	accel_data.x = (float)((((int16_t)raw_data[0]) << 8) | raw_data[1]);
//	accel_data.y = (float)((((int16_t)raw_data[2]) << 8) | raw_data[3]);
//	accel_data.z = (float)((((int16_t)raw_data[4]) << 8) | raw_data[5]);
//	gyro_data.x = (float)((((int16_t)raw_data[6]) << 8) | raw_data[7]);
//	gyro_data.y = (float)((((int16_t)raw_data[8]) << 8) | raw_data[9]);
//	gyro_data.z = (float)((((int16_t)raw_data[10]) << 8) | raw_data[11]);
}

void sensor_process(){

}


