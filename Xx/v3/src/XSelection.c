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
#include <CString.h>
#include <CStream.h>
#include <CFile.h>
#include <CError.h>

#include <XSelection.h>

#ifdef __cplusplus
extern "C"{
#endif
static void InitializeClass (Display*);
static void ReportError     (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned char        Uchar;
typedef unsigned long        Ulong;

#define NAME__OA_SELECT "_XSelect"

static struct 
{
  int          init;
  Display*     display;
  Window       window;
  Atom         atom;
  XSelectionPasteFunction paste_proc;
  Uchar*       paste_buffer;
  Ulong        paste_size;
  Window       copy_requestor;
  Uchar*       copy_buffer;
  Ulong        copy_size;
} Class = {0,NULL,0L,0L,NULL,NULL,0L,0L,NULL,0L};
/***************************************************************************/
static void InitializeClass (
 Display* a_display 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XSetWindowAttributes argwdw;
/*.........................................................................*/
  if(Class.init==1) return; /* already done */

  Class.display = a_display;

  Class.atom    = XInternAtom (Class.display,NAME__OA_SELECT , False);
  if (Class.atom == None) ReportError("InitializeClass",CErrorValue);

/* window for selection */
  argwdw.event_mask  = PropertyChangeMask;
  Class.window       = XCreateWindow (Class.display,
                                      XDefaultRootWindow (Class.display),
                                      0, 0, 1, 1, 0,
                                      (int)CopyFromParent,InputOutput,(Visual *)CopyFromParent,0L,&argwdw);

  Class.init         = 1;
}
/***************************************************************************/
void XSelectionClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==0) return;

  Class.paste_proc      = NULL;

  if(Class.paste_buffer!=NULL) XFree (Class.paste_buffer);
  Class.paste_buffer    = NULL;
  Class.paste_size      = 0L;

  CMemoryFreeBlock               (Class.copy_buffer);
  Class.copy_buffer     = NULL;
  Class.copy_size       = 0L;

  Class.display         = NULL;

  Class.init = 0;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XSelectionRequireData (
 Display* a_disp 
)
/***************************************************************************/
/* a_string is the script to exec when receiving SelectionNotify event     */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_disp==NULL)    return;
  InitializeClass     (a_disp);
  XConvertSelection   (Class.display,XA_PRIMARY,XA_STRING,Class.atom,Class.window,CurrentTime);
}
/***************************************************************************/
void XSelectionSendString (
 Display* a_disp 
,char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int                length;
/*.........................................................................*/
  if(a_disp==NULL)  return;
  InitializeClass         (a_disp);

  if(a_string==NULL)       return;

  length              = strlen(a_string)+1;

  CMemoryFreeBlock            (Class.copy_buffer);
  Class.copy_size             = 0L;
  Class.copy_buffer           = (Uchar*)CMemoryAllocateBlock(length*sizeof(Uchar));
  if(Class.copy_buffer==NULL) return;
  Class.copy_buffer[length-1] = '\0';
  Class.copy_size             = length;

  strcpy             ((char *)Class.copy_buffer,a_string);
  XSetSelectionOwner (Class.display,XA_PRIMARY,Class.window,CurrentTime);
}
/***************************************************************************/
void XSelectionSendFile (
 Display* a_disp 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE* file;
/*.........................................................................*/
  if(a_disp==NULL)    return;
  file                = CFileOpenForReading (a_name,NULL);
  if(file==NULL)      return;
  InitializeClass     (a_disp);
  CMemoryFreeBlock    (Class.copy_buffer);
  Class.copy_buffer   = (Uchar*)CStreamRead (file,&Class.copy_size);
  fclose              (file);
  XSetSelectionOwner  (Class.display,XA_PRIMARY,Class.window,CurrentTime);
}
/***************************************************************************/
int XSelectionDispatchEvent (
 XEvent*  a_event
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_event==NULL) return 0;
  if ( (a_event->xany.type==SelectionNotify) && XSelectionIsNotificationEvent(a_event) ) /*Else could be a Motif cut/paste.*/
    { 
      if(Class.paste_proc!=NULL) Class.paste_proc();
      return 1;
    }
  else if ( (a_event->xany.type==SelectionRequest) && XSelectionIsRequestEvent(a_event) ) /*Else could be a Motif cut/paste.*/
    { 
      return 1;
    }
  else
    {
      return 0;
    }
}
/***************************************************************************/
/**** Routine in main loop to survey selection *****************************/
/***************************************************************************/
/***************************************************************************/
int XSelectionIsNotificationEvent (
 XEvent* a_event  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XSelectionEvent *select_event;
  Atom     type;
  int      format;
  Ulong    number;
  Ulong    remaining;
  Uchar*   buff;
/*.........................................................................*/
  if(a_event==NULL) return 0;
  select_event    = (XSelectionEvent *)a_event;
  InitializeClass (select_event->display); /* to have Class.atom */

  if(Class.paste_buffer!=NULL) XFree (Class.paste_buffer);
  Class.paste_buffer    = NULL;
  Class.paste_size      = 0L;

  if(select_event->property!=Class.atom) return 0;

  buff   = NULL;
  XGetWindowProperty (select_event->display,
		      select_event->requestor,
		      select_event->property,
		      0L,
		      0x10000,
		      True,
		      select_event->target,
		      &type,
		      &format,
		      &number,
		      &remaining,
		      &buff);

  Class.paste_buffer = buff;
  Class.paste_size   = number;
  return             1;
}
/***************************************************************************/
int XSelectionIsRequestEvent (
 XEvent* a_event  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XEvent  select_event;
  Display *xdisp;
/*.........................................................................*/
  if(a_event==NULL) return 0;
  if( (Class.copy_buffer==NULL) || (Class.copy_size==0) ) return 0;  /*Nothing to send.*/

  select_event.type                 = SelectionNotify; 
  select_event.xselection.requestor = a_event->xselectionrequest.requestor;
  select_event.xselection.selection = a_event->xselectionrequest.selection;
  select_event.xselection.target    = a_event->xselectionrequest.target;
  select_event.xselection.time      = a_event->xselectionrequest.time;
  select_event.xselection.property  = a_event->xselectionrequest.property;

  Class.copy_requestor = a_event->xselectionrequest.requestor;
  xdisp                = a_event->xselectionrequest.display;
  if( (select_event.xselection.selection==XA_PRIMARY) &&
      (select_event.xselection.target   ==XA_STRING)
    )
    {
      XChangeProperty (xdisp,
                       select_event.xselection.requestor,
                       select_event.xselection.property,
                       select_event.xselection.target,
                       8,PropModeReplace,Class.copy_buffer,(int)Class.copy_size);
    }
  else
    {
      select_event.xselection.property = None;
    }

  XSelectInput (xdisp,a_event->xselectionrequest.requestor,PropertyChangeMask);

  XSendEvent   (xdisp,select_event.xselection.requestor,False,0L,&select_event);
  XFlush       (xdisp);
  return       1;
}
/***************************************************************************/
int XSelectionIsEndOfCopyEvent (
 XEvent* a_event  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_event==NULL) return 0;
  if(((XPropertyEvent *)a_event)->window!=Class.copy_requestor) return 0;

