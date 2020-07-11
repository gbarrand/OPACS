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

#include <XWidget.h> /*For shape.*/

/*Private.*/
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include <X11/CompositeP.h>

#include <XoShapeP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void             InitializeClass                (void);
static void             InitializeWidget               (Widget, Widget,ArgList,Cardinal*);
static void             RealizeWidget                  (Widget, XtValueMask*, XSetWindowAttributes*);
static void             ChangeWidgetSize               (Widget);
static void             DrawWidget                     (Widget, XEvent*, Region);
static Boolean          SetValues                      (Widget,Widget,Widget,ArgList,Cardinal *);
static XtGeometryResult GeometryManager                (Widget,XtWidgetGeometry*,XtWidgetGeometry*);
static void             ShapeWidget                    (Widget);
static void             ConvertXoRWindowShapeXtRString (XrmValuePtr, Cardinal*, XrmValuePtr, XrmValuePtr);
static void             ConvertXtRStringXoRWindowShape (XrmValuePtr, Cardinal*, XrmValuePtr, XrmValuePtr);
#ifdef __cplusplus
}
#endif

#define athis          ((XoShapeWidget)This)->draw
#define acur           ((XoShapeWidget)a_current)->draw
#define IFMOD(a_field) if(athis.a_field != acur.a_field)

static XtResource resources [] ={
  {XoNwindowShape  , "WindowShape"  , XoRWindowShape, sizeof(XShape),  
                               XtOffset(XoShapeWidget, draw.windowShape),XtRImmediate,(XtPointer)XShapeRectangular},
  {XoNexposeCallback   ,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
                               XtOffset(XoShapeWidget,draw.expose_callback),XtRImmediate,(XtPointer)NULL},
  {XoNresizeCallback   ,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
                               XtOffset(XoShapeWidget,draw.resize_callback),XtRImmediate,(XtPointer)NULL},
  {XoNuserCallback,XtCCallback  ,XtRCallback,sizeof(XtCallbackList),
                               XtOffset(XoShapeWidget,draw.user_callback),XtRImmediate,(XtPointer)NULL}
};

XoShapeClassRec  xoShapeClassRec = {
/* Core Class Part */
{
    (WidgetClass) &compositeClassRec, /* pointer to superclass ClassRec   */
    "XoShape",         		/* widget resource class name       */
    sizeof(XoShapeRec),	        /* size in bytes of widget record   */
    InitializeClass,            /* class_initialize                 */
    NULL, 			/* dynamic initialization           */
    FALSE,       		/* has class been initialized?      */
    InitializeWidget,           /* initialize                       */
    NULL,			/* notify that initialize called    */
    RealizeWidget,              /* XCreateWindow for widget         */
    NULL,     		        /* widget semantics name to proc map*/
    0, 			        /* number of entries in actions     */
    resources,                  /* resources for subclass fields    */
    XtNumber(resources),        /* number of entries in resources   */
    NULLQUARK,		        /* resource class quarkified        */
    TRUE,			/* compress MotionNotify for widget */
    TRUE,			/* compress Expose events for widget*/
    TRUE,			/* compress enter and leave events  */
    TRUE,			/* select for VisibilityNotify      */
    NULL,                       /* destroy                          */
    ChangeWidgetSize,           /* resize                           */
    DrawWidget,                 /* expose                           */
    SetValues,	                /* set subclass resource values     */
    NULL,			/* notify that SetValues called    */
    XtInheritSetValuesAlmost,   /* SetValues got "Almost" geo reply*/
    NULL,		        /* notify that get_values called    */
    XtInheritAcceptFocus,       /* assign input focus to widget     */
    XtVersion,	                /* version of intrinsics used       */
    NULL,		        /* list of callback offsets         */
    XtInheritTranslations,      /* translations                     */	
    XtInheritQueryGeometry,     /* return preferred geometry        */
    XtInheritDisplayAccelerator,/* display your accelerator         */
    NULL 		        /* pointer to extension record      */
},
/* Composite Class Part */
{
    GeometryManager,            /* geometry manager for children   */
    XtInheritChangeManaged,     /* change managed state of child   */
    XtInheritInsertChild,       /* physically add child to parent  */
    XtInheritDeleteChild,       /* physically remove child         */
    NULL 		        /* pointer to extension record     */
},
/* XoShape */
{
   NULL
}
   
};

