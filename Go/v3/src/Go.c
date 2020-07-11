/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
#define DEBUG 
*/

#include <string.h>
#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif
 
#include <CMemory.h>
#include <CString.h>
#include <CCharacter.h>
#include <CPrinter.h>
#include <CError.h>
#include <CVector.h>
#include <OMatrix.h>

#include <OCamera.h>
#include <OContextP.h>
#include <OPointListP.h>

#include <OContextmap.h>

#include <Go.h>

#ifdef __cplusplus
extern "C"{
#endif
static void AddRuleTicks      (ONode,int,double,double,double,OMatrix);
static void AddRuleNumbers    (ONode,int,double,double,double,OMatrix,OMatrix,double,double,char*);
static void AddCharacter      (ONode,OMatrix,char,double*);
static void MultiplyPoints    (OMatrix,OPointList);
static void AddRay            (OPointList,int,double,double,double,double,double,double,double,double,double,double);
static void AddHelix          (OPointList,int,double,int,double,double,double,double,double,double,double,double,double,double,double,double,double,double);
static void GetTrackPosition  (double,double,double,double,double,double,double*,double*,double*);
static void ReportError       (char*,int);
#ifdef __cplusplus
}
#endif

#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)
#define NotFound (-1)
#define CHECK_DIV(a_var,a_pro) {if((a_var)==0.) {ReportError(a_pro,CErrorValue);(a_var)=1.;}}
#define ONumber(arr)            (sizeof(arr) / sizeof(arr[0]))

static struct 
{
  char string[128];
} Class;
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void GoSetColorIndex (
 int a_value
)
/***************************************************************************/
/*
 Description :
   Changes color index of static OContext object. Static OContext object
  is used by the GoAdd...ToNode routines to capture graphical attributs. 
 Arguments :
   a_value: color index. The color index is used when a primitive is drawn
  by an OCamera. The index permits to retreive the rgb color stored in the 
  OColormap object associated to the OCamera.
 Example : draw a green cube. It is assume that the colormap of the OCamera
           that will view the node is the default "ocolormap_X" OColormap object.  
...
#include <Go.h>
...
  {ONode onode;
   onode = ONodeCreate("Cube");
   GoSetColorIndex (OColorGreen);
   GoAddLineToNode (onode, 0.5,-0.5,0.5, 0.5, 0.5,0.5);
...
  }
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OContextSetColorIndex (OContextGetStaticInstance(),a_value);
}
/***************************************************************************/
void GoSetLineWidth (
 int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OContextSetLineWidth (OContextGetStaticInstance(),a_value);
}
/***************************************************************************/
void GoSetLineStyle (
 OLineStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OContextSetLineStyle (OContextGetStaticInstance(),a_value);
}
/***************************************************************************/
void GoSetMarkerStyle (
 OMarkerStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OContextSetMarkerStyle (OContextGetStaticInstance(),a_value);
}
/***************************************************************************/
void GoSetAreaStyle (
 OAreaStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OContextSetAreaStyle (OContextGetStaticInstance(),a_value);
}
/***************************************************************************/
void GoAddLineToNode (
 ONode a_obj 
,double a_beg_x 
,double a_beg_y 
,double a_beg_z 
,double a_end_x 
,double a_end_y 
,double a_end_z 
)
/***************************************************************************/
/*
 Description :
   Adds a line to an ONode object.
 Arguments :
   a_beg_x, a_beg_y, a_beg_z : world coordinates of the begin point of the line.
   a_end_x, a_end_y, a_end_z : world coordinates of the end point of the line.
 Example : 
...
#include <Go.h>
...
  {ONode onode;
   onode = ONodeCreate("Cube");
   GoAddLineToNode (onode, 0.5,-0.5,0.5, 0.5, 0.5,0.5);
...
  }
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double xs[2];
  double ys[2];
  double zs[2];
/*.........................................................................*/
  xs[0] = a_beg_x;
  ys[0] = a_beg_y;
  zs[0] = a_beg_z;
  xs[1] = a_end_x;
  ys[1] = a_end_y;
  zs[1] = a_end_z;
  ONodeAddPrimitive (a_obj,OPrimitiveCreate (OPrimitiveLines,OContextGetStaticInstance(),2,xs,ys,zs));
}
/***************************************************************************/
void GoAddMarkerToNode (
 ONode a_obj 
,double a_x 
,double a_y 
,double a_z 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive  (a_obj,OPrimitiveCreate (OPrimitiveMarkers,OContextGetStaticInstance(),1,&a_x,&a_y,&a_z));
}
/***************************************************************************/
void GoAddPolygonToNode (
 ONode a_obj 
,OPointList a_line 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive  (a_obj,OPrimitiveCreate (OPrimitivePolygon,OContextGetStaticInstance(),a_line->number,a_line->xs,a_line->ys,a_line->zs));
}
/***************************************************************************/
void GoAddLinesToNode (
 ONode a_obj 
,OPointList a_line 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive  (a_obj,OPrimitiveCreate (OPrimitiveLines,OContextGetStaticInstance(),a_line->number,a_line->xs,a_line->ys,a_line->zs));
}
/***************************************************************************/
void GoAddMarkersToNode (
 ONode a_obj 
,OPointList a_line 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive  (a_obj,OPrimitiveCreate (OPrimitiveMarkers,OContextGetStaticInstance(),a_line->number,a_line->xs,a_line->ys,a_line->zs));
}
/***************************************************************************/
void GoAddSegmentsToNode (
 ONode a_obj 
,OPointList a_line 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive  (a_obj,OPrimitiveCreate (OPrimitiveSegments,OContextGetStaticInstance(),a_line->number,a_line->xs,a_line->ys,a_line->zs));
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void GoAddColoredBoxToNode (
 ONode a_obj 
,double  a_vx
,double  a_vy
,double  a_vz
,double  a_sx
,double  a_sy
,double  a_sz
,int     a_index_x  
,int     a_index_y  
,int     a_index_z  
)
/***************************************************************************/
/* Faces given counterclockwise                                               */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double      x,y,z;
  double      x_maximum,y_maximum,z_maximum;
  double      cx[8];
  double      cy[8];
  double      cz[8];
  OPointList     points;
/*.........................................................................*/
  points         = OPointListCreate(5);
  x              = a_vx;
  y              = a_vy;
  z              = a_vz;
  x_maximum      = x + a_sx;
  y_maximum      = y + a_sy;
  z_maximum      = z + a_sz;

  cx[0] = x;
  cy[0] = y;
  cz[0] = z;
  cx[1] = x_maximum;
  cy[1] = y;
  cz[1] = z;
  cx[2] = x;
  cy[2] = y_maximum;
  cz[2] = z;
  cx[3] = x_maximum;
  cy[3] = y_maximum;
  cz[3] = z;
  cx[4] = x;
  cy[4] = y;
  cz[4] = z_maximum;
  cx[5] = x_maximum;
  cy[5] = y;
  cz[5] = z_maximum;
  cx[6] = x;
  cy[6] = y_maximum;
  cz[6] = z_maximum;
  cx[7] = x_maximum;
  cy[7] = y_maximum;
  cz[7] = z_maximum;

/*z back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,2,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,3,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  OContextSetF (OContextGetStaticInstance(),"c",a_index_z);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*y front*/
  OPointListSetIthEntry   (points,0,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,1,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,4,cx[2],cy[2],cz[2]);
  OContextSetF (OContextGetStaticInstance(),"c",a_index_y);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*x front*/
  OPointListSetIthEntry   (points,0,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,1,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,4,cx[1],cy[1],cz[1]);
  OContextSetF (OContextGetStaticInstance(),"c",a_index_x);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*y back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,2,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,3,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  OContextSetF (OContextGetStaticInstance(),"c",a_index_y);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*x back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,2,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,3,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  OContextSetF (OContextGetStaticInstance(),"c",a_index_x);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*z front*/
  OPointListSetIthEntry   (points,0,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,1,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,4,cx[4],cy[4],cz[4]);
  OContextSetF (OContextGetStaticInstance(),"c",a_index_z);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
  OPointListDelete     (points);
}
/***************************************************************************/
void GoAddAxisToNode (
 ONode a_obj 
,double a_bx
,double a_by
,double a_bz
,double a_ex
,double a_ey
,double a_ez
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double           x,y,z;
  OMatrix          matrix,scale,x_to_z,z_to_vector,to_begin;
  double           length;
/*.........................................................................*/
  if((a_bx==a_ex)&&(a_by==a_ey)&&(a_bz==a_ez)) return;

  x = a_ex - a_bx;
  y = a_ey - a_by;
  z = a_ez - a_bz;

  length                = CVectorNormalize (&x,&y,&z);
  CHECK_DIV             (length,"RepAxis")

  scale                 = OMatrixCreate (OMatrixScale,length*1.3,0.,0.);
  x_to_z                = OMatrixCreate (OMatrixVectorToZ,1.,0.,0.);
  z_to_vector           = OMatrixCreate (OMatrixZToVector,x,y,z);
  to_begin              = OMatrixCreate (OMatrixTranslation,a_bx,a_by,a_bz);

  matrix                = OMatrixMultiplyMatrices (4,to_begin,z_to_vector,x_to_z,scale);

  GoAddArrowToNode      (a_obj,matrix,1);

  OMatrixDelete         (scale);
  OMatrixDelete         (x_to_z);
  OMatrixDelete         (z_to_vector);
  OMatrixDelete         (to_begin);
  OMatrixDelete         (matrix);
}
/***************************************************************************/
void GoAddFrameToNode (
 ONode a_obj 
,double a_size 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix         matrix,zoom,rotate;
  double          sizex,sizey,sizez;
  OPointList      points;
/*.........................................................................*/
  if(a_obj==NULL) return;
  if(a_size<=0.)  return;

  points                = OPointListCreate(6);
  OPointListSetIthEntry (points,0,0.,0.,0.);
  OPointListSetIthEntry (points,1,a_size,0.,0.);
  OPointListSetIthEntry (points,2,0.,0.,0.);
  OPointListSetIthEntry (points,3,0.,a_size,0.);
  OPointListSetIthEntry (points,4,0.,0.,0.);
  OPointListSetIthEntry (points,5,0.,0.,a_size);
  GoAddLinesToNode      (a_obj,points);
  OPointListDelete      (points);

/* common string parameter */
  sizex             = 0.1 * a_size;
  sizey             = 0.1 * a_size;
  sizez             = 0.1 * a_size;
  zoom              = OMatrixCreate(OMatrixScale,sizex,sizey,1.0);
/* shape_string X */
  GoAddStringToNode (a_obj,"X",a_size - sizex,-1.5*sizex,0.0,zoom);
/* shape_string Y */
  GoAddStringToNode (a_obj,"Y",-1.5*sizey,a_size - sizey,0.0,zoom);
/* shape_string Z */
  rotate            = OMatrixCreate   (OMatrixRotationY,M_PI_2);
  matrix            = OMatrixMultiply (rotate,zoom);
  GoAddStringToNode (a_obj,"Z",0.0,-1.5*sizez,a_size - sizez,matrix);

  OMatrixDelete     (zoom);
  OMatrixDelete     (rotate);
  OMatrixDelete     (matrix);
}
/***************************************************************************/
void GoAddRZ_FrameToNode (
 ONode a_obj 
,double a_size 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix         matrix,zoom;
  double          sizex,sizey;
  OPointList      points;
/*.........................................................................*/
  if(a_obj==NULL) return;
  if(a_size<=0.)  return;

  points                = OPointListCreate(4);
  OPointListSetIthEntry (points,0,0.,0.,0.);
  OPointListSetIthEntry (points,1,a_size,0.,0.);
  OPointListSetIthEntry (points,2,0.,0.,0.);
  OPointListSetIthEntry (points,3,0.,a_size,0.);
  GoAddLinesToNode      (a_obj,points);
  OPointListDelete      (points);

/* common string parameter */
  sizex             = 0.1 * a_size;
  sizey             = 0.1 * a_size;
  zoom              = OMatrixCreate(OMatrixScale,sizex,sizey,1.0);
/* shape_string X */
  matrix            = OMatrixDuplicate       (zoom);
  GoAddStringToNode (a_obj,"R",a_size - sizex,-1.5*sizex,0.0,matrix);
/* shape_string Y */
  GoAddStringToNode (a_obj,"Z",-1.5*sizey,a_size - sizey,0.0,matrix);

  OMatrixDelete     (zoom);
  OMatrixDelete     (matrix);
}
/***************************************************************************/
void GoAddRuleToNode (
 ONode a_obj    
,double a_beg    
,double a_width  
,double a_min    
,double a_max    
,int a_ndiv     
,char* a_format  
,double a_height 
,OMatrix a_matrix   
,OMatrix a_smatrix 
)
/***************************************************************************/
/* if a_height < 0. tick are downward                                      */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double    xx,w2,h1,h2;
  int       ndiv_1,ndiv_2;
