/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glDriver_h
#define glDriver_h

#include <gldef.h>
#include <glContext.h>

typedef          void(*glClearClassFunction)();
typedef unsigned long(*glRGB_ToPixelFunction)(double,double,double);
typedef           int(*glPixelToRGB_Function)(unsigned long,double*,double*,double*);
typedef          void(*glSetViewportFunction)(int,int,int,int);
typedef          void(*glClearColorBufferFunction)(DColor);
typedef          void(*glDrawPrimitiveFunction)(GLenum,int,double*,double*,double*,glContext);
typedef          void(*glDrawBufferFunction)(double,double,int,int,float,float,unsigned char*,glContext);

#ifdef __cplusplus
extern "C"{
#endif
void      glSetDriver  (glClearClassFunction,glRGB_ToPixelFunction,glPixelToRGB_Function,glSetViewportFunction,glClearColorBufferFunction,glDrawPrimitiveFunction,glDrawBufferFunction);
void      glClearClass ();
#ifdef __cplusplus
}
#endif

#endif  /*glDriver_h*/
