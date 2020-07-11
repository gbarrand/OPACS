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
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#ifdef HAS_XM
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#endif /*HAS_XM*/

#include <CMemory.h>
#include <CFile.h>
#include <CPrinter.h>

#include <XWidget.h>

#include <XoGraph.h>
#include <Xo.h>

#ifdef __cplusplus
extern "C"{
#endif

void ValueChangedCallback   (Widget, XtPointer, XtPointer);
void ValueChangedW2callback (Widget, XtPointer, XtPointer);
void DeleteMeCallback       (Widget, XtPointer, XtPointer);
void CreateAgainCallback    (Widget, XtPointer, XtPointer);
void SetCallback            (Widget, XtPointer, XtPointer);
void Set2Callback           (Widget, XtPointer, XtPointer);
void Set3Callback           (Widget, XtPointer, XtPointer);
static void CloseWindow  (Widget,XEvent*,char**,Cardinal*);
#ifdef __cplusplus
}
#endif

XoLink l1;
int ftimew1 = 1,
    ftimew2 = 1,
    dead  = 0;

static XtAppContext appContext = NULL;
Widget w1, w2, w3, w4, w5, w6, w7, graph;
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget top,shell;
  Arg args[10];
  int n;
/*.........................................................................*/
 {
#if XtSpecificationRelease == 4
  Cardinal argn;
  argn     = (Cardinal)a_argn;
#else
  int      argn;
  argn     = a_argn;
#endif
  top      = XtAppInitialize(&appContext,"XoGraphT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  XDisplayPutFileInResourceDatabase   (XtDisplay(top),"XoT.xrm");

/*Add WMClose does not works on ApplicationShell.*/
  shell            = XtAppCreateShell ("xoShellT","XoGraphT",topLevelShellWidgetClass,XtDisplay(top),args,0); 

  n = 0;
  XtSetArg(args[n],XtNwidth,500); n++;
  XtSetArg(args[n],XtNheight,350); n++;
  graph = XoCreateGraph(shell,"xoGraphT",args, n);

#ifdef HAS_XM
  n = 0;
  XtSetArg(args[n],XtNx,20); n++;
  XtSetArg(args[n],XtNy,20); n++;
  w1 = XtCreateManagedWidget("ToggleMoveButton",xmToggleButtonWidgetClass, graph, args, n);

  n = 0;
  XtSetArg(args[n],XtNx,100); n++;
  XtSetArg(args[n],XtNy,200); n++;
  w2 = XtCreateManagedWidget("ToggleLinkButton", xmToggleButtonWidgetClass, graph, args, n);

  l1 = XoLinkCreate (w1, w2);

  n = 0;
  XtSetArg(args[n],XtNx,300); n++;
  XtSetArg(args[n],XtNy,150); n++;
  w3 = XtCreateManagedWidget("DestroyMeButton", xmPushButtonWidgetClass, graph, args, n);

  n = 0;
  XtSetArg(args[n],XtNx,260); n++;
  XtSetArg(args[n],XtNy,260); n++;
  w4 = XtCreateManagedWidget("RebuildDestoyedButton", xmPushButtonWidgetClass, graph, args, n);


  XoLinkCreate (w3, w2);
  XoLinkCreate (w1, w3);
  n = 0;
  XtSetArg(args[n],XtNx,50); n++;
  XtSetArg(args[n],XtNy,260); n++;
  w5 = XtCreateManagedWidget("Toggle link_visible", xmToggleButtonWidgetClass, graph, args, n);

  n = 0;
  XtSetArg(args[n],XtNx,300); n++;
  XtSetArg(args[n],XtNy,20); n++;
  w6 = XtCreateManagedWidget("Toggle link_policy", xmToggleButtonWidgetClass, graph, args, n);

  n = 0;
  XtSetArg(args[n],XtNx,150); n++;
  XtSetArg(args[n],XtNy,310); n++;
  w7 = XtCreateManagedWidget("Toggle spread_policy", xmToggleButtonWidgetClass, graph, args, n);

  XtAddCallback (w1, XmNvalueChangedCallback, ValueChangedCallback, NULL);
  XtAddCallback (w2, XmNvalueChangedCallback, ValueChangedW2callback, NULL);
  XtAddCallback (w3, XmNactivateCallback, DeleteMeCallback, NULL);
  XtAddCallback (w4, XmNactivateCallback, CreateAgainCallback, NULL);
  XtAddCallback (w5, XmNvalueChangedCallback, SetCallback, NULL);
  XtAddCallback (w6, XmNvalueChangedCallback, Set2Callback, NULL);
  XtAddCallback (w7, XmNvalueChangedCallback, Set3Callback, NULL);
#endif /*HAS_XM*/

  XtRealizeWidget       (shell);
  XtMapWidget           (shell);

  XWidgetSetWindowManagerCloseFunction (shell,CloseWindow); /*Must be realized.*/
  XtAppMainLoop         (appContext);

  return                EXIT_SUCCESS;
}


void ValueChangedCallback (Widget w, XtPointer client_data,
                        XtPointer call_data)
{
  Arg	args[2];
  if (ftimew1==1)
  {
    XtSetArg(args[0],XtNx,150);
    XtSetArg(args[1],XtNy,60);
    ftimew1 = 0;
  }
  else
  {
    XtSetArg(args[0],XtNx,20);
    XtSetArg(args[1],XtNy,20);
    ftimew1 = 1;
  }
    XtSetValues (w, args, 2);
  client_data = NULL;
  call_data   = NULL;
}

void ValueChangedW2callback (Widget w, XtPointer client_data,
                        XtPointer call_data)
{
  if (ftimew2==1)
  {
    XoLinkDelete (l1);
    ftimew2 = 0;
  }
  else
  {
    l1 = XoLinkCreate (w1, w2);
    ftimew2 = 1;
  }
  w           = 0;
  client_data = NULL;
  call_data   = NULL;
}

void DeleteMeCallback (Widget w, XtPointer client_data,
                        XtPointer call_data)
{
  dead = 1;
  XtDestroyWidget (w);
  client_data = NULL;
  call_data   = NULL;
}

void CreateAgainCallback (Widget w, XtPointer client_data,
                        XtPointer call_data)
{
  if (dead==1)
  {
#ifdef HAS_XM
    int n;
    Arg args[10];
    n = 0;
    XtSetArg(args[n],XtNx,300); n++;
    XtSetArg(args[n],XtNy,150); n++;
    w3 = XtCreateManagedWidget("DestroyMeButton", xmPushButtonWidgetClass, graph, args, n);
    XtAddCallback     (w3, XmNactivateCallback, DeleteMeCallback, NULL);
    XoLinkCreate (w3, w2);
    XoLinkCreate (w1, w3);
#endif /*HAS_XM*/
    dead = 0;
  }
  w           = 0;
  client_data = NULL;
  call_data   = NULL;
}
void SetCallback (Widget This , XtPointer client_data, XtPointer call_data)
{
  Arg args[1];
  Boolean visible;

  Widget      w = XtParent(This);
  XtSetArg    (args[0], XoNlinksVisible, &visible);
  XtGetValues (w,args,1);
  visible     = !visible;
  XtSetArg    (args[0], XoNlinksVisible, visible);
  XtSetValues (w, args, 1);
  client_data = NULL;
  call_data   = NULL;
}

void Set2Callback (Widget This , XtPointer client_data, XtPointer call_data)
{
  Arg args[1];
  int policy;
  Widget      w = XtParent(This);
  XtSetArg    (args[0], XoNlinkingPolicy, &policy);
  XtGetValues (w,args,1);
  policy      = !policy;
  XtSetArg    (args[0], XoNlinkingPolicy, policy);
  XtSetValues (w, args, 1);
  client_data = NULL;
  call_data   = NULL;
}

void Set3Callback (Widget This, XtPointer client_data, XtPointer call_data)
{
  Arg args[1];
  Boolean policy;
  Widget      w = XtParent(This);
  XtSetArg    (args[0], XoNlinkSpreadType, &policy);
  XtGetValues (w,args,1);
  policy      = !policy;
  XtSetArg    (args[0], XoNlinkSpreadType, policy);
  XtSetValues (w, args, 1);
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
  if(This==NULL)                         return;
  if(a_event==NULL)                      return;
  if(a_event->type!=ClientMessage) return;

/*XtDestroyWidget              (shell); Do not execute XoGraph.destroy method.*/
  proc           = XClassGetDestroyFunction (xoGraphWidgetClass);
  if(proc!=NULL) proc (graph);

  XtDestroyApplicationContext  (appContext);

  CFileClearClass              ();
  CInfoF                       ("%s\n",CMemoryGetStatus());

  exit                         (EXIT_SUCCESS);
  a_argn  = 0;
  a_args  = NULL; 
}
