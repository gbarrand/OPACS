/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CVector_h
#define CVector_h

#ifdef __cplusplus
extern "C"{
#endif
void   CVectorDump          (double,double,double);
double CVectorGetNorme      (double,double,double);
double CVectorNormalize     (double*,double*,double*);
double CVectorGetDotProduct (double,double,double,double,double,double);
double CVectorGetCosine     (double,double,double,double,double,double);
double CVectorGetDistance   (double,double,double,double,double,double);
void   CVectorMultiply      (double,double,double,double,double,double,double*,double*,double*);
double CVectorGetPolarAngle (double,double,double);
double CVectorGetAzimuth    (double,double,double);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCVector(0); /**/
#endif

#endif  /*CVector_h*/
