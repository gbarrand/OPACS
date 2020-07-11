/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifdef HAS_X

/*#define DEBUG*/

#include <stdio.h>

#include <math.h>

#include <X11/Xlib.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CColor.h>
#include <CError.h>

#include <glDriver.h>
#include <glDriverX.h>

typedef unsigned long Ulong;
typedef unsigned int  Uint;
typedef unsigned char Uchar;

#ifdef __cplusplus
extern "C"{
#endif
static void  ClearColors               ();
static int   AllocatePoints            (int,int,glContext);
static int   ConvertLineToXPoints      (int,double*,double*,double*);
static void  DrawPolygon               (int,XPoint*,Ulong,glArea);
static void  DrawLines                 (int,XPoint*,Ulong,glLine,int);
static void  DrawSegments              (int,XPoint*,Ulong,glLine,int);
static void  DrawMarkers               (int,XPoint*,Ulong,glMarker,int);
static int   GetMarkerNumberOfSegments (glMarker);
static void  GetMarkerSegments         (glMarker,int,int,int,XSegment*);
static void  InitializeContext         ();
static void  SetupContext              ();
static void  SetContextColor           (Ulong);
static void  SetContextLineWidth       (int);
static void  SetContextLineStyle       (glLine);
static void  SetContextAreaStyle       (glArea);
static void  ChangeContext             ();
static int   GetPixelFromRGB           (Display*,double,double,double,unsigned long*,int*);
static void  ReportError               (char*,int);
#ifdef __cplusplus
}
#endif

#define DBL_INT                 (int)(float)
#define DBL_SHORT               (short)(float)
#define MAX_SHORT               32768.
#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)

