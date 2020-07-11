/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoImageP_h
#define XoImageP_h 

#include <XoImage.h>

typedef struct
{
   void* extension;
} XoImageClassPart;

typedef struct _XoImageClassRec
{
	CoreClassPart		  core_class;
	CompositeClassPart	  composite_class;
        XoShapeClassPart           draw_class;
	XoImageClassPart	  image_class;
} XoImageClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XoImageClassRec xoImageClassRec;
#ifdef __cplusplus
}
#endif

typedef struct _ORectRecord
{
  int x,y,width,height;
} ORectRecord,*ORect;

typedef struct
{
/*Resources.*/
        char*            virtualColormap;
        XtCallbackList   collectCallback;
        char*            postScriptFile;
        char*            gifFile;
        char*            dataFile;
/*Image.*/
        GC               gc;
        OImage           oimage;
        OColormap        ocmap;
	OLUT             olut;
	int              iwidth,iheight;
	ORectRecord      part;
	int              tx,ty;
	double           scale;
	int              beg_x,beg_y,end_x,end_y;
        int              pixelPrecision;
/*Sub widgets.*/
        Widget           showPixelWidget;
/*Else*/
	GC               rubberGC;
} XoImagePart;


typedef struct _XoImageRec
{
	CorePart		core;
	CompositePart		composite;
  	XoShapePart		draw;
  	XoImagePart		image;
} XoImageRec;

#endif /*XoImageP_h*/
