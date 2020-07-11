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
#undef HAS_GL
*/
#ifdef HAS_LESSTIF
#define HAS_XAW
#undef HAS_XM
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#ifdef HAS_XM
#include <Xm/RowColumn.h> 
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h> 
#endif

#ifdef HAS_XAW
#include <Xaw/SimpleMenu.h>   
#include <Xaw/SmeBSB.h>   
#include <Xaw/SmeLine.h>  
#endif

#ifdef HAS_GL
#include <GL/glx.h>  /*Mesa.*/
#endif /*HAS_GL*/

/*Co*/
#include <CMemory.h>
#include <CString.h>
#include <CSystem.h>
#include <CList.h>
#include <CPS.h>
#include <CError.h>
#include <CPrinter.h>
#include <CVector.h>
#include <CText.h>
#include <CFile.h>
#include <OType.h>

/*Go*/
#include <OCamera.h>
#include <OCamVRML.h>
#include <OCamPOV.h>
#include <OCamDAWN.h>
#include <GoF77.h>
#include <OContext.h>
#include <GoTypes.h>

/*Xx*/
#include <XWidget.h>
#include <XSelection.h>

/*Private.*/
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>

#include <XoShapeP.h>
#include <XoCameraP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void     InitializeClass      (void);
static void     InitializeWidget     (Widget, Widget,ArgList,Cardinal*);
static void     RealizeWidget        (Widget, XtValueMask*, XSetWindowAttributes*);
static void     DestroyWidget        (Widget);
static void     ChangeWidgetSize     (Widget);
static void     DrawWidget           (Widget, XEvent*, Region);
static Boolean  SetValues            (Widget,Widget,Widget,ArgList,Cardinal *);
static void     ZoomAction           (Widget,XEvent*, String* ,Cardinal*);
static void     MoveAction           (Widget,XEvent*, String* ,Cardinal*);
static void     CollectAction        (Widget,XEvent*, String* ,Cardinal*);
static void     PopupAction          (Widget,XEvent*,String*,Cardinal*);
static void     HandleMessage        (Widget, XtPointer, XEvent*,Boolean*);

