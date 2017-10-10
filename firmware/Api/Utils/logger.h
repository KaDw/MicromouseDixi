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
#include "Port/logger_port.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LOGGER_ADD_ERROR_FUN_NAME
#define LOG_CRITICAL(...)   logger_log('C', __FUNCTION__, __VA_ARGS__)
#define LOG_ERR(...)        logger_log('E', __FUNCTION__, __VA_ARGS__)
#else
#define LOG_CRITICAL(...)   logger_log('C', 0, __VA_ARGS__)
#define LOG_ERR(...)        logger_log('E', 0, __VA_ARGS__)
#endif
#define LOG_WARNING(...)    logger_log('W', 0, __VA_ARGS__)
#define LOG_INFO(...)       logger_log('I', 0, __VA_ARGS__)
#define LOG_DEBUG(...)      logger_log('D', 0, __VA_ARGS__)

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
void logger_log(int type, const char* prefix, const char* frm, ...);

// wysyla dane do urzadzen logujacych w wolnej chwili
// wywolywane z glownego watku
void logger_process();

// port layer sould call this function, after transmission is completed
void logger_sendCompleted();

#ifdef __cplusplus
}
#endif
#endif
