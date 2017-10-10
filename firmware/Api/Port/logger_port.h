///
/// author: Karol Trzciński
/// date: 10-2017
///

#ifndef __LOGGER_PORT_H__
#define __LOGGER_PORT_H__
#include "common_port.h"

/// ilosc buforow sluzacych przechowywaniu sformatowanych logow
#define LOGGER_BUF_COUNT 2

/// zostana utworzone LOGGER_BUF_COUNT buforów podanej wielkosci
/// beda sluzyc do przechowywania logow az do czasu
/// wyslania poprzednich we wszystkie miejsca docelowe
#define LOGGER_BUF_SIZE 120

/// when 1 then each log will be in only one buffer -
/// logs won't be divided into 2 buffers
#define LOGGER_CONTINOUS_LOG 1


/// log suffix
#define LOGGER_ADD_SLASH_R 			0

/// log suffix
#define LOGGER_ADD_SLASH_ZERO 	1

/// error option
#define LOGGER_ADD_ERROR_FUN_NAME	1

/// typ zwracany przez funcje odpowiedzialna za
/// uruchamianie mutexow dla loggera
typedef int mutex_state_t;


/// platform dependent logger initializaction
void logger_init_port();

#endif
