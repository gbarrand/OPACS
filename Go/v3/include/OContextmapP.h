#ifndef ContextmapP_h
#define ContextmapP_h

#include <OContextmap.h>

typedef struct _OContextmapRecord
{
  OString       name;
  OType         referenceType;
  OProperty     referenceProperty;
  int           referencePropertyType;
  OString       defaultContextScript;
  OString       defaultLabel;
  OPacking      labelPacking;
  OString       title;
  OString       labelContextScript;
  double        labelHeight;
  double        labelPositionX;
  double        labelPositionY;
  double        labelPositionZ;
  OString       labelRotationScript;
  OMatrix       labelRotationMatrix;
  int           iconTypes;
  double        iconSizeStretch;
  int           showHitsOnly;
  /*OContexts*/
  int           contextn;
  OContext*     contexts;
  int*          hits;
  OString*      contextsScript;
  /**/
  int           valuen;
  double*       values;
  int           labeln;
  OString*      labels;
} OContextmapRecord;

#endif/*ContextmapP_h*/