WidgetClass xoShapeWidgetClass = (WidgetClass) &xoShapeClassRec;
/***************************************************************************/
static void InitializeClass (
 void
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XtAddConverter (XtRString,XoRWindowShape ,ConvertXtRStringXoRWindowShape          ,NULL,0);
  XtAddConverter (XoRWindowShape ,XtRString,ConvertXoRWindowShapeXtRString          ,NULL,0);
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

  athis.shapeModified = 0;
  ShapeWidget         (This);

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
  if(xoShapeWidgetClass->core_class.superclass->core_class.realize!=NULL)
    (xoShapeWidgetClass->core_class.superclass->core_class.realize)(This,a_mask,a_watbs);
  ShapeWidget (This);
}
/***************************************************************************/
static void ChangeWidgetSize (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(xoShapeWidgetClass->core_class.superclass->core_class.resize!=NULL)
    (xoShapeWidgetClass->core_class.superclass->core_class.resize)(This);
  XtCallCallbacks  (This, XoNresizeCallback,NULL);
  ShapeWidget      (This);
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
  if(xoShapeWidgetClass->core_class.superclass->core_class.expose!=NULL)
     (xoShapeWidgetClass->core_class.superclass->core_class.expose)(This,a_event,a_region);
  XtCallCallbacks  (This, XoNexposeCallback,NULL);
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
  IFMOD(windowShape) ShapeWidget (This);

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0; /*avoid C++ warnings*/

  return    False;
}
/***************************************************************************/
static XtGeometryResult GeometryManager (
 Widget		   a_child
,XtWidgetGeometry* a_request
,XtWidgetGeometry* a_reply
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_child   = NULL;
  a_request = NULL;
  a_reply   = NULL;
  return    (XtGeometryYes);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ShapeWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  switch(athis.windowShape)
    {
      case XShapeRectangular: 
                        if(athis.shapeModified==0) return;
      default:
                        athis.shapeModified = 1;
                        break;
    }
  XWidgetSetShape (This,athis.windowShape);
}
/***************************************************************************/
static void ConvertXoRWindowShapeXtRString (
 XrmValuePtr a_args
,Cardinal*   a_narg
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char string[64] = "";
/*.........................................................................*/
  strcpy (string,"rectangular");
  switch (*((XShape *)a_source->addr))
    {
      case XShapeOval:            strcpy (string,"oval");break;
      case XShapeDiamond:         strcpy (string,"diamond");break;
      case XShapeBallonArcRight:  strcpy (string,"ballon_arc_right");break;
      case XShapeBallonArcLeft:   strcpy (string,"ballon_arc_left");break;
      case XShapeBallonRectRight: strcpy (string,"ballon_rect_right");break;
      case XShapeBallonRectLeft:  strcpy (string,"ballon_rect_left");break;
      default:                    strcpy (string,"rectangular");break;
    }  
  a_dest->size = (unsigned int)(strlen(string)+1);  
  a_dest->addr = (XPointer)string;      
  a_args       = NULL; 
  a_narg       = 0;
}
/***************************************************************************/
static void ConvertXtRStringXoRWindowShape (
 XrmValuePtr a_args
,Cardinal*   a_narg
,XrmValuePtr a_source
,XrmValuePtr a_dest
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static XShape value = XShapeRectangular;
/*.........................................................................*/
  if(a_source->addr==NULL) return;
  value  = XShapeRectangular;
       if(strcmp(a_source->addr,"retangular")==0)        value  = XShapeRectangular;
  else if(strcmp(a_source->addr,"oval")==0)              value  = XShapeOval;
  else if(strcmp(a_source->addr,"diamond")==0)           value  = XShapeDiamond;
  else if(strcmp(a_source->addr,"ballon_arc_right")==0)  value  = XShapeBallonArcRight;
  else if(strcmp(a_source->addr,"ballon_arc_left")==0)   value  = XShapeBallonArcLeft;
  else if(strcmp(a_source->addr,"ballon_rect_right")==0) value  = XShapeBallonRectRight;
  else if(strcmp(a_source->addr,"ballon_rect_left")==0)  value  = XShapeBallonRectLeft;
  a_dest->addr = (char *)&value;
  a_dest->size = sizeof(XShape);
  a_args       = NULL;
  a_narg       = 0;
}
