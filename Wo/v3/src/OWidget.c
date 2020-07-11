/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
#define DEBUG
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>  

#include <CMemory.h>
#include <CPrinter.h>
#include <CCharacter.h>
#include <CString.h>
#include <CText.h>
#include <CStream.h>
#include <CFile.h>
#include <CList.h>
#include <CTime.h>
#include <CPS.h>
#include <CSystem.h>
#include <CError.h>

/*Xo*/
#include <XDisplay.h>
#include <XSelection.h>
#include <XMailbox.h>
#include <XWidget.h>

/*Wo*/
#include <OClass.h>
#include <OInterpreter.h>
#include <OEvent.h>
#include <XtwTools.h>
#include <OInterface.h>
#include <OResource.h>

#include <OPartP.h>
#include <OWidget.h>

#ifdef __cplusplus
extern "C"{
#endif
static char*        GetResource          (Widget,char*,char*,int);
static int          IsResourceMarked     (Widget,int);
static void         MoveAction           (Widget,XEvent*,char**,Cardinal*);
static void         ChangeSizeAction     (Widget,XEvent*,char**,Cardinal*);
static void         ExecuteCallback      (Widget,XtPointer,XtPointer);
static void         DestroyCallback      (Widget,XtPointer,XtPointer);
static void         ExecuteHandler       (Widget,XtPointer,XEvent*,Boolean*);
static void         HelpHandler          (Widget,XtPointer,XEvent*,Boolean*);
static char*        BuildName            (Widget,char*);
static void         CloseWindow          (Widget,XEvent*,char**,Cardinal*);
static Widget       DuplicateTree        (Widget,char*,Widget,Boolean);
static void         SetDuplicate         (Widget,Widget);
static Widget       GetDuplicate         (Widget);
static void         DuplicateResources   (Widget,Widget,Boolean);
static OPart        CreatePrivatePart    (Widget);
static void         WriteWidget          (Widget);
static void         ReportError          (char*,int);
static void         SetTarget            (Widget);
static char*        GetCallValue         (Widget);
static void         SetEvent             (Widget,OEvent,XtPointer,int);

void _XtAddCallback    (XtCallbackList*,XtCallbackProc,XtPointer);
#ifdef __cplusplus
}
#endif

#define XoNcreateCallback         "createCallback"
#define XoNtargetChangedCallback  "targetChangedCallback"
#define XoNeventHandler           "eventHandler"

#define LOOP_BEGIN_RES(This)     {int res_number,res_count;XtResource res;\
                                  char* res_name;char* res_type;int res_size;\
                                  res_number = XWidgetGetNumberOfResources (This);\
                                  for(res_count=0;res_count<res_number;res_count++){\
                                     XWidgetGetIthResource (This,res_count,&res);\
                                     res_name = res.resource_name;\
                                     res_type = res.resource_type;\
                                     res_size = res.resource_size;{
#define LOOP_END_RES              }}}

#define MAX_RES           128 /* also defined in OXt.c */
#define MAX_CBK           10
#define NotFound          (-1)
#define StringNotNull(s)  ((s)==NULL?"":(s))

static struct 
{
  Boolean        init;
  OTrace         verbose;
  Boolean        toggle_help;
  Boolean        forcedBackgroundExecution;
  char**         cbks;
  char**         cbkNames;
  void*          cbkTag;
  char**         handlers;
  char**         ress;
  XtCallbackProc destroy;
  Position       wx,wy; /*for PS*/
  Dimension      ww,wh; /*for PS*/
  char*          filter;
  XtTranslations trans_table;
  char*          dragAndDropTable;
  XtActionProc   dragAndDropAction;
  OWidgetDragAndDropFunction dragAndDropBegin;
  OWidgetDragAndDropFunction dragAndDropEnd;
  WidgetClass    xclassShell;
  WidgetClass    xclassTopLevelShell;
  WidgetClass    xclassOverrideShell;
  WidgetClass    xclassTransientShell;
  WidgetClass    xclassApplicationShell;
  WidgetClass    xclassSimpleMenu;    /*Xaw*/
  WidgetClass    xclassXmDialogShell; /*Xm*/
  WidgetClass    xclassXmMenuShell;   /*Xm*/
  WidgetClass    xclassXmForm;
} Class = {
False,
OTraceNone,
False,
False,
NULL,NULL,NULL, /*cbks*/
NULL,           /*handlers*/
NULL,           /*ress*/
NULL,           /*destroy*/
0,0,            /*wx,wy*/
0,0,            /*ww,wh*/
NULL,
NULL,
NULL,NULL,NULL,NULL,  /*dragAndDrop*/
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL  /*xclass*/
};
/***************************************************************************/
void OWidgetSetTrace (
  OTrace a_mode
)
/***************************************************************************/
{
/*.........................................................................*/
  Class.verbose = a_mode;
}
/***************************************************************************/
XtCallbackProc OWidgetSetDestroyFunction (
 XtCallbackProc a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtCallbackProc old_proc;
/*.........................................................................*/
  old_proc      = Class.destroy;
  Class.destroy = a_proc;
  return        old_proc;
}
/***************************************************************************/
Widget OWidgetGetFirstShell (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList  items,itema;
/*.........................................................................*/
  items = XWidgetGetLeaves(XWidgetGetTop());
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if(OWidgetIsShell(*itema)==1)
	{ 
	  Widget     widget;
	  widget     = *itema;
	  CListDelete ((CList)items);
	  return     widget;
	}
    }
  CListDelete ((CList)items);
  return     NULL;
}
/***************************************************************************/
char** OWidgetGetShells (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int         number;
  char**      list = NULL;
  int         nwidget;
  WidgetList  items,itema;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  nwidget      = XWidgetGetNumberOfSubWidgets (XWidgetGetTop());
  if(nwidget==0) return NULL;
  list         = (char**)CMemoryAllocateBlock(nwidget*sizeof(char*));
  if(list==NULL)    return NULL;
  items        = XWidgetGetLeaves(XWidgetGetTop());
  number       = 0;
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if(  (OWidgetIsShell(*itema)==1) && (XtName(*itema)!=NULL) )
	{
	  list[number] = CStringDuplicate(XtName(*itema));
	  number++;
	}
    }
  if(a_number!=NULL) *a_number = number;
  CListDelete   ((CList)items);
  return       list;
}
/***************************************************************************/
int OWidgetHasMappedShell (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int         nwidget;
  WidgetList  items,itema;
/*.........................................................................*/
  nwidget      = XWidgetGetNumberOfSubWidgets (XWidgetGetTop());
  if(nwidget==0) return 0;
  items        = XWidgetGetLeaves(XWidgetGetTop());
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if( (OWidgetIsShell(*itema)==1) && (XWidgetIsVisible(*itema)==True) )
	{
	  CListDelete ((CList)items);
	  return   1;
	}
    }
  CListDelete ((CList)items);
  return     0;
}
/***************************************************************************/
void OWidgetInitializeClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==True) return; /* already done */

  Class.cbks       = NULL;
  Class.cbkNames   = NULL;
  Class.cbkTag     = NULL;
  Class.handlers   = NULL;

  XClassSetIsValidFunction         ((XClassIsValidFunction)OClassIsWidgetClassValid);
  XClassSetGetIdentifierFunction   ((XClassGetIdentifierFunction)OClassGetWidgetClassIdentifier);
  XWidgetSetTargetChangedFunction  (SetTarget);
  XWidgetSetGetLocalTargetFunction (OWidgetGetLocalTarget);
  XDisplaySetGrabDispatchFunction  (OEventDispatchGrab);

  Class.init     = True;
}
/***************************************************************************/
void OWidgetClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==False) return;

  CListDestroy            ((CList)Class.cbks,(CListVisitEntryFunction)CMemoryFreeBlock);
  Class.cbks              = NULL;
  CListDestroy            ((CList)Class.cbkNames,(CListVisitEntryFunction)CMemoryFreeBlock);
  Class.cbkNames          = NULL;
  Class.cbkTag            = NULL;

  CListDestroy            ((CList)Class.handlers,(CListVisitEntryFunction)CMemoryFreeBlock);
  Class.handlers          = NULL;

  OPartClearClass         ();

  XtwClearClass           ();

  CMemoryFreeBlock        (Class.ress);
  Class.ress              = NULL;

  CStringDelete           (Class.filter);
  Class.filter            = NULL;

  CStringDelete           (Class.dragAndDropTable);
  Class.dragAndDropTable  = NULL;
  Class.dragAndDropAction = NULL;
  Class.dragAndDropBegin  = NULL;
  Class.dragAndDropEnd    = NULL;

  Class.trans_table       = NULL;

  Class.xclassShell            = NULL;
  Class.xclassTopLevelShell    = NULL;
  Class.xclassOverrideShell    = NULL;
  Class.xclassTransientShell   = NULL;
  Class.xclassApplicationShell = NULL;
  Class.xclassSimpleMenu       = NULL;
  Class.xclassXmDialogShell    = NULL;
  Class.xclassXmMenuShell      = NULL;
  Class.xclassXmForm           = NULL;

  Class.init              = False;
}
/***************************************************************************/
void OWidgetSetHelp (
  Boolean a_flag 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.toggle_help = a_flag;
}
/***************************************************************************/
Boolean OWidgetGetHelp (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.toggle_help;
}
/***************************************************************************/
void OWidgetSetForcedBackgroundExecution (
  Boolean a_flag 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.forcedBackgroundExecution = a_flag;
}
/***************************************************************************/
void OWidgetSetDragAndDrop (
 char*        a_table
,XtActionProc a_proc
,OWidgetDragAndDropFunction a_begin
,OWidgetDragAndDropFunction a_end
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete           (Class.dragAndDropTable);
  Class.dragAndDropTable  = CStringDuplicate(a_table);
  Class.dragAndDropAction = a_proc;
  Class.dragAndDropBegin  = a_begin;
  Class.dragAndDropEnd    = a_end;
}
/***************************************************************************/
void OWidgetDoRubberBandGrab (
 Widget This
,int* a_beg_x
,int* a_beg_y
,int* a_end_x
,int* a_end_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Position beg_x,beg_y,end_x,end_y;
/*.........................................................................*/
  if(a_beg_x!=NULL) *a_beg_x = 0;
  if(a_beg_y!=NULL) *a_beg_y = 0;
  if(a_end_x!=NULL) *a_end_x = 0;
  if(a_end_y!=NULL) *a_end_y = 0;
  if(This==NULL) return;
  XDisplayDoRubberBandGrab (XtDisplay(This),1,(Cursor)XWidgetConvertStringToCursor(This,"crosshair"),&beg_x,&beg_y,&end_x,&end_y);
  if(a_beg_x!=NULL) *a_beg_x = (int)beg_x;
  if(a_beg_y!=NULL) *a_beg_y = (int)beg_y;
  if(a_end_x!=NULL) *a_end_x = (int)end_x;
  if(a_end_y!=NULL) *a_end_y = (int)end_y;
}
/***************************************************************************/
Widget OWidgetCreate (
 char* a_name   
,Widget a_parent 
,char* a_type   
,Boolean a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*                 name;
  Widget                widget;
/*.........................................................................*/
  if(a_parent==NULL)
    {
      CWarn  ("Try to create an orphan widget.\n");
      return NULL;
    }

  name                  = BuildName (a_parent,a_name);
  if(name==NULL)        return NULL;

  if(OClassGetIdentifier(name)!=NULL)
    {
      CWarnF ("Widget name %s can't be a widget type name.\n",name);
      return NULL;
    }

  widget                = OWidgetBuildToolkitPart (name,a_parent,a_type,OWidgetGetFile(a_parent),False,False);
  CStringDelete         (name);
  if(widget==NULL)      return NULL;

  XWidgetRealize        (widget);

  if(a_mark==True)      OInterfaceMark        ((OInterface)OWidgetGetFile(a_parent),1);

/* compound widget create intermediate Xt items: init their OPart */
  if(OClassGetCreateFunction(OClassGetIdentifier(a_type))!=NULL) 
    XWidgetVisitHierarchyForward   (XWidgetGetTop(),(XWidgetVisitFunction)OWidgetBuildPrivatePart);

  return                 widget;
}
/***************************************************************************/
Widget OWidgetBuildToolkitPart (
 char* a_name   
,Widget a_parent
,char* a_type
,void* a_oui
,int a_volatil
,int a_volatilChild
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OClass                     oclass;
  Widget                     widget;
  Arg                        args[1];
  OClassCreateWidgetFunction create_proc;
  WidgetClass                xclass;
/*.........................................................................*/
  if(a_name==NULL)   return NULL;
  if(a_type==NULL)   return NULL;
  if(a_parent==NULL)
    {
      CWarnF ("Try to create orphan widget %s\n",a_name);
      return NULL;
    }
  oclass         = OClassGetIdentifier(a_type);
  if(oclass==NULL)
    {
      CWarnF ("Widget type %s not declared\n",a_type);
      return NULL;
    }
  if( !XtIsComposite(a_parent) && (OClassIsShell(oclass)==0) )
    {
      CWarnF ("Bad parent type for %s.\n",XtName(a_parent));
      return NULL;
    }

  
  if(Class.xclassShell==NULL)
    {
      Class.xclassShell          = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("Shell"));
      Class.xclassTopLevelShell  = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("TopLevelShell"));
      Class.xclassOverrideShell  = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("OverrideShell"));
      Class.xclassTransientShell = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("TransientShell"));

      Class.xclassSimpleMenu     = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("SimpleMenu"));

      Class.xclassXmDialogShell  = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("XmDialogShell"));
      Class.xclassXmMenuShell    = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("XmMenuShell"));
    }


  if ( (Class.xclassXmDialogShell!=NULL) && 
       XtIsSubclass(a_parent,Class.xclassXmDialogShell) 
      ) 
    {  WidgetList    items;
       int           number;
       items         = XWidgetGetChildren (a_parent,&number);
       CListDelete    ((CList)items);
       if(number!=0) 
         {
           CWarnF ("Parent %s admit no more than one child.\n",XtName(a_parent));
           return NULL;
         }
    }

  xclass  = (WidgetClass)OClassGetWidgetClass (oclass);
  if(xclass==NULL)
    { 
      CWarnF ("Widget type %s has no Xt WidgetClass.\n",a_type);
      return NULL;
    }

  if(Class.verbose==OTraceWidget) 
    {
      if(XtName(a_parent)==NULL)
	CInfoF("Create : name = %s, type = %s, parent = (nil).\n",a_name,a_type);
      else
	CInfoF("Create : name = %s, type = %s, parent = %s.\n",a_name,a_type,XtName(a_parent));
    }

  create_proc = OClassGetCreateFunction  (oclass);
  if(create_proc!=NULL) /* compound widget */
    { 
#ifdef DEBUG
      printf ("debug : %s %s : use create_proc.\n",a_name,a_type);
#endif
      if(Class.verbose==OTraceWidget) CInfo ("Is compound.\n");
      widget = create_proc(a_parent,a_name,args,0);
    }
  else if( (Class.xclassTopLevelShell!=NULL) && (xclass==Class.xclassTopLevelShell) )
    {
/* In fact should do: widget = XtAppCreateShell   (a_name,"Wo",xclass,XtDisplay(a_parent),args,0); */
/* But then it is not connected to top. XtCreateWidget seems to works.*/
#ifdef DEBUG
      printf ("debug : %s %s : use XtCreateWidget.\n",a_name,a_type);
#endif
      widget = XtCreateWidget     (a_name,xclass,a_parent,args,0);
    }
  else if( 
	  ( (Class.xclassXmDialogShell!=NULL)  && (xclass==Class.xclassXmDialogShell)  ) ||
	  ( (Class.xclassOverrideShell!=NULL)  && (xclass==Class.xclassOverrideShell)  ) ||
	  ( (Class.xclassTransientShell!=NULL) && (xclass==Class.xclassTransientShell) ) ||
	  ( (Class.xclassSimpleMenu!=NULL)     && (xclass==Class.xclassSimpleMenu) ) 
	)
    {
#ifdef DEBUG
      printf ("debug : %s %s : use XtCreatePopupShell.\n",a_name,a_type);
#endif
      widget = XtCreatePopupShell (a_name,xclass,a_parent,args,0);
    }
  else 
    {
#ifdef DEBUG
      printf ("debug : %s %s : use XtCreateWidget.\n",a_name,a_type);
#endif
      widget = XtCreateWidget     (a_name,xclass,a_parent,args,0);
    }

  if(widget==NULL)   return NULL;

