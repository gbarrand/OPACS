/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay. (barrand@lalcls.in2p3.fr) | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifdef HAS_XM

#include <stdio.h>
#include <stdlib.h>

#include <X11/StringDefs.h>

#include <Xm/PushB.h>
#include <Xm/BulletinB.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    button_cbk    (Widget,XtPointer,XtPointer);
#ifdef __cplusplus
}
#endif

static XtAppContext appContext = NULL;
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget top,board,widget;
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
  top      = XtAppInitialize(&appContext,"XmPushBT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}

  board  = XtCreateManagedWidget("board"  ,xmBulletinBoardWidgetClass , top, args, 0);
  widget = XtCreateManagedWidget("button1",xmPushButtonWidgetClass , board, args, 0);
  XtAddCallback     (widget,XmNactivateCallback,button_cbk,(XtPointer)0);
  widget = XtCreateManagedWidget("button2",xmPushButtonWidgetClass , board, args, 0);
  XtAddCallback     (widget,XmNactivateCallback,button_cbk,(XtPointer)0);

 {Pixmap            pixmap; 
  Pixel             back = 0,fore = 0;
  XtSetArg          (args[0],XtNbackground,&back);
  XtSetArg          (args[1],XtNforeground,&fore);
  XtGetValues       (widget,args,2);
  pixmap            = XmGetPixmap(XtScreen(widget),"eye.xbm",fore,back);
  if(pixmap==XmUNSPECIFIED_PIXMAP)
    {
      printf("Pixmap in file eye.xbm not found.\n");
    }
  else 
    {
      XtSetArg    (args[0],XmNlabelType,XmPIXMAP);
      XtSetArg    (args[1],XmNlabelPixmap,pixmap);
      XtSetValues (widget,args,2);
    }}

  XtRealizeWidget (top);

  XtAppMainLoop   (appContext);
  return          1;
}
/***************************************************************************/
static void button_cbk (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XEvent xevent;
/*.........................................................................*/
  if(a_data && (((XmAnyCallbackStruct*)a_data)->event) )
    {
      xevent = *(((XmAnyCallbackStruct*)a_data)->event);
      printf ("event type:%d\n",xevent.type);
      exit   (1);
    }
  This   = 0;
  a_tag  = 0;
  a_data = 0;
}

#else  /*HAS_XM*/
#include <stdio.h>
#define CWarn printf
int main() {CWarn ("C macro HAS_XM not defined.\n");return 0;}
#endif /*HAS_XM*/
