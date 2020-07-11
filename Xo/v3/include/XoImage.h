/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoImage_h
#define XoImage_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _XoImageClassRec *XoImageWidgetClass;
typedef struct _XoImageRec      *XoImageWidget;

/* XoImage */
#ifndef XoNvirtualColormap
#define XoNvirtualColormap   "virtualColormap"
#endif
#ifndef XoNcollectCallback
#define XoNcollectCallback   "collectCallback"
#endif
#ifndef XoNpostScriptFile
#define XoNpostScriptFile    "postScriptFile"
#endif
#ifndef XoNgifFile
#define XoNgifFile           "gifFile"
#endif
#ifndef XoNdataFile
#define XoNdataFile          "dataFile"
#endif
#ifndef XoNpixelPrecision
#define XoNpixelPrecision    "pixelPrecision"
#endif

#ifdef __cplusplus
extern "C"{
#endif
extern  WidgetClass          xoImageWidgetClass;

void    XoImageClearClass                   ();
void    XoImageDraw                         (Widget);
void    XoImageRestoreDefaultTransformation (Widget);
void    XoImageTranslate                    (Widget,int,int);
void    XoImageScale                        (Widget,double);
void    XoImageTranslateScreen              (Widget,int,int);
void    XoImageCenter                       (Widget,int,int);
void    XoImageMagnify                      (Widget,int,int,int,int);
XImage* XoImageGetPart                      (Widget,int,int,int,int);
void    XoImageGetCollectedRegion           (Widget,int*,int*,int*,int*);
void    XoImageGetLocalPosition             (Widget,int*,int*);
void    XoImageGetLocalBox                  (Widget,int,int,int,int,int*,int*,int*,int*);
char*   XoImageGetInformations              (Widget);
void    XoImagePutPixmapInPS_Stream         (Widget);
void    XoImageAddPopupEntry                (Widget,char*,XtCallbackProc,XtPointer);

void*   XoImageGetImage                     (Widget);
void*   XoImageGetColormap                  (Widget);
void*   XoImageGetLUT                       (Widget);
void    XoImageSetColormap                  (Widget,void*);
void    XoImageAttachImageColormapLUT       (Widget,void*,void*,void*);

#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoImage(0); /**/
#endif

#endif /*XoImage_h*/