/*.........................................................................*/
  GoAddArrowToNode     (a_obj,a_matrix,0);
/* draw ticks */
  ndiv_1     = a_ndiv / 100;
  ndiv_2     = a_ndiv % 100;

  h1        = a_height/2.;
  h2        = h1/2.;
  if(ndiv_1<=0) return;

  AddRuleTicks  (a_obj,ndiv_1,a_beg,a_width,h1,a_matrix);
  AddRuleNumbers(a_obj,ndiv_1,a_beg,a_width,a_height >=0. ? - a_height/2. : -fabs(a_height),
                     a_matrix,a_smatrix,a_min,a_max,a_format);

  if(ndiv_2<=0) return;
  w2 = a_width / ndiv_2;
  for(count=0;count<ndiv_1;count++)
    {
      xx        = a_beg + count * a_width;
      AddRuleTicks (a_obj,ndiv_2,xx,w2,h2,a_matrix);
    }
}
/***************************************************************************/
void GoAddArrowToNode (
 ONode a_obj  
,OMatrix a_matrix 
,int a_arrow 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OPointList    points;
  double     angle,length_arrow,xpt,ypt;
/*.........................................................................*/
  points     = a_arrow==1 ? OPointListCreate(5) : OPointListCreate(2);
/* build canonic on X */
  count      =  0;
  OPointListSetIthEntry (points,count,0.,0.,0.); count++;
  OPointListSetIthEntry (points,count,1.,0.,0.); count++;
/* arrow */
  if(a_arrow==1) 
    {
      length_arrow     = 1./20.;
      angle            = M_PI/6.;
      xpt              = 1. - length_arrow * cos(angle);
      ypt              =      length_arrow * sin(angle);
      OPointListSetIthEntry  (points,count,xpt, ypt,0.);count++;
      OPointListSetIthEntry  (points,count,xpt,-ypt,0.);count++;
      OPointListSetIthEntry  (points,count, 1.,  0.,0.);count++;
    }
/*load*/
  MultiplyPoints  (a_matrix,points);
  GoAddLinesToNode      (a_obj,points);
  OPointListDelete        (points);
}
/***************************************************************************/
static void AddRuleTicks (
 ONode a_obj
,int a_ndiv 
,double a_beg 
,double a_wbin 
,double a_height 
,OMatrix a_matrix 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double      xx;
  OPointList     points;
/*.........................................................................*/
  points = OPointListCreate(2);
  xx     = a_beg;
  for(count=0;count<a_ndiv+1;count++)
    {
      if((xx>=0.)&&(xx<=1.))
        {
          OPointListSetIthEntry    (points,0,xx,      0.,0.);
          OPointListSetIthEntry    (points,1,xx,a_height,0.);
          MultiplyPoints  (a_matrix,points);
          GoAddLinesToNode      (a_obj,points);
        }
      xx                 += a_wbin;
    }
  OPointListDelete(points);
}
/***************************************************************************/
static void AddRuleNumbers (
 ONode a_obj
,int a_ndiv 
,double a_beg 
,double a_wbin 
,double a_yy
,OMatrix a_matrix 
,OMatrix a_smatrix 
,double a_min
,double a_max
,char* a_format
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double      width,xx;
  OMatrix     matrix,trans;
/*.........................................................................*/
  if(a_format==NULL) return;
  xx        = a_beg;
  width     = fabs(a_max - a_min)/a_ndiv;
  for(count=0;count<a_ndiv+1;count++)
    {
      if((xx>=0.)&&(xx<=1.))
        { 
	  double             x,y,z;
	  char*              string;
          OMatrixMultiplyVector       (a_matrix,xx,a_yy,0.,&x,&y,&z);
          string             = CStringCreateF     (128,a_format,a_min + count * width);
          trans              = OMatrixCreate      (OMatrixStringAlign,string,(int)'C',(int)'T');
          matrix             = OMatrixMultiply (a_smatrix,trans);
          GoAddStringToNode         (a_obj,string,x,y,z,matrix);
          OMatrixDelete      (trans);
          OMatrixDelete      (matrix);
          CStringDelete      (string);
        }
      xx                 += a_wbin;
    }
}

/***************************************************************************/
void GoAddAlignedStringToNode (
 ONode a_obj 
,char*    a_string  
,double   a_vertex_x
,double   a_vertex_y
,double   a_vertex_z
,double   a_Xx
,double   a_Xy
,double   a_Xz
,double   a_Yx
,double   a_Yy
,double   a_Yz
,double   a_size    
,char     a_xalign  
,char     a_yalign  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix    matrix,trans,scale,P;
  double     Xx,Xy,Xz;
  double     Yx,Yy,Yz;
  double     Zx,Zy,Zz;
/*.........................................................................*/
  if(a_string==NULL) return;
  if(*a_string=='\0') return;
  Xx              = a_Xx;
  Xy              = a_Xy;
  Xz              = a_Xz;
  Yx              = a_Yx;
  Yy              = a_Yy;
  Yz              = a_Yz;
  CVectorNormalize    (&Xx,&Xy,&Xz);
  CVectorNormalize    (&Yx,&Yy,&Yz);
  CVectorMultiply        (Xx,Xy,Xz,Yx,Yy,Yz,&Zx,&Zy,&Zz);
  CVectorMultiply        (Zx,Zy,Zz,Xx,Xy,Xz,&Yx,&Yy,&Yz);
/* now X,Y,Z normalized frame */
  P                   = OMatrixCreate(OMatrixFollowing,
  Xx,Yx,Zx,0.,
  Xy,Yy,Zy,0.,
  Xz,Yz,Zz,0.,
   0.,0.,0.,1.);

  trans           = OMatrixCreate (OMatrixStringAlign,a_string,(int)a_xalign,(int)a_yalign);
  scale           = OMatrixCreate (OMatrixScale,.8 * a_size,a_size,1.);

  matrix          = OMatrixMultiplyMatrices  (3,P,scale,trans);
  OMatrixDelete   (trans);
  OMatrixDelete   (scale);
  OMatrixDelete   (P);

  GoAddStringToNode      (a_obj,a_string,a_vertex_x,a_vertex_y,a_vertex_z,matrix);
  OMatrixDelete   (matrix);
}
/***************************************************************************/
void GoAddTextFieldToNode (
 ONode a_obj 
,char*   a_string  
,double a_x
,double a_y
,double a_z
,double a_width   
,double a_height  
)
/***************************************************************************/
/*
  a_width,a_height: size of one character in world coordinate
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix;
/*.........................................................................*/
  matrix            = OMatrixCreate(OMatrixScale,.8 * a_width,a_height,1.);
  GoAddStringToNode (a_obj,a_string,a_x,a_y,a_z,matrix);
  OMatrixDelete     (matrix);
}
/***************************************************************************/
void GoAddStringToNode (
 ONode a_obj
,char* a_string 
,double a_x
,double a_y
,double a_z
,OMatrix a_matrix 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix      matrix,translate;
  char*        itema;
  double       length,width = 0.;
/*.........................................................................*/
  if(a_obj==NULL) return;
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  translate     = OMatrixCreate(OMatrixTranslation,a_x,a_y,a_z);
  matrix        = OMatrixCreate(OMatrixIdentity);
  length        = 0.;
  for(itema=a_string;*itema!='\0';itema++)
    { 
      OMatrix                          shift;
      shift                            = OMatrixCreate(OMatrixTranslation,length,0.,0.);
      OMatrixMultiplyMatricesAndAssign (matrix,3,translate,a_matrix,shift);
      AddCharacter                     (a_obj,matrix,*itema,&width);
      OMatrixDelete                    (shift); 
      length                         += width + 0.2;
    }
  OMatrixDelete (matrix);
  OMatrixDelete (translate);
}
/***************************************************************************/
double GoAddPAW_StringToNode (
 ONode a_obj
,char* a_string
,double a_x
,double a_y
,double a_z
,OMatrix a_matrix
)
/***************************************************************************/
/* From Wolfgang Grandegger.
 * Special version of GoAddStringToNode allowing escape characters a la PAW:
 * '[' greek font,
 * ']' latin font,
 * '^' superscript,
 * '_' subscript,
 * '!' normalscript.
 * '@' backspace.
 * Note: double escape characters permit to print them e.g. '[['.
 * Note: the character height is directly passed to this routine!
 * Note: returns the total length of the string in characters.
 */        
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix      matrix,translate,script;
  OMatrix      superscript = NULL,subscript = NULL;
  char*        itema;
  double       length = 0,width = 0.,height;
  double       length_previous = 0;
  OFont        font;
