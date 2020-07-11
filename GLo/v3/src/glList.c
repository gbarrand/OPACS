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
#include <stdarg.h>
#include <string.h>

#include <CPrinter.h>
#include <CMemory.h>
#include <CList.h>
#include <CError.h>

#include <glListP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void FreeRecord          (glList);
static int  GetIdentifier       (glList,va_list);
static void FreeOperationRecord (glOperation);
static void ReportError         (char*,int);
#ifdef __cplusplus
}
#endif

#define NotFound                (-1)

static struct 
{
  glList* extent;
} Class = {NULL};
/***************************************************************************/
void glListClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy         ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent        = NULL;
}
/***************************************************************************/
glList* glListGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
glList glListGetIdentifier (
 GLuint a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (glList)CListSearchEntry ((CList)Class.extent,(CListSearchEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 glList This
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  GLuint name;
/*.........................................................................*/
  name   = va_arg(a_args,GLuint);
  return glListGetName(This) == name ? 1 : 0;
}
/***************************************************************************/
glList glListMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  glList This;
/*.........................................................................*/
  This                     = (glList) CMemoryAllocateBlock (sizeof(glListRecord));
  if(This==NULL) return NULL;

  CListAddEntry             ((CList*)&(Class.extent),This);

  This->name               = 0;
  This->operations         = NULL;

  return                   This;
}
/***************************************************************************/
glList glListCreate (
 GLuint a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  glList This;
/*.........................................................................*/
  This           = glListMake();
  if(This==NULL) return NULL;

  This->name     = a_name;

  return         This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int glListIsValid (
 glList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
void glListDelete (
 glList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  CListRemoveEntry ((CList)Class.extent,This);
  FreeRecord       (This);
}
/***************************************************************************/
static void FreeRecord (
 glList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  This->name       = 0;
  CListDestroy     ((CList)This->operations,(CListVisitEntryFunction)FreeOperationRecord);
  This->operations = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
GLuint glListGetName (
 glList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->name;
}
/***************************************************************************/
void glListAddOperation (
 glList This 
,int a_code
,glListOperationFunction a_routine
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list     args;
  glOperation op;
/*.........................................................................*/
  if(This==NULL) return;
  op            = (glOperation) CMemoryAllocateBlock (sizeof(glOperationRecord));
  if(op==NULL)  return;
  op->code      = a_code;
  op->routine   = a_routine;
  op->pointer1  = NULL;
  if(a_code==glOperation_glBegin)
    {
      va_start           (args,a_routine);
      op->mode           = va_arg(args,GLenum);
      va_end             (args);
    }
  else if(a_code==glOperation_glEnd)
    {
    }
  else if(a_code==glOperation_glVertex3d)
    {
      va_start           (args,a_routine);
      op->double1        = va_arg(args,GLdouble);
      op->double2        = va_arg(args,GLdouble);
      op->double3        = va_arg(args,GLdouble);
      va_end             (args);
    }
  else if(a_code==glOperation_glColor3d)
    {
      va_start           (args,a_routine);
      op->double1        = va_arg(args,GLdouble);
      op->double2        = va_arg(args,GLdouble);
      op->double3        = va_arg(args,GLdouble);
      va_end             (args);
    }
  else if(a_code==glOperation_glLineWidth)
    {
      va_start           (args,a_routine);
      op->double1        = va_arg(args,GLdouble);
      va_end             (args);
    }
  else if(a_code==glOperation_glBitmap)
    {
      GLubyte*           bitmap;
      int                size;
      va_start           (args,a_routine);
      op->int1           = va_arg(args,GLsizei);
      op->int2           = va_arg(args,GLsizei);
      op->float1         = va_arg(args,GLfloat);
      op->float2         = va_arg(args,GLfloat);
      op->float3         = va_arg(args,GLfloat);
      op->float4         = va_arg(args,GLfloat);
      bitmap             = va_arg(args,GLubyte*);
      va_end             (args);
      size               = op->int1 * op->int2; /*Too much.*/
      if( (bitmap!=NULL) && (size>0) )
	{
	  op->pointer1   = (GLubyte*)CMemoryAllocateBlock ( size * sizeof(GLubyte));
	  if(op->pointer1!=NULL)
	    memcpy         ((void*)op->pointer1,(void*)bitmap,(size_t)size);
	}
    }
  else
    {
      FreeOperationRecord (op);
      return;
    }
  CListAddEntry ((CList*)&(This->operations),op);  /*Valid operation : add it to the list.*/
}
/***************************************************************************/
void glListExecute (
 glList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  glOperation* itema;
/*.........................................................................*/
  if(This==NULL) return;
  for(itema = This->operations;(itema!=NULL) && (*itema!=NULL);itema++) 
    {
      glOperation op;
      op          = *itema;
#ifdef DEBUG
      printf      ("debug : glListExecute : %d \n",op->code);
#endif
      if(op->code==glOperation_glBegin)
	{
          ((void(*)(GLenum))op->routine)(op->mode);
	}
      else if(op->code==glOperation_glEnd)
	{
          ((void(*)())op->routine)();
	}
      else if(op->code==glOperation_glVertex3d)
	{
          ((void(*)(GLdouble,GLdouble,GLdouble))op->routine)(op->double1,op->double2,op->double3);
	}
      else if(op->code==glOperation_glColor3d)
	{
          ((void(*)(GLdouble,GLdouble,GLdouble))op->routine)(op->double1,op->double2,op->double3);
	}
      else if(op->code==glOperation_glLineWidth)
	{
          ((void(*)(GLfloat))op->routine)((GLfloat)op->double1);
	}
      else if(op->code==glOperation_glBitmap)
	{
          ((void(*)(GLsizei,GLsizei,GLfloat,GLfloat,GLfloat,GLfloat,const GLubyte*))op->routine)
          ((GLsizei)op->int1,(GLsizei)op->int2,
	   (GLfloat)op->float1,(GLfloat)op->float2,(GLfloat)op->float3,(GLfloat)op->float4,
           (GLubyte*)op->pointer1);
	}
    }
}
/***************************************************************************/
static void FreeOperationRecord (
 glOperation This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  This->code       = 0;
  This->routine    = NULL;
  CMemoryFreeBlock (This->pointer1);
  This->pointer1   = NULL;
  CMemoryFreeBlock (This);
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
  CErrorHandle ("GLo","glList",a_name,a_code);
}



