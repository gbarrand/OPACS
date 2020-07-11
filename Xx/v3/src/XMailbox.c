/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <string.h>

#include <X11/Xatom.h>

#include <CMemory.h>
#include <CPrinter.h>

#include <XMailbox.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    InitializeClass (Display*);
static int     SendMessage     (Display*,Window,char*,int);
static int     IsProperty      (Display*,Window,Atom);
static Window  GetAddress      (Display*,Window,char*);
#ifdef __cplusplus
}
#endif

#define NAME__OA_MBX "_XMbx"

static struct 
{
  int        init;
  Display*   display;
  Atom       atom;
} Class = {0,NULL,0L};
/***************************************************************************/
static void InitializeClass (
 Display* a_disp 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==1) return; /* already done */

  Class.display = a_disp;

  Class.atom = XInternAtom (Class.display,NAME__OA_MBX, False);

  Class.init     = 1;
}
/***************************************************************************/
void XMailboxClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==0) return;

  Class.display = NULL;

  Class.init = 0;
}
/***************************************************************************/
Display* XMailboxGetDisplay (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return  Class.display;
}
/***************************************************************************/
XMailbox XMailboxCreate (
 char* a_name     
,Display* a_disp 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Window               window;
  XSetWindowAttributes argwdw;
/*.........................................................................*/
  if(a_disp==NULL)  return 0L;
  InitializeClass      (a_disp);

  if(a_name==NULL)  return 0L;

  argwdw.event_mask = PropertyChangeMask|ExposureMask;
  
  window = XCreateWindow (Class.display,
                          XDefaultRootWindow (Class.display),
                          0, 0,
                          1, 1,
                          0,
                          (int)CopyFromParent, 
                          InputOutput,
                          (Visual *)CopyFromParent,
                          CWEventMask,
                          &argwdw);


                            
  XStoreName (Class.display, window, a_name);
  return     (XMailbox)window;
}  
/***************************************************************************/
XMailbox XMailboxGetIdentifier (
 char* a_name 
,Display* a_disp 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_disp==NULL)  return 0L;
  InitializeClass   (a_disp);
  return GetAddress (Class.display,(Window)XDefaultRootWindow (Class.display),a_name);
}
/***************************************************************************/
char* XMailboxWaitMessage (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XEvent  xevent;
  int     length;
  char*   message;
  char*   string;
/*.........................................................................*/
  if(Class.display==NULL)  return NULL;
  while(1)
    {
      XNextEvent  (Class.display,&xevent);  
      if (xevent.xany.type==PropertyNotify) 
        {
          message = XMailboxGetMessage (&xevent,&length);
          if(message!=NULL) 
            {  /*get an external message:transform it to C string */
              string             = (char *)CMemoryAllocateBlock((length+1)*sizeof(char));
              if(string!=NULL) 
		{
		  string[length]     = '\0';
		  strncpy            (string,message,(size_t)length);
		  string[length]     = '\0';
		}
              XFree              (message);
              return             string;
            }
        }
   }
}
/***************************************************************************/
char* XMailboxGetMessage (
 XEvent* a_event  
,int* a_length 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XPropertyEvent *prop_event;
  char* name;
  Atom type;
  int format;
  unsigned long number,remaining;
  unsigned char *buff;
/*.........................................................................*/
  if(a_length!=NULL) *a_length = 0;
  if(a_event==NULL) return NULL;
  if(Class.atom==0L)  return NULL;
  prop_event = (XPropertyEvent *)a_event;
  name   = NULL;
  name   = (char*) XGetAtomName (prop_event->display,Class.atom);
  if( (name==NULL) || (strcmp(name,NAME__OA_MBX)!=0) ) return NULL;
  XFree  (name);
  buff   = NULL;
  XGetWindowProperty (prop_event->display,
		      prop_event->window,
		      Class.atom,
		      0L,
		      0x10000,
		      True,
		      AnyPropertyType,
		      &type,
		      &format,
		      &number,
		      &remaining,
		      &buff);
  if (type==0L) return NULL;
  
  if(a_length!=NULL) *a_length = (int)number;
  return    (char*)buff;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int XMailboxSendMessage (
 XMailbox This 
,char* a_buff 
,int a_length 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return SendMessage (Class.display,This,a_buff,a_length);
}
/***************************************************************************/
int XMailboxSendString (
 XMailbox This 
,char* a_string 
)
/***************************************************************************/
/* do not send ending null character */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_string==NULL) return 0;
  return XMailboxSendMessage (This,a_string,strlen(a_string));
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int SendMessage (
 Display* a_display 
,Window a_window  
,char* a_buff 
,int a_length 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_display==NULL)   return 0;
  if(a_window==0L)    return 0;
  if(a_buff==NULL)      return 0;
  if(Class.atom==0L)  return 0;

  while (IsProperty(a_display,a_window,Class.atom)==1);

  XChangeProperty (a_display,(Window)a_window,Class.atom,XA_INTEGER,8,
/*
                   PropModeAppend,
*/
                   PropModeReplace,
                   (unsigned char *)a_buff,
                   a_length);
  XFlush (a_display);
  return 1;
}
/***************************************************************************/
static int IsProperty (
 Display*    a_display
,Window     a_window
,Atom         a_mbx_atom
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  Atom *a;
  int number;
/*.........................................................................*/
  a = (Atom *) XListProperties (a_display,(Window)a_window, &number);
  for(count=0;count<number;count++)
  {
    if (a[count] == a_mbx_atom) return 1;
  }
  return 0;
}
/***************************************************************************/
static Window GetAddress (
 Display* a_display 
,Window a_window
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
  Status           status;
  Window           root;
  Window           parent;
  Window*          children;
  Window           window;

  unsigned int     number;
  char*            name;
/*.........................................................................*/
  if(a_name==NULL) return 0L;
  children = NULL;
  status   = XQueryTree (a_display,(Window)a_window,&root,&parent,&children,&number);
  if(status==0) return 0L;

/*printf("debug:%d %d\n",root,parent);*/

/* look if in children list */
  for(count=0;count<number;count++)
  {
    window = 0L;  
    name   = NULL;  
    status = XFetchName (a_display, children[count], &name);
    if ( (status!=0) && (name!=NULL) && (strcmp(name,a_name)==0) ) 
      {
        window = children[count];
        XFree  (name);
        XFree  ((char *)children);
        return (Window) window;
      }
    if(name!=NULL) XFree(name);
  }
/* try children subtree */
  for(count=0;count<number;count++)
  {
    window = (Window)GetAddress (a_display,(Window) children[count], a_name);
    if(window!=0L)   
    {
      XFree  ((char *)children);
      return (Window) window;
    }
  }
  XFree  ((char *)children);
  return 0L;
}
