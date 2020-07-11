/*************************************************************
 *
 * XoRheostat.c
 * XoRheostat widget implementation
 *
 * Author: Joe English, joe@trystero.art.com
 *
 *************************************************************
 *
 * Future enhancements:
 *
 * BUG: If you drag the arrow to a position outside the valid
 *    range too quickly, it sticks inside the dial; it should
 *    peg out at the minimum or maximum value.  I'm not sure
 *    how to determine which, though...
 * write query_geometry method.
 * Add set(value) action
 * SetValues() doesn't check for as much as it should;
 * SetValues() and Initialize() need to do MUCH more range checking
 * When number_intervals is is not a divisor of ValueRange, ticks
 *    aren't drawn where the arrow actually goes.
 * Do we need a tickThickness resource?
 */

#include <stdlib.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0  /*Hex  2^ 1 * 1.921FB54442D18 */
#endif

#include <X11/StringDefs.h>
#ifdef HAS_XM
#include <Xm/Xm.h>
#endif /*HAS_XM*/

/*Private.*/
#include <X11/IntrinsicP.h>

#ifdef HAS_XM
#if (XmVERSION == 1) && (XmREVISION <= 1)
#include <Xm/XmP.h>
#else
#include <Xm/PrimitiveP.h>
#endif
#endif  /*HAS_XM*/

#include <XoRheostatP.h>


/***********************************************************************
 *
 * Convenience macros, defaults, and declarations:
 *
 ***********************************************************************/

#define RADIANS(d)  (M_PI * (d)/ 180.0)
#define DEGREES(r)  ((r) * 180.0 / M_PI)
#define DEG_TO_RAD (180.0 / M_PI)
#define RAD_TO_DEG (M_PI / 180.0)
#define MINIMUM(x,y)	((x) < (y) ? (x) : (y))
#define MAXIMUM(x,y)	((x) > (y) ? (x) : (y))

/* XoRheostat-specific: */
#define MinAngle(w)     ((w)->rheostat.minimum_angle)
#define MaxAngle(w)     ((w)->rheostat.maximum_angle)
#define MinValue(w)     ((w)->rheostat.minimum_value)
#define MaxValue(w)     ((w)->rheostat.maximum_value)
#define AngleRange(w)   ((w)->rheostat.maximum_angle-(w)->rheostat.minimum_angle)
#define ValueRange(w)   ((w)->rheostat.maximum_value-(w)->rheostat.minimum_value)
#define ValueInc(w)   	(((w)->rheostat.maximum_value-(w)->rheostat.minimum_value) \
			 / (w)->rheostat.number_intervals)
#define CenterX(w)      ((int)((w)->core.width)/2)
#define CenterY(w)      ((int)((w)->core.height)/2)

/* MARGIN(w) = space from border to radius. */
#define _MARGIN(w) \
    ( (w)->rheostat.outer_margin \
    + (w)->rheostat.tick_length \
    + (w)->rheostat.dial_thickness \
    + (w)->rheostat.inner_margin)

#ifdef HAS_XM
#define MARGIN(w) (_MARGIN(w)  \
    + (w)->primitive.highlight_thickness  \
    + (w)->primitive.shadow_thickness)
#else
#define MARGIN(w) _MARGIN(w)
#endif

/* Cast operator to keep lint/gcc happy: */
#define W (Widget)

/*
 * Default values for various resources:
 */
#define MIN_RADIUS	2
#define DFLT_RADIUS	30
#define DFLT_OUTER	25
#define DFLT_INNER	20
#define DFLT_WIDTH	20
#define DFLT_ARROWTHICKNESS	0
/* Another good set of defaults: RADIUS=INNER=30, OUTER=15, WIDTH=10 */

#define DFLT_TICKLEN	5
#define DFLT_TICKTHICKNESS	0
#define DFLT_DIALTHICKNESS	3
#define DFLT_MINVALUE	0
#define DFLT_MAXVALUE	100
#define DFLT_MINANGLE	45
#define DFLT_MAXANGLE	315

