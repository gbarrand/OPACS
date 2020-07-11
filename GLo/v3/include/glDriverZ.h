/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glDriverZ_h
#define glDriverZ_h

#include <glContext.h>
#ifdef __cplusplus
extern "C"{
#endif
void           glDriverZ_SetWindow          (int,int);
void           glDriverZ_ClearClass         ();
unsigned long  glDriverZ_ConvertRGB_ToPixel (double,double,double);
void           glDriverZ_SetViewport        (int,int,int,int);
void           glDriverZ_ClearColorBuffer   (DColor);
void           glDriverZ_ClearDepthBuffer   (double);
void           glDriverZ_DrawPrimitive      (unsigned int,int,double*,double*,double*,glContext);
void           glDriverZ_DrawBuffer         (double,double,int,int,float,float,unsigned char*,glContext);
unsigned long* glDriverZ_ReadPixels         (int,int,int,int);
#ifdef __cplusplus
}
#endif


#endif  /*glDriverZ_h*/
