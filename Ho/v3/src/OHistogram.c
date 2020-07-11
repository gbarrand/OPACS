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
  For a lecture on histogramming probabilities,
 see book of Frodesen, Skjeggestad, Tofte ; 
  Probability and statistics in particle physics.
 Look p 67-68, 75, 86-87.


   After filling, the realized 
 mean value and standard dev of an axis
 are defined by : 

                   S w x
            <X>  = -----      
                    S w

  sig(X) = sqrt (<(X-<X>**2)) = sqrt (<X**2> - <X>**2)

  Th  : < (X - <X>)**2 > = <X**2> - <X>**2 
  dem : 
                   Sw(x-<X>)**2   Sw(x**2) - 2 <X> Swx + <X>**2 Sw
    <(X-<X>)**2> = ----------   = -------------------------------- = <X**2> -2<X>**2 + <X>**2 = <X**2> - <X>**2  
                     Sw                Sw

   After filling, the realized 
 bar height of a bin is defined by : 

    bar(i) = sqrt ( Sw**2) 

 where S run over bin entries.

*/


#include <string.h>
#include <stdio.h>
#include <math.h>

#include <CMemory.h>
#include <CString.h>
#include <CError.h>
#include <CPrinter.h>
#include <CList.h>
#include <CMath.h>

