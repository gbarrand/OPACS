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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <CPrinter.h>
#include <CList.h>
#include <CMemory.h>
#include <CString.h>
#include <CText.h>
#include <CError.h>
#include <CFile.h>
#include <CLkStat.h>

#include <ONodeP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void     ClearRecord              (ONode);
static ONode    FindNode                 (char*);
static void     CountNodes               (ONode,int*);
static void     VisitNodesForward        (ONode,int*,ONode*);
static void     VisitNodesBackward       (ONode,int*,ONode*);
static void     RefreshViews             (CList,int);
static CList    GetAllViews              (ONode);
static void     RequireRefreshment       (ONode);
static void     RequireDrawing           (ONode);
static ONode    Read2r0                  (FILE*);
static ONode    Read3r0                  (FILE*);
static int      ReadPoints               (FILE*,double*,double*,double*);
static int      ReadMatrix               (OMatrix,FILE*);

static void     ClearEntry                 (LkEntry);
static ONode    GetObjectFromEntry         (LkEntry);
static ONode    GetObjectFromChildrenEntry (LkEntry);
#ifdef __cplusplus
}
#endif

#define NotFound      (-1)
#define MINIMUM(a,b)  ((a)<(b)?a:b)
#define MAXIMUM(a,b)  ((a)>(b)?a:b)
typedef unsigned long Ulong;
typedef char*         OString;

static struct 
{
  LkListRec            extent;
  ONode*               identifiers;
  int                  blinkNumber;
  ONodeVoidFunction    startBlink;
  ONodeVoidFunction    stopBlink;
  CList                viewsToRefresh;
  CList                viewsToRedraw;
  ONodeVoidFunction    sendProc;
  ONodeIsValidFunction existProc; 
  ONodeRefreshFunction refreshProc; 
  ONodeRefreshFunction redrawProc; 
  int                  somethingToRefresh;
  int                  somethingToRedraw;
} Class = {
{NULL,NULL},
NULL,
0,
NULL,NULL,
NULL,NULL,
NULL,NULL,NULL,NULL,
0,0};
/***************************************************************************/
void ONodeClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef DEBUG
  printf            ("debug : ONodeClearClass.\n");
#endif

  CListDelete             (Class.viewsToRefresh);
  Class.viewsToRefresh    = NULL;

  CListDelete             (Class.viewsToRedraw);
  Class.viewsToRedraw     = NULL;

  LkListClear             (&(Class.extent),ClearEntry);
  LkListInit              (&(Class.extent));

  CListDelete             ((CList)Class.identifiers);
  Class.identifiers       = NULL;;

  Class.somethingToRefresh = 0;
  Class.somethingToRedraw  = 0;
}
/***************************************************************************/
ONode* ONodeGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int         number,count;
/*.........................................................................*/
#ifdef DEBUG
  printf            ("debug : ONodeGetIdentifiers.\n");
#endif
  number            = LkListCount (&(Class.extent));
  CListDelete       ((CList)Class.identifiers);
  Class.identifiers = (ONode*)CListCreate(number);
  count             = 0;
 {LkIterator        iterator;
  LkEntry           entry;
  iterator          = LkIteratorNew (&(Class.extent));
  while ((entry=LkIteratorNext(iterator))!=NULL)
    { 
      Class.identifiers[count] = GetObjectFromEntry(entry);
      count++;
    }
  LkIteratorDelete (iterator);}
  return           Class.identifiers;
}
/***************************************************************************/
void ONodeSetBlinkFunctions (
 ONodeVoidFunction a_startBlink
,ONodeVoidFunction a_stopBlink
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.startBlink = a_startBlink;
  Class.stopBlink  = a_stopBlink;
}
/***************************************************************************/
int ONodeGetNumberOfBlinks (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.blinkNumber;
}
/***************************************************************************/
void ONodeSetNumberOfBlinksToZero (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.blinkNumber = 0;
}
/***************************************************************************/
ONode ONodeGetIdentifier (
 char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"ONode",5)==0) )
    {
      int        status;
      ONode    identifier;
      identifier = (ONode)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return FindNode (pos+1);
	}
      else
	{
	  if(ONodeIsValid(identifier)==0)
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
      return FindNode (a_name);
    }
}
/***************************************************************************/
static ONode FindNode (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int   found;
  ONode node = NULL;
/*.........................................................................*/
#ifdef DEBUG
  printf            ("debug : FindEntry.\n");
#endif
  found      = 0;
 {LkIterator iterator;
  LkEntry    entry;
  iterator   = LkIteratorNew (&(Class.extent));
  while ((entry=LkIteratorNext(iterator))!=NULL)
    { 
      node  = GetObjectFromEntry(entry);
      if(CStringCompare(node->node.name,a_name)==1)
	{
	  found = 1;
          break;
	}
    }
  LkIteratorDelete (iterator);}
  return           (found==1 ? node : (ONode)NULL) ;
}
/***************************************************************************/
void ONodeInitialize (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                    return;
  LkEntryInit                       (&(This->node.links),&(Class.extent));
  LkListInit                        (&(This->node.children));
  LkEntryInit                       (&(This->node.childrenLinks),NULL);
  This->node.parent                 = NULL;
  This->node.name                   = NULL;
  This->node.views                  = NULL;
  This->node.prims                  = NULL;
  This->node.highLight              = 0;
  This->node.matrix                 = NULL;
  This->node.pickable               = 1;
  This->node.visible                = 1;
  This->node.pickId                 = 0;
  This->node.blinkType              = GBlinkOff;
  This->node.projected              = 1;
  This->node.cameraAligned          = 0;
  This->node.xHook                  = 0.;
  This->node.yHook                  = 0.;
  This->node.zHook                  = 0.;
  This->node.toRefresh              = 0;
  This->node.toRedraw               = 0;
  This->node.isShape                = 0;
  This->node.representShapeFunction = NULL;
  This->node.deleteShapeFunction    = NULL;
  This->node.touched                = 0;
}
/***************************************************************************/
ONode ONodeMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode This;
/*.........................................................................*/
  This            = (ONode) CMemoryAllocateBlock  (sizeof(ONodeRecord));
  if(This==NULL)  return NULL;
  ONodeInitialize (This);
