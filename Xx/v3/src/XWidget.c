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

static char what[] = "@(#)Xx v3";

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>


#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <X11/CoreP.h>

#ifdef HAS_XMU
#include <Xmu/Converters.h>
/*#include <Xmu/Drawing.h>*/
/*On vms XmuLocatePixmapFile unknow, use the local one defined in LocPixmap.c.*/
#ifdef __cplusplus
extern "C"{
#endif
Pixmap  XmuLocatePixmapFile (Screen*,char*,unsigned long,unsigned long,unsigned int,char*,int,int*,int*,int*,int*);
#ifdef __cplusplus
}
#endif
#endif /*HAS_XMU*/

#include <CMemory.h>
#include <CFile.h>
#include <CList.h>
#include <CPS.h>
#include <CText.h>
#include <CError.h>
#include <CPrinter.h>
#include <CString.h>
#include <CBatch.h>
#include <OBatch.h>
#include <OShell.h>
#include <OColor.h>

#include <XDisplay.h>
#include <XWidget.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    CountNodes           (Widget,int*);
static void    VisitNodes           (Widget,int*,WidgetList);
static void    CloseWindow          (Widget,XEvent*,char**,Cardinal*);
static void    CopyWindow           (Widget,Widget,GC);
static Boolean DispatchEvents       (Display*,XEvent*,XPoint*,XPoint*);
static char*   GetUpHierarchy       (Widget);
static void    SetResource          ();
static int     GetEventInformation  (char*,int*);
static void    TreatInput           (OBatch);
static void    ExecuteInputCallback (OBatch,int*,XtInputId*);
static void    ReportError          (char*,int);

