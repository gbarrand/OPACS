/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OScatter_h
#define OScatter_h

typedef struct _OScatterRecord    *OScatter;

#include <OAxis.h>
#ifdef __cplusplus
extern "C"{
#endif
void         OScatterClearClass                  ();
OScatter*    OScatterGetIdentifiers              ();
OScatter     OScatterGetIdentifier               (char*);
OScatter     OScatterMake                        ();
OScatter     OScatterCreate                      (char*,int,...);
int          OScatterIsValid                     (OScatter);
void         OScatterDelete                      (OScatter);
void         OScatterSetName                     (OScatter,char*);
char*        OScatterGetName                     (OScatter);
void         OScatterConfigure                   (OScatter,int,...);
void         OScatterClear                       (OScatter);

void         OScatterAllocatePoints              (OScatter,int);
int          OScatterGetNumberOfEntries          (OScatter);
void         OScatterGetIthPoint                 (OScatter,int,...);

void         OScatterFill                        (OScatter,...);
void         OScatterFillOneDimensional          (OScatter,double);
void         OScatterFillTwoDimensional          (OScatter,double,double);
void         OScatterFillThreeDimensional        (OScatter,double,double,double);

void         OScatterSetDimension                (OScatter,int);
int          OScatterGetDimension                (OScatter);
void         OScatterDump                        (OScatter);
void         OScatterSetTitle                    (OScatter,char*);
char*        OScatterGetTitle                    (OScatter);

OAxis        OScatterGetIthAxis                  (OScatter,OAxisIndex);
int          OScatterSetIthAxisAttributesF       (OScatter,OAxisIndex,char*,...);
void         OScatterGetIthAxisAttributesF       (OScatter,OAxisIndex,char*,...);
void         OScatterSetIthAxisUnderflow         (OScatter,OAxisIndex,int);
int          OScatterGetIthAxisUnderflow         (OScatter,OAxisIndex);
void         OScatterSetIthAxisOverflow          (OScatter,OAxisIndex,int);
int          OScatterGetIthAxisOverflow          (OScatter,OAxisIndex);
double       OScatterGetIthAxisMeanValue         (OScatter,OAxisIndex);
double       OScatterGetIthAxisStandardDeviation (OScatter,OAxisIndex);
int          OScatterSetAttribute                (OScatter,char*,void*,void*,int);
int          OScatterGetAttribute                (OScatter,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOScatter(0); /**/
#endif

#endif  /*OScatter_h*/
