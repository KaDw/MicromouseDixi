/*
 * fusion.h
 *
 *  Created on: 09.10.2017
 *      Author: Karol
 */

#ifndef UTILS_FUSION_H_
#define UTILS_FUSION_H_

#define IMU_DATA_RDY 1
#define ENC_DATA_RDY 2

//#if !defined MPU6050_GYRO_SENSITIVITY || !defined MPU6050_ACCEL_SENSITIVITY
//#define MPU6050_GYRO_SENSITIVITY 131.0f
//#define MPU6050_ACCEL_SENSITIVITY 16384.0f
//#endif

/* in sceonds */
#define SAMPLE_TIME 0.001f


typedef struct
{
	float x;
	float y;
	float z;
}mpu_data;




#endif /* UTILS_FUSION_H_ */
