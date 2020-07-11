/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OHistogram_h
#define OHistogram_h

typedef struct _OHistogramRecord    *OHistogram;

#define OHistogramAdd       0
#define OHistogramSubstract 1
#define OHistogramMultiply  2
#define OHistogramDivide    3

      /*12345678901234567890123456789012*/
#ifdef HAS_SHORT_NAME
#define OHistogramSetIthAxisSumOfWeightedCoordinates        OHistogramSetIthAxisSXW
#define OHistogramGetIthAxisSumOfWeightedCoordinates        OHistogramGetIthAxisSXW
#define OHistogramSetIthAxisSumOfWeightedSquaredCoordinates OHistogramSetIthAxisSX2W
#define OHistogramGetIthAxisSumOfWeightedSquaredCoordinates OHistogramGetIthAxisSX2W
#endif

#include <OAxis.h>
#ifdef __cplusplus
extern "C"{
#endif
void         OHistogramClearClass                                ();
OHistogram*  OHistogramGetIdentifiers                            ();
OHistogram   OHistogramGetIdentifier                             (char*);
OHistogram   OHistogramMake                                      ();
OHistogram   OHistogramCreate                                    (char*,int,...);
int          OHistogramIsValid                                   (OHistogram);
void         OHistogramDelete                                    (OHistogram);
void         OHistogramSetName                                   (OHistogram,char*);
char*        OHistogramGetName                                   (OHistogram);
void         OHistogramSetBarsType                               (OHistogram,int);
int          OHistogramIsBarsType                                (OHistogram);
void         OHistogramConfigure                                 (OHistogram,int,...);
void         OHistogramAllocateMemory                            (OHistogram);
void         OHistogramClear                                     (OHistogram);
void         OHistogramAddContents                               (OHistogram,OHistogram);
int          OHistogramOperateBinary                             (OHistogram,OHistogram,OHistogram,int,double,double);
int          OHistogramFindFirstEmptyBin                         (OHistogram);
int          OHistogramFindFirstNotEmptyBin                      (OHistogram);
int          OHistogramFindLastNotEmptyBin                       (OHistogram);
int          OHistogramGetHalfLowerMeanPoint                     (OHistogram,double*,double*);
int          OHistogramGetHalfUpperMeanPoint                     (OHistogram,double*,double*);
void         OHistogramFillFromArray                             (OHistogram,double*,double);
void         OHistogramFill                                      (OHistogram,...);
void         OHistogramFillOneDimensional                        (OHistogram,double,double);
void         OHistogramFillTwoDimensional                        (OHistogram,double,double,double);
void         OHistogramDoProfile                                 (OHistogram);
void         OHistogramSetDimension                              (OHistogram,int);
int          OHistogramGetDimension                              (OHistogram);
void         OHistogramDump                                      (OHistogram);
void         OHistogramSetNumberOfEntries                        (OHistogram,int);
int          OHistogramGetNumberOfEntries                        (OHistogram);
void         OHistogramSetSumOfWeights                           (OHistogram,double);
double       OHistogramGetSumOfWeights                           (OHistogram);
void         OHistogramSetTitle                                  (OHistogram,char*);
char*        OHistogramGetTitle                                  (OHistogram);
void         OHistogramSetUserData                               (OHistogram,void*);
void*        OHistogramGetUserData                               (OHistogram);
void         OHistogramSetProfile                                (OHistogram,int);
int          OHistogramIsProfile                                 (OHistogram);
void         OHistogramSetMonitoringMode                         (OHistogram,char*);
char*        OHistogramGetMonitoringMode                         (OHistogram);
int          OHistogramGetBinsRange                              (OHistogram,int,int*,int*,double*,double*,double*,double*);
void         OHistogramSetBinSumOfWeights                        (OHistogram,...);
double       OHistogramGetBinSumOfWeights                        (OHistogram,...);
void         OHistogramSetBinSumOfSquaredWeights                 (OHistogram,...);
double       OHistogramGetBinSumOfSquaredWeights                 (OHistogram,...);
void         OHistogramSetBinNumberOfEntries                     (OHistogram,...);
int          OHistogramGetBinNumberOfEntries                     (OHistogram,...);
double       OHistogramGetBinBar                                 (OHistogram,...);
void         OHistogramGetBinSumOfWeightsRange                   (OHistogram,double*,double*);

void         OHistogramAdjustAxisOfValues                        (OHistogram,int);
void         OHistogramSetAxisOfValuesTitle                      (OHistogram,char*);
char*        OHistogramGetAxisOfValuesTitle                      (OHistogram);
OAxis        OHistogramGetIthAxis                                (OHistogram,OAxisIndex);
int          OHistogramSetIthAxisAttributesF                     (OHistogram,OAxisIndex,char*,...);
void         OHistogramGetIthAxisAttributesF                     (OHistogram,OAxisIndex,char*,...);
double       OHistogramGetIthAxisBinWidth                        (OHistogram,OAxisIndex);
double       OHistogramGetIthAxisBinMinimum                      (OHistogram,OAxisIndex,int);
double       OHistogramGetIthAxisBinMaximum                      (OHistogram,OAxisIndex,int);
double       OHistogramGetIthAxisBinMiddle                       (OHistogram,OAxisIndex,int);
int          OHistogramGetIthAxisBinIndex                        (OHistogram,OAxisIndex,double);
void         OHistogramSetIthAxisUnderflow                       (OHistogram,OAxisIndex,int);
int          OHistogramGetIthAxisUnderflow                       (OHistogram,OAxisIndex);
void         OHistogramSetIthAxisOverflow                        (OHistogram,OAxisIndex,int);
int          OHistogramGetIthAxisOverflow                        (OHistogram,OAxisIndex);
void         OHistogramSetIthAxisSumOfWeightedCoordinates        (OHistogram,OAxisIndex,double);
double       OHistogramGetIthAxisSumOfWeightedCoordinates        (OHistogram,OAxisIndex);
void         OHistogramSetIthAxisSumOfWeightedSquaredCoordinates (OHistogram,OAxisIndex,double);
double       OHistogramGetIthAxisSumOfWeightedSquaredCoordinates (OHistogram,OAxisIndex);
double       OHistogramGetIthAxisMeanValue                       (OHistogram,OAxisIndex);
double       OHistogramGetIthAxisStandardDeviation               (OHistogram,OAxisIndex);

int          OHistogramSetAttribute                              (OHistogram,char*,void*,void*,int);
int          OHistogramGetAttribute                              (OHistogram,char*,void*,void*,int*);
int          OHistogramDoMethod                                  (OHistogram,char*,void*,int,char**,void*,int*);

char**       OHistogramGetDimensions                             (int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOHistogram(0); /**/
#endif

#endif  /*OHistogram_h*/
