/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OContextP_h
#define OContextP_h

#include <OContext.h>

typedef struct _OContextRecord 
{ 
/*Rendering*/
  int           color;
  int           lineWidth;
  int           markerSize;
  OLineStyle    lineStyle;
  OMarkerStyle  markStyle;
  OAreaStyle    areaStyle;
/*Modeling*/
  OModeling     modeling; 
  OPainting     painting;
  OFont         font;
} OContextRecord;

#endif /*OContextP_h*/

