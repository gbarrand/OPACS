/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OTuple_h
#define OTuple_h

typedef struct _OTupleRecord   *OTuple;

#ifdef __cplusplus
extern "C"{
#endif
void         OTupleClearClass               ();
OTuple*      OTupleGetIdentifiers           ();
OTuple       OTupleGetIdentifier            (char*);
void         OTupleDelete                   (OTuple);
OTuple       OTupleMake                     ();
OTuple       OTupleCreate                   (char*,int,int,char*,int,char**);
int          OTupleReadFileHeader           (char*,char**,int*,char***,char***);
OTuple       OTupleLoadFile                 (char*);
void         OTupleClear                    (OTuple);
void         OTupleInitialize               (OTuple);
int          OTupleIsEmpty                  (OTuple);
void         OTupleCopy                     (OTuple,OTuple);
int          OTupleIsValid                  (OTuple);
void         OTupleSetName                  (OTuple,char*);
char*        OTupleGetName                  (OTuple);
void         OTupleSetTitle                 (OTuple,char*);
char*        OTupleGetTitle                 (OTuple);
void         OTupleSetColumns               (OTuple,int);
void         OTupleSetRows                  (OTuple,int);
void         OTupleSetLabels                (OTuple,int,char**);
void         OTupleSetIthColumnLabel        (OTuple,int,char*);
void         OTupleFillRow                  (OTuple,double*);
void         OTupleFillRowWithFloats        (OTuple,float*);
void         OTupleEndFilling               (OTuple);
void         OTupleAddRows                  (OTuple,int);
void         OTupleLimitRows                (OTuple,int);
void         OTupleFill                     (OTuple,int,int,double);
double       OTupleGetValue                 (OTuple,int,int);
double       OTupleGetValueCareless         (OTuple,int,int);
int          OTupleGetColumnIndex           (OTuple,char*);
void         OTupleSave                     (OTuple,char*);
void         OTupleDump                     (OTuple,int,int,int,int);
int          OTupleVerifyColumnIndex        (OTuple,int);
int          OTupleVerifyRowIndex           (OTuple,int);
void         OTupleSetFileName              (OTuple,char*);
char*        OTupleGetFileName              (OTuple);
char**       OTupleGetLabels                (OTuple,int*);
char*        OTupleGetIthLabel              (OTuple,int);
void         OTupleGetColumnRange           (OTuple,int,double*,double*);
int          OTupleGetNumberOfRows          (OTuple);
int          OTupleGetNumberOfColumns       (OTuple);
double*      OTupleGetRowPointer            (OTuple,int);
char*        OTupleGetInformations          (OTuple);
int          OTupleGetColumnIndexFromString (OTuple,char*);
int          OTupleSetAttribute             (OTuple,char*,void*,void*,int);
int          OTupleGetAttribute             (OTuple,char*,void*,void*,int*);
int          OTupleDoMethod                 (OTuple,char*,void*,int,char**,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOTuple(0); /**/
#endif

#endif  /*OTuple_h*/
