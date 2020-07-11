/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

/*Co*/
#include <CPrinter.h>
#include <CMemory.h>
#include <OType.h>

/*Xx*/
#include <XDisplay.h>
#include <XWidget.h>

/*Xo*/
#include <XoColormap.h>

#ifdef __cplusplus
extern "C"{
#endif
static void SelectCallback (Widget, XtPointer, XtPointer);
static void CloseWindow  (Widget,XEvent*,char**,Cardinal*);
#ifdef __cplusplus
}
#endif

static XtAppContext appContext = NULL;
Widget wcmap;
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget top,shell;
  Arg    args[1];
/*.........................................................................*/
 {
#if XtSpecificationRelease == 4
  Cardinal argn;
  argn     = (Cardinal)a_argn;
#else
  int      argn;
  argn     = a_argn;
#endif
  top      = XtAppInitialize(&appContext,"XoColormapT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  XDisplayPutFileInResourceDatabase    (XtDisplay(top),"XoT.xrm");

/*Add WMClose does not works on ApplicationShell.*/
  shell                  = XtAppCreateShell ("xoShellT","XoColormapT",topLevelShellWidgetClass,XtDisplay(top),args,0); 

  wcmap                  = XtCreateManagedWidget("xoColormapT",xoColormapWidgetClass ,shell, args, 0);

  XtAddCallback          (wcmap, XoNselectCallback,SelectCallback, NULL);

  XtRealizeWidget        (shell);
  XtMapWidget            (shell);

  XWidgetSetWindowManagerCloseFunction  (shell,CloseWindow);  /*Must be realized.*/

  XtAppMainLoop          (appContext);
  return                 EXIT_SUCCESS;
}
/***************************************************************************/
static void SelectCallback (
 Widget    w
,XtPointer client_data
,XtPointer call_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg   args[1];
  char* value;
/*.........................................................................*/
  XtSetArg    (args[0], XoNselectedColor, &value);
  XtGetValues (w,args,1);
  CInfoF      ("color:%s\n",value);

  client_data = NULL;
  call_data   = NULL;
}
/***************************************************************************/
static void CloseWindow (
 Widget This 
,XEvent* a_event 
,char** a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWidgetVisitFunction proc;
/*.........................................................................*/
  if(This==NULL)                    return;
  if(!a_event)                      return;
  if (a_event->type!=ClientMessage) return;

/*XtDestroyWidget (shell); Do not execute XoCamera.destroy method.*/
  proc            = XClassGetDestroyFunction (xoColormapWidgetClass);
  if(proc!=NULL)  proc (wcmap);

  XoColormapClearClass         ();
  XtDestroyApplicationContext  (appContext);

  OTypeClearClass              ();

  exit                         (EXIT_SUCCESS);
  a_argn  = 0;
  a_args  = NULL; 
}

