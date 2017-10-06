/*
 * mymain.c
 *
 *  Created on: 06.10.2017
 *      Author: Karol
 */

#include "Utils/UI.h"

void initModules()
{
	logger_init();
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
