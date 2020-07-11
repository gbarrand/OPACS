/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
static char what[] = "@(#)Go v3";

/*#undef HAS_GL*/
/*
#define DEBUG
*/

#ifdef DEBUG
#include <time.h>
#endif

#include <stdarg.h>
#include <string.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#ifdef HAS_GL
#include <GL/gl.h>
#include <GL/glu.h>
#endif /*HAS_GL*/

#include <CPrinter.h>
#include <CMemory.h>
#include <CString.h>
#include <CList.h>
#include <CVector.h>
#include <CMath.h>
#include <CError.h>
#include <CPS.h>
#include <OColor.h>

#include <GoF77.h>
#include <ONode.h>

#include <OCameraP.h>

typedef unsigned long Ulong;
typedef unsigned char Uchar;

#ifdef __cplusplus
extern "C"{
#endif
static void      InitializeClass       ();
static void      FreeRecord            (OCamera);
static int       GetIdentifier         (OCamera,char*);
static ONode*    Traverse              (OCamera,int,int,int,int,int);
static ONode     GetObjectPicked       (ONode*,int);
static void      DrawPrimitive         (OCamera,ONode,OPrimitive,int,int,int,OContext);
static void      SetLineStyle          (OLineStyle);
static void      StartBlink            ();
static void      StopBlink             ();
static void      DoBlink               (void*,void*);
static void      GetWorldCoordinates   (OCamera,ONode,double*,double*,double*);
static int       GetExtension          (ONode*,double*,double*,double*,double*,double*,double*);
static OMatrix   GetSurfaceMatrix      ();
static OMatrix   GetHistogramMatrix    ();
static void      ViewNode              (OCamera,ONode);
static void      ViewNodes             (OCamera,ONode*);
static void      UnviewNode            (OCamera,ONode);
static char*     GetColorName          (OCamera,int);
static int       AllocatePoints        (int);
static void      DrawMarker            (OMarkerStyle,int);

static int       GetRGB                (int,int,double*,double*,double*);
#ifdef HAS_GL
static void      PS_SetColor           ();
static void      PS_DrawPolygon        (int,double*,double*,double*,OAreaStyle);
static void      PS_DrawLines          (int,double*,double*,double*,OLineStyle,int);
static void      PS_DrawSegments       (int,double*,double*,double*,OLineStyle,int);
static void      PS_DrawMarkers        (int,double*,double*,double*,OMarkerStyle,int);
static void      PS_PutMarker          (OMarkerStyle,double);
#endif /*HAS_GL*/
static void      ReportError           (char*,int);
#ifdef __cplusplus
}
#endif

typedef char* OString;
#define NotFound                (-1)
#define DBL_INT                 (int)(float)
#define CHECK_DIV(a_var,a_pro)  {if((a_var)==0.) {ReportError(a_pro,CErrorValue);(a_var)=1.;}}
#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)
#define ONumber(arr)            (sizeof(arr) / sizeof(arr[0]))
#define CameraName(This)        (This->name!=NULL ? This->name:"(nil)")

#define TRAVERSE_DRAW   0
#define TRAVERSE_SELECT 1 
#define TRAVERSE_PS     2

static struct 
{
  int       init;
  OCamera*  extent;
  OCamera   current;
  Ulong     blinkDelay;
  Ulong     blinkTimeOut;
  ONode*    nodes;
  int       number;
  double*   xs;
  double*   ys;
  double*   zs;
  float*    items;
  int       width,height;
} Class = {0,NULL,NULL,1000,0L,NULL,0,NULL,NULL,NULL,NULL,0,0};
/***************************************************************************/
static void InitializeClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==1) return; /* already done */

  ONodeSetBlinkFunctions    (StartBlink,StopBlink);

  Class.init              = 1;
}
/***************************************************************************/
void OCameraClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==0) return;

  ONodeClearClass  ();

  GoF77_ClearClass ();

  CListDestroy     ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent     = NULL;

  CListDelete      ((CList)Class.nodes);
  Class.nodes      = NULL;

  CMemoryFreeBlock (Class.xs);
  CMemoryFreeBlock (Class.ys);
  CMemoryFreeBlock (Class.zs);
  Class.xs         = NULL;
  Class.ys         = NULL;
  Class.zs         = NULL;
  Class.number     = 0;

  Class.init       = 0;

  what[0] = '@'; /*c++ no warning.*/
}
/***************************************************************************/
OCamera* OCameraGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OCamera OCameraGetIdentifierF (
 int   a_number
,char* a_format
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list       args;
  char*         string;
  OCamera       This;
/*.........................................................................*/
  va_start       (args,a_format);
  string         = CStringCreateV(a_number,a_format,args);
  va_end         (args);
  This           = OCameraGetIdentifier (string);
  CStringDelete  (string);
  return         This;
}
/***************************************************************************/
OCamera OCameraGetIdentifier (
 char* a_name  
)
/***************************************************************************/
/*
 Description :
   Returns the address in memory of an OCamera object knowing its name.
  The first OCamera whose name matches the given string is returned.
 Arguments :
   a_name : name of the wanted OCamera.
 Example : 
...
#include <OCamera.h>
...
  {OCamera ocamera;
   ocamera = OCameraGetIdentifier ("Camera");
...
  }
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"OCamera",7)==0) )
    {
      int        status;
      OCamera    identifier;
      identifier = (OCamera)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return (OCamera)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,pos+1);
	}
      else
	{
	  if(OCameraIsValid(identifier)==0)
	    {
	      return NULL;
	    }
	  else
	    {
	      return identifier;
	    }
	}
    }
  else
    {
      return (OCamera)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
    }
}
/***************************************************************************/
static int GetIdentifier (
 OCamera This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OCameraGetName(This),a_name);
}
/***************************************************************************/
OCamera OCameraMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OCamera This;
/*.........................................................................*/
  InitializeClass          ();
  This                     = (OCamera) CMemoryAllocateBlock (sizeof(OCameraRecord));
  if(This==NULL)           return NULL;

  CListAddEntry            ((CList*)&(Class.extent),This);

  This->name               = NULL;
  This->width              = 0;
  This->height             = 0;

  This->ocmap              = NULL;
  This->bw                 = 0;
  This->backgroundColor    = 0;  /*Index in OColormap.*/
  This->edgeColor          = This->backgroundColor;
  This->highlightContext   = CStringDuplicate("color 0 lineWidth 2");

  This->edgesVisible       = 1;

  This->sendMoveProc      = NULL;
  This->sendMoveArg       = NULL;
  This->sendMoveDone      = 0;

  This->sendRefreshProc   = NULL;
  This->sendRefreshArg    = NULL;
  This->sendRefreshDone   = 0;

/*Init 3d part.*/
/*Create root node.*/
  This->toView            = NULL;
  This->referenceCamera       = NULL;

  This->buffering          = OBufferingSimple;

  This->aspectRatio            = 1;
  This->center_x               = 0.;
  This->center_y               = 0.;
  This->center_z               = 0.;
  This->viewVrp_x              = 0.;
  This->viewVrp_y              = 0.;
  This->viewVrp_z              = 1.;
  This->viewUp_x               = 0.;
  This->viewUp_y               = 1.;
  This->viewUp_z               = 0.;
  This->fieldMin               = -1.;
  This->fieldMax               =  1.;
  This->depthTreatment     = ODepthTreatmentNone;

  This->ctmInit                = OMatrixCreate(OMatrixIdentity);
  This->ctm                    = OMatrixCreate(OMatrixIdentity);
  This->global                 = NULL;

  This->projection             = OProjectionParallel;
  This->projectionAngle        = M_PI/2.;
  This->vrp_x                  = This->viewVrp_x;
  This->vrp_y                  = This->viewVrp_y;
  This->vrp_z                  = This->viewVrp_z;
  This->up_x                   = This->viewUp_x;
  This->up_y                   = This->viewUp_y;
  This->up_z                   = This->viewUp_z;

  This->picks                  = NULL;

  This->rootNode               = NULL;

  This->specialProjection.specialProjection           = OSpecialProjectionXYZ;
  This->specialProjection.doSpecialProjectionFunction = NULL;
  This->specialProjection.xcompress                   = 1.;
  This->specialProjection.fisheye_a                   = 0.;
  This->specialProjection.rhoz_phi1                   = 0.;
  This->specialProjection.v_rho_max                   = 194.;
  This->specialProjection.v_z_max                     = 165.25;
  This->specialProjection.v_k                         = 0.;

  This->nextRefreshOnTouched = 0;

/*ctm stack*/
  This->ctmn                   = 0;
  for(count=0;count<MAX__CTM;count++) This->ctms[count] = NULL;

  return                   This;
}
/***************************************************************************/
OCamera OCameraCreate (
 char*  a_name 
,int a_width 
,int a_height 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera This;
/*.........................................................................*/
  if( (a_width<=0)||(a_height<=0)) {ReportError("OCameraMake",CErrorValue);return NULL;}

  This           = OCameraMake();
  if(This==NULL) return NULL;

  This->name     = CStringDuplicate(a_name);
  This->width    = a_width;
  This->height   = a_height;

  return         This;
}
/***************************************************************************/
OCamera OCameraGetCurrent (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.current;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OCameraSetName (
 OCamera This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->name);
  This->name    = CStringDuplicate (a_name);
}
/***************************************************************************/
void OCameraSetRootNode (
 OCamera This 
,ONode a_node
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->rootNode = a_node;
}
/***************************************************************************/
ONode OCameraGetRootNode (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->rootNode;
}
/***************************************************************************/
void OCameraSetCurrent (
 OCamera This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.current = This;
}
/***************************************************************************/
int OCameraIsValid (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
void OCameraDelete (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  CListRemoveEntry  ((CList)Class.extent,This);
  FreeRecord       (This);
}
/***************************************************************************/
static void FreeRecord (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)        return;
  CListDelete           ((CList)This->toView);
  This->toView          = NULL;
  CListDelete           ((CList)This->referenceCamera);
  This->referenceCamera = NULL;
  for(count=0;count<MAX__CTM;count++) 
    {
      OMatrixDelete     (This->ctms[count]);
      This->ctms[count] = NULL;
    }
  OMatrixDelete (This->ctmInit);
  OMatrixDelete (This->ctm);
  OMatrixDelete (This->global);
  CStringDelete (This->name);
  CStringDelete (This->highlightContext);
  This->highlightContext = NULL;
  This->name    = NULL;
  This->ctm     = NULL;
  This->global  = NULL;
  This->ctmInit = NULL;
  CListDestroy   ((CList)This->picks,(CListVisitEntryFunction)OPickDelete);
  This->picks   = NULL;
  CMemoryFreeBlock (This);
  /*Reset global list*/
  CMemoryFreeBlock (Class.xs);
  CMemoryFreeBlock (Class.ys);
  CMemoryFreeBlock (Class.zs);
  Class.xs         = NULL;
  Class.ys         = NULL;
  Class.zs         = NULL;
  Class.number     = 0;
}
/***************************************************************************/
void OCameraSetAspectRatio (
 OCamera This 
,int a_aspect 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_aspect==This->aspectRatio) return;
  This->aspectRatio  = a_aspect;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
int OCameraGetAspectRatio (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return This->aspectRatio;
}
/***************************************************************************/
void OCameraSetBlackAndWhite (
 OCamera This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_value==This->bw) return;
  This->bw           = a_value;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
int OCameraGetBlackAndWhite (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->bw;
}
/***************************************************************************/
void OCameraSetBackgroundColorIndex (
 OCamera This 
,int a_index
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_index==This->backgroundColor) return;
  This->backgroundColor = a_index;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
int OCameraGetBackgroundColorIndex (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->backgroundColor;
}
/***************************************************************************/
void OCameraSetEdgesVisible (
 OCamera This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)        return;
  if(a_value==This->edgesVisible) return;
  This->edgesVisible   = a_value;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
