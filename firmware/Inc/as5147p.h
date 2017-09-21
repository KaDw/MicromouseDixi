/*
 * as5147p.h
 *
 *  Created on: 20.09.2017
 *      Author: Karol
 */
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

//			register		read	parity
#define AS5147P_ANGLE | 0x4000 | 0x8000


extern uint16_t SpiTxBuffer[2];
extern uint16_t SpiRxBuffer[2];

typedef enum{
	ENCR = 0,
	ENCL = 1
}ENC;

uint16_t spi_read(uint16_t addr, uint8_t size, ENC device);

#endif /* AS5147P_H_ */
