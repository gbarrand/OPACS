/* $XConsortium: Label.c,v 1.92 91/07/24 16:56:28 converse Exp $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/*#define HAS_XMU*/
/*
 * Label.c - Label widget
 *
 */

#include <stdio.h>
#include <ctype.h>

/*Barrand : index is not ANSI.*/
#include <string.h>
/*index -> strchr*/

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

/*Label.h*/
#define XawTextEncoding8bit 0
#define XawTextEncodingChar2b 1
#define XtNleftBitmap "leftBitmap"
#define XtCLeftBitmap "LeftBitmap"
#define XtNencoding "encoding"
#define XtCEncoding "Encoding"

#ifdef HAS_XMU
#include <Xmu/Converters.h>
#include <Xmu/Drawing.h>
#else /*HAS_XMU*/
typedef enum {
    XtJustifyLeft,       /* justify text to left side of button   */
    XtJustifyCenter,     /* justify text in center of button      */
    XtJustifyRight       /* justify text to right side of button  */
} XtJustify;
#ifdef __cplusplus
extern "C"{
#endif
static void   XmuCvtStringToJustify    (XrmValuePtr,Cardinal*,XrmValuePtr,XrmValuePtr);
static Pixmap XmuCreateStippledPixmap  (Screen*,Pixel,Pixel,unsigned int);
static void   XmuReleaseStippledPixmap (Screen*,Pixmap);
static void   XmuCopyISOLatin1Lowered  (char* dst,char *src);
#ifdef __cplusplus
}
#endif
#endif /*HAS_XMU*/

#include <X11/IntrinsicP.h>
#include <XoLabelP.h>

#define streq(a,b) (strcmp( (a), (b) ) == 0)

#define MULTI_LINE_LABEL 32767

#ifdef CRAY
#define WORD64
#endif

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffsetOf(XoLabelRec, field)
static XtResource resources[] = {
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(label.foreground), XtRString, XtDefaultForeground},
    {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(label.font),XtRString, XtDefaultFont},
    {XtNlabel,  XtCLabel, XtRString, sizeof(String),
	offset(label.label), XtRString, NULL},
    {XtNencoding, XtCEncoding, XtRUnsignedChar, sizeof(unsigned char),
	offset(label.encoding), XtRImmediate, (XtPointer)XawTextEncoding8bit},
    {XtNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),
	offset(label.justify), XtRImmediate, (XtPointer)XtJustifyCenter},
    {XtNinternalWidth, XtCWidth, XtRDimension,  sizeof(Dimension),
	offset(label.internal_width), XtRImmediate, (XtPointer)4},
    {XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
	offset(label.internal_height), XtRImmediate, (XtPointer)2},
    {XtNleftBitmap, XtCLeftBitmap, XtRBitmap, sizeof(Pixmap),
       offset(label.left_bitmap), XtRImmediate, (XtPointer) None},
    {XtNbitmap, XtCPixmap, XtRBitmap, sizeof(Pixmap),
	offset(label.pixmap), XtRImmediate, (XtPointer)None},
    {XtNresize, XtCResize, XtRBoolean, sizeof(Boolean),
	offset(label.resize), XtRImmediate, (XtPointer)True},
    {XtNdoubleBuffer, XtCDoubleBuffer, XtRBoolean, sizeof(Boolean),
	offset(label.double_buffer), XtRImmediate, (XtPointer)False}
};
#undef offset

#ifdef __cplusplus
extern "C"{
#endif
static void Initialize (Widget,Widget);
static void Resize (Widget);
static void Redisplay (Widget,XEvent*,Region);
static Boolean SetValues (Widget,Widget,Widget,ArgList,Cardinal*);
static void ClassInitialize(void);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry (Widget w,XtWidgetGeometry* intended,XtWidgetGeometry* preferred);

static void SetTextWidthAndHeight (XoLabelWidget);
static void GetbackgroundGC (XoLabelWidget);
static void GetnormalGC (XoLabelWidget);
static void GetgrayGC (XoLabelWidget);
static void compute_bitmap_offsets (XoLabelWidget);
static void set_bitmap_info (XoLabelWidget);
static void _Reposition (XoLabelWidget lw,Dimension width,Dimension height,Position *dx,Position *dy);
#ifdef __cplusplus
}
#endif

