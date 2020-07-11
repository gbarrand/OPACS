/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoShape_h
#define XoShape_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _XoShapeClassRec *XoShapeWidgetClass;
typedef struct _XoShapeRec      *XoShapeWidget;

#ifndef XoNexposeCallback
#define XoNexposeCallback   "exposeCallback"
#endif

#ifndef XoNresizeCallback
#define XoNresizeCallback   "resizeCallback"
#endif

#ifndef XoNuserCallback
#define XoNuserCallback     "userCallback"
#endif

#ifndef XoNwindowShape
#define XoNwindowShape      "windowShape"
#endif

#ifndef XoRWindowShape
#define XoRWindowShape      "WindowShape"
#endif

#ifdef __cplusplus
extern "C"{
#endif
extern WidgetClass xoShapeWidgetClass;
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoShape(0); /**/
#endif

#endif /*XoShape_h*/
