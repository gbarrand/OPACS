/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OFunction_h
#define OFunction_h
 
typedef struct _OFunctionRecord *OFunction;

#include <OAxis.h>
#ifdef __cplusplus
extern "C"{
#endif
void      OFunctionClearClass            ();
OFunction OFunctionGetIdentifier         (char*);
OFunction OFunctionMake                  ();
OFunction OFunctionCreate                (char*);
int       OFunctionIsValid               (OFunction);
char*     OFunctionGetName               (OFunction);
void      OFunctionDelete                (OFunction);
OAxis     OFunctionGetIthAxis            (OFunction,OAxisIndex);
int       OFunctionSetIthAxisAttributesF (OFunction,OAxisIndex,char*,...);
void      OFunctionGetIthAxisAttributesF (OFunction,OAxisIndex,char*,...);
void      OFunctionSetVariable           (OFunction,OAxisIndex,char*);
void      OFunctionSetDimension          (OFunction,int);
int       OFunctionGetDimension          (OFunction);
void      OFunctionSetScript             (OFunction,char*);
char*     OFunctionGetScript             (OFunction);
void      OFunctionGetRange              (OFunction,double*,double*);
void      OFunctionAdjustAxisOfValues    (OFunction);
double    OFunctionEvaluate              (OFunction,int,double*);
double    OFunctionEvaluateF             (OFunction,int,...);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOFunction(0); /**/
#endif

#endif  /*OFunction_h*/
