/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OColormapP_h
#define OColormapP_h

#include <OColormap.h>

typedef struct _OColormapRecord
{
  char*          name;
  int            number;
  double*        reds;
  double*        greens;
  double*        blues;
  int            namen;
  char**         names;
} OColormapRecord;

#endif /*OColormapP_h*/
