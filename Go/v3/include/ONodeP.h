/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef ONodeP_h
#define ONodeP_h

#include <ONode.h>

typedef enum {
  GBlinkOff  = 1,
  GBlinkLow,
  GBlinkHigh
} OBlink;

typedef struct _NodeRecord
{
  LkEntryRec                  links;
  LkListRec                   children;
  LkEntryRec                  childrenLinks;
  ONode                       parent;
  char*                       name;
  CList                       views;
  OPrimitive*                 prims;
  int                         highLight;
  int                         touched;
  OMatrix                     matrix;
  int                         pickable;
  int                         visible;
  int                         pickId;
  OBlink                      blinkType;
  int                         projected;
  int                         cameraAligned;
  double                      xHook,yHook,zHook;
  char                        toRefresh,toRedraw;
  /*Shapes.*/
  int                         isShape;
  ONodeRepresentShapeFunction representShapeFunction;
  ONodeDeleteShapeFunction    deleteShapeFunction;
} NodeRecord;

typedef struct _ONodeRecord
{
  NodeRecord    node;
} ONodeRecord;

#endif /*ONodeP_h*/



