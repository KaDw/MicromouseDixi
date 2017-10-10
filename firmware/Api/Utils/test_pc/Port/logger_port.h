///
/// author: Karol Trzciński
/// date: 10-2017
///

#ifndef __LOGGER_PORT_H__
#define __LOGGER_PORT_H__

/// ilosc buforow sluzacych przechowywaniu sformatowanych logow
#define LOGGER_BUF_COUNT 2

/// zostana utworzone LOGGER_BUF_COUNT buforów podanej wielkosci
/// beda sluzyc do przechowywania logow az do czasu
/// wyslania poprzednich we wszystkie miejsca docelowe
#define LOGGER_BUF_SIZE 120

/// log suffix
#define LOGGER_ADD_SLASH_R 			0

/// log suffix
#define LOGGER_ADD_SLASH_ZERO 	1

/// error option
#define LOGGER_ADD_ERROR_FUN_NAME	1

/// buffering option - when 0, then log can start in buffer (eg) 0 and end in next buffer (here) 1
#define LOGGER_CONTINOUS_LOG 		1

/// typ zwracany przez funcje odpowiedzialna za
/// uruchamianie mutexow dla loggera
typedef int mutex_state_t;

// only for pc test
void p_logger_makeTxReady();
#endif
