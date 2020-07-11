/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OMatrix_h
#define OMatrix_h

typedef struct _OMatrixRecord *OMatrix;

#define OMatrixIdentity         1
#define OMatrixScale            2
#define OMatrixScaleIsometric   3
#define OMatrixScaleTranslation 4
#define OMatrixTranslationScale 5
#define OMatrixRotationX        6
#define OMatrixRotationY        7
#define OMatrixRotationZ        8
#define OMatrixRotationAxis     9
#define OMatrixShear            10
#define OMatrixTranslation      11
#define OMatrixZToVector        12
#define OMatrixVectorToZ        13
#define OMatrixFollowing        14
#define OMatrixStringAlign      15
#define OMatrixTranslationAxis  16
#define OMatrixGL               17

#ifdef __cplusplus
extern "C"{
#endif
void     OMatrixGetDirection                (double,double,double,double,double,double,double,double,double,double,double,double,double*,double*,double*,double*);

OMatrix  OMatrixMake                        ();
OMatrix  OMatrixCreate                      (int,...);
void     OMatrixDelete                      (OMatrix);
void     OMatrixInitialize                  (OMatrix,int,...);
void     OMatrixCopy                        (OMatrix,OMatrix);
double   OMatrixInvertAndAssign             (OMatrix,OMatrix);
void     OMatrixTransposeAndAssign          (OMatrix,OMatrix);
void     OMatrixMultiplyByConstantAndAssign (OMatrix,OMatrix,double);
void     OMatrixAddAndAssign                (OMatrix,OMatrix,OMatrix);
void     OMatrixSubstractAndAssign          (OMatrix,OMatrix,OMatrix);
void     OMatrixMultiplyAndAssign           (OMatrix,OMatrix,OMatrix);
void     OMatrixMultiplyTripletAndAssign    (OMatrix,OMatrix,OMatrix,OMatrix);
void     OMatrixMultiplyMatricesAndAssign   (OMatrix,int,...);
void     OMatrixSetElement32                (OMatrix,double);
void     OMatrixSetElement33                (OMatrix,double);
void     OMatrixSetElement23                (OMatrix,double);
double   OMatrixGetElement00                (OMatrix);
double   OMatrixGetElement11                (OMatrix);
double   OMatrixGetElement22                (OMatrix);
double   OMatrixGetElement01                (OMatrix);
double   OMatrixGetElement02                (OMatrix);
double   OMatrixGetElement12                (OMatrix);
void     OMatrixMultiplyVector              (OMatrix,double,double,double,double*,double*,double*);
void     OMatrixConvertToGL_Matrix          (OMatrix,double*);
OMatrix  OMatrixDuplicate                   (OMatrix);
void     OMatrixRemoveTranslations          (OMatrix);
void     OMatrixRemoveIsometricScale        (OMatrix);
OMatrix  OMatrixMultiply                    (OMatrix,OMatrix);
OMatrix  OMatrixMultiplyTriplet             (OMatrix,OMatrix,OMatrix);
OMatrix  OMatrixMultiplyMatrices            (int,...);
OMatrix  OMatrixAdd                         (OMatrix,OMatrix);
OMatrix  OMatrixSubstract                   (OMatrix,OMatrix);
int      OMatrixIsIdentity                  (OMatrix);
double   OMatrixGetDeterminant              (OMatrix);
double   OMatrixGetTrace                    (OMatrix);
OMatrix  OMatrixInvert                      (OMatrix);
OMatrix  OMatrixTranspose                   (OMatrix);
OMatrix  OMatrixMultiplyByConstant          (OMatrix,double);
OMatrix  OMatrixGetSymetricPart             (OMatrix);
OMatrix  OMatrixGetAntisymetricPart         (OMatrix);
void     OMatrixDump                        (OMatrix);
int      OMatrixSetAttribute                (OMatrix,char*,void*,void*,int);
int      OMatrixGetAttribute                (OMatrix,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOMatrix(0); /**/
#endif

#endif  /*OMatrix_h*/
