// author: Karol Trzciński
// date: 2017
//
// multi buffered logger implementation
// one buffer is for fast writing from interrupts or main thread
// others buffers are for sendind via UART, USB, SD... from main thread
// buffers automaticaly changes before sending starts


/// podstawowa jednostka buforowa
typedef struct
{
  char buff[LOGGER_BUF_SIZE+2]; // dodatkowe miejsce na '\r\n'
  int len = 0
  bool busy;
  bool full;
} buff_t;

typedef struct* buff_ptr;

/// glowna struktura loggera
typedef struct
{
  buff_t buff[LOGGER_BUF_COUNT];
  char buff_temp[LOGGER_BUF_TEMP_SIZE];
  int wbuff_index, rbuff_index;
} logget_t;

logger_t logger;

// funkcja inicjalizujaca strukture
static void _logger_init_struct(logget_t* l)
{
  LOGGER_ASSERT(LOGGER_BUF_COUNT > 1);
  LOGGER_ASSERT(LOGGER_BUF_SIZE > LOGGER_BUF_TEMP_SIZE);

  for(int i = 0; i < LOGGER_BUF_COUNT; ++i)
  {
    l->buff[i].busy = l->buff[i].full = false;
    l->buff[i].len = 0;
    l->buff[i].buff[0] = 0; // ustaw zero w pierwszym znaku
  }

  l->wbuff_index = l->rbuff_index = 0;
}

void logger_init()
{
  _logger_init_struct(&logger);
}

/// @param1: current index of buffer
static inline int logger_next_buff(int current)
{
  return (current + 1) % LOGGER_BUF_COUNT;
}


void logger_log(int type, const char* frm, ...)
{
  // czy jest miejsce w buforze
  if(logger_next_buff(logger.wbuff_index) == logger.rbuff_index)
    return;

  // przetworz napisy
  // bo to moze zajac chwile
  int len = 0;
	va_list arg;
  if(type < '!') // drugi znak drukowalny, za spacja
  {
  	itoa(type, logger.buff_temp, 10);
  	len += strlen(buf);
  	buf[len++] = '\t';
  }
  else
  {
    logger.buff_temp[len++] = (char)type;
    logger.buff_temp[len++] = ' ';
  }
	va_start(arg, frm);
	len += vsnprintf(logger.buff_temp+len, LOGGER_BUF_TEMP_SIZE-len, frm, arg);
  logger.buff_temp[len++] = '\n';
  logger.buff_temp[len] = '\0'; // tymczasowo zakoncz napis
	va_end(arg);

  // poczatek sekcji krytycznej
  //while(log_isCopying == true) // tylko przy DMA
  //{}
  buff_ptr wptr = &logger.buff[logger.wbuff_index];
  if(wptr->busy != false)
  {
    return;
  }
  wptr->busy = true;

  // sprawdz czy nowy log zmiesci sie w aktualnym buforze
  // gdy nie, to inkrementuj bufor. Bufor jest juz zaterminowany
  // znakiem '\0'
  if(wptr->len + len >= LOGGER_BUF_SIZE)
  {
    // zapisz piewrsza czesc wiadomosci w starym buforze
    int firstLen = LOGGER_BUF_SIZE-wptr->len;
    memcpy(&wptr->buff[wptr->len], &logger.buff_temp[0], firstLen);
    wptr->len += firstLen;
    len -= firstLen;
    // i zmien bufor zapisu
    logger.wbuff_index = logger_next_buff(logger.wbuff_index);
  }

  // dopisz wiadomosc do bufora
  memcpy(&wptr->buff[wptr->len], &logger.buff_temp[0], len);
  wptr->len += len;
  wptr->busy = false;
}


void logger_process()
{
  // gdy nie ma zadnej wiadomosci
  if(logger.buff[logger.wbuff_index].buff[0]) {
    return;
  }

  // gdy zapis do bufora sie nie zakonczyl
  __disable_irq();
  if(logger.buff[logger.rbuff_index].busy) {
    __enable_irq();
    return;
  }

  // gdy bufor do czytania jest rowniez buforem do pisania
  // to inkrementuj bufor do pisania
  if(logger.wbuff_index == logger.rbuff_index) {
    logger.wbuff_index = logger_next_buff(logger.wbuff_index);
  }

  // bufor do czytania jest zajety
  buff_ptr rptr = &logger.buff[logger.rbuff_index];
  rptr.busy = true;
  __enable_irq();

  // przystap do transmisji
  HAL_UartTransmit(&huart1, &rptr->buff[0], rptr->len);
  HAL_SD_Save();

  // inkrementuj bufor do czytania, bo ten zostal juz obsluzony
  logger.rbuff_index = logger_next_buff(logger.rbuff_index);
}