#define DFLT_NUMTICKS	10
#define DFLT_MARGIN	2

#ifdef __cplusplus
extern "C"{
#endif
/*
 * Method functions:
 */
static void     InitializeWidget  (Widget, Widget,ArgList,Cardinal*);
static void     RealizeWidget     (Widget, XtValueMask*, XSetWindowAttributes*);
static void     DestroyWidget     (Widget);
static void     ChangeWidgetSize  (Widget);
static void     DrawWidget        (Widget, XEvent*, Region);
static Boolean  SetValues         (Widget,Widget,Widget,ArgList,Cardinal *);
/*
 * Action functions:
 */
static void     Set       (Widget,XEvent*,String*,Cardinal*);
static void     Notify    (Widget,XEvent*,String*,Cardinal*);
static void     Increment (Widget,XEvent*,String*,Cardinal*);

/*
 * Private functions:
 */
static void     DrawArrow	   (Widget,GC);
static void     DrawTicks          (Widget,GC);
static void     DrawDial      	   (Widget,GC);
static void     CalculatePosition  (Widget);
static void     GetGCs     	   (Widget);
static void     FreeGCs     	   (Widget);
static void	CallCallbacks	   (Widget, String, XEvent* );

#ifdef __cplusplus
}
#endif
#define athis   ((XoRheostatWidget)This)->rheostat

/***********************************************************************
 *
 * Translation, action, and resource tables:
 *
 ***********************************************************************/

#ifndef HAS_XM		/* use osfXXX instead of XXX */
static char     default_translations[] =
    "<Btn1Down>:        set() \n\
     <Btn1Motion>:      set() \n\
     <Btn1Up>:      	notify() \n\
     <Key>minus:	increment(-1) notify() \n\
     <Key>plus:		increment(+1) notify() \n\
     <Key>osfPageUp:   	increment(+1i) notify() \n\
     <Key>osfPageDown:  increment(-1i) notify() \n\
     <Key>Return:	notify() \n\
    ";
#else			/* no "virtual keysym" braindamage */
static char     default_translations[] =
    "<Btn1Down>:        set() \n\
     <Btn1Motion>:      set() \n\
     <Btn1Up>:      	notify() \n\
     <Key>minus:	increment(-1) notify() \n\
     <Key>plus:		increment(+1) notify() \n\
     <Key>Prior:   	increment(-1i) notify() \n\
     <Key>Next:  	increment(+1i) notify() \n\
     <Key>Home:		increment(min) notify() \n\
     <Key>End:		increment(max) notify() \n\
     <Key>Return:	notify() \n\
    ";
#endif

static XtActionsRec actions[] = {
    { "set",       Set },
    { "notify",    Notify },
    { "increment", Increment }
};