#ifdef DEBUG
  printf          ("debug : ONodeMake : %lu.\n",This);
#endif
  return          This;
}
/***************************************************************************/
ONode ONodeCreate (
 char* a_name
)
/***************************************************************************/
/*
 Description :
   Creates an ONode object. An ONode object is a container that 
  store a list of graphical primitives (lines, polygones,...). 
  A one parent hierarchy of ONode could be built. By default 
  an ONode has no parent. A node could be viewed by an OCamera 
  object with the routine OCameraViewNode.
 Arguments :
  a_name : name of the created object. 
 Example : 
...
#include <ONode.h>
...
  {ONode onode;
   onode = ONodeCreate("Cube");
...
  }
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode This;
/*.........................................................................*/
  This            = ONodeMake ();
  if(This==NULL)  return NULL;
  This->node.name      = CStringDuplicate(a_name);
  return          This;
}
/***************************************************************************/
ONode ONodeCreateF (
 int   a_number
,char* a_format
,...
)
/***************************************************************************/
/*
 Description :
   Creates an ONode object. Its name is given by a formatted string.
 Arguments :
  - a_number : evaluation of the size of memory in bytes that 
               could contain the built name. It is used
               to allocate memory for the built name.
  - a_format : format according the printf convention.
  - ... : varing list of arguments.
 Example : 
...
#include <ONode.h>
...
  {ONode onode;
   int   cube = 1000;
   onode = ONodeCreateF (64,"Cube/%d",cube);
...
  }
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list       args;
  char*         string;
  ONode       This;
/*.........................................................................*/
  va_start       (args,a_format);
  string         = CStringCreateV(a_number,a_format,args);
  va_end         (args);
  This           = ONodeMake ();
  if(This!=NULL) This->node.name = string;
  else           CStringDelete (string);
  return         This;
}
/***************************************************************************/
void ONodeDelete (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
#ifdef DEBUG
  printf            ("debug : ONodeDelete : %lu %s.\n",This,This->node.name);
#endif

 /*Warn views that This ONode is deleted.*/
 {void**      views;
  void**      viewa;
  views       = GetAllViews (This);
  for(viewa = views;(viewa!=NULL) && (*viewa!=NULL);viewa++) ONodeRequireViewRefreshment (*viewa);
  CListDelete ((void**)views);}

/*remove all references of this object in other objects:
  - parent
  - in OCamera existance of root object is tested.
*/
 {LkEntry entry;
  while ((entry = LkListFirstEntry (&(This->node.children)))!=NULL)  /*From LkListClear algorithm.*/
    {
      ONodeRemoveChild  (This,GetObjectFromChildrenEntry(entry));
    }}
  ONodeRemoveChild    (This->node.parent,This);

  LkEntryDisconnect   (&(This->node.links));
  ClearRecord           (This);
  CMemoryFreeBlock    (This);
}
/***************************************************************************/
static void ClearRecord (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)  return;

  if(This->node.deleteShapeFunction!=NULL) This->node.deleteShapeFunction(This);

  CListDestroy      ((CList)(This->node.prims),(CListVisitEntryFunction)OPrimitiveDelete);
  This->node.prims  = NULL;

  LkListInit        (&(This->node.children));
  LkEntryInit       (&(This->node.childrenLinks),NULL);
  This->node.parent = NULL;

  CStringDelete     (This->node.name);
  This->node.name   = NULL;

  OMatrixDelete     (This->node.matrix);
  This->node.matrix = NULL;

  CListDelete       (This->node.views);
  This->node.views  = NULL;

  This->node.toRefresh = 0;
  This->node.toRedraw  = 0;
}
/***************************************************************************/
int ONodeIsValid (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int         found;
/*.........................................................................*/
#ifdef DEBUG
  printf            ("debug : ONodeIsValid.\n");
#endif
  found       = 0;
 {LkIterator  iterator;
  LkEntry     entry;
  iterator    = LkIteratorNew (&(Class.extent));
  while ((entry=LkIteratorNext(iterator))!=NULL)
    { 
      if(GetObjectFromEntry(entry)==This)
	{
	  found = 1;
          break;
	}
    }
  LkIteratorDelete (iterator);}
  return           found;
}
/***************************************************************************/
char* ONodeGetName (
 ONode This
)
/***************************************************************************/
/*
 Description :
   Returns the pointer to the name of an ONode object.
 Arguments :
  This : ONode concerned.
 Example : 
...
#include <CPrinter.h>
#include <ONode.h>
...
 {ONode  onode;
...
  CInfoF ("name : %s\n",ONodeGetName(onode));}
...
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->node.name;
}
/***************************************************************************/
void ONodeSetName (
 ONode This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete  (This->node.name);
  This->node.name     = CStringDuplicate(a_name);
}
/***************************************************************************/
int ONodeGetHighlight (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return    This->node.highLight;
}
/***************************************************************************/
void ONodeSetHighlight (
 ONode This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->node.highLight==a_value) return;
  This->node.highLight = a_value;
  This->node.touched   = 1;
  RequireDrawing  (This);
}
/***************************************************************************/
void ONodeToggleHighlight (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->node.highLight = This->node.highLight==1 ? 0 : 1;
  This->node.touched   = 1;
  RequireDrawing       (This);
}
/***************************************************************************/
int ONodeGetTouched (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return    This->node.touched;
}
/***************************************************************************/
void ONodeResetTouched (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->node.touched = 0;
}
/***************************************************************************/
int ONodeGetProjected (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return    This->node.projected;
}
/***************************************************************************/
void ONodeSetProjected (
 ONode This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->node.projected==a_value) return;
  This->node.projected = a_value;
  RequireDrawing (This);
}
/***************************************************************************/
int ONodeGetCameraAligned (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return    This->node.cameraAligned;
}
/***************************************************************************/
void ONodeSetCameraAligned (
 ONode This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->node.cameraAligned==a_value) return;
  This->node.cameraAligned = a_value;
  RequireDrawing (This);
}
/***************************************************************************/
OMatrix ONodeGetMatrix (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->node.matrix;
}
/***************************************************************************/
void ONodeSetMatrix (
 ONode This 
,OMatrix a_matrix
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OMatrixDelete      (This->node.matrix);
  This->node.matrix       = a_matrix!=NULL ? OMatrixDuplicate(a_matrix) : (OMatrix)NULL;
  RequireRefreshment (This);
}
/***************************************************************************/
void ONodeAssignMatrixToIdentity (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OMatrixInitialize  (This->node.matrix,OMatrixIdentity);
  RequireRefreshment (This);
}
/***************************************************************************/
void ONodeMultiplyMatrix (
 ONode This 
,OMatrix a_matrix
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->node.matrix!=NULL)
    {
      OMatrix            matrix;
      matrix             = OMatrixMultiply  (a_matrix,This->node.matrix);
      OMatrixCopy        (This->node.matrix,matrix);
      OMatrixDelete      (matrix);
      RequireRefreshment (This);
    }
  else
    {
      ONodeSetMatrix   (This,a_matrix);
    }        
}
/***************************************************************************/
int ONodeGetPickability (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->node.pickable;
}
/***************************************************************************/
void ONodeSetPickability (
 ONode This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->node.pickable = a_value;
}
/***************************************************************************/
int ONodeGetVisibility (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return This->node.visible;
}
/***************************************************************************/
void ONodeSetVisibility (
 ONode This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->node.visible==a_value) return;
  This->node.visible = a_value;
  RequireRefreshment (This);
}
/***************************************************************************/
ONode ONodeGetParent (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->node.parent;
}
/***************************************************************************/
int ONodeGetPickIdentifier (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->node.pickId;
}
/***************************************************************************/
void ONodeSetPickIdentifier (
 ONode This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->node.pickId = a_value;
}
/***************************************************************************/
void ONodeAddChild (
 ONode This
,ONode a_child
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)            return;
  if(a_child==NULL)         return;