int OCameraAreEdgesVisible (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return         This->edgesVisible;
}
/***************************************************************************/
void OCameraSetEdgeColorIndex (
 OCamera This 
,int a_index
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)        return;
  if(a_index==This->edgeColor) return;
  This->edgeColor  = a_index;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraSetHighlightContext (
 OCamera This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->highlightContext)==1) return;
  CStringDelete             (This->highlightContext);
  This->highlightContext    = CStringDuplicate(a_string);
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
char* OCameraGetHighlightContext (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->highlightContext;
}
/***************************************************************************/
int OCameraGetEdgeColorIndex (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->edgeColor;
}
/***************************************************************************/
char* OCameraGetName (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OCameraGetSize (
 OCamera This  
,int* a_width  
,int* a_height 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_width!=NULL)  *a_width  = 0;
  if(a_height!=NULL) *a_height = 0;
  if(This==NULL) return;
  if(a_width!=NULL)  *a_width  = This->width;
  if(a_height!=NULL) *a_height = This->height;
}
/***************************************************************************/
OColormap OCameraGetColormap (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->ocmap;
}
/***************************************************************************/
void OCameraSetColormap (
 OCamera This 
,OColormap a_cmap 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)            return;
  if(a_cmap==NULL)          return;
  if(a_cmap==This->ocmap)   return;
  This->ocmap               = a_cmap;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraChangeSize (
 OCamera This 
,int a_width 
,int a_height 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_width>0)        This->width  = a_width;
  if(a_height>0)       This->height = a_height;
  ONodeRequireViewRefreshment (This);
  OCameraRequireMovement    (This);
}
/***************************************************************************/
static ONode* Traverse (
 OCamera This 
,int a_mode
,int a_pick_x
,int a_pick_y
,int a_pick_width
,int a_pick_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode* objects = NULL;
  int    objecti = 0;
#ifdef DEBUG
  time_t start,at;
#endif
/*.........................................................................*/
  if(a_mode!=TRAVERSE_SELECT) 
    {
      if(OColormapIsValid(This->ocmap)==0) 
	{
          CWarnF  ("Camera %s has no OColormap.\n",CameraName(This));
	  return  NULL;
	}
    }

#ifdef DEBUG
  start  = time(NULL);
  printf ("debug : OCamera : traverse %s.\n",CameraName(This));
#endif

#ifdef HAS_GL

/*Init GL part.*/
 {double         red,green,blue;
  ONode*         items;
  ONode*         itema;
  int            itemn = 0;
  int            BUFSIZE;
  double         left,right,bottom,top,neard = 0.,fard = 0.; /*near,far do not compile on NT !!!!*/
  GLint          viewport[4];
  GLdouble       matrix[16];
  GLuint*        selectBuffer = NULL;

  items = OCameraGetNodes (This);

  if(a_mode==TRAVERSE_SELECT) 
    {
      itemn          = CListGetSize((CList)items);
      BUFSIZE        = (itemn + 1) * (3 + 1); 
      selectBuffer   = (GLuint*)CMemoryAllocateBlock ( BUFSIZE * sizeof(GLuint));
      glSelectBuffer (BUFSIZE,selectBuffer);
      glRenderMode   (GL_SELECT);
      glInitNames    ();
      glPushName     (0);
    }
  else
    {
      glRenderMode   (GL_RENDER);
    }


  OCameraGetViewLimits  (This,&left,&right,&bottom,&top);

  if(This->depthTreatment==ODepthTreatmentZBuffer) glEnable  (GL_DEPTH_TEST);
  else                                             glDisable (GL_DEPTH_TEST);

  glViewport     (0,0,This->width,This->height);

  glGetIntegerv  (GL_VIEWPORT,viewport);
/*aspect         = ((double)viewport[2])/((double)viewport[3]);*/

  switch (This->projection)
    {
      double        size;
      case OProjectionParallel:
        size          = (This->fieldMax - This->fieldMin);
	neard         = - 1000. * size;
	fard          =   1000. * size;
	break;
      case OProjectionPerspective:
	neard         = 2. * tan(This->projectionAngle/2.);
	CHECK_DIV     (neard,"Traverse")
	neard         = (This->fieldMax - This->fieldMin)/neard;
	fard          = 1000. * neard;
	break;
    }

  if(a_mode==TRAVERSE_SELECT) 
    {
      double         xpick,ypick;

      double         ln,rn,bn,tn;

      xpick          = (double)a_pick_x;
      ypick          = (double)(This->height - a_pick_height - a_pick_y);

     {double x,y;
      x      = xpick - ((GLdouble)viewport[0]);
      y      = ypick - ((GLdouble)viewport[1]);

      ln = left   + x                             * (right-left) / ((GLdouble)viewport[2]);
      rn = left   + (x +  ((double)a_pick_width)) * (right-left) / ((GLdouble)viewport[2]);
      bn = bottom + y                             * (top-bottom) / ((GLdouble)viewport[3]);
      tn = bottom + (y + ((double)a_pick_height)) * (top-bottom) / ((GLdouble)viewport[3]);}

      glViewport     ((GLint)xpick,(GLint)ypick,(GLsizei)a_pick_width,(GLsizei)a_pick_height);
      glMatrixMode   (GL_PROJECTION); 
      glLoadIdentity ();
      switch (This->projection)
	{
	case OProjectionParallel:
	  glOrtho        (ln,rn,bn,tn,neard,fard);
	  break;
	case OProjectionPerspective:
	  glFrustum      (ln,rn,bn,tn,neard,fard);
	  break;
	}
    }
  else
    {
      glMatrixMode   (GL_PROJECTION); 
      glLoadIdentity ();
      switch (This->projection)
	{
	case OProjectionParallel:
	  glOrtho   (left,right,bottom,top,neard,fard);
	  break;
	case OProjectionPerspective:
	  glFrustum (left,right,bottom,top,neard,fard);
	  break;
	}
           if(This->bw==1) 
	{ 
	  red = green = blue = 0.;
	}
      else if(This->bw==2) 
	{ 
	  red = green = blue = 1.;
	}
      else
	{
	  OColormapGetIthRGB    (This->ocmap,This->backgroundColor,&red,&green,&blue);
	}
      if(a_mode==TRAVERSE_PS)
	{
	  CPS_PutBackgroundInStream   (red,green,blue,(double)This->width,(double)This->height);
	} 
      else
	{
	  glClearColor (red,green,blue,0.0);
	  if((This->depthTreatment!=ODepthTreatmentNone) || (This->nextRefreshOnTouched==0) )
	    {
	      glClear      (GL_COLOR_BUFFER_BIT);
	    }
	  if(This->depthTreatment==ODepthTreatmentZBuffer) glClear (GL_DEPTH_BUFFER_BIT);
	}
    }

  glMatrixMode   (GL_MODELVIEW);
  glLoadIdentity ();

  switch (This->projection)
    {
    case OProjectionParallel:
      gluLookAt      (This->vrp_x,This->vrp_y,This->vrp_z,
		      This->center_x,This->center_y,This->center_z,
		      This->up_x,This->up_y,This->up_z);  
      break;
    case OProjectionPerspective:
     {double       dopx,dopy,dopz;
      double       eyex,eyey,eyez;

      OCameraGetDOP (This,&(dopx),&(dopy),&(dopz));

      eyex         = This->vrp_x - dopx * neard;
      eyey         = This->vrp_y - dopy * neard;
      eyez         = This->vrp_z - dopz * neard;

      gluLookAt     (eyex,eyey,eyez,                  
		     This->center_x,This->center_y,This->center_z,
		     This->up_x,This->up_y,This->up_z);}
    break;
    }

 {double            views[16];
  double            projs[16];
  double            width,height;
  OMatrix           view,proj,ndc;
  glGetDoublev      (GL_PROJECTION_MATRIX,views);
  glGetDoublev      (GL_MODELVIEW_MATRIX ,projs);
  view              = OMatrixCreate   (OMatrixGL,views);
  proj              = OMatrixCreate   (OMatrixGL,projs);
  width             = This->width/2.;
  height            = This->height/2.;
  ndc               = OMatrixCreate (OMatrixScaleTranslation,width,-height,1.,width,height ,0.);
  OMatrixDelete     (This->global);
  This->global      = OMatrixMultiplyMatrices (3,ndc,proj,view);
  OMatrixDelete     (view);
  OMatrixDelete     (proj);
  OMatrixDelete     (ndc);
  }

  OMatrixConvertToGL_Matrix (This->ctm,matrix);
  glMultMatrixd             (matrix);

  if(items!=NULL)
    {
      int                   highLight = 0;
      int                   back      = 0;
      int                   glname    = 1;
      OContext              context;
      OMatrix               cameraAligned = NULL;
      context               = OContextMake();
      OContextSetFromScript (context,This->highlightContext);
#ifdef DEBUG
      at     = time(NULL);
      printf ("debug : OCamera : traverse %s : start onodes %d.\n",CameraName(This),at-start);
      start  = time(NULL);
#endif
      for(itema=items;*itema!=NULL;itema++)
	{ 
	  int visible = 0;
	  if(ONodeGetVisibility(*itema)==1) 
	    {
	      if(This->depthTreatment==ODepthTreatmentNone)
		{
		 if(This->nextRefreshOnTouched==1) 
		   {
		     if(ONodeGetTouched(*itema)==1)
		       {
			 visible = 1;
		       }
		   }
		 else
		   {
		     visible = 1;
		   }
		}
	      else
		{
		  visible = 1;
		}
	    }
	  ONodeResetTouched(*itema);

	  if(visible==1)
	    {
	      OMatrix   ctm;
	      int       pushed = 0;

	      if(ONodeGetCameraAligned(*itema)==1) 
		{
		  double                      x,y,z;
		  OMatrix                     trans_1,trans,tmp,rot;
		  OMatrixDelete               (cameraAligned);
		  cameraAligned               = NULL;
		  ONodeGetHookPoint           (*itema,&x,&y,&z);
		  trans_1                     = OMatrixCreate(OMatrixTranslation,-x,-y,-z);
		  trans                       = OMatrixCreate(OMatrixTranslation,x,y,z);
		  tmp                         = OMatrixDuplicate(This->ctm);
		  OMatrixRemoveTranslations   (tmp);
		  OMatrixRemoveIsometricScale (tmp);
		  rot                         = OMatrixInvert(tmp);                    
		  OMatrixDelete               (tmp);
		  cameraAligned               = OMatrixMultiplyMatrices (3,trans,rot,trans_1);
		  OMatrixDelete               (trans);
		  OMatrixDelete               (trans_1);
		  OMatrixDelete               (rot);
		  ctm                         = cameraAligned;
                }
	      else
		{
		  ctm = ONodeGetMatrix (*itema);
		}
	      if(ctm!=NULL)
		{
		  glPushMatrix  ();
		  pushed        = 1;
		  OMatrixConvertToGL_Matrix   (ctm,matrix);
		  glMultMatrixd (matrix);
		}
	      
	      highLight = ONodeGetHighlight (*itema); 
	      back      = ONodeGetBlink     (*itema);
	  
	      if(a_mode==TRAVERSE_SELECT) 
		{ 
		  if(ONodeGetPickability(*itema)==1)
		    {
		      ONodeSetPickIdentifier (*itema,glname);
		      glLoadName             (glname);
		      glname++;
		    }
		  else
		    {
		      ONodeSetPickIdentifier (*itema,0);
		      glLoadName             (0);
		    }
		}
	  
	      /*Loop over primitives.*/
	     {OPrimitive* prima;
	      OPrimitive* prims;
	      prims       = ONodeIsShape(*itema)==1 ? ONodeGetShapePrimitives(*itema):
	                                              ONodeGetPrimitives(*itema);
	      if(prims!=NULL)
		{
		  for(prima=prims;*prima!=NULL;prima++)
		    {
		      DrawPrimitive  (This,*itema,*prima,a_mode,back,highLight,context);
		    }
		}
	      if(ONodeIsShape(*itema)==1)
		CListDestroy ((CList)prims,(CListVisitEntryFunction)OPrimitiveDelete);}
	      
	      if(pushed==1) 
		{
		  glPopMatrix ();
		}
	      
	    }
	} /*end_for*/
      OContextDelete (context);

      if(a_mode==TRAVERSE_SELECT) 
	{
           int count;
	  GLint   hitn;
	  GLuint* ptr;

	  objects = (ONode*)CListCreate(itemn);
	  objecti = 0;

	  hitn    = glRenderMode   (GL_RENDER);
/*Loop over hit; get first name.*/
	  ptr     = selectBuffer; 
	  for(count=0;count<hitn;count++)
	    { 
	      GLuint       namen,/*z1,z2,*/glname;
	      ONode        object;

	      namen        = *ptr;ptr++;
	    /*z1           = *ptr;*/ptr++;
	    /*z2           = *ptr;*/ptr++;
	      glname       = namen <= 0 ? 0 : (*ptr);
              ptr         += namen;
              object       = GetObjectPicked(items,glname);
              if(object!=NULL)
                {
		  objects[objecti] = object;
		  objecti++;
		}
	    }
	}
      OMatrixDelete (cameraAligned);
      cameraAligned = NULL;
    }

  if(a_mode==TRAVERSE_SELECT) 
    {
      CMemoryFreeBlock     (selectBuffer);
      selectBuffer = NULL;
    }
  else
    {
      glFinish ();
    }}


#else
/*  ReportError         ("Traverse",CErrorHas);*/
  CWarn         ("OCamera.c file compiled without HAS_GL macro defined.\n");
  a_pick_x      = 0;
  a_pick_y      = 0;
  a_pick_width  = 0;
  a_pick_height = 0;
#endif

#ifdef DEBUG
  at     = time(NULL);
  printf ("debug : OCamera : traverse %s : end %d.\n",CameraName(This),at-start);
#endif

  This->nextRefreshOnTouched = 0;

  if(objecti<=0)  
    {
      CListDelete  ((CList)objects);
      return      NULL;
    }
  else
    {
      objects[objecti] = 0;
      return objects;
    }
}
/***************************************************************************/
void OCameraGetViewLimits (
 OCamera This 
,double* a_left
,double* a_right
,double* a_bottom
,double* a_top
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  width,height;
/*.........................................................................*/
  if(a_left!=NULL)   *a_left   = 0.;
  if(a_right!=NULL)  *a_right  = 0.;
  if(a_top!=NULL)    *a_top    = 0.;
  if(a_bottom!=NULL) *a_bottom = 0.;
  if(This==NULL) return;
  width   = This->width;
  height  = This->height;
  if(This->aspectRatio==1) 
    {
      if(width>=height)
        {
	  if(a_bottom!=NULL) *a_bottom = This->fieldMin;
	  if(a_top!=NULL)    *a_top    = This->fieldMax;
	  CHECK_DIV (height,"view_size")
	  if(a_left!=NULL)   *a_left   = *a_bottom * width/height;
	  if(a_right!=NULL)  *a_right  = *a_top    * width/height;
        }
      else
        {
          if(a_left!=NULL)   *a_left  = This->fieldMin;
          if(a_right!=NULL)  *a_right = This->fieldMax;
          CHECK_DIV (width,"view_size")
          if(a_bottom!=NULL) *a_bottom = *a_left  * height/width;
          if(a_top!=NULL)    *a_top    = *a_right * height/width;
        }
    }
  else
    {
      if(width>=height)
        {
          if(a_bottom!=NULL) *a_bottom = This->fieldMin;
          if(a_top!=NULL)    *a_top    = This->fieldMax;
          if(a_left!=NULL)   *a_left   = *a_bottom;
          if(a_right!=NULL)  *a_right  = *a_top;
        }
      else
        {
          if(a_left!=NULL)   *a_left   = This->fieldMin;
          if(a_right!=NULL)  *a_right  = This->fieldMax;
          if(a_bottom!=NULL) *a_bottom = *a_left;
          if(a_top!=NULL)    *a_top    = *a_right;
        }
    }
}
/***************************************************************************/
static ONode GetObjectPicked (
 ONode*  a_items
,int     a_glname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode* itema;
/*.........................................................................*/
  for(itema=a_items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if( (ONodeGetPickability(*itema)==1) && 
	  (ONodeGetPickIdentifier(*itema)==a_glname)) return *itema;
    }
  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OCameraSetRequireRefreshmentFunction (
 OCamera  This 
,OCameraSendFunction a_proc
,void*    a_arg
,int      a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_proc!=NULL)
    {
      This->sendRefreshProc = a_proc;
      This->sendRefreshArg  = a_arg;
    }
  This->sendRefreshDone = a_value;
}
/***************************************************************************/
int OCameraIsRefreshDone (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->sendRefreshDone;
}
/***************************************************************************/
void OCameraRequireRefreshment (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                  return;

  if(This->sendRefreshDone==1)    return;

  if(This->sendRefreshProc==NULL) return;

#ifdef DEBUG
  printf ("debug : OCameraRequireRefreshment : %s\n",CameraName(This));
#endif
  if(This->sendRefreshProc(This->sendRefreshArg)==0) return;

  This->sendRefreshDone   = 1;
}
/***************************************************************************/
void OCameraRequireDrawing (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : OCameraRequireDrawing : %s\n",CameraName(This));
#endif
  OCameraRequireRefreshment (This);
}
/***************************************************************************/
void OCameraReceiveRefreshmentRequest (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)               return;
  if(This->sendRefreshDone==0) return;
  Traverse              (This,TRAVERSE_DRAW,0,0,0,0);
  This->sendRefreshDone = 0;
}
/***************************************************************************/
void OCameraRefresh (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  Traverse  (This,TRAVERSE_DRAW,0,0,0,0);
}
/***************************************************************************/
void OCameraSetNextRefreshOnTouched (
 OCamera This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->nextRefreshOnTouched = 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OCameraSetRequireMovementFunction (
 OCamera  This 
,OCameraSendFunction a_proc
,void*    a_arg
,int      a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_proc!=NULL)
    {
      This->sendMoveProc = a_proc;
      This->sendMoveArg  = a_arg;
    }
  This->sendMoveDone = a_value;
}
/***************************************************************************/
void OCameraRequireMovement (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)               return;
  if(This->sendMoveDone==1)    return;
  if(This->sendMoveProc==NULL) return;

