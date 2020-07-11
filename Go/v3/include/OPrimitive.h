/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OPrimitive_h
#define OPrimitive_h

typedef struct _OPrimitiveRecord      *OPrimitive;

#define  OPrimitiveLines    0
#define  OPrimitiveSegments 1
#define  OPrimitiveMarkers  2
#define  OPrimitivePolygon  3

#include <OContext.h>
#ifdef __cplusplus
extern "C"{
#endif
void          OPrimitiveClearClass     ();
OPrimitive    OPrimitiveMake           ();
OPrimitive    OPrimitiveCreate         (int,OContext,int,double*,double*,double*);

void          OPrimitiveDelete         (OPrimitive);
OPrimitive    OPrimitiveDuplicate      (OPrimitive);
void          OPrimitiveAddPoints      (OPrimitive,int,double*,double*,double*);
int           OPrimitiveGetType        (OPrimitive);
void          OPrimitiveSetContext     (OPrimitive,OContext);
void          OPrimitiveSetLabel       (OPrimitive,int);
int           OPrimitiveGetLabel       (OPrimitive);
int           OPrimitiveGetColorIndex  (OPrimitive);
int           OPrimitiveGetLineWidth   (OPrimitive);
OLineStyle    OPrimitiveGetLineStyle   (OPrimitive);
int           OPrimitiveGetMarkerSize  (OPrimitive);
OMarkerStyle  OPrimitiveGetMarkerStyle (OPrimitive);
OAreaStyle    OPrimitiveGetAreaStyle   (OPrimitive);
void          OPrimitiveSetColorIndex  (OPrimitive,int);
void          OPrimitiveSetLineWidth   (OPrimitive,int);
void          OPrimitiveSetMarkerSize  (OPrimitive,int);
void          OPrimitiveSetLineStyle   (OPrimitive,OLineStyle);
void          OPrimitiveSetMarkerStyle (OPrimitive,OMarkerStyle);
void          OPrimitiveSetAreaStyle   (OPrimitive,OAreaStyle);
int           OPrimitiveGetPoints      (OPrimitive,double**,double**,double**);
int           OPrimitiveGetSize        (OPrimitive,double*,double*,double*,double*,double*,double*);
int           OPrimitiveGetSumOfPoints (OPrimitive,double*,double*,double*);
int           OPrimitiveGetIthPoint    (OPrimitive,int,double*,double*,double*);

int           OPrimitiveSetAttribute   (OPrimitive,char*,void*,void*,int);
int           OPrimitiveGetAttribute   (OPrimitive,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOPrimitive(0); /**/
#endif

#endif  /*OPrimitive_h*/