static XtResource resources[] = {
#   define OFFSET(x) (XtOffset(XoRheostatWidget, rheostat.x))
    {XoNvalue, XtCValue, XtRInt, sizeof(int),
        OFFSET(value), XtRImmediate, NULL},
    {XoNminimumValue, XtCMinimum, XtRInt, sizeof(int),
        OFFSET(minimum_value), XtRImmediate, (XtPointer) DFLT_MINVALUE},
    {XoNmaximumValue, XtCMaximum, XtRInt, sizeof(int),
        OFFSET(maximum_value), XtRImmediate, (XtPointer) DFLT_MAXVALUE},
    {XoNminimumAngle, XtCMinimum, XtRInt, sizeof(int),
        OFFSET(minimum_angle), XtRImmediate, (XtPointer) DFLT_MINANGLE},
    {XoNmaximumAngle, XtCMaximum, XtRInt, sizeof(int),
        OFFSET(maximum_angle), XtRImmediate, (XtPointer) DFLT_MAXANGLE},
    {XoNtickGravity, XtCGravity, XtRBoolean, sizeof(Boolean),
        OFFSET(tick_gravity), XtRImmediate, (XtPointer)True},
    {XoNnumberIntervals, XtCNumberIntervals, XtRInt, sizeof(int),
        OFFSET(number_intervals), XtRImmediate, (XtPointer) DFLT_NUMTICKS},
    {XoNresizeArrow, XtCBoolean, XtRBoolean, sizeof(Boolean),
	OFFSET(resize_arrow), XtRString, "True"},
    {XoNsetCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
        OFFSET(set), XtRCallback, NULL},
    {XoNnotifyCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
        OFFSET(notify), XtRCallback, NULL},
    {XoNouterMargin, XtCMargin, XtRDimension, sizeof(Dimension),
	OFFSET(outer_margin), XtRImmediate, (XtPointer) DFLT_MARGIN},
    {XoNtickLength, XtCMargin, XtRDimension, sizeof(Dimension),
        OFFSET(tick_length), XtRImmediate, (XtPointer) DFLT_TICKLEN},
    {XoNdialThickness, XtCThickness, XtRDimension, sizeof(Dimension),
        OFFSET(dial_thickness), XtRImmediate, (XtPointer) DFLT_DIALTHICKNESS},
    {XoNinnerMargin, XtCMargin, XtRDimension, sizeof(Dimension),
	OFFSET(inner_margin), XtRImmediate, (XtPointer) DFLT_MARGIN},
    {XoNradius, XtCLength, XtRDimension, sizeof(Dimension),
        OFFSET(radius), XtRImmediate, (XtPointer) DFLT_RADIUS},
    {XoNouterArrowLength, XtCLength, XtRDimension, sizeof(Dimension),
	OFFSET(outer_arrow_length), XtRImmediate, (XtPointer) DFLT_OUTER},
    {XoNinnerArrowLength, XtCLength, XtRDimension, sizeof(Dimension),
	OFFSET(inner_arrow_length), XtRImmediate, (XtPointer) DFLT_INNER},
    {XoNarrowWidth, XtCWidth, XtRDimension, sizeof(Dimension),
	OFFSET(arrow_width), XtRImmediate, (XtPointer) DFLT_WIDTH},
    {XoNfillArrow, XtCBoolean, XtRBoolean, sizeof(Boolean),
	OFFSET(fill_arrow), XtRString, "False" },
    {XoNarrowThickness, XtCThickness, XtRDimension, sizeof(Dimension),
	OFFSET(arrow_thickness), XtRImmediate, (XtPointer) DFLT_ARROWTHICKNESS},
    {XoNtickThickness, XtCThickness, XtRDimension, sizeof(Dimension),
        OFFSET(tick_thickness), XtRImmediate, (XtPointer) DFLT_TICKTHICKNESS},
#ifdef HAS_XM
    {XoNuseShadowColors, XtCBoolean, XtRBoolean, sizeof(Boolean),
	OFFSET(use_shadow_colors), XtRString, "False"},
#endif
    {XoNarrowColor, XtCForeground, XtRPixel, sizeof(Pixel),
        OFFSET(arrow_pixel), XtRString, XtDefaultForeground},
    {XoNdialColor, XtCForeground, XtRPixel, sizeof(Pixel),
        OFFSET(dial_pixel), XtRString, XtDefaultForeground},
    {XoNtickColor, XtCForeground, XtRPixel, sizeof(Pixel),
        OFFSET(tick_pixel), XtRString, XtDefaultForeground},
#   undef OFFSET
};

