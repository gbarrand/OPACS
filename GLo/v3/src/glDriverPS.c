/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>

#include <CPS.h>
#include <CMemory.h>
#include <CError.h>
#include <CPrinter.h>

#include <gldef.h>

#include <glDriverPS.h>

typedef unsigned long Ulong;
typedef unsigned int  Uint;
typedef unsigned char Uchar;

#ifdef __cplusplus
extern "C"{
#endif
static void  DrawPolygon        (int,double*,double*,double*,DColor,glArea);
static void  DrawLines          (int,double*,double*,double*,DColor,glLine,int);
static void  DrawSegments       (int,double*,double*,double*,DColor,glLine,int);
static void  DrawMarkers        (int,double*,double*,double*,DColor,glMarker,int);
static void  SetColor           (DColor);
static void  PutMarker          (int,double);
static void  ReportError        (char*,int);
#ifdef __cplusplus
}
#endif

#define PS_STYLE(a_style)      {switch(a_style)     {case glLineSolid      :CPS_PutInStreamF("ss ") ;break;\
                                                     case glLineDashed     :CPS_PutInStreamF("sd ") ;break;\
                                                     case glLineDotted     :CPS_PutInStreamF("so ") ;break;\
                                                     case glLineDashDotted :CPS_PutInStreamF("sdo ");break;}}
#define PS_NEWPATH             CPS_PutInStreamF("n ")
#define PS_STROKE              CPS_PutInStreamF("s ")
#define PS_FILL                CPS_PutInStreamF("f ")
#define PS_CLOSEPATH           CPS_PutInStreamF("cl ")
#define PS_WIDTH(a_x)          CPS_PutLineWidthInStream(a_x)
#define PS_CAP(a_x)            CPS_PutInStreamF ("%1d lc ",a_x)
#define PS_CIRCLE(a_x,a_y,a_r) CPS_PutInStreamF ("%.2f %.2f %.2f 0 360 arc s ",a_x,a_y,a_r)
#define PS_RLINETO(a_x,a_y)    CPS_PutInStreamF ("%.2f %.2f rl ",a_x,a_y)
#define PS_RMOVE(a_x,a_y)      CPS_PutInStreamF ("%.2f %.2f rm ",a_x,a_y)
#define PS_MOVE(a_x,a_y)       CPS_PutInStreamF ("%.2f %.2f m ",a_x,a_y)
#define PS_FONT(a_x,a_y)       CPS_PutInStreamF ("df [%.2f  0 0 %.2f  0 0] mf ",a_x,a_y)

#define MAX_PATH_POINT 100