#ifdef DEBUG
  printf ("debug : OCameraRequireMovement : %s\n",CameraName(This));
#endif

  if(This->sendMoveProc(This->sendMoveArg)==0) return;
  This->sendMoveDone = 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
OMatrix OCameraGetTotalMatrix (
 OCamera This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)         return NULL;
  return OMatrixMultiply (This->global,This->ctm);
}
/***************************************************************************/
double OCameraGetRuleScale (
 OCamera This    
,OCamera a_rule    
,int* a_fdigit 
,int* a_nbzero 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double      sctm,scale,nbzero,nbpix,deter;
  int         fdigit;
  double      bx,by,bz;
  double      ex,ey,ez;
  OMatrix     matrix,inv;
  double      Vx,Vy,Vz;
/*.........................................................................*/
  if(a_fdigit!=NULL)     *a_fdigit = NotFound;
  if(a_nbzero!=NULL)     *a_nbzero = 0;
  if(This==NULL)         return 1.;
  if(a_rule==NULL)       return 1.;
  if(This->global==NULL) return 1.;

/* get scale of This */
  deter                = OMatrixGetDeterminant (This->ctm);
  CHECK_DIV            (deter,"OCameraGetRuleScale")
  sctm                 = pow(deter,1./3.);
  scale                = fabs(This->fieldMax-This->fieldMin) / sctm;

  nbzero               = floor(log10(scale));
  fdigit               = DBL_INT (scale/pow(10.,nbzero)); 
/*
  int                  sdigit;
  sdigit               = DBL_INT ((scale - fdigit * pow(10.,nbzero))/pow(10.,nbzero-1));
*/
  fdigit              ++;
  scale                = fdigit * pow(10.,nbzero) ;

/* get nb of pixel of scale */
  CVectorMultiply      (This->vrp_x,This->vrp_y,This->vrp_z,This->up_x,This->up_y,This->up_z,&Vx,&Vy,&Vz);
  CVectorNormalize     (&Vx,&Vy,&Vz);

  OMatrixMultiplyVector (This->global,0.,0.,0.,&bx,&by,&bz);
  OMatrixMultiplyVector (This->global,sctm * scale * Vx, sctm * scale * Vy,sctm * scale * Vz,&ex,&ey,&ez);

  nbpix                 = sqrt( (ey-by) * (ey-by) + (ex-bx) * (ex-bx));

/* compute scale of rule */
  matrix                = OCameraGetTotalMatrix (a_rule);
  inv                   = OMatrixInvert (matrix);
  OMatrixMultiplyVector (inv,0.   ,0.,0.,&(bx),&(by),&(bz));
  OMatrixMultiplyVector (inv,nbpix,0.,0.,&(ex),&(ey),&(ez));
  OMatrixDelete         (matrix);
  OMatrixDelete         (inv);

  if(a_fdigit!=NULL)   *a_fdigit = fdigit;
  if(a_nbzero!=NULL)   *a_nbzero = DBL_INT nbzero;
  return               sqrt( (ey-by) * (ey-by) + (ex-bx) * (ex-bx));

}
/***************************************************************************/
void OCameraGetNDC (
 OCamera This  
,OMatrix a_ndc   
,OMatrix a_ndc_1 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double    width,height;
/*.........................................................................*/
  if(This==NULL) return;
  width              = This->width/2.;
  height             = This->height/2.;
/* view - >ndc */
  OMatrixInitialize        (a_ndc  ,OMatrixScaleTranslation,width,-height,1.,width,height ,0.);
  OMatrixInvertAndAssign   (a_ndc_1,a_ndc);
}
/***************************************************************************/
double OCameraGetScale (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double deter;
/*.........................................................................*/
  if(This==NULL) return 1.;
  deter     = OMatrixGetDeterminant (This->ctm);
  CHECK_DIV (deter,"OCameraGetScale")
  return    fabs(This->fieldMax-This->fieldMin) / pow(deter,1./3.);
}
/***************************************************************************/
void OCameraSetProjection (
 OCamera This 
,OProjection a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_type==This->projection) return;
  This->projection = a_type;
  ONodeRequireViewRefreshment      (This);
}
/***************************************************************************/
OProjection OCameraGetProjection (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OProjectionParallel;
  return This->projection;
}
/***************************************************************************/
void OCameraSetProjectionAngle (
 OCamera This 
,double a_angle 
)
/***************************************************************************/
/*
  a_angle : angle in radian.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_angle==This->projectionAngle) return;
  This->projectionAngle          = a_angle;
  if((This->projectionAngle<=0.)||(This->projectionAngle>=M_PI)) This->projectionAngle  = M_PI/2.;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
double OCameraGetProjectionAngle (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return M_PI/2.;
  return This->projectionAngle;
}
/***************************************************************************/
void OCameraSetSpecialProjection (
 OCamera This 
,OSpecialProjection a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_type==This->specialProjection.specialProjection) return;
  This->specialProjection.specialProjection = a_type;
  ONodeRequireViewRefreshment               (This);
}
/***************************************************************************/
OSpecialProjection OCameraGetSpecialProjection (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OSpecialProjectionXYZ;
  return This->specialProjection.specialProjection;
}
/***************************************************************************/
void OCameraSetDoSpecialProjectionFunction (
 OCamera This 
,OCameraDoSpecialProjectionFunction a_function
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->specialProjection.doSpecialProjectionFunction==a_function) return;
  This->specialProjection.doSpecialProjectionFunction = a_function;
  ONodeRequireViewRefreshment    (This);
}
/***************************************************************************/
OCameraDoSpecialProjectionFunction OCameraGetDoSpecialProjectionFunction (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->specialProjection.doSpecialProjectionFunction;
}
/***************************************************************************/
void OCameraSetDepthTreatment (
 OCamera This 
,ODepthTreatment a_DepthTest 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_DepthTest==This->depthTreatment) return;
  This->depthTreatment = a_DepthTest;
  ONodeRequireViewRefreshment    (This);
}
/***************************************************************************/
ODepthTreatment OCameraGetDepthTreatment (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return ODepthTreatmentNone;
  return This->depthTreatment;
}
/***************************************************************************/
void OCameraSetBuffering (
 OCamera This 
,OBuffering a_buffering 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_buffering==This->buffering) return;
  This->buffering  = a_buffering;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
