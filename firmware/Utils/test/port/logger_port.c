///
/// author: Karol Trzciński
/// date: 10-2017
///

#include <stdio.h>
#include "../../logger.h"

mutex_state_t p_logger_mutex_on()
{
  return 0;
}

void p_logger_mutex_off(mutex_state_t st)
{

}

bool p_logger_send(char* ptr, int len)
{
	while (len > 0) {
		len -= printf("%s", ptr);
	}
  return true;
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
