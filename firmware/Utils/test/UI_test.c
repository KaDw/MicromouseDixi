#include <stdio.h>
#include "UI.h"

void main()
{
	printf("=========\n");
	printf("=UI_test=\n");
	printf("=========\n\n");

	printf("\tInit -> everything should be OFF\n");
	UI_Init();

	printf("\n\tTurn On buzzer and LED_F\n");
	UI_SetBuzzer(UI_BUZZER_ST_ON);
	UI_SetLed(LED_F, UI_LED_ST_ON);
	UI_Process();
	UI_Process();
	UI_Process();

	printf("\n\t Short Flash LED_L and slow pulse buzzer\n");
	UI_SetLed(LED_L, UI_LED_ST_FLASH_SHORT);
	UI_SetBuzzer(UI_BUZZER_ST_PULSE_SLOW);
	printf("\t time start\n");
	int min = UI_BUZZER_PULSE_SLOW_PERIOD > UI_LED_FLASH_SHORT_PERIOD ? UI_BUZZER_PULSE_SLOW_PERIOD : UI_LED_FLASH_SHORT_PERIOD;
	int max = UI_BUZZER_PULSE_SLOW_PERIOD < UI_LED_FLASH_SHORT_PERIOD ? UI_BUZZER_PULSE_SLOW_PERIOD : UI_LED_FLASH_SHORT_PERIOD;
	for (int i = 0; i < min; ++i) {
		UI_Process();
	}
	UI_Process();
	printf("\tpierwsza zmiana stanu\n");	
	for (int i = min; i < 2*max; ++i) {
		UI_Process();
	}
	printf("\tdruga zmiana stanu\n");
	UI_Process();

	printf("\n\n");
}