static void ConvertUnknown                  (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void ConvertXtRIntXtRString          (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void ConvertXtRFloatXtRString        (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void ConvertXtRBooleanXtRString      (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void ConvertXtRShortXtRString        (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void ConvertXtRDimensionXtRString    (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void ConvertXtRPositionXtRString     (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void ConvertXtRUnsignedCharXtRString (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static void ConvertXtRWidgetClassXtRString  (XrmValuePtr, Cardinal*, XrmValuePtr, XrmValuePtr);
static void ConvertXtRStringXtRWidgetClass  (XrmValuePtr, Cardinal*, XrmValuePtr, XrmValuePtr);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
typedef int (*CompareFunction)(const void *, const void *);
#else
typedef int (*CompareFunction)();
#endif

typedef unsigned long Uint;
typedef unsigned long Ulong;
typedef char* OString;

#define NotFound         (-1)
#define MINIMUM(a,b)  ((a)<(b)?a:b)
#define MAXIMUM(a,b)     ((a)>(b)?a:b)
#define StringNotNull(s) ((s)==NULL?"":(s))

static XtResource xt_res_wo [] ={
  {"createCallback"       ,XtCCallback   ,XtRCallback   ,sizeof(XtCallbackList),0,XtRImmediate,(XtPointer)NULL},
  {"targetChangedCallback",XtCCallback   ,XtRCallback   ,sizeof(XtCallbackList),0,XtRImmediate,(XtPointer)NULL},
  {"eventHandler"         ,"EventHandler","EventHandler",sizeof(XtPointer)     ,0,XtRImmediate,(XtPointer)NULL}
};

#define MAX_RES 128
static struct 
{
  int                     add;
  Widget                  widget;
  int                     count;
  Arg                     list[MAX_RES];
  XResourceChangeFunction proc[MAX_RES];
} Class_changes = {0,NULL,0};

static struct 
{
  Widget                        topWidget;
  XClassIsValidFunction         widget_class_exist;
  XClassGetIdentifierFunction   widget_class_fr_name;
  Widget                        to_find_widget;
  Pixel                         selected_pixel;
  Widget                        thisWidget;
  int                           typeData;
  XtPointer                     callData;
  char*                         callValue;
  Widget                        targetWidget;
  Pixel                         prev_border_color;
  Dimension                     prev_border_width;
  Pixel                         target_border_color;
  Dimension                     target_border_width;
  XWidgetVisitFunction          targetChangedProc;
  XWidgetVisitFunction          mapProc;
  XWidgetVisitFunction          unmapProc;
  XWidgetGetLocalTargetFunction localTargetProc;
  XWidgetCallbackEventFunction  cbkEventProc;
  XDisplayGrabFunction          grabDispatchProc;
  int                           grabbingPointer;
  Atom                          wm_delete_window_atom;
  XtTranslations                trans_table;
  XrmResourceList               xrm_res_wo;
  Widget*                       extent;
  Uint                          grabButton;
  char*                         grabCursor;
  char*                         defaultCursor;
  Position                      grabbedX,grabbedY;
  Dimension                     grabbedWidth,grabbedHeight;
  Widget                        grabbedWidget;
#define MAX_STRING 1024
  int                           pos;
/*Not initialised.*/
  char                          string[MAX_STRING];
} Class = {
NULL,             /*topWidget*/
NULL,NULL,        /*widget_class_exist,widget_class_fr_name*/
NULL,             /*to_find_widget*/
0L,               /*selected_pixel*/
NULL,0,NULL,NULL, /*thisWidget,typeData,callData,callValue*/
NULL,0L,0,0L,0,NULL,
NULL,  /*mapProc*/
NULL,  /*unmapProc*/
NULL,  /*localTargetProc*/
NULL,  /*cbkEventProc*/
NULL,  /*grabDispatchProc*/
0,     /*grabbingPointer*/
0L,    /*wm_delete_window_atom*/
NULL,
NULL,  /*xrm_res_wo*/
NULL,  /*extent*/
1,     /*grabButton*/
NULL,  /*grabCursor*/
"question_arrow" , /*defaultCursor*/
0,0,   /*grabbedX,grabbedY*/
0,0,   /*grabbedWidth,grabbedHeight*/
NULL,  /*grabbedWidget*/
0,      /*pos*/
""};
/***************************************************************************/
void XDisplayUpdate (
 Display* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XEvent event;
/*.........................................................................*/
  if(This==NULL) return;
  XSync (This, 0);
  while (XCheckMaskEvent(This,ExposureMask,&event)==True) XtDispatchEvent(&event);
}
/***************************************************************************/
XDisplayGrabFunction XDisplaySetGrabDispatchFunction (
 XDisplayGrabFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XDisplayGrabFunction   old;
/*.........................................................................*/
  old                    = Class.grabDispatchProc;
  Class.grabDispatchProc = a_proc;
  return                 old;
}
/***************************************************************************/
Window XDisplayGrabPointer (
 Display* This
,unsigned int a_button
,Cursor a_cursor
,Position* a_x 
,Position* a_y 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Window       root;
  Time         time = 0;
  XtAppContext context;
  XEvent       xevent;
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0;
  if(a_y!=NULL) *a_y = 0;
  if(This==NULL) return 0L;
  if(a_button>3) return 0L;
  if(a_cursor==0L) return 0L;
  root          = XDefaultRootWindow(This);
  XGrabPointer  (This,root,
                 1,(unsigned int)(ButtonPressMask|ButtonReleaseMask|PointerMotionMask),
                 GrabModeAsync,GrabModeAsync,root,a_cursor,time);
  Class.grabbingPointer = 1;
  context = XtDisplayToApplicationContext(This);
  while(1)
    {
      XtAppNextEvent  (context,&xevent);  
      XtDispatchEvent (&xevent);
      if(((Ulong)xevent.type==ButtonRelease)&&(xevent.xbutton.button==a_button))  break;
      if(Class.grabDispatchProc!=NULL) Class.grabDispatchProc(&xevent);
    }
  XUngrabPointer (This,time);
  Class.grabbingPointer = 0;
  if(a_x!=NULL) *a_x = xevent.xbutton.x;
  if(a_y!=NULL) *a_y = xevent.xbutton.y;
  return  xevent.xany.window;
}
/***************************************************************************/
int XDisplayIsGrabbingPointer (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.grabbingPointer;
}
/***************************************************************************/
void XDisplayDoRubberBandGrab (
 Display* This
,unsigned int a_button
,Cursor   a_cursor
,Position* a_beg_x
,Position* a_beg_y
,Position* a_end_x
,Position* a_end_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Time         time = 0;
  Window       root;
  XPoint       begin,end;
  XtAppContext context;
/*.........................................................................*/
  if(a_beg_x!=NULL) *a_beg_x = 0;
  if(a_beg_y!=NULL) *a_beg_y = 0;
  if(a_end_x!=NULL) *a_end_x = 0;
  if(a_end_y!=NULL) *a_end_y = 0;
  if(This==NULL) return;
  if(a_button>3) return;
  if(a_cursor==0L) return;
  root          = XDefaultRootWindow(This);
  XGrabPointer (This,root,
                a_button,(unsigned int)(ButtonPressMask|ButtonReleaseMask|PointerMotionMask),
                GrabModeAsync,GrabModeAsync,root,a_cursor,time);
  context = XtDisplayToApplicationContext(This);
  while(1)
    { XEvent         xevent;
      XtAppNextEvent (context,&xevent);  /*wait X events*/
      if(DispatchEvents(This,&xevent,&begin,&end)==True) break;
      if(Class.grabDispatchProc!=NULL) Class.grabDispatchProc(&xevent);
    }
  XUngrabPointer (This,time);
  if(a_beg_x!=NULL) *a_beg_x = (int)begin.x;
  if(a_beg_y!=NULL) *a_beg_y = (int)begin.y;
  if(a_end_x!=NULL) *a_end_x = (int)end.x;
  if(a_end_y!=NULL) *a_end_y = (int)end.y;
}
/***************************************************************************/
static Boolean DispatchEvents (
 Display* This
,XEvent* a_event
,XPoint* a_begin
,XPoint* a_end
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    GC      gc;
    int     x,y;
    XPoint  begin,end;
  } pointer = {NULL,0,0,{0,0},{0,0}};
  Window  root;
/*.........................................................................*/
  root = XDefaultRootWindow(This);
  switch(a_event->type)
    {
      case ButtonPress:
            pointer.x              = a_event->xbutton.x_root;
            pointer.y              = a_event->xbutton.y_root;
              { 
                XGCValues arggc;
                arggc.function         = GXxor;
                arggc.foreground       = XWhitePixelOfScreen(XDefaultScreenOfDisplay(This));
                arggc.subwindow_mode   = IncludeInferiors;
                pointer.gc             = (GC) XCreateGC (This,root,GCFunction|GCForeground|GCSubwindowMode,&arggc);
                pointer.begin.x        = pointer.x;
                pointer.begin.y        = pointer.y;
                pointer.end            = pointer.begin;
                XDisplayDrawRectangle          (This,root,pointer.gc,&pointer.begin,&pointer.end);
              }
            break;
      case ButtonRelease:
            XDisplayDrawRectangle     (This,root,pointer.gc,&pointer.begin,&pointer.end);
            XFreeGC           (This,pointer.gc);
            *a_begin          = pointer.begin;
            *a_end            = pointer.end;
            pointer.gc        = NULL;
            return            True;
      case MotionNotify:
            if(pointer.gc==NULL)   return False;
            XDisplayDrawRectangle     (This,root,pointer.gc,&pointer.begin,&pointer.end);
            pointer.end.x    += a_event->xmotion.x_root - pointer.x;
            pointer.end.y    += a_event->xmotion.y_root - pointer.y;
            XDisplayDrawRectangle     (This,root,pointer.gc,&pointer.begin,&pointer.end);
            pointer.x         = a_event->xmotion.x_root;
            pointer.y         = a_event->xmotion.y_root;
            break;
    }
  return False;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XWidgetClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete         (Class.grabCursor);
  Class.grabCursor      = NULL;
  CListDelete           ((CList)Class.extent);
  Class.extent          = NULL;
  CStringDelete         (Class.callValue);
  Class.callValue       = NULL;
  Class.trans_table     = NULL;
  what[0] = '@'; /*c++ no warning.*/
}
/***************************************************************************/
char** XWidgetGetResourceOptions (
 char* a_type 
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_type==NULL) return NULL;

       if(strcmp(a_type,XtRBoolean)==0)
	 { 
	   static char* boolean[2] = {
"True",
"False"
};
	   if(a_number!=NULL) *a_number = 2;
	   return    boolean;
	 } 
  else if(strcmp(a_type,XtROrientation)==0)
    {
      static char* orientation[2] = {
"vertical",
"horizontal"
};
      if(a_number!=NULL) *a_number = 2;
      return    orientation;
    }
  else if(strcmp(a_type,XtRShapeWindow)==0)
    {
      static char* windowShape[2] = {
"rectangular",
"oval"
};
      if(a_number!=NULL) *a_number = 2;
      return    windowShape;
    }
  else if(strcmp(a_type,XtRPixel)==0)
    {
      return OColorGetNames (a_number);
    }
  return NULL;
}
/***************************************************************************/
void XWidgetInitializeClass (
 Display* a_display
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtAppContext context;
/*.........................................................................*/
  if(a_display==NULL) return;

  CBatchSetDisplay (XDisplayString(a_display));

  XDisplayPutFileInResourceDatabase (a_display,"Xtw.xrm");

  context = XtDisplayToApplicationContext(a_display);

#ifdef HAS_XMU
 {static XtConvertArgRec screenConvertArg[] = {              /*From Xaw/Vendor.c.*/
    {XtWidgetBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.screen),sizeof(Screen *)}};
  XtAppAddConverter (context,XtRString, XtRBitmap, XmuCvtStringToBitmap,  screenConvertArg, XtNumber(screenConvertArg));
  }
#endif

/*convertor declaration*/
  XtAppAddConverter (context,XtRString       ,XtRWidgetClass,ConvertXtRStringXtRWidgetClass,NULL,0);
  XtAppAddConverter (context,XtRWidgetClass  ,XtRString,     ConvertXtRWidgetClassXtRString,NULL,0);

/* Do not do conversion with Xt of string to widget.
   "w1" is converted to an address w.
   If we kill widget w, recreate it with name "w1" 
   and then ask a conversion, Xt will return the bad address w
*/
  XtAppAddConverter (context,XtRString       ,XtRWidget,     ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRString       ,XtRWindow,     ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRString       ,"MenuWidget",  ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRString       ,XtRWidgetList, ConvertUnknown,NULL,0);

  XtAppAddConverter (context,XtRWidget       ,XtRString,     ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRWindow       ,XtRString,     ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"MenuWidget"    ,XtRString,     ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRWidgetList   ,XtRString,     ConvertUnknown,NULL,0);

/* On alpha/OSF1 X11R5 or MOTIF1.2 add a PixelToString 
   converter that does not give named color.
   Then convertion is done in OXWidget.c/GetResourcet.
*/

  XtAppAddConverter (context,XtRFloat              ,XtRString,ConvertXtRFloatXtRString,NULL,0);
  XtAppAddConverter (context,XtRBoolean            ,XtRString,ConvertXtRBooleanXtRString,NULL,0);     
  XtAppAddConverter (context,XtRInt                ,XtRString,ConvertXtRIntXtRString,NULL,0);        
  XtAppAddConverter (context,XtRShort              ,XtRString,ConvertXtRShortXtRString,NULL,0);     
  XtAppAddConverter (context,XtRPosition           ,XtRString,ConvertXtRPositionXtRString,NULL,0);
  XtAppAddConverter (context,XtRDimension          ,XtRString,ConvertXtRDimensionXtRString,NULL,0);  
  XtAppAddConverter (context,XtRUnsignedChar       ,XtRString,ConvertXtRUnsignedCharXtRString,NULL,0);
  XtAppAddConverter (context,XtRBool               ,XtRString,ConvertXtRIntXtRString,NULL,0);
  XtAppAddConverter (context,"HorizontalPosition"  ,XtRString,ConvertXtRPositionXtRString,NULL,0);
  XtAppAddConverter (context,"VerticalPosition"    ,XtRString,ConvertXtRPositionXtRString,NULL,0);
  XtAppAddConverter (context,"HorizontalDimension" ,XtRString,ConvertXtRDimensionXtRString,NULL,0);
  XtAppAddConverter (context,"VerticalDimension"   ,XtRString,ConvertXtRDimensionXtRString,NULL,0);
  XtAppAddConverter (context,"ShellHorizPos"       ,XtRString,ConvertXtRPositionXtRString,NULL,0);
  XtAppAddConverter (context,"ShellVertPos"        ,XtRString,ConvertXtRPositionXtRString,NULL,0);
  XtAppAddConverter (context,"ShellVertDim"        ,XtRString,ConvertXtRDimensionXtRString,NULL,0);
  XtAppAddConverter (context,"ShellHorizDim"       ,XtRString,ConvertXtRDimensionXtRString,NULL,0);
  XtAppAddConverter (context,"HorizontalInt"       ,XtRString,ConvertXtRIntXtRString,NULL,0);
  XtAppAddConverter (context,"VerticalInt"         ,XtRString,ConvertXtRIntXtRString,NULL,0);
  XtAppAddConverter (context,"VisibleItemCount"    ,XtRString,ConvertXtRIntXtRString,NULL,0);        
  XtAppAddConverter (context,"Cardinal"            ,XtRString,ConvertXtRIntXtRString,NULL,0);

/*
  XtAppAddConverter (context,XtRPixmap               ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"Bitmap"                ,XtRString,ConvertUnknown,NULL,0);
*/

  XtAppAddConverter (context,XtRFont                 ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRTranslationTable     ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRCursor               ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRAcceleratorTable     ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRCallProc             ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRColor                ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRDisplay              ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRFile                 ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRFontStruct           ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRFunction             ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRGeometry             ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRImmediate            ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRJustify              ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,XtRPointer              ,XtRString,ConvertUnknown,NULL,0);

  XtAppAddConverter (context,"ShellUnitType"         ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"DeleteResponse"        ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"KeyboardFocusPolicy"   ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"Screen"                ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"Colormap"              ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"Visual"                ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"Atom"                  ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"InitialState"          ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"StringArray"           ,XtRString,ConvertUnknown,NULL,0);
  XtAppAddConverter (context,"NavigationType"        ,XtRString,ConvertUnknown,NULL,0);


}
/***************************************************************************/
void XWidgetSetTop (
 Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.topWidget = a_widget;
}
/***************************************************************************/
Widget XWidgetGetTop (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.topWidget;
}
/***************************************************************************/
Widget* XWidgetGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDelete   ((CList)Class.extent);
  Class.extent = XWidgetGetLeaves (XWidgetGetTop()); 
  return       Class.extent;
}
/***************************************************************************/
WidgetList XWidgetGetLeaves (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (WidgetList) CListCreateFromTree (This,(CListCountLeafFunction)CountNodes,(CListVisitLeafFunction)VisitNodes);
}
/***************************************************************************/
static void CountNodes (
 Widget This
,int* a_count
)
/***************************************************************************/
/*
  From routine Recursive in Destroy.c Xt sources 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int    count;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_count==NULL) return;

/*if(This->core.being_destroyed) return;*/

  (*a_count)++;
  /* Recurse down normal children */
  if (XtIsComposite(This)) 
    { CompositePart   *cwp;
      cwp = &(((CompositeWidget)This)->composite);
      for (count=0;count< cwp->num_children;count++) 
        {
	  CountNodes(cwp->children[count],a_count);
	}
    }

  /* Recurse down popup children */
  if (XtIsWidget(This)) 
    {
      for (count=0;count<This->core.num_popups;count++) 
        {
	  CountNodes(This->core.popup_list[count],a_count);
	}
    }
} 
/***************************************************************************/
static void VisitNodes (
 Widget This
,int* a_count
,WidgetList a_list
)
/***************************************************************************/
/*
  From routine Recursive in Destroy.c Xt sources 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int    count;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_count==NULL) return;

/*if(This->core.being_destroyed) return;*/

  a_list[*a_count] = This;
  (*a_count)++;

  /* Recurse down normal children */
  if (XtIsComposite(This)) 
    { CompositePart   *cwp;
      cwp = &(((CompositeWidget)This)->composite);
      for (count=0;count< cwp->num_children;count++) 
        {
	  VisitNodes(cwp->children[count],a_count,a_list);
	}
    }

  /* Recurse down popup children */
  if (XtIsWidget(This)) 
    {
      for (count=0;count<This->core.num_popups;count++) 
        {
	  VisitNodes(This->core.popup_list[count],a_count,a_list);
	}
    }
} 
/***************************************************************************/
Boolean XWidgetIsValid (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return  XWidgetIsInHierarchy (XWidgetGetTop(),This);
}
/***************************************************************************/
Widget XWidgetGetIdentifier  (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"Widget",6)==0) )
    {
      int        status;
      Widget     identifier;
      identifier = (Widget)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return XWidgetFindInHierarchy (XWidgetGetTop(),XrmStringToName(pos+1));
	}
      else
	{
	  if(XWidgetIsValid(identifier)==0)
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
      return  XWidgetFindInHierarchy (XWidgetGetTop(),XrmStringToName(a_name));
    }
}
/***************************************************************************/
/***************************************************************************/
void XWidgetSetName (
 Widget This
,char* a_name
)
/***************************************************************************/
/* From _XtCreate */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  if( (a_name==NULL) || (*a_name=='\0') ) return;
  This->core.xrm_name   = XrmStringToName(a_name==NULL ? "" : a_name );
  if (XtIsWidget(This)) This->core.name = XrmNameToString(This->core.xrm_name);
}
/***************************************************************************/
XrmQuark XWidgetGetQuarkifiedName (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return 0;
  return This->core.xrm_name;
}
/***************************************************************************/
int XWidgetGetNumberOfSubWidgets (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
/*.........................................................................*/
  if(This==NULL) return 0;
  count           = 0;
  CountNodes (This,&count);
  return          count;
}
/***************************************************************************/
Boolean XWidgetIsInHierarchy (
 Widget This
,Widget a_to_find
)
/***************************************************************************/
/*
  From routine Recursive in Destroy.c Xt sources 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int    count;
/*.........................................................................*/
  if(This==NULL)      return False;
  if(a_to_find==NULL) return False;

  if(This->core.being_destroyed==True) return False;

  if(This==a_to_find) return True;

  /* Recurse down normal children */
  if (XtIsComposite(This)) 
    { CompositePart   *cwp;
      cwp = &(((CompositeWidget)This)->composite);
      for (count=0;count< cwp->num_children;count++) 
        {
 	  if(XWidgetIsInHierarchy(cwp->children[count],a_to_find)==True) 
	    return True;
	}
    }

  /* Recurse down popup children */
  if (XtIsWidget(This)) 
    {
      for (count=0;count<This->core.num_popups;count++) 
        {
	  if(XWidgetIsInHierarchy(This->core.popup_list[count],a_to_find)==True) 
	    return True;
	}
    }

  return False;
} 
/***************************************************************************/
Widget XWidgetFindInHierarchy (
 Widget This
,XrmQuark a_name
)
/***************************************************************************/
/*
  From routine Recursive in Destroy.c Xt sources 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int    count;
/*.........................................................................*/
  if(This==NULL)             return NULL;
  if(a_name==NULLQUARK) return NULL;

  if(This->core.being_destroyed==True)  return NULL;

  if(a_name==This->core.xrm_name) return This;

  /* Recurse down normal children */
  if (XtIsComposite(This)) 
    { CompositePart   *cwp;
      cwp = &(((CompositeWidget)This)->composite);
      for (count=0;count< cwp->num_children;count++) 
        {
	  Widget   widget;
	  widget = XWidgetFindInHierarchy(cwp->children[count],a_name);
	  if(widget!=NULL) return widget;
	}
    }

  /* Recurse down popup children */
  if (XtIsWidget(This)) 
    {
      for (count=0;count<This->core.num_popups;count++) 
        {
	  Widget   widget;
	  widget   = XWidgetFindInHierarchy(This->core.popup_list[count],a_name);
	  if(widget!=NULL) return widget;
	}
    }

  return NULL;
} 
/***************************************************************************/
void XWidgetVisitHierarchyForward (
 Widget This
,XWidgetVisitFunction a_proc
)
/***************************************************************************/
/*
  Exec forward 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_proc==NULL) return;

  a_proc(This);

  /* Recurse down normal children */
  if (XtIsComposite(This)) 
    { CompositePart* cwp;
      cwp            = &(((CompositeWidget)This)->composite);
      for (count=0;count<cwp->num_children;count++) 
        {
	  XWidgetVisitHierarchyForward(cwp->children[count],a_proc);
	}
    }

  /* Recurse down popup children */
  if (XtIsWidget(This)) 
    {
      for (count=0;count<This->core.num_popups;count++) 
        {
	  XWidgetVisitHierarchyForward(This->core.popup_list[count],a_proc);
	}
    }

} 
/***************************************************************************/
Cardinal XWidgetGetRecordSize (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if (This==NULL)  return 0;
  return      This->core.widget_class->core_class.widget_size;
}
/***************************************************************************/
void XWidgetRealize (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
/* When realizing a widget all sub children (not popup) are realized */
  if(XtIsRealized(This)) return;
  XtRealizeWidget (This);
/* for shell look if we have to map (hole in Xt) */
  if(XtIsShell(This)) 
    {
      Boolean     value = False;
      Arg         args[1];
      XtSetArg    (args[0],XtNmappedWhenManaged,&value);
      XtGetValues (This,args,1);
      if(value==True)   XWidgetMap (This);
    }
}
/***************************************************************************/
void XWidgetMapChildren (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList items,itema;
/*.........................................................................*/
  if(This==NULL) return;
  items = XWidgetGetChildren (This,NULL);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) XWidgetMap (*itema);
  CListDelete ((CList)items);
}
/***************************************************************************/
char* XWidgetGetClassName (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if (This==NULL)  return NULL;
  return      This->core.widget_class->core_class.class_name;
}
/***************************************************************************/
Boolean XWidgetIsConstraint (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if (This==NULL)        return False;
  if (!XtIsWidget(This)) return False; 
  return ( !XtIsShell(This) && XtIsConstraint(XtParent(This)) ) ? True : False;
}
/***************************************************************************/
int XWidgetGetResources (
 Widget This
,char* a_name
,XtResourceList a_resource
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetClass     widget_class;
  unsigned int    count;
  XtResourceList* list;
  int             ipos;
  XrmQuark        quark;
/*.........................................................................*/
  if (a_resource==NULL)  return NotFound;
  a_resource->resource_name   = NULL;
  a_resource->resource_class  = NULL;
  a_resource->resource_type   = NULL;
  a_resource->resource_size   = 0;
  a_resource->default_type    = NULL;
  a_resource->default_addr    = NULL;
  if (This==NULL)        return NotFound;
  if (a_name==NULL)      return NotFound;
  
  widget_class  =  This->core.widget_class;
  if(widget_class->core_class.class_inited==(XtEnum)0) 
    {
      ReportError   ("XWidgetGetResources",CErrorFailure);
      return  NotFound;
    }

  quark = XrmStringToQuark(a_name); 
  if(quark==NULLQUARK) 
    {
      ReportError ("XWidgetGetResources",CErrorConversion);
      return NotFound;
    }

  if(Class.xrm_res_wo==NULL)
    {
      Class.xrm_res_wo = (XrmResourceList)XtMalloc(XtNumber(xt_res_wo)*sizeof(XrmResource));
      for (count=0;count<XtNumber(xt_res_wo);count++) 
	{
	  Class.xrm_res_wo[count].xrm_name         = XrmStringToQuark (xt_res_wo[count].resource_name);
	  Class.xrm_res_wo[count].xrm_class        = XrmStringToQuark (xt_res_wo[count].resource_class);
	  Class.xrm_res_wo[count].xrm_type         = XrmStringToQuark (xt_res_wo[count].resource_type);
	  Class.xrm_res_wo[count].xrm_size         = xt_res_wo[count].resource_size;
	  Class.xrm_res_wo[count].xrm_offset       = xt_res_wo[count].resource_offset;
	  Class.xrm_res_wo[count].xrm_default_type = XrmStringToQuark (xt_res_wo[count].default_type);
	  Class.xrm_res_wo[count].xrm_default_addr = NULL;
	}
    }

  ipos = 0;
  for (count=0;count<XtNumber(xt_res_wo);count++) 
    {
      if(quark==Class.xrm_res_wo[count].xrm_name)
        {
          *a_resource = xt_res_wo[count];
          return ipos;
        }
      ipos ++;
    }

  list = (XtResourceList *) widget_class->core_class.resources;
  for (count=0;count<widget_class->core_class.num_resources;count++) 
    {
        if (list[count] != NULL) 
	  {
	    if(quark == (XrmQuark)(list[count]->resource_name))
	      {
                a_resource->resource_name = (String) XrmQuarkToString((XrmQuark) list[count]->resource_name);
                a_resource->resource_type = (String) XrmQuarkToString((XrmQuark) list[count]->resource_type);
                a_resource->resource_size = list[count]->resource_size;
                return ipos;
            }
	  }
        ipos ++;
    }
 
  if (XWidgetIsConstraint(This)==True) 
    {

      ConstraintWidgetClass cons_class = (ConstraintWidgetClass)XtClass(XtParent(This));

      list          = (XtResourceList *) cons_class->constraint_class.resources;
      for (count=0;count<cons_class->constraint_class.num_resources;count++) 
	{
	  if (list[count] != NULL) 
	    {
	      if(quark == (XrmQuark)(list[count]->resource_name))
		{
		  a_resource->resource_name = (String) XrmQuarkToString((XrmQuark) list[count]->resource_name);
		  a_resource->resource_type = (String) XrmQuarkToString((XrmQuark) list[count]->resource_type);
		  a_resource->resource_size = list[count]->resource_size;
		  return ipos;
		}
	    }
	  ipos ++;
	}
      
    } /* end hasConstraints */

  return NotFound; /* not found */
}
/***************************************************************************/
void XWidgetGetIthResource (
 Widget This
,int a_count
,XtResourceList a_resource
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetClass     widget_class;
  XtResourceList* list;
/*.........................................................................*/
  if (a_resource==NULL)  return;
  a_resource->resource_name   = NULL;
  a_resource->resource_class  = NULL;
  a_resource->resource_type   = NULL;
  a_resource->resource_size   = 0;
  a_resource->default_type    = NULL;
  a_resource->default_addr    = NULL;
  if (This==NULL)        return;
  
  widget_class  =  This->core.widget_class;
  if(widget_class->core_class.class_inited==(XtEnum)0) 
    {
      ReportError ("XWidgetGetIthResource",CErrorFailure);
      return;
    }

  if( (a_count>=0) && (a_count<(int)XtNumber(xt_res_wo)) ) 
    {
      *a_resource = xt_res_wo[a_count];
      return;
    }
  a_count -= XtNumber(xt_res_wo);

  if ( (a_count>=0)  && (a_count<(int)widget_class->core_class.num_resources) )
    {
        list = (XtResourceList *) widget_class->core_class.resources;
        if (list[a_count] != NULL) 
            {
                a_resource->resource_name = (String) XrmQuarkToString((XrmQuark) list[a_count]->resource_name);
                a_resource->resource_type = (String) XrmQuarkToString((XrmQuark) list[a_count]->resource_type);
                a_resource->resource_size = list[a_count]->resource_size;
                return;
            }
    }
 
  if (XWidgetIsConstraint(This)==True) 
    {

      int                   count;
      ConstraintWidgetClass cons_class = (ConstraintWidgetClass)XtClass(XtParent(This));

      count = a_count - widget_class->core_class.num_resources;

      if ( (count>=0) && (count<(int)cons_class->constraint_class.num_resources) )
	{  
	  list  = (XtResourceList *) cons_class->constraint_class.resources;
	  if (list[count] != NULL) 
            {
	      a_resource->resource_name = (String) XrmQuarkToString((XrmQuark) list[count]->resource_name);
	      a_resource->resource_type = (String) XrmQuarkToString((XrmQuark) list[count]->resource_type);
	      a_resource->resource_size = list[count]->resource_size;
	      return;
            }
	}
      
    } /* end hasConstraints */
}
/***************************************************************************/
int XWidgetGetNumberOfResources (
 Widget This
)
/***************************************************************************/
/*
   XtNumber(xt_res_wo)
 + all inherited resources
 + all constraint resources
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetClass    widget_class;
  int            nres;
/*.........................................................................*/
  if(This==NULL)        return 0;
  
  widget_class  =  This->core.widget_class;
  if(widget_class->core_class.class_inited==(XtEnum)0) 
    {  
      ReportError ("XWidgetGetNumberOfResources",CErrorFailure);
      return 0;
    }

  nres  = XtNumber(xt_res_wo);
  
  nres +=  widget_class->core_class.num_resources;
 
  if (XWidgetIsConstraint(This)==True) 
    {

      ConstraintWidgetClass cons_class = (ConstraintWidgetClass)XtClass(XtParent(This));
      
      nres += cons_class->constraint_class.num_resources;
      
    } /* end hasConstraints */
  
  return nres;
}
/***************************************************************************/
Boolean XWidgetIsVisible (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWindowAttributes watbs;
/*.........................................................................*/
  if(This==NULL) return False;
/*
  return This->core.visible;
*/
  if( !XtIsWidget(This) || !XtIsRealized(This) ) return False;
  XGetWindowAttributes (XtDisplay(This),XtWindow(This),&watbs);
  if(watbs.map_state!=IsViewable) return False;
  return True;
}
/***************************************************************************/
void XWidgetInstallColormap (
 Widget This 
)
/***************************************************************************/
/* 
  From Mesa/widgets/GLwDrawingArea.c/post_colormap routine.
  Could use also XtSetWMColormapWindows.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Display*          display;
  XWindowAttributes watbs;
  Widget            shell;
  Window*           ws = NULL;
  int               wn = 0,found,count;
  Window            wshell,wthis;
  Colormap          cmapthis;
/*.........................................................................*/
  if(This==NULL) return;
  if( !XtIsWidget(This) || !XtIsRealized(This) ) return;
  shell = XWidgetGetShell (This);
  if(shell==NULL) return;
  display = XtDisplay (This);
  wthis   = XtWindow  (This);
  wshell  = XtWindow  (shell);
  XGetWMColormapWindows (display,wshell, &ws, &wn);
