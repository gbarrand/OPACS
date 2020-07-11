/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoXtw_h
#define WoXtw_h

#define HAS_XTW   /*Required.*/

#ifdef HAS_XTW

/*Xo*/
#include <XWidget.h>

/*Wo*/
#include <OClass.h>
#include <OInterface.h>
#include <OResource.h>
#include <XtwTools.h>
#include <Wo.h>

extern WidgetClass coreWidgetClass;
extern WidgetClass compositeWidgetClass;
extern WidgetClass constraintWidgetClass;
extern WidgetClass shellWidgetClass;
extern WidgetClass wmShellWidgetClass;
extern WidgetClass applicationShellWidgetClass;
extern WidgetClass overrideShellWidgetClass;
extern WidgetClass transientShellWidgetClass;
extern WidgetClass topLevelShellWidgetClass;
/* pb on HP-UX if linking with libXm.sl and libXt.a */
/*
extern WidgetClass vendorShellWidgetClass; 
*/

#endif /*HAS_XTW*/

#endif /*WoXtw_h*/
