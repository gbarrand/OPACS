/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
#define DEBUG
#include <stdio.h>
*/

#include <X11/Intrinsic.h>

#include <CPrinter.h>
#include <CMemory.h>
#include <CString.h>
#include <CPrinter.h>

#include <XWidget.h>

#include <OEventP.h>

typedef struct _MailboxRecord  *Mailbox;

typedef struct _MailboxRecord
{
  Display* display;
  Window   window;
} MailboxRecord;

#ifdef __cplusplus
extern "C"{
#endif
static Mailbox MailboxGetIdentifierFromDisplay (Display*);
#ifdef __cplusplus
}
#endif

static struct 
{
  int            boxen;
  MailboxRecord* boxes;
} Class = {0,NULL};
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OEventClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
/*.........................................................................*/
  for(count=0;count<Class.boxen;count++)
    {
      Mailbox      mailbox;
      XtAppContext appContext;
      mailbox      = &(Class.boxes[count]);
      appContext   =  XtDisplayToApplicationContext(mailbox->display);
      while(1)
	{ XEvent  xevent;
	  if(XtAppPending(appContext)==0) break;
	  XtAppNextEvent  (appContext,&xevent);  /*wait X events*/
	  if(OEventIsPrivateEvent(&xevent)==1)
	    {
	      OEventDelete  ((OEvent)(xevent.xclient.data.l[1]));             
	    }
	}
      XDestroyWindow   (mailbox->display,mailbox->window);
      mailbox->display = NULL;
      mailbox->window  = 0L;
    }
  CMemoryFreeBlock (Class.boxes);
  Class.boxes      = NULL;
  Class.boxen      = 0;
}
/***************************************************************************/
int OEventDispatchGrab (
 XEvent* a_event
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OEvent     event;
/*.........................................................................*/
  if(a_event==NULL)                    return 0;
  if(OEventIsPrivateEvent(a_event)==0) return 0;
  event      = (OEvent)(a_event->xclient.data.l[1]);
  OEventSend (event,event->display);
  return     1;
}
/***************************************************************************/
int OEventIsPrivateEvent (
 XEvent* a_event
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_event==NULL)                                   return 0;
  if(a_event->xany.type!=ClientMessage)               return 0;
  if(a_event->xclient.data.l[0]==OEventNone)          return 1;
  if(a_event->xclient.data.l[0]==OEventExit)          return 1;
  if(a_event->xclient.data.l[0]==OEventExecuteScript) return 1;
  return 0;
}
/***************************************************************************/
int OEventIsExit (
 XEvent* a_event
,int* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_value!=NULL) *a_value = 0;
  if(a_event==NULL)                                   return 0;
  if(a_event->xany.type!=ClientMessage)               return 0;
  if(a_event->xclient.data.l[0]!=OEventExit)          return 0;
  if(a_value!=NULL) *a_value = (int)(a_event->xclient.data.l[1]);
  return 1;
}
/***************************************************************************/
OEvent OEventMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OEvent This;
/*.........................................................................*/
  This             = (OEventRecord*)CMemoryAllocateBlock(sizeof(OEventRecord));
  if(This==NULL)   return NULL;
  This->type       = OEventNone;
  This->display    = NULL;
  This->widget     = NULL;
  This->string     = NULL;
  This->value      = NULL;
  This->event      = NULL;
  return           This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OEventDelete (
 OEvent This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete     (This->string);
  CStringDelete     (This->value);
  CMemoryFreeBlock  (This->event);
  This->string      = NULL;
  This->value       = NULL;
  This->event       = NULL;
  This->widget      = NULL;
  This->display     = NULL;
  CMemoryFreeBlock  (This);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OEventSetWidget (
 OEvent This 
,Widget a_widget 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->widget = a_widget;
}
/***************************************************************************/
Widget OEventGetWidget (
 OEvent This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->widget;
}
/***************************************************************************/
void OEventSetType (
 OEvent This 
,int a_type  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->type = a_type;
}
/***************************************************************************/
int OEventGetType (
 OEvent This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OEventNone;
  return This->type;
}
/***************************************************************************/
void OEventSetString (
 OEvent This 
,char* a_string  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete  (This->string);
  This->string   = CStringDuplicate (a_string);
}
/***************************************************************************/
char* OEventGetString (
 OEvent This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->string;
}
/***************************************************************************/
void OEventSetValue (
 OEvent This 
,char* a_string  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete  (This->value);
  This->value    = CStringDuplicate (a_string);
}
/***************************************************************************/
char* OEventGetValue (
 OEvent This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->value;
}
/***************************************************************************/
void OEventSetEvent (
 OEvent This 
,XEvent* a_event
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  CMemoryFreeBlock  (This->event);
  This->event       = NULL;
  if(a_event==NULL) return;
  This->event       = (XEvent*)CMemoryAllocateBlock(sizeof(XEvent));
  if(This->event==NULL) return;
  *(This->event)    = *a_event;
}
/***************************************************************************/
XEvent* OEventGetEvent (
 OEvent This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->event;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OEventSend (
 OEvent This 
,Display* a_display
)
/***************************************************************************/
/* 
  "This" must not be free after OEventSend. 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Mailbox mailbox;
/*.........................................................................*/
  if(This==NULL) return;

  mailbox = MailboxGetIdentifierFromDisplay (a_display);
  if(mailbox==0)
    {
      CWarn("OEventSend : can't create mailbox.\n");
      return;
    }

  This->display       = a_display;
  XDisplaySendMessage (mailbox->display,mailbox->window,(long)This->type,(long)This);
}
/***************************************************************************/
void OEventSendExit (
 Display* a_display
,int a_retval 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OEvent          event;
/*.........................................................................*/
  event           = OEventMake();
  if(event==NULL) return;
  event->type     = OEventExit;
  event->string   = CStringCreateF (64,"%d",a_retval);
  OEventSend      (event,a_display);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static Mailbox MailboxGetIdentifierFromDisplay (
 Display* a_display
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int                  count;
  XSetWindowAttributes swa;
  Window               window;
/*.........................................................................*/
  if(a_display==NULL) return NULL;

  for(count=0;count<Class.boxen;count++)
    {
      if(Class.boxes[count].display==a_display) return &(Class.boxes[count]);
    }

  /*Create mailbox.*/
  swa.event_mask       = StructureNotifyMask | ExposureMask;
  window               = XCreateWindow(a_display, 
				       XDefaultRootWindow(a_display),
				       0,0,10,10,
				       0,
				       XDefaultDepth(a_display,XDefaultScreen(a_display)),
				       InputOutput,
				       XDefaultVisual(a_display,XDefaultScreen(a_display)),
				       CWEventMask,&swa);
  if(window==0L) return NULL;

  Class.boxes = Class.boxen==0 ? 
               (MailboxRecord*)CMemoryAllocateBlock(sizeof(MailboxRecord)) :
               (MailboxRecord*)CMemoryChangeBlockSize((char*)(Class.boxes),(Class.boxen+1)*sizeof(MailboxRecord));
  if(Class.boxes==NULL) 
    {
      XDestroyWindow (a_display,window);
      Class.boxen    = 0;
      return         NULL;
    }

#ifdef DEBUG
  printf ("debug : Mailbox created.\n");
#endif

  Class.boxes[Class.boxen].display = a_display;
  Class.boxes[Class.boxen].window  = window;
  Class.boxen++;

  return &(Class.boxes[Class.boxen-1]);
}
