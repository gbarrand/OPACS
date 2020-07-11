/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OCamera_h
#define OCamera_h
 
typedef struct _OCameraRecord  *OCamera;

#include <ONode.h>

typedef int(*OCameraSendFunction)(void*);
typedef int(*OCameraDoSpecialProjectionFunction)(OCamera,ONode,int,double*,double*,double*,double**,double**,double**);

typedef enum {
  OBufferingSimple,
  OBufferingDouble
} OBuffering;

typedef enum {
  OProjectionParallel,
  OProjectionPerspective
} OProjection;

typedef enum 
{
  ODepthTreatmentNone,
  ODepthTreatmentZBuffer
} ODepthTreatment;

typedef enum {
  OSpecialProjectionXYZ,
  OSpecialProjectionRZ,
  OSpecialProjectionPHIZ,
  OSpecialProjectionRHOZ,
  OSpecialProjectionTHETAPHI,
  OSpecialProjectionPHITHETA,
  OSpecialProjectionPHIR,
  OSpecialProjectionPHIRHO,
  OSpecialProjectionVPLOT,
  OSpecialProjectionFISHEYE
} OSpecialProjection;

#include <OMatrix.h>
#include <OColormap.h>
#include <OPick.h>
#ifdef __cplusplus
extern "C"{
#endif
void                 OCameraClearClass                    ();
OCamera*             OCameraGetIdentifiers                ();
OCamera              OCameraGetIdentifierF                (int,char*,...);
OCamera              OCameraGetIdentifier                 (char*);
OCamera              OCameraMake                          ();
OCamera              OCameraCreate                        (char*,int,int);
OCamera              OCameraGetCurrent                    ();
void                 OCameraSetCurrent                    (OCamera);
int                  OCameraIsValid                       (OCamera);
void                 OCameraDelete                        (OCamera);
void                 OCameraSetName                       (OCamera,char*);
char*                OCameraGetName                       (OCamera);

void                 OCameraSetEdgesVisible               (OCamera,int);
int                  OCameraAreEdgesVisible               (OCamera);
void                 OCameraSetBackgroundColorIndex       (OCamera,int);
int                  OCameraGetBackgroundColorIndex       (OCamera);
void                 OCameraSetEdgeColorIndex             (OCamera,int);
int                  OCameraGetEdgeColorIndex             (OCamera);
void                 OCameraSetHighlightContext           (OCamera,char*);
char*                OCameraGetHighlightContext           (OCamera);
void                 OCameraSetDepthTreatment             (OCamera,ODepthTreatment);
ODepthTreatment      OCameraGetDepthTreatment             (OCamera);
void                 OCameraSetBuffering                  (OCamera,OBuffering);
OBuffering           OCameraGetBuffering                  (OCamera);
void                 OCameraSetProjection                 (OCamera,OProjection);
OProjection          OCameraGetProjection                 (OCamera);
void                 OCameraSetProjectionAngle            (OCamera,double);
double               OCameraGetProjectionAngle            (OCamera);
void                 OCameraSetSpecialProjection          (OCamera,OSpecialProjection);
OSpecialProjection   OCameraGetSpecialProjection          (OCamera);
void                 OCameraSetDoSpecialProjectionFunction(OCamera,OCameraDoSpecialProjectionFunction);
OCameraDoSpecialProjectionFunction OCameraGetDoSpecialProjectionFunction (OCamera);
int                  OCameraDoDefaultSpecialProjection    (OCamera,ONode,int,double*,double*,double*,double**,double**,double**);

void                 OCameraGetSize                      (OCamera,int*,int*);
void                 OCameraGetViewLimits                (OCamera,double*,double*,double*,double*);
OColormap            OCameraGetColormap                  (OCamera);
void                 OCameraSetColormap                  (OCamera,OColormap);
void                 OCameraChangeSize                   (OCamera,int,int);
OMatrix              OCameraGetTotalMatrix               (OCamera);
double               OCameraGetRuleScale                 (OCamera,OCamera,int*,int*);
void                 OCameraGetNDC                       (OCamera,OMatrix,OMatrix);
double               OCameraGetScale                     (OCamera);
void                 OCameraSetAspectRatio               (OCamera,int);
int                  OCameraGetAspectRatio               (OCamera);
void                 OCameraSetBlackAndWhite             (OCamera,int);
int                  OCameraGetBlackAndWhite             (OCamera);
void                 OCameraGetWorldCoordinates          (OCamera,int,int,double*,double*,double*);

void                 OCameraGetField                     (OCamera,double*,double*);
void                 OCameraGetDefaultVRP                (OCamera,double*,double*,double*);
void                 OCameraGetDefaultUpVector           (OCamera,double*,double*,double*);
void                 OCameraGetVRP                       (OCamera,double*,double*,double*);
void                 OCameraGetUpVector                  (OCamera,double*,double*,double*);
void                 OCameraGetDOP                       (OCamera,double*,double*,double*);
void                 OCameraRestoreDefaultUpVectorAndVRP (OCamera);
void                 OCameraSetField                     (OCamera,double,double);
void                 OCameraSetDefaultVRP                (OCamera,double,double,double);
void                 OCameraSetVRP                       (OCamera,double,double,double);
void                 OCameraSetDefaultUpVector           (OCamera,double,double,double);
void                 OCameraSetUpVector                  (OCamera,double,double,double);
void                 OCameraSetCenter                    (OCamera,double,double,double);
void                 OCameraGetCenter                    (OCamera,double*,double*,double*);
void                 OCameraScaleDefaultFieldAndVRP      (OCamera,double);
void                 OCameraMultiplyUpVectorAndVRP       (OCamera,OMatrix);
   
void                 OCameraSetRequireMovementFunction   (OCamera,OCameraSendFunction,void*,int);
void                 OCameraRequireMovement              (OCamera);

void                 OCameraSetRequireRefreshmentFunction (OCamera,OCameraSendFunction,void*,int);
int                  OCameraIsRefreshDone                (OCamera);
void                 OCameraRequireRefreshment           (OCamera);
void                 OCameraRequireDrawing               (OCamera);
void                 OCameraReceiveRefreshmentRequest    (OCamera);
void                 OCameraRefresh                      (OCamera);
void                 OCameraSetNextRefreshOnTouched      (OCamera);

OMatrix              OCameraGetMatrix                    (OCamera);
void                 OCameraSetDefaultMatrix             (OCamera,OMatrix);
void                 OCameraRestoreDefaultMatrix         (OCamera);
void                 OCameraSetMatrix                    (OCamera,OMatrix);
void                 OCameraMultiplyMatrix               (OCamera,OMatrix);
double               OCameraGetMatrixScale               (OCamera);

void                 OCameraAddPick                      (OCamera,OPick);
void                 OCameraDeletePicks                  (OCamera);
OPick                OCameraGetPick                      (OCamera,int);
void                 OCameraCenterMatrixOnPick           (OCamera,OPick);
void                 OCameraAlignMatrixOnPick            (OCamera,OPick,OPick);
void                 OCameraRotateMatrixOnPick           (OCamera,double,OPick,OPick);
void                 OCameraTranslateMatrixOnPick        (OCamera,double,OPick,OPick);

void                 OCameraPutMatrixOnStack             (OCamera);
void                 OCameraRestoreLastMatrixOfStack     (OCamera);
void                 OCameraRestoreFirstMatrixOfStack    (OCamera);

ONode*               OCameraGetNodes                     (OCamera);
void                 OCameraViewNode                     (OCamera,ONode);
void                 OCameraViewNodes                    (OCamera,ONode*);
void                 OCameraUnviewNode                   (OCamera,ONode);
ONode*               OCameraCollect                      (OCamera,int,...);
int                  OCameraIsEmpty                      (OCamera);
void                 OCameraErase                        (OCamera);
void                 OCameraHighlightNodes               (OCamera);
void                 OCameraUnhighlightNodes             (OCamera);
void                 OCameraConnect                      (OCamera,OCamera);
void                 OCameraNormalizeMatrix              (OCamera,ONode*);
ONode*               OCameraGetNodesInRegion             (OCamera,int,int,int,int,OPick*);
void                 OCameraSetPlotLayout                (OCamera,int);
void                 OCameraSetBlinkDelay                (unsigned long);
int                  OCameraSetAttribute                 (OCamera,char*,void*,void*,int);
int                  OCameraGetAttribute                 (OCamera,char*,void*,void*,int*);

ONode                OCameraGetRootNode                  (OCamera);
void                 OCameraSetRootNode                  (OCamera,ONode);

void                 OCameraOpenPS_FileForWriting        (char*);
void                 OCameraClosePS_Stream               ();
void                 OCameraPutSceneInPS_Stream          (OCamera);
void                 OCameraPutScenePageInPS_Stream      (OCamera);
void                 OCameraProducePS_File               (OCamera,char*);

char**               OBufferingGetNames              (int*);
OBuffering           OBufferingGetIdentifier         (char*);
char*                OBufferingGetName               (OBuffering);
char**               OProjectionGetNames             (int*);
OProjection          OProjectionGetIdentifier        (char*);
char*                OProjectionGetName              (OProjection);
char**               ODepthTreatmentGetNames         (int*);
ODepthTreatment      ODepthTreatmentGetIdentifier    (char*);
char*                ODepthTreatmentGetName          (ODepthTreatment);
char**               OSpecialProjectionGetNames      (int*);
OSpecialProjection   OSpecialProjectionGetIdentifier (char*);
char*                OSpecialProjectionGetName       (OSpecialProjection);

#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOCamera(0); /**/
#endif

#endif  /*OCamera_h*/
