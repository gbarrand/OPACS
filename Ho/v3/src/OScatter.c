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
#include <stdio.h>
*/

#include <string.h>
#include <math.h>

#include <CMemory.h>
#include <CString.h>
#include <CError.h>
#include <CPrinter.h>
#include <CList.h>

#include <OScatterP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    InitializeClass     ();
static void    ConfigureV          (OScatter,int,va_list*);
static void    FreeRecord          (OScatter);
static int     GetIdentifier       (OScatter,char*);
static void    ReportError         (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
#define DBL_INT        (int)(float)
#define NotFound       (-1)
#define MINIMUM(a,b)   ((a)<(b)?a:b)
#define MAXIMUM(a,b)   ((a)>(b)?a:b)
#define DVOID__POS     -1.
#define ONumber(arr)  (sizeof(arr)/sizeof(arr[0]))

static struct 
{
  OScatter*  extent;
} Class = {NULL};
/***************************************************************************/
void OScatterClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy     ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent    = NULL;
}
/***************************************************************************/
OScatter* OScatterGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OScatter OScatterGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OScatter)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OScatter  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OScatterGetName(This),a_name);
}
/***************************************************************************/
OScatter OScatterMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OScatter    This;
/*.........................................................................*/
  This           = (OScatter)CMemoryAllocateBlock (sizeof(OScatterRecord));
  if(This==NULL) return NULL;
  CListAddEntry  ((CList*)&(Class.extent),This);

  This->name         = NULL;
  This->dimension    = 0;
  This->title        = NULL;

  This->outFlow      = 0;

  This->pointn       = 0;
  This->entries      = 0;
  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      This->axis          [count] = OAxisMake();
      This->axisOverFlow  [count] = 0;
      This->axisUnderFlow [count] = 0;
      This->axisPoint     [count] = NULL;
    }

  return This;
}
/***************************************************************************/
OScatter OScatterCreate (
 char* a_name  
,int a_dimension    
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OScatter    This;
  va_list  args;
/*.........................................................................*/
  This           = OScatterMake ();
  if(This==NULL) return NULL;

  This->name     = CStringDuplicate(a_name);

  va_start       (args,a_dimension);
  ConfigureV     (This,a_dimension,&args);
  va_end         (args);

  return         This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OScatterIsValid (
 OScatter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
void OScatterDelete (
 OScatter This 
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
char* OScatterGetName (
 OScatter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OScatterConfigure (
 OScatter This 
,int a_dimension    
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
/*.........................................................................*/
  va_start    (args,a_dimension);
  ConfigureV (This,a_dimension,&args);
  va_end      (args);
}
/***************************************************************************/
static void ConfigureV (
 OScatter This 
,int a_dimension 
,va_list* a_args  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)       return;
  if((a_dimension<=0)||(a_dimension>OAxisMaximumNumber)) return;
/* init */
  This->dimension    = a_dimension;

/* depack args */
  for(count=0;count<This->dimension;count++) 
    { 
      double   min,max;
      char*    title;
      title    = va_arg((*a_args),char*);
      min      = va_arg((*a_args),double);
      max      = va_arg((*a_args),double);
      OAxisSetAttributesF (This->axis[count],"mxt",min,max,title);
    }
}
/***************************************************************************/
static void FreeRecord (
 OScatter This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  CStringDelete     (This->name);
  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      OAxisDelete            (This->axis[count]);
      This->axis[count]      = NULL;
      CMemoryFreeBlock              (This->axisPoint[count]);
      This->axisPoint[count] = NULL;
    }
  CStringDelete     (This->title);
  This->name        = NULL;
  This->title       = NULL;
  This->pointn      = 0;
  This->entries     = 0;
  CMemoryFreeBlock          (This);
}
/***************************************************************************/
void OScatterClear (
 OScatter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      CMemoryFreeBlock              (This->axisPoint[count]);
      This->axisPoint[count] = NULL;
    }
  This->pointn  = 0;
  This->entries = 0;
}
/***************************************************************************/
void OScatterAllocatePoints (
 OScatter This 
,int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( (a_number<=0) || (This->dimension<=0) )
    {
      OScatterClear (This);
    }
  else if(a_number>This->pointn)
    {
      int           count;
      int           ok = 1;
      OScatterClear (This);
      This->pointn  = a_number;
      for(count=0;count<This->dimension;count++) 
	{
	  This->axisPoint[count] = (double*)CMemoryAllocateBlock(This->pointn*sizeof(double));
	  if(This->axisPoint[count]==NULL) 
	    {
	      ok = 0;
	      break;
	    }
	}
      if(ok==0) OScatterClear(This);
    }
  This->entries = 0;
}
/***************************************************************************/
int OScatterGetNumberOfEntries (
 OScatter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return    This->entries;
}
/***************************************************************************/
void OScatterGetIthPoint (
 OScatter This 
,int a_count 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      count;
  va_list  args;
/*.........................................................................*/
  if(This==NULL) return;
  if((a_count<0)||(a_count>=This->entries)) return;
  va_start          (args,a_count);
  for(count=0;count<This->dimension;count++) 
    { 
      double*  addr;
      addr     = va_arg(args,double*);
      if(addr!=NULL) *addr = This->axisPoint[count][a_count];
    }
  va_end (args);
}
/***************************************************************************/
void OScatterFill (
 OScatter This 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      count;
  va_list  args;
  double   abin;
  double   abins[OAxisMaximumNumber];
/*.........................................................................*/
  if(This==NULL)                  return;
  if(This->dimension<=0)          return;
  if(This->entries>=This->pointn) return;

  va_start          (args,This);
  for(count=0;count<This->dimension;count++) 
    { 
      double       min,max;
      min          = OAxisGetMinimumValue (This->axis[count]);
      max          = OAxisGetMaximumValue (This->axis[count]);
      abins[count] = abin = va_arg(args,double);
           if( abin > max) 
        {
          This->axisOverFlow[count]++;
          This->outFlow++;
          return;
        }
      else if( abin <  min)
        {
          This->axisUnderFlow[count]++;
          This->outFlow++;
          return;
        }
    }
  va_end (args);

  for(count=0;count<This->dimension;count++) 
    { 
      This->axisPoint[count][This->entries] = abins[count];
    }
  This->entries  ++;
}
/***************************************************************************/
void OScatterFillOneDimensional (
 OScatter This 
,double a_xx
)
/***************************************************************************/
/* 
  Fast fill : no check (assume hist exist etc...).
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double   min,max;
/*.........................................................................*/
  if(This==NULL)                  return;
  if(This->dimension!=1)          return;
  if(This->entries>=This->pointn) return;

  min        = OAxisGetMinimumValue (This->axis[0]);
  max        = OAxisGetMaximumValue (This->axis[0]);
  if( a_xx >  max) {This->axisOverFlow [0]++;This->outFlow++;return;}
  if( a_xx <  min) {This->axisUnderFlow[0]++;This->outFlow++;return;}

  This->axisPoint[0][This->entries] = a_xx;
  This->entries ++;
}
/***************************************************************************/
void OScatterFillTwoDimensional (
 OScatter This 
,double a_xx  
,double a_yy  
)
/***************************************************************************/
/* 
  Fast fill : do no object check (assume hist exist etc...) .
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double   xmin,xmax;
  double   ymin,ymax;
/*.........................................................................*/
  if(This==NULL)                  return;
  if(This->dimension!=2)          return;
  if(This->entries>=This->pointn) return;

  xmin        = OAxisGetMinimumValue (This->axis[0]);
  xmax        = OAxisGetMaximumValue (This->axis[0]);
  if( a_xx >  xmax) {This->axisOverFlow [0]++;This->outFlow++;return;}
  if( a_xx <  xmin) {This->axisUnderFlow[0]++;This->outFlow++;return;}
  ymin        = OAxisGetMinimumValue (This->axis[1]);
  ymax        = OAxisGetMaximumValue (This->axis[1]);
  if( a_yy >  ymax) {This->axisOverFlow [1]++;This->outFlow++;return;}
  if( a_yy <  ymin) {This->axisUnderFlow[1]++;This->outFlow++;return;}

  This->axisPoint[0][This->entries] = a_xx;
  This->axisPoint[1][This->entries] = a_yy;
  This->entries++;
}
/***************************************************************************/
void OScatterFillThreeDimensional (
 OScatter This 
,double a_xx  
,double a_yy  
,double a_zz
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double   xmin,xmax;
  double   ymin,ymax;
  double   zmin,zmax;
/*.........................................................................*/
  if(This==NULL)                  return;
  if(This->dimension!=3)          return;
  if(This->entries>=This->pointn) return;

/* fast fill: do no object check (assume hist exist etc...) */
  xmin        = OAxisGetMinimumValue (This->axis[0]);
  xmax        = OAxisGetMaximumValue (This->axis[0]);
  if( a_xx >  xmax) {This->axisOverFlow [0]++;This->outFlow++;return;}
  if( a_xx <  xmin) {This->axisUnderFlow[0]++;This->outFlow++;return;}
  ymin        = OAxisGetMinimumValue (This->axis[1]);
  ymax        = OAxisGetMaximumValue (This->axis[1]);
  if( a_yy >  ymax) {This->axisOverFlow [1]++;This->outFlow++;return;}
  if( a_yy <  ymin) {This->axisUnderFlow[1]++;This->outFlow++;return;}
  zmin        = OAxisGetMinimumValue (This->axis[2]);
  zmax        = OAxisGetMaximumValue (This->axis[2]);
  if( a_zz >  zmax) {This->axisOverFlow [2]++;This->outFlow++;return;}
  if( a_zz <  zmin) {This->axisUnderFlow[2]++;This->outFlow++;return;}

  This->axisPoint[0][This->entries] = a_xx;
  This->axisPoint[1][This->entries] = a_yy;
  This->axisPoint[2][This->entries] = a_zz;
  This->entries++;
}
/***************************************************************************/
void OScatterSetDimension (
 OScatter This 
,int a_dimension 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_dimension==This->dimension) return;
  This->dimension = a_dimension;
  OScatterClear   (This);
}
/***************************************************************************/
int OScatterGetDimension (
 OScatter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NotFound;
  return This->dimension;
}
/***************************************************************************/
void OScatterDump (
 OScatter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  if(This->name==NULL)  CDump ("(nil)");
  else                  CDumpF("%s",This->name);
  if(This->title==NULL) CDump (" : (nil)");
  else                  CDumpF(" : %s",This->title);
  CDumpF(" : %dD",This->dimension);
  CDumpF(" : %d points\n",This->pointn);
  for(count=0;count<This->pointn;count++) 
    {
      int axis;
      CDumpF("%d :",count);
      for(axis=0;axis<This->dimension;axis++) 
	{
	  CDumpF(" %g",This->axisPoint[axis][count]);
	}
      CDump("\n");
    }
}
/***************************************************************************/
void OScatterSetName (
 OScatter This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  CStringDelete (This->name);
  This->name    = CStringDuplicate (a_name);
}
/***************************************************************************/
void OScatterSetTitle (
 OScatter This 
,char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->title);
  This->title   = CStringDuplicate (a_string);
}
/***************************************************************************/
char* OScatterGetTitle (
 OScatter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->title;
}
/***************************************************************************/
/****** Axes ***************************************************************/
/***************************************************************************/
/***************************************************************************/
OAxis OScatterGetIthAxis (
 OScatter This 
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
int OScatterSetIthAxisAttributesF (
 OScatter This 
,OAxisIndex a_axis     
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      changed;
/*.........................................................................*/
  if(This==NULL) return 0;
  if(a_format==NULL) return 0;
  va_start    (args,a_format);
  if(OAxisVerifyIndex(a_axis)==0)
    OAxisSetAttributesV (This->axis[0],&changed,a_format,args);
  else
    OAxisSetAttributesV (This->axis[a_axis],&changed,a_format,args);
  va_end (args);
  return changed;
}
/***************************************************************************/
void OScatterGetIthAxisAttributesF (
 OScatter This 
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
  if(OAxisVerifyIndex(a_axis)==0) 
    OAxisGetAttributesV (NULL,a_format,args);
  else
    OAxisGetAttributesV (This->axis[a_axis],a_format,args);
  va_end      (args);
}
/***************************************************************************/
void OScatterSetIthAxisUnderflow (
 OScatter This 
,OAxisIndex a_axis     
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                 return;
  if(a_number<0)                 return;
  if(OAxisVerifyIndex(a_axis)==0) return;
  This->axisUnderFlow[a_axis]  = a_number;
}
/***************************************************************************/
int OScatterGetIthAxisUnderflow (
 OScatter This 
,OAxisIndex a_axis     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NotFound;
  if(OAxisVerifyIndex(a_axis)==0) return NotFound;
  return This->axisUnderFlow[a_axis];
}
/***************************************************************************/
void OScatterSetIthAxisOverflow (
 OScatter This 
,OAxisIndex a_axis     
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                  return;
  if(a_number<0)             return;
  if(OAxisVerifyIndex(a_axis)==0) return;
  This->axisOverFlow[a_axis]   = a_number;
}
/***************************************************************************/
int OScatterGetIthAxisOverflow (
 OScatter This 
,OAxisIndex a_axis     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NotFound;
  if(OAxisVerifyIndex(a_axis)==0) return NotFound;
  return This->axisOverFlow[a_axis];
}
/***************************************************************************/
double OScatterGetIthAxisMeanValue (
 OScatter This 
,OAxisIndex a_axis
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double mean = 0.;
/*.........................................................................*/
  if(This==NULL)          return 0.;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  if(This->entries==0) return 0.;
  mean = 0.;
  for(count=0;count<This->entries;count++) 
    { 
      mean += This->axisPoint[a_axis][count];
    }
  return mean/This->entries;
}
/***************************************************************************/
double OScatterGetIthAxisStandardDeviation (
 OScatter This 
,OAxisIndex a_axis
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double mean,Sx2;
/*.........................................................................*/
  if(This==NULL)        return 0.;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  if(This->entries==0) return 0.;
  Sx2 = 0.;
  for(count=0;count<This->entries;count++) 
    { 
      double value;
      value  = This->axisPoint[a_axis][count];
      Sx2   += value * value;
    }
  mean = OScatterGetIthAxisMeanValue(This,a_axis);
  return sqrt (fabs( (Sx2 / This->entries) - mean * mean));
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OScatterGetAttribute (
 OScatter  This
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

       if(strcmp(a_name,"identifier")==0) *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"name")==0)               *((char**)a_addr)  = OScatterGetName(This);
  else if(strcmp(a_name,"dimension")==0)          *((int*)a_addr)    = OScatterGetDimension(This);
  else if(strcmp(a_name,"title")==0)              *((char**)a_addr)  = OScatterGetTitle(This);
  else if(strcmp(a_name,"xAxisMinimum")==0)       OScatterGetIthAxisAttributesF (This,OAxisX,"m",a_addr);
  else if(strcmp(a_name,"xAxisMaximum")==0)       OScatterGetIthAxisAttributesF (This,OAxisX,"x",a_addr);
  else if(strcmp(a_name,"yAxisMinimum")==0)       OScatterGetIthAxisAttributesF (This,OAxisY,"m",a_addr);
  else if(strcmp(a_name,"yAxisMaximum")==0)       OScatterGetIthAxisAttributesF (This,OAxisY,"x",a_addr);
  else if(strcmp(a_name,"zAxisMinimum")==0)       OScatterGetIthAxisAttributesF (This,OAxisZ,"m",a_addr);
  else if(strcmp(a_name,"zAxisMaximum")==0)       OScatterGetIthAxisAttributesF (This,OAxisZ,"x",a_addr);
  else if(strcmp(a_name,"numberOfEntries")==0)    *((int*)a_addr)    = OScatterGetNumberOfEntries (This);
  else if(strcmp(a_name,"points")==0) 
    {
      int     number = 0;
      double* doubles;
      doubles = NULL;
      if(This->entries!=0)
        {
	  doubles   = (double*)CMemoryAllocateBlock( This->entries * This->dimension * sizeof(double));
	  if(doubles!=NULL)
            {
	      int icount,count;
	      for(icount=0;icount<This->entries;icount++) 
		{
		  for(count=0;count<This->dimension;count++) 
		    { 
		      doubles[number] = This->axisPoint[count][icount];
		      number++;
		    }
		}
	    }
	}
      if(a_number!=NULL) *a_number = number;
      *((double**)a_addr) = doubles;
      return FREE_BLOCK;
    }
  else 
    {
      CInfoF ("OScatterGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  return 1;
}
/***************************************************************************/
int OScatterSetAttribute (
 OScatter  This
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

       if(strcmp(a_name,"identifier")==0) {}
  else if(strcmp(a_name,"dimension")==0)          OScatterSetDimension          (This,*((int*)a_addr));
  else if(strcmp(a_name,"name")==0)               OScatterSetName               (This,*((char**)a_addr));
  else if(strcmp(a_name,"title")==0)              OScatterSetTitle              (This,*((char**)a_addr));
  else if(strcmp(a_name,"xAxisMinimum")==0)       OScatterSetIthAxisAttributesF (This,OAxisX,"m",*((double*)a_addr));
  else if(strcmp(a_name,"xAxisMaximum")==0)       OScatterSetIthAxisAttributesF (This,OAxisX,"x",*((double*)a_addr));
  else if(strcmp(a_name,"yAxisMinimum")==0)       OScatterSetIthAxisAttributesF (This,OAxisY,"m",*((double*)a_addr));
  else if(strcmp(a_name,"yAxisMaximum")==0)       OScatterSetIthAxisAttributesF (This,OAxisY,"x",*((double*)a_addr));
  else if(strcmp(a_name,"zAxisMinimum")==0)       OScatterSetIthAxisAttributesF (This,OAxisZ,"m",*((double*)a_addr));
  else if(strcmp(a_name,"zAxisMaximum")==0)       OScatterSetIthAxisAttributesF (This,OAxisZ,"x",*((double*)a_addr));
  else if(strcmp(a_name,"numberOfEntries")==0)    {}
  else if(strcmp(a_name,"points")==0) 
    {
      int           dimension,count,pointn;
      double*       list;
      list          = *((double**)a_addr);
      OScatterClear (This);
      dimension     = OScatterGetDimension(This);
      if(dimension==1)
	{
	  pointn                 = a_number;
	  OScatterAllocatePoints (This,pointn);
	  for(count=0;count<pointn;count++) 
	    {
	      OScatterFillOneDimensional (This,list[count]);
	    }
	}
      else if(dimension==2)
	{
	  pointn                 = a_number/2;
#ifdef DEBUG
	  printf ("debug : %d\n",pointn);
#endif
	  OScatterAllocatePoints (This,pointn);
	  for(count=0;count<pointn;count++) 
	    {
	      double x,y;
              x  = *list;list++;
              y  = *list;list++;
	      OScatterFillTwoDimensional (This,x,y);
#ifdef DEBUG
	      printf ("debug : %g %g\n",x,y);
#endif
	    }
	}
      else if(dimension==3)
	{
	  pointn                 = a_number/3;
	  OScatterAllocatePoints (This,pointn);
	  for(count=0;count<pointn;count++) 
	    {
	      double x,y,z;
              x  = *list;list++;
              y  = *list;list++;
              z  = *list;list++;
	      OScatterFillThreeDimensional (This,x,y,z);
	    }
	}
      else 
	{
	  CInfoF ("OScatterSetAttribute: can't fill an %d OScatter.\n",dimension);
	  return 0;
	}
    }
  else 
    {
      CInfoF ("OScatterSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
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
  CErrorHandle ("Ho","OScatter",a_name,a_code);
}

