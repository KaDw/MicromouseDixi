/*
 * as5147p.c
 *
 *  Created on: 20.09.2017
 *      Author: Karol
 */

#include "as5147p.h"
#include "gpio.h"
#include "spi.h"

uint16_t SpiTxBuffer[2];
uint16_t SpiRxBuffer[2];

// AS5147P is using even parity
// if the count of bits with a value of 1 is odd, the parity bit value is set to 1

// odd number of bits return 1
uint16_t spi_parity(uint16_t x){
	x ^= x >> 8;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return x & 1;
}

uint16_t spi_read(uint16_t addr, uint8_t size, ENC device){
	uint16_t a = 0xffff;
	/* Read mode mask */
	addr |= 0x4000;
	if(spi_parity(addr)) // parity bit (bit 15)
		addr |= 0x8000;

	if(device)
		HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port,SPI2_CS1_Pin , GPIO_PIN_RESET); // CS LOW
	else
		HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port,SPI2_CS2_Pin , GPIO_PIN_RESET); // CS LOW

	/* Setup time for CS signal 350ns */
	HAL_SPI_TransmitReceive_DMA(&hspi2, &addr, (uint8_t*)SpiRxBuffer, 1);
	while (!(SPI2->SR & SPI_FLAG_TXE)); // check if transmit buffer has been shifted out
	while ((SPI2->SR & SPI_FLAG_BSY));

	if(device)
		HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port,SPI2_CS1_Pin , GPIO_PIN_SET); // CS HIGH
	else
		HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port,SPI2_CS2_Pin , GPIO_PIN_SET); // CS HIGH
	// High time of CSn between two transmissions: 350ns

	for(int i = 0; i < 10; i++){ // 700ns delay
		addr |= 0x4000;
	}

	if(device)
		HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port,SPI2_CS1_Pin , GPIO_PIN_RESET); // CS LOW
	else
		HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port,SPI2_CS2_Pin , GPIO_PIN_RESET); // CS LOW

	// for multi-read mode
	for(int i = 0; i < size; i++){
		HAL_SPI_TransmitReceive_DMA(&hspi2, &a, (uint8_t*)&SpiRxBuffer[i], 1);

		while (!(SPI2->SR & SPI_FLAG_TXE)); // check if transmit buffer has been shifted out
		while ((SPI2->SR & SPI_FLAG_BSY)); // shouldnt be here
	}
	while ((SPI2->SR & SPI_FLAG_BSY)); // check BUSY flag

	if(device)
		HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port,SPI2_CS1_Pin , GPIO_PIN_SET); // CS HIGH
	else
		HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port,SPI2_CS2_Pin , GPIO_PIN_SET); // CS HIGH

	return addr;
}

void spi_write(uint16_t addr, uint16_t data, uint8_t size, ENC device){
	/* Write: MSB is 0*/
	/* Setup time for CS signal 350ns */
	// MSB is 0 + address

	// every frame needs parity
	if(spi_parity(addr)) // parity bit (bit 15)
		addr |= 0x8000;

	if(spi_parity(data)) // parity bit (bit 15)
		data |= 0x8000;

	if(device)
		HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port,SPI2_CS1_Pin , GPIO_PIN_RESET); // CS LOW
	else
		HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port,SPI2_CS2_Pin , GPIO_PIN_RESET); // CS LOW

	HAL_SPI_TransmitReceive_DMA(&hspi2, &addr, SpiRxBuffer, 1);

	while (!(SPI2->SR & SPI_FLAG_TXE)); // check if transmit buffer has been shifted out
	while ((SPI2->SR & SPI_FLAG_BSY));

	HAL_SPI_TransmitReceive_DMA(&hspi2, &data, SpiRxBuffer, 1);

	while (!(SPI2->SR & SPI_FLAG_TXE)); // check if transmit buffer has been shifted out
	while ((SPI2->SR & SPI_FLAG_BSY)); // check BUSY flag

	if(device)
		HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port,SPI2_CS1_Pin , GPIO_PIN_SET); // CS HIGH
	else
		HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port,SPI2_CS2_Pin , GPIO_PIN_SET); // CS HIGH
}


uint16_t spi_read_enc(ENC device){


}