XoRheostatClassRec    xoRheostatClassRec =
{
    /* CoreClassPart */
    {
#ifdef HAS_XM
	/* superclass            */	(WidgetClass) &xmPrimitiveClassRec,
#else
	/* superclass            */	(WidgetClass) &widgetClassRec,
#endif
	/* class_name            */	"XoRheostat",
	/* widget_size           */	sizeof(XoRheostatRec),
	/* class_initialize      */	NULL,
	/* class_part_initialize */	NULL,
	/* class_inited          */	FALSE,
	/* initialize            */	(XtInitProc)InitializeWidget,
	/* initialize_hook       */	NULL,
	/* realize               */	(XtRealizeProc)RealizeWidget,
	/* actions               */	actions,
	/* num_actions           */	XtNumber(actions),
	/* resources             */	resources,
	/* num_resources         */	XtNumber(resources),
	/* xrm_class             */	NULLQUARK,
	/* compress_motion       */	TRUE,
	/* compress_exposure     */	TRUE,
	/* compress_enterleave   */	TRUE,
	/* visible_interest      */	TRUE,
	/* destroy               */	(XtWidgetProc)DestroyWidget,
	/* resize                */	(XtWidgetProc)ChangeWidgetSize,
	/* expose                */	(XtExposeProc)DrawWidget,
	/* SetValues            */	(XtSetValuesFunc)SetValues,
	/* SetValues_hook       */	NULL,
	/* SetValues_almost     */	XtInheritSetValuesAlmost,
	/* get_values_hook       */	NULL,
	/* accept_focus          */	NULL,
	/* version               */	XtVersion,
	/* callback private      */	NULL,
	/* tm_table              */	default_translations,
	/* query_geometry        */	XtInheritQueryGeometry,
	/* display_accelerator   */	XtInheritDisplayAccelerator,
	/* extension             */	NULL
    },
#ifdef HAS_XM
    /* Primitive class fields */
    {
	/* border_highlight      */	(XtWidgetProc) _XtInherit,
	/* border_unhighlight    */	(XtWidgetProc) _XtInherit,
	/* translations          */	XtInheritTranslations,
#if (XmVERSION == 1) && (XmREVISION <= 1)
	/* arm_and_activate      */	(XmArmAndActivate)Notify,  /*see arm_and_activate  Xm/XmP.h*/
#else
	/* arm_and_activate      */	(XtActionProc)Notify,      /*see arm_and_activate in Xm/PrimitiveP.h*/
#endif
	/* syn_resources         */	NULL,
	/* num_syn_resources     */	0,
	/* extension             */	NULL
    },
#endif
    /* XoRheostat class fields */
    {
	/* ignore                */	NULL
    }
};

WidgetClass     xoRheostatWidgetClass = (WidgetClass) &xoRheostatClassRec;
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
  int margin      = MARGIN((XoRheostatWidget)This);
  int user_radius = athis.radius;	                                      /* request from user */
  int size_radius = (int)MINIMUM(This->core.height,This->core.width)/2 - margin;     /* calculated from size */
  int min_radius  = (int)MAXIMUM(athis.inner_arrow_length,athis.outer_arrow_length); /* from arrow dimens */
  int min_dimen;
/*.........................................................................*/
  /*
   * Check geometry:
   * Set radius from  user value, else widget size, else default.
   * Make sure radius is >= inner length & outer length.
   *  %%% This is a bit restrictive -- e.g., r=10,i=10,o=12 should be allowed
   * Make sure width and height are >= 2*(radius + margins)
   */
  if (user_radius != 0)	
    athis.radius = user_radius;
  else if (This->core.width != 0 && This->core.height != 0)
    athis.radius = size_radius;
  else
    athis.radius = DFLT_RADIUS;

  /* Make sure radius is large enough: */
  if ((int)athis.radius < min_radius) 	athis.radius = min_radius;

  /* Make sure widget is large enough: */
  min_dimen = 2*(athis.radius+margin);
  if ((int)This->core.width < min_dimen)   This->core.width = min_dimen;
  if ((int)This->core.height < min_dimen) 	This->core.height = min_dimen;

#ifdef HAS_XM
  if (athis.use_shadow_colors) 
    {
      athis.arrow_pixel = ((XoRheostatWidget)This)->primitive.bottom_shadow_color;
      athis.dial_pixel = ((XoRheostatWidget)This)->primitive.top_shadow_color;
    }