/* management */
/* if compound a_parent could be different of XtParent(widget) */
  if( ( (XtParent(widget)!=NULL)   && (
        ( (Class.xclassXmDialogShell!=NULL) && XtIsSubclass(XtParent(widget),Class.xclassXmDialogShell) ) ||
        ( (Class.xclassXmMenuShell!=NULL)   && XtIsSubclass(XtParent(widget),Class.xclassXmMenuShell) ) )
      ) ||
        (XClassIsSubclass(xclass,Class.xclassShell)==True)
    )
    {
    }
  else 
    {
      if(Class.verbose==OTraceWidget) CInfo ("Is managed.\n");
      XtManageChild (widget);
    }

  OPartInitialize (CreatePrivatePart(widget),a_parent,a_oui,a_type,a_volatil,a_volatilChild);

  if(Class.verbose==OTraceWidget) 
    {
      CInfoF("Create : %lu created.\n",widget);
    }

  return                widget;
}
/***************************************************************************/
static char* BuildName (
 Widget This 
,char* a_name 
)
/***************************************************************************/
{
/*.........................................................................*/
  if(This==NULL) return NULL;

  if( (a_name==NULL) || (*a_name=='\0') ) 
    { 
      OInterface ointerface;
      char*      prefix;
      ointerface = (OInterface)OWidgetGetFile(This);
      prefix     = OInterfaceGetPrefix (ointerface);
      if(prefix==NULL)
	{
	  return XWidgetBuildName(XWidgetGetTop(),"Xt_") ; /* This could be a pure Xt Widget */
	}
      else
	{
	  return XWidgetBuildName(XWidgetGetTop(),prefix);
	}
    }
  else
    {
       if(XWidgetGetIdentifier(a_name)!=NULL)
         {
            CWarnF ("Widget name %s already exists.\n",a_name);
            return NULL;
         }
       else 
         {
           if(CStringIsWord(a_name)==0)
             {
               CWarnF ("%s:contains invalid characters\n",a_name);
               return NULL;
             }
           else
	     {
	       return CStringDuplicate(a_name);
	     }
         }
    }
}
/***************************************************************************/
void OWidgetSetPrivatePart (
 Widget This 
,Widget a_parent
,void* a_interface
,char*  a_type
,Boolean a_volatil
,Boolean a_volatilChild
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)  return;
  OPartInitialize (CreatePrivatePart(This),a_parent,a_interface,a_type,a_volatil,a_volatilChild);
}
/***************************************************************************/
int OWidgetIsShell (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return OClassIsShell(OClassGetIdentifier(OWidgetGetType(This)));
}
/***************************************************************************/
void OWidgetSetName (
 Widget This 
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_name==NULL)  return;
  name                 = BuildName (This,a_name);
  if(name==NULL)            return;
  XWidgetSetName       (This,name);
  CStringDelete        (name);
  OInterfaceMark        ((OInterface)OWidgetGetFile(This),1);
}
/***************************************************************************/
void OWidgetAddCallback (
 Widget This 
,char* a_name   
,char* a_value  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtResource    res;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_name==NULL)  return;
  if(a_value==NULL) return;
  if( (strcmp(a_name,XoNcreateCallback)==0) || (strcmp(a_name,XoNtargetChangedCallback)==0) )
    {
      CWarnF("For widget %s, only one %s supported.\n",XtName(This),a_name);
      return;
    }
  if(XWidgetGetResources(This,a_name,&res)==NotFound) 
    {
      CWarnF("%s not a resource of %s\n",a_name,XtName(This));
      return;
    }
  if( (res.resource_type!=NULL) && (strcmp(res.resource_type,XtRCallback)==0) )
   {
     char*         string;
     CListAddEntry ((CList*)&(Class.cbks)    ,string = CStringDuplicate(a_value));
     CListAddEntry ((CList*)&(Class.cbkNames),CStringDuplicate(a_name));
     XtAddCallback (This,a_name,ExecuteCallback,(XtPointer)string);
   }
}
/***************************************************************************/
void OWidgetCallCallbacks (
 Widget This 
,char* a_name  
)
/***************************************************************************/
/*
  Call only Wo callbacks. 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtCallbackList   cbks = NULL;
  int              number;
  XtCallbackList   ocbks = NULL;
  int              count;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_name==NULL)  return;
  if(XtHasCallbacks(This,a_name)!=XtCallbackHasSome) return;

  cbks         = (XtCallbackList)XWidgetGetResource (This,a_name);
  number       = 0;
  if(cbks!=NULL)     
    {
      while(1) 
	{
	  if( (cbks[number].callback!=NULL) && (cbks[number].closure!=NULL) ) 
	    number++; 
	  else 
	    break;
	}
    }
  if(number==0)  return;

  ocbks        = NULL;
/* get Wo cbk */
  for(count=0;count<number;count++)
    {
      if(cbks[count].callback==(XtCallbackProc)ExecuteCallback)
        { 
          _XtAddCallback (&ocbks,cbks[count].callback,cbks[count].closure);
        }
    }

  XtCallCallbackList (This,ocbks,NULL);

  CMemoryFreeBlock (ocbks);  

}
/***************************************************************************/
char* OWidgetFindCurrentCallbackName (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int index;
/*.........................................................................*/
  if(This==NULL)         return NULL;
  if(Class.cbkTag==NULL) return NULL;
  index  = CListGetEntryIndex ((CList)Class.cbks,(CEntry)Class.cbkTag,NULL);
  if(index==NotFound) return NULL;
  return (char*)CListGetIthEntry((CList)Class.cbkNames,index);
}
/***************************************************************************/
void OWidgetAddEventHandler (
 Widget This 
,EventMask a_mask
,char* a_value  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*              string;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_value==NULL)  return;
  CListAddEntry      ((CList*)&Class.handlers,string = CStringDuplicate(a_value));
  XtAddEventHandler  (This,a_mask ,False ,ExecuteHandler,(XtPointer)string);
}
/***************************************************************************/
void OWidgetRemoveEventHandler (
 Widget This 
,EventMask a_mask
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return;
  XtRemoveEventHandler (This,a_mask ,False ,ExecuteHandler,opart->eventHandlerData);
}
/***************************************************************************/
char* OWidgetGetAndConvertResourceToString (
 Widget This 
,char* a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtResource res;
/*.........................................................................*/
  if(This==NULL) return NULL;
  if(XWidgetGetResources(This,a_name,&res)==NotFound) 
    {
      CWarnF("For widget %s, %s not a resource of %s.\n",XtName(This),a_name,XWidgetGetClassName(This));
      return NULL;
    }
  return GetResource (This,res.resource_name,res.resource_type,res.resource_size);
}
/***************************************************************************/
int OWidgetGetAndConvertResourceToInteger (
 Widget This 
,char* a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         string;
  int           value;
/*.........................................................................*/
  string        = OWidgetGetAndConvertResourceToString (This,a_name);
  if(string==NULL)   return 0;
  value         = (int)CStringConvertToLong(string,NULL);
  CStringDelete (string);
  return        value;
}
/***************************************************************************/
double OWidgetGetAndConvertResourceToDouble (
 Widget This 
,char* a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         string;
  double        value;
/*.........................................................................*/
  string        = OWidgetGetAndConvertResourceToString (This,a_name);
  if(string==NULL)   return 0.;
  value         = CStringConvertToDouble (string,NULL);
  CStringDelete (string);
  return        value;
}
/***************************************************************************/
Boolean OWidgetGetAndConvertResourceToBoolean (
 Widget This 
,char* a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         string;
  Boolean       value;
/*.........................................................................*/
  string        = OWidgetGetAndConvertResourceToString (This,a_name);
  if(string==NULL)   return False;
  value         = ( (strcmp(string,"true")==0)  || (strcmp(string,"True")==0) ) ? 1 : 0;
  CStringDelete (string);
  return        value;
}
/***************************************************************************/
Widget OWidgetGetAndConvertResourceToWidget (
 Widget This 
,char* a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         string;
  Widget        value;
/*.........................................................................*/
  string        = OWidgetGetAndConvertResourceToString (This,a_name);
  if(string==NULL)   return NULL;
  value         = XWidgetGetIdentifier(string);
  CStringDelete (string);
  return        value;
}
/***************************************************************************/
void OWidgetRequireResourceScriptExecution (
 Widget This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtResource res;
  char*      string;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_name==NULL) return;
  if(XWidgetGetResources(This,a_name,&res)==NotFound) return;
  string                        = GetResource (This,res.resource_name,res.resource_type,res.resource_size);
  OWidgetRequireScriptExecution (This,string,NULL,0);
  CStringDelete                 (string);
}
/***************************************************************************/
static char* GetResource (
 Widget  This 
,char*  a_name 
,char*  a_type 
,int     a_size 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char      val1  = 0;
  short     val2  = 0;
  int       val4  = 0;
  long      val8  = 0L;
  XtArgVal  value = 0L;
  XrmValue  from,to;
  char*     string;
  Arg       args[1];
/*.........................................................................*/
  if(This==NULL)    return NULL;
  if(a_name==NULL)  return NULL;
  if(a_type==NULL)  return NULL;

  if(Class.verbose==OTraceResource) CInfoF("Get:%s %s %s %d.\n",XtName(This),a_name,a_type,a_size);

  if(strcmp(a_name,XoNcreateCallback)==0)
    { OPart opart;
      opart = OPartGetIdentifierFromWidget(This);
      if(opart==NULL) return NULL;
      return CStringDuplicate(StringNotNull(opart->createCallback));
    }

  if(strcmp(a_name,XoNtargetChangedCallback)==0)
    { OPart opart;
      opart = OPartGetIdentifierFromWidget(This);
      if(opart==NULL) return NULL;
      return CStringDuplicate(StringNotNull(opart->targetChangedCallback));
    }

  if(strcmp(a_name,XoNeventHandler)==0)
    { OPart opart;
      opart = OPartGetIdentifierFromWidget(This);
      if(opart==NULL) return NULL;
      return CStringDuplicate(StringNotNull(opart->eventHandler));
    }


  switch(a_size)
    { 
      case  1: XtSetArg    (args[0],a_name,&val1);
               XtGetValues (This,args,1);
               value       = val1;
               break;
      case  2: XtSetArg    (args[0],a_name,&val2);
               XtGetValues (This,args,1);
               value       = val2;
               break;
      case  4: XtSetArg    (args[0],a_name,&val4);
               XtGetValues (This,args,1);
               value       = val4;
               break;
      case  8: XtSetArg    (args[0],a_name,&val8);
               XtGetValues (This,args,1);
               value       = val8;
               break;
      default: ReportError       ("GetResource",CErrorCase);
               value       = 0L;
               break; 
    }

  string       = OResourceConvertValueToString (This,a_name,a_type,&value);
  if(string!=NULL)   return string;

  if(strcmp(a_type,XtRCallback)==0)
    { 
      XtCallbackList cbks = NULL;
      int            count;
      cbks      = (XtCallbackList)value;
      if(cbks==NULL) return CStringDuplicate("");
/* search first Wo callback */
      count     = 0;      
      while(1)      
        {
          if(cbks[count].callback==NULL) 
	    return CStringDuplicate(""); /* no more callback in list */
          if(cbks[count].callback==(XtCallbackProc)ExecuteCallback) 
            return CStringDuplicate((String)(cbks[count].closure));
          count++;
        }
    }

  from.size = a_size;
  if(sizeof(long)==8) /*64 bits machines.*/
    {
      switch(a_size)
	{ 
	case  1: from.addr   = (XPointer)&val1;break;
	case  2: from.addr   = (XPointer)&val2;break;
	case  4: from.addr   = (XPointer)&val4;break;
	case  8: from.addr   = (XPointer)&val8;break;
	default: ReportError       ("GetResource",CErrorCase);
	         return      NULL;
	}
    }
  else
    {
      switch(a_size)
	{ 
	case  1: from.addr   = (XPointer)&val1;break;
	case  2: from.addr   = (XPointer)&val2;break;
	case  4: from.addr   = (XPointer)&val4;break;
	default: ReportError       ("GetResource",CErrorCase);
	         return      NULL;
	}
    }
  XtConvert (This,a_type,&from,XtRString,&to);
  if(Class.verbose==OTraceResource)
    {
      if(to.addr!=NULL) 
	{ /*Res could be very large.*/
	  CInfoF ("Get:%s %s %d:%lu converted:",a_name,a_type,a_size,value);
	  CPrinterPut (to.addr);
	  CPrinterPut ("\n");
	}
      else
	{
	  CInfoF("Get:%s %s %d:%lu converted:(nil).\n",a_name,a_type,a_size,value);
	}
    }
  return  CStringDuplicate((String)(to.addr));
}
/***************************************************************************/
void OWidgetBeginResourceChanges (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XResourceChangesInitialize ();
}
/***************************************************************************/
void OWidgetCommitResourceChanges (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XResourceChangesCommit ();
}
/***************************************************************************/
void OWidgetSetResourceFromInteger (
 Widget This 
,char* a_name   
,int a_value  
,Boolean a_mark
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char string[64];
/*.........................................................................*/
  sprintf              (string,"%d",a_value);
  OWidgetSetResourceFromString (This,a_name,string,a_mark);
}
/***************************************************************************/
void OWidgetSetResourceFromBoolean (
 Widget This 
,char* a_name   
,Boolean a_value  
,Boolean a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_value==True)  OWidgetSetResourceFromString (This,a_name,"true",a_mark);
  else               OWidgetSetResourceFromString (This,a_name,"false",a_mark);
}
/***************************************************************************/
void OWidgetSetResourceFromDouble (
 Widget This 
,char* a_name   
,double a_value  
,Boolean a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char string[64];
/*.........................................................................*/
  sprintf              (string,"%g",a_value);
  OWidgetSetResourceFromString (This,a_name,string,a_mark);
}
/***************************************************************************/
void OWidgetSetResourceFromWidget (
 Widget This 
,char* a_name   
,Widget a_value  
,Boolean a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(XWidgetIsValid(a_value)==0) return;
  OWidgetSetResourceFromString (This,a_name,XtName(a_value),a_mark);
}
/***************************************************************************/
void OWidgetSetResourceFromString (
 Widget This 
,char* a_name   
,char* a_value  
,Boolean a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtResource res;
  int        count;
/*.........................................................................*/
  if(This==NULL) return;
  count      = XWidgetGetResources(This,a_name,&res);
  if(count==NotFound) 
    {
      CWarnF("For widget %s, %s not a resource of %s\n",XtName(This),a_name,XWidgetGetClassName(This));
      return;
    }
  OWidgetAssignResource (This,res.resource_name,res.resource_type,a_value);
  if(a_mark==True)  
    { OPart        opart;
      opart       = OPartGetIdentifierFromWidget(This);
      OPartMarkIthResource (opart,count,1);
      if(opart!=NULL)   
	OInterfaceMark  ((OInterface)(opart->oui),1);
    }
}
/***************************************************************************/
void OWidgetAssignResource (
 Widget This 
,char* a_name  
,char* a_type  
,char* a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_name==NULL)  return;
  if(a_type==NULL)  return;
  if(a_value==NULL) return;
