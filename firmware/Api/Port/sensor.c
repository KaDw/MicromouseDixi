#include "stm32f4xx_hal.h"
#include "adc.h"
#include "sensor.h"
#include "tim.h"

volatile uint32_t g_sens[6];
static uint32_t g_cal[6];


void sensor_read_ambient(){
	ADC1->SQR1 = (ADC_SQR1_L_2|ADC_SQR1_L_0); // 6 conversions (0x05, count from 0)
	// channels order is important!
	ADC1->SQR3 = (ADC_LS)|(ADC_RS<<5)|(ADC_L<<10)|(ADC_R<<15)|(ADC_LF<<20)|(ADC_RF<<25); // channels to convert: 2, 11, 12, 13 and 9 for battery
	HAL_ADC_Start_DMA(&hadc1, g_cal, 6);
}

void sensor_read_channel(uint8_t CHx, volatile uint32_t *buf){
	/*
	ADC_ChannelConfTypeDef sConfig;

	sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

	*/
	ADC1->SQR1 &= ~ADC_SQR1_L; // 1 conversion
	ADC1->SQR3 = CHx; // channel to be converted
	// TODO error handler
	if(HAL_ADC_Start_DMA(&hadc1, buf, 1) != HAL_OK)
		return;
}

void sensor_read_channel2(uint8_t CHx1, uint8_t CHx2, volatile uint32_t *buf){
	ADC1->SQR1 = ADC_SQR1_L_0;
	ADC1->SQR3 = (CHx1)|(CHx2<<5);
	if(HAL_ADC_Start_DMA(&hadc1, buf, 2) != HAL_OK)
		return;
}

//void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
//
//
//}


/* 	00: Input (reset state)
	01: General purpose output mode
	10: Alternate function mode
	11: Analog mode */


void sensor_off(){
	// config ir led as input with pulldown
	// 00: Input (reset state)
	// ir leds on the right (D_Rx)
	GPIOC->MODER &= ~(GPIO_MODER_MODER13_0|GPIO_MODER_MODER13_1|
					  GPIO_MODER_MODER14_0|GPIO_MODER_MODER14_1|
					  GPIO_MODER_MODER15_0|GPIO_MODER_MODER15_1);
	// ir leds on the left (D_Lx)
	GPIOA->MODER &= ~(GPIO_MODER_MODER4_0|GPIO_MODER_MODER4_0|
			  	  	  GPIO_MODER_MODER5_0|GPIO_MODER_MODER5_1|
					  GPIO_MODER_MODER6_0|GPIO_MODER_MODER6_1);

	// pulldown
	GPIOC->PUPDR |= ((GPIO_PUPDR_PUPD13_1 & GPIO_PUPDR_PUPD13_Msk)|
					(GPIO_PUPDR_PUPD14_1 & GPIO_PUPDR_PUPD14_Msk)|
					(GPIO_PUPDR_PUPD15_1 & GPIO_PUPDR_PUPD15_Msk));

	GPIOA->PUPDR |= ((GPIO_PUPDR_PUPD4_1 & GPIO_PUPDR_PUPD4_Msk)|
					(GPIO_PUPDR_PUPD5_1 & GPIO_PUPDR_PUPD5_Msk)|
					(GPIO_PUPDR_PUPD6_1 & GPIO_PUPDR_PUPD6_Msk));
}

void sensor_callback(void)
{
	static int count = 0;
		switch(count){
		case 0:
			sensor_read_ambient(); // read ambient light
			HAL_GPIO_WritePin(D_LF_GPIO_Port, D_LF_Pin, GPIO_PIN_SET);
			break;
		case 2: // case 3: // 40us
			sensor_read_channel(ADC_LF, &g_sens[0]); // LF read
			HAL_GPIO_WritePin(D_LF_GPIO_Port, D_LF_Pin, GPIO_PIN_RESET);
			break;
		case 3: //120us case 6: // 100us
			HAL_GPIO_WritePin(D_RF_GPIO_Port, D_RF_Pin, GPIO_PIN_SET);
			break;
		case 5: //160us case 8: // 140us
			sensor_read_channel(ADC_RF, &g_sens[1]); // RF read
			HAL_GPIO_WritePin(D_RF_GPIO_Port, D_RF_Pin, GPIO_PIN_RESET);
			break;
		case 7: // 240us case 11: // 200us
			HAL_GPIO_WritePin(D_L_GPIO_Port, D_L_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(D_R_GPIO_Port, D_R_Pin, GPIO_PIN_SET);
			break;
		case 8: // 280us case 13: // 240us
			sensor_read_channel2(ADC_L, ADC_R, &g_sens[2]); // L, R read
			HAL_GPIO_WritePin(D_L_GPIO_Port, D_L_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(D_R_GPIO_Port, D_R_Pin, GPIO_PIN_RESET);
			break;
		case 10: // 360us case 16: // 300us
			HAL_GPIO_WritePin(D_LS_GPIO_Port, D_LS_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(D_RS_GPIO_Port, D_RS_Pin, GPIO_PIN_SET);
			break;
		case 11: // case 18: // 340us
			sensor_read_channel2(ADC_LS, ADC_RS, &g_sens[4]); // LS, RS read
			HAL_GPIO_WritePin(D_LS_GPIO_Port, D_LS_Pin, GPIO_PIN_RESET); // side sensors off
			HAL_GPIO_WritePin(D_RS_GPIO_Port, D_RS_Pin, GPIO_PIN_RESET);
			for(int i = 0; i < SENS_COUNT; ++i)
			{
				if(g_sens[i] < g_cal[i])
					g_sens[i] = g_cal[i];

				g_sens[i] -= g_cal[i];
			}

			count = 0;
			HAL_TIM_Base_Stop(&htim7);
			return;


	}
	count++;
}