static void     ConvertStringToColorIndex       (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void     ConvertStringToBufferingType    (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void     ConvertStringToProjectionType   (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void     ConvertStringToDepthTestType    (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void     ConvertStringToTransformation   (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);
static void     ConvertStringToMove             (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);
static void     ConvertStringToAround           (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);
static void     ConvertStringToSpecialProjection(XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);

static void     ConvertColorIndexToString       (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void     ConvertBufferingTypeToString    (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void     ConvertProjectionTypeToString   (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void     ConvertDepthTestTypeToString    (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void     ConvertMoveTypeToString         (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);
static void     ConvertMoveWhatToString         (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);
static void     ConvertMoveAroundToString       (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);
static void     ConvertSpecialProjectionToString(XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);

static int         SendRefresh      (Widget);
static int         SendMove         (Widget);
static void        SendToMove       (Widget);
static void        DoRefresh        (Widget);
static void        CreatePopup      (Widget);
static void        ActivateCallback (Widget,XtPointer,XtPointer);
static void        SetGoRefreshProc (Display*);
static void        SendToGo         ();

static double            GetTransformationFactor     (Widget);
static OTransformation   GetTransformationIdentifier (char*);
static char*             GetTransformationName       (OTransformation);
static OMove             GetMoveIdentifier           (char*);
static char*             GetMoveName                 (OMove);
static OAround           GetAroundIdentifier         (char*);
static char*             GetAroundName               (OAround);
static int               MakeCurrent                 (Widget);
static void              TranslateOnRectangle        (Widget,XPoint*,XPoint*);
static void              PasteProc                   ();
static void              RestoreDefaults             (Widget);
static Widget            FindPopupEntry              (char*);
static void              ReportError                 (char*,int);
#ifdef __cplusplus
}
#endif

typedef struct
{
    int     reason;
    XEvent* event;
} XoAnyCallbackStruct;

#define NotFound      (-1)
#define DBL_INT       (int)(float)
#define AROUND(a_prec,a_around,a_number) if(fabs((a_number)-(a_around))<a_prec) {a_number = a_around;}
#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)

#define athis   ((XoCameraWidget)This)->camera
#define othis   ((XoCameraWidget)This)->camera.ocamera

#define acur   ((XoCameraWidget)a_current)->camera

#define IFMOD(a_field)  if(athis.a_field != acur.a_field)
#define IFSMOD(a_field) if(CStringCompare(athis.a_field,acur.a_field)==0)
#define IFLMOD(a_field) if(CListCompare((CList)athis.a_field,(CList)acur.a_field)==0)

#define MAX_STRING 128
static struct 
{
  XSelectionPasteFunction pasteProc;
  Cursor      cursor_fleur;
  Cursor      cursor_top_left_arrow;
  Widget      This;
  Widget      popup;
  Widget      goMbx;
  int         popupEnabled;
  int         goRefreshSent;
  Display*    gldisp;
#ifdef HAS_GL
  GLXContext  glContext1;
  GLXContext  glContext2;
#else
  void*       glContext1;
  void*       glContext2;
#endif /*HAS_GL*/
  int         glDepth1,glDepth2;
  Visual*     glVisual1;
  Visual*     glVisual2;
  Colormap    glColormap1;
  Colormap    glColormap2;
  Boolean     glInstall1;
  Boolean     glInstall2;
  char        string[MAX_STRING];
  XEvent      This_event;
} Class = 
{
NULL,
0L,0L, /*Cursors*/
NULL,NULL,NULL,
1,
0,
NULL,
NULL,NULL,                     /*GLXContext*/
0,0,                           /*Depth*/
CopyFromParent,CopyFromParent, /*Visual*/
0L,0L,                         /*Colormap*/
False,False,                   /*Install*/
""};

static char translations[] =
            "\
           Shift Ctrl <Btn1Down>: CameraZoom()\n\
   Shift Ctrl Button1 <PtrMoved>: CameraZoom()\n\
             Shift Ctrl <Btn1Up>: CameraZoom()\n\
                 Ctrl <Btn1Down>: CameraMove()\n\
         Ctrl Button1 <PtrMoved>: CameraMove()\n\
                   Ctrl <Btn1Up>: CameraMove()\n\
                Shift <Btn1Down>: CameraCollect(Shift)\n\
        Shift Button1 <PtrMoved>: CameraCollect(Shift)\n\
                  Shift <Btn1Up>: CameraCollect(Shift)\n\
                      <Btn1Down>: CameraCollect(None)\n\
              Button1 <PtrMoved>: CameraCollect(None)\n\
                        <Btn1Up>: CameraCollect(None)\n\
                      <Btn3Down>: CameraPopup()";

static XtActionsRec actions[] = 
{
  { "CameraZoom"      , ZoomAction},
  { "CameraMove"      , MoveAction},
  { "CameraCollect"   , CollectAction},
  { "CameraPopup"     , PopupAction}
};

/*virtualColormap must be before *Index resources so that ConvertStringToColorIndex works.*/

static XtResource resources [] ={
  {XoNvirtualColormap, "VirtualColormap", XtRString    , sizeof(String) ,
   XtOffset(XoCameraWidget, camera.virtualColormap),XtRString,  "ocolormap_X"},
  {XoNbackgroundColor   , "BackgroundColor", XoRColorIndex , sizeof(int)    ,
   XtOffset(XoCameraWidget, camera.backgroundColor),XtRString,"black"},
  {XoNhighlightContext  , "HighlightContext", XtRString , sizeof(String)    ,
   XtOffset(XoCameraWidget, camera.highlightContext),XtRString,"color white lineWidth 2"},
  {XoNedgeColor   , "EdgeColor", XoRColorIndex , sizeof(int)    ,
   XtOffset(XoCameraWidget, camera.edgeColor),XtRString,"black"},
  {XoNtransformationSensitiveWidgets ,"TransformationSensitiveWidgets", XtRWidgetList, sizeof(WidgetList) ,
   XtOffset(XoCameraWidget,camera.transformationSensitiveWidgets),XtRImmediate,NULL},
  {XoNreferenceCamera ,"ReferenceCamera", XtRWidget, sizeof(Widget) ,
   XtOffset(XoCameraWidget,camera.referenceCamera),XtRImmediate,NULL},
  {XoNviewLimits     , "ViewLimits"     , XtRString    , sizeof(String) ,
   XtOffset(XoCameraWidget, camera.viewLimits)     ,XtRString   ,"-1 1"},
  {XoNupVector         , "UpVector"         , XtRString    , sizeof(String) ,
   XtOffset(XoCameraWidget, camera.upVector)         ,XtRString   ,"0 1 0"},
  {XoNviewPoint      , "ViewPoint"      , XtRString    , sizeof(String) ,
   XtOffset(XoCameraWidget, camera.viewPoint)      ,XtRString   ,"0 0 1"},
  {XoNprojectionAngle, "ProjectionAngle", XtRInt       , sizeof(int)    ,
   XtOffset(XoCameraWidget, camera.projectionAngle),XtRString   ,"90"},
  {XoNbuffering  , "Buffering"  , XoRBuffering, sizeof(OBuffering),  
   XtOffset(XoCameraWidget, camera.buffering),XtRImmediate,(XtPointer)OBufferingSimple},
  {XoNdepthTreatment, "DepthTreatment"  , XoRDepthTreatment, sizeof(ODepthTreatment),  
   XtOffset(XoCameraWidget, camera.depthTreatment),XtRImmediate,(XtPointer)ODepthTreatmentNone},
  {XoNprojection , "Projection"  , XoRProjection, sizeof(OProjection),  
   XtOffset(XoCameraWidget, camera.projection),XtRImmediate,(XtPointer)OProjectionParallel},
  {XoNspecialProjection , "SpecialProjection"  , XoRSpecialProjection, sizeof(OSpecialProjection),  
   XtOffset(XoCameraWidget, camera.specialProjection),XtRImmediate,(XtPointer)OSpecialProjectionXYZ},
  {XoNtransformation  ,"Transformation" , XoRTransformation, sizeof(OTransformation),
   XtOffset(XoCameraWidget, camera.transformation) ,XtRImmediate,(XtPointer)OTransformationScale},
  {XoNtransformedObject  ,"TransformedObject" , XoRTransformedObject, sizeof(OMove),
   XtOffset(XoCameraWidget, camera.transformedObject) ,XtRImmediate,(XtPointer)OMoveScene},
  {XoNtransformationAxis,"TransformationAxis" , XoRTransformationAxis, sizeof(OAround),
   XtOffset(XoCameraWidget, camera.transformationAxis) ,XtRImmediate,(XtPointer)OAroundXY},
  {XoNtransformationFactor ,"TransformationFactor" , XtRInt, sizeof(int),
   XtOffset(XoCameraWidget, camera.transformationFactor) ,XtRImmediate,(XtPointer)30},
  {XoNtransformCallback   ,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
   XtOffset(XoCameraWidget,camera.transformCallback),XtRImmediate,(XtPointer)NULL},
  {XoNhelpCallback   ,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
   XtOffset(XoCameraWidget,camera.helpCallback),XtRImmediate,(XtPointer)NULL},
  {XoNreceiveTransformCallback   ,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
   XtOffset(XoCameraWidget,camera.receiveTransformCallback),XtRImmediate,(XtPointer)NULL},
  {XoNcollectCallback,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
   XtOffset(XoCameraWidget,camera.collectCallback),XtRImmediate,(XtPointer)NULL},
  {XoNedgesVisible ,"EdgesVisible"  ,XtRBoolean,sizeof(Boolean),
   XtOffset(XoCameraWidget,camera.edgesVisible),XtRImmediate,(XtPointer)True},
  {XoNblackAndWhite, "BlackAndWhite", XtRInt       , sizeof(int)    ,
   XtOffset(XoCameraWidget, camera.blackAndWhite),XtRImmediate,NULL}
};

XoCameraClassRec  xoCameraClassRec = {
/* Core Class Part */
{
   (WidgetClass)&xoShapeClassRec,/* pointer to superclass ClassRec   */
    "XoCamera",         	/* widget resource class name       */
    sizeof(XoCameraRec),	/* size in bytes of widget record   */
    InitializeClass,            /* class_initialize                 */
    NULL, 			/* dynamic initialization           */
    FALSE,       		/* has class been initialized?      */
    InitializeWidget,           /* initialize                       */
    NULL,			/* notify that initialize called    */
    RealizeWidget,              /* XCreateWindow for widget         */
    actions,     		/* widget semantics name to proc mapWidget*/
    XtNumber(actions),          /* number of entries in actions     */
    resources,                  /* resources for subclass fields    */
    XtNumber(resources),        /* number of entries in resources   */
    NULLQUARK,		        /* resource class quarkified        */
    TRUE,			/* compress MotionNotify for widget */
    TRUE,			/* compress Expose events for widget*/
    TRUE,			/* compress enter and leave events  */
    TRUE,			/* select for VisibilityNotify      */
    DestroyWidget,	        /* free data for subclass pointers  */
    ChangeWidgetSize,           /* geom manager changed widget size */
    DrawWidget,	                /* rediplay window                  */
    SetValues,	                /* set subclass resource values     */
    NULL,			/* notify that SetValues called    */
    XtInheritSetValuesAlmost,   /* SetValues got "Almost" geo reply*/
    NULL,		        /* notify that get_values called    */
    XtInheritAcceptFocus,       /* assign input focus to widget     */
    XtVersion,	                /* version of intrinsics used       */
    NULL,		        /* list of callback offsets         */
    translations,	        /* state machine                    */
    XtInheritQueryGeometry,     /* return preferred geometry        */
    XtInheritDisplayAccelerator,/* display your accelerator         */
    NULL 		        /* pointer to extension record      */
},
/* Composite Class Part */
{
    XtInheritGeometryManager,   /* geometry manager for children   */
    XtInheritChangeManaged,     /* change managed state of child   */
    XtInheritInsertChild,       /* physically add child to parent  */
    XtInheritDeleteChild,       /* physically remove child         */
    NULL 		        /* pointer to extension record     */
},
/* XoShape */
{
   NULL 
},
/* XoCamera */
{
   NULL 
}
   
};

WidgetClass xoCameraWidgetClass = (WidgetClass) &xoCameraClassRec;
/***************************************************************************/
static void InitializeClass (
 void
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static int             cvt_count  = 0;
  static XtConvertArgRec cvt_args[] = {
{XtResourceString,(XtPointer)XoNvirtualColormap,sizeof(char*)},
{XtAddress,(XtPointer)&cvt_count,sizeof(int)}   /*To avoid caching.*/
};
/*.........................................................................*/
#ifndef HAS_GL
  CWarn ("Xo package has not been compiled with -DHAS_GL option.\n");
#endif

#ifdef DEBUG
  printf ("debug : XoCamera : InitializeClass.\n");
#endif

/*
  XtAddConverter adds a converter to all application context.
*/
  XtAddConverter (XtRString,XoRColorIndex         ,(XtConverter)ConvertStringToColorIndex        ,cvt_args,2);
  XtAddConverter (XtRString,XoRBuffering          ,(XtConverter)ConvertStringToBufferingType     ,NULL,0);
  XtAddConverter (XtRString,XoRProjection         ,(XtConverter)ConvertStringToProjectionType    ,NULL,0);
  XtAddConverter (XtRString,XoRDepthTreatment     ,(XtConverter)ConvertStringToDepthTestType     ,NULL,0);
  XtAddConverter (XtRString,XoRTransformation     ,(XtConverter)ConvertStringToTransformation    ,NULL,0);
  XtAddConverter (XtRString,XoRTransformedObject  ,(XtConverter)ConvertStringToMove              ,NULL,0);
  XtAddConverter (XtRString,XoRTransformationAxis ,(XtConverter)ConvertStringToAround            ,NULL,0);
  XtAddConverter (XtRString,XoRSpecialProjection  ,(XtConverter)ConvertStringToSpecialProjection ,NULL,0);

  XtAddConverter (XoRColorIndex         ,XtRString,(XtConverter)ConvertColorIndexToString        ,cvt_args,2);
  XtAddConverter (XoRBuffering          ,XtRString,(XtConverter)ConvertBufferingTypeToString     ,NULL,0);
  XtAddConverter (XoRProjection         ,XtRString,(XtConverter)ConvertProjectionTypeToString    ,NULL,0);
  XtAddConverter (XoRDepthTreatment     ,XtRString,(XtConverter)ConvertDepthTestTypeToString     ,NULL,0);
  XtAddConverter (XoRTransformation     ,XtRString,(XtConverter)ConvertMoveTypeToString          ,NULL,0);
  XtAddConverter (XoRTransformedObject  ,XtRString,(XtConverter)ConvertMoveWhatToString          ,NULL,0);
  XtAddConverter (XoRTransformationAxis ,XtRString,(XtConverter)ConvertMoveAroundToString        ,NULL,0);
  XtAddConverter (XoRSpecialProjection  ,XtRString,(XtConverter)ConvertSpecialProjectionToString ,NULL,0);

  GoLoadColormapX (); /*Warning. InitializeClass is not reexecuted if full restart from Wo. */

#ifdef HAS_XAW
  XtRegisterGrabAction(PopupAction,True,ButtonPressMask | ButtonReleaseMask,GrabModeAsync,GrabModeAsync);
#endif /*HAS_XAW*/

}
/***************************************************************************/
static void InitializeWidget (
 Widget a_request
,Widget This
,ArgList a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_request->core.width<=0)  This->core.width  = 100;
  if(a_request->core.height<=0) This->core.height = 100;

#ifdef DEBUG
  printf ("debug : XoCamera : InitializeWidget : %s\n",XtName(This));
#endif

#ifdef DEBUG
  printf ("debug : XoCamera : InitializeWidget : %s colormap : %s\n",XtName(This),athis.virtualColormap);
#endif

  othis  = OCameraCreate (This->core.name,This->core.width,This->core.height);

  if(athis.transformationSensitiveWidgets!=NULL)
    { 
      athis.transformationSensitiveWidgets = (WidgetList)CListDuplicate((CList)athis.transformationSensitiveWidgets);
      CListRemoveDuplicates    ((CList)athis.transformationSensitiveWidgets,NULL,0);
      SendToMove          (This);
    }

    { double min,max;
      if (sscanf (athis.viewLimits,"%le %le",&min,&max)!=2)  {min = -1.;max = 1.;}
      OCameraSetField (othis,min,max);
    }
    { 
      double x,y,z;
      if (sscanf (athis.viewPoint,"%le %le %le",&x,&y,&z)!=3)  {x = 0.;y = 0.;z = 1.;}
      OCameraSetDefaultVRP (othis,x,y,z);
    }
    { 
      double x,y,z;
      if (sscanf (athis.upVector,"%le %le %le",&x,&y,&z)!=3)  {x = 0.;y = 1.;z = 0.;}
      OCameraSetDefaultUpVector    (othis,x,y,z);
    }

  OCameraConnect                 (othis,(OCamera)XoCameraGetCamera(athis.referenceCamera));
  OCameraSetColormap             (othis,OColormapGetIdentifierFromString(athis.virtualColormap));
  OCameraSetBackgroundColorIndex (othis,athis.backgroundColor);
  OCameraSetHighlightContext     (othis,athis.highlightContext);
  OCameraSetEdgeColorIndex       (othis,athis.edgeColor);

  OCameraSetProjectionAngle      (othis,(double)(athis.projectionAngle * M_PI/180.));
  OCameraSetDepthTreatment       (othis,athis.depthTreatment);
  OCameraSetBuffering            (othis,athis.buffering);
  OCameraSetProjection           (othis,athis.projection);
  OCameraSetSpecialProjection    (othis,athis.specialProjection);
  OCameraSetEdgesVisible         (othis,(int)athis.edgesVisible);
  OCameraSetBlackAndWhite        (othis,(int)athis.blackAndWhite);

  athis.viewLimits         = CStringDuplicate(athis.viewLimits);
  athis.viewPoint          = CStringDuplicate(athis.viewPoint);
  athis.upVector           = CStringDuplicate(athis.upVector);
  athis.virtualColormap    = CStringDuplicate(athis.virtualColormap);
  athis.highlightContext   = CStringDuplicate(athis.highlightContext);

  XtAddEventHandler                    (This,0L,True,HandleMessage,NULL);
  OCameraSetRequireMovementFunction    (othis,(OCameraSendFunction)SendMove   ,This,0);
  OCameraSetRequireRefreshmentFunction (othis,(OCameraSendFunction)SendRefresh,This,0);

  SetGoRefreshProc         (XtDisplay(This));

  athis.initialDepth       = This->core.depth;
  athis.initialColormap    = This->core.colormap;
  athis.visual             = CopyFromParent;
  athis.installColormap    = False;
  athis.rubberGC           = NULL;

#ifdef HAS_GL
 {Display*            display;
  int                 attributeList[9];
  XVisualInfo*        vinfo;
  display             = XtDisplay(This);

 {int                 error,event;
  if(glXQueryExtension(display,&error,&event)==0) 
    {
      CWarn ("X server do not have OpenGL extensions.\n");
    }}

  if(Class.glContext1==NULL)  /*First context for RGBA without double buffer.*/
    {
      attributeList[0] = GLX_RGBA;
      attributeList[1] = GLX_RED_SIZE;
      attributeList[2] = 1;
      attributeList[3] = GLX_GREEN_SIZE;
      attributeList[4] = 1;
      attributeList[5] = GLX_BLUE_SIZE;
      attributeList[6] = 1;
      attributeList[7] = None;
      vinfo = glXChooseVisual  (display,DefaultScreen(display),attributeList);
      if(vinfo==NULL)  /*Try something else.*/
	{
	  attributeList[0] = GLX_RGBA;
	  attributeList[1] = GLX_RED_SIZE;
	  attributeList[2] = 1;
	  attributeList[3] = GLX_GREEN_SIZE;
	  attributeList[4] = 1;
	  attributeList[5] = GLX_BLUE_SIZE;
	  attributeList[6] = 1;
	  attributeList[7] = GLX_DOUBLEBUFFER;
	  attributeList[8] = None;
	  vinfo = glXChooseVisual  (display,DefaultScreen(display),attributeList);
	  if(vinfo==NULL)  
	    {
	      CWarn ("Can't choose a visual.\n");
	      return;
	    }
	}
      Class.glContext1  = glXCreateContext (display,vinfo,NULL,GL_FALSE);
      if(Class.glContext1==NULL) 
	{
	  CWarn ("Can't create a GLX context.\n");
	}
      if( (vinfo->depth ==DefaultDepth (display,DefaultScreen(display)))  &&
	  (vinfo->visual==DefaultVisual(display,DefaultScreen(display))) )
	 {
	  Class.glColormap1 = XDefaultColormap (display,DefaultScreen(display));
	  Class.glInstall1  = False;
	 }
      else
	{
	  Class.glColormap1 = XCreateColormap  (display,XDefaultRootWindow(display),vinfo->visual, AllocNone); 
	  Class.glInstall1  = True;
	}
      Class.glDepth1    = vinfo->depth;
      Class.glVisual1   = vinfo->visual;
      XFree             (vinfo);
      if(Class.glColormap1==0L)
	{
	  CWarn ("Can't create a X colormap.\n");
	}
    }

  if(Class.glContext2==NULL) /*Second context for RGBA with double buffer.*/
    {
      attributeList[0] = GLX_RGBA;
      attributeList[1] = GLX_RED_SIZE;
      attributeList[2] = 1;
      attributeList[3] = GLX_GREEN_SIZE;
      attributeList[4] = 1;
      attributeList[5] = GLX_BLUE_SIZE;
      attributeList[6] = 1;
      attributeList[7] = GLX_DOUBLEBUFFER;
      attributeList[8] = None;
      vinfo = glXChooseVisual  (display,DefaultScreen(display),attributeList);
      if(vinfo==NULL)  
	{
	  CWarn ("Can't choose a visual for double buffering.\n");
	  return;
	}
      Class.glContext2 = glXCreateContext (display,vinfo,NULL,GL_FALSE);
      if(Class.glContext2==NULL) 
	{
	  CWarn ("Can't create a GLX context.\n");
	}
      if( (vinfo->depth ==Class.glDepth1) &&
	  (vinfo->visual==Class.glVisual1) )  /*Can take same as first context.*/
	 {
	   Class.glColormap2 = Class.glColormap1;
	   Class.glInstall2  = Class.glInstall1;
	   Class.glDepth2    = Class.glDepth1;
	   Class.glVisual2   = Class.glVisual1;
	 }
      else
	{
	  if( (vinfo->depth ==DefaultDepth (display,DefaultScreen(display)))  &&
	      (vinfo->visual==DefaultVisual(display,DefaultScreen(display))) )
	    {
	      Class.glColormap2 = XDefaultColormap (display,DefaultScreen(display));
	      Class.glInstall2  = False;
	    }
	  else
	    {
	      Class.glColormap2 = XCreateColormap  (display,XDefaultRootWindow(display),vinfo->visual,AllocNone); 
	      Class.glInstall2  = True;
	    }
	  Class.glDepth2  = vinfo->depth;
	  Class.glVisual2 = vinfo->visual;
	}
      XFree           (vinfo);
      if(Class.glColormap2==0L)
	{
	  CWarn ("Can't create a X colormap.\n");
	}
    }
  Class.gldisp = display;

  if(athis.buffering==OBufferingSimple)
    {
      This->core.depth      = Class.glDepth1;
      This->core.colormap   = Class.glColormap1;
      athis.visual          = Class.glVisual1;
      athis.installColormap = Class.glInstall1;
    }
  else if(athis.buffering==OBufferingDouble)
    {
      This->core.depth      = Class.glDepth2;
      This->core.colormap   = Class.glColormap2;
      athis.visual          = Class.glVisual2;
      athis.installColormap = Class.glInstall2;
    } }
#endif

#ifdef DEBUG
  printf                   ("debug : XoCamera : InitializeWidget : end\n");
#endif

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/
}
/***************************************************************************/
static void RealizeWidget (
 Widget This
,XtValueMask* a_mask
,XSetWindowAttributes* a_watbs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef DEBUG
  printf                   ("debug : XoCamera : RealizeWidget : %s\n",XtName(This));
#endif

/*Have to create window ourselves due to OpenGL that compells it's visual.*/
/*In principle colormap is correctly set in a_watbs.*/
  XtCreateWindow (This, (unsigned int) InputOutput,athis.visual, *a_mask, a_watbs);

/* Call the Realize procedure (XtInheritRealize) */
  if(xoCameraWidgetClass->core_class.superclass->core_class.realize!=NULL)
    (xoCameraWidgetClass->core_class.superclass->core_class.realize)(This, a_mask, a_watbs);

    athis.rubberGC             = XDisplayGetRubberGC(XtDisplay(This),XtWindow(This));

  ONodeRequireViewRefreshment  (othis);
  OCameraRequireMovement       (othis);

/*If window is delete, all seems ok.*/
  if(athis.installColormap==True) XWidgetInstallColormap (This);

#ifdef DEBUG
  printf                   ("debug : XoCamera : RealizeWidget : end\n");
#endif
}
/***************************************************************************/
static void DestroyWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(athis.installColormap==True)  
    {
      XWidgetUninstallColormap (This);
      athis.installColormap    = False;
    }
  if(athis.rubberGC!=NULL)
    {
      XFreeGC        (XtDisplay(This),athis.rubberGC);
      athis.rubberGC = NULL;
    }

  XtRemoveAllCallbacks   (This, XoNcollectCallback);
  XtRemoveAllCallbacks   (This, XoNtransformCallback);
  XtRemoveAllCallbacks   (This, XoNreceiveTransformCallback);

  CListDelete            ((CList)athis.transformationSensitiveWidgets);
  athis.transformationSensitiveWidgets = NULL;

  CStringDelete          (athis.viewLimits);
  CStringDelete          (athis.viewPoint);
  CStringDelete          (athis.upVector);
  CStringDelete          (athis.virtualColormap);
  CStringDelete          (athis.highlightContext);
  athis.viewLimits       = NULL;
  athis.viewPoint        = NULL;
  athis.upVector         = NULL;
  athis.virtualColormap  = NULL;
  athis.highlightContext = NULL;

  OCameraDelete          (othis);
}
/***************************************************************************/
static Boolean SetValues (
 Widget a_current
,Widget a_request
,Widget This
,ArgList a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  IFMOD(backgroundColor)  OCameraSetBackgroundColorIndex (othis,athis.backgroundColor);
  IFMOD(edgeColor)        OCameraSetEdgeColorIndex       (othis,athis.edgeColor);
  IFMOD(edgesVisible)     OCameraSetEdgesVisible         (othis,(int)athis.edgesVisible);
  IFMOD(blackAndWhite)    OCameraSetBlackAndWhite        (othis,(int)athis.blackAndWhite);

  IFLMOD(transformationSensitiveWidgets)
    { 
      CListDelete             ((CList)acur.transformationSensitiveWidgets);
      athis.transformationSensitiveWidgets      = (WidgetList)CListDuplicate((CList)athis.transformationSensitiveWidgets);
      CListRemoveDuplicates     ((CList)athis.transformationSensitiveWidgets,NULL,0);
      SendToMove             (This);
    }
  else {athis.transformationSensitiveWidgets = acur.transformationSensitiveWidgets;}

  IFSMOD(viewLimits)
    { double min,max;
      CStringDelete    (acur.viewLimits);
      athis.viewLimits = CStringDuplicate (athis.viewLimits);
      if (sscanf (athis.viewLimits,"%le %le",&min,&max)!=2)  {min = -1.;max = 1.;}
      OCameraSetField (othis,min,max);
    }
  else {athis.viewLimits = acur.viewLimits;}

  IFSMOD(viewPoint)
    { 
      double x,y,z;
      CStringDelete    (acur.viewPoint);
      athis.viewPoint  = CStringDuplicate (athis.viewPoint);
      if (sscanf (athis.viewPoint,"%le %le %le",&x,&y,&z)!=3)  { x = 0.;y = 0.;z = 1.;}
      OCameraSetDefaultVRP (othis,x,y,z);
    }
  else {athis.viewPoint = acur.viewPoint;}

  IFSMOD(upVector)
    { 
      double x,y,z;
      CStringDelete    (acur.upVector);
      athis.upVector     = CStringDuplicate (athis.upVector);
      if (sscanf (athis.upVector,"%le %le %le",&x,&y,&z)!=3)  { x = 0.;y = 1.;z = 0.;}
      OCameraSetDefaultUpVector    (othis,x,y,z);
    }
  else {athis.upVector = acur.upVector;}

  IFSMOD(virtualColormap) 
    {
      CStringDelete         (acur.virtualColormap);
      athis.virtualColormap = CStringDuplicate (athis.virtualColormap);
      OCameraSetColormap    (othis,OColormapGetIdentifierFromString(athis.virtualColormap));
    }
  else {athis.virtualColormap = acur.virtualColormap;}

  IFSMOD(highlightContext) 
    {
      CStringDelete              (acur.highlightContext);
      athis.highlightContext     = CStringDuplicate (athis.highlightContext);
      OCameraSetHighlightContext (othis,athis.highlightContext);
    }
  else {athis.highlightContext = acur.highlightContext;}

  IFMOD(referenceCamera)   OCameraConnect              (othis,(OCamera)XoCameraGetCamera(athis.referenceCamera));
  IFMOD(projectionAngle)   OCameraSetProjectionAngle   (othis,(double)(athis.projectionAngle * M_PI/180.));
  IFMOD(projection)        OCameraSetProjection        (othis,athis.projection);
  IFMOD(specialProjection) OCameraSetSpecialProjection (othis,athis.specialProjection);
  IFMOD(edgesVisible)      OCameraSetEdgesVisible      (othis,(int)athis.edgesVisible);
  IFMOD(blackAndWhite)     OCameraSetBlackAndWhite     (othis,(int)athis.blackAndWhite);
  IFMOD(depthTreatment)    OCameraSetDepthTreatment    (othis,athis.depthTreatment);

/*
  Can't change buffering here if X window is created. 
  With OpenGL, buffering fix parameter of the X window.
  Buffering must be choosen before the execution of the 
  Realize method that create the window.
*/
  IFMOD(buffering)  
    {
      if(XtIsRealized(This) && (athis.installColormap==True))
	{
	  CWarn               ("Can't change buffering after \"realization\" of the widget.\n");
	  athis.buffering     = acur.buffering;
	}
      else
	{
	  OCameraSetBuffering (othis,athis.buffering);
	}
    }

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/

  return    False;
}
/***************************************************************************/
static void ChangeWidgetSize (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef DEBUG
  printf                   ("debug : XoCamera : ChangeWidgetSize : %s\n",XtName(This));
#endif

/* Call the Resize procedure (XtInheritResize) */
  if(xoCameraWidgetClass->core_class.superclass->core_class.resize!=NULL)
    (xoCameraWidgetClass->core_class.superclass->core_class.resize)(This);

  OCameraChangeSize          (othis,This->core.width,This->core.height);

#ifdef DEBUG
  printf                   ("debug : XoCamera : ChangeWidgetSize : end\n");
#endif

}
/***************************************************************************/
static void DrawWidget (
 Widget  This
,XEvent* a_event
,Region a_region
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef DEBUG
  printf                   ("debug : XoCamera : DrawWidget : %s\n",XtName(This));
#endif

  if(xoCameraWidgetClass->core_class.superclass->core_class.expose!=NULL)
    (xoCameraWidgetClass->core_class.superclass->core_class.expose)(This,a_event,a_region);

  ONodeRequireViewDrawing (othis);

#ifdef DEBUG
  printf                   ("debug : XoCamera : DrawWidget : end\n");
#endif

  a_event           = NULL;
  a_region          = NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void MoveAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    int      ox,oy;
  } pointer = {0,0};
/*.........................................................................*/
  if(This==NULL) return;
  switch(a_event->type)
    {
      case ButtonPress:
            if(Class.cursor_fleur==0L) 
	      Class.cursor_fleur = XCreateFontCursor(XtDisplay(This),XC_fleur);
            if(Class.cursor_top_left_arrow==0L) 
	      Class.cursor_top_left_arrow = XCreateFontCursor(XtDisplay(This),XC_top_left_arrow);
            XDefineCursor     (XtDisplay(This),XtWindow(This),Class.cursor_fleur) ;
            pointer.ox        = a_event->xbutton.x;
            pointer.oy        = a_event->xbutton.y;
	    OCameraPutMatrixOnStack (othis);
            break;
      case ButtonRelease:
            XDefineCursor     (XtDisplay(This),XtWindow(This),Class.cursor_top_left_arrow) ;
            break;
      case MotionNotify:
           {int     vx,vy;
            int     delta;
            double  transFactor = 400.;

            vx     = a_event->xmotion.x - pointer.ox;
            vy     = a_event->xmotion.y - pointer.oy;
            delta  = DBL_INT sqrt ((double)(vx*vx + vy*vy));
            if(delta<=10) break;

            if(athis.transformation==OTransformationScale)  /* scale */
              {
                delta = DBL_INT ((abs(vx)>abs(vy) ? vx : -vy) * GetTransformationFactor(This));
                     if(athis.transformedObject==OMoveCamera)
		  {
                    OCameraScaleDefaultFieldAndVRP (othis,exp(-0.02 * delta));
                  }
                else if(athis.transformedObject==OMoveScene)
		  { OMatrix             matrix;
                    matrix              = OMatrixCreate(OMatrixScaleIsometric,exp(0.02*delta));
                    OCameraMultiplyMatrix   (othis,matrix);
                    OMatrixDelete       (matrix);
                  }
                else if(athis.transformedObject==OMoveHighLight)
		  { OMatrix              matrix;
                    ONode*             list;
		    ONode*             itema;
                    matrix               = OMatrixCreate(OMatrixScaleIsometric,exp(0.02*delta));
                    list                 = ONodeCollect(OCameraGetNodes(othis),OCollectHighlighted);
                    for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) ONodeMultiplyMatrix (*itema,matrix);
                    CListDelete           ((CList)list);
                    OMatrixDelete        (matrix);
                  }

              }

            else if(athis.transformationAxis==OAroundAxis)
              {
                     if(athis.transformedObject==OMoveCamera)
		  {
                  }
                else if(athis.transformedObject==OMoveScene)
		  { 
		    delta = DBL_INT ((abs(vx)>abs(vy) ? vx : -vy) * GetTransformationFactor(This));
		    if (athis.transformation==OTransformationRotate)
		      {
			OCameraRotateMatrixOnPick (othis,(double)(2. * M_PI * delta / (double)This->core.width),
						   OCameraGetPick(othis,0),OCameraGetPick(othis,1));
		      }              
		    else if (athis.transformation==OTransformationTranslate)
		      {
			OCameraTranslateMatrixOnPick (othis,OCameraGetScale(othis) * delta/transFactor,
						      OCameraGetPick(othis,0),OCameraGetPick(othis,1));
		      }              
                  }
                else if(athis.transformedObject==OMoveHighLight)
		  { 
                  }
              }

            else if(athis.transformationAxis==OAroundZ)
              { 
                OMatrix matrix = NULL;
                delta = DBL_INT ((abs(vx)>abs(vy) ? vx : -vy) * GetTransformationFactor(This));

                         if(athis.transformation==OTransformationRotate)
		      { /* rotate along dop axis od camera */
                        double     x,y,z;
                        OCameraGetDOP (othis,&x,&y,&z);
                        matrix = OMatrixCreate (OMatrixRotationAxis,(double)(-2. * M_PI * delta / (double)This->core.width),x,y,z);
                      }
                    else if(athis.transformation==OTransformationTranslate)
		      { /* translate along dop axis of camera */
                        double        x,y,z;
                        OCameraGetDOP    (othis,&x,&y,&z);
                        matrix        = OMatrixCreate (OMatrixTranslationAxis,-OCameraGetScale(othis) * delta/transFactor,x,y,z);
                      }

                     if(athis.transformedObject==OMoveCamera)
		  { 
                    OCameraMultiplyUpVectorAndVRP (othis,matrix);
                  }
                else if(athis.transformedObject==OMoveScene)
		  { 
                    OCameraMultiplyMatrix    (othis,matrix);
                  }
                else if(athis.transformedObject==OMoveHighLight)
                  { 
                    ONode*             list;
		    ONode*             itema;
                    list                 = ONodeCollect(OCameraGetNodes(othis),OCollectHighlighted);
                    for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) ONodeMultiplyMatrix (*itema,matrix);
                    CListDelete           ((CList)list);
                   }
                OMatrixDelete       (matrix);

              }
            else if(abs(vx)>abs(vy)) /* move cursor horizontal */
              { 
                OMatrix matrix = NULL;
                delta    = DBL_INT (vx * GetTransformationFactor(This));

                         if(athis.transformation==OTransformationRotate)
		      { /* rotate along up axis of camera */
                        double    x,y,z;
                        OCameraGetUpVector (othis,&x,&y,&z);
                        matrix = OMatrixCreate (OMatrixRotationAxis,(double)(2. * M_PI * delta / (double)This->core.width),x,y,z);
                      }
                    else if(athis.transformation==OTransformationTranslate)
		      { /* translate along -(up v dop) axis of camera */
                        double        up_x,up_y,up_z;
                        double        dop_x,dop_y,dop_z;
                        double        x,y,z;
                        OCameraGetUpVector     (othis,&up_x,&up_y,&up_z);
                        OCameraGetDOP    (othis,&dop_x,&dop_y,&dop_z);
                        CVectorMultiply      (up_x,up_y,up_z,dop_x,dop_y,dop_z,&x,&y,&z);
                        matrix        = OMatrixCreate(OMatrixTranslationAxis,- OCameraGetScale(othis) * delta/transFactor,x,y,z);
                      }


                     if(athis.transformedObject==OMoveCamera)
		  { 
                    OCameraMultiplyUpVectorAndVRP    (othis,matrix);
                  }
                else if(athis.transformedObject==OMoveScene)
                  { 
                    OCameraMultiplyMatrix       (othis,matrix);
                  }
                else if(athis.transformedObject==OMoveHighLight)
                  { 
                    ONode*             list;
		    ONode*             itema;
                    list                 = ONodeCollect(OCameraGetNodes(othis),OCollectHighlighted);
                    for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) ONodeMultiplyMatrix (*itema,matrix);
                    CListDelete           ((CList)list);
                   }
                OMatrixDelete       (matrix);
              }
            else                    /* move cursor vertical */
              {
                OMatrix matrix = NULL;
                delta    = DBL_INT (vy * GetTransformationFactor(This));

                         if(athis.transformation==OTransformationRotate) 
		      { /* rotate along -(up v dop) axis of camera */
                        double        up_x,up_y,up_z;
                        double        dop_x,dop_y,dop_z;
                        double        x,y,z;
                        OCameraGetUpVector     (othis,&up_x,&up_y,&up_z);
                        OCameraGetDOP    (othis,&dop_x,&dop_y,&dop_z);
                        CVectorMultiply      (up_x,up_y,up_z,dop_x,dop_y,dop_z,&x,&y,&z);
                        matrix        = OMatrixCreate 
                                        (OMatrixRotationAxis,(double)( -2. * M_PI * delta / (double)This->core.width),x,y,z);
                      }
                    else if(athis.transformation==OTransformationTranslate) 
		      { /* translate along up axis of camera */
                        double        x,y,z;
                        OCameraGetUpVector     (othis,&x,&y,&z);
                        matrix = OMatrixCreate   (OMatrixTranslationAxis,-OCameraGetScale(othis) * delta/transFactor,x,y,z);
                      }

                     if(athis.transformedObject==OMoveCamera)
		  { 
                    OCameraMultiplyUpVectorAndVRP (othis,matrix);
                  }
                else if(athis.transformedObject==OMoveScene)
		  { 
                    OCameraMultiplyMatrix   (othis,matrix);
                  }
                else if(athis.transformedObject==OMoveHighLight)
                  { 
		    ONode*             list;
		    ONode*             itema;
                    list                 = ONodeCollect(OCameraGetNodes(othis),OCollectHighlighted);
                    for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) ONodeMultiplyMatrix (*itema,matrix);
                    CListDelete           ((CList)list);
                  }
                OMatrixDelete       (matrix);
              }


            }
            pointer.ox       = a_event->xmotion.x;
            pointer.oy       = a_event->xmotion.y;
            break;
    }
  a_args = NULL; /* to remove warning of c++ compiler */
  a_argn = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static double GetTransformationFactor (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return pow (10.,(athis.transformationFactor-50.)/50.);
}
/***************************************************************************/
char** XoCameraGetResourceOptions (
 char* a_type 
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_type==NULL)  return NULL;
       if(strcmp(a_type,XoRBuffering)==0)         return OBufferingGetNames(a_number);
  else if(strcmp(a_type,XoRDepthTreatment)==0)    return ODepthTreatmentGetNames(a_number);
  else if(strcmp(a_type,XoRProjection)==0)        return OProjectionGetNames(a_number);
  else if(strcmp(a_type,XoRSpecialProjection)==0) return OSpecialProjectionGetNames(a_number);
  else if(strcmp(a_type,"VirtualColormap")==0)    return OColormapGetNames (a_number);
  return NULL;
}
/***************************************************************************/
void XoCameraClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.popupEnabled   = 1;
  OColormapClearClass  ();
  OCameraClearClass    ();
