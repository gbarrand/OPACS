/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CLkListP_h
#define CLkListP_h

typedef struct _LkListRec*     LkList;
typedef struct _LkEntryRec*    LkEntry;
typedef struct _LkIteratorRec* LkIterator;

typedef void   (*LkFunction)(LkEntry);

#define LkListItems firstEntry,lastEntry

typedef struct _LkListRec {
  LkEntry LkListItems;
} LkListRec;

#define LkEntryItems prevEntry,nextEntry;LkList fatherList

typedef struct _LkEntryRec {
  LkEntry LkEntryItems;
} LkEntryRec;

typedef struct _LkIteratorRec {
  LkList  list;
  LkEntry current;
} LkIteratorRec;

/*-----------------------------------------------------------------------*/
#define _LkListFirstEntry(This)           (This->firstEntry)
#define _LkListLastEntry(This)            (This->lastEntry)
#define _LkListSetFirstEntry(This, entry) (This->firstEntry = entry)
#define _LkListSetLastEntry(This, entry)  (This->lastEntry = entry)
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
#define _LkEntryPrevious(This)            (This->prevEntry)
#define _LkEntryNext(This)                (This->nextEntry)
#define _LkEntryFather(This)              (This->fatherList)

#define _LkEntrySetPrevious(This, entry)  (This->prevEntry = entry)
#define _LkEntrySetNext(This, entry)      (This->nextEntry = entry)
#define _LkEntrySetFather(This, list)     (This->fatherList = list)
/*-----------------------------------------------------------------------*/

#endif /*CLkList_h*/