#define MAX_COLORS 256
static struct 
{
  int       gsave;
  int       gsave_done;
  double    viewportX,viewportY,viewportWidth,viewportHeight;
  int       colori;
  glColorRecord colors[MAX_COLORS];  
} Class = {0,0,0.,0.,0.,0.,0};
/***************************************************************************/
void glDriverPS_ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.colori    = 0;
  if(Class.gsave!=0) CWarnF("glDriverPS: bad gsave/grestore balance: %d.\n",Class.gsave);
  Class.gsave     = 0;
}
/***************************************************************************/
void glDriverPS_GetRGBs (
 int*     a_colorn
,double** a_reds
,double** a_greens
,double** a_blues
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Ulong    colorn;
  double*  reds   = NULL;
  double*  greens = NULL;
  double*  blues  = NULL;
/*.........................................................................*/
  if( (a_colorn==NULL) && 
      (a_reds==NULL)   && 
      (a_greens==NULL) && 
      (a_blues==NULL) ) {Class.colori = 0;return;}

  if(a_colorn!=NULL)  *a_colorn = 0;
  if(a_reds!=NULL)    *a_reds   = NULL;
  if(a_greens!=NULL)  *a_greens = NULL;
  if(a_blues!=NULL)   *a_blues  = NULL;

  colorn        = Class.colori;
  if(colorn==0) return;  

  reds   = (double*)CMemoryAllocateBlock((size_t)colorn * sizeof(double));  
  greens = (double*)CMemoryAllocateBlock((size_t)colorn * sizeof(double));  
  blues  = (double*)CMemoryAllocateBlock((size_t)colorn * sizeof(double));  
  if( (reds==NULL) || (greens==NULL) || (blues==NULL) ) 
    {     
      CMemoryFreeBlock  (reds);
      CMemoryFreeBlock  (greens);
      CMemoryFreeBlock  (blues);
      colorn    = 0;
      reds      = greens = blues = NULL;
    }
  else
    {
       int count;
      for(count=0;count<Class.colori;count++)
	{
          reds  [count] = Class.colors[count].red;
          greens[count] = Class.colors[count].green;
          blues [count] = Class.colors[count].blue;
	}
    }

  if(a_colorn!=NULL)  *a_colorn = (int)colorn;
  if(a_reds!=NULL)    *a_reds   = reds;
  if(a_greens!=NULL)  *a_greens = greens;
  if(a_blues!=NULL)   *a_blues  = blues;
}
/***************************************************************************/
Ulong glDriverPS_ConvertRGB_ToPixel (
 double a_red
,double a_green
,double a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int     found;
  Ulong   pixel;
/*.........................................................................*/
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
  if(Class.colori>=MAX_COLORS)  {ReportError("glDriverPS_ConvertRGB_ToPixel",CErrorFlow);return 0L;}
  Class.colors[Class.colori].red       = a_red;  
  Class.colors[Class.colori].green     = a_green;
  Class.colors[Class.colori].blue      = a_blue;
  pixel                                = Class.colori;
  Class.colors[Class.colori].pixel     = pixel;
  Class.colors[Class.colori].allocated = 0;
  Class.colori ++;
  return       pixel;
}
/***************************************************************************/
int glDriverPS_ConvertPixelToRGB (
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
void glDriverPS_RestoreGraphicState (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_PutRestoreStateInStream ();
  Class.gsave--;
  Class.gsave_done = 0;
}
/***************************************************************************/
void glDriverPS_SetViewport (
 int a_x
,int a_y
,int a_width
,int a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.gsave_done!=0)
    {
      CPS_PutRestoreStateInStream ();
      Class.gsave--;
      Class.gsave_done = 0;
    }
  Class.viewportX       = (double)a_x;
  Class.viewportY       = (double)a_y;
  Class.viewportWidth   = (double)a_width;
  Class.viewportHeight  = (double)a_height;

  CPS_PutSaveStateInStream         ();
  Class.gsave++;
  Class.gsave_done = 1;

/* trans(a_x,a_y) * scale * trans * point */
  CPS_PutTranslationInStream      ((double)Class.viewportX,(double)Class.viewportY);
  CPS_PutScaleInStream      (1.,-1.);
  CPS_PutTranslationInStream      (0.,-(double)Class.viewportHeight);
}
/***************************************************************************/
void glDriverPS_ClearColorBuffer (
 DColor a_color
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_PutBackgroundInStream  (a_color->red,a_color->green,a_color->blue,Class.viewportWidth,Class.viewportHeight); /*clip*/
}
/***************************************************************************/
void glDriverPS_DrawPrimitive (
 Uint          a_mode
,int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,glContext          a_atb
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  switch  (a_mode)
    {
      case GL_POINTS:
	DrawMarkers    (a_pointn,a_xs,a_ys,a_zs,&(a_atb->color),a_atb->markStyle,a_atb->width);
        break;
      case GL_LINE_STRIP:
      case GL_LINE_LOOP:
	DrawLines    (a_pointn,a_xs,a_ys,a_zs,&(a_atb->color),a_atb->lineStyle,a_atb->width);
        break;
      case GL_LINES:
	DrawSegments (a_pointn,a_xs,a_ys,a_zs,&(a_atb->color),a_atb->lineStyle,a_atb->width);
        break;
      case GL_POLYGON:
             if(a_atb->polygonMode==GL_FILL)
	  DrawPolygon    (a_pointn,a_xs,a_ys,a_zs,&(a_atb->color),a_atb->areaStyle);
        else if(a_atb->polygonMode==GL_LINE)
  	  DrawLines    (a_pointn,a_xs,a_ys,a_zs,&(a_atb->color),a_atb->lineStyle,a_atb->width);
        break;
    }
}
/***************************************************************************/
void glDriverPS_DrawBuffer (
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
	      x           = a_x - (double)a_xorig + (double)col;
	      y           = a_y - (double)a_yorig + (double)row;
	      PS_NEWPATH;
	      PS_MOVE     (x,y);
	      PS_RLINETO  (0.,0.);
	      SetColor    (&(a_atb->color));
	      PS_WIDTH    (1);
	      PS_CAP      (1);
	      PS_STROKE;
	    }
	}
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void DrawPolygon (
 int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,DColor        a_color
,glArea        a_style
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double     x,y;
  double     ox,oy;
  double     fx,fy;
/*.........................................................................*/
  PS_NEWPATH; 
  x                 = a_xs[0];
  y                 = a_ys[0];
  PS_MOVE          (x,y);
  ox               = x;
  oy               = y;
  fx               = x;
  fy               = y;
  for (count=1;count<a_pointn;count++)
    {
      x          = a_xs[count];
      y          = a_ys[count];
      PS_RLINETO (x - ox,y - oy);
      ox         = x;
      oy         = y;
    }
  if ((x==fx)&&(y==fy)) PS_CLOSEPATH;
  SetColor (a_color);
  PS_FILL;
  a_style = glAreaSolid;
  a_zs    = NULL;
}
/***************************************************************************/
static void DrawLines (
 int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,DColor        a_color
,glLine        a_style 
,int           a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double     x,y;
  double     ox,oy;
  double     fx,fy;
/*.........................................................................*/
  x                 = a_xs[0];
  y                 = a_ys[0];
  PS_MOVE          (x,y);
  ox               = x;
  oy               = y;
  fx               = x;
  fy               = y;
  for (count=1;count<a_pointn;count++)
    {  
      x           = a_xs[count];
      y           = a_ys[count];
      PS_RLINETO (x - ox,y - oy);
      ox         = x;
      oy         = y;
    }
  if ((x==fx)&&(y==fy)) PS_CLOSEPATH;
  SetColor (a_color);
  PS_WIDTH (a_width);
  PS_CAP   (1);
  PS_STYLE (a_style)
  PS_STROKE;
  a_zs    = NULL;
}
/***************************************************************************/
static void DrawSegments (
 int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,DColor        a_color
,glLine        a_style 
,int           a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int        segment_number;
  double     x,y;
  double     ox,oy;
/*.........................................................................*/
  segment_number = a_pointn/2;
  if(segment_number<=0) return;

  x                 = a_xs[0];
  y                 = a_ys[0];
  PS_MOVE          (x,y);
  ox               = x;
  oy               = y;

  for(count=0;count<segment_number;count++)
    {
      double     x1,y1,x2,y2;
      x1         = a_xs[2*count];
      y1         = a_ys[2*count];
      PS_RMOVE   (x1 - ox,y1 - oy);
      x2         = a_xs[2*count+1];
      y2         = a_ys[2*count+1];
      PS_RLINETO (x2 - x1,y2 - y1);
      ox         = x2;
      oy         = y2;
    }
  SetColor       (a_color);
  PS_WIDTH       (a_width);
  PS_CAP         (1);
  PS_STYLE       (a_style)
  PS_STROKE;      
  a_zs    = NULL;
}
/***************************************************************************/
static void DrawMarkers (
 int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,DColor        a_color
,glMarker      a_style 
,int           a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double      x,y;
  double      mark_size;
  int         icount;
/*.........................................................................*/
  mark_size  = (double)(a_width <=0 ? 1 : a_width);   
  mark_size *= .6;
  if(a_style==glMarkerCircle)
    {
      PS_NEWPATH;
      icount = 1;
      for(count=0;count<a_pointn;count++)
        {
	  x          = a_xs[count];
	  y          = a_ys[count];
          PS_CIRCLE  (x,y,mark_size);
          if(icount==MAX_PATH_POINT)
            {
              SetColor (a_color);
              PS_WIDTH (1);
              PS_CAP   (1);
              PS_STROKE;
              icount     = 1;
              if(count!=a_pointn-1) PS_NEWPATH;
            }
          else icount++;
        }
      SetColor (a_color);
      PS_WIDTH (1);
      PS_CAP   (1);
      PS_STROKE;
    }
  else
    {
      PS_NEWPATH;
      icount = 1;
      for(count=0;count<a_pointn;count++)
        {
	  x           = a_xs[count];
	  y           = a_ys[count];
          PS_MOVE     (x,y);
          PutMarker (a_style,mark_size);
          if(icount==MAX_PATH_POINT)
            {
              SetColor (a_color);
              PS_WIDTH (1);
              PS_CAP   (1);
              PS_STROKE;
              icount     = 1;
              if(count!=a_pointn-1) PS_NEWPATH;
            }
          else icount++;
        }
      SetColor (a_color);
      PS_WIDTH (1);
      PS_CAP   (1);
      PS_STROKE;
    }
  a_zs    = NULL;
}
/***************************************************************************/
/******** POSTSCRIPT *******************************************************/
/***************************************************************************/
/***************************************************************************/
static void SetColor (
 DColor a_color
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_PutRGB_InStream (a_color->red,a_color->green,a_color->blue);
}
/***************************************************************************/
static void PutMarker (
 int a_style 
,double a_size 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  switch (a_style)
    {
      case glMarkerPlus    :
	CPS_PutMarkerSizeInStream (a_size);
        CPS_PutInStreamF   ("plus ");
        break;
      case glMarkerAsterisk:
	CPS_PutMarkerSizeInStream (a_size);
        CPS_PutInStreamF   ("asterisk ");
        break;
      case glMarkerCross       :
	CPS_PutMarkerSizeInStream (a_size);
        CPS_PutInStreamF   ("cross ");
        break;
      case glMarkerStar    :
	CPS_PutMarkerSizeInStream (a_size);
        CPS_PutInStreamF   ("star ");
        break;
      default                  :
        PS_RLINETO  (0.,0.);
        break;
    }
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
  CErrorHandle ("GL","glDriverPS",a_name,a_code);
}