#ifdef HAS_GL
  if(Class.glContext1!=NULL)
    {
      glXMakeCurrent     (Class.gldisp,None,NULL);
      glXDestroyContext  (Class.gldisp,Class.glContext1);
      XFreeColormap      (Class.gldisp,Class.glColormap1);
      Class.glContext1   = NULL;
      Class.glColormap1  = 0L;
    }
  if(Class.glContext2!=NULL)
    {
      glXMakeCurrent     (Class.gldisp,None,NULL);
      glXDestroyContext  (Class.gldisp,Class.glContext2);
      XFreeColormap      (Class.gldisp,Class.glColormap2);
      Class.glContext2   = NULL;
      Class.glColormap2  = 0L;
    }
  Class.gldisp       = NULL;
#endif
  GoF77_ClearClass   ();
}
/***************************************************************************/
static void RestoreDefaults (
 Widget This 
)
/***************************************************************************/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return;

         if(athis.transformedObject==OMoveCamera)
      {
	OCameraRestoreDefaultUpVectorAndVRP    (othis);
      }
    else if(athis.transformedObject==OMoveScene)
      {
	OCameraPutMatrixOnStack     (othis);
	OCameraRestoreDefaultMatrix (othis);
      }
    else if(athis.transformedObject==OMoveHighLight)
      { 
	ONode*             list;
	ONode*             itema;
	list                 = ONodeCollect(OCameraGetNodes(othis),OCollectHighlighted);
	for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) ONodeAssignMatrixToIdentity (*itema);
	CListDelete           ((CList)list);
      }

}
/***************************************************************************/
static void CollectAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    XPoint  beg,end;
  } pointer = {{0,0},{0,0}};
