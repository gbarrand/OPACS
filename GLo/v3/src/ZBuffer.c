/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <float.h> /*FLT_MAX*/

#include <CMemory.h>

#include <ZPolygon.h>

#include <ZPointP.h>

#include <ZBuffer.h>

typedef void (*WriteFunction)(ZPos,ZPos,double,ZPos);
typedef void (*WritePointFunction)(ZPos,ZPos,double);

#ifdef __cplusplus
extern "C"{
#endif
static void   WriteLine          (ZPoint,ZPoint);
static void   ScanLine           (ZPos,ZPos,double,ZPos,ZPos,double,WriteFunction);
static void   WritePoint         (ZPos,ZPos,double);
static void   WriteEdgePoint     (ZPos,ZPos,double);
static void   WriteScanLine      (int,int,int);
static void   Write1             (ZPos,ZPos,double,ZPos);
static void   Write2             (ZPos,ZPos,double,ZPos);
static void   Write3             (ZPos,ZPos,double,ZPos);
static void   Write4             (ZPos,ZPos,double,ZPos);
#ifdef __cplusplus
}
#endif

#define MINIMUM(a,b) ((a)<(b)?a:b)
#define MAXIMUM(a,b) ((a)>(b)?a:b)
#define WriteZPoint(a_point)  Class.writePoint((a_point)->x,(a_point)->y,(a_point)->z)

typedef float ZReal;
/*-HUGE_VAL does not work on some system (exa: ULTRIX)*/
#define ZREAL_HUGE FLT_MAX 

static struct 
{
  ZReal*         zbuffer;
  ZPixel*        zimage;
  int            zbw,zbh;
  ZPos           begX,begY,endX,endY;
  ZPixel         pixel;
  double         planeAC;
  double         planeBC;
  double         planeDC;
  ZReal          zmin,zmax;
  int            edge;
  int            zboundPrec;
  WritePointFunction writePoint;
} Class = {NULL,NULL,0,0,0,0,0,0, 0L, 0.,0.,0.,0.,0.,0,10,WritePoint};
/***************************************************************************/
void ZBufferClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock         (Class.zbuffer);
  CMemoryFreeBlock         (Class.zimage);
  Class.zbuffer   = NULL;
  Class.zimage    = NULL;
  Class.zbw       = 0;
  Class.zbh       = 0;
  ZPolygonClearClass ();
}
/***************************************************************************/
int ZBufferChangeSize  (
 int a_width
,int a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.zbuffer &&  (Class.zbw==a_width)  && (Class.zbh==a_height)  ) return 0;

  if(Class.zbuffer!=NULL)
    {
      CMemoryFreeBlock       (Class.zbuffer);
      Class.zbuffer = NULL;
      CMemoryFreeBlock       (Class.zimage);
      Class.zimage  = NULL;
    }
  if( (a_width<=0) || (a_height<=0) ) 
    {
      Class.zbuffer       = NULL;
      Class.zimage        = NULL;
      Class.zbw           = 0;
      Class.zbh           = 0;
    } 
  else 
    {
/*    printf ("debug:ZBufferChangeSize:%d %d\n",a_width,a_height);*/
      Class.zbw           = a_width;
      Class.zbh           = a_height;
      Class.zbuffer       = (ZReal*)  CMemoryAllocateBlock   ( (Class.zbw * Class.zbh) * sizeof(ZReal));
      if(Class.zbuffer!=NULL)
	{
	  Class.zimage        = (ZPixel*) CMemoryAllocateBlock   ( (Class.zbw * Class.zbh) * sizeof(ZPixel) );
	  if(Class.zimage==NULL)   
	    {
	      CMemoryFreeBlock       (Class.zbuffer);
	      Class.zbuffer = NULL;
	    }
	}
    }