#endif

  GetGCs            (This);
  CalculatePosition (This);
  
  athis.orig_radius       = athis.radius;
  athis.orig_outer_length = athis.outer_arrow_length;
  athis.orig_inner_length = athis.inner_arrow_length;
  athis.orig_width        = athis.arrow_width;

  a_request = NULL;
  a_args    = NULL;
  a_argn    = 0;
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

  if(xoRheostatWidgetClass->core_class.superclass->core_class.realize)
    (xoRheostatWidgetClass->core_class.superclass->core_class.realize)(This, a_mask, a_watbs);

  ChangeWidgetSize (This);

}
/***************************************************************************/
static void DestroyWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  FreeGCs(This);
}
/***************************************************************************/
static void ChangeWidgetSize (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int newr;
/*.........................................................................*/
  if(xoRheostatWidgetClass->core_class.superclass->core_class.resize)
    (xoRheostatWidgetClass->core_class.superclass->core_class.resize)(This);

  newr = (int)MINIMUM(This->core.height,This->core.width) / 2 - MARGIN((XoRheostatWidget)This);

  if (newr < 2) newr = 2;

  if (athis.resize_arrow) 
    {
      int oldr = athis.orig_radius;

      athis.outer_arrow_length = (Dimension)(((int)athis.orig_outer_length * newr) / oldr);
      athis.inner_arrow_length = (Dimension)(((int)athis.orig_inner_length * newr) / oldr);
      athis.arrow_width        = (Dimension)(((int)athis.orig_width        * newr) / oldr);
    }
  athis.radius = newr;
  
  CalculatePosition (This);
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
#ifdef HAS_XM
#if XmREVISION == 1
  {int hlt;
   hlt = ((XoRheostatWidget)This)->primitive.highlight_thickness;
   _XmDrawShadow
     (XtDisplay(This), XtWindow(This),
      ((XoRheostatWidget)This)->primitive.top_shadow_GC, 
      ((XoRheostatWidget)This)->primitive.bottom_shadow_GC,
      (int)((XoRheostatWidget)This)->primitive.shadow_thickness,
      hlt, hlt, (int)(This->core.width - 2*hlt), (int)(This->core.height - 2*hlt));}
#endif
#endif

  DrawTicks (This, athis.tick_GC);
  DrawDial  (This, athis.dial_GC);
  DrawArrow (This, athis.arrow_GC);
  a_event  = NULL;
  a_region = NULL;
}
/***************************************************************************/
static Boolean SetValues (
 Widget current
,Widget a_request
,Widget This
,ArgList a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    Boolean redraw = FALSE;		/* TRUE=>widget needs to be redrawn */
    Boolean recalc = FALSE;		/* TRUE=>arrow position changed */
/*.........................................................................*/
#   define  CHECK(fld)  (((XoRheostatWidget)This)->fld != ((XoRheostatWidget)current)->fld)

    /*
     * Check rheostat parameters:
     */
    if (   CHECK(rheostat.value)
        || CHECK(rheostat.minimum_value) || CHECK(rheostat.maximum_value)
        || CHECK(rheostat.maximum_angle) || CHECK(rheostat.minimum_angle)
    )
    {
        recalc=TRUE;
        redraw=TRUE;
    }

    /*
     * Bounds check:
     */
    if (athis.value > athis.maximum_value)
        athis.value = athis.maximum_value;
    if (athis.value < athis.minimum_value)
        athis.value = athis.minimum_value;

    /*
     * Margin, geometry parameters -- may affect radius;
     */
    if (    CHECK(rheostat.outer_margin)
	 || CHECK(rheostat.dial_thickness)
	 || CHECK(rheostat.inner_margin)
#ifdef HAS_XM
	 || CHECK(primitive.shadow_thickness)
	 || CHECK(primitive.highlight_thickness)
#endif
    )
    {
	int newr = (int)MINIMUM(This->core.height,This->core.width) / 2 - MARGIN((XoRheostatWidget)This);
	if (newr < MIN_RADIUS)
	    newr = MIN_RADIUS;
	athis.radius = newr;
	recalc=TRUE;
    }

    /*
     * Radius and arrow sizes:
     */
    if (   CHECK(rheostat.radius)
	|| CHECK(rheostat.outer_arrow_length)
	|| CHECK(rheostat.inner_arrow_length)
	|| CHECK(rheostat.arrow_width)
    )
    {
	athis.orig_radius = athis.radius;
	athis.orig_outer_length = athis.outer_arrow_length;
	athis.orig_inner_length = athis.inner_arrow_length;
	athis.orig_width = athis.arrow_width;
	recalc = TRUE;
	redraw = TRUE;
    }

    /*
     * Check for color change:
     */
    if (   CHECK(core.background_pixel)
        || CHECK(rheostat.tick_pixel)
        || CHECK(rheostat.dial_pixel)
        || CHECK(rheostat.arrow_pixel)
	|| CHECK(rheostat.arrow_thickness)
	|| CHECK(rheostat.dial_thickness)
    )
    {
        GetGCs  (This);
	FreeGCs (current);
        redraw = TRUE;
    }

    /*
     * Other display resources:
     */
    if (   CHECK(rheostat.number_intervals)
	|| CHECK(rheostat.fill_arrow)
    )
    {
	redraw = TRUE;
    }

    /*
     * Wrap up:
     */
#   undef CHECK
    if (recalc) CalculatePosition (This);
   a_request = NULL;
   a_args    = NULL;
   a_argn    = 0;
    return redraw;
}