/*.........................................................................*/
  if(a_obj==NULL) return 0.;
  if( (a_string==NULL) || (*a_string=='\0') ) return 0.;

  translate       = OMatrixCreate(OMatrixTranslation,a_x,a_y,a_z);
  matrix          = OMatrixCreate(OMatrixIdentity);
  length          = 0.;

  script          = a_matrix;
  height          = OMatrixGetElement11(a_matrix);
  font            = OContextGetFont(OContextGetStaticInstance());
  OContextSetFont (OContextGetStaticInstance(),OFontLatin); 

  for(itema=a_string;*itema!='\0';itema++)
    {
      OMatrix shift;

      switch (*itema)
	{
	case '[':
	  if (*(itema+1)==*itema) {itema++; break;};
	  OContextSetFont(OContextGetStaticInstance(),OFontGreek); 
	  continue;
	case ']':
	  if (*(itema+1)==*itema) {itema++; break;};
	  OContextSetFont(OContextGetStaticInstance(),OFontLatin); 
	  continue;
	case '!':
	  if (*(itema+1)==*itema) {itema++; break;};
	  script = a_matrix;
	  continue;
	case '^':
	  if (*(itema+1)==*itema) {itema++; break;};
	  if(superscript==NULL)
	    {
	      OMatrix       temp;
              temp          = OMatrixCreate   (OMatrixScaleTranslation,1.,.6,1.,0., height/2.,0.);
	      superscript   = OMatrixMultiply (temp,a_matrix);
	      OMatrixDelete (temp);
	    }
	  script           = superscript;
	  continue;
	case '_':
	  if (*(itema+1)==*itema) {itema++; break;};
	  if(subscript==NULL)
	    {
	      OMatrix       temp;
              temp          = OMatrixCreate(OMatrixScaleTranslation,1.,.6,1.,0.,-height/2.,0.);
	      subscript     = OMatrixMultiply(temp,a_matrix);
	      OMatrixDelete (temp);
	    }
	  script           = subscript;
	  continue;
	case '@':
	  if (*(itema+1)==*itema) {itema++; break;};
	  length       = length_previous;
	  continue;
	}

      shift                            = OMatrixCreate(OMatrixTranslation,length,0.,0.);
      OMatrixMultiplyMatricesAndAssign (matrix,3,translate,script,shift);
      AddCharacter                     (a_obj,matrix,*itema,&width);
      OMatrixDelete                    (shift);
      length_previous                  = length;
      length                         += width + 0.2;
    }
  OMatrixDelete (matrix);
  OMatrixDelete (translate);
  if(superscript!=NULL) OMatrixDelete (superscript);
  if(  subscript!=NULL) OMatrixDelete (  subscript);
  OContextSetFont(OContextGetStaticInstance(),font); 

  return length;
}
/***************************************************************************/
static void AddCharacter (
 ONode a_obj
,OMatrix a_matrix 
,char a_char 
,double* a_width
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define HERSHEY__MAX_POLY   4
#define HERSHEY__MAX_POINT  160 /* 40 * 4 */
  int    max_point[HERSHEY__MAX_POLY];
  double xp[HERSHEY__MAX_POINT],yp[HERSHEY__MAX_POINT];
  int    ipoly;
  int    number,ipoint;
/*.........................................................................*/
  if (OContextGetFont(OContextGetStaticInstance())==OFontGreek)
    CCharacterGetGreekFontPoints  (a_char,&number,max_point,xp,yp,a_width);
  else
    CCharacterGetASCII_FontPoints (a_char,&number,max_point,xp,yp,a_width);
  ipoint = 0;
  for (ipoly=0;ipoly<number;ipoly++)
    { 
      int    pointn;
      pointn = max_point[ipoly];
      if(pointn>0) 
	{
	   int count;
	  OPointList points;
	  points     = OPointListCreate (pointn);
	  for(count=0;count<pointn;count++)
	    {
	      double                x,y,z;
	      OMatrixMultiplyVector (a_matrix,xp[ipoint],yp[ipoint],0.,&x,&y,&z);
	      ipoint                ++;
	      OPointListSetIthEntry (points,count,x,y,z);
	    }
          GoAddLinesToNode  (a_obj,points);
	  OPointListDelete  (points);
	}
    }
}
/***************************************************************************/
void GoAddBoxToNode (
 ONode a_obj 
,double  a_vx
,double  a_vy
,double  a_vz
,double  a_sx
,double  a_sy
,double  a_sz
)
/***************************************************************************/
/* Faces given counterclockwise                                               */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double      x,y,z;
  double      x_maximum,y_maximum,z_maximum;
  double      cx[8];
  double      cy[8];
  double      cz[8];
  OPointList     points;
/*.........................................................................*/
  points         = OPointListCreate(5);
  x              = a_vx;
  y              = a_vy;
  z              = a_vz;
  x_maximum      = x + a_sx;
  y_maximum      = y + a_sy;
  z_maximum      = z + a_sz;

  cx[0] = x;
  cy[0] = y;
  cz[0] = z;
  cx[1] = x_maximum;
  cy[1] = y;
  cz[1] = z;
  cx[2] = x;
  cy[2] = y_maximum;
  cz[2] = z;
  cx[3] = x_maximum;
  cy[3] = y_maximum;
  cz[3] = z;
  cx[4] = x;
  cy[4] = y;
  cz[4] = z_maximum;
  cx[5] = x_maximum;
  cy[5] = y;
  cz[5] = z_maximum;
  cx[6] = x;
  cy[6] = y_maximum;
  cz[6] = z_maximum;
  cx[7] = x_maximum;
  cy[7] = y_maximum;
  cz[7] = z_maximum;

/*z back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,2,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,3,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*y front*/
  OPointListSetIthEntry   (points,0,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,1,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,4,cx[2],cy[2],cz[2]);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*x front*/
  OPointListSetIthEntry   (points,0,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,1,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,4,cx[1],cy[1],cz[1]);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*y back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,2,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,3,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*x back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,2,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,3,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
/*z front*/
  OPointListSetIthEntry   (points,0,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,1,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,4,cx[4],cy[4],cz[4]);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
  OPointListDelete     (points);
}
/***************************************************************************/
void GoAddGridToNode (
 ONode a_obj    
,double a_vx
,double a_vy
,double a_vz
,double a_Xx
,double a_Xy
,double a_Xz
,double a_Yx
,double a_Yy
,double a_Yz
,int  a_rows    
,int  a_cols    
,double a_sx  
,double a_sy  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int row,col;
  OPointList     points;
  double      x0,y0,z0;
  double      x,y,z;
  double      Xx,Xy,Xz;
  double      Yx,Yy,Yz;
/*.........................................................................*/
  if(a_rows<=0)   return;
  if(a_cols<=0)   return;
  if(a_sx<=0.) return;
  if(a_sy<=0.) return;

  Xx = a_Xx;
  Xy = a_Xy;
  Xz = a_Xz;

  Yx = a_Yx;
  Yy = a_Yy;
  Yz = a_Yz;

  CVectorNormalize    (&Xx,&Xy,&Xz);
  CVectorNormalize    (&Yx,&Yy,&Yz);

  points = OPointListCreate(5);
  for(row=0;row<a_rows;row++) 
    {
      for(col=0;col<a_cols;col++) 
        {
           x = x0 = a_vx + a_Xx * row + a_Yx * col;
           y = y0 = a_vy + a_Xy * row + a_Yy * col;
           z = z0 = a_vz + a_Xz * row + a_Yz * col;
           OPointListSetIthEntry   (points,0,x,y,z);
           OPointListSetIthEntry   (points,4,x,y,z);
           x  = x0 + Xx * a_sx;
           y  = y0 + Xy * a_sx;
           z  = z0 + Xz * a_sx;
           OPointListSetIthEntry   (points,1,x,y,z);
           x  = x0 + Xx * a_sx + Yx * a_sy;
           y  = y0 + Xy * a_sx + Yy * a_sy;
           z  = z0 + Xz * a_sx + Yz * a_sy;
           OPointListSetIthEntry   (points,2,x,y,z);
           x  = x0 + Yx * a_sy;
           y  = y0 + Yy * a_sy;
           z  = z0 + Yz * a_sy;
           OPointListSetIthEntry   (points,3,x,y,z);
           if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
	     GoAddPolygonToNode (a_obj,points); 
	   else 
	     GoAddLinesToNode (a_obj,points);
        }
    }
 OPointListDelete (points);


}
/***************************************************************************/
void GoAddTubeToNode (
 ONode a_obj    
,double a_radin 
,double a_radout 
,double a_length 
,double a_x
,double a_y
,double a_z
,double a_nx
,double a_ny
,double a_nz
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double  dangle,angle,cangle,sangle;
  OPointList lfront,lback,lin,lout,lgone;
  OMatrix matrix,rotate,translate,global;
#define MAX__TUBE_MODULE 90
/*.........................................................................*/
  if( (a_radin <=0.) ||
      (a_radout<=0.) ||
      (a_length<=0.) ) {ReportError("GoAddTubeToNode",CErrorValue);return;}
  lfront = OPointListCreate(4);
  lback  = OPointListCreate(4);
  lin    = OPointListCreate(4);
  lout   = OPointListCreate(4);
  lgone  = OPointListCreate(4);
/* matrix */
  translate = OMatrixCreate    (OMatrixTranslation,a_x,a_y,a_z);
  rotate    = OMatrixCreate    (OMatrixZToVector,a_nx,a_ny,a_nz);
  global    = OMatrixMultiply  (translate,rotate);
  dangle  = 2. * M_PI / MAX__TUBE_MODULE;
  cangle = cos(dangle);
  sangle = sin(dangle);
/* one front face z = length */
  OPointListSetIthEntry(lfront,0,a_radin          ,0.                 ,a_length);
  OPointListSetIthEntry(lfront,1,a_radout         ,0.                 ,a_length);
  OPointListSetIthEntry(lfront,2,a_radout * cangle,a_radout * sangle  ,a_length);
  OPointListSetIthEntry(lfront,3,a_radin  * cangle,a_radin  * sangle  ,a_length);
/* one back face z = 0. */
  OPointListSetIthEntry(lback,0,a_radin          ,0.                 ,0.);
  OPointListSetIthEntry(lback,1,a_radin  * cangle,a_radin  * sangle  ,0.);
  OPointListSetIthEntry(lback,2,a_radout * cangle,a_radout * sangle  ,0.);
  OPointListSetIthEntry(lback,3,a_radout         ,0.                 ,0.);
/* one in face */
  OPointListSetIthEntry(lin,0,a_radin          ,0.                 ,0.);
  OPointListSetIthEntry(lin,1,a_radin          ,0.                 ,a_length);
  OPointListSetIthEntry(lin,2,a_radin  * cangle,a_radin  * sangle  ,a_length);
  OPointListSetIthEntry(lin,3,a_radin  * cangle,a_radin  * sangle  ,0.);
/* one out face */
  OPointListSetIthEntry(lout,0,a_radout         ,0.                 ,0.);
  OPointListSetIthEntry(lout,1,a_radout * cangle,a_radout * sangle  ,0.);
  OPointListSetIthEntry(lout,2,a_radout * cangle,a_radout * sangle  ,a_length);
  OPointListSetIthEntry(lout,3,a_radout         ,0.                 ,a_length);
/* loop over all module */
  OMatrixDelete  (rotate);
  angle        = 0.;
  for(count=0;count<MAX__TUBE_MODULE;count++)
    {
      rotate = OMatrixCreate(OMatrixRotationZ,angle);
      matrix = OMatrixMultiply    (global,rotate);
/* load front face */
      OPointListCopy             (lgone,lfront);
      MultiplyPoints       (matrix,lgone);
      GoAddPolygonToNode            (a_obj,lgone);
/* load back face */
      OPointListCopy             (lgone,lback);
      MultiplyPoints       (matrix,lgone);
      GoAddPolygonToNode            (a_obj,lgone);
/* load in face */
      OPointListCopy             (lgone,lin);
      MultiplyPoints       (matrix,lgone);
      GoAddPolygonToNode            (a_obj,lgone);
/* load out face */
      OPointListCopy             (lgone,lout);
      MultiplyPoints       (matrix,lgone);
      GoAddPolygonToNode            (a_obj,lgone);
      angle                  += dangle;
      OMatrixDelete           (rotate);
      OMatrixDelete           (matrix);
    }
  OMatrixDelete  (translate);
  OMatrixDelete  (global);

  OPointListDelete (lfront);
  OPointListDelete (lback);
  OPointListDelete (lin);
  OPointListDelete (lout);
  OPointListDelete (lgone);
}
/***************************************************************************/
static void MultiplyPoints (
 OMatrix a_matrix
,OPointList a_list 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int number;
/*.........................................................................*/
  if(a_matrix==NULL) return;
  if(a_list==NULL)   return;
  number = OPointListGetSize(a_list);
  for(count=0;count<number;count++) 
    {
      double             x,y,z;
      double             nx,ny,nz;
      OPointListGetIthEntry       (a_list,count,&x,&y,&z);
      OMatrixMultiplyVector       (a_matrix,x,y,z,&nx,&ny,&nz);
      OPointListSetIthEntry    (a_list,count,nx,ny,nz);
    }
}
/***************************************************************************/
void GoAddCircleToNode (
 ONode a_obj 
,double a_radius 
,double a_x
,double a_y
,double a_z
,double a_nx
,double a_ny
,double a_nz
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OPointList     points;
  double      dangle,angle;
  OMatrix     matrix,matrix_z_n,translate;
#define MAX_CIRCLE_POINT 100
/*.........................................................................*/
  if(a_obj==NULL) return;
  if(a_radius<=0.) {ReportError ("GoAddCircleToNode",CErrorValue);return;}
  points      = OPointListCreate      (MAX_CIRCLE_POINT);
  matrix_z_n  = OMatrixCreate      (OMatrixZToVector,a_nx,a_ny,a_nz);
  translate   = OMatrixCreate      (OMatrixTranslation,a_x,a_y,a_z);
  matrix      = OMatrixMultiply (translate,matrix_z_n);
  dangle      = 2. * M_PI / (MAX_CIRCLE_POINT-1);
  angle       = 0.;
  for(count=0;count<MAX_CIRCLE_POINT;count++)
    { 
      double               x,y,z;
      OMatrixMultiplyVector         (matrix,a_radius * cos(angle),a_radius * sin(angle),0.,&x,&y,&z);
      OPointListSetIthEntry      (points,count,x,y,z);
      angle               += dangle;
    }
  OMatrixDelete (matrix_z_n);
  OMatrixDelete (translate);
  OMatrixDelete (matrix);

  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
  OPointListDelete (points);
}
/***************************************************************************/
void GoAddHelixToNode (
 ONode a_obj 
,int    a_hand 
,double a_radius 
,double a_step 
,int    a_max 
,double a_max_radius 
,double a_vx
,double a_vy
,double a_vz
,double a_sx
,double a_sy
,double a_sz
,double a_tx
,double a_ty
,double a_tz
,double a_nx
,double a_ny
,double a_nz
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  cos_phi;
  int     isray;
  OPointList points;
/*.........................................................................*/
  if(a_obj==NULL) return;
  if( (a_radius<=0.) || (a_step<=0.) ) {ReportError("GoAddHelixToNode",CErrorValue);return;}
/* Look if really an helix */
  cos_phi = CVectorGetCosine (a_nx,a_ny,a_nz,a_tx,a_ty,a_tz);
  isray   = (fabs(cos_phi)==1.) ? 1 : 0;
  if(isray==1)
    {
      if (fabs(a_vx)   >a_sx) return;
      if (fabs(a_vy)   >a_sy) return;
      if (fabs(a_vz)   >a_sz) return;
    }
  points = OPointListCreate(0);
  if(isray==1)
    AddRay   (points,a_max,a_step,a_vx,a_vy,a_vz,a_sx,a_sy,a_sz,a_tx,a_ty,a_tz);
  else
    AddHelix (points,a_max,a_max_radius,a_hand,a_step,a_radius,
                 a_vx,a_vy,a_vz,a_sx,a_sy,a_sz,a_tx,a_ty,a_tz,a_nx,a_ny,a_nz);
  GoAddLinesToNode (a_obj,points);
  OPointListDelete (points);
}
/***************************************************************************/
void GoAddRayToNode  (
 ONode a_obj 
,int    a_max 
,double a_step 
,double a_vx
,double a_vy
,double a_vz
,double a_sx
,double a_sy
,double a_sz
,double a_tx
,double a_ty
,double a_tz
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPointList points;
/*.........................................................................*/
  if(a_obj==NULL) return;
  if(a_step<=0.) {ReportError("GoAddRayToNode",CErrorValue);return;}
  if (fabs(a_vx)   >a_sx) return;
  if (fabs(a_vy)   >a_sy) return;
  if (fabs(a_vz)   >a_sz) return;
  points        = OPointListCreate(0);
  AddRay     (points,a_max,a_step,a_vx,a_vy,a_vz,a_sx,a_sy,a_sz,a_tx,a_ty,a_tz);
  GoAddLinesToNode (a_obj,points);
  OPointListDelete (points);
}
/***************************************************************************/
static void AddRay (
 OPointList a_points
,int a_max_point 
,double a_step 
,double a_vx
,double a_vy
,double a_vz
,double a_sx
,double a_sy
,double a_sz
,double a_tx
,double a_ty
,double a_tz
)
/***************************************************************************/
/* vertex     give first point of helix                                       */
/* tangent    give tangent at first point                                     */
/* step       tracking step                                                   */
/* max_point  limit on number of point                                        */
/* size       limit on helix extend                                           */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double    step;
  double    tx,ty,tz;
