/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef HoXz_h
#define HoXz_h

#include <CList.h>
#ifdef __cplusplus
extern "C"{
#endif
void*   HoXzConvert              (char*);
void    HoXzBuildHBOOK_Object    (void*,int);
int     HoXzConvertToHBOOK       (char*);
void    HoXzFitWithMINUIT        (void*,int,int,double*,double*,double*,int,double*,double*);
void    HoXzProduceRZ_File       (char*,CList,int);
int     HoXzGetIdentifier        (char*);
#ifdef __cplusplus
}
#endif

#endif  /*HoXz_h*/
