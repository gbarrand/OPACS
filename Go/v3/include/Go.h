/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef Go_h
#define Go_h

#include <OPointList.h>
#include <ONode.h>
#ifdef __cplusplus
extern "C"{
#endif
void     GoSetColorIndex           (int);
void     GoSetLineWidth            (int);
void     GoSetLineStyle            (OLineStyle);
void     GoSetMarkerStyle          (OMarkerStyle);
void     GoSetAreaStyle            (OAreaStyle);

void     GoAddLineToNode           (ONode,double,double,double,double,double,double);
void     GoAddMarkerToNode         (ONode,double,double,double);
void     GoAddPolygonToNode        (ONode,OPointList);
void     GoAddLinesToNode          (ONode,OPointList);
void     GoAddMarkersToNode        (ONode,OPointList);
void     GoAddSegmentsToNode       (ONode,OPointList);

void     GoAddColoredBoxToNode     (ONode,double,double,double,double,double,double,int,int,int);
void     GoAddAxisToNode           (ONode,double,double,double,double,double,double);
void     GoAddFrameToNode          (ONode,double);
void     GoAddRZ_FrameToNode       (ONode,double);
void     GoAddArrowToNode          (ONode,OMatrix,int);
void     GoAddRuleToNode           (ONode,double,double,double,double,int,char*,double,OMatrix,OMatrix);
void     GoAddAlignedStringToNode  (ONode,char*,double,double,double,double,double,double,double,double,double,double,char,char);
void     GoAddTextFieldToNode      (ONode,char*,double,double,double,double,double);
void     GoAddStringToNode         (ONode,char*,double,double,double,OMatrix);
double   GoAddPAW_StringToNode     (ONode,char*,double,double,double,OMatrix);
void     GoAddBoxToNode            (ONode,double,double,double,double,double,double);
void     GoAddGridToNode           (ONode,double,double,double,double,double,double,double,double,double,int,int,double,double);
void     GoAddTubeToNode           (ONode,double,double,double,double,double,double,double,double,double);
void     GoAddCircleToNode         (ONode,double,double,double,double,double,double,double);
void     GoAddHelixToNode          (ONode,int,double,double,int,double,double,double,double,double,double,double,double,double,double,double,double,double);
void     GoAddRayToNode            (ONode,int,double,double,double,double,double,double,double,double,double,double);
void     GoAddHelicalTrackToNode   (ONode,double,double,double,double,double,double);
void     GoAddRectangleToNode      (ONode,double,double,double,double,double,double,double,double);
void     GoAddDiamondToNode        (ONode,double,double,double,double,double,double,double,double);

void     GoAddCylinderToNode       (ONode,OMatrix,double,double);
void     GoAddLineOfCylinderToNode (ONode,OMatrix, double,double, double, double, double, double);
void     GoAddArcToNode            (ONode,OMatrix, double,double, double, double);
void     GoAddGEANT_TRD2_ToNode    (ONode,OMatrix,double,double,double,double,double);
void     GoAddGEANT_TUBE_ToNode    (ONode,OMatrix,double,double,double);
void     GoAddGEANT_TUBS_ToNode    (ONode,OMatrix,double,double,double,double,double);
void     GoAddGEANT_CONE_ToNode    (ONode,OMatrix,double,double,double,double,double);
void     GoAddGEANT_CONS_ToNode    (ONode,OMatrix,double,double,double,double,double,double,double);
void     GoAddGEANT_PCON_ToNode    (ONode,OMatrix,float*);
void     GoAddGEANT_PGON_ToNode    (ONode,OMatrix,float*);
void     GoAddGEANT_HYPE_ToNode    (ONode,OMatrix,double,double,double,double);
void     GoAddGEANT_TRAP_ToNode    (ONode,OMatrix,double,double,double,double,double,double,double,double,double,double,double);

void*    GoRepresentOContextmap    (void*,void*);

int      GoDoMethod                (void*,char*,void*,int,char**,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindGo(0); /**/
#endif

#endif  /*Go_h*/