/* Check if colormap of this is a colormap of a Window in list.*/
  XGetWindowAttributes  (display,wthis,&watbs);
  cmapthis              = watbs.colormap;
  found                 = -1;
  for(count=0;count<wn;count++)
    {
      Colormap             cmap;
      XGetWindowAttributes (display,ws[count],&watbs);
      cmap                 = watbs.colormap;
      if(cmap==cmapthis)
	{
	  XFree (ws);
	  return;  /*done*/
	}
      if(ws[count]==wshell)
	{
	  found = count;
	}
    }
  /*Have to add window of this in list.*/
  if(wn==0)
    {
      if(ws!=NULL) XFree(ws);
      ws = (Window*)malloc ( 2 * sizeof(Window));
    }
  else
    {
      ws = (Window*)realloc (ws,(wn + 2) * sizeof(Window));  
    }
  if(ws==NULL) return;
  if(found==-1) /*Window of shell not in list.*/
    {
      ws[wn] = wthis; wn++;
      ws[wn] = wshell;wn++;
    }
  else
    {
      ws[found] = wthis;
      ws[wn]    = wshell; wn++;  /*Shell must be last.*/
    }
  if (XSetWMColormapWindows(display,wshell, ws, wn)==0)
    {
      CWarnF ("XWidgetInstallColormap: can't install colormap of %s in %s.\n",XtName(This),XtName(shell));
    }
  XFree (ws);
}
/***************************************************************************/
void XWidgetUninstallColormap (
 Widget This 
)
/***************************************************************************/
/* 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
            int count;
  Widget            shell;
  Display*          display;
  Window            wthis,wshell;
  Window*           ws  = NULL;
  int               wn  = 0;
  Window*           nws = NULL;
  int               nwn = 0;
/*.........................................................................*/
  if(This==NULL) return;
  if( !XtIsWidget(This) || !XtIsRealized(This) ) return;
  shell = XWidgetGetShell (This);
  if(shell==NULL) return;
  display               = XtDisplay (This);
  wthis                 = XtWindow  (This);
  wshell                = XtWindow  (shell);
  XGetWMColormapWindows (display,wshell, &ws, &wn);
  if( (wn==0) || (ws==NULL) ) return;
  nws                   = (Window*)malloc ( wn  * sizeof(Window));
  if(nws==NULL) 
    {
      XFree (ws);
      return;
    }
  nwn = 0;
  for(count=0;count<wn;count++)
    {
      if(ws[count]!=wthis)
	{
	  nws[nwn] = ws[count];
	  nwn++;
	}
    }
  if(wn!=nwn)
    {
      if (XSetWMColormapWindows(display,wshell, nws, nwn)==0)
	{
	  CWarnF ("XWidgetUninstallColormap: can't install colormap of %s in %s.\n",XtName(This),XtName(shell));
	}
    }
  XFree (ws);
  XFree (nws);
}
/***************************************************************************/
void XWidgetGetPosition (
 Widget This 
,Position* a_x
,Position* a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL)  *a_x        = 0;
  if(a_y!=NULL)  *a_y        = 0;
  if(This==NULL) return;
  if(a_x!=NULL)  *a_x        = This->core.x;
  if(a_y!=NULL)  *a_y        = This->core.y;
}
/***************************************************************************/
void XWidgetGetDimension (
 Widget This 
,Dimension* a_width  
,Dimension* a_height
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_width!=NULL)  *a_width    = 0;
  if(a_height!=NULL) *a_height   = 0;
  if(This==NULL)     return;
  if(a_width!=NULL)  *a_width    = This->core.width;
  if(a_height!=NULL) *a_height   = This->core.height;
}
/***************************************************************************/
void XWidgetSetSensitive (
 Widget This
,Boolean a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL) return;
  XtSetArg     (args[0],XtNsensitive,a_value); 
  XtSetValues  (This,args,1);
}
/***************************************************************************/
void XWidgetSetPosition (
 Widget This 
,Position a_x 
,Position a_y 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[2];
/*.........................................................................*/
  if(This==NULL) return;
  XtSetArg     (args[0],XtNx,a_x); 
  XtSetArg     (args[1],XtNy,a_y);
  XtSetValues  (This,args,2);
}
/***************************************************************************/
void XWidgetSetDimension (
 Widget This  
,Dimension a_width  
,Dimension a_height 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)      return;
  if((a_width<=0)||(a_height<=0)) return;
  XtResizeWidget (This,a_width,a_height,This->core.border_width);
}
/***************************************************************************/
void XWidgetSetPositionAndDimension (
 Widget This  
,Position a_x 
,Position a_y 
,Dimension a_width 
,Dimension a_height 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XtConfigureWidget (This,a_x,a_y,a_width,a_height,This->core.border_width);
}
/***************************************************************************/
void XWidgetGetRootPosition (
 Widget This 
,Position* a_x 
,Position* a_y 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Position          rx,ry;
/*.........................................................................*/
  if(This==NULL)         return;
  if(a_x==NULL)          return;
  if(a_y==NULL)          return;
  XtTranslateCoords (This,*a_x,*a_y,&rx,&ry);
  *a_x              = rx;
  *a_y              = ry;
}
/***************************************************************************/
int XWidgetSendMessage (
 Widget This
,long a_1
,long a_2
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XClientMessageEvent event;
/*.........................................................................*/
  if (This==NULL)          return 0;
  if (!XtIsRealized(This)) return 0;
  if (!XtIsWidget(This))   return 0; 
  event.type           = ClientMessage;
  event.display        = XtDisplay (This);
  event.window         = XtWindow  (This);
  event.message_type   = XA_INTEGER;
  event.format         = 8;    /* put 8 because bug with 32 on VMCMS */
  event.data.l[0]      = a_1;
  event.data.l[1]      = a_2;
  event.data.l[2]      = 0L;
  event.data.l[3]      = 0L;
  event.data.l[4]      = 0L;
  if(XSendEvent        (event.display,event.window,False,0L,(XEvent*)&event)==0) return 0;
  XFlush               (event.display);
  return               1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XWidgetSetResource (
 Widget This 
,char* a_name   
,XtArgVal a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return;
  XtSetArg     (args[0],a_name,a_value);
  XtSetValues  (This,args,1);
}
/***************************************************************************/
void XWidgetSetIntegerResource (
 Widget This 
,char* a_name   
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return;
  XtSetArg     (args[0],a_name,a_value);
  XtSetValues  (This,args,1);
}
/***************************************************************************/
void XWidgetSetPixelResource (
 Widget This 
,char* a_name   
,Pixel a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return;
  XtSetArg     (args[0],a_name,a_value);
  XtSetValues  (This,args,1);
}
/***************************************************************************/
void XWidgetSetWidgetResource (
 Widget This 
,char* a_name   
,Widget a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return;
  XtSetArg     (args[0],a_name,a_value);
  XtSetValues  (This,args,1);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
XtArgVal XWidgetGetResource (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtArgVal     value = 0L;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return 0L;
  XtSetArg     (args[0],a_name,&value);
  XtGetValues  (This,args,1);
  return       value;
}
/***************************************************************************/
XtPointer XWidgetGetAddressResource (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtPointer    value = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return NULL;
  XtSetArg     (args[0],a_name,&value);
  XtGetValues  (This,args,1);
  return       value;
}
/***************************************************************************/
Pixel XWidgetGetPixelResource (
 Widget This 
,char* a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Pixel        pixel = 0L;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL) return 0L;
  XtSetArg     (args[0],a_name,&pixel);
  XtGetValues  (This,args,1);
  return       pixel;
}
/***************************************************************************/
Boolean XWidgetGetBooleanResource (
 Widget This 
,char* a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Boolean      bval = False;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return False;
  XtSetArg     (args[0],a_name,&bval);
  XtGetValues  (This,args,1);
  return       bval;
}
/***************************************************************************/
int XWidgetGetIntegerResource (
 Widget This 
,char* a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int          value = 0;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return 0;
  XtSetArg     (args[0],a_name,&value);
  XtGetValues  (This,args,1);
  return       value;
}
/***************************************************************************/
Widget XWidgetGetWidgetResource (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget       widget = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return NULL;
  XtSetArg     (args[0],a_name,&widget);
  XtGetValues  (This,args,1);
  return       (XWidgetIsValid(widget)==1 ? widget : (Widget)NULL);
}
/***************************************************************************/
int XWidgetGetAndConvertStringResourceToInteger (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*        string = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return 0;
  XtSetArg     (args[0],a_name,&string);
  XtGetValues  (This,args,1);
  if(string==NULL)  return 0; /* a_name not a resource of widget */
  return       (int)CStringConvertToLong(string,NULL);
}
/***************************************************************************/
double XWidgetGetAndConvertStringResourceToDouble (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*        string = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return 0.;
  XtSetArg     (args[0],a_name,&string);
  XtGetValues  (This,args,1);
  if(string==NULL)  return 0.; /* a_name not a resource of widget */
  return       CStringConvertToDouble (string,NULL);
}
/***************************************************************************/
Boolean XWidgetGetAndConvertStringResourceToBoolean (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*        string = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return False;
  XtSetArg     (args[0],a_name,&string);
  XtGetValues  (This,args,1);
  if( (string!=NULL)  && 
      ( (strcmp(string,"true")==0)  || (strcmp(string,"True")==0) )
     )   return 1;
  return       False;
}
/***************************************************************************/
Widget XWidgetGetAndConvertStringResourceToWidget (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*        string = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return NULL;
  XtSetArg     (args[0],a_name,&string);
  XtGetValues  (This,args,1);
  return       XWidgetGetIdentifier(string);
}
/***************************************************************************/
WidgetList XWidgetGetWidgetListResource (
 Widget This 
,char* a_name   
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList   list = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(a_number!=NULL) *a_number    = 0;
  if(This==NULL)    return NULL;
  XtSetArg     (args[0],a_name,&list);
  XtGetValues  (This,args,1);
  if(a_number!=NULL) *a_number    = CListGetSize ((CList)list);
  return       list;
}
/***************************************************************************/
Widget XWidgetGetFirstEntryOfWidgetListResource (
 Widget This 
,char* a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList   list = NULL;
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)    return NULL;
  XtSetArg     (args[0],a_name,&list);
  XtGetValues  (This,args,1);
  if( (list!=NULL) && (list[0]!=NULL) ) return list[0];
  return       NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
Boolean XWidgetIsEntryOfWidgetListResource (
 Widget This
,char* a_name
,Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg        args[1];
  WidgetList list = NULL;
  int        pos;
/*.........................................................................*/
  if(This==NULL)   return False;
  if(a_name==NULL) return False;
  XtSetArg    (args[0],a_name,&list);
  XtGetValues (This,args,1);
  pos         = CListGetEntryIndex((CList)list,a_widget,NULL);
  return      (pos==NotFound ? False : True);
}
/***************************************************************************/
void XWidgetAddEntryInWidgetListResource (
 Widget This
,char* a_name
,Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg        args[1];
  WidgetList list   = NULL;
  WidgetList lset;
/*.........................................................................*/
  if(This==NULL)        return;
  if(a_name==NULL)      return;
  XtSetArg              (args[0],a_name,&list);
  XtGetValues           (This,args,1);
  lset                  = (WidgetList)CListDuplicate((CList)list);
  CListAddEntry          ((CList*)&lset,a_widget);
  XtSetArg              (args[0],a_name,lset);
  XtSetValues           (This,args,1);
  CListDelete            ((CList)lset);
}
/***************************************************************************/
void XWidgetRemoveEntryFromWidgetListResource (
 Widget This
,char* a_name
,Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg        args[1];
  WidgetList list   = NULL;
  WidgetList lset;
/*.........................................................................*/
  if(XWidgetIsEntryOfWidgetListResource (This,a_name,a_widget)==False) return;
  if(This==NULL)        return;
  if(a_name==NULL)      return;
  XtSetArg              (args[0],a_name,&list);
  XtGetValues           (This,args,1);
  lset                  = (WidgetList)CListDuplicate((CList)list);
  CListRemoveEntry          ((CList)lset,a_widget);
  XtSetArg              (args[0],a_name,lset);
  XtSetValues           (This,args,1);
  CListDelete            ((CList)lset);
}
/***************************************************************************/
Cursor XWidgetConvertStringToCursor (
 Widget This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XrmValue  from,to;
/*.........................................................................*/
  if(This==NULL)                 return 0L;
  if( (a_name==NULL) || (*a_name=='\0') ) return 0L;
  from.size    = strlen(a_name)+1;
  from.addr    = (XPointer)a_name;
  XtConvert    (This,XtRString,&from,XtRCursor,&to);
  if(to.addr==NULL) return 0L;
  return       *((Cursor*)to.addr);
}
/***************************************************************************/
void XWidgetSetCursor (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( !XtIsWidget(This) || !XtIsRealized(This) ) return;
  if(a_name==NULL)
    {
      XUndefineCursor (XtDisplay(This),XtWindow(This));
    }
  else
    {
      Cursor     cursor;
      cursor     = XWidgetConvertStringToCursor(This,a_name);
      if(cursor!=0L) 
	XDefineCursor (XtDisplay(This),XtWindow(This),cursor);
    }
  XSync           (XtDisplay(This),False);
}
/***************************************************************************/
WidgetList XWidgetGetChildren (
 Widget This
,int* a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList     list;
  unsigned int   count;
  CompositePart* cwp;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)             return NULL;
  if(!XtIsComposite(This))   return NULL;
  cwp                        = &(((CompositeWidget)This)->composite);
  list                       = (WidgetList)CListCreate(cwp->num_children);
  if(list==NULL)                  return NULL;
  for (count=0;count<cwp->num_children;count++) list[count] = cwp->children[count];
  if(a_number!=NULL)               *a_number = cwp->num_children;
  return                     list;
}
/***************************************************************************/
WidgetList XWidgetGetPopupChildren (
 Widget This 
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList     list;
  unsigned int   count;
/*.........................................................................*/
  if(a_number!=NULL)     *a_number = 0;
  if(This==NULL)         return NULL;
  if(!XtIsWidget(This))  return NULL;
  list                   = (WidgetList)CListCreate(This->core.num_popups);
  if(list==NULL)              return NULL;
  for (count=0;count<This->core.num_popups;count++) list[count] = This->core.popup_list[count];
  if(a_number!=NULL)           *a_number = This->core.num_popups;
  return                 list;
}
/***************************************************************************/
WidgetList XWidgetGetAllChildren (
 Widget This 
,int* a_number
)
/***************************************************************************/
/*
  free returned list after usage
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count,icount;
  WidgetList list;
  int number;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)    return NULL;
  number       = 0;
  if (XtIsComposite(This)) number += ((CompositeWidget)This)->composite.num_children;
  if (XtIsWidget(This))    number += This->core.num_popups;
  list      = (WidgetList)CListCreate(number);
  if(list==NULL) return NULL;
  icount    = 0;

  if (XtIsComposite(This)) 
    { CompositePart   *cwp;
      cwp = &(((CompositeWidget)This)->composite);
      for (count=0;count<cwp->num_children;count++) 
        {
	  list[icount] = cwp->children[count];
          icount++;
	}
    }

  /* Recurse down popup children */
  if (XtIsWidget(This)) 
    {
      for (count=0;count<This->core.num_popups;count++) 
        {
	  list[icount] = This->core.popup_list[count];
          icount++;
	}
    }
  if(a_number!=NULL) *a_number = number;
  return       list;
}
/***************************************************************************/
Widget XWidgetGetFirstChild (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList    list;
  Widget        child = NULL;
/*.........................................................................*/
  if(This==NULL)  return NULL;
  list       = XWidgetGetChildren (This,NULL);
  if( (list!=NULL) && (list[0]!=NULL) ) child  = list[0];
  CListDelete ((CList)list);
  return     child;
}
/***************************************************************************/
char* XWidgetBuildName (
 Widget This
,char* a_prefix
)
/***************************************************************************/
{
  char* string;
  int   maximum;
/*.........................................................................*/
  if(a_prefix==NULL) return NULL;
  string  = (char *) CMemoryAllocateBlock ((strlen(a_prefix)+64)*sizeof(char));  
  if(string==NULL) return NULL;
  maximum = XWidgetGetMaximumNameIndex(This,a_prefix);
  maximum = maximum + 1;
  sprintf (string,"%s%d",a_prefix,maximum);
  return  string;
}
/***************************************************************************/
int XWidgetGetMaximumNameIndex (
 Widget This
,char* a_prefix 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList  items,itema;
  int         length,maxval,val;
/*.........................................................................*/
  if(a_prefix==NULL) return 0;
  length   = strlen(a_prefix);
  maxval   = 0;
  items    = XWidgetGetLeaves(This);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    {  
      char*  name;
      name   = XtName(*itema);
      if ( (name!=NULL) && (strncmp(name,a_prefix,length)==0)  )
	{ 
	  char*  string;
	  int    status;
	  string = name + length;
	  val    = (int) CStringConvertToLong (string,&status);
	  if (status!=0)
	    {
	      maxval = MAXIMUM (maxval,val);     
	    }
	}
    }
  CListDelete ((CList)items);
  return     maxval;
}
/***************************************************************************/
XWidgetVisitFunction XWidgetSetMapFunction (
 XWidgetVisitFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWidgetVisitFunction old;
/*.........................................................................*/
  old           = Class.mapProc;
  Class.mapProc = a_proc;
  return        old;
}
/***************************************************************************/
void XWidgetMap (
 Widget This 
)
/***************************************************************************/
/* to protect against Gadget mapping                                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(Class.mapProc!=NULL)  
    {
      Class.mapProc(This);
    }
  else 
    {
      XWidgetMapWindow (This);
    }
}
/***************************************************************************/
void XWidgetMapWindow (
 Widget This 
)
/***************************************************************************/
/* to protect against Gadget mapping                                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( !XtIsWidget(This) || !XtIsRealized(This) ) return;
  XMapWindow (XtDisplay(This),XtWindow(This));
}
/***************************************************************************/
void XWidgetPopup (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XtPopup (This,XtGrabNonexclusive);
}
/***************************************************************************/
void XWidgetPopdown (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XtPopdown (This);
}
/***************************************************************************/
void XWidgetManage (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XtManageChild (This);
}
/***************************************************************************/
void XWidgetUnmanage (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XtUnmanageChild (This);
}
/***************************************************************************/
XWidgetVisitFunction XWidgetSetUnmapFunction (
 XWidgetVisitFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWidgetVisitFunction old;
/*.........................................................................*/
  old             = Class.unmapProc;
  Class.unmapProc = a_proc;
  return          old;
}
/***************************************************************************/
void XWidgetUnmap (
 Widget This 
)
/***************************************************************************/
/* to protect against Gadget mapping                                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(Class.unmapProc!=NULL)  
    {
      Class.unmapProc(This);
    }
  else 
    {
      XWidgetUnmapWindow (This);
    }
}
/***************************************************************************/
void XWidgetUnmapWindow (
 Widget This 
)
/***************************************************************************/
/* to protect against Gadget unmapping                                     */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( !XtIsWidget(This) || !XtIsRealized(This) ) return;
  XUnmapWindow (XtDisplay(This),XtWindow(This));
}
/***************************************************************************/
void XWidgetRaise (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( !XtIsWidget(This) || !XtIsRealized(This) ) return;
  XDisplaySetWindowToNormalState (XtDisplay(This),XtWindow(This)); /*XWidgetUniconify*/
  XRaiseWindow                   (XtDisplay(This),XtWindow(This));
}
/***************************************************************************/
void XWidgetLower (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( !XtIsWidget(This) || !XtIsRealized(This) ) return;
  XLowerWindow (XtDisplay(This),XtWindow(This));
}
/***************************************************************************/
void XWidgetMove (
 Widget This 
,Position a_x 
,Position a_y 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XtMoveWidget (This,a_x,a_y);
}
/***************************************************************************/
void XWidgetCallCallbacks (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_name==NULL) return;
  if(XtHasCallbacks(This,a_name)!=XtCallbackHasSome) return;
  XtCallCallbacks (This,a_name,NULL);
}
/***************************************************************************/
Pixel XWidgetConvertStringToPixel (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XrmValue  from,to;
/*.........................................................................*/
  if(This==NULL)                     return 0L;
  if( (a_string==NULL) || (*a_string=='\0')) return 0L;
  if( (strcmp(a_string,"None")==0) || 
      (strcmp(a_string,"none")==0) ) 
    return XWidgetGetPixelResource(This,XtNbackground);
  from.size    = strlen(a_string)+1;
  from.addr    = (XPointer)a_string;
  XtConvert    (This,XtRString,&from,XtRPixel,&to);
  if(to.addr==NULL) return 0L;
  return       (*((Pixel*)to.addr));
}
/***************************************************************************/
Font XWidgetConvertStringToFont (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XrmValue  from,to;
/*.........................................................................*/
  if(This==NULL)                     return 0L;
  if( (a_string==NULL) || (*a_string=='\0')) return 0L;
  from.size    = strlen(a_string)+1;
  from.addr    = (XPointer)a_string;
  XtConvert    (This,XtRString,&from,XtRFont,&to);
  if(to.addr==NULL) return 0L;
  return       (*((Font*)to.addr));
}
/***************************************************************************/
XFontStruct* XWidgetConvertStringToFontStruct (
 Widget This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XrmValue  from,to;
/*.........................................................................*/
  if(This==NULL)                     return NULL;
  if( (a_string==NULL) || (*a_string=='\0')) return NULL;
  from.size    = strlen(a_string)+1;
  from.addr    = (XPointer)a_string;
  XtConvert    (This,XtRString,&from,XtRFontStruct,&to);
  if(to.addr==NULL) return NULL;
  return       (*((XFontStruct**)to.addr));
}
/***************************************************************************/
void XWidgetShowArming (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)             return;
  Class.selected_pixel = XWidgetGetPixelResource (This,XtNbackground);
  if(This->core.depth==1)
    XtSetArg              (args[0],XtNbackground,XWidgetConvertStringToPixel (This,"black"));
  else
    XtSetArg              (args[0],XtNbackground,XWidgetConvertStringToPixel (This,"Red"));
  XtSetValues           (This,args,1);
}
/***************************************************************************/
void XWidgetShowDisarming (
  Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg          args[1];
/*.........................................................................*/
  if(This==NULL)             return;
  XtSetArg              (args[0],XtNbackground,Class.selected_pixel);
  XtSetValues           (This,args,1);
  Class.selected_pixel = 0L;
}
/***************************************************************************/
Widget XWidgetGetShell (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(This==NULL) return NULL;
  widget = This;
  while(1)
    {
      if(widget==NULL) return NULL;
      if(XtIsShell(widget)) return widget;
      widget = XtParent(widget);
    }
}
/***************************************************************************/
Pixmap XWidgetGetPixmap (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(XWidgetIsVisible(This)==False) return 0L;
  return XDisplayGetDrawablePixmap (XtDisplay(This),XtWindow(This));
}
/***************************************************************************/
Pixmap XWidgetLoadPixmapFile (
 Widget This 
,char* a_fname
)
/***************************************************************************/
/* Avoid Motif routine:                                                    */
/*        pixmap  = XmGetPixmap(screen,fname,fore,back);                   */
/*        if(pixmap==XmUNSPECIFIED_PIXMAP) pixmap = 0L;                    */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Arg             args[2];
  Pixel           back   = 0L;
  Pixel           fore   = 0L;
  Screen*         screen;
  unsigned int    depth;
  Pixmap          pixmap = 0L;