XoLabelClassRec xoLabelClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) (&coreClassRec),
    /* class_name	  	*/	"XoLabel",
    /* widget_size	  	*/	sizeof(XoLabelRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	(XtInitProc)Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	XtInheritRealize,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* resize		  	*/	Resize,
    /* expose		  	*/	Redisplay,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  }
};
WidgetClass xoLabelWidgetClass = (WidgetClass)&xoLabelClassRec;
/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void ClassInitialize(void)
{
    XtAddConverter( XtRString, XtRJustify, XmuCvtStringToJustify, NULL, 0 );
}

#ifndef WORD64

#define TXT16 XChar2b

#else

#define TXT16 char

static XChar2b *buf2b;
static int buf2blen = 0;

_XawLabelWidth16(fs, str, n)
    XFontStruct *fs;
    char *str;
    int	n;
{
    int i;
    XChar2b *ptr;

    if (n > buf2blen) {
	buf2b = (XChar2b *)XtRealloc((char *)buf2b, n * sizeof(XChar2b));
	buf2blen = n;
    }
    for (ptr = buf2b, i = n; --i >= 0; ptr++) {
	ptr->byte1 = *str++;
	ptr->byte2 = *str++;
    }
    return XTextWidth16(fs, buf2b, n);
}

_XawLabelDraw16(dpy, d, gc, x, y, str, n)
    Display *dpy;
    Drawable d;
    GC gc;
    int x, y;
    char *str;
    int n;
{
    int i;
    XChar2b *ptr;

    if (n > buf2blen) {
	buf2b = (XChar2b *)XtRealloc((char *)buf2b, n * sizeof(XChar2b));
	buf2blen = n;
    }
    for (ptr = buf2b, i = n; --i >= 0; ptr++) {
	ptr->byte1 = *str++;
	ptr->byte2 = *str++;
    }
    XDrawString16(dpy, d, gc, x, y, buf2b, n);
}

#define XTextWidth16 _XawLabelWidth16
#define XDrawString16 _XawLabelDraw16

#endif /* WORD64 */

/*
 * Calculate width and height of displayed text in pixels
 */

static void SetTextWidthAndHeight (XoLabelWidget lw)
{
     XFontStruct	*fs = lw->label.font;
    char *nl;

    if (lw->label.pixmap != None) {
	Window root;
	int x, y;
	unsigned int width, height, bw, depth;
	if (XGetGeometry(XtDisplay(lw), lw->label.pixmap, &root, &x, &y,
			 &width, &height, &bw, &depth)) {
	    lw->label.label_height = height;
	    lw->label.label_width = width;
	    lw->label.label_len = depth;
	    return;
	}
    }

    lw->label.label_height = fs->max_bounds.ascent + fs->max_bounds.descent;
    if (lw->label.label == NULL) {
	lw->label.label_len = 0;
	lw->label.label_width = 0;
    }
    else if ((nl = strchr(lw->label.label, '\n')) != NULL) {
	char *label;
	lw->label.label_len = MULTI_LINE_LABEL;
	lw->label.label_width = 0;
	for (label = lw->label.label; nl != NULL; nl = strchr(label, '\n')) {
	    int width;

	    if (lw->label.encoding)
		width = XTextWidth16(fs, (TXT16*)label, (int)(nl - label)/2);
	    else
		width = XTextWidth(fs, label, (int)(nl - label));
	    if (width > (int)lw->label.label_width)
		lw->label.label_width = width;
	    label = nl + 1;
	    if (*label)
		lw->label.label_height +=
		    fs->max_bounds.ascent + fs->max_bounds.descent;
	}
	if (*label) {
	    int width = XTextWidth(fs, label, strlen(label));

	    if (lw->label.encoding)
		width = XTextWidth16(fs, (TXT16*)label, (int)strlen(label)/2);
	    else
		width = XTextWidth(fs, label, strlen(label));
	    if (width > (int) lw->label.label_width)
		lw->label.label_width = width;
	}
    } else {
	lw->label.label_len = strlen(lw->label.label);
	if (lw->label.encoding)
	    lw->label.label_width =
		XTextWidth16(fs, (TXT16*)lw->label.label,
			     (int) lw->label.label_len/2);
	else
	    lw->label.label_width =
		XTextWidth(fs, lw->label.label, (int) lw->label.label_len);
    }
}

