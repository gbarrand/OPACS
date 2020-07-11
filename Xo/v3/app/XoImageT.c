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

#include <OShell.h>
#include <OType.h>
#include <CoCommands.h>

#include <GoCommands.h>

#include <XWidget.h>

#include <XoImage.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    CloseWindow  (Widget,XEvent*,char**,Cardinal*);
#ifdef __cplusplus
}
#endif

static XtAppContext appContext = NULL;
static Widget widget;
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShell    osh;
  Widget top,shell;
  Arg    args[1];
/*.........................................................................*/
  osh           = OShellCreate("osh");
  CoAddCommands (osh);
  GoAddCommands (osh);
  OShellExecute (osh,"load oimage.odb");

 {
#if XtSpecificationRelease == 4
  Cardinal argn;
  argn     = (Cardinal)a_argn;
#else
  int      argn;
  argn     = a_argn;
#endif
  top      = XtAppInitialize(&appContext,"XoImageT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  XDisplayPutFileInResourceDatabase   (XtDisplay(top),"XoT.xrm");

/*Add WMClose does not works on ApplicationShell.*/
  shell                  = XtAppCreateShell ("xoShellT","XoImageT",topLevelShellWidgetClass,XtDisplay(top),args,0); 

  widget                = XtCreateManagedWidget ("xoImageT",xoImageWidgetClass ,shell, args, 0);

  XtRealizeWidget       (shell);
  XtMapWidget           (shell);

  XWidgetSetWindowManagerCloseFunction (shell,CloseWindow); /*Must be realized.*/

  XtAppMainLoop         (appContext);

  return              EXIT_SUCCESS;
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
  if(This==NULL)                   return;
  if(a_event==NULL)                return;
  if(a_event->type!=ClientMessage) return;
/*XtDestroyWidget              (shell); Do not execute XoCamera.destroy method.*/
  proc           = XClassGetDestroyFunction      (xoImageWidgetClass);
  if(proc!=NULL) proc (widget);


  XoImageClearClass            ();  
  XtDestroyApplicationContext  (appContext);

  OTypeClearClass              ();

  exit                         (EXIT_SUCCESS);
  a_argn  = 0;
  a_args  = NULL; 
}

