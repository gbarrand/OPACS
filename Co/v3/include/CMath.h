/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CMath_h
#define CMath_h

#ifdef __cplusplus
extern "C"{
#endif
void    CMathGetDigitsOfDouble (double,int*,int*,int*);
double  CMathGetGauss          (double,double,double,double);
double  CMathGetBreitWigner    (double,double,double,double);
double  CMathGetPolynomial     (double,int,...);
double  CMathGetArcTangent     (double,double);
double  CMathGetRandomFlat     ();
double  CMathGetRandomGauss    ();
int     CMathSetAttribute      (void*,char*,void*,void*,int);
int     CMathGetAttribute      (void*,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCMath(0); /**/
#endif

#endif  /*CMath_h*/