#ifdef DEBUG
  printf            ("debug : ONodeAddChild : %lu %s %lu %s.\n",This,This->node.name,a_child,a_child->node.name);
#endif
  if(This==a_child) 
    {
      CWarn ("ONodeAddChild : can't add a node to itself.\n");
      return;
    }
  if(a_child->node.parent!=NULL) 
    {
      CWarn ("ONodeAddChild : node has already a parent.\n");
      return;
    }
  LkEntryInit     (&(a_child->node.childrenLinks),&(This->node.children));
  a_child->node.parent = This;
}
/***************************************************************************/
void ONodeRemoveChild (
 ONode This
,ONode a_child
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)            return;
  if(a_child==NULL)         return;
#ifdef DEBUG
  printf            ("debug : ONodeRemoveChild : %lu %s %lu %s.\n",This,This->node.name,a_child,a_child->node.name);
#endif
  if(a_child->node.parent!=This) 
    {
      CWarn ("ONodeRemoveChild : parent mismatch.\n");
      return;
    }
  LkEntryDisconnect (&(a_child->node.childrenLinks));
  a_child->node.parent   = NULL;
}
/***************************************************************************/
void ONodeDestroyChildren (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
#ifdef DEBUG
  printf            ("debug : ONodeDestroyChildren : %s.\n",This->node.name);
#endif
 {LkEntry entry;
  while ((entry = LkListFirstEntry (&(This->node.children)))!=NULL)  /*From LkListClear algorithm.*/
    {
      ONodeDelete (GetObjectFromChildrenEntry(entry));
    }}
  LkListInit (&(This->node.children));
}
/***************************************************************************/
void ONodeAddPrimitive (
 ONode This
,OPrimitive a_prim
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_prim==NULL)  return;
  CListAddEntry     ((CList*)&(This->node.prims),a_prim);
#ifdef DEBUG
  printf                   ("debug : ONodeAddPrimitive.\n");
#endif
  RequireDrawing    (This);
}
/***************************************************************************/
void ONodeRemovePrimitives (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)             return;  
  if(This->node.prims==NULL) return;
  CListDestroy      ((CList)(This->node.prims),(CListVisitEntryFunction)OPrimitiveDelete);
  This->node.prims  = NULL;
  RequireDrawing    (This);
}
/***************************************************************************/
void ONodeRemovePrimitive (
 ONode This
,OPrimitive a_prim
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;  
  if(a_prim==NULL)  return;
  CListRemoveEntry  ((CList)This->node.prims,a_prim);
#ifdef DEBUG
  printf                   ("debug : ONodeRemovePrimitive.\n");
#endif
  RequireDrawing    (This);
}
/***************************************************************************/
void ONodeAddPointsToPrimitive (
 ONode      This
,OPrimitive a_prim
,int        a_pointn
,double*    a_xs
,double*    a_ys
,double*    a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)      return;  
  if(a_prim==NULL)    return;
  OPrimitiveAddPoints (a_prim,a_pointn,a_xs,a_ys,a_zs);
#ifdef DEBUG
  printf                   ("debug : ONodeAddPointsToPrimitive.\n");
