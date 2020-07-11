/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OContext_h
#define OContext_h
 
typedef struct _OContextRecord *OContext;

/* polyline styles */
typedef enum {
      OLineStyleSolid = 0,
      OLineStyleDashed,
      OLineStyleDotted,
      OLineStyleDashDotted
} OLineStyle;

typedef enum {
      OMarkerStyleDot = 0,
      OMarkerStylePlus,
      OMarkerStyleAsterisk,
      OMarkerStyleCircle,
      OMarkerStyleCross,
      OMarkerStyleStar
} OMarkerStyle;

typedef enum {
      OAreaStyleSolid = 0,
      OAreaStyleHatched,
      OAreaStyleChecker
} OAreaStyle;

typedef enum {
  OModelingWireFrame,
  OModelingSolid,
  OModelingCloud,
  OModelingSegments
} OModeling;

typedef enum {
    OPaintingColor,
    OPaintingShadow,
    OPaintingLUT,
    OPaintingGradient
} OPainting;

typedef enum {
    OFontLatin,
    OFontGreek
} OFont;

/*Co*/
#include <OColor.h>

#ifndef NO_C_H
#include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
OContext      OContextGetStaticInstance    ();
OContext      OContextMake                 ();
void          OContextDelete               (OContext);
void          OContextInitialize           (OContext);
void          OContextCopy                 (OContext,OContext);
OContext      OContextDuplicate            (OContext);
void          OContextSetColorIndex        (OContext,int);
int           OContextGetColorIndex        (OContext);
void          OContextSetLineWidth         (OContext,int);
int           OContextGetLineWidth         (OContext);
void          OContextSetMarkerSize        (OContext,int);
int           OContextGetMarkerSize        (OContext);
void          OContextSetLineStyle         (OContext,OLineStyle);
OLineStyle    OContextGetLineStyle         (OContext);
void          OContextSetMarkerStyle       (OContext,OMarkerStyle);
OMarkerStyle  OContextGetMarkerStyle       (OContext);
void          OContextSetAreaStyle         (OContext,OAreaStyle);
OAreaStyle    OContextGetAreaStyle         (OContext);
void          OContextSetModeling          (OContext,OModeling);
OModeling     OContextGetModeling          (OContext);
void          OContextSetPainting          (OContext,OPainting);
OPainting     OContextGetPainting          (OContext);
void          OContextSetFont              (OContext,OFont);
OFont         OContextGetFont              (OContext);
void          OContextRestoreDefaultValues (OContext);
void          OContextSetF                 (OContext,char*,...);
void          OContextGetF                 (OContext,char*,...);
void          OContextSetV                 (OContext,char*,va_list);
void          OContextGetV                 (OContext,char*,va_list);
void          OContextSetFromScript        (OContext,char*);
int           OContextSetAttribute         (OContext,char*,void*,void*,int);
int           OContextGetAttribute         (OContext,char*,void*,void*,int*);
int           OContextDoMethod             (OContext,char*,void*,int,char**,void*,int*);

char**        OLineStyleGetNames           (int*);
OLineStyle    OLineStyleGetIdentifier      (char*);
char*         OLineStyleGetName            (OLineStyle);

char**        OMarkerStyleGetNames         (int*);
OMarkerStyle  OMarkerStyleGetIdentifier    (char*);
char*         OMarkerStyleGetName          (OMarkerStyle);

char**        OAreaStyleGetNames           (int*);
OAreaStyle    OAreaStyleGetIdentifier      (char*);
char*         OAreaStyleGetName            (OAreaStyle);

char**        OModelingGetNames            (int*);
OModeling     OModelingGetIdentifier       (char*);
char*         OModelingGetName             (OModeling);

char**        OPaintingGetNames            (int*);
OPainting     OPaintingGetIdentifier       (char*);
char*         OPaintingGetName             (OPainting);

char**        OFontGetNames                (int*);
OFont         OFontGetIdentifier           (char*);
char*         OFontGetName                 (OFont);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOContext(0); /**/
#endif

#endif  /*OContext_h*/