/*  printf ("debug:set res:%s %s %s\n%s\n",XtName(This),a_name,a_type,a_value);*/
  if(Class.verbose==OTraceResource) 
    {
      CInfoF ("Set:%s %s %s:",XtName(This),a_name,a_type);
      CPrinterPut (a_value);
      CPrinterPut ("\n");
    }

  if(strcmp(a_name,XoNcreateCallback)==0)
    { OPart opart;
      opart = OPartGetIdentifierFromWidget(This);
      if(opart==NULL) return;
      CStringDelete            (opart->createCallback);
      opart->createCallback = CStringDuplicate(a_value);
      return;
    }

  if(strcmp(a_name,XoNtargetChangedCallback)==0)
    { OPart opart;
      opart = OPartGetIdentifierFromWidget(This);
      if(opart==NULL) return;
      CStringDelete            (opart->targetChangedCallback);
      opart->targetChangedCallback = CStringDuplicate(a_value);
      return;
    }

  if(strcmp(a_name,XoNeventHandler)==0)
    { OPart opart;
      opart = OPartGetIdentifierFromWidget(This);
      if(opart==NULL) return;
      { String            string,script;
        string            = CStringDuplicate(a_value);
        script            = strstr(string,":");
        if(script!=NULL) 
	  {
	    EventMask                  mask;
	    *script                    = '\0';
	    script                     ++;
	    mask                       = XStringGetMask(string);
	    CStringDelete              (opart->eventHandler);
	    opart->eventHandler        = CStringDuplicate(a_value);
	    opart->eventHandlerData    = CStringDuplicate(script);
	    CListAddEntry               ((CList*)&Class.handlers,opart->eventHandlerData);
	    XtAddEventHandler          (This,mask ,False ,ExecuteHandler,opart->eventHandlerData);
        }
        CStringDelete     (string);
      }
      return;
    }

