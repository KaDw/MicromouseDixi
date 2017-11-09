/*
 * SLAM_port.h
 *
 *  Created on: 27.10.2017
 *      Author: Karol
 */

#ifndef PORT_SLAM2D_PORT_H_
#define PORT_SLAM2D_PORT_H_

#include "common_port.h"


// Encoders
extern TIM_HandleTypeDef 			htim2;
extern TIM_HandleTypeDef	 		htim5;
#define MOTOR_HTIM_ENC_L 			htim5
#define MOTOR_HTIM_ENC_R 			htim2


// wheelbase in mm
#define WHEELBASE							66

// wheel diameter in mm
#define WHEEL_DIAMETER				37

//
#define TICKS_PER_REVOLUTION	3520


// must be signed
typedef int16_t 							encInt;

static inline encInt SLAM_port_getEncL() { return MOTOR_HTIM_ENC_L.Instance->CNT; }
static inline encInt SLAM_port_getEncR() { return MOTOR_HTIM_ENC_R.Instance->CNT; }


// rad per second
float SLAM_port_GetGyroW();


#endif /* PORT_SLAM2D_PORT_H_ */
