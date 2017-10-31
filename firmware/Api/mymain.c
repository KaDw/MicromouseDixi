/*
 * mymain.c
 *
 *  Created on: 06.10.2017
 *      Author: Karol
 */

#include "Utils/logger.h"
#include "Utils/UI.h"
#include "Port/mpu6050.h"

void testModules();

void initModules()
{
	logger_init();
	UI_Init();
	mpu_init();

	testModules();
}

void testModules()
{
	int res = mpu_self_test();
	//while(res != 0) ;

	UI_SetLed(LED_L, UI_LED_ST_BLINK_FAST);
}

void everyCycleFun()
{
	// ret = Fusion_Process();
	// Motor Control
	// start new measurements
	UI_Process();

	// switch (ret)
	// 	case .. : mapping()
	//  case .. : localNavigation()
}