#define MAX_COLORS   256
static struct 
{
  Display*  display;
  Colormap  colormap;
  int       vclass;
  Drawable  drawable;
  GC        gc;
  int       windowWidth,windowHeight;
  int       viewportX,viewportY,viewportWidth,viewportHeight;
  int       xpointn;
  XPoint*   xpoints;
  int       xsegmentn;
  XSegment* xsegments;
  int       warnDoneColor,warnDoneShort;
  Pixmap    stipple_hatched_pixmap;
  Pixmap    stipple_checker_pixmap;
  int       colori;
  glColorRecord colors[MAX_COLORS];  
  struct { 
    Ulong         pixel;
    int           width;
    glLine    lineStyle;
    glArea    areaStyle;
    XGCValues     args;
    unsigned long flags;
  } atb;
} Class = {
NULL,0L,0,0L,NULL,
0,0,0,0,0,0, 
0,NULL,
0,NULL,
0,0,   /*warnDone*/
0L,0L, /*pixmap*/
0};
/***************************************************************************/
void glDriverX_ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.display==NULL) return;

  CMemoryFreeBlock   (Class.xpoints);
  Class.xpoints      = NULL;
  Class.xpointn      = 0;

  CMemoryFreeBlock   (Class.xsegments);
  Class.xsegments    = NULL;
  Class.xsegmentn    = 0;

  if(Class.gc!=NULL) XFreeGC (Class.display,Class.gc);
  Class.gc           = NULL;

  if(Class.stipple_hatched_pixmap!=0L) XFreePixmap (Class.display,Class.stipple_hatched_pixmap);
  if(Class.stipple_checker_pixmap!=0L) XFreePixmap (Class.display,Class.stipple_checker_pixmap);
  Class.stipple_hatched_pixmap = 0L;
  Class.stipple_checker_pixmap = 0L;

  ClearColors        ();

  Class.display       = NULL;
  Class.colormap      = 0L;
  Class.vclass        = 0;
  Class.drawable      = 0L;
  Class.warnDoneColor = 0;
  Class.warnDoneShort = 0;

}
/***************************************************************************/
void glDriverX_SetColormap (
 Ulong a_colormap
,int   a_vclass
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_colormap!=Class.colormap) ClearColors ();
  Class.colormap = (Colormap)a_colormap;
  Class.vclass   = a_vclass;
}
/***************************************************************************/
void glDriverX_SetCurrentWindow (
 void* a_display
,Ulong a_drawable
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_display==NULL)  return;
  if(a_drawable==0L)   return;

  if( (Class.display!=NULL) && 
      (Class.gc!=NULL)  
     )   XFreeGC (Class.display,Class.gc);
  Class.gc       = NULL;

  ClearColors     ();

  Class.display   = (Display*)a_display;
  Class.drawable  = (Drawable)a_drawable;
}
/***************************************************************************/
Ulong glDriverX_ConvertRGB_ToPixel (
 double a_red
,double a_green
,double a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     count;
  int     found;
  Ulong   pixel;
  int     status,allocated;
/*.........................................................................*/
  if(Class.display==NULL) return 0L;

  found = 0;
  for(count=0;count<Class.colori;count++)
    {
      if (
           (a_red   == Class.colors[count].red)   &&
           (a_green == Class.colors[count].green) &&
           (a_blue  == Class.colors[count].blue)  
         )
        {
          found = 1;
          break;
	}
    }
  if(found==1) return Class.colors[count].pixel;

  status = GetPixelFromRGB (Class.display,
		            a_red,a_green,a_blue,
			    &pixel,&allocated);
  if(status==0) 
    {
      if(Class.warnDoneColor==0)
	{
	  ReportError         ("glDriverX_ConvertRGB_ToPixel",CErrorConversion);
	  Class.warnDoneColor = 1;
	}
    }

  if(Class.colori>=MAX_COLORS)  {ReportError("glDriverX_ConvertRGB_ToPixel",CErrorFlow);return pixel;}
  Class.colors[Class.colori].red       = a_red;  
  Class.colors[Class.colori].green     = a_green;
  Class.colors[Class.colori].blue      = a_blue;
  Class.colors[Class.colori].pixel     = pixel;
  Class.colors[Class.colori].allocated = allocated;
  Class.colori ++;
#ifdef DEBUG
  printf ("debug : glDriverX_ConvertRGB_ToPixel : %d : %lu : %g %g %g\n",Class.colori,pixel,a_red,a_green,a_blue);
#endif
  return       pixel;
}
/***************************************************************************/
int glDriverX_ConvertPixelToRGB (
 Ulong   a_pixel
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  for(count=0;count<Class.colori;count++)
    {
      if(a_pixel==Class.colors[count].pixel)
        {
	  if(a_red  !=NULL) *a_red   = Class.colors[count].red;
	  if(a_green!=NULL) *a_green = Class.colors[count].green;
	  if(a_blue !=NULL) *a_blue  = Class.colors[count].blue;
          return 1;
	}
    }
  if(a_red  !=NULL) *a_red   = 0.;
  if(a_green!=NULL) *a_green = 0.;
  if(a_blue !=NULL) *a_blue  = 0.;
  return 0;
}
/***************************************************************************/
void glDriverX_SetWindow (
 int a_width
,int a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.windowWidth  = a_width;
  Class.windowHeight = a_height;
}
/***************************************************************************/
void glDriverX_SetViewport (
 int a_x
,int a_y
,int a_width
,int a_height
)
/***************************************************************************/
/*
   a_x, a_y are in gl coordinates.
   - gl origin is bottom-left corner.
   - X  origin is top-left    corner.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XRectangle    rect;
/*.........................................................................*/
  if(Class.display==NULL)  return;
  if(Class.drawable==0L)   return;

  Class.viewportX      = a_x;
  Class.viewportY      = Class.windowHeight - a_height - a_y;
  Class.viewportWidth  = a_width;
  Class.viewportHeight = a_height;
  InitializeContext    ();
  if(Class.gc==NULL)   return;
  rect.x               = 0;
  rect.y               = 0;
  rect.width           = (short)Class.viewportWidth;
  rect.height          = (short)Class.viewportHeight;
  XSetClipRectangles   (Class.display,Class.gc,Class.viewportX,Class.viewportY,&rect,1,Unsorted);

}
/***************************************************************************/
void glDriverX_ClearColorBuffer (
 DColor a_color
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.display==NULL) return;
  if(Class.drawable==0L)  return;
  InitializeContext       ();
  if(Class.gc==NULL)      return;
  SetupContext            ();
  SetContextColor         (a_color->pixel);
  SetContextAreaStyle     (glAreaSolid);
  ChangeContext           ();
  XFillRectangle          (Class.display,Class.drawable,Class.gc,
			   Class.viewportX,Class.viewportY,Class.viewportWidth,Class.viewportHeight);
}
/***************************************************************************/
void glDriverX_DrawPrimitive (
 Uint          a_mode
,int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,glContext     a_atb
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  InitializeContext ();
  if(AllocatePoints(a_mode,a_pointn,a_atb)==0) return;
  switch  (a_mode)
    {
      case GL_POINTS:
	if(ConvertLineToXPoints (a_pointn,a_xs,a_ys,a_zs)==1) 
	  {
	    DrawMarkers    (a_pointn,Class.xpoints,a_atb->color.pixel,a_atb->markStyle,a_atb->width);
	  }
        break;
      case GL_LINE_STRIP:
      case GL_LINE_LOOP:
	if(ConvertLineToXPoints (a_pointn,a_xs,a_ys,a_zs)==1) 
	  {
	    DrawLines    (a_pointn,Class.xpoints,a_atb->color.pixel,a_atb->lineStyle,a_atb->width);
	  }
        break;
      case GL_LINES:
	if(ConvertLineToXPoints (a_pointn,a_xs,a_ys,a_zs)==1) 
	  {
	    DrawSegments (a_pointn,Class.xpoints,a_atb->color.pixel,a_atb->lineStyle,a_atb->width);
	  }
        break;
      case GL_POLYGON:
	if(ConvertLineToXPoints(a_pointn,a_xs,a_ys,a_zs)==1)
          {
                 if(a_atb->polygonMode==GL_FILL)
		   {
		     DrawPolygon    (a_pointn,Class.xpoints,a_atb->color.pixel,a_atb->areaStyle);
		   }
            else if(a_atb->polygonMode==GL_LINE)
	      {
		DrawLines    (a_pointn,Class.xpoints,a_atb->color.pixel,a_atb->lineStyle,a_atb->width);
	      }
	  }
        break;
    }
}
/***************************************************************************/
void glDriverX_DrawBuffer (
 double    a_x
,double    a_y
,int       a_width
,int       a_height
,float     a_xorig
,float     a_yorig
,Uchar*    a_bytes
,glContext a_atb
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    row,col;
  Uchar* bytes;
  int    count;
/*.........................................................................*/
  if(Class.display==NULL)  return;
  if(Class.drawable==0L)   return;
  if(Class.gc==NULL)       return;
  SetupContext        ();
  SetContextColor     (a_atb->color.pixel);
  ChangeContext       ();
  bytes = a_bytes;
  count = 7;
  for(row=0;row<a_height;row++)
    {
      for(col=0;col<a_width;col++)
	{
	  int value;
          if(count<0)
	    {
	      bytes++;
	      count = 7;
	    }
	  value = ((*bytes) >> count) & 0x01;
	  count--;
	  if(value==1)
	    {
	      double     x,y;
	      x          = a_x - (double)a_xorig + (double)col + (double)Class.viewportX;
	      y          = a_y - (double)a_yorig + (double)row + (double)Class.viewportY;
	      XDrawPoint (Class.display,Class.drawable,Class.gc,(int)x,(int)y);
	    }
	}
    }
}
/***************************************************************************/
static int ConvertLineToXPoints (
 int           a_number
,double*       a_xs
,double*       a_ys
,double*       a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        count;
  double     x,y;
  double     xmin,xmax,ymin,ymax;
/*.........................................................................*/
  if(a_number<=0)  return 0;
  x                 = a_xs[0];
  y                 = a_ys[0];
  x                += Class.viewportX;
  y                += Class.viewportY;
  xmax              = xmin = x;
  ymax              = ymin = y;
  if((fabs(x)>=MAX_SHORT)||
     (fabs(y)>=MAX_SHORT))
    {
      if(Class.warnDoneShort==0)
	{
	  CWarn               ("glDriverX: can't convert double to short.\n");
	  Class.warnDoneShort = 1;
	}
      return 0;
    }
  Class.xpoints[0].x = DBL_SHORT x;
  Class.xpoints[0].y = DBL_SHORT y;
  for(count=1;count<a_number;count++)
    { 
      x                 = a_xs[count];
      y                 = a_ys[count];
      x                += Class.viewportX;
      y                += Class.viewportY;
      xmin              = MINIMUM(xmin,x);
      xmax              = MAXIMUM(xmax,x);
      ymin              = MINIMUM(ymin,y);
      ymax              = MAXIMUM(ymax,y);
      if((fabs(x)>=MAX_SHORT)||
	 (fabs(y)>=MAX_SHORT) ) 
	{
	  if(Class.warnDoneShort==0)
	    {
	      CWarn               ("glDriverX: can't convert double to short.\n");
	      Class.warnDoneShort = 1;
	    }
	  return 0;
	}
      Class.xpoints[count].x = DBL_SHORT x;
      Class.xpoints[count].y = DBL_SHORT y;
    }
/*out of viewport*/ /*a true clipping over edges would be better*/
  if(xmin>=(double)(Class.viewportX+Class.viewportWidth))  return 0; 
  if(xmax <(double)Class.viewportX)                        return 0;
  if(ymin>=(double)(Class.viewportY+Class.viewportHeight)) return 0;
  if(ymax <(double)Class.viewportY)                        return 0;
/*ok*/
  a_zs   = NULL;
  return 1;
}
/***************************************************************************/
static void DrawPolygon (
 int        a_pointn
,XPoint*    a_points
,Ulong      a_pixel
,glArea a_style
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.display==NULL)  return;
  if(Class.drawable==0L)   return;
  if(Class.gc==NULL)       return;
  SetupContext        ();
  SetContextColor     (a_pixel);
  SetContextAreaStyle (a_style);
  ChangeContext       ();
  XFillPolygon    (Class.display,Class.drawable,Class.gc,a_points,a_pointn,Nonconvex,CoordModeOrigin);
}
/***************************************************************************/
static void DrawLines (
 int        a_pointn
,XPoint*    a_points
,Ulong      a_pixel
,glLine     a_style
,int        a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.display==NULL)  return;
  if(Class.drawable==0L)   return;
  if(Class.gc==NULL)       return;
  SetupContext        ();
  SetContextColor     (a_pixel);
  SetContextLineStyle (a_style);
  SetContextLineWidth (a_width);
  ChangeContext       ();
  if( (a_pointn==2) &&
      (a_points[0].x == (int)a_points[1].x) &&
      (a_points[0].y == (int)a_points[1].y) 
    )
    {
      XDrawPoint (Class.display,Class.drawable,Class.gc,(int)a_points[0].x,(int)a_points[0].y);
    }
  else
    {
      XDrawLines (Class.display,Class.drawable,Class.gc,a_points,a_pointn,CoordModeOrigin);
    }
}
/***************************************************************************/
static void DrawSegments (
 int        a_pointn
,XPoint*    a_points
,Ulong      a_pixel
,glLine     a_style
,int        a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int  segment_number;
/*.........................................................................*/
  if(Class.display==NULL)  return;
  if(Class.drawable==0L)   return;
  if(Class.gc==NULL)       return;
  segment_number = a_pointn/2;
  for(count=0;count<segment_number;count++)
    {
      Class.xsegments[count].x1   = a_points[2*count].x;
      Class.xsegments[count].y1   = a_points[2*count].y;
      Class.xsegments[count].x2   = a_points[2*count+1].x;
      Class.xsegments[count].y2   = a_points[2*count+1].y;
    }
  SetupContext        ();
  SetContextColor     (a_pixel);
  SetContextLineStyle (a_style);
  SetContextLineWidth (a_width);
  ChangeContext       ();
  XDrawSegments       (Class.display,Class.drawable,Class.gc,Class.xsegments,segment_number);
}
/***************************************************************************/
static void DrawMarkers (
 int        a_pointn
,XPoint*    a_points
,Ulong      a_pixel
,glMarker a_style 
,int        a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int       mark_size;
  int       line_width = 1;
  int       center_x,center_y;
/*.........................................................................*/
  if(Class.display==NULL)  return;
  if(Class.drawable==0L)   return;
  if(Class.gc==NULL)       return;

  mark_size  = a_width <=0 ? 4 : a_width;   
  if(a_style==glMarkerCircle)
    {
      SetupContext        ();
      SetContextColor     (a_pixel);
      SetContextLineStyle (glLineSolid);
      SetContextLineWidth (line_width);
      ChangeContext       ();
      for(count=0;count<a_pointn;count++)
        {
           center_x        = a_points[count].x - mark_size/2;
           center_y        = a_points[count].y - mark_size/2;
           XDrawArc        (Class.display,Class.drawable,Class.gc,center_x,center_y,
                            (unsigned int)mark_size,(unsigned int)mark_size,0,23040);
        }
    }
  else if(a_style==glMarkerDot)
    { 
      SetupContext    ();
      SetContextColor (a_pixel);
      ChangeContext   ();
      XDrawPoints     (Class.display,Class.drawable,Class.gc,a_points,a_pointn,CoordModeOrigin);
    }
  else
    {
      int    ipoint,number;
      number = GetMarkerNumberOfSegments (a_style);
      ipoint = 0;
      for(count=0;count<a_pointn;count++)
        {
           center_x     = a_points[count].x;
           center_y     = a_points[count].y;
           GetMarkerSegments (a_style,mark_size,center_x,center_y,&(Class.xsegments[ipoint]));
           ipoint      += number;
        }
      if(ipoint==0)       return;
      SetupContext        ();
      SetContextColor     (a_pixel);
      SetContextLineStyle (glLineSolid);
      SetContextLineWidth (line_width);
      ChangeContext       ();
      XDrawSegments       (Class.display,Class.drawable,Class.gc,Class.xsegments,ipoint);
    }
}
/***************************************************************************/
static int GetMarkerNumberOfSegments (
 glMarker a_style 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  switch (a_style)
    {
      case glMarkerPlus    :  return 2;
      case glMarkerAsterisk:  return 4;
      case glMarkerCross   :  return 2;
      case glMarkerStar    :  return 3;
      case glMarkerCircle  :  return 0;
      case glMarkerDot     :  return 0;
      default              :  return 1;
    }
}
/***************************************************************************/
static void GetMarkerSegments (
 glMarker a_style 
,int a_size 
,int a_center_x 
,int a_center_y 
,XSegment* a_segment 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XSegment *segment; 
/*.........................................................................*/
  segment  = (XSegment *)a_segment;
  a_size  /= 2;
  a_size   = a_size==0 ? 1 :a_size;
  switch (a_style)
    {
      case glMarkerPlus    :
        segment[0].x1 = (short)(a_center_x - a_size);
        segment[0].y1 = (short)a_center_y ;
        segment[0].x2 = (short)(a_center_x + a_size);
        segment[0].y2 = (short)a_center_y ;
                        
        segment[1].x1 = (short)a_center_x ;
        segment[1].y1 = (short)(a_center_y - a_size);
        segment[1].x2 = (short)a_center_x ;
        segment[1].y2 = (short)(a_center_y + a_size);
        break;
      case glMarkerAsterisk:
        segment[0].x1 = (short)(a_center_x - a_size);
        segment[0].y1 = (short)a_center_y ;
        segment[0].x2 = (short)(a_center_x + a_size);
        segment[0].y2 = (short)a_center_y ;

        segment[1].x1 = (short)a_center_x ;
        segment[1].y1 = (short)(a_center_y - a_size);
        segment[1].x2 = (short)a_center_x ;
        segment[1].y2 = (short)(a_center_y + a_size);

        segment[2].x1 = (short)(a_center_x - DBL_INT (a_size * .707));
        segment[2].y1 = (short)(a_center_y - DBL_INT (a_size * .707));
        segment[2].x2 = (short)(a_center_x + DBL_INT (a_size * .707));
        segment[2].y2 = (short)(a_center_y + DBL_INT (a_size * .707));

        segment[3].x1 = (short)(a_center_x + DBL_INT (a_size * .707));
        segment[3].y1 = (short)(a_center_y - DBL_INT (a_size * .707));
        segment[3].x2 = (short)(a_center_x - DBL_INT (a_size * .707));
        segment[3].y2 = (short)(a_center_y + DBL_INT (a_size * .707));
        break;
      case glMarkerCross       :
        segment[0].x1 = (short)(a_center_x - a_size);
        segment[0].y1 = (short)(a_center_y - a_size);
        segment[0].x2 = (short)(a_center_x + a_size);
        segment[0].y2 = (short)(a_center_y + a_size);

        segment[1].x1 = (short)(a_center_x + a_size);
        segment[1].y1 = (short)(a_center_y - a_size);
        segment[1].x2 = (short)(a_center_x - a_size);
        segment[1].y2 = (short)(a_center_y + a_size);
        break;
      case glMarkerStar    :
        segment[0].x1 = (short)a_center_x;
        segment[0].y1 = (short)(a_center_y + a_size);
        segment[0].x2 = (short)(a_center_x - DBL_INT (a_size * .6));
        segment[0].y2 = (short)(a_center_y - DBL_INT (a_size * .5));

        segment[1].x1 = segment[0].x2 ;
        segment[1].y1 = segment[0].y2 ;
        segment[1].x2 = (short)(a_center_x + DBL_INT (a_size * .6));
        segment[1].y2 = (short)(a_center_y - DBL_INT (a_size * .5));

        segment[2].x1 = segment[1].x2;
        segment[2].y1 = segment[1].y2;
        segment[2].x2 = segment[0].x1;
        segment[2].y2 = segment[0].y1;
        break;
      default                  :
        segment[0].x1 = (short)a_center_x;
        segment[0].y1 = (short)a_center_y;
        segment[0].x2 = (short)a_center_x;
        segment[0].y2 = (short)a_center_y;
        break;
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void InitializeContext (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.gc!=NULL)       return;

  if(Class.display==NULL) return;

  Class.atb.flags            = 0L;

  Class.atb.args.background  = XBlackPixelOfScreen(XDefaultScreenOfDisplay(Class.display));
  Class.atb.flags           |= GCBackground;

  Class.atb.pixel            = XWhitePixelOfScreen(XDefaultScreenOfDisplay(Class.display));
  Class.atb.args.foreground  = XWhitePixelOfScreen(XDefaultScreenOfDisplay(Class.display));
  Class.atb.flags           |= GCForeground;

  Class.atb.width           = 0;
  Class.atb.args.line_width = 0;
  Class.atb.flags          |= GCLineWidth;

  Class.atb.lineStyle       = glLineSolid;
  Class.atb.args.line_style = LineSolid;
  Class.atb.flags          |= GCLineStyle;

  Class.atb.areaStyle       = glAreaSolid;
  Class.atb.args.fill_style = FillSolid;
  Class.atb.flags          |= GCFillStyle;

  Class.gc                  = (GC) XCreateGC (Class.display,
                                              XDefaultRootWindow(Class.display),
                                              Class.atb.flags,&(Class.atb.args));

}
/***************************************************************************/
static void SetupContext (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.atb.flags = 0L;
}
/***************************************************************************/
static void SetContextColor (
 Ulong a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_pixel==Class.atb.pixel) return;
  Class.atb.pixel            = a_pixel;
  Class.atb.args.foreground  = a_pixel;
  Class.atb.flags           |= GCForeground;
}
/***************************************************************************/
static void SetContextLineWidth (
 int a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_width==Class.atb.width) return;
  Class.atb.width           = a_width;
  Class.atb.args.line_width = a_width<=1 ? 0 : a_width;
  Class.atb.flags          |= GCLineWidth;
}
/***************************************************************************/
static void SetContextLineStyle (
 glLine a_style 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char        line_style_dashed        [] = {6,3};
  static char        line_style_dotted        [] = {3,6};
  static char        line_style_dashed_dotted [] = {6,3,3,3};
/*.........................................................................*/
  if(a_style==Class.atb.lineStyle) return;
  Class.atb.lineStyle = a_style; 
  switch(a_style)
    {
      case glLineSolid              :Class.atb.args.line_style = LineSolid;
                                    Class.atb.flags |= GCLineStyle;
                                    break;
      case glLineDashed             :Class.atb.args.line_style = LineOnOffDash;
                                    XSetDashes(Class.display,Class.gc,0,line_style_dashed       ,2);
                                    Class.atb.flags |= GCLineStyle;
                                    break;
      case glLineDotted             :Class.atb.args.line_style = LineOnOffDash;
                                    XSetDashes(Class.display,Class.gc,0,line_style_dotted       ,2);
                                    Class.atb.flags |= GCLineStyle;
                                    break;
      case glLineDashDotted         :Class.atb.args.line_style = LineOnOffDash;
                                    XSetDashes(Class.display,Class.gc,0,line_style_dashed_dotted,4);
                                    Class.atb.flags |= GCLineStyle;
                                    break;
     }
}
/***************************************************************************/
static void SetContextAreaStyle (
 glArea a_style 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define stipple_hatched_width 16
#define stipple_hatched_height 16
  static unsigned char stipple_hatched_bits[] = {
   0x00, 0x7f, 0x80, 0x3f, 0xc0, 0x1f, 0xe0, 0x0f, 0xf0, 0x07, 0xf8, 0x03,
   0xfc, 0x01, 0xfe, 0x00, 0x7f, 0x00, 0x3f, 0x80, 0x1f, 0xc0, 0x0f, 0xe0,
   0x07, 0xf0, 0x03, 0xf8, 0x01, 0xfc, 0x00, 0xfe};

#define stipple_checker_width  16
#define stipple_checker_height 16
  static unsigned char        stipple_checker_bits [] = {
0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,
0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,
0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,
0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff};
/*.........................................................................*/
  if(a_style==Class.atb.areaStyle) return;
  Class.atb.areaStyle = a_style; 
  switch(a_style)
     {
      case glAreaSolid         :Class.atb.args.fill_style = FillSolid;
                               Class.atb.flags |= GCFillStyle;
                               break;
      case glAreaHatched       :Class.atb.args.fill_style = FillOpaqueStippled;
                               if(Class.stipple_hatched_pixmap==0L)
				 {
				   Class.stipple_hatched_pixmap = XCreateBitmapFromData (
				   Class.display,
				   XDefaultRootWindow(Class.display),
                                   (char*)stipple_hatched_bits,stipple_hatched_width,stipple_hatched_height);
				 }
                               XSetStipple(Class.display,Class.gc,Class.stipple_hatched_pixmap);
                               Class.atb.flags |= GCFillStyle;
                               break;
      case glAreaChecker:       Class.atb.args.fill_style = FillOpaqueStippled;
                               if(Class.stipple_checker_pixmap==0L)
				 {
				   Class.stipple_checker_pixmap = XCreateBitmapFromData (
                                   Class.display,
                                   XDefaultRootWindow(Class.display),
                                   (char*)stipple_checker_bits,stipple_checker_width,stipple_checker_height);
				 }
                               XSetStipple(Class.display,Class.gc,Class.stipple_checker_pixmap);
                               Class.atb.flags |= GCFillStyle;
                               break;
     }
}
/***************************************************************************/
static void ChangeContext (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.atb.flags==0L) return;
  XChangeGC (Class.display,Class.gc,Class.atb.flags,&(Class.atb.args));
}
/***************************************************************************/
static int GetPixelFromRGB (
 Display* This 
,double a_red
,double a_green
,double a_blue
,Ulong* a_pixel
,int* a_alloc
)
/***************************************************************************/
/* read_write visual class: PseudoColor, GrayScale, DirectColor.           */
/* read_only  visual class: TrueColor, StaticColor, StaticGray.            */
/* Could allocate a read_only cell in a read_write, read_only visual class.*/
/* Could allocate a read_write cell only in read_write visual class.       */
/* StaticGray + (depth=1) is monochrome                                    */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           vclass;
  Colormap      cmap;
  int           is_grey;
  double        grey;
  XColor        xcolor;
