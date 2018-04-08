/*
 * profiler.c
 *
 *  Created on: 10.11.2017
 *      Author: Karol
 */
#include "math.h"
#include "profiler.h"

profiler_t profiler;

void progiler_init()
{

}

void set_vel(SLAM2D_pos_t pos) {
	float dx = profiler.target.pos[SLAM_X] - pos.pos[SLAM_X];
	float dy = profiler.target.pos[SLAM_Y] - pos.pos[SLAM_Y];
	float dist_to_tar = sqrtf(dx*dx + dy*dy);
	float ang_to_tar_point = atan2(dy, dx) - pos.angle[SLAM_A_G];
	float ang_to_tar_angle = profiler.target.angle[SLAM_A_G] - pos.angle[SLAM_A_G];

	float s, c;
	sincosf(dist_to_tar, &s, &c);
	float cossinc = ang_to_tar_point*ang_to_tar_point < 0.0001 ? 1.0 : s*c/ang_to_tar_point;

	float v = profiler.k[0] * dist_to_tar;
	float w = profiler.k[1] * ang_to_tar_point + profiler.k[0] * cossinc*(ang_to_tar_point + profiler.k[2] * ang_to_tar_angle);


}
