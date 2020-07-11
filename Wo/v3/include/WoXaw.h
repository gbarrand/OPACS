/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoXaw_h
#define WoXaw_h

#ifdef HAS_XAW

/*Xo*/
#include <XWidget.h>

/*Wo*/
#include <OClass.h>
#include <OInterface.h>
#include <OResource.h>
#include <Wo.h>

#include <XawTools.h>

/*Not in all distrib.*/
/*
extern WidgetClass asciiStringWidgetClass;
extern WidgetClass asciiDiskWidgetClass;
*/

/*Not in all distrib.*/
/*
extern WidgetClass clockWidgetClass;
extern WidgetClass logoWidgetClass;
extern WidgetClass mailboxWidgetClass;
*/
/*treeWidgetClass exists in libXaw and libpawlib where it is the Young widget. Crash on ULTRIX.*/
/*
extern WidgetClass treeWidgetClass;
*/

extern WidgetClass asciiTextWidgetClass;
extern WidgetClass boxWidgetClass;
extern WidgetClass commandWidgetClass;
extern WidgetClass dialogWidgetClass;
extern WidgetClass formWidgetClass;
extern WidgetClass gripWidgetClass;
extern WidgetClass labelWidgetClass;
extern WidgetClass listWidgetClass;
extern WidgetClass menuButtonWidgetClass;
extern WidgetClass panedWidgetClass;
extern WidgetClass pannerWidgetClass;
extern WidgetClass portholeWidgetClass;
extern WidgetClass repeaterWidgetClass;
extern WidgetClass scrollbarWidgetClass;
extern WidgetClass simpleWidgetClass;
extern WidgetClass simpleMenuWidgetClass;
extern WidgetClass stripChartWidgetClass;
extern WidgetClass textWidgetClass;
extern WidgetClass toggleWidgetClass;
extern WidgetClass viewportWidgetClass;

extern WidgetClass asciiSinkObjectClass;
extern WidgetClass asciiSrcObjectClass;
extern WidgetClass smeObjectClass;
extern WidgetClass smeBSBObjectClass;
extern WidgetClass smeLineObjectClass;
extern WidgetClass textSinkObjectClass;
extern WidgetClass textSrcObjectClass;

#endif /*HAS_XAW*/

#endif /*WoXaw_h*/
