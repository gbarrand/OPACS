/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CGIF_h
#define CGIF_h

#ifndef NO_C_H
#include <stdio.h>   
#endif

typedef unsigned long (*CGIF_GetPixelFunction)(int,int);

#ifdef __cplusplus
extern "C"{
#endif
unsigned char* CGIF_GetImageFromStream (FILE*,unsigned int*,unsigned int*,int*,double**,double**,double**);
void           CGIF_PutBytesInStream   (FILE*,unsigned char*,unsigned int,unsigned int,int,double*,double*,double*);
void           CGIF_PutImageInStream   (FILE*,CGIF_GetPixelFunction,unsigned int,unsigned int,int,double*,double*,double*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCGIF(0); /**/
#endif

#endif  /*CGIF_h*/