/*.........................................................................*/
  if(This==NULL)    return 0L;
  if(a_fname==NULL) return 0L;
  XtSetArg          (args[0],XtNbackground,&back);
  XtSetArg          (args[1],XtNforeground,&fore);
  XtGetValues       (This,args,2);
  screen            = XtScreen(This);
  depth             = DefaultDepthOfScreen(screen);
#ifdef DEBUG
  printf ("debug : XWidgetLoadPixmapFile : %s\n",a_fname);
#endif
#ifdef HAS_XMU
  pixmap            = XmuLocatePixmapFile (screen,a_fname,fore,back,depth,NULL,0,NULL,NULL,NULL,NULL);
#else  /*HAS_XMU*/
  pixmap            = 0L;
#endif /*HAS_XMU*/
#ifdef DEBUG
  printf ("debug : XWidgetLoadPixmapFile : %lu\n",pixmap);
#endif
  return            pixmap == None ? 0L : pixmap;
}
/***************************************************************************/
void XWidgetPutPixmapInRootWindow (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Pixmap pixmap;
/*.........................................................................*/
  pixmap             = XWidgetGetPixmap (This);
  XDisplayPutPixmapInRootWindow  (XtDisplay(This),pixmap);
  if(pixmap!=0L)         XFreePixmap (XtDisplay(This),pixmap);
}
/***************************************************************************/
void XWidgetSetShape (
 Widget This 
,XShape a_shape
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget    parent;
  va_list   vargs;
/*.........................................................................*/
  if(This==NULL) return;
  if( !XtIsWidget(This) || !XtIsRealized(This) ) return;

  parent      = XtParent(This);
  if(parent==NULL) return;
  if(!XtIsShell(parent)) parent = This;
  if( !XtIsWidget(parent) || !XtIsRealized(parent) ) return;

/*MOTIF resources*/
 {Arg              args[4];
  XtSetArg         (args[0],"marginWidth"       ,0);
  XtSetArg         (args[1],"marginHeight"      ,0);
  XtSetArg         (args[2],"highlightThickness",0);
  XtSetArg         (args[3],"shadowThickness"   ,0);
  XtSetValues      (This,args,4);}

  if(a_shape==XShapePixmap)
    { Pixmap                  pixmap;
      Pixel                   pixel;
      va_start                (vargs,a_shape);
      pixmap                  = va_arg(vargs,Pixmap);
      va_end                  (vargs);
      pixel                   = XWidgetGetPixelResource(This,XtNbackground);
      XDisplaySetWindowShapeE (XtDisplay(This),XtWindow(This),XtWindow(parent),a_shape,pixmap,pixel);
    }
  else
    {
      va_start                (vargs,a_shape);
      XDisplaySetWindowShapeV (XtDisplay(This),XtWindow(This),XtWindow(parent),a_shape,vargs);
      va_end                  (vargs);
    }

}
/***************************************************************************/
void XWidgetExecuteChangeSizeFunction (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->core.widget_class->core_class.resize==NULL) return;
  (This->core.widget_class->core_class.resize)(This);
}
/***************************************************************************/
void XWidgetExecuteExposeFunction (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->core.widget_class->core_class.expose==NULL) return;
  (This->core.widget_class->core_class.expose)(This,NULL,NULL);
}
/***************************************************************************/
void XWidgetIconify (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(!XtIsRealized(This)) return;
  XIconifyWindow(XtDisplay(This),XtWindow(This),XScreenNumberOfScreen(XtScreen(This)));
}
/***************************************************************************/
void XWidgetUniconify (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  if(!XtIsRealized(This)) return;
  XDisplaySetWindowToNormalState (XtDisplay(This),XtWindow(This));
}
/***************************************************************************/
void XWidgetSetWindowManagerCloseFunction (
 Widget This 
,XtActionProc a_proc
)
/***************************************************************************/
/*
  Widget must be realized to install a WMClose procedure.                
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static XtActionsRec  action = {"CloseWindow",CloseWindow};
/*.........................................................................*/
  if(This==NULL)          return;
  if(!XtIsShell(This))    return;
  if(!XtIsRealized(This)) return;

  if(Class.trans_table==NULL) 
    {
      XtAppAddActions             (XtWidgetToApplicationContext(This),&action,1);
      Class.trans_table           = XtParseTranslationTable("<Message>WM_PROTOCOLS:CloseWindow()");
      Class.wm_delete_window_atom = XInternAtom(XtDisplay(This), "WM_DELETE_WINDOW",False);
    }

  if(a_proc==NULL)
    {
      if(action.proc!=CloseWindow)
        {
	  action.proc               = CloseWindow;
	  XtAppAddActions           (XtWidgetToApplicationContext(This),&action,1);
	}
    }
  else 
    {
      if(a_proc!=action.proc)
        {
	  action.proc               = a_proc;
	  XtAppAddActions           (XtWidgetToApplicationContext(This),&action,1);
        }
    }

  XtOverrideTranslations    (This,Class.trans_table);
  XSetWMProtocols           (XtDisplay(This), XtWindow(This),&Class.wm_delete_window_atom, 1);
}
/***************************************************************************/
static void CloseWindow (
 Widget This 
,XEvent* a_event 
,char** a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_event==NULL) return;
  if (Class.wm_delete_window_atom==0L)                           return;
  if (a_event->type              != ClientMessage)               return;
  if ((Ulong)a_event->xclient.data.l[0] != Class.wm_delete_window_atom) return;
  XtDestroyApplicationContext  (XtWidgetToApplicationContext(This));
  exit                         (EXIT_SUCCESS);
  a_argn  = 0;
  a_args  = NULL; 
}
/***************************************************************************/
void XWidgetPutPixmapPageInPS_Stream (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Pixmap                       pixmap;
/*.........................................................................*/
  if(This==NULL)               return;
  pixmap                       = XWidgetGetPixmap  (This);
  if(pixmap==0L)               return;
  XDisplayPutPixmapInPS_Stream (XtDisplay(This),pixmap);
  XFreePixmap                  (XtDisplay(This),pixmap);
}
/***************************************************************************/
void XWidgetPutFrameInPS_Stream (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Dimension         ww,wh;
  double            red,green,blue;
/*.........................................................................*/
  if(This==NULL)         return;

  XWidgetGetDimension (This,&ww,&wh);
  XDisplayGetPixelRGB     (XtDisplay(This),XWidgetGetPixelResource(This,"background"),&red,&green,&blue);
  CPS_PutBackgroundInStream       (red,green,blue,(double)ww,(double)wh);

  XDisplayGetPixelRGB     (XtDisplay(This),XWidgetGetPixelResource(This,"borderColor"),&red,&green,&blue);
  CPS_PutFrameInStream      (red,green,blue,(double)ww,(double)wh);
}
/***************************************************************************/
void XWidgetPutPixmapInPS_Stream (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Pixmap    pixmap;
  XImage*   image;
  Dimension ww,wh;
  double    red,green,blue;
/*.........................................................................*/
  if(This==NULL)              return;

  pixmap                 = XWidgetGetPixmap  (This);

  image                  = XDisplayGetPixmapImage (XtDisplay(This),pixmap);
  XDisplayPutImageInPS_Stream      (XtDisplay(This),image);
  if(image!=NULL)              XDestroyImage (image);
  if(pixmap!=0L)             XFreePixmap (XtDisplay(This),pixmap);

/* border not in widget pixmap */
  XWidgetGetDimension      (This,&ww,&wh);
  XDisplayGetPixelRGB          (XtDisplay(This),XWidgetGetPixelResource(This,"borderColor"),&red,&green,&blue);
  CPS_PutFrameInStream           (red,green,blue,(double)ww,(double)wh);

}
/***************************************************************************/
void XWidgetSavePixmapInGIF_File (
 Widget This
,char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Pixmap   pixmap;
  FILE*    file;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_fname==NULL) return;
  pixmap          = XWidgetGetPixmap  (This);
  if(pixmap==0L)  return;
  file            = CFileOpenForWriting (a_fname);
  if(file!=NULL) 
    {
      XImage*            image;
      image              = XDisplayGetPixmapImage (XtDisplay(This),pixmap);
      XDisplayPutImageInGIF_Stream (XtDisplay(This),image,file);
      if(image!=NULL)    XDestroyImage (image);
      fclose             (file);
    }
  XFreePixmap  (XtDisplay(This),pixmap);
}
/***************************************************************************/
void XWidgetSavePixmapInXBM_File (
 Widget This
,char* a_fname
,char* a_name
,Pixel a_back
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Pixmap   pixmap;
  FILE*    file;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_fname==NULL) return;
  pixmap          = XWidgetGetPixmap  (This);
  if(pixmap==0L)  return;
  file            = CFileOpenForWriting (a_fname);
  if(file!=NULL) 
    {
      XImage*            image;
      image              = XDisplayGetPixmapImage (XtDisplay(This),pixmap);
      XImagePutBitmapInXBM_Stream (image,file,a_name,a_back);
      if(image!=NULL)          XDestroyImage (image);
      fclose             (file);
    }
  XFreePixmap  (XtDisplay(This),pixmap);
}
/***************************************************************************/
void XWidgetSwapWindows (
 Widget This 
,Widget a_1 
,Widget a_2 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static int    swap = 0;
  static Widget wto = NULL ,w1 = NULL ,w2 = NULL;
  static GC     gc = NULL;
/*.........................................................................*/
  if( (This==NULL) && (a_1==NULL) && (a_2==NULL) )
    {
      if(swap==1) 
	{
	  swap = 0;
	  CopyWindow (wto,w1,gc);
	}
      else    
	{
	  swap = 1;
	  CopyWindow (wto,w2,gc);
	}  
    }
  else
    {
      XGCValues  values;
      XtGCMask   mask = 0L;
      swap       = 0;
      wto        = This;
      w1         = a_1;
      w2         = a_2;
      if(gc!=NULL) XtReleaseGC (This,gc);
      gc         = XtGetGC((Widget)This, mask, &values);
    }
}
/***************************************************************************/
static void CopyWindow (
 Widget This 
,Widget a_from 
,GC     a_gc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)      return;
  if(a_from==NULL)    return;
  if( !XtIsWidget(This)   || !XtIsRealized(This)   ) return;
  if( !XtIsWidget(a_from) || !XtIsRealized(a_from) ) return;

  XCopyArea       (XtDisplay(This),
                   XtWindow(a_from),XtWindow(This),
                   a_gc,0,0,a_from->core.width,a_from->core.height,0,0);

}
/***************************************************************************/
Widget XWidgetGetNextBrother (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget     parent;
  WidgetList list;
  int        number;
   int count;
/*.........................................................................*/
  if(This==NULL) return NULL;
  parent     = XtParent(This);
  list       = XWidgetGetChildren (parent,&number);
  if( (number!=0)  && (list!=NULL) ) 
    {
      for(count=0;count<number;count++) 
        {
	  if(list[count]==This)
	    {
	      Widget      nextBrother;
	      nextBrother = (count<number-1) ? list[count+1] : list[0];
	      CMemoryFreeBlock    (list);
	      return      nextBrother;
	    }
	}
    }
  CMemoryFreeBlock (list);
  return   NULL;
}
/***************************************************************************/
XWidgetGetLocalTargetFunction XWidgetSetGetLocalTargetFunction (
 XWidgetGetLocalTargetFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWidgetGetLocalTargetFunction old;
/*.........................................................................*/
  old                   = Class.localTargetProc;
  Class.localTargetProc = a_proc;
  return                old;
}
/***************************************************************************/
XWidgetGetLocalTargetFunction XWidgetGetGetLocalTargetFunction (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.localTargetProc;
}
/***************************************************************************/
Widget XWidgetGetLocalTarget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget                        widget;
  XWidgetGetLocalTargetFunction proc;
  char*                         string;
