/*************************************************************
 *
 * XoRheostat.h.
 * Public header file for XoRheostat widget.
 *
 * Author: Joe English, joe@trystero.art.com
 *
 *************************************************************
 */

#ifndef  XoRheostat_h
#define  XoRheostat_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _XoRheostatClassRec *XoRheostatWidgetClass;
typedef struct _XoRheostatRec      *XoRheostatWidget;

/*
 * Resources:
 * Angles are specified in degrees, going clockwise from straight down.
 * 0 = down, 90 = left, 180 = up, 270 = right, 360 = down again.
 *
 * The Dial value is an integer, with minimumValue <= value <= maximumValue
 *
 * Setting minimumAngle=minimumValue=0 and maximumAngle=maximumValue=360 
 * will configure the XoRheostat for specifying angles.
 *
 * If resizeArrow is True, then the arrow will be resized proportionally
 * when the XoRheostat is resized.
 *
 * If tickGravity is True, then clicking near a XoRheostat tick will snap
 * the indicator to that tick.
 *
 */ 

#define XoNvalue		"value"
#define XoNmaximumAngle		"maximumAngle"
#define XoNminimumAngle		"minimumAngle"
#define XoNmaximumValue		"maximumValue"
#define XoNminimumValue		"minimumValue"
#define XoNnumberIntervals	"numberIntervals"
#define XoNresizeArrow		"resizeArrow"
#define XoNtickGravity		"tickGravity"

/* 
 * Geometry-specifying resources (outside to inside):
 */

#define XoNouterMargin		"outerMargin"
#define XoNtickLength		"tickLength"
#define XoNdialThickness	"dialThickness"
#define XoNinnerMargin		"innerMargin"
#define XoNradius		"radius"

/*
 * Appearance resources:
 */
#define XoNdialColor		"dialColor"
#define XoNtickColor		"tickColor"
#define XoNarrowColor		"arrowColor"
#define XoNtickThickness	"tickThickness"

/*
 * Arrow appearance:
 */
#define XoNarrowWidth		"arrowWidth"
#define XoNinnerArrowLength	"innerArrowLength"
#define XoNouterArrowLength	"outerArrowLength"
#define XoNfillArrow		"fillArrow"
#define XoNarrowThickness	"arrowThickness"

/* if useShadowColors is True, then the Motif * topShadowColor and 
 * bottomShadowcolor resources will be used for dialColor and arrowColor.
 * Only valid in Motif version of XoRheostat.
 */
#define XoNuseShadowColors	"useShadowColors"


/*
 * Callbacks:
 */
#define XoNnotifyCallback	"notifyCallback"
#define XoNsetCallback		"setCallback"

#define XtCNumberIntervals	"NumberIntervals"
#define XtCGravity		"Gravity"
#define XtCMinimum		"Minimum"
#define XtCMaximum		"Maximum"

/*
 * XoRheostat callback structure:
 */
typedef struct {
    int 	reason;		/* for Motif compatibility only */
    XEvent 	*event;		/* Ditto */
    int		value;		/* current dial value */
} XoRheostatCallbackStruct;

/*
 * Convenience callback function:
 * 'closure' must be an 'int *', into which is stored the current dial value.
 */

#ifdef __cplusplus
extern "C"{
#endif
extern WidgetClass              xoRheostatWidgetClass;
void   XoRheostatDrawArrow      (Display* ,Drawable,GC,Position,Position,int,int,Dimension,Dimension,Dimension,Boolean);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoRheostat(0); /**/
#endif

#endif	/*XoRheostat_h*/
