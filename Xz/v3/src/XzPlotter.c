/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
static char what[] = "@(#)Xz v3";

/*
#define DEBUG
*/

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <stdarg.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

/*Co*/
#include <CMemory.h>
#include <CString.h>
#include <CText.h>
#include <CPrinter.h>
#include <FString.h>
/*Xx*/
#include <XWidget.h>
/*Xz*/
#include <HBook.h>
#include <HPlot.h>
#include <HFile.h>

#define IXHIGZ
#include <HF77.h>

/*Private*/
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>
#include <X11/ConstrainP.h>

typedef enum {
    CommandNone        = 0,
    CommandPlotHist,
    CommandPlotLego,
    CommandPlotTuple,
    CommandFit,
    CommandScale,
    CommandParameter,
    CommandOption,
    CommandTitle,
    CommandPageTitle,
    CommandAxesTitle,
    CommandZones
} CommandType;

#include <XzPlotterP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void             InitializeClass            (void);
static void             InitializeWidget           (Widget, Widget,ArgList,Cardinal*);
static void             RealizeWidget              (Widget, XtValueMask*, XSetWindowAttributes*);
static void             DestroyWidget              (Widget);
static void             ChangeWidgetSize           (Widget);
static void             DrawWidget                 (Widget, XEvent*, Region);
static Boolean          SetValues                  (Widget,Widget,Widget,ArgList,Cardinal *);
static XtGeometryResult GeometryManager            (Widget,XtWidgetGeometry*,XtWidgetGeometry*);
static void             MoveAction                 (Widget,XEvent*, String* ,Cardinal*);
static void             CollectAction              (Widget,XEvent*, String* ,Cardinal*);
static void             ShowAction                 (Widget,XEvent*, String* ,Cardinal*);
static HplotCommand     AddCommand                 (Widget,CommandType,...);
static void             ExecuteCommand             (Widget,HplotCommand);
static void             SendBuild                  (Widget);
static void             SendRefresh                (Widget);
static void             HandleMessage              (Widget, XtPointer, XEvent*);
static void             DoBuild                    (Widget);
static void             BuildCommandsFromResources (Widget);
static void             BuildCommandsFromCommands  (Widget);
static void             DoRefresh                  (Widget);
static void             GetBins                    (Widget,int,int,int,int);
static int              GetIdentifier              (Widget);
static void             ClearCommands              (Widget);
static void             TreatLine                  (Widget,char*);
static char**           GetLines                   (char*,int*);
static int              GetHBOOK_Object            (Widget,char*);
static void             SetColor                   (int,double,double,double);
#ifdef __cplusplus
}
#endif

#define NotFound        (-1)
#define MAXIMUM(a,b)    ((a)>(b)?a:b)
#define DBL_INT         (int)(float)
#define athis           ((XzPlotterWidget)This)->hplot
#define acur            ((XzPlotterWidget)a_current)->hplot
#define IFMOD(a_field)  if(athis.a_field != acur.a_field)
#define IFSMOD(a_field) if(CStringCompare(athis.a_field,acur.a_field)==0)

static struct 
{
  Display*                display;
  Window                  window;
  int                     wkid;
  int                     wid;
  Window                  pixmap;
  Dimension               pixw,pixh;
  Cursor                  cursor_fleur,cursor_top_left_arrow;
  XzPlotterCallbackStruct call_data;
} Class = {NULL,0L,1,0,0L,0,0,0L,0L,{0,NULL,0,0,0}};


static char translations[] =
            "\
                 Ctrl <Btn1Down>: move()\n\
         Ctrl Button1 <PtrMoved>: move()\n\
                   Ctrl <Btn1Up>: move()\n\
                Shift <Btn1Down>: collect(Shift)\n\
        Shift Button1 <PtrMoved>: collect(Shift)\n\
                  Shift <Btn1Up>: collect(Shift)\n\
                 Lock <Btn1Down>: show_pixel()\n\
         Lock Button1 <PtrMoved>: show_pixel()\n\
                   Lock <Btn1Up>: show_pixel()\n\
                      <Btn1Down>: collect(None)\n\
              Button1 <PtrMoved>: collect(None)\n\
                        <Btn1Up>: collect(None)";

static XtActionsRec actions[] = 
{
  { "move"      , (XtActionProc)MoveAction},
  { "collect"   , (XtActionProc)CollectAction},
  { "show_pixel", (XtActionProc)ShowAction}
};

static XtResource resources [] ={
  {XzNcollectCallback,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
                               XtOffset(XzPlotterWidget, hplot.collectCallback),XtRImmediate,(XtPointer)NULL},
  {XzNcollectIdentifier   ,"CollectIdentifier" , XtRInt, sizeof(int),
                               XtOffset(XzPlotterWidget,hplot.collectIdentifier)      ,XtRImmediate,NULL},
  {XzNcollectBeginPosition,"CollectBeginPosition" , XtRInt, sizeof(int),
                               XtOffset(XzPlotterWidget,hplot.collectBeginPosition)   ,XtRImmediate,NULL},
  {XzNcollectEndPosition  ,"CollectEndPosition"   , XtRInt, sizeof(int),
                               XtOffset(XzPlotterWidget,hplot.collectEndPosition)     ,XtRImmediate,NULL},
  {XzNzones               ,"Zones"   , XtRString   , sizeof(String),
                               XtOffset(XzPlotterWidget,hplot.zones)   ,XtRString,NULL},
  {XzNplottedObjects      , "PlottedObjects"        , XtRString   , sizeof(String),
                               XtOffset(XzPlotterWidget, hplot.plottedObjects),XtRString ,NULL},
  {XzNuserCallback,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
                               XtOffset(XzPlotterWidget,hplot.user_callback),XtRImmediate,(XtPointer)NULL},
  {XzNcommands     ,"Commands"    , XtRString   , sizeof(String)   ,
                            XtOffset(XzPlotterWidget,  hplot.commands)    ,XtRString   ,NULL},
  {XzNhistogramContext      ,"HistogramContext"   , XtRString   , sizeof(String),
                               XtOffset(XzPlotterWidget,hplot.histogramContext)   ,XtRString ,NULL},
  {XzNlayout          ,"Layout" , XtRString, sizeof(String),
                               XtOffset(XzPlotterWidget, hplot.layout) ,XtRString,NULL},
  {XzNstatisticsVisible   ,"StatisticsVisible"     , XtRBoolean   , sizeof(Boolean),
                  XtOffset(XzPlotterWidget, hplot.statisticsVisible)   ,XtRImmediate,(XtPointer)False}, /*HPLOT default.*/
  {XzNtitle    ,"Title"    , XtRString   , sizeof(String)   ,
                            XtOffset(XzPlotterWidget,  hplot.title)    ,XtRString   ,NULL},
  {XzNpageTitle    ,"PageTitle"    , XtRString   , sizeof(String)   ,
                            XtOffset(XzPlotterWidget,  hplot.pageTitle)    ,XtRString   ,NULL},
  {XzNoptions      ,"Options"    , XtRString   , sizeof(String)   ,
                            XtOffset(XzPlotterWidget,  hplot.options)    ,XtRString   ,NULL},
  {XzNparameters   ,"Parameters"    , XtRString   , sizeof(String)   ,
                            XtOffset(XzPlotterWidget,  hplot.parameters)    ,XtRString   ,NULL},
  {XzNfitType      ,"FitType"    , XtRString   , sizeof(String)   ,
                            XtOffset(XzPlotterWidget,  hplot.fitType)    ,XtRString   ,NULL},
  {XzNvAxisLogScale        , "VAxisLogScale"          , XtRBoolean  , sizeof(Boolean),
                           XtOffset(XzPlotterWidget, hplot.vAxisLogScale) ,XtRImmediate,(XtPointer)False},
  {XzNvAxisAutomatic       , "VAxisAutomatic"         , XtRBoolean  , sizeof(Boolean),
                           XtOffset(XzPlotterWidget, hplot.vAxisAutomatic),XtRImmediate,(XtPointer)True},
  {XzNvAxisMinimum        , "VAxisMinimum"          , XtRString   , sizeof(String)  ,
                           XtOffset(XzPlotterWidget, hplot.vAxisMinimum) ,XtRString   ,(XtPointer)"0."},
  {XzNvAxisMaximum        , "VAxisMaximum"          , XtRString   , sizeof(String)  ,
                           XtOffset(XzPlotterWidget, hplot.vAxisMaximum) ,XtRString   ,(XtPointer)"0."},
  {XzNvAxisTitle          , "VAxisTitle"          , XtRString   , sizeof(String)  ,
                           XtOffset(XzPlotterWidget, hplot.vAxisTitle) ,XtRString   ,(XtPointer)NULL},
  {XzNgridsVisible    ,"GridsVisible"   , XtRBoolean   , sizeof(Boolean)    ,
                           XtOffset(XzPlotterWidget, hplot.gridsVisible)   ,XtRImmediate,(XtPointer)False},
  {XzNbarsVisible    ,"BarsVisible"   , XtRBoolean   , sizeof(Boolean)    ,
                           XtOffset(XzPlotterWidget, hplot.barsVisible)   ,XtRImmediate,(XtPointer)False},
  {XzNxyModeling           ,"XY_Modeling"   , XtRString   , sizeof(String),
                           XtOffset(XzPlotterWidget,hplot.xyModeling)   ,XtRString,(XtPointer)"random"}
			      
};

