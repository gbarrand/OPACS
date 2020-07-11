/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OColormapX_h
#define OColormapX_h

#ifndef NO_X_H
#include <X11/Xlib.h>
#endif
 
#ifdef __cplusplus
extern "C"{
#endif
void           OColormapXClearClass    ();
unsigned long  OColormapXGetPixel      (void*,int,Display*);
unsigned long* OColormapXGetPixels     (void*,Display*,int,double);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOColormapX(0); /**/
#endif

#endif  /*OColormapX_h*/
