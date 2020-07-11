/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
static char what[] = "@(#)GLo v3";

/*
#define DEBUG
*/
/*
  In OpenGL defaults are :
   eye    = (0.,0., 0.) 
   vrp    = (0.,0.,-1.) window place.
   center = (0.,0.,-2.) looked point.
   dop    = (0.,0.,-1.) direction of projection.
   up     = (0.,1.,0.);
*/

/*
   cam_total = cam_ndc * cam_perspective * cam_projection * cam_canonic

parallel: 
   NDC               PARA          PROJ   
   |w  0  0  w|    
   |0 -h  0  h|       ID            ID
   |0  0  1  0|    
   |0  0  0  1|    

perspective:
   NDC               PERS               PROJ   
   |w  0  0  w|    |1/sd  0     0     0|  |1   0   0    0  |
   |0 -h  0  h|    |0     1/sd  0     0|  |0   1   0    0  |
   |0  0  1  0|    |0     0     b/sd sc|  |0   0   1    a/b|
   |0  0  0  1|    |0     0     0     1|  |0   0   1/d  0  |

   |w  0  0  w|    |1/sd  0     0          0   |
   |0 -h  0  h|    |0     1/sd  0          0   |
   |0  0  1  0|    |0     0     b/sd+sc/d  a/sd|
   |0  0  0  1|    |0     0     1/d        0   |

   |w/sd  0     w/d        0   |
   |0    -h/sd  h/d        0   |
   |0     0     b/sd+sc/d  a/sd|
   |0     0     1/d        0   |

    x          w(z+x)/sd           sw(z+x)/z
    y   -->    h(z-y)/sd        -> sh(z-y)/z 
    z          (as+(bs+sc)z)/d     s(a/z+b+c) = f(z)
    1          z/d                 1

where s = signe(d)   [sd = fabs(d)]


for a plane:
      Ax + By + Cz + D = 0
 ->   Asw(z+x)/z + Bsh(z-y)/z+C(sb+sc+sa/z) + D   = 0
 ->   (Asw)x + (-Bsh)y + s(Aw+Bh+C(b+c)+D)z + Csa = 0     it is still a plane.

in Vandam: a=0, b=d, c=-d then
    x          w(z+x)/d      w(z+x)/z
    y   -->    h(z-y)/d   -> h(z-y)/z 
    z          z             0
    1          z/d           1

for zbuffer we need:
 - to be sensible in z (-> a!=0).
 - that a plane is still a plane.
 - f(z) = s(a/z+b+c) increase with z (sa<0).
 
we take:  
  b = 1, c = 0

 d>0 s=1  -> a=-1
    x          w(z+x)/d      w(z+x)/z
    y   -->    h(z-y)/d   -> h(z-y)/z 
    z          (1+z)/d        1-1/z
    1          z/d           1
 d<0 s=-1 -> a=1
    x          w(z+x)/d      w(z+x)/z
    y   -->    h(z-y)/d   -> h(z-y)/z 
    z          (1+z)/d       -1-1/z
    1          z/d           1
implementation:
  zproj = d;
PERS is set in: ComputeViewing:
      matrix                = OMatrixCreate (OMatrixScale,1./fabs(zproj),1./fabs(zproj),1./fabs(zproj));
PROJ is set in: SetMatrixViewing:
      OMatrixInitialize     (matrix,OMatrixIdentity);
      OMatrixSetElement32   (matrix,1/zproj);
      OMatrixSetElement33   (matrix,0.);
      OMatrixSetElement23   (matrix,zproj>0. ? -1.:1.);
*/

#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

/*Co*/
#include <CMemory.h>
#include <CPrinter.h>
#include <OMatrixP.h>

/*GLo*/
#include <glDriverZ.h>
#include <glList.h>

typedef struct _GPointRec *GPoint;

typedef struct _GPointRec 
{
  double    x,y,z;
} GPointRec;

#include <gl.h>

#include <glDriver.h>

typedef enum {
  GProjectionParallel,
  GProjectionPerspective
} GProjectionType;

typedef unsigned long Ulong;

typedef int           (*ClipPointFunction)(GPoint,int,double);
typedef int           (*ClipLineFunction) (GPoint,GPoint,int,double,int*);


#ifdef __cplusplus
extern "C"{
#endif
/*debug: Not GL*/
void      glPointStyle    (GLenum);

static void   InitializeFrame       ();
static void   SetProjection         (GProjectionType,double,double,double,double,double,double);
static void   SetMatrixViewing      (GProjectionType,GPoint,GPoint,GPoint,GPoint,double,double,double,double,double,double,OMatrix,OMatrix,double*,double*);
static Ulong  ConvertRgbToPixel     (double,double,double);
static void   MultiplyMatrix        (OMatrix);
static void   ReportError           (char*,int);

static void   CheckGlobal           ();
static double GetNorme              (GPoint);
static double NormalizeVector       (GPoint);
static void   GetCrossProduct       (GPoint,GPoint,GPoint);
static void   MultiplyPoint         (GPoint,OMatrix);
static void   Select                (int,int,int,int);

static int    ClipPolyline          (int,int,double*,double*,double*,OMatrix,GPoint);
static int    ClipPointParallel     (GPoint,int,double);
static int    ClipLineParallel      (GPoint,GPoint,int,double,int*);
static int    ClipPointPerspective  (GPoint,int,double);
static int    ClipLinePerspective   (GPoint,GPoint,int,double,int*);
static Ulong  GetOutParallel        (GPoint,int);
static Ulong  GetOutPrspective      (GPoint,int,double);

static void   MultiplyPoints          (OMatrix);
static void   ClearPoints           ();
#ifdef __cplusplus
}
#endif

#define CHECK_DIV(a_var,a_pro) {if((a_var)==0.) {ReportError (a_pro,GL_INVALID_VALUE);(a_var)=1.;}}
#define FILTER__NOZ            0xf
#define FILTER__Z              0x3f
#define MINIMUM(a,b)           ((a)<(b)?a:b)
#define MAXIMUM(a,b)           ((a)>(b)?a:b)

#define GL_NOT_IMPLEMENTED 0

#define MAX_POINTS 256
#define MAX_NAMES  100
#define MAX_VIEWS   32
#define MAX_PROJS   8
static struct 
{
  int                 error;
  int                 initFrame;
  int                 initProj;

  glClearClassFunction    clearClassProc;
  glRGB_ToPixelFunction   rgbPixelProc;
  glPixelToRGB_Function   pixelRgbProc;
/*Called if renderMode is GL_RENDER.*/
  glSetViewportFunction   setViewportProc;
  glClearColorBufferFunction clearColorProc;
  glDrawPrimitiveFunction drawPrimitiveProc;
  glDrawBufferFunction    drawBufferProc;

  glSetViewportFunction   driverSetViewportProc;
  glClearColorBufferFunction driverClearColorProc;
  glDrawPrimitiveFunction driverDrawPrimitiveProc;
  glDrawBufferFunction    driverDrawBufferProc;

  int                 begin;
  int                 clippedName;
  GLenum              vertexMode;
  glContextRecord     atb;
  glColorRecord       backColor;
  double              clearDepth;
  GLenum              matrixMode;
  OMatrixRecord       tempMatrix;
  OMatrixRecord       projMatrix,viewMatrix;

  int                 computeGlobal;
  OMatrixRecord       global;

  int                 viewportX,viewportY,viewportWidth,viewportHeight;
  GProjectionType     projection;
  ClipPointFunction   clipPoint;
  ClipLineFunction    clipLine;
  double              zmin;

  GLboolean           depthTest;
  GLboolean           normalize;
  GLenum              renderMode;
  GLint               hitn,selecti;
  GLsizei             selectSize;
  GLuint*             selectBuffer;

  double              rasterPosX,rasterPosY,rasterPosZ;

  int                 namei;
  int                 viewi;
  int                 proji;

  int                 pointi;
  int                 pointn;
  double*             pointx;
  double*             pointy;
  double*             pointz;

  GLuint              lists;
  glList              list;

  GLuint              names[MAX_NAMES];
  OMatrixRecord       views[MAX_VIEWS];
  OMatrixRecord       projs[MAX_PROJS];

} Class = {
  0,                                    /*error*/
  0,0,

  NULL,NULL,NULL,NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,                  /*driverProc*/

  0,                                    /*begin*/
  -1,                                   /*clippedName*/
  GL_POINTS,                            /*vertexMode*/
  {{1.,1.,1.,1L,0},1,glLineSolid,glMarkerDot,glAreaSolid,GL_FILL},  /*atb*/
  {0.,0.,0.,0L,0},                      /*backColor*/
  -1.,                                  /*clearDepth*/

  GL_MODELVIEW,
  OMatrix_identity,                     /*tempMatrix*/
  OMatrix_identity,OMatrix_identity,    /*projMatrix,viewMatrix*/

  0,                                    /*computeGlobal*/
  OMatrix_identity,                      

  0,0,100,100,                          /*viewportX,viewportY,viewportWidth,viewportHeight*/
  GProjectionParallel,                  /*projection*/
  ClipPointParallel,                    /*clipPoint*/
  ClipLineParallel,                     /*clipLine*/

  0.,                                   /*zmin*/

  GL_FALSE,                             /*depthTest*/
  GL_FALSE,                             /*normalize*/
  GL_RENDER,                            /*renderMode*/
  0,0,                                  /*hitn,selecti*/
  0,                                    /*selectSize*/
  NULL,                                 /*selectBuffer*/

  0.,0.,0.,                             /*rasterPosX,rasterPosY,rasterPosZ*/

  -1,-1,-1,                             /*namei,viewi,proji*/
  0,0,NULL,NULL,NULL,                   /*pointi,pointn,pointx,pointy,pointz*/
  1,NULL                                /*lists*/
};
/***************************************************************************/
void glSetDriver (
 glClearClassFunction a_clearClass 
,glRGB_ToPixelFunction a_rgbPixel
,glPixelToRGB_Function a_pixelRgb
,glSetViewportFunction a_setViewport
,glClearColorBufferFunction  a_clearColor
,glDrawPrimitiveFunction a_drawPrimitive
,glDrawBufferFunction a_drawBuffer
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.clearClassProc     = a_clearClass;
  Class.rgbPixelProc       = a_rgbPixel;
  Class.pixelRgbProc       = a_pixelRgb;
  Class.setViewportProc    = a_setViewport;
  Class.clearColorProc     = a_clearColor;
  Class.drawPrimitiveProc  = a_drawPrimitive;
  Class.drawBufferProc     = a_drawBuffer;

  Class.driverSetViewportProc   = Class.setViewportProc;
  Class.driverClearColorProc    = Class.clearColorProc;
  Class.driverDrawPrimitiveProc = Class.drawPrimitiveProc;
  Class.driverDrawBufferProc    = Class.drawBufferProc;
}
/***************************************************************************/
void glClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.clearClassProc!=NULL) Class.clearClassProc ();

  Class.rasterPosX         = 0.;
  Class.rasterPosY         = 0.;
  Class.rasterPosZ         = 0.;

  glDriverZ_ClearClass     ();
  glListClearClass         ();
  Class.lists              = 1;
  Class.list               = NULL;

  ClearPoints              ();

  Class.clearClassProc    = NULL;
  Class.rgbPixelProc      = NULL;
  Class.pixelRgbProc      = NULL;
  Class.setViewportProc   = NULL;
  Class.clearColorProc    = NULL;
  Class.drawPrimitiveProc = NULL;
  Class.drawBufferProc    = NULL;

  what[0] = '@'; /*c++ no warning.*/
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void glBegin (
 GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.list!=NULL)
    {
#ifdef DEBUG
      printf      ("debug : glBegin : store in list.\n");
#endif
      glListAddOperation (Class.list,glOperation_glBegin,(glListOperationFunction)glBegin,a_mode);
      return;
    }
