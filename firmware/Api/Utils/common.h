///
/// author: Karol Trzciński
/// date: 10-2017
///

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

#define PI 3.14159265358979323846f


#ifndef ABS
#define ABS(x)  (((x)<0)?-(x):(x))
#endif

#ifndef SGN
#define SGN(x) (((x)>0)-((x)<0))
#endif

// contrain x value to <lo; hi> range
int contrain(int x, int lo, int hi);

#ifdef __cplusplus
}
#endif
#endif