/*Init buffer done by further call to ZBufferErase. */
/*
  if( (Class.zbuffer!=NULL) && (Class.zimage!=NULL) )
    {
       int     count;
       ZReal*  zbuffer;
       ZPixel* zimage;
      int     size;
      size    = Class.zbw * Class.zbh;
      zbuffer = Class.zbuffer;
      zimage  = Class.zimage;
      for(count=0;count<size;count++,zbuffer++,zimage++)
	{
	  *zimage   = 0;
	  *zbuffer  = - ZREAL_HUGE;
      }
    }
*/

  ZBufferSetClippingRegion     (0,0,Class.zbw,Class.zbh);
  ZPolygonClearClass ();
  return           1;
}
/***************************************************************************/
double ZBufferGetZ_Maximum (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return ZREAL_HUGE;
}
/***************************************************************************/
ZPixel* ZBufferGetColorBuffer  (
 int* a_width
,int* a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_width!=NULL)  *a_width  = Class.zbw;
  if(a_height!=NULL) *a_height = Class.zbh;
  return       Class.zimage;
}
/***************************************************************************/
void ZBufferClearColorBuffer (
 ZPixel a_pixel
)
/***************************************************************************/
/* Erase acoording clip region.                                            */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ZPos row,col;
/*.........................................................................*/
  if(Class.zbuffer==NULL) return;
/*printf("debug:ZBufferClearColorBuffer: %u.\n",a_pixel);*/
  for(row=Class.begY;row<=Class.endY;row++)
    {
       ZPixel* zimage;
       long    offset;
      offset      = row * Class.zbw + Class.begX;
      zimage      = Class.zimage  + offset;
      for(col=Class.begX;col<=Class.endX;col++,zimage++)
	{
	  *zimage = a_pixel;
      }
    }
}
/***************************************************************************/
void ZBufferClearDepthBuffer (
 double a_depth
)
/***************************************************************************/
/* Erase acoording clip region.                                            */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   ZPos row,col;
/*.........................................................................*/
  if(Class.zbuffer==NULL) return;