#ifdef DEBUG
  printf      ("debug : glBegin : do it.\n");
#endif

  if(Class.begin==1)   {ReportError("glBegin",GL_INVALID_OPERATION);return;}
  if( (a_mode!=GL_POINTS)     &&
      (a_mode!=GL_LINE_STRIP) &&
      (a_mode!=GL_LINE_LOOP)  &&
      (a_mode!=GL_LINES)      &&
      (a_mode!=GL_QUADS)      &&
      (a_mode!=GL_POLYGON) ) {ReportError("glBegin",GL_INVALID_ENUM);return;}
  Class.begin       = 1;
  Class.vertexMode  = a_mode;
  Class.pointi      = 0;
}
/***************************************************************************/
void glVertex3d (
 GLdouble a_x
,GLdouble a_y
,GLdouble a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.list!=NULL)
    {
#ifdef DEBUG
      printf      ("debug : glVertex3d : store in list.\n");
#endif
      glListAddOperation (Class.list,glOperation_glVertex3d,(glListOperationFunction)glVertex3d,a_x,a_y,a_z);
      return;
    }
#ifdef DEBUG
  printf      ("debug : glVertex3d : do it.\n");
#endif

  if(Class.begin==0) {ReportError ("glVertex3d",GL_INVALID_OPERATION);return;}
  if(Class.pointi>=Class.pointn)
    {
      if(Class.pointn==0)
        {
	  Class.pointn  = MAX_POINTS;
	  Class.pointx  = (double*)CMemoryAllocateBlock (Class.pointn * sizeof(double));
	  Class.pointy  = (double*)CMemoryAllocateBlock (Class.pointn * sizeof(double));
	  Class.pointz  = (double*)CMemoryAllocateBlock (Class.pointn * sizeof(double));
	}       
      else
        {
	  Class.pointn += MAX_POINTS;
	  Class.pointx  = (double*)CMemoryChangeBlockSize (Class.pointx, Class.pointn * sizeof(double));
	  Class.pointy  = (double*)CMemoryChangeBlockSize (Class.pointy, Class.pointn * sizeof(double));
	  Class.pointz  = (double*)CMemoryChangeBlockSize (Class.pointz, Class.pointn * sizeof(double));
	}
      if ( (Class.pointx==NULL) || 
	   (Class.pointy==NULL) || 
	   (Class.pointz==NULL)
	  )
	{
	  ClearPoints ();
          return;
	}
    }
  Class.pointx[Class.pointi] = a_x;
  Class.pointy[Class.pointi] = a_y;
  Class.pointz[Class.pointi] = a_z;
  Class.pointi++;
}
/***************************************************************************/
void glVertex2i (
 GLint a_x
,GLint a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glVertex3d((GLdouble)a_x,(GLdouble)a_y,0.);
}
/***************************************************************************/
void glVertex2d (
 GLdouble a_x
,GLdouble a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glVertex3d(a_x,a_y,0.);
}
/***************************************************************************/
void glVertex3f (
 GLfloat a_x
,GLfloat a_y
,GLfloat a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glVertex3d ((GLdouble)a_x,(GLdouble)a_y,(GLdouble)a_z);
}
/***************************************************************************/
void glVertex2f (
 GLfloat a_x
,GLfloat a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glVertex3d ((GLdouble)a_x,(GLdouble)a_y,0.);
}
/***************************************************************************/
void glVertex2fv (
 const GLfloat* a_v
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glVertex3d ((GLdouble)(a_v[0]),(GLdouble)(a_v[1]),0.);
}
/***************************************************************************/
void glVertex2s (
 GLshort a_x
,GLshort a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glVertex3d ((GLdouble)a_x,(GLdouble)a_y,0.);
}
/***************************************************************************/
void glVertex2sv (
 const GLshort* a_v
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glVertex2s (a_v[0],a_v[1]);
}
/***************************************************************************/
void glLineWidth (
 GLfloat a_width
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.list!=NULL)
    {
#ifdef DEBUG
      printf      ("debug : glLineWidth : store in list : %f.\n",a_width);
#endif
      /*va_arg and float does not work quite well, better to pass a double.*/
      glListAddOperation (Class.list,glOperation_glLineWidth,(glListOperationFunction)glLineWidth,(GLdouble)a_width);
      return;
    }
#ifdef DEBUG
  printf      ("debug : glLineWidth : do it : %f.\n",a_width);
#endif

  if(Class.begin==1) {ReportError ("glLineWidth",GL_INVALID_OPERATION);return;}
  if(a_width<=0.)    {ReportError ("glLineWidth",GL_INVALID_VALUE);return;}
  Class.atb.width = (int)a_width;
}
/***************************************************************************/
void glLineStipple (
 GLint    a_factor
,GLushort a_pattern
)
/***************************************************************************/
/*
  Must do a glEnable(GL_LINE_STIPPLE).
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glLineStipple",GL_INVALID_OPERATION);return;}
       if(a_pattern==0x0000)  Class.atb.lineStyle = glLineSolid;
  else if(a_pattern==0x00FF)  Class.atb.lineStyle = glLineDashed;
  else if(a_pattern==0x0101)  Class.atb.lineStyle = glLineDotted;
  else if(a_pattern==0x1C47)  Class.atb.lineStyle = glLineDashDotted;
  else                        Class.atb.lineStyle = glLineSolid;
  a_factor = 0;
}
/***************************************************************************/
void glPointSize (
 GLfloat a_size
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glPointSize",GL_INVALID_OPERATION);return;}
  if(a_size<=0.)  {ReportError ("glPointSize",GL_INVALID_VALUE);return;}
  Class.atb.width = (int)a_size;
}
/***************************************************************************/
void glFlush (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)         {ReportError("glFlush",GL_INVALID_OPERATION);return;}
}
/***************************************************************************/
void glFinish (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)         {ReportError ("glFinish",GL_INVALID_OPERATION);return;}
}
/***************************************************************************/
void glFrustum (
 GLdouble a_left
,GLdouble a_right
,GLdouble a_bottom
,GLdouble a_top
,GLdouble a_near
,GLdouble a_far
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)    {ReportError ("glFrustum",GL_INVALID_OPERATION);return;}
  if(a_right<=a_left)   {ReportError ("glFrustum",GL_INVALID_VALUE);return;}
  if(a_top<=a_bottom)   {ReportError ("glFrustum",GL_INVALID_VALUE);return;}
  if(a_near<=0.)        {ReportError ("glFrustum",GL_INVALID_VALUE);return;}
  if(a_far<=0.)         {ReportError ("glFrustum",GL_INVALID_VALUE);return;}

  Class.clipPoint       = ClipPointPerspective;
  Class.clipLine        = ClipLinePerspective;

