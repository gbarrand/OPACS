/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoShapeP_h
#define XoShapeP_h 

#include <XoShape.h>

#ifndef XtInheritGeoMatrixCreate  /* for HP-UX MOTIF */
#define XtInheritGeoMatrixCreate ((XmGeoCreateProc) _XtInherit)
#endif

typedef struct
{
   void* extension;
} XoShapeClassPart;


typedef struct _XoShapeClassRec
{
	CoreClassPart		  core_class;
	CompositeClassPart	  composite_class;
        XoShapeClassPart           draw_class;
} XoShapeClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XoShapeClassRec xoShapeClassRec;
#ifdef __cplusplus
}
#endif

typedef struct
{
        int              shapeModified;
        XShape           windowShape;
        XtCallbackList   resize_callback;
        XtCallbackList   expose_callback;
        XtCallbackList   user_callback;
} XoShapePart;

typedef struct _XoShapeRec
{
	CorePart	 core;
	CompositePart	 composite;
  	XoShapePart	 draw;
} XoShapeRec;

#endif /*XoShapeP_h*/
