/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoPlotter_h
#define XoPlotter_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _XoPlotterClassRec    *XoPlotterWidgetClass;
typedef struct _XoPlotterRec         *XoPlotterWidget;

/* XoPlotter */
#define XoNlayout                "layout"
#define XoNaspectRatio           "aspectRatio"
#define XoNwallsVisible          "wallsVisible"
#define XoNgridsVisible          "gridsVisible"
#define XoNtextContext           "textContext"
#define XoNwallContext           "wallContext"
#define XoNgridContext           "gridContext"
#define XoNhistogramContext       "histogramContext"
#define XoNsecondHistogramContext "secondHistogramContext"
#define XoNfunctionContext       "functionContext"
#define XoNsecondFunctionContext "secondFunctionContext"
#define XoNscatterContext        "scatterContext"
#define XoNsecondScatterContext  "secondScatterContext"
#define XoNplottedObjects        "plottedObjects"
#define XoNstatisticsVisible     "statisticsVisible"
#define XoNstatisticsShort       "statisticsShort"
#define XoNtitleEditable         "titleEditable"
#define XoNtitlePosition         "titlePosition"
#define XoNbinBarType            "binBarType"
#define XoNtupleDataReceiver     "tupleDataReceiver"
#define XoNnumberOfBoundColumns  "numberOfBoundColumns"
#define XoNfunctionDimension     "functionDimension"
#define XoNvalueColorMapping     "valueColorMapping"
#define XoNtitle                 "title"
#define XoNtextScale             "textScale"
#define XoNcuts                  "cuts"
#define XoNfunctions             "functions"
#define XoNtuple                 "tuple"
#define XoNxyModeling            "xyModeling"
#define XoNbarsVisible           "barsVisible"
/* AxeX */
#define XoNxAxisBinding          "xAxisBinding"
#define XoNxAxisNumberOfBins     "xAxisNumberOfBins"
#define XoNxAxisMinimum          "xAxisMinimum"
#define XoNxAxisMaximum          "xAxisMaximum"
#define XoNxAxisAutomatic        "xAxisAutomatic"
#define XoNxAxisLogScale         "xAxisLogScale"
/* AxeY */
#define XoNyAxisBinding          "yAxisBinding"
#define XoNyAxisNumberOfBins     "yAxisNumberOfBins"
#define XoNyAxisMinimum          "yAxisMinimum"
#define XoNyAxisMaximum          "yAxisMaximum"
#define XoNyAxisAutomatic        "yAxisAutomatic"
#define XoNyAxisLogScale         "yAxisLogScale"
/* AxeZ */
#define XoNzAxisBinding          "zAxisBinding"
#define XoNzAxisNumberOfBins     "zAxisNumberOfBins"
#define XoNzAxisMinimum          "zAxisMinimum"
#define XoNzAxisMaximum          "zAxisMaximum"
#define XoNzAxisAutomatic        "zAxisAutomatic"
#define XoNzAxisLogScale         "zAxisLogScale"
/* plot AxeV */
#define XoNvAxisNumberOfBins     "vAxisNumberOfBins"
#define XoNvAxisMinimum          "vAxisMinimum"
#define XoNvAxisMaximum          "vAxisMaximum"
#define XoNvAxisAutomatic        "vAxisAutomatic"
#define XoNvAxisLogScale         "vAxisLogScale"
#define XoNvAxisTitle            "vAxisTitle"

#define XoNfitType               "fitType"
#define XoNfitScript             "fitScript"
#define XoNfitNumberOfParameters "fitNumberOfParameters"
#define XoNfitStartingValues     "fitStartingValues"
#define XoNfitSteps              "fitSteps"
#define XoNfitVariableNames      "fitVariableNames"

#define XoRLayout               "OLayout"
#define XoRReceiver             "OReceiver"
#define XoRBinBarType           "OBar"
#define XoRFitType              "OFitType"
#define XoRXY_Modeling          "OXY_Modeling"

#include <OPlotter.h>
#ifdef __cplusplus
extern "C"{
#endif
extern WidgetClass                     xoPlotterWidgetClass;

char**   XoPlotterGetResourceOptions        (char*,int*);
void     XoPlotterClearClass                ();
OPlotter XoPlotterGetPlotter                (Widget);
void*    XoPlotterGetTuple                  (Widget);
void     XoPlotterMagnifyCollectedBins      (Widget);
void     XoPlotterUndoMagnification         (Widget);
void     XoPlotterRestoreFirstMagnification (Widget);
void     XoPlotterRequireRepresentation     (Widget);
void     XoPlotterUpdateNow                 (Widget);
void     XoPlotterSetLayout                 (Widget);
void     XoPlotterSetPlottedObjects         (Widget,char*);
void     XoPlotterAddPopupEntry             (Widget,char*,XtCallbackProc,XtPointer);
void     XoPlotterAddPopupSeparator         (Widget,char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoPlotter(0); /**/
#endif

#endif /*XoPlotter_h*/
