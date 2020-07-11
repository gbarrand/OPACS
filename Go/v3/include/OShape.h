/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OShape_h
#define OShape_h

#include <ONode.h>
#ifdef __cplusplus
extern "C"{
#endif
ONode OShapeBoxMake       ();
ONode OShapeBoxCreate     (double,double,double,double,double,double,OContext,char*);
ONode OShapeBoxCreateF    (double,double,double,double,double,double,OContext,int,char*,...);

ONode OShapeStringMake           ();
ONode OShapeStringCreate         (char*,double,double,double,double,double,double,double,double,double,double,char,char,OContext,char*);
ONode OShapeStringCreateF        (char*,double,double,double,double,double,double,double,double,double,double,char,char,OContext,int,char*,...);
ONode OShapeAlignedStringCreateF (char*,double,double,double,double,double,double,double,double,double,double,char,char,OContext,int,char*,...);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOShape(0); /**/
#endif

#endif  /*OShape_h*/