XzPlotterClassRec  xzPlotterClassRec = {
/* Core Class Part */
{
    (WidgetClass) &constraintClassRec,         /* pointer to superclass ClassRec   */
    "XzPlotter",         	/* widget resource class name       */
    sizeof(XzPlotterRec),	/* size in bytes of widget record   */
    InitializeClass,            /* class_initialize                 */
    NULL, 			/* dynamic initialization           */
    FALSE,       		/* has class been initialized?      */
    InitializeWidget,           /* initialize                       */
    NULL,			/* notify that initialize called    */
    RealizeWidget,              /* XCreateWindow for widget         */
    actions,     		/* widget semantics name to proc map*/
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
    NULL,			/* notify that SetValues called     */
    XtInheritSetValuesAlmost,   /* SetValues got "Almost" geo reply */
    NULL,		        /* notify that get_values called    */
    XtInheritAcceptFocus,       /* assign input focus to widget     */
    XtVersion,	                /* version of intrinsics used       */
    NULL,		        /* list of callback offsets         */
    translations,	        /* state machine                    */
    XtInheritQueryGeometry,     /* return preferred geometry        */
    XtInheritDisplayAccelerator,/* display your accelerator         */
    NULL,		        /* pointer to extension record      */
},
/* Composite Class Part */
{
    GeometryManager,            /* geometry manager for children   */
    XtInheritChangeManaged,     /* change managed state of child   */
    XtInheritInsertChild,       /* physically add child to parent  */
    XtInheritDeleteChild,       /* physically remove child         */
    NULL,		        /* pointer to extension record     */
},
/* Constraint Class Part */
{
    NULL,		        /* constraint resource list            */
    0,			        /* number of constraints in list       */
    0,			        /* size of constraint record           */
    NULL,		        /* constraint initialization           */
    NULL,	                /* constraint destroy proc             */
    NULL,		        /* constraint SetValues proc           */
    NULL,		        /* pointer to extension record         */
},
/* XzPlotter */
{
   NULL
}
   
};
WidgetClass xzPlotterWidgetClass = (WidgetClass) &xzPlotterClassRec;
/***************************************************************************/
static void InitializeClass (
 void
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int wktp  = 0;
/*.........................................................................*/
#ifdef DEBUG
  printf          ("debug : XzPlotter : InitializeClass.\n");
#endif
  HBookInitialize (0);
  hplint          (&wktp);
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
#define MAXWK 10
/*.........................................................................*/
#ifdef DEBUG
  printf                   ("debug : XzPlotter : InitializeWidget : %s\n",XtName(This));
#endif

  if(Class.window==0L)
    { 
      int                     conid  = 1;
      int                     wtype  = 1;
      Class.display           = XtDisplay(This); 

      Class.window            = XCreateWindow (Class.display,XDefaultRootWindow(Class.display),
                                               0,0,10,10,0,This->core.depth,
                                               InputOutput,CopyFromParent,0L,NULL);

      ixsdswi                 (Class.display,Class.window);
      iopwk                   (&(Class.wkid),&conid,&wtype);
      iacwk                   (&(Class.wkid));
/* get HIGZ wid, X window id */
      oigwid                  (&(Class.wkid),&(Class.wid));
      SetColor                (0,1.,1.,1.);
      SetColor                (1,0.,0.,0.);
      SetColor                (2,1.,0.,0.);
      SetColor                (3,0.,1.,0.);
      SetColor                (4,0.,0.,1.);
      SetColor                (5,1.,1.,0.);
      SetColor                (6,1.,0.,1.);
      SetColor                (7,0.,1.,1.);
    }
 
  if(a_request->core.width<=0)  This->core.width  = 100;
  if(a_request->core.height<=0) This->core.height = 100;

  athis.cmdn            = 0;
  athis.cmds            = NULL;
  athis.refreshSent     = 0;
  athis.buildSent       = 0;
  athis.cmd_lego        = NULL;
  athis.tmpn            = 0;
  athis.tmps            = NULL;
/*HPLOT drawing resources.*/
  athis.zones           = CStringDuplicate(athis.zones);
  athis.title           = CStringDuplicate(athis.title);
  athis.pageTitle       = CStringDuplicate(athis.pageTitle);
  athis.options         = CStringDuplicate(athis.options);
  athis.parameters      = CStringDuplicate(athis.parameters);
  athis.plottedObjects  = CStringDuplicate(athis.plottedObjects);
  athis.histogramContext = CStringDuplicate(athis.histogramContext);
  athis.layout          = CStringDuplicate(athis.layout);
  athis.fitType         = CStringDuplicate(athis.fitType);
  athis.vAxisMinimum    = CStringDuplicate(athis.vAxisMinimum);
  athis.vAxisMaximum    = CStringDuplicate(athis.vAxisMaximum);
  athis.vAxisTitle      = CStringDuplicate(athis.vAxisTitle);
  athis.xyModeling      = CStringDuplicate(athis.xyModeling);
/*HPLOT commands.*/
  athis.commands        = CStringDuplicate(athis.commands);

#ifdef DEBUG
  printf                ("debug : XzPlotter : InitializeWidget : end.\n");
#endif

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/
}
/***************************************************************************/
static void DestroyWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
#ifdef DEBUG
  printf ("debug : XzPlotter : DestroyWidget : %s\n",XtName(This));