static void GetbackgroundGC (XoLabelWidget lw)
{
    XGCValues	values;

    values.foreground	      = lw->core.background_pixel;
    values.graphics_exposures = False;

    lw->label.background_GC = XtGetGC(
	(Widget)lw,
	(unsigned) GCForeground | GCGraphicsExposures,
	&values);
}

static void GetnormalGC (XoLabelWidget lw)
{
    XGCValues	values;

    values.foreground	= lw->label.foreground;
    values.background	= lw->core.background_pixel;
    values.font		= lw->label.font->fid;
    values.graphics_exposures = False;

    lw->label.normal_GC = XtGetGC(
	(Widget)lw,
	(unsigned) GCForeground | GCBackground | GCFont | GCGraphicsExposures,
	&values);
}

static void GetgrayGC (XoLabelWidget lw)
{
    XGCValues	values;

    values.foreground = lw->label.foreground;
    values.background = lw->core.background_pixel;
    values.font	      = lw->label.font->fid;
    values.fill_style = FillTiled;
    values.tile       = XmuCreateStippledPixmap(XtScreen((Widget)lw),
						lw->label.foreground, 
						lw->core.background_pixel,
						lw->core.depth);
    values.graphics_exposures = False;

    lw->label.stipple = values.tile;
    lw->label.gray_GC = XtGetGC((Widget)lw, 
				(unsigned) GCForeground | GCBackground |
					   GCFont | GCTile | GCFillStyle |
					   GCGraphicsExposures,
				&values);
}

static void compute_bitmap_offsets (XoLabelWidget lw)
{
    /*
     * label will be displayed at (internal_width, internal_height + lbm_y)
     */
    if (lw->label.lbm_height != 0) {
	lw->label.lbm_y = (((int) lw->core.height) -
			   ((int) lw->label.internal_height * 2) -
			   ((int) lw->label.lbm_height)) / 2;
    } else {
	lw->label.lbm_y = 0;
    }
}


static void set_bitmap_info (XoLabelWidget lw)
{
    Window root;
    int x, y;
    unsigned int bw, depth;

    if (!(lw->label.left_bitmap &&
	  XGetGeometry (XtDisplay(lw), lw->label.left_bitmap, &root, &x, &y,
			&lw->label.lbm_width, &lw->label.lbm_height,
			&bw, &depth))) {
	lw->label.lbm_width = lw->label.lbm_height = 0;
    }
    compute_bitmap_offsets (lw);
}



/* ARGSUSED */
static void Initialize (Widget request,Widget a_new)
{
    XoLabelWidget lw = (XoLabelWidget) a_new;

    if (lw->label.label == NULL) 
        lw->label.label = (char*)XtNewString(lw->core.name);
    else {
        lw->label.label = (char*)XtNewString(lw->label.label);
    }

    GetbackgroundGC(lw);
    GetnormalGC(lw);
    GetgrayGC(lw);

    SetTextWidthAndHeight(lw);

    if (lw->core.height == 0)
        lw->core.height = lw->label.label_height + 2*lw->label.internal_height;

    set_bitmap_info (lw);		/* need core.height */

    if (lw->core.width == 0)		/* need label.lbm_width */
        lw->core.width = (lw->label.label_width + 2 * lw->label.internal_width
			  + LEFT_OFFSET(lw));

    lw->label.label_x = lw->label.label_y = 0;
    (*XtClass(a_new)->core_class.resize) ((Widget)lw);

    lw->label.pixmap_buffer = 0L;

    request = NULL;
} /* Initialize */

