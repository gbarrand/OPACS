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
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <CPrinter.h>
#include <CFile.h>
#include <CMemory.h>

#include <XWidget.h>

#include <XoMosaic.h>

#ifdef __cplusplus
extern "C"{
#endif
static void CloseWindow  (Widget,XEvent*,char**,Cardinal*);
#ifdef __cplusplus
}
#endif

static XtAppContext appContext = NULL;
static Widget wmosaic = NULL;
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget top,shell;
/*.........................................................................*/
 {
#if XtSpecificationRelease == 4
  Cardinal argn;
  argn     = (Cardinal)a_argn;
#else
  int      argn;
  argn     = a_argn;
#endif
  top      = XtAppInitialize(&appContext,"XoMosaicT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  XDisplayPutFileInResourceDatabase       (XtDisplay(top),"XoT.xrm");

/*Add WMClose does not works on ApplicationShell.*/
  shell                  = XtAppCreateShell ("xoShellT","XoMosaicT",topLevelShellWidgetClass,XtDisplay(top),NULL,0); 

  wmosaic                = XtCreateManagedWidget ("xoMosaicT",xoMosaicWidgetClass ,shell, NULL, 0);

/* UNIX> XoMosaicT 'CoT.htm' */
  if(a_argn==2)          XoMosaicShowTopic (wmosaic,a_args[1]);

  XtRealizeWidget        (shell);
  XtMapWidget            (shell);

  XWidgetSetWindowManagerCloseFunction  (shell,CloseWindow); /*Must be realized.*/

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
  XWidgetVisitFunction proc;
/*.........................................................................*/
  if(This==NULL)                   return;
  if(a_event==NULL)                return;
  if(a_event->type!=ClientMessage) return;

/*XtDestroyWidget              (shell); Do not execute XoMosaic.destroy method.*/

  proc           = XClassGetDestroyFunction      (xoMosaicWidgetClass);
  if(proc!=NULL) proc (wmosaic);

  XtDestroyApplicationContext  (appContext);

  CFileClearClass              ();
  CInfoF                       ("%s\n",CMemoryGetStatus());

  exit                         (EXIT_SUCCESS);
  a_argn  = 0;
  a_args  = NULL; 
}
