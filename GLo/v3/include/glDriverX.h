/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glDriverX_h
#define glDriverX_h

#include <glContext.h>
#ifdef __cplusplus
extern "C"{
#endif
void          glDriverX_SetCurrentWindow   (void*,unsigned long);
void          glDriverX_SetWindow          (int,int);
void          glDriverX_SetColormap        (unsigned long,int);
void          glDriverX_ClearClass         ();
unsigned long glDriverX_ConvertRGB_ToPixel (double,double,double);
int           glDriverX_ConvertPixelToRGB  (unsigned long,double*,double*,double*);
void          glDriverX_SetViewport        (int,int,int,int);
void          glDriverX_ClearColorBuffer   (DColor);
void          glDriverX_DrawPrimitive      (unsigned int,int,double*,double*,double*,glContext);
void          glDriverX_DrawBuffer         (double,double,int,int,float,float,unsigned char*,glContext);
#ifdef __cplusplus
}
#endif


#endif  /*glDriverX_h*/
