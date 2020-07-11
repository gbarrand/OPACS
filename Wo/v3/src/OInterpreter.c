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
#include <CString.h>
#include <CList.h>
#include <CError.h>

#include <OInterpreterP.h>

#ifdef __cplusplus
extern "C"{
#endif
static char*   GetScripts    (char*,OInterpreter*);
static void    FreeRecord    (OInterpreter);
static int     GetIdentifier (OInterpreter,char*);
static void    ReportError   (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;

static struct 
{
  OInterpreter*   extent;
} Class = {NULL}; 
/***************************************************************************/
void OInterpreterClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy    ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent   = NULL;
}
/***************************************************************************/
OInterpreter* OInterpreterGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OInterpreter OInterpreterGetReference  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OInterpreter)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OInterpreter  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OInterpreterGetName(This),a_name);
}
/***************************************************************************/
OInterpreter OInterpreterMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OInterpreter This;
/*.........................................................................*/
  This              = (OInterpreter)CMemoryAllocateBlock (sizeof(OInterpreterRecord));
  if(This==NULL)    return NULL;
  CListAddEntry     ((CList*)&(Class.extent),This);

  This->name        = NULL;
  This->execProc    = NULL;
  This->clearProc   = NULL;
  This->syntaxProc  = NULL;
  This->cbkProc     = NULL;

  return This;
}
/***************************************************************************/
OInterpreter OInterpreterCreate (
 char*    a_name
,OInterpreterDoFunction a_exec
,OInterpreterVoidFunction a_clear
,OInterpreterDoFunction a_syntax
,OInterpreterDoFunction a_cbk
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OInterpreter This;
/*.........................................................................*/
  This              = OInterpreterMake();
  if(This==NULL)    return NULL;

  This->name        = CStringDuplicate(a_name);
  This->execProc    = a_exec;
  This->clearProc   = a_clear;
  This->syntaxProc  = a_syntax;
  This->cbkProc     = a_cbk;

  return This;
}
/***************************************************************************/
OInterpreter OInterpreterAddInterpreter (
 char* a_prompt
,OInterpreterDoFunction a_proc
)
/***************************************************************************/ 
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return OInterpreterCreate (a_prompt,a_proc,NULL,NULL,NULL);
}
/***************************************************************************/
char* OInterpreterGetName (
 OInterpreter This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
int OInterpreterGetAttribute (
 OInterpreter    This
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

       if(strcmp(a_name,"identifier")==0) *((Ulong*)a_addr)    = (Ulong)This;
  else if(strcmp(a_name,"name")==0)       *((char**)a_addr)    = This->name;
  else 
    {
      CInfoF("OInterpreterGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OScriptIsBackgroundScript (
 char*  a_string 
,char** a_pos
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if(a_pos!=NULL) *a_pos = NULL;
  if( (a_string==NULL) || (*a_string=='\0') ) return 0;
  pos = CStringGetFirstNotSpacePosition (a_string);
  if(pos==NULL) return 0;
  if(*pos!='&') return 0;
  if(a_pos!=NULL) *a_pos = pos+1;
  return 1;
}
/***************************************************************************/
void OScriptExecute (
 char* a_string   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*   script;
  OInterpreter This;
/*.........................................................................*/
  script    = GetScripts (a_string,&This);
  if(This==NULL) return;
  if(This->execProc!=NULL) This->execProc(script);
}
/***************************************************************************/
void OScriptVerifySyntax (
 char* a_string   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*     script;
  OInterpreter   This;
/*.........................................................................*/
  script    = GetScripts (a_string,&This);
  if(This==NULL) return;
  if(This->syntaxProc==NULL) return;
  This->syntaxProc(script);
}
/***************************************************************************/
void OScriptGenerateCallbackCode (
 char* a_string   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*       script;
  OInterpreter   This;
/*.........................................................................*/
  script    = GetScripts (a_string,&This);
  if(This==NULL) return;
  if(This->cbkProc!=NULL) This->cbkProc(script);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void FreeRecord (
 OInterpreter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This->clearProc!=NULL)  This->clearProc();
  This->clearProc      = NULL;
  CStringDelete        (This->name);
  This->name           = NULL;
  This->execProc       = NULL;
  This->syntaxProc     = NULL;
  This->cbkProc        = NULL;
  CMemoryFreeBlock     (This);
}
/***************************************************************************/
static char* GetScripts (
 char* a_string
,OInterpreter* a_This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int          lspace;
  char*        pos;
  size_t       lpos;
  char         prompt[32] = "";
  OInterpreter This;
/*.........................................................................*/
  if(a_This)  *a_This = NULL;
  if( (a_string==NULL) || (*a_string=='\0') ) return NULL;
  lspace        = strspn  (a_string," ");
  pos           = strstr  (a_string+lspace,">");
  if(pos==NULL) return NULL;
  lpos          = strcspn (a_string+lspace,">");
  strncpy       (prompt,a_string+lspace,lpos);
  prompt[lpos]  = 0;
  This          = OInterpreterGetReference(prompt);
  if(This==NULL)    
    {
      CWarnF ("No interpreter declared for script:\n%s\n",a_string);
      return NULL;
    }
  if(a_This!=NULL)  *a_This = This;
  return       (pos+1);
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
  CErrorHandle ("Wo","OInterpreter",a_name,a_code);
}
