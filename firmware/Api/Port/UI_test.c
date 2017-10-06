/*
 * UI_test.c
 *
 *  Created on: 06.10.2017
 *      Author: Karol
 */


#include "Utils/UI.h"

void UI_test_loop()
{
	UI_Init();

  int t = 0;
  UI_SetLed(LED_F, UI_LED_ST_BLINK_FAST);

  while (1)
  {
  	++t;

  	switch(t){
  		case 100:
  			UI_SetBuzzer(UI_BUZZER_ST_BEEP_LONG);
  			UI_SetLed(LED_L, UI_LED_ST_ON);
  			UI_SetLed(LED_R, UI_LED_ST_BLINK_FAST);
  			break;
  		case 500:
  			UI_SetBuzzer(UI_BUZZER_ST_BEEP_SHORT);
  			UI_SetLed(LED_L, UI_LED_ST_OFF);
  			UI_SetLed(LED_R, UI_LED_ST_BLINK_SLOW);
  			break;
  		case 1000:
  			UI_SetLed(LED_R, UI_LED_ST_OFF);
  			break;
  		case 1100:
  			t = 0;
  			break;
  	}

  	UI_Process();
  	HAL_Delay(1);
  }
}