/*.........................................................................*/
  if(a_pixel!=NULL) *a_pixel     = 0L;
  if(a_alloc!=NULL) *a_alloc     = 0;
  if(This==NULL)         return 0;
  if(Class.colormap==0L) return 0;

  vclass        = Class.vclass;
  cmap          = Class.colormap;

  is_grey       = ((vclass==GrayScale) || (vclass==StaticGray) ) ? 1 : 0;
  grey          = is_grey==1 ?  CColorConvertRGB_ToGrey(a_red,a_green,a_blue) : 0.;

  xcolor.pixel  = 0L;
  xcolor.red    = (unsigned short) ((is_grey==1 ? grey : a_red)   * 0xffff);
  xcolor.green  = (unsigned short) ((is_grey==1 ? grey : a_green) * 0xffff);
  xcolor.blue   = (unsigned short) ((is_grey==1 ? grey : a_blue)  * 0xffff);
  if(XAllocColor(This,cmap,&xcolor)==0) /*Could be done on read_only/read_write visual class.*/
    {                                 /*Cell is taken from a common pool. It is read only.*/
      /*Color not found. Try to allocate a private color cell.*/
      if( (vclass==TrueColor)   || 
          (vclass==StaticColor) || 
          (vclass==StaticGray)
        ) /*Viusal class is read_only.*/
        {
          return   0;
        }
      else
        {
	  if(XAllocColorCells (This,cmap,False,0L,0,&(xcolor.pixel),1)==0) /*read/write cell*/
	    { /*Alloc fail.*/
	      return   0;
	    }
	  else
	    {
	      xcolor.flags = DoRed|DoGreen|DoBlue;
	      XStoreColor  (This,cmap,&xcolor);
	      if(a_pixel!=NULL)  *a_pixel     = xcolor.pixel;
	      if(a_alloc!=NULL)  *a_alloc     = 1;
	    }
	}
    }
  else
    {
      if(a_pixel!=NULL) *a_pixel = xcolor.pixel;
      if(a_alloc!=NULL) *a_alloc = 0;
    }
