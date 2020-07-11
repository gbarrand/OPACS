/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <CError.h>
#include <CMemory.h>
#include <CString.h>

#include <OHandle.h>

typedef struct _OHandleRecord
{
  void* type;
  void* ident;
} OHandleRecord;
 
#ifdef __cplusplus
extern "C"{
#endif
static void  FreeRecord  (OHandle);
static void  ReportError (char*,int);
#ifdef __cplusplus
}
#endif

/***************************************************************************/
OHandle OHandleCreate (
 void* a_type
,void* a_ident
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHandle           This;
/*.........................................................................*/
  This           = (OHandle)CMemoryAllocateBlock (sizeof(OHandleRecord));
  if(This==NULL) return NULL;
  This->type     = a_type;
  This->ident    = a_ident;
  return         This;
}
/***************************************************************************/
void OHandleDelete (
  OHandle This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  FreeRecord       (This);
}
/***************************************************************************/
static void FreeRecord (
 OHandle This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This->type==NULL) CStringDelete ((char*)This->ident);
  This->type   = NULL;
  This->ident  = NULL;
  CMemoryFreeBlock     (This);
}
/***************************************************************************/
void* OHandleGetType (
 OHandle This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->type;
}
/***************************************************************************/
void* OHandleGetIdentifier (
 OHandle This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->ident;
}
/***************************************************************************/
void OHandleInitialize (
 OHandle This
,void* a_type
,void* a_ident
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->type  = a_type;
  This->ident = a_ident;
}
/***************************************************************************/
void OHandleClear (
 OHandle This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->type  = NULL;
  This->ident = NULL;
}
