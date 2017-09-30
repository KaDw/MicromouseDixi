#include "UI.h"

#if UI_USE_LEDS

typedef struct
{
	char state;
	char isOn;
	int counter;
} _UI_LedStat_t;

static _UI_LedStat_t _LedsStat[LED_COUNT];

/// set led ON or OFF and update _LedState[i].isOn state
static void _UI_LedPinSet(int i, char on)
{
	if (UI_Led[i].inv != on) {
		UI_LedPinHigh(&UI_Led[i]);
	}
	else {
		UI_LedPinLow(&UI_Led[i]);
	}
	_LedsStat[i].isOn = on;
}

/// set led on or off correcponding to led counter and period
static void _UI_LedBlinkingProcess(int i, int period)
{
	int temp = 2 * (_LedsStat[i].counter % period);

	// gdy jest to 'gorna polowa' to led powinien byc wy³¹czony
	if (temp > period && _LedsStat[i].isOn == ON)
	{
		_UI_LedPinSet(i, OFF);
	}
	else if (temp < period && _LedsStat[i].isOn == OFF)
	{
		_UI_LedPinSet(i, ON);
	}
}

/// check if any led need to change state
static void _UI_ProcessLED()
{
	for (int i = 0; i < LED_COUNT; ++i)
	{
		switch (_LedsStat[i].state)
		{
		case UI_LED_ST_BLINK_SLOW:
			_UI_LedBlinkingProcess(i, UI_LED_BLINK_SLOW_PERIOD);
			break;
		case UI_LED_ST_BLINK_FAST:
			_UI_LedBlinkingProcess(i, UI_LED_BLINK_FAST_PERIOD);
			break;
		default:
			break;
		}

		// LED'y w stanie FLASH zliczaja w dó³
		// pozosta³e w górê
		if ((_LedsStat[i].state & _UI_LEDS_COUNTDOWN) == 0) {
			++_LedsStat[i].counter;
		}
		else
		{
			if (_LedsStat[i].counter > 0) {
				--_LedsStat[i].counter;
			}
			else {
				UI_SetLed(i, UI_LED_ST_OFF);
			}
		}
	}
}

/// change led state by user
void UI_SetLed(int LED, int UI_LED_ST)
{
	_LedsStat[LED].state = UI_LED_ST;
	_LedsStat[LED].counter = 0;

	switch (UI_LED_ST)
	{
	case UI_LED_ST_ON:
		_UI_LedPinSet(LED, ON);
		break;
	case UI_LED_ST_OFF:
		_UI_LedPinSet(LED, OFF);
		break;
	case UI_LED_ST_FLASH_SHORT:
		_LedsStat[LED].counter = UI_LED_FLASH_SHORT_PERIOD;
		_UI_LedPinSet(LED, ON);
		break;
	case UI_LED_ST_FLASH_LONG:
		_LedsStat[LED].counter = UI_LED_FLASH_LONG_PERIOD;
		_UI_LedPinSet(LED, ON);
		break;
	default:
		// blinking states only need to have correct state
		break;
	}
}
#endif // UI_USE_LEDS

#if UI_USE_BUZZER
typedef struct
{
	char state;
	char isOn;
	int counter;
} _UI_BuzzerStat_t;

static _UI_BuzzerStat_t _BuzzerStat;

/// set led ON or OFF and update _LedState[i].isOn state
static void _UI_BuzzerPinSet(char on)
{
	if (UI_Buzzer.inv != on) {
		UI_BuzzerPinHigh(&UI_Buzzer);
	}
	else {
		UI_BuzzerPinLow(&UI_Buzzer);
	}
	_BuzzerStat.isOn = on;
}

/// set buzzer on or off correcponding to buzzer stat counter and period
static void _UI_BuzzerBlinkingProcess(int period)
{
	int temp = 2 * (_BuzzerStat.counter % period);

	// gdy jest to 'gorna polowa' to buzzer powinien byc wy³¹czony
	if (temp > period && _BuzzerStat.isOn == ON)
	{
		_UI_BuzzerPinSet(OFF);
	}
	else if (temp < period && _BuzzerStat.isOn == OFF)
	{
		_UI_BuzzerPinSet(ON);
	}
}

/// check if buzzer need to change state
static void _UI_ProcessBuzzer()
{
	switch (_BuzzerStat.state)
	{
	case UI_LED_ST_BLINK_SLOW:
		_UI_BuzzerBlinkingProcess(UI_BUZZER_PULSE_SLOW_PERIOD);
		break;
	case UI_LED_ST_BLINK_FAST:
		_UI_BuzzerBlinkingProcess(UI_BUZZER_PULSE_FAST_PERIOD);
		break;
	default:
		break;
	}

	// sprawdz czy zliczaæ w dó³ czy w górê
	if ((_BuzzerStat.state & _UI_BUZZER_COUNTDOWN) == 0) {
		++_BuzzerStat.counter;
	}
	else {
		if (_BuzzerStat.counter > 0) {
			--_BuzzerStat.counter;
		}
		else {
			UI_SetBuzzer(UI_BUZZER_ST_OFF);
		}
	}
}

/// change buzzer state by user
void UI_SetBuzzer(int UI_BUZZER_ST)
{
	_BuzzerStat.state = UI_BUZZER_ST;
	_BuzzerStat.counter = 0;

	switch (UI_BUZZER_ST)
	{
	case UI_BUZZER_ST_ON:
		_UI_BuzzerPinSet(ON);
		break;
	case UI_BUZZER_ST_OFF:
		_UI_BuzzerPinSet(OFF);
		break;
	case UI_BUZZER_ST_BEEP_SHORT:
		_BuzzerStat.counter = UI_BUZZER_BEEP_SHORT_PERIOD;
		_UI_BuzzerPinSet(ON);
		break;
	case UI_BUZZER_ST_BEEP_LONG:
		_BuzzerStat.counter = UI_BUZZER_BEEP_LONG_PERIOD;
		_UI_BuzzerPinSet(ON);
		break;
	default:
		// blinking states only need to have correct state
		break;
	}
}

#endif


void UI_Init()
{
#if UI_USE_LEDS
	// wy³¹cz wszytskie LED'y i buzzer
	for (int i = 0; i < LED_COUNT; ++i)
	{
		_UI_LedPinSet(i, OFF);
	}
#endif

#if UI_USE_BUZZER
	_UI_BuzzerPinSet(OFF);
#endif
}

/// each function call is one UI period
void UI_Process()
{
#if UI_USE_LEDS
	_UI_ProcessLED();
#endif

#if UI_USE_BUZZER
	_UI_ProcessBuzzer();
#endif

}


