/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef ZPointP_h
#define ZPointP_h
 
#include <ZPoint.h>

typedef long  ZPos;

typedef struct _ZPointRecord {
  ZPos   x;
  ZPos   y;
  double z;
} ZPointRecord;

typedef struct _ZPlaneRecord {
  double A,B,C,D;
  double zmin,zmax;
} ZPlaneRecord;

#endif  /*ZPointP_h*/