/*
 * Repaint the widget window
 */

/* ARGSUSED */
static void Redisplay (Widget w,XEvent *event,Region region)
{
   XoLabelWidget lw = (XoLabelWidget) w;
   GC gc;
   Drawable drawable;

   if (region != NULL) {
       int x = lw->label.label_x;
       unsigned int width = lw->label.label_width;
       if (lw->label.lbm_width) {
	   if (lw->label.label_x > (x = lw->label.internal_width))
	       width += lw->label.label_x - x;
       }
       if (XRectInRegion(region, x, lw->label.label_y,
			 width, lw->label.label_height) == RectangleOut)
	   return;
   }

   gc = XtIsSensitive((Widget)lw) ? lw->label.normal_GC : lw->label.gray_GC;

   if(lw->label.double_buffer)
     {
       if(lw->label.pixmap_buffer==0L)
         {
	   lw->label.pixmap_buffer        = XCreatePixmap  (XtDisplay(w),
							    XDefaultRootWindow(XtDisplay(w)),
							    lw->core.width,lw->core.height,lw->core.depth);
	   if(!lw->label.pixmap_buffer)
	     {
	       lw->label.pixmap_buffer = 0L;
	       drawable                = XtWindow(w);
	     }
	   else
	     {
	       lw->label.pixmap_width  = lw->core.width;
	       lw->label.pixmap_height = lw->core.height;
	       drawable                = lw->label.pixmap_buffer;
	       XFillRectangle          (XtDisplay(w),drawable,lw->label.background_GC,0,0,lw->core.width,lw->core.height);
	     }
	 }
       else
	 {
	   if( (lw->core.width!=lw->label.pixmap_width)  || (lw->core.height!=lw->label.pixmap_height) ) 
	     {
	       XFreePixmap                    (XtDisplay(w), lw->label.pixmap_buffer);
	       lw->label.pixmap_buffer        = XCreatePixmap  (XtDisplay(w),
								XDefaultRootWindow(XtDisplay(w)),
								lw->core.width,lw->core.height,lw->core.depth);
	       if(!lw->label.pixmap_buffer)
		 {
		   lw->label.pixmap_buffer = 0L;
		   drawable                = XtWindow(w);
		 }
	       else
		 {
		   lw->label.pixmap_width  = lw->core.width;
		   lw->label.pixmap_height = lw->core.height;
		   drawable                = lw->label.pixmap_buffer;
		   XFillRectangle          (XtDisplay(w),drawable,lw->label.background_GC,0,0,lw->core.width,lw->core.height);
		 }
	     }
	   else
	     {
	       drawable       = lw->label.pixmap_buffer;
	       XFillRectangle (XtDisplay(w),drawable,lw->label.background_GC,0,0,lw->core.width,lw->core.height);
	     }
	 }
     }
   else
     {
       drawable = XtWindow(w);
     }

#ifdef notdef
   if (region != NULL) XSetRegion(XtDisplay(w), gc, region);
#endif /*notdef*/
   if (lw->label.pixmap == None) {
       int len = lw->label.label_len;
       char *label = lw->label.label;
       Position y = lw->label.label_y + lw->label.font->max_bounds.ascent;

       /* display left bitmap */
       if (lw->label.left_bitmap && lw->label.lbm_width != 0) {
	   XCopyPlane (XtDisplay(w), lw->label.left_bitmap, drawable, gc,
		       0, 0, lw->label.lbm_width, lw->label.lbm_height,
		       (int) lw->label.internal_width, 
		       (int) lw->label.internal_height + lw->label.lbm_y, 
		       (unsigned long) 1L);
       }

       if (len == MULTI_LINE_LABEL) {
	   char *nl;
	   while ((nl = strchr(label, '\n')) != NULL) {
	       if (lw->label.encoding)
		   XDrawString16(XtDisplay(w), drawable, gc,
				 lw->label.label_x, y,
				 (TXT16*)label, (int)(nl - label)/2);
	       else
		   XDrawString(XtDisplay(w), drawable, gc,
			       lw->label.label_x, y, label, (int)(nl - label));
	       y += lw->label.font->max_bounds.ascent + lw->label.font->max_bounds.descent;
	       label = nl + 1;
	   }
	   len = strlen(label);
       }
       if (len) {
	   if (lw->label.encoding)
	       XDrawString16(XtDisplay(w), drawable, gc,
			     lw->label.label_x, y, (TXT16*)label, len/2);
	   else
	       XDrawString(XtDisplay(w), drawable, gc,
			   lw->label.label_x, y, label, len);
       }
   } else if (lw->label.label_len == 1) { /* depth */
       XCopyPlane(XtDisplay(w), lw->label.pixmap, drawable, gc,
		  0, 0, lw->label.label_width, lw->label.label_height,
		  lw->label.label_x, lw->label.label_y, 1L);
   } else {
       XCopyArea(XtDisplay(w), lw->label.pixmap, drawable, gc,
		 0, 0, lw->label.label_width, lw->label.label_height,
		 lw->label.label_x, lw->label.label_y);
   }
#ifdef notdef
   if (region != NULL) XSetClipMask(XtDisplay(w), gc, (Pixmap)None);
#endif /* notdef */
   if(lw->label.pixmap_buffer!=0L)
     {
       XCopyArea (XtDisplay(w),lw->label.pixmap_buffer,XtWindow(w),gc,0,0,lw->core.width,lw->core.height,0,0);
     }
   event = NULL;
}

