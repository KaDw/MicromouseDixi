/*
 * motor_port.h
 *
 *  Created on: 12.10.2017
 *      Author: Karol
 */

#ifndef PORT_MOTOR_PORT_H_
#define PORT_MOTOR_PORT_H_
#include "Utils/common.h" // ABS
#include "Utils/logger.h"
#include "Utils/SLAM2D.h"
#include "common_port.h"

// PWM
extern TIM_HandleTypeDef	 		htim3;
#define MOTOR_GPIO 						GPIOC
#define MOTOR_HTIM  					htim3
#define MOTOR_CH_L						TIM_CHANNEL_3
#define MOTOR_CH_R						TIM_CHANNEL_4

void motor_port_init();
static inline encInt motor_port_getEncL() { return SLAM_port_getEncL(); }
static inline encInt motor_port_getEncR() { return SLAM_port_getEncR(); }
void motor_port_setPwmL(int PWM);
void motor_port_setPwmR(int PWM);

#define MOTOR_ASSERT(x) 				while(!(x))
#define MOTOR_LOG_WARNING(...)	LOG_WARNING(__VA_ARGS__)

// in Hz
#define MOTOR_DRIVER_FREQ			1000

// position controller
#define MOTOR_MAX_PWM					999

#define MOTOR_MAX_PWM_IN_USE	250


// ==============
// == settings ==
// ==============

// in deg/s
#define MAX_ANGULAR_VEL				200.7f

// [mm/s/s]
#define MOTOR_ACC_V						(2500.0f)

// ACC_W[rad/s/s]
#define MOTOR_ACC_W						5.0f // 174.532925 rad/s/s = 10000 deg/s/s


#endif /* PORT_MOTOR_PORT_H_ */
