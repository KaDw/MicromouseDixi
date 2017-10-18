#ifndef PORT_SENSOR_H_
#define PORT_SENSOR_H_

/* safe min max functions */

#define MAX(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define SENS_COUNT 6

/*
 * lf - 3
 * ls - 2
 * l - 13
 * rf - 10
 * rs - 11
 * r - 12
 * batt - 9
 * */

/*
CH2 sens[0] - LF - Left
CH10 sens[1] - RF - Right
CH13 sens[2] - L - Left Front
CH12 sens[3] - R - Right Front
CH3 sens[4] - LS - Left Side
CH11 sens[5] - RS - Right Side

      L	  LF   RF	R
	  \   |	   |   /
	   \  |    |  /
		\ |    | /
	LS ---		--- RS

*/
extern volatile uint32_t read[6];

typedef enum {
	ADC_LS = 2,
	ADC_LF,
	ADC_BATT = 9,
	ADC_RF,
	ADC_RS,
	ADC_R,
	ADC_L
} ADCx;

void sensor_callback();
void sensor_read_ambient();
void sensor_read_channel(uint8_t CHx, volatile uint32_t *buf);
void sensor_read_channel2(uint8_t CHx1, uint8_t CHx2, volatile uint32_t *buf);

#endif /* PORT_SENSOR_H_ */
