///
/// author: Karol Trzciński, Poland
/// date: 2017
///

#include <stdio.h>
#include <assert.h>
#include "../ring.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int x, y;
} point_t;

#define _Q1_SIZE 5
point_t _Q1_BUFF[_Q1_SIZE];

ring_typedef(point_t, point_q);

int main()
{
  	printf("=================\n");
  	printf(" ring test start\n");
  	printf("==================\n");

		point_q points;
		point_t data = {.x = 0, .y = 7};

    printf("\ninit ring, size: %d\n", _Q1_SIZE);
		ring_init(&points, _Q1_SIZE, _Q1_BUFF);

    for(int i = 0; i < _Q1_SIZE+3; ++i)
    {
      printf("after %d push size of ring is %d\n", i, ring_count(&points));
      data.x = i;
	    ring_push(&points, data);
      assert(points.end < points.size);
    }

    ring_peek(&points, data, 0);
    printf("\nring peek: {0,7} == {%d, %d}\n\n", data.x, data.y);

    int i = 0;
    while(!ring_is_empty(&points))
    {
      ring_pop(&points, data);
      printf("ring pop: {%d,7} == {%d, %d}\n", i, data.x, data.y);
      ++i;
    }

    ++data.x;
    data.y = 2;
    ring_push(&points, data);
    ring_pop(&points, data);
    printf("\nring pop after overflow: {%d,2} == {%d, %d}\n", _Q1_SIZE, data.x, data.y);

    printf("\n ring_create shortcut\n");
    ring_create(point_t, pozycje, 6);

    for(int i = 0; i < pozycje.size+3; ++i)
    {
      printf("after %d push the size of ring is %d\n", i, ring_count(&points));
      data.x = i;
	    ring_push(&points, data);
    }

    printf("\n ring_create T with array\n");
    typedef struct { int coords[3]; } tab_t;
    tab_t tab = {.coords = {1, 2, 3}};
    ring_create(tab_t, tab_ring, 4);
    for(int i = 0; i < tab_ring.size+3; ++i)
    {
      printf("after %d push the size of ring is %d\n", i, ring_count(&tab_ring));
      tab.coords[1] = i;
	    ring_push(&tab_ring, tab);
    }
    i = 0;
    while(!ring_is_empty(&tab_ring))
    {
      ring_pop(&tab_ring, tab);
      printf("ring pop: {1,%d,3} == {%d,%d,%d}\n", i, tab.coords[0], tab.coords[1], tab.coords[2]);
      ++i;
    }

}

#ifdef __cplusplus
}
#endif
