/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OPointList_h
#define OPointList_h
 
typedef struct _OPointListRecord *OPointList;

#ifdef __cplusplus
extern "C"{
#endif
OPointList  OPointListMake         ();
OPointList  OPointListCreate       (int);
void        OPointListDelete       (OPointList);
void        OPointListInitialize   (OPointList,int);
void        OPointListClear        (OPointList);
void        OPointListAddEntry     (OPointList,double,double,double);
void        OPointListDump         (OPointList);
void        OPointListLimit        (OPointList,int);
OPointList  OPointListDuplicate    (OPointList);
void        OPointListCopy         (OPointList,OPointList);
void        OPointListReverse      (OPointList);
void        OPointListSetIthEntry  (OPointList,int,double,double,double);
void        OPointListCopyEntry    (OPointList,int,int);
int         OPointListGetSize      (OPointList);
void        OPointListGetIthEntry  (OPointList,int,double*,double*,double*);
double      OPointListGetIthEntryX (OPointList,int);
double      OPointListGetIthEntryY (OPointList,int);
double      OPointListGetIthEntryZ (OPointList,int);
int         OPointListCompare      (OPointList,OPointList);
double*     OPointListGetListOfX   (OPointList);
double*     OPointListGetListOfY   (OPointList);
double*     OPointListGetListOfZ   (OPointList);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOPointList(0); /**/
#endif

#endif  /*OPointList_h*/
