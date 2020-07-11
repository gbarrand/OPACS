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
#ifdef HAS_LESSTIF
#define HAS_XAW
#undef HAS_XM
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>

#ifdef HAS_XM
#include <Xm/Text.h>        /*PLOTTER_TITLE,PLOTTER_STATS,PLOTTER_FIT*/
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#endif /*HAS_XM*/

#ifdef HAS_XAW
#include <Xaw/AsciiText.h>        /*PLOTTER_TITLE,PLOTTER_STATS,PLOTTER_FIT*/
#include <Xaw/SimpleMenu.h>   
#include <Xaw/SmeBSB.h>   
#include <Xaw/SmeLine.h>  
#endif

/*Co*/
#include <CSystem.h>
#include <CString.h>
#include <CText.h>
#include <CMemory.h>
#include <CList.h>
#include <CError.h>
#include <CPrinter.h>
#include <OType.h>

/*Ho*/
#include <OCut.h>
#include <OFit.h>
#include <OHistogram.h>
#include <OFunction.h>
#include <Ho.h>

/*Go*/
#include <GoTypes.h>
#include <OCamera.h>

/*Xx*/
#include <XWidget.h> /*For XShape.*/

/*Xo*/
#include <OPlotter.h>

/*Private.*/
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>

#include <XoShapeP.h>
#include <XoCameraP.h>

typedef struct
{
    int     reason;
    XEvent* event;
} XoAnyCallbackStruct;

typedef enum {
    OBarNone,
    OBarSigma,
    OBarMeanError
} OBarType;

typedef enum {
    OReceiverHistogram,
    OReceiverScatter,
    OReceiverProfile
} OReceiver;

#include <XoPlotterP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void       InitializeClass               (void);
static void       InitializeWidget              (Widget,Widget,ArgList,Cardinal*);
static void       RealizeWidget                 (Widget, XtValueMask*, XSetWindowAttributes*);
static void       DestroyWidget                 (Widget);
static void       ChangeWidgetSize              (Widget);
static Boolean    SetValues                     (Widget, Widget, Widget,ArgList,Cardinal*);

