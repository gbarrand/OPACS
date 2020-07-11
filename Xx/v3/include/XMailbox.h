/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XMailbox_h
#define XMailbox_h

#ifndef NO_X_H
#include <X11/Xlib.h>
#endif

typedef Window XMailbox;
 
#ifdef __cplusplus
extern "C"{
#endif
void     XMailboxClearClass    ();
Display* XMailboxGetDisplay    ();
XMailbox XMailboxCreate        (char*,Display*);
XMailbox XMailboxGetIdentifier (char*,Display*);
char*    XMailboxWaitMessage   ();
char*    XMailboxGetMessage    (XEvent*,int*);
int      XMailboxSendMessage   (XMailbox,char*,int);
int      XMailboxSendString    (XMailbox,char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXMailbox(0); /**/
#endif

#endif  /*XMailbox_h*/
