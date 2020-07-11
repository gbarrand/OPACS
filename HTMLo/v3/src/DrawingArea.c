/* 
 *  A minimalistic constraint widget
 *
 *  Sun Sep 12 20:03:49 GMT 1993     Gustaf Neumann                 
 */

#ifndef MOTIF

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/*
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/CharSet.h>
#include <X11/Xaw/XawInit.h>
*/

#include "DrawingAreaP.h"

/* Private Definitions */

static void ClassInitialize(void);
static void ClassPartInitialize(WidgetClass);
static void Initialize(Widget, Widget,ArgList,Cardinal*);
static void Resize(Widget);
static Boolean SetValues(Widget current, Widget request, Widget neww,ArgList args,Cardinal *num_args);
static XtGeometryResult GeometryManager(Widget w,XtWidgetGeometry *request,XtWidgetGeometry *reply);
static XtGeometryResult PreferredGeometry(Widget widget,XtWidgetGeometry *request, XtWidgetGeometry *reply);

DrawingAreaClassRec drawingAreaClassRec = {
  { /* core_class fields */
    /* superclass         */    (WidgetClass) &constraintClassRec,
    /* class_name         */    "DrawingArea",
    /* widget_size        */    sizeof(DrawingAreaRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    ClassPartInitialize,
    /* class_inited       */    FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    NULL,
    /* num_resources      */    0,
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    Resize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	PreferredGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager   */   GeometryManager,
    /* change_managed     */   NULL, /*ChangeManaged,*/
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
  },
  { /* constraint_class fields */
    /* subresourses       */   NULL,
    /* subresource_count  */   0,
    /* constraint_size    */   0, 
    /* initialize         */   NULL, /*ConstraintInitialize,*/
    /* destroy            */   NULL,
    /* set_values         */   NULL, /*ConstraintSetValues,*/
    /* extension          */   NULL
  },
  { /* DrawingArea_class fields */
    /* dummy              */   0
  }
};

WidgetClass drawingAreaWidgetClass = (WidgetClass)&drawingAreaClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void ClassInitialize(void)
{
}

static void ClassPartInitialize(WidgetClass wclass)
{
  wclass = NULL;
}

/* ARGSUSED */
static void Initialize(Widget request, Widget neww,ArgList args,Cardinal *num_args)
{
/*
    DrawingAreaWidget daw = (DrawingAreaWidget)neww;
*/
  request  = NULL;
  neww     = NULL;
  args     = NULL;
  num_args = NULL;  
}

static void Resize(Widget w)
{
/*
    DrawingAreaWidget daw = (DrawingAreaWidget)w;
    WidgetList children = daw->composite.children;
    int num_children = daw->composite.num_children;
    Widget *childP;
    Position x, y;
    Dimension width, height;
*/
 w = NULL;
}


/* ARGSUSED */
static XtGeometryResult GeometryManager(Widget w,XtWidgetGeometry *request,XtWidgetGeometry *reply)
{
/*
    Dimension old_width, old_height;
    DrawingAreaWidget daw = (DrawingAreaWidget) XtParent(w);
    DrawingAreaConstraints DrawingArea = (DrawingAreaConstraints) w->core.constraints;
    XtWidgetGeometry allowed;
    XtGeometryResult ret_val;
*/
    w       = NULL;
    request = NULL;
    reply   = NULL;
    return(XtGeometryNo);
}


/* ARGSUSED */
static Boolean SetValues(Widget current, Widget request, Widget neww,ArgList args,Cardinal *num_args)
{
    current = NULL;
    request = NULL;
    neww    = NULL;
    args     = NULL;
    num_args = NULL;  
    return( FALSE );
}


static XtGeometryResult PreferredGeometry(Widget widget,XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
/*
    DrawingAreaWidget w = (DrawingAreaWidget)widget;
 */   
    widget  = NULL;
    request = NULL;
    reply   = NULL;
    return XtGeometryNo;
}



#else   /*MOTIF*/   /*Barrand*/
#include <stddef.h> /*Barrand*/
#endif  /*MOTIF*/   /*Barrand*/
