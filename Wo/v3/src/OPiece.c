/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdarg.h>
#include <stdlib.h>

#include <CList.h>
#include <CError.h>
#include <CMemory.h>
#include <CString.h>
#include <CPrinter.h>
#include <CText.h>

#include <XWidget.h>

#include <OClass.h>
#include <OWidget.h>

#include <OPieceP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void     FreeRecord       (OPiece);
static int      GetIdentifier    (OPiece,char*);
static void     GetNumberOfNodes (OPiece,int*);
static void     AddNodeInList    (OPiece,int*,OPiece*);
static Boolean  IsChild          (char*,char*);
static void     ReportError      (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;

static struct 
{
  OPiece* extent;
} Class = {NULL};
/***************************************************************************/
void OPieceClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy   ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent  = NULL;
}
/***************************************************************************/
OPiece* OPieceGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OPiece OPieceGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OPiece)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OPiece  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OPieceGetName(This),a_name);
}
/***************************************************************************/
static void FreeRecord (
 OPiece This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  CStringDelete     (This->name);
  CStringDelete     (This->type);
  CStringDelete     (This->dataBaseName);
  CStringDelete     (This->stringChildren);
  CMemoryFreeBlock  (This->children);
  CTextDelete       (This->resn,This->rnms);
  CTextDelete       (This->resn,This->rvls);
  This->resn        = 0;
  This->rnms        = NULL;
  This->rvls        = NULL;
  This->name        = NULL;
  This->type        = NULL;
  This->dataBaseName= NULL;
  This->stringChildren = NULL;
  This->nchild         = 0;
  This->children       = NULL;
  CMemoryFreeBlock     (This);
}
/***************************************************************************/
OPiece OPieceMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPiece This;
/*.........................................................................*/
  This               = (OPiece)CMemoryAllocateBlock (sizeof(OPieceRecord));
  if(This==NULL)     return NULL;
  CListAddEntry      ((CList*)&(Class.extent),This);
  This->name           = NULL;
  This->type           = NULL;
  This->stringChildren = NULL;
  This->nchild       = 0;
  This->children     = NULL;
  This->parent       = NULL;
  This->volatil      = 0;
  This->volatilChild = 0;
  This->oui          = NULL;
  This->dataBaseName = NULL;
  This->duplicate    = NULL;
  This->xtWidget     = NULL;
  This->resn         = 0;
  This->rnms         = NULL;
  This->rvls         = NULL;
  return             This;
}
/***************************************************************************/
OPiece OPieceCreate (
 char*  a_name 
,char*  a_type 
,void*  a_xtWidget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPiece This;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0'))  
    {
      ReportError("OPieceCreate",CErrorValue);
      return NULL;
    } 

  if(CStringIsWord(a_name)==0) 
    {
      CWarnF ("Bad name:%s\n  (must have same syntax than a C variable).\n",a_name);
      return NULL;
    }
  This               = OPieceMake();
  if(This==NULL)     return NULL;

  This->name         = CStringDuplicate(a_name);
  This->type         = CStringDuplicate(a_type);
  This->xtWidget     = a_xtWidget;

  return             This;
}
/***************************************************************************/
void OPieceDelete (
  OPiece This 
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
int OPieceSetParent (
 OPiece This 
,OPiece a_parent 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return 0;
  if(a_parent==NULL) return 0;
  if(IsChild(a_parent->type,This->type)==0)
    {
      CWarnF("Bad parent %s (%s) for %s (%s)\n",a_parent->name,a_parent->type,This->name,This->type);
      return 0;
    }
/* do child,parent connection */
  return OPieceAddChild(a_parent,This);
}
/***************************************************************************/
int OPieceAddChild (
 OPiece This 
,OPiece a_child
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return 0;
  if(a_child==NULL)  return 0;
/* check child,parent connection */
  if(a_child->parent!=NULL)
    {
      CWarnF("Child %s has already a parent \n",a_child->name);
      return 1;  /* not considerd as incorrect */
    }
/* do child,parent connection */
  This->children = !This->nchild ? (OPiece*)CMemoryAllocateBlock(2 * sizeof(OPiece)) :
                                   (OPiece*)CMemoryChangeBlockSize((char*)(This->children),(This->nchild+2)*sizeof(OPiece));
  if(This->children==NULL) return 0;
  This->children[This->nchild] = a_child;
  This->nchild++;
  This->children[This->nchild] = NULL;
  a_child->parent = This;
  return       1;
}
/***************************************************************************/
OPiece* OPieceGetChildren (
 OPiece This 
)
/***************************************************************************/
/*
  Used only in ouiToodb.c.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->children;
}
/***************************************************************************/
void OPieceSetVolatile (
 OPiece This 
,int a_flag 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->volatil      = a_flag;
  This->volatilChild = a_flag;
}
/***************************************************************************/
void OPieceSetVolatileChild (
 OPiece This 
,int a_flag 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->volatilChild = a_flag;
}
/***************************************************************************/
int OPieceIsVolatile (
 OPiece This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->volatil;
}
/***************************************************************************/
OPiece OPieceGetParent (
 OPiece This 
)
/***************************************************************************/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->parent;
}
/***************************************************************************/
void OPieceSetFile (
 OPiece  This 
,void* a_oui    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->oui = a_oui;
}
/***************************************************************************/
void* OPieceGetFile (
 OPiece  This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->oui;
}
/***************************************************************************/
void OPieceSetName (
 OPiece This 
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->name);
  This->name    = CStringDuplicate (a_value);
}
/***************************************************************************/
void OPieceSetType (
 OPiece This 
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->type);
  This->type    = CStringDuplicate (a_value);
}
/***************************************************************************/
char* OPieceGetType (
 OPiece This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->type;
}
/***************************************************************************/
char* OPieceGetName (
 OPiece This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OPieceSetDataBaseName (
 OPiece This 
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete      (This->dataBaseName);
  This->dataBaseName = CStringDuplicate (a_value);
}
/***************************************************************************/
char* OPieceGetDataBaseName (
 OPiece This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->dataBaseName;
}
/***************************************************************************/
char* OPieceGetStringOfChildren (
 OPiece This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->stringChildren;
}
/***************************************************************************/
void OPieceSetStringOfChildren (
 OPiece This 
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete        (This->stringChildren);
  This->stringChildren = CStringDuplicate (a_value);
}
/***************************************************************************/
void OPieceAddResource (
 OPiece This 
,char* a_name
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( (a_name==NULL)  || (a_value==NULL) ) return;
  CTextAddLine(&(This->resn),&(This->rnms),a_name);
  This->resn--;
  CTextAddLine(&(This->resn),&(This->rvls),a_value);
}
/***************************************************************************/
int OPieceGetResources (
 OPiece This 
,char*** a_names
,char*** a_values
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_names!=NULL)  *a_names  = NULL;
  if(a_values!=NULL) *a_values = NULL;
  if(This==NULL) return 0;
  if(a_names!=NULL)  *a_names  = This->rnms;
  if(a_values!=NULL) *a_values = This->rvls;
  return This->resn;
}
/***************************************************************************/
void OPieceDeleteResources (
 OPiece This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return;
  CTextDelete       (This->resn,This->rnms);
  CTextDelete       (This->resn,This->rvls);
  This->resn      = 0;
  This->rnms      = NULL;
  This->rvls      = NULL;
}
/***************************************************************************/
void* OPieceGetWidget (
 OPiece This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return  This->xtWidget;
}
/***************************************************************************/
void OPieceBuildWidget (
 OPiece This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)           return;
  if(This->xtWidget!=NULL) return; /* already created */
  if(This->parent==NULL) 
    {
      CWarnF("%s (%s) is an orphan widget\n",This->name,This->type);
      return; /* orphan widget   */
    }

  This->xtWidget = OWidgetBuildToolkitPart (This->name,(Widget)This->parent->xtWidget,This->type,
                                          (XtPointer)This->oui,This->volatil,This->volatilChild);

  if(This->xtWidget==NULL)
    {
      CWarnF("For %s; Xt widget not created\n",This->name);
      exit(EXIT_SUCCESS);
    }
}
/***************************************************************************/
void OPieceConnect (
 OPiece    This
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetClass wclass_shell;
  OPiece        oshell,oroot,oparent;
/*.........................................................................*/
  if(This==NULL) return;
  oroot        = va_arg(a_args,OPiece);
  oshell       = va_arg(a_args,OPiece);
  oparent      = va_arg(a_args,OPiece);
  wclass_shell = va_arg(a_args,WidgetClass);

  if( (OPieceGetParent(This)==NULL) && 
      (This!=oroot)  && 
      (This!=oshell) && 
      (This!=oparent) 
     )
    { char*    type;
      OClass   wtype;
      type     = OPieceGetType      (This);
      wtype    = OClassGetIdentifier (type);
      if(wtype!=NULL)
	{
	  if(XClassIsSubclass((WidgetClass)OClassGetWidgetClass(wtype),wclass_shell)==True)
	    {
	      OPieceSetParent       (This,oroot);
	    }          
	  else if(oparent!=NULL)
	    {
	      OPieceSetParent        (This,oparent);
	      OPieceSetVolatileChild (This,1);
	    }          
	  else if(OClassIsShell(wtype)==1)
	    {
	      OPieceSetParent        (This, /*oshell ? oshell :*/ oroot );
	      OPieceSetVolatileChild (This,1);
	    }          
	  else 
	    {
	      CWarnF("\
Widget %s (%s) has no parent and is not a shell:\n\
  it will not be connected\n\
",OPieceGetName (This),type);
	    }
	}
    }
}
/***************************************************************************/
OPiece* OPieceGetLeaves (
 OPiece This       
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList    list;
/*.........................................................................*/
  list               = CListCreateFromTree (This,(CListCountLeafFunction)GetNumberOfNodes,(CListVisitLeafFunction)AddNodeInList);
  if(a_number!=NULL) *a_number = CListGetSize(list);
  return             (OPiece*)list;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void GetNumberOfNodes (
 OPiece This       
,int* a_count
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_count==NULL) return;

  (*a_count)++;

  for(count=0;count<This->nchild;count++)
    { 
      GetNumberOfNodes (This->children[count],a_count);
    }
}
/***************************************************************************/
static void AddNodeInList (
 OPiece This       
,int* a_count
,OPiece* a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_count==NULL) return;

  a_list[*a_count] = This;
  (*a_count)++;

  for(count=0;count<This->nchild;count++)
    { 
      AddNodeInList (This->children[count],a_count,a_list);
    }
}
/***************************************************************************/
static Boolean IsChild (
 char* a_type
,char* a_child 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OClass itype,ictype;
/*.........................................................................*/
  itype = OClassGetIdentifier (a_type);
  if(itype==NULL) return False;
  if(XClassIsSubclass((WidgetClass)OClassGetWidgetClass(itype),compositeWidgetClass)==True) 
    return True;

  ictype = OClassGetIdentifier (a_child);
  if(ictype==NULL) return False;
  if(OClassIsShell(ictype)==1) return True;
  return False;
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
  CErrorHandle ("Wo","OPiece",a_name,a_code);
}