#endif
  if(Class.pixmap!=0L) /*Release pixmap.*/
    {
      ixsavwi      ();
      ixselwi      (&(Class.wid));
      ixclpx       ();
      ixsetwi      ();
      XFreePixmap  (Class.display,Class.pixmap);
      Class.pixmap = 0L;
      Class.pixw   = 0;
      Class.pixh   = 0;
    }
  for(count=0;count<athis.tmpn;count++)  
    {
      int    ident;
      ident  = athis.tmps[count];
      if(hexist(&ident)!=0) hdelet (&ident);
    }
  CMemoryFreeBlock     (athis.tmps);
  athis.tmpn           = 0;
  athis.tmps           = NULL;
  ClearCommands        (This);
  CStringDelete        (athis.zones);
  CStringDelete        (athis.title);
  CStringDelete        (athis.pageTitle);
  CStringDelete        (athis.options);
  CStringDelete        (athis.parameters);
  CStringDelete        (athis.plottedObjects);
  CStringDelete        (athis.histogramContext);
  CStringDelete        (athis.layout);
  CStringDelete        (athis.commands);
  CStringDelete        (athis.fitType);
  CStringDelete        (athis.vAxisMinimum);
  CStringDelete        (athis.vAxisMaximum);
  CStringDelete        (athis.vAxisTitle);
  CStringDelete        (athis.xyModeling);
  athis.title          = NULL;
  athis.pageTitle      = NULL;
  athis.options        = NULL;
  athis.parameters     = NULL;
  athis.zones          = NULL;
  athis.plottedObjects = NULL;
  athis.histogramContext     = NULL;
  athis.layout         = NULL;
  athis.fitType        = NULL;
  athis.commands       = NULL;
  athis.vAxisMinimum   = NULL;
  athis.vAxisMaximum   = NULL;
  athis.vAxisTitle     = NULL;
  athis.xyModeling     = NULL;
  XtRemoveAllCallbacks (This, XzNcollectCallback);
  XtRemoveAllCallbacks (This, XzNuserCallback);
#ifdef DEBUG
  printf               ("debug : XzPlotter : DestroyWidget : end.\n");
#endif
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
  printf                   ("debug : XzPlotter : RealizeWidget : %s\n",XtName(This));
#endif
/* backing store */
  (*a_mask)               |= CWBackingStore;
  a_watbs->backing_store   =  NotUseful;

  (xzPlotterWidgetClass->core_class.superclass->core_class.realize)(This, a_mask, a_watbs);

  XtAddEventHandler        (This,0L,True,(XtEventHandler)HandleMessage,NULL);

  SendBuild                (This);

#ifdef DEBUG
  printf                   ("debug : XzPlotter : RealizeWidget : end.\n");
#endif
}
/***************************************************************************/
void XzPlotterClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.window!=0L) 
    {
      iclwk            (&(Class.wkid));
      XDestroyWindow   (Class.display,Class.window);
    }
  Class.window         = 0L;
  Class.display        = NULL;
  what[0] = '@'; /*c++ no warning.*/
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
                   SendBuild      (This);}\
  else /*not modified*/ {athis.a_field = acur.a_field;}
/*.........................................................................*/
  if(This==NULL) return False;

  SET_SRES (plottedObjects)
  SET_SRES (zones)
  SET_SRES (title)
  SET_SRES (pageTitle)
  SET_SRES (layout)
  SET_SRES (options)
  SET_SRES (parameters)
  SET_SRES (fitType)
  SET_SRES (commands)
  SET_SRES (vAxisMinimum)
  SET_SRES (vAxisMaximum)
  SET_SRES (vAxisTitle)
  SET_SRES (xyModeling)

  IFMOD(statisticsVisible) SendBuild (This);
  IFMOD(vAxisLogScale)     SendBuild (This);
  IFMOD(vAxisAutomatic)    SendBuild (This);
  IFMOD(gridsVisible)      SendBuild (This);
  IFMOD(barsVisible)       SendBuild (This);

  a_current = NULL;
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
  SendRefresh (This);
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
  SendRefresh (This);
  a_event     = NULL;
  a_region    = NULL;
}
/***************************************************************************/
static XtGeometryResult GeometryManager (
 Widget		   This
,XtWidgetGeometry* a_request
,XtWidgetGeometry* a_reply
)
/***************************************************************************/
/* debug */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  This      = NULL;  
  a_request = NULL;
  a_reply   = NULL;
  return    (XtGeometryYes);
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
	      Class.cursor_fleur          = XCreateFontCursor(XtDisplay(This),XC_fleur);
            if(Class.cursor_top_left_arrow==0L) 
	      Class.cursor_top_left_arrow = XCreateFontCursor(XtDisplay(This),XC_top_left_arrow);
            XDefineCursor     (XtDisplay(This),XtWindow(This),Class.cursor_fleur) ;
            pointer.ox        = a_event->xbutton.x;
            pointer.oy        = a_event->xbutton.y;
            break;
      case ButtonRelease:
            XDefineCursor     (XtDisplay(This),XtWindow(This),Class.cursor_top_left_arrow) ;
            break;
      case MotionNotify:
            if(athis.cmd_lego!=NULL) 
	      {
		int     vx,vy;
		int     delta;
		vx     = a_event->xmotion.x - pointer.ox;
		vy     = a_event->xmotion.y - pointer.oy;
		delta  = DBL_INT sqrt ((double)(vx*vx + vy*vy));
		if(delta<=10) break;
		delta = DBL_INT (abs(vx)>abs(vy) ? vx : -vy);
/* now delta aroud 5 degrees */
		if(abs(vx)>abs(vy)) /* move cursor horizontal */
		  {
		    athis.cmd_lego->phi   += ((double)delta)/2.;
		  }
		else 
		  {
		    athis.cmd_lego->theta -= ((double)delta)/2.;
		  }
		/*printf ("debug:delta:theta:%g phi:%g\n",athis.cmd_lego->theta,athis.cmd_lego->phi);*/
		DoRefresh (This);
	      }
            pointer.ox       = a_event->xmotion.x;
            pointer.oy       = a_event->xmotion.y;
            break;
    }
  a_args = NULL; /* to remove warning of c++ compiler */
  a_argn = 0; /* to remove warning of c++ compiler */
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
    GC      rubberGC;
  } pointer = {{0,0},{0,0},NULL};
/*.........................................................................*/
  switch(a_event->type)
    {
      case ButtonPress:
            pointer.end.x    = pointer.beg.x      = a_event->xbutton.x;
            pointer.end.y    = pointer.beg.y      = a_event->xbutton.y;
	    pointer.rubberGC = XDisplayGetRubberGC(XtDisplay(This),XtWindow(This));
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),pointer.rubberGC,&pointer.beg,&pointer.end);
            break;
      case ButtonRelease:
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),pointer.rubberGC,&pointer.beg,&pointer.end);
	    XFreeGC               (XtDisplay(This),pointer.rubberGC);
	    pointer.rubberGC      = NULL;

            if(athis.collectCallback!=NULL)
              { 
                Class.call_data.reason     = XzCR_COLLECT;
                Class.call_data.event      = a_event;
                GetBins                    (This,pointer.beg.x,pointer.beg.y,pointer.end.x,pointer.end.y);
                athis.collectIdentifier    = Class.call_data.id;
                athis.collectBeginPosition = Class.call_data.beg;
                athis.collectEndPosition   = Class.call_data.end;
                XtCallCallbacks            (This, XzNcollectCallback,(XtPointer)&(Class.call_data));
              }

            break;
      case MotionNotify:
