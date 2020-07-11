/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XSelection_h
#define XSelection_h

#ifndef NO_X_H
#include <X11/Xlib.h>
#endif

typedef void(*XSelectionPasteFunction)();

#ifdef __cplusplus
extern "C"{
#endif
void                    XSelectionClearClass          ();
void                    XSelectionRequireData         (Display*);
void                    XSelectionSendString          (Display*,char*);
void                    XSelectionSendFile            (Display*,char*);
int                     XSelectionIsNotificationEvent (XEvent*);
int                     XSelectionIsRequestEvent      (XEvent*);
int                     XSelectionIsEndOfCopyEvent    (XEvent*);
int                     XSelectionDispatchEvent       (XEvent*);
XSelectionPasteFunction XSelectionSetPasteFunction    (XSelectionPasteFunction);
XSelectionPasteFunction XSelectionGetPasteFunction    ();
char*                   XSelectionPutBufferInString   ();
void                    XSelectionPutBufferInFile     (char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXSelection(0); /**/
#endif

#endif  /*XSelection_h*/
