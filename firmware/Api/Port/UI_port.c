///
/// author: Karol Trzciński
/// date: 10-2017
///

#include "Utils/UI.h"

#if UI_USE_LEDS
// fill leds port data
const UI_Led_t UI_Led[] = {
	{ .pin = LED2_Pin,.port = LED3_GPIO_Port,.inv = OFF }, // LED_L
	{ .pin = LED3_Pin,.port = LED3_GPIO_Port,.inv = ON }, // LED_R
	{ .pin = LED4_Pin,.port = LED4_GPIO_Port,.inv = OFF } // LED_F
};

void UI_LedPinHigh(const UI_Led_t* pLed)
{
	if (pLed->inv == OFF)
	{
		HAL_GPIO_WritePin(pLed->port, pLed->pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(pLed->port, pLed->pin, GPIO_PIN_RESET);
	}
}

void UI_LedPinLow(const UI_Led_t* pLed)
{
	//printf("Led L %c:%d ", pLed->port, pLed->pin);
	if (pLed->inv != OFF)
	{
		//printf("(ON)\n");
	}
	else
	{
		//printf("(OFF)\n");
	}
}
#endif


#if UI_USE_BUZZER
const UI_Buzzer_t UI_Buzzer = { .pin = 1,.port = 'B',.inv = OFF };

void UI_BuzzerPinHigh(const UI_Buzzer_t* pBuzzer)
{
	if (pBuzzer->inv == OFF)
	{
		//printf("(ON)\n");
	}
	else
	{
		//printf("(OFF)\n");
	}
}
void UI_BuzzerPinLow(const UI_Buzzer_t* pBuzzer)
{
	if (pBuzzer->inv != OFF)
	{
		//printf("(ON)\n");
	}
	else
	{
		//printf("(OFF)\n");
	}
}
#endif
