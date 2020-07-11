/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
#define DEBUG
*/

#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>

/*Co*/
#include <CMemory.h>
#include <CPrinter.h>
#include <OType.h>
#include <OBatch.h>
/*Xx*/
#include <XSelection.h>
#include <XWidget.h>
/*Go*/
#include <OCamera.h>
#include <Go.h>
#include <OShape.h>
/*Xo*/
#include <XoCamera.h>

#ifdef __cplusplus
extern "C"{
#endif
static ONode RepresentCube        ();
static void  TreatInput           (Widget,char*);
static void  CloseWindow          (Widget,XEvent*,char**,Cardinal*);
#ifdef __cplusplus
}
#endif

static XtAppContext appContext = NULL;
static Widget       wcamera    = NULL;
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget       top,shell;
  Arg          args[1];
  Display*     display;
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : XoCameraT : begin.\n");
#endif

 {
#if XtSpecificationRelease == 4
  Cardinal argn;
  argn     = (Cardinal)a_argn;
#else
  int      argn;
  argn     = a_argn;
#endif
  top      = XtAppInitialize(&appContext,"XoCameraT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  display  = XtDisplay(top);
  XDisplayPutFileInResourceDatabase   (display,"XoT.xrm");

#ifdef DEBUG
  printf ("debug : XoCameraT : create widgets.\n");
#endif

/* 
  When linking with Motif, add WMClose does not works on ApplicationShell.
  But if using a TopLevelShellWidgetClass the application class name
 do not works in resource file !
*/
  shell                  = XtAppCreateShell ("xoShellT","XoCameraT",topLevelShellWidgetClass,display,args,0); 
/*shell                  = top;*/

  wcamera                = XtCreateManagedWidget ("xoCameraT",xoCameraWidgetClass ,shell, args, 0);

  OCameraViewNode        ((OCamera)XoCameraGetCamera(wcamera),RepresentCube());

 {ONode                  node;
  node                   = OShapeBoxCreate(0.5,0.5,0.5,0.5,1.,2.,NULL,"Box");
  OCameraViewNode        ((OCamera)XoCameraGetCamera(wcamera),node);} 

#ifdef DEBUG
  printf ("debug : XoCameraT : realize.\n");
#endif

  XtRealizeWidget        (shell);

  XtMapWidget            (shell);

#ifdef DEBUG
  printf ("debug : XoCameraT : map.\n");
#endif

/*Widget must be realized.*/
  XWidgetSetWindowManagerCloseFunction  (shell,CloseWindow);

  XWidgetSpawnShellScript (shell,"sleep 3;ls",TreatInput);

/*XtAppMainLoop          (appContext);*/

/*Main loop that take into account Cut/Paste.*/
  while(1)
    { 
      XEvent              event;
      XtAppNextEvent     (appContext,&event);
      if(XSelectionDispatchEvent(&event)==0) XtDispatchEvent(&event);        
    }

  return                 EXIT_SUCCESS;
}
/***************************************************************************/
static ONode RepresentCube (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode gocube;
  OPointList points;
/*.........................................................................*/
  OContextSetF        (OContextGetStaticInstance(),"clw",OColorYellow,OLineStyleDashed,4);

  gocube          = ONodeCreate ("Cube");
  points          = OPointListCreate(5);
  OPointListSetIthEntry (points,0, 0.5,-0.5,0.5);
  OPointListSetIthEntry (points,1, 0.5, 0.5,0.5);
  OPointListSetIthEntry (points,2,-0.5, 0.5,0.5);
  OPointListSetIthEntry (points,3,-0.5,-0.5,0.5);
  OPointListSetIthEntry (points,4, 0.5,-0.5,0.5);
  GoAddLinesToNode   (gocube,points);
  OPointListSetIthEntry (points,0, 0.5,-0.5,-0.5);
  OPointListSetIthEntry (points,1, 0.5, 0.5,-0.5);
  OPointListSetIthEntry (points,2,-0.5, 0.5,-0.5);
  OPointListSetIthEntry (points,3,-0.5,-0.5,-0.5);
  OPointListSetIthEntry (points,4, 0.5,-0.5,-0.5);
  GoAddLinesToNode   (gocube,points);
  OPointListDelete   (points);
  points          = OPointListCreate(8);
  OPointListSetIthEntry (points,0, 0.5,-0.5, 0.5);
  OPointListSetIthEntry (points,1, 0.5,-0.5,-0.5);
  OPointListSetIthEntry (points,2, 0.5, 0.5, 0.5);
  OPointListSetIthEntry (points,3, 0.5, 0.5,-0.5);
  OPointListSetIthEntry (points,4,-0.5, 0.5, 0.5);
  OPointListSetIthEntry (points,5,-0.5, 0.5,-0.5);
  OPointListSetIthEntry (points,6,-0.5,-0.5, 0.5);
  OPointListSetIthEntry (points,7,-0.5,-0.5,-0.5);
  GoAddSegmentsToNode(gocube,points);
  OPointListDelete   (points);

  return gocube;
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
  if(This==NULL)                    return;
  if(!a_event)                      return;
  if (a_event->type!=ClientMessage) return;
/*XtDestroyWidget              (shell); Do not execute XoCamera.destroy method.*/
  proc                 = XClassGetDestroyFunction      (xoCameraWidgetClass);
  if(proc!=NULL)       proc (wcamera);

  XoCameraClearClass           ();  
  XSelectionClearClass         ();
  XtDestroyApplicationContext  (appContext);

  OTypeClearClass              ();

  exit                         (EXIT_SUCCESS);
  a_narg  = 0;
  a_args  = NULL; 
}
/***************************************************************************/
static void TreatInput (
 Widget a_data
,char*  a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CInfo  (a_string);
  a_data = NULL;
}