/*Produce Class.zmin. Multiply with Clas.projMatrix.*/
  SetProjection         (GProjectionPerspective,a_near,a_far,a_left,a_right,a_bottom,a_top);

}
/***************************************************************************/
void glOrtho (
 GLdouble a_left
,GLdouble a_right
,GLdouble a_bottom
,GLdouble a_top
,GLdouble a_near
,GLdouble a_far
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)    {ReportError ("glOrtho",GL_INVALID_OPERATION);return;}
  if(a_right<=a_left)   {ReportError ("glOrtho",GL_INVALID_VALUE);return;}
  if(a_top<=a_bottom)   {ReportError ("glOrtho",GL_INVALID_VALUE);return;}

  Class.clipPoint       = ClipPointParallel;
  Class.clipLine        = ClipLineParallel;

  SetProjection         (GProjectionParallel,a_near,a_far,a_left,a_right,a_bottom,a_top);

}
/***************************************************************************/
void glMatrixMode (
 GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glMatrixMode",GL_INVALID_OPERATION);return;}
  if( (a_mode!=GL_PROJECTION) &&
      (a_mode!=GL_MODELVIEW) ) {ReportError("glMatrixMode",GL_INVALID_ENUM);return;}
  Class.matrixMode  = a_mode;
}
/***************************************************************************/
void glLoadIdentity (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glLoadIdentity",GL_INVALID_OPERATION);return;}
       if(Class.matrixMode==GL_PROJECTION)
    {
      OMatrixInitialize (&(Class.projMatrix),OMatrixIdentity);
    }
  else if(Class.matrixMode==GL_MODELVIEW)
    {
      OMatrixInitialize (&(Class.viewMatrix),OMatrixIdentity);
    }
  Class.computeGlobal = 1;
}
/***************************************************************************/
void glLoadMatrixd (
 GLdouble* a_matrix
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glLoadMatrixd",GL_INVALID_OPERATION);return;}

  if(a_matrix==NULL) return;

       if(Class.matrixMode==GL_PROJECTION)
    {
      OMatrixInitialize           (&(Class.projMatrix),OMatrixFollowing,
                                   a_matrix[0],a_matrix[4],a_matrix[8] ,a_matrix[12],
				   a_matrix[1],a_matrix[5],a_matrix[9] ,a_matrix[13],
				   a_matrix[2],a_matrix[6],a_matrix[10],a_matrix[14],
				   a_matrix[3],a_matrix[7],a_matrix[11],a_matrix[15]);

    }
  else if(Class.matrixMode==GL_MODELVIEW)
    {
      OMatrixInitialize           (&(Class.viewMatrix),OMatrixFollowing,
                                   a_matrix[0],a_matrix[4],a_matrix[8] ,a_matrix[12],
				   a_matrix[1],a_matrix[5],a_matrix[9] ,a_matrix[13],
				   a_matrix[2],a_matrix[6],a_matrix[10],a_matrix[14],
				   a_matrix[3],a_matrix[7],a_matrix[11],a_matrix[15]);
    }

  Class.computeGlobal = 1;

}
/***************************************************************************/
void glMultMatrixd (
 const GLdouble* a_matrix
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glMultMatrixd",GL_INVALID_OPERATION);return;}

  if(a_matrix==NULL) return;

  OMatrixInitialize (&(Class.tempMatrix),OMatrixFollowing,
                                   a_matrix[0],a_matrix[4],a_matrix[8] ,a_matrix[12],
				   a_matrix[1],a_matrix[5],a_matrix[9] ,a_matrix[13],
				   a_matrix[2],a_matrix[6],a_matrix[10],a_matrix[14],
				   a_matrix[3],a_matrix[7],a_matrix[11],a_matrix[15]);

  MultiplyMatrix (&(Class.tempMatrix));

}
/***************************************************************************/
void glMultMatrixf (
 const GLfloat* a_matrix
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glMultMatrixf",GL_INVALID_OPERATION);return;}

  if(a_matrix==NULL) return;

  OMatrixInitialize (&(Class.tempMatrix),OMatrixFollowing,
		     (double)a_matrix[0],(double)a_matrix[4],(double)a_matrix[8] ,(double)a_matrix[12],
		     (double)a_matrix[1],(double)a_matrix[5],(double)a_matrix[9] ,(double)a_matrix[13],
		     (double)a_matrix[2],(double)a_matrix[6],(double)a_matrix[10],(double)a_matrix[14],
		     (double)a_matrix[3],(double)a_matrix[7],(double)a_matrix[11],(double)a_matrix[15]);

  MultiplyMatrix (&(Class.tempMatrix));

}
/***************************************************************************/
void glPushMatrix (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glPushMatrix",GL_INVALID_OPERATION);return;}
       if(Class.matrixMode==GL_PROJECTION)
    {
      if(Class.proji<=0)              Class.proji = 0;
      if(Class.proji>=MAX_PROJS)      {ReportError ("glPushMatrix",GL_STACK_OVERFLOW);return;}
      OMatrixCopy                     (&(Class.projs[Class.proji]),&(Class.projMatrix));
      Class.proji++;
    }
  else if(Class.matrixMode==GL_MODELVIEW)
    {
      if(Class.viewi<=0)              Class.viewi = 0;
      if(Class.viewi>=MAX_VIEWS)      {ReportError ("glPushMatrix",GL_STACK_OVERFLOW);return;}
      OMatrixCopy                     (&(Class.views[Class.viewi]),&(Class.viewMatrix));
      Class.viewi++;
    }
}
/***************************************************************************/
void glPopMatrix (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glPopMatrix",GL_INVALID_OPERATION);return;}
       if(Class.matrixMode==GL_PROJECTION)
    {
      if(Class.proji<=0) {ReportError ("glPopMatrix",GL_STACK_UNDERFLOW);return;}
      Class.proji--;
      OMatrixCopy    (&(Class.projMatrix),&(Class.projs[Class.proji]));
    }
  else if(Class.matrixMode==GL_MODELVIEW)
    {
      if(Class.viewi<=0) {ReportError ("glPopMatrix",GL_STACK_UNDERFLOW);return;}
      Class.viewi--;
      OMatrixCopy    (&(Class.viewMatrix),&(Class.views[Class.viewi]));
    }
  Class.computeGlobal = 1;
}
/***************************************************************************/
void glRotatef (
 GLfloat a_angle
,GLfloat a_x
,GLfloat a_y
,GLfloat a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glRotated ((GLdouble)a_angle,(GLdouble)a_x,(GLdouble)a_y,(GLdouble)a_z);
}
/***************************************************************************/
void glRotated (
 GLdouble a_angle
,GLdouble a_x
,GLdouble a_y
,GLdouble a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glRotated",GL_INVALID_OPERATION);return;}

  OMatrixInitialize  (&(Class.tempMatrix),OMatrixRotationAxis,a_angle * M_PI/180.,a_x,a_y,a_z);

  MultiplyMatrix     (&(Class.tempMatrix));
}
/***************************************************************************/
void glTranslated (
 GLdouble a_x
,GLdouble a_y
,GLdouble a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glTranslated",GL_INVALID_OPERATION);return;}

  OMatrixInitialize (&(Class.tempMatrix),OMatrixTranslation,a_x,a_y,a_z);

  MultiplyMatrix (&(Class.tempMatrix));

}
/***************************************************************************/
void glTranslatef (
 GLfloat a_x
,GLfloat a_y
,GLfloat a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glTranslated ((GLdouble)a_x,(GLdouble)a_y,(GLdouble)a_z);
}
/***************************************************************************/
void glScalef (
 GLfloat a_x
,GLfloat a_y
,GLfloat a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glScaled ((GLdouble)a_x,(GLdouble)a_y,(GLdouble)a_z);
}
/***************************************************************************/
void glScaled (
 GLdouble a_x
,GLdouble a_y
,GLdouble a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glScaled",GL_INVALID_OPERATION);return;}

  OMatrixInitialize (&(Class.tempMatrix),OMatrixScale,a_x,a_y,a_z);

  MultiplyMatrix (&(Class.tempMatrix));

}
/***************************************************************************/
void glPointStyle (
 GLenum a_style
)
/***************************************************************************/
/*
  Not OpenGL complient.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glPointStyle",GL_INVALID_OPERATION);return;}
  Class.atb.markStyle = (glMarker)a_style;
}
/***************************************************************************/
/****** Need a driver ******************************************************/
/***************************************************************************/
/***************************************************************************/
void glEnd (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.list!=NULL)
    {
#ifdef DEBUG
      printf      ("debug : glEnd : store in list.\n");
#endif
      glListAddOperation (Class.list,glOperation_glEnd,(glListOperationFunction)glEnd);
      return;
    }

#ifdef DEBUG
  printf      ("debug : glEnd : do it.\n");
#endif

  if(Class.begin==0) {ReportError("glEnd",GL_INVALID_OPERATION);return;}

  InitializeFrame ();

  if(Class.initProj==0) 
    { /*Default ortho projection:  umin,umax,vmin,vmax = -1.,1.,-1.,1. */
      GLenum           matrixMode;
      matrixMode       = Class.matrixMode;
      Class.matrixMode = GL_PROJECTION;
      SetProjection    (GProjectionParallel,1.,2.,-1.,1.,-1.,1.);
      Class.matrixMode = matrixMode;
    }
  
  Class.begin = 0;

  switch (Class.vertexMode)
    {
      case GL_POINTS:
        if(Class.pointi<=0) {ReportError ("glEnd",GL_INVALID_VALUE);return;}
        break;
      case GL_LINE_STRIP:
      case GL_LINES:
        if(Class.pointi<=1) {ReportError ("glEnd",GL_INVALID_VALUE);return;}
        break;
      case GL_QUADS:
        if(Class.pointi<=3) {ReportError ("glEnd",GL_INVALID_VALUE);return;}
        break;
      case GL_LINE_LOOP:
      case GL_POLYGON:
        if(Class.pointi<=2)  {ReportError ("glEnd",GL_INVALID_VALUE);return;}
	Class.pointx[Class.pointi] = Class.pointx[0];
	Class.pointy[Class.pointi] = Class.pointy[0];
	Class.pointz[Class.pointi] = Class.pointz[0];
	Class.pointi++;
        break;
    }

       if( (Class.renderMode==GL_RENDER) && (Class.drawPrimitiveProc!=NULL))
        {
	  CheckGlobal     ();
          MultiplyPoints  (&Class.global);
	  if(Class.vertexMode==GL_QUADS)
	    {
	      int   quadn,quadi;
              quadn = Class.pointi/4;
	      for(quadi=0;quadi<quadn;quadi++)         
		{
		  int    count;
		  double pointx[5];
		  double pointy[5];
		  double pointz[5];
		  for(count=0;count<4;count++)         
		    {
		      pointx[count] = *(Class.pointx + quadi * 4 + count);
		      pointy[count] = *(Class.pointy + quadi * 4 + count);
		      pointz[count] = *(Class.pointz + quadi * 4 + count);
		    }
		  pointx[4] = pointx[0];
		  pointy[4] = pointy[0];
		  pointz[4] = pointz[0];
		  Class.drawPrimitiveProc (GL_POLYGON,5,pointx,pointy,pointz,&(Class.atb));
		}
	    }
	  else
	    {
	      Class.drawPrimitiveProc (Class.vertexMode,Class.pointi,Class.pointx,Class.pointy,Class.pointz,&(Class.atb));
	    }
	}
  else if(Class.renderMode==GL_SELECT)
    {
      if(Class.error!=GL_STACK_OVERFLOW)
	{
	  GPointRec         point;
	  OMatrixRecord     matrix;
	  GLuint            name;
	  
	  name              = Class.names[Class.namei-1];  /*current name on stack.*/

	  OMatrixMultiplyAndAssign  (&matrix,&(Class.projMatrix),&(Class.viewMatrix)); /*SetProjection mul projMatrix by canonic.*/
      
	  if ( (ClipPolyline
		(Class.vertexMode,Class.pointi,Class.pointx,Class.pointy,Class.pointz,&matrix,&point)==1)  &&
	      ((int)name != Class.clippedName)   /*One hit per name.*/
	      )
	    { 
	       int count;
	      GLsizei  hit_size;
	      /*Clipped.*/
	      /*printf ("debug:clipped:%d\n",name);*/
	      hit_size = 3 + Class.namei;
	      if(Class.selecti+hit_size>=Class.selectSize)
		{
		  Class.error = GL_STACK_OVERFLOW;
		}
	      else
		{
		  Class.selectBuffer[Class.selecti] = Class.namei;
		  Class.selecti++;
		  Class.selectBuffer[Class.selecti] = 0;
		  Class.selecti++;
		  Class.selectBuffer[Class.selecti] = 0;
		  Class.selecti++;
		  for(count=0;count<Class.namei;count++)         
		    {
		      Class.selectBuffer[Class.selecti] = Class.names[count];
		      Class.selecti++;
		    }
		  Class.hitn++;
		  Class.clippedName = name;
		}
	    }
	} /*Class.error*/
    }

}
/***************************************************************************/
void glColor3d (
 GLdouble a_red
,GLdouble a_green
,GLdouble a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.list!=NULL)
    {
#ifdef DEBUG
      printf      ("debug : glColor3d : store in list.\n");
#endif
      glListAddOperation (Class.list,glOperation_glColor3d,(glListOperationFunction)glColor3d,a_red,a_green,a_blue);
      return;
    }
