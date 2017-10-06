///
/// author: Karol Trzciński
/// date: 10-2017
///

#ifndef __UI_H__
#define __UI_H__

#include <Port/UI_port.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if UI_USE_LEDS
#define _UI_LEDS_COUNTDOWN	0x80
#define UI_LED_ST_OFF			OFF
#define UI_LED_ST_ON			ON
#define UI_LED_ST_BLINK_SLOW	2
#define UI_LED_ST_BLINK_FAST	3
#define UI_LED_ST_FLASH_SHORT	(8|_UI_LEDS_COUNTDOWN)
#define UI_LED_ST_FLASH_LONG	(9|_UI_LEDS_COUNTDOWN)

void UI_SetLed(int LED, int UI_LED_ST);
#endif

#if UI_USE_BUZZER
#define _UI_BUZZER_COUNTDOWN	0x80
#define UI_BUZZER_ST_OFF		OFF
#define UI_BUZZER_ST_ON			ON
#define UI_BUZZER_ST_PULSE_SLOW	2
#define UI_BUZZER_ST_PULSE_FAST	3
#define UI_BUZZER_ST_BEEP_SHORT	(8|_UI_BUZZER_COUNTDOWN)
#define UI_BUZZER_ST_BEEP_LONG	(9|_UI_BUZZER_COUNTDOWN)

void UI_SetBuzzer(int UI_BUZZER_ST);
#endif


void UI_Init();
void UI_Process();

#ifdef __cplusplus
extern "C" {
#endif
#endif
