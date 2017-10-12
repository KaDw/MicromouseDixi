/*
 * motor_port.c
 *
 *  Created on: 12.10.2017
 *      Author: Karol
 */

#include "Port/motor_port.h"

void motor_port_init()
{
	__HAL_TIM_PRESCALER(&MOTOR_HTIM, 7);
	__HAL_TIM_SetAutoreload(&MOTOR_HTIM, MOTOR_MAX_PWM);

	// filtracja sygnalu
	//MOTOR_HTIM_ENC_L.Instance->SMCR |= TIM_SMCR_ETF_3;
	//MOTOR_HTIM_ENC_R.Instance->SMCR |= TIM_SMCR_ETF_3;

	HAL_TIM_PWM_Start(&MOTOR_HTIM, MOTOR_CH_L);
	HAL_TIM_PWM_Start(&MOTOR_HTIM, MOTOR_CH_R);
	HAL_TIM_Encoder_Start(&MOTOR_HTIM_ENC_L, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&MOTOR_HTIM_ENC_R, TIM_CHANNEL_ALL);
}

void motor_port_setPwmL(int PWM) {
	HAL_GPIO_WritePin(MOTOR_GPIO, ML_IN1_Pin, PWM>=0 ? GPIO_PIN_SET		 : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_GPIO, ML_IN2_Pin, PWM>=0 ? GPIO_PIN_RESET  : GPIO_PIN_SET);

	__HAL_TIM_SetCompare(&MOTOR_HTIM, MOTOR_CH_L, ABS(PWM));
}

void motor_port_setPwmR(int PWM) {
	HAL_GPIO_WritePin(MOTOR_GPIO, MR_IN1_Pin, PWM<=0 ? GPIO_PIN_SET 	: GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOTOR_GPIO, MR_IN2_Pin, PWM<=0 ? GPIO_PIN_RESET	: GPIO_PIN_SET);

	__HAL_TIM_SetCompare(&MOTOR_HTIM, MOTOR_CH_R, ABS(PWM));
}
