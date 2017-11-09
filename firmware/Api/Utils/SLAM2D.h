/*
 * SLAM.h
 *
 *  Created on: 27.10.2017
 *      Author: Karol
 */

#ifndef SLAM_H_
#define SLAM_H_

#include "Port/SLAM2D_port.h"
#include "common.h"

#define SLAM_X 0
#define SLAM_Y 1
#define SLAM_Z -1
#define SLAM_A_A -1
#define SLAM_A_B -1
#define SLAM_A_G 0

typedef struct
{
		float pos[2];
		float angle[1];
} SLAM2D_pos_t;


typedef struct
{
		encInt lastEnc, encChange;
		unsigned int enc, idealEnc;
} SLAM_enc_t;


typedef struct
{
		SLAM2D_pos_t state;
		SLAM_enc_t enc[2];
		SLAM2D_pos_t position;
		float imu_importance;
		float led_importance;
} SLAM2D_t;

void SLAM2D_init();

#endif /* SLAM_H_ */
