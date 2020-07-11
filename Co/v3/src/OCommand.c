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

#include <CPrinter.h>
#include <CError.h>
#include <CMemory.h>
#include <CList.h>
#include <CString.h>

#include <OCommandP.h>

#ifdef __cplusplus
extern "C"{
#endif
static int   GetIdentifier  (OCommand,char*);
static void  FreeRecord     (OCommand);
static void  ReportError    (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
#define NotFound    (-1)

static struct 
{
  OCommand*  extent;
} Class = {NULL};
/***************************************************************************/
void OCommandClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy         ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent        = NULL;
}
/***************************************************************************/
OCommand* OCommandGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OCommand OCommandGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OCommand)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OCommand  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OCommandGetName(This),a_name);
}
/***************************************************************************/
int OCommandIsValid (
 OCommand This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
OCommand OCommandCreate (
 char*                   a_name 
,OCommandExecuteFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCommand          This;
  char*         pos;
  char*         name;
  char*         group;
/*.........................................................................*/
  if( (a_name==NULL)  || 
      (*a_name=='\0') || 
      (a_proc==NULL) 
     ) 
    {
      ReportError("OCommandCreate",CErrorValue);
      return NULL;
    }

  group         = NULL;
  name          = CStringDuplicate(a_name);
  pos           = strchr (name,'/');  
  if(pos!=NULL)
    {
      *pos        = '\0';
      if(*(pos+1)=='\0')
	{
	  CStringDelete (name);
          group         = NULL;
          name          = NULL;
	}        
      else
	{
	  group       = name;
	  name        = CStringDuplicate(pos+1);
	}
    }
  if( (name==NULL) || (*name=='\0') ) 
    {
      CStringDelete (name);
      CWarnF        ("OCommandCreate : syntax error in :\n%s\n",a_name);
      return        NULL;
    }

  This                  = (OCommand)CMemoryAllocateBlock (sizeof(OCommandRecord));
  if(This==NULL)        return NULL;
  CListAddEntry         ((CList*)&(Class.extent),This);
  This->group           = group;
  This->name            = name;
  This->executeFunction = a_proc;
  return                This;
}
/***************************************************************************/
void OCommandDelete (
  OCommand This 
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
 OCommand This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete (This->group);
  CStringDelete (This->name);
  This->group   = NULL;
  This->name    = NULL;
  This->executeFunction    = NULL;
  CMemoryFreeBlock      (This);
}
/***************************************************************************/
char* OCommandGetName (
 OCommand This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
OCommandExecuteFunction OCommandGetExecuteFunction (
 OCommand This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->executeFunction;
}
/***************************************************************************/
char* OCommandGetGroup (
 OCommand This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->group;
}
/***************************************************************************/
int OCommandGetAttribute (
 OCommand   This
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
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0) *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"name")==0)       *((char**)a_addr)  = This->name;
  else if(strcmp(a_name,"group")==0)      *((char**)a_addr)  = This->group;
  else 
    {
      CInfoF ("OCommandGetAttribute : unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Co","OCommand",a_name,a_code);
}
