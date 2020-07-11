/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoCameraP_h
#define XoCameraP_h 

#include <XoCamera.h>

typedef enum {
    OTransformationScale,
    OTransformationRotate,
    OTransformationTranslate
} OTransformation;

typedef enum {
    OMoveScene,
    OMoveCamera,
    OMoveHighLight
} OMove;

typedef enum {
    OAroundXY,
    OAroundZ,
    OAroundAxis
} OAround;

typedef struct
{
   void* extension;
} XoCameraClassPart;

typedef struct _XoCameraClassRec
{
	CoreClassPart		  core_class;
	CompositeClassPart	  composite_class;
        XoShapeClassPart           draw_class;
	XoCameraClassPart	  camera_class;
} XoCameraClassRec;

#ifdef __cplusplus
extern "C"{
#endif
extern XoCameraClassRec xoCameraClassRec;
#ifdef __cplusplus
}
#endif

typedef struct
{
        OCamera             ocamera;
        WidgetList          transformationSensitiveWidgets;
        Widget              referenceCamera;
        char*               viewLimits;
        char*               viewPoint;
        char*               upVector;
        char*               virtualColormap;
        int                 projectionAngle; /*in degree.*/
        OBuffering          buffering;
        ODepthTreatment     depthTreatment;
        OProjection         projection;
        OSpecialProjection  specialProjection;
        OTransformation     transformation;
        OMove               transformedObject;
        OAround             transformationAxis;
        int                 transformationFactor;
        XtCallbackList      transformCallback;
        XtCallbackList      helpCallback;
        XtCallbackList      receiveTransformCallback;
        XtCallbackList      collectCallback;
        int                 backgroundColor;
        char*               highlightContext;
        int                 edgeColor;
        Boolean             edgesVisible;
        int                 blackAndWhite;
/*For OpenGL.*/
        Boolean             installColormap;
	Visual*             visual;
	Cardinal            initialDepth;
	Colormap            initialColormap;
	GC                  rubberGC;
} XoCameraPart;

typedef struct _XoCameraRec
{
	CorePart		core;
	CompositePart		composite;
  	XoShapePart		draw;
  	XoCameraPart		camera;
} XoCameraRec;

#endif /*XoCameraP_h*/
