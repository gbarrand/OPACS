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

#include <CMemory.h>
#include <CPrinter.h>
#include <CList.h>
#include <CString.h>
#include <CError.h>

#include <OMatrix33P.h>
#ifdef __cplusplus
extern "C"{
#endif
static void  FreeRecord      (OMatrix33);
static int   GetIdentifier   (OMatrix33,char*);
static void  testClass       ();
static int   testObject      (OMatrix33);
static void  ReportError     (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
#define MVAL(a_elm,a_row,a_col) (a_elm[a_row * 3 + a_col])

static struct 
{
  OMatrix33* extent;
  int      ints[9];
} Class = {NULL};
/***************************************************************************/
void OMatrix33ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy         ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent        = NULL;
}
/***************************************************************************/
OMatrix33* OMatrix33GetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OMatrix33 OMatrix33GetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OMatrix33)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OMatrix33 This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OMatrix33GetName(This),a_name);
}
/***************************************************************************/
OMatrix33 OMatrix33Make (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix33 This;
/*.........................................................................*/
  This              = (OMatrix33) CMemoryAllocateBlock (sizeof(OMatrix33Record));
  if(This==NULL)        return NULL;

  CListAddEntry      ((CList*)&(Class.extent),This);

  This->name        = NULL;
  This->elm         = (int*)CMemoryAllocateBlock(9*sizeof(int));
  OMatrix33Initialize        (This,1,0,0, 0,1,0, 0,0,1, 1);
  This->invsc       = 1;

  return            This;
}
/***************************************************************************/
OMatrix33 OMatrix33Create (
 char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix33 This;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0')) 
    {
      ReportError("OMatrix33Make",CErrorValue);
      return NULL;
    }

  This              = OMatrix33Make();
  if(This==NULL)    return NULL;

  This->name        = CStringDuplicate(a_name);

  return            This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void FreeRecord (
 OMatrix33 This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete (This->name);
  CMemoryFreeBlock      (This->elm);
  This->name    = NULL;
  This->elm     = NULL;
  CMemoryFreeBlock      (This);
}
/***************************************************************************/
void OMatrix33Delete (
  OMatrix33 This 
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
char* OMatrix33GetName (
  OMatrix33 This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return NULL;
  return This->name;
}
/***************************************************************************/
void OMatrix33SetName (
 OMatrix33 This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)      return;
  if( (a_name==NULL) || (*a_name=='\0') ) 
    {
      ReportError("OMatrix33SetName",CErrorValue);
      return;
    }
  CStringDelete  (This->name);
  This->name     = CStringDuplicate(a_name);
}
/******************************************************************************/
void OMatrix33Initialize (
 OMatrix33 This 
,int a_m11 
,int a_m12 
,int a_m13 
,int a_m21 
,int a_m22 
,int a_m23 
,int a_m31 
,int a_m32 
,int a_m33 
,int a_isc 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*............................................................................*/
  if(This==NULL)      return;
  if(This->elm==NULL) return;

  MVAL(This->elm,0,0) = a_m11;
  MVAL(This->elm,0,1) = a_m12;
  MVAL(This->elm,0,2) = a_m13;

  MVAL(This->elm,1,0) = a_m21;
  MVAL(This->elm,1,1) = a_m22;
  MVAL(This->elm,1,2) = a_m23;

  MVAL(This->elm,2,0) = a_m31;
  MVAL(This->elm,2,1) = a_m32;
  MVAL(This->elm,2,2) = a_m33;

  This->invsc         = a_isc;
}
/******************************************************************************/
int* OMatrix33GetElements (
 OMatrix33 This 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*............................................................................*/
  if(This==NULL) return NULL;
  return This->elm;
}
/******************************************************************************/
int OMatrix33GetScale (
 OMatrix33 This 
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*............................................................................*/
  if(This==NULL) return 1;
  return This->invsc;
}
/***************************************************************************/
int OMatrix33SetAttribute (
 OMatrix33  This
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

       if(strcmp(a_name,"identifier")==0)        {}
  else if(strcmp(a_name,"name")==0)       OMatrix33SetName      (This,*((char**)a_addr));
  else if(strcmp(a_name,"values")==0)
    {
      if(a_number!=9)
	{
	  CWarnF ("OMatrix33SetAttribute:For property %s, 9 values are requested.\n",a_name);
	  return 0;
	}
      else
	{
	  int* list;
	  list = *((int**)a_addr);
          if(This->elm!=NULL)
	    {
	      MVAL(This->elm,0,0) = list[0];
	      MVAL(This->elm,0,1) = list[1];
	      MVAL(This->elm,0,2) = list[2];

	      MVAL(This->elm,1,0) = list[3];
	      MVAL(This->elm,1,1) = list[4];
	      MVAL(This->elm,1,2) = list[5];

	      MVAL(This->elm,2,0) = list[6];
	      MVAL(This->elm,2,1) = list[7];
	      MVAL(This->elm,2,2) = list[8];
	    }
	}
    }
  else if(strcmp(a_name,"scale")==0)      This->invsc = *((int*)a_addr);
  else 
    {
      CInfoF ("OMatrix33SetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OMatrix33GetAttribute (
 OMatrix33  This
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
  if(a_addr==NULL)  return 0;  
  if(a_name==NULL)  return 0;
  if(This==NULL)    return 0;

       if(strcmp(a_name,"identifier")==0) *((Ulong*)a_addr)       = (Ulong)This;
  else if(strcmp(a_name,"name")==0)       *((char**)a_addr)       = This->name;
  else if(strcmp(a_name,"scale")==0)      *((int*)a_addr)         = This->invsc;
  else if(strcmp(a_name,"values")==0)
    {
      if(This->elm!=NULL)
	{
	  Class.ints[0]  = MVAL(This->elm,0,0);
	  Class.ints[1]  = MVAL(This->elm,0,1);
	  Class.ints[2]  = MVAL(This->elm,0,2);

	  Class.ints[3]  = MVAL(This->elm,1,0);
	  Class.ints[4]  = MVAL(This->elm,1,1);
	  Class.ints[5]  = MVAL(This->elm,1,2);

	  Class.ints[6]  = MVAL(This->elm,2,0);
	  Class.ints[7]  = MVAL(This->elm,2,1);
	  Class.ints[8]  = MVAL(This->elm,2,2);

	  if(a_number!=NULL)   *a_number = 9;
	  *((int**)a_addr) = Class.ints;
        }
      else
	{
	  if(a_number!=NULL) *a_number = 0;
	  *((int**)a_addr) = NULL;
	}        
    }
  else 
    {
      CInfoF ("OMatrix33GetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;  
}
/***************************************************************************/
int OMatrix33DoMethod (
 OMatrix33 This
,char*  a_name
,void*  a_user
,int    a_argn
,char** a_args
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;

  if(This==NULL) 
    {
      if(strcmp(a_name,"testClass")==0)  
	{
	  testClass();
	}
      else 
	{
	  CInfoF ("OMatrix33DoMethod: %s is not a class property.\n",a_name);
	  return 0;
	}
      return 1;
    }


      if(strcmp(a_name,"testObject")==0)    
    {
      *((int*)a_addr) = testObject(This);
    }
  else 
    {
      CInfoF ("OMatrix33DoMethod: %s is not a routine property.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_argn   = 0;
  a_args   = NULL;
  a_addr   = NULL;
  a_number = NULL;
  return   1;
}
/***************************************************************************/
static void testClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CInfoF("In classFunction.\n");
}
/***************************************************************************/
static int testObject (
 OMatrix33 This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  CInfoF ("In objectFunction for %lu.\n",This);
  return 33;
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
  CErrorHandle ("Co","OMatrix33",a_name,a_code);
}