/*.........................................................................*/
  tx           = a_tx;
  ty           = a_ty;
  tz           = a_tz;
  CVectorNormalize (&tx,&ty,&tz);
  step         = a_step;
  step        *= 2.;
  do
    {
      double                  cx,cy,cz;
      step                    /= 2.;
      OPointListClear            (a_points);
      cx                      = a_vx;
      cy                      = a_vy;
      cz                      = a_vz;
      OPointListAddEntry         (a_points,cx,cy,cz);
      while(1)
        {
          if ((OPointListGetSize(a_points)+1)       >a_max_point ) break;
          cx      += tx * step ;
          cy      += ty * step ;
          cz      += tz * step ;
          if (fabs(cx)   >a_sx) break;
          if (fabs(cy)   >a_sy) break;
          if (fabs(cz)   >a_sz) break;
	  OPointListAddEntry (a_points,cx,cy,cz);
        }
    } while (OPointListGetSize(a_points)==1) ;  /* Check that step size was not too big */
}
/***************************************************************************/
static void AddHelix (
 OPointList a_points
,int a_max_point 
,double a_max_radius 
,int a_hand 
,double a_step 
,double a_radius 
,double a_vx
,double a_vy
,double a_vz
,double a_sx
,double a_sy
,double a_sz
,double a_tx
,double a_ty
,double a_tz
,double a_nx
,double a_ny
,double a_nz
)
/***************************************************************************/
/* a_hand = 1 Right, 0 Left                                                   */
/* normal     give direction of helix                                         */
/* vertex     give first point of helix                                       */
/* tangent    give tangent at first point                                     */
/* radius     give radius of helix                                            */
/* step       tracking step                                                   */
/* max_point  limit on number of point                                        */
/* size       limit on helix extend                                           */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double    step_thres,step,cos_phi,helix_step_on_two_pi,signus,arc,signormal;
  double    dangle,angle,alph,beta,zstep;
  double    ux,uy,uz;
  double    vx,vy,vz;
/*.........................................................................*/
  signus                      = (a_hand==1 ? 1. : -1.);
  cos_phi                     = CVectorGetCosine (a_nx,a_ny,a_nz,a_tx,a_ty,a_tz);
  signormal                   = (cos_phi>=0. ? 1. : -1.);

  CVectorMultiply                    (a_tx,a_ty,a_tz,a_nx,a_ny,a_nz,&ux,&uy,&uz);
  CVectorNormalize                (&ux,&uy,&uz);
  CVectorMultiply                    (a_nx,a_ny,a_nz,ux,uy,uz,&vx,&vy,&vz);
  CVectorNormalize                (&vx,&vy,&vz);

  helix_step_on_two_pi        = ( cos_phi==0. ? 0.  : fabs(a_radius/tan(acos(cos_phi)))   );
  arc                         = sqrt( a_radius * a_radius + helix_step_on_two_pi * helix_step_on_two_pi);
  CHECK_DIV                   (arc,"AddHelix")
/* tracking of helix */
  step        = a_step;
  step_thres  = 0.1 * a_radius;
  if (step_thres<step) step = step_thres;
  step       *= 2.;
  do
    {
      step                    /= 2.;
      OPointListClear            (a_points);
      dangle                  = step/arc;
      angle                   = 0.;
      while(1)
        {
          double cx,cy,cz;
          double rx,ry,rz;
          if ((OPointListGetSize(a_points)+1)       >a_max_point ) break;
          alph       = signus    * a_radius * (cos(angle)-1.);
          beta       =             a_radius *  sin(angle);
          zstep      = signormal * helix_step_on_two_pi * angle;
          cx         = a_vx + alph * ux + beta * vx + zstep * a_nx;
          cy         = a_vy + alph * uy + beta * vy + zstep * a_ny;
          cz         = a_vz + alph * uz + beta * vz + zstep * a_nz;
          CVectorMultiply   (cx,cy,cz,a_nx,a_ny,a_nz,&rx,&ry,&rz);
          if (CVectorGetNorme (rx,ry,rz)>a_max_radius) break;
          if (fabs(cx)   >a_sx) break;
          if (fabs(cy)   >a_sy) break;
          if (fabs(cz)   >a_sz) break;
	  OPointListAddEntry (a_points,cx,cy,cz);
          angle += dangle;
        }
    } while (OPointListGetSize(a_points)==1) ;  /* Check that step size was not too big */
}
/***************************************************************************/
void GoAddHelicalTrackToNode (
 ONode  a_obj 
,double a_W
,double a_CotTheta
,double a_Phi0
,double a_D0
,double a_Z0
,double a_arcLength
)
/***************************************************************************/
/*
  From HepVis/SoHelicalTrack.c
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define HTMPL 100
  int        count;
  OPointList points;
/*.........................................................................*/
  if(a_obj==NULL) return;
  points = OPointListCreate(HTMPL);
  for(count=0;count<HTMPL;count++)
    {
      double                s,x,y,z;
      s                     = (a_arcLength * count)/HTMPL;
      GetTrackPosition      (a_W,a_CotTheta,a_Phi0,a_D0,a_Z0,s,&x,&y,&z);
      OPointListSetIthEntry (points,count,x,y,z);
    }
  GoAddLinesToNode (a_obj,points);
  OPointListDelete (points);
}
/***************************************************************************/
static void GetTrackPosition (
 double a_W
,double a_CotTheta
,double a_Phi0
,double a_D0
,double a_Z0
,double a_s
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*
  From HepVis/SoHelicalTrack.c
  Get the trajectory as a function of distance
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double Theta,CosTheta,SinTheta;
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  Theta    = M_PI/2.0 - atan(a_CotTheta);
  CosTheta = cos(Theta);
  SinTheta = sin(Theta);
  if (a_s==0.0 || a_W==0.0) 
    {
      double phi1,xtan,ytan,ztan;
      phi1     = a_Phi0 + a_s * a_W * SinTheta;
      xtan     = SinTheta * cos(phi1);
      ytan     = SinTheta * sin(phi1);
      ztan     = CosTheta;
      if(a_x!=NULL) *a_x = -a_D0*cos(a_Phi0+M_PI/2.0) + a_s * xtan;
      if(a_y!=NULL) *a_y = -a_D0*sin(a_Phi0+M_PI/2)   + a_s * ytan;
      if(a_z!=NULL) *a_z = a_Z0                       + a_s * ztan;
    }
  else
    {
      if(a_x!=NULL) *a_x = (cos(a_Phi0)*sin(a_s*a_W*SinTheta)-sin(a_Phi0)*(-a_W*a_D0+1.0-cos(a_s*a_W*SinTheta)))/a_W;
      if(a_y!=NULL) *a_y = (sin(a_Phi0)*sin(a_s*a_W*SinTheta)+cos(a_Phi0)*(-a_W*a_D0+1.0-cos(a_s*a_W*SinTheta)))/a_W;       
      if(a_z!=NULL) *a_z = a_s*CosTheta + a_Z0;
    }
}
/***************************************************************************/
void GoAddRectangleToNode (
 ONode a_obj 
,double a_width
,double a_height
,double a_x
,double a_y
,double a_z
,double a_nx
,double a_ny
,double a_nz
)
/***************************************************************************/
/* From Wolfgang Grandegger.                                               */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double      half_width, half_height;
  OPointList  points;
  double      x,y,z;
  OMatrix     matrix,matrix_z_n,translate;
