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

#include <XWidget.h>

#include <OClassP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void FreeRecord    (OClass);
static int  GetIdentifier (OClass,char*);
static void ReportError   (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;

static struct 
{
  OClass*     extent;
  WidgetClass xclassShell;
} Class = {NULL,NULL}; 
/***************************************************************************/
void OClassClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy      ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent      = NULL;
  Class.xclassShell = NULL;
}
/***************************************************************************/
OClass* OClassGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OClass OClassGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OClass)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OClass  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OClassGetName(This),a_name);
}
/***************************************************************************/
OClass OClassMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OClass This;
/*.........................................................................*/
  This               = (OClass)CMemoryAllocateBlock (sizeof(OClassRecord));
  if(This==NULL)     return NULL;
  CListAddEntry      ((CList*)&(Class.extent),This);

  This->group          = NULL;
  This->name           = NULL;
  This->widget_class   = NULL;
  This->create_name    = NULL;
  This->create_proc    = NULL;
  This->inc_pub        = NULL;
  This->psProc         = NULL;
  This->targetProc     = NULL;
  This->clearClassProc = NULL;

  return This;
}
/***************************************************************************/
OClass OClassCreate (
 char* a_name
,WidgetClass a_class
,char* a_include
,char* a_create_name
,OClassCreateWidgetFunction a_proc
,OClassVisitWidgetFunction a_ps
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  pos;
  char*  name;
  char*  group;
  OClass This;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;

  group         = NULL;
  name          = CStringDuplicate(a_name);
  pos           = strchr (name,'/');  
  if(pos!=NULL)
    {
      *pos        = 0;
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
      CStringDelete(name);
      CWarnF("OClassCreate: syntax error in :\n%s\n",a_name);
      return NULL;
    }

  if(OClassGetIdentifier(name)!=NULL) return NULL; /*done*/

  This              = OClassMake();
  if(This==NULL)    return NULL;

  This->group          = group;
  This->name           = name;
  This->widget_class   = a_class;
  This->create_name    = CStringDuplicate(a_create_name);
  This->create_proc    = a_proc;
  This->inc_pub        = CStringDuplicate(a_include);
  This->psProc         = a_ps;

  return This;
}
/***************************************************************************/
OClass OClassDeclareWidgetClass (
 char* a_name
,WidgetClass a_class
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return OClassCreate (a_name,a_class,NULL,NULL,NULL,XWidgetPutPixmapInPS_Stream);
}
/***************************************************************************/
OClass OClassDeclareCompoundWidget (
 char* a_name
,OClassCreateWidgetFunction a_createProc
,WidgetClass a_class
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return OClassCreate (a_name,a_class,NULL,NULL,a_createProc,XWidgetPutPixmapInPS_Stream);
}
/***************************************************************************/
int OClassIsShell (
 OClass This
)
/***************************************************************************/
/* shells + compound nested in a shell.                                    */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  if(This->name==NULL) return 0;
  if(Class.xclassShell==NULL)
    {
      Class.xclassShell = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("Shell"));
      if(Class.xclassShell==NULL) CWarn("Shell widget class not declared to Wo.\n");
    }
  if( (XClassIsSubclass(This->widget_class,Class.xclassShell)==True) ||
      (strcmp(This->name,"XmFileSelectionDialog")==0) ||                
      (strcmp(This->name,"XmFormDialog")==0) ||
      (strcmp(This->name,"XmPopupMenu")==0) ||
      (strcmp(This->name,"XmBulletinBoardDialog")==0) ||
      (strcmp(This->name,"XmInformationDialog")==0) ||
      (strcmp(This->name,"XmMessageDialog")==0) ||
      (strcmp(This->name,"XmErrorDialog")==0) ||
      (strcmp(This->name,"XmPromptDialog")==0) ||
      (strcmp(This->name,"XmQuestionDialog")==0) ||
      (strcmp(This->name,"XmSelectionDialog")==0) ||
      (strcmp(This->name,"XmWarningDialog")==0) ||
      (strcmp(This->name,"XmWorkingDialog")==0) || 
      (strcmp(This->name,"XoColormapDialog")==0) ||
      (strcmp(This->name,"XoHelpDialog")==0) 
    ) return 1;
  return 0;
}
/***************************************************************************/
char* OClassGetName (
 OClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
char* OClassGetGroup (
 OClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->group;
}
/***************************************************************************/
char* OClassGetCreateFunctionName (
 OClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->create_name;
}
/***************************************************************************/
OClassCreateWidgetFunction OClassGetCreateFunction (
 OClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->create_proc;
}
/***************************************************************************/
void OClassSetPS_Function (
 OClass This
,OClassVisitWidgetFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->psProc = a_proc;
}
/***************************************************************************/
OClassVisitWidgetFunction OClassGetPS_Function (
 OClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->psProc;
}
/***************************************************************************/
void OClassSetTargetChangedFunction (
 OClass This
,OClassVisitWidgetFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->targetProc = a_proc;
}
/***************************************************************************/
OClassVisitWidgetFunction OClassGetTargetChangedFunction (
 OClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->targetProc;
}
/***************************************************************************/
void OClassSetClearClassFunction (
 OClass This
,OClassVoidFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->clearClassProc = a_proc;
}
/***************************************************************************/
WidgetClass OClassGetWidgetClass (
 OClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->widget_class;
}
/***************************************************************************/
char* OClassGetPublicHeaderFileName (
 OClass This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->inc_pub;
}
/***************************************************************************/
int OClassGetAttribute (
 OClass    This
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
  else if(strcmp(a_name,"group")==0)      *((char**)a_addr)    = This->group;
  else 
    {
      CInfoF("OClassGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
WidgetClass OClassGetWidgetClassIdentifier (
  char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OClass* itema;
/*.........................................................................*/
  if(a_name==NULL) return NULL;
  for(itema=Class.extent;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      char* name;
      name  = XClassGetName((*itema)->widget_class);
      if( (name!=NULL) && (strcmp(a_name,name)==0) )
	return (*itema)->widget_class;
    }
  return NULL;
}
/***************************************************************************/
int OClassIsWidgetClassValid (
  WidgetClass a_class
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OClass* itema;
/*.........................................................................*/
  if(a_class==NULL) return 0;
  for(itema=Class.extent;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(a_class==(*itema)->widget_class) return 1;
    }
  return 0;
}
/***************************************************************************/
static void FreeRecord (
 OClass This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This->clearClassProc!=NULL) This->clearClassProc();
  CStringDelete      (This->group);
  CStringDelete      (This->name);
  CStringDelete      (This->create_name);
  CStringDelete      (This->inc_pub);
  This->group        = NULL;
  This->name         = NULL;
  This->create_name  = NULL;
  This->inc_pub      = NULL;
  CMemoryFreeBlock   (This);
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
  CErrorHandle ("Wo","OClass",a_name,a_code);
}