/*
  printf ("debug:%d %lu:%g %g %g:%ld:%d %d %d\n",vclass,cmap,a_red,a_green,a_blue,*a_pixel,xcolor.red,xcolor.green,xcolor.blue);
*/
  return   1;
}
/***************************************************************************/
static int AllocatePoints (
 int  a_mode
,int  a_pointn
,glContext a_atb
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_pointn==0) return 0;
  if(a_pointn>Class.xpointn)
    {
      Class.xpoints  = Class.xpointn!=0 ? 
	(XPoint*)CMemoryChangeBlockSize (Class.xpoints,a_pointn * sizeof(XPoint)) :
	(XPoint*)CMemoryAllocateBlock   (              a_pointn * sizeof(XPoint));
      if (Class.xpoints==NULL)
	{
	  CMemoryFreeBlock      (Class.xpoints);
	  Class.xpoints = NULL;
	  Class.xpointn = 0;
	  return        0;
	}
      Class.xpointn  = a_pointn;
    }

  if( (a_mode==GL_LINES) || (a_mode==GL_POINTS) )
    { 
      int      segmentn;
      segmentn = (a_mode==GL_POINTS) ? GetMarkerNumberOfSegments(a_atb->markStyle) * a_pointn : a_pointn/2;
      if(segmentn>Class.xsegmentn)
	{
	  Class.xsegments  = Class.xsegmentn!=0 ? 
	      (XSegment*)CMemoryChangeBlockSize (Class.xsegments, segmentn * sizeof(XSegment)) :
	      (XSegment*)CMemoryAllocateBlock   (                 segmentn * sizeof(XSegment));
	  if (Class.xsegments==NULL)
	    {
	      CMemoryFreeBlock (Class.xsegments);
              Class.xsegments  = NULL;
	      Class.xsegmentn  = 0;
	      return           0;
	    }
	  Class.xsegmentn  = segmentn;
	}
    }
 return 1;
}
/***************************************************************************/
static void ClearColors (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  Ulong        pixels[MAX_COLORS];
  int          pixeli = 0;
/*.........................................................................*/
  if(Class.display==NULL) return;
  if(Class.colormap==0L)  return;
#ifdef DEBUG
  printf ("debug : ClearColors.\n");
#endif
  for(count=0;count<Class.colori;count++)
    {
      if(Class.colors[count].allocated==1)
	{
	  pixels[pixeli] = Class.colors[count].pixel;
	  pixeli ++;
	}
    }
  XFreeColors  (Class.display,Class.colormap,pixels,pixeli,0L);
  Class.colori = 0;
}
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("GL","glDriverX",a_name,a_code);
}