/* Destroy previous box by redrawing it !!! (in mode COMP this is equivallent to a deletion ) */
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),pointer.rubberGC,&pointer.beg,&pointer.end);
            pointer.end.x = a_event->xmotion.x;
            pointer.end.y = a_event->xmotion.y;
            XDisplayDrawRectangle (XtDisplay(This),XtWindow(This),pointer.rubberGC,&pointer.beg,&pointer.end);
            break;
    }
  a_args = NULL;
  a_argn = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static void GetBins (
 Widget This
,int a_beg_x
,int a_beg_y
,int a_end_x
,int a_end_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           id;
/*.........................................................................*/
  Class.call_data.id       = 0;
  Class.call_data.beg      = 0;
  Class.call_data.end      = 0;

  if(This==NULL) return;
  id             = GetIdentifier  (This);
  if(id==0)      return;

  if(ohtype(&id)==1)
    { 
      FString    fstr;
      int        lstr;
      int        nx,ny;
      float      xmi,xma,ymi,yma;
      float      xsiz,ysiz,xl,yl,xh,yh;

      lstr       = 80;
      fstr       = FStringCreate(lstr);
      ohgivh     (&id,fstr,&lstr,&nx,&xmi,&xma,&ny,&ymi,&yma);
      FStringDelete (fstr);

      DoRefresh  (This);
      ohpsiz     (&xsiz,&ysiz);
      hplgiv     (&xl,&yl,&xh,&yh);

      if(a_beg_x==a_end_x) 
	{
	  float      xloc;
	  xloc       = ((float)a_beg_x)/((float)This->core.width);
	  xloc       = (xloc * xsiz - xl)/(xh-xl);
	  xloc       = xloc * (xma-xmi);
	  if ( (xloc>=xmi) && (xloc<xma))
	    { 
	      int     icx;
	      hxi     (&id,&xloc,&icx);
	      hi      (&id,&icx);
	      Class.call_data.id      = id;
	      Class.call_data.beg     = icx;
	      Class.call_data.end     = icx;
	    }
	  
	} 
      else 
	{
	  float    xb,xe;
	  xb       = ((float)a_beg_x)/((float)This->core.width);
	  xb       = (xb * xsiz - xl)/(xh-xl);
	  xb       = xb * (xma-xmi);
	  xe       = ((float)a_end_x)/((float)This->core.width);
	  xe       = (xe * xsiz - xl)/(xh-xl);
	  xe       = xe * (xma-xmi);
	  if ( (xb<xma) && (xe>=xmi))
	    { 
	      int          icb,ice;
	      if (xb<xmi)  icb = 1;
	      else         hxi    (&id,&xb,&icb);
	      if (xe>=xma) ice = nx;
	      else         hxi    (&id,&xe,&ice);
	      Class.call_data.id      = id;
	      Class.call_data.beg     = icb;
	      Class.call_data.end     = ice;
	    }
	}
    }
  a_beg_y = 0;
  a_end_y = 0;
}
/***************************************************************************/
static void ShowAction (
 Widget This
,XEvent* a_event
,String* a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  a_event = NULL;
  a_args  = NULL; /* to remove warning of c++ compiler */
  a_argn  = 0; /* to remove warning of c++ compiler */
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static HplotCommand AddCommand (
 Widget This
,CommandType a_type
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  char*    opt;
  char*    opt2;
/*.........................................................................*/
  if(a_type==CommandNone) return NULL;
  athis.cmds = athis.cmds!=NULL ? 
               (HplotCommandRec*)CMemoryChangeBlockSize((char*)athis.cmds,(athis.cmdn+1)*sizeof(HplotCommandRec)):
	       (HplotCommandRec*) CMemoryAllocateBlock(sizeof(HplotCommandRec));
  if(athis.cmds==NULL) 
    {
      athis.cmdn = 0;
      return NULL;
    }
  athis.cmds[athis.cmdn].type = a_type;
  athis.cmds[athis.cmdn].opt  = NULL;
  athis.cmds[athis.cmdn].opt2 = NULL;
  switch(a_type)
    {
      case CommandNone:    break;
      case CommandPlotHist:
                       va_start                   (args,a_type);
                       athis.cmds[athis.cmdn].id  = va_arg(args,int);
                       opt                        = va_arg(args,char*);
                       va_end                     (args);
                       athis.cmds[athis.cmdn].opt = CStringDuplicate(opt);
                       break;
      case CommandPlotLego:
                       va_start (args,a_type);
                       athis.cmds[athis.cmdn].id    = va_arg(args,int);
                       athis.cmds[athis.cmdn].theta = va_arg(args,double);
                       athis.cmds[athis.cmdn].phi   = va_arg(args,double);
                       va_end   (args);
                       athis.cmd_lego    = &(athis.cmds[athis.cmdn]);
                       break;
      case CommandPlotTuple:
                       va_start (args,a_type);
                       athis.cmds[athis.cmdn].id   = va_arg(args,int);
                       athis.cmds[athis.cmdn].varx = va_arg(args,int);
                       athis.cmds[athis.cmdn].vary = va_arg(args,int);
                       va_end   (args);
                       break;
      case CommandFit:
                       va_start (args,a_type);
                       athis.cmds[athis.cmdn].id   = va_arg(args,int);
                       opt                         = va_arg(args,char*);
                       athis.cmds[athis.cmdn].varx = va_arg(args,int);
                       va_end   (args);
                       athis.cmds[athis.cmdn].opt  = CStringDuplicate(opt);
                       break;
      case CommandScale:
                       va_start (args,a_type);
                       athis.cmds[athis.cmdn].id   = va_arg(args,int);
                       athis.cmds[athis.cmdn].theta = va_arg(args,double);
                       athis.cmds[athis.cmdn].phi   = va_arg(args,double);
                       va_end   (args);
                       break;
      case CommandParameter:
                       va_start (args,a_type);
                       opt                         = va_arg(args,char*);
                       athis.cmds[athis.cmdn].val  = va_arg(args,double);
                       va_end   (args);
                       athis.cmds[athis.cmdn].opt  = CStringDuplicate(opt);
                       break;
      case CommandOption:
      case CommandTitle:
      case CommandPageTitle:
                       va_start (args,a_type);
                       opt      = va_arg(args,char*);
                       va_end   (args);
                       athis.cmds[athis.cmdn].opt  = CStringDuplicate(opt);
                       break;
      case CommandAxesTitle:
                       va_start (args,a_type);
                       opt      = va_arg(args,char*);
                       opt2     = va_arg(args,char*);
                       va_end   (args);
                       athis.cmds[athis.cmdn].opt  = CStringDuplicate(opt);
                       athis.cmds[athis.cmdn].opt2 = CStringDuplicate(opt2);
                       break;
      case CommandZones:
                       va_start (args,a_type);
                       athis.cmds[athis.cmdn].varx = va_arg(args,int);
                       athis.cmds[athis.cmdn].vary = va_arg(args,int);
                       athis.cmds[athis.cmdn].varz = va_arg(args,int);
                       opt                         = va_arg(args,char*);
                       va_end   (args);
                       athis.cmds[athis.cmdn].opt  = CStringDuplicate(opt);
                       break;
    }
  athis.cmdn++;
  return &(athis.cmds[athis.cmdn-1]);
}
/***************************************************************************/
static void ExecuteCommand (
 Widget This
,HplotCommand a_cmd
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_cmd==NULL) return;
#ifdef DEBUG
  printf ("debug : XzPlotter : ExecuteCommand : %s : type : %d\n",XtName(This),a_cmd->type);
#endif
  switch(a_cmd->type)
    {
      case CommandNone:    break;
      case CommandPlotHist: 
                       if(hexist(&(a_cmd->id))==0) HBookBringDirectoryInMemory();
                       if(hexist(&(a_cmd->id))==0) break;
#ifdef DEBUG
		       printf ("debug : XzPlotter : ExecuteCommand : %s : PlotHistogram : %d\n",XtName(This),a_cmd->id);
#endif
                       HPlotDrawHistogram (a_cmd->id,a_cmd->opt);
                       break;
      case CommandPlotLego:
                       if(hexist(&(a_cmd->id))==0) HBookBringDirectoryInMemory();
                       if(hexist(&(a_cmd->id))==0) break;
                       { float  theta,phi;
                          theta  = a_cmd->theta;
                          phi    = a_cmd->phi;
                          hplego (&(a_cmd->id),&theta,&phi);
                       }
                       break;
      case CommandPlotTuple:
	               if(hexist(&(a_cmd->id))==0) HBookBringDirectoryInMemory();
                       if(hexist(&(a_cmd->id))==0) break;
                       {int   isel   = 0;
                        int   ifrom  = 1;
                        int   ito    = 1000000000;
                        char* uwfunc = NULL;
                        hplnt       (&(a_cmd->id),&isel,&uwfunc,&ifrom,&ito,&(a_cmd->varx),&(a_cmd->vary));
                       }
                       break;
      case CommandFit:
                       if(hexist(&(a_cmd->id))==0) HBookBringDirectoryInMemory();
                       if(hexist(&(a_cmd->id))==0) break;
                       {
		        if( (strcmp(a_cmd->opt,"gaussian")==0) || (strcmp(a_cmd->opt,"gau")==0) )
			  {
			    int    ic = 2;
			    float  c,av,sd,chi2,sig[3];
			    hfitga (&(a_cmd->id),&c,&av,&sd,&chi2,&ic,sig);
#ifdef DEBUG
	          printf ("debug : XzPlotter : fit : %d : %g %g %g %g : %g %g %g\n",a_cmd->id,c,av,sd,chi2,sig[0],sig[1],sig[2]);
#endif
			  }
		        else if( (strcmp(a_cmd->opt,"exponential")==0) || (strcmp(a_cmd->opt,"exp")==0) )
			  {
			    int    ic = 2;
			    float  aa,bb,chi2,sig[2];
			    hfitex (&(a_cmd->id),&aa,&bb,&chi2,&ic,sig);
			  }
		        else if( (strcmp(a_cmd->opt,"polynomial")==0) || (strcmp(a_cmd->opt,"pol")==0) )
			  {
			    int    ic = 2,np;
			    float* a;
			    float  chi2;
			    float* sig;
                            np     = a_cmd->varx;
			    if(np==0) np = 2;  
			    a   = (float*) CMemoryAllocateBlock( np * sizeof(float));
			    sig = (float*) CMemoryAllocateBlock( np * sizeof(float));
			    if( (a!=NULL) && (sig!=NULL) )
			      {
				hfitpo (&(a_cmd->id),&np,a,&chi2,&ic,sig);
			      }
			    CMemoryFreeBlock     (a);
			    CMemoryFreeBlock     (sig);
			  }
		        else if(strcmp(a_cmd->opt,"none")==0)
			  {
			  }
			else
			  {
			    CWarnF ("For %s, unsupported fit type : %s\n",XtName(This),a_cmd->opt);
			  }
                       }
                       break;
      case CommandScale:
                       if(hexist(&(a_cmd->id))==0) HBookBringDirectoryInMemory();
                       if(hexist(&(a_cmd->id))==0) break;
                       { float   cmin,cmax;
                          cmin   = a_cmd->theta;
                          cmax   = a_cmd->phi;
                          hminim (&(a_cmd->id),&cmin);
                          hmaxim (&(a_cmd->id),&cmax);
                       }
                       break;
      case CommandParameter:
                       HPlotSetParameter    (a_cmd->opt,a_cmd->val);
                       break;
      case CommandOption:
                       HPlotSetOption   (a_cmd->opt);
                       break;
      case CommandTitle:
                       HPlotSetTitle (a_cmd->opt);
                       break;
      case CommandPageTitle:
                       HBookSetTitle (a_cmd->opt);
                       break;
      case CommandAxesTitle:
                       HPlotSetAxesTitle (a_cmd->opt,a_cmd->opt2);
                       break;
      case CommandZones:
                       HPlotSetZones (a_cmd->varx,a_cmd->vary,a_cmd->varz,a_cmd->opt);
                       break;
    }
  This = NULL;
}
/***************************************************************************/
static int GetIdentifier (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int  count;
/*.........................................................................*/
  if(This==NULL) return 0;
  for(count=athis.cmdn-1;count>=0;count--) 
    { HplotCommand cmd;
      cmd      = &(athis.cmds[count]);
      switch(cmd->type)
        {
          case CommandPlotHist:
          case CommandPlotLego:
          case CommandPlotTuple:
                           return cmd->id;
          case CommandFit:
          case CommandScale:
          case CommandParameter:
          case CommandOption:
          case CommandTitle:
          case CommandPageTitle:
          case CommandAxesTitle:
	  case CommandZones:
          case CommandNone:    
                           break;
        }
    }
  return 0;
}
/***************************************************************************/
static void SendRefresh (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(athis.refreshSent==1) return;
  if(XWidgetSendMessage (This,XMSG_REFRESH,0L)==0) return;
  athis.refreshSent    = 1;
}
/***************************************************************************/
static void SendBuild (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(athis.buildSent==1) return;
  if(XWidgetSendMessage (This,XMSG_BUILD,0L)==0) return;
  athis.buildSent    = 1;
}
/***************************************************************************/
static void HandleMessage (
 Widget This
,XtPointer a_tag
,XEvent* a_event 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_event->type!=ClientMessage) return;