static int        SendBuild                     (Widget);
static void       HandleMessage                 (Widget, XtPointer, XEvent*,Boolean*);
static void       DoRepresentation              (Widget);
static void       CreatePlottedObjectList       (Widget);
static void       DeletePlottedObjectList       (Widget);
static void       CreateCutList                 (Widget);
static void       DeleteCutList                 (Widget);
static OFunction* CreateFunctionList            (Widget,char*);
static void       UpdateAxes                    (Widget);
static char*      FitHistogram                  (Widget,OHistogram,int,int*,int*,char**);
static void       PutHistogramParametersOnStack (Widget,OHistogram);
static void       ConvertStringToFitType        (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void       ConvertFitTypeToString        (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);

static void       ConvertStringToReceiver       (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void       ConvertHistTypeToString       (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);

static void       ConvertStringToBarType        (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);
static void       ConvertBarTypeToString        (XrmValuePtr, int*, XrmValuePtr, XrmValuePtr);

static void       ConvertStringToLayout         (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);
static void       ConvertLayoutToString         (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);

static void       ConvertStringToXY_Modeling    (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);
static void       ConvertXY_ModelingToString    (XrmValuePtr, int* ,XrmValuePtr, XrmValuePtr);

static void       SetChildrenGeometry           (Widget);
static void       CollectAction                 (Widget,XEvent*, String* ,Cardinal*);
static void       PopupAction                   (Widget,XEvent*,String*,Cardinal*);
static void       CreatePopup                   (Widget);
static void       ActivateCallback              (Widget,XtPointer,XtPointer);
static char**     GetBarTypeNames               (int*);
static OBarType   GetBarTypeIdentifier          (char*);
static char*      GetBarTypeName                (OBarType);
static char**     GetReceiverNames              (int*);
static OReceiver  GetReceiverIdentifier         (char*);
static char*      GetReceiverName               (OReceiver);
static char*      GetDefaultTitle               (int,char*,char*,int);
static char*      GetHistogramInfos             (OHistogram,char*,char*,Boolean);
static Widget     FindPopupEntry                (char*);
static void       ReportError                   (char*,int);
#ifdef __cplusplus
}
#endif

#define NotFound     (-1)
#define DBL_INT      (int)(float)
#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)

#define athis      ((XoPlotterWidget)This)->plot
#define othis      ((XoPlotterWidget)This)->plot.oplot
#define acur       ((XoPlotterWidget)a_current)->plot
#define thisCamera ((XoPlotterWidget)This)->camera
#define IFMOD(a_field)  if(athis.a_field != acur.a_field)
#define IFSMOD(a_field) if(CStringCompare(athis.a_field,acur.a_field)==0)

#define MAX_STRING 128
static struct 
{
  Widget popup;
  Widget This;
  char   string[MAX_STRING];
  XEvent This_event;
} Class = {NULL,NULL,""};

static char translations[] =
            "\
           Shift Ctrl <Btn1Down>: CameraZoom()\n\
   Shift Ctrl Button1 <PtrMoved>: CameraZoom()\n\
             Shift Ctrl <Btn1Up>: CameraZoom()\n\
                 Ctrl <Btn1Down>: CameraMove()\n\
         Ctrl Button1 <PtrMoved>: CameraMove()\n\
                   Ctrl <Btn1Up>: CameraMove()\n\
                Shift <Btn1Down>: PlotterCollect(Shift)\n\
        Shift Button1 <PtrMoved>: PlotterCollect(Shift)\n\
                  Shift <Btn1Up>: PlotterCollect(Shift)\n\
                      <Btn1Down>: PlotterCollect(None)\n\
              Button1 <PtrMoved>: PlotterCollect(None)\n\
                        <Btn1Up>: PlotterCollect(None)\n\
                      <Btn3Down>: PlotterPopup()";

static XtActionsRec actions[] = 
{
  { "PlotterCollect"   , CollectAction},
  { "PlotterPopup"     , PopupAction}
};

static XtResource resources [] ={
  {XoNlayout  ,"Layout" , XoRLayout, sizeof(OLayout),                                   /* Set to a not valid layout,*/
                  XtOffset(XoPlotterWidget, plot.layout) ,XtRImmediate,(XtPointer)-1},  /*so that layoutAutomated is default.*/
  {XoNaspectRatio    ,"AspectRatio"   , XtRBoolean   , sizeof(Boolean)    ,
                  XtOffset(XoPlotterWidget, plot.aspectRatio)   ,XtRImmediate,(XtPointer)False},
  {XoNwallsVisible    ,"WallsVisible"   , XtRBoolean   , sizeof(Boolean)    ,
                  XtOffset(XoPlotterWidget, plot.wallsVisible)   ,XtRImmediate,(XtPointer)True},
  {XoNgridsVisible    ,"GridsVisible"   , XtRBoolean   , sizeof(Boolean)    ,
                  XtOffset(XoPlotterWidget, plot.gridsVisible)   ,XtRImmediate,(XtPointer)True},
  {XoNgridContext      ,"GridContext"   , XtRString   , sizeof(String)        ,
                  XtOffset(XoPlotterWidget, plot.gridContext)   ,XtRString ,"color black lineStyle dotted"},
  {XoNwallContext      ,"WallContext"   , XtRString   , sizeof(String)        ,
                  XtOffset(XoPlotterWidget, plot.wallContext)   ,XtRString ,"color wheat modeling solid"},
  {XoNtextContext      ,"TextContext"   , XtRString   , sizeof(String)        ,
                  XtOffset(XoPlotterWidget, plot.textContext)   ,XtRString ,"color black"},
  {XoNhistogramContext      ,"HistogramContext"   , XtRString   , sizeof(String)        ,
                  XtOffset(XoPlotterWidget, plot.histogramContext)   ,XtRString ,
                  "color brown lineStyle solid areaStyle solid markerStyle dot"},
  {XoNsecondHistogramContext     ,"SecondHistogramContext"   , XtRString   , sizeof(String)        ,
                  XtOffset(XoPlotterWidget, plot.secondHistogramContext)   ,XtRString ,
                  "color coral lineStyle solid areaStyle solid markerStyle dot"},
  {XoNfunctionContext      ,"FunctionContext"   , XtRString   , sizeof(String)        ,
                  XtOffset(XoPlotterWidget, plot.functionContext)   ,XtRString ,
                  "color black lineStyle solid areaStyle solid"},
  {XoNsecondFunctionContext     ,"SecondFunctionContext"   , XtRString   , sizeof(String)        ,
                  XtOffset(XoPlotterWidget, plot.secondFunctionContext)   ,XtRString ,
                  "color coral lineStyle solid areaStyle solid"},
  {XoNscatterContext      ,"ScatterContext"   , XtRString   , sizeof(String)        ,
                  XtOffset(XoPlotterWidget, plot.scatterContext)   ,XtRString ,
                  "color brown lineStyle solid markerStyle dot modeling cloud"},
  {XoNsecondScatterContext     ,"SecondScatterContext"   , XtRString   , sizeof(String)        ,
                  XtOffset(XoPlotterWidget, plot.secondScatterContext)   ,XtRString ,
                  "color coral lineStyle solid areaStyle solid markerStyle dot modeling cloud"},
  {XoNplottedObjects      , "PlottedObjects"        , XtRString   , sizeof(String) ,
                  XtOffset(XoPlotterWidget, plot.plottedObjects),XtRString  ,NULL},
  {XoNstatisticsVisible   ,"StatisticsVisible"     , XtRBoolean   , sizeof(Boolean)    ,
                  XtOffset(XoPlotterWidget, plot.statisticsVisible)   ,XtRImmediate,(XtPointer)False},
  {XoNstatisticsShort     ,"StatisticsShort"     , XtRBoolean   , sizeof(Boolean)    ,
                  XtOffset(XoPlotterWidget, plot.statisticsShort)   ,XtRImmediate,(XtPointer)True},
  {XoNtitleEditable   ,"TitleEditable"     , XtRBoolean   , sizeof(Boolean)    ,
                  XtOffset(XoPlotterWidget, plot.titleEditable)   ,XtRImmediate,(XtPointer)False},
  {XoNtitlePosition   ,"TitlePosition"     , XtRString  , sizeof(String)    ,
                  XtOffset(XoPlotterWidget, plot.titlePosition)   ,XtRString,(XtPointer)"up"},
  {XoNbinBarType  ,"BinBarType"  , XoRBinBarType  , sizeof(OBarType) ,
                            XtOffset(XoPlotterWidget, plot.binBarType)   ,XtRImmediate,(XtPointer)OBarNone},
  {XoNtupleDataReceiver ,"TupleDataReceiver" , XoRReceiver , sizeof(OReceiver),
                            XtOffset(XoPlotterWidget,  plot.tupleDataReceiver) ,XtRImmediate,(XtPointer)OReceiverHistogram},
  {XoNnumberOfBoundColumns  ,"NumberOfBoundColumns", XtRInt      , sizeof(int)      ,
                            XtOffset(XoPlotterWidget,  plot.numberOfBoundColumns)  ,XtRImmediate,(XtPointer)1},
  {XoNfunctionDimension  ,"FunctionDimension", XtRInt      , sizeof(int)      ,
                            XtOffset(XoPlotterWidget,  plot.functionDimension)  ,XtRImmediate,NULL},
  {XoNcuts     ,"Cuts"    , XtRString   , sizeof(String)   ,
                            XtOffset(XoPlotterWidget,  plot.cuts)     ,XtRString   ,NULL},
  {XoNfunctions,"Functions", XtRString   , sizeof(String)   ,
                            XtOffset(XoPlotterWidget,  plot.functions),XtRString   ,NULL},
  {XoNvalueColorMapping          ,"ValueColorMapping"   , XtRString   , sizeof(String)        ,
                            XtOffset(XoPlotterWidget, plot.valueColorMapping)   ,XtRString , NULL},
  {XoNtitle    ,"Title"    , XtRString   , sizeof(String)   ,
                            XtOffset(XoPlotterWidget,  plot.title)    ,XtRString   ,NULL},
  {XoNtextScale, "TextScale"          , XtRString   , sizeof(String)  ,
                            XtOffset(XoPlotterWidget, plot.textScale) ,XtRString,(XtPointer)"1."},
  {XoNtuple     ,"Tuple"    , XtRString   , sizeof(String)   ,
                            XtOffset(XoPlotterWidget,  plot.stuple)    ,XtRString   ,NULL},
  {XoNxyModeling ,"XY_Modeling" , XoRXY_Modeling, sizeof(OXY_Modeling), 
                  XtOffset(XoPlotterWidget, plot.xyModeling) ,XtRImmediate,(XtPointer)OXY_ModelingRandom}, 
  {XoNbarsVisible   ,"BarsVisible"     , XtRBoolean   , sizeof(Boolean)    ,
                  XtOffset(XoPlotterWidget, plot.barsVisible)   ,XtRImmediate,(XtPointer)False},

  {XoNxAxisBinding       , "XAxisBinding", XtRString,sizeof(String),
                            XtOffset(XoPlotterWidget, plot.xAxisBinding),XtRString,"0"},
  {XoNxAxisAutomatic       , "XAxisAutomatic"         , XtRBoolean  , sizeof(Boolean),
                            XtOffset(XoPlotterWidget, plot.xAxisAutomatic),XtRImmediate,(XtPointer)True},
  {XoNxAxisLogScale        , "XAxisLogScale"          , XtRBoolean  , sizeof(Boolean),
                            XtOffset(XoPlotterWidget, plot.xAxisLogScale) ,XtRImmediate,(XtPointer)False},
  {XoNxAxisNumberOfBins       , "XAxisNumberOfBins"         , XtRInt      , sizeof(int)    ,
                            XtOffset(XoPlotterWidget, plot.xAxisNumberOfBins),XtRImmediate,NULL},
  {XoNxAxisMinimum        , "XAxisMinimum"          , XtRString   , sizeof(String)  ,
                            XtOffset(XoPlotterWidget, plot.xAxisMinimum) ,XtRString,(XtPointer)"0."},
  {XoNxAxisMaximum        , "XAxisMaximum"          , XtRString   , sizeof(String)  ,
                            XtOffset(XoPlotterWidget, plot.xAxisMaximum) ,XtRString,(XtPointer)"0."},

  {XoNyAxisBinding       , "YAxisBinding", XtRString,sizeof(String),
                            XtOffset(XoPlotterWidget, plot.yAxisBinding),XtRString,"1"},
  {XoNyAxisAutomatic       , "YAxisAutomatic"         , XtRBoolean  , sizeof(Boolean),
                            XtOffset(XoPlotterWidget, plot.yAxisAutomatic),XtRImmediate,(XtPointer)True},
  {XoNyAxisLogScale        , "YAxisLogScale"          , XtRBoolean  , sizeof(Boolean),
                            XtOffset(XoPlotterWidget, plot.yAxisLogScale) ,XtRImmediate,(XtPointer)False},
  {XoNyAxisNumberOfBins       , "YAxisNumberOfBins"         , XtRInt      , sizeof(int)    ,
                            XtOffset(XoPlotterWidget, plot.yAxisNumberOfBins),XtRImmediate,NULL},
  {XoNyAxisMinimum        , "YAxisMinimum"          , XtRString   , sizeof(String)  ,
                            XtOffset(XoPlotterWidget, plot.yAxisMinimum) ,XtRString   ,(XtPointer)"0."},
  {XoNyAxisMaximum        , "YAxisMaximum"          , XtRString   , sizeof(String)  ,
                            XtOffset(XoPlotterWidget, plot.yAxisMaximum) ,XtRString   ,(XtPointer)"0."},

  {XoNzAxisBinding       , "ZAxisBinding", XtRString,sizeof(String),
                            XtOffset(XoPlotterWidget, plot.zAxisBinding),XtRString,"2"},
  {XoNzAxisAutomatic       , "ZAxisAutomatic"         , XtRBoolean  , sizeof(Boolean),
                            XtOffset(XoPlotterWidget, plot.zAxisAutomatic),XtRImmediate,(XtPointer)True},
  {XoNzAxisLogScale        , "ZAxisLogScale"          , XtRBoolean  , sizeof(Boolean),
                            XtOffset(XoPlotterWidget, plot.zAxisLogScale) ,XtRImmediate,(XtPointer)False},
  {XoNzAxisNumberOfBins       , "ZAxisNumberOfBins"         , XtRInt      , sizeof(int)    ,
                            XtOffset(XoPlotterWidget, plot.zAxisNumberOfBins),XtRImmediate,NULL},
  {XoNzAxisMinimum        , "zAxisMinimum"          , XtRString   , sizeof(String)  ,
                            XtOffset(XoPlotterWidget, plot.zAxisMinimum) ,XtRString   ,(XtPointer)"0."},
  {XoNzAxisMaximum        , "ZAxisMaximum"          , XtRString   , sizeof(String)  ,
                            XtOffset(XoPlotterWidget, plot.zAxisMaximum) ,XtRString   ,(XtPointer)"0."},

  {XoNvAxisAutomatic       , "VAxisAutomatic"         , XtRBoolean  , sizeof(Boolean),
                           XtOffset(XoPlotterWidget, plot.vAxisAutomatic),XtRImmediate,(XtPointer)True},
  {XoNvAxisLogScale        , "VAxisLogScale"          , XtRBoolean  , sizeof(Boolean),
                           XtOffset(XoPlotterWidget, plot.vAxisLogScale) ,XtRImmediate,(XtPointer)False},
  {XoNvAxisNumberOfBins       , "VAxisNumberOfBins"         , XtRInt      , sizeof(int)    ,
                            XtOffset(XoPlotterWidget, plot.vAxisNumberOfBins),XtRImmediate,NULL},
  {XoNvAxisMinimum        , "VAxisMinimum"          , XtRString   , sizeof(String)  ,
                           XtOffset(XoPlotterWidget, plot.vAxisMinimum) ,XtRString   ,(XtPointer)"0."},
  {XoNvAxisMaximum        , "VAxisMaximum"          , XtRString   , sizeof(String)  ,
                           XtOffset(XoPlotterWidget, plot.vAxisMaximum) ,XtRString   ,(XtPointer)"0."},
  {XoNvAxisTitle          , "VAxisTitle"          , XtRString   , sizeof(String)  ,
                           XtOffset(XoPlotterWidget, plot.vAxisTitle) ,XtRString   ,(XtPointer)NULL},

  {XoNfitType       ,"FitType"   , XoRFitType , sizeof(OFitType),
                                   XtOffset(XoPlotterWidget, plot.fitType)   ,XtRImmediate,(XtPointer)OFitTypeNone},
  {XoNfitScript     ,"FitScript"       , XtRString   , sizeof(String)   ,
                                   XtOffset(XoPlotterWidget, plot.fitScript)     ,XtRString   ,NULL},
  {XoNfitNumberOfParameters ,"FitNumberOfParameters" , XtRInt      , sizeof(int)      ,
                                   XtOffset(XoPlotterWidget, plot.fitNumberOfParameters) ,XtRImmediate,(XtPointer)2},
  {XoNfitStartingValues,"FitStartingValues", XtRString   , sizeof(String)   ,
                                   XtOffset(XoPlotterWidget, plot.fitStartingValues),XtRString   ,NULL},
  {XoNfitSteps ,"FitSteps" , XtRString   , sizeof(String)   ,
                                   XtOffset(XoPlotterWidget, plot.fitSteps) ,XtRString   ,NULL},
  {XoNfitVariableNames ,"FitVariableNames", XtRString   , sizeof(String)   ,
                                   XtOffset(XoPlotterWidget, plot.fitVariableNames),XtRString   ,NULL}
};

XoPlotterClassRec  xoPlotterClassRec = {
/* CoreClassPart */
  {
   (WidgetClass)&xoCameraClassRec, /* superclass            */
   "XoPlotter",                    /* class_name            */
   sizeof(XoPlotterRec),           /* widget_size           */
   InitializeClass,                /* class_initialize      */
   NULL,                           /* class_part_initialize */
   FALSE,                          /* class_inited          */
   InitializeWidget,               /* initialize            */
   NULL,                           /* initialize_hook       */
   RealizeWidget,                  /* realize               */
   actions,                        /* actions               */
   XtNumber(actions),              /* num_actions           */
   resources,                      /* resources             */
   XtNumber(resources),            /* resources             */
   NULLQUARK,                      /* xrm_class             */
   TRUE,                           /* compress_motion       */
   TRUE,                           /* compress_exposure     */
   TRUE,                           /* compress_enterleave   */
   TRUE,                           /* visible_interest      */
   DestroyWidget,                  /* destroy               */
   ChangeWidgetSize,               /* resize                */
   XtInheritExpose,                /* expose                */
   SetValues,                      /* SetValues             */
   NULL,                           /* SetValues_hook        */
   XtInheritSetValuesAlmost,       /* SetValues_almost      */
   NULL,                           /* get_values_hook       */
   XtInheritAcceptFocus,           /* accept_focus          */
   XtVersion,                      /* version               */
   NULL,                           /* callback private      */
   translations,	           /* translations          */	
   XtInheritQueryGeometry,         /* return preferred geometry        */
   XtInheritDisplayAccelerator,    /* display your accelerator         */
   NULL 		           /* pointer to extension record      */
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
},
/* XoPlotter class fields */
{
   NULL
}
};

WidgetClass xoPlotterWidgetClass = (WidgetClass) &xoPlotterClassRec;
/***************************************************************************/
static void InitializeClass (
 void
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/*
  XtAddConverter adds a converter to all application context.
*/
  XtAddConverter (XtRString,XoRLayout     ,(XtConverter)ConvertStringToLayout        ,NULL,0);
  XtAddConverter (XtRString,XoRBinBarType ,(XtConverter)ConvertStringToBarType       ,NULL,0);
  XtAddConverter (XtRString,XoRReceiver   ,(XtConverter)ConvertStringToReceiver      ,NULL,0);
  XtAddConverter (XtRString,XoRFitType    ,(XtConverter)ConvertStringToFitType       ,NULL,0);
  XtAddConverter (XtRString,XoRXY_Modeling,(XtConverter)ConvertStringToXY_Modeling   ,NULL,0);

  XtAddConverter (XoRLayout     ,XtRString,(XtConverter)ConvertLayoutToString        ,NULL,0);
  XtAddConverter (XoRBinBarType ,XtRString,(XtConverter)ConvertBarTypeToString       ,NULL,0);
  XtAddConverter (XoRReceiver   ,XtRString,(XtConverter)ConvertHistTypeToString      ,NULL,0);
  XtAddConverter (XoRFitType    ,XtRString,(XtConverter)ConvertFitTypeToString       ,NULL,0);
  XtAddConverter (XoRXY_Modeling,XtRString,(XtConverter)ConvertXY_ModelingToString   ,NULL,0);

  GoLoadColormapGradient (); /*Warning. InitializeClass is not reexecuted if full restart from Wo. */

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
  printf                   ("debug : XoPlotter : InitializeWidget : %s\n",XtName(This));
#endif

  othis               = OPlotterCreate (This->core.name);
  OCameraViewNode     (thisCamera.ocamera,(ONode)OPlotterGetRootNode (othis));
   
  if(OLayoutIsValid(athis.layout)==1)
    {
      OPlotterSetLayoutAutomated (othis,0);
      OPlotterSetLayout          (othis,athis.layout);
    }
  else
    {
      OPlotterSetLayoutAutomated (othis,1);
    }
  OCameraSetAspectRatio      (thisCamera.ocamera,(int)athis.aspectRatio);
  OPlotterSetGridsVisible    (othis,(int)athis.gridsVisible);
  OPlotterSetWallsVisible    (othis,(int)athis.wallsVisible);
  OPlotterSetXY_Modeling     (othis,athis.xyModeling);
  OPlotterSetBarsVisible     (othis,(int)athis.barsVisible);

  OPlotterSetTextContext           (othis,athis.textContext);
  OPlotterSetGridContext           (othis,athis.gridContext);
  OPlotterSetWallContext           (othis,athis.wallContext);
  OPlotterSetHistogramContext      (othis,athis.histogramContext);
  OPlotterSetSecondHistogramContext(othis,athis.secondHistogramContext);
  OPlotterSetFunctionContext       (othis,athis.functionContext);
  OPlotterSetSecondFunctionContext (othis,athis.secondFunctionContext);
  OPlotterSetScatterContext        (othis,athis.scatterContext);
  OPlotterSetSecondScatterContext  (othis,athis.secondScatterContext);
  OPlotterSetValueColorMapping     (othis,athis.valueColorMapping);

  athis.plottedObjects       = CStringDuplicate(athis.plottedObjects);
  athis.xAxisBinding         = CStringDuplicate(athis.xAxisBinding);
  athis.yAxisBinding         = CStringDuplicate(athis.yAxisBinding);
  athis.zAxisBinding         = CStringDuplicate(athis.zAxisBinding);
  athis.title                = CStringDuplicate(athis.title);
  athis.textScale            = CStringDuplicate(athis.textScale);
  athis.cuts                 = CStringDuplicate(athis.cuts);
  athis.functions            = CStringDuplicate(athis.functions);
  athis.titlePosition        = CStringDuplicate(athis.titlePosition);

  athis.xAxisMinimum         = CStringDuplicate(athis.xAxisMinimum);
  athis.xAxisMaximum         = CStringDuplicate(athis.xAxisMaximum);

  athis.yAxisMinimum         = CStringDuplicate(athis.yAxisMinimum);
  athis.yAxisMaximum         = CStringDuplicate(athis.yAxisMaximum);

  athis.zAxisMinimum         = CStringDuplicate(athis.zAxisMinimum);
  athis.zAxisMaximum         = CStringDuplicate(athis.zAxisMaximum);

  athis.vAxisMinimum         = CStringDuplicate(athis.vAxisMinimum);
  athis.vAxisMaximum         = CStringDuplicate(athis.vAxisMaximum);
  athis.vAxisTitle           = CStringDuplicate(athis.vAxisTitle);

  athis.fitScript             = CStringDuplicate(athis.fitScript);
  athis.fitStartingValues     = CStringDuplicate(athis.fitStartingValues);
  athis.fitSteps              = CStringDuplicate(athis.fitSteps);
  athis.fitVariableNames      = CStringDuplicate(athis.fitVariableNames);

  athis.wallContext           = CStringDuplicate(athis.wallContext);
  athis.gridContext           = CStringDuplicate(athis.gridContext);
  athis.textContext           = CStringDuplicate(athis.textContext);
  athis.histogramContext            = CStringDuplicate(athis.histogramContext);
  athis.secondHistogramContext      = CStringDuplicate(athis.secondHistogramContext);
  athis.functionContext       = CStringDuplicate(athis.functionContext);
  athis.secondFunctionContext = CStringDuplicate(athis.secondFunctionContext);
  athis.scatterContext        = CStringDuplicate(athis.scatterContext);
  athis.secondScatterContext  = CStringDuplicate(athis.secondScatterContext);
  athis.valueColorMapping     = CStringDuplicate(athis.valueColorMapping);

  XtAddEventHandler                        (This,0,True,HandleMessage,0);
  OPlotterSetRequireRepresentationFunction (othis,(OPlotterSendFunction)SendBuild,This,0);

  OPlotterSetColorGradient         (OColormapGetIdentifierFromString("ocolormap_gradient"),65,10);

  athis.objn           = 0;
  athis.objs           = NULL;
  athis.otuple         = NULL;
  athis.stuple         = NULL;

  athis.ncut           = 0;
  athis.lcut           = NULL;
  athis.acut           = NULL;

  athis.zoom_count    = 0;

  athis.wstats            = NULL;
  athis.wstatsLineHeight  = (Dimension)0;

#ifdef DEBUG
  printf                   ("debug : XoPlotter : InitializeWidget : end\n");
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
/* Call the Realize procedure (XtInheritRealize) */
  if(xoPlotterWidgetClass->core_class.superclass->core_class.realize!=NULL)
    (xoPlotterWidgetClass->core_class.superclass->core_class.realize)(This, a_mask, a_watbs);

  OPlotterRequireRepresentation   (othis);
}
/***************************************************************************/
static void DestroyWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  athis.wstats                = NULL;
  athis.wstatsLineHeight      = (Dimension)0;

  OPlotterDelete              (othis);

  CStringDelete               (athis.plottedObjects);
  CStringDelete               (athis.xAxisBinding);
  CStringDelete               (athis.yAxisBinding);
  CStringDelete               (athis.zAxisBinding);
  CStringDelete               (athis.title);
  CStringDelete               (athis.titlePosition);
  CStringDelete               (athis.textScale);
  CStringDelete               (athis.cuts);
  CStringDelete               (athis.functions);
  CStringDelete               (athis.xAxisMinimum);
  CStringDelete               (athis.xAxisMaximum);
  CStringDelete               (athis.yAxisMinimum);
  CStringDelete               (athis.yAxisMaximum);
  CStringDelete               (athis.zAxisMinimum);
  CStringDelete               (athis.zAxisMaximum);
  CStringDelete               (athis.vAxisMinimum);
  CStringDelete               (athis.vAxisMaximum);
  CStringDelete               (athis.vAxisTitle);
  CStringDelete               (athis.fitScript);
  CStringDelete               (athis.fitStartingValues);
  CStringDelete               (athis.fitSteps);
  CStringDelete               (athis.fitVariableNames);
  CStringDelete               (athis.gridContext);
  CStringDelete               (athis.wallContext);
  CStringDelete               (athis.textContext);
  CStringDelete               (athis.histogramContext);
  CStringDelete               (athis.secondHistogramContext);
  CStringDelete               (athis.functionContext);
  CStringDelete               (athis.secondFunctionContext);
  CStringDelete               (athis.scatterContext);
  CStringDelete               (athis.secondScatterContext);
  CStringDelete               (athis.valueColorMapping);

  athis.plottedObjects        = NULL;
  athis.xAxisBinding          = NULL;
  athis.yAxisBinding          = NULL;
  athis.zAxisBinding          = NULL;
  athis.title                 = NULL;
  athis.titlePosition         = NULL;
  athis.textScale             = NULL;
  athis.cuts                  = NULL;
  athis.functions             = NULL;
  athis.xAxisMinimum          = NULL;
  athis.xAxisMaximum          = NULL;
  athis.yAxisMinimum          = NULL;
  athis.yAxisMaximum          = NULL;
  athis.zAxisMinimum          = NULL;
  athis.zAxisMaximum          = NULL;
  athis.vAxisMinimum          = NULL;
  athis.vAxisMaximum          = NULL;
  athis.vAxisTitle            = NULL;
  athis.fitScript             = NULL;
  athis.fitStartingValues     = NULL;
  athis.fitSteps              = NULL;
  athis.fitVariableNames      = NULL;
  athis.gridContext           = NULL;
  athis.wallContext           = NULL;
  athis.textContext           = NULL;
  athis.histogramContext      = NULL;
  athis.secondHistogramContext = NULL;
  athis.functionContext        = NULL;
  athis.secondFunctionContext  = NULL;
  athis.scatterContext         = NULL;
  athis.secondScatterContext   = NULL;
  athis.valueColorMapping      = NULL;

  DeletePlottedObjectList      (This);
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
#define SET_SRES(a_field)  \
  IFSMOD(a_field) {CStringDelete  (acur.a_field);\
                   athis.a_field  = CStringDuplicate (athis.a_field);/*do a local copy of string given by user*/\
                   OPlotterRequireRepresentation   (othis);}\
  else /*not modified*/ {athis.a_field = acur.a_field;}
#define SET_SRES_2(a_field,a_proc)  \
  IFSMOD(a_field) {CStringDelete  (acur.a_field);\
                   athis.a_field  = CStringDuplicate (athis.a_field);/*do a local copy of string given by user*/\
		   a_proc         (othis,athis.a_field);\
                   OPlotterRequireRepresentation (othis);}\
  else /*not modified*/ {athis.a_field = acur.a_field;}
/*.........................................................................*/
  IFMOD(layout)       
    {
      OPlotterSetLayoutAutomated (othis,0);
      OPlotterSetLayout          (othis,athis.layout);
    }
  IFMOD(gridsVisible) OPlotterSetGridsVisible (othis,(int)athis.gridsVisible);
  IFMOD(aspectRatio)  OCameraSetAspectRatio   (thisCamera.ocamera,(int)athis.aspectRatio);
  IFMOD(wallsVisible) OPlotterSetWallsVisible (othis,(int)athis.wallsVisible);
  IFMOD(xyModeling)   OPlotterSetXY_Modeling  (othis,athis.xyModeling);
  IFMOD(barsVisible)  OPlotterSetBarsVisible  (othis,(int)athis.barsVisible);

  SET_SRES_2 (textContext  ,OPlotterSetTextContext)
  SET_SRES_2 (gridContext  ,OPlotterSetGridContext)
  SET_SRES_2 (wallContext  ,OPlotterSetWallContext)
  SET_SRES_2 (histogramContext   ,OPlotterSetHistogramContext)
  SET_SRES_2 (secondHistogramContext  ,OPlotterSetSecondHistogramContext)
  SET_SRES_2 (functionContext  ,OPlotterSetFunctionContext)
  SET_SRES_2 (secondFunctionContext ,OPlotterSetSecondFunctionContext)
  SET_SRES_2 (scatterContext  ,OPlotterSetScatterContext)
  SET_SRES_2 (secondScatterContext ,OPlotterSetSecondScatterContext)
  SET_SRES_2 (valueColorMapping     ,OPlotterSetValueColorMapping)

  SET_SRES          (plottedObjects)
  SET_SRES          (xAxisBinding) 
  SET_SRES          (yAxisBinding) 
  SET_SRES          (zAxisBinding) 
  SET_SRES          (title)
  SET_SRES          (textScale)
  SET_SRES          (cuts)
  SET_SRES          (functions)
  SET_SRES          (xAxisMinimum)
  SET_SRES          (xAxisMaximum)
  SET_SRES          (yAxisMinimum)
  SET_SRES          (yAxisMaximum)
  SET_SRES          (zAxisMinimum)
  SET_SRES          (zAxisMaximum)
  SET_SRES          (vAxisMinimum)
  SET_SRES          (vAxisMaximum)
  SET_SRES          (vAxisTitle)
  SET_SRES          (fitScript)
  SET_SRES          (fitStartingValues)
  SET_SRES          (fitSteps)
  SET_SRES          (fitVariableNames)
  SET_SRES          (titlePosition)

  IFMOD(statisticsVisible)      OPlotterRequireRepresentation (othis);
  IFMOD(statisticsShort)        OPlotterRequireRepresentation (othis);
  IFMOD(titleEditable)          OPlotterRequireRepresentation (othis);
  IFMOD(binBarType)             OPlotterRequireRepresentation (othis);
  IFMOD(tupleDataReceiver)      OPlotterRequireRepresentation (othis);
  IFMOD(numberOfBoundColumns)   OPlotterRequireRepresentation (othis);
  IFMOD(functionDimension)      OPlotterRequireRepresentation (othis);

  IFMOD(xAxisAutomatic)         OPlotterRequireRepresentation (othis);
  IFMOD(xAxisLogScale)          OPlotterRequireRepresentation (othis);
  IFMOD(xAxisNumberOfBins)      OPlotterRequireRepresentation (othis);

  IFMOD(yAxisAutomatic)         OPlotterRequireRepresentation (othis);
  IFMOD(yAxisLogScale)          OPlotterRequireRepresentation (othis);
  IFMOD(yAxisNumberOfBins)      OPlotterRequireRepresentation (othis);

  IFMOD(zAxisAutomatic)         OPlotterRequireRepresentation (othis);
  IFMOD(zAxisLogScale)          OPlotterRequireRepresentation (othis);
  IFMOD(zAxisNumberOfBins)      OPlotterRequireRepresentation (othis);

  IFMOD(vAxisAutomatic)         OPlotterRequireRepresentation (othis);
  IFMOD(vAxisLogScale)          OPlotterRequireRepresentation (othis);
  IFMOD(fitType)                OPlotterRequireRepresentation (othis);
  IFMOD(fitNumberOfParameters)  OPlotterRequireRepresentation (othis);

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
/* Call the Resize procedure (XtInheritResize) */
  if(xoPlotterWidgetClass->core_class.superclass->core_class.resize!=NULL)
    (xoPlotterWidgetClass->core_class.superclass->core_class.resize)(This);
  SetChildrenGeometry (This);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int SendBuild (
 Widget This
)
/***************************************************************************/
/*
  Never call this routine directly.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return XWidgetSendMessage (This,XMSG_BUILD,0L);
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

  if(a_event->xclient.data.l[0]==XMSG_BUILD) 
    { 
      DoRepresentation (This);
    }

  a_tag         = NULL;
  a_continue    = NULL;
}
/***************************************************************************/
static void DoRepresentation (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           count;
  char*         string;
  OHistogram*   hists = NULL;
  OScatter*     scats = NULL;
  OFunction*    funcs = NULL;
  int           histn,scatn,funcn;
  OHistogram    otuple_hist = NULL;
  OScatter      otuple_scat = NULL;
  OHistogram    fitted_hist = NULL;
  char*         fitted_hist_name = NULL;
  char*         fitInfos = NULL;
  int           binn = 0;
  int*          is   = NULL;
  int*          js   = NULL;
/*.........................................................................*/
/*Below line needed in case of call from XoPlotterDoRepresentation to disconnect further action of XMSG_BUILD messages.*/
  if(OPlotterIsRebuildDone(othis)==0) return;

 {OHistogram hist;
  hist       = (OHistogram)OPlotterGetHighlightedBins (othis,&binn,&is,&js);
  if(hist!=NULL)
    {
      fitted_hist_name = OHistogramGetName (hist);
      if(fitted_hist_name==NULL)
	{
           CWarnF("Highlighted histogram has no name.\n");
	}
    }
  else
    {
      fitted_hist_name = NULL;
    }}

  OHistogramDelete (otuple_hist); /*This histo must survive DoRepresentation so that bin info collection works.*/
  otuple_hist      = NULL;
  OScatterDelete   (otuple_scat); /*This scat  must survive DoRepresentation so that bin info collection works.*/
  otuple_scat      = NULL;

  DeletePlottedObjectList (This);
  CreatePlottedObjectList (This);

  if(athis.otuple!=NULL)
    {
      char*         binds[OAxisMaximumNumber];
      CreateCutList (This);
      if(athis.tupleDataReceiver!=OReceiverScatter)
        {
	  otuple_hist             = OHistogramMake();

          /*Build name. Needed if fitting over highlighted bins.*/
	  if(XtName(This)!=NULL)
	    {
	      string                  = CStringCreateF(strlen(XtName(This))+11,"%s_tuple_hist",XtName(This));
	      OHistogramSetName       (otuple_hist,string);
	      CStringDelete           (string);
	    }
	  else
	    {
	      CWarnF("XoPlotter %lu has no name.\n",This);
	    }

	  CListAddEntry           ((CList*)&hists,(CEntry)otuple_hist);

	  OHistogramSetDimension  (otuple_hist,
				   athis.tupleDataReceiver==OReceiverProfile ? 1 : athis.numberOfBoundColumns);

	  OHistogramSetBarsType   (otuple_hist,(int)athis.binBarType);
	      
	  OHistogramSetIthAxisAttributesF           (otuple_hist,OAxisX,"alndu",
						     (int)athis.xAxisAutomatic,
						     (int)athis.xAxisLogScale,
						     athis.xAxisNumberOfBins,
						     athis.xAxisMinimum,
						     athis.xAxisMaximum);
	  OHistogramSetIthAxisAttributesF           (otuple_hist,OAxisY,"alndu",
						     (int)athis.yAxisAutomatic,
						     (int)athis.yAxisLogScale,
						     athis.yAxisNumberOfBins,
						     athis.yAxisMinimum,
						     athis.yAxisMaximum);
	  OHistogramSetIthAxisAttributesF           (otuple_hist,OAxisZ,"alndu",
						     (int)athis.zAxisAutomatic,
						     (int)athis.zAxisLogScale,
						     athis.zAxisNumberOfBins,
						     athis.zAxisMinimum,
						     athis.zAxisMaximum);
	  OHistogramSetIthAxisAttributesF           (otuple_hist,OAxisV,"aldu", 
						     (int)athis.vAxisAutomatic,
						     (int)athis.vAxisLogScale,
						     athis.vAxisMinimum,
						     athis.vAxisMaximum);
	  
	  binds[0] = athis.xAxisBinding;
	  binds[1] = athis.yAxisBinding;
	  binds[2] = athis.zAxisBinding;
	  OHistogramClear           (otuple_hist);
	  HoFillHistogramFromTuple  (otuple_hist,
				     athis.otuple,
				     athis.tupleDataReceiver==OReceiverProfile ? 2 : athis.numberOfBoundColumns,binds,
				     athis.ncut,athis.lcut, 
				     athis.tupleDataReceiver==OReceiverProfile ? 1 : 0);
	      
	  if( (athis.title!=NULL) && (*athis.title!='\0') )
	    {
	      OHistogramSetTitle (otuple_hist,athis.title);
	    }
	  else 
	    {
		  char*         title;
                  title         = GetDefaultTitle(OHistogramGetDimension(otuple_hist),
						  OAxisGetTitle (OHistogramGetIthAxis(otuple_hist,OAxisX)),
						  OAxisGetTitle (OHistogramGetIthAxis(otuple_hist,OAxisY)),
						  athis.tupleDataReceiver==OReceiverProfile?1:0);
		  OHistogramSetTitle (otuple_hist,title);
	    }
	}
      else
        {
	  otuple_scat             = OScatterMake();
	  CListAddEntry           ((CList*)&scats,(CEntry)otuple_scat);

	  OScatterSetDimension      (otuple_scat,athis.numberOfBoundColumns);

	  OScatterSetIthAxisAttributesF           (otuple_scat,OAxisX,"aldu",
						   (int)athis.xAxisAutomatic,
						   (int)athis.xAxisLogScale,
						   athis.xAxisMinimum,
						   athis.xAxisMaximum);
	  OScatterSetIthAxisAttributesF           (otuple_scat,OAxisY,"aldu",
						   (int)athis.yAxisAutomatic,
						   (int)athis.yAxisLogScale,
						   athis.yAxisMinimum,
						   athis.yAxisMaximum);
	  OScatterSetIthAxisAttributesF           (otuple_scat,OAxisZ,"aldu",
						   (int)athis.zAxisAutomatic,
						   (int)athis.zAxisLogScale,
						   athis.zAxisMinimum,
						   athis.zAxisMaximum);
	  
	  binds[0]               = athis.xAxisBinding;
	  binds[1]               = athis.yAxisBinding;
	  binds[2]               = athis.zAxisBinding;
	  OScatterClear          (otuple_scat);
	  HoFillScatterFromTuple (otuple_scat,
				  athis.otuple,
				  athis.numberOfBoundColumns,binds,
				  athis.ncut,athis.lcut);
	  
	  if( (athis.title!=NULL) && (*athis.title!='\0') )
	    {
	      OScatterSetTitle        (otuple_scat,athis.title);
	    }
	  else 
	    {
	      char*         title;
	      title         = GetDefaultTitle (OScatterGetDimension(otuple_scat),
					       OAxisGetTitle (OScatterGetIthAxis(otuple_scat,OAxisX)),
					       OAxisGetTitle (OScatterGetIthAxis(otuple_scat,OAxisY)),0);
	      OScatterSetTitle (otuple_scat,title);
	    }
	}
      DeleteCutList       (This);
    }
  else if(athis.objn!=0)
    {
      for(count=0;count<athis.objn;count++)
	{
	  if(athis.objs[count].type==ObjectTypeHistogram) 
	    {
	      CListAddEntry ((CList*)&hists,athis.objs[count].reference);
	    }
	  else if(athis.objs[count].type==ObjectTypeScatter)
	    {
	      CListAddEntry ((CList*)&scats,athis.objs[count].reference);
	    }
	}
    }

  histn = CListGetSize ((CList)hists);
  scatn = CListGetSize ((CList)scats);
  
  /*Fitting*/
 {char*        string;
  fitted_hist  = OHistogramGetIdentifier(fitted_hist_name);
  if(fitted_hist==NULL)
    {
      fitted_hist = histn!=0 ? hists[0] : (OHistogram)NULL;
    }
  string             = FitHistogram (This,fitted_hist,binn,is,js,&fitInfos);
  if(string==NULL)   string = CStringDuplicate(athis.functions);
  funcs              = CreateFunctionList (This,string);
  funcn              = CListGetSize ((CList)funcs);
  CStringDelete      (string);}

  if(histn!=0)
    {
      OHistogramSetIthAxisAttributesF (hists[0],OAxisV,"aldu",(int)athis.vAxisAutomatic,
				       (int)athis.vAxisLogScale,
				       athis.vAxisMinimum,
				       athis.vAxisMaximum);
      if(athis.vAxisTitle!=NULL) OHistogramSetAxisOfValuesTitle (hists[0],athis.vAxisTitle);
    }


  if( (histn==0) && (scatn==0) && (funcn!=0) )
    {
      OFunctionSetIthAxisAttributesF           (funcs[0],OAxisX,"alndu",
						(int)athis.xAxisAutomatic,
						(int)athis.xAxisLogScale,
						athis.xAxisNumberOfBins,
						athis.xAxisMinimum,
						athis.xAxisMaximum);
      
      OFunctionSetIthAxisAttributesF           (funcs[0],OAxisY,"alndu",
						(int)athis.yAxisAutomatic,
						(int)athis.yAxisLogScale,
						athis.yAxisNumberOfBins,
						athis.yAxisMinimum,
						athis.yAxisMaximum);
      
      OFunctionSetIthAxisAttributesF           (funcs[0],OAxisZ,"alndu",
						(int)athis.zAxisAutomatic,
						(int)athis.zAxisLogScale,
						athis.zAxisNumberOfBins,
						athis.zAxisMinimum,
						athis.zAxisMaximum);
      
      OFunctionSetIthAxisAttributesF           (funcs[0],OAxisV,"aldu",
						(int)athis.vAxisAutomatic,
						(int)athis.vAxisLogScale,
						athis.vAxisMinimum,
						athis.vAxisMaximum);
      
    }
  
  if(athis.textScale!=NULL)
    {
      double value;
      int    status;
      value  = CStringConvertToDouble(athis.textScale,&status);
      if(status==1) OPlotterSetTextScale (othis,value);
    }
  
  OPlotterSetTitleVisible    (othis,(athis.titleEditable==True ? 0:1));

  if( (athis.titlePosition!=NULL) && (strcmp(athis.titlePosition,"down")==0) )
    OPlotterSetTitlePosition   (othis,1);
  else
    OPlotterSetTitlePosition   (othis,0);

  OPlotterSetColormap                  (othis,OCameraGetColormap(thisCamera.ocamera));
  OPlotterReceiveRepresentationRequest (othis,(CList)hists,(CList)funcs,(CList)scats);
  XoPlotterSetLayout                   (This);
  OPlotterHighlightBins                (othis,fitted_hist,binn,is,js);
  CMemoryFreeBlock                     (is);
  CMemoryFreeBlock                     (js);

  if( (histn!=0) || (scatn!=0) || (funcn!=0) ) UpdateAxes   (This);
  
  if( (histn!=0) && (athis.statisticsVisible==True) ) 
    {
      if(hists[0]==otuple_hist)
	{
	  string          = CStringDuplicate(GetHistogramInfos(hists[0],
							       OAxisGetTitle (OHistogramGetIthAxis(hists[0],OAxisX)),
							       OAxisGetTitle (OHistogramGetIthAxis(hists[0],OAxisY)),
							       athis.statisticsShort));
	}
      else
	{
	  string          = CStringDuplicate(GetHistogramInfos(hists[0],
							       NULL,
							       NULL,
							       athis.statisticsShort));
	}
      if( (fitInfos!=NULL) && (*fitInfos!='\0') ) 
	{
	  CStringConcatenate (&string,"\n");
	  CStringConcatenate (&string,fitInfos);
	}
      if( (string!=NULL) && (*string!='\0') ) 
	{
	  Arg             args[5];
	  int             rows;
	  Dimension       width,height;
	  int             margin = 2;
	  rows            = CStringCountLines (string);
#ifdef HAS_XM
	  if(athis.wstats==NULL) 
	    {
	      XtSetArg               (args[0],XmNvalue             ," ");
	      XtSetArg               (args[1],XmNeditMode          ,XmMULTI_LINE_EDIT);
	      XtSetArg               (args[2],XmNhighlightThickness,0);
	      XtSetArg               (args[3],XmNshadowThickness   ,0);
	      XtSetArg               (args[4],XmNmarginHeight      ,margin);
	      athis.wstats           = XmCreateText(This,"PLOTTER_STATS",args,5);
	      XWidgetGetDimension    (athis.wstats,&width,&height); /*Retreive height for one line.*/
	      athis.wstatsLineHeight = height - 2 * margin;
	    }
	  XtSetArg            (args[0],XmNvalue,string);
	  XtSetValues         (athis.wstats, args,1);
#endif /*HAS_XM*/
#ifdef HAS_XAW
	  if(athis.wstats==NULL) 
	    {
	      XtSetArg               (args[0],XtNstring      ," ");
	      XtSetArg               (args[1],XtNtopMargin   ,margin);
	      XtSetArg               (args[2],XtNbottomMargin,margin);
	      XtSetArg               (args[3],XtNeditType    ,XawtextEdit);
	      athis.wstats           = XtCreateWidget ("PLOTTER_STATS",asciiTextWidgetClass,This,args,4);
	      XWidgetGetDimension    (athis.wstats,&width,&height); /*Retreive height for one line.*/
	      athis.wstatsLineHeight = height - 2 * margin;
	    }
	  XtSetArg            (args[0],XtNstring      ,string);
	  XtSetValues         (athis.wstats, args,1);
#endif /*HAS_XAW*/
	  if(athis.wstats!=NULL) 
	    {
	      Dimension           borderWidth;
	      XtSetArg            (args[0],XtNborderWidth,&borderWidth);
	      XtGetValues         (athis.wstats,args,1);
	      XWidgetGetDimension (athis.wstats,&width,&height);
	      height              = rows * athis.wstatsLineHeight + 2 * margin;
	      XtResizeWidget      (athis.wstats,width,height,borderWidth);
	      XtManageChild       (athis.wstats);
	      XWidgetGetDimension (athis.wstats,&width,&height);
	      XtMoveWidget        (athis.wstats,This->core.width-width,0);
	      XWidgetMap          (athis.wstats);
	    }
	}
      else
	{
	  XWidgetUnmap   (athis.wstats);
	}
      CStringDelete (string);
    } 
  else 
    {
      XWidgetUnmap   (athis.wstats);
    }

  CStringDelete         (fitInfos);
  CListDelete           ((CList)hists);
  CListDelete           ((CList)scats);
  CListDestroy          ((CList)funcs,(CListVisitEntryFunction)OFunctionDelete);
}
/***************************************************************************/
static void CreatePlottedObjectList (
 Widget  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char** lines = NULL;
  int    nline = 0;
   int count;
/*.........................................................................*/
  athis.objn    = 0;
  athis.objs    = NULL;
  athis.otuple  = NULL;
  CStringDelete (athis.stuple);
  athis.stuple  = NULL;

  /*
  printf ("debug : %s : plottedObjects : %s\n",XtName(This),athis.plottedObjects);
  */

  lines       = CTextCreateFromString        (athis.plottedObjects,&nline);
  if( (nline==0) || (lines==NULL) ) return;

  athis.objs  = (ToPlotRecord*)CMemoryAllocateBlock(nline * sizeof(ToPlotRecord));
  if(athis.objs==NULL) 
    {
      CTextDelete (nline,lines);
      return;
    }

  for(count=0;count<nline;count++)
    {
       void*  obj;

/*     printf ("debug:plottedObjects:%s\n",lines[count]);*/

       obj = OTupleGetIdentifier(lines[count]); 
       if(obj!=NULL)
         {
	   athis.objs[athis.objn].reference = obj;
	   athis.objs[athis.objn].type      = ObjectTypeTuple;
	   athis.objs[athis.objn].to_delete = 0;
	   if(athis.otuple==NULL)
	     {
	       athis.otuple   = (OTuple)obj;
	       CStringDelete   (athis.stuple);
	       athis.stuple    = CStringDuplicate(OTupleGetName((OTuple)obj));
	     }
	   (athis.objn)++;
         }
       else if( (obj=OHistogramGetIdentifier(lines[count])) !=NULL)
         {
	   athis.objs[athis.objn].reference = obj;
	   athis.objs[athis.objn].type      = ObjectTypeHistogram;
	   athis.objs[athis.objn].to_delete = 0;
	   (athis.objn)++;
         }
       else if( (obj=OScatterGetIdentifier(lines[count])) !=NULL)
         {
	   athis.objs[athis.objn].reference = obj;
	   athis.objs[athis.objn].type      = ObjectTypeScatter;
	   athis.objs[athis.objn].to_delete = 0;
	   (athis.objn)++;
         }
       else if( (obj=HoConvert(lines[count])) !=NULL)
         {
	   if(OHistogramIsValid((OHistogram)obj)==1)
	     {
	       athis.objs[athis.objn].reference = (OHistogram)obj;
	       athis.objs[athis.objn].type      = ObjectTypeHistogram;
	       athis.objs[athis.objn].to_delete = 1;
	       (athis.objn)++;
	     }
	   else if(OTupleIsValid((OTuple)obj)==1)
	     {
	       athis.objs[athis.objn].reference = (OTuple)obj;
	       athis.objs[athis.objn].type      = ObjectTypeTuple;
	       athis.objs[athis.objn].to_delete = 1;
	       if(athis.otuple==NULL) 
		 {
		   athis.otuple    = (OTuple)obj;
		   CStringDelete   (athis.stuple);
		   athis.stuple    = CStringDuplicate(OTupleGetName((OTuple)obj));
		 }
	       (athis.objn)++;
	     }
       else
         {
           CWarnF("Unable to find OHistogram/OTuple %s.\n",lines[count]);
         } 
        }

    }

  if(athis.objn==0)
    {
      CMemoryFreeBlock   (athis.objs);
      athis.objs = NULL;
    }

  CTextDelete (nline,lines);
}
/***************************************************************************/
static void DeletePlottedObjectList (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(athis.objs==NULL) return;
  for(count=0;count<athis.objn;count++) 
    {
      if(athis.objs[count].to_delete==1)
        {
               if(athis.objs[count].type==ObjectTypeHistogram) 
	    OHistogramDelete  ((OHistogram) (athis.objs[count].reference));
          else if(athis.objs[count].type==ObjectTypeTuple) 
	    OTupleDelete ((OTuple)(athis.objs[count].reference));
          else if(athis.objs[count].type==ObjectTypeScatter)  
	    OScatterDelete  ((OScatter) (athis.objs[count].reference));
        }
    }
  CMemoryFreeBlock      (athis.objs);
  athis.objn    = 0;
  athis.objs    = NULL;
  athis.otuple  = NULL;
  CStringDelete (athis.stuple);
  athis.stuple  = NULL;
}
/***************************************************************************/
static void CreateCutList (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char** lines = NULL;
  int    nline = 0;
   int count;
/*.........................................................................*/
  athis.ncut = 0;
  athis.lcut = NULL;
  athis.acut = NULL;
  lines       = CTextCreateFromString        (athis.cuts,&nline);
  if( (nline==0) || (lines==NULL) ) return;

  athis.lcut  = (OCut*)CMemoryAllocateBlock(nline * sizeof(OCut));
  athis.acut  = (int*) CMemoryAllocateBlock(nline * sizeof(int));
  if( (athis.lcut==NULL) || (athis.acut==NULL) )
    {
      CTextDelete (nline,lines);
      return;
    }

  for(count=0;count<nline;count++)
    { 
      OCut   cut;
      cut    = OCutGetIdentifier(lines[count]);
      if(cut==NULL)
        {
          cut                    = OCutMake ();
          OCutSetScript          (cut,lines[count]);          
	  athis.lcut[athis.ncut] = cut;
	  athis.acut[athis.ncut] = 1;
	  (athis.ncut)++;
        }
      else
	{
	  athis.lcut[athis.ncut] = cut;
	  athis.acut[athis.ncut] = 0;
	  (athis.ncut)++;
	}
    }

  if(athis.ncut==0)
    {
      CMemoryFreeBlock (athis.lcut);
      CMemoryFreeBlock (athis.acut);
      athis.lcut       = NULL;
      athis.acut       = NULL;
    }

  CTextDelete (nline,lines);
}
/***************************************************************************/
static void DeleteCutList (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(athis.lcut==NULL) return;
  if(athis.acut==NULL) return;
  for(count=0;count<athis.ncut;count++) 
    {
      if(athis.acut[count]==1) OCutDelete(athis.lcut[count]);
    }
  CMemoryFreeBlock (athis.lcut);
  CMemoryFreeBlock (athis.acut);
  athis.ncut       = 0;
  athis.lcut       = NULL;
  athis.acut       = NULL;
}
/***************************************************************************/
static OFunction* CreateFunctionList (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char**     lines = NULL;
  int        nline = 0;
  int        count;
  OFunction* list = NULL;
/*.........................................................................*/
  lines       = CTextCreateFromString (a_string,&nline);
  if( (nline==0) || (lines==NULL) ) return NULL;

  for(count=0;count<nline;count++)
    { 
      OFunction                    func;
      func                         = OFunctionMake ();
      CListAddEntry                ((CList*)&list,(CEntry)func);
      OFunctionSetDimension        (func,athis.functionDimension);
      OFunctionSetScript           (func,lines[count]);          
    }

  CTextDelete (nline,lines);

  return      list;
}
/***************************************************************************/
static int IsFitEnabled (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(athis.fitType==OFitTypeNone) return 0;
  return 1;
}
/***************************************************************************/
static char* FitHistogram (
 Widget This
,OHistogram a_hist
,int a_binn
,int* a_is
,int* a_js
,char** a_infos
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OFit  ofit;
  char* sfunc = NULL;
/*.........................................................................*/
  if(a_infos!=NULL) *a_infos = NULL;
  if( (a_hist==NULL)  || (athis.fitType==OFitTypeNone) )
    {
      return       NULL;
    }

  ofit = OFitMake();

  OFitSetType (ofit,athis.fitType);
       if(athis.fitType==OFitTypePolynomial)
    {
      OFitSetType               (ofit,OFitTypePolynomial);
      OFitSetNumberOfParameters (ofit,athis.fitNumberOfParameters);
    }
  else if( (athis.fitType==OFitTypeUser) && 
	   (athis.fitScript!=NULL)       && 
	   (athis.fitVariableNames!=NULL) 
	  )     /* user given fit function */
    { char** list;
      int    nitem;
      OFitSetType   (ofit,OFitTypeUser);
      list          = CTextCreateFromString(athis.fitVariableNames,&nitem);
      OFitSetScript (ofit,athis.fitScript,nitem,list);
      CTextDelete   (nitem,list);
    }

  if( (athis.fitStartingValues!=NULL) && (strcmp(athis.fitStartingValues,"auto")==0) )
    {
      OFitSetStartValuesAutomated (ofit,1);
    }
  else
    { double* list;
      int     number;
      list    = CStringConvertToDoubles (athis.fitStartingValues,&number);
      if(list!=NULL)  
        {
          OFitSetStartValuesAutomated (ofit,0);
          OFitSetStartValues     (ofit,number,list);
        }
      CMemoryFreeBlock          (list);
     }

  if( (athis.fitSteps!=NULL) && (strcmp(athis.fitSteps,"auto")==0) )
    {
      OFitSetStepsAutomated (ofit,1);
    }
  else
    { double* list;
      int     number;
      list    = CStringConvertToDoubles (athis.fitSteps,&number);
      if(list!=NULL)  
        {
          OFitSetStepsAutomated (ofit,0);
          OFitSetSteps          (ofit,number,list);
        }
      CMemoryFreeBlock          (list);
     }

  if(HoFitHistogram(a_hist,ofit,a_binn,a_is,a_js)==1) 
    { 
      sfunc            = OFitBuildScript (ofit);
      if(athis.functions!=NULL) 
	{
	  CStringConcatenate       (&sfunc,"\n");
	  CStringConcatenate       (&sfunc,athis.functions);
	}
      if(a_infos!=NULL) *a_infos = OFitGetInformations(ofit);
    }
  else
    {
      sfunc        = NULL;
    }


  OFitDelete (ofit);
  return     sfunc;
}
/***************************************************************************/
static void UpdateAxes (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double d;
  int    b;
  char*  s;
/*.........................................................................*/
/* axeX */
  OPlotterGetIthAxisAttributesF (othis,OAxisX,"a",&b);
  athis.xAxisAutomatic          = b;
  OPlotterGetIthAxisAttributesF (othis,OAxisX,"l",&b);
  athis.xAxisLogScale           = b;
  OPlotterGetIthAxisAttributesF (othis,OAxisX,"n",&(athis.xAxisNumberOfBins));

  OPlotterGetIthAxisAttributesF (othis,OAxisX,"m",&d);
  CStringDelete                 (athis.xAxisMinimum);
  athis.xAxisMinimum            = CStringCreateF(64,"%g",d);

  OPlotterGetIthAxisAttributesF (othis,OAxisX,"x",&d);
  CStringDelete                 (athis.xAxisMaximum);
  athis.xAxisMaximum            = CStringCreateF(64,"%g",d);

/* axeY */
  OPlotterGetIthAxisAttributesF (othis,OAxisY,"a",&b);
  athis.yAxisAutomatic          = b;
  OPlotterGetIthAxisAttributesF (othis,OAxisY,"l",&b);
  athis.yAxisLogScale           = b;
  OPlotterGetIthAxisAttributesF (othis,OAxisY,"n",&(athis.yAxisNumberOfBins));

  OPlotterGetIthAxisAttributesF (othis,OAxisY,"m",&d);
  CStringDelete                 (athis.yAxisMinimum);
  athis.yAxisMinimum            = CStringCreateF(64,"%g",d);

  OPlotterGetIthAxisAttributesF (othis,OAxisY,"x",&d);
  CStringDelete                 (athis.yAxisMaximum);
  athis.yAxisMaximum            = CStringCreateF(64,"%g",d);

/* axeZ */
  OPlotterGetIthAxisAttributesF (othis,OAxisZ,"a",&b);
  athis.zAxisAutomatic          = b;
  OPlotterGetIthAxisAttributesF (othis,OAxisZ,"l",&b);
  athis.zAxisLogScale           = b;
  OPlotterGetIthAxisAttributesF (othis,OAxisZ,"n",&(athis.zAxisNumberOfBins));

  OPlotterGetIthAxisAttributesF (othis,OAxisZ,"m",&d);
  CStringDelete                 (athis.zAxisMinimum);
  athis.zAxisMinimum            = CStringCreateF(64,"%g",d);

  OPlotterGetIthAxisAttributesF (othis,OAxisZ,"x",&d);
  CStringDelete                 (athis.zAxisMaximum);
  athis.zAxisMaximum            = CStringCreateF(64,"%g",d);

/* axeV */
  OPlotterGetIthAxisAttributesF (othis,OAxisV,"a",&b);
  athis.vAxisAutomatic          = b;
  OPlotterGetIthAxisAttributesF (othis,OAxisV,"l",&b);
  athis.vAxisLogScale           = b;

  OPlotterGetIthAxisAttributesF (othis,OAxisV,"m",&d);
  CStringDelete                 (athis.vAxisMinimum);
  athis.vAxisMinimum            = CStringCreateF(64,"%g",d);

  OPlotterGetIthAxisAttributesF (othis,OAxisV,"x",&d);
  CStringDelete                 (athis.vAxisMaximum);
  athis.vAxisMaximum            = CStringCreateF(64,"%g",d);
  athis.vAxisNumberOfBins       = 0;

  OPlotterGetIthAxisAttributesF (othis,OAxisV,"t",&s);
  CStringDelete                 (athis.vAxisTitle);
  athis.vAxisTitle              = CStringDuplicate(s);

}
/***************************************************************************/
static void SetChildrenGeometry (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (athis.statisticsVisible==True) && (athis.wstats!=NULL) )
    {
      Dimension           width,height;
      XWidgetGetDimension (athis.wstats,&width,&height);
      XtMoveWidget        (athis.wstats,This->core.width-width,0);
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
  int status;
/*.........................................................................*/
  switch(a_event->type)
    {
      case ButtonPress:
            pointer.end.x    = pointer.beg.x      = a_event->xbutton.x;
            pointer.end.y    = pointer.beg.y      = a_event->xbutton.y;
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),thisCamera.rubberGC,&pointer.beg,&pointer.end);
            break;
      case ButtonRelease:
            XDisplayDrawRectangle    (XtDisplay(This),XtWindow(This),thisCamera.rubberGC,&pointer.beg,&pointer.end);
            if(OPlotterGetPickability(othis)==0)
	      {
		OPlotterSetPickability                  (othis,1);
		OPlotterSetRequireRepresentationFunction (othis,NULL,NULL,1);
		DoRepresentation                        (This);
	      }
	    if(athis.layout==OLayoutXV)
	      status = XoCameraCollectInRegion (This,pointer.beg.x,0,pointer.end.x,This->core.height,0);
	    else
	      status = XoCameraCollectInRegion (This,pointer.beg.x,pointer.beg.y,pointer.end.x,pointer.end.y,0);

	    if( (status==1) && (IsFitEnabled(This)==1) ) /*Something picked and a fit over highlighted bins could be required.*/
	      {
		OPlotterSetRequireRepresentationFunction (othis,NULL,NULL,1);
		DoRepresentation                         (This);
	      }
           
	    {XoAnyCallbackStruct  value;
	     value.reason         = XoCR_COLLECT;
	     value.event          = a_event;
	     XtCallCallbacks      (This, XoNcollectCallback,(XtPointer)&value);}

            break;
      case MotionNotify:
/* Destroy previous box by redrawing it !!! (in mode COMP this is equivallent to a deletion ) */
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),thisCamera.rubberGC,&pointer.beg,&pointer.end);
            pointer.end.x = a_event->xmotion.x;
            pointer.end.y = a_event->xmotion.y;
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),thisCamera.rubberGC,&pointer.beg,&pointer.end);
            break;
    }
  a_argn = 0; /* to remove warning of c++ compiler */
  a_args = NULL;
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

  CreatePopup (This);

  if(Class.popup!=NULL)
    { 
#ifdef HAS_XM
      XmMenuPosition    (Class.popup,(XButtonPressedEvent*)a_event);
      XtManageChild     (Class.popup);
#endif
#ifdef HAS_XAW
     {Arg                 args[2];
      XtSetArg            (args[0], XtNx, ((XButtonPressedEvent*)a_event)->x_root);
      XtSetArg            (args[1], XtNy, ((XButtonPressedEvent*)a_event)->y_root);
      XtSetValues         (Class.popup, args,2);
      XtPopupSpringLoaded (Class.popup);}
#endif /*HAS_XM*/
      Class.This        = This;
      Class.This_event  = *a_event;
    }

  a_args  = NULL; 
  a_argn  = 0;
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
  if(thisCamera.initialDepth!=This->core.depth) /*XmCreatePopupMenu can't create a window from a 24 plans one !!!*/
    {
      int                 depth;
      Colormap            colormap;
      depth               = This->core.depth;
      colormap            = This->core.colormap;
      This->core.depth    = thisCamera.initialDepth;
      This->core.colormap = thisCamera.initialColormap;
      Class.popup         = XmCreatePopupMenu (This,"xoplotter_popup",args,1);
      This->core.depth    = depth;
      This->core.colormap = colormap;
    }
  else
    {
      Class.popup         = XmCreatePopupMenu (This,"xoplotter_popup",args,1);
    }}
