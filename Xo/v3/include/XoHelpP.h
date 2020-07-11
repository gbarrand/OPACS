/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoHelpP_h
#define XoHelpP_h 

#include <XoHelp.h>

#ifndef XtInheritGeoMatrixCreate  /* for HP-UX MOTIF */
#define XtInheritGeoMatrixCreate ((XmGeoCreateProc)_XtInherit)
#endif

typedef struct
{
   void* extension;
} XoHelpClassPart;


typedef struct _XoHelpClassRec
{
	CoreClassPart		  core_class;
	CompositeClassPart	  composite_class;
        XoHelpClassPart           help_class;
} XoHelpClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XoHelpClassRec xoHelpClassRec;
#ifdef __cplusplus
}
#endif

typedef struct
{
        Widget           form,text,list;
        XtCallbackList   resize_callback;
        XtCallbackList   expose_callback;
} XoHelpPart;

typedef struct _XoHelpRec
{
	CorePart		core;
	CompositePart		composite;
  	XoHelpPart		help;
} XoHelpRec;

#endif /*XoHelpP_h*/
