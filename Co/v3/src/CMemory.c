/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <CMemory.h>
#include <CPrinter.h>

#define COMEMCHECK

#define SIZE 8

#define MAXIMUM(a,b) ((a)>(b)?a:b)

static struct {
 int    allocFreeBalance;
 int    allocNumber;
 long   balance;
 long   maxAllocated;
} Class = {0,0,0L,0L};

static struct {
    int    allocFreeBalance;
    int    allocNumber;
    long   balance;
    long   maxAllocated;
} Local = {0,0,0L,0L};
/***************************************************************************/
int CMemoryGetAllocationBalance (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.allocFreeBalance;
}
/***************************************************************************/
int CMemoryGetNumberOfAllocations (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.allocNumber;
}
/***************************************************************************/
int CMemoryGetByteBalance (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.balance;
}
/***************************************************************************/
int CMemoryGetMaximumAllocatedBytes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.maxAllocated;
}
/***************************************************************************/
void* CMemoryAllocateBlock (
 size_t a_size
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  void* p = NULL;  
/*.........................................................................*/
  if(a_size<=0) {CWarn("CMemoryAllocateBlock : try to alloc 0 byte.\n");return NULL;}
#ifdef COMEMCHECK
  a_size += SIZE;
#endif
  p = malloc(a_size);  
  if(p==NULL) 
    {
      printf ("CMemoryAllocateBlock : can't alloc %lu bytes.\n",(unsigned long)a_size);

/*produce a volunteer crash to have a core and then a traceback*/
      /*      {double zero=0.;1./zero;}  */

      exit     (EXIT_FAILURE);

    }
  Class.allocFreeBalance ++;
  Class.allocNumber      ++;
#ifdef COMEMCHECK
  Class.balance      += a_size;
  Class.maxAllocated  = MAXIMUM(Class.maxAllocated,Class.balance);
  *((long*)p)      = a_size;
  return           (void*)(((char*)p)+SIZE);
#else
  return           p;
#endif
}
/***************************************************************************/
void* CMemoryChangeBlockSize (
 void* This
,size_t a_size
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  void* p = NULL;  
/*.........................................................................*/
  if(a_size<=0) {CWarn("CMemoryChangeBlockSize : try to realloc 0 byte.\n");return NULL;}
#ifdef COMEMCHECK
  if(This!=NULL)
    {
      This            = (void*)(((char*)This)-SIZE);
      Class.balance -=  *((long*)This);
    }
  a_size += SIZE;
#endif
  p = realloc(This,a_size);  
  if(p==NULL) 
    {
      printf ("CMemoryChangeBlockSize : can't realloc %lu bytes.\n",(unsigned long)a_size);

/*produce a volunteer crash to have a core and then a traceback*/
      /*      {double zero=0.;1./zero;}  */

      exit     (EXIT_FAILURE);

    }
#ifdef COMEMCHECK
  Class.balance      += a_size;
  Class.maxAllocated  = MAXIMUM(Class.maxAllocated,Class.balance);
  *((long*)p)      = a_size;
  return           (void*)(((char*)p)+SIZE);
#else
  return           p;
#endif
}
/***************************************************************************/
void CMemoryFreeBlock (
 void* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This!=NULL) 
    {
#ifdef COMEMCHECK
      This            = (void*)(((char*)This)-SIZE);
      Class.balance -=  *((long*)This);
#endif
      free             (This);
      Class.allocFreeBalance--;
    }
}
/***************************************************************************/
long CMemoryGetBlockSize (
 void* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0L; 
#ifdef COMEMCHECK
  return *(((char*)This)-SIZE);
#else
  return 0L;
#endif
}
/***************************************************************************/
void CMemorySaveStatus (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Local.allocFreeBalance = Class.allocFreeBalance;
  Local.allocNumber      = Class.allocNumber;
  Local.balance          = Class.balance;
  Local.maxAllocated     = Class.maxAllocated;
}
/***************************************************************************/
char* CMemoryGetStatus (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char string[512];
  int length,count;
/*.........................................................................*/
  string[0] = '\0';
  length    = strlen(string);

  sprintf (string+length,"\
CMemoryGetStatus :\n\
%d alloc done by Co.\n\
maximum of %ld bytes allocated by Co.\
",Class.allocNumber  - Local.allocNumber,
  Class.maxAllocated - Local.maxAllocated);
  length  = strlen(string);

  count = Class.allocFreeBalance - Local.allocFreeBalance;
       if(count>0) 
    {
      sprintf (string+length,"\n%d free missing."     , count);
      length  = strlen(string);
    }
  else if(count<0)  
    {
      sprintf (string+length,"\n%d free already done.",-count);
      length  = strlen(string);
    }

#ifdef COMEMCHECK
  count = (int)(Class.balance - Local.balance);
       if(count>0)
    {
      sprintf (string+length,"\n%d bytes not freed."    , count);
      length  = strlen(string);
    }
  else if(count<0) 
    {
      sprintf (string+length,"\n%d bytes already freed.",-count);
      length  = strlen(string);
    }
#endif

  Local.allocFreeBalance = 0;
  Local.allocNumber      = 0;
  Local.balance          = 0;
  Local.maxAllocated     = 0;
  
  return string;
}
/***************************************************************************/
char* CMemoryGetErrors (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char string[512];
  int length,count;
  int ok;
/*.........................................................................*/
  ok      = 1;

  string[0] = '\0';
  length    = strlen(string);

  count   = Class.allocFreeBalance - Local.allocFreeBalance;
       if(count>0) 
    {
      sprintf (string+length,"Co : %d free missing.\n"     , count);
      length  = strlen(string);
      ok      = 0;
    }
  else if(count<0)  
    {
      sprintf (string+length,"Co : %d free already done.\n",-count);
      length  = strlen(string);
      ok      = 0;
    }

#ifdef COMEMCHECK
  count = (int)(Class.balance - Local.balance);
       if(count>0)
    {
      sprintf (string+length,"Co : %d bytes not freed.\n"    , count);
      length  = strlen(string);
      ok      = 0;
    }
  else if(count<0) 
    {
      sprintf (string+length,"Co : %d bytes already freed.\n",-count);
      length  = strlen(string);
      ok      = 0;
    }
#endif
  if(length!=0) string[length-1] = '\0';

  Local.allocFreeBalance = 0;
  Local.allocNumber      = 0;
  Local.balance          = 0;
  Local.maxAllocated     = 0;
  
  return ok==1 ? (char*)NULL : string;
}
/***************************************************************************/
int CMemoryGetAttribute (
 void* This
,char* a_name
,void* a_user
,void* a_addr 
,int*  a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0;  

       if(strcmp(a_name,"byteBalance")==0)       
    *((int*)a_addr)   = CMemoryGetByteBalance();
  else if(strcmp(a_name,"numberOfAllocations")==0)         
    *((int*)a_addr)   = CMemoryGetNumberOfAllocations();
  else if(strcmp(a_name,"maximumAllocatedBytes")==0)
    *((int*)a_addr)   = CMemoryGetMaximumAllocatedBytes();
  else 
    {
      CInfoF ("CMemoryGetAttribute : unknown property %s.\n",a_name);
      return 0;
    }

  This   = NULL;
  a_user = NULL;
  return 1;
}
/***************************************************************************/
void CMemoryFreeVariable (
 void* a_addr
,int   a_number
,int   a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_addr==NULL) return;
  if(a_type==FREE_BLOCK)
    {
      void*            addr;
      addr             = *((void**)a_addr);
      CMemoryFreeBlock (addr);
    }
  else if(a_type==FREE_BLOCKS)
    {
       int count;
      void**           list;
      list             = *((void***)a_addr);
      for(count=0;count<a_number;count++) CMemoryFreeBlock(list[count]);
      CMemoryFreeBlock (list);
    }
}


