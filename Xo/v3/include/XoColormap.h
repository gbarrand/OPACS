/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoColormap_h
#define XoColormap_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif


typedef struct _XoColormapClassRec *XoColormapWidgetClass;
typedef struct _XoColormapRec      *XoColormapWidget;

typedef struct _XoColormapCallbackStruct
{
  unsigned long reason;
  XEvent*       event;
  char*         selectedColor;
} XoColormapCallbackStruct;

#ifndef XoNselectCallback
#define XoNselectCallback     "selectCallback"
#endif
#ifndef XoNvirtualColormap
#define XoNvirtualColormap    "virtualColormap"
#endif
#ifndef XoNselectedColor
#define XoNselectedColor      "selectedColor"
#endif
#ifndef XoNtitleFont
#define XoNtitleFont          "titleFont"
#endif
#ifndef XoNvalueColorMapping
#define XoNvalueColorMapping  "valueColorMapping"
#endif

#ifdef __cplusplus
extern "C"{
#endif
extern WidgetClass xoColormapWidgetClass;

void   XoColormapClearClass ();
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoColormap(0); /**/
#endif

#endif /*XoColormap_h*/
