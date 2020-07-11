/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OShapeP_h
#define OShapeP_h

#include <OShape.h>

typedef struct _BoxRecord
{
  OContextRecord context;
  double         x,y,z;
  double         sx,sy,sz;
} BoxRecord;

typedef struct _OShapeBoxRecord
{
  NodeRecord    node;
  BoxRecord     box;
} OShapeBoxRecord;

typedef struct _StringRecord
{
  OContextRecord context;
  char*          string;
  double         x,y,z;
  double         Xx,Xy,Xz;
  double         Yx,Yy,Yz;
  double         size;    
  char           xalign,yalign;
} StringRecord;

typedef struct _OShapeStringRecord
{
  NodeRecord    node;
  StringRecord  string;
} OShapeStringRecord;

#endif /*OShapeP_h*/



