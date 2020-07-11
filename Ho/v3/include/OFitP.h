/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OFitP_h
#define OFitP_h

#include <OFit.h>

#define MAX_PARAM 50  /* MINUIT limit */
typedef struct _OFitRecord 
{
  OFitType    fitType;
  int         npar;
  double*     pars;
  double*     starts;
  double*     steps;
  double*     errs;
  int         auto_starts,auto_steps;
/* user defined fit (using interpreters) */
  char*       script;
  char*       variable[MAX_PARAM+1];
  OStatement       ostmt;
} OFitRecord;

#endif /*OFitP_h*/
