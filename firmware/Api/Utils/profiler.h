/*
 * profiler.h
 *
 *  Created on: 10.11.2017
 *      Author: Karol
 */

#ifndef UTILS_PROFILER_H_
#define UTILS_PROFILER_H_
#include "SLAM2D.h"

void profiler_init();
void profiler_get();

typedef struct
{
		SLAM2D_pos_t target;
		float k[] = {1.0, 0.1, 0.2};
} profiler_t;


#endif /* UTILS_PROFILER_H_ */
