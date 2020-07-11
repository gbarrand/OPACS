/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OMatrixP_h
#define OMatrixP_h

#include <OMatrix.h>

typedef struct _OMatrixRecord 
{
  double e00,e01,e02,e03,
         e10,e11,e12,e13,
         e20,e21,e22,e23,
         e30,e31,e32,e33;
} OMatrixRecord;

#define OMatrix_identity {1.,0.,0.,0., 0.,1.,0.,0., 0.,0.,1.,0. ,0.,0.,0.,1.}

#endif /*OMatrixP_h*/