#endif
  RequireDrawing      (This);
}
/***************************************************************************/
OPrimitive* ONodeGetPrimitives (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->node.prims;
}
/***************************************************************************/
ONode ONodeDuplicate (
 ONode a_obj   
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode This;
  OPrimitive*  itema;
/*.........................................................................*/
  if(a_obj==NULL) return NULL;
  This = a_name==NULL  ? ONodeCreate (a_obj->node.name) : ONodeCreate (a_name);
  if(This==NULL) return NULL;

  This->node.highLight              = a_obj->node.highLight;
  This->node.matrix                 = OMatrixDuplicate (a_obj->node.matrix);
  This->node.pickable               = a_obj->node.pickable;
  This->node.visible                = a_obj->node.visible;
  This->node.projected              = a_obj->node.projected;
  This->node.cameraAligned          = a_obj->node.cameraAligned;
  This->node.xHook                  = a_obj->node.xHook;
  This->node.yHook                  = a_obj->node.yHook;
  This->node.zHook                  = a_obj->node.zHook;
  This->node.isShape                = a_obj->node.isShape;
  This->node.representShapeFunction = a_obj->node.representShapeFunction;
  This->node.deleteShapeFunction    = a_obj->node.deleteShapeFunction;

  for(itema = This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++) 
    ONodeAddPrimitive(This,OPrimitiveDuplicate(*itema));

  return This;
}
/***************************************************************************/
void ONodeSetHookPoint (
 ONode  This
,double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( 
     (This->node.xHook==a_x) && 
     (This->node.yHook==a_y) &&
     (This->node.zHook==a_z) 
    ) return;
  This->node.xHook = a_x;
  This->node.yHook = a_y;
  This->node.zHook = a_z;
  RequireDrawing   (This);
}
/***************************************************************************/
void ONodeGetHookPoint (
 ONode  This
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL) return;
  if(a_x!=NULL) *a_x = This->node.xHook;
  if(a_y!=NULL) *a_y = This->node.yHook;
  if(a_z!=NULL) *a_z = This->node.zHook;
}
/***************************************************************************/
int ONodeSetAttribute (
 ONode  This
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

       if(strcmp(a_name,"identifier")==0)    {}
  else if(strcmp(a_name,"name")==0)          ONodeSetName                 (This,*((char**)a_addr));
  else if(strcmp(a_name,"highlight")==0)     ONodeSetHighlight            (This,*((int*)a_addr));
  else if(strcmp(a_name,"pickable")==0)      ONodeSetPickability          (This,*((int*)a_addr));
  else if(strcmp(a_name,"visible")==0)       ONodeSetVisibility           (This,*((int*)a_addr));
  else if(strcmp(a_name,"projected")==0)     ONodeSetProjected            (This,*((int*)a_addr));
  else if(strcmp(a_name,"cameraAligned")==0) ONodeSetCameraAligned        (This,*((int*)a_addr));
  else if(strcmp(a_name,"color")==0)         ONodeSetPrimitivesColorIndex (This,OColorGetIdentifier(*((char**)a_addr)));
  else if(strcmp(a_name,"colorIndex")==0)    ONodeSetPrimitivesColorIndex (This,*((int*)a_addr));
  else if(strcmp(a_name,"lineWidth")==0)     ONodeSetPrimitivesLineWidth  (This,*((int*)a_addr));
  else if(strcmp(a_name,"markerSize")==0)    ONodeSetPrimitivesMarkerSize (This,*((int*)a_addr));
  else if(strcmp(a_name,"lineStyle")==0)     ONodeSetPrimitivesLineStyle  (This,OLineStyleGetIdentifier  (*((char**)a_addr)));
  else if(strcmp(a_name,"markerStyle")==0)   ONodeSetPrimitivesMarkerStyle(This,OMarkerStyleGetIdentifier(*((char**)a_addr)));
  else if(strcmp(a_name,"areaStyle")==0)     ONodeSetPrimitivesAreaStyle  (This,OAreaStyleGetIdentifier  (*((char**)a_addr)));
  else if(strcmp(a_name,"primitives")==0) {}
  else 
    {
      CInfoF ("ONodeSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int ONodeGetAttribute (
 ONode  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number) *a_number = 0;
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0)    *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"name")==0)          *((char**)a_addr)  = ONodeGetName (This);
  else if(strcmp(a_name,"highlight")==0)     *((int*)a_addr)    = This->node.highLight;
  else if(strcmp(a_name,"pickable")==0)      *((int*)a_addr)    = This->node.pickable;
  else if(strcmp(a_name,"visible")==0)       *((int*)a_addr)    = This->node.visible;
  else if(strcmp(a_name,"projected")==0)     *((int*)a_addr)    = This->node.projected;
  else if(strcmp(a_name,"cameraAligned")==0) *((int*)a_addr)    = This->node.cameraAligned;
  else if(strcmp(a_name,"color")==0)
    {
      OPrimitive*         itema;
      *((OString*)a_addr) = NULL;
      for(itema = This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++) 
	{ 
	  static char         name[32]; 
	  sprintf             (name,"%d",OPrimitiveGetColorIndex(*itema));
	  *((OString*)a_addr) = name;
	  break;
	}
    }
  else if(strcmp(a_name,"colorIndex")==0)
    {
      OPrimitive*     itema;
      *((int*)a_addr) = 0;
      for(itema = This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++) 
	{ 
	  *((int*)a_addr) = OPrimitiveGetColorIndex(*itema);
	  break;
	}
    }
  else if(strcmp(a_name,"lineWidth")==0)   
    {
      OPrimitive* itema;
      *((int*)a_addr) = 0;
      for(itema = This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++) 
	{ 
	  *((int*)a_addr) = OPrimitiveGetLineWidth(*itema);
	  break;
	}
    }
  else if(strcmp(a_name,"markerSize")==0)   
    {
      OPrimitive* itema;
      *((int*)a_addr) = 0;
      for(itema = This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++) 
	{ 
	  *((int*)a_addr) = OPrimitiveGetMarkerSize(*itema);
	  break;
	}
    }
  else if(strcmp(a_name,"lineStyle")==0)   
    {
      OPrimitive*         itema;
      *((OString*)a_addr) = NULL;
      for(itema = This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++) 
	{ 
	  *((OString*)a_addr) = OLineStyleGetName(OPrimitiveGetLineStyle(*itema));
	  break;
	}
    }
  else if(strcmp(a_name,"markerStyle")==0) 
    {
      OPrimitive*         itema;
      *((OString*)a_addr) = NULL;
      for(itema = This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++) 
	{ 
	  *((OString*)a_addr) = OMarkerStyleGetName(OPrimitiveGetMarkerStyle(*itema));
	  break;
	}
    }
  else if(strcmp(a_name,"areaStyle")==0)   
    {
      OPrimitive*         itema;
      *((OString*)a_addr) = NULL;
      for(itema = This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++) 
	{ 
	  *((OString*)a_addr) = OAreaStyleGetName(OPrimitiveGetAreaStyle(*itema));
	  break;
	}
    }
  else if(strcmp(a_name,"primitives")==0)
    {
      int           count;
      OPrimitive*   itema;
      int           number;
      int           stringn = 0;
      OString*      strings = NULL;
      number        = 0;
      for(itema=This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++) number++;
      if(number!=0)
        {
	  strings = (OString*)CMemoryAllocateBlock (number * sizeof(OString));
          if(strings!=NULL)
            {
	      stringn = number;
	      count   = 0;
	      for(itema=This->node.prims;(itema!=NULL) && (*itema!=NULL);itema++)
		{
		  strings[count] = CStringCreateF (64,"%lu",*itema);
                  count++;
		}
	      if(a_number!=NULL) *a_number = stringn;
	      *((OString**)a_addr) = strings;
	    }
	  else
	    {
	      if(a_number!=NULL) *a_number = 0;
	      *((OString**)a_addr) = NULL;
	    }
	}
      else
        {
	  if(a_number!=NULL) *a_number = 0;
	  *((OString**)a_addr) = NULL;
	}
      return FREE_BLOCKS;
    }
  else 
    {
      CInfoF ("ONodeGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
ONode* ONodeGetLeavesForward (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
#ifdef DEBUG
  printf            ("debug : ONodeGetLeavesForward : %lu %s.\n",This,This->node.name);
#endif
  return (ONode*) CListCreateFromTree (This,(CListCountLeafFunction)CountNodes,(CListVisitLeafFunction)VisitNodesForward);
}
/***************************************************************************/
ONode* ONodeGetLeavesBackward (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
#ifdef DEBUG
  printf            ("debug : ONodeGetLeavesBackward.\n");
#endif
  return (ONode*) CListCreateFromTree (This,(CListCountLeafFunction)CountNodes,(CListVisitLeafFunction)VisitNodesBackward);
}
/***************************************************************************/
static void CountNodes (
 ONode This
,int* a_count
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;

  (*a_count)++;

 {LkIterator        iterator;
  LkEntry           entry;
  iterator          = LkIteratorNew (&(This->node.children));
  while ((entry=LkIteratorNext(iterator))!=NULL)
    { 
      CountNodes (GetObjectFromChildrenEntry(entry),a_count);
    }
  LkIteratorDelete (iterator);}

} 
/***************************************************************************/
static void VisitNodesForward (
 ONode This
,int* a_count
,ONode* a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;

  a_list[*a_count] = This;
  (*a_count)++;

 {LkIterator        iterator;
  LkEntry           entry;
  iterator          = LkIteratorNew (&(This->node.children));
  while ((entry=LkIteratorNext(iterator))!=NULL)
    { 
      VisitNodesForward (GetObjectFromChildrenEntry(entry),a_count,a_list);
    }
  LkIteratorDelete (iterator);}

} 
/***************************************************************************/
static void VisitNodesBackward (
 ONode This
,int* a_count
,ONode* a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;

 {LkIterator        iterator;
  LkEntry           entry;
  iterator          = LkIteratorNew (&(This->node.children));
  while ((entry=LkIteratorNext(iterator))!=NULL)
    { 
      VisitNodesBackward (GetObjectFromChildrenEntry(entry),a_count,a_list);
    }
  LkIteratorDelete (iterator);}

  a_list[*a_count] = This;
  (*a_count)++;
} 
/***************************************************************************/
int ONodeGetBlink (
 ONode This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int back;
/*.........................................................................*/
  if(This==NULL) return 0;
  back       = 0;
  if (This->node.blinkType!=GBlinkOff)
    {
      if (This->node.blinkType==GBlinkLow)
	{
	  This->node.blinkType = GBlinkHigh;
	  back            = 1;
	}
      else if (This->node.blinkType==GBlinkHigh)
	{
	  This->node.blinkType = GBlinkLow;
	  back            = 0;
	}
    }
  return back;
}
/***************************************************************************/
ONode* ONodeCollectV (
 ONode* a_list
,int a_what        
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  ONode* list;
  ONode* itema;
  char*  string;
/*.........................................................................*/
  if(a_list==NULL) return NULL;
  list  = (ONode*)CListDuplicate((CList)a_list);
  if(list==NULL)   return NULL;
  count = 0;
  switch(a_what)
    {
      case OCollectAll:
               for(itema=a_list;(itema!=NULL) && (*itema!=NULL);itema++)
		 {
		   list[count] = *itema;count++;
		 }
               break;
      case OCollectHighlighted:
               for(itema=a_list;(itema!=NULL) && (*itema!=NULL);itema++)
		 {
		   if((*itema)->node.highLight==1) {list[count] = (*itema);count++;}
		 }
               break;
      case OCollectNoHighlighted:
               for(itema=a_list;(itema!=NULL) && (*itema!=NULL);itema++)
		 {
		   if((*itema)->node.highLight==0) {list[count] = (*itema);count++;}
		 }
               break;
      case OCollectMatchingName:
               string   = va_arg(a_args,char*);
               for(itema=a_list;(itema!=NULL) && (*itema!=NULL);itema++)
		 {
		   if(CStringMatchPattern((*itema)->node.name,string)==1) {list[count] = (*itema);count++;}
		 }
               break;
      case OCollectHighlightedAndMatchingName:
               string   = va_arg(a_args,char*);
               for(itema=a_list;(itema!=NULL) && (*itema!=NULL);itema++)
		 {
		   if(((*itema)->node.highLight==1) && 
		      (CStringMatchPattern((*itema)->node.name,string)==1) ) 
		     {list[count] = (*itema);count++;}
		 }
               break;
      case OCollectFirstNodeMatchingColorIndex:
	      {int         color;
               OPrimitive* prima;
               color       = va_arg(a_args,int);
               for(itema=a_list;(itema!=NULL) && (*itema!=NULL);itema++)
		 {
		   if((*itema)->node.prims!=NULL)
		     {
		       for(prima=(*itema)->node.prims;*prima!=NULL;prima++)
			 {
			   if(OPrimitiveGetColorIndex(*prima)==color)
			     {
			       list[count] = (*itema);count++;
			       break;
			     }
			 }
		     }
		 }
	       }
               break;
      default: break;
    }
  list[count] = NULL;
  return    list;
}
/***************************************************************************/
ONode* ONodeCollect (
 ONode* a_list
,int a_what        
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  ONode*   list;
/*.........................................................................*/
  if(a_list==NULL) return NULL;
  va_start (args,a_what);
  list     = ONodeCollectV (a_list,a_what,args);
  va_end   (args);
  return   list;
}
/***************************************************************************/
void ONodeSetPrimitivesColorIndex (
 ONode This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive* prima;
/*.........................................................................*/
  if(This==NULL) return;  
  if(This->node.prims==NULL) return;
  for(prima=This->node.prims;*prima!=NULL;prima++)
    {
      OPrimitiveSetColorIndex(*prima,a_value);
    }
  if(This->node.prims!=NULL) RequireDrawing (This);
}
/***************************************************************************/
void ONodeSetPrimitivesLineStyle (
 ONode This
,OLineStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive* prima;
/*.........................................................................*/
  if(This==NULL) return;  
  if(This->node.prims==NULL) return;
  for(prima=This->node.prims;*prima!=NULL;prima++)
    {
      OPrimitiveSetLineStyle(*prima,a_value);
    }
  if(This->node.prims!=NULL) RequireDrawing (This);
}
/***************************************************************************/
void ONodeSetPrimitivesMarkerStyle (
 ONode This
,OMarkerStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive* prima;
/*.........................................................................*/
  if(This==NULL) return;  
  if(This->node.prims==NULL) return;
  for(prima=This->node.prims;*prima!=NULL;prima++)
    {
      OPrimitiveSetMarkerStyle(*prima,a_value);
    }
  if(This->node.prims!=NULL) RequireDrawing (This);
}
/***************************************************************************/
void ONodeSetPrimitivesAreaStyle (
 ONode This
,OAreaStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive* prima;
/*.........................................................................*/
  if(This==NULL) return;  
  if(This->node.prims==NULL) return;
  for(prima=This->node.prims;*prima!=NULL;prima++)
    {
      OPrimitiveSetAreaStyle(*prima,a_value);
    }
  if(This->node.prims!=NULL) RequireDrawing (This);
}
/***************************************************************************/
void ONodeSetPrimitivesLineWidth (
 ONode This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive* prima;
/*.........................................................................*/
  if(This==NULL) return;  
  if(This->node.prims==NULL) return;
  for(prima=This->node.prims;*prima!=NULL;prima++)
    {
      OPrimitiveSetLineWidth(*prima,a_value);
    }
  if(This->node.prims!=NULL) RequireDrawing (This);
}
/***************************************************************************/
void ONodeSetPrimitivesMarkerSize (
 ONode This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive* prima;
/*.........................................................................*/
  if(This==NULL) return;  
  if(This->node.prims==NULL) return;
  for(prima=This->node.prims;*prima!=NULL;prima++)
    {
      OPrimitiveSetMarkerSize(*prima,a_value);
    }
  if(This->node.prims!=NULL) RequireDrawing (This);
}
/***************************************************************************/
int ONodeGetSize (
 ONode This
,double* a_xmin   
,double* a_xmax   
,double* a_ymin   
,double* a_ymax   
,double* a_zmin   
,double* a_zmax   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive* prims;
  OPrimitive* itema;
  int         first;
/*.........................................................................*/
  if(This==NULL)   return 0;
  if(a_xmin==NULL) return 0;
  if(a_xmax==NULL) return 0;
  if(a_ymin==NULL) return 0;
  if(a_ymax==NULL) return 0;
  if(a_zmin==NULL) return 0;
  if(a_zmax==NULL) return 0;
/* get wc size */
  prims = ONodeIsShape(This)==1 ? ONodeGetShapePrimitives(This): ONodeGetPrimitives(This);
  first = 0;
  for(itema = prims;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      double xmin,xmax,ymin,ymax,zmin,zmax;
      if(OPrimitiveGetSize (*itema,&xmin,&xmax,&ymin,&ymax,&zmin,&zmax)==1)
	{
	  if(first==0) 
	    {
	      *a_xmin = xmin;
	      *a_ymin = ymin;
	      *a_zmin = zmin;

	      *a_xmax = xmax;
	      *a_ymax = ymax;
	      *a_zmax = zmax;
	    }
	  else
            {
	      *a_xmax = MAXIMUM(*a_xmax,xmax);
	      *a_ymax = MAXIMUM(*a_ymax,ymax);         
	      *a_zmax = MAXIMUM(*a_zmax,zmax);
	  
	      *a_xmin = MINIMUM(*a_xmin,xmin);
	      *a_ymin = MINIMUM(*a_ymin,ymin);
	      *a_zmin = MINIMUM(*a_zmin,zmin);
	    }
	  first = 1;
	}
    }
  if(ONodeIsShape(This)==1) 
    CListDestroy ((CList)prims,(CListVisitEntryFunction)OPrimitiveDelete);
  return first; /* no point */
}
/***************************************************************************/
int ONodeGetSumOfPoints (
 ONode This
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPrimitive* prims;
  OPrimitive* itema;
  int         count;
/*.........................................................................*/
  if(This==NULL) return 0;
  if(a_x==NULL)  return 0;
  if(a_y==NULL)  return 0;
  if(a_z==NULL)  return 0;
  *a_x  = 0.;
  *a_y  = 0.;
  *a_z  = 0.;
  count = 0;
  prims = ONodeIsShape(This)==1 ? ONodeGetShapePrimitives(This): ONodeGetPrimitives(This);
  for(itema = prims;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      count += OPrimitiveGetSumOfPoints (*itema,a_x,a_y,a_z);
    }
  if(ONodeIsShape(This)==1) 
    CListDestroy ((CList)prims,(CListVisitEntryFunction)OPrimitiveDelete);
  return count;
}
/***************************************************************************/
void ONodeSetBlink (
 ONode This
,int a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
#ifdef DEBUG
  printf                   ("debug : ONodeSetBlink.\n");
#endif
  if(a_value!=0)
    {
      if(This->node.blinkType == GBlinkOff)
	{
	  This->node.blinkType  = GBlinkLow;
	  Class.blinkNumber++;
	}
      RequireDrawing     (This);
      if(Class.startBlink!=NULL) Class.startBlink();
    }
  else
    {
      if(This->node.blinkType != GBlinkOff)
	{
	  This->node.blinkType = GBlinkOff;
	  Class.blinkNumber--;
	}
      RequireDrawing    (This);
      if(Class.stopBlink!=NULL) Class.stopBlink();
    }
}
/***************************************************************************/
static void RequireRefreshment (
 ONode This
)
/***************************************************************************/
/*
  Refreshment = clear window + Drawing.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)            return;
#ifdef DEBUG
  printf                   ("debug : RequireRefreshment\n");
#endif
  This->node.toRefresh           = 1;
  Class.somethingToRefresh  = 1;
  if(Class.sendProc!=NULL) Class.sendProc ();
}
/***************************************************************************/
static void RequireDrawing (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)            return;
#ifdef DEBUG
  printf                   ("debug : RequireDrawing\n");
#endif
  This->node.toRedraw            = 1;
  Class.somethingToRedraw   = 1;
  if(Class.sendProc!=NULL) Class.sendProc ();
}
/***************************************************************************/
void ONodeAddView (
 ONode This
,void* a_view
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CListGetEntryIndex(This->node.views,a_view,NULL)!=NotFound) return;
  CListAddEntry (&(This->node.views),a_view);
}
/***************************************************************************/
void ONodeRemoveView (
 ONode This
,void* a_view
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CListGetEntryIndex(This->node.views,a_view,NULL)==NotFound) return;
  CListRemoveEntry (This->node.views,a_view);
}
/***************************************************************************/
CList ONodeGetViews (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->node.views;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void ONodeRequireViewRefreshment (
 void* a_view
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_view==NULL)         return;
  if(CListGetEntryIndex(Class.viewsToRefresh,a_view,NULL)!=NotFound) return;
  CListAddEntry            (&(Class.viewsToRefresh),a_view);
#ifdef DEBUG
  printf                   ("debug : ONodeRequireViewRefreshment\n");
#endif
  if(Class.sendProc!=NULL) Class.sendProc ();
}
/***************************************************************************/
void ONodeRequireViewDrawing (
 void* a_view
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_view==NULL)            return;
  if(CListGetEntryIndex(Class.viewsToRedraw,a_view,NULL)!=NotFound) return;
  CListAddEntry            (&(Class.viewsToRedraw),a_view);
