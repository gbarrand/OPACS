/*
* $XConsortium: EyesP.h,v 1.9 90/12/01 13:04:43 rws Exp $
*/

#ifndef XoEyesP_h
#define XoEyesP_h

#include <XoEyes.h>

typedef struct _transform {
	double	mx, bx;
	double	my, by;
} Transform;

typedef struct _TPoint {
	double	x, y;
} TPoint;

typedef struct _TRectangle {
	double	x, y, width, height;
} TRectangle;

# define Xx(x,y,t)	((int)((t)->mx * (x) + (t)->bx + 0.5))
# define Xy(x,y,t)	((int)((t)->my * (y) + (t)->by + 0.5))
# define Xwidth(w,h,t)	((int)((t)->mx * (w) + 0.5))
# define Xheight(w,h,t)	((int)((t)->my * (h) + 0.5))
# define Tx(x,y,t)	((((double) (x)) - (t)->bx) / (t)->mx)
# define Ty(x,y,t)	((((double) (y)) - (t)->by) / (t)->my)
# define Twidth(w,h,t)	(((double) (w)) / (t)->mx)
# define Theight(w,h,t)	(((double) (h)) / (t)->my)

#define SEG_BUFF_SIZE		128

/* New fields for the eyes widget instance record */
typedef struct {
	 Pixel		puppixel;	/* foreground pixel */
	 Pixel		outline;	/* outline pixel */
	 Pixel		center;		/* inside pixel */
	 GC		outGC;		/* pointer to GraphicsContext */
	 GC		pupGC;		/* pointer to GraphicsContext */
	 GC		centerGC;	/* pointer to GraphicsContext */
	 GC		shapeGC;	/* pointer to GraphicsContext */
/* start of graph stuff */
	 int		backing_store;	/* backing store variety */
	 Boolean	reverse_video;	/* swap fg and bg pixels */
	 Boolean	shape_window;	/* use SetWindowShapeMask */
	 int		update;		/* current timeout index */
	 TPoint		mouse;		/* old mouse position */
	 TPoint		pupil[2];	/* pupil position */
	 Transform	t;
	 Transform	maskt;
	 XtIntervalId	interval_id;
	 Pixmap		shape_mask;	/* window shape */
   } EyesPart;

/* Full instance record declaration */
typedef struct _EyesRec {
   CorePart core;
   EyesPart eyes;
   } EyesRec;

/* New fields for the Eyes widget class record */
typedef struct {
  void* extension;
} EyesClassPart;

/* Full class record declaration. */
typedef struct _EyesClassRec {
   CoreClassPart core_class;
   EyesClassPart eyes_class;
   } EyesClassRec;

/* Class pointer. */
#ifdef __cplusplus
extern "C"{
#endif
extern EyesClassRec xoEyesClassRec;
#ifdef __cplusplus
}
#endif

#endif /*XoEyesP_h*/
