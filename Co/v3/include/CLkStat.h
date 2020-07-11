/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CLkStat_h
#define CLkStat_h

#ifdef HAS_LKLIST

#include <LkListP.h>

#else /*no HAS_LKLIST*/

#include <CLkListP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void       CLkListInit         (LkList);
static void       CLkListClear        (LkList,LkFunction);
static int        CLkListCount        (LkList);
static void       CLkListConnectEntry (LkList,LkEntry);
static LkEntry    CLkListFirstEntry   (LkList);

static LkEntry    CLkEntryCreate      (LkList,int);
static void       CLkEntryDelete      (LkEntry);
static void       CLkEntryDisconnect  (LkEntry);
static int        CLkEntryPosition    (LkEntry);
static void       CLkEntryInit        (LkEntry,LkList);
static LkList     CLkEntryFather      (LkEntry);

static LkIterator CLkIteratorCreate   (LkList);
static LkEntry    CLkIteratorNext     (LkIterator);
#ifdef __cplusplus
}
#endif

#define LkListInit         CLkListInit
#define LkListClear        CLkListClear
#define LkListCount        CLkListCount
#define LkListConnectEntry CLkListConnectEntry
#define LkListFirstEntry   CLkListFirstEntry

#define LkEntryDelete      CLkEntryDelete
#define LkEntryPosition    CLkEntryPosition
#define LkEntryInit        CLkEntryInit 
#define LkEntryDisconnect  CLkEntryDisconnect
#define LkEntryFather      CLkEntryFather

#define LkIteratorNew      CLkIteratorCreate
#define LkIteratorNext     CLkIteratorNext
#define LkIteratorDelete   CMemoryFreeBlock

#endif /*HAS_LKLIST*/

#endif /*CLkStat_h*/
