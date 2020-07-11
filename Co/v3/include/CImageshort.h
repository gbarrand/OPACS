/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CImageshort_h
#define CImageshort_h

#ifndef CImageAdd
#define CImageAdd 0
#define CImageSub 1
#define CImageMul 2
#define CImageDiv 3

#define short_min  -32768
#define short_max   32767

#define ushort_min      0
#define ushort_max  65535
#endif

#ifdef __cplusplus
extern "C"{
#endif
void   CImageshortGetLUT                (short*,unsigned int,unsigned int,int*,int*,int*);
void   CImageshortAddConstant           (short*,unsigned int,unsigned int,short);
void   CImageshortSubstractConstant     (short*,unsigned int,unsigned int,short);
void   CImageshortMultiplyByConstant    (short*,unsigned int,unsigned int,short);
void   CImageshortDivideByConstant      (short*,unsigned int,unsigned int,short);
int    CImageshortConvolute             (short*,unsigned int,unsigned int,int,short*);
void   CImageshortFill                  (short*,unsigned int,unsigned int,short);
void   CImageshortCutLower              (short*,unsigned int,unsigned int,short,short);
void   CImageshortCutUpper              (short*,unsigned int,unsigned int,short,short);
short* CImageshortGetPart               (short*,unsigned int,unsigned int,int,int,unsigned int,unsigned int);
short  CImageshortGetPixel              (short*,unsigned int,unsigned int,int,int);
void   CImageshortPutBitmapInXBM_Stream (short*,unsigned int,unsigned int,char*,void*);

void   CImageshortFilterMedian          (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,double);
void   CImageshortFilterMinimum         (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,double);
void   CImageshortFilterMaximum         (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,double);
void   CImageshortFilterBackground      (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,double,double,int);
void   CImageshortFilterLinear          (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int);
void   CImageshortApplyLowThresold      (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,short,short);
void   CImageshortApplyHighThresold     (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,short,short);
void   CImageshortOperateUnary          (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,int,short);
void   CImageshortOperateBinary         (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,int,short,short);
void   CImageshortConvoluteByMatrix     (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,int*,int,short);

void   CImageshortChangeBinning          (short*,unsigned int,unsigned int,short*,unsigned int,unsigned int,int,int,int,int,int,int,int,double,double);
void   CImageshortTransform              (short*,unsigned int,unsigned int,int,int,double);
void   CImageshortLoadTransformFile      (char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCImageshort(0); /**/
#endif

#endif  /*CImageshort_h*/
