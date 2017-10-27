/*
 * motor_port.h
 *
 *  Created on: 13.10.2017
 *      Author: Karol
 */

#ifndef API_UTILS_TEST_PC_PORT_MOTOR_PORT_H_
#define API_UTILS_TEST_PC_PORT_MOTOR_PORT_H_


// must be signed
typedef int16_t 							encInt;

void motor_port_init();
static inline encInt motor_port_getEncL() { return 0; }
static inline encInt motor_port_getEncR() { return 0; }
void motor_port_setPwmL(int PWM);
void motor_port_setPwmR(int PWM);

#define MOTOR_ASSERT(x) 				while(!(x))
#define MOTOR_LOG_WARNING(...)	printf(__VA_ARGS__)


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
#define MOTOR_ACC_W						5.0f


#endif /* API_UTILS_TEST_PC_PORT_MOTOR_PORT_H_ */