#include <OHistogramP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    InitializeClass  ();
static void    FreeBins         (OHistogram);
static void    ConfigureV       (OHistogram,int,va_list*);
static void    FreeRecord       (OHistogram);
static int     GetIdentifier    (OHistogram,char*);
static void    SetBinNumber     (OHistogram);
static void    InitializeData   (OHistogram);
static void    SetBinWidths     (OHistogram);
static int     GetOffset        (OHistogram,va_list*);
static double  GetBinBar        (OHistogram,int);
static void    GetBinBarRange   (OHistogram,double*,double*);
static void    ReportError      (char*,int);
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
  OHistogram*  extent;
} Class = {NULL};
/***************************************************************************/
void OHistogramClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy     ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent     = NULL;
}
/***************************************************************************/
OHistogram* OHistogramGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OHistogram OHistogramGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"OHistogram",10)==0) )
    {
      int        status;
      OHistogram identifier;
      identifier = (OHistogram)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return (OHistogram)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,pos+1);
	}
      else
	{
	  if(OHistogramIsValid(identifier)==0)
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
      return (OHistogram)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
    }
}
/***************************************************************************/
static int GetIdentifier (
 OHistogram  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OHistogramGetName(This),a_name);
}
/***************************************************************************/
OHistogram OHistogramMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OHistogram    This;
/*.........................................................................*/
  This           = (OHistogram)CMemoryAllocateBlock (sizeof(OHistogramRecord));
  if(This==NULL) return NULL;
  CListAddEntry  ((CList*)&(Class.extent),This);

  This->name           = NULL;
  This->dimension      = 0;
  This->title          = NULL;
  This->barsType       = 0;
  This->userData       = NULL;
  This->profile        = 0;
  This->monitoringMode = NULL;

  This->binNumber      = 0;
  This->binSw          = NULL;
  This->binSw2         = NULL;
  This->binEntries     = NULL;

  This->Sw             = 0.;
  This->entries        = 0;
  This->outFlow        = 0;

  for(count=0;count<OAxisMaximumNumber;count++) This->axis[count] = OAxisMake();
  This->axisOfValues     = OAxisMake ();
  OAxisSetAttributesF    (This->axisOfValues,"a",1);

  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      This->axisOffset    [count] = 0;
      This->axisBinWidth  [count] = 0.;
      This->axisOverFlow  [count] = 0;
      This->axisUnderFlow [count] = 0;
      This->axisSxw       [count] = 0.;
      This->axisSx2w      [count] = 0.;
    }

  return This;
}
/***************************************************************************/
OHistogram OHistogramCreate (
 char* a_name  
,int a_dimension    
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram     This;
  va_list        args;
/*.........................................................................*/
  This           = OHistogramMake ();
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
int OHistogramIsValid (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
void OHistogramDelete (
 OHistogram This 
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
char* OHistogramGetName (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OHistogramConfigure (
 OHistogram This 
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
 OHistogram This 
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
    { int binn;double min,max;
      char*  title;
      title  = va_arg((*a_args),char*);
      binn   = va_arg((*a_args),int);
      min    = va_arg((*a_args),double);
      max    = va_arg((*a_args),double);
      OAxisSetAttributesF (This->axis[count],"nmxt",binn,min,max,title);
    }

  OHistogramAllocateMemory  (This);
}
/***************************************************************************/
void OHistogramAllocateMemory (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  SetBinNumber  (This);
  SetBinWidths (This);
}
/***************************************************************************/
static void FreeRecord (
 OHistogram This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  CStringDelete     (This->name);
  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      OAxisDelete           (This->axis[count]);
      This->axis[count]     = NULL;
    }
  OAxisDelete          (This->axisOfValues);
  CMemoryFreeBlock     (This->binSw);
  CMemoryFreeBlock     (This->binSw2);
  CMemoryFreeBlock     (This->binEntries);
  CStringDelete        (This->title);
  CStringDelete        (This->monitoringMode);
  This->name           = NULL;
  This->title          = NULL;
  This->monitoringMode = NULL;
  This->binSw          = NULL;
  This->binSw2         = NULL;
  This->binEntries     = NULL;
  This->axisOfValues   = NULL;
  CMemoryFreeBlock     (This);
}
/***************************************************************************/
void OHistogramClear (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  InitializeData (This);
}
/***************************************************************************/
void OHistogramFillFromArray (
 OHistogram This 
,double* a_values
,double a_weight
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double   abin;
  int      offset,ibin;
  double   abins[OAxisMaximumNumber];
/*.........................................................................*/
  if(This==NULL) return;

  offset            = 0;
  for(count=0;count<This->dimension;count++) 
    { 
      double     min,max;
      min        = OAxisGetMinimumValue (This->axis[count]);
      max        = OAxisGetMaximumValue (This->axis[count]);
      abins[count] = abin = a_values[count];
           if( abin >= max) 
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
      ibin    = DBL_INT  ((abin - min)/This->axisBinWidth[count]);
      offset += ibin * This->axisOffset[count];
    }

  This->binSw[offset]      += a_weight;
  This->binSw2[offset]     += a_weight * a_weight;
  This->binEntries[offset] ++;

  This->Sw           += a_weight;
  This->entries      ++;
  for(count=0;count<This->dimension;count++) 
    {
      This->axisSxw [count] += abins[count] * a_weight;
      This->axisSx2w[count] += abins[count] * abins[count] * a_weight;
    }

}
/***************************************************************************/
void OHistogramFill (
 OHistogram This 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  va_list  args;
  double   abin,weight;
  int      offset,ibin;
  double   abins[OAxisMaximumNumber];
/*.........................................................................*/
  if(This==NULL) return;

  va_start          (args,This);
  offset            = 0;
  for(count=0;count<This->dimension;count++) 
    { 
      double     min,max;
      min        = OAxisGetMinimumValue (This->axis[count]);
      max        = OAxisGetMaximumValue (This->axis[count]);
      abins[count] = abin = va_arg(args,double);
           if( abin >= max) 
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
      ibin    = DBL_INT  ((abin - min)/This->axisBinWidth[count]);
      offset += ibin * This->axisOffset[count];
    }
  weight            =  va_arg(args,double);
  va_end            (args);

  This->binSw[offset]      += weight;
  This->binSw2[offset]     += weight * weight;
  This->binEntries[offset] ++;

  This->Sw           += weight;
  This->entries      ++;
  for(count=0;count<This->dimension;count++) 
    {
      This->axisSxw [count] += abins[count] * weight;
      This->axisSx2w[count] += abins[count] * abins[count] * weight;
    }

}
/***************************************************************************/
void OHistogramFillOneDimensional (
 OHistogram This 
,double a_xx  
,double a_weight 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double   min,max;
  int      offset;
/*.........................................................................*/
  if(This==NULL) return;

/* fast fill: do no object check (assume hist exist etc...) */
  min        = OAxisGetMinimumValue (This->axis[0]);
  max        = OAxisGetMaximumValue (This->axis[0]);
  if( a_xx >= max) {This->axisOverFlow [0]++;This->outFlow++;return;}
  if( a_xx <  min) {This->axisUnderFlow[0]++;This->outFlow++;return;}

  offset                    = DBL_INT  ((a_xx - min)/This->axisBinWidth[0]);
  This->binSw[offset]     += a_weight;
  This->binSw2[offset]  += a_weight * a_weight;
  This->binEntries[offset]++;

  This->Sw     += a_weight;
  This->entries++;

  This->axisSxw [0]    += a_xx * a_weight;
  This->axisSx2w[0]    += a_xx * a_xx * a_weight;
}
/***************************************************************************/
void OHistogramFillTwoDimensional (
 OHistogram This 
,double a_xx  
,double a_yy  
,double a_weight 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double   xmin,xmax;
  double   ymin,ymax;
  int      ibin,offset;
/*.........................................................................*/
  if(This==NULL) return;

/* fast fill: do no object check (assume hist exist etc...) */
  xmin        = OAxisGetMinimumValue (This->axis[0]);
  xmax        = OAxisGetMaximumValue (This->axis[0]);
  if( a_xx >= xmax) {This->axisOverFlow [0]++;This->outFlow++;return;}
  if( a_xx <  xmin) {This->axisUnderFlow[0]++;This->outFlow++;return;}
  ymin        = OAxisGetMinimumValue (This->axis[1]);
  ymax        = OAxisGetMaximumValue (This->axis[1]);
  if( a_yy >= ymax) {This->axisOverFlow [1]++;This->outFlow++;return;}
  if( a_yy <  ymin) {This->axisUnderFlow[1]++;This->outFlow++;return;}

  offset                    = 0;
  ibin                      = DBL_INT  ((a_xx - xmin)/This->axisBinWidth[0]);
  offset                   += ibin * This->axisOffset[0];
  ibin                      = DBL_INT  ((a_yy - ymin)/This->axisBinWidth[1]);
  offset                   += ibin * This->axisOffset[1];
  This->binSw     [offset] += a_weight;
  This->binSw2    [offset] += a_weight * a_weight;
  This->binEntries[offset]++;
  
  This->axisSxw [0]    += a_xx * a_weight;
  This->axisSx2w[0]    += a_xx * a_xx * a_weight;
  This->axisSxw [1]    += a_yy * a_weight;
  This->axisSx2w[1]    += a_yy * a_yy * a_weight;

  This->Sw     += a_weight;
  This->entries++;
}
/***************************************************************************/
void OHistogramDoProfile (
 OHistogram This 
)
/***************************************************************************/
/*
  If OHistogram is a profile, do usual OHistogramFill on 
 an one dimensional histogram and after call this routine.
 Subsequent filling will have no sense !!!
  Used by Ho/HoFillHistogramFromTuple used itself by Xo/XoPlotter.c.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  for(count=0;count<This->binNumber;count++) 
    { 
      int         nent;
      nent        = This->binEntries[count];
      if(nent!=0) 
	{ 
	  double m,sig;
	  m      = This->binSw[count]/nent;
	  sig    = sqrt(fabs(This->binSw2[count]/nent - m * m ));
	  This->binSw[count]  = m;
	  This->binSw2[count] = sig * sig;  /*So that GetBinBar return sig.*/
	}
    }
}
/***************************************************************************/
void OHistogramSetDimension (
 OHistogram This 
,int a_dimension 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_dimension==This->dimension) return;
  This->dimension    = a_dimension;
  SetBinNumber       (This);
}
/***************************************************************************/
int OHistogramGetDimension (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NotFound;
  return This->dimension;
}
/***************************************************************************/
void OHistogramDump (
 OHistogram This 
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
  CDumpF(" : %d bins\n",This->binNumber);
  for(count=0;count<This->binNumber;count++) 
    /*    if(This->binEntries[count]!=0) */
      CDumpF("%d : %d %g\n",count,This->binEntries[count],This->binSw[count]);
}
/***************************************************************************/
void OHistogramSetNumberOfEntries (
 OHistogram This 
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)         return;
  if(a_number<0)    return;
  This->entries = a_number;
}
/***************************************************************************/
int OHistogramGetNumberOfEntries (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->entries;
}
/***************************************************************************/
void OHistogramSetSumOfWeights (
 OHistogram This 
,double a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->Sw = a_value;
}
/***************************************************************************/
double OHistogramGetSumOfWeights (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return This->Sw;
}
/***************************************************************************/
void OHistogramSetName (
 OHistogram This 
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
void OHistogramSetTitle (
 OHistogram This 
,char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( (a_string!=NULL) && (This->title!=NULL) && (strcmp(a_string,This->title)==0) ) return;
  CStringDelete (This->title);
  This->title   = CStringDuplicate (a_string);
}
/***************************************************************************/
char* OHistogramGetTitle (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->title;
}
/***************************************************************************/
void OHistogramSetUserData (
 OHistogram This 
,void* a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->userData = a_user;
}
/***************************************************************************/
void* OHistogramGetUserData (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->userData;
}
/***************************************************************************/
void OHistogramSetProfile (
 OHistogram This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->profile = a_value;
}
/***************************************************************************/
int OHistogramIsProfile (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->profile;
}
/***************************************************************************/
void OHistogramSetMonitoringMode (
 OHistogram This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete        (This->monitoringMode);
  This->monitoringMode = CStringDuplicate (a_string);
}
/***************************************************************************/
char* OHistogramGetMonitoringMode (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->monitoringMode;
}
/***************************************************************************/
void OHistogramSetBarsType (
 OHistogram This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->barsType = a_value;
}
/***************************************************************************/
int OHistogramIsBarsType (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->barsType;
}
/***************************************************************************/
void OHistogramAddContents (
 OHistogram This 
,OHistogram a_hist
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)        return;
  if(a_hist==NULL)      return;

  if(This->dimension != a_hist->dimension) 
    {
      CWarn ("OHistogramAddContents : histograms have not same dimension.\n");
      return;
    }

  for(count=0;count<This->dimension;count++) 
    { 
      int      n1,n2;
      OAxisGetAttributesF (This->axis[count]  ,"n",&n1);
      OAxisGetAttributesF (a_hist->axis[count],"n",&n2);
      if(n1!=n2)
	{
	  CWarn ("OHistogramAddContents : histograms have not same bin number.\n");
	  return;
	}
    }

  for(count=0;count<This->binNumber;count++) 
    {
      This->binSw      [count] += a_hist->binSw      [count];      
      This->binSw2     [count] += a_hist->binSw2     [count];      
      This->binEntries [count] += a_hist->binEntries [count];      
    }

  This->entries  += a_hist->entries;
  This->Sw       += a_hist->Sw;

  for(count=0;count<This->dimension;count++) 
    {
      This->axisOverFlow   [count] += a_hist->axisOverFlow  [count];
      This->axisUnderFlow  [count] += a_hist->axisUnderFlow [count];

      This->axisSxw        [count] += a_hist->axisSxw  [count];
      This->axisSx2w       [count] += a_hist->axisSx2w [count];
    }

}
/***************************************************************************/
int OHistogramOperateBinary (
 OHistogram This 
,OHistogram a_h1
,OHistogram a_h2
,int    a_operation
,double a_c1
,double a_c2
)
/***************************************************************************/
/*
  This = a_c1 * a_h1 (a_operation) a_c2 * a_h2 
 What to do with This->binSw2 ?
 What to do with This->Sw, This->axisSxw, This->axisSx2w ?
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)  return 0;
  if(a_h1==NULL)  return 0;
  if(a_h2==NULL)  return 0;

  OHistogramClear (This);

  if(This->dimension != a_h1->dimension)
    {
      CWarn  ("OHistogramOperateBinary : histograms have not same dimension.\n");
      return 0;
    }

  if(This->dimension != a_h2->dimension)
    {
      CWarn  ("OHistogramOperateBinary : histograms have not same dimension.\n");
      return 0;
    }

  for(count=0;count<This->dimension;count++) 
    { 
      int                 n,n1,n2;
      OAxisGetAttributesF (This->axis[count] ,"n",&n);
      OAxisGetAttributesF (a_h1->axis[count] ,"n",&n1);
      OAxisGetAttributesF (a_h2->axis[count] ,"n",&n2);
      if(n!=n1)
	{
	  CWarn  ("OHistogramOperateBinary : histograms have not same bin number.\n");
	  return 0;
	}
      if(n!=n2)
	{
	  CWarn  ("OHistogramOperateBinary : histograms have not same bin number.\n");
	  return 0;
	}
    }

  /*What to do with binSw2 ?*/
  if(a_operation==OHistogramAdd)
    {
      for(count=0;count<This->binNumber;count++) 
	{
	  This->binSw      [count]  = a_c1 * a_h1->binSw [count] + a_c2 * a_h2->binSw [count];      
	  This->binSw2     [count]  = a_h1->binSw2     [count] + a_h2->binSw2     [count];      
	  This->binEntries [count]  = a_h1->binEntries [count] + a_h2->binEntries [count];      
	}
    }
  else if(a_operation==OHistogramSubstract)
    {
      for(count=0;count<This->binNumber;count++) 
	{
	  This->binSw      [count]  = a_c1 * a_h1->binSw [count] - a_c2 * a_h2->binSw [count];      
	  This->binSw2     [count]  = a_h1->binSw2     [count] + a_h2->binSw2     [count];      
	  This->binEntries [count]  = a_h1->binEntries [count] + a_h2->binEntries [count];      
	}
    }
  else if(a_operation==OHistogramMultiply)
    {
      for(count=0;count<This->binNumber;count++) 
	{
	  This->binSw      [count]  = a_c1 * a_h1->binSw [count] * a_c2 * a_h2->binSw [count];      
	  This->binSw2     [count]  = a_h1->binSw2     [count] + a_h2->binSw2     [count];      
	  This->binEntries [count]  = a_h1->binEntries [count] + a_h2->binEntries [count];      
	}
    }
  else if(a_operation==OHistogramDivide)
    {
      if(a_c2==0.)
	{
	  CWarnF ("OHistogramOperateBinary : can't divide by zero.\n");
	  return 0;
	}
      for(count=0;count<This->binNumber;count++) 
	{
	  if(a_h2->binSw[count]==0.)
	    {
	      This->binSw [count]  = 0.;
	    }
	  else
	    {
	      This->binSw [count]  = a_c1 * a_h1->binSw [count] / (a_c2 * a_h2->binSw [count]);      
	    }
	  This->binSw2     [count] = a_h1->binSw2     [count] + a_h2->binSw2     [count];      
	  This->binEntries [count] = a_h1->binEntries [count] + a_h2->binEntries [count];      
	}
    }
  else
    {
      CWarnF ("OHistogramOperateBinary : %d unknown operation.\n",a_operation);
      return 0;
    }

  This->entries   = a_h1->entries + a_h2->entries;
  This->Sw        = a_h1->Sw      + a_h2->Sw;

  for(count=0;count<This->dimension;count++) 
    {
      This->axisOverFlow   [count] = a_h1->axisOverFlow  [count] + a_h2->axisOverFlow  [count];
      This->axisUnderFlow  [count] = a_h1->axisUnderFlow [count] + a_h2->axisUnderFlow [count];
      /*What to do with axisSxw, axisSx2w ?*/
      This->axisSxw        [count] = a_h1->axisSxw  [count] + a_h2->axisSxw  [count];
      This->axisSx2w       [count] = a_h1->axisSx2w [count] + a_h2->axisSx2w [count];
    }

  return 1;
}
/***************************************************************************/
int OHistogramFindFirstEmptyBin (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)         return NotFound;
  if(This->dimension!=1) return NotFound;
  for(count=0;count<This->binNumber;count++) 
    {
      if(This->binEntries[count]==0)
	{
	  return count;
	}
    }
  return NotFound;
}
/***************************************************************************/
int OHistogramFindFirstNotEmptyBin (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)         return NotFound;
  if(This->dimension!=1) return NotFound;
  for(count=0;count<This->binNumber;count++) 
    {
      if(This->binEntries[count]!=0)
	{
	  return count;
	}
    }
  return NotFound;
}
/***************************************************************************/
int OHistogramFindLastNotEmptyBin (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)         return NotFound;
  if(This->dimension!=1) return NotFound;
  for(count=This->binNumber-1;count>=0;count--) 
    {
      if(This->binEntries[count]!=0)
	{
	  return count;
	}
    }
  return NotFound;
}
/****************************************************************************/
int OHistogramGetHalfLowerMeanPoint (
 OHistogram This
,double* a_x
,double* a_v
)
/***************************************************************************/
/*
  Used to find a starting point for linear, exponential fit.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int    binn,end;
  int    n;
  double x,v;
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_v!=NULL) *a_v = 0.;
  if(This==NULL)         return 0;
  if(This->dimension!=1) return 0;
  OHistogramGetIthAxisAttributesF (This,OAxisX,"n",&binn);
  end = binn/2;
  n = 0;
  x = 0.;
  v = 0.;
  for(count=0;count<end;count++)
    { 
      double           xx,val;
      int              nent;
      nent             = OHistogramGetBinNumberOfEntries (This,count);
      if(nent==0)      continue;  /* do not take bin without entry */
      xx               = OHistogramGetIthAxisBinMiddle (This,OAxisX,count);  /* take middle of bin */
      val              = OHistogramGetBinSumOfWeights  (This,count);
      x               += xx;
      v               += val;
      n++;
    }
  if(n<=0) return 0;
  if(a_x!=NULL) *a_x = x/n;
  if(a_v!=NULL) *a_v = v/n;
  return 1;
}
/****************************************************************************/
int OHistogramGetHalfUpperMeanPoint (
 OHistogram This
,double* a_x
,double* a_v
)
/***************************************************************************/
/*
  Used to find an end point for linear, exponential fit.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int    binn,begin;
  int    n;
  double x,v;
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_v!=NULL) *a_v = 0.;
  if(This==NULL)         return 0;
  if(This->dimension!=1) return 0;
  OHistogramGetIthAxisAttributesF (This,OAxisX,"n",&binn);
  begin = binn/2;
  n = 0;
  x = 0.;
  v = 0.;
  for(count=begin;count<binn;count++)
    { 
      double           xx,val;
      int              nent;
      nent             = OHistogramGetBinNumberOfEntries (This,count);
      if(nent==0)      continue;  /* do not take bin without entry */
      xx               = OHistogramGetIthAxisBinMiddle (This,OAxisX,count);  /* take middle of bin */
      val              = OHistogramGetBinSumOfWeights  (This,count);
      x               += xx;
      v               += val;
      n++;
    }
  if(n<=0) return 0;
  if(a_x!=NULL) *a_x = x/n;
  if(a_v!=NULL) *a_v = v/n;
  return 1;
}
/***************************************************************************/
/******* Bins **************************************************************/
/***************************************************************************/
/***************************************************************************/
void OHistogramSetBinSumOfWeights (
 OHistogram This 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      offset;
  double   value;
/*.........................................................................*/
  if(This==NULL)          return;
  if(This->binNumber==0)  return;
  va_start          (args,This);
  offset            = GetOffset (This,&args);
  value             = va_arg(args,double);
  va_end            (args);
  if(offset==NotFound) return;
  This->binSw[offset] = value;
}
/***************************************************************************/
double OHistogramGetBinSumOfWeights (
 OHistogram This 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      offset;
/*.........................................................................*/
  if(This==NULL) return DVOID__POS;
  if(This->binNumber==0) return DVOID__POS;
  va_start          (args,This);
  offset            = GetOffset (This,&args);
  va_end            (args);
  if(offset==NotFound) return DVOID__POS;
  return            This->binSw[offset];
}
/***************************************************************************/
void OHistogramSetBinNumberOfEntries (
 OHistogram This 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      offset;
  int      nent;
/*.........................................................................*/
  if(This==NULL)         return;
  if(This->binNumber==0)  return;
  va_start          (args,This);
  offset            = GetOffset (This,&args);
  nent              = va_arg(args,int);
  va_end            (args);
  if(offset==NotFound) return;
  This->binEntries[offset] = nent;
}
/***************************************************************************/
int OHistogramGetBinNumberOfEntries (
 OHistogram This 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      offset;
/*.........................................................................*/
  if(This==NULL) return NotFound;
  if(This->binNumber==0) return NotFound;
  va_start          (args,This);
  offset            = GetOffset (This,&args);
  va_end            (args);
  if(offset==NotFound) return NotFound;
  return            This->binEntries[offset];
}
/***************************************************************************/
void OHistogramSetBinSumOfSquaredWeights (
 OHistogram This 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      offset;
  double   value;
/*.........................................................................*/
  if(This==NULL)         return;
  if(This->binNumber==0)  return;
  va_start          (args,This);
  offset            = GetOffset (This,&args);
  value             = va_arg(args,double);
  va_end            (args);
  if(offset==NotFound)  return;
  This->binSw2[offset] = value;
}
/***************************************************************************/
double OHistogramGetBinSumOfSquaredWeights (
 OHistogram This 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      offset;
/*.........................................................................*/
  if(This==NULL) return DVOID__POS;
  if(This->binNumber==0) return DVOID__POS;
  va_start          (args,This);
  offset            = GetOffset (This,&args);
  va_end            (args);
  if(offset==NotFound) return DVOID__POS;
  return This->binSw2[offset];
}
/***************************************************************************/
void OHistogramGetBinSumOfWeightsRange (
 OHistogram This 
,double* a_min 
,double* a_max 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_max!=NULL) *a_max = 0.;
  if(a_min!=NULL) *a_min = 0.;
  if(This==NULL) return;
  for(count=0;count<This->binNumber;count++) 
    { double val;
      val    = This->binSw[count];
      if(count==0)
        {  
          if(a_max!=NULL) *a_max = val;
	  if(a_min!=NULL) *a_min = val;
        }
      else 
        {
          if(a_max!=NULL) *a_max = MAXIMUM (*a_max,val);
          if(a_min!=NULL) *a_min = MINIMUM (*a_min,val);
        }
    }
}
/***************************************************************************/
double OHistogramGetBinBar (
 OHistogram This 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      offset;
/*.........................................................................*/
  if(This==NULL) return DVOID__POS;
  if(This->binNumber==0) return DVOID__POS;
  va_start          (args,This);
  offset            = GetOffset (This,&args);
  va_end            (args);
  if(offset==NotFound) return DVOID__POS;
  return            GetBinBar(This,offset);
}
/***************************************************************************/
static double GetBinBar (
 OHistogram This 
,int a_offset
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return sqrt(fabs(This->binSw2[a_offset]));
}
/***************************************************************************/
static void GetBinBarRange (
 OHistogram This 
,double* a_min 
,double* a_max 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_max!=NULL) *a_max = 0.;
  if(a_min!=NULL) *a_min = 0.;
  if(This==NULL) return;
  for(count=0;count<This->binNumber;count++) 
    { double val,error;
      val    = This->binSw[count];
      error  = GetBinBar(This,count);
      error  = error/2.;
      if(count==0)
        {
          if(a_max!=NULL) *a_max = val + error;
          if(a_min!=NULL) *a_min = val - error;
        }
      else 
        {
          if(a_max!=NULL) *a_max = MAXIMUM (*a_max,val+error);
          if(a_min!=NULL) *a_min = MINIMUM (*a_min,val-error);
        }
    }
}
/***************************************************************************/
/****** Axes ***************************************************************/
/***************************************************************************/
/***************************************************************************/
void OHistogramAdjustAxisOfValues (
 OHistogram This 
,int a_bar
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double min,max;
  int autoscale;
/*.........................................................................*/
  if(This==NULL) return;
/* axis value set */
  OAxisSetAttributesF (This->axisOfValues,"n",0);
  OAxisGetAttributesF (This->axisOfValues,"a",&autoscale);
  if(autoscale==1) 
    {
      if(a_bar==0)        OHistogramGetBinSumOfWeightsRange (This,&min,&max);
      else                GetBinBarRange                    (This,&min,&max);
      OAxisSetAttributesF (This->axisOfValues,"mx",min,max);
    }
  OAxisAdjust         (This->axisOfValues);
  if(OAxisGetTitle(This->axisOfValues)==NULL) 
    OAxisSetAttributesF (This->axisOfValues,"t","Bin content");
}
/***************************************************************************/
void OHistogramSetAxisOfValuesTitle (
 OHistogram This 
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OAxisSetAttributesF (This->axisOfValues,"t",a_value);
}
/***************************************************************************/
char* OHistogramGetAxisOfValuesTitle (
 OHistogram This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return OAxisGetTitle(This->axisOfValues);
}
/***************************************************************************/
OAxis OHistogramGetIthAxis (
 OHistogram This 
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
int OHistogramSetIthAxisAttributesF (
 OHistogram This 
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
  if(a_format==NULL) return 0;
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
        case 'n':SetBinNumber (This);
	         SetBinWidths(This);
                 break;
        case 'm':SetBinWidths(This);break;
        case 'x':SetBinWidths(This);break;
        case 'd':SetBinWidths(This);break;
        case 'u':SetBinWidths(This);break;
      }
    }  
  return changed;
}
/***************************************************************************/
void OHistogramGetIthAxisAttributesF (
 OHistogram This 
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
double OHistogramGetIthAxisBinWidth (
 OHistogram This 
,OAxisIndex a_axis     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return DVOID__POS;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  return This->axisBinWidth[a_axis];
}
/***************************************************************************/
double OHistogramGetIthAxisBinMinimum (
 OHistogram This 
,OAxisIndex a_axis     
,int a_bin 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    binn;
  double min;
/*.........................................................................*/
  if(This==NULL) return DVOID__POS;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  OAxisGetAttributesF (This->axis[a_axis],"nm",&binn,&min);
  if((a_bin<0) ||(a_bin>=binn)) return DVOID__POS;
  return (min + a_bin * This->axisBinWidth[a_axis]);
}
/***************************************************************************/
double OHistogramGetIthAxisBinMaximum (
 OHistogram This 
,OAxisIndex a_axis     
,int a_bin 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    binn;
  double min;
/*.........................................................................*/
  if(This==NULL) return DVOID__POS;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  OAxisGetAttributesF (This->axis[a_axis],"nm",&binn,&min);
  if((a_bin<0) ||(a_bin>=binn)) return DVOID__POS;
  return (min + (a_bin + 1)* This->axisBinWidth[a_axis]);
}
/***************************************************************************/
double OHistogramGetIthAxisBinMiddle (
 OHistogram This 
,OAxisIndex a_axis     
,int a_bin 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    binn;
  double min;
/*.........................................................................*/
  if(This==NULL) return DVOID__POS;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  OAxisGetAttributesF(This->axis[a_axis],"nm",&binn,&min);
  if((a_bin<0) ||(a_bin>=binn)) return DVOID__POS;
  return (min + (a_bin + .5) * This->axisBinWidth[a_axis] );
}
/***************************************************************************/
int OHistogramGetIthAxisBinIndex (
 OHistogram This 
,OAxisIndex a_axis     
,double a_bin 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    binn;
  double min,max;
/*.........................................................................*/
  if(This==NULL) return NotFound;
  if(OAxisVerifyIndex(a_axis)==0) return NotFound;
  OAxisGetAttributesF(This->axis[a_axis],"nmx",&binn,&min,&max);
  if((a_bin<min) ||(a_bin>=max)) return NotFound;
  return DBL_INT((a_bin - min)/This->axisBinWidth[a_axis]);
}
/***************************************************************************/
void OHistogramSetIthAxisUnderflow (
 OHistogram This 
,OAxisIndex a_axis     
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)             return;
  if(a_number<0)             return;
  if(OAxisVerifyIndex(a_axis)==0) return;
  This->axisUnderFlow[a_axis]  = a_number;
}
/***************************************************************************/
int OHistogramGetIthAxisUnderflow (
 OHistogram This 
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
void OHistogramSetIthAxisOverflow (
 OHistogram This 
,OAxisIndex a_axis     
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)             return;
  if(a_number<0)             return;
  if(OAxisVerifyIndex(a_axis)==0) return;
  This->axisOverFlow[a_axis]   = a_number;
}
/***************************************************************************/
int OHistogramGetIthAxisOverflow (
 OHistogram This 
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
void OHistogramSetIthAxisSumOfWeightedCoordinates (
 OHistogram This 
,OAxisIndex a_axis     
,double a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                  return;
  if(OAxisVerifyIndex(a_axis)==0) return;
  This->axisSxw[a_axis]    = a_value;
}
/***************************************************************************/
double OHistogramGetIthAxisSumOfWeightedCoordinates (
 OHistogram This 
,OAxisIndex a_axis     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                  return DVOID__POS;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  return                     This->axisSxw[a_axis];
}
/***************************************************************************/
void OHistogramSetIthAxisSumOfWeightedSquaredCoordinates (
 OHistogram This 
,OAxisIndex a_axis     
,double a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                  return;
  if(OAxisVerifyIndex(a_axis)==0) return;
  This->axisSx2w[a_axis]       = a_value;
}
/***************************************************************************/
double OHistogramGetIthAxisSumOfWeightedSquaredCoordinates (
 OHistogram This 
,OAxisIndex a_axis     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                  return DVOID__POS;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  return                     This->axisSx2w[a_axis];
}
/***************************************************************************/
double OHistogramGetIthAxisMeanValue (
 OHistogram This 
,OAxisIndex a_axis
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)        return 0.;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  if(This->Sw==0.) return 0.; 
  return (This->axisSxw[a_axis] / This->Sw);
}
/***************************************************************************/
double OHistogramGetIthAxisStandardDeviation (
 OHistogram This 
,OAxisIndex a_axis
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double mean;
/*.........................................................................*/
  if(This==NULL)        return 0.;
  if(OAxisVerifyIndex(a_axis)==0) return DVOID__POS;
  if(This->Sw==0.) return 0.; 
  mean   = (This->axisSxw[a_axis] / This->Sw);
  return sqrt (fabs( (This->axisSx2w[a_axis] / This->Sw) - mean * mean));
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OHistogramGetBinsRange (
 OHistogram This
,int a_binn
,int* a_is
,int* a_js
,double* a_xmin 
,double* a_xmax 
,double* a_ymin 
,double* a_ymax 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int    first;
/*.........................................................................*/
  if(This==NULL) return 0;
  first = 0;
       if( (OHistogramGetDimension(This)==1) && 
	   (a_is!=NULL)   && 
	   (a_xmin!=NULL) && 
	   (a_xmax!=NULL) 
	  )
    { 
      for(count=0;count<a_binn;count++)
        { 
	  double min,max;
	  min    = OHistogramGetIthAxisBinMinimum   (This,OAxisX,a_is[count]);
	  max    = OHistogramGetIthAxisBinMaximum   (This,OAxisX,a_is[count]);
	  if(first==0)
	    {
	      *a_xmin  = min;
	      *a_xmax  = max;
	    }
	  else
	    {
	      *a_xmin  = MINIMUM(*a_xmin,min);
	      *a_xmax  = MAXIMUM(*a_xmax,max);
	    }
	  first = 1;
        }
    }
  else if( (OHistogramGetDimension(This)==2) && 
	   (a_is!=NULL)   && 
	   (a_js!=NULL)   && 
	   (a_xmin!=NULL) && 
           (a_xmax!=NULL) && 
	   (a_ymin!=NULL) && 
	   (a_ymax!=NULL)
	  )
    {
      for(count=0;count<a_binn;count++)
        { 
	  double xmin,xmax,ymin,ymax;
	  xmin   = OHistogramGetIthAxisBinMinimum   (This,OAxisX,a_is[count]);
	  xmax   = OHistogramGetIthAxisBinMaximum   (This,OAxisX,a_is[count]);
	  ymin   = OHistogramGetIthAxisBinMinimum   (This,OAxisY,a_js[count]);
	  ymax   = OHistogramGetIthAxisBinMaximum   (This,OAxisY,a_js[count]);
	  if(first==0)
	    {
	      *a_xmin      = xmin;
	      *a_xmax      = xmax;
	      *a_ymin      = ymin;
	      *a_ymax      = ymax;
	    }
	  else
	    {
	      *a_xmin      = MINIMUM(*a_xmin,xmin);
	      *a_xmax      = MAXIMUM(*a_xmax,xmax);
	      *a_ymin      = MINIMUM(*a_ymin,ymin);
	      *a_ymax      = MAXIMUM(*a_ymax,ymax);
	    }
	  first = 1;
        }
    }
  return first;
}
/***************************************************************************/
int OHistogramGetAttribute (
 OHistogram  This
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

       if(strcmp(a_name,"identifier")==0)        *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"name")==0)              *((char**)a_addr)  = This->name;
  else if(strcmp(a_name,"dimension")==0)         *((int*)a_addr)    = This->dimension;
  else if(strcmp(a_name,"title")==0)             *((char**)a_addr)  = This->title;
  else if(strcmp(a_name,"xAxisTitle")==0)        OHistogramGetIthAxisAttributesF (This,OAxisX,"t",a_addr);
  else if(strcmp(a_name,"xAxisNumberOfBins")==0) OHistogramGetIthAxisAttributesF (This,OAxisX,"n",a_addr);
  else if(strcmp(a_name,"xAxisMinimum")==0)      OHistogramGetIthAxisAttributesF (This,OAxisX,"m",a_addr);
  else if(strcmp(a_name,"xAxisMaximum")==0)      OHistogramGetIthAxisAttributesF (This,OAxisX,"x",a_addr);
  else if(strcmp(a_name,"yAxisTitle")==0)        OHistogramGetIthAxisAttributesF (This,OAxisY,"t",a_addr);
  else if(strcmp(a_name,"yAxisNumberOfBins")==0) OHistogramGetIthAxisAttributesF (This,OAxisY,"n",a_addr);
  else if(strcmp(a_name,"yAxisMinimum")==0)      OHistogramGetIthAxisAttributesF (This,OAxisY,"m",a_addr);
  else if(strcmp(a_name,"yAxisMaximum")==0)      OHistogramGetIthAxisAttributesF (This,OAxisY,"x",a_addr);
  else if(strcmp(a_name,"zAxisTitle")==0)        OHistogramGetIthAxisAttributesF (This,OAxisZ,"t",a_addr);
  else if(strcmp(a_name,"zAxisNumberOfBins")==0) OHistogramGetIthAxisAttributesF (This,OAxisZ,"n",a_addr);
  else if(strcmp(a_name,"zAxisMinimum")==0)      OHistogramGetIthAxisAttributesF (This,OAxisZ,"m",a_addr);
  else if(strcmp(a_name,"zAxisMaximum")==0)      OHistogramGetIthAxisAttributesF (This,OAxisZ,"x",a_addr);
  else if(strcmp(a_name,"vAxisTitle")==0)        OHistogramGetIthAxisAttributesF (This,OAxisV,"t",a_addr);
  else if(strcmp(a_name,"monitoringMode")==0)    *((char**)a_addr) = This->monitoringMode;
  else if(strcmp(a_name,"profile")==0)           *((int*)a_addr)   = This->profile;
  else if(strcmp(a_name,"bins")==0) 
    {
      int        number = 0;
      double*    doubles;
      doubles    = NULL;
      if(This->binNumber!=0)
        {
	  doubles = (double*)CMemoryAllocateBlock( This->binNumber * sizeof(double));
	  if(doubles!=NULL)
            {
	        int count;
	      number    = This->binNumber;
	      for(count=0;count<number;count++) 
		doubles[count] = This->binSw[count];
	    }
	}
      if(a_number!=NULL) *a_number = number;
      *((double**)a_addr) = doubles;
      return FREE_BLOCK;
    }
  else 
    {
      CInfoF ("OHistogramGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OHistogramSetAttribute (
 OHistogram  This
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
  else if(strcmp(a_name,"dimension")==0)         OHistogramSetDimension (This,*((int*)a_addr));
  else if(strcmp(a_name,"name")==0)              OHistogramSetName      (This,*((char**)a_addr));
  else if(strcmp(a_name,"title")==0)             OHistogramSetTitle     (This,*((char**)a_addr));
  else if(strcmp(a_name,"xAxisTitle")==0)        OHistogramSetIthAxisAttributesF      (This,OAxisX,"t",*((char**) a_addr));
  else if(strcmp(a_name,"xAxisNumberOfBins")==0) OHistogramSetIthAxisAttributesF      (This,OAxisX,"n",*((int*)   a_addr));
  else if(strcmp(a_name,"xAxisMinimum")==0)      OHistogramSetIthAxisAttributesF      (This,OAxisX,"m",*((double*)a_addr));
  else if(strcmp(a_name,"xAxisMaximum")==0)      OHistogramSetIthAxisAttributesF      (This,OAxisX,"x",*((double*)a_addr));
  else if(strcmp(a_name,"yAxisTitle")==0)        OHistogramSetIthAxisAttributesF      (This,OAxisY,"t",*((char**) a_addr));
  else if(strcmp(a_name,"yAxisNumberOfBins")==0) OHistogramSetIthAxisAttributesF      (This,OAxisY,"n",*((int*)   a_addr));
  else if(strcmp(a_name,"yAxisMinimum")==0)      OHistogramSetIthAxisAttributesF      (This,OAxisY,"m",*((double*)a_addr));
  else if(strcmp(a_name,"yAxisMaximum")==0)      OHistogramSetIthAxisAttributesF      (This,OAxisY,"x",*((double*)a_addr));
  else if(strcmp(a_name,"zAxisTitle")==0)        OHistogramSetIthAxisAttributesF      (This,OAxisZ,"t",*((char**) a_addr));
  else if(strcmp(a_name,"zAxisNumberOfBins")==0) OHistogramSetIthAxisAttributesF      (This,OAxisZ,"n",*((int*)   a_addr));
  else if(strcmp(a_name,"zAxisMinimum")==0)      OHistogramSetIthAxisAttributesF      (This,OAxisZ,"m",*((double*)a_addr));
  else if(strcmp(a_name,"zAxisMaximum")==0)      OHistogramSetIthAxisAttributesF      (This,OAxisZ,"x",*((double*)a_addr));
  else if(strcmp(a_name,"vAxisTitle")==0)        OHistogramSetIthAxisAttributesF      (This,OAxisV,"t",*((char**) a_addr));
  else if(strcmp(a_name,"monitoringMode")==0)    OHistogramSetMonitoringMode          (This,*((char**)a_addr));
  else if(strcmp(a_name,"profile")==0)           OHistogramSetProfile                 (This,*((int*)a_addr));
  else if(strcmp(a_name,"bins")==0) 
    {
      if( (a_number!=0) && (This->binNumber!=0) )
        {
	  if(a_number!=This->binNumber)
            {
              CWarnF
		("OHistogramSetAttribute: for %s bin number mismatch:\ngiven:%d requested:%d.\n",
                This->name!=NULL ? This->name : "(nil)",a_number,This->binNumber);
	      return 0;
	    }
	  else
            {
	        int count;
	      double*   list;
	      list      = *((double**)a_addr);
	      for(count=0;count<a_number;count++) This->binSw[count] = list[count];
	    }
	}
    }
  else 
    {
      CInfoF ("OHistogramSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int OHistogramDoMethod (
 OHistogram This
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
      CInfoF("OHistogramDoMethod: %s is not a class routine property.\n",a_name);
      return 0;
    }

  if(strcmp(a_name,"fill")==0)
    {
      int     hasWeight;
      double* values;
      double  weight = 1.;
      int     count;
      if(This->dimension<=0) return 1;
      if( (a_argn!=This->dimension) && (a_argn!=(This->dimension+1)) ) 
	{
	  CWarnF  ("%d or %d arguments expected for %s property.\n",
		   This->dimension,This->dimension+1,a_name);
	  return  0;
	}
      hasWeight    = a_argn==This->dimension ? 0 : 1;
      values       = (double*)CMemoryAllocateBlock( This->dimension * sizeof(double));
      if(values==NULL) return 0;
      for(count=0;count<This->dimension;count++) 
	{ 
	  int           status;
	  values[count] = CStringConvertToDouble(a_args[count],&status);
	  if(status==0) 
	    {
	      CWarnF           ("Argument %s not a number.\n",a_args[count]);
	      CMemoryFreeBlock (values);
	      return           0;
	    }
	}
      if(hasWeight==1)
	{ 
	  int    status;
	  weight = CStringConvertToDouble(a_args[This->dimension],&status);
	  if(status==0) 
	    {
	      CWarnF           ("Argument %s not a number.\n",a_args[This->dimension]);
	      CMemoryFreeBlock (values);
	      return           0;
	    }
	}
      OHistogramFillFromArray (This,values,weight);
      CMemoryFreeBlock        (values);
    }
  else if(strcmp(a_name,"fillRandom")==0)
    {
      int     type = 1;
      int     status,number;
      int     count,i;
      double* values;
      if( (a_argn!=1) && (a_argn!=2) )
	{
	  CWarnF ("One or two arguments expected for %s property.\n",a_name);
	  return 0;
	}
      if(a_argn==2)
	{
	  if(strcmp(a_args[1],"gauss")==0)
	    {
	      type = 1;
	    }
	  else if(strcmp(a_args[1],"flat")==0)
	    {
	      type = 2;
	    }
	  else 
	    {
	      CWarnF ("For %s property, %s is a bad keyword (flat, gauss expected).\n",
		      a_name,a_args[1]);
	      return 0;
	    }
	}
      number = CStringConvertToDouble(a_args[0],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[0]);
	  return 0;
	}
      values = (double*)CMemoryAllocateBlock( This->dimension * sizeof(double));
      if(values==NULL) return 0;
      for(count=0;count<number;count++) 
	{ 	 
	  for(i=0;i<This->dimension;i++) 
	    {
	      if(type==1) {
		values[i] = CMathGetRandomGauss();
	      } else if(type==2) {
		values[i] = CMathGetRandomFlat();
	      }
	    }
	  OHistogramFillFromArray (This,values,1.);
	}
      CMemoryFreeBlock        (values);
    }
  else if(strcmp(a_name,"clear")==0)
    {
      OHistogramClear (This);
    }
  else 
    {
      CInfoF ("OHistogramDoMethod: %s is not a routine property.\n",a_name);
      return 0;
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  a_addr = NULL;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
char** OHistogramGetDimensions (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* dimension[2] = {"1","2"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 2;
  return dimension;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int GetOffset (
 OHistogram This 
,va_list*  a_args  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int      offset,ibin;
/*.........................................................................*/
  offset = 0;
  for(count=0;count<This->dimension;count++) 
    { int  binn;
      binn = OAxisGetNumberOfBins(This->axis[count]);
      ibin = va_arg((*a_args),int);
           if( ibin >= binn) 
        {
          return NotFound;
        }
      else if( ibin <0)
        {
          return NotFound;
        }
      offset += ibin * This->axisOffset[count];
    }
  return  offset;
}
/***************************************************************************/
static void SetBinNumber (
 OHistogram This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int new_binn;
/*.........................................................................*/
  new_binn = 0;
  for(count=0;count<This->dimension;count++) 
    { int binn;
      OAxisGetAttributesF(This->axis[count],"n",&binn);
      new_binn = count == 0 ? binn : new_binn * binn;
    }
  if(new_binn==0) 
    {
      FreeBins (This);
      return;
    }
  if(new_binn>This->binNumber) 
    {
      CMemoryFreeBlock (This->binSw);
      CMemoryFreeBlock (This->binSw2);
      CMemoryFreeBlock (This->binEntries);
      This->binSw      = NULL;
      This->binSw2     = NULL;
      This->binEntries = NULL;
      This->binSw      = (double*)CMemoryAllocateBlock(new_binn*sizeof(double));
      This->binSw2     = (double*)CMemoryAllocateBlock(new_binn*sizeof(double));
      This->binEntries = (int   *)CMemoryAllocateBlock(new_binn*sizeof(int));
      if( (This->binSw==NULL) || (This->binSw2==NULL) || (This->binEntries==NULL) )
        { 
          This->binSw    = NULL;
          This->binSw2 = NULL;
          This->binEntries = NULL;
          new_binn     = 0;
        }
    }
  This->binNumber    = new_binn;
/* offsets */
  This->axisOffset[0] = 1;
  for(count=1;count<This->dimension;count++) 
    { int binn;
      OAxisGetAttributesF(This->axis[count-1],"n",&binn);
      This->axisOffset[count] = This->axisOffset[count-1] * binn;
    }
  InitializeData  (This);
}
/***************************************************************************/
static void FreeBins (
 OHistogram This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock (This->binSw);
  CMemoryFreeBlock (This->binSw2);
  CMemoryFreeBlock (This->binEntries);
  This->binSw    = NULL;
  This->binSw2 = NULL;
  This->binEntries = NULL;
  This->binNumber   = 0;
}
/***************************************************************************/
static void InitializeData (
 OHistogram This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  for(count=0;count<This->binNumber;count++) 
    {
      This->binSw      [count] = 0.;
      This->binSw2     [count] = 0.;
      This->binEntries [count] = 0;
    }
  This->entries  = 0;
  This->outFlow  = 0;
  This->Sw       = 0.;
  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      This->axisOverFlow   [count] = 0;
      This->axisUnderFlow  [count] = 0;
      This->axisSxw        [count] = 0.;
      This->axisSx2w       [count] = 0.;
    }
}
/***************************************************************************/
static void SetBinWidths (
 OHistogram This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  for(count=0;count<This->dimension;count++) 
    { 
      int        binn;
      binn       = OAxisGetNumberOfBins (This->axis[count]);
      if(binn!=0) 
	{
	  double     min,max;
	  min        = OAxisGetMinimumValue       (This->axis[count]);
	  max        = OAxisGetMaximumValue       (This->axis[count]);
	  This->axisBinWidth  [count] = (max - min)/ binn;
	}
    }
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
  CErrorHandle ("Ho","OHistogram",a_name,a_code);
}