#endif /*HAS_XM*/
#ifdef HAS_XAW
 {Arg         args[1];
  Class.popup = XtCreatePopupShell ("xoplotter_popup",simpleMenuWidgetClass,This,args,0);}
#endif /*HAS_XAW*/

  if(Class.popup==NULL) return;

#define ACTION_Destroy      1
#define ACTION_Bins         2
#define ACTION_Target       3
#define ACTION_Zoom         4
#define ACTION_Back         5
#define ACTION_Reset        6
#define ACTION_ScaleText    7
#define ACTION_Stats        8
#define ACTION_GIF          9
#define ACTION_PostScript  10
#define ACTION_DAWN        11
#define ACTION_Rotate      12
#define ACTION_Translate   13
#define ACTION_Scale       14
#define ACTION_Raise       15
#define ACTION_Center      16
#define ACTION_ZBuffer     17
#define ACTION_DBuffer     18
  XtAddCallback    (Class.popup   ,XtNdestroyCallback ,ActivateCallback,(XtPointer)ACTION_Destroy);

  /*Default menu.*/
  XoPlotterAddPopupEntry (This,"Bins"      ,ActivateCallback,(XtPointer)ACTION_Bins);
  XoPlotterAddPopupEntry (This,"Target"    ,ActivateCallback,(XtPointer)ACTION_Target);
  /*
  XoPlotterAddPopupEntry (This,"Zoom"      ,ActivateCallback,(XtPointer)ACTION_Zoom);
  XoPlotterAddPopupEntry (This,"Back"      ,ActivateCallback,(XtPointer)ACTION_Back);
  XoPlotterAddPopupEntry (This,"Reset"     ,ActivateCallback,(XtPointer)ACTION_Reset);
  */
  XoPlotterAddPopupEntry (This,"Scale text",ActivateCallback,(XtPointer)ACTION_ScaleText);
  XoPlotterAddPopupEntry (This,"Stats"     ,ActivateCallback,(XtPointer)ACTION_Stats);
  XoPlotterAddPopupEntry (This,"Raise"     ,ActivateCallback,(XtPointer)ACTION_Raise);
  if(CSystemIsSecured()==0)
    {
  XoPlotterAddPopupEntry (This,"GIF"       ,ActivateCallback,(XtPointer)ACTION_GIF);
  XoPlotterAddPopupEntry (This,"PostScript",ActivateCallback,(XtPointer)ACTION_PostScript);
  XoPlotterAddPopupEntry (This,"DAWN"      ,ActivateCallback,(XtPointer)ACTION_DAWN);
    }
  XoPlotterAddPopupSeparator (This,"sep");
  XoPlotterAddPopupEntry     (This,"Rotate"    ,ActivateCallback,(XtPointer)ACTION_Rotate);
  XoPlotterAddPopupEntry     (This,"Trans"     ,ActivateCallback,(XtPointer)ACTION_Translate);
  XoPlotterAddPopupEntry     (This,"Scale"     ,ActivateCallback,(XtPointer)ACTION_Scale);
  XoPlotterAddPopupEntry     (This,"Center"    ,ActivateCallback,(XtPointer)ACTION_Center);
  XoPlotterAddPopupEntry     (This,"Reset"     ,ActivateCallback,(XtPointer)ACTION_Reset);
  XoPlotterAddPopupEntry     (This,"Zbuffer"   ,ActivateCallback,(XtPointer)ACTION_ZBuffer);
  XoPlotterAddPopupEntry     (This,"Dbuffer"   ,ActivateCallback,(XtPointer)ACTION_DBuffer);
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
  OPlotter ogthis;
