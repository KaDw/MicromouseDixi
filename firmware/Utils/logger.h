// author: Karol Trzciński
// date: 2017
//
// multi buffered logger implementation
// one buffer is for fast writing from interrupts or main thread
// others buffers are for sendind via UART, USB, SD... from main thread
// buffers automaticaly changes before sending starts

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger_conf.h" // must be before logger_conf.h


#define LOG_CRITICAL(...)   logger_log('C',__VA_ARGS__)
#define LOG_ERR(...)        logger_log('E',__VA_ARGS__)
#define LOG_WARNING(...)    logger_log('W',__VA_ARGS__)
#define LOG_INFO(...)       logger_log('I',__VA_ARGS__)
#define LOG_DEBUG(...)      logger_log('D',__VA_ARGS__)

// definicje funkcji portowanych
typedef mutex_state_t (*logger_fun_mutex_on_t)();
typedef void (*logger_fun_mutex_off_t)(mutex_state_t st);
typedef bool (*logger_fun_send_t)(char* ptr, int len);
typedef void (*logger_fun_assert_fun_t)(int line, const char* msg);

// wskazniki na funkcje zalezne od platformy
typedef struct {
  logger_fun_mutex_on_t mutex_on;
  logger_fun_mutex_off_t mutex_off;
  logger_fun_send_t send;
  logger_fun_assert_fun_t assert_fun;
} logger_fun_t;


/// set loggers variable default value
void logger_init();

/// @param1: rodzaj logu, jakis enum urzytkownika
/// @param2: format stringu - patrz fprintf
/// @param3: elpisis
void logger_log(int type, const char* frm, ...);

// wysyla dane do urzadzen logujacych w wolnej chwili
// wywolywane z glownego watku
void logger_process();

#endif
