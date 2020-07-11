#ifndef HoCm_h
#define HoCm_h

#include <OHistogram.h>
#ifdef __cplusplus
extern "C"{
#endif
int        HoCmPutHistogram (void*,OHistogram);
OHistogram HoCmGetHistogram (void*,int);
#ifdef __cplusplus
}
#endif

#endif  /*HoCm_h*/