/*.........................................................................*/
  ogthis  = (OPlotter)XoPlotterGetPlotter(Class.This);
/*OCamera  ocamera;
  ocamera = ((XoPlotterWidget)Class.This)->camera.ocamera;*/
  if((long)a_tag==ACTION_Zoom)
    {
      XoPlotterMagnifyCollectedBins (Class.This);
    }
  else if((long)a_tag==ACTION_Back)
    {
      XoPlotterUndoMagnification (Class.This);
    }
  /*
  else if((long)a_tag==ACTION_Reset)
    {
      XoPlotterRestoreFirstMagnification (Class.This);
    }
    */
  else if((long)a_tag==ACTION_Bins)
    {
      char*            string;
      string           = OPlotterGetInformations (ogthis);
      CDumpF           ("---- Bins collected in XoPlotter : %s ----\n",XtName(Class.This));
      if(string!=NULL) CDump  (string);
      CStringDelete    (string);
    }
  else if((long)a_tag==ACTION_Target) 
    {
      XWidgetSetTarget      (Class.This);
    }
  else if((long)a_tag==ACTION_PostScript)
    {
      XWidgetSetCursor       (Class.This,"watch");
      XoCameraProducePS_File (Class.This,"out.ps");
      XWidgetSetCursor       (Class.This,NULL);
    }
  else if((long)a_tag==ACTION_DAWN)
    {
      XWidgetSetCursor        (Class.This,"watch");
      XoCameraProduceDAWN_File(Class.This,"out.prim");
      XWidgetSetCursor        (Class.This,NULL);
    }
  else if((long)a_tag==ACTION_GIF)
    { 
      XWidgetSetCursor            (Class.This,"watch");
      XWidgetSavePixmapInGIF_File (Class.This,"out.gif");
      XWidgetSetCursor            (Class.This,NULL);
    }
  else if((long)a_tag==ACTION_Raise)
    { 
      XWidgetRaise (Class.This);
    }
  else if((long)a_tag==ACTION_ScaleText)
    { 
      if( (((XoPlotterWidget)Class.This)->plot.textScale!=NULL) && 
          (strcmp(((XoPlotterWidget)Class.This)->plot.textScale,"1.")==0) )
	{
          CStringDelete (((XoPlotterWidget)Class.This)->plot.textScale);
	  ((XoPlotterWidget)Class.This)->plot.textScale = CStringDuplicate("2.");
	}
      else
	{
	  CStringDelete (((XoPlotterWidget)Class.This)->plot.textScale);
          ((XoPlotterWidget)Class.This)->plot.textScale = CStringDuplicate("1.");
	}
      OPlotterRequireRepresentation (ogthis);
    }
  else if((long)a_tag==ACTION_Stats)
    { 
      if(((XoPlotterWidget)Class.This)->plot.statisticsVisible==True)
	((XoPlotterWidget)Class.This)->plot.statisticsVisible = False;
      else
	((XoPlotterWidget)Class.This)->plot.statisticsVisible = True;
      OPlotterRequireRepresentation (ogthis);
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
  else if((long)a_tag==ACTION_Center)
    {
      XoCameraCenterMatrixOnPick (Class.This);
    }
  else if((long)a_tag==ACTION_Reset)
    {
      XoPlotterSetLayout (Class.This);
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

  This   = NULL;
  a_data = NULL;
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
  name  = CStringCreateF (16+strlen(a_label),"xoplotter_popup_%s",a_label);
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
/***************************************************************************/
/***************************************************************************/
void XoPlotterAddPopupEntry (
 Widget This
,char* a_label
,XtCallbackProc a_proc
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*            name;
  Arg              args[1];
  Widget           widget= NULL;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_label==NULL) return;
  if(a_proc==NULL)  return;
  CreatePopup       (This);
  if(Class.popup==NULL) return;
  if(FindPopupEntry(a_label)!=NULL) return;
  name             = CStringCreateF (16+strlen(a_label),"xoplotter_popup_%s",a_label);
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
void XoPlotterAddPopupSeparator (
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
  name             = CStringCreateF (16+strlen(a_label),"xoplotter_popup_%s",a_label);
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
char** XoPlotterGetResourceOptions (
 char* a_type 
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_type==NULL) return NULL;
       if(strcmp(a_type,XoRLayout)==0)     return OLayoutGetNames(a_number);
  else if(strcmp(a_type,XoRReceiver)==0)   return GetReceiverNames(a_number);
  else if(strcmp(a_type,XoRBinBarType)==0) return GetBarTypeNames(a_number);
  else if(strcmp(a_type,XoRFitType)==0)    return OFitTypeGetNames(a_number);
  return NULL;
}
/***************************************************************************/
void XoPlotterClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OPlotterClearClass      ();
  OTupleClearClass        ();
  OHistogramClearClass    ();
  OScatterClearClass      ();
  OCutClearClass          ();
  OFunctionClearClass     ();
  HoClearClass            ();
}
/***************************************************************************/
void XoPlotterRequireRepresentation (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return;
  OPlotterRequireRepresentation (othis);
}
/***************************************************************************/
void XoPlotterUpdateNow (
 Widget This 
)
/***************************************************************************/
/*
  Force to build representation of plot and refresh camera immediatly.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return;
  OPlotterSetRequireRepresentationFunction (othis,NULL,NULL,1);
  DoRepresentation                        (This);
  XoCameraRefreshNow                      (This);
}
/***************************************************************************/
OPlotter XoPlotterGetPlotter (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return NULL;
  if(OPlotterIsValid(othis)==0) return NULL;
  return     othis;
}
/***************************************************************************/
void* XoPlotterGetTuple (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return NULL;
  if(OTupleIsValid(athis.otuple)==0) return NULL;
  return (void*)(athis.otuple);
}
/***************************************************************************/
void XoPlotterSetLayout (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OLayout layout;
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return;
  layout                = OPlotterGetLayout(othis);
  OCameraSetAspectRatio (thisCamera.ocamera,(int)athis.aspectRatio);
  OCameraSetPlotLayout  (thisCamera.ocamera,(layout==OLayoutXYV) || (layout==OLayoutXYZ) ? 1 : 0);
  athis.layout          = layout;
}
/***************************************************************************/
void XoPlotterSetPlottedObjects (
 Widget This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return;
  CStringDelete                 (athis.plottedObjects);
  athis.plottedObjects          = CStringDuplicate (a_string);
  OPlotterRequireRepresentation (othis);
}
/***************************************************************************/
void XoPlotterMagnifyCollectedBins (
 Widget This
)
/***************************************************************************/
/*
  Only for XoPlotter connected to an OTuple.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram    hist = NULL;
  int      autoscale = 0;
  int      nbin;
  int*     is;
  int*     js;
  double   xmin,xmax,ymin,ymax,dbin;
/*.........................................................................*/
  if(This==NULL) return;
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return;
/* get hist and bins min,max */
  hist = (OHistogram)OPlotterGetHighlightedBins (othis,&nbin,&is,&js);
  if(OHistogramGetBinsRange(hist,nbin,is,js,&xmin,&xmax,&ymin,&ymax)==1)
    {
/* suppress autoscale and change axe min,max by leting bin width constant */
      if( (OHistogramGetDimension(hist)==1)  && (xmin<xmax) )
	{ 
	  PutHistogramParametersOnStack (This,hist);
	  dbin                          = OHistogramGetIthAxisBinWidth (hist,OAxisX);
	  nbin                          = DBL_INT ((xmax-xmin)/dbin);
	  OPlotterSetIthAxisAttributesF (othis,OAxisX,"anmx",autoscale,nbin,xmin,xmax);
	  UpdateAxes                    (This);
	}
      else if( (OHistogramGetDimension(hist)==2) && (xmin<xmax) && (ymin<ymax) )
	{
	  PutHistogramParametersOnStack (This,hist);
	  dbin                          = OHistogramGetIthAxisBinWidth (hist,OAxisX);
	  nbin                          = DBL_INT ((xmax-xmin)/dbin);
	  OPlotterSetIthAxisAttributesF (othis,OAxisX,"anmx",autoscale,nbin,xmin,xmax);
	  dbin                          = OHistogramGetIthAxisBinWidth (hist,OAxisY);
	  nbin                          = DBL_INT ((ymax-ymin)/dbin);
	  OPlotterSetIthAxisAttributesF (othis,OAxisY,"anmx",autoscale,nbin,ymin,ymax);
	  UpdateAxes                    (This);
	}
    }
  CMemoryFreeBlock           (is);
  CMemoryFreeBlock           (js);
}
/***************************************************************************/
void XoPlotterUndoMagnification (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int autoscale = 0;
/*.........................................................................*/
  if(This==NULL) return; 
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return;
  athis.zoom_count--;
  if(athis.zoom_count<0) {athis.zoom_count = 0;return;}
  OPlotterSetIthAxisAttributesF (othis,
                      OAxisX,"anmx",autoscale,athis.zoom_list[athis.zoom_count].inbin,
	                                     athis.zoom_list[athis.zoom_count].imin,
                                             athis.zoom_list[athis.zoom_count].imax);
  OPlotterSetIthAxisAttributesF (othis,
                      OAxisY,"anmx",autoscale,athis.zoom_list[athis.zoom_count].jnbin,
                                             athis.zoom_list[athis.zoom_count].jmin,
                                             athis.zoom_list[athis.zoom_count].jmax);
  UpdateAxes (This);
}
/***************************************************************************/
void XoPlotterRestoreFirstMagnification (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int autoscale = 0;
/*.........................................................................*/
  if(This==NULL) return; 
  if(!XtIsSubclass(This,xoPlotterWidgetClass)) return;
  athis.zoom_count = 0;
  OPlotterSetIthAxisAttributesF (othis,
                      OAxisX,"anmx",autoscale,athis.zoom_list[athis.zoom_count].inbin,
                                             athis.zoom_list[athis.zoom_count].imin,
                                             athis.zoom_list[athis.zoom_count].imax);
  OPlotterSetIthAxisAttributesF (othis,
                      OAxisY,"anmx",autoscale,athis.zoom_list[athis.zoom_count].jnbin,
                                             athis.zoom_list[athis.zoom_count].jmin,
                                             athis.zoom_list[athis.zoom_count].jmax);
  UpdateAxes (This);
}
/***************************************************************************/
static void PutHistogramParametersOnStack (
 Widget This  
,OHistogram a_hist  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return; 
  if(athis.zoom_count>=MAX__ZOOM) athis.zoom_count--;
  OHistogramGetIthAxisAttributesF (a_hist,OAxisX,"nmx",&(athis.zoom_list[athis.zoom_count].inbin),
		                   &(athis.zoom_list[athis.zoom_count].imin),
                                   &(athis.zoom_list[athis.zoom_count].imax));
  OHistogramGetIthAxisAttributesF (a_hist,OAxisY,"nmx",&(athis.zoom_list[athis.zoom_count].jnbin),
                                   &(athis.zoom_list[athis.zoom_count].jmin),
                                   &(athis.zoom_list[athis.zoom_count].jmax));
  athis.zoom_count++;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ConvertStringToFitType (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OFitType value = OFitTypeNone;
/*.........................................................................*/
  value        = OFitTypeGetIdentifier(a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OFitType);
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
static void ConvertFitTypeToString (
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
  string       = OFitTypeGetName(*((OFitType *)a_source->addr));
  if(string==NULL) return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
static void ConvertStringToReceiver (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OReceiver value = OReceiverHistogram;
/*.........................................................................*/
  value        = GetReceiverIdentifier(a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OReceiver);
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertHistTypeToString (
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
  string       = GetReceiverName(*((OReceiver *)a_source->addr));
  if(string==NULL) return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToBarType (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OBarType value = OBarNone;
/*.........................................................................*/
  value        = GetBarTypeIdentifier(a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OBarType);
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
static void ConvertBarTypeToString (
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
  string       = GetBarTypeName(*((OBarType *)a_source->addr));
  if(string==NULL) return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args = NULL;
  a_argn = 0;
}
/***************************************************************************/
static void ConvertStringToLayout (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OLayout value = OLayoutXV;
/*.........................................................................*/
  value        = OLayoutGetIdentifier(a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OLayout);
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertLayoutToString (
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
  string       = OLayoutGetName(*((OLayout *)a_source->addr));
  if(string==NULL)  return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertStringToXY_Modeling (
 XrmValuePtr a_args
,int*        a_argn
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static OXY_Modeling value = OXY_ModelingRandom;
/*.........................................................................*/
  value        = OXY_ModelingGetIdentifier(a_source->addr);
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(OXY_Modeling);
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
static void ConvertXY_ModelingToString (
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
  string            = OXY_ModelingGetName(*((OXY_Modeling *)a_source->addr));
  if(string==NULL)  return;
  strcpy       (Class.string,string);
  a_dest->size = (unsigned int)(strlen(Class.string)+1);  
  a_dest->addr = (XPointer)Class.string;      
  a_args       = NULL; 
  a_argn       = 0;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static struct {
  char*    string;
  OBarType value;
} OBarTypeList [] = {
  {"none",        OBarNone},
  {"sigma",       OBarSigma},
  {"mean_error",  OBarMeanError}
};  
/***************************************************************************/
static char** GetBarTypeNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* binBarType[3] = {"none","sigma","mean_error"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 3;
  return binBarType;
}
/***************************************************************************/
static OBarType GetBarTypeIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OBarTypeList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OBarTypeList[count].string)==0) ) 
      return OBarTypeList[count].value;
  CWarnF ("%s not an OBarType.\n",a_string);
  return OBarSigma;
}
/***************************************************************************/
static char* GetBarTypeName (
 OBarType a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OBarTypeList);count++)
    if(a_value==OBarTypeList[count].value) 
      return OBarTypeList[count].string;
  CWarnF ("%d not an OBarType.\n",a_value);
  return NULL;
}
static struct {
  char*     string;
  OReceiver value;
} OReceiverList [] = {
  {"histogram",     OReceiverHistogram},
  {"scatter",       OReceiverScatter},
  {"profile",       OReceiverProfile}
};  
/***************************************************************************/
static char** GetReceiverNames (
 int* a_number
)
/***************************************************************************/
/* CMemoryFreeBlock returned list when used */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* histType[3] = {"histogram","scatter","profile"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 3;
  return histType;
}
/***************************************************************************/
static OReceiver GetReceiverIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OReceiverList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OReceiverList[count].string)==0) ) 
      return OReceiverList[count].value;
  CWarnF ("%s not an OReceiver.\n",a_string);
  return OReceiverHistogram;
}
/***************************************************************************/
static char* GetReceiverName (
 OReceiver a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<XtNumber(OReceiverList);count++)
    if(a_value==OReceiverList[count].value) 
      return OReceiverList[count].string;
  CWarnF ("%d not an OReceiver.\n",a_value);
  return NULL;
}
/***************************************************************************/
static char* GetDefaultTitle (
 int   a_dim
,char* a_axeXbind
,char* a_axeYbind
,int a_profile
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.string[0] = '\0';
  if( (a_dim==2) && (a_axeXbind!=NULL) && (a_axeYbind!=NULL) )
    {
      sprintf(Class.string,"%s vs %s",a_axeXbind,a_axeYbind);
    }
  else if(a_dim==1)
    {
      if( (a_profile==1) && (a_axeXbind!=NULL) && (a_axeYbind!=NULL) )
        sprintf(Class.string,"%s vs Profile of %s",a_axeXbind,a_axeYbind);
      else if(a_axeXbind!=NULL)
        sprintf(Class.string,"%s distribution",a_axeXbind);
    }
  return Class.string;
}
/***************************************************************************/
static char* GetHistogramInfos (
 OHistogram This 
,char* a_axeXbind
,char* a_axeYbind
,Boolean a_short
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.string[0] = '\0';
  if(This==NULL) return Class.string;
  switch(OHistogramGetDimension(This))
    { 
      case 1:
        if(a_short==True)
	  {
	    sprintf(Class.string,
"\
 N : %d\n\
<X : %d\n\
X< : %d",
                            OHistogramGetNumberOfEntries(This),
                            OHistogramGetIthAxisUnderflow(This,OAxisX),
                            OHistogramGetIthAxisOverflow(This,OAxisX));
	  }
        else if(a_axeXbind!=NULL)
	  {
	    sprintf(Class.string,
"\
     N : %d\n\
    <X : %d\n\
    X< : %d\n\
   <X> : %g\n\
rms(X) : %g\n\
     X : %s",
                            OHistogramGetNumberOfEntries(This),
                            OHistogramGetIthAxisUnderflow(This,OAxisX),
                            OHistogramGetIthAxisOverflow(This,OAxisX),
                            OHistogramGetIthAxisMeanValue(This,OAxisX),OHistogramGetIthAxisStandardDeviation(This,OAxisX),a_axeXbind);
	  }
        else
	  {
	    sprintf(Class.string,
"\
     N : %d\n\
    <X : %d\n\
    X< : %d\n\
   <X> : %g\n\
rms(X) : %g",
                            OHistogramGetNumberOfEntries(This),
                            OHistogramGetIthAxisUnderflow(This,OAxisX),
                            OHistogramGetIthAxisOverflow(This,OAxisX),
                            OHistogramGetIthAxisMeanValue(This,OAxisX),OHistogramGetIthAxisStandardDeviation(This,OAxisX));
	  }
        return     Class.string;
      case 2:
        if( (a_axeXbind!=NULL) && (a_axeYbind!=NULL) )
	  {
	    sprintf(Class.string,
"\
     N : %d\n\
   <X> : %g\n\
rms(X) : %g\n\
     X : %s\n\
   <Y> : %g\n\
rms(Y) : %g\n\
     Y : %s",
                            OHistogramGetNumberOfEntries(This),
                            OHistogramGetIthAxisMeanValue(This,OAxisX),OHistogramGetIthAxisStandardDeviation(This,OAxisX),a_axeXbind,
                            OHistogramGetIthAxisMeanValue(This,OAxisY),OHistogramGetIthAxisStandardDeviation(This,OAxisY),a_axeYbind);
	  }
        else
	  {
	    sprintf(Class.string,
"\
     N : %d\n\
   <X> : %g\n\
rms(X) : %g\n\
   <Y> : %g\n\
rms(Y) : %g",
                            OHistogramGetNumberOfEntries(This),
                            OHistogramGetIthAxisMeanValue(This,OAxisX),OHistogramGetIthAxisStandardDeviation(This,OAxisX),
                            OHistogramGetIthAxisMeanValue(This,OAxisY),OHistogramGetIthAxisStandardDeviation(This,OAxisY));
	  }
        return     Class.string;
      default:
        return     Class.string;
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
  CErrorHandle ("Xo","XoPlotter",a_name,a_code);
}
