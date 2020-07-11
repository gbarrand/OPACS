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

/*Co*/
#include <OShell.h>
#include <CoCommands.h>
#include <OType.h>
/*Xx*/
#include <XWidget.h>

#include <HFile.h>
#include <XzPlotter.h>
#include <XzTypes.h>

#ifdef __cplusplus
extern "C"{
#endif
static void CollectCallback (Widget,XtPointer,XtPointer);
static void CloseWindow     (Widget,XEvent*,char**,Cardinal*);
#ifdef __cplusplus
}
#endif

static XtAppContext appContext = NULL;
static Widget whplot;
/***************************************************************************/
int main (
 int a_argn
,String a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg    args[2];
  Widget top,shell;
/*.........................................................................*/
  XzSetTypes (); /*For HBookClearClass.*/

 {
#if XtSpecificationRelease == 4
  Cardinal argn;
  argn     = (Cardinal)a_argn;
#else
  int      argn;
  argn     = a_argn;
#endif
  top      = XtAppInitialize(&appContext,"XzPlotterT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  XDisplayPutFileInResourceDatabase (XtDisplay(top),"XzT.xrm");

  shell       = XtAppCreateShell ("xzShellT","Shell",topLevelShellWidgetClass,XtDisplay(top),args,0); 

  XtSetArg    (args[0],XtNwidth ,500);
  XtSetArg    (args[1],XtNheight,500);
  whplot      = XtCreateManagedWidget("xzPlotterT",xzPlotterWidgetClass , shell, args,2);

/* must be done here because InitializeClass of XzPlotter: contains HLIMIT */
/* Can't read directly ra files. Have to cvt to rz format first with /cern/pro/bin/rfra tool.*/
  if(HFileOpenZEBRA_FileForReading(0,"phan.rz")==NULL) return 0;

 {OShell         osh;
  osh            = OShellCreate("osh");
  CoAddCommands  (osh); 
  OShellExecute  (osh,"collect HFile | dump -");}

/* Done in usr/XzT.xrm file
  XzPlotterDrawHistogram     (whplot,12);
  XzPlotterDrawManhattanPlot (whplot,28,30.,30.);
*/

  XtRealizeWidget        (shell);
  XtMapWidget            (shell);
  XWidgetSetWindowManagerCloseFunction  (shell,CloseWindow);

  XtAddCallback   (whplot,XzNcollectCallback,CollectCallback,0);

  XtAppMainLoop   (appContext);
  return          0;
}
/***************************************************************************/
static void CollectCallback (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_data==NULL) return;
  printf ("id %d beg %d end %d\n",
     ((XzPlotterCallbackStruct*)a_data)->id,
     ((XzPlotterCallbackStruct*)a_data)->beg,
     ((XzPlotterCallbackStruct*)a_data)->end);
  This  = 0;
  a_tag = 0;
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
  XWidgetVisitFunction proc;
/*.........................................................................*/
  if(This==NULL)                   return;
  if(a_event==NULL)                return;
  if(a_event->type!=ClientMessage) return;
/*XtDestroyWidget              (shell); Do not execute XzPlotter.destroy method.*/
  proc = XClassGetDestroyFunction      (xzPlotterWidgetClass);
  if(proc!=NULL) proc (whplot);

/*
  XzPlotterClearClass          ();  
*/
  XtDestroyApplicationContext  (appContext);

  OTypeClearClass              ();

  exit                         (EXIT_SUCCESS);
  a_narg  = 0;
  a_args  = 0; 
}