OBuffering OCameraGetBuffering (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OBufferingSimple;
  return This->buffering;
}
/***************************************************************************/
void OCameraGetWorldCoordinates (
 OCamera This 
,int a_ix
,int a_iy
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix,inv;
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL)      return;
  matrix         = OCameraGetTotalMatrix (This);
  inv            = OMatrixInvert         (matrix);
  OMatrixMultiplyVector   (inv,(double)a_ix,(double)a_iy,0.,a_x,a_y,a_z);
  OMatrixDelete  (matrix);
  OMatrixDelete  (inv);
}
/***************************************************************************/
void OCameraGetField (
 OCamera This 
,double* a_min 
,double* a_max 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_min!=NULL) *a_min = -1.;
  if(a_max!=NULL) *a_max =  1.;
  if(This==NULL) return;
  if(a_min!=NULL) *a_min = This->fieldMin;
  if(a_max!=NULL) *a_max = This->fieldMax;
}
/***************************************************************************/
void OCameraGetDefaultVRP (
 OCamera This        
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL) return;
  if(a_x!=NULL) *a_x = This->viewVrp_x;
  if(a_y!=NULL) *a_y = This->viewVrp_y;
  if(a_z!=NULL) *a_z = This->viewVrp_z;
}
/***************************************************************************/
void OCameraGetDefaultUpVector (
 OCamera This        
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL) return;
  if(a_x!=NULL) *a_x = This->viewUp_x;
  if(a_y!=NULL) *a_y = This->viewUp_y;
  if(a_z!=NULL) *a_z = This->viewUp_z;
}
/***************************************************************************/
void OCameraGetVRP (
 OCamera This        
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL) return;
  if(a_x!=NULL) *a_x = This->vrp_x;
  if(a_y!=NULL) *a_y = This->vrp_y;
  if(a_z!=NULL) *a_z = This->vrp_z;
}
/***************************************************************************/
void OCameraGetCenter (
 OCamera This        
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL) return;
  if(a_x!=NULL) *a_x = This->center_x;
  if(a_y!=NULL) *a_y = This->center_y;
  if(a_z!=NULL) *a_z = This->center_z;
}
/***************************************************************************/
void OCameraGetUpVector (
 OCamera This        
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL) return;
  if(a_x!=NULL) *a_x = This->up_x;
  if(a_y!=NULL) *a_y = This->up_y;
  if(a_z!=NULL) *a_z = This->up_z;
}
/***************************************************************************/
void OCameraGetDOP (
 OCamera This        
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double dop_x,dop_y,dop_z;
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 1.;
  if(This==NULL) return;
  dop_x        = This->center_x - This->vrp_x;
  dop_y        = This->center_y - This->vrp_y;
  dop_z        = This->center_z - This->vrp_z;
  CVectorNormalize (&(dop_x),&(dop_y),&(dop_z));
  if(a_x!=NULL) *a_x = dop_x;
  if(a_y!=NULL) *a_y = dop_y;
  if(a_z!=NULL) *a_z = dop_z;
}
/***************************************************************************/
void OCameraRestoreDefaultUpVectorAndVRP (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->vrp_x                = This->viewVrp_x;
  This->vrp_y                = This->viewVrp_y;
  This->vrp_z                = This->viewVrp_z;
  This->up_x                 = This->viewUp_x;
  This->up_y                 = This->viewUp_y;
  This->up_z                 = This->viewUp_z;
  ONodeRequireViewRefreshment  (This);
  OCameraRequireMovement     (This);
}
/***************************************************************************/
void OCameraSetField (
 OCamera This 
,double a_min 
,double a_max 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->fieldMin   = a_min;
  This->fieldMax   = a_max;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraSetDefaultVRP (
 OCamera This 
,double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  This->viewVrp_x    = a_x;
  This->viewVrp_y    = a_y;
  This->viewVrp_z    = a_z;
  OCameraRestoreDefaultUpVectorAndVRP  (This);
}
/***************************************************************************/
void OCameraSetCenter (
 OCamera This 
,double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->center_x        = a_x;
  This->center_y        = a_y;
  This->center_z        = a_z;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraSetVRP (
 OCamera This 
,double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->vrp_x        = a_x;
  This->vrp_y        = a_y;
  This->vrp_z        = a_z;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraSetDefaultUpVector (
 OCamera This 
,double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)         return;
  This->viewUp_x    = a_x;
  This->viewUp_y    = a_y;
  This->viewUp_z    = a_z;
  OCameraRestoreDefaultUpVectorAndVRP (This);
}
/***************************************************************************/
void OCameraSetUpVector (
 OCamera This 
,double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  This->up_x         = a_x;
  This->up_y         = a_y;
  This->up_z         = a_z;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraScaleDefaultFieldAndVRP (
 OCamera This 
,double a_scale 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->fieldMin       *= a_scale;
  This->fieldMax       *= a_scale;

  This->viewVrp_x    *= a_scale;
  This->viewVrp_y    *= a_scale;
  This->viewVrp_z    *= a_scale;

  This->vrp_x                  = This->viewVrp_x;
  This->vrp_y                  = This->viewVrp_y;
  This->vrp_z                  = This->viewVrp_z;

  ONodeRequireViewRefreshment      (This);
  OCameraRequireMovement       (This);
}
/***************************************************************************/
void OCameraMultiplyUpVectorAndVRP (
 OCamera This 
,OMatrix a_matrix 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double x,y,z;
/*.........................................................................*/
  if(This==NULL) return;
  OMatrixMultiplyVector            (a_matrix,This->vrp_x,This->vrp_y,This->vrp_z,&x,&y,&z);
  This->vrp_x            = x;
  This->vrp_y            = y;
  This->vrp_z            = z;

 {double ox,oy,oz;
  double x,y,z;
  OMatrixMultiplyVector   (a_matrix,0.,0.,0.,&ox,&oy,&oz);
  OMatrixMultiplyVector   (a_matrix,This->up_x,This->up_y,This->up_z,&x,&y,&z);
  This->up_x    = x - ox;
  This->up_y    = y - oy;
  This->up_z    = z - oz;}

  ONodeRequireViewRefreshment     (This);
  OCameraRequireMovement        (This);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void DrawPrimitive (
 OCamera     This 
,ONode       a_node
,OPrimitive  a_prim 
,int         a_mode
,int         a_back
,int         a_highlight
,OContext    a_context
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           pointn;
  double*       xs;
  double*       ys;
  double*       zs;
  double        red,green,blue;
  int           index;
  int           width,size;
  OLineStyle    sline;
  OMarkerStyle  smark;
  OAreaStyle    sarea;
/*.........................................................................*/
  if(This==NULL) return;

  pointn = OPrimitiveGetPoints (a_prim,&xs,&ys,&zs);
  if(This->specialProjection.doSpecialProjectionFunction!=NULL)
    pointn = This->specialProjection.doSpecialProjectionFunction (This,a_node,pointn,xs,ys,zs,&xs,&ys,&zs);
  else
    pointn = OCameraDoDefaultSpecialProjection (This,a_node,pointn,xs,ys,zs,&xs,&ys,&zs);

  if( (xs==NULL) || (ys==NULL) || (zs==NULL) )  return;
  if(pointn<=0)          return;


  if(a_back!=0)
    {
      index = This->backgroundColor;
    }
  else 
    {
      index = a_highlight==1 ? OContextGetColorIndex(a_context) : OPrimitiveGetColorIndex(a_prim);
    }
  if((This->depthTreatment==ODepthTreatmentNone) && (This->nextRefreshOnTouched==1) )
    {
      width  = OPrimitiveGetLineWidth     (a_prim);
      size   = OPrimitiveGetMarkerSize    (a_prim);
      sline  = OPrimitiveGetLineStyle     (a_prim);
      smark  = OPrimitiveGetMarkerStyle   (a_prim);
    }
  else
    {
      width  = a_highlight==1 ? OContextGetLineWidth  (a_context) : OPrimitiveGetLineWidth     (a_prim);
      size   = a_highlight==1 ? OContextGetMarkerSize (a_context) : OPrimitiveGetMarkerSize    (a_prim);
      sline  = a_highlight==1 ? OContextGetLineStyle  (a_context) : OPrimitiveGetLineStyle     (a_prim);
      smark  = a_highlight==1 ? OContextGetMarkerStyle(a_context) : OPrimitiveGetMarkerStyle   (a_prim);
    }

  sarea  = OPrimitiveGetAreaStyle (a_prim);

       if(This->bw==1) 
    { 
      red = green = blue = 1.;
    }
  else if(This->bw==2) 
    { 
      red = green = blue = 0.;
    }
  else 
    {
      OColormapGetIthRGB    (This->ocmap,index,&red,&green,&blue);
    }

#ifdef HAS_GL

  glColor3d (red,green,blue);

  switch (OPrimitiveGetType(a_prim))
    {
       int count;
          case OPrimitivePolygon:
	    if(a_mode==TRAVERSE_PS) 
	      {
		PS_DrawPolygon (pointn,xs,ys,zs,sarea);
	      }
	    else
	      {
		SetLineStyle  (sline);
		glLineWidth   ((GLfloat)width);
		glPolygonMode (GL_FRONT,GL_FILL);
		glBegin       (GL_POLYGON);
		for (count=0;count<pointn;count++) 
		  {
		    glVertex3d (xs[count],ys[count],zs[count]);
		  }
		glEnd   ();
		if(This->edgesVisible==1)
		  {
		    if(This->bw==1) 
		      { 
			red = green = blue = 0.;
		      }
		    else if(This->bw==2) 
		      { 
			red = green = blue = 1.;
		      }
		    else 
		      {
			OColormapGetIthRGB    (This->ocmap,This->edgeColor,&red,&green,&blue);
		      }
		    glColor3d     (red,green,blue);
		    glPolygonMode (GL_FRONT,GL_LINE);
		    glBegin       (GL_POLYGON);
		    for (count=0;count<pointn;count++) 
		      {
			glVertex3d (xs[count],ys[count],zs[count]);
		      }
		    glEnd   ();
		  }
	      }
            break;
          case OPrimitiveLines      :
	    if(a_mode==TRAVERSE_PS) 
	      {
		PS_DrawLines (pointn,xs,ys,zs,sline,width);
	      }
	    else
	      {
		SetLineStyle  (sline);
		glLineWidth   ((GLfloat)width);
		glBegin       (GL_LINE_STRIP);
		for (count=0;count<pointn;count++) 
		  {
		    glVertex3d (xs[count],ys[count],zs[count]);
		  }
		glEnd   ();
	      }
            break;
          case OPrimitiveSegments      :
	    if(a_mode==TRAVERSE_PS) 
	      {
		PS_DrawSegments (pointn,xs,ys,zs,sline,width);
	      }
	    else
	      {
		SetLineStyle  (sline);
		glLineWidth   ((GLfloat)width);
		glBegin       (GL_LINES);
		for (count=0;count<pointn;count++) 
		  {
		    glVertex3d (xs[count],ys[count],zs[count]);
		  }
		glEnd   ();
	      }
            break;
          case OPrimitiveMarkers      :
	    if(a_mode==TRAVERSE_PS) 
	      {
		PS_DrawMarkers (pointn,xs,ys,zs,smark,size);
	      }
	    else
	      {
		if(smark==OMarkerStyleDot) glPointSize ((GLfloat)size);
		else          	           glPointSize (1.0);
		glBegin      (GL_POINTS);
		for (count=0;count<pointn;count++) 
		  {
		    glVertex3d (xs[count],ys[count],zs[count]); /*So that picking works.*/
		  }
		glEnd   ();
		glPixelStorei (GL_UNPACK_ALIGNMENT,1);
		for (count=0;count<pointn;count++) 
		  {
		    glRasterPos3d (xs[count],ys[count],zs[count]);
		    DrawMarker    (smark,size);
		  }
	      }
            break;
    }

#else
  ReportError ("DrawPrimitive",CErrorHas);
#endif

}
/***************************************************************************/
static void SetLineStyle (
 OLineStyle a_style
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  switch(a_style)
    {
      case OLineStyleSolid:
        glLineStipple (1,0x0000);
        break;
      case OLineStyleDashed:
        glLineStipple (1,0x00FF);
        break;
      case OLineStyleDotted:
        glLineStipple (1,0x0101);
        break;
      case OLineStyleDashDotted:
        glLineStipple (1,0x1C47);
        break;
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/******** OCamera matrix (ctm) *********************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
OMatrix OCameraGetMatrix (
 OCamera This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->ctm;
}
/***************************************************************************/
void OCameraSetDefaultMatrix (
 OCamera This 
,OMatrix a_matrix 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(a_matrix==NULL)      return;
  OMatrixCopy                 (This->ctmInit,a_matrix);
  OCameraRestoreDefaultMatrix (This);
}
/***************************************************************************/
void OCameraRestoreDefaultMatrix (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  OMatrixCopy        (This->ctm,This->ctmInit);
  ONodeRequireViewRefreshment   (This);
  OCameraRequireMovement    (This);
}
/***************************************************************************/
void OCameraSetMatrix (
 OCamera This 
,OMatrix a_matrix 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)         return;
  if(a_matrix==NULL)     return;
  OMatrixCopy               (This->ctm,a_matrix);
  ONodeRequireViewRefreshment (This);
  OCameraRequireMovement    (This);
}
/***************************************************************************/
void OCameraMultiplyMatrix (
 OCamera This 
,OMatrix a_matrix 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix;
/*.........................................................................*/
  if(This==NULL)      return;
  matrix              = OMatrixMultiply  (a_matrix,This->ctm);
  OMatrixCopy         (This->ctm,matrix);
  OMatrixDelete       (matrix);

  ONodeRequireViewRefreshment (This);
  OCameraRequireMovement      (This);
}
/***************************************************************************/
double OCameraGetMatrixScale (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1.;
  return   pow(OMatrixGetDeterminant (This->ctm),1./3.);
}
/***************************************************************************/
void OCameraPutMatrixOnStack (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return; 
  if(This->ctmn>=MAX__CTM) This->ctmn--;
  OMatrixDelete          (This->ctms[This->ctmn]);
  This->ctms[This->ctmn] = OMatrixDuplicate (This->ctm);
  This->ctmn++;
}
/***************************************************************************/
void OCameraRestoreLastMatrixOfStack (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return; 
  This->ctmn--;
  if(This->ctmn<0) {This->ctmn = 0;return;}
  OCameraSetMatrix (This,This->ctms[This->ctmn]);
}
/***************************************************************************/
void OCameraRestoreFirstMatrixOfStack (
 OCamera This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)        return; 
  This->ctmn       = 0;
  OCameraSetMatrix (This,This->ctms[This->ctmn]);
}
/***************************************************************************/
void OCameraCenterMatrixOnPick (
 OCamera This 
,OPick a_pick 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix   matrix;
  double    x,y,z;
/*.........................................................................*/
  if(This==NULL)        return;
  if(a_pick==NULL)      return;
  OPickGetPoint         (a_pick,&x,&y,&z);
  GetWorldCoordinates   (This,(ONode)OPickGetUserData(a_pick),&x,&y,&z);
  matrix                = OMatrixCreate(OMatrixTranslation,-x,-y,-z);
  OCameraMultiplyMatrix (This,matrix);
  OMatrixDelete         (matrix);
}
/***************************************************************************/
void OCameraAlignMatrixOnPick (
 OCamera This 
,OPick a_begin 
,OPick a_end   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double           bx,by,bz;
  double           ex,ey,ez;
  OMatrix          matrix,translate,axis_to_z,z_to_dop;
  double           dopx,dopy,dopz;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_begin==NULL) return;
  if(a_end==NULL)   return;
  OPickGetPoint            (a_begin,&bx,&by,&bz);
  OPickGetPoint            (a_end  ,&ex,&ey,&ez);
  GetWorldCoordinates             (This,(ONode)OPickGetUserData(a_begin),&bx,&by,&bz);
  GetWorldCoordinates             (This,(ONode)OPickGetUserData(a_end)  ,&ex,&ey,&ez);
  if((bx==ex)&&(by==ey)&&(bz==ez)) return;

  translate             = OMatrixCreate       (OMatrixTranslation,- bx,- by,- bz);
  OCameraGetDOP         (This,&dopx,&dopy,&dopz);
  axis_to_z             = OMatrixCreate       (OMatrixVectorToZ,ex - bx,ey - by,ez - bz);
  z_to_dop              = OMatrixCreate       (OMatrixZToVector,dopx,dopy,dopz);

  matrix                = OMatrixMultiplyMatrices (3,z_to_dop,axis_to_z,translate);
  OCameraMultiplyMatrix (This,matrix);
  OMatrixDelete         (translate);
  OMatrixDelete         (axis_to_z);
  OMatrixDelete         (z_to_dop);
  OMatrixDelete         (matrix);
}
/***************************************************************************/
void OCameraRotateMatrixOnPick (
 OCamera This 
,double  a_value 
,OPick a_begin 
,OPick a_end   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double           bx,by,bz;
  double           ex,ey,ez;
  OMatrix          matrix,origin,rotate,original;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_begin==NULL) return;
  if(a_end==NULL)   return;

  OPickGetPoint            (a_begin,&bx,&by,&bz);
  OPickGetPoint            (a_end  ,&ex,&ey,&ez);
  GetWorldCoordinates             (This,(ONode)OPickGetUserData(a_begin),&bx,&by,&bz);
  GetWorldCoordinates             (This,(ONode)OPickGetUserData(a_end)  ,&ex,&ey,&ez);
  if((bx==ex)&&(by==ey)&&(bz==ez)) return;

  origin                = OMatrixCreate         (OMatrixTranslation  ,-bx,-by,-bz);
  rotate                = OMatrixCreate         (OMatrixRotationAxis ,a_value,ex-bx,ey-by,ez-bz);
  original              = OMatrixCreate         (OMatrixTranslation  ,bx,by,bz);
  matrix                = OMatrixMultiplyMatrices (3,original,rotate,origin);

  OCameraMultiplyMatrix (This,matrix);
  OMatrixDelete         (origin);
  OMatrixDelete         (original);
  OMatrixDelete         (rotate);
  OMatrixDelete         (matrix);
}
/***************************************************************************/
void OCameraTranslateMatrixOnPick (
 OCamera This 
,double  a_value 
,OPick a_begin 
,OPick a_end   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double           bx,by,bz;
  double           ex,ey,ez;
  OMatrix          matrix,origin,translate,original;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_begin==NULL) return;
  if(a_end==NULL)   return;

  OPickGetPoint          (a_begin,&bx,&by,&bz);
  OPickGetPoint          (a_end  ,&ex,&ey,&ez);
  GetWorldCoordinates           (This,(ONode)OPickGetUserData(a_begin),&bx,&by,&bz);
  GetWorldCoordinates           (This,(ONode)OPickGetUserData(a_end)  ,&ex,&ey,&ez);
  if((bx==ex)&&(by==ey)&&(bz==ez)) return;

  origin              = OMatrixCreate         (OMatrixTranslation,-bx,-by,-bz);
  translate           = OMatrixCreate         (OMatrixTranslationAxis,a_value,ex-bx,ey-by,ez-bz);
  original            = OMatrixCreate         (OMatrixTranslation,bx,by,bz);
  matrix              = OMatrixMultiplyMatrices (3,original,translate,origin);

  OCameraMultiplyMatrix (This,matrix);
  OMatrixDelete         (origin);
  OMatrixDelete         (original);
  OMatrixDelete         (translate);
  OMatrixDelete         (matrix);
}
/***************************************************************************/
static void GetWorldCoordinates (
 OCamera This
,ONode a_obj  
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  x,y,z;
  OMatrix matrix;
/*.........................................................................*/
  if(This==NULL)        return;
  matrix           = ONodeGetMatrix(a_obj)!=NULL ? 
                      OMatrixMultiply    (This->ctm,ONodeGetMatrix(a_obj)) :
                      OMatrixDuplicate   (This->ctm);
  OMatrixMultiplyVector      (matrix,*a_x,*a_y,*a_z,&x,&y,&z);
  *a_x             = x;
  *a_y             = y;
  *a_z             = z;
  OMatrixDelete    (matrix);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/******** OCamera ONode connection *****************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
ONode* OCameraGetNodes (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode* itema;
/*.........................................................................*/
  if(This==NULL)      return NULL;
  CListDelete ((CList)Class.nodes);
  Class.nodes = NULL;
  for(itema=This->toView;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if( (ONodeIsValid(*itema)==1) && (CListGetEntryIndex(ONodeGetViews(*itema),This,NULL)!=NotFound) )
	{
	  ONode*           list;
	  list             = ONodeGetLeavesForward (*itema);
	  CListConcatenate ((CList*)&(Class.nodes),(CList)list);
	  CListDelete      ((CList)list);
	 }
      else
        {
	  CListRemoveEntry ((CList)This->toView,*itema);
	  itema--;
	}
    }
  return Class.nodes;
}
/***************************************************************************/
void OCameraViewNode (
 OCamera This
,ONode a_node
)
/***************************************************************************/
/*
 Description :
   Puts a node in the viewing list of an OCamera. When an 
  OCameraRefresh is performed, all nodes in the viewing list, 
  along with their children, are traversed to be rendered. 
  See Go/v3/src/GoT.c for a running example.
 Arguments :
   This : OCamera that accepts the a_node ONode.
   a_node : ONode to add in the This OCamera viewing list.
 Example : 
...
#include <OCamera.h>
...
  {OCamera ocamera;
   ONode   onode;
   ocamera = OCameraGetIdentifier ("Camera");
   onode   = ONodeCreate ("Cube");
...
   OCameraViewNode (ocamera,onode);
  }
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_node==NULL) return;
  ViewNode         (This,a_node);
/*Add object in OCameras connected to This.*/
 {OCamera* itema;
  for(itema=This->referenceCamera;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if(OCameraIsValid(*itema)==1) ViewNode (*itema,a_node);
    }}
}
/***************************************************************************/
void OCameraUnviewNode (
 OCamera This
,ONode a_node
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_node==NULL) return;
  UnviewNode       (This,a_node);
