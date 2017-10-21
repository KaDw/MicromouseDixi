#include "stm32f4xx_hal.h"
#include "adc.h"
#include "sensor.h"
#include "tim.h"

uint32_t sens[6];
uint32_t cal[6];
volatile uint32_t read[6];


void sensor_read_ambient(){
	ADC1->SQR1 = (ADC_SQR1_L_2|ADC_SQR1_L_0); // 6 conversions (0x05, count from 0)
	// channels order is important!
	ADC1->SQR3 = (ADC_LS)|(ADC_RS<<5)|(ADC_L<<10)|(ADC_R<<15)|(ADC_LF<<20)|(ADC_RF<<25); // channels to convert: 2, 11, 12, 13 and 9 for battery
	//ADC1->SQR2 = CH9;
	//CH3, CH10
	HAL_ADC_Start_DMA(&hadc1, cal, 6);
	//vbat = cal[6]; // *0.0025 i dodac 0,067
	// battery
//	if(cal[6] < 3408 && cal[6] > 2650){ // 7,4-6,8
//		vbat += cal[6];
//		batcnt++;
//		if(vbat){
//			batcnt = 0;
//			UI_LedOnAll();
//			batError = 1;
//		}
//	}
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


//void sensor_off(){
//	// config ir led as input with pulldown
//	// ir leds on the right (D_Rx)
//	GPIOC->MODER &= ~(GPIO_MODER_MODER14_0|GPIO_MODER_MODER14_1|
//					  GPIO_MODER_MODER15_0|GPIO_MODER_MODER15_1|
//					  GPIO_MODER_MODER13_0|GPIO_MODER_MODER13_1);
//	// ir leds on the left (D_Lx)
//	GPIOA->MODER &= ~(GPIO_MODER_MODER6_0|GPIO_MODER_MODER6_0
//			  	  	  GPIO_MODER_MODER4_0|GPIO_MODER_MODER4_1|
//					  GPIO_MODER_MODER5_0|GPIO_MODER_MODER5_1);
//	// 10
//	//GPIO_PUPDR_PUPD14_1|
//
//
//}

void sensor_callback(void)
{
	static int count = 0;

	//static int COUNTER_MAX = MOTOR_DRIVER_T/0.00002;
		switch(count){
		case 0:
			sensor_read_ambient(); // read ambient light
			HAL_GPIO_WritePin(D_LF_GPIO_Port, D_LF_Pin, GPIO_PIN_SET);
			break;
		case 2: // case 3: // 40us
			sensor_read_channel(ADC_LF, &read[0]); // LF read
			HAL_GPIO_WritePin(D_LF_GPIO_Port, D_LF_Pin, GPIO_PIN_RESET);
			break;
		case 3: //120us case 6: // 100us
			HAL_GPIO_WritePin(D_RF_GPIO_Port, D_RF_Pin, GPIO_PIN_SET);
			break;
		case 5: //160us case 8: // 140us
			sensor_read_channel(ADC_RF, &read[1]); // RF read
			HAL_GPIO_WritePin(D_RF_GPIO_Port, D_RF_Pin, GPIO_PIN_RESET);
			break;
		case 7: // 240us case 11: // 200us
			HAL_GPIO_WritePin(D_L_GPIO_Port, D_L_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(D_R_GPIO_Port, D_R_Pin, GPIO_PIN_SET);
			break;
		case 8: // 280us case 13: // 240us
			sensor_read_channel2(ADC_L, ADC_R, &read[2]); // L, R read
			HAL_GPIO_WritePin(D_L_GPIO_Port, D_L_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(D_R_GPIO_Port, D_R_Pin, GPIO_PIN_RESET);
			break;
		case 10: // 360us case 16: // 300us
			HAL_GPIO_WritePin(D_LS_GPIO_Port, D_LS_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(D_RS_GPIO_Port, D_RS_Pin, GPIO_PIN_SET);
			break;
		case 11: // case 18: // 340us
			sensor_read_channel2(ADC_LS, ADC_RS, &read[4]); // LS, RS read
			HAL_GPIO_WritePin(D_LS_GPIO_Port, D_LS_Pin, GPIO_PIN_RESET); // side sensors off
			HAL_GPIO_WritePin(D_RS_GPIO_Port, D_RS_Pin, GPIO_PIN_RESET);
			break;
		case 12:
			for(int i = 0; i < SENS_COUNT; ++i)
			{
				// sprawdz czy naswietlona sciana nie jest jasniejsza od nienaswietlonej
				if(read[i] < cal[i])
					read[i] = cal[i];

				sens[i] = read[i] - cal[i];
			}
//
//				//odejmij skladowa stala od sygnalu
//				sens[i] = read[i] - cal[i];
//
//				// oblicz licznik i mianownik do wyjscia
//				// butter 100Hz/1000Hz, 2 stopnia
//				float num = 0.00554271721028068 * read[i] - 177863177782459 * read_buf[0][i] + 0.800802646665708 * read_buf[1][i];
//				float den = 0.00554271721028068 * sens[i] + 0.0110854344205614 * sens_buf[0][i] + 0.00554271721028068 * sens_buf[1][i];
//
//				// sprawdz czy mozesz bezpiecznie dzielic
////				if(den != 0)
////					sens[i] = num/den;
////				else
//					sens[i] = (sens_buf[1][i] + read[i] + 2*read_buf[0][i] + 4*read_buf[1][i]) * 0.125f;
//
//				// zaktualizuj bufory
//				read_buf[1][i]=read_buf[0][i]; read_buf[0][i]=read[i];
//				sens_buf[1][i]=sens_buf[0][i]; sens_buf[1][i]=sens[i];
//
//				// zlinearyzuj led'y
//				LinLEDleft();
//				LinLEDright();
//				//LinLEDs();
//			}
//
			break;
//		case 13:
//			GyroGetAngle(0.001);
//			break;
//
//		case 14:
//			//MotorStepResponse(160, 150, 1500);
//			MotorUpdate();
//			break;
	}


	count++;

	if(count >= 25){
		//HAL_GPIO_WritePin(GPIOB, CS_A_Pin, 1);
		count = 0;
	}
}

