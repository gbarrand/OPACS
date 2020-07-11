/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoColormapP_h
#define XoColormapP_h 

#include <XoColormap.h>

typedef struct
{
   void* extension;
} XoColormapClassPart;

typedef struct _XoColormapClassRec
{
	CoreClassPart		core_class;
	XoColormapClassPart	cmap_class;
} XoColormapClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XoColormapClassRec xoColormapClassRec;
#ifdef __cplusplus
}
#endif


typedef struct
{
        char*          virtualColormap;
        char*          valueColorMapping;
	char*          selectedColor;
        Font           titleFont;
        XtCallbackList selectCallback;
} XoColormapPart;

typedef struct _XoColormapRec
{
	CorePart		core;
  	XoColormapPart		cmap;
} XoColormapRec;

#endif /*XoColormapP_h*/