/*Remove object in OCameras connected to This.*/
 {OCamera* itema;
  for(itema=This->referenceCamera;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if(OCameraIsValid(*itema)==1) UnviewNode (*itema,a_node);
    }}
}
/***************************************************************************/
static void ViewNode (
 OCamera This
,ONode a_node
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(a_node==NULL)        return;
  if(CListGetEntryIndex((CList)This->toView,a_node,NULL)==NotFound)
    {
      CListAddEntry ((CList*)&(This->toView),a_node);
    }
  ONodeAddView              (a_node,This); /*Do not involve refresh.*/
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
static void UnviewNode (
 OCamera This
,ONode a_node
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(a_node==NULL)        return;
  if(CListGetEntryIndex((CList)This->toView,a_node,NULL)!=NotFound) 
    {
      CListRemoveEntry ((CList)(This->toView),a_node);
    }
  ONodeRemoveView           (a_node,This); /*Do not involve refresh.*/
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraViewNodes (
 OCamera This
,ONode* a_nodes
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_nodes==NULL) return;
  ViewNodes         (This,a_nodes);
/*Add object in OCameras connected to This.*/
 {OCamera* itema;
  for(itema=This->referenceCamera;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if(OCameraIsValid(*itema)==1) ViewNodes (*itema,a_nodes);
    }}
}
/***************************************************************************/
static void ViewNodes (
 OCamera This
,ONode* a_nodes
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode* itema;
/*.........................................................................*/
  if(This==NULL)              return;
  if(a_nodes==NULL)           return;
  CListConcatenate            ((void***)&(This->toView),(void**)a_nodes);
  CListRemoveDuplicates       ((void**)(This->toView),NULL,0);
  for(itema=a_nodes;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      ONodeAddView  (*itema,This);
    }
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
ONode* OCameraCollect (
 OCamera This 
,int a_what
,...
)
/***************************************************************************/
/*
 Description :
   Returns a list of ONode viewed by an OCamera object.
  A subselection could be done according to the a_what parameter and its 
  following arguments. The returned list has to be freed with the 
  routine CListDelete.
 Arguments :
  - This : OCamera concerned.
  - a_what : 
     OCollectAll : to have all nodes viewed by This.
     OCollectHighlighted : to get only the highlighted nodes.
     OCollectNoHighlighted : for the not highlighted nodes.
     CollectMatchingName : for nodes whose name matches 
       a pattern given as third argument.
     OCollectHighlightedAndMatchingName : for highlighted nodes whose 
       name matches a pattern given as third argument.
     OCollectFirstNodeMatchingColorIndex : to get first node that have a graphical
       primitive that matches a color index given as third argument.
 Example : to retrieve the highlighted nodes viewed by an OCamera.
...
#include <CList.h>
#include <OCamera.h>
...
  {ONode* nodes;
   int    noden,count;
   nodes  = OCameraCollect (OCameraGetIdentifier("my_camera"),OCollectHighlighted);
   noden  = CListGetSize   ((CList)nodes);
   ...
   CListDelete ((CList)nodes);}
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode*  nodes;
  va_list args;
  ONode*  list;
/*.........................................................................*/
  if(This==NULL) return NULL;
  nodes    = OCameraGetNodes (This);
  va_start (args,a_what);
  list     = ONodeCollectV (nodes,a_what,args);
  va_end   (args);
  return   list;
}
/***************************************************************************/
int OCameraIsEmpty (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode*  itema;
/*.........................................................................*/
  if(This==NULL) return 0;
  itema     = OCameraGetNodes   (This);
  return    ( (itema!=NULL) && (*itema!=NULL) ) ? 1 : 0;
}
/***************************************************************************/
void OCameraErase (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode*  itema;
/*.........................................................................*/
  if(This==NULL)     return;
  itema              = OCameraGetNodes   (This);
  for(;(itema!=NULL) && (*itema!=NULL);itema++) ONodeDelete (*itema);
  CListDelete        ((CList)This->toView);
  This->toView       = NULL;
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraHighlightNodes (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode*  itema;
/*.........................................................................*/
  if(This==NULL)            return;
  itema                     = OCameraGetNodes   (This);
  for(;(itema!=NULL) && (*itema!=NULL);itema++) ONodeSetHighlight (*itema,1);
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraUnhighlightNodes (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode*  itema;
/*.........................................................................*/
  if(This==NULL)            return;
  itema                     = OCameraGetNodes   (This);
  for(;(itema!=NULL) && (*itema!=NULL);itema++) ONodeSetHighlight (*itema,0);
  OCameraDeletePicks        (This);
  ONodeRequireViewRefreshment (This);
}
/***************************************************************************/
void OCameraConnect (
 OCamera This
,OCamera a_camera
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                    return;
  if(OCameraIsValid(a_camera)==0) return;
  CListAddEntry       ((CList*)&(a_camera->referenceCamera),This);
  CListRemoveDuplicates ((CList)a_camera->referenceCamera,NULL,0);
/*Let all object viewed by a_camera, viewable by This.*/
  CListDelete         ((CList)This->toView);
  This->toView        = NULL;
 {ONode*              itema;
  itema               = OCameraGetNodes   (a_camera);
  for(;(itema!=NULL) && (*itema!=NULL);itema++) OCameraViewNode (This,*itema);}
}
/***************************************************************************/
void OCameraNormalizeMatrix (
 OCamera  This
,ONode* a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode*  items;
  OMatrix   matrix,mxtrans,mxscale;
  double    xmin,xmax,ymin,ymax,zmin,zmax,dx,dy,dz,radius,scale;
  double    centerx,centery,centerz;
/*.........................................................................*/
  if(This==NULL)     return;

  items         = a_list!=NULL ? a_list : OCameraGetNodes   (This);

  if(GetExtension (items,&xmin,&xmax,&ymin,&ymax,&zmin,&zmax)==1)
    {
      centerx         = (xmin+xmax)/2.;
      centery         = (ymin+ymax)/2.;
      centerz         = (zmin+zmax)/2.;
      /* radius of sphere containing max wc box */
      dx               = xmax - xmin;
      dy               = ymax - ymin;
      dz               = zmax - zmin;
      
      radius           = sqrt ( dx * dx + dy * dy + dz * dz)/2.;
      if(radius<=0.)   radius = 1.;

      scale            = .9 * fabs(This->fieldMax-This->fieldMin) / (2.*radius);

/* reset ctm */
      mxtrans           = OMatrixCreate   (OMatrixTranslation,-centerx,-centery,-centerz);
      mxscale           = OMatrixCreate   (OMatrixScaleIsometric,scale);
      matrix            = OMatrixMultiply (mxscale,mxtrans);
      OCameraSetMatrix  (This,matrix);
      
      OMatrixDelete     (matrix);
      OMatrixDelete     (mxtrans);
      OMatrixDelete     (mxscale);

    }
}
/***************************************************************************/
void OCameraAddPick (
 OCamera This 
,OPick a_pick 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CListAddEntry  ((CList*)&(This->picks),a_pick);
}
/***************************************************************************/
void OCameraDeletePicks (
 OCamera This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CListDestroy   ((CList)This->picks,(CListVisitEntryFunction)OPickDelete);
  This->picks    = NULL;
}
/***************************************************************************/
OPick OCameraGetPick (
 OCamera This 
,int a_count 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     number;
/*.........................................................................*/
  if(This==NULL) return NULL;
  number = CListGetSize((CList)This->picks);
  if((a_count<0)||(a_count>=number)) return NULL;
  return This->picks[a_count];
}
/***************************************************************************/
ONode* OCameraGetNodesInRegion (
 OCamera This 
,int a_beg_x
,int a_beg_y
,int a_end_x
,int a_end_y
,OPick* a_pick 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode*    objects;
  int         bx,by,bw,bh;
  int         size = 6;
/*.........................................................................*/
  if(a_pick!=NULL) *a_pick = NULL;
  if(This==NULL)  return NULL;

  bx          = MINIMUM (a_beg_x,a_end_x);
  by          = MINIMUM (a_beg_y,a_end_y);
  bw          = MAXIMUM (a_beg_x,a_end_x) - bx;
  bh          = MAXIMUM (a_beg_y,a_end_y) - by;

  if(a_beg_x==a_end_x)
    { 
      bx         = a_beg_x - size/2;
      bw         = size;
    }

  if(a_beg_y==a_end_y) 
    { 
      by         = a_beg_y - size/2;
      bh         = size;
    }


  if(bx<0)               bx = 0;
  if(by<0)               by = 0;
  if(bx+bw>This->width)  bw = This->width  - bx;
  if(by+bh>This->height) bh = This->height - by;

  objects = Traverse   (This,TRAVERSE_SELECT,bx,by,bw,bh);

/*GL unable to return picking point. Pick point is then set as barycenter of ONode.*/
  if( (a_pick!=NULL) && (objects!=NULL) && (objects[0]!=NULL) )
    {
      double x,y,z;
      int    pointn;
      pointn = ONodeGetSumOfPoints (objects[0],&x,&y,&z);
      if(pointn!=0)
	{
	  OPick            opick;
	  opick            = OPickMake();
	  OPickSetUserData (opick,objects[0]);
	  OPickSetPoint    (opick,x/pointn,y/pointn,z/pointn);
	  if(a_pick!=NULL) *a_pick = opick;
	}
    }

  return     objects;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OCameraSetPlotLayout (
 OCamera This
,int a_3d
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_3d==1)
    { 
      OMatrix                  matrix,mhisto,msurface;
      mhisto                   = GetHistogramMatrix   ();
      msurface                 = GetSurfaceMatrix ();
      matrix                   = OMatrixMultiply       (msurface,mhisto);
      OMatrixDelete            (mhisto);
      OMatrixDelete            (msurface);
      OCameraSetDefaultMatrix  (This,matrix);
      OMatrixDelete            (matrix);
    }
  else
    { 
      OMatrix                  matrix;
      matrix                   = GetHistogramMatrix ();
      OCameraSetDefaultMatrix  (This,matrix);
      OMatrixDelete            (matrix);
    }
}
/***************************************************************************/
static OMatrix GetSurfaceMatrix (
)
/***************************************************************************/
/*
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix,z45,rot,scale;
/*.........................................................................*/
  z45     = OMatrixCreate(OMatrixRotationZ, 45.*M_PI/180.);
  rot     = OMatrixCreate(OMatrixRotationX,-60.*M_PI/180.);
  scale   = OMatrixCreate(OMatrixScaleIsometric,0.7);
  matrix  = OMatrixMultiplyMatrices (3,scale,rot,z45);
  OMatrixDelete(scale);
  OMatrixDelete(rot);
  OMatrixDelete(z45);
  return matrix;
}
/***************************************************************************/
static OMatrix GetHistogramMatrix (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double    adjust;
  OMatrix   matrix,mtx0,mtx1;
/*.........................................................................*/
/*position*/
  adjust  = .7;
  mtx0    = OMatrixCreate(OMatrixScaleTranslation,adjust,adjust,adjust,.2,.2,.2);
  mtx1    = OMatrixCreate(OMatrixScaleTranslation,2.,2.,2.,-1.,-1.,-1.);
  matrix  = OMatrixMultiply (mtx1,mtx0);
  OMatrixDelete (mtx0);
  OMatrixDelete (mtx1);
  return matrix;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OCameraSetBlinkDelay (
 Ulong a_blinkDelay
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.blinkDelay = a_blinkDelay;
}
/***************************************************************************/
static void StartBlink (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (Class.blinkTimeOut==0L) && (ONodeGetNumberOfBlinks()>0) ) 
    {
      DoBlink (NULL,NULL);
    }
}
/***************************************************************************/
static void StopBlink (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(ONodeGetNumberOfBlinks()<=0) /*nothing to blink: stop blinkTimeOut*/
   {
     ONodeSetNumberOfBlinksToZero ();
     if(Class.blinkTimeOut!=0L) 
       {
/*	 XtRemoveTimeOut    (Class.blinkTimeOut);*/
         Class.blinkTimeOut = 0L;
       }
   }
}
/***************************************************************************/
static void DoBlink (
 void* a_pointer
,void* a_ident 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/*GoObjectDoRefresh  ();*/
/*  Class.blinkTimeOut = XtAddTimeOut (Class.blinkDelay,DoBlink,NULL);*/
  Class.blinkTimeOut = 0L;
  a_pointer          = NULL; /*avoid C++ warning*/
  a_ident            = NULL; /*avoid C++ warning*/
}
/***************************************************************************/
static int GetExtension (
 ONode* This
,double* a_xmin   
,double* a_xmax   
,double* a_ymin   
,double* a_ymax   
,double* a_zmin   
,double* a_zmax   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode* itema;
  int      first;
/*.........................................................................*/
  if(This==NULL)   return 0;
  if(a_xmin==NULL) return 0;
  if(a_xmax==NULL) return 0;
  if(a_ymin==NULL) return 0;
  if(a_ymax==NULL) return 0;
  if(a_zmin==NULL) return 0;
  if(a_zmax==NULL) return 0;
/* get wc size */
  first = 0;
  for(itema=This;(itema!=NULL) && (*itema!=NULL);itema++)
    { double xmin,xmax,ymin,ymax,zmin,zmax;
      if( (ONodeGetVisibility(*itema)==1) && 
	  (ONodeGetSize (*itema,&xmin,&xmax,&ymin,&ymax,&zmin,&zmax)==1) )
	 {
	    if(first==0)
              {
		*a_xmin = xmin;
                *a_ymin = ymin;
                *a_zmin = zmin;

		*a_xmax = xmax;
                *a_ymax = ymax;
                *a_zmax = zmax;
	      }
	    else
	      {
		*a_xmax = MAXIMUM(*a_xmax,xmax);
		*a_ymax = MAXIMUM(*a_ymax,ymax);
		*a_zmax = MAXIMUM(*a_zmax,zmax);
	    
		*a_xmin = MINIMUM(*a_xmin,xmin);
		*a_ymin = MINIMUM(*a_ymin,ymin);
		*a_zmin = MINIMUM(*a_zmin,zmin);
	      }
            first = 1;
	  }
    }
  return first; /* no point */
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
  CErrorHandle ("Go","OCamera",a_name,a_code);
}
/***************************************************************************/
int OCameraGetAttribute (
 OCamera  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

  if(This==NULL)
    {
       if(strcmp(a_name,"current")==0) 
	 {
	   OCamera           camera;
	   camera            = OCameraGetCurrent();
	   if(camera==NULL)  return 0;
	   *((char**)a_addr) = CStringCreateF   (8+64,"OCamera/%lu",camera);
	   return            FREE_BLOCK;
	 }
       else 
	 {
	   CInfoF("OCameraGetAttribute: %s is not a class property.\n",a_name);
	   return 0;
	 }
    }

       if(strcmp(a_name,"identifier")==0)       *((Ulong*)a_addr)    = (Ulong)This;
  else if(strcmp(a_name,"name")==0)             *((char**)a_addr)    = This->name;
  else if(strcmp(a_name,"colormap")==0)         *((char**)a_addr)    = OColormapGetName(This->ocmap);
  else if(strcmp(a_name,"width")==0)            *((int*)a_addr)      = This->width;
  else if(strcmp(a_name,"height")==0)           *((int*)a_addr)      = This->height;
  else if(strcmp(a_name,"backgroundColor")==0)  
    {
      *((OString*)a_addr)  = GetColorName(This,This->backgroundColor);
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"highlightContext")==0) *((OString*)a_addr)  = This->highlightContext;
  else if(strcmp(a_name,"edgeColor")==0) 
    {
      *((OString*)a_addr)  = GetColorName(This,This->edgeColor);
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"buffering")==0)        
    *((OString*)a_addr)  = OBufferingGetName   (OCameraGetBuffering(This));
  else if(strcmp(a_name,"depthTreatment")==0)   
    *((OString*)a_addr)  = ODepthTreatmentGetName (OCameraGetDepthTreatment(This));
  else if(strcmp(a_name,"projection")==0)       
    *((OString*)a_addr)  = OProjectionGetName  (OCameraGetProjection(This));
  else if(strcmp(a_name,"projectionAngle")==0)  
    *((double*)a_addr)   = 180. * OCameraGetProjectionAngle (This) / M_PI;
  else if(strcmp(a_name,"viewLimits")==0) 
    {
      int           number = 0;
      double*       doubles;
      doubles       = (double*)CMemoryAllocateBlock( 2 * sizeof(double));
      if(doubles!=NULL)
	{
	  number           = 2;
	  doubles[0] = This->fieldMin;
	  doubles[1] = This->fieldMax;
	}
      if(a_number!=NULL) *a_number = number;
      *((double**)a_addr) = doubles;
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"viewPoint")==0) 
    {
      int     number = 0;
      double* doubles;
      doubles = (double*)CMemoryAllocateBlock( 3 * sizeof(double));
      if(doubles!=NULL)
	{
	  number           = 3;
	  doubles[0] = This->viewVrp_x;
	  doubles[1] = This->viewVrp_y;
	  doubles[2] = This->viewVrp_z;
	}
      if(a_number!=NULL) *a_number = number;
      *((double**)a_addr) = doubles;
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"upVector")==0) 
    {
      int     number = 0;
      double* doubles;
      doubles = (double*)CMemoryAllocateBlock( 3 * sizeof(double));
      if(doubles!=NULL)
	{
	  number           = 3;
	  doubles[0] = This->viewUp_x;
	  doubles[1] = This->viewUp_y;
	  doubles[2] = This->viewUp_z;
	}
      if(a_number!=NULL) *a_number = number;
      *((double**)a_addr) = doubles;
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"specialProjection")==0) 
    *((OString*)a_addr)  = OSpecialProjectionGetName  (OCameraGetSpecialProjection(This));
  else if(strcmp(a_name,"xcompress")==0) 
    {
      *((double*)a_addr) = This->specialProjection.xcompress;
    }
  else if(strcmp(a_name,"fisheye_a")==0) 
    {
      *((double*)a_addr) = This->specialProjection.fisheye_a;
    }
  else if(strcmp(a_name,"rhoz_phi1")==0) 
    {
      *((double*)a_addr) = This->specialProjection.rhoz_phi1;
    }
  else if(strcmp(a_name,"v_rho_max")==0) 
    {
      *((double*)a_addr) = This->specialProjection.v_rho_max;
    }
  else if(strcmp(a_name,"v_z_max")==0) 
    {
      *((double*)a_addr) = This->specialProjection.v_z_max  ;
    }
  else if(strcmp(a_name,"v_k")==0) 
    {
      *((double*)a_addr) = This->specialProjection.v_k      ;
    }
  else 
    {
      CInfoF("OCameraGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OCameraSetAttribute (
 OCamera  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

  if(This==NULL)
    {
       if(strcmp(a_name,"current")==0) 
	 {
	   OCamera ocamera;
	   ocamera = OCameraGetIdentifier(*((OString*)a_addr));
	   if(ocamera!=NULL)
	     {
	       OCameraSetCurrent (ocamera);
	     }
	   else 
	     {
	       CWarnF("OCameraSetAttribute: Unknown OCamera %s.\n",*((OString*)a_addr));
	       return 0;
	     }
	 }
       else 
	 {
	   CInfoF ("OCameraSetAttribute: %s is not a class property.\n",a_name);
	   return 0;
	 }
       return 1;
    }

       if(strcmp(a_name,"identifier")==0)      
	 {}
  else if(strcmp(a_name,"name")==0)            
    OCameraSetName                (This,*((OString*)a_addr));
  else if(strcmp(a_name,"colormap")==0)        
    OCameraSetColormap            (This,OColormapGetIdentifier(*((OString*)a_addr)));
  else if(strcmp(a_name,"width")==0)           
    OCameraChangeSize             (This,*((int*)a_addr),0);
  else if(strcmp(a_name,"height")==0)          
    OCameraChangeSize             (This,0,*((int*)a_addr));
  else if(strcmp(a_name,"backgroundColor")==0) 
    OCameraSetBackgroundColorIndex(This,OColorGetIdentifier(*((OString*)a_addr)));
  else if(strcmp(a_name,"highlightContext")==0)
    OCameraSetHighlightContext    (This,*((OString*)a_addr));
  else if(strcmp(a_name,"edgeColor")==0)       
    OCameraSetEdgeColorIndex      (This,OColorGetIdentifier(*((OString*)a_addr)));
  else if(strcmp(a_name,"buffering")==0)       
    OCameraSetBuffering           (This,OBufferingGetIdentifier  (*((OString*)a_addr)));
  else if(strcmp(a_name,"depthTreatment")==0)  
    OCameraSetDepthTreatment      (This,ODepthTreatmentGetIdentifier(*((OString*)a_addr)));
  else if(strcmp(a_name,"projection")==0)      
    OCameraSetProjection          (This,OProjectionGetIdentifier (*((OString*)a_addr)));
  else if(strcmp(a_name,"projectionAngle")==0) 
    OCameraSetProjectionAngle     (This,M_PI * (*((double*)a_addr))/180.);
  else if(strcmp(a_name,"viewLimits")==0)
    {
      if(a_number!=2)
	{
	  CWarnF ("OCameraSetAttribute:For property %s of obj %s 2 values are requested.\n",a_name,CameraName(This));
	  return 0;
	}
      else
	{
	  double*         list;
	  list            = *((double**)a_addr);
          OCameraSetField (This,list[0],list[1]);
	}
    }
  else if(strcmp(a_name,"viewPoint")==0) 
    {
      if(a_number!=3)
	{
	  CWarnF("OCameraSetAttribute:For property %s of obj %s 3 values are requested.\n",a_name,CameraName(This));
	  return 0;
	}
      else
	{
	  double*             list;
	  list                = *((double**)a_addr);
	  OCameraSetDefaultVRP (This,list[0],list[1],list[2]);
	}
    }
  else if(strcmp(a_name,"upVector")==0) 
    {
      if(a_number!=3)
	{
	  CWarnF("OCameraSetAttribute:For property %s of obj %s 3 values are requested.\n",a_name,CameraName(This));
	  return 0;
	}
      else
	{
	  double*                   list;
	  list                      = *((double**)a_addr);
	  OCameraSetDefaultUpVector (This,list[0],list[1],list[2]);
	}
    }
  else if(strcmp(a_name,"specialProjection")==0) 
    OCameraSetSpecialProjection (This,OSpecialProjectionGetIdentifier (*((OString*)a_addr)));
  else if(strcmp(a_name,"xcompress")==0) 
    {
      This->specialProjection.xcompress = *((double*)a_addr);
      ONodeRequireViewRefreshment       (This);
    }
  else if(strcmp(a_name,"fisheye_a")==0) 
    {
      This->specialProjection.fisheye_a = *((double*)a_addr);
      ONodeRequireViewRefreshment       (This);
    }
  else if(strcmp(a_name,"rhoz_phi1")==0) 
    {
      This->specialProjection.rhoz_phi1 = *((double*)a_addr);
      ONodeRequireViewRefreshment       (This);
    }
  else if(strcmp(a_name,"v_rho_max")==0) 
    {
      This->specialProjection.v_rho_max = *((double*)a_addr);
      ONodeRequireViewRefreshment       (This);
    }
  else if(strcmp(a_name,"v_z_max")==0) 
    {
      This->specialProjection.v_z_max   = *((double*)a_addr);
      ONodeRequireViewRefreshment       (This);
    }
  else if(strcmp(a_name,"v_k")==0) 
    {
      This->specialProjection.v_k       = *((double*)a_addr);
      ONodeRequireViewRefreshment       (This);
    }
  else 
    {
      CInfoF ("OCameraSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static struct {
  char*      string;
  OBuffering value;
} OBufferingTypeList [] = {
  {"simple",     OBufferingSimple},
  {"double",     OBufferingDouble}
};  
static struct {
  char*       string;
  OProjection value;
} OProjectionTypeList [] = {
  {"parallel",     OProjectionParallel},
  {"perspective",  OProjectionPerspective}
};  
static struct {
  char*           string;
  ODepthTreatment value;
} ODepthTreatmentList [] = {
  {"none",          ODepthTreatmentNone},
  {"zbuffer",       ODepthTreatmentZBuffer}
};  
static struct {
  char*              string;
  OSpecialProjection value;
} OSpecialProjectionTypeList [] = {
  {"xyz",      OSpecialProjectionXYZ},
  {"rz",       OSpecialProjectionRZ},
  {"phiz",     OSpecialProjectionPHIZ},
  {"rhoz",     OSpecialProjectionRHOZ},
  {"thetaphi", OSpecialProjectionTHETAPHI},
  {"phitheta", OSpecialProjectionPHITHETA},
  {"phir",     OSpecialProjectionPHIR},
  {"phirho",   OSpecialProjectionPHIRHO},
  {"vplot",    OSpecialProjectionVPLOT},
  {"fisheye",  OSpecialProjectionFISHEYE}
};  
/***************************************************************************/
char** OBufferingGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* buffering[2] = {"simple","double"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 2;
  return buffering;
}
/***************************************************************************/
OBuffering OBufferingGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return OBufferingSimple;
  for(count=0;count<ONumber(OBufferingTypeList);count++)
    if(strcmp(a_string,OBufferingTypeList[count].string)==0) 
      return OBufferingTypeList[count].value;
  CWarnF("%s not an OBuffering.\n",a_string);
  return OBufferingSimple;
}
/***************************************************************************/
char* OBufferingGetName (
 OBuffering a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OBufferingTypeList);count++)
    if(a_value==OBufferingTypeList[count].value) return OBufferingTypeList[count].string;
  CWarnF("%d not an OBuffering.\n",a_value);
  return NULL;
}
/***************************************************************************/
char** OProjectionGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* projection[2] = {"parallel","perspective"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 2;
  return projection;
}
/***************************************************************************/
OProjection OProjectionGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0')) return OProjectionParallel;
  for(count=0;count<ONumber(OProjectionTypeList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OProjectionTypeList[count].string)==0)) 
      return OProjectionTypeList[count].value;
  CWarnF("%s not an OProjection.\n",a_string);
  return OProjectionParallel;
}
/***************************************************************************/
char* OProjectionGetName (
 OProjection a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OProjectionTypeList);count++)
    if(a_value==OProjectionTypeList[count].value) return OProjectionTypeList[count].string;
  CWarnF("%d not an OProjection.\n",a_value);
  return NULL;
}
/***************************************************************************/
char** ODepthTreatmentGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* depthTest[2] = {"none","zbuffer"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 2;
  return depthTest;
}
/***************************************************************************/
ODepthTreatment ODepthTreatmentGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0')) return ODepthTreatmentNone;
  for(count=0;count<ONumber(ODepthTreatmentList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,ODepthTreatmentList[count].string)==0)) 
      return ODepthTreatmentList[count].value;
  CWarnF("%s not an ODepthTreatment.\n",a_string);
  return ODepthTreatmentNone;
}
/***************************************************************************/
char* ODepthTreatmentGetName (
 ODepthTreatment a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(ODepthTreatmentList);count++)
    if(a_value==ODepthTreatmentList[count].value) return ODepthTreatmentList[count].string;
  CWarnF("%d not an ODepthTreatment.\n",a_value);
  return NULL;
}
/***************************************************************************/
char** OSpecialProjectionGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* specialProjection[10] = 
   {"xyz","rz","phiz","rhoz","thetaphi","phitheta","phir","phirho","vplot","fisheye"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 10;
  return specialProjection;
}
/***************************************************************************/
OSpecialProjection OSpecialProjectionGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0')) return OSpecialProjectionXYZ;
  for(count=0;count<ONumber(OSpecialProjectionTypeList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OSpecialProjectionTypeList[count].string)==0)) 
      return OSpecialProjectionTypeList[count].value;
  CWarnF("%s not an OSpecialProjection.\n",a_string);
  return OSpecialProjectionXYZ;
}
/***************************************************************************/
char* OSpecialProjectionGetName (
 OSpecialProjection a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OSpecialProjectionTypeList);count++)
    if(a_value==OSpecialProjectionTypeList[count].value) return OSpecialProjectionTypeList[count].string;
  CWarnF("%d not an OSpecialProjection.\n",a_value);
  return NULL;
}
/***************************************************************************/
static char* GetColorName (
 OCamera  This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(CStringCompare(OColormapGetName(This->ocmap),"ocolormap_X")==1)
    {
      return CStringDuplicate(OColorGetName((OColor)a_value));
    }
  else
    {
      return CStringCreateF (16,"%d",a_value);
    }				     
}
/***************************************************************************/
int OCameraDoDefaultSpecialProjection (
 OCamera  This
,ONode    a_node
,int      a_number
,double*  a_xs
,double*  a_ys
,double*  a_zs
,double** a_pxs
,double** a_pys
,double** a_pzs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_pxs!=NULL) *a_pxs = NULL;
  if(a_pys!=NULL) *a_pys = NULL;
  if(a_pzs!=NULL) *a_pzs = NULL;
  if(This==NULL)  return 0;
  if(a_number<=0) return 0;
  if( (a_xs==NULL) || (a_ys==NULL) || (a_zs==NULL) )  return 0;
  if( (This->specialProjection.specialProjection==OSpecialProjectionRZ) && 
      (ONodeGetProjected(a_node)==1) )
    {
      if(AllocatePoints(a_number)==0) return 0;
      for (count=0;count<a_number;count++) 
	{
	  double x,y;
	  x      = a_xs[count];
	  y      = a_ys[count];
	  Class.xs[count] = sqrt ( x * x + y * y);
	  Class.ys[count] = a_zs[count];
	  Class.zs[count] = 0.;
	}
      if(a_pxs!=NULL) *a_pxs = Class.xs;
      if(a_pys!=NULL) *a_pys = Class.ys;
      if(a_pzs!=NULL) *a_pzs = Class.zs;
      return          a_number;
    }
  else if( (This->specialProjection.specialProjection==OSpecialProjectionPHIZ) && 
	   (ONodeGetProjected(a_node)==1) )
    {
      /*
      if(AllocatePoints(a_number)==0) return 0;
      for (count=0;count<a_number;count++) 
	{
	  double x,y;
	  x      = a_xs[count];
	  y      = a_ys[count];
	  Class.xs[count] = CMathGetArcTangent (y,x);
	  Class.ys[count] = a_zs[count];
	  Class.zs[count] = 0.;
	}
	*/
      double v_left,v_right,v_top,v_bottom, scale; 
      if(AllocatePoints(a_number)==0) return 0;
      OCameraGetViewLimits(This,&v_left,&v_right,&v_top,&v_bottom);
      scale = 0.9 * (v_top - v_bottom) / M_PI / 2; 
      for (count=0;count<a_number;count++)
        {
          double x,y;
          x      = a_xs[count];
          y      = a_ys[count];
	  Class.ys[count] = CMathGetArcTangent (y,x) * scale;
          Class.xs[count] = a_zs[count] / This->specialProjection.xcompress;
          Class.zs[count] = 0.;
        }
      if(a_pxs!=NULL) *a_pxs = Class.xs;
      if(a_pys!=NULL) *a_pys = Class.ys;
      if(a_pzs!=NULL) *a_pzs = Class.zs;
      return          a_number;
    }
  else if( (This->specialProjection.specialProjection==OSpecialProjectionRHOZ) && 
	   (ONodeGetProjected(a_node)==1) )
    {
      /*
      if(AllocatePoints(a_number)==0) return 0;
      for (count=0;count<a_number;count++) 
	{
	  double x,y;
	  x      = a_xs[count];
	  y      = a_ys[count];
	  Class.xs[count] = a_zs[count];
	  Class.ys[count] = sqrt ( x * x + y * y);
	  if (y < 0.0)	  Class.ys[count] *= -1.0;
	  Class.zs[count] = 0.;
	}
      */
      double   phi1from, phi1to;
      phi1from = This->specialProjection.rhoz_phi1 / 180. * M_PI;
      phi1to   = phi1from + M_PI;
      if(AllocatePoints(a_number)==0) return 0;
      for (count=0;count<a_number;count++)
        {
          double r,x,y,phi;
          x      = a_xs[count];
          y      = a_ys[count];
	  r      = sqrt ( x * x + y * y);
	  Class.ys[count] = r / (1 +  This->specialProjection.fisheye_a * r);
          Class.xs[count] = a_zs[count] / This->specialProjection.xcompress;
	  phi    = CMathGetArcTangent (y,x);
	  if(phi < 0) phi = M_PI - phi;
	  if(phi < phi1from || phi >= phi1to) Class.ys[count] *= -1;
          Class.zs[count] = 0.;
        }
      if(a_pxs!=NULL) *a_pxs = Class.xs;
      if(a_pys!=NULL) *a_pys = Class.ys;
      if(a_pzs!=NULL) *a_pzs = Class.zs;
      return          a_number;
    }
  else if( (This->specialProjection.specialProjection==OSpecialProjectionTHETAPHI) && 
	   (ONodeGetProjected(a_node)==1) )
    {
      if(AllocatePoints(a_number)==0) return 0;
      for (count=0;count<a_number;count++) 
	{
	  double x,y,z;
	  x      = a_xs[count];
	  y      = a_ys[count];
	  z      = a_zs[count];
	  Class.xs[count] = CMathGetArcTangent (y, sqrt( x * x + z * z));
	  Class.ys[count] = CMathGetArcTangent (y,x);
	  Class.zs[count] = 0.;
	}
      if(a_pxs!=NULL) *a_pxs = Class.xs;
      if(a_pys!=NULL) *a_pys = Class.ys;
      if(a_pzs!=NULL) *a_pzs = Class.zs;
      return          a_number;
    }
  else if( (This->specialProjection.specialProjection==OSpecialProjectionPHITHETA) && 
	   (ONodeGetProjected(a_node)==1) )
    {
      double r_scale, z_scale; 
      if(AllocatePoints(a_number)==0) return 0;
      r_scale = 1. + This->specialProjection.v_rho_max * This->specialProjection.fisheye_a;
      z_scale = 1. + This->specialProjection.v_z_max   * This->specialProjection.fisheye_a;
      for (count=0;count<a_number;count++)
        {
          double x,y,z,r,phi;
          x      = a_xs[count];
          y      = a_ys[count];
          z      = a_zs[count];
	  if (This->specialProjection.fisheye_a > 0.0)
	    {
	      r   = sqrt ( x * x + y * y);
	      r  /= 1 + This->specialProjection.fisheye_a * r;
	      r  *= r_scale;
	      phi = CMathGetArcTangent (y,x);
	      Class.xs[count] = r * sin(phi) / This->specialProjection.xcompress;
	      Class.ys[count] = r * cos(phi);
	      Class.zs[count] = z / (1 +  This->specialProjection.fisheye_a * z) * z_scale;
	    }
	  else
	    {
	      Class.xs[count] = x / This->specialProjection.xcompress;
	      Class.ys[count] = y;
	    }
	  Class.zs[count] = z;
        }
      if(a_pxs!=NULL) *a_pxs = Class.xs;
      if(a_pys!=NULL) *a_pys = Class.ys;
      if(a_pzs!=NULL) *a_pzs = Class.zs;
      return          a_number;
    }
  else if( (This->specialProjection.specialProjection==OSpecialProjectionPHIR) && 
	   (ONodeGetProjected(a_node)==1) )
    {
      double v_left,v_right,v_top,v_bottom, scale; 
      if(AllocatePoints(a_number)==0) return 0;
      OCameraGetViewLimits(This,&v_left,&v_right,&v_top,&v_bottom);
      scale = 0.9 * (v_top - v_bottom) / M_PI / 2; 
      for (count=0;count<a_number;count++)
        {
          double x,y,z;
          x      = a_xs[count];
          y      = a_ys[count];
          z      = a_zs[count];
	  Class.ys[count] = CMathGetArcTangent (y,x) * scale;
          Class.xs[count] = sqrt ( x*x + y*y + z*z ) / This->specialProjection.xcompress;
          Class.zs[count] = 0.;
        }
      if(a_pxs!=NULL) *a_pxs = Class.xs;
      if(a_pys!=NULL) *a_pys = Class.ys;
      if(a_pzs!=NULL) *a_pzs = Class.zs;
      return          a_number;
    }
  else if( (This->specialProjection.specialProjection==OSpecialProjectionPHIRHO) && 
	   (ONodeGetProjected(a_node)==1) )
    {
      double v_left,v_right,v_top,v_bottom, scale; 
      if(AllocatePoints(a_number)==0) return 0;
      OCameraGetViewLimits(This,&v_left,&v_right,&v_top,&v_bottom);
      scale = 0.9 * (v_top - v_bottom) / M_PI / 2; 
      for (count=0;count<a_number;count++)
        {
          double x,y;
          x      = a_xs[count];
          y      = a_ys[count];
	  Class.ys[count] = CMathGetArcTangent (y,x) * scale;
          Class.xs[count] = sqrt (x * x + y * y) / This->specialProjection.xcompress;
          Class.zs[count] = 0.;
        }
      if(a_pxs!=NULL) *a_pxs = Class.xs;
      if(a_pys!=NULL) *a_pys = Class.ys;
      if(a_pzs!=NULL) *a_pzs = Class.zs;
      return          a_number;
    }
  else if( (This->specialProjection.specialProjection==OSpecialProjectionVPLOT) && 
	   (ONodeGetProjected(a_node)==1) )
    {
      double v_left,v_right,v_top,v_bottom, scale; 

      if(AllocatePoints(2*a_number)==0) return 0;
      OCameraGetViewLimits(This,&v_left,&v_right,&v_top,&v_bottom);
      scale = 0.9 * (v_top - v_bottom) / M_PI / 2; 
      for (count=0;count<a_number;count++)
        {
          double r,x,y,z,phi,theta,rmax,d_k;
          x      = a_xs[count];
          y      = a_ys[count];
          z      = a_zs[count];
	  r      = sqrt (x * x + y * y);
	  phi    = CMathGetArcTangent (y,x);
	  theta  = CMathGetArcTangent (r,z);
	  rmax   = MINIMUM(This->specialProjection.v_rho_max/sin(theta),
			   This->specialProjection.v_z_max/fabs(cos(theta)));
	  d_k    = This->specialProjection.v_k * (rmax - r);
          Class.xs[count]          = (theta + d_k) * scale;
	  Class.ys[count]          = phi           * scale;
          Class.zs[count]          = 0.;
          Class.xs[count+a_number] = (theta - d_k) * scale; 
	  Class.ys[count+a_number] = Class.ys[count]; 
	  Class.zs[count+a_number] = Class.zs[count]; 
        }
      if(a_pxs!=NULL) *a_pxs = Class.xs;
      if(a_pys!=NULL) *a_pys = Class.ys;
      if(a_pzs!=NULL) *a_pzs = Class.zs;
      return          2*a_number;
    }
  else if( (This->specialProjection.specialProjection==OSpecialProjectionFISHEYE) && 
	   (ONodeGetProjected(a_node)==1) )
    {
      double r_scale, z_scale; 
      if(AllocatePoints(a_number)==0) return 0;
      r_scale = 1. + This->specialProjection.v_rho_max * This->specialProjection.fisheye_a;
      z_scale = 1. + This->specialProjection.v_z_max   * This->specialProjection.fisheye_a;
      for (count=0;count<a_number;count++)
        {
          double x,y,z,r,phi;
          x      = a_xs[count];
          y      = a_ys[count];
          z      = a_zs[count];
	  if (This->specialProjection.fisheye_a > 0.0)
	    {
	      r   = sqrt ( x * x + y * y);
	      r  /= 1 + This->specialProjection.fisheye_a * r;
	      r  *= r_scale;
	      phi = CMathGetArcTangent (y,x);
	      Class.xs[count] = r * sin(phi) / This->specialProjection.xcompress;
	      Class.ys[count] = r * cos(phi);
	      Class.zs[count] = z / (1 +  This->specialProjection.fisheye_a * z) * z_scale;
	    }
	  else
	    {
	      Class.xs[count] = x / This->specialProjection.xcompress;
	      Class.ys[count] = y;
	    }
	  Class.zs[count] = z;
        }
      if(a_pxs!=NULL) *a_pxs = Class.xs;
      if(a_pys!=NULL) *a_pys = Class.ys;
      if(a_pzs!=NULL) *a_pzs = Class.zs;
      return          a_number;
    }
  else /*Do nothing.*/
    {
      if(a_pxs!=NULL) *a_pxs = a_xs;
      if(a_pys!=NULL) *a_pys = a_ys;
      if(a_pzs!=NULL) *a_pzs = a_zs;
      return          a_number;
    }
}
/***************************************************************************/
static int AllocatePoints (
 int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number>Class.number)
    {
      CMemoryFreeBlock (Class.xs);
      CMemoryFreeBlock (Class.ys);
      CMemoryFreeBlock (Class.zs);
      Class.number     = 0;
      Class.xs         = (double*)CMemoryAllocateBlock(a_number*sizeof(double));
      Class.ys         = (double*)CMemoryAllocateBlock(a_number*sizeof(double));
      Class.zs         = (double*)CMemoryAllocateBlock(a_number*sizeof(double));
      if( (Class.xs==NULL) || (Class.ys==NULL) || (Class.zs==NULL) )
	{
	  CMemoryFreeBlock (Class.xs);
	  CMemoryFreeBlock (Class.ys);
	  CMemoryFreeBlock (Class.zs);
	  Class.xs         = NULL;
	  Class.ys         = NULL;
	  Class.zs         = NULL;
	  return           0;
	}
      Class.number      = a_number;
    }
  if( (Class.xs==NULL) || (Class.ys==NULL) || (Class.zs==NULL) )  return 0;
  return 1;
}
/***************************************************************************/
static void DrawMarker (
 OMarkerStyle a_style
,int a_width
)
/***************************************************************************/
/* 
  From Wolfgang Grandegger.
  It implements marker sizes from 1 to 7 (4x4, 6x6 ...
 16x16). I have created them with marker_bitmap.c.
 To center the marker around the central dot
 I left the first column and row free ! 
  Hope this is correct.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  switch(a_style)
    {
    case OMarkerStyleDot:
      /* 
      {static unsigned char bits[] = {
	0x80,0x00, 
	0x00,0x00,  
	0x00,0x00,  
	0x00,0x00,  
	0x00,0x00, 
	0x00,0x00,  
	0x00,0x00,  
	0x00,0x00,  
	0x00,0x00,  
	0x00,0x00,  
	0x00,0x00, 
	0x00,0x00,  
	0x00,0x00,  
	0x00,0x00,  
	0x00,0x00,  
	0x00,0x00
      };
      glBitmap (16,16,0.,0.,0.,0.,bits);
      }*/
      break;
    case OMarkerStylePlus:
      {static unsigned char bits[7][32] = {
	{0x02,0x72}
       ,{0x00,0x41,0x1f,0x10,0x40}
       ,{0x00,0x08,0x08,0x08,0x7f,0x08,0x08,0x08}
       ,{0x00,0x01,0x00,0x40,0x10,0x04,0x1f,0xf0,
	 0x40,0x10,0x04,0x01,0x00}
       ,{0x00,0x00,0x20,0x02,0x00,0x20,0x02,0x00,
	 0x20,0x7f,0xf0,0x20,0x02,0x00,0x20,0x02,
	 0x00,0x20}
       ,{0x00,0x00,0x04,0x00,0x10,0x00,0x40,0x01,
	 0x00,0x04,0x00,0x10,0x1f,0xff,0x01,0x00,
	 0x04,0x00,0x10,0x00,0x40,0x01,0x00,0x04,
	 0x00}
       ,{0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,
	 0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
	 0x7f,0xff,0x00,0x80,0x00,0x80,0x00,0x80,
	 0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80}
      };
      static int    bitn[7] = {4 ,6 ,8 ,10 ,12 ,14 ,16 };
      static double bito[7] = {2.,3.,4., 5., 6., 7., 8.};
      if(--a_width>6) a_width=6;
      glBitmap (bitn[a_width],bitn[a_width],
		bito[a_width],bito[a_width],
		0.,0.,bits[a_width]);
      }break;
    case OMarkerStyleCircle:
      {static unsigned char bits[7][32] = {
        {0x02,0x52}
       ,{0x00,0xe4,0x51,0x44,0xe0}
       ,{0x00,0x1c,0x22,0x41,0x41,0x41,0x22,0x1c}
       ,{0x00,0x03,0x83,0x18,0x82,0x40,0x50,0x14,
	 0x04,0x82,0x31,0x83,0x80}
       ,{0x00,0x00,0xf8,0x10,0x42,0x02,0x40,0x14,
	 0x01,0x40,0x14,0x01,0x40,0x12,0x02,0x10,
	 0x40,0xf8}
       ,{0x00,0x00,0x1f,0x00,0x82,0x04,0x04,0x20,
	 0x09,0x00,0x14,0x00,0x50,0x01,0x40,0x05,
	 0x00,0x12,0x00,0x84,0x04,0x08,0x20,0x1f,
	 0x00}
       ,{0x00,0x00,0x03,0xe0,0x0c,0x18,0x10,0x04,
	 0x20,0x02,0x20,0x02,0x40,0x01,0x40,0x01,
	 0x40,0x01,0x40,0x01,0x40,0x01,0x20,0x02,
	 0x20,0x02,0x10,0x04,0x0c,0x18,0x03,0xe0}
      };
      static int    bitn[7] = {4 ,6 ,8 ,10 ,12 ,14 ,16 };
      static double bito[7] = {2.,3.,4., 5., 6., 7., 8.};
      if(--a_width>6) a_width=6;
      glBitmap (bitn[a_width],bitn[a_width],
		bito[a_width],bito[a_width],
		0.,0.,bits[a_width]);
      }break;
    case OMarkerStyleAsterisk:
    case OMarkerStyleCross:
      {static unsigned char bits[7][32] = {
	{0x05,0x25}
       ,{0x01,0x12,0x84,0x29,0x10}
       ,{0x00,0x41,0x22,0x14,0x08,0x14,0x22,0x41}
       ,{0x00,0x10,0x12,0x08,0x44,0x0a,0x01,0x00,
	 0xa0,0x44,0x20,0x90,0x10}
       ,{0x00,0x04,0x01,0x20,0x21,0x04,0x08,0x80,
	 0x50,0x02,0x00,0x50,0x08,0x81,0x04,0x20,
	 0x24,0x01}
       ,{0x00,0x01,0x00,0x12,0x00,0x84,0x04,0x08,
	 0x20,0x11,0x00,0x28,0x00,0x40,0x02,0x80,
	 0x11,0x00,0x82,0x04,0x04,0x20,0x09,0x00,
	 0x10}
       ,{0x00,0x00,0x40,0x01,0x20,0x02,0x10,0x04,
	 0x08,0x08,0x04,0x10,0x02,0x20,0x01,0x40,
	 0x00,0x80,0x01,0x40,0x02,0x20,0x04,0x10,
	 0x08,0x08,0x10,0x04,0x20,0x02,0x40,0x01}
      };
      static int    bitn[7] = {4 ,6 ,8 ,10 ,12 ,14 ,16 };
      static double bito[7] = {2.,3.,4., 5., 6., 7., 8.};
      if(--a_width>6) a_width=6;
      glBitmap (bitn[a_width],bitn[a_width],
		bito[a_width],bito[a_width],
		0.,0.,bits[a_width]);
      }break;
    case OMarkerStyleStar:
      {static unsigned char bits[7][32] = {
	{0x07,0x77}
       ,{0x01,0x53,0x9f,0x39,0x50}
       ,{0x00,0x49,0x2a,0x1c,0x7f,0x1c,0x2a,0x49}
       ,{0x00,0x11,0x12,0x48,0x54,0x0e,0x1f,0xf0,
	 0xe0,0x54,0x24,0x91,0x10}
       ,{0x00,0x04,0x21,0x22,0x21,0x24,0x0a,0x80,
	 0x70,0x7f,0xf0,0x70,0x0a,0x81,0x24,0x22,
	 0x24,0x21}
       ,{0x00,0x01,0x04,0x12,0x10,0x84,0x44,0x09,
	 0x20,0x15,0x00,0x38,0x1f,0xff,0x03,0x80,
	 0x15,0x00,0x92,0x04,0x44,0x21,0x09,0x04,
	 0x10}
       ,{0x00,0x00,0x40,0x81,0x20,0x82,0x10,0x84,
	 0x08,0x88,0x04,0x90,0x02,0xa0,0x01,0xc0,
	 0x7f,0xff,0x01,0xc0,0x02,0xa0,0x04,0x90,
	 0x08,0x88,0x10,0x84,0x20,0x82,0x40,0x81}
      };
      static int    bitn[7] = {4 ,6 ,8 ,10 ,12 ,14 ,16 };
      static double bito[7] = {2.,3.,4., 5., 6., 7., 8.};
      if(--a_width>6) a_width=6;
      glBitmap (bitn[a_width],bitn[a_width],
		bito[a_width],bito[a_width],
		0.,0.,bits[a_width]);
      }break;
    }
}
/***************************************************************************/
/***************************************************************************/
/******** OCamera PS *******************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OCameraProducePS_File (
 OCamera This
,char*   a_fname
)
/***************************************************************************/
/*
  Produce a PostScript file.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OCameraOpenPS_FileForWriting   (a_fname);
  OCameraPutScenePageInPS_Stream (This);
  OCameraClosePS_Stream          ();
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OCameraOpenPS_FileForWriting (
 char*   a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_OpenFileForWriting (a_fname);
}
/***************************************************************************/
void OCameraClosePS_Stream (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPS_CloseStream  ();
  CPS_ClearClass   ();
}
/***************************************************************************/
void OCameraPutScenePageInPS_Stream (
 OCamera This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int                        width,height;
/*.........................................................................*/
  OCameraGetSize             (This,&width,&height);
  if(width <=0)              width  = 100;
  if(height<=0)              height = 100;
  CPS_PutBeginPageInStream   ();
  CPS_PutPageScaleInStream   ((double)width,(double)height);
  OCameraPutSceneInPS_Stream (This);
  CPS_PutEndPageInStream     ();
}
/***************************************************************************/
void OCameraPutSceneInPS_Stream (
 OCamera This
)
/***************************************************************************/
/*
  ZBuffer not treated here. Done at the XoCamera level by retreiving 
 window pixmap.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CPS_PutSaveStateInStream    ();
  Traverse                    (This,TRAVERSE_PS,0,0,0,0);
  CPS_PutRestoreStateInStream ();
}
/***************************************************************************/
static int GetRGB (
 int a_col
,int a_row
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  float* itema;
/*.........................................................................*/
  itema      = Class.items + 3 * ((Class.height - 1 - a_row ) * Class.width + a_col);
  *a_red     = (double)*itema;itema++;
  *a_green   = (double)*itema;itema++;
  *a_blue    = (double)*itema;itema++;
  return     1;
}
/***************************************************************************/
/***************************************************************************/
#ifdef HAS_GL
/***************************************************************************/
#define PS_STYLE(a_style)      {switch(a_style)     {case OLineStyleSolid      :CPS_PutInStreamF("ss ") ;break;\
                                                     case OLineStyleDashed     :CPS_PutInStreamF("sd ") ;break;\
                                                     case OLineStyleDotted     :CPS_PutInStreamF("so ") ;break;\
                                                     case OLineStyleDashDotted :CPS_PutInStreamF("sdo ");break;}}
