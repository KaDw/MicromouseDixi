// author: Karol Trzciński
// date: 2017
//
// multi buffered logger implementation
// one buffer is for fast writing from interrupts or main thread
// others buffers are for sendind via UART, USB, SD... from main thread
// buffers automaticaly changes before sending starts

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include "logger.h"

#define LOGGER_ASSERT(x) if(!(x)) { logger_fun.assert_fun(__LINE__, __FUNCTION__); }

/// place for sufix: "\r\n\0"
#define LOGGER_BUF_EXTRA_SIZE 3

/// podstawowa jednostka buforowa
typedef struct
{
  char buff[LOGGER_BUF_SIZE + LOGGER_BUF_EXTRA_SIZE];
  int len;
  bool busy;
} buff_t;

typedef buff_t* buff_ptr;

/// glowna struktura loggera
typedef struct
{
  buff_t buff[LOGGER_BUF_COUNT];
  buff_t buff_temp;
  int wbuff_index, rbuff_index;
} logger_t;

// globalna struktura ze wskaxnikami na funkcje loggera
extern logger_fun_t logger_fun;

// globalna instancja loggera
logger_t logger;

// funkcja inicjalizujaca strukture
static void _logger_init_struct(logger_t* l)
{
	LOGGER_ASSERT(logger_fun.mutex_on != 0);
	LOGGER_ASSERT(logger_fun.mutex_off != 0);
	LOGGER_ASSERT(logger_fun.send != 0);
	LOGGER_ASSERT(logger_fun.assert_fun != 0);
	for (int i = 0; i < LOGGER_BUF_COUNT; ++i)
	{
		l->buff[i].busy = false;
		l->buff[i].len = 0;
		l->buff[i].buff[0] = 0; // ustaw zero w pierwszym znaku
	}

	l->wbuff_index = l->rbuff_index = 0;
}

void logger_init()
{
	LOGGER_ASSERT(LOGGER_BUF_COUNT > 0);
	_logger_init_struct(&logger);
}

/// @param1: current index of buffer
static inline int logger_next_buff(int current)
{
	return (current + 1) % LOGGER_BUF_COUNT;
}

static int _logger_write_prefix(int type, char* buff)
{
	int len = 0;
	if (type < '!') { // ! to drugi znak drukowalny, za spacja
		snprintf(buff, sizeof(logger.buff_temp), "%d", type);
		len += strlen(buff);
		buff[len++] = ' ';
	}
	else {
		buff[len++] = (char)type;
		buff[len++] = ' ';
	}
	return len;
}

/// return 0 when ok, -1 otherwise
static int _logger_tryMoveTemp() {
	// sprawdz czy jest cos do zapisanie
	if (logger.buff_temp.len == 0) {
		return 0;
	}

	// czy jest miejsce w buforze, czyli sformatowany tekst nie zmiesci
	// sie w aktualnym buforze oraz nastepny bufor nie jest wolny
	if (logger.buff[logger.wbuff_index].len + logger.buff_temp.len >= LOGGER_BUF_SIZE &&
		logger_next_buff(logger.wbuff_index) == logger.rbuff_index) {
		// tutaj sygnalizacja bledu przepelnienia wszystkich buforow
		return -1;
	}

	// poczatek sekcji krytycznej
	// sprawdz czy poprzednia operacja kopiowania juz sie zakonczyla
	// dla aktualnego bufora odbiorczego, gdy nie, to pierwsza wiadomosc
	// zostanie zachowana w buforze tymczasowym, a kolejnen przepadna
	buff_ptr wptr = &logger.buff[logger.wbuff_index];
	if (wptr->busy == true) {
		return -1;
	}

#if LOGGER_CONTINOUS_LOG
	// wersja z ciągłością w pamęci logów
	// w poprzednim if sprawdzilismy, ze kolejny bufor jest wolny
	// (bo nie jsst to buf do czytania)
	if (wptr->len + logger.buff_temp.len >= LOGGER_BUF_SIZE + LOGGER_BUF_EXTRA_SIZE) {
		logger.wbuff_index = logger_next_buff(logger.wbuff_index);

		wptr = &logger.buff[logger.wbuff_index];
		if (wptr->busy == true) {
			return -1;
		}
	}
#else
	// wersja bez ciagłości w pamięci logów
	// sprawdz czy nowy log zmiesci sie w aktualnym buforze
	// gdy nie, to przekoiuj tresc, zakoncz aktualny bufor
	// znakiem '\0' i inkrementuj bufor
	if (wptr->len + logger.buff_temp.len >= LOGGER_BUF_SIZE)
	{
		// zapisz piewrsza czesc wiadomosci w starym buforze
		int firstLen = LOGGER_BUF_SIZE - wptr->len;
		memcpy(&wptr->buff[wptr->len], &logger.buff_temp.buff[0], firstLen);
		wptr->len += firstLen;
		wptr->buff[wptr->len] = '\0';
		logger.buff_temp.len -= firstLen;
		// i zmien bufor zapisu
		logger.wbuff_index = logger_next_buff(logger.wbuff_index);
		wptr = &logger.buff[logger.wbuff_index];
	}
#endif
	wptr->busy = true;

	// dopisz wiadomosc do bufora
	memcpy(&wptr->buff[wptr->len], &logger.buff_temp.buff[0], logger.buff_temp.len);
	wptr->len += logger.buff_temp.len;

	// zwolnij bufory
	logger.buff_temp.len = 0;
	logger.buff_temp.busy = false;
	wptr->busy = false;
	return 0;
}