/*.........................................................................*/
  switch(a_event->type)
    {
      case ButtonPress:
            pointer.end.x    = pointer.beg.x      = a_event->xbutton.x;
            pointer.end.y    = pointer.beg.y      = a_event->xbutton.y;
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
            break;
      case ButtonRelease:
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
	    if( (a_args[0]!=NULL) && (strcmp(a_args[0],"Shift")==0) ) 
	      XoCameraCollectInRegion (This,pointer.beg.x,pointer.beg.y,pointer.end.x,pointer.end.y,1);
	    else
	      XoCameraCollectInRegion (This,pointer.beg.x,pointer.beg.y,pointer.end.x,pointer.end.y,0);

	    {XoAnyCallbackStruct  value;
	     value.reason         = XoCR_COLLECT;
	     value.event          = a_event;
	     XtCallCallbacks      (This, XoNcollectCallback,(XtPointer)&value);}

            break;
      case MotionNotify:
/* Destroy previous box by redrawing it !!! (in mode COMP this is equivallent to a deletion ) */
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
            pointer.end.x = a_event->xmotion.x;
            pointer.end.y = a_event->xmotion.y;
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
            break;
    }
  a_argn = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static void ZoomAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    XPoint  beg,end;
  } pointer = {{0,0},{0,0}};
