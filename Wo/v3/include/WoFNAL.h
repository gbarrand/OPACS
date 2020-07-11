/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoFNAL_h
#define WoFNAL_h

#ifdef HAS_FNAL

#include <X11/Intrinsic.h>

extern WidgetClass hist2DWidgetClass;
extern WidgetClass scat3DWidgetClass;
extern WidgetClass cellWidgetClass;
extern WidgetClass curvesWidgetClass;
extern WidgetClass h1DWidgetClass;
extern WidgetClass scatWidgetClass;
extern WidgetClass xyWidgetClass;

#include <FNAL_Commands.h>

#include <OClass.h>
#include <Wo.h>

#endif /*HAS_FNAL*/

#endif /*WoFNAL_h*/



