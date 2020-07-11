/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoF77_h
#define WoF77_h

#ifndef HAS_NOT_U
#define wofile wofile_
#define wocmd  wocmd_
#define worun  worun_
#endif

#include <FString.h>
#ifdef __cplusplus
extern "C"{
#endif
/*F77*/
void wofile (FString,int*);
int  worun  ();
void wocmd  (FString,int*,void(*)());
/*F77*/
#ifdef __cplusplus
}
#endif

#endif /*WoF77_h*/