/*printf("debug:ZBufferClearDepthBuffer: %g.\n",a_depth);*/
  for(row=Class.begY;row<=Class.endY;row++)
    {
       ZReal*  zbuff;
       long    offset;
      offset      = row * Class.zbw + Class.begX;
      zbuff       = Class.zbuffer + offset;
      for(col=Class.begX;col<=Class.endX;col++,zbuff++)
	{
/*	  *zbuff  = a_depth;*/
	  *zbuff  = - ZREAL_HUGE;
      }
    }
  a_depth = 0.;
}
/***************************************************************************/
ZPixel ZBufferGetPixel (
 int a_x
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.zimage==NULL) return 0;
  return *(Class.zimage + a_y * Class.zbw + a_x);
}
/***************************************************************************/
ZPixel ZBufferGetClippedPixel (
 int a_x
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.zimage==NULL) return 0;
  if((a_x<Class.begX) || (a_x>Class.endX))  return 0;
  if((a_y<Class.begY) || (a_y>Class.endY))  return 0;
  return *(Class.zimage + a_y * Class.zbw + a_x);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void ZPointInitialize (
 ZPoint This
,double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(sizeof(long)==sizeof(double)) /*long 64 bits, double 64 bits, OK*/
    {
      This->x = (long) a_x;
      This->y = (long) a_y;
    }
  else                             /*long 32 bits, double 64 bits*/
    {
      This->x = (long)((float)a_x);
      This->y = (long)((float)a_y);
    }
  This->z = a_z;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void ZBufferSetClippingRegion (
 int a_x
,int a_y
,int a_width
,int a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/*  printf ("debug:ZBufferSetClippingRegion:%d %d %d %d.\n",a_x,a_y,a_width,a_height);*/

  Class.begX      = a_x;
  Class.begY      = a_y;
  Class.endX      = a_x + a_width  - 1;
  Class.endY      = a_y + a_height - 1;

  if(Class.begX<0) Class.begX = 0;
  if(Class.begY<0) Class.begY = 0;
  if(Class.endX>(Class.zbw-1)) Class.endX = Class.zbw-1;
  if(Class.endY>(Class.zbh-1)) Class.endY = Class.zbh-1;

/*  printf ("debug:ZBufferSetClippingRegion:beg:%d %d end:%d %d.\n",Class.begX,Class.endX,Class.begY,Class.endY);*/

}
/***************************************************************************/
void ZBufferDrawPoint (
 ZPoint This
,ZPixel a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(Class.zbuffer==NULL) return;
  Class.pixel        = a_pixel;
  Class.writePoint   = WritePoint;
  WriteZPoint        (This);
}
/***************************************************************************/
void ZBufferDrawLine (
 ZPoint a_beg
,ZPoint a_end
,ZPixel a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_beg==NULL)         return;
  if(a_end==NULL)         return;
  if(Class.zbuffer==NULL) return;
  Class.pixel        = a_pixel;
  Class.writePoint   = WritePoint;
  WriteLine          (a_beg,a_end);
}
/***************************************************************************/
void ZBufferDrawLines (
 int a_number
,ZPointRecordList a_list 
,ZPixel a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_list==NULL)        return;
  if(Class.zbuffer==NULL) return;

  Class.pixel        = a_pixel;
  Class.writePoint   = WritePoint;

  for(count=1;count<a_number;count++)
     WriteLine (&(a_list[count-1]),&(a_list[count]));

}
/***************************************************************************/
void ZBufferDrawSegments (
 int a_number
,ZPointRecordList a_list 
,ZPixel a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int segment_number;
/*.........................................................................*/
  if(a_list==NULL)        return;
  if(Class.zbuffer==NULL) return;

  Class.pixel        = a_pixel;
  Class.writePoint   = WritePoint;

  segment_number = a_number/2;
  for(count=0;count<segment_number;count++)
     WriteLine (&(a_list[2*count]),&(a_list[2*count+1]));

}
/***************************************************************************/
void ZBufferDrawMarkers (
 int a_number
,ZPointRecordList a_list 
,ZPixel a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_list==NULL)        return;
  if(Class.zbuffer==NULL) return;

  Class.pixel        = a_pixel;
  Class.writePoint   = WritePoint;

  for(count=0;count<a_number;count++)
    {
      WriteZPoint (&(a_list[count]));
    }

}
/***************************************************************************/
void ZBufferDrawPolygon (
 int a_number
,ZPointRecordList a_list 
,ZPlane a_plane
,ZPixel a_pixel
)
/***************************************************************************/
/* 
  Assume a_list is closed.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double              dz;
/*.........................................................................*/
  if(a_list==NULL)        return;
  if(Class.zbuffer==NULL) return;

  if(a_number<3)         return;

  if( (a_plane==NULL) || (a_plane->C==0.))  return; /*polygone seen from the edge*/
  if(Class.zboundPrec<0) Class.zboundPrec = 0;

  Class.pixel        = a_pixel;
  Class.planeAC      = a_plane->A/a_plane->C;
  Class.planeBC      = a_plane->B/a_plane->C;
  Class.planeDC      = a_plane->D/a_plane->C;

/*if this polygon A is quite perpandicular to screen and close to an other B than |dz| 
  then some pixel of A could overwrite pixel of B. Your have then to give a lower Class.zboundPrec */
  dz                  = Class.zboundPrec * (a_plane->zmax - a_plane->zmin)/100.;
  Class.zmin         = (ZReal)(a_plane->zmin - dz);
  Class.zmax         = (ZReal)(a_plane->zmax + dz);

/*printf ("debug:ZBufferDrawPolygon: min max:%g %g\n",Class.zmin,Class.zmax);*/

  ZPolygonScan          (a_number,a_list,0,WriteScanLine);

}
/***************************************************************************/
void ZBufferDrawPolygonWithEdge (
 int a_number
,ZPointRecordList a_list 
,ZPlane a_plane
,ZPixel a_pixel
)
/***************************************************************************/
/* 
  Assume a_list is closed.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double              dz;
   int        count;
/*.........................................................................*/
  if(a_list==NULL)         return;
  if(Class.zbuffer==NULL) return;

  if(a_number<3)         return;

  if( (a_plane==NULL) || (a_plane->C==0.))  return; /*polygone seen from the edge*/
  if(Class.zboundPrec<0) Class.zboundPrec = 0;

  Class.pixel        = a_pixel;
  Class.planeAC      = a_plane->A/a_plane->C;
  Class.planeBC      = a_plane->B/a_plane->C;
  Class.planeDC      = a_plane->D/a_plane->C;