static void _Reposition (XoLabelWidget lw,Dimension width,Dimension height,Position *dx,Position *dy)
{
    Position newPos;
    Position leftedge = lw->label.internal_width + LEFT_OFFSET(lw);

    switch (lw->label.justify) {

	case XtJustifyLeft   :
	    newPos = leftedge;
	    break;

	case XtJustifyRight  :
	    newPos = width -
		(lw->label.label_width + lw->label.internal_width);
	    break;

	case XtJustifyCenter :
	default:
	    newPos = (int)(width - lw->label.label_width) / 2;
	    break;
    }
    if (newPos < (Position)leftedge)
	newPos = leftedge;
    *dx = newPos - lw->label.label_x;
    lw->label.label_x = newPos;
    *dy = (newPos = (int)(height - lw->label.label_height) / 2)
	  - lw->label.label_y;
    lw->label.label_y = newPos;
    return;
}

static void Resize (Widget w)
{
    XoLabelWidget lw = (XoLabelWidget)w;
    Position dx, dy;
    _Reposition(lw, w->core.width, w->core.height, &dx, &dy);
    compute_bitmap_offsets (lw);
}

/*
 * Set specified arguments into widget
 */

#define PIXMAP 0
#define WIDTH 1
#define HEIGHT 2
#define NUM_CHECKS 3

