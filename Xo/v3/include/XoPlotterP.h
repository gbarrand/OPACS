/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoPlotterP_h
#define XoPlotterP_h

#include <XoPlotter.h>

/* Plot class structure */


typedef struct _XoPlotterClassPart {
       void* extension;
} XoPlotterClassPart;


/* Full class record declaration for Plot class */

typedef struct _XoPlotterClassRec {
	CoreClassPart		  core_class;
	CompositeClassPart	  composite_class;
        XoShapeClassPart           draw_class;
	XoCameraClassPart	  camera_class;
        XoPlotterClassPart        plot_class;
} XoPlotterClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XoPlotterClassRec xoPlotterClassRec;
#ifdef __cplusplus
}
#endif

/* Plot instance record */

typedef enum {
    ObjectTypeHistogram,
    ObjectTypeTuple,
    ObjectTypeScatter
} ObjectType;

typedef struct _ToPlotRecord {
  void*      reference;
  ObjectType type;
  int        to_delete;
} ToPlotRecord;

typedef struct _XoPlotterPart {
/*resources*/
   OLayout         layout;
   OXY_Modeling    xyModeling;
   Boolean         aspectRatio;
   Boolean         wallsVisible;
   Boolean         gridsVisible;
   Boolean         barsVisible;
   char*           textContext;
   char*           gridContext;
   char*           wallContext;
   char*           histogramContext;
   char*           secondHistogramContext;
   char*           functionContext;
   char*           secondFunctionContext;
   char*           scatterContext;
   char*           secondScatterContext;
   char*           plottedObjects;
   Boolean         statisticsVisible;
   Boolean         statisticsShort;
   Boolean         titleEditable;
   String          titlePosition;
   OBarType        binBarType;
   OReceiver       tupleDataReceiver;
   int             numberOfBoundColumns;
   char*           valueColorMapping;
   char*           title;
   char*           textScale;
   char*           cuts;
   char*           functions;
   int             functionDimension;

   char*           xAxisBinding;
   Boolean         xAxisAutomatic;
   Boolean         xAxisLogScale;
   int             xAxisNumberOfBins;
   char*           xAxisMinimum;
   char*           xAxisMaximum;

   char*           yAxisBinding;
   Boolean         yAxisAutomatic;
   Boolean         yAxisLogScale;
   int             yAxisNumberOfBins;
   char*           yAxisMinimum;
   char*           yAxisMaximum;

   char*           zAxisBinding;
   Boolean         zAxisAutomatic;
   Boolean         zAxisLogScale;
   int             zAxisNumberOfBins;
   char*           zAxisMinimum;
   char*           zAxisMaximum;

   Boolean         vAxisAutomatic;
   Boolean         vAxisLogScale;
   int             vAxisNumberOfBins;
   char*           vAxisMinimum;
   char*           vAxisMaximum;
   char*           vAxisTitle;

   OFitType        fitType;
   char*           fitScript;
   int             fitNumberOfParameters;
   char*           fitStartingValues;
   char*           fitSteps;
   char*           fitVariableNames;
/*internal*/
   Widget          wstats;
   Dimension       wstatsLineHeight;
   OPlotter        oplot;
   int             objn;
   ToPlotRecord*   objs;
   OTuple          otuple;
   char*           stuple;
   int             ncut;
   OCut*           lcut;
   int*            acut;
   OFunction*      funcs;
/*zoom stack*/
#define MAX__ZOOM 20
   struct
     {
       double    imin,imax;
       int       inbin;
       double    jmin,jmax;
       int       jnbin;
     } zoom_list[MAX__ZOOM];
   int           zoom_count;
} XoPlotterPart;

/* Full instance record declaration */

typedef struct _XoPlotterRec {
	CorePart		core;
	CompositePart		composite;
  	XoShapePart		draw;
  	XoCameraPart		camera;
        XoPlotterPart           plot;
} XoPlotterRec;

#endif /*XoPlotterP_h*/
