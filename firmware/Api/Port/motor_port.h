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
#include "common_port.h"

// PWM
extern TIM_HandleTypeDef	 		htim3;
#define MOTOR_GPIO 						GPIOC
#define MOTOR_HTIM  					htim3
#define MOTOR_CH_L						TIM_CHANNEL_3
#define MOTOR_CH_R						TIM_CHANNEL_4

// Encoders
extern TIM_HandleTypeDef 			htim2;
extern TIM_HandleTypeDef	 		htim5;
#define MOTOR_HTIM_ENC_L 			htim5
#define MOTOR_HTIM_ENC_R 			htim2

// must be signed
typedef int16_t 							encInt;

void motor_port_init();
static inline encInt motor_port_getEncL() { return MOTOR_HTIM_ENC_L.Instance->CNT; }
static inline encInt motor_port_getEncR() { return MOTOR_HTIM_ENC_R.Instance->CNT; }
void motor_port_setPwmL(int PWM);
void motor_port_setPwmR(int PWM);

#define MOTOR_ASSERT(x) 				while(!(x))
#define MOTOR_LOG_WARNING(...)	LOG_WARNING(__VA_ARGS__)


// wheelbase in mm
#define WHEELBASE							66

// wheel diameter in mm
#define WHEEL_DIAMETER				37

//
#define TICKS_PER_REVOLUTION	3520

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
