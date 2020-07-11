/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XxF77_h
#define XxF77_h

#ifdef __CI_SPECIFIC__
#define HAS_NOT_U
#endif

#ifndef HAS_NOT_U
#define xdpope xdpope_
#define xdpclo xdpclo_
#define xmbope xmbope_
#define xmbsen xmbsen_
#endif

#include <FString.h>
#ifdef __cplusplus
extern "C"{
#endif
/*F77*/
int  xdpope ();
void xdpclo ();
int  xmbope (FString,int*);
void xmbsen (FString,int*);
/*F77*/
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXxF77(0); /**/
#endif

#endif /*XxF77_h*/