/*.........................................................................*/
  switch(a_event->type)
    {
      case ButtonPress:
            pointer.end.x    = pointer.beg.x      = a_event->xbutton.x;
            pointer.end.y    = pointer.beg.y      = a_event->xbutton.y;
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
            break;
      case ButtonRelease:
            XDisplayDrawRectangle   (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);

	   {double                  sx,sy;
	    OCameraPutMatrixOnStack (othis);
	    TranslateOnRectangle    (This, &pointer.beg, &pointer.end);
	    sx                      = pointer.end.x-pointer.beg.x;
	    sx                      = fabs(sx);
	    sy                      = pointer.end.y-pointer.beg.y;
	    sy                      = fabs(sy);
	    if((sx>0.) && (sy>0.) && (This->core.width>0) && (This->core.height>0) )
	      { 
		OMatrix               matrix;
		double                scale;
		sx                    = ((double)(This->core.width)) /sx;
		sy                    = ((double)(This->core.height))/sy;
		scale                 = MINIMUM(sx,sy);
		matrix                = OMatrixCreate(OMatrixScaleIsometric,scale);
		OCameraMultiplyMatrix (othis,matrix);
		OMatrixDelete         (matrix);
	      }}

            break;
      case MotionNotify:
/* Destroy previous box by redrawing it !!! (in mode COMP this is equivallent to a deletion ) */
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
            pointer.end.x = a_event->xmotion.x;
            pointer.end.y = a_event->xmotion.y;
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),athis.rubberGC,&pointer.beg,&pointer.end);
            break;
    }
  a_args = NULL; /* to remove warning of c++ compiler */
  a_argn = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static void TranslateOnRectangle (
 Widget  This
,XPoint* a_beg
,XPoint* a_end
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  transx,transy,fmin,fmax,scale;
  double  up_x ,up_y ,up_z;
  double  dop_x,dop_y,dop_z;
  double  ax_x,ax_y,ax_z;
  OMatrix matrix;
/*.........................................................................*/
  if(This->core.width==0)  return;
  if(This->core.height==0) return;

  transx = (fabs((double)a_end->x + (double)a_beg->x) - (double)This->core.width)/2.;
  transy = (fabs((double)a_end->y + (double)a_beg->y) - (double)This->core.height)/2.;

  OCameraGetUpVector (othis,&up_x ,&up_y ,&up_z);
  OCameraGetDOP      (othis,&dop_x,&dop_y,&dop_z);

  OCameraGetField (othis,&fmin,&fmax);
  scale           = fabs(fmax - fmin);

  if(OCameraGetAspectRatio(othis)==1)
    {
      if (This->core.width < This->core.height)
	{
	  transx = scale * transx / ((double)This->core.width);
	  transy = scale * transy / ((double)This->core.width);
	}
      else
	{
	  transx = scale * transx / ((double)This->core.height);
	  transy = scale * transy / ((double)This->core.height);
	}
    }
  else
    {
      transx = scale * transx / ((double)This->core.width);
      transy = scale * transy / ((double)This->core.height);
    }

  CVectorMultiply       (up_x,up_y,up_z,dop_x,dop_y,dop_z,&ax_x,&ax_y,&ax_z);
  matrix                = OMatrixCreate(OMatrixTranslationAxis,transx,ax_x,ax_y,ax_z);
  OCameraMultiplyMatrix (othis,matrix);
  OMatrixDelete         (matrix);

  matrix                = OMatrixCreate(OMatrixTranslationAxis,transy,up_x,up_y,up_z);
  OCameraMultiplyMatrix (othis,matrix);
  OMatrixDelete         (matrix);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ConvertStringToColorIndex (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*
  In cache it is the a_args at the out of this routine that are stored.
  Then, to avoid caching, it is sufficent to produce at each conversion
 a differant a_args set.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*      name;
  static int value = 0;
  static int count = 0;  /*To avoid caching.*/
/*.........................................................................*/
  if(*a_argn!=2) {ReportError("ConvertStringToColorIndex",CErrorFlow);return;}
  if(a_source->addr==NULL) return;
  a_args[1].addr = (XPointer)&count;count++; /*To avoid caching.*/
  name           = *((String*)a_args[0].addr);
  value          = OColormapGetColorNameIndex (OColormapGetIdentifier(name),a_source->addr);
  a_dest->addr   = (char *)&value;
  a_dest->size   = sizeof(int);
}
/***************************************************************************/
static void ConvertColorIndexToString (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*        name;
  int          index;
  char*        string; 
  static int count = 0;  /*To avoid caching.*/
/*.........................................................................*/
  if(*a_argn!=2) {ReportError("ConvertColorIndexToString",CErrorFlow);return;}
  a_args[1].addr = (XPointer)&count;count++; /*To avoid caching.*/
  name           = *((String*)a_args[0].addr);
  index          = *((int*)a_source->addr); 
  string         = OColormapGetIthColorName (OColormapGetIdentifier(name),index);
  if(string==NULL) return;
  strcpy         (Class.string,string);
  a_dest->size   = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr   = (XPointer)Class.string;      
}
/***************************************************************************/
static void ConvertBufferingTypeToString (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  string       = OBufferingGetName(*((OBuffering *)a_source->addr));
  if(string==NULL)  return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToBufferingType (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OBuffering value = OBufferingSimple;
/*.........................................................................*/
  value        = OBufferingGetIdentifier(a_source->addr);
  a_dest->addr = (char*)&value;
  a_dest->size = sizeof(OBuffering);
  a_args       = NULL;
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertProjectionTypeToString (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  string       = OProjectionGetName(*((OProjection *)a_source->addr));
  if(string==NULL)  return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL;
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToProjectionType (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OProjection value = OProjectionParallel;
/*.........................................................................*/
  value        = OProjectionGetIdentifier(a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OProjection);
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
static void ConvertSpecialProjectionToString (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  string       = OSpecialProjectionGetName(*((OSpecialProjection *)a_source->addr));
  if(string==NULL)  return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL;
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToSpecialProjection (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OSpecialProjection value = OSpecialProjectionXYZ;
/*.........................................................................*/
  value        = OSpecialProjectionGetIdentifier(a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OSpecialProjection);
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
static void ConvertDepthTestTypeToString (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  string       = ODepthTreatmentGetName(*((ODepthTreatment *)a_source->addr));
  if(string==NULL)  return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToDepthTestType (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static ODepthTreatment value = ODepthTreatmentNone;
/*.........................................................................*/
  value        = ODepthTreatmentGetIdentifier(a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(ODepthTreatment);
  a_args = NULL;
  a_argn = 0;
}
static struct {
  char         *name;
  OTransformation    ref;
} OTransformationList [] = {
  {"scale" ,     OTransformationScale},
  {"rotate",     OTransformationRotate},
  {"translate",  OTransformationTranslate}
};  
/***************************************************************************/
static OTransformation GetTransformationIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OTransformationList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OTransformationList[count].name)==0) ) 
      return OTransformationList[count].ref;
  ReportError  ("GetTransformationIdentifier",CErrorName);
  return OTransformationScale;
}
/***************************************************************************/
static char* GetTransformationName (
 OTransformation This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OTransformationList);count++)
    if(This==OTransformationList[count].ref) 
      return OTransformationList[count].name;
  return NULL;
}
/***************************************************************************/
static void ConvertMoveTypeToString (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  string       = GetTransformationName(*((OTransformation *)a_source->addr));
  if(string==NULL)  return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToTransformation (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OTransformation value = OTransformationScale;
/*.........................................................................*/
  value        = GetTransformationIdentifier (a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OTransformation);
  a_args = NULL;
  a_argn = 0;
}
static struct {
  char*    name;
  OMove    ref;
} OMoveList [] = {
  {"scene" ,        OMoveScene},
  {"camera",        OMoveCamera},
  {"highlight",     OMoveHighLight}
};  
/***************************************************************************/
static OMove GetMoveIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OMoveList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OMoveList[count].name)==0) ) 
      return OMoveList[count].ref;
  ReportError  ("GetMoveIdentifier",CErrorName);
  return OMoveScene;
}
/***************************************************************************/
static char* GetMoveName (
 OMove This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OMoveList);count++)
    if(This==OMoveList[count].ref) 
      return OMoveList[count].name;
  return NULL;
}
/***************************************************************************/
static void ConvertMoveWhatToString (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  string       = GetMoveName(*((OMove *)a_source->addr));
  if(string==NULL)  return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToMove (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OMove value = OMoveScene;
/*.........................................................................*/
  value        = GetMoveIdentifier(a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OMove);
  a_args = NULL;
  a_argn = 0;
}
static struct {
  char         *name;
  OAround  ref;
} OAroundList [] = {
  {"xy" ,     OAroundXY},
  {"z",       OAroundZ},
  {"axis",    OAroundAxis}
};  
/***************************************************************************/
static OAround GetAroundIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OAroundList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OAroundList[count].name)==0) ) 
      return OAroundList[count].ref;
  ReportError  ("GetAroundIdentifier",CErrorName);
  return OAroundXY;
}
/***************************************************************************/
static char* GetAroundName (
 OAround This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OAroundList);count++)
    if(This==OAroundList[count].ref) 
      return OAroundList[count].name;
  return NULL;
}
/***************************************************************************/
static void ConvertMoveAroundToString (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  string       = GetAroundName(*((OAround *)a_source->addr));
  if(string==NULL)  return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToAround (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OAround value = OAroundXY;
/*.........................................................................*/
  value        = GetAroundIdentifier (a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OAround);
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
static int SendMove (
 Widget This 
)
/***************************************************************************/
/*
  Never call this routine directly.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return XWidgetSendMessage (This,XMSG_MOVE,0L);
}
/***************************************************************************/
static int SendRefresh (
 Widget This
)
/***************************************************************************/
/*
  Never call this routine directly.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return XWidgetSendMessage (This,XMSG_REFRESH,0L);
}
/***************************************************************************/
static void SendToMove (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget* itema;
/*.........................................................................*/
  for(itema=athis.transformationSensitiveWidgets;(itema!=NULL) && (*itema!=NULL);itema++)
    {  
       if(XWidgetIsValid(*itema)==True) 
	 {
	   XWidgetSendMessage (*itema,XMSG_TO_MOVE,(long)This);
	}
    }
}
/***************************************************************************/
static void HandleMessage (
 Widget This
,XtPointer a_tag
,XEvent* a_event 
,Boolean* a_continue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_event->type!=ClientMessage) return;

/* dispatch */
       if(a_event->xclient.data.l[0]==XMSG_TO_MOVE)
    { 
      Widget  sender;
      sender  = (Widget) a_event->xclient.data.l[1];
      if(XWidgetIsValid(sender)==True)
	{
	  XoCameraCallbackStruct value;
	  value.reason           = XoCR_RECEIVE_TRANSFORM;
	  value.event            = a_event;
	  value.widget           = sender;
	  XtCallCallbacks        (This, XoNreceiveTransformCallback,(XtPointer)&value);
	}
    }
  else if(a_event->xclient.data.l[0]==XMSG_MOVE)
    {
      XoAnyCallbackStruct value;
      value.reason        = XoCR_TRANSFORM;
      value.event         = a_event;
      XtCallCallbacks     (This, XoNtransformCallback,(XtPointer)&value);
      SendToMove          (This);
      OCameraSetRequireMovementFunction  (othis,NULL,NULL,False);
    }
  else if(a_event->xclient.data.l[0]==XMSG_GO_REFRESH)
    {
#ifdef DEBUG
      printf              ("debug : XoCamera : receive GO_REFRESH\n");
#endif
      if(Class.goRefreshSent==1) 
	{
	  ONodeRefreshViews   ();
	  Class.goRefreshSent = 0;
	}
    }
  else if(a_event->xclient.data.l[0]==XMSG_REFRESH)
    {
      DoRefresh (This);
    }

  a_tag         = NULL;
  a_continue    = NULL;
}
/***************************************************************************/
static void DoRefresh (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCameraIsRefreshDone(othis)==0) return;
  if(MakeCurrent(This)==1) 
    {
      OCameraReceiveRefreshmentRequest (othis);
      glXSwapBuffers                   (XtDisplay(This),XtWindow(This));
    }
}
/***************************************************************************/
static void CreatePopup (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)        return;
  if(Class.popup!=NULL) return; /*Done.*/

