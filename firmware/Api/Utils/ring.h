///
/// author: Karol Trzciński, Poland
/// date: 2017
///
/// reference: https://github.com/pthrasher/c-generic-ring-buffer/blob/master/ringbuffer.h
///
/// description: macro based ring implementation
///              it is possible to make many ring with different size and data type
///              writing (push) when buffer is full delete older element
///              reading (only pop) from empty buffer will be ommited
///
// usage:
// typedef struct { int x; int y; } myDataStruct_t;
// #define myDataStructArraySize 15
// myDataStruct _buffInstance[myDataStructArraySize];
//
// ring_typedef(myDataStruct_t, PointsQueue_t);
// PointsQueue_t q1;
// ring_init(&q1, myDataStructArraySize, &_buffInstance);
// // or just ring_create(q1, myDataStruct_t, myDataStructArraySize);
//
// myDataStruct_t dataR, dataW = {.x=5, .y=7};
// ring_push(&q1, &dataW);
// ring_pop(&q1, &dataR);
// assert(memcmp(dataW, dataR, sizeof(dataW)) == 0);
//

#ifndef __RING_H__
#define __RING_H__


// define ring data type NAME, holding data in type T
#define ring_typedef(T, NAME) \
  typedef struct { \
    int size; \
    int start; \
    int end; \
    int write_count;\
    int read_count;\
    T* elems; \
  } NAME


// fill fields of ring instance BUF with default values
// point to buffer BUFMEM with capacity of S elements
#define ring_init(BUF, S, BUFMEM) \
  do {\
    (BUF)->size = S; \
    (BUF)->start = 0; \
    (BUF)->end = 0; \
    (BUF)->read_count=0;\
    (BUF)->write_count=0;\
    (BUF)->elems = BUFMEM; \
  } while(0)


// combination of ring_typedef and ring_init queue BUF of type T
// also create in local scope buffer of capacity S
#define ring_create(T, BUF, S) \
    ring_typedef(T, BUF##_r);\
    T _##BUF##_buff[S];\
    BUF##_r BUF = {\
      .size = S,\
      .start = 0,\
      .end = 0,\
      .read_count=0,\
      .write_count=0,\
      .elems = _##BUF##_buff\
    }


#define _ring_next(BUF, I) ((I + 1) % ((BUF)->size))


// write to ring BUF
#define ring_push(BUF, ELEM) \
  do { \
    if(ring_is_full(BUF)) { \
      (BUF)->start = _ring_next((BUF), (BUF)->start); \
      (BUF)->read_count++; \
    } \
    (BUF)->elems[(BUF)->end] = ELEM;\
    (BUF)->end = _ring_next((BUF), (BUF)->end); \
    (BUF)->write_count++; \
  } while(0)


// read from ring BUF and delete readed element
#define ring_pop(BUF, ELEM) \
  do { \
    if(!ring_is_empty(BUF)) { \
      (ELEM) = (BUF)->elems[(BUF)->start]; \
      (BUF)->start = _ring_next(BUF, (BUF)->start); \
      (BUF)->read_count++; \
    } \
  } while(0)


// read from ring BUF without deleting this element
#define ring_peek(BUF, ELEM, INDEX) \
  do { \
    (ELEM) = (BUF)->elems[((BUF)->start+INDEX)%(BUF)->size]; \
  } while(0)


// make ring empty
#define ring_flush(BUF)\
do { \
  (BUF)->start = 0; \
  (BUF)->end = 0; \
  (BUF)->read_count=0;\
  (BUF)->write_count=0; \
} while(0)


// return number of elements in ring BUF
#define ring_count(BUF) (((BUF)->write_count-(BUF)->read_count))


// compare ring count to his size
#define ring_is_full(BUF) (ring_count(BUF)==(BUF)->size))


// compare ring count to zero
#define ring_is_empty(BUF) ((BUF)->start==(BUF)->end)

#endif
