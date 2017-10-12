/*
 * motor.h
 *
 *  Created on: 12.10.2017
 *      Author: Karol Trzcinski
 */

#ifndef UTILS_MOTOR_H_
#define UTILS_MOTOR_H_

#include <math.h> // sqrt
#include <string.h> // memset
#include "Utils/common.h"
#include "Utils/logger.h"
#include "Port/motor_port.h"


typedef enum
{
	MOTOR_STOPPED,
	MOTOR_RUNNING_STOP, // motor is running now, stop wheels when end running
	MOTOR_RUNNING_FLOAT, // motor is running now, float wheels when end running
	MOTOR_FLOATING,
	MOTOR_CONST_VEL,
	MOTOR_ELSE
} motor_stat;

typedef struct
{
	int PWM; // [0..MOTOR_MAX_PWM]
	float vel, a;
	float targetVel; // mm/s
	encInt lastEnc, encChange;
	unsigned int enc, idealEnc;
	int errP, errI, errD;
} motor_t;


typedef struct
{
	motor_t mot[2];
	unsigned int time;
	motor_stat status;
	int flags;
} motors_t;


typedef struct
{
		void (*moveEnd)(motors_t* motors);
		void (*turnStart)(motors_t* motors, int* angle, int* r, float* vel); // called before calculation
} motors_cb_s;

extern motors_cb_s motors_cb;


#define MOTOR_FLAG_PID_EN						(1<<0)
#define MOTOR_FLAG_GYRO_EN					(1<<1)
#define MOTOR_FLAG_SENSOR_EN				(1<<2)
#define MOTOR_FLAG_ENCODER_EN				(1<<3)
#define MOTOR_FLAG_FREEZE						(1<<4)


void motor_init(void);
void motor_update(void);
void motor_stop(void);
void motor_setPWM(void);

void motor_flagSet(int flag);
void motor_flagClear(int flag);

void motor_go(int left, int right, float vel); // [mm] [mm] [mm/s]
void motor_goA(int left, int right, float vel); // [mm] [mm] [mm/s]
void motor_turn(int angle, int r, float vel); // [deg] [mm] [mm/s]
void motor_turnA(int angle, int r, float vel); // [deg] [mm] [mm/s]

#endif /* UTILS_MOTOR_H_ */