#ifdef HAS_XM
 {Arg      args[1];
  XtSetArg (args[0],XmNwhichButton,Button3);
  if(athis.initialDepth!=This->core.depth) /*XmCreatePopupMenu can't create a window from a 24 plans one !!!*/
    {
      int                 depth;
      Colormap            colormap;
      depth               = This->core.depth;
      colormap            = This->core.colormap;
      This->core.depth    = athis.initialDepth;
      This->core.colormap = athis.initialColormap;
      Class.popup         = XmCreatePopupMenu (This,"xocamera_popup",args,1);
      This->core.depth    = depth;
      This->core.colormap = colormap;
    }
  else
    {
      Class.popup         = XmCreatePopupMenu (This,"xocamera_popup",args,1);
    }}
#endif /*HAS_XM*/

#ifdef HAS_XAW
 {Arg         args[1];
  Class.popup = XtCreatePopupShell ("xocamera_popup",simpleMenuWidgetClass,This,args,0);}
#endif /*HAS_XAW*/

  if(Class.popup==NULL) return;

#define ACTION_Destroy      1
#define ACTION_Rotate       2
#define ACTION_Translate    3
#define ACTION_Scale        4
#define ACTION_Reset        5
#define ACTION_Center       6
#define ACTION_Align        7
#define ACTION_SaveCTM      8
#define ACTION_Back         9
#define ACTION_Normalize   10
#define ACTION_Target      11
#define ACTION_ZBuffer     12
#define ACTION_DBuffer     13
#define ACTION_Cut         14
#define ACTION_Paste       15
#define ACTION_ODB         16
#define ACTION_GIF         17
#define ACTION_VRML        18
#define ACTION_PostScript  19
#define ACTION_POV         20
#define ACTION_DAWN        21
#define ACTION_Erase       22
  XtAddCallback             (Class.popup   ,XtNdestroyCallback ,ActivateCallback,(XtPointer)ACTION_Destroy);

