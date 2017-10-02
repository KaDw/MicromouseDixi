///
/// author: Karol Trzciński
/// date: 2017
///
/// reference: https://github.com/jeethridge/embedded-c-generic-fifo
///
// usage:
// typedef struct { int x; int y; } myDataStruct_t;
// #define myDataStructArraySize 15
// myDataStruct _buffInstance[myDataStructArraySize];
//
// fifo_typedef(myDataStruct_t, PointsQueue_t);
// PointsQueue_t q1;
// fifo_init(&q1, myDataStructArraySize, &_buffInstance);
// // or just fifo_create(q1, myDataStruct_t, myDataStructArraySize);
//
// myDataStruct_t dataR, dataW = {.x=5, .y=7};
// fifo_push(&q1, &dataW);
// fifo_pop(&q1, &dataR);
// assert(memcmp(dataW, dataR, sizeof(dataW)) == 0);
//



#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define fifo_typedef(T, NAME) \
  typedef struct { \
    int size; \
    int start; \
    int end; \
    int write_count;\
    int read_count;\
    T* elems; \
  } NAME

#define fifo_init(BUF, S, BUFMEM) \
  do {\
    (BUF)->size = S;\
    (BUF)->start = 0;\
    (BUF)->end = 0;\
    (BUF)->read_count=0;\
    (BUF)->write_count=0;\
    (BUF)->elems = BUFMEM;\
  } while (0)

#define fifo_create(BUF, T, S) \
    fifo_typedef(T, BUF##_q);\
    T _##BUF##_buff[S];\
    BUFF##_q ##BUFF = {\
      .size = S;\
      .start = 0;\
      .end = 0;\
      .read_count = 0;\
      .write_count = 0;\
      .elems = _##BUF##_buff/}

#define fifo_push(BUF, PELEM)\
  do {\
    if(!fifo_is_full(BUF)) {\
      (BUF)->elems[(BUF)->end]=(*PELEM);\
      (BUF)->write_count++;\
      (BUF)->end=((BUF)->end+1)%(BUF)->size;\
    }\
  } while(0)

#define fifo_pop(BUF, PELEM)\
  do {\
    if(!fifo_is_empty(BUF)){\
      (*PELEM)=(BUF)->elems[(BUF)->start];\
      (BUF)->read_count++;\
      (BUF)->start=((BUF)->start+1)%(BUF)->size;\
    } \
  } while(0)

#define fifo_peek(BUF,PELEM,INDEX)\
  do {\
    if(INDEX<fifo_count(BUF)){ \
      (*PELEM)=(BUF)->elems[((BUF)->start+INDEX)%((BUF)->size)];\
    }\
  } while(0)

#define fifo_flush(BUF)\
  do { \
    (BUF)->start = 0; \
    (BUF)->end = 0; \
    (BUF)->read_count=0;\
    (BUF)->write_count=0; \
  } while(0)

#define fifo_count(BUF) ((BUF)->write_count-(BUF)->read_count)
#define fifo_is_full(BUF) (fifo_count(BUF)==(BUF)->size)
#define fifo_is_empty(BUF) (fifo_count(BUF)==0)

#ifdef __cplusplus
}
#endif

#endif
