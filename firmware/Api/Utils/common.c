/*
 * common.c
 *
 *  Created on: 12.10.2017
 *      Author: Karol
 */

#include "common.h"


int contrain(int x, int lo, int hi){
	if(x < lo){
		return lo;
	} else if(x > hi) {
		return hi;
	} else {
		return x;
	}
}
