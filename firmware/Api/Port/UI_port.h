///
/// author: Karol Trzciński
/// date: 10-2017
///

#ifndef __UI_PORT_H__
#define __UI_PORT_H__
#include <stdint.h>
#include "common_port.h"


#define UI_USE_BUZZER 1
#define UI_USE_LEDS	  1

#define UI_TIME_SCALE(x) ((int)(0.01f*x))

// ======
//  LEDS
// ======
#if UI_USE_LEDS

#define UI_LED_BLINK_SLOW_PERIOD	UI_TIME_SCALE(500)
#define UI_LED_BLINK_FAST_PERIOD	UI_TIME_SCALE(100)
#define UI_LED_FLASH_SHORT_PERIOD	UI_TIME_SCALE(400)
#define UI_LED_FLASH_LONG_PERIOD	UI_TIME_SCALE(800)

typedef struct
{
	uint16_t pin;
	GPIO_TypeDef* port;
	char inv; // inverted pin logic (Low-On, High-Off)
} UI_Led_t;

extern const UI_Led_t UI_Led[]; // in UI_port.c

// define LED's mnemonics

#define LED_L 0
#define LED_R 1
#define LED_F 2
#define LED_COUNT 3

// HAL function - platform dependent
void UI_LedPinHigh(const UI_Led_t*); // in UI_port.c
void UI_LedPinLow(const UI_Led_t*); // in UI_port.c

#endif // UI_USE_LEDS


// ======
// BUZZER
// ======
#if UI_USE_BUZZER

#define UI_BUZZER_BEEP_SHORT_PERIOD		UI_TIME_SCALE(100)
#define UI_BUZZER_BEEP_LONG_PERIOD		UI_TIME_SCALE(500)
#define UI_BUZZER_PULSE_FAST_PERIOD		UI_TIME_SCALE(100)
#define UI_BUZZER_PULSE_SLOW_PERIOD		UI_TIME_SCALE(500)

typedef struct
{
	uint32_t pin;
	char port;
	char inv; // inverted pin logic (Low-On, High-Off)
} UI_Buzzer_t;

extern const UI_Buzzer_t UI_Buzzer; // in UI_port.c

// HAL function - platform dependent
void UI_BuzzerPinHigh(const UI_Buzzer_t*); // in UI_port.c
void UI_BuzzerPinLow(const UI_Buzzer_t*); // in UI_port.c

#endif

#endif