/***********************************************************************
 *
 * Action functions:
 *
 ***********************************************************************/

    /*ARGSUSED*/
/***************************************************************************/
static void Set (
 Widget This
,XEvent* event
,String*   a_params
,Cardinal* a_paramn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if (event->type == ButtonPress || event->type == MotionNotify)
    {
      int x,y,v;
      double theta,length,radius;

      x = event->xbutton.x - CenterX(This);
      y = event->xbutton.y - CenterY(This);

      if (!x && !y)       /* click at center of widget -- no angle */
	return;
      /* else */

      radius = sqrt((double)(x*x + y*y));

      /*
       * Calculate value in range 0 .. 360
       */
      theta = DEGREES(atan2((double)(-x), (double)(y)));
      theta -= athis.minimum_angle;
      if (theta < 0.0) theta += 360.0;

      v = (int)( theta * (double)ValueRange((XoRheostatWidget)This) / (double)AngleRange((XoRheostatWidget)This) ) 
	+ athis.minimum_value;


      /*
       * If tick_gravity is on, and click is in tick region,
       * snap to nearest increment:
       */
      length = (double)athis.radius + (double)athis.inner_margin;
      if (   athis.tick_gravity
	  && (radius >= length)
	  && (radius <= (length + (double)athis.dial_thickness + (double)athis.tick_length)))
	{
	  double inc = ValueInc((XoRheostatWidget)This);
	  v = (int)(v / inc + 0.5);
	  v = (int)(v * inc);
	}
      
      /*
       * Bounds-check:
       * Note: v should never be < minimum_value.
       */
      if (v > athis.maximum_value || v < athis.minimum_value) return;
      
      DrawArrow         (This,athis.eraser_GC);
      athis.value        = v;
      CalculatePosition (This);
      DrawArrow         (This,athis.arrow_GC);
    }
  
  CallCallbacks(This, XoNsetCallback, event);
  a_params = NULL;
  a_paramn = 0;
}

/***************************************************************************/
static void Increment (
 Widget This
,XEvent* event
,String*   params
,Cardinal* nparams
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    double inc = ValueInc((XoRheostatWidget)This);
    Boolean snap = False;
    double v;
/*.........................................................................*/
    /*
     * Figure out increment from parameter:
     */
    v = athis.value;
    if (*nparams != 1) {	/* default: step to nearest interval: */
	v += inc;
	snap = True;
    } else {
	if (!strcmp(params[0],"max")) v = athis.maximum_value;
	else if (!strcmp(params[0],"min")) v = athis.minimum_value;
	else if (!strcmp(params[0],"+1i")) { v += inc; snap = True; }
	else if (!strcmp(params[0],"-1i")) { v -= inc; snap = True; }
	else v += atof(params[0]);
    }
    if (snap) v = (int)(v / inc + 0.5) * inc;

    /*
     * Bounds-check:
     */
    if (v > athis.maximum_value) v = athis.maximum_value;
    if (v < athis.minimum_value) v = athis.minimum_value;

    DrawArrow         (This,athis.eraser_GC);
    athis.value        = (int)v;
    CalculatePosition (This);
    DrawArrow         (This,athis.arrow_GC);

    CallCallbacks     (This,XoNsetCallback,event);
}