/*.........................................................................*/
  if(This==NULL) return NULL;
  proc          = XWidgetGetGetLocalTargetFunction ();
  string        = proc==NULL ? CStringNULL : proc(This);
  widget        = XWidgetGetIdentifier (string);
  CStringDelete (string);
  return        widget;
}
/***************************************************************************/
XWidgetCallbackEventFunction XWidgetSetCallbackEventFunction (
 XWidgetCallbackEventFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XWidgetCallbackEventFunction old;
/*.........................................................................*/
  old                = Class.cbkEventProc;
  Class.cbkEventProc = a_proc;
  return             old;
}
/***************************************************************************/
char* XWidgetGetInformations (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*        sval;
  char*        schildren;
  char*        spopup;
  char*        hierarchy;
  Cardinal     length;
  char*        string;
  WidgetList   items,itema;
/*.........................................................................*/
  if(This==NULL) return NULL;

  sval        = CStringCreate (128);

/* give children */
  schildren   = CStringDuplicate("");   
  items       = XWidgetGetChildren (This,NULL);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      sprintf    (sval,"%-32s %s\n",XtName(*itema),XWidgetGetClassName(*itema));
      CStringConcatenate (&schildren,sval);
    }
  CListDelete ((CList)items);

/* give popups */
  spopup  = CStringDuplicate("");   
  items   = XWidgetGetPopupChildren (This,NULL);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      sprintf    (sval,"%-32s %s\n",XtName(*itema),XWidgetGetClassName(*itema));
      CStringConcatenate (&spopup,sval);
    }
  CListDelete    ((CList)items);

  hierarchy     = GetUpHierarchy (XtParent(This));

  length     = strlen (StringNotNull(XtName(This))) + 
               strlen (XWidgetGetClassName(This)) + 
               20 +
               strlen (schildren) + 
               strlen (spopup) +
               strlen (StringNotNull(hierarchy))+128;

  string     = CStringCreateF (length,
"\
     name:%s\n\
    class:%s\n\
   window:%d\n\
 children:\n%s\n\
   popups:\n%s\n\
  parents:\n%s\n\
", 
                   StringNotNull(XtName(This)),
                   XWidgetGetClassName(This),
                   XtWindow(This),
                   schildren,
                   spopup,
                   StringNotNull(hierarchy));

  CStringDelete    (schildren);
  CStringDelete    (spopup);
  CStringDelete    (hierarchy);
  CStringDelete    (sval);

  return           string;
}
/***************************************************************************/
static char* GetUpHierarchy (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
  char name[128];
/*.........................................................................*/
  if(This==NULL)            return NULL;
  if(XtName(This)==NULL)    return NULL;
  string     = GetUpHierarchy (XtParent(This));
  sprintf    (name,"%-32s %s\n",XtName(This),XWidgetGetClassName(This));
  CStringConcatenate (&string,name);
  return     string;
}
/***************************************************************************/
int XWidgetSetAttribute (
 Widget This
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
       if(strcmp(a_name,"target")==0) 
	 {
	   Widget  widget;
           widget  = XWidgetGetIdentifier (*((OString*)a_addr));
	   if(widget!=NULL)
	     {
	       XWidgetSetTarget (widget);
	     }
	   else 
	     {
	       CWarnF("XWidgetSetAttribute: Unknown widget %s.\n",*((OString*)a_addr));
	       return 0;
	     }
	 }
       else if(strcmp(a_name,"grabButton")==0)  
	 {
	   Class.grabButton = *((Uint*)a_addr);
	   if(Class.grabButton>3) Class.grabButton = 1;
	 }
       else if(strcmp(a_name,"grabCursor")==0)  
	 {
	   char*             string;
	   CStringDelete     (Class.grabCursor);
	   Class.grabCursor  = 0;
	   string            = *((OString*)a_addr);
	   if( (string==NULL) || (strcmp(string,"default")==0) ) 
	     Class.grabCursor = CStringDuplicate(Class.defaultCursor);
	   else
	     Class.grabCursor = CStringDuplicate(string);
	 }
       else if(strcmp(a_name,"grabbedX")==0)  
	 {}
       else if(strcmp(a_name,"grabbedY")==0)  
	 {}
       else if(strcmp(a_name,"grabbedWidth")==0)  
	 {}
       else if(strcmp(a_name,"grabbedHeight")==0)  
	 {}
       else if(strcmp(a_name,"grabbedWidget")==0)  
	 {}
       else 
	 {
	   CInfoF ("XWidgetSetAttribute: %s is not a class property.\n",a_name);
	   return 0;
	 }
       return 1;
    }

  if(strcmp(a_name,"name")==0)  
    {
      XWidgetSetName (This,*((OString*)a_addr));
    }
  else if(strcmp(a_name,"cursor")==0)  
    {
      char*  string;
      string = *((OString*)a_addr);
      if(string==NULL) return 0;
      if(strcmp(string,"default")==0)
	XWidgetSetCursor (This,NULL);
      else
	XWidgetSetCursor (This,string);
    }
  else if(strcmp(a_name,"shape")==0)  
    {
      char*  string;
      string = *((OString*)a_addr);
      if(string==NULL) return 0;
      if(strcmp(string,"foreground")==0)
	{
	  Pixmap           pixmap;
	  pixmap           = XWidgetGetPixmap (This);
	  XWidgetSetShape  (This,XShapePixmap,pixmap);
	  if(pixmap!=0L)   XFreePixmap (XtDisplay(This),pixmap);
	}
      else if(strcmp(string,"oval")==0)
	{
	  XWidgetSetShape  (This,XShapeOval);
	}
      else if(strcmp(string,"rectangular")==0)
	{
	  XWidgetSetShape  (This,XShapeRectangular);
	}
      else
	{
	  Pixmap           pixmap;
	  pixmap           = XWidgetGetResource (This,string);
	  XWidgetSetShape  (This,XShapePixmap,pixmap);
	}
    }
  else if(strcmp(a_name,"raise")==0)  
    {}
  else if(strcmp(a_name,"lower")==0)  
    {}
  else if(strcmp(a_name,"map")==0)  
    {}
  else if(strcmp(a_name,"unmap")==0)  
    {}
  else if(strcmp(a_name,"manage")==0)  
    {}
  else if(strcmp(a_name,"unmanage")==0)  
    {}
  else if(strcmp(a_name,"popup")==0)  
    {}
  else if(strcmp(a_name,"popdown")==0)  
    {}
  else if(strcmp(a_name,"draw")==0)  
    {}
  else if(strcmp(a_name,"sendPixmapToRootWindow")==0)  
    {}
  else if(strcmp(a_name,"putPixmapInPS")==0)  
    {}
  else if(strcmp(a_name,"grabRootWindowPart")==0)  
    {}
  else if(strcmp(a_name,"grabWidget")==0)  
    {}
  else 
    {
      CInfoF ("XWidgetSetAttribute: %s is not a property.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int XWidgetGetAttribute (
 Widget This
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
       if(strcmp(a_name,"this")==0) 
	 {
	   Widget            widget;
	   widget            = XWidgetGetThisWidget();
	   if(widget==NULL)  return 0;
	   sprintf           (Class.string,"Widget/%lu",(unsigned long)widget);
	   *((char**)a_addr) = Class.string;
	 }
       else if(strcmp(a_name,"target")==0) 
	 {
	   Widget        widget;
	   widget        = XWidgetGetTarget();
	   if(widget==NULL)  return 0;
	   sprintf       (Class.string,"Widget/%lu",(unsigned long)widget);
	   *((char**)a_addr) = Class.string;
	 }
       else if(strcmp(a_name,"thisParent")==0)     
	 {
	   Widget        widget;
	   widget        = XtParent(XWidgetGetThisWidget());
	   if(widget==NULL)  return 0;
	   sprintf       (Class.string,"Widget/%lu",(unsigned long)widget);
	   *((char**)a_addr) = Class.string;
	 }
       else if(strcmp(a_name,"thisShell")==0)     
	 {
	   Widget        widget;
	   widget        = XWidgetGetShell(XWidgetGetThisWidget());
	   if(widget==NULL)  return 0;
	   sprintf       (Class.string,"Widget/%lu",(unsigned long)widget);
	   *((char**)a_addr) = Class.string;
	 }
       else if(strcmp(a_name,"thisLocalTarget")==0) 
	 {
	   Widget            widget;
	   widget            = XWidgetGetLocalTarget (XWidgetGetThisWidget());
	   if(widget==NULL)  return 0;
	   sprintf           (Class.string,"Widget/%lu",(unsigned long)widget);
	   *((char**)a_addr) = Class.string;
	 }
       else if(strcmp(a_name,"eventType")==0) 
	 {
	   int number;
	   GetEventInformation("type",&number);
	   *((int*)a_addr) = number;
	 }
       else if(strcmp(a_name,"eventX")==0) 
	 {
	   int number;
	   GetEventInformation("x",&number);
	   *((int*)a_addr) = number;
	 }
       else if(strcmp(a_name,"eventY")==0) 
	 {
	   int number;
	   GetEventInformation("y",&number);
	   *((int*)a_addr) = number;
	 }
       else if(strcmp(a_name,"grabButton")==0)  *((int*)a_addr)     = Class.grabButton;
       else if(strcmp(a_name,"grabCursor")==0)  *((OString*)a_addr) = Class.grabCursor;
       else if(strcmp(a_name,"grabbedX")==0)        *((int*)a_addr) = Class.grabbedX;
       else if(strcmp(a_name,"grabbedY")==0)        *((int*)a_addr) = Class.grabbedY;
       else if(strcmp(a_name,"grabbedWidth")==0)    *((int*)a_addr) = Class.grabbedWidth;
       else if(strcmp(a_name,"grabbedHeight")==0)   *((int*)a_addr) = Class.grabbedHeight;
       else if(strcmp(a_name,"grabbedWidget")==0)   
	 {
	   Widget        widget;
	   widget        = Class.grabbedWidget;
	   if(widget==NULL)  return 0;
	   sprintf       (Class.string,"Widget/%lu",(unsigned long)widget);
	   *((char**)a_addr) = Class.string;
	 }
       else 
	 {
	   CInfoF("XWidgetGetAttribute: %s is not a class property.\n",a_name);
	   return 0;
	 }
       return 1;
    }


       if(strcmp(a_name,"identifier")==0)     *((Ulong*)a_addr) = (Ulong)This;
  else if(strcmp(a_name,"name")==0)           *((char**)a_addr) = XtName(This);
  else if(strcmp(a_name,"class")==0)          *((char**)a_addr) = XWidgetGetClassName(This);
  else if(strcmp(a_name,"parent")==0)     
    {
      Widget        widget;
      widget        = XtParent(This);
      if(widget==NULL)  return 0;
      sprintf       (Class.string,"Widget/%lu",(unsigned long)widget);
      *((char**)a_addr) = Class.string;
    }
  else if(strcmp(a_name,"shell")==0)     
    {
      Widget        widget;
      widget        = XWidgetGetShell(This);
      if(widget==NULL)  return 0;
      sprintf       (Class.string,"Widget/%lu",(unsigned long)widget);
      *((char**)a_addr) = Class.string;
    }
  else if(strcmp(a_name,"nextBrother")==0)     
    {
      Widget        widget;
      widget        = XWidgetGetNextBrother(This); 
      if(widget==NULL)  return 0;
      sprintf       (Class.string,"Widget/%lu",(unsigned long)widget);
      *((char**)a_addr) = Class.string;
    }
  else if(strcmp(a_name,"localTarget")==0) 
    {
      Widget                        widget;
      XWidgetGetLocalTargetFunction proc;
      char*                         string;
      proc          = XWidgetGetGetLocalTargetFunction ();
      string        = proc==NULL ? CStringNULL : proc(This);
      widget        = XWidgetGetIdentifier (string);
      CStringDelete (string);
      if(widget==NULL)  return 0;
      sprintf       (Class.string,"Widget/%lu",(unsigned long)widget);
      *((char**)a_addr) = Class.string;
    }
  else if(strcmp(a_name,"children")==0)
    {
      WidgetList    items,itema;
      int           stringn = 0;
      char**        strings = NULL;
      items         = XWidgetGetChildren (This,NULL);
      for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) 
	{
	  Widget       widget;
          widget       = *itema;
	  if(widget!=NULL)
	    {
	      sprintf      (Class.string,"Widget/%lu",(unsigned long)widget);
	      CTextAddLine (&stringn,&strings,Class.string);
	    }
	}
      CListDelete ((CList)items);
      if(a_number!=NULL) *a_number = stringn;
      *((char***)a_addr) = strings;
      return FREE_BLOCKS;
    }
  else if(strcmp(a_name,"managedChildren")==0)
    {
      WidgetList    items,itema;
      int           stringn = 0;
      char**        strings = NULL;
      items         = XWidgetGetChildren (This,NULL);
      for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) 
	{
	  Widget       widget;
          widget       = *itema;
	  if( (widget!=NULL) && (XtIsManaged(widget)==True) )
	    {
	      sprintf      (Class.string,"Widget/%lu",(unsigned long)widget);
	      CTextAddLine (&stringn,&strings,Class.string);
	    }
	}
      CListDelete ((void**)items);
      if(a_number!=NULL) *a_number = stringn;
      *((char***)a_addr) = strings;
      return FREE_BLOCKS;
    }
  else if(strcmp(a_name,"cursor")==0)      *((char**)a_addr) = NULL;
  else if(strcmp(a_name,"shape")==0)       *((char**)a_addr) = NULL;
  else 
    {
      CInfoF ("XWidgetGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int XWidgetDoMethod (
 Widget This
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

  if(This==NULL)
    {
      CInfoF ("XWidgetDoMethod: %s is not a class routine property.\n",a_name);
      return 0;
    }


       if(strcmp(a_name,"raise")==0)          XWidgetRaise(This);
  else if(strcmp(a_name,"lower")==0)          XWidgetLower(This);
  else if(strcmp(a_name,"map")==0)            XWidgetMap(This);
  else if(strcmp(a_name,"unmap")==0)          XWidgetUnmap(This);
  else if(strcmp(a_name,"manage")==0)         XWidgetManage(This);
  else if(strcmp(a_name,"unmanage")==0)       XWidgetUnmanage(This);
  else if(strcmp(a_name,"popup")==0)          XWidgetPopup(This);
  else if(strcmp(a_name,"popdown")==0)        XWidgetPopdown(This);
  else if(strcmp(a_name,"iconify")==0)        XWidgetIconify(This);
  else if(strcmp(a_name,"uniconify")==0)      XWidgetUniconify(This);
  else if(strcmp(a_name,"draw")==0)           XWidgetExecuteExposeFunction(This);
  else if(strcmp(a_name,"sendPixmapToRootWindow")==0) XWidgetPutPixmapInRootWindow(This);
  else if(strcmp(a_name,"putPixmapInPS")==0)          XWidgetPutPixmapPageInPS_Stream(This);
  else if(strcmp(a_name,"move")==0)  
    {
      int      status;
      Position x,y;
      if(a_argn!=2) 
	{
	  CWarnF  ("Two arguments expected for %s property.\n",a_name);
	  return 0;
	}

      x      = (Position)CStringConvertToLong(a_args[0],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[0]);
	  return 0;
	}
      y      = (Position)CStringConvertToLong(a_args[1],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[1]);
	  return 0;
	}
      XWidgetMove   (This,x,y);
    }
  else if(strcmp(a_name,"changeSize")==0)  
    {
      int       status;
      Dimension width,height;
      if(a_argn!=2) 
	{
	  CWarnF  ("Two arguments expected for %s property.\n",a_name);
	  return 0;
	}
      width  = (Position)CStringConvertToLong(a_args[0],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[0]);
	  return 0;
	}
      height = (Position)CStringConvertToLong(a_args[1],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[1]);
	  return 0;
	}
      XWidgetSetDimension (This,width,height);
    }
  else if(strcmp(a_name,"callCallbacks")==0)  
    {
      if(a_argn!=1) 
	{
	  CWarnF  ("One argument expected for %s property.\n",a_name);
	  return 0;
	}
      XWidgetCallCallbacks  (This,a_args[0]);
    }
  else if(strcmp(a_name,"getInformations")==0)
    {
      *((OString*)a_addr)   = XWidgetGetInformations (This);
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"grabRootWindowPart")==0)  
    {
      XWidgetGrabRootWindowPart(This);
    }
  else if(strcmp(a_name,"grabWidget")==0)  
    {
      XWidgetGrab(This);
    }
  else 
    {
      CInfoF ("XWidgetDoMethod: %s is not a routine property.\n",a_name);
      return 0;
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  a_addr = NULL;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XWidgetSetThisWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.thisWidget = This;
}
/***************************************************************************/
Widget XWidgetGetThisWidget (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.thisWidget;
}
/***************************************************************************/
void XWidgetSetCallData (
 XtPointer a_data
,int a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.callData = a_data;
  Class.typeData = a_type;
}
/***************************************************************************/
XtPointer XWidgetGetCallData (
 int* a_type
)
/***************************************************************************/
/*
  - for a callBack       return a_data pointer  (*a_type=0).
  - for an event handler return a_event pointer (*a_type=1).
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_type!=NULL) *a_type = Class.typeData;
  return Class.callData;
}
/***************************************************************************/
void XWidgetSetCallValue (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete   (Class.callValue);
  Class.callValue = CStringDuplicate (a_string);
}
/***************************************************************************/
char* XWidgetGetCallValue (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.callValue;
}
/***************************************************************************/
Widget XWidgetGetTarget (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.targetWidget;
}
/***************************************************************************/
void XWidgetSetTarget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
 Arg          args[2];