#else  /*HAS_X*/
#include <stddef.h>
#include <CPrinter.h>
#include <glDriverX.h>
typedef unsigned long Ulong;
void glDriverX_ClearClass () {CWarn("glDriverX_ClearClass : dummy.\n");}
void glDriverX_SetCurrentWindow (void* a_display,unsigned long a_drawable) {
  CWarn      ("glDriverX_SetCurrentWindow : dummy.\n");
  a_display  = NULL;
  a_drawable = 0L;
}
Ulong glDriverX_ConvertRGB_ToPixel (double a_red,double a_green,double a_blue) {
  CWarn   ("glDriverX_ConvertRGB_ToPixel : dummy.\n");
  a_red   = 0.;
  a_green = 0.;
  a_blue  = 0.;
  return  0L;
}
void glDriverX_SetWindow (int a_width,int a_height) {
  CWarn   ("glDriverX_SetWindow : dummy.\n");
  a_width  = 0;
  a_height = 0;
}
void glDriverX_SetViewport (int a_x,int a_y,int a_width,int a_height) {
  CWarn    ("glDriverX_SetViewport : dummy.\n");
  a_x      = 0;
  a_y      = 0;
  a_width  = 0;
  a_height = 0;
}
void drXClearFrame (DColor a_color) {
  CWarn    ("drXClearFrame : dummy.\n");
  a_color  = 0;
}
void glDriverX_DrawPrimitive (int a_mode,int a_pointn,double* a_xs,double* a_ys,double* a_zs,glContext a_atb) {
  CWarn    ("glDriverX_DrawPrimitive : dummy.\n");
  a_mode   = 0;
  a_pointn = 0;
  a_xs     = NULL;
  a_ys     = NULL;
  a_zs     = NULL;
  a_atb    = 0;
}

#endif /*HAS_X*/
