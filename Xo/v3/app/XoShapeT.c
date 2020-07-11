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

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#ifdef HAS_XM
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#endif /*HAS_XM*/

#include <CPrinter.h>
#include <CFile.h>
#include <CMemory.h>

#include <XWidget.h>

#include <XoShape.h>

#ifdef __cplusplus
extern "C"{
#endif
static void CloseWindow  (Widget,XEvent*,char**,Cardinal*);
#ifdef __cplusplus
}
#endif


static XtAppContext appContext = NULL;
/***************************************************************************/
int main (
 int a_argn
,String a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget top,shell,wdraw;
  Arg    args[2];
/*.........................................................................*/
 {
#if XtSpecificationRelease == 4
  Cardinal argn;
  argn     = (Cardinal)a_argn;
#else
  int      argn;
  argn     = a_argn;
#endif
  top      = XtAppInitialize(&appContext,"XoShapeT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  XDisplayPutFileInResourceDatabase    (XtDisplay(top),"XoT.xrm");

/*Add WMClose does not works on ApplicationShell.*/
  shell              = XtAppCreateShell ("xoShellT","XoShapeT",topLevelShellWidgetClass,XtDisplay(top),args,0); 

  wdraw              = XtCreateManagedWidget("xoShapeT",xoShapeWidgetClass , shell , args, 0);

#ifdef HAS_XM
 {Widget          wdraw_button_1,wdraw_button_2;
  wdraw_button_1  = XmCreatePushButton    (wdraw,"wdraw_button_1"    ,args,0);
  wdraw_button_2  = XmCreatePushButton    (wdraw,"wdraw_button_2"    ,args,0);
  XtManageChild   (wdraw_button_1);
  XtManageChild   (wdraw_button_2);

  XtSetArg        (args[0],XmNleftAttachment,XmATTACH_WIDGET);
  XtSetArg        (args[1],XmNleftWidget,wdraw_button_1);
  XtSetValues     (wdraw_button_2,args,2);}
#endif /*HAS_XM*/

  XtRealizeWidget        (shell);
  XtMapWidget            (shell);

  XWidgetSetWindowManagerCloseFunction  (shell,CloseWindow);  /*Must be realized.*/

  XtAppMainLoop          (appContext);
  return                 EXIT_SUCCESS;
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
/*.........................................................................*/
  if(This==NULL)                         return;
  if(a_event==NULL)                      return;
  if(a_event->type!=ClientMessage) return;

  XtDestroyApplicationContext  (appContext);

  CFileClearClass              ();
  CInfoF                       ("%s\n",CMemoryGetStatus());

  exit                         (EXIT_SUCCESS);
  a_argn  = 0;
  a_args  = NULL; 
}

