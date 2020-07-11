/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef ONode_h
#define ONode_h

typedef struct _ONodeRecord *ONode;

typedef void(*ONodeVoidFunction)();
typedef int(*ONodeIsValidFunction)(void*);
typedef void(*ONodeRefreshFunction)(void*);

#include <OPrimitive.h>
/*For shapes.*/
typedef OPrimitive*(*ONodeRepresentShapeFunction)(ONode);
typedef void(*ONodeDeleteShapeFunction)(ONode);

#define OCollectAll                          0
#define OCollectHighlighted                  1
#define OCollectNoHighlighted                2
#define OCollectMatchingName                 3
#define OCollectHighlightedAndMatchingName   4
#define OCollectFirstNodeMatchingColorIndex  5

#ifndef NO_C_H
#include <stdarg.h>
#endif

#include <CList.h>
#include <OMatrix.h>
#ifdef __cplusplus
extern "C"{
#endif
void        ONodeClearClass               ();
ONode*      ONodeGetIdentifiers           ();
void        ONodeSetBlinkFunctions        (ONodeVoidFunction,ONodeVoidFunction);
int         ONodeGetNumberOfBlinks        ();
void        ONodeSetNumberOfBlinksToZero  ();
ONode       ONodeGetIdentifier            (char*);
void        ONodeInitialize               (ONode);
ONode       ONodeMake                     ();
ONode       ONodeCreate                   (char*);
ONode       ONodeCreateF                  (int,char*,...);
void        ONodeSetViewFunctions         (ONodeVoidFunction,ONodeIsValidFunction,ONodeRefreshFunction,ONodeRefreshFunction);
void        ONodeRefreshViews             ();
void        ONodeRequireViewRefreshment   (void*);
void        ONodeRequireViewDrawing       (void*);

int         ONodeIsValid                  (ONode);
void        ONodeDelete                   (ONode);
ONode       ONodeDuplicate                (ONode,char*);
char*       ONodeGetName                  (ONode);
void        ONodeSetName                  (ONode,char*);
int         ONodeGetHighlight             (ONode);
void        ONodeSetHighlight             (ONode,int);
void        ONodeToggleHighlight          (ONode);
OMatrix     ONodeGetMatrix                (ONode);
void        ONodeSetMatrix                (ONode,OMatrix);
void        ONodeAssignMatrixToIdentity   (ONode);
void        ONodeMultiplyMatrix           (ONode,OMatrix);
int         ONodeGetVisibility            (ONode);
void        ONodeSetVisibility            (ONode,int);
int         ONodeGetPickability           (ONode);
void        ONodeSetPickability           (ONode,int);
int         ONodeGetTouched               (ONode);
void        ONodeResetTouched             (ONode);
int         ONodeGetProjected             (ONode);
void        ONodeSetProjected             (ONode,int);
int         ONodeGetCameraAligned         (ONode);
void        ONodeSetCameraAligned         (ONode,int);
ONode       ONodeGetParent                (ONode);
int         ONodeGetPickIdentifier        (ONode);
void        ONodeSetPickIdentifier        (ONode,int);
void        ONodeAddChild                 (ONode,ONode);
void        ONodeRemoveChild              (ONode,ONode);
void        ONodeDestroyChildren          (ONode);
void        ONodeAddPrimitive             (ONode,OPrimitive);
void        ONodeRemovePrimitives         (ONode);
void        ONodeRemovePrimitive          (ONode,OPrimitive);
void        ONodeAddPointsToPrimitive     (ONode,OPrimitive,int,double*,double*,double*);
OPrimitive* ONodeGetPrimitives            (ONode);
ONode*      ONodeGetLeavesForward         (ONode);
ONode*      ONodeGetLeavesBackward        (ONode);
int         ONodeGetBlink                 (ONode);
void        ONodeSetBlink                 (ONode,int);
int         ONodeGetSize                  (ONode,double*,double*,double*,double*,double*,double*);
int         ONodeGetSumOfPoints           (ONode,double*,double*,double*);
void        ONodeAddView                  (ONode,void*);
void        ONodeRemoveView               (ONode,void*);
CList       ONodeGetViews                 (ONode);
void        ONodeSetHookPoint             (ONode,double,double,double);
void        ONodeGetHookPoint             (ONode,double*,double*,double*);
int         ONodeSetAttribute             (ONode,char*,void*,void*,int);
int         ONodeGetAttribute             (ONode,char*,void*,void*,int*);
ONode*      ONodeCollectV                 (ONode*,int,va_list);
ONode*      ONodeCollect                  (ONode*,int,...);

void        ONodeSetPrimitivesColorIndex  (ONode,int);
void        ONodeSetPrimitivesLineStyle   (ONode,OLineStyle);
void        ONodeSetPrimitivesMarkerStyle (ONode,OMarkerStyle);
void        ONodeSetPrimitivesAreaStyle   (ONode,OAreaStyle);
void        ONodeSetPrimitivesLineWidth   (ONode,int);
void        ONodeSetPrimitivesMarkerSize  (ONode,int);
void        ONodeAddLine                  (ONode,OContext,double,double,double,double,double,double);
void        ONodeAddMarker                (ONode,OContext,double,double,double);
void        ONodeAddPolygon               (ONode,OContext,int,double*,double*,double*);
void        ONodeAddLines                 (ONode,OContext,int,double*,double*,double*);
void        ONodeAddMarkers               (ONode,OContext,int,double*,double*,double*);
void        ONodeAddSegments              (ONode,OContext,int,double*,double*,double*);

int         ONodeIsShape                  (ONode);
OPrimitive* ONodeGetShapePrimitives       (ONode);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindONode(0); /**/
#endif

#endif  /*ONode_h*/
