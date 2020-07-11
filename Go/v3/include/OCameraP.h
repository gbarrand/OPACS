/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OCameraP_h
#define OCameraP_h

#include <OCamera.h>

typedef struct _SpecialProjectionRecord
{
  OSpecialProjection                 specialProjection;
  OCameraDoSpecialProjectionFunction doSpecialProjectionFunction;
  double                             xcompress;
  double                             fisheye_a;
  double                             rhoz_phi1;
  double                             v_rho_max;
  double                             v_z_max;
  double                             v_k;
} SpecialProjectionRecord;

typedef struct _OCameraRecord
{
  char*                   name;
  int                     width,height;
  OCameraSendFunction     sendMoveProc;
  void*                   sendMoveArg;
  int                     sendMoveDone;
  OCameraSendFunction     sendRefreshProc;
  void*                   sendRefreshArg;
  int                     sendRefreshDone;
/*draw*/
  OColormap               ocmap;
  int                     bw;
  int                     backgroundColor,edgeColor;
  char*                   highlightContext;
  int                     edgesVisible;
  ONode*                  toView;
  OCamera*                referenceCamera;
  int                     aspectRatio;
  ODepthTreatment         depthTreatment;
  OBuffering              buffering;
  OProjection             projection;
  double                  projectionAngle;
  double                  fieldMin,fieldMax;
  double                  center_x,center_y,center_z;
  double                  viewVrp_x,viewVrp_y,viewVrp_z;
  double                  viewUp_x,viewUp_y,viewUp_z;
  double                  vrp_x,vrp_y,vrp_z;
  double                  up_x,up_y,up_z;
  OMatrix                 ctmInit;
  OMatrix                 ctm;
  OMatrix                 global;
  OPick*                  picks;
  SpecialProjectionRecord specialProjection;
  int                     nextRefreshOnTouched;
/*For OnX v6.*/
  ONode                   rootNode;
/*ctm stack*/
#define MAX__CTM 20
  int                     ctmn;
  OMatrix                 ctms[MAX__CTM];
} OCameraRecord;

#endif /*OCameraP_h*/
