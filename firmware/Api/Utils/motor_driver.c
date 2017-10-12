#include "Utils/motor.h"

extern motors_t motors;

// control each motor independent
void motor_driverPID_VV();

// control motor by translation and rotation velocity
void motor_driverPID_VW();



// handler of motor driver in use
void (*motor_driver)() = motor_driverPID_VW;




void motor_driverUpdateErrorPositional()
{
	int err;
	for(int i = 0; i < 2; ++i)
	{
		// oblicz uchyba jako roznice pomiedzy pozycja idelana i ta rzeczywista
		err = motors.mot[i].idealEnc - motors.mot[i].enc;
		motors.mot[i].errD = err - motors.mot[i].errP;
		motors.mot[i].errI+= err;
		motors.mot[i].errP = err;
	}
}


void motor_driverUpdateErrorVelocity()
{
	int errPosition, errVelocity;
	for(int i = 0; i < 2; ++i)
	{
		// oblicz uchyba jako roznice pomiedzy pozycja idelana i ta rzeczywista
		errPosition = motors.mot[i].idealEnc - motors.mot[i].enc; // positional
		errVelocity = errPosition - motors.mot[i].errI;
		motors.mot[i].errD = errVelocity - motors.mot[i].errP;
		motors.mot[i].errP = errVelocity;
		motors.mot[i].errI = errPosition;
	}
}


void motor_driverAddExtraFeedback()
{
	int feedback = 0;

	if(motors.flags & MOTOR_FLAG_SENSOR_EN) {
		feedback += 0; //todo: add sensors feedback
	}

	if(motors.flags & MOTOR_FLAG_GYRO_EN) {
		feedback += 0; //todo: add gyro feedback
	}

	motors.mot[0].idealEnc += feedback;
	motors.mot[1].idealEnc -= feedback;
}


void motor_driverPID_VV()
{
	const float KP = 5.f;
	const float KI = (0.f / MOTOR_DRIVER_FREQ);
	const float KD = (0.8f * MOTOR_DRIVER_FREQ);
	const int errIMax = 1200;

	motor_driverUpdateErrorPositional();

	// dla obu silnikow
	for(int i = 0; i < 2; ++i)
	{
		// nasycenie czlonu I
		motors.mot[i].errI = contrain(motors.mot[i].errI, -errIMax, errIMax);

		// sprzezenie od bledu pozycji
		motors.mot[i].PWM = (int)(KP*((float)(motors.mot[i].errP) // 500
														+ KI*(float)(motors.mot[i].errI) // 1000
														+ KD*(float)(motors.mot[i].errD))); // 56
		// sprzezenie od zadanej predkosci
		motors.mot[i].PWM += (int)(0.17f*(motors.mot[i].vel
														+ 0.3f*motors.mot[i].a)); // 0.03
	}
}


void motor_driverPID_VW()
{
	const float KPV = 5.f;
	const float KDV = (0.008f * MOTOR_DRIVER_FREQ);
	const float KPW = 1.f;
	const float KDW = (0.05f * MOTOR_DRIVER_FREQ);

	// nasycenie czlonu I
	const int errIMax = 1200;

	motor_driverUpdateErrorPositional(motors);

	// nasycenie czlonu I
	motors.mot[0].errI = contrain(motors.mot[0].errI, -errIMax, errIMax);
	motors.mot[1].errI = contrain(motors.mot[1].errI, -errIMax, errIMax);

	int errVP = motors.mot[0].errP + motors.mot[1].errP;
	int errVD = motors.mot[0].errD + motors.mot[1].errD;
	int errWP = motors.mot[0].errP - motors.mot[1].errP;
	int errWD = motors.mot[0].errD - motors.mot[1].errD;

	int errV = KPV*errVP + KDV*errVD;
	int errW = KPW*errWP + KDW*errWD;

	motors.mot[0].PWM = errV + errW;
	motors.mot[1].PWM = errV - errW;
}
