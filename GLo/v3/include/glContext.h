/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glContext_h
#define glContext_h
 
/* polyline styles */
typedef enum {
      glLineSolid = 0,
      glLineDashed,
      glLineDotted,
      glLineDashDotted
} glLine;

typedef enum {
      glMarkerDot = 0,
      glMarkerPlus,
      glMarkerAsterisk,
      glMarkerCircle,
      glMarkerCross,
      glMarkerStar
} glMarker;

typedef enum {
      glAreaSolid = 0,
      glAreaHatched,
      glAreaChecker
} glArea;

typedef struct _glColorRecord *DColor;

typedef struct _glColorRecord 
{
  double        red,green,blue;
  unsigned long pixel;
  int           allocated;
} glColorRecord;

typedef struct _glContextRecord *glContext;

typedef struct _glContextRecord 
{
  glColorRecord     color;
  int           width;
  glLine    lineStyle;
  glMarker    markStyle;
  glArea    areaStyle;
  int           polygonMode;
} glContextRecord;


#endif  /*glContext_h*/
