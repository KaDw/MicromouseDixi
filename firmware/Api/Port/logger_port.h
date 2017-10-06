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


#define LOGGER_CONTINOUS_LOG 1
/*
/// bufor urzywany do formatowania pojedynczych logow
/// za pomoca funkcji snprintf
#define LOGGER_BUF_TEMP_SIZE 64*/

/// typ zwracany przez funcje odpowiedzialna za
/// uruchamianie mutexow dla loggera
typedef int mutex_state_t;

#endif
