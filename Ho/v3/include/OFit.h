/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OFit_h
#define OFit_h

typedef enum {
    OFitTypeNone        = 0,
    OFitTypeGaussian    = 1,
    OFitTypeExponential = 2,
    OFitTypePolynomial  = 3,
    OFitTypeBreitWigner = 4,
    OFitTypeUser        = 5
} OFitType;

typedef struct _OFitRecord *OFit;

typedef void (*OFitFitFunction)(void*,int,int,double*,double*,double*,int,double*,double*);

#ifdef __cplusplus
extern "C"{
#endif
void       OFitSetFitFunction          (OFitFitFunction);
OFit       OFitMake                    ();
void       OFitDelete                  (OFit);
void       OFitSetType                 (OFit,OFitType);
OFitType   OFitGetType                 (OFit);
void       OFitSetScript               (OFit,char*,int,char**);
void       OFitSetNumberOfParameters   (OFit,int);
int        OFitGetNumberOfParameters   (OFit);
void       OFitSetStartValues          (OFit,int,double*);
double     OFitGetIthStartValue        (OFit,int);
void       OFitSetStartValuesAutomated (OFit,int);
int        OFitAreStartValuesAutomated (OFit);
void       OFitSetSteps                (OFit,int,double*);
double     OFitGetIthStep              (OFit,int);
void       OFitSetStepsAutomated       (OFit,int);
int        OFitAreStepsAutomated       (OFit);
int        OFitExecute                 (OFit,int,double*,double*,double*);
char*      OFitGetInformations         (OFit);
char*      OFitBuildScript             (OFit);
double     OFitEvaluateScript          (OFit,double,int,double*);
char**     OFitGetVariableNames        (OFit,int*);
char*      OFitGetScript               (OFit);

char**     OFitTypeGetNames            (int*);
OFitType   OFitTypeGetIdentifier       (char*);
char*      OFitTypeGetName             (OFitType);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOFit(0); /**/
#endif

#endif  /*OFit_h*/
