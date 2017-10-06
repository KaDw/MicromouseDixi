/*
 * logger_test.c
 *
 *  Created on: 06.10.2017
 *      Author: Karol
 */

#include "Utils/logger.h"


void logger_test_loop()
{
	logger_init();


	while(1)
	{
		LOG_DEBUG("Debug");
		LOG_INFO("Info");
		LOG_WARNING("warning");
		LOG_ERR("Error");
		LOG_CRITICAL("Error");

		logger_process();
		HAL_Delay(1000);


		for(int i = 0; i < 100; ++i) {
			LOG_DEBUG("Try overload logger");
		}
		logger_process();
		HAL_Delay(1000);

	}
}
