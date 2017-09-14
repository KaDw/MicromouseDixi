/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define D_LF_Pin GPIO_PIN_14
#define D_LF_GPIO_Port GPIOC
#define D_LS_Pin GPIO_PIN_15
#define D_LS_GPIO_Port GPIOC
#define ADC_RS_Pin GPIO_PIN_0
#define ADC_RS_GPIO_Port GPIOC
#define ADC_RF_Pin GPIO_PIN_1
#define ADC_RF_GPIO_Port GPIOC
#define ADC_R_Pin GPIO_PIN_2
#define ADC_R_GPIO_Port GPIOC
#define ADC_L_Pin GPIO_PIN_3
#define ADC_L_GPIO_Port GPIOC
#define ADC_LS_Pin GPIO_PIN_2
#define ADC_LS_GPIO_Port GPIOA
#define ADC_LF_Pin GPIO_PIN_3
#define ADC_LF_GPIO_Port GPIOA
#define D_RF_Pin GPIO_PIN_4
#define D_RF_GPIO_Port GPIOC
#define D_RS_Pin GPIO_PIN_5
#define D_RS_GPIO_Port GPIOC
#define ADC_VBAT_Pin GPIO_PIN_1
#define ADC_VBAT_GPIO_Port GPIOB
#define BTN_Pin GPIO_PIN_2
#define BTN_GPIO_Port GPIOB
#define BTN_EXTI_IRQn EXTI2_IRQn
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define ML_IN1_Pin GPIO_PIN_6
#define ML_IN1_GPIO_Port GPIOC
#define ML_IN2_Pin GPIO_PIN_7
#define ML_IN2_GPIO_Port GPIOC
#define ML_PWM_Pin GPIO_PIN_8
#define ML_PWM_GPIO_Port GPIOC
#define MR_PWM_Pin GPIO_PIN_9
#define MR_PWM_GPIO_Port GPIOC
#define MR_IN2_Pin GPIO_PIN_8
#define MR_IN2_GPIO_Port GPIOA
#define MR_IN1_Pin GPIO_PIN_9
#define MR_IN1_GPIO_Port GPIOA
#define USART1_TX_Pin GPIO_PIN_6
#define USART1_TX_GPIO_Port GPIOB
#define USART1_RX_Pin GPIO_PIN_7
#define USART1_RX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
