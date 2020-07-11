/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <CMemory.h>
#include <CString.h>
#include <CList.h>

#include <XWidget.h>

#include <OPartP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  FreeRecord              (OPart);
static int   GetIdentifierFromQuark  (OPart,XrmQuark);
static int   GetIdentifierFromWidget (OPart,void*);
#ifdef __cplusplus
}
#endif

static struct 
{
  OPart*  extent;
} Class = {NULL};
/***************************************************************************/
void OPartClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy         ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent        = NULL;
}
/***************************************************************************/
OPart OPartCreate (
 void* a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPart This;
/*.........................................................................*/
  This                        = (OPart)CMemoryAllocateBlock (sizeof(OPartRecord));
  if(This==NULL)                  return NULL;
  CListAddEntry                ((CList*)&(Class.extent),This);

  This->widget                = a_widget;
  This->wui_parent            = NULL;
  This->oui                   = NULL;
  This->type                  = NULL;
  This->volatil               = False;
  This->volatilChild          = False;
  This->createCallback        = NULL;
  This->targetChangedCallback = NULL;
  This->eventHandler          = NULL;
  This->eventHandlerData      = NULL;
  This->nbyte                 = 0;
  This->resources_changed     = NULL;
  This->duplicate             = NULL;

  return This;
}
/***************************************************************************/
void OPartDelete (
 OPart This
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
 OPart This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->widget                = NULL;
  This->wui_parent            = NULL;
  This->oui                   = NULL;
  This->volatil               = False;
  This->volatilChild          = False;
  This->eventHandlerData      = NULL;
  This->duplicate             = NULL;
  CStringDelete               (This->type);
  CStringDelete               (This->createCallback);
  CStringDelete               (This->targetChangedCallback);
  CStringDelete               (This->eventHandler);
  CMemoryFreeBlock                    (This->resources_changed);
  This->type                  = NULL;
  This->createCallback        = NULL;
  This->targetChangedCallback = NULL;
  This->eventHandler          = NULL;
  This->eventHandlerData      = NULL;
  This->resources_changed     = NULL;
  This->nbyte                 = 0;
  CMemoryFreeBlock                    (This);
}
/***************************************************************************/
OPart OPartGetIdentifier (
  char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XrmQuark    quark;
/*.........................................................................*/
  if(a_name==NULL) return NULL;
  quark    = XrmStringToQuark(a_name); 
  if(quark==NULLQUARK) return NULL;
  return (OPart)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifierFromQuark,(void*)quark);
}
/***************************************************************************/
static int GetIdentifierFromQuark (
 OPart This
,XrmQuark a_quark
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (XWidgetGetQuarkifiedName((Widget)(This->widget))==a_quark ? 1 : 0);
}
/***************************************************************************/
OPart OPartGetIdentifierFromWidget (
 void* a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OPart)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifierFromWidget,a_widget);
}
/***************************************************************************/
static int GetIdentifierFromWidget (
 OPart This
,void* a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (This->widget==a_widget ? 1 : 0);
}
/***************************************************************************/
void* OPartGetWidget (
 OPart This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->widget;
}
/***************************************************************************/
void OPartMarkIthResource (
 OPart This 
,int a_count   
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           ibyte;
  unsigned char bmsk;
/*.........................................................................*/
  if(This==NULL) return;
  if(This->resources_changed==NULL)
    { int                              nbyte,count;
      nbyte                            = XWidgetGetNumberOfResources ((Widget)This->widget);
      nbyte                            = nbyte/8;
      nbyte                           ++;
      This->resources_changed      = (unsigned char *)CMemoryAllocateBlock(nbyte*sizeof(char));
      if(This->resources_changed==NULL) return;
      for(count=0;count<nbyte;count++) This->resources_changed[count] = 0;
      This->nbyte                  = nbyte;
    }
  ibyte     = a_count/8;
  if((ibyte<0)||(ibyte>= This->nbyte )) return;
  if(a_value==0) 
    bmsk      = 0<<(a_count - ibyte * 8);
  else
    bmsk      = 1<<(a_count - ibyte * 8);
  This->resources_changed[ibyte] |= bmsk;
}
/***************************************************************************/
void OPartInitialize (
 OPart This 
,void*  a_wui_parent
,void*  a_oui
,char*  a_type
,int    a_volatil
,int    a_volatilChild
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                   return;
  This->wui_parent            = a_wui_parent;
  This->oui                   = a_oui;
  CStringDelete               (This->type);
  This->type                  = CStringDuplicate(a_type);
  This->volatil               = a_volatil;
  This->volatilChild          = a_volatilChild;
}
