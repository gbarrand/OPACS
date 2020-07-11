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

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#ifdef HAS_CI
#include <Ci.h>
#include <OStatement.h>
#ifdef __cplusplus
extern "C"{
#endif
void CiBindmath      (void(*)());
void CiBindstdarg    (void(*)());
void CiBindstdio     (void(*)());
void CiBindstdlib    (void(*)());
void CiBindstring    (void(*)());
void CiBindtime      (void(*)());
void CiBindCi        (void(*)());
#ifdef __cplusplus
}
#endif
#endif

/*Co*/
#include <OType.h>
/*Ho*/
#include <HoTypes.h>
/*Xx*/
#include <XWidget.h>
/*Xo*/
#include <OTuple.h>
#include <XoPlotter.h>
#include <XoCamera.h>

#ifdef __cplusplus
extern "C"{
#endif
static void CloseWindow    (Widget,XEvent*,char**,Cardinal*);
#ifdef __cplusplus
}
#endif

static XtAppContext appContext = NULL;
static Widget wplot = NULL;
/***************************************************************************/
int main (
 int    a_argn
,char*  a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget top,shell;
  Arg    args[2];
/*.........................................................................*/
#ifdef HAS_CI
/*Declare Ci to Ho/OStatement */
  OStatementSetFunctions  (
 (void(*)())CiParserStartup
,(void(*)())CiVariableGetReference
,(void(*)())CiTextCompile
,(void(*)())CiVariableDelete
,(void(*)())CiVariableGetValue
,(void(*)())CiValuePSet
,(void(*)())CiValueCastToDouble
,(void(*)())CiStatementExecute
,(void(*)())CiFunctionGetBind
,(void(*)())CiSymbolGetReference
,(void(*)())CiStatementDelete
,(void(*)())CiFunctionSwitchNew
,(void(*)())CiDo
,(void(*)())CiFunctionBind
,(void(*)())CiParserCleanup
,(void(*)())CiPathNew
,(void(*)())CiParserDeclareErrorHandler
,(void(*)())CiBindClass
,(void(*)())CiSetPrintFunction
,(void(*)())CiBindmath
,(void(*)())CiBindstdarg
,(void(*)())CiBindstdio
,(void(*)())CiBindstdlib
,(void(*)())CiBindstring
,(void(*)())CiBindtime
,(void(*)())CiBindCi
);
#endif

 {
#if XtSpecificationRelease == 4
  Cardinal argn;
  argn     = (Cardinal)a_argn;
#else
  int      argn;
  argn     = a_argn;
#endif
  top      = XtAppInitialize(&appContext,"XoPlotterT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  XDisplayPutFileInResourceDatabase (XtDisplay(top),"XoT.xrm");
  
  HoSetTypes             ();
  OTypeLoadFile          ("naine.odb",NULL);

/* 
  When linking with Motif, add WMClose does not works on ApplicationShell.
  But if using a TopLevelShellWidgetClass the application class name
 do not works in resource file !
*/
  shell                  = XtAppCreateShell ("xoShellT","XoPlotterT",topLevelShellWidgetClass,XtDisplay(top),args,0); 

  XtSetArg               (args[0],XtNwidth ,500);
  XtSetArg               (args[1],XtNheight,500);
  wplot                  = XtCreateManagedWidget("xoPlotterT",xoPlotterWidgetClass,shell,args,2);

  XtRealizeWidget        (shell);
  XtMapWidget            (shell);
  XWidgetSetWindowManagerCloseFunction  (shell,CloseWindow);

  XtAppMainLoop             (appContext);

  return                 0;
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
  proc = XClassGetDestroyFunction      (xoPlotterWidgetClass);
  if(proc!=NULL) proc (wplot);
  proc = XClassGetDestroyFunction      (xoCameraWidgetClass);
  if(proc!=NULL)  proc (wplot);

  XoPlotterClearClass          ();  
  XoCameraClearClass           ();
  XtDestroyApplicationContext  (appContext);

  OTypeClearClass              ();

  exit                         (EXIT_SUCCESS);
  a_argn  = 0;
  a_args  = NULL; 
}
