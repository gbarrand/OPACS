/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OHistogramP_h
#define OHistogramP_h

#include <OHistogram.h>

typedef struct _OHistogramRecord
{
/*General.*/
  char*     name;
  int       dimension;
  char*     title;
  int       barsType;
  double    Sw;
  int       entries;
  int       outFlow;
  void*     userData;
  int       profile;
  char*     monitoringMode;

/*Bins.*/
  int       binNumber;
  double*   binSw;
  double*   binSw2;
  int*      binEntries;

/*Axes.*/
  OAxis     axis[OAxisMaximumNumber],axisOfValues;
  int       axisOffset[OAxisMaximumNumber];
  int       axisOverFlow[OAxisMaximumNumber],axisUnderFlow[OAxisMaximumNumber];
  double    axisBinWidth[OAxisMaximumNumber];
  double    axisSxw[OAxisMaximumNumber],axisSx2w[OAxisMaximumNumber];
} OHistogramRecord;


#endif /*OHistogramP_h*/
