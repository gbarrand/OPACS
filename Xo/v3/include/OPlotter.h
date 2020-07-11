/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OPlotter_h
#define OPlotter_h

typedef struct _OPlotterRecord *OPlotter;

typedef int(*OPlotterSendFunction)(void*);

typedef enum {
    OLayoutXV = 0,
    OLayoutXY,
    OLayoutXYV,
    OLayoutXYZ
} OLayout;

typedef enum {
    OXY_ModelingRandom = 0,
    OXY_ModelingBox,
    OXY_ModelingText,
    OXY_ModelingColor
} OXY_Modeling;

#include <CList.h>
#ifdef __cplusplus
extern "C"{
#endif
void         OPlotterClearClass                       ();
OPlotter*    OPlotterGetIdentifiers                   ();
OPlotter     OPlotterGetIdentifier                    (char*);
OPlotter     OPlotterMake                             ();
OPlotter     OPlotterCreate                           (char*);
void         OPlotterSetColorGradient                 (void*,int,int);

int          OPlotterIsValid                          (OPlotter);
void         OPlotterDelete                           (OPlotter);
char*        OPlotterGetName                          (OPlotter);
int          OPlotterGetDimension                     (OPlotter);
void         OPlotterSetLayout                        (OPlotter,OLayout);
OLayout      OPlotterGetLayout                        (OPlotter);
void         OPlotterSetXY_Modeling                   (OPlotter,OXY_Modeling);
OXY_Modeling OPlotterGetXY_Modeling                   (OPlotter);
void         OPlotterSetLayoutAutomated               (OPlotter,int);
void         OPlotterSetBarsVisible                   (OPlotter,int);
int          OPlotterAreBarsVisible                   (OPlotter);

int          OPlotterGetPickability                   (OPlotter);
void         OPlotterSetPickability                   (OPlotter,int);
void         OPlotterSetGridsVisible                  (OPlotter,int);
int          OPlotterAreGridsVisible                  (OPlotter);
void         OPlotterSetWallsVisible                  (OPlotter,int);
void         OPlotterSetTitleVisible                  (OPlotter,int);
void         OPlotterSetTitlePosition                 (OPlotter,int);
int          OPlotterAreWallsVisible                  (OPlotter);
void         OPlotterSetHistogramContext              (OPlotter,char*);
char*        OPlotterGetHistogramContext              (OPlotter);
void         OPlotterSetSecondHistogramContext        (OPlotter,char*);
char*        OPlotterGetSecondHistogramContext        (OPlotter);
void         OPlotterSetFunctionContext               (OPlotter,char*);
void         OPlotterSetSecondFunctionContext         (OPlotter,char*);
void         OPlotterSetScatterContext                (OPlotter,char*);
void         OPlotterSetSecondScatterContext          (OPlotter,char*);
void         OPlotterSetWallContext                   (OPlotter,char*);
void         OPlotterSetGridContext                   (OPlotter,char*);
void         OPlotterSetTextContext                   (OPlotter,char*);
void         OPlotterSetColormap                      (OPlotter,void*);
void         OPlotterSetValueColorMapping             (OPlotter,char*);
void         OPlotterSetTitle                         (OPlotter,char*);
char*        OPlotterGetTitle                         (OPlotter);
void         OPlotterSetTextScale                     (OPlotter,double);
double       OPlotterGetTextScale                     (OPlotter);
void         OPlotterSetIthAxisAttributesF            (OPlotter,unsigned int,char*,...);
void         OPlotterGetIthAxisAttributesF            (OPlotter,unsigned int,char*,...);

void         OPlotterSetRequireRepresentationFunction (OPlotter,OPlotterSendFunction,void*,int);
int          OPlotterIsRebuildDone                    (OPlotter);
void         OPlotterRequireRepresentation            (OPlotter);
void         OPlotterErase                            (OPlotter);
void         OPlotterClear                            (OPlotter);
void*        OPlotterGetRootNode                      (OPlotter);
void         OPlotterReceiveRepresentationRequest     (OPlotter,CList,CList,CList);
void         OPlotterRepresent                        (OPlotter,CList,CList,CList);
void         OPlotterDoRepresentation                 (OPlotter,CList,CList,CList);
char*        OPlotterGetInformations                  (OPlotter);
void*        OPlotterGetHighlightedBins               (OPlotter,int*,int**,int**);
void         OPlotterHighlightBins                    (OPlotter,void*,int,int*,int*);
int          OPlotterGetAttribute                     (OPlotter,char*,void*,void*,int*);
int          OPlotterDoMethod                         (OPlotter,char*,void*,int,char**,void*,int*);

char**       OLayoutGetNames                          (int*);
OLayout      OLayoutGetIdentifier                     (char*);
char*        OLayoutGetName                           (OLayout);
int          OLayoutIsValid                           (OLayout);

char**       OXY_ModelingGetNames                     (int*);
OXY_Modeling OXY_ModelingGetIdentifier                (char*);
char*        OXY_ModelingGetName                      (OXY_Modeling);
int          OXY_ModelingIsValid                      (OXY_Modeling);

#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOPlotter(0); /**/
#endif

#endif  /*OPlotter_h*/