/* callback set from resource is the first Wo Callback: */
/*  we replace the first Wo existing one by the new one */
/* get cbks */
  if(strcmp(a_type,XtRCallback)==0)
    { 
      XtCallbackList   cbks = NULL;
      char*            string;
      CListAddEntry     ((CList*)&(Class.cbks)    ,string = CStringDuplicate(a_value));
      CListAddEntry     ((CList*)&(Class.cbkNames),CStringDuplicate(a_name));
      if(XtHasCallbacks(This,a_name)==XtCallbackHasSome) 
        { Arg                  args[1];
          XtCallbackRec        ncbks[MAX_CBK];
           int         count;
          int                  found;
          XtSetArg             (args[0],a_name,&cbks);
          XtGetValues          (This,args,1);

/* look if one Wo cbk */
          found                = 0;
          count                = 0;
          while(1)      
            {
              if(count>=MAX_CBK) 
                {
		  ReportError ("OWidgetAssignResource",CErrorFlow);
		  ncbks[count-1].callback = NULL;
		  ncbks[count-1].closure  = NULL;
		  break;
		}
              if(cbks[count].callback==NULL) 
		{
		  ncbks[count] = cbks[count];
		  break;
		}
              if( (found==0) && (cbks[count].callback==(XtCallbackProc)ExecuteCallback)) 
                { /* one encountered: replace it's data field with new script */
                  ncbks[count].callback = (XtCallbackProc)ExecuteCallback;
                  ncbks[count].closure  = (XtPointer)string;
                  found                 = 1;
                }
              else 
                ncbks[count] = cbks[count];
              count++;
            }
          if(found==1)
            {
              XtRemoveAllCallbacks (This,a_name);
              XtAddCallbacks       (This,a_name,ncbks);
            }
          else
            { /* no Wo cbk; simply add to existing list */
              XtAddCallback    (This,a_name,ExecuteCallback,(XtPointer)string);
            }
        }
      else 
        { /* no cbks; simply add */
          XtAddCallback (This,a_name,ExecuteCallback,(XtPointer)string);
        }
      return;
    }

  if(OResourceConvertStringToChange(This,a_name,a_type,a_value)==0) 
    {
      XtConvertStringToResourceChange (This,a_name,a_type,a_value);
    }

}
/***************************************************************************/
int OWidgetHasResource (
 Widget This 
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtResource res;
  int        count;
/*.........................................................................*/
  if(This==NULL)   return 0;
  if(a_name==NULL) return 0;
  count                = XWidgetGetResources (This,a_name,&res);
  if(count==NotFound)  return 0;
  return               1;
}
/***************************************************************************/
void OWidgetMarkResource (
 Widget This 
,char* a_name 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtResource res;
  int        count;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_name==NULL) return;
  count                = XWidgetGetResources (This,a_name,&res);
  if(count==NotFound)  return;
 {OPart        opart;
  opart       = OPartGetIdentifierFromWidget(This);
  OPartMarkIthResource (opart,count,a_value);
  if(opart!=NULL) OInterfaceMark  ((OInterface)(opart->oui),1);}
}
/***************************************************************************/
void OWidgetMarkResourceToBeSaved (
 Widget This 
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OWidgetMarkResource (This,a_name,True);
}
/***************************************************************************/
static int IsResourceMarked (
 Widget This 
,int  a_count 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart       opart;
  int           ibyte;
  unsigned char bmsk;
/*.........................................................................*/
  if(This==NULL)                          return 0;
  opart                           = OPartGetIdentifierFromWidget(This);
  if(opart==NULL)                      return 0;
  if(opart->resources_changed==NULL) return 0;
  ibyte     = a_count/8;
  if((ibyte<0)||(ibyte>= opart->nbyte ))
    {
      ReportError  ("IsResourceMarked",CErrorValue);
      return 0;
    }
  bmsk      = 1<<(a_count - ibyte * 8);
  return   (int) ((opart->resources_changed[ibyte] & bmsk) ? 1 : 0);
}
/***************************************************************************/
char** OWidgetGetMarkedResources (
 Widget This 
,int* a_number
,int a_format
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     res_number;
  int     number;
  char**  list;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)  return NULL;

  res_number = XWidgetGetNumberOfResources (This);
  list       = (char**)CMemoryAllocateBlock(res_number * sizeof(char*));  
  if(list==NULL)  return NULL;
  number     = 0;

  LOOP_BEGIN_RES(This)
      if( (res_name!=NULL) && (IsResourceMarked(This,res_count)==1) ) 
	{
	  char*        value;
	  value        = GetResource (This,res_name,res_type,res_size);
	  if(value!=NULL) 
	    {
	      if(OResourceIsMultiLine (res_type,res_name)==1)
		{ 
		  CStringReplacePart (&value,"\n","\\\n");
		}
	      else if( (res_type!=NULL) && (strcmp(res_type,XtRString)==0) )
		{ 
		  CStringReplacePart (&value,"\n","\\n");
		}
	      if( (res_name!=NULL) && (strcmp(res_name,"linkedWidgets")==0) ) /*XoGraph*/
		{
		  WidgetList    list = NULL;
		  Arg           args[1];
		  int           count;
		  int           nwidget,ipos,npair;
		  XtSetArg      (args[0],res_name,&list);
		  XtGetValues   (This,args,1);
		  nwidget       = CListGetSize ((CList)list);
		  CStringDelete (value);                  
		  value         = (char*)CMemoryAllocateBlock(nwidget * 64 * sizeof(char));
		  if(value!=NULL)    
		    {
		      ipos         = CStringPrintF (value,"");
		      npair        = nwidget/2;
		      for(count=0;count<npair;count++)
			{
			  if( (XWidgetIsValid(list[2*count])==True) && 
			      (XWidgetIsValid(list[2*count+1])==True) 
			     )
			    ipos += CStringPrintF  
			      (value+ipos,"%s\\n%s\\\n",XtName(list[2*count]),XtName(list[2*count+1]));
			}
		      if(npair!=0) value[ipos-2] = 0; /* remove last \ */
		      if (npair*2!=nwidget)
			{
			  CWarnF ("For widget %s, linkedWidgets resource has not an even number of items.\n",
				  XtName(This));
			}
		    }
		  
		}
	      
	      {Cardinal        lstring; 
	       char*           string;
	       lstring         = strlen(XtName(This)) + strlen(res_name) + strlen(value) + 4;
	       string          = (char*)CMemoryAllocateBlock(lstring*sizeof(char));
	       if(string!=NULL)  
		 {
		   if(a_format==1)
		     {
		       sprintf         (string," %s = %s",res_name,value);
		     }
		   else
		     {
		       sprintf         (string,"*%s.%s:%s",XtName(This),res_name,value);
		     }
		   list[number] = string;
		   number++;
		 }}
	    }
	  CStringDelete(value);
	}
  LOOP_END_RES

  if(a_number!=NULL) *a_number = number;
  return list;
}
/***************************************************************************/
void OWidgetSendScript (
 Widget This   
,char* a_string   
,int a_do_it
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL) return;
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  XWidgetSetCallData (NULL,0);
  string = GetCallValue (This);
  if(a_do_it==1)
    { 
      if(Class.verbose==OTraceEvent) CInfoF("Execute:%s\n",a_string);
      XWidgetSetThisWidget    (This);
      XWidgetSetCallValue     (string);
      OScriptExecute          (a_string);
      XWidgetSetThisWidget    (NULL);
      XWidgetSetCallValue     (NULL);
    }
  else
    { 
      OEvent           event;
      event            = OEventMake();
      OEventSetWidget  (event,This);
      OEventSetType    (event,OEventExecuteScript);
      OEventSetString  (event,a_string);
      OEventSetValue   (event,string);
      OEventSend       (event,XtDisplay(This));
    }
  CStringDelete       (string);
  XWidgetSetCallData  (NULL,0);
}
/***************************************************************************/
void OWidgetRequireScriptExecution (
 Widget This   
,char* a_string   
,XtPointer a_data
,int a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  pos;
  char*  string;
/*.........................................................................*/
  if(This==NULL) return;
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  XWidgetSetCallData (a_data,a_type);
  string             = GetCallValue (This);
  if(OScriptIsBackgroundScript(a_string,&pos)==1)
    { 
      OEvent           event;
      event            = OEventMake();
      OEventSetWidget  (event,This);
      OEventSetType    (event,OEventExecuteScript);
      OEventSetString  (event,pos);
      OEventSetValue   (event,string);
      SetEvent         (This,event,a_data,a_type);
      OEventSend       (event,XtDisplay(This));
    }
  else if(Class.forcedBackgroundExecution==True)
    {
      OEvent           event;
      event            = OEventMake();
      OEventSetWidget  (event,This);
      OEventSetType    (event,OEventExecuteScript);
      OEventSetString  (event,a_string);
      OEventSetValue   (event,string);
      SetEvent         (This,event,a_data,a_type);
      OEventSend       (event,XtDisplay(This));
    }
  else   /*Do it now.*/
    { 
      if(Class.verbose==OTraceEvent) CInfoF("Execute:%s\n",a_string);
      XWidgetSetThisWidget (This);
      XWidgetSetCallValue  (string);
      OScriptExecute       (a_string);
      XWidgetSetThisWidget (NULL);
      XWidgetSetCallValue  (NULL);
    }
  CStringDelete       (string);
  XWidgetSetCallData  (NULL,0);
}
/***************************************************************************/
static void SetEvent (
 Widget This
,OEvent a_event
,XtPointer a_data
,int a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  typedef struct
  {
    int     reason;
    XEvent* event;
  } AnyCallbackStruct;
/*.........................................................................*/
  if(a_type==1)    /* a_data is an XEvent pointer.*/
    {
      OEventSetEvent (a_event,(XEvent*)a_data);
    }
  else if(a_data!=NULL) /* a_data is a callback data.*/
    {
      char* type;
      type  = OWidgetGetType (This);
      if(type==NULL) return;
      if( (strncmp(type,"Xo",2)==0) ||
	  (strncmp(type,"Xm",2)==0) 
	  )
	{  /* with some MOTIF: bug with xmScale in the copy of a_data->event */
	  OEventSetEvent (a_event,(((AnyCallbackStruct*)a_data)->event));
	} 
    } 
}
/***************************************************************************/
static void MoveAction (
 Widget This 
,XEvent* a_event 
,char** a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    Widget    widget;
    int       x,y;
    XPoint    start,begin,end;
    int       px,py;
    Dimension pw,ph;
    GC        rubberGC;
  } pointer = {NULL,0,0,{0,0},{0,0},{0,0},0,0,0,0,NULL};
  int       posx,posy;
  Dimension ww,wh;
  Position  wx,wy;
  Window    child;
  Widget    parent;
  Window    root;
  Display*  display;
