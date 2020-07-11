/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OEvent_h
#define OEvent_h
 
#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _OEventRecord   *OEvent;

#define OEventNone          111
#define OEventExit          222
#define OEventExecuteScript 333

#ifdef __cplusplus
extern "C"{
#endif
void    OEventClearClass     ();
int     OEventDispatchGrab   (XEvent*);
int     OEventIsPrivateEvent (XEvent*);
OEvent  OEventMake           ();
void    OEventDelete         (OEvent);
void    OEventSetWidget      (OEvent,Widget);
Widget  OEventGetWidget      (OEvent);
void    OEventSetType        (OEvent,int);
int     OEventGetType        (OEvent);
void    OEventSetString      (OEvent,char*);
char*   OEventGetString      (OEvent);
void    OEventSetValue       (OEvent,char*);
char*   OEventGetValue       (OEvent);
void    OEventSetEvent       (OEvent,XEvent*);
XEvent* OEventGetEvent       (OEvent);
void    OEventSend           (OEvent,Display*);
void    OEventSendExit       (Display*,int);
int     OEventIsExit         (XEvent*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOEvent(0);  /**/
#endif

#endif  /*OEvent_h*/


