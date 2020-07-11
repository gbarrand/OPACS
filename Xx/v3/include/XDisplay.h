/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XDisplay_h
#define XDisplay_h

#ifndef NO_X_H
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#endif
 
typedef void(*XDisplayShapeFunction)(Display*,Pixmap,GC,char*);

typedef enum {
  XShapeFunction,
  XShapePixmap,
  XShapeRectangular,
  XShapeOval,
  XShapeDiamond,
  XShapeBallonArcRight,
  XShapeBallonArcLeft,
  XShapeBallonArcXY,
  XShapeBallonRectRight,
  XShapeBallonRectLeft,
  XShapeBallonRectXY
} XShape;

#ifndef NO_C_H
#include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
void           XDisplayClearClass                  ();
void           XDisplayDumpScreen                  (Display*);
int            XDisplayHasExtension                (Display*,char*);
void           XDisplayGetSize                     (Display*,unsigned int*,unsigned int*);
int            XDisplayGetDepth                    (Display*);
int            XDisplayGetBitsPerPixel             (Display*);
unsigned long  XDisplayGetWhitePixel               (Display*);
unsigned long  XDisplayGetBlackPixel               (Display*);
void           XDisplayRingBell                    (Display*,int);
void           XDisplaySynchronize                 (Display*);
GC             XDisplayGetRubberGC                 (Display*,Drawable);
void           XDisplayDrawRectangle               (Display*,Drawable,GC,XPoint*,XPoint*);

void           XDisplayGetPixelRGB                 (Display*,unsigned long,double*,double*,double*);
int            XDisplayGetPixelFromRGB             (Display*,double,double,double,unsigned long*,int*);
void           XDisplayFreePixels                  (Display*,int,unsigned long*);

void           XDisplayGetRGBsOfColorNames         (Display*,int*,double**,double**,double**);
char*          XDisplayConvertPixelToColorName     (Display*,unsigned long);
void           XDisplayConvertColorNameToRGB       (Display*,char*,double*,double*,double*);
char*          XDisplayConvertRGB_ToColorName      (Display*,double,double,double);

void           XDisplayGetDrawableSize             (Display*,Drawable,unsigned int*,unsigned int*);
unsigned int   XDisplayGetDrawableDepth            (Display*,Drawable);
Pixmap         XDisplayGetDrawablePixmap           (Display*,Drawable);

XImage*        XDisplayGetPixmapImage              (Display*,Pixmap);
void           XDisplayPutPixmapInPS_Stream        (Display*,Pixmap);
void           XDisplayPutPixmapInRootWindow       (Display*,Pixmap);
Pixmap         XDisplayConvertPixmapToBitmap       (Display*,Pixmap,unsigned long);
Pixmap         XDisplayLoadPixmapFile              (Display*,char*);

Pixmap         XDisplayGetWindowPixmap             (Display*,Window,int,int,int,int);
void           XDisplayIconifyWindow               (Display*,Window);
void           XDisplaySetWindowToNormalState      (Display*,Window);

void           XDisplaySetWindowShapeE             (Display*,Window,Window,XShape,...);
void           XDisplaySetWindowShapeV             (Display*,Window,Window,XShape,va_list);
int            XDisplaySendMessage                 (Display*,Window,long,long);

void           XDisplayPutFileInResourceDatabase   (Display*,char*);
void           XDisplayPutStringInResourceDatabase (Display*,char*);
void           XDisplayDumpResource                (Display*,char*,char*);

void           XDisplayGetImageColormap            (Display*,XImage*,int*,double**,double**,double**);
void           XDisplayPutImageInGIF_Stream        (Display*,XImage*,void*);
void           XDisplayPutImageInPS_Stream         (Display*,XImage*);
XImage*        XDisplayCompressImage               (Display*,XImage*,int);
XImage*        XDisplayExpandImage                 (Display*,XImage*,int);
XImage*        XDisplayAdaptImageToScreen          (Display*,XImage*);

int            XImageDelete                        (XImage*);
void           XImageGetSize                       (XImage*,unsigned int*,unsigned int*);
char*          XImageGetBytes                      (XImage*);
int            XImageDestroy                       (XImage*);
unsigned long  XImageGetPixel                      (XImage*,int,int);
void           XImagePutPixel                      (XImage*,int,int,unsigned long);
void           XImageAddPixel                      (XImage*,long);
int            XImageConvolute                     (XImage*,int,unsigned long*);
void           XImagePutBitmapInXBM_Stream         (XImage*,void*,char*,unsigned long);

void           XColorGetRGB                        (XColor*,double*,double*,double*);
unsigned long  XStringGetMask                      (char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXDisplay(0); /**/
#endif

#endif  /*XDisplay_h*/
