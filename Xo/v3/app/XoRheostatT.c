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

#include <XWidget.h>

#include <XoRheostat.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    CloseWindow  (Widget,XEvent*,char**,Cardinal*);
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
  top      = XtAppInitialize(&appContext,"XoRheostatT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}

/*Add WMClose does not works on ApplicationShell.*/
  shell                  = XtAppCreateShell ("xoShellT","XoRheostatT",topLevelShellWidgetClass,XtDisplay(top),args,0); 

  XtCreateManagedWidget ("xoRheostatT",xoRheostatWidgetClass , shell, args, 0);

  XtRealizeWidget       (shell);
  XtMapWidget           (shell);

  XWidgetSetWindowManagerCloseFunction (shell,CloseWindow); /*Must be realized.*/

  XtAppMainLoop        (appContext);
  return                EXIT_SUCCESS;
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
  if(This==NULL)                   return;
  if(a_event==NULL)                return;
  if(a_event->type!=ClientMessage) return;

  XtDestroyApplicationContext  (appContext);

  exit                         (EXIT_SUCCESS);
  a_argn  = 0;
  a_args  = NULL; 
}
