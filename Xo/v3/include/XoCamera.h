/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XoCamera_h
#define XoCamera_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

typedef struct _XoCameraClassRec *XoCameraWidgetClass;
typedef struct _XoCameraRec      *XoCameraWidget;

typedef struct
{
    int     reason;
    XEvent* event;
    Widget  widget;
} XoCameraCallbackStruct;

/* XoCamera */
#ifndef XoNvirtualColormap
#define XoNvirtualColormap                 "virtualColormap"
#endif
#ifndef XoNcollectCallback
#define XoNcollectCallback                 "collectCallback"
#endif
#ifndef XoNuserCallback
#define XoNuserCallback                    "userCallback"
#endif
#define XoNviewLimits                      "viewLimits"
#define XoNviewPoint                       "viewPoint"
#define XoNupVector                        "upVector"
#define XoNbuffering                       "buffering"
#define XoNdepthTreatment                  "depthTreatment"
#define XoNprojection                      "projection"
#define XoNprojectionAngle                 "projectionAngle"
#define XoNtransformation                  "transformation"
#define XoNtransformedObject               "transformedObject"
#define XoNtransformationAxis              "transformationAxis"
#define XoNtransformationFactor            "transformationFactor"
#define XoNtransformationSensitiveWidgets  "transformationSensitiveWidgets"
#define XoNhelpCallback                    "helpCallback"
#define XoNtransformCallback               "transformCallback"
#define XoNreceiveTransformCallback        "receiveTransformCallback"
#define XoNbackgroundColor                 "backgroundColor"
#define XoNhighlightContext                "highlightContext"
#define XoNedgeColor                       "edgeColor"
#define XoNedgesVisible                    "edgesVisible"
#define XoNblackAndWhite                   "blackAndWhite"
#define XoNreferenceCamera                 "referenceCamera"
#define XoNspecialProjection               "specialProjection"

#ifndef XoRColorIndex 
#define XoRColorIndex                     "OColorIndex"
#endif
#define XoRBuffering                      "OBuffering"
#define XoRDepthTreatment                 "ODepthTreatment"
#define XoRProjection                     "OProjection"
#define XoRTransformation                 "OTransformation"
#define XoRTransformedObject              "OMove"
#define XoRTransformationAxis             "OAround"
#define XoRSpecialProjection              "OSpecialProjection"

#define XoCR_COLLECT           1
#define XoCR_TRANSFORM         2
#define XoCR_RECEIVE_TRANSFORM 3

#include <CList.h>
#ifdef __cplusplus
extern "C"{
#endif
extern WidgetClass                 xoCameraWidgetClass;

char**  XoCameraGetResourceOptions  (char*,int*);
void    XoCameraClearClass          ();
void    XoCameraCancelGoRefresh     ();
void    XoCameraDisablePopup        ();
void    XoCameraEnablePopup         ();

void    XoCameraRefreshNow          (Widget);
void*   XoCameraGetCamera           (Widget);
void    XoCameraProducePS_File      (Widget,char*);
void    XoCameraProducePOV_File     (Widget,char*);
void    XoCameraProduceDAWN_File    (Widget,char*);
void    XoCameraPutSceneInPS_Stream (Widget);
void    XoCameraCenterMatrixOnPick  (Widget);
void    XoCameraBecomeTarget        (Widget);
void*   XoCameraGetPick             (Widget,int);
void    XoCameraSetVirtualColormap  (Widget,char*);
void    XoCameraAddPopupEntry       (Widget,char*,XtCallbackProc,XtPointer);
void    XoCameraAddPopupSeparator   (Widget,char*);
void    XoCameraRemovePopupEntry    (Widget,char*);
int     XoCameraCollectInRegion     (Widget,int,int,int,int,int);
CList   XoCameraGetHighlightedNodes (Widget);
CList   XoCameraGetNodes            (Widget);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoCamera(0); /**/
#endif

#ifdef __CI_SPECIFIC__
#include <XWidget.h> /*thisWidget, targetWidget, thisLocalTargetWidget*/

#define thisCamera                 (XoCameraGetCamera(thisWidget))
#define thisFirstPick              (XoCameraGetPick(thisWidget,0))
#define thisSecondPick             (XoCameraGetPick(thisWidget,1))

#define targetCamera               (XoCameraGetCamera(targetWidget))
#define targetFirstPick            (XoCameraGetPick(targetWidget,0))
#define targetSecondPick           (XoCameraGetPick(targetWidget,1))
#define targetHighlightedNodes     (XoCameraGetHighlightedNodes(targetWidget))

#define thisLocalTargetCamera           (XoCameraGetCamera(thisLocalTargetWidget))
#define thisLocalTargetFirstPick        (XoCameraGetPick(thisLocalTargetWidget,0))
#define thisLocalTargetSecondPick       (XoCameraGetPick(thisLocalTargetWidget,1))
#define thisLocalTargetHighlightedNodes (XoCameraGetHighlightedNodes(thisLocalTargetWidget))
#endif

#endif /*XoCamera_h*/
