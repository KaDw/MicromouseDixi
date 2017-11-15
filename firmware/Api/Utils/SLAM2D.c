/*
 * SALM.c
 *
 *  Created on: 27.10.2017
 *      Author: Karol
 */

#include <Utils/SLAM2D.h>
#include "math.h"

SLAM2D_t SLAM2D;


void SLAM2D_init()
{
	memset(&SLAM2D, 0, sizeof(SLAM2D));
}


static void _SLAM2D_ProcessEnc(SLAM_enc_t* enc, encInt val)
{
	// te zmienne powinny byz liczbami calkowitymi ze znakiem
	// oraz miec taka sama wielkosc jak rejestr przechowujacy
	// impulsy z eknkodera - patrz przejscia CNT przez 0!
	encInt delta = val - enc->lastEnc;
	enc->encChange = delta;
	enc->enc += delta;
	enc->lastEnc = val;
}


// return current
SLAM2D_pos_t SLAM2D_Odometry()
{
	SLAM2D_pos_t delta;
	// update from encoders
	_SLAM2D_ProcessEnc(&SLAM2D.enc[0], SLAM_port_getEncL());
	_SLAM2D_ProcessEnc(&SLAM2D.enc[1], SLAM_port_getEncR());

	float dSl = SLAM2D.enc[0].encChange * PI * WHEEL_DIAMETER / TICKS_PER_REVOLUTION;
	float dSr = SLAM2D.enc[1].encChange * PI * WHEEL_DIAMETER / TICKS_PER_REVOLUTION;
	float dS = (dSl + dSr) * 0.5f;

	float dGamma = (dSl - dSr) / (2*HALF_WHEELBASE);
	delta.pos[SLAM_X] = dS*sin(SLAM2D.state.angle[SLAM_A_G]);
	delta.pos[SLAM_Y] = dS*cos(SLAM2D.state.angle[SLAM_A_G]);
	delta.angle[SLAM_A_G] = (dSr - dSl) / (2*HALF_WHEELBASE);
	return delta;
}


// return current dAlpha
SLAM2D_pos_t SLAM2D_IMU()
{
	SLAM2D_pos_t delta;
	return delta;
}


// return current dAlpha
SLAM2D_pos_t SLAM2D_LED()
{
	SLAM2D_pos_t delta;
	return delta;
}


void SLAM2D_Process()
{
	SLAM2D_pos_t odometry = SLAM2D_Odometry();
	SLAM2D_pos_t imu = SLAM2D_IMU();
	SLAM2D_pos_t led = SLAM2D_LED();
	SLAM2D_pos_t meas;

	// calc angle
	float dangle_odo = odometry.angle[SLAM_A_G] * (1.0f - SLAM2D.imu_importance);
	float dangle_imu = imu.angle[SLAM_A_G] * SLAM2D.imu_importance;
	meas.angle[SLAM_A_G] += dangle_imu + dangle_odo;

	// calc position
	float dpos_odo[] = {odometry.pos[SLAM_X]*(1.0f-SLAM2D.led_importance),
											odometry.pos[SLAM_Y]*(1.0f-SLAM2D.led_importance)};
	float dpos_led[] = {odometry.pos[SLAM_X]*(SLAM2D.led_importance),
											odometry.pos[SLAM_Y]*(SLAM2D.led_importance)};
	meas.pos[0] += dpos_odo[0] + dpos_led[0];
	meas.pos[1] += dpos_odo[1] + dpos_led[1];

	// update current position
	SLAM2D.position.angle[SLAM_A_G] += meas.angle[SLAM_A_G];
	SLAM2D.position.pos[SLAM_X] 		+= meas.angle[SLAM_X];
	SLAM2D.position.pos[SLAM_Y] 		+= meas.angle[SLAM_Y];
}


