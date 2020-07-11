/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoHelp_h
#define XoHelp_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _XoHelpClassRec *XoHelpWidgetClass;
typedef struct _XoHelpRec      *XoHelpWidget;

#define XoHELP_CANCEL_BUTTON 1

#ifdef __cplusplus
extern "C"{
#endif
extern WidgetClass xoHelpWidgetClass;

void   XoHelpClearClass ();
void   XoHelpShowTopic  (Widget,char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoHelp(0); /**/
#endif

#endif /*XoHelp_h*/
