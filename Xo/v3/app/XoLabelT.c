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

/*Co*/
#include <CMemory.h>
#include <CFile.h>

/*Xx*/
#include <XDisplay.h>
#include <XWidget.h>

#include <XoLabel.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  Start        (XtPointer,XtIntervalId*);
static void  CloseWindow  (Widget,XEvent*,char**,Cardinal*);
#ifdef __cplusplus
}
#endif

static XtAppContext appContext = NULL;
static Widget xoLabel = NULL;
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
  top      = XtAppInitialize(&appContext,"XoLabelT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  XDisplayPutFileInResourceDatabase   (XtDisplay(top),"XoT.xrm");

/*Add WMClose does not works on ApplicationShell.*/
  shell                  = XtAppCreateShell ("xoShellT","XoLabelT",topLevelShellWidgetClass,XtDisplay(top),args,0); 

  xoLabel                 = XtCreateManagedWidget ("xoLabelT",xoLabelWidgetClass ,shell, args, 0);

  XtRealizeWidget        (shell);
  XtMapWidget            (shell);

  XWidgetSetWindowManagerCloseFunction  (shell,CloseWindow);  /*Must be realized.*/

  Start                  (NULL,NULL);

  XtAppMainLoop          (appContext);

  return                 EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void Start (
 XtPointer     a_closure
,XtIntervalId* a_ident 
)
/***************************************************************************/
/* The redisplay involves by SetValues return True,                        */
/*  implies a XClearArea on the window.                                    */
/* In case of double buffer we return False an do the refresh by an        */
/*  explicit call, without clearing the window.                            */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Boolean double_buffer;
  Arg     args[1];
  static int count = 0;
  static  char  string[16];
/*.........................................................................*/
  XtSetArg                (args[0],XtNdoubleBuffer,&double_buffer);
  XtGetValues             (xoLabel,args,1);
  sprintf                 (string,"%d",count);
  XtSetArg                (args[0],XtNlabel,string);
  XtSetValues             (xoLabel,args,1);  
  if(double_buffer==True) XoLabelDraw (xoLabel);
  count++;
  XtAppAddTimeOut 	  (appContext,(unsigned long)50,(XtTimerCallbackProc)Start,NULL);
  a_closure           = NULL;
  a_ident             = NULL;
}
/***************************************************************************/
static void CloseWindow (
 Widget This 
,XEvent* a_event 
,char** a_args
,Cardinal* a_narg
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*      string;
  XWidgetVisitFunction proc;
/*.........................................................................*/
  if(This==NULL)                   return;
  if(a_event==NULL)                return;
  if(a_event->type!=ClientMessage) return;
/*XtDestroyWidget              (shell); Do not execute XoLabel.destroy method.*/
  proc           = XClassGetDestroyFunction      (xoLabelWidgetClass);
  if(proc!=NULL) proc        (xoLabel);

  XtDestroyApplicationContext  (appContext);

  CFileClearClass              ();
  string           = CMemoryGetStatus();
  if(string!=NULL) printf ("%s\n",string);

  exit             (EXIT_SUCCESS);
  a_narg  = 0;
  a_args  = NULL; 
}