/*.........................................................................*/
  display       = XtDisplay(This);
  root          = XDefaultRootWindow(display);
  switch(a_event->type)
    {
      case ButtonPress:
            if(XtIsShell(This))    return;
            pointer.widget         = This;
            pointer.x              = a_event->xbutton.x_root;
            pointer.y              = a_event->xbutton.y_root;

            pointer.start.x        = pointer.x;
            pointer.start.y        = pointer.y;

            XWidgetGetDimension      (This,&ww,&wh);

            XTranslateCoordinates  (display,XtWindow(This),root,0,0,&posx,&posy,&child);
            pointer.begin.x        = posx;
            pointer.begin.y        = posy;

            XTranslateCoordinates  (display,XtWindow(This),root,ww,wh,&posx,&posy,&child);
            pointer.end.x          = posx;
            pointer.end.y          = posy;

	    pointer.rubberGC       = XDisplayGetRubberGC(XtDisplay(This),root);
            XDisplayDrawRectangle  (display,root,pointer.rubberGC,&pointer.begin,&pointer.end);

/* parent window infos */
            parent                 = XtParent(This);
            XWidgetGetDimension    (parent,&(pointer.pw),&(pointer.ph));
            XTranslateCoordinates  (display,XtWindow(parent),root,0,0,&(pointer.px),&(pointer.py),&child);
            break;
      case ButtonRelease:
            if(pointer.widget==NULL) return; /* button press not done */
            XDisplayDrawRectangle (display,root,pointer.rubberGC,&pointer.begin,&pointer.end);
	    if(pointer.rubberGC!=NULL)
	      {
		XFreeGC               (XtDisplay(This),pointer.rubberGC);
		pointer.rubberGC      = NULL;
	      }

            XWidgetGetPosition      (pointer.widget,&wx,&wy);
            wx                    += pointer.x - pointer.start.x;
            wy                    += pointer.y - pointer.start.y;

            XWidgetMove          (pointer.widget,wx,wy);
            OWidgetMarkResource (pointer.widget,XtNx,1);
            OWidgetMarkResource (pointer.widget,XtNy,1);

            pointer.x               = 0;
            pointer.y               = 0;
            pointer.widget          = NULL;
            break;
      case MotionNotify:
            if(pointer.widget==NULL) return; /* button press not done */
/* protect widget out of parent */
            if( (a_event->xmotion.x_root<=pointer.px) ||
	        (a_event->xmotion.x_root>=(pointer.px+(int)pointer.pw)) ||
                (a_event->xmotion.y_root<=pointer.py) ||
	        (a_event->xmotion.y_root>=(pointer.py+(int)pointer.ph)) ) break;

            XDisplayDrawRectangle   (display,root,pointer.rubberGC,&pointer.begin,&pointer.end);
            pointer.begin.x +=  a_event->xmotion.x_root - pointer.x;
            pointer.begin.y +=  a_event->xmotion.y_root - pointer.y;
            pointer.end.x   +=  a_event->xmotion.x_root - pointer.x;
            pointer.end.y   +=  a_event->xmotion.y_root - pointer.y;
            XDisplayDrawRectangle   (display,root,pointer.rubberGC,&pointer.begin,&pointer.end);

            pointer.x      = a_event->xmotion.x_root;
            pointer.y      = a_event->xmotion.y_root;
            break;
    }
  a_argn = 0; /* to remove warning of c++ compiler */
  a_args  = NULL; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static void ChangeSizeAction (
 Widget This 
,XEvent* a_event 
,char** a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  struct  
  {
    Widget  widget;
    int     x,y;
    XPoint  start,begin,end;
    GC      rubberGC;
  } pointer = {NULL,0,0,{0,0},{0,0},{0,0},NULL};
  int       posx,posy;
  Position  wx,wy;
  Dimension ww,wh;
  int       width,height;
  Window    child;
  Window    root;
#define RESIZE__OFFSET 5
  Display*  display;
/*.........................................................................*/
  display       = XtDisplay(This);
  root          = XDefaultRootWindow(display);
  switch(a_event->type)
    {
      case ButtonPress:
            if(XtIsShell(This))    return;
            pointer.widget         = This;
            XWidgetGetDimension      (This,&ww,&wh);
            wx = pointer.x         = ww  - RESIZE__OFFSET;
            wy = pointer.y         = wh  - RESIZE__OFFSET;
            XWidgetGetRootPosition (This,&wx,&wy);
            pointer.x              = wx;
            pointer.y              = wy;
/*set pointer to lower right corner of widget window*/
            XWarpPointer           (display,None,root,0,0,0,0,pointer.x,pointer.y);

            pointer.start.x        = pointer.x;
            pointer.start.y        = pointer.y;
            XTranslateCoordinates  (display,XtWindow(This),root,0,0,&posx,&posy,&child);
            pointer.begin.x        = posx;
            pointer.begin.y        = posy;
            XTranslateCoordinates  (display,XtWindow(This),root,ww,wh,&posx,&posy,&child);
            pointer.end.x          = posx;
            pointer.end.y          = posy;

	    pointer.rubberGC       = XDisplayGetRubberGC(XtDisplay(This),root);
            XDisplayDrawRectangle  (display,root,pointer.rubberGC,&pointer.begin,&pointer.end);
            break;
      case ButtonRelease:
            if(pointer.widget==NULL) return;
            XDisplayDrawRectangle (display,root,pointer.rubberGC,&pointer.begin,&pointer.end);
	    if(pointer.rubberGC!=NULL)
	      {
		XFreeGC               (XtDisplay(This),pointer.rubberGC);
		pointer.rubberGC      = NULL;
	      }

            XWidgetGetDimension     (pointer.widget,&ww,&wh);
            /* width, height  are signed: right terms could be <=0 */
            width                 = ww + a_event->xbutton.x_root - pointer.start.x;
            height                = wh + a_event->xbutton.y_root - pointer.start.y;
         
            if( (width>0) && (height>0) ) 
              {
                XWidgetSetDimension   (pointer.widget,(Dimension)width,(Dimension)height);
                OWidgetMarkResource (pointer.widget,XtNwidth,1);
                OWidgetMarkResource (pointer.widget,XtNheight,1);
              }

            pointer.x             = 0;
            pointer.y             = 0;
            pointer.widget        = NULL;
            break;
      case MotionNotify:
            if(pointer.widget==NULL) return;
            XDisplayDrawRectangle (display,root,pointer.rubberGC,&pointer.begin,&pointer.end);
            pointer.end.x  +=  a_event->xmotion.x_root - pointer.x;
            pointer.end.y  +=  a_event->xmotion.y_root - pointer.y;
            XDisplayDrawRectangle (display,root,pointer.rubberGC,&pointer.begin,&pointer.end);

            pointer.x            = a_event->xmotion.x_root;
            pointer.y            = a_event->xmotion.y_root;
            break;
    }
  a_argn = 0; /* to remove warning of c++ compiler */
  a_args  = NULL; /* to remove warning of c++ compiler */
}
/***************************************************************************/
static void ExecuteCallback (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int status;
/*.........................................................................*/
  if(XDisplayIsGrabbingPointer()==1) return;
  if(Class.dragAndDropBegin!=NULL)   
    {
      status = Class.dragAndDropBegin(This,a_tag);
      if(status==0) return;
    }
  if(OScriptIsBackgroundScript((String)a_tag,NULL)==1)
    {
      OWidgetRequireScriptExecution (This,(String)a_tag,NULL,0);
    }
  else
    {
      Class.cbkTag                  = a_tag;
      OWidgetRequireScriptExecution (This,(String)a_tag,a_data,0);
      Class.cbkTag                  = NULL;
    }
  if(Class.dragAndDropEnd!=NULL)   
    {
      status = Class.dragAndDropEnd(This,a_tag);
      if(status==0) return;
    }
}
/***************************************************************************/
static void ExecuteHandler (
 Widget This 
,XtPointer a_tag
,XEvent* a_event
,Boolean* a_dispatch
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OScriptIsBackgroundScript((String)a_tag,NULL)==1)
    {
      OWidgetRequireScriptExecution (This,(String)a_tag,NULL,0);
    }
  else
    {
      OWidgetRequireScriptExecution (This,(String)a_tag,(XtPointer)a_event,1);
    }
  a_dispatch = NULL; /* avoid C++ warnings */
}
/***************************************************************************/
void OWidgetVerifySyntaxOfCallbacks (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  LOOP_BEGIN_RES(This)
      if( (res_type!=NULL) && (strcmp(res_type,XtRCallback)==0) )
        { 
	  char*  string;
	  char*  pos;
          string = GetResource (This,res_name,res_type,res_size);
	  if(OScriptIsBackgroundScript(string,&pos)==1)
	    OScriptVerifySyntax (pos);
	  else
	    OScriptVerifySyntax (string);
          CStringDelete            (string);
        }
  LOOP_END_RES
}
/***************************************************************************/
char* OWidgetGetInformations (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart        opart;
  char*        sxt;
  char*        fname;
  Cardinal     length;
  char*        string;
/*.........................................................................*/
  if(This==NULL) return NULL;

  sxt        = XWidgetGetInformations (This);

  opart      = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return sxt;

  fname      = OInterfaceGetFileName((OInterface)opart->oui);

  length     = strlen (sxt) +
               strlen (StringNotNull(opart->type)) +
               strlen (StringNotNull(fname))  +
               128 ;


  string     = CStringCreateF (length,
"\
%s\
----Wo-----\n\
     type:%s\n\
     file:%s\n\
",                 sxt,
                   StringNotNull(opart->type),
                   StringNotNull(fname));

  CStringDelete    (sxt);

  return           string;
}
/***************************************************************************/
Widget OWidgetGetParent (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return NULL;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return NULL;
  return (Widget)(opart->wui_parent);
}
/***************************************************************************/
void* OWidgetGetFile (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return NULL;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return NULL;
  return opart->oui;
}
/***************************************************************************/
char* OWidgetGetType (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return NULL;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return NULL;
  return opart->type;
}
/***************************************************************************/
void OWidgetSetVolatile (
 Widget This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return;
  opart->volatil = a_value;
}
/***************************************************************************/
int OWidgetIsVolatile (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return 0;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return True;
  return opart->volatil;
}
/***************************************************************************/
void OWidgetSetVolatileChild (
 Widget This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return;
  opart->volatilChild = a_value;
}
/***************************************************************************/
int OWidgetIsVolatileChild (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return 0;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return True;
  return opart->volatilChild;
}
/***************************************************************************/
void OWidgetDelete (
 Widget This 
,Boolean  a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;

/*destroy Xt widget*/
  if(Class.verbose==OTraceWidget) CInfoF("Delete %s (%s).\n",XtName(This),XWidgetGetClassName(This));

  if(a_mark==True)
    {
      OInterfaceMark   ((OInterface)OWidgetGetFile(This)          ,1);
      OInterfaceMark   ((OInterface)OWidgetGetFile(XtParent(This)),1);
    }

  if(This==XWidgetGetTop()) 
    { /* destroy children of top items but not top widget */
      WidgetList items,itema;
      items      = XWidgetGetAllChildren(This,NULL);
      for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) XtDestroyWidget(*itema);
      CListDelete ((CList)items);
    }
  else
    {
      XtDestroyWidget (This);  
    }
