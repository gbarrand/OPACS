/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OAxis_h
#define OAxis_h
 
typedef struct _OAxisRecord *OAxis;

#define OAxisX  0
#define OAxisY  1
#define OAxisZ  2
#define OAxisT  3
#define OAxisV  999
typedef int OAxisIndex;

#define OAxisMaximumNumber 4  /* change OAxisMaximumNumber --> change OTupleFill in OTuple.c */

#ifndef NO_C_H
#include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
int        OAxisVerifyIndex        (int);
OAxis      OAxisMake               ();
void       OAxisDelete             (OAxis);
void       OAxisInitialize         (OAxis);
void       OAxisClear              (OAxis);
void       OAxisCopy               (OAxis,OAxis);
int        OAxisSetAttributesF     (OAxis,char*,...);
void       OAxisGetAttributesF     (OAxis,char*,...);
void       OAxisSetAttributesV     (OAxis,int*,char*,va_list);
void       OAxisGetAttributesV     (OAxis,char*,va_list);
void       OAxisSetAttributes      (OAxis,char*);
int        OAxisVerifyNumberOfBins (OAxis,int);
void       OAxisVerifyRange        (OAxis,double,double);
int        OAxisAdjust             (OAxis);
void       OAxisComputeTicks       (OAxis);
int        OAxisGetNumberOfTicks   (OAxis);
double     OAxisGetIthTickValue    (OAxis,int);
char*      OAxisGetIthTickLabel    (OAxis,int);
int        OAxisGetNumberOfBins    (OAxis);
double     OAxisGetMinimumValue    (OAxis);
double     OAxisGetMaximumValue    (OAxis);
char*      OAxisGetTitle           (OAxis);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOAxis(0); /**/
#endif

#endif  /*OAxis_h*/
