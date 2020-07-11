/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifdef HAS_FNAL

#include <math.h>
#include <float.h>
#include <stdlib.h>

/*FNAL*/
#include <X11/Intrinsic.h>
#include <2DHist.h>
#include <H1D.h>
#include <Curves.h>
#include <Scat.h>
#include <3DScat.h>
#include <Cell.h>
#include <Xm/Xm.h>
#include <XY.h>

/*Co*/
#include <CPrinter.h>

/*Xx*/
#include <XWidget.h>

#include <FNAL_Commands.h>

#ifdef __cplusplus
extern "C"{
#endif
static int    fermiLogo   (float,float);
static void   createRects (Widget,CellRect**,int*);
static float* createBins  ();
#ifdef __cplusplus
}
#endif

/***************************************************************************/
void FNAL_AddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("FNAL_example")!=NULL) return; /*done*/

  OShellAddNewCommand (a_osh,"Wo/FNAL_example"       ,FNAL_Execute_FNAL_example);
}
/***************************************************************************/
int FNAL_Execute_FNAL_example (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> FNAL_example <string:Widget>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  widget = XWidgetGetIdentifier (a_args[1]);
  if(widget==NULL)
    {
      CWarnF ("Unknown widget %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }


  /* Create data, labels, and limits for a the 2DHist widget */
       
  if(XtIsSubclass(widget,hist2DWidgetClass))
    { /*From examples/2DHistDemo.c */
#define NUM_X_BINS 20
#define NUM_Y_BINS 20
      h2DHistSetup hist; 
      float data[NUM_X_BINS * NUM_Y_BINS];
      float *dataPoint, x, y;
      int i, j;

      /* Create example data */
      dataPoint = data;
      for (i = 0; i < NUM_X_BINS; i++) {
    	for (j = 0; j < NUM_Y_BINS; j++) {
	  x = -1.0 + i*2. / NUM_X_BINS;   
	  y = -1.0 + j*2. / NUM_Y_BINS;
	  if (fermiLogo(x,y)) {     /* if points within range */
	    *dataPoint++ = 1.0;    
	  } else {
	    *dataPoint++ = 0.0;
	  }
    	}
      }
      
      /* Pass the histogram data to the widget.*/
      hist.nXBins = NUM_X_BINS;
      hist.nYBins = NUM_Y_BINS;
      hist.xMin = -1.0;
      hist.xMax = 1.0;
      hist.yMin = -1.0;
      hist.yMax = 1.0;
      hist.xScaleType = H2D_LINEAR;
      hist.xScaleBase = 0.0;
      hist.yScaleType = H2D_LINEAR;
      hist.yScaleBase = 0.0;
      hist.xLabel = "X Axis Label";
      hist.yLabel = "Y Axis Label";
      hist.zLabel = "Z Axis Label";
      hist.bins = data; 
      hist2DSetHistogram (widget, &hist);
#undef NUM_X_BINS
#undef NUM_Y_BINS
    }      
  else if(XtIsSubclass(widget,h1DWidgetClass))
    { /*From examples/h1DDemo.c */
      /* This function creates histogram data for a plain (non-adaptive)
	 histogram.  The variable bins holds the points for
	 the histogram, the variables uppErr, lowErr hold the
	 points for the upper and lower error bars for the 
	 histogram. When the showErrorBars is FALSE, uppErr and
	 lowErr are NULL, to indicate no error bars.  */
      Boolean showErrorBars = False;
#define NUM_BINS 50
#define X_MIN 0.0
#define X_MAX 100.0
      float bins[NUM_BINS], *bin;
      float *upErr, upErrs[NUM_BINS], *lowErr, lowErrs[NUM_BINS];
      int i, x;
    
      /* Create bin data */
      for (i=0, bin=bins; i < NUM_BINS; i++, bin++)
        *bin = (float)(i > NUM_BINS / 2 ? NUM_BINS - i : i);

      /* Create error bar data */
      if (showErrorBars) {
        for (i=0, bin=bins, upErr=upErrs, lowErr=lowErrs; i < NUM_BINS; i++,
	       bin++, upErr++, lowErr++) {
	  *upErr = *lowErr = *bin * .1;
        }
      }
    
      /* Display the data in the histogram widget */
      H1DSetContents(widget, X_MIN, X_MAX, NUM_BINS, bins, 
		     showErrorBars?upErrs:NULL, showErrorBars?lowErrs:NULL, H1D_RESCALE);
    }
  else if(XtIsSubclass(widget,curvesWidgetClass))
    { /*From examples/curvesDemo.c */
      Boolean isTimeSeries  = TRUE;
      Boolean showErrorBars = TRUE;
#define NUM_POINTS  10
      static float data1[NUM_POINTS] = {1, 5, 12, 6, 18, 4, 6, 9, 14, 13};
      static float data2[NUM_POINTS] = {8, 6, 3, 2, 0, 5, 4, 2, 3, 6};
      static float data3[NUM_POINTS] = {15, 4, 6, 8 ,12, 9, 4, 8, 25, 2};
      static float tErr2data[NUM_POINTS] = {16, 5, 7, 9, 13, 10, 5, 9, 26, 3};
      static float bErr2data[NUM_POINTS] = {14, 3, 5, 7, 11, 8, 3, 7, 24, 1};
      static CurveStruct curves[5] = {
	{"Curve1", NUM_POINTS, SQUARE_MARK, PLAIN_LINE,
	 CURVE_NO_OPTIONS, data1},
	{"Curve2", NUM_POINTS, BLANK_MARK,
	 LINE_2, CURVE_NO_OPTIONS, data2},
	{"Curve3", NUM_POINTS, SOLID_CIRCLE_MARK,
	 LINE_3, CURVE_NO_OPTIONS, data3},
	{"", NUM_POINTS, 0, 0, CURVE_TOP_ERROR, tErr2data},
	{"", NUM_POINTS, 0, 0, CURVE_BOTTOM_ERROR, bErr2data}
      };
      CurvesSetContents (widget, curves, showErrorBars ? 5 : 3,CURVES_RESCALE, isTimeSeries);
#undef NUM_POINTS
    }
  else if(XtIsSubclass(widget,scatWidgetClass))
    { /*From examples/scatDemo.c */
      /* create the points to put in the plot and display them. */
#define NUM_POINTS 10000
      int numPoints = 0;
      float x, y;
      ScatPoint *point, *points;
      points = (ScatPoint *)XtMalloc(sizeof(ScatPoint) * NUM_POINTS);
      point = points;
      srand(1);
      while (numPoints < NUM_POINTS) {
        x = rand() / (RAND_MAX / 2.) - 1.;
        y = rand() / (RAND_MAX / 2.) - 1.;
        if (fermiLogo(x, y)) {
	  point->x = x + 1.;
	  point->y = y + 1.;
	  point->pixel = BlackPixelOfScreen(XtScreen(widget));
	  point++;
	  numPoints++;
        }
      }
      ScatSetContents(widget, points, NUM_POINTS, SCAT_RESCALE);  
      XtFree((char *)points);
#undef NUM_POINTS
    }
  else if(XtIsSubclass(widget,scat3DWidgetClass))
    { /*From examples/3DScatDemo.c */
#define NUM_POINTS 20000
      int numPoints = 0;
      Scat3DPoint *point, *points;
      float x, y, z;
      Scat3DSetAxesNames(widget, "X Axis", "Y Axis", "Z Axis");
      /* Create the points to put in the plot. */
      points = (Scat3DPoint *)XtMalloc(sizeof(Scat3DPoint) * NUM_POINTS);
      point = points;
      srand(1);
      while (numPoints < NUM_POINTS) 
	{
	  x = rand() / (RAND_MAX / 2.) - 1.;
	  y = rand() / (RAND_MAX / 2.) - 1.;
	  z = rand() / (RAND_MAX / 2.) - 1.;
	  if (fermiLogo(x, y) && (z < -.8 || z > .8)) 
	    {
	      point->x = x + 1.;
	      point->y = y + 1.;
	      point->z = z + 1.001; /* make sure it's log scaleable */
	      point++;
	      numPoints++;
	    }
	}
      /* Display the points. */
      Scat3DSetContents(widget, points, numPoints, SCAT3D_RESCALE);
      XtFree((char *)points);
#undef NUM_POINTS
    }
  else if(XtIsSubclass(widget,cellWidgetClass))
    { /*From examples/cellDemo.c */
      CellRect *rects;
      int nRects;
      createRects     (widget, &rects, &nRects); 
      CellSetContents (widget, rects, nRects, CELL_RESCALE);
    }
  else if(XtIsSubclass(widget,xyWidgetClass))
    { /*From examples/xyDemo.c */
      Boolean init = TRUE;
      Boolean showErrorBars = TRUE;
#define NUM_POINTS  20
#define NUM_CURVES  3
      static XYPoint Data[NUM_CURVES][NUM_POINTS];
      static XYErrorBar HBars[NUM_CURVES][NUM_POINTS];
      static XYErrorBar VBars[NUM_CURVES][NUM_POINTS];
      static XYCurve Curves[NUM_CURVES] = {
    	{NULL, XY_SQUARE_MARK, XY_SMALL, XY_2_DOT_DASH,
    	    	0, 1, NUM_POINTS, Data[0], NULL, NULL},
    	{NULL, XY_SQUARE_MARK, XY_LARGE,  XY_FINE_DASH,
    	    	2, 3, NUM_POINTS, Data[1], NULL, NULL},
    	{NULL, XY_X_MARK, XY_SMALL,  XY_MED_FINE_DASH,
    	    	4, 5, NUM_POINTS, Data[2], NULL, NULL}};
      Pixel black = BlackPixelOfScreen(XtScreen(widget));
      int i, j;
      float yVal;
      char name[100];

      for (i=0; i<NUM_CURVES; i++) {
    	Curves[i].markerPixel = black;
    	Curves[i].linePixel = black;
    	sprintf(name, "Curve%d", i);
    	Curves[i].name = XmStringCreateSimple(name);
      }
      
      srand(1);
      for (i=0; i<NUM_CURVES; i++) {
    	for (j=0; j<NUM_POINTS; j++) {
	  Curves[i].points[j].x = j + 2;
	  yVal = rand() / (RAND_MAX / 100) + 6;
	  Curves[i].points[j].y = yVal;
	  HBars[i][j].min = j + 1;
	  HBars[i][j].max = j + 3;
	  VBars[i][j].min = yVal - 5;
	  VBars[i][j].max = yVal + 5;
    	}
      }
      
      for (i=0; i<NUM_CURVES/2; i++) {
    	Curves[i].vertBars = showErrorBars ? VBars[i] : NULL;
    	Curves[i].horizBars = showErrorBars ? HBars[i] : NULL;
    	Curves[i].lineStyle = XY_NO_LINE;
      }
      
      XYSetContents(widget, Curves, NUM_CURVES,init ? XY_RESCALE : XY_RESCALE_AT_MAX);
#undef NUM_POINTS
#undef NUM_CURVES
    }
  else
    {
      CWarnF ("For widget %s, bad widget class in :\n%s\n",XtName(widget),OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
static int fermiLogo (
 float x
,float y
)
/***************************************************************************/
/* 
   From examples/demoUtils.c.

   Simple logical function that draws the Fermi Logo.  Returns 1 (TRUE) or
   0 (FALSE) depending wether we are  on the dark (white) part of the Logo.
   The dimension of the Logo are approximately 1.0 by 1.0, leaving a bit of
   spacing for clarity. 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    int result;
    float xa, ya, x1, y1, yMin, yMax, xx, yy, rOut2;
    static float barThickness = 0.2500,  lengthShort = 0.5417,
                posShort = 0.45,  lengthLong = 0.7500,
                posLong = 0.08333,  radiousIn = 0.166667,
                radiousOut = 0.083333,  xCircleOut = 0.416666,
                xCircleIn = 0.250,  topHeight = 0.80;
/*.........................................................................*/
    /* Assume quadrant symmetry. */
    
    xa = fabs(x);
    ya = fabs(y);
    
    if ( xa > lengthLong )
        return 0;
    if ( ya > topHeight)
        return 0;
    if ( xa < lengthShort && (ya > posShort && ya <(posShort + barThickness)))
        return 1;

        
    /* Assume eight fold symmetry. */
    
    if ( xa > ya ) {
        x1 = xa;
        y1 = ya;
     }
    else {
        x1 = ya;
        y1 = xa;
     }
     
    if (y1 < posLong)
        return 0;
    if (x1 > xCircleOut && y1 > (posLong + barThickness))
        return 0;
        
    /* More complicated case, do the circles... */
    
    if (x1 > xCircleOut)
    	yMax = posLong + barThickness;
    else if (x1 < (xCircleOut - radiousOut * 0.7071068))
    	    yMax = x1;
    else {
    	 xx = x1 - xCircleOut;
    	 rOut2 = radiousOut * radiousOut;
    	 yy = radiousOut - sqrt(rOut2 - xx * xx);
    	 yMax = posLong + barThickness + yy;
    }
    	 
    if (x1 > xCircleIn) 
    	yMin = posLong;
    else {
    	xx = xCircleIn - x1;
    	rOut2 = radiousIn * radiousIn;
    	yy = radiousIn - sqrt(rOut2 - xx * xx);
    	yMin = posLong + yy;
    }
    
    if (y1 < yMin || y1 > yMax)
    	return 0;
    	
    return 1;
}
/***************************************************************************/
static void createRects (
  Widget Cell
,CellRect** rects
,int* nRects
)
/***************************************************************************/
/* 
   Create an array of CellRect structures for the Cell widget to display.
*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    int i, j;
    float *bins;
    float xMin = -1.0, yMin = -1.0, xMax = 1.0, yMax = 1.0;
    float xBinWidth, yBinWidth, xScale, yScale, binValue;
    float minBinValue = FLT_MAX, maxBinValue = -FLT_MAX;
    CellRect *rect;
/*.........................................................................*/
#define NUM_X_BINS 50
#define NUM_Y_BINS 50
    /* Create data in the form an array of histogram bin values */
    bins = createBins();
    
    /* find the range of the data: minBinValue & maxBinValue */
    for (i=0; i<NUM_X_BINS; i++) {
    	for (j=0; j<NUM_Y_BINS; j++) {
    	    binValue = bins[i*NUM_Y_BINS + j];
    	    if (binValue < minBinValue) minBinValue = binValue;
    	    if (binValue > maxBinValue) maxBinValue = binValue;
	}
    }
    
    /* calculate constants used in creating rectangles for cell widget */
    xBinWidth = (xMax - xMin)/NUM_X_BINS;
    yBinWidth = (yMax - yMin)/NUM_Y_BINS;
    xScale = xBinWidth / (maxBinValue - minBinValue);
    yScale = yBinWidth / (maxBinValue - minBinValue);
    *nRects = NUM_X_BINS * NUM_Y_BINS;
    
    /* create the rectangles for the cell widget to draw */
    *rects = (CellRect *)XtMalloc(*nRects * sizeof(CellRect));
    rect = *rects;
    for (i=0; i<NUM_X_BINS; i++) {
    	for (j=0; j<NUM_Y_BINS; j++) {
    	    binValue = bins[i*NUM_Y_BINS + j];
    	    rect->x = xMin + i*xBinWidth + xBinWidth/2;
    	    rect->y = yMin + j*yBinWidth + yBinWidth/2;
    	    rect->dx = (binValue - minBinValue) * xScale;
    	    rect->dy = (binValue - minBinValue) * yScale;
    	    rect->pixel = BlackPixel(XtDisplay(Cell), 0);
    	    rect++;
    	}
    }
    XtFree((char *)bins);
#undef NUM_X_BINS
#undef NUM_Y_BINS
}
/***************************************************************************/
static float *createBins (
)
/***************************************************************************/
/* 
   Create binned data for the cell widget to plot.
   The points will be arranged to display the FermiLab logo.
   First, a two dimensional array is created with twice the
   size of NUM_X_BINS by NUM_Y_BINS (bins2) and another two
   dimensional array of NUM_X_BINS by NUM_Y_BINS (bins). Second,
   four points will be averaged and placed in the bins array.
   A pointer to the beginning of the points is returned. 
*/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i, j;
  float x, y, *bins, *logo, *bin, total, *logoRow1, *logoRow2;
/*.........................................................................*/
#define NUM_X_BINS 50
#define NUM_Y_BINS 50
  bins = (float *)XtMalloc(sizeof(float) * NUM_X_BINS * NUM_Y_BINS);
  logo = (float *)XtMalloc(sizeof(float) * NUM_X_BINS * NUM_Y_BINS * 4);   
    
  /* Create Fermi logo points. */	  
  bin = logo;	    
  for (i = 0; i < (NUM_X_BINS * 2); i++) {
    for (j = 0; j < (NUM_Y_BINS * 2); j++) {
      x = -1.0 + (float)i / NUM_X_BINS;
      y = -1.0 + (float)j / NUM_Y_BINS; 
      if (fermiLogo(x,y)) 
	*bin++ = 1.0; 
      else
	*bin++ = 0.0;
    }
  }
  
  /* Average the four points and place the average into bins. */
  logoRow1 = logo;
  logoRow2 = logo + NUM_Y_BINS * 2;
  bin = bins;
  for (i = 0; i < NUM_X_BINS; i++) {
    for (j = 0; j < NUM_Y_BINS ; j++) { 
      total = 0.0;
      total += *logoRow1++;
      total += *logoRow1++;
      total += *logoRow2++;
      total += *logoRow2++;
      *bin++ = total / 4.;
    }
    logoRow1 += NUM_Y_BINS * 2;
    logoRow2 += NUM_Y_BINS * 2;
  }
  
  XtFree((char *)logo); 
  return (bins);
#undef NUM_X_BINS
#undef NUM_Y_BINS
}


#else /*HAS_FNAL*/
#include <stddef.h>
#endif /*HAS_FNAL*/