/*.........................................................................*/
  if(a_obj==NULL)  return;
  if(a_width<=0.)  return;
  if(a_height<=0.) return;

  matrix_z_n  = OMatrixCreate    (OMatrixZToVector,a_nx,a_ny,a_nz);
  translate   = OMatrixCreate    (OMatrixTranslation,a_x,a_y,a_z);
  matrix      = OMatrixMultiply  (translate,matrix_z_n);

  half_width            = a_width/2.;
  half_height           = a_height/2.;
  points                = OPointListCreate(5);
  OMatrixMultiplyVector (matrix,-half_width,-half_height,0.,&x,&y,&z);
  OPointListSetIthEntry (points,0,x,y,z);
  OPointListSetIthEntry (points,4,x,y,z);
  OMatrixMultiplyVector (matrix,+half_width,-half_height,0.,&x,&y,&z);
  OPointListSetIthEntry (points,1,x,y,z);
  OMatrixMultiplyVector (matrix,+half_width,+half_height,0.,&x,&y,&z);
  OPointListSetIthEntry (points,2,x,y,z);
  OMatrixMultiplyVector (matrix,-half_width,+half_height,0.,&x,&y,&z);
  OPointListSetIthEntry (points,3,x,y,z);

  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
  OPointListDelete      (points);

  OMatrixDelete (matrix_z_n);
  OMatrixDelete (translate);
  OMatrixDelete (matrix);

}
/***************************************************************************/
void GoAddDiamondToNode (
 ONode a_obj 
,double a_width
,double a_height
,double a_x
,double a_y
,double a_z
,double a_nx
,double a_ny
,double a_nz
)
/***************************************************************************/
/* From Wolfgang Grandegger.                                               */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double      half_width, half_height;
  OPointList  points;
  double      x,y,z;
  OMatrix     matrix,matrix_z_n,translate;
/*.........................................................................*/
  if(a_obj==NULL)  return;
  if(a_width<=0.)  return;
  if(a_height<=0.) return;

  matrix_z_n  = OMatrixCreate    (OMatrixZToVector,a_nx,a_ny,a_nz);
  translate   = OMatrixCreate    (OMatrixTranslation,a_x,a_y,a_z);
  matrix      = OMatrixMultiply  (translate,matrix_z_n);

  half_width            = a_width/2.;
  half_height           = a_height/2.;
  points                = OPointListCreate(5);
  OMatrixMultiplyVector (matrix,-half_width,             0.,0.,&x,&y,&z);
  OPointListSetIthEntry (points,0,x,y,z);
  OPointListSetIthEntry (points,4,x,y,z);
  OMatrixMultiplyVector (matrix,         0.,-half_height,0.,&x,&y,&z);
  OPointListSetIthEntry (points,1,x,y,z);
  OMatrixMultiplyVector (matrix,+half_width,             0.,0.,&x,&y,&z);
  OPointListSetIthEntry (points,2,x,y,z);
  OMatrixMultiplyVector (matrix,         0.,-half_height,0.,&x,&y,&z);
  OPointListSetIthEntry (points,3,x,y,z);

  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (a_obj,points); 
  else 
    GoAddLinesToNode (a_obj,points);
  OPointListDelete      (points);

  OMatrixDelete (matrix_z_n);
  OMatrixDelete (translate);
  OMatrixDelete (matrix);
}
/***************************************************************************/
/***************************************************************************/
/***** GEANT ***************************************************************/
/***************************************************************************/
/***************************************************************************/
void GoAddLineOfCylinderToNode (
 ONode This
,OMatrix a_matrix
,double a_R1
,double a_PhiMin
,double a_Z1
,double a_R2
,double a_PhiMax
,double a_Z2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  X1, Y1, X2, Y2;
  double  Phi1, Phi2;
  OPointList points;
/*.........................................................................*/
  points = OPointListCreate(2);
  Phi1   = a_PhiMin * M_PI/180.;
  Phi2   = a_PhiMax * M_PI/180.;
  X1     = a_R1 * cos(Phi1);
  Y1     = a_R1 * sin(Phi1);
  X2     = a_R2 * cos(Phi2);
  Y2     = a_R2 * sin(Phi2);
  OPointListSetIthEntry (points,0,X1,Y1,a_Z1);
  OPointListSetIthEntry (points,1,X2,Y2,a_Z2);
  MultiplyPoints        (a_matrix, points);
  GoAddLinesToNode      (This,points);
  OPointListDelete      (points);
}
/***************************************************************************/
void GoAddArcToNode (
 ONode This
,OMatrix a_matrix
,double a_radius
,double a_Z
,double a_PhiMin
,double a_PhiMax
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPointList points;
/*.........................................................................*/
  if(a_radius<0.)         {CWarn("GoAddArcToNode: invalid radius.\n");return;}
  if(a_PhiMax<a_PhiMin)   {CWarn("GoAddArcToNode: invalid phi: max<min.\n");return;}
  if(a_radius==0.)
    {
      points             = OPointListCreate (1);
      OPointListSetIthEntry    (points,0,0.,0.,a_Z);
      MultiplyPoints  (a_matrix,points);
      GoAddMarkersToNode     (This,points);
      OPointListDelete      (points);
    }
  else
    {
       int count;
      int     pointn;
      double  dangle,angle;
      double  Phi0, Phi1;
/* 360. -> 181 points = 180 segments.*/
      pointn = (int)(180. * (a_PhiMax - a_PhiMin)/ 360.);
      pointn ++;
      if(pointn<=1) pointn = 2;

      Phi0    = a_PhiMin * M_PI/180.;
      Phi1    = a_PhiMax * M_PI/180.;
      dangle  = (Phi1-Phi0) / (pointn-1);
      angle   = Phi0;
      points  = OPointListCreate (pointn);
      for(count=0;count<pointn;count++)
	{
	  double          X, Y;
	  X               = a_radius * cos(angle);
	  Y               = a_radius * sin(angle);
	  OPointListSetIthEntry (points,count, X, Y, a_Z);
	  angle          += dangle;
	}
      MultiplyPoints  (a_matrix,points);
      GoAddLinesToNode      (This,points);
      OPointListDelete      (points);
    }
}
/***************************************************************************/
void GoAddCylinderToNode (
 ONode This
,OMatrix a_matrix 
,double a_radius
,double a_hlz
)
/***************************************************************************/
/*
 Description :
   Adds a cylinder to an ONode object.
 Arguments :
   a_matrix : the address of an OMatrix object to place the cylinder in space.
              If NULL is given no transformation is done over the cylinder.
   a_radius : radius of the cylinder.
   a_hlz : half length of the cylinder.
 Example : to create a cylinder of length 1. and radius 1. :
...
#include <Go.h>
...
  {ONode onode;
   onode = ONodeCreate("Cylinder");
   GoAddCylinderToNode (onode, NULL,1.,0.5);
...
  }
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  GoAddArcToNode     (This, a_matrix,a_radius, -a_hlz,0.,360.);
  GoAddArcToNode     (This, a_matrix,a_radius,  a_hlz,0.,360.);
  GoAddLineOfCylinderToNode (This, a_matrix, a_radius,  0.,-a_hlz, a_radius,  0.,a_hlz);
  GoAddLineOfCylinderToNode (This, a_matrix, a_radius,180.,-a_hlz, a_radius,180.,a_hlz);
}
/***************************************************************************/
void GoAddGEANT_TRD2_ToNode (
 ONode This 
,OMatrix a_matrix 
,double  a_hlx_zd
,double  a_hly_zd
,double  a_hlx_zu
,double  a_hly_zu
,double  a_hlz
)
/***************************************************************************/
/*
* 'TRD2' is a trapezoid  with only the x length varying  with z.  It   *
*        has  4 parameters,  the  half  length in  x  at  the low  z   *
*        surface, that at  the high z surface, the half  length in y   *
*        and in z.                                                     *
C.    *   3       TRD2     DX1,DX2,DY1,DY2,DZ                          *
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double      a_x,a_y,a_z;
  double      cx[8];
  double      cy[8];
  double      cz[8];
  OPointList     points;
/*.........................................................................*/
  if(a_matrix==NULL) return;
  points = OPointListCreate(5);

  a_x = a_y = a_z = 0.;

  cx[0] = a_x-a_hlx_zd;
  cy[0] = a_y-a_hly_zd;
  cz[0] = a_z-a_hlz;
  cx[1] = a_x+a_hlx_zd;
  cy[1] = a_y-a_hly_zd;
  cz[1] = a_z-a_hlz;
  cx[2] = a_x-a_hlx_zd;
  cy[2] = a_y+a_hly_zd;
  cz[2] = a_z-a_hlz;
  cx[3] = a_x+a_hlx_zd;
  cy[3] = a_y+a_hly_zd;
  cz[3] = a_z-a_hlz;
  cx[4] = a_x-a_hlx_zu;
  cy[4] = a_y-a_hly_zu;
  cz[4] = a_z+a_hlz;
  cx[5] = a_x+a_hlx_zu;
  cy[5] = a_y-a_hly_zu;
  cz[5] = a_z+a_hlz;
  cx[6] = a_x-a_hlx_zu;
  cy[6] = a_y+a_hly_zu;
  cz[6] = a_z+a_hlz;
  cx[7] = a_x+a_hlx_zu;
  cy[7] = a_y+a_hly_zu;
  cz[7] = a_z+a_hlz;

/*z back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,2,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,3,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  MultiplyPoints (a_matrix,points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This,points); 
  else 
    GoAddLinesToNode (This,points);
/*y front*/
  OPointListSetIthEntry   (points,0,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,1,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,4,cx[2],cy[2],cz[2]);
  MultiplyPoints (a_matrix,points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This,points); 
  else 
    GoAddLinesToNode (This,points);
/*x front*/
  OPointListSetIthEntry   (points,0,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,1,cx[3],cy[3],cz[3]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,4,cx[1],cy[1],cz[1]);
  MultiplyPoints (a_matrix,points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This,points); 
  else 
    GoAddLinesToNode (This,points);
/*y back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[1],cy[1],cz[1]);
  OPointListSetIthEntry   (points,2,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,3,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  MultiplyPoints (a_matrix,points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This,points); 
  else 
    GoAddLinesToNode (This,points);
/*x back*/
  OPointListSetIthEntry   (points,0,cx[0],cy[0],cz[0]);
  OPointListSetIthEntry   (points,1,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,2,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,3,cx[2],cy[2],cz[2]);
  OPointListSetIthEntry   (points,4,cx[0],cy[0],cz[0]);
  MultiplyPoints (a_matrix,points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This,points); 
  else 
    GoAddLinesToNode (This,points);
