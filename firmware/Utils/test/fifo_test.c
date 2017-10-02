///
/// author: Karol Trzciński
/// date: 2017
///

#include <stdio.h>
#include "../fifo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int x, y;
} point_t;

#define _Q1_SIZE 5
point_t _Q1_BUFF[_Q1_SIZE];

fifo_typedef(point_t, point_q);

int main()
{
  	printf("=================\n");
  	printf(" FIFO test start\n");
  	printf("==================\n");

		point_q points;
		point_t data = {.x = 0, .y = 7};

    printf("\ninit fifo, size: %d\n", _Q1_SIZE);
		fifo_init(&points, _Q1_SIZE, _Q1_BUFF);

    for(int i = 0; i < _Q1_SIZE+3; ++i)
    {
      printf("after %d push size of fifo is %d\n", i, fifo_count(&points));
      data.x = i;
	    fifo_push(&points, &data);
    }

    fifo_peek(&points, &data, 0);
    printf("\nfifo peek: {0,7} == {%d, %d}\n\n", data.x, data.y);

    int i = 0;
    while(!fifo_is_empty(&points))
    {
      fifo_pop(&points, &data);
      printf("fifo pop: {%d,7} == {%d, %d}\n", i, data.x, data.y);
      ++i;
    }

    ++data.x;
    data.y = 2;
    fifo_push(&points, &data);
    fifo_pop(&points, &data);
    printf("\nfifo pop after overflow: {%d,2} == {%d, %d}\n", _Q1_SIZE, data.x, data.y);

}

#ifdef __cplusplus
}
#endif