/***************************************************************************/
static void Notify (
 Widget This
,XEvent* event
,String*   a_params
,Cardinal* a_paramn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CallCallbacks(This, XoNnotifyCallback, event);
  a_params = NULL;
  a_paramn = 0;
}

/***********************************************************************
 *
 * Utility routines:
 *
 ***********************************************************************/
/***************************************************************************/
static void CallCallbacks (
 Widget This
,String  callback_name
,XEvent* event
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XoRheostatCallbackStruct cb;
/*.........................................................................*/
  cb.reason = 0;		/* this is never used, even under MOTIF */
  cb.event  = event;
  cb.value  = athis.value;
  XtCallCallbacks(This, callback_name, (XtPointer)&cb);
}


/***************************************************************************/
static void DrawArrow (
 Widget This
,GC gc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XoRheostatDrawArrow(XtDisplay(This), XtWindow(This), gc,
		  athis.tip_x, 
		  athis.tip_y,
		  (int)(athis.tip_x) - CenterX((XoRheostatWidget)This), 
		  (int)(athis.tip_y) - CenterY((XoRheostatWidget)This),
		  athis.outer_arrow_length,
		  athis.inner_arrow_length,
		  athis.arrow_width,
		  athis.fill_arrow);
}
/***************************************************************************/
static void DrawDial (
 Widget This
,GC gc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int radius  = athis.radius
    + athis.inner_margin
      + ((int)athis.dial_thickness+1) / 2;
/*.........................................................................*/
  XDrawArc(XtDisplay(This), XtWindow(This), athis.dial_GC,
	   CenterX((XoRheostatWidget)This) - radius,
	   CenterY((XoRheostatWidget)This) - radius,
	   (unsigned int)2*radius,
	   (unsigned int)2*radius,
	   64 * ((270 - athis.minimum_angle + 360) % 360),
	   64 * - AngleRange((XoRheostatWidget)This)
	   );
  gc = NULL;
}
/***************************************************************************/
static void DrawTicks (
 Widget This
,GC gc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i,cx,cy;
  double theta,inc;
  double ro,ri;       /* inner & outer radii of ticks */
/*.........................................................................*/
  if (!athis.number_intervals) return;

    /*
     * %%% should check if minimum_angle == maximum_angle (mod 360),
     * so the last tick doesn't coincide with the first.
     */
  inc = RADIANS((double)AngleRange((XoRheostatWidget)This))/(double)(athis.number_intervals);
  ri = (double)athis.radius + (double)athis.inner_margin + ((double)athis.dial_thickness+1.)/2.;
  ro = ri +  (double)athis.tick_length + ((double)athis.dial_thickness+1.)/2.;
  cx = CenterX((XoRheostatWidget)This);
  cy = CenterY((XoRheostatWidget)This);

    /*
     * Draw segments:
     */
  theta = RADIANS((double)MinAngle((XoRheostatWidget)This));
  i = athis.number_intervals + 1;
  while (i--) {
    double c = cos(theta);
    double s = sin(theta);
    
    XDrawLine(XtDisplay(This),XtWindow(This),gc,
	      cx - (int)(ro * s), cy + (int)(ro * c),
	      cx - (int)(ri * s), cy + (int)(ri * c));
    theta += inc;
  }
}
/***************************************************************************/
static void CalculatePosition (
  Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    double theta,length;
/*.........................................................................*/
    length = (double)athis.radius;
    /*
     * Calculate angle: theta = V*(maxTheta-minTheta) / (maxV - minV) + minTheta
     */
    theta = (double)(athis.value)
          * (double)(AngleRange((XoRheostatWidget)This))
          / (double)(ValueRange((XoRheostatWidget)This))
          + (double)(MinAngle((XoRheostatWidget)This));
    theta = RADIANS(theta);

    athis.tip_x = (Position)(  CenterX((XoRheostatWidget)This) - (int)(length * sin(theta)));
    athis.tip_y = (Position)(  CenterY((XoRheostatWidget)This) + (int)(length * cos(theta)));
}