/* if window does not exist (widget not realized): some implementation could crash in XtDestroyWidget */

  if(Class.verbose==OTraceWidget) CInfo ("Delete end.\n");
}
/***************************************************************************/
void OWidgetKillChildren (
 Widget This 
,Boolean  a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList items,itema;
/*.........................................................................*/
  if(This==NULL) return;
  items = XWidgetGetAllChildren(This,NULL);  /* children + popups */
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) OWidgetDelete (*itema,a_mark);
  CListDelete ((CList)items);
}
/***************************************************************************/
void* OWidgetBuildPrivatePart (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  opart = OPartGetIdentifierFromWidget(This);
  if(opart!=NULL) return (void*)opart; /*done*/

  if(OClassGetIdentifier(XWidgetGetClassName(This))==NULL)
    {
      CWarnF ("For %s, %s class not known.\n",XtName(This)!=NULL?XtName(This):"",XWidgetGetClassName(This));
      return NULL; 
    }

  return (void*)CreatePrivatePart(This);
}
/***************************************************************************/
static OPart CreatePrivatePart (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart  opart;
/*.........................................................................*/
  opart = OPartCreate(This);
  if(opart==NULL) return NULL;

  if(Class.trans_table==NULL)
    {
      static XtActionsRec   actions[] = {
        {"WoMove"          ,MoveAction},
        {"WoResize"        ,ChangeSizeAction},
        {"WoDragAndDrop"   ,NULL}
      };
      if( (Class.dragAndDropAction!=NULL) && (Class.dragAndDropTable!=NULL) )
	{
          char*            string;
          actions[2].proc  = Class.dragAndDropAction;
	  XtAppAddActions  (XtWidgetToApplicationContext(This),actions,3);
	  string           = CStringCreateF (512+strlen(Class.dragAndDropTable),"\
                Shift <Btn2Down>: WoMove()\n\
        Shift Button2 <PtrMoved>: WoMove()\n\
                  Shift <Btn2Up>: WoMove()\n\
                 Ctrl <Btn2Down>: WoResize()\n\
         Ctrl Button2 <PtrMoved>: WoResize()\n\
                   Ctrl <Btn2Up>: WoResize()\n\
                              %s: WoDragAndDrop()",Class.dragAndDropTable);
	  Class.trans_table = XtParseTranslationTable(string);
          CStringDelete     (string);
	}
      else
	{
	  XtAppAddActions   (XtWidgetToApplicationContext(This),actions,2);
	  Class.trans_table = XtParseTranslationTable("\
                Shift <Btn2Down>: WoMove()\n\
        Shift Button2 <PtrMoved>: WoMove()\n\
                  Shift <Btn2Up>: WoMove()\n\
                 Ctrl <Btn2Down>: WoResize()\n\
         Ctrl Button2 <PtrMoved>: WoResize()\n\
                   Ctrl <Btn2Up>: WoResize()");
	}
    }

  if(XtIsWidget(This)) 
    {
      XtOverrideTranslations (This,Class.trans_table);
      XtAddEventHandler      (This,EnterWindowMask|LeaveWindowMask,False ,HelpHandler,NULL);
    }

  XtAddCallback (This,XtNdestroyCallback,DestroyCallback,NULL);

  return        opart;
}
/***************************************************************************/
static void DestroyCallback (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.verbose==OTraceWidget) CInfoF("DestroyCallback:%s %s.\n",XtName(This),XWidgetGetClassName(This));
  if(Class.destroy!=NULL) Class.destroy (This,a_tag,a_data);
  OPartDelete (OPartGetIdentifierFromWidget(This));
  if(Class.verbose==OTraceWidget) CInfo ("DestroyCallback:end.\n");
}
/***************************************************************************/
static void HelpHandler (
 Widget This 
,XtPointer a_tag
,XEvent* a_event
,Boolean* a_dispatch
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.toggle_help==0) return;
  if(XtHasCallbacks(This,"helpCallback")!=XtCallbackHasSome) return;
       if(a_event->type==EnterNotify)
    {
      XtCallCallbacks (This,"helpCallback",NULL);
    }
  else if(a_event->type==LeaveNotify)
    {
      XWidgetUnmap    (XWidgetGetIdentifier("xmask_ballon"));
    }

  a_tag      = NULL; /* avoid C++ warnings */
  a_dispatch = NULL; /* avoid C++ warnings */
}
/***************************************************************************/
void OWidgetSetFile (
 Widget This 
,void* a_oui
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart   opart;
/*.........................................................................*/
  opart           = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return;
  opart->oui      = a_oui;
}
/***************************************************************************/
Widget OWidgetGetShell (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget widget;
/*.........................................................................*/
  if(This==NULL) return NULL;
  widget = This;
  while(1)
    {
      if(widget==NULL) return NULL;
      if(OWidgetIsShell(widget)==1) return widget;
      widget = XtParent(widget);
    }
}
/***************************************************************************/
char** OWidgetGetPossibleChildren (
 Widget This 
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     number;
  char**  list;
  OClass* items;
  OClass* itema;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return NULL;
  number    = CListGetSize((CList)OClassGetIdentifiers());
  list      = (char**)CMemoryAllocateBlock( number * sizeof(char*));
  if(list==NULL) return NULL;
  number    = 0;
  items     = OClassGetIdentifiers();
  if(XtIsComposite(This))
    { /* all widget types acceptables */
      for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
        {
          if(OClassGetName(*itema)!=NULL)
	    {
	      list[number] = OClassGetName(*itema);
	      number++;
	    }
        }
    }
  else
    { /* only shells could be children (added in popup_list) */
      for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
        { 
          if( (OClassIsShell(*itema)==1) && (OClassGetName(*itema)!=NULL) )
            {
              list[number] = OClassGetName(*itema);
              number++;
            }
        }
    }
  if(a_number!=NULL) *a_number = number;
  return    list;
}
/***************************************************************************/
Widget OWidgetDuplicate (
 Widget This 
,char* a_name   
,Widget a_parent 
,Boolean a_mark 
)
/***************************************************************************/
{
  Widget  widget,parent;
  char*   name;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  parent      = a_parent!=NULL ? a_parent : XtParent(This) ;
  if(parent==NULL) return NULL;
  name           = BuildName (parent,a_name);
  if(name==NULL) return NULL;
  widget         = OWidgetCreate(name,parent,OWidgetGetType(This),a_mark);
  CStringDelete (name);
  if(widget==NULL)   return NULL;

  LOOP_BEGIN_RES(This)
      if( (res_name!=NULL) && (IsResourceMarked(This,res_count)==1) ) 
	{
	  char*                value;
	  value                = GetResource (This,res_name,res_type,res_size);
	  OWidgetSetResourceFromString (widget,res_name,value,a_mark);
	  CStringDelete        (value);
	}
  LOOP_END_RES

 {char*                         string;
  string                        = OWidgetGetAndConvertResourceToString  (widget,"createCallback");
  OWidgetRequireScriptExecution (widget,string,NULL,0);
  CStringDelete                 (string);}

  return              widget;
}
/***************************************************************************/
Widget OWidgetDuplicateHierarchy (
 Widget This 
,char* a_name   
,Widget a_parent 
,Boolean a_mark 
)
/***************************************************************************/
{
  Widget     dwidget,parent;
  char*      name;
  WidgetList items,itema;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  parent      = a_parent!=NULL ? a_parent : XtParent(This) ;
  if(parent==NULL) return NULL;
  name          = BuildName (parent,a_name);
  if(name==NULL)     return NULL;

  dwidget       = DuplicateTree (This,name,parent,a_mark);

  CStringDelete (name);
  if(dwidget==NULL)  return NULL;

  DuplicateResources (dwidget,This,a_mark);

/* reset duplicate field */
  items = XWidgetGetLeaves(This);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++) SetDuplicate (*itema,NULL);
  CListDelete ((CList)items);

