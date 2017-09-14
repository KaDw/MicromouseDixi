// author: Karol Trzciński
// date: 2017
//
// multi buffered logger implementation
// one buffer is for fast writing from interrupts or main thread
// others buffers are for sendind via UART, USB, SD... from main thread
// buffers automaticaly changes before sending starts

/// ilosc buforow sluzacych przechowywaniu sformatowanych logow
#define LOGGER_BUF_COUNT 2

/// zostana utworzone LOGGER_BUF_COUNT buforów podanej wielkosci
/// beda sluzyc do przechowywania logow az do czasu
/// wyslania poprzednich we wszystkie miejsca docelowe
#define LOGGER_BUF_SIZE 124

/// bufor urzywany do formatowania pojedynczych logow
/// za pomoca funkcji snprintf
#define LOGGER_BUF_TEMP_SIZE 64

/// makro asercji loggera
#define LOGGER_ASSERT(x)    if(x){assert_fun(__FUNCTION__);}

#define LOG_CRITICAL(...)   logger_log('C', ...)
#define LOG_ERR(...)        logger_log('E', ...)
#define LOG_WARNING(...)    logger_log('W', ...)
#define LOG_INFO(...)       logger_log('I', ...)
#define LOG_DEBUG(...)      logger_log('D', ...)

/// set loggers variable default value
void logger_init();

/// @param1: rodzaj logu, jakis enum urzytkownika
/// @param2: format stringu - patrz fprintf
/// @param3: elpisis
void logger_log(int type, const char* frm, ...);

// wysyla dane do urzadzen logujacych w wolnej chwili
// wywolywane z glownego watku
void logger_process();