#ifdef DEBUG
  printf                   ("debug : ONodeRequireViewDrawing\n");
#endif
  if(Class.sendProc!=NULL) Class.sendProc ();
}
/***************************************************************************/
void ONodeSetViewFunctions (
 ONodeVoidFunction    a_sendProc
,ONodeIsValidFunction a_existProc
,ONodeRefreshFunction a_refreshProc
,ONodeRefreshFunction a_redrawProc
)
/***************************************************************************/
/* Called in XoCamera. */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.sendProc    = a_sendProc;
  Class.existProc   = a_existProc;
  Class.refreshProc = a_refreshProc;
  Class.redrawProc  = a_redrawProc;
}
/***************************************************************************/
void ONodeRefreshViews (
)
/***************************************************************************/
/*
   warns views that something has changed and that a refresh is necessary.
   Refresh = erase drawing area + redraw.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList   views,itema;
/*.........................................................................*/
  if(Class.existProc==NULL)   return;
  if(Class.refreshProc==NULL) return;
  if(Class.redrawProc==NULL)  return;

  /*  printf              ("debug: ONodeRefreshViews\n");*/

/*refresh*/
  views = NULL;
  if(Class.somethingToRefresh==1)
    {
      LkIterator        iterator;
      LkEntry           entry;
      iterator          = LkIteratorNew (&(Class.extent));
      while ((entry=LkIteratorNext(iterator))!=NULL)
	{ 
	  ONode            node;
	  node             = GetObjectFromEntry(entry);
	  if(node->node.toRefresh==1)
	    {
	      CList            list;
	      list             = GetAllViews (node);
	      for(itema = list;(itema!=NULL) && (*itema!=NULL);itema++)
		{
		  if(CListGetEntryIndex((void**)views,*itema,NULL)==NotFound) 
		    {
		      CListAddEntry ((void***)&views,*itema);
		    }
		}
	      CListDelete      (list);
	      node->node.toRefresh  = 0;
	    }
	}
      LkIteratorDelete (iterator);
    }
  for(itema = Class.viewsToRefresh;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(CListGetEntryIndex((void**)views,*itema,NULL)==NotFound) 
	{
	  CListAddEntry ((void***)&views,*itema);
	}
    }
  RefreshViews           (views,0);
  CListDelete            (views);