/*z front*/
  OPointListSetIthEntry   (points,0,cx[4],cy[4],cz[4]);
  OPointListSetIthEntry   (points,1,cx[5],cy[5],cz[5]);
  OPointListSetIthEntry   (points,2,cx[7],cy[7],cz[7]);
  OPointListSetIthEntry   (points,3,cx[6],cy[6],cz[6]);
  OPointListSetIthEntry   (points,4,cx[4],cy[4],cz[4]);
  MultiplyPoints (a_matrix,points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This,points); 
  else 
    GoAddLinesToNode (This,points);
  OPointListDelete     (points);
}
/***************************************************************************/
void GoAddGEANT_TUBE_ToNode (
 ONode This 
,OMatrix a_matrix 
,double  a_rmin
,double  a_rmax
,double  a_hlz
)
/*****************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  GoAddCylinderToNode  (This,a_matrix,a_rmin,a_hlz);
  GoAddCylinderToNode  (This,a_matrix,a_rmax,a_hlz);
}
/***************************************************************************/
void GoAddGEANT_TUBS_ToNode (
 ONode This 
,OMatrix a_matrix 
,double  a_rmin
,double  a_rmax
,double  a_hlz
,double  a_phmin
,double  a_phmax
)
/*****************************************************************************/
/*
 Description :
   Adds a phi segment of a tube to an ONode object.
 Arguments :
  - a_matrix : an OMatrix that permit to position the object in space.
  - a_rmin : inside radius.
  - a_rmax : outside radius.
  - a_hlz : half length in z.                     
  - a_phmin, a_phmax : phi limits in degree. The segment starts at the
      first limit and includes increasing phi value up to the second limit 
      or that plus 360 degrees.
 Example : to add a translated phi segment of a tube to a node :
...
#include <Go.h>
...
  {ONode   onode;
   OMatrix omatrix;
   onode                  = ONodeCreate("Tube");
   omatrix                = OMatrixCreate (OMatrixTranslation,0.,0.,100.);
   GoAddGEANT_TUBS_ToNode (onode,omatrix,1.0,1.1,0.5,30.,35.);
   OMatrixDelete          (omatrix);}
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_matrix==NULL) return;

  GoAddArcToNode     (This, a_matrix,a_rmin, -a_hlz,a_phmin,a_phmax);
  GoAddArcToNode     (This, a_matrix,a_rmax, -a_hlz,a_phmin,a_phmax);

  GoAddArcToNode     (This, a_matrix,a_rmin,a_hlz,a_phmin,a_phmax);
  GoAddArcToNode     (This, a_matrix,a_rmax,a_hlz,a_phmin,a_phmax);

  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin,a_phmin,-a_hlz, a_rmax,a_phmin,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin,a_phmax,-a_hlz, a_rmax,a_phmax,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin,a_phmin,a_hlz, a_rmax,a_phmin,a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin,a_phmax,a_hlz, a_rmax,a_phmax,a_hlz );

  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin,a_phmin,a_hlz, a_rmin,a_phmin,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin,a_phmax,a_hlz, a_rmin,a_phmax,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmax,a_phmin,a_hlz, a_rmax,a_phmin,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmax,a_phmax,a_hlz, a_rmax,a_phmax,-a_hlz );
}
/***************************************************************************/
void GoAddGEANT_CONE_ToNode (
 ONode This 
,OMatrix a_matrix 
,double  a_hlz
,double  a_rmin1
,double  a_rmax1
,double  a_rmin2
,double  a_rmax2
)
/*****************************************************************************/
/*
* 'CONE' is a conical tube.  It has 5 parameters, the half length in   *
*        z, the  inside and  outside radii  at the  low z  limit and   *
*        those at the high z limit.                                    *
*
*   7       CONE     DZ,RMIN1,RMAX1,RMIN2,RMAX2                        *
*
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_matrix==NULL) return;

  GoAddArcToNode     (This, a_matrix,a_rmin1,-a_hlz,0.,360.);
  GoAddArcToNode     (This, a_matrix,a_rmax1,-a_hlz,0.,360.);

  GoAddArcToNode     (This, a_matrix,a_rmin2,  a_hlz,0.,360.);
  GoAddArcToNode     (This, a_matrix,a_rmax2,  a_hlz,0.,360.);

  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin2,  0.,a_hlz, a_rmin1,  0.,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin2,180.,a_hlz, a_rmin1,180.,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmax2,  0.,a_hlz, a_rmax1,  0.,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmax2,180.,a_hlz, a_rmax1,180.,-a_hlz );
}
/***************************************************************************/
void GoAddGEANT_CONS_ToNode (
 ONode This 
,OMatrix a_matrix 
,double  a_hlz
,double  a_rmin1
,double  a_rmax1
,double  a_rmin2
,double  a_rmax2
,double  a_phmin
,double  a_phmax
)
/*****************************************************************************/
/*
* 'CONS' is a phi  segment of a conical tube.  It  has 7 parameters,   *
*        the same 5 as 'CONE' plus the phi limits.                     *
*
*   8       CONS     DZ,RMIN1,RMAX1,RMIN2,RMAX2,PHIMIN,PHIMAX          *
*
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_matrix==NULL) return;

  GoAddArcToNode     (This, a_matrix,a_rmin1,-a_hlz,a_phmin,a_phmax);
  GoAddArcToNode     (This, a_matrix,a_rmax1,-a_hlz,a_phmin,a_phmax);

  GoAddArcToNode     (This, a_matrix,a_rmin2,  a_hlz,a_phmin,a_phmax);
  GoAddArcToNode     (This, a_matrix,a_rmax2,  a_hlz,a_phmin,a_phmax);

  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin1,a_phmin,-a_hlz, a_rmax1,a_phmin,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin1,a_phmax,-a_hlz, a_rmax1,a_phmax,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin2,a_phmin,a_hlz, a_rmax2,a_phmin,a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin2,a_phmax,a_hlz, a_rmax2,a_phmax,a_hlz );

  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin2,a_phmin,a_hlz, a_rmin1,a_phmin,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmin2,a_phmax,a_hlz, a_rmin1,a_phmax,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmax2,a_phmin,a_hlz, a_rmax1,a_phmin,-a_hlz );
  GoAddLineOfCylinderToNode (This, a_matrix, a_rmax2,a_phmax,a_hlz, a_rmax1,a_phmax,-a_hlz );
}
/***************************************************************************/
void GoAddGEANT_PGON_ToNode (
 ONode This
,OMatrix a_matrix
,float* a_paras
)
/*****************************************************************************/
/*
* 'PGON' is a polygon.  It has at least 10 parameters, the lower phi   *
*        limit, the range  in phi, the number of  straight sides (of   *
*        equal length)  between those  phi limits,  the number  of z   *
*        planes (at least two) where the distances to the z axis are   *
*        changing,  z  coordinate  of   first  plane,  the  shortest   *
*        distances  RMIN  & RMAX  from  the  z  axis to  the  inside   *
*        straight edge and  the outside straight edge  for the first   *
*        plane, Z, RMIN, RMAX for the second plane, and so on.
*
*  11       PGON     PHIMIN,DPHI,NDIV,NZ,Z(1),RMIN(1),RMAX(1),...      *
*
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    Foffset;
  double PhiMin,PhiMax;
  int    NDIV,NZ;
  double DPHI,FACT;
  int    iz;
/*.........................................................................*/
  Foffset = 1;
  PhiMin  = a_paras[1-Foffset];
  PhiMax  = PhiMin + a_paras[2-Foffset];
  NDIV    = (int)(a_paras[3-Foffset]);
  NZ      = (int)(a_paras[4-Foffset]);
  DPHI    = (PhiMax-PhiMin)/ NDIV ;
  FACT    = 1./cos(DPHI*M_PI/360.) ;

  for ( iz=1; iz<NZ+1; iz++)
    {
      a_paras [6-Foffset +(iz-1)*3] = a_paras [6-Foffset +(iz-1)*3]*FACT;
      a_paras [7-Foffset +(iz-1)*3] = a_paras [7-Foffset +(iz-1)*3]*FACT;
    }

  for ( iz=1; iz<NZ+1; iz++)
    {
      int idiv;
      double ZI,R0,R1,R0Pre,R0Post,R1Pre,R1Post;

      ZI     = a_paras [5-Foffset+((iz-1)*3)];
      R0     = a_paras [6-Foffset+((iz-1)*3)];
      R1     = a_paras [7-Foffset+((iz-1)*3)];
      R0Pre  = a_paras[6-Foffset+((iz-2)*3)];
      R0Post = a_paras [ 6-Foffset+(iz*3)];
      R1Pre  = a_paras[7-Foffset+((iz-2)*3)];
      R1Post = a_paras [ 7-Foffset+(iz*3)];
      if ( ((iz==1)||(iz==NZ)) || ((R0!=R0Pre)&&(R0!=R0Post)) )
	{
	  double Phi0, Phi1 ;
	  for (idiv=1; idiv<NDIV+1; idiv++)
	    {
	      Phi0 = PhiMin + (idiv-1) * DPHI;
	      Phi1 = Phi0 + DPHI;
	      GoAddLineOfCylinderToNode (This, a_matrix, R0, Phi0, ZI, R0, Phi1, ZI);
	    }
	}

/* 4005   CONTINUE */
      if ( ((iz==1)||(iz==NZ)) || ((R1!=R1Pre)&&(R1!=R1Post)) )
	{
	  double Phi0, Phi1 ;
	  for (idiv=1; idiv<NDIV+1; idiv++)
	    {
	      Phi0 = PhiMin + (idiv-1) * DPHI;
	      Phi1 = Phi0 + DPHI;
	      GoAddLineOfCylinderToNode (This, a_matrix, R1, Phi0, ZI, R1, Phi1, ZI);
	    }
	}
/* 4010   CONTINUE */
      if (  ((iz==1)||(iz==NZ)) && ( (PhiMax-PhiMin)!=360.) )
	{
	  GoAddLineOfCylinderToNode (This, a_matrix, R0, PhiMin, ZI, R1,  PhiMin, ZI );
	  GoAddLineOfCylinderToNode (This, a_matrix, R0, PhiMax, ZI, R1,  PhiMax, ZI );
	}

      if (iz!=1)
	{
	  double ZI0,R00,R10;
	  ZI0 = a_paras [ 5-Foffset+((iz-2)*3)];
	  R00 = a_paras [ 6-Foffset+((iz-2)*3)];
	  R10 = a_paras [ 7-Foffset+((iz-2)*3)];
	  for (idiv=1; idiv<NDIV+1; idiv++)
	    {
	      double      ph;
	      ph          = PhiMin + (idiv-1) * DPHI;
	      GoAddLineOfCylinderToNode (This, a_matrix, R00, ph, ZI0, R0, ph, ZI);
	      GoAddLineOfCylinderToNode (This, a_matrix, R10, ph, ZI0, R1, ph, ZI);
	    }
	  GoAddLineOfCylinderToNode (This, a_matrix, R00, PhiMax, ZI0, R0, PhiMax, ZI);
	  GoAddLineOfCylinderToNode (This, a_matrix, R10, PhiMax, ZI0, R1, PhiMax, ZI);
	}
      
    }
}
/***************************************************************************/
void GoAddGEANT_PCON_ToNode (
 ONode This
,OMatrix a_matrix
,float* a_paras
)
/*****************************************************************************/
/*
* 'PCON' is a polycone.  It has at least 9 parameters, the lower phi   *
*        limit, the range  in phi, the number of z  planes (at least   *
*        two) where  the radius are  changing, the z  coordinate and   *
*        the minimum and maximum radius for each z boundary.
*
*  12       PCON     PHIMIN,DPHI,NZ,Z(1),RMIN(1),RMAX(1),Z(2),...      *
*
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    Foffset = 1;
  double PhiMin,PhiMax;
  int    NZ,iz;
/*.........................................................................*/
  PhiMin = a_paras[1-Foffset];
  PhiMax = PhiMin + a_paras[2-Foffset];
  NZ     = (int)(a_paras[3-Foffset]);
  for ( iz=1; iz<NZ+1; iz++)
    {
      double ZI,R0,R1,R1Pre,R1Post;
      ZI     = a_paras [4-Foffset+((iz-1)*3)];
      R0     = a_paras [5-Foffset+((iz-1)*3)];
      R1     = a_paras [6-Foffset+((iz-1)*3)];
      R1Pre  = a_paras[6-Foffset+((iz-2)*3)];
      R1Post = a_paras [ 6-Foffset+(iz*3)];
      if ((iz==1)||(iz==NZ))
	{
	  GoAddArcToNode (This, a_matrix, R0,  ZI, PhiMin,   PhiMax);
	  GoAddArcToNode (This, a_matrix, R1,  ZI, PhiMin,   PhiMax);
	}
      else if ((R1>R1Pre)&&(R1>R1Post))
	{
	  GoAddArcToNode (This,a_matrix, R0, ZI, PhiMin, PhiMax);
	  GoAddArcToNode (This,a_matrix, R1, ZI, PhiMin, PhiMax);
	}
/* 5015   CONTINUE */
      if ((PhiMax- PhiMin)!=360.)
	{
	  GoAddLineOfCylinderToNode (This, a_matrix, R0, PhiMin, ZI, R1, PhiMin, ZI);
	  GoAddLineOfCylinderToNode (This, a_matrix, R0, PhiMax, ZI, R1, PhiMax, ZI);
	}
/* 5020   CONTINUE */
      if (iz!=1)
	{
	  double ZI0,R00,R10;
	  ZI0 = a_paras [ 4-Foffset+((iz-2)*3)];
	  R00 = a_paras [ 5-Foffset+((iz-2)*3)];
	  R10 = a_paras [ 6-Foffset+((iz-2)*3)];
	  if  ((PhiMax- PhiMin)!=360.)
	    {
	      GoAddLineOfCylinderToNode (This, a_matrix, R00, PhiMin, ZI0, R0, PhiMin, ZI);
	      GoAddLineOfCylinderToNode (This, a_matrix, R10, PhiMin, ZI0, R1, PhiMin, ZI);
	      GoAddLineOfCylinderToNode (This, a_matrix, R00, PhiMax, ZI0, R0, PhiMax, ZI);
	      GoAddLineOfCylinderToNode (This, a_matrix, R10, PhiMax, ZI0, R1, PhiMax, ZI);
	    }
	  
/* 5030   CONTINUE
   PHIP=AMOD((GPHI+90.),360.)
   PHIM=AMOD((GPHI+270.),360.)
  ... to be completed ...
*/
	}
    }
}
/***************************************************************************/
void GoAddGEANT_HYPE_ToNode (
 ONode This
,OMatrix a_matrix
,double a_RMIN
,double a_RMAX
,double a_DZ
,double a_PHI
)
/*****************************************************************************/
/*
* 'HYPE' is a hyperbolic  tube, ie the inner and  outer surfaces are   *
*        hyperboloids, as would be formed by a system of cylindrical   *
*        wires  which were  then  rotated  tangentially about  their   *
*        centres.  The 4  parameters are the inner  and outer radii,   *
*        the  half lenght  in z,  and  the "stereo  angle" theta  in   *
*        degrees,  such that  the hyperbolic  surfaces are  given by   *
*        r**2 = (z*tan(theta))**2 + (r at z=0)**2                      *
*
*  14       HYPE     RMIN,RMAX,DZ,PHI                                  *
*
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  RMIN1,RMAX1,Z1,Z2,TANTHS;
  double  RMIN12, RMAX12, RMIN2, RMAX2 ;
  OPointList points;
  int     NZSTEP = 20;
  double  DELZ;
  int     IZ, ISY, ISZ, J ;
  double  ZZ, ZZZ;
  double  x1, y1, z1, x2, y2, z2 ;
  double  PPY[9], PPZ[9] ;
  double  PX[9], PY[9], PZ[9] ;
/*.........................................................................*/
  RMIN1  = a_RMIN;
  RMAX1  = a_RMAX;
  Z2     = a_DZ;
  TANTHS = tan (  a_PHI * M_PI/180.);
  Z1     = -Z2;
  points = OPointListCreate (2);
  DELZ   = Z2 / NZSTEP;

  TANTHS = TANTHS * TANTHS;
  RMIN12 = RMIN1*RMIN1;
  RMAX12 = RMAX1*RMAX1 ;
  RMIN2  = sqrt (RMIN12 + Z2*Z2*TANTHS);
  RMAX2  = sqrt (RMAX12 + Z2*Z2*TANTHS);
