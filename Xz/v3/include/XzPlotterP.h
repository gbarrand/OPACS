/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XzPlotterP_h
#define XzPlotterP_h 

#include <XzPlotter.h>

typedef struct _HplotCommandRec
{
  CommandType type;
  int         id;
  int         varx;
  int         vary;
  int         varz;
  char*       opt;
  char*       opt2;
  double      val,theta,phi;
} HplotCommandRec,*HplotCommand;

typedef struct
{
   void* extension;
} XzPlotterClassPart;

typedef struct _XzPlotterClassRec
{
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ConstraintClassPart	constraint_class;
	XzPlotterClassPart	hplot_class;
} XzPlotterClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XzPlotterClassRec xzPlotterClassRec;
#ifdef __cplusplus
}
#endif

typedef struct
{
/* int              wkId; */
   int              cmdn;
   HplotCommandRec* cmds;
   int              refreshSent;
   int              buildSent;
   HplotCommand     cmd_lego;
   XtCallbackList   collectCallback;
   int              collectIdentifier;
   int              collectBeginPosition;
   int              collectEndPosition;
   XtCallbackList   user_callback;
   int              tmpn;
   int*             tmps;   

   char*            zones;
   char*            plottedObjects;
   char*            histogramContext;
   char*            layout;
   Boolean          statisticsVisible;
   char*            title;
   char*            pageTitle;
   char*            options;
   char*            parameters;
   char*            fitType;
   Boolean          vAxisAutomatic;
   Boolean          vAxisLogScale;
   char*            vAxisMinimum;
   char*            vAxisMaximum;
   char*            vAxisTitle;
   Boolean          gridsVisible;
   Boolean          barsVisible;
   char*            xyModeling;

   char*            commands;
} XzPlotterPart;


typedef struct _XzPlotterRec
{
	CorePart		core;
	CompositePart		composite;
	ConstraintPart		constraint;
  	XzPlotterPart		hplot;
} XzPlotterRec;

#endif /*XzPlotterP_h*/