/* send create */
  items = XWidgetGetLeaves(dwidget);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      char*                         string;
      string                        = OWidgetGetAndConvertResourceToString  (*itema,"createCallback");
      OWidgetRequireScriptExecution (*itema,string,NULL,0);
      CStringDelete                 (string);
    }
  CListDelete ((CList)items);

  return   dwidget;
}
/***************************************************************************/
static Widget DuplicateTree (
 Widget This
,char* a_name
,Widget a_parent
,Boolean a_mark
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget   widget;
/*.........................................................................*/
  if(This==NULL)     return NULL;
  if(a_name==NULL)   return NULL;
  if(a_parent==NULL) return NULL;

  if(OWidgetGetFile(This)!=NULL)
    {
      widget        = OWidgetCreate (a_name,a_parent,OWidgetGetType(This),a_mark);
      if(widget==NULL)   return NULL;
    }
  else
    {
      widget = a_parent;
    }
  SetDuplicate (This,widget);

/*duplicate children: look also write_children in OInterface.c */
 {WidgetList items,itema;
  items      = XWidgetGetAllChildren (This,NULL);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      char*          name;
      name           = BuildName (a_parent,NULL);
      DuplicateTree (*itema,name,widget,a_mark);
      CStringDelete  (name);
    }
  CListDelete ((CList)items);}

  return widget;
}
/***************************************************************************/
static void SetDuplicate (
 Widget This 
,Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return;
  opart->duplicate = a_widget;
}
/***************************************************************************/
static Widget GetDuplicate (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart opart;
/*.........................................................................*/
  if(This==NULL) return NULL;
  opart = OPartGetIdentifierFromWidget(This);
  if(opart==NULL) return NULL;
  return (Widget)(opart->duplicate);
}
/***************************************************************************/
static void DuplicateResources (
 Widget This
,Widget a_from
,Boolean a_mark
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Boolean       dup_attach;
   int  count;
  int           nres = 0;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_from==NULL) return;
  if(Class.xclassXmForm==NULL)
    {
      Class.xclassXmForm = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("XmForm"));
    }
  if(Class.xclassXmForm==NULL) return;

  dup_attach     =  XtIsSubclass(XtParent(This),Class.xclassXmForm) &&
                    XtIsSubclass(XtParent(a_from),Class.xclassXmForm) ? True : False;

  if(Class.ress==NULL) 
    {
      Class.ress = (char**)CMemoryAllocateBlock(MAX_RES * sizeof(char*));
      if(Class.ress==NULL) return;
    }
  nres           = 0;
  XResourceChangesInitialize ();   /* use res list for widget attachment */
  LOOP_BEGIN_RES(a_from)
      if( (res_name!=NULL) && (IsResourceMarked(a_from,res_count)==1) ) 
	{
	  char*                value;
	  Boolean              is_attach;
	  value                = GetResource  (a_from,res_name,res_type,res_size);
	  is_attach            = OResourceIsAttachment (NULL,res_name);
/* widget list ?????? */
	  if( (res_type!=NULL) && (strcmp(res_type,"WidgetList")==0) )
	    { 
	      WidgetList          list;
	      int                 number,count,ipos;
	      char*               string;
	      list                = XWidgetGetWidgetListResource (a_from,res_name,&number);
	      string              = (char*)CMemoryAllocateBlock  (number * 64 * sizeof(char));
	      if(string!=NULL)          
		{
		  ipos                = CStringPrintF (string,"");
		  for(count=0;count<number;count++)
		    { Widget              from_connected,to_connected;
		      from_connected      = list[count];
		      if(XWidgetIsValid(from_connected)==True) 
			{
			  to_connected        = GetDuplicate(from_connected);
			  if(XWidgetIsValid(to_connected)==True) 
			    {
			      ipos += CStringPrintF  (string+ipos,"%s\\n",XtName(to_connected));
			    }
			}        
		    }
		  if(ipos!=0)
		    { /* remove last \n */
		      string[ipos-2] = 0;
		    }
		  OWidgetSetResourceFromString  (This,res_name,string,a_mark);
		  Class.ress[nres]     = string;
		  nres++;}
	    }
      else if( (res_name!=NULL) && 
	       (res_type!=NULL) && 
	      ((strcmp(res_name,"workWindow")==0) || /* for XmScrolledWindow */
	       (strcmp(res_type,"MenuWidget")==0) ||
	       (strcmp(res_type,"Widget")==0)     || /* XoGraph */
	       ( (is_attach==1)     && 
		 (dup_attach==True) && 
		 (strcmp(res_type,"Window")==0) )   /* XmForm Widget attachement */
	       )
	      )
        { Widget              from_connected,to_connected;
          from_connected      = XWidgetGetIdentifier(value);
          if(XWidgetIsValid(from_connected)==True) 
            {
              to_connected        = GetDuplicate(from_connected);
              if(XWidgetIsValid(to_connected)==True) 
                {
                  OWidgetSetResourceFromString  (This,res_name,XtName(to_connected),a_mark);
                }
            }
        }
      else if(is_attach==1)
        {
          if(dup_attach==True) 
            {
              OWidgetSetResourceFromString  (This,res_name,value,a_mark);
            }
        }
      else
        {
          OWidgetSetResourceFromString  (This,res_name,value,a_mark);
        }
      Class.ress[nres]           = value;
      nres++;
      /*CStringDelete        (value);*/
      }
  LOOP_END_RES
  XResourceChangesCommit ();
  for(count=0;count<nres;count++) {CStringDelete(Class.ress[count]);}
  nres = 0;

 {WidgetList   items_This,items_from;
  int          number_This,number_from,count;
  items_This   = XWidgetGetAllChildren (This,NULL);
  number_This  = CListGetSize          ((CList)items_This);
  items_from   = XWidgetGetAllChildren (a_from,NULL);
  number_from  = CListGetSize          ((CList)items_from);
  if( number_This != number_from)
    {
      CWarn ("Try to duplicate resource of not similar hierarchy.\n");
      return;
    }
  for(count=0;count<number_This;count++) 
    { 
      DuplicateResources (items_This[count],items_from[count],a_mark);
    }
  CListDelete ((CList)items_This);
  CListDelete ((CList)items_from);}

}
/***************************************************************************/
void OWidgetMove (
 Widget This 
,Position a_x 
,Position a_y 
,Boolean a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  XWidgetMove (This,a_x,a_y);
  OWidgetMarkResource (This,XtNx,a_mark);
  OWidgetMarkResource (This,XtNy,a_mark);
}
/***************************************************************************/
void OWidgetSetWindowManagerCloseAction (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  XWidgetSetWindowManagerCloseFunction (This,CloseWindow);
}
/***************************************************************************/
static void CloseWindow (
 Widget This 
,XEvent* a_event 
,char** a_args
,Cardinal* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                   return;
  if(a_event==NULL)                return;
  if(a_event->type!=ClientMessage) return;
  OEventSendExit (XtDisplay(This),0);
  a_argn  = 0;
  a_args  = NULL; 
}
/***************************************************************************/
void OWidgetSaveHierarchy (
 Widget This 
,char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OInterface        noui,ooui;
  WidgetList items,itema;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_fname==NULL) return;

  if( (CFileIsValid(a_fname)==1) ||
      (OInterfaceGetIdentifier(a_fname)!=NULL)
     )
    {
      CWarnF("File %s already exists.\n",a_fname);
      return;
    }

  ooui           = (OInterface)OWidgetGetFile (This);
  noui           = OInterfaceCreate (a_fname,a_fname);
  OInterfaceSetPrefix (noui,OInterfaceGetPrefix(ooui));

/* sub widget belonging to This hierachy */
  items = XWidgetGetLeaves(This);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if((OInterface)OWidgetGetFile(*itema)==ooui)  OWidgetSetFile (*itema,noui);
    }

  OInterfaceMark (noui,True);
  OInterfaceSave       (noui);

/* restore oui */
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      if((OInterface)OWidgetGetFile(*itema)==noui)  OWidgetSetFile (*itema,ooui);
    }

  CListDelete ((CList)items);

  OInterfaceDelete (noui);
}
/***************************************************************************/
static void SetTarget (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart            opart;
/*.........................................................................*/
  if(This==NULL)        return;
  opart            = OPartGetIdentifierFromWidget(This);
  if(opart==NULL)       return;
  if(This==XWidgetGetTarget()) 
    {
      OClassVisitWidgetFunction targetProc;
      targetProc                = OClassGetTargetChangedFunction(OClassGetIdentifier(OWidgetGetType(This))); 
      if(targetProc!=NULL)      targetProc (This);
    }
  OWidgetRequireScriptExecution (This,opart->targetChangedCallback,NULL,0); 
}
/***************************************************************************/
void OWidgetPutPageInPS_Stream (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget          target;
  double          red,green,blue;
/*.........................................................................*/
  if(This==NULL)               return;
  if(CPS_GetStream()==NULL)    CPS_OpenFileForWriting (NULL);
  if(CPS_GetStream()==NULL)    return;

  target                       = XWidgetGetTarget ();
  XWidgetSetTarget             (NULL);

  XWidgetGetDimension          (This,&Class.ww,&Class.wh);
  Class.wx                     = 0;
  Class.wy                     = 0;
  XWidgetGetRootPosition       (This,&Class.wx,&Class.wy);

  XDisplayGetPixelRGB          (XtDisplay(This),XWidgetGetPixelResource(This,"background"),&red,&green,&blue);

  CPS_PutBeginPageInStream     ();
  CPS_PutPageScaleInStream     ((double)Class.ww,(double)Class.wh);
  CPS_PutBackgroundInStream    (red,green,blue,(double)Class.ww,(double)Class.wh); /*clip*/
  XWidgetVisitHierarchyForward (This,WriteWidget);
  CPS_PutEndPageInStream       ();

  XWidgetSetTarget             (target);
}
/***************************************************************************/
static void WriteWidget (
 Widget This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OClassVisitWidgetFunction psProc;
/*.........................................................................*/
  if(This==NULL)                    return;
  if(XWidgetIsVisible(This)==False) return;

/*printf ("debug : OWidgetPS : begin : %s\n",XtName(This));*/

  psProc = OClassGetPS_Function(OClassGetIdentifier(OWidgetGetType(This))); 
  if(psProc!=NULL)
    {
      Position                    xpos,ypos;
      Dimension                   ww,wh;
      XWidgetGetDimension         (This,&ww,&wh);
      xpos                        = 0;
      ypos                        = 0;
      XWidgetGetRootPosition      (This,&xpos,&ypos);
      xpos                        = xpos - Class.wx;
      ypos                        = (Class.wy+Class.wh) - (ypos+wh);
      CPS_PutSaveStateInStream    ();
      CPS_PutTranslationInStream  ((double)xpos,(double)ypos);
      psProc                      (This);
      CPS_PutRestoreStateInStream ();
    }

/*printf ("debug:OWidgetPS:end:%s\n",XtName(This));*/

}
/***************************************************************************/
char* OWidgetGetLocalTarget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*        prefix;
  char*        string;
  Widget       text;
