/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoMosaic_h
#define XoMosaic_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _XoMosaicClassRec *XoMosaicWidgetClass;
typedef struct _XoMosaicRec      *XoMosaicWidget;

#ifndef XoNhomeDocument
#define XoNhomeDocument    "homeDocument"
#endif
#ifndef XoNcontrolsVisible
#define XoNcontrolsVisible "controlsVisible"
#endif
#ifndef XoNcurrentURL
#define XoNcurrentURL      "currentURL"
#endif
#ifndef XoNmenuFont
#define XoNmenuFont       "menuFont"
#endif

#ifdef __cplusplus
extern "C"{
#endif
extern WidgetClass xoMosaicWidgetClass;

void XoMosaicLoadWindowText (Widget,char*);
void XoMosaicShowTopic      (Widget,char*);
void XoMosaicAddPopupEntry  (Widget,char*,XtCallbackProc,XtPointer);
void XoMosaicSave           (Widget,char*,char*,char*);

int    mo_not_busy    ();
int    mo_busy        ();
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoMosaic(0); /**/
#endif

#endif /*XoMosaic_h*/
