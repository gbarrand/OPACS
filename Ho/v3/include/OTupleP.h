/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OTupleP_h
#define OTupleP_h

#include <OTuple.h>

typedef struct _OTupleRecord
{
  char*        name;
  char*        title;
  int          rows,cols;
  int          labeln;
  char**       labels;
  int          valuen;
  double*      values;
  int          crow;
  char*        fname;
} OTupleRecord;

#endif /*OTupleP_h*/