/*  7410 CONTINUE */
  PPY[1] = RMAX2 ;
  PPY[3] = RMIN2 ;
  PX[1]  = 0.;
  PX[2]  = 0.;
  PX[3]  = 0.;
  PX[4]  = 0.;
  PPZ[1] = Z2;
  PPZ[3] = Z2;
  GoAddArcToNode  (This, a_matrix,  RMAX2,Z1, 0. , 360.);
  GoAddArcToNode  (This, a_matrix,  RMIN2,Z1, 0. , 360.);
  GoAddArcToNode  (This, a_matrix,  RMAX2,Z2, 0. , 360.);
  GoAddArcToNode  (This, a_matrix,  RMIN2,Z2, 0. , 360.);

  for (IZ=1; IZ< NZSTEP+1; IZ++)
    {
      ZZ     = Z2 - IZ*DELZ;
      PPZ[2] = ZZ;
      PPZ[4] = ZZ;
      ZZZ    = ZZ*ZZ*TANTHS ;
      PPY[2] = sqrt(RMAX12 + ZZZ) ;
      PPY[4] = sqrt(RMIN12 + ZZZ) ;
      for (ISY= -1; ISY<2; ISY=ISY+2 )
	{
	  for (ISZ= -1; ISZ<2; ISZ=ISZ+2 )
	    {
	      for (J=1; J<5; J++)
		{
		  PY[J] = ISY * PPY [J];
		  PZ[J] = ISZ * PPZ [J];
		}
/* 7420     CONTINUE
            CALL GDLINE(P(1,1),P(1,2))
            CALL GDLINE(P(1,3),P(1,4))
*/
	      x1= PX[1]; y1= PY[1]; z1 = PZ[1];
	      x2= PX[2]; y2= PY[2]; z2 = PZ[2];

	      OPointListSetIthEntry    (points,0,x1,y1,z1);
	      OPointListSetIthEntry    (points,1,x2,y2,z2);
	      MultiplyPoints  (a_matrix, points);
	      GoAddLinesToNode      (This,points);
	      x1 = PX[3]; y1 = PY[3]; z1 = PZ[3];
	      x2 = PX[4]; y2 = PY[4]; z2 = PZ[4];
  /*     GoAddLineToNode (This, x1, y1, z1, x2, y2, z2); */
	      OPointListSetIthEntry    (points,0,x1,y1,z1);
	      OPointListSetIthEntry    (points,1,x2,y2,z2);
	      MultiplyPoints  (a_matrix, points);
	      GoAddLinesToNode      (This,points);

/* 7430    CONTINUE */
	    }
	  PPY[1] = PPY[2];
	  PPY[3] = PPY[4];
	  PPZ[1] = PPZ[2];
	  PPZ[3] = PPZ[4];
/* 7440    CONTINUE */
	}
    }
  OPointListDelete        (points);
}
/***************************************************************************/
void GoAddGEANT_TRAP_ToNode (
 ONode This
,OMatrix a_matrix
,double DZ
,double TX
,double TY
,double H1
,double BL1
,double TL1
,double TTH1
,double H2
,double BL2
,double TL2
,double TTH2
)
/*****************************************************************************/
/*
 'TRAP' is  a  general trapezoid,  i.e.  one  for which  the  faces   
        perpendicular to z  are trapezia and their  centres are not   
        at the same x, y.  It has 11 parameters: Dz the half length   
        in z, Th & Phi the polar angles from the centre of the face   
        at  z=-Dz to  that at  z=+Dz, H1  the half  length in  y at   
        z=-Dz, LB1  the half length in  x at z=-Dz and  y=low edge,   
        LH1 the half length in x at z=-Dz and y= high edge, Th1 the   
        angle w.r.t. the  y axis from the centre of  the low y edge   
        to the centre of the high y edge, and H2, LB2, LH2, Th2 the   
        corresponding quantities to the 1s but at z=+Dz.              

   4       TRAP     DZ,TX,TY,H1,BL1,TL1,TTH1,H2,BL2,TL2,TTH2          

*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  PX[9], PY[9], PZ[9];
  OPointList points;
/*.........................................................................*/
  points = OPointListCreate(5);

/*Put -1.*XXX because CC HP-UX do not compile -XXX !!!*/
  PX[1] = -1. * DZ * TX + TTH1 * H1 + TL1 ;
  PY[1] =  H1 - DZ * TY ;
  PZ[1] = -1. * DZ ;
  PX[2] = -1.*DZ*TX+TTH1*H1-TL1 ;
  PY[2] =  H1-DZ*TY ;
  PZ[2] = -1.*DZ ;
  PX[3] = -1.*DZ*TX-TTH1*H1-BL1 ;
  PY[3] = -1.*H1-DZ*TY ;
  PZ[3] = -1.*DZ ;
  PX[4] = -1.*DZ*TX-TTH1*H1+BL1 ;
  PY[4] = -1.*H1-DZ*TY ;
  PZ[4] = -1.*DZ ;
  PX[5] =  DZ*TX+TTH2*H2+TL2 ;
  PY[5] =  H2+DZ*TY ;
  PZ[5] =  DZ ;
  PX[6] =  DZ*TX+TTH2*H2-TL2 ;
  PY[6] =  H2+DZ*TY ;
  PZ[6] =  DZ ;
  PX[7] =  DZ*TX-TTH2*H2-BL2 ;
  PY[7] = -1.*H2+DZ*TY ;
  PZ[7] =  DZ ;
  PX[8] =  DZ*TX-TTH2*H2+BL2 ;
  PY[8] = -1.*H2+DZ*TY ;
  PZ[8] =  DZ ;
                   
/*
      CALL GDRECT(P(1,1),P(1,2),P(1,3),P(1,4))
      CALL GDRECT(P(1,5),P(1,6),P(1,7),P(1,8))
      CALL GDLINE(P(1,1),P(1,5))
      CALL GDLINE(P(1,2),P(1,6))
      CALL GDLINE(P(1,3),P(1,7))
      CALL GDLINE(P(1,4),P(1,8))
*/

/* side Z-  */
  OPointListSetIthEntry   (points,0, PX[1], PY[1], PZ[1]);
  OPointListSetIthEntry   (points,1, PX[2], PY[2], PZ[2]);
  OPointListSetIthEntry   (points,2, PX[3], PY[3], PZ[3]);
  OPointListSetIthEntry   (points,3, PX[4], PY[4], PZ[4]);
  OPointListSetIthEntry   (points,4, PX[1], PY[1], PZ[1]);
  MultiplyPoints  (a_matrix, points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This, points); 
  else 
    GoAddLinesToNode  (This, points);

/* side Z+  */
  OPointListSetIthEntry   (points,0, PX[5], PY[5], PZ[5]);
  OPointListSetIthEntry   (points,1, PX[6], PY[6], PZ[6]);
  OPointListSetIthEntry   (points,2, PX[7], PY[7], PZ[7]);
  OPointListSetIthEntry   (points,3, PX[8], PY[8], PZ[8]);
  OPointListSetIthEntry   (points,4, PX[5], PY[5], PZ[5]);
  MultiplyPoints  (a_matrix, points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This, points); 
  else 
    GoAddLinesToNode  (This, points);
                                         