static Boolean SetValues (Widget current,Widget request,Widget a_new,ArgList args,Cardinal *num_args)
{
    XoLabelWidget curlw = (XoLabelWidget) current;
    XoLabelWidget reqlw = (XoLabelWidget) request;
    XoLabelWidget newlw = (XoLabelWidget) a_new;
    unsigned int i;
    Boolean was_resized = False, redisplay = False, checks[NUM_CHECKS];

    for (i = 0; i < NUM_CHECKS; i++)
	checks[i] = FALSE;

    for (i = 0; i < *num_args; i++) {
	if (streq(XtNbitmap, args[i].name))
	    checks[PIXMAP] = TRUE;
	if (streq(XtNwidth, args[i].name))
	    checks[WIDTH] = TRUE;
	if (streq(XtNheight, args[i].name))
	    checks[HEIGHT] = TRUE;
    }

    if (newlw->label.label == NULL) {
	newlw->label.label = newlw->core.name;
    }

    /*
     * resize on bitmap change
     */
    if (curlw->label.left_bitmap != newlw->label.left_bitmap) {
	was_resized = True;
    }

    if (curlw->label.encoding != newlw->label.encoding)
	was_resized = True;

    if (curlw->label.label != newlw->label.label) {
        if (curlw->label.label != curlw->core.name)
	  {
	    XtFree( (char *)curlw->label.label );
	    curlw->label.label = NULL;
	  }

	if (newlw->label.label != newlw->core.name) {
	    newlw->label.label = (char*)XtNewString( newlw->label.label );
	}
	was_resized = True;
    }

    if (was_resized || (curlw->label.font != newlw->label.font) ||
	(curlw->label.justify != newlw->label.justify) || checks[PIXMAP]) {

	SetTextWidthAndHeight(newlw);
	was_resized = True;
    }

    /* recalculate the window size if something has changed. */
    if (newlw->label.resize && was_resized) {
	if ((curlw->core.height == reqlw->core.height) && !checks[HEIGHT])
	    newlw->core.height = (newlw->label.label_height + 
				  2 * newlw->label.internal_height);

	set_bitmap_info (newlw);

	if ((curlw->core.width == reqlw->core.width) && !checks[WIDTH])
	    newlw->core.width = (newlw->label.label_width +
				 LEFT_OFFSET(newlw) +
				 2 * newlw->label.internal_width);
    }

    if (curlw->label.foreground != newlw->label.foreground
	|| curlw->core.background_pixel != newlw->core.background_pixel
	|| curlw->label.font->fid != newlw->label.font->fid) {

	XtReleaseGC(a_new, curlw->label.background_GC);
	XtReleaseGC(a_new, curlw->label.normal_GC);
	XtReleaseGC(a_new, curlw->label.gray_GC);
	XmuReleaseStippledPixmap( XtScreen(current), curlw->label.stipple );
	GetbackgroundGC(newlw);
	GetnormalGC(newlw);
	GetgrayGC(newlw);
	redisplay = True;
    }

    if ((curlw->label.internal_width != newlw->label.internal_width)
        || (curlw->label.internal_height != newlw->label.internal_height)
	|| was_resized) {
	/* Resize() will be called if geometry changes succeed */
	Position dx, dy;
	_Reposition(newlw, curlw->core.width, curlw->core.height, &dx, &dy);
    }

   if(newlw->label.double_buffer)
     {
       return False; /*return True involve an XClearArea (Xt/SetValues.c)*/
     }
   else
     {
       return was_resized || redisplay ||
	   XtIsSensitive(current) != XtIsSensitive(a_new);
     }
}

static void Destroy (Widget w)
{
    XoLabelWidget lw = (XoLabelWidget)w;

    if(lw->label.pixmap_buffer!=0L) XFreePixmap (XtDisplay(w),lw->label.pixmap_buffer);
    XtFree     ( lw->label.label );
    lw->label.label = NULL;
    XtReleaseGC( w, lw->label.background_GC );
    XtReleaseGC( w, lw->label.normal_GC );
    XtReleaseGC( w, lw->label.gray_GC);
    XmuReleaseStippledPixmap( XtScreen(w), lw->label.stipple );
}


static XtGeometryResult QueryGeometry (Widget w,XtWidgetGeometry* intended,XtWidgetGeometry* preferred)
{
     XoLabelWidget lw = (XoLabelWidget)w;

    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = (lw->label.label_width + 2 * lw->label.internal_width +
			LEFT_OFFSET(lw));
    preferred->height = lw->label.label_height + 2*lw->label.internal_height;
    if (  ((intended->request_mode & (CWWidth | CWHeight))
	   	== (CWWidth | CWHeight)) &&
	  intended->width == preferred->width &&
	  intended->height == preferred->height)
	return XtGeometryYes;
    else if (preferred->width == w->core.width &&
	     preferred->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
}
/***************************************************************************/
void XoLabelDraw (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  Redisplay (This,NULL,NULL);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
#ifdef HAS_XMU
#else /*HAS_XMU*/
#include <Xmu.ic>
#endif /*HAS_XMU*/
