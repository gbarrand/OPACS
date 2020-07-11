/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifdef HAS_XAW

/*
#define DEBUG
*/

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

#include <Xaw/Viewport.h>
#include <Xaw/AsciiText.h>

/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget   top,viewport,text;
  Arg      args[3];
/*.........................................................................*/
#if XtSpecificationRelease == 4
 {Cardinal narg;
  narg     = (Cardinal)a_argn;
  top      = XtInitialize(a_args[0], "XawT", NULL, 0, &narg,a_args);}
#else
 {int      narg;
  narg     = a_argn;
  top      = XtInitialize(a_args[0], "XawT", NULL, 0, &narg,a_args);}
#endif

  XtSetArg        (args[0],XtNwidth ,100);
  XtSetArg        (args[1],XtNheight,100);
  viewport        = XtCreateManagedWidget ("viewport",viewportWidgetClass ,top, args, 2);

  XtSetArg        (args[0],XtNstring  ,"hello\ncoucou\naaa\nbbb");
  XtSetArg        (args[1],XtNeditType,XawtextEdit);
  XtSetArg        (args[2],XtNresize  ,XawtextResizeHeight);
  text            = XtCreateManagedWidget ("text",asciiTextWidgetClass,viewport,args,2);

  XtRealizeWidget (top);

  XtMapWidget     (top);

  XtMainLoop      ();

  return          EXIT_SUCCESS;
}

#else  /*HAS_XM*/
#include <stdio.h>
#define CWarn printf
int main() {CWarn ("C macro HAS_XAW not defined.\n");return 0;}
#endif /*HAS_XM*/
