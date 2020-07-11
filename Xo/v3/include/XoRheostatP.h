/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*************************************************************
 *
 * XoRheostatP.h
 * Private header file for XoRheostat widget.
 *
 * Author: Joe English, joe@trystero.art.com
 *
 *************************************************************
 */

#ifndef XoRheostatP_h
#define XoRheostatP_h

#include <XoRheostat.h>

typedef struct _XoRheostatClassPart {
    void* extension;
} XoRheostatClassPart;

typedef struct _XoRheostatClassRec {
    CoreClassPart   		core_class;
#ifdef HAS_XM
    XmPrimitiveClassPart	primitive_class;
#endif
    XoRheostatClassPart   	rheostat_class;
} XoRheostatClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XoRheostatClassRec xoRheostatClassRec;
#ifdef __cplusplus
}
#endif

typedef struct _XoRheostatPart 
{
    int             	value;              	/* value */
    int             	minimum_value;      	/* minimum value */
    int             	maximum_value;      	/* maximum value */
    int             	minimum_angle;      	/* angle of minimum value */
    int             	maximum_angle;		/* angle of maximum value */
    int   	 	number_intervals;	/* #tick mark intervals */
    Boolean		tick_gravity;	/* click by tick sticks to tick? */
    Boolean		resize_arrow;	/* resize arrow? */

    XtCallbackList	set;            /* callbacks for Set() action */
    XtCallbackList	notify;         /* callbacks for Notify() */

    Dimension		outer_margin;
    Dimension		tick_length;
    Dimension		dial_thickness;
    Dimension		inner_margin;
    Dimension		radius;
    Dimension		outer_arrow_length;	/* from tip to outer points */
    Dimension		inner_arrow_length;	/* from tip to inner point */
    Dimension		arrow_width;		/* width of arrow */
    Boolean		fill_arrow;		/* filled/outlined arrow */
    Dimension		arrow_thickness;	/* line thickness if outlined */
    Dimension		tick_thickness;

#ifdef HAS_XM
    Boolean		use_shadow_colors;	/* for arrow & dial */
#endif
    Pixel		arrow_pixel;		/* arrow color */
    Pixel		tick_pixel;		/* ticks  */
    Pixel		dial_pixel;		/* circular border */

    /* Private: */
    GC          	dial_GC;            	/* assorted GCs */
    GC          	arrow_GC;		/* to draw pointer */
    GC          	eraser_GC;		/* to erase pointer */
    GC          	tick_GC;		/* to erase pointer */
    Position    	tip_x;              	/* coords of arrow tip */
    Position    	tip_y;

    Dimension		orig_radius;		/* Used to resize arrow */
    Dimension		orig_outer_length;	/*  if resize_arrow True */
    Dimension		orig_inner_length;
    Dimension		orig_width;

} XoRheostatPart;

typedef struct _XoRheostatRec {
    CorePart        	core;
#ifdef HAS_XM
    XmPrimitivePart	primitive;
#endif
    XoRheostatPart        rheostat;
} XoRheostatRec;

#endif  /*XoRheostatP_h*/
