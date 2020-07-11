/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glx_h
#define glx_h

#include <GL/gl.h>

#ifndef NO_X_H
#include <X11/Xlib.h>     /*no_cigen*/
#include <X11/Xutil.h>    /*no_cigen*/
#endif
 
typedef struct _GLXContextRecord *GLXContext;
typedef unsigned long          GLXDrawable;

/*
** Names for attributes to glXGetConfig.
*/
#define GLX_USE_GL              1       /* support GLX rendering */
#define GLX_RGBA                4       /* true if RGBA mode */
#define GLX_DOUBLEBUFFER        5       /* double buffering supported */
#define GLX_RED_SIZE		8
#define GLX_GREEN_SIZE		9
#define GLX_BLUE_SIZE		10
#define GLX_ALPHA_SIZE		11
#define GLX_DEPTH_SIZE		12

#ifdef __cplusplus
extern "C"{
#endif
Bool         glXQueryExtension         (Display*,int*,int*);
Bool         glXQueryVersion           (Display*,int*,int*);
XVisualInfo* glXChooseVisual           (Display*,int,int*);
GLXContext   glXCreateContext          (Display*,XVisualInfo*,GLXContext,Bool);
void         glXDestroyContext         (Display*,GLXContext);
Bool         glXMakeCurrent            (Display*,GLXDrawable,GLXContext);
void         glXSwapBuffers            (Display*,GLXDrawable);
void         glXWaitX                  ();
void         glXWaitGL                 ();
Bool         glXIsDirect               (Display*,GLXContext);
void         glXUseXFont               (Font,int,int,int);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindglx(0); /**/
#endif

#endif  /*glx_h*/