#define PS_MOVE(a_x,a_y)       CPS_PutInStreamF("%.2f %.2f m ",a_x,a_y)
#define PS_WIDTH(a_x)          CPS_PutLineWidthInStream(a_x)
#define PS_CAP(a_x)            CPS_PutInStreamF("%1d lc ",a_x)
#define PS_RLINETO(a_x,a_y)    CPS_PutInStreamF("%.2f %.2f rl ",a_x,a_y)
#define PS_RMOVE(a_x,a_y)      CPS_PutInStreamF("%.2f %.2f rm ",a_x,a_y)
#define PS_STROKE              CPS_PutInStreamF("s ")
#define PS_CLOSEPATH           CPS_PutInStreamF("cl ")
#define PS_NEWPATH             CPS_PutInStreamF("n ")
#define PS_FILL                CPS_PutInStreamF("f ")
#define PS_CIRCLE(a_x,a_y,a_r) CPS_PutInStreamF("%.2f %.2f %.2f 0 360 arc s ",a_x,a_y,a_r)
#define MAX_PATH_POINT 100
/***************************************************************************/
static void PS_SetColor (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLdouble            rgba[4];
/*.........................................................................*/
  glGetDoublev        (GL_CURRENT_COLOR,rgba);
  CPS_PutRGB_InStream (rgba[0],rgba[1],rgba[2]);
}
/***************************************************************************/
static void PS_DrawPolygon (
 int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,OAreaStyle    a_style
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLdouble   model[16],proj[16];
  GLint      viewport[4];
  double     x,y,z;
   int count;
  double     ox,oy;
  double     fx,fy;
/*.........................................................................*/
  glGetDoublev     (GL_MODELVIEW_MATRIX,model);
  glGetDoublev     (GL_PROJECTION_MATRIX,proj);
  glGetIntegerv    (GL_VIEWPORT,viewport);
  PS_NEWPATH; 
  gluProject       (a_xs[0],a_ys[0],a_zs[0],model,proj,viewport,&x,&y,&z);
  PS_MOVE          (x,y);
  ox               = x;
  oy               = y;
  fx               = x;
  fy               = y;
  for (count=1;count<a_pointn;count++)
    {
      gluProject (a_xs[count],a_ys[count],a_zs[count],model,proj,viewport,&x,&y,&z);
      PS_RLINETO (x - ox,y - oy);
      ox         = x;
      oy         = y;
    }
  if ((x==fx)&&(y==fy)) PS_CLOSEPATH;
  PS_SetColor ();
  PS_FILL;
  a_style = OAreaStyleSolid;
}
/***************************************************************************/
static void PS_DrawLines (
 int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,OLineStyle    a_style
,int           a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLdouble      model[16],proj[16];
  GLint         viewport[4];
  double        x,y,z;
   int count;
  double        ox,oy;
  double        fx,fy;
/*.........................................................................*/
  glGetDoublev     (GL_MODELVIEW_MATRIX,model);
  glGetDoublev     (GL_PROJECTION_MATRIX,proj);
  glGetIntegerv    (GL_VIEWPORT,viewport);
  gluProject       (a_xs[0],a_ys[0],a_zs[0],model,proj,viewport,&x,&y,&z);
  PS_MOVE          (x,y);
  ox               = x;
  oy               = y;
  fx               = x;
  fy               = y;
  for (count=1;count<a_pointn;count++)
    {  
      gluProject (a_xs[count],a_ys[count],a_zs[count],model,proj,viewport,&x,&y,&z);
      PS_RLINETO (x - ox,y - oy);
      ox         = x;
      oy         = y;
    }
  if ((x==fx)&&(y==fy)) PS_CLOSEPATH;
  PS_SetColor ();
  PS_WIDTH    (a_width);
  PS_CAP      (1);
  PS_STYLE    (a_style)
  PS_STROKE;
}
/***************************************************************************/
static void PS_DrawSegments (
 int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,OLineStyle    a_style
,int           a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLdouble   model[16],proj[16];
  GLint      viewport[4];
   int count;
  int        segment_number;
  double     x,y,z;
  double     ox,oy;
/*.........................................................................*/
  glGetDoublev     (GL_MODELVIEW_MATRIX,model);
  glGetDoublev     (GL_PROJECTION_MATRIX,proj);
  glGetIntegerv    (GL_VIEWPORT,viewport);

  segment_number = a_pointn/2;
  if(segment_number<=0) return;

  gluProject       (a_xs[0],a_ys[0],a_zs[0],model,proj,viewport,&x,&y,&z);
  PS_MOVE          (x,y);
  ox               = x;
  oy               = y;

  for(count=0;count<segment_number;count++)
    {
      double     x1,y1,z1,x2,y2,z2;
      gluProject (a_xs[2*count],a_ys[2*count],a_zs[2*count],model,proj,viewport,&x1,&y1,&z1);
      PS_RMOVE   (x1 - ox,y1 - oy);
      gluProject (a_xs[2*count+1],a_ys[2*count+1],a_zs[2*count+1],model,proj,viewport,&x2,&y2,&z2);
      PS_RLINETO (x2 - x1,y2 - y1);
      ox         = x2;
      oy         = y2;
    }
  PS_SetColor    ();
  PS_WIDTH       (a_width);
  PS_CAP         (1);
  PS_STYLE       (a_style)
  PS_STROKE;      
}
/***************************************************************************/
static void PS_DrawMarkers (
 int           a_pointn
,double*       a_xs
,double*       a_ys
,double*       a_zs
,OMarkerStyle  a_style 
,int           a_width 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLdouble    model[16],proj[16];
  GLint       viewport[4];
  double      x,y,z;
   int count;
  double      mark_size;
  int         icount;
/*.........................................................................*/
  glGetDoublev     (GL_MODELVIEW_MATRIX,model);
  glGetDoublev     (GL_PROJECTION_MATRIX,proj);
  glGetIntegerv    (GL_VIEWPORT,viewport);

  mark_size  = (double)(a_width <=0 ? 1 : a_width);   
  mark_size *= .6;
  if(a_style==OMarkerStyleCircle)
    {
      PS_NEWPATH;
      icount = 1;
      for(count=0;count<a_pointn;count++)
        {
	  gluProject (a_xs[count],a_ys[count],a_zs[count],model,proj,viewport,&x,&y,&z);
          PS_CIRCLE  (x,y,mark_size);
          if(icount==MAX_PATH_POINT)
            {
              PS_SetColor ();
              PS_WIDTH    (1);
              PS_CAP      (1);
              PS_STROKE;
              icount     = 1;
              if(count!=a_pointn-1) PS_NEWPATH;
            }
          else icount++;
        }
      PS_SetColor ();
      PS_WIDTH    (1);
      PS_CAP      (1);
      PS_STROKE;
    }
  else
    {
      PS_NEWPATH;
      icount = 1;
      for(count=0;count<a_pointn;count++)
        {
	  gluProject   (a_xs[count],a_ys[count],a_zs[count],model,proj,viewport,&x,&y,&z);
          PS_MOVE      (x,y);
          PS_PutMarker (a_style,mark_size);
          if(icount==MAX_PATH_POINT)
            {
              PS_SetColor ();
              PS_WIDTH    (1);
              PS_CAP      (1);
              PS_STROKE;
              icount     = 1;
              if(count!=a_pointn-1) PS_NEWPATH;
            }
          else icount++;
        }
      PS_SetColor ();
      PS_WIDTH    (1);
      PS_CAP      (1);
      PS_STROKE;
    }
}
/***************************************************************************/
static void PS_PutMarker (
 OMarkerStyle a_style 
,double a_size 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  switch (a_style)
    {
      case OMarkerStylePlus    :
	CPS_PutMarkerSizeInStream (a_size);
        CPS_PutInStreamF   ("plus ");
        break;
      case OMarkerStyleAsterisk:
	CPS_PutMarkerSizeInStream (a_size);
        CPS_PutInStreamF   ("asterisk ");
        break;
      case OMarkerStyleCross       :
	CPS_PutMarkerSizeInStream (a_size);
        CPS_PutInStreamF   ("cross ");
        break;
      case OMarkerStyleStar    :
	CPS_PutMarkerSizeInStream (a_size);
        CPS_PutInStreamF   ("star ");
        break;
      default                  :
        PS_RLINETO  (0.,0.);
        break;
    }
}
#endif /*HAS_GL*/



