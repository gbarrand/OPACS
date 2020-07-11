/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glDriverPS_h
#define glDriverPS_h

#include <glContext.h>
#ifdef __cplusplus
extern "C"{
#endif
void          glDriverPS_ClearClass          ();
void          glDriverPS_RestoreGraphicState ();
void          glDriverPS_GetRGBs             (int*,double**,double**,double**);
unsigned long glDriverPS_ConvertRGB_ToPixel  (double,double,double);
int           glDriverPS_ConvertPixelToRGB   (unsigned long,double*,double*,double*);
void          glDriverPS_SetViewport         (int,int,int,int);
void          glDriverPS_ClearColorBuffer    (DColor);
void          glDriverPS_DrawPrimitive       (unsigned int,int,double*,double*,double*,glContext);
void          glDriverPS_DrawBuffer          (double,double,int,int,float,float,unsigned char*,glContext);
#ifdef __cplusplus
}
#endif


#endif  /*glDriverPS_h*/
