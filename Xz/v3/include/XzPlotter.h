/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XzPlotter_h
#define XzPlotter_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _XzPlotterClassRec *XzPlotterWidgetClass;
typedef struct _XzPlotterRec      *XzPlotterWidget;

#define XzNcollectCallback      "collectCallback"
#define XzNcollectIdentifier    "collectIdentifier"
#define XzNcollectBeginPosition "collectBeginPosition"
#define XzNcollectEndPosition   "collectEndPosition"
#define XzNuserCallback         "userCallback"

#define XzNcommands             "commands"

#define XzNplottedObjects       "plottedObjects"
#define XzNzones                "zones"
#define XzNstatisticsVisible    "statisticsVisible"
#define XzNlayout               "layout"
#define XzNhistogramContext     "histogramContext"
#define XzNtitle                "title"
#define XzNpageTitle            "pageTitle"
#define XzNoptions              "options"
#define XzNparameters           "parameters"
#define XzNfitType              "fitType"
#define XzNvAxisLogScale        "vAxisLogScale"
#define XzNvAxisMinimum         "vAxisMinimum"
#define XzNvAxisMaximum         "vAxisMaximum"
#define XzNvAxisAutomatic       "vAxisAutomatic"
#define XzNvAxisTitle           "vAxisTitle"
#define XzNgridsVisible         "gridsVisible"
#define XzNbarsVisible          "barsVisible"
#define XzNxyModeling           "xyModeling"


typedef struct
{
   int         reason;
   XEvent*     event;
   int         id;
   int         beg;
   int         end;
 } XzPlotterCallbackStruct;

#define XzCR_NONE        0
#define XzCR_COLLECT     1

#ifdef __cplusplus
extern "C"{
#endif
extern  WidgetClass         xzPlotterWidgetClass;

void    XzPlotterDrawHistogram                (Widget,int);
void    XzPlotterDrawManhattanPlot            (Widget,int,double,double);
void    XzPlotterDrawTuple                    (Widget,int,int,int);
void    XzPlotterClear                        (Widget);
void    XzPlotterSetParameter                 (Widget,char*,double);
void    XzPlotterSetOption                    (Widget,char*);
void    XzPlotterSetTitle                     (Widget,char*);
void    XzPlotterSetPageTitle                 (Widget,char*);
void    XzPlotterSetZones                     (Widget,int,int,int,char*);
void    XzPlotterAddCommand                   (Widget,char*);
char*   XzPlotterGetCollectedBinsInformations (Widget);
void    XzPlotterClearClass                   ();
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXzPlotter(0); /**/
#endif

#endif /*XzPlotter_h*/
