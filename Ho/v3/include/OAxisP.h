/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OAxisP_h
#define OAxisP_h

#include <OAxis.h>

#define N__TICKS 20

typedef struct _OAxisRecord
{
  double    minimumValue;
  double    maximumValue;
  int       autoscale,logscale;
  char*     title;
/* ticks */
  int       tickn;
  double    tickValues [N__TICKS];
  char      tickLabels[N__TICKS][10];
  int       mag;
  int       tick_up;
  int       binNumber;
} OAxisRecord;

#endif /*OAxisP_h*/
