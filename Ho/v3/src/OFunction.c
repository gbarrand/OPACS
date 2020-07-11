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
#include <stdio.h>

#include <CMemory.h>
#include <CString.h>
#include <CPrinter.h>
#include <CList.h>
#include <CError.h>

#include <OStatement.h>

#include <OFunctionP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  InitializeClass  ();
static void  FreeRecord       (OFunction);
static int   GetIdentifier    (OFunction,char*);
static void  ReportError      (char*,int);
#ifdef __cplusplus
}
#endif

typedef double  (*OCFundouble1)(double);
typedef double  (*OCFundouble2)(double,double);
typedef double  (*OCFundouble3)(double,double,double);
typedef double  (*OCFundouble4)(double,double,double,double);

#define XtNumber(arr)  (sizeof(arr) / sizeof(arr[0]))
#define NotFound       (-1)
#define DVOID__POS      -1.
#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)

static struct 
{
  OFunction* extent;
} Class = {NULL};
/***************************************************************************/
void OFunctionClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OStatementClearClass ();
  CListDestroy     ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent    = NULL;
}
/***************************************************************************/
OFunction OFunctionGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OFunction)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OFunction  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OFunctionGetName(This),a_name);
}
/***************************************************************************/
OFunction OFunctionMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OFunction This;
/*.........................................................................*/
  This               = (OFunction)CMemoryAllocateBlock (sizeof(OFunctionRecord));
  if(This==NULL)     return NULL;
  CListAddEntry      ((CList*)&(Class.extent),This);
  This->name         = NULL;
  This->dimension    = 1;
  This->script       = NULL;
  This->proc         = NULL;
  This->ostmt        = NULL;
  for(count=0;count<OAxisMaximumNumber;count++) This->axis[count] = OAxisMake();
  This->axisOfValues  = OAxisMake ();
  OAxisSetAttributesF (This->axisOfValues,"a",1);

  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      This->variable[count] = NULL;
    }    
  OFunctionSetVariable   (This,OAxisX,"x");
  OFunctionSetVariable   (This,OAxisY,"y");

  return             This;
}
/***************************************************************************/
OFunction OFunctionCreate (
 char*  a_name   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OFunction This;
/*.........................................................................*/
  This               = OFunctionMake();
  if(This==NULL)     return NULL;
  This->name         = CStringDuplicate(a_name);
  return             This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OFunctionIsValid (
 OFunction This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
char* OFunctionGetName (
 OFunction This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OFunctionDelete (
  OFunction This 
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
 OFunction This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  CStringDelete     (This->name);
  This->name        = NULL;
  OStatementDelete  (This->ostmt);
  This->ostmt       = NULL;
  CStringDelete     (This->script);
  This->script      = NULL;
  for(count=0;count<OAxisMaximumNumber;count++) OAxisDelete (This->axis[count]);
  OAxisDelete        (This->axisOfValues);
  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      CStringDelete         (This->variable[count]);
      This->variable[count] = NULL;
    }
  CMemoryFreeBlock      (This);
}
/***************************************************************************/
OAxis OFunctionGetIthAxis (
 OFunction This 
,OAxisIndex a_axis     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OAxisVerifyIndex(a_axis)==0) return NULL;
  return This->axis[a_axis];
}
/***************************************************************************/
int OFunctionSetIthAxisAttributesF (
 OFunction This 
,OAxisIndex a_axis     
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  va_list  args;
  int      length,changed;
/*.........................................................................*/
  if(This==NULL) return 0;
  va_start    (args,a_format);
  if(a_axis==OAxisV)
    OAxisSetAttributesV (This->axisOfValues,&changed,a_format,args);
  else if(OAxisVerifyIndex(a_axis)==0)
    OAxisSetAttributesV (This->axis[0],&changed,a_format,args);
  else
    OAxisSetAttributesV (This->axis[a_axis],&changed,a_format,args);
  va_end (args);
  length = strlen(a_format);
  for(count=0;count<length;count++) 
    { char c;
      c  = (char) (((char *)a_format)[count]);
      switch(c) {
        case 't':if(OAxisVerifyIndex(a_axis)==1)
                   { char*                 string;
                     OAxisGetAttributesF   (This->axis[a_axis],"t",&string);
                     CStringDelete         (This->variable[a_axis]);
                     This->variable[a_axis] = CStringDuplicate (string);}
                 break;
      }
    }  
  return changed;
}
/***************************************************************************/
void OFunctionGetIthAxisAttributesF (
 OFunction This 
,OAxisIndex a_axis     
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
/*.........................................................................*/
  if(This==NULL) return;
  va_start    (args,a_format);
  if(a_axis==OAxisV)
    OAxisGetAttributesV (This->axisOfValues,a_format,args);
  else if(OAxisVerifyIndex(a_axis)==0)
    OAxisGetAttributesV (NULL,a_format,args);
  else
    OAxisGetAttributesV (This->axis[a_axis],a_format,args);
  va_end      (args);
}
/***************************************************************************/
void OFunctionSetVariable (
 OFunction This 
,OAxisIndex a_count 
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(OAxisVerifyIndex(a_count)==0) return;
  CStringDelete(This->variable[a_count]);
  This->variable[a_count] = CStringDuplicate (a_name);
  OFunctionSetIthAxisAttributesF  (This,a_count,"t",a_name);
}
/***************************************************************************/
void OFunctionSetDimension (
 OFunction This 
,int a_dimension 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if((a_dimension<=0)||(a_dimension>OAxisMaximumNumber)) return;
  This->dimension = a_dimension;
}
/***************************************************************************/
int OFunctionGetDimension (
 OFunction This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NotFound;
  return This->dimension;
}
/***************************************************************************/
void OFunctionSetScript (
 OFunction This 
,char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;

  OStatementDelete   (This->ostmt);
  This->ostmt   = NULL;
  This->proc    = NULL;

  CStringDelete (This->script);
  This->script  = CStringDuplicate (a_string);

  This->proc    = a_string!=NULL ? OStatementGetFunctionIdentifier (a_string) : NULL ;

  if( (This->proc==NULL) && (This->script!=NULL) ) 
    This->ostmt = OStatementCreate (This->script,This->dimension,This->variable);

}
/***************************************************************************/
char* OFunctionGetScript (
 OFunction This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->script;
}
/***************************************************************************/
void OFunctionGetRange (
 OFunction This 
,double* a_min 
,double* a_max 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double val;
/*.........................................................................*/
  if(a_max!=NULL) *a_max = 0.;
  if(a_min!=NULL) *a_min = 0.;
  if(This==NULL) return;
  if(This->dimension==1)
    { 
       int count;
      double xx,dx;
      int      n0;double   m0,x0;
      OFunctionGetIthAxisAttributesF (This,OAxisX,"nmx",&n0,&m0,&x0);
      if(n0<=0) return;
      dx     = (x0 - m0)/n0;
      xx     = m0;
      val    = OFunctionEvaluateF(This,1,xx);
      if(a_max!=NULL) *a_max = val;
      if(a_min!=NULL) *a_min = val;
      n0++;
      for(count=0;count<n0;count++) 
        {
          val        = OFunctionEvaluateF(This,1,xx);
          if(a_min!=NULL) *a_min     = MINIMUM (*a_min,val);
          if(a_max!=NULL) *a_max     = MAXIMUM (*a_max,val);
          xx        += dx;
        }
    }
  else if(This->dimension==2)
    {  int ix,iy;
      double xx,yy,dx,dy;
      int      n0;double m0,x0;
      int      n1;double m1,x1;
      OFunctionGetIthAxisAttributesF(This,OAxisX,"nmx",&n0,&m0,&x0);
      OFunctionGetIthAxisAttributesF(This,OAxisY,"nmx",&n1,&m1,&x1);
      if(n0<=0 || n1<=0) return;
      dx     = (x0-m0)/n0;
      dy     = (x1-m1)/n1;
      xx     = m0;
      yy     = m1;
      val    = OFunctionEvaluateF(This,2,xx,yy);
      if(a_max!=NULL) *a_max = val;
      if(a_min!=NULL) *a_min = val;
      for(iy=0;iy<n1+1;iy++)
        {
	  for(ix=0;ix<n0+1;ix++)
	    {
	      val        = OFunctionEvaluateF(This,2,xx,yy);
	      if(a_min!=NULL) *a_min     = MINIMUM (*a_min,val);
	      if(a_max!=NULL) *a_max     = MAXIMUM (*a_max,val);
	      xx         = xx + dx;
          }
        xx     = m0;
        yy    += dy;
        }
    }
}
/***************************************************************************/
void OFunctionAdjustAxisOfValues (
 OFunction This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double min,max;
  int autoscale;
/*.........................................................................*/
  if(This==NULL) return;
  OAxisSetAttributesF(This->axisOfValues,"n",0);
  OAxisGetAttributesF(This->axisOfValues,"a",&autoscale);
  if(autoscale==1) 
    {
      OFunctionGetRange (This,&min,&max);
      OAxisSetAttributesF           (This->axisOfValues,"mx",min,max);
    }
  OAxisAdjust         (This->axisOfValues);
  if(OAxisGetTitle(This->axisOfValues)==NULL) 
    OAxisSetAttributesF (This->axisOfValues,"t",This->script);
}
/***************************************************************************/
double OFunctionEvaluateF (
 OFunction  This   
,int    a_number 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      count;
  double   largs[128];
  double   value;
/*.........................................................................*/
  if(This==NULL)    return DVOID__POS;
  if(a_number<=0)   return DVOID__POS;
  if(a_number>=100) return DVOID__POS;
  va_start          (args,a_number);
  for(count=0;count<a_number;count++) largs[count]   = va_arg(args,double);
  va_end            (args);
  value             = OFunctionEvaluate(This,a_number,largs);
  return            value;
}
/***************************************************************************/
double OFunctionEvaluate (
 OFunction   This   
,int     a_valn
,double* a_vals
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return DVOID__POS;
  if(a_valn<=0)    return DVOID__POS;
  if(a_vals==NULL) return DVOID__POS;
  if(This->proc!=NULL) 
    { double        val = 0.;
      switch(This->dimension)
        { 
          case 4:
                 val = ((OCFundouble4)This->proc)(a_vals[0],a_vals[1],a_vals[2],a_vals[3]);
                 break;
          case 3:
                 val = ((OCFundouble3)This->proc)(a_vals[0],a_vals[1],a_vals[2]);
                 break;
          case 2:
                 val = ((OCFundouble2)This->proc)(a_vals[0],a_vals[1]);
                 break;
          case 1:
                 val = ((OCFundouble1)This->proc)(a_vals[0]);
                 break;
        }
       return        val;
    }
  else if(This->ostmt!=NULL) 
    { 
      return OStatementEvaluate(This->ostmt,a_valn,a_vals);
    }
  else
    return DVOID__POS;
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
  CErrorHandle ("Ho","OFunction",a_name,a_code);
}
