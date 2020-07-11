/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CList.h>
#include <CString.h>
#include <CError.h>

#include <OLUTP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  FreeRecord      (OLUT);
static int   GetIdentifier    (OLUT,char*);
static void  ReportError     (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned char Uchar;
typedef unsigned long Ulong;
#define XtNumber(arr)  (sizeof(arr) / sizeof(arr[0]))
#define MINIMUM(a,b)   ((a)<(b)?a:b)
#define NotFound       (-1)

static struct 
{
  OLUT* extent;
  char* types[3];
} Class = {NULL,{"identity","linear","log"}};
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OLUT_ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy         ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent        = NULL;
}
/***************************************************************************/
OLUT* OLUT_GetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OLUT OLUT_GetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OLUT)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OLUT  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OLUT_GetName(This),a_name);
}
/***************************************************************************/
OLUT OLUT_Make (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OLUT          This;
/*.........................................................................*/
  This           = (OLUT)CMemoryAllocateBlock (sizeof(OLUT_Record));
  if(This==NULL) return NULL;
  CListAddEntry  ((CList*)&(Class.extent),This);
  This->name     = NULL;
  This->list     = NULL;
  This->number   = 0;
  This->type     = OLUT_Identity;
  This->min      = 0;
  This->max      = 0;
  This->nbin     = 0;
  return         This;
}
/***************************************************************************/
OLUT OLUT_Create (
 char*  a_name
,int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OLUT          This;
/*.........................................................................*/
  This           = OLUT_Make();
  if(This==NULL) return NULL;
  This->name     = CStringDuplicate(a_name);

  if(a_number!=0)
    {
      This->list           = (Uchar*)CMemoryAllocateBlock(a_number*sizeof(Uchar));
      if(This->list!=NULL) This->number    = a_number;
    }

  return        This;
}
/***************************************************************************/
void OLUT_Delete (
  OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  CListRemoveEntry  ((CList)Class.extent,This);
  FreeRecord       (This);
}
/***************************************************************************/
static void FreeRecord (
 OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete (This->name);
  This->name    = NULL;
  CMemoryFreeBlock      (This->list);
  This->list    = NULL;
  This->number  = 0;
  CMemoryFreeBlock      (This);
}
/***************************************************************************/
int OLUT_IsValid (
 OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
char* OLUT_GetName (
 OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OLUT_Initialize (
 OLUT This 
,int a_type 
,int a_min 
,int a_max 
,int a_nbin 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int i,j;
  int px,dpx;
/*.........................................................................*/
  if(This==NULL)        return;
  if(a_min>=a_max) return;
  if(a_min<0) return;
  if(a_max<0) return;
  if( (This->list==NULL) || (a_max>This->number) )
    {
      CMemoryFreeBlock       (This->list);
      This->list     = NULL;
      This->number   = 0;
      This->list     = (Uchar*)CMemoryAllocateBlock(a_max * sizeof(Uchar));
      if(This->list!=NULL) This->number = a_max;
    }
  if(This->list==NULL)  return;


  if(a_type==OLUT_Identity)
    { 
      for(i=0;i<This->number;i++) This->list[i] = (Uchar)i;
    }
  else if(a_type==OLUT_Linear)
    { 
      if((a_nbin<=0)||(a_nbin>254)) return;  /* a_nbin in [1,254] */
      for(i=0;i<a_min;i++) This->list[i] = (Uchar)0;
      px = a_min;
      dpx = (a_max-a_min)/a_nbin;
      if(dpx<=0) return;
      for(j=1;j<=a_nbin;j++)      
        {
          for(i=px;i<px+dpx;i++) This->list[i] = (Uchar)j;
          px += dpx;
        }
      for(i=px;i<This->number;i++) This->list[i] = (Uchar)(a_nbin+1);
    }
  else if(a_type==OLUT_Log)
    {
      double fpx,fmulpx;
      if((a_nbin<=0)||(a_nbin>254)) return;  /* a_nbin in [1,254] */
      for(i=0;i<a_min;i++) This->list[i] = (Uchar)0;
      px     = a_min;
      fpx    = (double)(a_max)/(double)(a_min);
      fmulpx = pow( (double)(10.0) , log10(fpx)/(double)(a_nbin)  );
      fpx    = (double)(a_min);
      for(j=0;j<a_nbin;j++)
        {
          fpx *= fmulpx;
          dpx  = (int)(fpx)-px;
          for(i=px;i<px+dpx;i++) This->list[i] = (Uchar)j;
          px  += dpx;
        }
      for(i=px;i<This->number;i++) This->list[i] = (Uchar)(a_nbin+1);
    }
  This->type = a_type;
  This->min     = a_min;  
  This->max     = a_max;
  This->nbin    = a_nbin;
}
/***************************************************************************/
Uchar* OLUT_GetList (
 OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->list;
}
/***************************************************************************/
int OLUT_GetSize (
 OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->number;
}
/***************************************************************************/
int OLUT_GetMinimum (
 OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->min;
}
/***************************************************************************/
int OLUT_GetMaximum (
 OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->max;
}
/***************************************************************************/
int OLUT_GetNumberOfBins (
 OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->nbin;
}
/***************************************************************************/
int OLUT_GetType (
 OLUT This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OLUT_Identity;
  return This->type;
}
/***************************************************************************/
void OLUT_SetName (
 OLUT This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)      return;
  CStringDelete  (This->name);
  This->name     = CStringDuplicate(a_name);
}
/***************************************************************************/
void OLUT_SetType (
 OLUT This 
,int a_type 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->type = a_type;
  OLUT_Initialize    (This,This->type,This->min,This->max,This->nbin);
}
/***************************************************************************/
void OLUT_SetMinimum (
 OLUT This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->min = a_value;
  OLUT_Initialize   (This,This->type,This->min,This->max,This->nbin);
}
/***************************************************************************/
void OLUT_SetMaximum (
 OLUT This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->max = a_value;
  OLUT_Initialize   (This,This->type,This->min,This->max,This->nbin);
}
/***************************************************************************/
void OLUT_SetNumberOfBins (
 OLUT This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->nbin = a_value;
  OLUT_Initialize    (This,This->type,This->min,This->max,This->nbin);
}
/***************************************************************************/
void OLUT_SetSize (
 OLUT This 
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  CMemoryFreeBlock     (This->list);
  This->list   = NULL;
  This->number = 0;
  if(a_number!=0)
    {
      This->list           = (Uchar*)CMemoryAllocateBlock(a_number*sizeof(Uchar));
      if(This->list!=NULL) This->number    = a_number;
    }
  OLUT_Initialize    (This,This->type,This->min,This->max,This->nbin);
}
/***************************************************************************/
int OLUT_SetAttribute (
 OLUT  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0)        
	 {}
  else if(strcmp(a_name,"name")==0)       
    OLUT_SetName      (This,*((char**)a_addr));
  else if(strcmp(a_name,"type")==0)
    {
       char*  string;
       string = *((char**)a_addr);
            if( (string!=NULL) && (strcmp(string,"identity")==0) )  OLUT_SetType      (This,OLUT_Identity);
       else if( (string!=NULL) && (strcmp(string,"linear")==0)   )  OLUT_SetType      (This,OLUT_Linear);
       else if( (string!=NULL) && (strcmp(string,"log")==0)      )  OLUT_SetType      (This,OLUT_Log);
    }
  else if(strcmp(a_name,"minimum")==0)      OLUT_SetMinimum       (This,*((int*)a_addr));
  else if(strcmp(a_name,"maximum")==0)      OLUT_SetMaximum       (This,*((int*)a_addr));
  else if(strcmp(a_name,"numberOfBins")==0) OLUT_SetNumberOfBins      (This,*((int*)a_addr));
  else if(strcmp(a_name,"size")==0)         OLUT_SetSize    (This,*((int*)a_addr));
  else 
    {
      CInfoF ("OLUT_SetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int OLUT_GetAttribute (
 OLUT  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0)     *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"name")==0)           *((char**)a_addr)  = This->name;
  else if(strcmp(a_name,"type")==0)           *((char**)a_addr)  = Class.types[This->type];
  else if(strcmp(a_name,"minimum")==0)        *((int*)a_addr)    = This->min;
  else if(strcmp(a_name,"maximum")==0)        *((int*)a_addr)    = This->max;
  else if(strcmp(a_name,"numberOfBins")==0)   *((int*)a_addr)    = This->nbin;
  else if(strcmp(a_name,"size")==0)           *((int*)a_addr)    = This->number;
  else 
    {
      CInfoF ("OLutGetProp: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Co","OLUT",a_name,a_code);
}