/*Default menu.*/
  XoCameraAddPopupEntry     (This,"Rotate"    ,ActivateCallback,(XtPointer)ACTION_Rotate);
  XoCameraAddPopupEntry     (This,"Trans"     ,ActivateCallback,(XtPointer)ACTION_Translate);
  XoCameraAddPopupEntry     (This,"Scale"     ,ActivateCallback,(XtPointer)ACTION_Scale);
  XoCameraAddPopupEntry     (This,"Reset"     ,ActivateCallback,(XtPointer)ACTION_Reset);
  XoCameraAddPopupEntry     (This,"Center"    ,ActivateCallback,(XtPointer)ACTION_Center);
  XoCameraAddPopupEntry     (This,"Align"     ,ActivateCallback,(XtPointer)ACTION_Align);
  XoCameraAddPopupEntry     (This,"Save CTM"  ,ActivateCallback,(XtPointer)ACTION_SaveCTM);
  XoCameraAddPopupEntry     (This,"Back"      ,ActivateCallback,(XtPointer)ACTION_Back);
  XoCameraAddPopupEntry     (This,"Normalize" ,ActivateCallback,(XtPointer)ACTION_Normalize);
  XoCameraAddPopupEntry     (This,"Target"    ,ActivateCallback,(XtPointer)ACTION_Target);
  XoCameraAddPopupEntry     (This,"Zbuffer"   ,ActivateCallback,(XtPointer)ACTION_ZBuffer);
  XoCameraAddPopupEntry     (This,"Dbuffer"   ,ActivateCallback,(XtPointer)ACTION_DBuffer);
  XoCameraAddPopupEntry     (This,"Cut"       ,ActivateCallback,(XtPointer)ACTION_Cut);
  XoCameraAddPopupEntry     (This,"Paste"     ,ActivateCallback,(XtPointer)ACTION_Paste);
  if(CSystemIsSecured()==0)
    {
  XoCameraAddPopupEntry     (This,"odb"       ,ActivateCallback,(XtPointer)ACTION_ODB);
  XoCameraAddPopupEntry     (This,"GIF"       ,ActivateCallback,(XtPointer)ACTION_GIF);
  XoCameraAddPopupEntry     (This,"VRML"      ,ActivateCallback,(XtPointer)ACTION_VRML);
  XoCameraAddPopupEntry     (This,"PostScript",ActivateCallback,(XtPointer)ACTION_PostScript);
  XoCameraAddPopupEntry     (This,"POV"       ,ActivateCallback,(XtPointer)ACTION_POV);
  XoCameraAddPopupEntry     (This,"DAWN"      ,ActivateCallback,(XtPointer)ACTION_DAWN);
    }
  XoCameraAddPopupSeparator (This,"sep");
  XoCameraAddPopupEntry     (This,"Erase"     ,ActivateCallback,(XtPointer)ACTION_Erase);

}
/***************************************************************************/
static void ActivateCallback (
 Widget    This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera ogthis;
/*.........................................................................*/
  ogthis = (OCamera)XoCameraGetCamera(Class.This);
       if((long)a_tag==ACTION_Target)  XWidgetSetTarget                (Class.This);
  else if((long)a_tag==ACTION_SaveCTM) OCameraPutMatrixOnStack         (ogthis);
  else if((long)a_tag==ACTION_Back)    OCameraRestoreLastMatrixOfStack (ogthis);
  else if((long)a_tag==ACTION_Reset)   RestoreDefaults                 (Class.This);
  else if((long)a_tag==ACTION_Erase)  
    {
      ONode*                  list;
      list                    = ONodeCollect(OCameraGetNodes(ogthis),OCollectHighlighted);
      if( (list!=NULL) && (list[0]!=NULL) )     
	{
	  ONode*  itema;
	  for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) ONodeDelete (*itema);
	}
      else
	{
	  OCameraErase           (ogthis);
	}
      CListDelete              ((CList)list);
    }
  else if((long)a_tag==ACTION_Normalize) 
    {
      ONode*                  list;
      OCameraPutMatrixOnStack (ogthis);
      list                    = ONodeCollect(OCameraGetNodes(ogthis),OCollectHighlighted);
      if( (list!=NULL) && (list[0]!=NULL) )     
	OCameraNormalizeMatrix  (ogthis,list);
      else
	OCameraNormalizeMatrix  (ogthis,NULL);
      CListDelete              ((CList)list);
    }
  else if((long)a_tag==ACTION_ZBuffer)
    { 
      Arg          args[1];
      if(((XoCameraWidget)Class.This)->camera.depthTreatment==ODepthTreatmentNone)
        XtSetArg     (args[0],XoNdepthTreatment,ODepthTreatmentZBuffer);
      else
        XtSetArg     (args[0],XoNdepthTreatment,ODepthTreatmentNone);
      XtSetValues  (Class.This,args,1);
    }
  else if((long)a_tag==ACTION_DBuffer)
    { 
      Arg          args[1];
      if(((XoCameraWidget)Class.This)->camera.buffering==OBufferingSimple)
        XtSetArg     (args[0],XoNbuffering,OBufferingDouble);
      else
        XtSetArg     (args[0],XoNbuffering,OBufferingSimple);
      XtSetValues  (Class.This,args,1);
    }
  else if((long)a_tag==ACTION_Cut)
    {
      FILE*    file;
      ONode*   list;
      OType    otype;
      list     = ONodeCollect(OCameraGetNodes(ogthis),OCollectHighlighted);
      otype    = OTypeGetIdentifier("ONode");
      if( (list!=NULL) && (list[0]!=NULL) && (otype!=NULL) ) 
	{
	  char* name;
	  name  = CFileBuildTemporaryName(NULL);
	  file = (FILE*)OTypeOpenODB_FileForWriting (name);
	  if(file!=NULL) 
	    {
	      ONode*             itema;
	      for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) OTypePutObjectInODB_Stream (otype,*itema,file);
	    }
	  fclose             (file);
	  XSelectionSendFile (XtDisplay(Class.This),name);
	  remove             (name);
	}
      CListDelete ((CList)list);
    }
  else if((long)a_tag==ACTION_Paste)
    {
      Class.pasteProc          = XSelectionSetPasteFunction (PasteProc);
      XSelectionRequireData    (XtDisplay(Class.This));
    }
  else if((long)a_tag==ACTION_PostScript)
    {
      XWidgetSetCursor       (Class.This,"watch");
      XoCameraProducePS_File (Class.This,"out.ps");
      XWidgetSetCursor       (Class.This,NULL);
    }
  else if((long)a_tag==ACTION_GIF)
    { 
      XWidgetSetCursor            (Class.This,"watch");
      XWidgetSavePixmapInGIF_File (Class.This,"out.gif");
      XWidgetSetCursor            (Class.This,NULL);
    }
  else if((long)a_tag==ACTION_VRML)
    { 
      XWidgetSetCursor            (Class.This,"watch");
      OCameraProduceVRML_File     (ogthis,"out.wrl");
      XWidgetSetCursor            (Class.This,NULL);
    }
  else if((long)a_tag==ACTION_POV)
    { 
      XWidgetSetCursor            (Class.This,"watch");
      XoCameraProducePOV_File     (Class.This,"out.pov");
      XWidgetSetCursor            (Class.This,NULL);
    }
  else if((long)a_tag==ACTION_DAWN)
    { 
      XWidgetSetCursor            (Class.This,"watch");
      XoCameraProduceDAWN_File    (Class.This,"out.prim");
      XWidgetSetCursor            (Class.This,NULL);
    }
  else if((long)a_tag==ACTION_ODB)
    { 
      FILE*    file;
      ONode*   list;
      OType    otype;
/*    list     = ONodeCollect(OCameraGetNodes(ogthis),OCollectHighlighted);*/
      list     = OCameraGetNodes(ogthis);
      otype    = OTypeGetIdentifier("ONode");
      if( (list!=NULL) && (list[0]!=NULL) && (otype!=NULL) ) 
	{
	  file = (FILE*)OTypeOpenODB_FileForWriting ("out.odb");
	  if(file!=NULL) 
	    {
	      ONode*             itema;
	      for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) OTypePutObjectInODB_Stream (otype,*itema,file);
	    }
	  fclose (file);
	}
      /*CListDelete ((CList)list);*/
    }
  else if((long)a_tag==ACTION_Center) 
    {
      XoCameraCenterMatrixOnPick (Class.This);
    }
  else if((long)a_tag==ACTION_Align) 
    {
      OCameraPutMatrixOnStack   (ogthis);
      OCameraAlignMatrixOnPick  (ogthis,OCameraGetPick(ogthis,0),OCameraGetPick(ogthis,1));
    }
  else if((long)a_tag==ACTION_Destroy)
    { 
      Class.popup = NULL;
    }
  else if((long)a_tag==ACTION_Rotate)
    {
      XWidgetSetResource        (Class.This,XoNtransformation,(XtArgVal)OTransformationRotate);  
    }
  else if((long)a_tag==ACTION_Translate)
    {
      XWidgetSetResource        (Class.This,XoNtransformation,(XtArgVal)OTransformationTranslate);  
    }
  else if((long)a_tag==ACTION_Scale)
    {
      XWidgetSetResource        (Class.This,XoNtransformation,(XtArgVal)OTransformationScale);  
    }

  This   = NULL;
  a_tag  = NULL;
  a_data = NULL;
}
/***************************************************************************/
static void PasteProc (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  fileName;
  OType  otype;
/*.........................................................................*/
  fileName     = CFileBuildTemporaryName(NULL);
  XSelectionPutBufferInFile (fileName);
  otype        = OTypeGetIdentifier("ONode");
  if(otype!=NULL)
    {
      OHandle*  hands;
      OCamera ogthis;
      ogthis  = (OCamera)XoCameraGetCamera(Class.This);
      hands   = OTypeGetExtentFromFile(fileName,"odb",NULL);
      if(hands!=NULL)
	{
	  OHandle*  handa;
	  for(handa=hands;*handa!=NULL;handa++)
	    {
	      if((OType)OHandleGetType(*handa)==otype)
		{
		  OCameraViewNode (ogthis,(ONode)OHandleGetIdentifier(*handa));
		}
	    }
	}
      CListDestroy ((CList)hands,(CListVisitEntryFunction)OHandleDelete);
    }
  remove              (fileName);
  XSelectionSetPasteFunction (Class.pasteProc);
}
/***************************************************************************/
static void PopupAction (
 Widget    This 
,XEvent*   a_event 
,String*   a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_event==NULL)                    return;
  if(a_event->type!=ButtonPress)       return;
  if(a_event->xbutton.button!=Button3) return;
  if(Class.popupEnabled==0)            return;

  CreatePopup (This);

  if(Class.popup!=NULL)
    { 
#ifdef HAS_XM
      XmMenuPosition    (Class.popup,(XButtonPressedEvent*)a_event);
      XtManageChild     (Class.popup);
#endif /*HAS_XM*/
#ifdef HAS_XAW
     {Arg                 args[2];
      XtSetArg            (args[0], XtNx, ((XButtonPressedEvent*)a_event)->x_root);
      XtSetArg            (args[1], XtNy, ((XButtonPressedEvent*)a_event)->y_root);
      XtSetValues         (Class.popup, args,2);
      XtPopupSpringLoaded (Class.popup);}
#endif /*HAS_XM*/
      Class.This          = This;
      Class.This_event    = *a_event;
    }
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
static void SetGoRefreshProc (
 Display* a_display
)
/***************************************************************************/
/*
  Should have one mailbox per Display !
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg                    args[2];
/*.........................................................................*/
  if(Class.goMbx!=NULL) return; /*done*/

  XtSetArg               (args[0],XtNgeometry,"100x100+0+0");
  Class.goMbx            = XtAppCreateShell 
                           ("goRefresh","GoRefresh",topLevelShellWidgetClass,a_display,args,1);
  if(Class.goMbx==NULL)   
    {
      CWarn ("XoCamera: unable to create mailbox widget for Go package.\n");
      return;
    }

  XtSetMappedWhenManaged   (Class.goMbx,False);
  XtRealizeWidget          (Class.goMbx);  /* must be realize to have a window */


  XtAddEventHandler        (Class.goMbx,0L,True,HandleMessage,NULL);

  ONodeSetViewFunctions    ((ONodeVoidFunction)SendToGo,
			    (ONodeIsValidFunction)OCameraIsValid,
			    (ONodeRefreshFunction)OCameraRequireRefreshment,
			    (ONodeRefreshFunction)OCameraRequireDrawing);

}
/***************************************************************************/
static void SendToGo (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.goRefreshSent==1)  return;