/*.........................................................................*/
  prefix         = OInterfaceGetPrefix((OInterface)OWidgetGetFile(This));
  if(prefix==NULL)  return NULL;
  string         = CStringCreateF (strlen(prefix)+11,"%slocalTarget",prefix);
  text           = XWidgetGetIdentifier(string);
  CStringDelete  (string);
  if(text==NULL) return NULL;
  string         = OWidgetGetAndConvertResourceToString(text,XtNvalue);
  if( (string==NULL) ||  (*string=='\0') )  
    {
      CStringDelete (string);
      string        = XWidgetGetTarget()!=NULL ? CStringDuplicate(XtName(XWidgetGetTarget())) : CStringNULL ;
    }
  return string;
}
/***************************************************************************/
char** OWidgetGetResources (
 Widget This
,char* a_filter
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define RES_FORMAT "%-40s %-40s %-20s %u"
  char**         lines = NULL;
  int            linen = 0;
  XtResourceList lres;
  Cardinal       nres;
  Cardinal       count;
  char*          string;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)     return NULL;
  if(a_filter==NULL) return NULL;

  string = CStringCreate (256);
  sprintf (string,RES_FORMAT,"createCallback",XtCCallback,XtRCallback,(unsigned int)sizeof(XtCallbackList));
  if( (strcmp(a_filter,"all")==0) || 
      (CStringMatchPattern("createCallback",a_filter)==1)
     ) CTextAddLine(&linen,&lines,string);
  sprintf (string,RES_FORMAT,"eventHandler","EventHandler",XtRPointer,(unsigned int)sizeof(XtPointer));
  if( (strcmp(a_filter,"all")==0) || 
      (CStringMatchPattern("eventHandler",a_filter)==1) ) 
    CTextAddLine(&linen,&lines,string);

  XtGetResourceList (XtClass(This),&lres,&nres);
  for(count=0;count<nres;count++)
    {
      if(     (strcmp(a_filter,"all")==0) ||
              ( (strcmp(a_filter,"Link")==0) && 
	        (OResourceIsReferenceList(lres[count].resource_type,NULL)==1) 
	      ) ||
              (CStringMatchPattern(lres[count].resource_name,a_filter)==1)
        )  
        {
          sprintf (string,RES_FORMAT,lres[count].resource_name,
		                     lres[count].resource_class,
		                     lres[count].resource_type,
              		             lres[count].resource_size);
          CTextAddLine(&linen,&lines,string);
        }
    }  
  XtFree    ((char *)lres);
  CTextSort (linen,lines);

  if(XWidgetIsConstraint(This)==True) 
    {
      char**       clines = NULL;
      int          clinen = 0;
      CTextAddLine(&linen,&lines,"----- constraint resources -----");
      XtGetConstraintResourceList (XtClass(XtParent(This)),&lres,&nres);
      for(count=0;count<nres;count++)
	{
	  if( (strcmp(a_filter,"all")==0) ||
	     ( (strcmp(a_filter,"Link")==0) && 
	       (OResourceIsReferenceList(lres[count].resource_type,NULL)==1)
	      ) ||
	      (CStringMatchPattern(lres[count].resource_name,a_filter)==1)
	     )  
	    {
	      sprintf (string,RES_FORMAT, lres[count].resource_name,
					  lres[count].resource_class,
					  lres[count].resource_type,
					  lres[count].resource_size);
	      CTextAddLine(&clinen,&clines,string);
	    }
	}  
      XtFree    ((char *)lres);
      CTextSort (clinen,clines);
      CTextConcatenate (&linen,&lines,clinen,clines);
      CTextDelete (clinen,clines);
    } /* end hasConstraints */

  CStringDelete (string);

  if(a_number!=NULL) *a_number = linen;
  return lines;
}
/***************************************************************************/
Widget OWidgetGetIdentifier (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return XWidgetGetIdentifier(a_string);
}
/***************************************************************************/
char* OWidgetGetThisValue (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringDuplicate (XWidgetGetCallValue());
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OWidgetSetAttribute (
 Widget This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

  if(This==NULL)
    {
       if(strcmp(a_name,"filter")==0) 
	 {
	   CStringDelete (Class.filter);
           Class.filter  = CStringDuplicate(*((char**)a_addr));
	 }
       else if(strcmp(a_name,"thisValue")==0) 
	 {
	   char*        xstring;
	   XtPointer    data;
	   int          type;
	   data              = XWidgetGetCallData (&type);
	   /*type = 0;called from a        callback, data is the a_data  pointer.*/
	   /*type = 1;called from an event handler,  data is the a_event pointer.*/
	   if(type==0)
	     {
	       xstring = OResourceGetCallbackData (XWidgetGetThisWidget(),data,*((char**)a_addr));
	       XtFree  (xstring);
	     }
	 }
      else
	{
	  if(XWidgetSetAttribute(This,a_name,a_user,a_addr,a_number)==0) return 0;
	}
      return 1;
    }

  if(strcmp(a_name,"type")==0)  
    {
    }
  else
    {
      if(XWidgetSetAttribute (This,a_name,a_user,a_addr,a_number)==0) return 0;
    }
  return 1;
}
/***************************************************************************/
int OWidgetGetAttribute (
 Widget This
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
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

  if(This==NULL)
    {
       if(strcmp(a_name,"filter")==0) 
	 {
           *((char**)a_addr) = Class.filter;
	 }
       else if(strcmp(a_name,"thisValue")==0) 
	 {
           *((char**)a_addr) = OWidgetGetThisValue ();
	   return FREE_BLOCK;
	 }
       else
	 {
	   return XWidgetGetAttribute (This,a_name,a_user,a_addr,a_number);
	 }
       return 1;
    }

       if(strcmp(a_name,"type")==0)
    {
      *((char**)a_addr) = OWidgetGetType (This);
    }
  else if(strcmp(a_name,"resources")==0)
    {
      int           stringn = 0;
      char**        strings = NULL;
      if(Class.filter==NULL) Class.filter   = CStringDuplicate ("all");
      strings      = OWidgetGetResources  (This,Class.filter,&stringn);
      if(a_number!=NULL) *a_number = stringn;
      *((char***)a_addr) = strings;
      return FREE_BLOCKS;
    }
  else 
    {
      return XWidgetGetAttribute (This,a_name,a_user,a_addr,a_number);
    }

  return 1;
}
/***************************************************************************/
int OWidgetDoMethod (
 Widget This
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
/*       if(strcmp(a_name,"grabRootWindowPart")==0)  
	 {
	 }
       else */
	 {
	   return XWidgetDoMethod (This,a_name,a_user,a_argn,a_args,a_addr,a_number);
	 }
    }


       if(strcmp(a_name,"putPageInPS")==0)       
    {
      OWidgetPutPageInPS_Stream (This);
    }
  else if(strcmp(a_name,"setResource")==0)       
    {
      if(a_argn!=2) 
	{
	  CWarnF  ("Two arguments expected for %s property.\n",a_name);
	  return 0;
	}
      OWidgetSetResourceFromString (This,a_args[0],a_args[1],False);
    }
  else if(strcmp(a_name,"setAndMarkResource")==0)       
    {
      if(a_argn!=2) 
	{
	  CWarnF  ("Two arguments expected for %s property.\n",a_name);
	  return 0;
	}
      OWidgetSetResourceFromString (This,a_args[0],a_args[1],True);
    }
  else if(strcmp(a_name,"getResource")==0)       
    {
      if(a_argn!=1) 
	{
	  CWarnF  ("Two arguments expected for %s property.\n",a_name);
	  return 0;
	}
      *((char**)a_addr) = OWidgetGetAndConvertResourceToString (This,a_args[0]);
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"addCallback")==0)       
    {
      if(a_argn!=2) 
	{
	  CWarnF  ("Two arguments expected for %s property.\n",a_name);
	  return 0;
	}
      OWidgetAddCallback   (This,a_args[0],a_args[1]);
    }
  else if(strcmp(a_name,"addEventHandler")==0)       
    {
      unsigned long mask;
      if(a_argn!=2) 
	{
	  CWarnF  ("Two arguments expected for %s property.\n",a_name);
	  return 0;
	}
      mask                   = XStringGetMask (a_args[0]);
      OWidgetAddEventHandler (This,mask,a_args[1]);
    }
  else if(strcmp(a_name,"saveHierarchy")==0)       
    {
      if(a_argn!=1) 
	{
	  CWarnF  ("One argument expected for %s property.\n",a_name);
	  return 0;
	}
      OWidgetSaveHierarchy (This,a_args[0]);
    }
  else if(strcmp(a_name,"markFile")==0)       
    {
      OInterfaceMark   ((OInterface)OWidgetGetFile(This),1);
    }
  else if(strcmp(a_name,"markResource")==0)       
    {
      if(a_argn!=1) 
	{
	  CWarnF  ("One argument expected for %s property.\n",a_name);
	  return 0;
	}
      OWidgetMarkResource  (This,a_args[0],1);
    }
  else if(strcmp(a_name,"unmarkResource")==0)       
    {
      if(a_argn!=1) 
	{
	  CWarnF  ("One argument expected for %s property.\n",a_name);
	  return 0;
	}
      OWidgetMarkResource  (This,a_args[0],0);
    }
  else if(strcmp(a_name,"duplicate")==0)       
    {
      Widget parent;
      if(a_argn!=1)
	{
	  CWarnF  ("One argument expected for %s property.\n",a_name);
	  return 0;
	}
      parent           = XWidgetGetIdentifier (a_args[0]);
      OWidgetDuplicate (This,NULL,parent,False);
    }
  else if(strcmp(a_name,"duplicateHierarchy")==0)       
    {
      Widget parent;
      if(a_argn!=1)
	{
	  CWarnF  ("One argument expected for %s property.\n",a_name);
	  return 0;
	}
      parent = XWidgetGetIdentifier (a_args[0]);
      OWidgetDuplicateHierarchy (This,NULL,parent,False);
    }
  else if(strcmp(a_name,"getInformations")==0)
    {
      *((char**)a_addr)   = OWidgetGetInformations (This);
      return FREE_BLOCK;
    }
  else
    {
      return XWidgetDoMethod (This,a_name,a_user,a_argn,a_args,a_addr,a_number);
    }
  return 1;
}
/***************************************************************************/
void* OWidgetConstruct (
 void*  a_type
,int    a_parn
,char** a_pars
,char** a_vals
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     count;
  char*   name      = NULL;
  char*   type      = NULL;
  char*   sparent   = NULL;
  char*   markFile  = NULL;
  Widget  parent;
  Boolean mark;
  Widget  This;
/*.........................................................................*/
  if(a_parn==0) return NULL;
  for(count=0;count<a_parn;count++)
    {
      char*  string;
      string = a_pars[count];
           if( (string!=NULL) && (strcmp(string,"name")==0) )      name     = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"type")==0) )      type     = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"parent")==0) )    sparent  = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"markFile")==0) )  markFile = a_vals[count];
      else
        { 
	  CWarnF ("OWidgetConstruct: %s is not a Widget property.\n",string);
          return NULL;
	}
    }
  if(XWidgetGetIdentifier(name)!=NULL)
    { 
      CWarnF   ("OWidgetConstruct: widget %s already exist.\n",name);
      return   NULL;
    }
  if(type==NULL)
    {
      CWarn    ("OWidgetConstruct: no type given.\n");
      return   NULL;
    }
  if(sparent==NULL)
    {
      CWarn    ("OWidgetConstruct: no parent given.\n");
      return   NULL;
    }

  if(markFile==NULL)
    {
      mark = 0;
    }
  else
    {
      int   status;
      mark  = (Boolean)CStringConvertToBoolean (markFile,&status);
      if(status==0)
        {
	  CWarnF ("OWidgetConstruct: Can't convert %s to boolean.\n",markFile);
	  return NULL;
	}
    }

  parent  = XWidgetGetIdentifier(sparent);

  This    = OWidgetCreate (name,parent,type,mark);
  if(This==NULL) return NULL;

 {Dimension            ww,wh;
  Boolean              changed;
  XWidgetGetDimension  (This,&ww,&wh);
  changed              = False;
  if(ww<=1)            {ww = 20;changed = True;}
  if(wh<=1)            {wh = 20;changed = True;}
  if(changed==True)    XWidgetSetDimension(This,ww,wh);
  if( XtIsSubclass(This,compositeWidgetClass) &&
     (XWidgetGetPixelResource(This,XtNbackground)==XWidgetGetPixelResource(parent,XtNbackground))) 
    {
      XWidgetSetPixelResource(This,XtNbackground,XWidgetGetPixelResource(parent,XtNforeground));
    }}

/*todo: if(XtIsSubclass(parent,xmRowColumnWidgetClass)) */ 
  XWidgetExecuteChangeSizeFunction (parent);

  a_type = NULL;
  return This;
}
/***************************************************************************/
static char* GetCallValue (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtPointer data;
  int       type;
  char*     xstring;
  char*     string;
/*.........................................................................*/
  if(This==NULL) return NULL;
  data              = XWidgetGetCallData (&type);
  /*type = 0;called from a        callback, data is the a_data  pointer.*/
  /*type = 1;called from an event handler,  data is the a_event pointer.*/
  if(type==1) return NULL;
  xstring = OResourceGetCallbackData (This,data,NULL);
  string  = CStringDuplicate(xstring);
  XtFree  (xstring);
  return  string;
}
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
  CErrorHandle ("Wo","OWidget",a_name,a_code);
}
