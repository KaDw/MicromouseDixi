///
/// author: Karol Trzcinski
/// date: 10-2017
///

#include <stdio.h>
#include "../../logger.h"
#include "timer.h"

mutex_state_t p_logger_mutex_on()
{
  return 0;
}

void p_logger_mutex_off(mutex_state_t st)
{

}


void timer_handler()
{
	logger_sendCompleted();
}

bool p_logger_send(char* ptr, int len)
{
	if(timer_isEnabled()) return 1;

	if(len == 0){
		printf("Zero len at start\n");
	}

	while (len > 0) {
		len -= printf("%s", ptr);
	}
	// while(in Transmission);
	// logger_SendCompleted(); return 1;
	start_timer(1, timer_handler); // opoxnij zakonczenie wysylania o 1ms
  return 0;
}

void p_logger_assert_fun(int line, const char* msg)
{
	printf("Assertion failed, line: %d, msg:%s\n", line, msg);
  while(0){
  }
}

// przypisz wskazniki funkcji
logger_fun_t logger_fun = {
  .mutex_on = p_logger_mutex_on,
  .mutex_off = p_logger_mutex_off,
  .send = p_logger_send,
  .assert_fun = p_logger_assert_fun
};
