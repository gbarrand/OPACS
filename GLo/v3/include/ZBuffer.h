/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef ZBuffer_h
#define ZBuffer_h
 
typedef unsigned int ZPixel; 
/*On X bits_per_pixel can't be > 32.
  If you change the definition of ZPixel, check creation of XImage Class.zimage in OCamera.c.
*/

#include <ZPoint.h>
#ifdef __cplusplus
extern "C"{
#endif
void    ZBufferClearClass          ();
int     ZBufferChangeSize          (int,int);
double  ZBufferGetZ_Maximum        ();
ZPixel* ZBufferGetColorBuffer      (int*,int*);
void    ZBufferClearColorBuffer    (ZPixel);
void    ZBufferClearDepthBuffer    (double);
void    ZPointInitialize           (ZPoint,double,double,double);
void    ZBufferSetClippingRegion   (int,int,int,int);
void    ZBufferDrawPoint           (ZPoint,ZPixel);
void    ZBufferDrawLine            (ZPoint,ZPoint,ZPixel);
void    ZBufferDrawLines           (int,ZPointRecordList,ZPixel);
void    ZBufferDrawSegments        (int,ZPointRecordList,ZPixel);
void    ZBufferDrawMarkers         (int,ZPointRecordList,ZPixel);
void    ZBufferDrawPolygon         (int,ZPointRecordList,ZPlane,ZPixel);
void    ZBufferDrawPolygonWithEdge (int,ZPointRecordList,ZPlane,ZPixel);
ZPixel  ZBufferGetPixel            (int,int);
ZPixel  ZBufferGetClippedPixel     (int,int);
#ifdef __cplusplus
}
#endif


#endif  /*ZBuffer_h*/