/***************************************************************************/
static void GetGCs (
  Widget This
)
/***************************************************************************/
/*
 * GetGCs
 * foreground & background. GCs.
 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    XGCValues       values;
    XtGCMask        mask;
/*.........................................................................*/

    /*
     * dial:
     */
    mask = GCForeground | GCBackground | GCLineWidth | GCCapStyle;
    values.foreground = athis.dial_pixel;
    values.background = This->core.background_pixel;
    values.line_width = athis.dial_thickness;
    values.cap_style = CapRound;
    athis.dial_GC = XtGetGC(This, mask, &values);

    /*
     * tick marks:
     */
    mask = GCForeground | GCBackground | GCFunction | GCLineWidth | GCCapStyle;
    values.foreground = athis.tick_pixel;
    values.background = This->core.background_pixel;
    values.function = GXcopy;
    values.line_width = athis.tick_thickness;
    values.cap_style = CapRound;
    athis.tick_GC = XtGetGC(This, mask, &values);

    /*
     * Arrow:
     */
    mask = GCForeground | GCBackground | GCFunction | GCLineWidth
	 | GCCapStyle | GCFillStyle;
    values.foreground = athis.arrow_pixel;
    values.background = This->core.background_pixel;
    values.line_width = athis.arrow_thickness;
    values.cap_style = CapRound;
    values.fill_style = FillSolid;
    athis.arrow_GC = XtGetGC(This, mask, &values);

    /*
     * Eraser (identical to Arrow except fg & bg pixels are swapped):
     */
    values.foreground = This->core.background_pixel;
    values.background = athis.arrow_pixel;
    athis.eraser_GC = XtGetGC(This, mask, &values);

    return;
}
/***************************************************************************/
static void FreeGCs (
  Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
    XtReleaseGC(This,athis.arrow_GC);
    XtReleaseGC(This,athis.eraser_GC);
    XtReleaseGC(This,athis.dial_GC);
    XtReleaseGC(This,athis.tick_GC);
    return;
}
/***************************************************************************/
void XoRheostatDrawArrow (
 Display* dpy
,Drawable d
,GC gc
,Position endx
,Position endy	
,int dx
,int dy		
,Dimension outer_length
,Dimension inner_length
,Dimension width	
,Boolean fill	
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
    XPoint points[5];
    float scalef = sqrt((double)(dx*dx+dy*dy)); /* normalization factor */
    int al = (int)outer_length,
	bl = (int)inner_length,
	aw = (int)width / 2,
	lx = (int)(al *  dx / scalef),	/* distance from tip to base */
	ly = (int)(al *  dy / scalef),
	mx = (int)(bl *  dx / scalef),	/* distance from tip to middle point */
	my = (int)(bl *  dy / scalef),
	wx = (int)(aw * -dy / scalef),	/* distance from base to outer points */
	wy = (int)(aw *  dx / scalef);
/*.........................................................................*/

    points[0].x = endx;  	points[0].y = endy;
    points[1].x = endx-lx + wx;	points[1].y = endy-ly + wy;
    points[2].x = endx-mx;	points[2].y = endy-my;
    points[3].x = endx-lx - wx;	points[3].y = endy-ly - wy;
    points[4].x = endx; 	points[4].y = endy;

    if (fill)
	XFillPolygon(dpy,d,gc,
		points,5,
		inner_length <= outer_length ? Nonconvex : Convex,
		CoordModeOrigin);
    else
	XDrawLines(dpy,d,gc,points,5,CoordModeOrigin);
}
