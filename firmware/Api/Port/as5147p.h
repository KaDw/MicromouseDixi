#include "stm32f4xx.h"

#ifndef AS5147P_H_
#define AS5147P_H_

// volatile registers
#define AS5147P_NOP		 	0x0000
#define AS5147P_ERRFL 		0x0001
#define AS5147P_PROG 		0x0003
#define AS5147P_DIAAGC 		0x3FFC
#define AS5147P_MAG 		0x3FFD
#define AS5147P_ANGLEUNC 	0x3FFE
#define AS5147P_ANGLECOM 	0x3FFF

// non-volatile registers One-Time Programmable (OTP)
#define AS5147P_ZPOSM 		0x0016
#define AS5147P_ZPOSL 		0x0017
#define AS5147P_SETTINGS1 	0x0018
#define AS5147P_SETTINGS2	0x0019
#define AS5147P_RED			0x001A

//						register		read	parity
#define AS5147P_ANGLE AS5147P_ANGLECOM | 0x4000 | 0x8000


typedef enum{
	ENCR = 0,
	ENCL = 1
}ENC;

uint8_t enc_check(ENC device);
uint16_t spi_read_enc(ENC device);



#endif /* AS5147P_H_ */
