/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CFITS_h
#define CFITS_h

#ifndef NO_C_H
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
short* CFITS_GetImageFromStream  (FILE*,int,int,unsigned int,unsigned int,int);
void   CFITS_GetHeaderFromStream (FILE*,unsigned int*,unsigned int*);
void   CFITS_DumpHeaderInStream  (FILE*);
void   CFITS_PutImageInStream    (FILE*,short*,unsigned int,unsigned int,int);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCFITS(0); /**/
#endif

#endif  /*CFITS_h*/