/* dispatch */
       if(a_event->xclient.data.l[0]==XMSG_REFRESH) 
    {
      DoRefresh         (This);
      athis.refreshSent = 0;
    }
  else if(a_event->xclient.data.l[0]==XMSG_BUILD) 
    {
      DoBuild         (This);
      athis.buildSent = 0;
    }

  a_tag         = NULL;
}
/***************************************************************************/
static void DoRefresh (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int       width,height;
/*.........................................................................*/
#ifdef DEBUG
  printf        ("debug : XzPlotter : DoRefresh : %s\n",XtName(This));
#endif
/* reset setup, options which are general (not attached to each wk) */
 {int           flag = 1;
  iclrwk        (&(Class.wkid),&flag);}

#ifdef DEBUG
  printf        ("debug : XzPlotter : DoRefresh : 1\n");
#endif

  width         = This->core.width;
  height        = This->core.height;
/* set up ratio so that histo map window size */
  oiswk         (&(Class.wkid),&width,&height);
  ixsavwi       ();
  ixselwi       (&(Class.wid));

#ifdef DEBUG
  printf        ("debug : XzPlotter : DoRefresh : 2\n");
#endif

  if( (Class.pixmap==0L)           || 
      ( (Class.pixw<This->core.width) || (Class.pixh<This->core.height) ) 
     )
    { int                  pixw,pixh;
#ifdef DEBUG
      printf        ("debug : XzPlotter : DoRefresh : 3\n");
#endif
      if(Class.pixmap!=0L) 
	{
	  ixclpx      ();
	  XFreePixmap (Class.display,Class.pixmap);
	}
      Class.pixw           = MAXIMUM(Class.pixw,This->core.width);
      Class.pixh           = MAXIMUM(Class.pixh,This->core.height);
      pixw                 = Class.pixw;
      pixh                 = Class.pixh;
      Class.pixmap        = (Drawable) ixopnpx (&pixw,&pixh); /*current HIGZ window drawing (cws->drawing) is now the pixmap.*/  
    }

#ifdef DEBUG
  printf         ("debug : XzPlotter : DoRefresh : pixmap : %lu : %d %d\n",Class.pixmap,Class.pixw,Class.pixh);
#endif

/* HPLOT orders that will draw in Class.pixmap */
  HPlotSetParameter   ("*",0.);
  HPlotSetOption      ("*");
  oisrat              (&width,&height);
  for(count=0;count<athis.cmdn;count++) ExecuteCommand (This,&(athis.cmds[count]));

/* send Class.pixmap to Widget window */
  if(Class.pixmap!=0L)
    {
      GC             gc;
      XGCValues      values;
      gc             = XtGetGC(This,0L,&values);
      XCopyArea      (Class.display,Class.pixmap,This->core.window,gc, 
		      0, Class.pixh-This->core.height,This->core.width, This->core.height, 0, 0 );
      XtReleaseGC    (This,gc);
    }

  ixsetwi        ();

#ifdef DEBUG
  printf         ("debug : XzPlotter : DoRefresh : end.\n");
#endif
}
/***************************************************************************/
static void DoBuild (
 Widget  This
)
/***************************************************************************/
/*
  Construct HPLOT command list executed when an XMSG_REFRESH message arrives.
  If athis.commands is NULL, XzPlotter HPLOT resources are read and a list 
 of HPLOT commands is constructed.
  If athis.commands is not NULL, XzPlotter HPLOT resources are ignored ;
 a list of HPLOT commands is constructed from athis.commands.  
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
/* Delete HBOOK objects resulting from previous convertions.*/
  for(count=0;count<athis.tmpn;count++)  
    {
      int    ident;
      ident  = athis.tmps[count];
      if(hexist(&ident)!=0) hdelet (&ident);
    }
  CMemoryFreeBlock   (athis.tmps);
  athis.tmpn         = 0;
  athis.tmps         = NULL;

