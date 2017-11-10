///
/// author: Karol Trzcinski
/// date: 10-2017
///

#include "Utils/UI.h"

#if UI_USE_LEDS
// fill leds port data
const UI_Led_t UI_Led[] = {
	{ .pin = LED2_Pin,.port = LED3_GPIO_Port,.inv = ON }, // LED_L
	{ .pin = LED3_Pin,.port = LED3_GPIO_Port,.inv = ON }, // LED_R
	{ .pin = LED4_Pin,.port = LED4_GPIO_Port,.inv = ON } // LED_F
};


void UI_LedPinOn(const UI_Led_t* pLed)
{
	HAL_GPIO_WritePin(pLed->port, pLed->pin, pLed->inv==OFF ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void UI_LedPinOff(const UI_Led_t* pLed)
{
	HAL_GPIO_WritePin(pLed->port, pLed->pin, pLed->inv==OFF ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
#endif



#if UI_USE_BUZZER
const UI_Buzzer_t UI_Buzzer = { .pin = BUZZER_Pin,.port = BUZZER_GPIO_Port,.inv = OFF };

void UI_BuzzerPinOn(const UI_Buzzer_t* pBuzzer)
{
	HAL_GPIO_WritePin(pBuzzer->port, pBuzzer->pin, pBuzzer->inv==OFF ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
void UI_BuzzerPinOff(const UI_Buzzer_t* pBuzzer)
{
	HAL_GPIO_WritePin(pBuzzer->port, pBuzzer->pin, pBuzzer->inv==OFF ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
#endif

#if UI_USE_BUTTONS
const UI_Btn_t UI_Btn[] = {
	{ .pin = BTN1_Pin,.port = BTN1_GPIO_Port,.inv = OFF}, // BTN 1
	{ .pin = BTN2_Pin,.port = BTN2_GPIO_Port,.inv = OFF} // BTN 2
};

GPIO_PinState UI_BtnReadGPIO(const UI_Btn_t* pBtn)
{
	return HAL_GPIO_ReadPin(pBtn->port, pBtn->pin);
}


#endif
