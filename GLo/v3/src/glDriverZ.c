/* ULTRIX: File glDriverZ.c must be compiled without optimisation. */

/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <CMemory.h>

#include <gldef.h>

#include <ZPolygon.h>
#include <ZBuffer.h>

#include <ZPointP.h>

#include <glDriverZ.h>

typedef unsigned long Ulong;
typedef unsigned int  Uint;
typedef unsigned char Uchar;

#ifdef __cplusplus
extern "C"{
#endif
static int  AllocatePoints          (int);
static int  ConvertPolygonToZPoints (int,double*,double*,double*,ZPlane);
static int  ConvertLinesToZPoints   (int,double*,double*,double*);
#ifdef __cplusplus
}
#endif

#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)

static struct 
{
  ZPointRecord* zpoints;
  int           zpointn;
  int           windowWidth,windowHeight;
  int           viewportX,viewportY,viewportWidth,viewportHeight;
} Class = {NULL,0,0,0,0,0,0,0};
/***************************************************************************/
void glDriverZ_ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ZBufferClearClass  ();
  CMemoryFreeBlock   (Class.zpoints);
  Class.zpoints      = NULL;
  Class.zpointn      = 0;
}
/***************************************************************************/
Ulong glDriverZ_ConvertRGB_ToPixel (
 double a_red
,double a_green
,double a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Ulong  pixel = 1L;
/*.........................................................................*/
  a_red   = 0.;
  a_green = 0.;
  a_blue  = 0.;
  return  pixel;
}
/***************************************************************************/
void glDriverZ_SetWindow (
 int a_width
,int a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/*printf ("debug:glDriverZ_SetWindow:%d %d.\n",a_width,a_height);*/
  Class.windowWidth  = a_width;
  Class.windowHeight = a_height;
}
/***************************************************************************/
void glDriverZ_SetViewport (
 int a_x
,int a_y
,int a_width
,int a_height
)
/***************************************************************************/
/*
   a_x, a_y are in gl coordinates.
   - gl      origin is bottom-left corner.
   - Zbuffer origin is top-left    corner.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ZPixel* data;
  int     zbw,zbh;
/*.........................................................................*/
/*printf ("debug:glDriverZ_SetViewport:%d %d %d %d.\n",a_x,a_y,a_width,a_height);*/
  Class.viewportX      = a_x;
  Class.viewportY      = Class.windowHeight - a_height - a_y;
  Class.viewportWidth  = a_width;
  Class.viewportHeight = a_height;
  data    = ZBufferGetColorBuffer(&zbw,&zbh);
  if(  (data==NULL) ||
       ((Class.viewportX + Class.viewportWidth>zbw) || (Class.viewportY + Class.viewportHeight > zbh))
    )
    {
      ZBufferChangeSize     (Class.viewportX + Class.viewportWidth,Class.viewportY + Class.viewportHeight);
    }
  ZBufferSetClippingRegion         (Class.viewportX,Class.viewportY,Class.viewportWidth,Class.viewportHeight);
