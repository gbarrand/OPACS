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
#include <CString.h>
#include <CError.h>
#include <CList.h>
#include <CPrinter.h>
#include <CText.h>

#include <OTouch.h>

typedef struct _OTouchRecord
{
  char*    type;
  char*    date;
  char*    comment;
  char*    configure;
  char*    libs;
  char*    apps;
  char*    ifapp;
  int      removen;
  char**   removes;
  int      filen;
  char**   files;
} OTouchRecord;
 
#ifdef __cplusplus
extern "C"{
#endif
static void    FreeRecord    (OTouch);
static void    ReportError   (char*,int);
#ifdef __cplusplus
}
#endif

typedef char* OString;
static struct 
{
  OTouch* extent;
} Class = {NULL};
/***************************************************************************/
void OTouchClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy         ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent        = NULL;
}
/***************************************************************************/
OTouch* OTouchGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OTouch OTouchMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OTouch            This;
/*.........................................................................*/
  This             = (OTouch)CMemoryAllocateBlock (sizeof(OTouchRecord));
  if(This==NULL)       return NULL;
  CListAddEntry     ((CList*)&(Class.extent),This);

  This->type      = NULL;
  This->date      = NULL;
  This->comment   = NULL;
  This->configure = NULL;
  This->libs      = NULL;
  This->apps      = NULL;
  This->ifapp     = NULL;
  This->removen   = 0;
  This->removes   = NULL;
  This->filen     = 0;
  This->files     = NULL;

  return         This;
}
/***************************************************************************/
void OTouchDelete (
  OTouch This 
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
 OTouch This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete    (This->type);
  CStringDelete    (This->date);
  CStringDelete    (This->comment);
  CStringDelete    (This->configure);
  CStringDelete    (This->libs);
  CStringDelete    (This->apps);
  CStringDelete    (This->ifapp);
  CTextDelete      (This->removen,This->removes);
  CTextDelete      (This->filen  ,This->files);
  This->type       = NULL;
  This->date       = NULL;
  This->comment    = NULL;
  This->configure  = NULL;
  This->libs       = NULL;
  This->apps       = NULL;
  This->ifapp      = NULL;
  This->removen    = 0;
  This->removes    = NULL;
  This->filen      = 0;
  This->files      = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void OTouchDump (
 OTouch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CDumpF    ("type : %s\n"    ,This->type!=NULL    ? This->type    : "(nil)");
  CDumpF    ("date : %s\n"    ,This->date!=NULL    ? This->date    : "(nil)");
  CDumpF    ("comment : %s\n" ,This->comment!=NULL ? This->comment : "(nil)");
  CDumpF    ("removes : %d\n",This->removen);
  CTextDump (This->removen,This->removes);
  CDumpF    ("files : %d\n",This->filen);
  CTextDump (This->filen,This->files);
}
/***************************************************************************/
char* OTouchGetDate (
 OTouch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->date;
}
/***************************************************************************/
char* OTouchGetType (
 OTouch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->type;
}
/***************************************************************************/
char* OTouchGetComment (
 OTouch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->comment;
}
/***************************************************************************/
char* OTouchGetLibs (
 OTouch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->libs;
}
/***************************************************************************/
char* OTouchGetConfigure (
 OTouch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->configure;
}
/***************************************************************************/
char* OTouchGetApps (
 OTouch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->apps;
}
/***************************************************************************/
char* OTouchGetIfapp (
 OTouch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->ifapp;
}
/***************************************************************************/
char** OTouchGetRemove (
 OTouch This
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return NULL;
  if(a_number!=NULL) *a_number = This->removen;
  return This->removes;
}
/***************************************************************************/
char** OTouchGetFiles (
 OTouch This
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return NULL;
  if(a_number!=NULL) *a_number = This->filen;
  return This->files;
}
/***************************************************************************/
int OTouchSetAttribute (
 OTouch  This
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
  else if(strcmp(a_name,"date")==0)   
    {
      CStringDelete (This->date);
      This->date    = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"type")==0)   
    {
      CStringDelete (This->type);
      This->type    = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"comment")==0)   
    {
      CStringDelete (This->comment);
      This->comment = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"configure")==0)   
    {
      CStringDelete (This->configure);
      This->configure = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"libs")==0)   
    {
      CStringDelete (This->libs);
      This->libs = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"apps")==0)   
    {
      CStringDelete (This->apps);
      This->apps = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"ifapp")==0)   
    {
      CStringDelete (This->ifapp);
      This->ifapp = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"remove")==0)   
    {
      CTextDelete (This->removen,This->removes);
      This->removes = CTextDuplicate (a_number,*((OString**)a_addr),&This->removen);
    }
  else if(strcmp(a_name,"files")==0)   
    {
      CTextDelete (This->filen,This->files);
      This->files = CTextDuplicate (a_number,*((OString**)a_addr),&This->filen);
    }
  else 
    {
      CInfoF ("OTouchSetAttribute: unknown property %s.\n",a_name);
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
  CErrorHandle ("Co","OTouch",a_name,a_code);
}