/*if this polygon A is quite perpandicular to screen and close to an other B than |dz| 
  then some pixel of A could overwrite pixel of B. Your have then to give a lower Class.zboundPrec */
  dz                  = Class.zboundPrec * (a_plane->zmax - a_plane->zmin)/100.;
  Class.zmin         = (ZReal)(a_plane->zmin - dz);
  Class.zmax         = (ZReal)(a_plane->zmax + dz);

/*draw edge*/
  Class.pixel        = a_pixel;
  Class.writePoint   = WriteEdgePoint;

/*some pixel could be out of range [plane->zmin,plane->zmax]*/
  for(count=1;count<a_number;count++)
    WriteLine (&(a_list[count-1]),&(a_list[count]));

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void WriteLine (
 ZPoint a_beg
,ZPoint a_end
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ZPos       dx,dy;
  double     dz;
/*.........................................................................*/
  dx         = a_end->x - a_beg->x;
  dy         = a_end->y - a_beg->y;
  dz         = a_end->z - a_beg->z;

/*
  6  2
 5     1     
 7     3
  8  4
*/

  if( (dx==0) && (dy==0) )
    { 
      WriteZPoint  (a_beg);
      WriteZPoint  (a_end);
    }
  else if(dx==0)
    {
      if(dy>0)
        ScanLine ( a_beg->y, a_beg->x,a_beg->z, dy, dx, dz,Write2);
      else
        ScanLine ( a_end->y, a_end->x,a_end->z,-dy, dx,-dz,Write2);
    }
  else if(dx>0)
    {
           if((0<=dy) && (dy<=dx))  /*1*/
        ScanLine ( a_beg->x, a_beg->y,a_beg->z, dx, dy, dz,Write1);
      else if(dx<dy)                /*2*/
        ScanLine ( a_beg->y, a_beg->x,a_beg->z, dy, dx, dz,Write2);
      else if((-dx<=dy) && (dy<0) ) /*3*/
        ScanLine ( a_beg->x, a_beg->y,a_beg->z, dx,-dy, dz,Write3);
      else if(dy<-dx)               /*4*/
        ScanLine ( a_end->y, a_end->x,a_end->z,-dy, dx,-dz,Write4);
    }
  else /*dx<0*/
    {
           if((0<=dy) && (dy<=-dx)) /*5*/
        ScanLine ( a_end->x, a_end->y,a_end->z,-dx, dy,-dz,Write3);
      else if(-dx<dy)               /*6*/
        ScanLine ( a_beg->y, a_beg->x,a_beg->z, dy,-dx, dz,Write4);
      else if((dx<=dy) && (dy<0) )  /*7*/
        ScanLine ( a_end->x, a_end->y,a_end->z,-dx,-dy,-dz,Write1);
      else if(dy<dx)                /*8*/
        ScanLine ( a_end->y, a_end->x,a_end->z,-dy,-dx,-dz,Write2);
    }

}
/***************************************************************************/
static void ScanLine (
 ZPos a_x
,ZPos a_y
,double a_z
,ZPos a_dx
,ZPos a_dy
,double a_dz
,WriteFunction a_proc
)
/***************************************************************************/
/*
 Mid point algorithm
 assume 0<dx    0<=dy<=dx
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ZPos    beg,end;
  double  incz;
/*.........................................................................*/
  end    = a_x + a_dx;
  beg    = a_y;
  incz   = a_dz/a_dx; 
  if(a_dy==0)
    {
      a_proc (a_x,a_y,a_z,beg);
      while       (a_x<end)
	{
	  a_x         ++;
	  a_z         += incz;
	  a_proc (a_x,a_y,a_z,beg);
	}
    }
  else if(a_dy==a_dx)
    {
      a_proc (a_x,a_y,a_z,beg);
      while       (a_x<end)
	{
	  a_x         ++;
	  a_y         ++;
	  a_z         += incz;
	  a_proc (a_x,a_y,a_z,beg);
	}
    }
  else
    {
      ZPos        d,incrE,incrNE;
      d           = 2 * a_dy - a_dx;
      incrE       = 2 * a_dy;
      incrNE      = 2 * ( a_dy - a_dx);
      a_proc      (a_x,a_y,a_z,beg);
      while       (a_x<end)
	{
	  if(d<=0)
	    {
	      d += incrE;
	      a_x++;
	    }
	  else
	    {
	      d += incrNE;
	      a_x++;
	      a_y++;
	    }
	  a_z        += incz;
	  a_proc     (a_x,a_y,a_z,beg);
	}
    }
}
/***************************************************************************/
static void WritePoint (
 ZPos a_x
,ZPos a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   ZReal*     zbuff;
   ZPixel*    zimage;
   long       offset;
   ZReal      zpoint;
/*.........................................................................*/
  if((a_x<Class.begX) || (a_x>Class.endX))  return;
  if((a_y<Class.begY) || (a_y>Class.endY))  return;

  zpoint      = (ZReal)a_z;
  offset      = a_y * Class.zbw + a_x;
  zbuff       = Class.zbuffer + offset;
/*test must be on same type (float,float) or (double,double) */
  if(zpoint<*zbuff) return;
  zimage      = Class.zimage  + offset;
  *zbuff      = zpoint;
  *zimage     = Class.pixel;
}
static void Write1 (ZPos a_x,ZPos a_y,double a_z,ZPos a_beg) {Class.writePoint(a_x,a_y,a_z);a_beg=0;}
static void Write2 (ZPos a_x,ZPos a_y,double a_z,ZPos a_beg) {Class.writePoint(a_y,a_x,a_z);a_beg=0;}
static void Write3 (ZPos a_x,ZPos a_y,double a_z,ZPos a_beg) {Class.writePoint(a_x,2 * a_beg-a_y,a_z);}
static void Write4 (ZPos a_x,ZPos a_y,double a_z,ZPos a_beg) {Class.writePoint(2 * a_beg-a_y,a_x,a_z);}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void WriteEdgePoint (
 ZPos a_x
,ZPos a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   ZReal*     zbuff;
   ZPixel*    zimage;
   long       offset;
   ZReal      zpoint;
/*.........................................................................*/
  if((a_x<Class.begX) || (a_x>Class.endX))  return;
  if((a_y<Class.begY) || (a_y>Class.endY))  return;

/*Computing must be the same as in WriteScanLine routine.*/
  zpoint      = (ZReal)(- Class.planeDC - Class.planeAC * a_x - Class.planeBC * a_y);
  if(zpoint>Class.zmax||zpoint<Class.zmin) return; /* for edge plane quite perpandicular to screen */

  offset      = a_y * Class.zbw + a_x;
  zbuff       = Class.zbuffer + offset;
/*test must be on same type (float,float) or (double,double) */
  if(zpoint<*zbuff) return;
  zimage      = Class.zimage  + offset;
  *zbuff      = zpoint;
  *zimage     = Class.pixel;

  a_z         = 0.; /*Not used.*/
}
/***************************************************************************/
static void WriteScanLine (
 int a_beg
,int a_end
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int     x,beg,end;
   long    offset;
   ZReal*  zbuff;
   ZPixel* zimage;
   ZReal   zpoint;
/*.........................................................................*/
  if((a_y<Class.begY) || (a_y>Class.endY)) return;
  if(a_end<=a_beg)                   return;

  if(a_beg>Class.endX)               return;
  if(a_end<Class.begX)               return;
  
/*border clip*/
  beg         =   MAXIMUM(a_beg,(int)Class.begX);
  end         =   MINIMUM(a_end,(int)Class.endX);

  offset      =   a_y * Class.zbw + beg;
  zbuff       =   Class.zbuffer + offset;
  zimage      =   Class.zimage  + offset;

  for(x=beg;x<=end;x++)
    {
/*Computing must be the same as in WriteEdgePoint routine.*/
      zpoint      = (ZReal)(- Class.planeDC - Class.planeAC * x - Class.planeBC * a_y); 
      if(
	 (zpoint>=(*zbuff))    &&
	 (zpoint>=Class.zmin) &&      /* for plane quite perpandicular to screen */
	 (zpoint<=Class.zmax) 
	 )
	{
	  *zbuff           = zpoint;
	  *zimage          = Class.pixel;
	}
      zbuff  ++;
      zimage ++;
    }
}

