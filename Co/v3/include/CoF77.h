/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CoF77_h
#define CoF77_h

#ifndef HAS_NOT_U
#define cwrite cwrite_
#define cbindu cbindu_
#define cdargn cdargn_
#define cdargi cdargi_
#define crando crando_
#endif

#include <FString.h>
#ifdef __cplusplus
extern "C"{
#endif
/*F77*/
void  cwrite (void*,int*);
void  cbindu (void*,int*,int*);
int   cdargn ();
int   cdargi (int*,FString,int*);
float crando ();
/*F77*/
#ifdef __cplusplus
}
#endif

#endif /*CoF77_h*/
