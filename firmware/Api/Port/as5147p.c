#include "as5147p.h"
#include "gpio.h"
#include "spi.h"


// AS5147P is using even parity
// if the count of bits with a value of 1 is odd, the parity bit value is set to 1

// odd number of bits return 1
static uint16_t spi_parity(uint16_t x){
	x ^= x >> 8;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return x & 1;
}

static inline void spi_select_device(ENC device, GPIO_PinState PinState){
	if(device)
		HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, PinState);
	else
		HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, PinState);
}

static uint16_t spi_read(uint16_t addr, ENC device){
	uint16_t SpiRxBuffer;

	/* Read mode mask */
	addr |= 0x4000;
	if(spi_parity(addr)) // add parity bit (MSB)
		addr |= 0x8000;

	spi_select_device(device, GPIO_PIN_RESET);

	/* Setup time for CS signal 350ns */
	HAL_SPI_Transmit(&hspi2, (uint8_t*)&addr, 1, 10);

	spi_select_device(device, GPIO_PIN_SET);
	// High time of CSn between two transmissions: 350ns

	for(int i = 0; i < 8; i++){ // 500ns delay
		addr |= 0x4000;
	}

	spi_select_device(device, GPIO_PIN_RESET);

	// for multi-read mode
	HAL_SPI_Receive(&hspi2, (uint8_t*)&SpiRxBuffer, 1, 10);

	spi_select_device(device, GPIO_PIN_SET);

	return SpiRxBuffer;
}

static void spi_write(uint16_t addr, uint16_t data, uint8_t size, ENC device){
	/* Write: MSB is 0*/
	/* Setup time for CS signal 350ns */

	// every frame needs parity
	if(spi_parity(addr)) // add parity bit (MSB)
		addr |= 0x8000;

	spi_select_device(device, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi2, (uint8_t*)&addr, 1, 10); // (uint8_t*)&addr ?

	spi_select_device(device, GPIO_PIN_SET);

	// High time of CSn between two transmissions: 350ns

	// we need to wait >350ns anyway, why not calculate parity now?
	if(spi_parity(data)) // TODO add parity bit (MSB)
		data |= 0x8000;
	for(int i = 0; i < 3; i++){ // delay, address is not used anymore
		addr |= 0x1234;
	}

	spi_select_device(device, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi2, (uint8_t*)&data, 1, 10); // (uint8_t*)&data ?

	spi_select_device(device, GPIO_PIN_SET);
}


uint8_t enc_check(ENC device){
	uint16_t temp;
	temp = spi_read(AS5147P_DIAAGC, device);
	if(temp & 0x0400){
		//log error: magnetic field strength too high
		return 2;
	}
	else if(temp & 0x0800){
		//log error: magnetic field strength too low
		return 1;
	}

	return 0;
}


uint16_t spi_read_enc(ENC device){
	return spi_read(AS5147P_ANGLECOM, device);
}