/* side Right (object) (Left Observer)  2-3-7-6*/
  OPointListSetIthEntry   (points,0, PX[2], PY[2], PZ[2]);
  OPointListSetIthEntry   (points,1, PX[3], PY[3], PZ[3]);
  OPointListSetIthEntry   (points,2, PX[7], PY[7], PZ[7]);
  OPointListSetIthEntry   (points,3, PX[6], PY[6], PZ[6]);
  OPointListSetIthEntry   (points,4, PX[2], PY[2], PZ[2]);
  MultiplyPoints  (a_matrix, points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This, points); 
  else 
    GoAddLinesToNode  (This, points);

/* side Left (object)  (Right Observer) 1-4-8-5 */
  OPointListSetIthEntry   (points,0, PX[1], PY[1], PZ[1]);
  OPointListSetIthEntry   (points,1, PX[4], PY[4], PZ[4]);
  OPointListSetIthEntry   (points,2, PX[8], PY[8], PZ[8]);
  OPointListSetIthEntry   (points,3, PX[5], PY[5], PZ[5]);
  OPointListSetIthEntry   (points,4, PX[1], PY[1], PZ[1]);
  MultiplyPoints  (a_matrix, points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This, points); 
  else 
    GoAddLinesToNode  (This, points);
            
/* Bottom   3-4-8-7 */
  OPointListSetIthEntry   (points,0, PX[3], PY[3], PZ[3]);
  OPointListSetIthEntry   (points,1, PX[4], PY[4], PZ[4]);
  OPointListSetIthEntry   (points,2, PX[8], PY[8], PZ[8]);
  OPointListSetIthEntry   (points,3, PX[7], PY[7], PZ[7]);
  OPointListSetIthEntry   (points,4, PX[3], PY[3], PZ[3]);
  MultiplyPoints  (a_matrix, points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This, points); 
  else 
    GoAddLinesToNode  (This, points);

/* Top 2-1-5-6 */
  OPointListSetIthEntry   (points,0, PX[2], PY[2], PZ[2]);
  OPointListSetIthEntry   (points,1, PX[1], PY[1], PZ[1]);
  OPointListSetIthEntry   (points,2, PX[5], PY[5], PZ[5]);
  OPointListSetIthEntry   (points,3, PX[6], PY[6], PZ[6]);
  OPointListSetIthEntry   (points,4, PX[2], PY[2], PZ[2]);
  MultiplyPoints  (a_matrix, points);
  if (OContextGetModeling(OContextGetStaticInstance())==OModelingSolid) 
    GoAddPolygonToNode (This, points); 
  else  
    GoAddLinesToNode  (This, points);

  OPointListDelete (points);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void* GoRepresentOContextmap (
 void* a_obj
,void* a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OContextmap This;
  int         contextn;
  ONode       onode = NULL;
  OMatrix     matrix;
  OContext    label_context;
  OPacking    packing;
  int         count;
  double      height, width;
  double      x,y,z,shift,length;
  double      xi,yi,zi,halfsize;
/*.........................................................................*/
  if(a_obj==NULL)        return NULL;

  This       = (OContextmap)a_obj;
  contextn   = OContextmapGetContextNumber(This);
#ifdef DEBUG
  printf ("debug : GoRepresentOContextmap : %s %d\n",OContextmapGetName(This),contextn);
#endif
  if(contextn==0)  return NULL;

  onode               = ONodeCreateF (64,"OContextmap/%s",OContextmapGetName(This));
  ONodeSetPickability (onode,0);

  height   = OContextmapGetLabelHeight(This);
  packing  = OContextmapGetLabelPacking(This);
  OContextmapGetLabelPosition (This,&x,&y,&z);
  shift    = height * OContextmapGetBaseLineStretch();
  width    = height * OContextmapGetLabelWidthStretch();
  matrix   = OMatrixCreate (OMatrixScale,width,height,1.);

  halfsize = OContextmapGetIconSize (This) / 2.;
  xi       = x;
  yi       = y;
  zi       = z;
  
  OContextmapSetLabelContext (This); /* save to be fast */
  label_context              = OContextMake();
  OContextCopy               (label_context,OContextGetStaticInstance());
  
  /* title */
  if(OContextmapGetTitle(This)!=NULL)
    {
      length = GoAddPAW_StringToNode (onode,OContextmapGetTitle(This),xi,yi,zi,matrix);
      if (packing==OPackingVertical)
	yi -= shift;
      else
	x += length*width + width;
      xi = x;
    }

  for(count=0;count<contextn;count++)
    {
      int   iconTypes;
      char* label;

      if ( (OContextmapShowHitsOnly(This)==1) && 
	   (OContextmapGetIthContextHitNumber(This,count)==0)
	   ) continue;
      
#ifdef DEBUG
  printf ("debug : GoRepresentOContextmap : %s %d %d\n",OContextmapGetName(This),contextn,count);
#endif

      if (packing==OPackingVertical) xi = x;
      xi  += halfsize;
      yi  += height * 0.4;
      
      /* icons */
      OContextmapSetIthContext (This,count);
      iconTypes = OContextmapGetIconTypes (This);
      if (iconTypes & ICON_LINE)
	{
	  GoAddLineToNode(onode, xi-halfsize*1.8,yi,zi,xi+halfsize*2.0,yi,zi);
	}
      if (iconTypes & ICON_MARKER)
	{
	  GoAddMarkerToNode(onode,xi,yi,zi);
	}
      if (iconTypes & ICON_CIRCLE)
	{
	  GoAddCircleToNode(onode,halfsize,xi,yi,zi,0.,0.,1.);
	}
      if (iconTypes & ICON_SQUARE)
	{
	  GoAddRectangleToNode (onode,halfsize*2.,halfsize*2.,xi,yi,zi,0.,0.,1.);
	}
      if (iconTypes & ICON_DIAMOND)
	{
	  GoAddDiamondToNode (onode,halfsize*2.,halfsize*2.,xi,yi,zi,0.,0.,1.);
	}
      
      xi += halfsize * 3.0;
      yi -= height   * 0.4;
      /*labels*/
      OContextCopy  (OContextGetStaticInstance(),label_context);
      label         = OContextmapGetContextLabel (This,count);
      length        = GoAddPAW_StringToNode (onode,label,xi,yi,zi,matrix);
      CStringDelete (label);
      
      if (packing==OPackingVertical)
	yi -= shift;
      else
	xi += length*width + width;
    }
  
  OContextDelete   (label_context);
  OMatrixDelete    (matrix);
  
  /* rotate icon-label pairs */
  matrix   = OContextmapGetLabelRotationMatrix(This);
  if(matrix!=NULL) ONodeMultiplyMatrix (onode,matrix);

  a_type = NULL;
  return onode;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int GoDoMethod (
 void*  This
,char*  a_name
,void*  a_user
,int    a_argn
,char** a_args
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;


       if(strcmp(a_name,"representFrame")==0) 
    {
      double value = 1.;
      int    status;
      ONode  onode;
      if( (a_argn!=2) && (a_argn!=3) )
	{
	  CWarnF ("Two or three arguments expected for representFrame method.\n");
	  return 0;
	}
      value = CStringConvertToDouble(a_args[1],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[1]);
	  return 0;
	}
      onode  = ONodeCreate (a_args[0]);
      if(onode==NULL) 
	{
	  CWarnF ("Can't create ONode %s.\n",a_args[0]);
	  return 0;
	}
      if(a_argn==3)
	{
	  if(strcmp(a_args[2],"rz")==0)
	    GoAddRZ_FrameToNode (onode,value);
	  else
	    GoAddFrameToNode    (onode,value);
	}
      else
        GoAddFrameToNode  (onode,value);
      sprintf           (Class.string,"ONode/%lu",(unsigned long)onode);
      *((char**)a_addr) = Class.string;
    }
  else if(strcmp(a_name,"representAxis")==0) 
    {
      OCamera ocamera;
      OPick   pick1,pick2;
      if(a_argn!=2) 
	{
	  CWarnF ("Two arguments expected for representFrame method.\n");
	  return 0;
	}
      ocamera    = OCameraGetIdentifier(a_args[1]);
      if(ocamera==NULL) 
	{
	  CWarnF ("OCamera %s is unknown.\n",a_args[1]);
	  return 0;
	}
      pick1      = OCameraGetPick(ocamera,0);
      pick2      = OCameraGetPick(ocamera,1);
      if( (pick1!=NULL) && (pick2!=NULL) )
	{
	  ONode  onode = NULL;
	  double x1,y1,z1;
	  double x2,y2,z2;
	  onode  = ONodeCreate(a_args[0]);
	  if(onode==NULL) 
	    {
	      CWarnF ("Can't create ONode %s.\n",a_args[0]);
	      return 0;
	    }
	  OPickGetPoint   (pick1,&x1,&y1,&z1);
	  OPickGetPoint   (pick2,&x2,&y2,&z2);
	  GoAddAxisToNode (onode,x1,y1,z1,x2,y2,z2);
	  sprintf         (Class.string,"ONode/%lu",(unsigned long)onode);
	  *((char**)a_addr) = Class.string;
	}
      else
	{
	  *((char**)a_addr) = NULL;
	}
    }
  else if(strcmp(a_name,"representTRAP")==0) 
    {
      double DZ;
      double TX;
      double TY;
      double H1;
      double BL1;
      double TL1;
      double TTH1;
      double H2;
      double BL2;
      double TL2;
      double TTH2;
      int    status;
      ONode  onode;
      if(a_argn!=12)
	{
	  CWarnF ("Twelve arguments expected for representFrame method.\n");
	  return 0;
	}
      DZ = CStringConvertToDouble(a_args[1],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[1]);
	  return 0;
	}
      TX = CStringConvertToDouble(a_args[2],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[2]);
	  return 0;
	}
      TY = CStringConvertToDouble(a_args[3],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[3]);
	  return 0;
	}
      H1 = CStringConvertToDouble(a_args[4],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[4]);
	  return 0;
	}
      BL1 = CStringConvertToDouble(a_args[5],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[5]);
	  return 0;
	}
      TL1 = CStringConvertToDouble(a_args[6],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[6]);
	  return 0;
	}
      TTH1 = CStringConvertToDouble(a_args[7],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[7]);
	  return 0;
	}
      H2 = CStringConvertToDouble(a_args[8],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[8]);
	  return 0;
	}
      BL2 = CStringConvertToDouble(a_args[9],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[9]);
	  return 0;
	}
      TL2 = CStringConvertToDouble(a_args[10],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[10]);
	  return 0;
	}
      TTH2 = CStringConvertToDouble(a_args[11],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[11]);
	  return 0;
	}
      onode  = ONodeCreate (a_args[0]);
      if(onode==NULL) 
	{
	  CWarnF ("Can't create ONode %s.\n",a_args[0]);
	  return 0;
	}
      GoAddGEANT_TRAP_ToNode (onode,NULL,DZ,TX,TY,H1,BL1,TL1,TTH1,H2,BL2,TL2,TTH2);
      sprintf                (Class.string,"ONode/%lu",(unsigned long)onode);
      *((char**)a_addr)      = Class.string;
    }
  else 
    {
      CInfoF("GoDoMethod: %s is not a method property.\n",a_name);
      return 0;
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  This   = NULL;
  a_addr = NULL;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Go","Go",a_name,a_code);
}
