/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CMemory_h
#define CMemory_h

#define FREE_BLOCK  2
#define FREE_BLOCKS 3

#ifndef NO_C_H
#include <stddef.h>   
#endif
#ifdef __cplusplus
extern "C"{
#endif
int    CMemoryGetAllocationBalance     ();
int    CMemoryGetNumberOfAllocations   ();
int    CMemoryGetByteBalance           ();
int    CMemoryGetMaximumAllocatedBytes ();
void*  CMemoryAllocateBlock            (size_t);
void*  CMemoryChangeBlockSize          (void*,size_t);
void   CMemoryFreeBlock                (void*);
long   CMemoryGetBlockSize             (void*);
void   CMemorySaveStatus               ();
char*  CMemoryGetStatus                ();
char*  CMemoryGetErrors                ();
int    CMemoryGetAttribute             (void*,char*,void*,void*,int*);
void   CMemoryFreeVariable             (void*,int,int);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCMemory(0); /**/
#endif

#endif  /*CMemory_h*/
