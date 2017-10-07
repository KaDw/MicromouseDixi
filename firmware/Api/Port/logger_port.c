///
/// author: Karol Trzcinski
/// date: 10-2017
///

#include "Utils/logger.h"


extern UART_HandleTypeDef* huart3;
#define LOGGER_UART huart3

void logger_port_TxCplt(struct __DMA_HandleTypeDef * hdma)
{
	logger_sendCompleted();
}

void logger_port_init()
{
	LOGGER_UART->hdmatx->XferCpltCallback = logger_port_TxCplt;
}

mutex_state_t logger_port_mutex_on()
{
  mutex_state_t st = __get_PRIMASK();
  __disable_irq();
  return st;
}

void logger_port_mutex_off(mutex_state_t st)
{
	if(st){
		__enable_irq();
	}
}


bool logger_port_send(char* ptr, int len)
{
	if((LOGGER_UART->hdmatx->Instance->CR & DMA_SxCR_EN) != 0) {
		return true; // wysylanie w trakcie - nie udalo sie wyslac tej paczki
	}
	HAL_UART_Transmit_DMA(LOGGER_UART, (uint8_t*)ptr, len);
	return false; // false - wiadomosc jest wysylana
}

void logger_port_assert_fun(int line, const char* msg)
{
	//printf("Assertion failed, line: %d, msg:%s\n", line, msg);
  while(1){
  }
}

// przypisz wskazniki funkcji
logger_fun_t logger_fun = {
  .mutex_on = logger_port_mutex_on,
  .mutex_off = logger_port_mutex_off,
  .send = logger_port_send,
  .assert_fun = logger_port_assert_fun
};