/* free stuff */
  CMemoryFreeBlock               (Class.copy_buffer);
  Class.copy_buffer     = NULL;
  Class.copy_size       = 0L;

  XSelectInput           (((XPropertyEvent *)a_event)->display,Class.copy_requestor,PropertyChangeMask);
  Class.copy_requestor  = 0L;
  return                1;
}
/***************************************************************************/
/**** Routine to retreive infos ********************************************/
/***************************************************************************/
/***************************************************************************/
XSelectionPasteFunction XSelectionSetPasteFunction (
 XSelectionPasteFunction a_proc
)
/***************************************************************************/
/* a_string is the script to exec when receiving SelectionNotify event     */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XSelectionPasteFunction old;
/*.........................................................................*/
  old                 = Class.paste_proc;
  Class.paste_proc    = a_proc;
  return              old;
}
/***************************************************************************/
XSelectionPasteFunction XSelectionGetPasteFunction (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.paste_proc;
}
/***************************************************************************/
char* XSelectionPutBufferInString (
)
/***************************************************************************/
/* write selection in a string                                             */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*    string;
  Uchar*   buff;
  int      length;
/*.........................................................................*/
  buff        = Class.paste_buffer;
  if(buff==NULL)   return NULL;
  length      = (int)Class.paste_size;
  if(length<=0)  return NULL;
/* check if buffer contain a null terminated string */
  if(buff[length-1]==0)
    {
      string           = (char *)CMemoryAllocateBlock( length * sizeof(char));
      if(string==NULL) return NULL;
      string[length-1] = '\0';
      strncpy          (string,(char *)buff,(size_t)(length-1));
      string[length-1] = '\0';
    }
  else 
    {
      string           = (char *)CMemoryAllocateBlock( (length+1) * sizeof(char));
      if(string==NULL) return NULL;
      string[length]   = '\0';
      strncpy          (string,(char *)buff,(size_t)length);
      string[length]   = '\0';
    }
  return   string;
}
/***************************************************************************/
void XSelectionPutBufferInFile (
 char* a_name
)
/***************************************************************************/
/* write selection in a a_file                                             */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*   file;
  Uchar*  buff;
  size_t  length;
/*.........................................................................*/
  if(a_name==NULL) return;
  file      = CFileOpenForWriting (a_name);
  if(file==NULL) return;
  buff      = Class.paste_buffer;
  if(buff==NULL) return;
  length    = (size_t)Class.paste_size;
  if(length<=0) return;
  if(fwrite(buff,length,1,file)!=1) ReportError("XSelectionPutBufferInFile",CErrorIO);
  fclose    (file);
}
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Xx","XSelection",a_name,a_code);
}
