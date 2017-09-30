#include <stdio.h>
#include "UI.h"

#if UI_USE_LEDS
// fill leds port data
const UI_Led_t UI_Led[] = {
	{ .pin = 0,.port = 'L',.inv = OFF }, // LED_L
	{ .pin = 1,.port = 'R',.inv = ON }, // LED_R
	{ .pin = 2,.port = 'F',.inv = OFF } // LED_F
};

void UI_LedPinHigh(const UI_Led_t* pLed)
{
	printf("Led H %c:%d ", pLed->port, pLed->pin);
	if (pLed->inv == OFF)
	{
		printf("(ON)\n");
	}
	else
	{
		printf("(OFF)\n");
	}
}

void UI_LedPinLow(const UI_Led_t* pLed)
{
	printf("Led L %c:%d ", pLed->port, pLed->pin);
	if (pLed->inv != OFF)
	{
		printf("(ON)\n");
	}
	else
	{
		printf("(OFF)\n");
	}
}
#endif


#if UI_USE_BUZZER
const UI_Buzzer_t UI_Buzzer = { .pin = 1,.port = 'B',.inv = OFF };

void UI_BuzzerPinHigh(const UI_Buzzer_t* pBuzzer)
{
	printf("Buzzer H %c:%d ", pBuzzer->port, pBuzzer->pin);
	if (pBuzzer->inv == OFF)
	{
		printf("(ON)\n");
	}
	else
	{
		printf("(OFF)\n");
	}
}
void UI_BuzzerPinLow(const UI_Buzzer_t* pBuzzer)
{
	printf("Buzzer L %c:%d ", pBuzzer->port, pBuzzer->pin);
	if (pBuzzer->inv != OFF)
	{
		printf("(ON)\n");
	}
	else
	{
		printf("(OFF)\n");
	}
}
#endif