/*.........................................................................*/
/* restore previous target */
  if(XWidgetIsValid(Class.targetWidget)==True) 
    { /* target widget still exist */
      XtSetArg     (args[0],XtNborderColor,Class.prev_border_color);
      XtSetArg     (args[1],XtNborderWidth,Class.prev_border_width);
      XtSetValues  (Class.targetWidget,args,2);
    }
  Class.targetWidget        = NULL;
  Class.prev_border_color   = 0L;
  Class.prev_border_width   = 0;
/* set up new target */
  if(XWidgetIsValid(This)==False)   return;
  Class.targetWidget         = This;
  XtSetArg                   (args[0],XtNborderColor,&(Class.prev_border_color));
  XtSetArg                   (args[1],XtNborderWidth,&(Class.prev_border_width));
  XtGetValues                (This,args,2);
  if(Class.target_border_color==0L) 
    {
      Class.target_border_color = XWidgetConvertStringToPixel (This,"red");
      Class.target_border_width = 3;
    }
  XtSetArg     (args[0],XtNborderColor,Class.target_border_color);
  XtSetArg     (args[1],XtNborderWidth,Class.target_border_width);
  XtSetValues  (This,args,2);
  if(Class.targetChangedProc!=NULL) 
    {
      XWidgetVisitHierarchyForward   (XWidgetGetTop(),Class.targetChangedProc);
    }
}
/***************************************************************************/
void XWidgetSetTargetChangedFunction (
 XWidgetVisitFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.targetChangedProc = a_proc;
}
/***************************************************************************/
void XWidgetSetTargetBorderPixel (
 Pixel a_pixel
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.target_border_color = a_pixel;
}
/***************************************************************************/
void XWidgetSetTargetBorderWidth (
 Dimension a_width
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.target_border_width = a_width;
}
/***************************************************************************/
char** WidgetListGetNames (
 WidgetList This
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        count;
  char**     list;
/*.........................................................................*/
  if(This==NULL)       return NULL;
  if(a_number<=0) return NULL;
  list            = (char**) CMemoryAllocateBlock ( a_number * sizeof(char*));
  if(list==NULL)       return NULL;
  for(count=0;count<a_number;count++) list[count] = XtName(This[count]);
  return          list;
}
/***************************************************************************/
WidgetList WidgetListCreateFromString (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList list = NULL;
  char*      string;
  int        length,pos,count;
/*.........................................................................*/
  if(a_string==NULL) return NULL;

  string         = (char*)XtNewString(a_string);
  if(string==NULL)    return NULL;
  length         = strlen(string);
  if(length<=0)  {XtFree(string);return NULL;}

  pos            = 0;
  list           = NULL;
  length        ++;
  for(count=0;count<length;count++) 
    {
      if( (string[count]=='\\') || (string[count]=='\n') || (string[count]==0) ) 
        { char           shift_one;
          Widget         widget;
          shift_one      = string[count]=='\n' ? 1 : 0;
          string [count] = 0;
          widget         = XWidgetGetIdentifier (string+pos);
          pos            = shift_one==1 ? count+1:count+2;
          CListAddEntry   ((CList*)&list,widget);
        }
    }

  XtFree    (string);
  return    list;
}
/***************************************************************************/
char* WidgetListConvertToString (
 WidgetList This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL)
    {
      string      = (char*)CMemoryAllocateBlock(2 * sizeof(char));
      if(string==NULL) return NULL;
      string[0]   = '\0';
    }
  else 
    {
      int         pos,number,count;
      string      = (char*)CMemoryAllocateBlock(MAX_STRING * sizeof(char));
      if(string==NULL) return NULL;
      string[0]   = '\0';
      pos         = 0;
      number      = CListGetSize  ((CList)This);
      for(count=0;count<number;count++) 
        { Widget widget;
          widget = This[count];
/* Protect against non existing widget.Could be a window not a widget.*/
          if(XWidgetIsValid(widget)==True)                   
            { 
	      char*       name;
              int         lname;
              name        = XtName (widget);
              lname       = strlen(name);      
              if(pos+lname+3>MAX_STRING) 
                {
                   ReportError    ("WidgetListString",CErrorFlow);
                   CMemoryFreeBlock (string);
                   return NULL;
                }
              sprintf     (string+pos,"%s\\n",name);
              pos += lname + 2;
            }
        }
      if(*string!='\0')
        { /* remove last \n */
          string[pos-2] = 0;
        }
  }
  return string;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ConvertUnknown (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_to->size = 0;
  a_to->addr = NULL;
  a_from     = NULL; /* to remove warning of c++ compiler */
  a_argn     = 0;
  a_args     = NULL;  
}
/***************************************************************************/
static void ConvertXtRIntXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  sprintf    (Class.string,"%d",*((int *)a_from->addr));
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  a_argn  = 0;
  a_args  = NULL;  
}
/***************************************************************************/
static void ConvertXtRBooleanXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(*((Boolean *)a_from->addr)==True) sprintf    (Class.string,"true");
  else                                 sprintf    (Class.string,"false");
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  a_argn  = 0;
  a_args  = NULL;  
}
/***************************************************************************/
static void ConvertXtRFloatXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  sprintf    (Class.string,"%g",*((float *)a_from->addr));
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  a_argn  = 0;
  a_args  = NULL;  
}
/***************************************************************************/
static void ConvertXtRShortXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  sprintf    (Class.string,"%d",*((short *)a_from->addr));
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  a_argn  = 0;
  a_args  = NULL;  
}
/***************************************************************************/
static void ConvertXtRDimensionXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  sprintf    (Class.string,"%d",*((Dimension *)a_from->addr));
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args  = NULL;  
}
/***************************************************************************/
static void ConvertXtRPositionXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  sprintf    (Class.string,"%d",*((Position *)a_from->addr));
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args  = NULL;  
}
/***************************************************************************/
static void ConvertXtRUnsignedCharXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  sprintf    (Class.string,"%c",*((unsigned char *)a_from->addr));
  a_to->size = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr = (XPointer)Class.string;      
  a_argn = 0;
  a_args  = NULL;  
}
/***************************************************************************/
static void ConvertXtRStringXtRWidgetClass (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static WidgetClass wclass = NULL;
/*.........................................................................*/
  a_argn      = 0;
  a_args      = NULL;  
  if(a_from->addr==NULL) return;
  wclass      = XClassGetIdentifier (a_from->addr);
  if(wclass!=NULL)
    { 
      a_to->size  = (unsigned int) sizeof(WidgetClass);
      a_to->addr  = (XPointer) &wclass;
    }
}
/***************************************************************************/
static void ConvertXtRWidgetClassXtRString (
 XrmValuePtr a_args 
,Cardinal* a_argn
,XrmValuePtr a_from
,XrmValuePtr a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
  WidgetClass wclass;
/*.........................................................................*/
  a_argn     = 0;
  a_args      = NULL;  
  wclass      = *((WidgetClass *)a_from->addr);
  if(XClassIsValid(wclass)==False) return; 
  name        = wclass->core_class.class_name;
  if(name==NULL)   return;
  strcpy      (Class.string,name);
  a_to->size  = (unsigned int)(strlen(Class.string)+1);             
  a_to->addr  = (XPointer)Class.string;      
}
/***************************************************************************/
void XClassSetIsValidFunction (
 XClassIsValidFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.widget_class_exist = a_proc;
}
/***************************************************************************/
void XClassSetGetIdentifierFunction (
 XClassGetIdentifierFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.widget_class_fr_name = a_proc;
}
/***************************************************************************/
WidgetClass XClassGetIdentifier  (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.widget_class_fr_name==NULL) 
    {
      ReportError("XClassGetIdentifier",CErrorHandler);
      return NULL;
    }
  return Class.widget_class_fr_name(a_name);
}
/***************************************************************************/
Boolean XClassIsValid (
 WidgetClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.widget_class_exist==NULL) 
    {
      ReportError("XClassIsValid",CErrorHandler);
      return False;
    }
  return Class.widget_class_exist(This);
}
/***************************************************************************/
char* XClassGetName (
 WidgetClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->core_class.class_name;
}
/***************************************************************************/
void XClassIncrementWidgetRecordSize (
 WidgetClass This
,Cardinal a_size
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->core_class.widget_size += a_size;
}
/***************************************************************************/
Boolean XClassIsSubclass (
 WidgetClass This
,WidgetClass a_class
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   WidgetClass w;
/*.........................................................................*/
  if(This==NULL)    return False;
  if(a_class==NULL) return False;
  for (w = This; w != NULL; w = w->core_class.superclass) 
    if (w == a_class) return True;
  return False;
}
/***************************************************************************/
XWidgetVisitFunction XClassGetDestroyFunction (
 WidgetClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return (XWidgetVisitFunction)(This->core_class.destroy);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void XResourceChangesInitialize (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class_changes.add    = 1;
}
/***************************************************************************/
void XResourceChangesCommit (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class_changes.add==0) return;
  SetResource       ();
  Class_changes.add = 0;
}
/***************************************************************************/
void XResourceChangesAddEntry (
 char*   a_name 
,XtArgVal a_value 
,XResourceChangeFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class_changes.count>=MAX_RES)
    {
      ReportError ("XResourceChangesAddEntry",CErrorFlow);
    }
  else
    {
      XtSetArg(Class_changes.list[Class_changes.count],CStringDuplicate(a_name),a_value);
      Class_changes.proc[Class_changes.count] = a_proc;
      Class_changes.count++;
    }
}
/***************************************************************************/
void XResourceChangesFlush (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class_changes.add==1) return;
  SetResource ();
}
/***************************************************************************/
static void SetResource (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class_changes.widget==NULL) return;
  if(Class_changes.count>0) 
    {
       int count;
      XtSetValues (Class_changes.widget,Class_changes.list,Class_changes.count);
      for(count=0;count<Class_changes.count;count++)
        {
	  CStringDelete(Class_changes.list[count].name);
	  if(Class_changes.proc[count]!=NULL) 
	    {
	      Class_changes.proc[count]((void*)(Class_changes.list[count].value));
	    }
	} 
    }
  Class_changes.widget = NULL;
  Class_changes.count  = 0;
}
/***************************************************************************/
int XResourceChangesSetWidget (
 Widget a_widget 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_widget==NULL) return 0;
  if(Class_changes.widget==NULL) 
    {
      Class_changes.widget = a_widget;
      return 1;
    }
  if(a_widget!=Class_changes.widget) 
    {
      CWarnF ("XResourceChangesSetWidget: widget mismatch (%s,%s).\n",XtName(a_widget),XtName(Class_changes.widget));
      return 0;
    }
  return 1;
}
/***************************************************************************/
void XtConvertStringToResourceChange (
 Widget a_widget 
,char* a_name  
,char* a_type  
,char* a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XrmValue       from,to;
/*.........................................................................*/
  if(a_widget==NULL) return;
  if(a_name==NULL)   return;
  if(a_type==NULL)   return;
  if(a_value==NULL)  return;
  if(XResourceChangesSetWidget(a_widget)==0) return;

  from.size = strlen((String)a_value)+1;  
  from.addr = (XPointer)a_value;
  XtConvert (a_widget,XtRString,&from,a_type,&to);
  
  if(to.addr!=NULL) 
    {
      if(sizeof(long)==8) /*64 bits machines.*/
        {
	  switch(to.size)
	    { 
	    case  1: XResourceChangesAddEntry (a_name,(XtArgVal)  (*((char      *)(to.addr))),NULL);
	             XResourceChangesFlush ();
	             break;
	    case  2: XResourceChangesAddEntry (a_name,(XtArgVal)  (*((short     *)(to.addr))),NULL);
                     XResourceChangesFlush ();
                     break;
	    case  4: XResourceChangesAddEntry (a_name,(XtArgVal)  (*((int       *)(to.addr))),NULL);
                     XResourceChangesFlush ();
                     break;
	    case  8: XResourceChangesAddEntry (a_name,(XtArgVal)  (*((long      *)(to.addr))),NULL);
                     XResourceChangesFlush ();
                     break;
            default: XResourceChangesAddEntry (a_name,(XtArgVal)  (*((long      *)(to.addr))),NULL);
                     XResourceChangesFlush ();
                     break; 
            }
	}
      else
	{
	  switch(to.size)
	    { 
	    case  1: XResourceChangesAddEntry (a_name,(XtArgVal)  (*((char      *)(to.addr))),NULL);
	             XResourceChangesFlush ();
	             break;
	    case  2: XResourceChangesAddEntry (a_name,(XtArgVal)  (*((short     *)(to.addr))),NULL);
                     XResourceChangesFlush ();
                     break;
	    case  4: XResourceChangesAddEntry (a_name,(XtArgVal)  (*((int       *)(to.addr))),NULL);
                     XResourceChangesFlush ();
                     break;
            default: XResourceChangesAddEntry (a_name,(XtArgVal)  (*((long      *)(to.addr))),NULL);
                     XResourceChangesFlush ();
                     break; 
            }
	}
    }
  else
    { 
      CWarnF("For %s; probleme to convert XtRString to %s (%s,%s)\n",XtName(a_widget),a_type,a_name,a_value);
      XResourceChangesFlush ();
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int GetEventInformation (
 char* a_what
,int* a_number
)
/***************************************************************************/
/*
  Must be executed in a callback or in an event handler. 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtPointer data;
  XEvent*   xevent = NULL;
  int       type;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_what==NULL) return 0;
  data = XWidgetGetCallData (&type);
  if(data==NULL) return 0;

/*type = 0;called from a        callback, data is the a_data  pointer.*/
/*type = 1;called from an event handler,  data is the a_event pointer.*/
  if(type==1)
    {
      xevent = (XEvent*)data;
    }
  else
    {
      XWidgetCallbackEventFunction cbkProc;
      cbkProc     = XWidgetSetCallbackEventFunction(NULL);      
      xevent      = cbkProc!=NULL ? cbkProc(data) : (XEvent*)NULL;
      XWidgetSetCallbackEventFunction (cbkProc);      
    }

  if(xevent==NULL) return 0;

 {Position x = 0,y = 0;
  int      treated = 1;
  type     = xevent->type; 
  if( (type==ButtonPress)   ||
      (type==ButtonRelease) 
     )
    {
      x = xevent->xbutton.x;
      y = xevent->xbutton.y;
    }
  else if( (type==EnterNotify)   ||
	   (type==LeaveNotify) 
     )
    {
      x = xevent->xcrossing.x;
      y = xevent->xcrossing.y;
    }
  else if( (type==MotionNotify) 
     )
    {
      x = xevent->xmotion.x;
      y = xevent->xmotion.y;
    }
  else
    {
      treated = 0;
    }
      
       if(strcmp(a_what,"type")==0)
    {
      if(a_number!=NULL) *a_number = type;
    }
  else if(strcmp(a_what,"x")==0)
    {
      if(treated==1)
	{
	  if(a_number!=NULL) *a_number = x;
	}
      else
	{
	  CWarnF ("XEvent type %d not treated today.\n",type);
	  return 0;
	}
    }
  else if(strcmp(a_what,"y")==0)
    {
      if(treated==1)
	{
	  if(a_number!=NULL) *a_number = y;
	}
      else
	{
	  CWarnF ("XEvent type %d not treated today.\n",type);
	  return 0;
	}
    }
  else
    {
      CWarnF ("Bad option %s (type, x, y expected).\n",a_what);
      return 0;
    }}
  
  return 1;
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
  CErrorHandle ("Xx","XWidget",a_name,a_code);
}
/***************************************************************************/
void XWidgetSetGrabButtonAndCursor (
 unsigned int  a_button
,char* a_cursor
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Uint     grabButton;
  char*    grabCursor;
/*.........................................................................*/
  grabButton = a_button==0 ? 1 : a_button;
  if( (grabButton<1) || (grabButton>3))
    {
      CWarnF     ("Bad button number %d for grabbing, take button1.\n",grabButton);
      grabButton = 1;
    }
  Class.grabButton = grabButton;

  grabCursor       = a_cursor==NULL ? Class.defaultCursor : a_cursor;

  CStringDelete    (Class.grabCursor);
  Class.grabCursor = CStringDuplicate (grabCursor);
}
/***************************************************************************/
int XWidgetGrabRootWindowPart (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*    grabCursor;
  Cursor   cursor;
  Position begx,begy,endx,endy;
/*.........................................................................*/
  Class.grabbedX       = 0;
  Class.grabbedY       = 0;
  Class.grabbedWidth   = 0;
  Class.grabbedHeight  = 0;
  if(This==NULL) return 0;
  grabCursor = Class.grabCursor;
  if(grabCursor==NULL) grabCursor = Class.defaultCursor;
  cursor = (Cursor)XWidgetConvertStringToCursor(This,grabCursor);
  if(cursor==0L) 
    {
      CWarnF ("Unknown cursor %s for grabbing.\n",grabCursor);
      return 0;
    }
  XDisplayDoRubberBandGrab   (XtDisplay(This),Class.grabButton,cursor,&begx,&begy,&endx,&endy);
  Class.grabbedX       = MINIMUM (begx,endx);
  Class.grabbedY       = MINIMUM (begy,endy);
  Class.grabbedWidth   = MAXIMUM (begx,endx) - Class.grabbedX;
  Class.grabbedHeight  = MAXIMUM (begy,endy) - Class.grabbedY;
  return 1;
}
/***************************************************************************/
Widget XWidgetGrab (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  grabCursor;
  Cursor cursor;
  Window window;
  Widget widget;
/*.........................................................................*/
  Class.grabbedX      = 0;
  Class.grabbedY      = 0;
  Class.grabbedWidget = NULL;
  if(This==NULL) return NULL;
  grabCursor = Class.grabCursor;
  if(grabCursor==NULL) grabCursor = Class.defaultCursor;
  cursor     = (Cursor)XWidgetConvertStringToCursor(This,grabCursor);
  if(cursor==0L) 
    {
      CWarnF ("Unknown cursor %s for grabbing.\n",grabCursor);
      return NULL;
    }
  window = XDisplayGrabPointer (XtDisplay(This),Class.grabButton,cursor,&Class.grabbedX,&Class.grabbedY); 
  widget = (window!=0L ? XtWindowToWidget(XtDisplay(This),window) : (Widget)NULL);
  Class.grabbedWidget = widget;
  return widget;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int XWidgetSpawnShellScript (
 Widget      This
,char*       a_string
,XWidgetShellFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OBatch                obatch;
/*.........................................................................*/
  if(This==NULL)        return 0;
  obatch                = OBatchCreate (a_string,TreatInput,NULL);
  if(obatch==NULL)      return 0;
  OBatchSetUserFunction (obatch,(OBatchUserFunction)a_proc);
  OBatchSetUserData     (obatch,(void*)This);
  XtAppAddInput         (XtWidgetToApplicationContext(This),
			 OBatchGetChannel(obatch),
			 (XtPointer)XtInputReadMask,
			 (XtInputCallbackProc)ExecuteInputCallback,
			 (XtPointer)obatch);
  return                1;
}
/***************************************************************************/
static void  ExecuteInputCallback (
 OBatch     This
,int*       a_fid
,XtInputId* a_id
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OBatchIsValid(This) && (OBatchSurvey(This)==0) ) 
    {
      XtRemoveInput (*a_id);
      OBatchDelete  (This);
    }
  a_fid  = NULL;
}
/***************************************************************************/
static void TreatInput (
 OBatch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OBatchUserFunction proc;
/*.........................................................................*/
  proc     = OBatchGetUserFunction (This);
  if(proc==NULL) return;  
  proc     (OBatchGetUserData(This),OBatchGetString(This));
}

