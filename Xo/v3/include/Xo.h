/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef Xo_h
#define Xo_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
void   XoInitializeClass               (Display*);
void   XoClearClass                    ();
Widget XoCreateFrame                   (Widget,String,ArgList,Cardinal);
Widget XoCreateRule                    (Widget,String,ArgList,Cardinal);
Widget XoCreatePlotter                 (Widget,char*,ArgList,Cardinal);
Widget XoCreateGraph                   (Widget,char*,ArgList,Cardinal);
Widget XoCreateColormapDialog          (Widget,char*,ArgList,Cardinal);
void   XoPlotCurve                     (Widget,int,double*,double*);
void   XoBuildGridOfPlotters           (Widget,char*,int,int);
void   XoAttachObjectsToGridOfPlotters (Widget,int,char**);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXo(0); /**/
#endif

#endif  /*Xo_h*/