/*printf ("debug:glDriverZ_SetViewport:end.\n");*/
}
/***************************************************************************/
void glDriverZ_ClearColorBuffer (
 DColor a_color
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ZBufferClearColorBuffer ((ZPixel)a_color->pixel);
}
/***************************************************************************/
void glDriverZ_ClearDepthBuffer (
 double a_depth
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ZBufferClearDepthBuffer (a_depth);
}
/***************************************************************************/
void glDriverZ_DrawPrimitive (
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
  ZPlaneRecord     plane;
/*.........................................................................*/
  if(AllocatePoints(a_pointn)==0) return;
  switch (a_mode)
    {
      case GL_POINTS:
	if(ConvertLinesToZPoints (a_pointn,a_xs,a_ys,a_zs)==1) 
	  ZBufferDrawMarkers    (a_pointn,Class.zpoints,(ZPixel)a_atb->color.pixel);
        break;
      case GL_LINE_STRIP:
      case GL_LINE_LOOP:
	if(ConvertLinesToZPoints (a_pointn,a_xs,a_ys,a_zs)==1) 
	  ZBufferDrawLines    (a_pointn,Class.zpoints,(ZPixel)a_atb->color.pixel);
        break;
      case GL_LINES:
	if(ConvertLinesToZPoints (a_pointn,a_xs,a_ys,a_zs)==1) 
	  ZBufferDrawSegments (a_pointn,Class.zpoints,(ZPixel)a_atb->color.pixel);
        break;
      case GL_POLYGON:
	if(ConvertPolygonToZPoints(a_pointn,a_xs,a_ys,a_zs,&plane)==1)
          {
                 if(a_atb->polygonMode==GL_FILL)
	      ZBufferDrawPolygon (a_pointn,Class.zpoints,&plane,(ZPixel)a_atb->color.pixel);
            else if(a_atb->polygonMode==GL_LINE)
	      ZBufferDrawPolygonWithEdge (a_pointn,Class.zpoints,&plane,(ZPixel)a_atb->color.pixel);
	  }
        break;
    }
}
/***************************************************************************/
void glDriverZ_DrawBuffer (
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
	  int   value;
          if(count<0)
	    {
	      bytes++;
	      count = 7;
	    }
	  value = ((*bytes) >> count) & 0x01;
	  count--;
	  if(value==1)
	    {
	      double           x,y;
	      ZPointRecord     zpoint;
	      x                = a_x - (double)a_xorig + (double)col + (double)Class.viewportX;
	      y                = a_y - (double)a_yorig + (double)row + (double)Class.viewportY;
	      ZPointInitialize (&zpoint,x,y,ZBufferGetZ_Maximum());
	      ZBufferDrawPoint (&zpoint,(ZPixel)a_atb->color.pixel);
	    }
	}
    }
}
/***************************************************************************/
Ulong* glDriverZ_ReadPixels (
 int   a_x
,int   a_y
,int   a_width
,int   a_height
)
/***************************************************************************/
/*
  In fact ZPixel is an unsigned int.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     row,col;
  Ulong*  items;
  Ulong*  itema;
/*.........................................................................*/
  itema = items = (Ulong*)CMemoryAllocateBlock ( (a_width * a_height) * sizeof(Ulong));
  if(items==NULL) return NULL;
  for(row=0;row<a_height;row++)
    {
      for(col=0;col<a_width;col++)
	{
	  int    x,y;
	  x      = a_x + col + Class.viewportX;
	  y      = a_y + row + Class.viewportY;
	  *itema = (Ulong)ZBufferGetPixel (x,y);
	  itema++;
	}
    }
  return items;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int ConvertPolygonToZPoints(
 int     a_number
,double* a_xs
,double* a_ys
,double* a_zs
,ZPlane  a_plane
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double     x,y,z;
  double     xmin,xmax,ymin,ymax;
  double     zmin,zmax;
  ZPointRecord  zpoint_1;
/*.........................................................................*/
  x                 = a_xs[0] + (double)Class.viewportX;
  y                 = a_ys[0] + (double)Class.viewportY;
  z                 = a_zs[0];
  xmax              = x;
  xmin              = x;
  ymax              = y;
  ymin              = y;
  zmax              = z;
  zmin              = z;
  ZPointInitialize  (Class.zpoints,x,y,z);
  a_plane->A        = a_plane->B = a_plane->C = a_plane->D = 0.;
  a_plane->zmin     = a_plane->zmax = 0.;
  zpoint_1          = Class.zpoints[0];
  for(count=1;count<a_number;count++)
    { 
      ZPointRecord      zpoint;
      x                 = a_xs[count] + (double)Class.viewportX;
      y                 = a_ys[count] + (double)Class.viewportY;
      z                 = a_zs[count];
      xmin              = MINIMUM(xmin,x);
      xmax              = MAXIMUM(xmax,x);
      ymin              = MINIMUM(ymin,y);
      ymax              = MAXIMUM(ymax,y);
      zmin              = MINIMUM(zmin,z);
      zmax              = MAXIMUM(zmax,z);
      ZPointInitialize  (Class.zpoints+count,x,y,z);
      zpoint            = Class.zpoints[count];
      a_plane->A       += (zpoint_1.z+zpoint.z) * (zpoint.y - zpoint_1.y);
      a_plane->B       += (zpoint_1.x+zpoint.x) * (zpoint.z - zpoint_1.z);
      a_plane->C       += (zpoint_1.y+zpoint.y) * (zpoint.x - zpoint_1.x);
      zpoint_1          = zpoint;
    }
/*out of screen*/  /*a true clipping over edges would be better*/
  if(xmin>=(double)(Class.viewportX+Class.viewportWidth))  return 0; 
  if(xmax <(double)Class.viewportX)                        return 0;
  if(ymin>=(double)(Class.viewportY+Class.viewportHeight)) return 0;
  if(ymax <(double)Class.viewportY)                        return 0;
/* plane */
  a_plane->A        /= -2.;
  a_plane->B        /= -2.;
  a_plane->C        /= -2.;
  a_plane->D         =  -  ( a_plane->A * zpoint_1.x + a_plane->B * zpoint_1.y + a_plane->C * zpoint_1.z);
  if(a_plane->C>=0.) return 0; /*back facing or by the edge*/
  a_plane->zmin      = zmin;
  a_plane->zmax      = zmax;
/*ok*/
  return 1;
}
/***************************************************************************/
static int ConvertLinesToZPoints (
 int     a_number
,double* a_xs
,double* a_ys
,double* a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double     x,y,z;
  double     xmin,xmax,ymin,ymax;
/*.........................................................................*/
  x                 = a_xs[0] + (double)Class.viewportX;
  y                 = a_ys[0] + (double)Class.viewportY;
  z                 = a_zs[0];
  xmax              = x;
  xmin              = x;
  ymax              = y;
  ymin              = y;
  ZPointInitialize  (Class.zpoints,x,y,z);
  for(count=1;count<a_number;count++)
    { 
      double            x,y,z;
      x                 = a_xs[count] + (double)Class.viewportX;
      y                 = a_ys[count] + (double)Class.viewportY;
      z                 = a_zs[count];
      xmin              = MINIMUM(xmin,x);
      xmax              = MAXIMUM(xmax,x);
      ymin              = MINIMUM(ymin,y);
      ymax              = MAXIMUM(ymax,y);
      ZPointInitialize  (Class.zpoints+count,x,y,z);
    }
/*out of screen*/ /*a true clipping over edges would be better*/
  if(xmin>=(double)(Class.viewportX+Class.viewportWidth))  return 0; 
  if(xmax <(double)Class.viewportX)                        return 0;
  if(ymin>=(double)(Class.viewportY+Class.viewportHeight)) return 0;
  if(ymax <(double)Class.viewportY)                        return 0;
/*ok*/
  return 1;
}
/***************************************************************************/
static int AllocatePoints (
 int  a_pointn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_pointn==0) return 0;
  if(a_pointn>Class.zpointn)
    {
      Class.zpoints  = Class.zpointn!=0 ? 
	  (ZPointRecord*)CMemoryChangeBlockSize (Class.zpoints, a_pointn * sizeof(ZPointRecord)) :
	  (ZPointRecord*)CMemoryAllocateBlock   (               a_pointn * sizeof(ZPointRecord));
      if (Class.zpoints==NULL)
	{
	  CMemoryFreeBlock      (Class.zpoints);
	  Class.zpoints = NULL;
	  Class.zpointn = 0;
	  return        0;
	}
      Class.zpointn = a_pointn;
    }
  return 1;
}
