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
#include <stdlib.h>
#include <stdio.h>

#include <CMemory.h>
#include <CString.h>
#include <CError.h>
#include <CList.h>
#include <CPrinter.h>
#include <CText.h>

#include <OModule.h>

typedef struct _OModuleRecord
{
  char*    name;
  char*    type;
  char*    language;
  char*    extension;
  char*    inLib;
  char*    directory;
  int      beforeLibraries;
  int      distributed;
  int      isANSI;
  int      incn;
  char**   incs;
  int      objn;
  char**   objs;
  int      libn;
  char**   libs;
  int      hasn;
  char**   hass;
} OModuleRecord;
 
#ifdef __cplusplus
extern "C"{
#endif
static void    FreeRecord   (OModule);
static int     GetIdentifier (OModule,char*);
static void    ReportError  (char*,int);
#ifdef __cplusplus
}
#endif

typedef char* OString;
static struct 
{
  OModule* extent;
} Class = {NULL};
/***************************************************************************/
void OModuleClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy         ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent        = NULL;
}
/***************************************************************************/
OModule* OModuleGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OModule OModuleGetIdentifier (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OModule)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OModule  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OModuleGetName(This),a_name);
}
/***************************************************************************/
OModule OModuleMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OModule            This;
/*.........................................................................*/
  This             = (OModule)CMemoryAllocateBlock (sizeof(OModuleRecord));
  if(This==NULL)       return NULL;
  CListAddEntry     ((CList*)&(Class.extent),This);

  This->name        = NULL;
  This->type        = CStringDuplicate("src");
  This->language    = CStringDuplicate("c");
  This->extension   = NULL;
  This->inLib       = CStringDuplicate("none");
  This->directory   = CStringDuplicate("$(src)");
  This->beforeLibraries = 0;
  This->distributed     = 1;
  This->isANSI          = 1;
  This->incn            = 0;
  This->incs            = NULL;
  This->objn            = 0;
  This->objs            = NULL;
  This->libn            = 0;
  This->libs            = NULL;
  This->hasn            = 0;
  This->hass            = NULL;

  return         This;
}
/***************************************************************************/
void OModuleDelete (
  OModule This 
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
 OModule This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete    (This->name);
  CStringDelete    (This->type);
  CStringDelete    (This->language);
  CStringDelete    (This->extension);
  CStringDelete    (This->inLib);
  CStringDelete    (This->directory);
  CTextDelete      (This->incn,This->incs);
  CTextDelete      (This->objn,This->objs);
  CTextDelete      (This->libn,This->libs);
  CTextDelete      (This->hasn,This->hass);
  This->name       = NULL;
  This->type       = NULL;
  This->language   = NULL;
  This->extension  = NULL;
  This->inLib      = NULL;
  This->directory  = NULL;
  This->incn       = 0;
  This->incs       = NULL;
  This->objn       = 0;
  This->objs       = NULL;
  This->libn       = 0;
  This->libs       = NULL;
  This->hasn       = 0;
  This->hass       = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void OModuleDump (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CDumpF    ("name : %s\n" ,This->name!=NULL ? This->name : "(nil)");
  CDumpF    ("type : %s\n" ,This->type);
  CDumpF    ("language : %s\n" ,This->language);
  CDumpF    ("extension : %s\n" ,This->extension);
  CDumpF    ("inLib : %s\n",This->inLib);
  CDumpF    ("directory : %s\n",This->directory);
  CDumpF    ("incs : %d\n",This->incn);
  CTextDump (This->incn,This->incs);
  CDumpF    ("objs : %d\n",This->objn);
  CTextDump (This->objn,This->objs);
  CDumpF    ("libs : %d\n",This->libn);
  CTextDump (This->libn,This->libs);
  CDumpF    ("hass : %d\n",This->hasn);
  CTextDump (This->hasn,This->hass);
}
/***************************************************************************/
char* OModuleGetName (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
char* OModuleGetLanguage (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->language;
}
/***************************************************************************/
char* OModuleGetExtension (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char string[3] = "c";
/*.........................................................................*/
  if(This==NULL) return NULL;
  if(This->extension==NULL)
    {
      if(CStringCompare(This->language,"c++")==1)
	{
	  sprintf (string,"%s","cxx");
	  return  string;
	}
      else if(CStringCompare(This->language,"f77")==1)
	{
	  sprintf (string,"%s","f");
	  return  string;
	}
      else if(CStringCompare(This->language,"rc")==1)
	{
	  sprintf (string,"%s","RC");
	  return  string;
	}
      else /*Default language is C.*/
	{
	  sprintf (string,"%s","c");
	  return  string;
	}
    }
  else
    {
      return This->extension;
    }
}
/***************************************************************************/
char* OModuleGetDirectory (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->directory;
}
/***************************************************************************/
char** OModuleGetOBJ_Files (
 OModule This
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return NULL;
  if(a_number!=NULL) *a_number = This->objn;
  return This->objs;
}
/***************************************************************************/
char** OModuleGetIncludeFiles (
 OModule This
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return NULL;
  if(a_number!=NULL) *a_number = This->incn;
  return This->incs;
}
/***************************************************************************/
char** OModuleGetLibraries (
 OModule This
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return NULL;
  if(a_number!=NULL) *a_number = This->libn;
  return This->libs;
}
/***************************************************************************/
char** OModuleGetHasFlags (
 OModule This
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return NULL;
  if(a_number!=NULL) *a_number = This->hasn;
  return This->hass;
}
/***************************************************************************/
int OModuleIsLibrary (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(This->type==NULL) return 0;
  if(strncmp(This->type,"a",1)==0)  return 1;
  if(strncmp(This->type,"ar",2)==0) return 1;
  if(strncmp(This->type,"sl",2)==0) return 1;
  if(strncmp(This->type,"so",2)==0) return 1;
  return 0;
}
/***************************************************************************/
int OModuleIsSharedLibrary (
 OModule This
,char* a_system
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     wordn  = 0;
  char**  words  = NULL;
  int     status = 0;
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(This->type==NULL) return 0;
  words = CStringGetWords (This->type,";",&wordn);
  if(wordn==1)
    {
           if(strcmp(words[0],"sl")==0) status = 1;
      else if(strcmp(words[0],"so")==0) status = 1;
    }
  else if( (wordn==2) && (strlen(words[1])==5) )
    {
      if( (strcmp(a_system,"NT")==0) &&
	  (strncmp(words[1],"NT",2)==0) )
	{
	       if(strcmp(words[1]+3,"sl")==0) status = 1;
	  else if(strcmp(words[1]+3,"so")==0) status = 1;
	}
      else
	{
	       if(strcmp(words[0],"sl")==0) status = 1;
	  else if(strcmp(words[0],"so")==0) status = 1;
	}
    }
  CTextDelete (wordn,words);
  return      status;
}
/***************************************************************************/
int OModuleIsSourceFile (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(This->type==NULL) return 0;
  if(strcmp(This->type,"src")==0) return 1;
  return 0;
}
/***************************************************************************/
int OModuleIsApplication (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(This->type==NULL) return 0;
  if(strcmp(This->type,"bin")==0)         return 1;
  if(strcmp(This->type,"app")==0)         return 1;
  if(strcmp(This->type,"application")==0) return 1;
  return 0;
}
/***************************************************************************/
int OModuleIsDistributed (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return This->distributed;
}
/***************************************************************************/
int OModuleIsANSI (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return This->isANSI;
}
/***************************************************************************/
int OModuleIsInLibrary (
 OModule This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)        return 0;
  if(a_name==NULL)      return 0;
  if(This->inLib==NULL) return 0;
  if(strcmp(This->inLib,a_name)==0) return 1;
  return 0;
}
/***************************************************************************/
int OModuleIsBeforeLibraries (
 OModule This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->beforeLibraries;
}
/***************************************************************************/
int OModuleSetAttribute (
 OModule  This
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
    {
      CStringDelete (This->name);
      This->name    = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"type")==0)   
    {
      CStringDelete (This->type);
      This->type    = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"language")==0)   
    {
      CStringDelete  (This->language);
      This->language = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"extension")==0)   
    {
      CStringDelete  (This->extension);
      This->extension = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"inLib")==0)   
    {
      CStringDelete  (This->inLib);
      This->inLib    = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"directory")==0)   
    {
      CStringDelete   (This->directory);
      This->directory = CStringDuplicate(*((char**)a_addr));
    }
  else if(strcmp(a_name,"incs")==0)   
    {
      CTextDelete  (This->incn,This->incs);
      This->incs   = CTextDuplicate (a_number,*((OString**)a_addr),&This->incn);
    }
  else if(strcmp(a_name,"ifhas")==0)   
    {
      CTextDelete  (This->hasn,This->hass);
      This->hass   = CTextDuplicate (a_number,*((OString**)a_addr),&This->hasn);
    }
  else if(strcmp(a_name,"objs")==0)   
    {
      CTextDelete  (This->objn,This->objs);
      This->objs   = CTextDuplicate (a_number,*((OString**)a_addr),&This->objn);
    }
  else if(strcmp(a_name,"libs")==0)   
    {
      CTextDelete  (This->libn,This->libs);
      This->libs   = CTextDuplicate (a_number,*((OString**)a_addr),&This->libn);
    }
  else if(strcmp(a_name,"beforeLibraries")==0)   
    {
      This->beforeLibraries = *((int*)a_addr);
    }
  else if(strcmp(a_name,"distributed")==0)   
    {
      This->distributed = *((int*)a_addr);
    }
  else if(strcmp(a_name,"isANSI")==0)   
    {
      This->isANSI = *((int*)a_addr);
    }
  else 
    {
      CInfoF ("OModuleSetAttribute: unknown property %s.\n",a_name);
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
  CErrorHandle ("Co","OModule",a_name,a_code);
}