#ifdef DEBUG
  printf                   ("debug : XoCamera : Send GO_REFRESH.\n");
#endif

  XWidgetSendMessage       (Class.goMbx,XMSG_GO_REFRESH,0L);

  Class.goRefreshSent      = 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void* XoCameraGetCamera (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return NULL;
  if(OCameraIsValid(othis)==0) return NULL;
  return     (void*)othis;
}
/***************************************************************************/
int XoCameraCollectInRegion (
 Widget This
,int a_beg_x
,int a_beg_y
,int a_end_x
,int a_end_y
,int a_add
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPick   pick;
  ONode*  list = NULL;
  int     retval = 0;
/*.........................................................................*/
  if(This==NULL)                              return 0;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return 0;
  if(MakeCurrent(This)==0)                    return 0;
  list   = (ONode*)OCameraGetNodesInRegion(othis,a_beg_x,a_beg_y,a_end_x,a_end_y,&pick);
  retval = list!=NULL ? 1 : 0;
  if(a_add==1)
    {
      ONode*             itema;
      for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) 
	{
	  ONodeToggleHighlight           (*itema);
	  OCameraSetNextRefreshOnTouched (othis);
	}
      OCameraAddPick                       (othis,pick);
    }
  else
    {
      ONode*             itema;
      for(itema=OCameraGetNodes(othis);(itema!=NULL) && (*itema!=NULL);itema++) 
	{
	  int               highLight;
	  highLight         = ONodeGetHighlight (*itema); 
	  ONodeSetHighlight (*itema,0);
	  if(highLight==1) 
	    {
	      OCameraSetNextRefreshOnTouched (othis);
	    }
	}
      for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) 
	{
	  int               highLight;
	  highLight         = ONodeGetHighlight (*itema); 
	  ONodeSetHighlight (*itema,1);
	  if(highLight==0) 
	    {
	      OCameraSetNextRefreshOnTouched (othis);
	    }
	}
      OCameraDeletePicks                   (othis);
      OCameraAddPick                       (othis,pick);
    }
  CListDelete ((CList)list);
  return      retval;
}
/***************************************************************************/
void XoCameraCancelGoRefresh (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.goRefreshSent      = 0;
}
/***************************************************************************/
void XoCameraRefreshNow (
 Widget This 
)
/***************************************************************************/
/*
  Force to refresh immediatly.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return;
  OCameraSetRequireRefreshmentFunction (othis,NULL,NULL,1);
  DoRefresh                           (This);
}
/***************************************************************************/
void XoCameraProducePS_File (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return;
  if(OCameraGetDepthTreatment(othis)==ODepthTreatmentNone)
    {
      OCameraProducePS_File (othis,a_name);
    }
  else /*ZBuffer*/
    {
      CPS_OpenFileForWriting          (a_name);
      CPS_SetBitsPerPixel             (8);
      CPS_SetColorScheme              (CPS_Color);
      XWidgetPutPixmapPageInPS_Stream (This);
      CPS_CloseStream                 ();
    }
}
/***************************************************************************/
void XoCameraProducePOV_File (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return;
  OCameraProducePOV_File (othis,a_name);
}
/***************************************************************************/
void XoCameraProduceDAWN_File (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return;
  OCameraProduceDAWN_File (othis,a_name);
}
/***************************************************************************/
void XoCameraPutSceneInPS_Stream (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Dimension ww,wh;
  double    red,green,blue;
/*.........................................................................*/
  if(This==NULL)          return;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return;
  if(OCameraGetDepthTreatment(othis)==ODepthTreatmentNone)
    {
      XWidgetGetDimension         (This,&ww,&wh);
      XDisplayGetPixelRGB         (XtDisplay(This),XWidgetGetPixelResource(This,"background"),&red,&green,&blue);
      CPS_PutBackgroundInStream   (red,green,blue,(double)ww,(double)wh);
      OCameraPutSceneInPS_Stream  (othis);
      XDisplayGetPixelRGB         (XtDisplay(This),XWidgetGetPixelResource(This,"borderColor"),&red,&green,&blue);
      CPS_PutFrameInStream        (red,green,blue,(double)ww,(double)wh);
    }
  else /*ZBuffer*/
    {
      XWidgetPutPixmapInPS_Stream (This);
    }
}
/***************************************************************************/
void XoCameraCenterMatrixOnPick (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return;
  OCameraPutMatrixOnStack   (othis);
  OCameraCenterMatrixOnPick (othis,OCameraGetPick(othis,0));
}
/***************************************************************************/
void XoCameraBecomeTarget (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return;
  OCameraSetCurrent  (othis);
}
/***************************************************************************/
void* XoCameraGetPick (
 Widget This 
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return NULL;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return NULL;
  return OCameraGetPick (othis,a_number);
}
/***************************************************************************/
void XoCameraSetVirtualColormap (
 Widget This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return;
  CStringDelete         (athis.virtualColormap);
  athis.virtualColormap = CStringDuplicate (a_string);
  OCameraSetColormap    (othis,OColormapGetIdentifierFromString(athis.virtualColormap));
}
/***************************************************************************/
CList XoCameraGetHighlightedNodes (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return NULL;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return NULL;
  return (CList)OCameraCollect (othis,OCollectHighlighted);
}
/***************************************************************************/
CList XoCameraGetNodes (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return NULL;
  if(!XtIsSubclass(This,xoCameraWidgetClass)) return NULL;
  return (CList)OCameraCollect (othis,OCollectAll);
}
/***************************************************************************/
void XoCameraDisablePopup (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.popup==NULL) return;
  Class.popupEnabled   = 0;
#ifdef HAS_XM
 {Arg          args[1];
  XtSetArg     (args[0],XmNwhichButton,Button5); /*Nobody has five buttons!*/
  XtSetValues  (Class.popup,args,1);}
#endif /*HAS_XM*/
#ifdef HAS_XAW
#endif /*HAS_XAW*/
}
/***************************************************************************/
void XoCameraEnablePopup (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.popup==NULL) return;
  Class.popupEnabled   = 1;
#ifdef HAS_XM
 {Arg          args[1];
  XtSetArg     (args[0],XmNwhichButton,Button3);
  XtSetValues  (Class.popup,args,1);}
#endif /*HAS_XM*/
#ifdef HAS_XAW
#endif /*HAS_XAW*/
}
/***************************************************************************/
void XoCameraAddPopupEntry (
 Widget This
,char* a_label
,XtCallbackProc a_proc
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*             name;
  Arg               args[1];
  Widget            widget = NULL;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_label==NULL) return;
  if(a_proc==NULL)  return;
  CreatePopup       (This);
  if(Class.popup==NULL) return;
  if(FindPopupEntry(a_label)!=NULL) return;
  name             = CStringCreateF (15+strlen(a_label),"xocamera_popup_%s",a_label);
#ifdef HAS_XM
  widget           = XmCreatePushButtonGadget (Class.popup,name,args,0);
 {XmString         cps;
  cps              = XmStringLtoRCreate(a_label,XmSTRING_DEFAULT_CHARSET);
  XtSetArg         (args[0],XmNlabelString,cps);
  XtSetValues      (widget,args,1);
  XmStringFree     (cps);}
  XtAddCallback    (widget,XmNactivateCallback,a_proc,(XtPointer)a_data);
#endif /*HAS_XM*/
#ifdef HAS_XAW
  widget           = XtCreateManagedWidget(name,smeBSBObjectClass,Class.popup,args,0);
  XtSetArg         (args[0],XtNlabel,a_label);
  XtSetValues      (widget,args,1);
  XtAddCallback    (widget,XtNcallback,a_proc,(XtPointer)a_data);
#endif /*HAS_XM*/
  if(widget!=NULL) XtManageChild    (widget);
  CStringDelete    (name);
}
/***************************************************************************/
void XoCameraAddPopupSeparator (
 Widget This
,char* a_label
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*             name;
  Arg               args[1];
  Widget            widget = NULL;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_label==NULL) return;
  CreatePopup       (This);
  if(Class.popup==NULL) return;
  if(FindPopupEntry(a_label)!=NULL) return;
  name             = CStringCreateF (15+strlen(a_label),"xocamera_popup_%s",a_label);
#ifdef HAS_XM
  widget           = XmCreateSeparatorGadget (Class.popup,name,args,0);
#endif /*HAS_XM*/
#ifdef HAS_XAW
  widget           = XtCreateWidget(name,smeLineObjectClass,Class.popup,args,0);
#endif /*HAS_XM*/
  if(widget!=NULL) XtManageChild    (widget);
  CStringDelete    (name);
}
/***************************************************************************/
void XoCameraRemovePopupEntry (
 Widget This
,char* a_label
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget          entry;
/*.........................................................................*/
  entry           = FindPopupEntry(a_label);
  if(entry==NULL) return;
  XtDestroyWidget (entry);
  This            = NULL;
}
/***************************************************************************/
static Widget FindPopupEntry (
 char* a_label
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList items,itema;
  char*      name;
  Widget     entry = NULL;
/*.........................................................................*/
  if(a_label==NULL)     return NULL;
  if(Class.popup==NULL) return NULL;
  name  = CStringCreateF (15+strlen(a_label),"xocamera_popup_%s",a_label);
  items = XWidgetGetChildren (Class.popup,NULL);
  entry = NULL;
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) 
    {
      char*  string;
      string = XtName(*itema);
      if( (string!=NULL) && (strcmp(name,string)==0) ) /*Already exists*/
	{
	  entry = *itema;
          break;
	}
    }
  CListDelete   ((CList)items);
  CStringDelete (name);
  return        entry;
}
/***************************************************************************/
static int MakeCurrent (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return 0;
  if(!XtIsRealized(This)) return 0;

#ifdef HAS_GL
 {
   OBuffering   buffering;
   buffering    = OCameraGetBuffering   (othis);
   if(buffering==OBufferingDouble)
     {
       if(Class.glContext2==NULL) return 0;
       return (int)glXMakeCurrent (XtDisplay(This),XtWindow(This),Class.glContext2);
     }
   else /*OBufferingSimple*/
     {
       if(Class.glContext1==NULL) return 0;
       return (int)glXMakeCurrent (XtDisplay(This),XtWindow(This),Class.glContext1);
     }
 }
#else 

  return 0;

#endif /*HAS_GL*/

}
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
  CErrorHandle ("Xo","XoCamera",a_name,a_code);
}