#ifdef DEBUG
  printf      ("debug : glColor3d : do it.\n");
#endif

  Class.atb.color.red   = a_red;
  Class.atb.color.green = a_green;
  Class.atb.color.blue  = a_blue;
  Class.atb.color.pixel = 1L;
  if(Class.rgbPixelProc!=NULL) 
    Class.atb.color.pixel = Class.rgbPixelProc(a_red,a_green,a_blue);
}
/***************************************************************************/
void glColor3fv (
 const GLfloat* a_rgbs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glColor3f (a_rgbs[0],a_rgbs[1],a_rgbs[2]);
}
/***************************************************************************/
void glColor3f (
 GLfloat a_red
,GLfloat a_green
,GLfloat a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glColor3d ((GLdouble)a_red,(GLdouble)a_green,(GLdouble)a_blue);
}
/***************************************************************************/
void glColor3ub (
 GLubyte a_red
,GLubyte a_green
,GLubyte a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glColor3d ((GLdouble)a_red,(GLdouble)a_green,(GLdouble)a_blue);
}
/***************************************************************************/
void glViewport (
 GLint a_x
,GLint a_y
,GLsizei a_width
,GLsizei a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)                {ReportError ("glViewport",GL_INVALID_OPERATION);return;}
  if((a_width<=0)||(a_height<=0))   {ReportError ("glViewport",GL_INVALID_VALUE);return;}

  Class.viewportX       = a_x;
  Class.viewportY       = a_y;
  Class.viewportWidth   = a_width;
  Class.viewportHeight  = a_height;

  if((Class.renderMode==GL_RENDER) && (Class.setViewportProc!=NULL) ) 
    Class.setViewportProc (Class.viewportX,Class.viewportY,Class.viewportWidth,Class.viewportHeight);

  Class.computeGlobal  = 1;

  Class.initFrame       = 1;

  a_x                   = 0;
  a_y                   = 0;
}
/***************************************************************************/
void glScissor (
 GLint a_x
,GLint a_y
,GLsizei a_width
,GLsizei a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glScissor",GL_INVALID_OPERATION);return;}
  a_x      = 0;
  a_y      = 0;
  a_width  = 0;
  a_height = 0;
}
/***************************************************************************/
void glShadeModel (
 GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glShadeModel",GL_INVALID_OPERATION);return;}
  a_mode = 0;
}
/***************************************************************************/
void glClear (
 GLbitfield a_mask
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glClear",GL_INVALID_OPERATION);return;}

       if((a_mask & GL_COLOR_BUFFER_BIT)==GL_COLOR_BUFFER_BIT)
    {
      InitializeFrame ();
      if((Class.renderMode==GL_RENDER) && (Class.clearColorProc!=NULL) ) 
        Class.clearColorProc(&Class.backColor);
    }

       if((a_mask & GL_DEPTH_BUFFER_BIT)==GL_DEPTH_BUFFER_BIT)
    {
      InitializeFrame ();
      if(Class.renderMode==GL_RENDER) glDriverZ_ClearDepthBuffer (Class.clearDepth);
    }
}
/***************************************************************************/
void glClearColor (
 GLclampf a_red
,GLclampf a_green
,GLclampf a_blue
,GLclampf a_alpha
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glClearColor",GL_INVALID_OPERATION);return;}
  Class.backColor.red   = a_red;
  Class.backColor.green = a_green;
  Class.backColor.blue  = a_blue;
  Class.backColor.pixel = 0L;
  if(Class.rgbPixelProc!=NULL) 
    Class.backColor.pixel = Class.rgbPixelProc(a_red,a_green,a_blue);
  a_alpha         = 0;
}
/***************************************************************************/
void glClearDepth (
 GLclampd a_depth
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glClearDepth",GL_INVALID_OPERATION);return;}
  if(a_depth<0.) return;
  if(a_depth>1.) return;
  Class.clearDepth = - a_depth;
}
/***************************************************************************/
void glSelectBuffer  (
 GLsizei a_size
,GLuint* a_buffer
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glSelectBuffer",GL_INVALID_OPERATION);return;}
  if(a_size<=0)      {ReportError ("glSelectBuffer",GL_INVALID_VALUE);return;}
  if(a_buffer==NULL) {ReportError ("glSelectBuffer",GL_INVALID_VALUE);return;}
  
  if(Class.renderMode==GL_SELECT) {ReportError ("glSelectBuffer",GL_INVALID_OPERATION);return;}

  Class.selectSize   = a_size;
  Class.selectBuffer = a_buffer;

}
/***************************************************************************/
GLint glRenderMode (
 GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int retval = 0;
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glRenderMode",GL_INVALID_OPERATION);return 0;}
  if( (a_mode!=GL_RENDER)   &&
      (a_mode!=GL_FEEDBACK) &&
      (a_mode!=GL_SELECT) 
    ) {ReportError("glRenderMode",GL_INVALID_ENUM);return 0;}
  
  if(a_mode==GL_SELECT)
    {
      if(Class.selectBuffer==NULL) {ReportError ("glSelectBuffer",GL_INVALID_OPERATION);return 0;}
      Class.error       = 0;
      Class.hitn        = 0;
      Class.selecti     = 0;
      Class.clippedName = -1;
    }

/*If old mode is GL_SELECT and new mode is not GL_SELECT:
  - get hit records.
  - reset name stack.
*/
  if( (Class.renderMode==GL_SELECT) &&
      (a_mode!=GL_SELECT)
    )
    { 
      if(Class.error==GL_STACK_OVERFLOW)
	{
	  ReportError       ("glRenderMode",GL_STACK_OVERFLOW);
	  retval      = -1;
	}
      else
	{
	  retval      = Class.hitn;
	}
      Class.error = 0;
      Class.namei = -1;
    }

  Class.renderMode = a_mode;

  return           retval;
}
/***************************************************************************/
void glInitNames (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)                 {ReportError ("glInitNames",GL_INVALID_OPERATION);return;}
  if(Class.renderMode!=GL_SELECT) return;
  Class.namei = -1;
}
/***************************************************************************/
void glLoadName (
 GLuint a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)    {ReportError ("glLoadName",GL_INVALID_OPERATION);return;}
  if(Class.namei<=0) {ReportError ("glLoadName",GL_INVALID_OPERATION);return;}
  Class.names[Class.namei-1] = a_name;
}
/***************************************************************************/
void glPushName (
 GLuint a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)                 {ReportError ("glPushName",GL_INVALID_OPERATION);return;}
  if(Class.renderMode!=GL_SELECT) return;
  if(Class.namei<=0)              Class.namei = 0;
  if(Class.namei>=MAX_NAMES)      {ReportError ("glPushName",GL_STACK_OVERFLOW);return;}
  Class.names[Class.namei]        = a_name;
  Class.namei++;
}
/***************************************************************************/
void glPopName (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)                 {ReportError ("glPopName",GL_INVALID_OPERATION);return;}
  if(Class.renderMode!=GL_SELECT) return;
  if(Class.namei<=0)              {ReportError ("glPopName",GL_STACK_UNDERFLOW);return;}
  Class.namei--;
}
/***************************************************************************/
void glPolygonMode (
 GLenum a_face
,GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)                 {ReportError ("glPolygonMode",GL_INVALID_OPERATION);return;}
  if( (a_mode!=GL_POINT) &&
      (a_mode!=GL_LINE)  &&
      (a_mode!=GL_FILL) 
    ) {ReportError ("glPolygonMode",GL_INVALID_ENUM);return;}
  Class.atb.polygonMode = a_mode;
  a_face = 0;
}
/***************************************************************************/
void glGetIntegerv (
 GLenum a_pname
,GLint* a_params
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)                 {ReportError ("glGetIntegerv",GL_INVALID_OPERATION);return;}
       if(a_pname==GL_VIEWPORT)  
    {
      a_params[0] = Class.viewportX;
      a_params[1] = Class.viewportY;
      a_params[2] = Class.viewportWidth;
      a_params[3] = Class.viewportHeight;
    }
  else if(a_pname==GL_RENDER_MODE)
    {
      a_params[0] = Class.renderMode;
    }
  else                            
    {
      ReportError("glGetIntegerv",GL_INVALID_ENUM);
      return;
    }
}
/***************************************************************************/
void glGetDoublev (
 GLenum a_pname
,GLdouble* a_params
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glGetDoublev",GL_INVALID_OPERATION);return;}
       if(a_pname==GL_MODELVIEW_MATRIX)
    {
      a_params[ 0] = Class.viewMatrix.e00;
      a_params[ 1] = Class.viewMatrix.e10;
      a_params[ 2] = Class.viewMatrix.e20;
      a_params[ 3] = Class.viewMatrix.e30;

      a_params[ 4] = Class.viewMatrix.e01;
      a_params[ 5] = Class.viewMatrix.e11;
      a_params[ 6] = Class.viewMatrix.e21;
      a_params[ 7] = Class.viewMatrix.e31;

      a_params[ 8] = Class.viewMatrix.e02;
      a_params[ 9] = Class.viewMatrix.e12;
      a_params[10] = Class.viewMatrix.e22;
      a_params[11] = Class.viewMatrix.e32;

      a_params[12] = Class.viewMatrix.e03;
      a_params[13] = Class.viewMatrix.e13;
      a_params[14] = Class.viewMatrix.e23;
      a_params[15] = Class.viewMatrix.e33;
    }
  else if(a_pname==GL_PROJECTION_MATRIX)
    {
      a_params[ 0] = Class.projMatrix.e00;
      a_params[ 1] = Class.projMatrix.e10;
      a_params[ 2] = Class.projMatrix.e20;
      a_params[ 3] = Class.projMatrix.e30;

      a_params[ 4] = Class.projMatrix.e01;
      a_params[ 5] = Class.projMatrix.e11;
      a_params[ 6] = Class.projMatrix.e21;
      a_params[ 7] = Class.projMatrix.e31;

      a_params[ 8] = Class.projMatrix.e02;
      a_params[ 9] = Class.projMatrix.e12;
      a_params[10] = Class.projMatrix.e22;
      a_params[11] = Class.projMatrix.e32;

      a_params[12] = Class.projMatrix.e03;
      a_params[13] = Class.projMatrix.e13;
      a_params[14] = Class.projMatrix.e23;
      a_params[15] = Class.projMatrix.e33;
    }
  else if(a_pname==GL_CURRENT_COLOR)
    {
      a_params[ 0] = Class.atb.color.red;
      a_params[ 1] = Class.atb.color.green;
      a_params[ 2] = Class.atb.color.blue;
      a_params[ 3] = 0.;
    }
  else                            
    {
      ReportError("glGetDoublev",GL_INVALID_ENUM);
      return;
    }
}
/***************************************************************************/
void glEnable (
 GLenum a_cap
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glEnable",GL_INVALID_OPERATION);return;}
  if(a_cap==GL_DEPTH_TEST)
    {
      Class.driverSetViewportProc   = Class.setViewportProc;
      Class.driverClearColorProc    = Class.clearColorProc;
      Class.driverDrawPrimitiveProc = Class.drawPrimitiveProc;
      Class.driverDrawBufferProc    = Class.drawBufferProc;

      Class.setViewportProc    = glDriverZ_SetViewport;
      Class.clearColorProc     = glDriverZ_ClearColorBuffer;
      Class.drawPrimitiveProc  = glDriverZ_DrawPrimitive;
      Class.drawBufferProc     = glDriverZ_DrawBuffer;

      Class.depthTest          = GL_TRUE;
    }
  else if(a_cap==GL_NORMALIZE)
    {
      Class.normalize          = GL_TRUE;
    }
  else if(a_cap==GL_SCISSOR_TEST)
    {
    }
  else 
    {
      ReportError("glEnable",GL_INVALID_ENUM);
      return;
    }
}
/***************************************************************************/
void glDisable (
 GLenum a_cap
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glDisable",GL_INVALID_OPERATION);return;}
  if(a_cap==GL_DEPTH_TEST)
    {
      Class.setViewportProc   = Class.driverSetViewportProc;
      Class.clearColorProc    = Class.driverClearColorProc;
      Class.drawPrimitiveProc = Class.driverDrawPrimitiveProc;
      Class.drawBufferProc    = Class.driverDrawBufferProc;

      Class.depthTest         = GL_FALSE;
    }
  else if(a_cap==GL_NORMALIZE)
    {
      Class.normalize         = GL_FALSE;
    }
  else if(a_cap==GL_SCISSOR_TEST)
    {
    }
  else 
    {
      ReportError("glDisable",GL_INVALID_ENUM);
      return;
    }
}
/***************************************************************************/
GLboolean glIsEnabled (
 GLenum a_cap
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glIsEnable",GL_INVALID_OPERATION);return GL_FALSE;}
  if(a_cap==GL_DEPTH_TEST)
    {
      return Class.depthTest;
    }
  else if(a_cap==GL_NORMALIZE)
    {
      return Class.normalize;
    }
  else {ReportError("glIsEnable",GL_INVALID_ENUM);return GL_FALSE;}
}
/***************************************************************************/
GLuint glGenLists (
 GLsizei a_range
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLuint      begin;
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glGenLists",GL_INVALID_OPERATION);return 0;}
  if(a_range<0)      {ReportError ("glGenLists",GL_INVALID_VALUE);    return 0;}
  begin        = Class.lists;
  Class.lists += a_range;
  return       begin;
}
/***************************************************************************/
void glNewList (
 GLuint a_list
,GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)                     {ReportError ("glNewList",GL_INVALID_OPERATION);return;}
  if( (a_mode!=GL_COMPILE)     &&
      (a_mode!=GL_COMPILE_AND_EXECUTE) ) {ReportError("glNewList",GL_INVALID_ENUM);return;}
  if(glListGetIdentifier(a_list)!=NULL)  {ReportError("glNewList",GL_INVALID_VALUE);return;}
  Class.list = glListCreate (a_list);
}
/***************************************************************************/
void glEndList (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glEndList",GL_INVALID_OPERATION);return;}
  Class.list = NULL;
}
/***************************************************************************/
void glCallList (
 GLuint a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  glList                olist,list;
/*.........................................................................*/
  list                  = glListGetIdentifier(a_list);
  if(list==NULL)        return;
  olist                 = Class.list;
  Class.list            = NULL; /*Ensure that Class.list is NULL.*/
  glListExecute         (list);
  Class.list            = olist;
}
/***************************************************************************/
void glDeleteLists (
 GLuint a_list
,GLsizei a_range
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLsizei count;
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glDeleteLists",GL_INVALID_OPERATION);return;}
  for(count=0;count<a_range;count++)
    {
      glListDelete (glListGetIdentifier(a_list+count));
    }
}
/***************************************************************************/
void glBitmap (
 GLsizei a_width
,GLsizei a_height
,GLfloat a_xorig
,GLfloat a_yorig
,GLfloat a_xmove
,GLfloat a_ymove
,const GLubyte* a_bitmap
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double x,y,z;
/*.........................................................................*/
  if(Class.list!=NULL)
    {
#ifdef DEBUG
      printf             ("debug : glBitmap : store in list.\n");
#endif
      glListAddOperation (Class.list,glOperation_glBitmap,(glListOperationFunction)glBitmap,
                          a_width,a_height,a_xorig,a_yorig,a_xmove,a_ymove,a_bitmap);
      return;
    }
  if(Class.begin==1)             {ReportError ("glBitmap",GL_INVALID_OPERATION);return;}
  if(Class.drawBufferProc==NULL) return;
  if(a_bitmap==NULL)             return;
  CheckGlobal           ();
  OMatrixMultiplyVector (&Class.global,
			 Class.rasterPosX,Class.rasterPosY,Class.rasterPosZ,
			 &x,&y,&z);
  Class.drawBufferProc  (x,y,a_width,a_height,a_xorig,a_yorig,(unsigned char*)a_bitmap,&(Class.atb));
  a_xmove = 0.;
  a_ymove = 0.;
}
/***************************************************************************/
void glReadPixels (
 GLint   a_x
,GLint   a_y
,GLsizei a_width
,GLsizei a_height
,GLenum  a_format
,GLenum  a_type
,GLvoid* a_pixels
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1)   {ReportError ("glReadPixels",GL_INVALID_OPERATION);return;}
  if(a_width<0)        {ReportError ("glReadPixels",GL_INVALID_VALUE);    return;}
  if(a_height<0)       {ReportError ("glReadPixels",GL_INVALID_VALUE);    return;}
  if(a_pixels==NULL)   {ReportError ("glReadPixels",GL_INVALID_VALUE);    return;}
  if(a_format!=GL_RGB) {ReportError ("glReadPixels",GL_INVALID_ENUM);     return;}
  if(a_type!=GL_FLOAT) {ReportError ("glReadPixels",GL_INVALID_ENUM);     return;}
  if(Class.depthTest==GL_TRUE)
    {
      if(Class.pixelRgbProc!=NULL)
	{
	  Ulong* items;
	  Ulong* itema;
	  itema  = items = glDriverZ_ReadPixels (a_x,a_y,a_width,a_height);
	  if(items!=NULL)
	    {
	      int    row,col;
	      for(row=0;row<a_height;row++)
		{
		  for(col=0;col<a_width;col++)
		    {
		      double red,green,blue;
		      float* pixels;
		      pixels  = ((float*)a_pixels) + 3 * ((a_height - 1 - row) * a_width + col);
		      Class.pixelRgbProc(*itema,&red,&green,&blue);
		      *pixels = (float)  red;pixels++;
		      *pixels = (float)green;pixels++;
		      *pixels = (float) blue;pixels++;
		      itema++;
		    }
		}
	    }
	  CMemoryFreeBlock (items);
	}
    }
  else
    {
      ReportError ("glReadPixels",GL_NOT_IMPLEMENTED);
    }
}
/***************************************************************************/
void glRectd (
 GLdouble   a_x1
,GLdouble   a_y1
,GLdouble   a_x2
,GLdouble   a_y2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glBegin(GL_LINE_STRIP);
  glVertex2d(a_x1,a_y1);
  glVertex2d(a_x2,a_y1);
  glVertex2d(a_x2,a_y2);
  glVertex2d(a_x1,a_y2);
  glVertex2d(a_x1,a_y1);
  glEnd();
}
/***************************************************************************/
void glRecti (
 GLint   a_x1
,GLint   a_y1
,GLint   a_x2
,GLint   a_y2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glRectd ((GLdouble)a_x1,(GLdouble)a_y1,(GLdouble)a_x2,(GLdouble)a_y2);
}
/***************************************************************************/
void glRects (
 GLshort   a_x1
,GLshort   a_y1
,GLshort   a_x2
,GLshort   a_y2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glRectd ((GLdouble)a_x1,(GLdouble)a_y1,(GLdouble)a_x2,(GLdouble)a_y2);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void glClearIndex (
 GLfloat a_index
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ReportError ("glClearIndex",GL_NOT_IMPLEMENTED);
  a_index = 0.;
}
/***************************************************************************/
void glIndexf (
 GLfloat a_index
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ReportError ("glIndexf",GL_NOT_IMPLEMENTED);
  a_index = 0.;
}
/***************************************************************************/
void glIndexi (
 GLint a_index
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glIndexf ((GLfloat)a_index);
}
/***************************************************************************/
void glCullFace    (
 GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glCullFace",GL_INVALID_OPERATION);return;}
  ReportError ("glCullFace",GL_NOT_IMPLEMENTED);
  a_mode = GL_BACK;
}
/***************************************************************************/
void glNormal3d (
 GLdouble a_nx
,GLdouble a_ny
,GLdouble a_nz
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static int dump = 0;
/*.........................................................................*/
  /*
  if(Class.normalize==GL_TRUE)
    {
        Normalize vector.
    }
    */
  if(dump==0)
    {
      ReportError ("glNormal3d",GL_NOT_IMPLEMENTED);
      dump = 1;
    }
  a_nx = 0.;
  a_ny = 0.;
  a_nz = 0.;
}
/***************************************************************************/
void glNormal3f (
 GLfloat a_nx
,GLfloat a_ny
,GLfloat a_nz
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  glNormal3d ((GLdouble)a_nx,(GLdouble)a_ny,(GLdouble)a_nz);
}
/***************************************************************************/
void glEdgeFlag (
 GLboolean a_flag
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ReportError ("glEdgeFlag",GL_NOT_IMPLEMENTED);
  a_flag = GL_FALSE;
}
/***************************************************************************/
void glDepthFunc (
 GLenum a_func
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glDepthFunc",GL_INVALID_OPERATION);return;}
  ReportError ("glDepthFunc",GL_NOT_IMPLEMENTED);
  a_func = 0;
}
/***************************************************************************/
void glDrawBuffer (
 GLenum a_mode
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glDrawBuffer",GL_INVALID_OPERATION);return;}
  ReportError ("glDrawBuffer",GL_NOT_IMPLEMENTED);
  a_mode = 0;
}
/***************************************************************************/
void glLightfv (
 GLenum a_light
,GLenum a_pname
,const GLfloat* a_params
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glLightfv",GL_INVALID_OPERATION);return;}
  ReportError ("glLightfv",GL_NOT_IMPLEMENTED);
  a_light  = 0;
  a_pname  = 0;
  a_params = NULL;
}
/***************************************************************************/
void glMaterialfv (
 GLenum a_face
,GLenum a_pname
,const GLfloat* a_params
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glMaterialfv",GL_INVALID_OPERATION);return;}
  ReportError ("glMaterialfv",GL_NOT_IMPLEMENTED);
  a_face     = 0;
  a_pname    = 0;
  a_params   = NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void glRasterPos3d (
 GLdouble a_x
,GLdouble a_y
,GLdouble a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.rasterPosX = a_x;
  Class.rasterPosY = a_y;
  Class.rasterPosZ = a_z;
}
/***************************************************************************/
void glPixelStorei (
 GLenum a_pname
,GLint  a_param
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.begin==1) {ReportError ("glPixelStorei",GL_INVALID_OPERATION);return;}
  a_pname = 0;
  a_param = 0;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void CheckGlobal (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrixRecord       ndc;
  double              width,height;
/*.........................................................................*/
  if(Class.computeGlobal==0) return;
  /* view - >ndc */
  width               = Class.viewportWidth/2.;
  height              = Class.viewportHeight/2.;
  OMatrixInitialize   (&ndc,OMatrixScaleTranslation,width,-height,1.,width,height,0.);
  OMatrixMultiplyTripletAndAssign (&Class.global,&ndc,&(Class.projMatrix),&(Class.viewMatrix));
  Class.computeGlobal = 0;
}
/***************************************************************************/
static void InitializeFrame (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.initFrame==1)  return;

  if((Class.renderMode==GL_RENDER) && (Class.setViewportProc!=NULL) ) 
    Class.setViewportProc (Class.viewportX,Class.viewportY,Class.viewportWidth,Class.viewportHeight);

  Class.initFrame    = 1;
}
/***************************************************************************/
static void SetProjection (
 GProjectionType a_projection
,double a_near
,double a_far
,double a_umin 
,double a_umax 
,double a_vmin 
,double a_vmax 
)
/***************************************************************************/
/* 
   Set OpenGL vrp,look,up,dop,cop,front,back.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GPointRec     vrp,up,look;
  GPointRec     cop,dop;
  OMatrixRecord view;
/*.........................................................................*/
  if (a_projection == GProjectionParallel)
    {
      vrp.x  =  0.;
      vrp.y  =  0.;
      vrp.z  = -1.;
    }
  else if (a_projection == GProjectionPerspective)
    {
      vrp.x  = 0.;
      vrp.y  = 0.;
      vrp.z  = a_near==0. ? -1. : - a_near;  
    }
  look.x =  0.;
  look.y =  0.;
  look.z = -2.;
  up.x   =  0.;
  up.y   =  1.;
  up.z   =  0.;
  dop.x  =  0.;
  dop.y  =  0.;
  dop.z  = -1.;
  cop.x  =  0.;
  cop.y  =  0.;
  cop.z  =  0.;

 {OMatrixRecord canonic,projection;
  double        zproj;
  SetMatrixViewing (a_projection,
		    &vrp,&up,&cop,&dop,
		    a_near,a_far,a_umin,a_umax,a_vmin,a_vmax,
                    &canonic,&projection,&zproj,&Class.zmin);

       if(Class.renderMode==GL_RENDER)
    {
      if (a_projection == GProjectionParallel)
	{
	  OMatrixMultiplyAndAssign    (&view,&projection,&canonic);
	}
      else if (a_projection == GProjectionPerspective)
	{ 
	  CHECK_DIV                       (zproj,"SetProjection")
	  OMatrixInitialize               (&(Class.tempMatrix),OMatrixScale,1./fabs(zproj),1./fabs(zproj),1./fabs(zproj));
	  OMatrixMultiplyTripletAndAssign (&view,&(Class.tempMatrix),&projection,&canonic);
	}
    }
  else if(Class.renderMode==GL_SELECT)
    {
      OMatrixCopy (&view,&canonic);
    }}

/*Canonical view volume :view volume bring in [-1.,1.][-1.,1.][0.,-1.]   for parallel    projection.*/
/*Canonical view volume :view volume bring in [-1.,1.][-1.,1.][zmin,-1.] for perspective projection.*/

  MultiplyMatrix (&view);

  Class.initProj = 1;
}
/***************************************************************************/
static void SetMatrixViewing (
 GProjectionType a_proj_type 
,GPoint a_vrp 
,GPoint a_up 
,GPoint a_cop 
,GPoint a_dop 
,double a_near
,double a_far
,double a_umin 
,double a_umax 
,double a_vmin 
,double a_vmax 
,OMatrix a_canonic 
,OMatrix a_projection 
,double* a_zproj 
,double* a_zmin 
)
/***************************************************************************/
/* 
  a_vrp,a_look,a_up,a_cop,a_dop              WC = World Coordinate           
  a_umin,a_umax,a_vmin,a_vmax,a_near,a_far   VRC=Viewing Reference Coordinate
  Van dam second edition 1990 p 261 
  Prespective: for OpenGL, FRONT van dam plane is contain the COP (eye) then
 zmin must be found null.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GPointRec    VRP,VPN,VUP,N,U,V;
  GPointRec    CW,PRP;/* in VRC */
  OMatrix      Shear = NULL;
  OMatrix      Trans,Pinv; /*vrc->wc:wc->vrc*/
  GPointRec    DOP,VRP1;
  double       shx,shy,udelta,vdelta,pdelta,FRONT,BACK,zproj,zmin;
