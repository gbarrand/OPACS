/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoMosaicP_h
#define XoMosaicP_h 

#include <XoMosaic.h>

#ifndef XtInheritGeoMatrixCreate  /* for HP-UX MOTIF */
#define XtInheritGeoMatrixCreate ((XmGeoCreateProc) _XtInherit)
#endif

typedef struct
{
   void* extension;
} XoMosaicClassPart;

typedef struct _XoMosaicClassRec
{
	CoreClassPart		  core_class;
	CompositeClassPart	  composite_class;
        XoMosaicClassPart         mosaic_class;
} XoMosaicClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XoMosaicClassRec xoMosaicClassRec;
#ifdef __cplusplus
}
#endif

typedef struct
{
/*Resources.*/
        char*        homeDocument;
        Boolean      controlsVisible;
        XFontStruct* menuFont;
/*Others.*/
        Widget       html;
        Widget       tracker;
        int          binary_transfer;
        char*        currentURL;
	char*        currentText;
	int          refi;
	char*        refs[128];
} XoMosaicPart;

typedef struct _XoMosaicRec
{
	CorePart		core;
	CompositePart		composite;
  	XoMosaicPart		mosaic;
} XoMosaicRec;

#endif /*XoMosaicP_h*/