void logger_log(int type, const char* frm, ...)
{
	// przetworz napisy
	va_list arg;

	// gdy jest cos w buforze temp, to postaraj sie to wypchnac
	// gdy sie nie uda, to nie wpisuj nowego loga
	if (logger.buff_temp.busy == true) {
		if(_logger_tryMoveTemp()){
			return;
		}
	}

	// wpisz przedrostek ktory bedzie kodem numerem kodu dla
	// znakow niedrukowalnych, o kodzie mniejszym od kodu '!'
	// w przeciwnym wypadku tym znakiem (gdy jest drukowalny)
	logger.buff_temp.busy = true;
	logger.buff_temp.len = _logger_write_prefix(type, logger.buff_temp.buff);

	// rozpocznij formatowanie zgodne z podanym szablonem
	va_start(arg, frm);
	logger.buff_temp.len += vsnprintf(logger.buff_temp.buff + logger.buff_temp.len, LOGGER_BUF_SIZE - logger.buff_temp.len, frm, arg);
	// wielkosc zwracana przez vsnprintf to wielkosc przerobionego napisu, 
	// a nie ilosc zapisanych danych, wiec sprawdz czy nie wyszedles poza zakres
	if (logger.buff_temp.len >= LOGGER_BUF_SIZE) {
		logger.buff_temp.len = LOGGER_BUF_SIZE-1; // -1 bo chcemy zastapic zero ktore zostalo wstawione na koncu
	}
	logger.buff_temp.buff[logger.buff_temp.len++] = '\r';
	logger.buff_temp.buff[logger.buff_temp.len++] = '\n';
	logger.buff_temp.buff[logger.buff_temp.len] = '\0'; // tymczasowo zakoncz napis
	va_end(arg);

	LOGGER_ASSERT(logger.buff_temp.len < sizeof(logger.buff_temp.buff)/sizeof(*logger.buff_temp.buff));
	_logger_tryMoveTemp();
}


void logger_process()
{
  // gdy nie ma zadnej wiadomosci
  if(logger.buff[logger.rbuff_index].len == 0) {
    return;
  }

  // gdy zapis do bufora sie nie zakonczyl
  // specjalnie robimy to w sekcji krytycznej,
  // by sie nie zmieniło 'po drodze'
  mutex_state_t mutex = logger_fun.mutex_on();
  if(logger.buff[logger.rbuff_index].busy) {
    logger_fun.mutex_off(mutex);
    return;
  }

  // gdy bufor do czytania jest rowniez buforem do pisania
  // to inkrementuj bufor do pisania
  if(logger.wbuff_index == logger.rbuff_index) {
    logger.wbuff_index = logger_next_buff(logger.wbuff_index);
  }

  // bufor do czytania jest zajety
  buff_ptr rptr = &logger.buff[logger.rbuff_index];
  rptr->busy = true;
  logger_fun.mutex_off(mutex);

  // przystap do transmisji, zawsze wysylamy bufor od poczatku
  // do jego aktualanego konca
  logger_fun.send(&rptr->buff[0], rptr->len);
  rptr->len = 0;
  rptr->buff[0] = '\0';
  rptr->busy = false;
  logger.rbuff_index = logger_next_buff(logger.rbuff_index);

  // sprawdz czy nie trzeba wyslac kolejnego bufora
  if (logger.buff_temp.len > 0) {
	  _logger_tryMoveTemp();
  }
  if (logger.buff[logger.rbuff_index].len > 0) {
	  logger_process();
  }

  // inkrementuj bufor do czytania, bo ten zostal juz obsluzony
  rptr->busy = false;
  rptr->len = 0;
}

#ifdef __cplusplus
}
#endif
