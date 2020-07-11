/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CList_h
#define CList_h

#ifndef NO_C_H
#include <stdarg.h>
#endif

typedef void** CList;
typedef void*  CEntry;
typedef void   (*CEntryFunction)();
typedef void   (*CListCountLeafFunction)(CEntry,int*);
typedef void   (*CListVisitLeafFunction)(CEntry,int*,CList);
typedef void   (*CListVisitEntryFunction)(CEntry);
typedef void   (*CListVisitEntryV_Function)(CEntry,va_list);
typedef int    (*CListTestEntryFunction)(CEntry);
typedef int    (*CListTestEntryV_Function)(CEntry,va_list);
typedef int    (*CListFindEntryFunction)(CEntry,void*);
typedef int    (*CListMatchEntryFunction)(CEntry,CEntry);
typedef int    (*CListSearchEntryFunction)(CEntry,va_list);

#ifdef __cplusplus
extern "C"{
#endif
CList  CListCreate            (int);
CList  CListCreateFromTree    (CEntry,CListCountLeafFunction,CListVisitLeafFunction);
void   CListDelete            (CList);
void   CListDestroy           (CList,CListVisitEntryFunction);
void   CListVisit             (CList,CListVisitEntryFunction);
void   CListVisitE            (CList,CListVisitEntryV_Function,...);
CList  CListExtract           (CList,CListTestEntryFunction);
CList  CListExtractE          (CList,CListTestEntryV_Function,...);
int    CListGetSize           (CList);
int    CListIsEmpty           (CList);
void   CListLimit             (CList,int);
CList  CListDuplicate         (CList);
int    CListCompare           (CList,CList);
CEntry CListFindEntry         (CList,CListFindEntryFunction,void*);
CEntry CListFindEntryBackward (CList,CListFindEntryFunction,void*);
CEntry CListSearchEntry       (CList,CListSearchEntryFunction,...);
CEntry CListGetIthEntry       (CList,int);
void   CListRemoveEntry       (CList,CEntry);
void   CListRemoveDuplicates  (CList,CListMatchEntryFunction,int);
int    CListGetEntryIndex     (CList,CEntry,CListMatchEntryFunction);

void   CListAddEntry          (CList*,CEntry);
void   CListAddEntryFunction  (CList*,CEntryFunction);
void   CListPrependEntry      (CList*,CEntry);
void   CListConcatenate       (CList*,CList);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCList(0); /**/
#endif

#endif  /*CList_h*/