/*Reset list of commands.*/
  ClearCommands      (This);

  if(athis.commands==NULL)
    {
      BuildCommandsFromResources (This);
    }
  else
    {
      BuildCommandsFromCommands  (This);
    }

  SendRefresh                (This);
}
/***************************************************************************/
static void BuildCommandsFromResources (
 Widget  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int   count;
  char* string;
/*.........................................................................*/
/*Zones.*/
  if(athis.zones!=NULL)
    {
      string        = CStringCreateF (6+strlen(athis.zones),"zones=%s",athis.zones);
      TreatLine     (This,string);
      CStringDelete (string);
    }

/*Statistics.*/
  if(athis.statisticsVisible==True)  AddCommand (This,CommandOption,"STA");
  else                               AddCommand (This,CommandOption,"NSTA");

/*Log scale.*/
  if(athis.vAxisLogScale==True)      AddCommand (This,CommandOption,"LOGY");
  else                               AddCommand (This,CommandOption,"LINY");

/*Grid.*/
  if(athis.gridsVisible==True)       AddCommand (This,CommandOption,"GRID");
  else                               AddCommand (This,CommandOption,"NGRI");

/*Page title. Set HBOOK title. Used by default by HPLOT to set page title.*/
  if(athis.pageTitle!=NULL) AddCommand (This,CommandPageTitle,athis.pageTitle);

/*Options. Could override statisticsVisible.*/
 {int    linen = 0;
  char** lines = NULL;
  lines  = GetLines (athis.options,&linen);
  if( (linen!=0) && (lines!=NULL) )
    {
      for(count=0;count<linen;count++)
	{
	  AddCommand (This,CommandOption,lines[count]);
	}
    }
  CTextDelete (linen,lines);}

/*Parameters.*/
 {int    linen = 0;
  char** lines = NULL;
  lines  = GetLines (athis.parameters,&linen);
  if( (linen!=0) && (lines!=NULL) )
    {
      for(count=0;count<linen;count++)
	{
          char* pos;
          pos   = strchr(lines[count],'=');
          if(pos==NULL)
	    {
	      pos   = strchr(lines[count],':');
	      if(pos==NULL)
		{
		  CWarnF ("For %s, bad syntax for resource \"parameters\" in : \n%s\n",XtName(This),lines[count]);
		}
	    }
	  else
	    {
	      int    status;
	      double value;
	      *pos   = '\0';
	      value  = CStringConvertToDouble(pos+1,&status);
	      if(status==0)      
		{
		  CWarnF ("For %s, bad syntax for resource \"parameters\" in : \n%s\n",XtName(This),lines[count]);
		}
	      else
		{
		  AddCommand (This,CommandParameter,lines[count],value);
		}
	    }
	}
    }
  CTextDelete (linen,lines);}

/*Things to plot.*/
 {int    linen = 0;
  char** lines = NULL;
  lines  = GetLines (athis.plottedObjects,&linen);
  if( (linen!=0) && (lines!=NULL) )
    {
      for(count=0;count<linen;count++)
	{
	  int obj;
          obj = GetHBOOK_Object (This,lines[count]);
	  if(obj!=NotFound)
	    {
	      int   type;
	      type = ohtype(&obj);
	      if(athis.fitType!=NULL) 
		{
		  AddCommand (This,CommandFit,obj,athis.fitType,0);
		}
	      if(athis.vAxisAutomatic==False)
		{
		  int    status;
		  double cmin = 0.,cmax = 0.;
		  cmin   = CStringConvertToDouble(athis.vAxisMinimum,&status);
		  if(status==0)      
		    {
		      CWarnF ("For %s, bad syntax for resource \"vAxisMinimum\" in : \n%s\n",XtName(This),athis.vAxisMinimum);
		    }
		  else
		    {
		      cmax = CStringConvertToDouble(athis.vAxisMaximum,&status);
		      if(status==0)      
			{
			  CWarnF ("For %s, bad syntax for resource \"vAxisMaximum\" in : \n%s\n",XtName(This),athis.vAxisMaximum);
			}
		      else
			{
			  AddCommand (This,CommandScale,obj,cmin,cmax);
			}
		    }
		}
	      else
		{
		  AddCommand (This,CommandScale,obj,0.,0.);
		}
	      if(type==1)     
		{
		  if(athis.barsVisible==True)
		    {
		      AddCommand (This,CommandPlotHist,obj," ");
		    }
		  else 
		    {
		      AddCommand (This,CommandPlotHist,obj,"HIST");
		    }
		  if(athis.vAxisTitle!=NULL) 
		    {
		      AddCommand (This,CommandAxesTitle,"",athis.vAxisTitle);
		    }
		}
	      else if(type==2)
		{
		  if(athis.layout!=NULL)
		    {
		      float theta = 30.,phi = 30.;
		      if(strcmp(athis.layout,"xyv")==0)
			{
			  AddCommand (This,CommandPlotLego,obj,(double)theta,(double)phi);
			}
		      else if(sscanf(athis.layout,"xyv,%f,%f",&theta,&phi)==2)
			{
			  AddCommand (This,CommandPlotLego,obj,(double)theta,(double)phi);
			}
		      else
			{
			  CWarnF ("For %s, bad syntax for resource \"layout\" in : \n%s\n",XtName(This),athis.layout);
			  AddCommand (This,CommandPlotLego,obj,(double)theta,(double)phi);  /*Must do something for good zoning.*/
			}
		    }
		  else  /*2-dim histo not represented as lego.*/
		    {
		      if(athis.xyModeling!=NULL)
			{
			       if(strcmp(athis.xyModeling,"box")==0)	         AddCommand (This,CommandPlotHist,obj,"BOX");
			  else if(strcmp(athis.xyModeling,"arrow")==0)           AddCommand (This,CommandPlotHist,obj,"ARR");
			  else if(strcmp(athis.xyModeling,"color")==0)           AddCommand (This,CommandPlotHist,obj,"COL");
			  else if(strcmp(athis.xyModeling,"text")==0)            AddCommand (This,CommandPlotHist,obj,"TEXT");
			  else if(strcmp(athis.xyModeling,"character")==0)       AddCommand (This,CommandPlotHist,obj,"CHAR");
			  else if(strcmp(athis.xyModeling,"contour")==0)         AddCommand (This,CommandPlotHist,obj,"CONT");
			  else if(strcmp(athis.xyModeling,"lego")==0)            AddCommand (This,CommandPlotHist,obj,"LEGO");
			  else if(strcmp(athis.xyModeling,"lego_colored")==0)    AddCommand (This,CommandPlotHist,obj,"LEGO1");
			  else if(strcmp(athis.xyModeling,"lego2")==0)           AddCommand (This,CommandPlotHist,obj,"LEGO2");
			  else if(strcmp(athis.xyModeling,"surface")==0)         AddCommand (This,CommandPlotHist,obj,"SURF");
			  else if(strcmp(athis.xyModeling,"surface_colored")==0) AddCommand (This,CommandPlotHist,obj,"SURF1");
			  else if(strcmp(athis.xyModeling,"surface2")==0)        AddCommand (This,CommandPlotHist,obj,"SURF2");
			  else            			                 AddCommand (This,CommandPlotHist,obj," ");
			}
                      else
  		        AddCommand (This,CommandPlotHist,obj," ");
		    }
		}
	    }
	}
    }
  CTextDelete (linen,lines);}
  
/*Title. If option UTIT, HPLOT takes title given with hpltit for the page title.*/
  if(athis.title!=NULL)     AddCommand (This,CommandTitle,athis.title);
}
/***************************************************************************/
static void BuildCommandsFromCommands (
 Widget  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    linen = 0;
  char** lines = NULL;
/*.........................................................................*/
  lines = GetLines (athis.commands,&linen);
  if( (linen!=0) && (lines!=NULL) )
    {
      int count;
      for(count=0;count<linen;count++)
	{
	  TreatLine (This,lines[count]);
	}
    }
  CTextDelete (linen,lines);
}
/***************************************************************************/
static void TreatLine (
 Widget This
,char* a_line
)
/***************************************************************************/
/*
   page_title[:,=]<string>
   title[:,=]<string>
   [opt,option][:,=]<string>
   [param,parameter][:,=]<string>,<value>
   axes[:,=]<string>,<string>
   zones[:,=]<int>,<int>,<int>,<string>
   plot[:,=]<ident>
   lego[:,=]<ident>
   lego[:,=]<ident>,<theta>,<phi>
   1or2[:,=]<ident>
   scale[:,=]<ident>,<cmin>,<cmax>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  pos;
/*.........................................................................*/
  if(a_line==NULL) return;
  pos = strchr(a_line,'=');
  if(pos==NULL)
    {
      pos = strchr(a_line,':');
    }
  if(pos==NULL)
    {
      CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
    }
  else
    {
      char*  verb;
      char*  string;
      *pos   = '\0';
      verb   = a_line;
      string = pos+1;
      if(*string=='\0')
	{
	  CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
	}
      else if(strcmp(verb,"page_title")==0)
	{
	  AddCommand (This,CommandPageTitle,string);
	}
      else if(strcmp(verb,"title")==0)
	{
	  AddCommand (This,CommandTitle,string);
	}
      else if( (strcmp(verb,"option")==0) || (strcmp(verb,"opt")==0) )
	{
	  AddCommand (This,CommandOption,string);
	}
      else
	{
	  int    status;
	  int    argn;
	  char** args;
          args   = CStringGetWords (string,",",&argn);
          if( (strcmp(verb,"parameter")==0) || (strcmp(verb,"param")==0) )
	    {
	      if(argn!=2)
		{
		  CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
		}
	      else
		{
		  double value;
		  value  = CStringConvertToDouble(args[1],&status);
		  if(status==0)      
		    {
		      CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
		    }
		  else
		    {
		      AddCommand  (This,CommandParameter,args[0],value);
		    }
		}
	    }
          else if(strcmp(verb,"axes")==0)
	    {
	      if(argn!=2)
		{
		  CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
		}
	      else
		{
		  AddCommand  (This,CommandAxesTitle,args[0],args[1]);
		}
	    }
	  else if(strcmp(verb,"zones")==0)
	    {
	      int   nx = 1,ny = 1,first = 1;
	      if(argn>=1)
		{
		  nx = (int)CStringConvertToLong(args[0],&status);
		  if(status==0)      
		    {
		      CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
		      nx = 1;
		    }
		}
	      if(argn>=2)
		{
		  ny = (int)CStringConvertToLong(args[1],&status);
		  if(status==0)      
		    {
		      CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
		      ny = 1;
		    }
		}
	      if(argn>=3)
		{
		  first = (int)CStringConvertToLong(args[2],&status);
		  if(status==0)      
		    {
		      CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
		      first = 1;
		    }
		}
#ifdef DEBUG
	      printf (" debug : %d %d %d %s\n",nx,ny,first);
#endif
	      if(argn==4)
		{
		  AddCommand (This,CommandZones,nx,ny,first,args[3]);
		}
	      else
		{
		  AddCommand (This,CommandZones,nx,ny,first," ");
		}
	    }
	  else if(argn>=1)
	    {
	      int obj;
	      obj = GetHBOOK_Object (This,args[0]);
	      if(obj!=NotFound)
		{
		  if( (strcmp(verb,"plot")==0) || (strcmp(verb,"hist")==0) )
		    {
		      AddCommand  (This,CommandPlotHist,obj," ");
		    }
		  else if(strcmp(verb,"1or2")==0)
		    {
		      int   type;
		      type = ohtype(&obj);
		      if(type==1)     
			{
			  AddCommand  (This,CommandOption  ,"STA");
			  AddCommand  (This,CommandPlotHist,obj," ");
			}
		      else if(type==2)     
			{
			  /* AddCommand  (This,CommandParameter  ,"2BUF",1.);*/
			  AddCommand  (This,CommandPlotLego,30.,30.);
			}
		      /* does not work
		      else if(type==3)     
		        {
			  AddCommand  (This,CommandPlotTuple,,);
		        }*/
		    }
		  else if(strcmp(verb,"lego")==0)
		    {
		      double theta = 30.,phi = 30.; 
		      if(argn==1)
			{
			  AddCommand  (This,CommandPlotLego,obj,theta,phi);
			}
		      else if(argn==3)
			{
			  int     status1,status2;
			  theta   = CStringConvertToDouble(args[1],&status1);
			  phi     = CStringConvertToDouble(args[2],&status2);
			  if( (status1==0) || (status2==0) )      
			    {
			      CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
			    }
			  else
			    {
			      AddCommand  (This,CommandPlotLego,obj,theta,phi);
			    }
			}
		    }
		  else if(strcmp(verb,"fit")==0)
		    {
		      if(argn==2)
			{
			  AddCommand  (This,CommandFit,obj,args[1],0);
			}
		      else if(argn==3) /*Polynomial, last arg is number of coefficients.*/
			{
			  int np;
			  np  = (int)CStringConvertToLong(args[2],&status);
			  if(status==0)      
			    {
			      CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
			    }
			  else
			    {
			      AddCommand  (This,CommandFit,obj,args[1],np);
			    }
			}
		      else
		        {
			  CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
			}
		    }
		  else if(strcmp(verb,"scale")==0)
		    {
		      double cmin = 0.,cmax = 0.;
		      if(argn==1)
			{
			  AddCommand  (This,CommandScale,obj,cmin,cmax);
			}
		      else if(argn==3)
			{
			  int     status1,status2;
			  cmin    = CStringConvertToDouble(args[1],&status1);
			  cmax    = CStringConvertToDouble(args[2],&status2);
			  if( (status1==0) || (status2==0) )      
			    {
			      CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
			    }
			  else
			    {
			      AddCommand  (This,CommandScale,obj,cmin,cmax);
			    }
			}
		    }
		}
	    }
	  else
	    {
	      CWarnF ("For %s, bad syntax in : \n%s\n",XtName(This),a_line);
	    }
	  CTextDelete (argn,args);
	}	  
    }
}
/***************************************************************************/
static char** GetLines (
 char* a_string
,int* a_linen
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  pos;
  int    linen = 0;
  char** lines = NULL;
/*.........................................................................*/
  if(a_linen!=NULL) *a_linen = 0;
  if(a_string==NULL) return NULL;
  pos   = strchr(a_string,';');
  if(pos!=NULL)
    {
      lines  = CStringGetWords (a_string,";",&linen);
    }
  else
    {
      lines  = CTextCreateFromString (a_string,&linen);
    }
  if(a_linen!=NULL) *a_linen = linen;
  return   lines; 
}
/***************************************************************************/
static void SetColor (
 int a_index
,double a_red
,double a_green
,double a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int   index;
  float red,green,blue;
/*.........................................................................*/
  index = a_index;
  red   = a_red;
  green = a_green;
  blue  = a_blue;
  iscr  (&(Class.wkid),&index,&red,&green,&blue);
}
/***************************************************************************/
static int GetHBOOK_Object (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int obj,status;
/*.........................................................................*/
  if(a_string==NULL) return NotFound;
  obj = (int)CStringConvertToLong(a_string,&status);
  if( (status!=0) && (hexist(&obj)!=0) ) /*HBOOK object*/
    {
      return obj;
    }
  else if( (obj=HBookConvert(a_string)) !=NotFound) /*Temporary HBOOK objects must be deleted at next rebuild.*/
    {
      athis.tmps = athis.tmps!=NULL ? 
               (int*)CMemoryChangeBlockSize((int*)athis.tmps,(athis.tmpn+1)*sizeof(int)):
	       (int*) CMemoryAllocateBlock (sizeof(int));
      if(athis.tmps==NULL) 
	{
	  athis.tmpn = 0;
	  return NotFound;
	}
      athis.tmps[athis.tmpn] = obj;
      athis.tmpn++;
      return obj;
    }
  return NotFound;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ClearCommands (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  for(count=0;count<athis.cmdn;count++)
    {
      CStringDelete          (athis.cmds[count].opt);
      CStringDelete          (athis.cmds[count].opt2);
      athis.cmds[count].opt  = NULL;
      athis.cmds[count].opt2 = NULL;
    }
  CMemoryFreeBlock (athis.cmds);
  athis.cmds       = NULL;
  athis.cmdn       = 0;
  athis.cmd_lego   = NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
char* XzPlotterGetCollectedBinsInformations (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* sinfos = NULL;
  int   pos,ibin;
/*.........................................................................*/
  if(This==NULL)                               return NULL;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return NULL;
  if(athis.collectIdentifier==0)               return NULL;  
  if(ohtype(&(athis.collectIdentifier))==1)
    {
      sinfos      = (char*)CMemoryAllocateBlock( (athis.collectEndPosition - athis.collectBeginPosition + 1) * 64 * sizeof(char));
      if(sinfos==NULL) return NULL;
      pos         = CStringPrintF (sinfos,"");
      for(ibin=athis.collectBeginPosition;ibin<=athis.collectEndPosition;ibin++)
        { float val,x;
          val   = hi (&(athis.collectIdentifier),&ibin);
          hix   (&(athis.collectIdentifier),&ibin,&x);
          pos  += CStringPrintF (sinfos+pos,"%-4d %-10g:%g\n",ibin,x,val);
        }
    }
  return sinfos;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XzPlotterClear (
 Widget This
)
/***************************************************************************/
/*
  Reset commands resource.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;

  CStringDelete        (athis.commands);
  athis.commands       = NULL;

  SendBuild            (This);
}
/***************************************************************************/
void XzPlotterAddCommand (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;
  if(a_string==NULL)                           return;

  if(athis.commands!=NULL) CStringConcatenate (&athis.commands,";");
  CStringConcatenate (&athis.commands,a_string);

  SendBuild          (This);
}
/***************************************************************************/
void XzPlotterSetZones (
 Widget This
,int a_nx
,int a_ny
,int a_first
,char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;

  if(athis.commands!=NULL) CStringConcatenate (&athis.commands,";");
  if(a_opt==NULL)
    string = CStringCreateF (8 + 3 * 64,"zones=%d,%d,%d",a_nx,a_ny,a_first);
  else
    string = CStringCreateF (9 + 3 * 64 + strlen(a_opt),"zones=%d,%d,%d,%s",a_nx,a_ny,a_first,a_opt);
  CStringConcatenate (&athis.commands,string);
  CStringDelete      (string);

  SendBuild          (This);
}
/***************************************************************************/
void XzPlotterDrawHistogram (
 Widget This
,int a_id
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;

  if(athis.commands!=NULL) CStringConcatenate (&athis.commands,";");
  string             = CStringCreateF (5 + 64,"plot=%d",a_id);
  CStringConcatenate (&athis.commands,string);
  CStringDelete      (string);

  SendBuild          (This);
}
/***************************************************************************/
void XzPlotterDrawManhattanPlot (
 Widget This
,int a_id
,double a_theta
,double a_phi
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;

  if(athis.commands!=NULL) CStringConcatenate (&athis.commands,";");
  string             = CStringCreateF (7 + 3 * 64,"lego=%d,%g,%g",a_id,a_theta,a_phi);
  CStringConcatenate (&athis.commands,string);
  CStringDelete      (string);

  SendBuild          (This);
}
/***************************************************************************/
void XzPlotterSetTitle (
 Widget This
,char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;
  if(a_opt==NULL)                              return;

  if(athis.commands!=NULL) CStringConcatenate (&athis.commands,";");
  string             = CStringCreateF (6 + strlen(a_opt),"title=%s",a_opt);
  CStringConcatenate (&athis.commands,string);
  CStringDelete      (string);

  SendBuild     (This);
}
/***************************************************************************/
void XzPlotterSetPageTitle (
 Widget This
,char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;
  if(a_opt==NULL)                              return;

  if(athis.commands!=NULL) CStringConcatenate (&athis.commands,";");
  string             = CStringCreateF (11 + strlen(a_opt),"page_title=%s",a_opt);
  CStringConcatenate (&athis.commands,string);
  CStringDelete      (string);

  SendBuild       (This);
}
/***************************************************************************/
void XzPlotterSetOption (
 Widget This
,char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;
  if(a_opt==NULL)                              return;

  if(athis.commands!=NULL) CStringConcatenate (&athis.commands,";");
  string             = CStringCreateF (7 + strlen(a_opt),"option=%s",a_opt);
  CStringConcatenate (&athis.commands,string);
  CStringDelete      (string);

  SendBuild          (This);
}
/***************************************************************************/
void XzPlotterSetParameter (
 Widget This
,char* a_opt
,double a_val
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;
  if(a_opt==NULL)                              return;

  if(athis.commands!=NULL) CStringConcatenate (&athis.commands,";");
  string             = CStringCreateF (7 + strlen(a_opt) + 64,"param=%s,%g",a_opt,a_val);
  CStringConcatenate (&athis.commands,string);
  CStringDelete      (string);

  SendBuild          (This);
}
/***************************************************************************/
void XzPlotterDrawTuple (
 Widget This
,int a_id
,int a_x
,int a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL)                               return;
  if(!XtIsSubclass(This,xzPlotterWidgetClass)) return;

  if(athis.commands!=NULL) CStringConcatenate (&athis.commands,";");
  string             = CStringCreateF (7 + 3 * 64,"1or2=%d,%d,%d",a_id,a_x,a_y);
  CStringConcatenate (&athis.commands,string);
  CStringDelete      (string);

  SendBuild          (This);
}
