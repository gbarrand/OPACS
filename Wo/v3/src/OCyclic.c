/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <X11/Intrinsic.h>

#include <CPrinter.h>
#include <CString.h>
#include <CMemory.h>
#include <CList.h>
#include <CError.h>

#include <XWidget.h>

#include <OInterpreter.h>

#include <OCyclicP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  FreeRecord    (OCyclic);
static int   GetIdentifier (OCyclic,char*);
static void  Start         (OCyclic,XtIntervalId*);
static void  ReportError   (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
#define NotFound (-1)

static struct 
{
  OCyclic* extent;
} Class = {NULL};
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OCyclicClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy         ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent        = NULL;
}
/***************************************************************************/
OCyclic* OCyclicGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OCyclic OCyclicGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"OCyclic",7)==0) )
    {
      int        status;
      OCyclic    identifier;
      identifier = (OCyclic)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return (OCyclic)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,pos+1);
	}
      else
	{
	  if(OCyclicIsValid(identifier)==0)
	    {
	      return NULL;
	    }
	  else
	    {
	      return identifier;
	    }
	}
    }
  else
    {
      return (OCyclic)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
    }
}
/***************************************************************************/
static int GetIdentifier (
 OCyclic  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OCyclicGetName(This),a_name);
}
/***************************************************************************/
int OCyclicIsValid (
 OCyclic This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
OCyclic OCyclicMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCyclic        This;
/*.........................................................................*/
  This           = (OCyclic)CMemoryAllocateBlock (sizeof(OCyclicRecord));
  if(This==NULL) return NULL;
  CListAddEntry    ((CList*)&(Class.extent),This);
  This->name       = NULL;
  This->widgetName = NULL;
  This->context    = NULL;
  This->script     = NULL;
  This->delay      = 0;
  This->ident      = 0L;
  return           This;
}
/***************************************************************************/
OCyclic OCyclicCreate (
 char* a_name   
,char* a_widgetName
,int  a_delay  
,char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCyclic        This;
/*.........................................................................*/
  if(a_delay<=0) return NULL;
  This           = OCyclicMake();
  if(This==NULL) return NULL;
  This->name       = CStringDuplicate(a_name);
  This->widgetName = CStringDuplicate(a_widgetName);
  This->script     = CStringDuplicate(a_string);
  This->delay      = a_delay;
  return           This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OCyclicDelete (
  OCyclic This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  CListRemoveEntry  ((CList)Class.extent,This);
  FreeRecord        (This);
}
/***************************************************************************/
void OCyclicStop (
  OCyclic This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  if(This->ident!=0L) XtRemoveTimeOut   (This->ident);
  This->ident     = 0L;
}
/***************************************************************************/
void OCyclicStart (
  OCyclic This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(This==NULL) return;
  if(This->ident!=0L) XtRemoveTimeOut   (This->ident);
  This->ident    = 0L;
  widget   = XWidgetGetIdentifier (This->widgetName);
  if(widget==NULL)
    {
      if( (This->name!=NULL) && (This->widgetName!=NULL) )
	CWarnF("OCyclic %s can't start ; %s is not a valid widget.\n",This->name,This->widgetName);
      else
	CWarn ("OCyclic can't start ; no widget provided.\n");
      return;
    }
  This->context = XtWidgetToApplicationContext(widget);
  Start (This,NULL);
}
/***************************************************************************/
void OCyclicSetDelay (
 OCyclic This
,int  a_delay  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_delay<=0) return;
  This->delay    = a_delay;
}
/***************************************************************************/
char* OCyclicGetName (
  OCyclic This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return    This->name;
}
/***************************************************************************/
void OCyclicSetName (
 OCyclic This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->name);
  This->name    = CStringDuplicate (a_name);
}
/***************************************************************************/
void OCyclicSetScript (
 OCyclic This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->script);
  This->script  = CStringDuplicate (a_string);
}
/***************************************************************************/
void OCyclicSetWidgetName (
 OCyclic This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete    (This->widgetName);
  This->widgetName = CStringDuplicate (a_string);
}
/***************************************************************************/
int OCyclicSetAttribute (
 OCyclic  This
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
  else if(strcmp(a_name,"name")==0)              OCyclicSetName       (This,*((char**)a_addr));
  else if(strcmp(a_name,"widget")==0)            OCyclicSetWidgetName (This,*((char**)a_addr));
  else if(strcmp(a_name,"script")==0)            OCyclicSetScript     (This,*((char**)a_addr));
  else if(strcmp(a_name,"delay")==0)             OCyclicSetDelay      (This,*((int*)a_addr));
  else 
    {
      CInfoF ("OCyclicSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int OCyclicGetAttribute (
 OCyclic    This
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
  else if(strcmp(a_name,"widget")==0)     *((char**)a_addr)    = This->widgetName;
  else if(strcmp(a_name,"script")==0)     *((char**)a_addr)    = This->script;
  else if(strcmp(a_name,"delay")==0)      *((int*)a_addr)      = This->delay;
  else 
    {
      CInfoF("OCyclicGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OCyclicDoMethod (
 OCyclic    This
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

       if(strcmp(a_name,"start")==0) OCyclicStart (This);
  else if(strcmp(a_name,"stop")==0)  OCyclicStop  (This);
  else 
    {
      CInfoF("OCyclicDoMethod: unknown routine property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  a_addr = NULL;
  return 1;
}
/***************************************************************************/
static void FreeRecord (
  OCyclic This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This->ident!=0L) XtRemoveTimeOut   (This->ident);
  CStringDelete    (This->name);
  CStringDelete    (This->widgetName);
  CStringDelete    (This->script);
  This->name       = NULL;
  This->widgetName = NULL;
  This->context    = NULL;
  This->script     = NULL;
  This->ident      = 0L;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void Start (
 OCyclic This
,XtIntervalId* a_ident 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCyclic           cyclic;
/*.........................................................................*/
  if(This==NULL)          return;
  if(This->delay<=0)      return;
  if(This->context==NULL) return;
  cyclic                  = (OCyclic)This;
  OScriptExecute          (cyclic->script);
  if(OCyclicIsValid(cyclic)==1) /*OCyclic could have been deleted in script.*/
    {
      cyclic->ident = XtAppAddTimeOut (This->context,
				       (unsigned long)cyclic->delay,
				       (XtTimerCallbackProc)Start,
				       (XtPointer)This);
    }
  a_ident             = NULL;
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
  CErrorHandle ("Wo","OCyclic",a_name,a_code);
}