/*redraw*/
  views             = NULL;
  if(Class.somethingToRedraw==1)
    {
      LkIterator        iterator;
      LkEntry           entry;
      iterator          = LkIteratorNew (&(Class.extent));
      while ((entry=LkIteratorNext(iterator))!=NULL)
	{ 
	  ONode            node;
	  node             = GetObjectFromEntry(entry);
	  if(node->node.toRedraw==1)
	    {
	      CList           list;
	      list             = GetAllViews (GetObjectFromEntry(entry));
	      for(itema = list;(itema!=NULL) && (*itema!=NULL);itema++)
		{
		  if(CListGetEntryIndex((void**)views,*itema,NULL)==NotFound) 
		    {
		      CListAddEntry ((void***)&views,*itema);
		    }
		}
	      CListDelete      (list);
	      node->node.toRedraw   = 0;
	    }
	}
      LkIteratorDelete (iterator);
    }
  for(itema = Class.viewsToRedraw;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(CListGetEntryIndex((void**)views,*itema,NULL)==NotFound) 
	{
	  CListAddEntry ((void***)&views,*itema);
	}
    }
  RefreshViews           (views,1);
  CListDelete            (views);

  CListDelete              (Class.viewsToRefresh);
  CListDelete              (Class.viewsToRedraw);
  Class.viewsToRefresh     = NULL;
  Class.viewsToRedraw      = NULL;
  Class.somethingToRefresh = 0;
  Class.somethingToRedraw  = 0;
}
/***************************************************************************/
static void RefreshViews (
 CList a_views
,int a_redraw
)
/***************************************************************************/
/*
   Warns views that something has changed and that a refresh is necessary.
   Refresh = erase drawing area + redraw.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList itema;
/*.........................................................................*/
  if(Class.existProc==NULL)   return;
  if(Class.refreshProc==NULL) return;
  if(Class.redrawProc==NULL)  return;
  
  for(itema = a_views;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(Class.existProc(*itema)==1)
	{
	  if(a_redraw==1) Class.redrawProc  (*itema);
	  else            Class.refreshProc (*itema);
	}
    } 
}
/***************************************************************************/
static CList GetAllViews (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList  views = NULL;
  ONode parent;
/*.........................................................................*/
  if(This==NULL) return NULL;
  /*if(!GObjectExist(This)) return NULL;*/ /*if job is well done in ONodeDelete do not need to check existance of object */
  parent = This;
  while(parent!=NULL)
    {
/*    if(ONodeIsValid(parent)==0) return views;*/ /*Do not need this if parent/children relation well maintained in ONode.*/
      CListConcatenate (&views,parent->node.views);
      parent = parent->node.parent;
    }
  return views;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void ONodeAddLine (
 ONode a_obj 
,OContext a_context
,double a_beg_x 
,double a_beg_y 
,double a_beg_z 
,double a_end_x 
,double a_end_y 
,double a_end_z 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double xs[2];
  double ys[2];
  double zs[2];
/*.........................................................................*/
  xs[0] = a_beg_x;
  ys[0] = a_beg_y;
  zs[0] = a_beg_z;
  xs[1] = a_end_x;
  ys[1] = a_end_y;
  zs[1] = a_end_z;
  ONodeAddPrimitive (a_obj,OPrimitiveCreate(OPrimitiveLines,a_context,2,xs,ys,zs));
}
/***************************************************************************/
void ONodeAddMarker (
 ONode a_obj 
,OContext a_context
,double a_x 
,double a_y 
,double a_z 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive (a_obj,OPrimitiveCreate(OPrimitiveMarkers,a_context,1,&a_x,&a_y,&a_z));
}
/***************************************************************************/
void ONodeAddPolygon (
 ONode a_obj 
,OContext a_context
,int a_pointn
,double* a_xs
,double* a_ys
,double* a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive (a_obj,OPrimitiveCreate(OPrimitivePolygon,a_context,a_pointn,a_xs,a_ys,a_zs));
}
/***************************************************************************/
void ONodeAddLines (
 ONode a_obj 
,OContext a_context
,int a_pointn
,double* a_xs
,double* a_ys
,double* a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive (a_obj,OPrimitiveCreate (OPrimitiveLines,a_context,a_pointn,a_xs,a_ys,a_zs));
}
/***************************************************************************/
void ONodeAddMarkers (
 ONode a_obj 
,OContext a_context
,int a_pointn
,double* a_xs
,double* a_ys
,double* a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive  (a_obj,OPrimitiveCreate(OPrimitiveMarkers,a_context,a_pointn,a_xs,a_ys,a_zs));
}
/***************************************************************************/
void ONodeAddSegments (
 ONode a_obj 
,OContext a_context
,int a_pointn
,double* a_xs
,double* a_ys
,double* a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  ONodeAddPrimitive  (a_obj,
    OPrimitiveCreate (OPrimitiveSegments,a_context,a_pointn,a_xs,a_ys,a_zs));
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int ONodeIsShape (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->node.isShape;
}
/***************************************************************************/
OPrimitive* ONodeGetShapePrimitives (
 ONode This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                              return NULL;
  if(This->node.isShape==0)                   return NULL; 
  if(This->node.representShapeFunction==NULL) return NULL;
  return This->node.representShapeFunction(This);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ClearEntry (
 LkEntry This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)  return;
  ClearRecord (GetObjectFromEntry(This));
}
/***************************************************************************/
static ONode GetObjectFromEntry (
 LkEntry This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  y0 = NULL;
  char*  x0;
  int    offset; 
/*.........................................................................*/
  x0     = (char*) & ((ONode)y0)->node.links;
  offset = x0-y0;
  return (ONode) ((char*) This - offset );
}
/***************************************************************************/
static ONode GetObjectFromChildrenEntry (
 LkEntry This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  y0 = NULL;
  char*  x0;
  int    offset; 
/*.........................................................................*/
  x0     = (char*) & ((ONode)y0)->node.childrenLinks;
  offset = x0-y0;
  return (ONode) ((char*) This - offset );
}
#include <CLkStat.ic>
