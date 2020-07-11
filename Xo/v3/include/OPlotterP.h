/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OPlotterP_h
#define OPlotterP_h

#include <OPlotter.h>

typedef struct _OPlotterRecord
{
  char*                name;
  OColormap            colormap;
  ONode                goplot;
  ONode                gobins;
  ONode                gofunc;
  ONode                goback;
  ONode                gogrid;
  ONode                gobar;
  ONode                gotitle;
  ONode                goColorMapping;
  ONode                goaxis[OAxisMaximumNumber],goaxisv;
  OLayout              layout;
  OXY_Modeling         xyModeling;
  int                  pickable;
  int                  layoutAutomated;
  OAxis                axis[OAxisMaximumNumber],value;
/*Modeling*/
  char*                wallContext;
  char*                gridContext;
  char*                textContext;
  char*                histogramContext;
  char*                secondHistogramContext;
  char*                functionContext;
  char*                secondFunctionContext;
  char*                scatterContext;
  char*                secondScatterContext;
  char*                valueColorMapping;
  char*                title;
  double               textScale;
  int                  gridsVisible;
  int                  wallsVisible;
  int                  drawTitle;
  int                  titlePosition;  
  int                  scatterModeling;
  int                  barsVisible;
/* for OPlotter associated to a widget */
  OPlotterSendFunction sendRebuildProc;
  void*                sendRebuildArg;
  int                  sendRebuildDone;
} OPlotterRecord;

#endif /*OPlotterP_h*/