/*.........................................................................*/
/* U,V,N  Pvrc->wc Pinv*/
  VRP              = *a_vrp;
  VPN.x            = -a_dop->x;
  VPN.y            = -a_dop->y;
  VPN.z            = -a_dop->z;
  VUP              = *a_up;
  N                = VPN;
  NormalizeVector  (&N);
  GetCrossProduct  (&U,&VUP,&N);
  NormalizeVector  (&U);
  GetCrossProduct  (&V,&N,&U);

  Pinv             = OMatrixCreate(OMatrixFollowing,
  U.x,U.y,U.z,0.,
  V.x,V.y,V.z,0.,
  N.x,N.y,N.z,0.,
   0.,0.,0.,1.);

  CW.x               = (a_umax+a_umin)/2.;
  CW.y               = (a_vmax+a_vmin)/2.;
  CW.z               = 0.;

  PRP.x              = a_cop->x-VRP.x;
  PRP.y              = a_cop->y-VRP.y;
  PRP.z              = a_cop->z-VRP.z;
  MultiplyPoint      (&PRP,Pinv); /* PRP now in VRC */
  
  Trans              = OMatrixCreate (OMatrixTranslation,-VRP.x,-VRP.y,-VRP.z);

/*OpenGL near  = distance (cop=eye,vrp=window)    */
/*OpenGL far   = distance (cop=eye,back plane)    */
/*Vandam FRONT = distance (cop=eye,vrp=window)    */
/*OpenGL BACK  = distance (vrp=window,back plane) */
  FRONT              =    a_near;
  BACK               = - (a_far - a_near);

  udelta             = a_umax-a_umin;
  CHECK_DIV          (udelta,"SetMatrixViewing")
  vdelta             = a_vmax-a_vmin;
  CHECK_DIV          (vdelta,"SetMatrixViewing")

  if (a_proj_type == GProjectionParallel)
    {
      OMatrix            Spar,Tpar;
/* canonical view volume :view volume bring in [-1.,1.][-1.,1.][0.,-1.] */
      DOP                = *a_dop;
      MultiplyPoint      (&DOP,Pinv); /* DOp now in VRC */
      shx                = - DOP.x/DOP.z;
      shy                = - DOP.y/DOP.z;
      Shear              = OMatrixCreate(OMatrixShear,shx,shy);

      Tpar                = OMatrixCreate(OMatrixTranslation,-CW.x,-CW.y,-FRONT);
      pdelta              = FRONT - BACK;
      CHECK_DIV           (pdelta,"SetMatrixViewing")
      Spar                = OMatrixCreate(OMatrixScale,2./udelta,2./vdelta,1./pdelta);
                  
      OMatrixMultiplyTripletAndAssign (&(Class.tempMatrix),Shear,Pinv,Trans);
      OMatrixMultiplyTripletAndAssign (a_canonic,Spar,Tpar,&(Class.tempMatrix));

/* projection */
      OMatrixInitialize   (a_projection,OMatrixIdentity);
      if(a_zproj!=NULL)   *a_zproj = 0.;
      if(a_zmin!=NULL)    *a_zmin  = 0.;
      OMatrixDelete       (Tpar);
      OMatrixDelete       (Spar);
    }
  else if (a_proj_type == GProjectionPerspective)
    {
      OMatrix             Tprp,Sper;
/* canonical view volume :view volume bring in [-1.,1.][-1.,1.][zmin,-1.]. zmin is in [0.,-1.[. */
      Tprp                = OMatrixCreate(OMatrixTranslation,-PRP.x,-PRP.y,-PRP.z);

      DOP.x               = CW.x-PRP.x;
      DOP.y               = CW.y-PRP.y;
      DOP.z               = CW.z-PRP.z;
      shx                 = - DOP.x/DOP.z;
      shy                 = - DOP.y/DOP.z;
      Shear               = OMatrixCreate(OMatrixShear,shx,shy);

      OMatrixMultiplyAndAssign (&(Class.tempMatrix),Shear,Tprp);
      VRP1.x                   = 0.;
      VRP1.y                   = 0.;
      VRP1.z                   = 0.;
      MultiplyPoint            (&VRP1,&(Class.tempMatrix));

      zproj               = VRP1.z+BACK;
      CHECK_DIV           (zproj,"SetMatrixViewing")
      zproj               = -VRP1.z/zproj;
/*In Van dam the back or far plane is at z = -1.*/
      Sper                = OMatrixCreate(OMatrixScale,-2.*zproj/udelta,-2.*zproj/vdelta,-1./(VRP1.z+BACK));

      OMatrixMultiplyTripletAndAssign (&(Class.tempMatrix),Tprp,Pinv,Trans);
      OMatrixMultiplyTripletAndAssign (a_canonic,Sper,Shear,&(Class.tempMatrix));

/* projection p 255.*/
      OMatrixInitialize   (&(Class.tempMatrix),OMatrixIdentity);
      CHECK_DIV           (zproj,"SetMatrixViewing")
      OMatrixSetElement32 (&(Class.tempMatrix),1/zproj);
      OMatrixSetElement33 (&(Class.tempMatrix),0.);
      OMatrixSetElement23 (&(Class.tempMatrix),zproj>0. ? -1.:1.);

      OMatrixCopy         (a_projection,&(Class.tempMatrix));

      zmin                = VRP1.z+BACK;
      CHECK_DIV           (zmin,"SetMatrixViewing")
      zmin                = -(VRP1.z+FRONT)/zmin;
      if(a_zproj!=NULL)   *a_zproj = zproj   /*Window or view plane.*/;
      if(a_zmin!=NULL)    *a_zmin  = zmin;   /*Front or near plane. We must have zmin >= zproj.*/

/*    printf              ("debug:%g %g zmin %g zproj %g\n",FRONT,BACK,zmin,zproj);*/

      OMatrixDelete       (Tprp);
      OMatrixDelete       (Sper);
    }

  OMatrixDelete(Shear);
  OMatrixDelete(Trans);
  OMatrixDelete(Pinv);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static Ulong ConvertRgbToPixel (
 double a_red
,double a_green
,double a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Ulong  pixel = 1;
/*.........................................................................*/
  a_red   = 0.;
  a_green = 0.;
  a_blue  = 0.;
  return  pixel;
}
/***************************************************************************/
static void MultiplyMatrix (
 OMatrix a_matrix
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrixRecord matrix;
/*.........................................................................*/
  if(a_matrix==NULL) return;

       if(Class.matrixMode==GL_PROJECTION)
    {
      OMatrixCopy               (&matrix,&(Class.projMatrix));
      OMatrixMultiplyAndAssign  (&(Class.projMatrix),&matrix,a_matrix);
    }
  else if(Class.matrixMode==GL_MODELVIEW)
    {
      OMatrixCopy               (&matrix,&(Class.viewMatrix));
      OMatrixMultiplyAndAssign  (&(Class.viewMatrix),&matrix,a_matrix);
    }
  Class.computeGlobal = 1;
}
/***************************************************************************/
static void ReportError (
 char* a_proc
,int a_error
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
       if(a_error==GL_INVALID_ENUM)      CWarnF("GL:gl:%s: invalid enum.\n",a_proc);
  else if(a_error==GL_INVALID_OPERATION) CWarnF("GL:gl:%s: invalid operation.\n",a_proc);
  else if(a_error==GL_INVALID_VALUE)     CWarnF("GL:gl:%s: invalid value.\n",a_proc);
  else if(a_error==GL_STACK_OVERFLOW)    CWarnF("GL:gl:%s: stack overflow.\n",a_proc);
  else if(a_error==GL_NOT_IMPLEMENTED)   CWarnF("GL:gl:%s: not implemented.\n",a_proc);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static double GetNorme (
 GPoint This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return sqrt(This->x * This->x + This->y * This->y + This->z * This->z);
}
/***************************************************************************/
static double NormalizeVector (
 GPoint This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double length;
/*.........................................................................*/
  if(This==NULL) return 0.;
  length         = GetNorme (This);
  if(length==0.) {ReportError ("NormalizeVector",GL_INVALID_VALUE);length=1.;}
  This->x /= length;
  This->y /= length;
  This->z /= length;
  return      length;
}
/***************************************************************************/
static void GetCrossProduct (
 GPoint This 
,GPoint a_v 
,GPoint a_w 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_v==NULL) return;
  if(a_w==NULL) return;
  This->x = a_v->y * a_w->z - a_v->z * a_w->y;
  This->y = a_v->z * a_w->x - a_v->x * a_w->z;
  This->z = a_v->x * a_w->y - a_v->y * a_w->x;
}
/***************************************************************************/
static void MultiplyPoint (
 GPoint  a_point
,OMatrix a_matrix
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double      x,y,z;
/*.........................................................................*/
  OMatrixMultiplyVector  (a_matrix,a_point->x,a_point->y,a_point->z,&x,&y,&z);
  a_point->x  = x;
  a_point->y  = y;
  a_point->z  = z;
}
/***************************************************************************/
/***************************************************************************/
/************ Selection ****************************************************/
/***************************************************************************/
/***************************************************************************/
static int ClipPolyline (
 int a_mode
,int a_pointn
,double* a_xs
,double* a_ys
,double* a_zs
,OMatrix a_matrix 
,GPoint a_point
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  GPointRec     begin,end;
/*.........................................................................*/
       if (a_pointn==1)
    {
      begin.x       = a_xs[0];
      begin.y       = a_ys[0];
      begin.z       = a_zs[0];
      if(a_point!=NULL)   *a_point = begin;
      MultiplyPoint (&begin,a_matrix);
      if(Class.clipPoint(&begin,0,Class.zmin)==1) return 1;
    }
  else if(a_mode==GL_POINTS)
    { 
      for(count=0;count<a_pointn;count++)
	{
	  begin.x       = a_xs[count];
	  begin.y       = a_ys[count];
	  begin.z       = a_zs[count];
	  if(a_point!=NULL)   *a_point      = begin;
	  MultiplyPoint (&begin,a_matrix);
	  if(Class.clipPoint(&begin,0,Class.zmin)==1) return 1;
	}
    }
  else if(a_mode==GL_LINES)
    { int  segment_number;
      segment_number = a_pointn/2;
      for(count=0;count<segment_number;count++)
	{
	  begin.x       = a_xs[2*count];
	  begin.y       = a_ys[2*count];
	  begin.z       = a_zs[2*count];
	  end.x         = a_xs[2*count+1];
	  end.y         = a_ys[2*count+1];
	  end.z         = a_zs[2*count+1];
	  MultiplyPoint (&begin,a_matrix);
	  MultiplyPoint (&end,a_matrix);
	  if(Class.clipLine(&begin,&end,0,Class.zmin,0)==1) 
	    {  
               OMatrixInvertAndAssign (&(Class.tempMatrix),a_matrix);
	       MultiplyPoint    (&begin,&(Class.tempMatrix));
	       if(a_point!=NULL)      *a_point         = begin;
	       return           1;
	     }
	}
    }
  else
    { 
      for(count=0;count<a_pointn-1;count++)
	{
	  begin.x       = a_xs[count];
	  begin.y       = a_ys[count];
	  begin.z       = a_zs[count];
	  end.x         = a_xs[count+1];
	  end.y         = a_ys[count+1];
	  end.z         = a_zs[count+1];
	  MultiplyPoint (&begin,a_matrix);
	  MultiplyPoint (&end,a_matrix);
	  if(Class.clipLine(&begin,&end,0,Class.zmin,0)==1) 
	    {  
	       OMatrixInvertAndAssign (&(Class.tempMatrix),a_matrix);
	       MultiplyPoint  (&begin,&(Class.tempMatrix));
	       if(a_point!=NULL)    *a_point       = begin;
	       return         1;
	     }
	}
    }
  return 0;
}
/***************************************************************************/
static int ClipPointParallel  (
 GPoint This 
,int    a_zflag 
,double a_zmin 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_zmin = 0.; /*avoid warning*/
  return ( (GetOutParallel  (This,a_zflag) & FILTER__NOZ) !=0  ? 0 : 1);
}
/***************************************************************************/
static int ClipLineParallel  (
 GPoint This 
,GPoint a_end 
,int a_zflag 
,double a_zmin
,int* a_toggle
)
/***************************************************************************/
/* 
   This  in, end  in: no toggle.     (toggle means This contains end and end contains This).
   This out, end out: no toggle.
   This out, end  in: no toggle, This = clipping point.
   This  in, end out: toggled This = clipping point.
    at out 
     This contain clipping point.
   toggle = 1 -> (begin,end) has been toggled.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned long outbegin,outend,out;
  int       reject,done;
  int       accept;
  GPointRec point;
  double    t;
/*.........................................................................*/
  if(a_toggle!=NULL) *a_toggle = 0;
  a_zmin = 0.; /*avoid warning*/
  reject = 0;
  accept = 0;
  done   = 0;
  do
    {
      outbegin = GetOutParallel  (This ,a_zflag);
      outend   = GetOutParallel  (a_end,a_zflag);
      reject   = ( (outbegin & outend  & FILTER__NOZ) !=0  ? 1 : 0);
      if(reject==1)
        {done = 1;}
      else
        {
          accept = ( (outbegin!=0L)||(outend  !=0L)  ? 0 : 1) ;
          if(accept==1)
            {done = 1;}
          else
            {
              if(outbegin==0L) /*This inside. toggle This and end.*/
		{
		  out      = outend;
		  point    = *a_end;
		  outend   = outbegin;
		  *a_end   = *This;
		  outbegin = out;
		  *This    = point;
		  if(a_toggle!=NULL) *a_toggle = 1;
		}
              if((outbegin & (1<<0))!=0)
                {
/* y = 1. */
                  t = a_end->y-This->y;
                  CHECK_DIV    (t,"ClipLineParallel")
                  t = (1.- This->y)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y  = 1.;
                  This->z += (a_end->z - This->z) * t;
                }
              else if((outbegin & (1<<1))!=0)
                {
/* y = -1. */
                  t = a_end->y-This->y;
                  CHECK_DIV    (t,"ClipLineParallel")
                  t = (-1.- This->y)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y  = -1.;
                  This->z += (a_end->z - This->z) * t;
                }
              else if((outbegin & (1<<2))!=0)
                {
/* x = 1. */
                  t = a_end->x-This->x;
                  CHECK_DIV    (t,"ClipLineParallel")
                  t = (1.- This->x)/t;
                  This->x  = 1.;
                  This->y += (a_end->y - This->y) * t;
                  This->z += (a_end->z - This->z) * t;
                }
              else if((outbegin & (1<<3))!=0)
                {
/* x = -1. */
                  t = a_end->x-This->x;
                  CHECK_DIV    (t,"ClipLineParallel")
                  t = (-1.- This->x)/t;
                  This->x  = -1.;
                  This->y += (a_end->y - This->y) * t;
                  This->z += (a_end->z - This->z) * t;
                }
/* z = -1. */
/* z =  0. */
/* suppress z clipping */
/*
              else if((a_zflag==1)&&((outbegin & (1<<4))!=0))
                {
                  t = a_end->z-This->z;
                  CHECK_DIV    (t,"ClipLineParallel")
                  t = (-1.- This->z)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y += (a_end->y - This->y) * t;
                  This->z  = -1.;
                }
              else if((a_zflag==1)&&((outbegin & (1<<5))!=0) )
                {
                  t = a_end->z-This->z;
                  CHECK_DIV    (t,"ClipLineParallel")
                  t = (- This->z)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y += (a_end->y - This->y) * t;
                  This->z  = 0.;
                }
*/
            }
        }
    } while (done==0);
  return accept;
}
/***************************************************************************/
static unsigned long GetOutParallel  (
 GPoint This 
,int a_zflag 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned long out; 
/*.........................................................................*/
  out = 0L;
  if(This->y> 1.) out |= (1<<0);
  if(This->y<-1.) out |= (1<<1);
  if(This->x> 1.) out |= (1<<2);
  if(This->x<-1.) out |= (1<<3);
  if(a_zflag==0) return out;
  if(This->z<-1.) out |= (1<<4);
  if(This->z> 0.) out |= (1<<5);
  return out;
}
/***************************************************************************/
static int ClipPointPerspective (
 GPoint This 
,int a_zflag 
,double a_zmin 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return ( (GetOutPrspective  (This,a_zflag,a_zmin) & FILTER__NOZ) !=0 ? 0 : 1);
}
/***************************************************************************/
static int ClipLinePerspective  (
 GPoint This 
,GPoint a_end 
,int a_zflag 
,double a_zmin 
,int* a_toggle
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned long     outbegin,outend,out;
  int       reject,done;
  int       accept;
  GPointRec point;
  double    t;
/*.........................................................................*/
  if(a_toggle!=NULL) *a_toggle = 0;
  reject = 0;
  accept = 0;
  done   = 0;
  do
    {
      outbegin = GetOutPrspective  (This,a_zflag,a_zmin);
      outend   = GetOutPrspective  (a_end  ,a_zflag,a_zmin);
      reject   = ( (outbegin & outend  & FILTER__NOZ) !=0  ? 1 : 0);
      if(reject==1)
        {done = 1;}
      else
        {
          accept = ( (outbegin!=0)||(outend  !=0)  ? 0 : 1) ;
          if(accept==1)
            {done = 1;}
          else
            {
              if(outbegin==0L) 
		{
		  out      = outend;
		  point    = *a_end;
		  outend   = outbegin;
		  *a_end   = *This;
		  outbegin = out;
		  *This = point;
		  if(a_toggle!=NULL) *a_toggle = 1;
		}
              if((outbegin & (1<<0))!=0)
                {
/* y = -z */
                  t = (a_end->y-This->y)+(a_end->z-This->z);
                  CHECK_DIV    (t,"ClipLinePerspective")
                  t = (-This->z - This->y)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y += (a_end->y - This->y) * t;
                  This->z  = -This->y;
                }
              else if((outbegin & (1<<1))!=0)
                {
/* y = z */
                  t = (a_end->y-This->y)-(a_end->z-This->z);
                  CHECK_DIV    (t,"ClipLinePerspective")
                  t = (This->z - This->y)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y += (a_end->y - This->y) * t;
                  This->z  = This->y;
                }
              else if((outbegin & (1<<2))!=0)
                {
/* x = -z */
                  t = (a_end->x-This->x)+(a_end->z-This->z);
                  CHECK_DIV    (t,"ClipLinePerspective")
                  t = (-This->z - This->x)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y += (a_end->y - This->y) * t;
                  This->z  = -This->x;
                }
              else if((outbegin & (1<<3))!=0)
                {
/* x = z */
                  t = (a_end->x-This->x)-(a_end->z-This->z);
                  CHECK_DIV    (t,"ClipLinePerspective")
                  t = (This->z - This->x)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y += (a_end->y - This->y) * t;
                  This->z  = This->x;
                }
/* z = -1. */
/* z = a_zmin */
/* suppress z clipping */
/*
              else if((a_zflag==1)&&((outbegin & (1<<4))!=0))
                {
                  t = a_end->z-This->z;
                  CHECK_DIV    (t,"ClipLinePerspective")
                  t = (-1.- This->z)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y += (a_end->y - This->y) * t;
                  This->z  = -1.;
                }
              else if((a_zflag==1)&&((outbegin & (1<<5))!=0))
                {
                  t = a_end->z-This->z;
                  CHECK_DIV    (t,"ClipLinePerspective")
                  t = (a_zmin- This->z)/t;
                  This->x += (a_end->x - This->x) * t;
                  This->y += (a_end->y - This->y) * t;
                  This->z  = a_zmin;
                }
*/
            }
        }
    } while (done==0);
  return accept;
}
/***************************************************************************/
static unsigned long GetOutPrspective  (
 GPoint This 
,int a_zflag 
,double a_zmin 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned long out;
/*.........................................................................*/
  out = 0L;
  if(This->y>-This->z) out |= (1<<0);
  if(This->y< This->z) out |= (1<<1);
  if(This->x>-This->z) out |= (1<<2);
  if(This->x< This->z) out |= (1<<3);
  if(a_zflag==0) return out;
  if(This->z<-1.) out |= (1<<4);
  if(This->z>a_zmin) out |= (1<<5);
  return out;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void MultiplyPoints (
 OMatrix a_matrix 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
   double* xs;
   double* ys;
   double* zs;
/*.........................................................................*/
  xs = Class.pointx;
  ys = Class.pointy;
  zs = Class.pointz;
  for(count=0;count<Class.pointi;count++,xs++,ys++,zs++)
    {
      double       x,y,z;
      OMatrixMultiplyVector (a_matrix,*xs,*ys,*zs,&x,&y,&z);
      *xs          = x;
      *ys          = y;
      *zs          = z;
    }
}
/***************************************************************************/
static void ClearPoints (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock            (Class.pointx);
  CMemoryFreeBlock            (Class.pointy);
  CMemoryFreeBlock            (Class.pointz);
  Class.pointx        = NULL;
  Class.pointy        = NULL;
  Class.pointz        = NULL;
  Class.pointi        = 0;
  Class.pointn        = 0;
}
