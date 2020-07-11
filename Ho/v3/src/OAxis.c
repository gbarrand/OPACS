/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <math.h>
#include <stdio.h>
#include <string.h>
 
#include <CMemory.h>
#include <CString.h>
#include <CCharacter.h>
#include <CError.h>
#include <CPrinter.h>
#include <CText.h>

#include <OAxisP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void   AdjustAxis     (double*,double*,int,int);
static double CalculateTicks (double,double*);
static void   ReportError    (char*,int);
#ifdef __cplusplus
}
#endif

#define DBL_INT        (int)(float)
#define MAXIMUM(a,b)   ((a)>(b)?a:b)
/***************************************************************************/
int OAxisVerifyIndex (
 int a_index
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return ((a_index<0) ||(a_index>=OAxisMaximumNumber)) ? 0 :1;
}
/***************************************************************************/
OAxis OAxisMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OAxis This;
/*.........................................................................*/
  This            = (OAxisRecord*)CMemoryAllocateBlock(sizeof(OAxisRecord));
  if(This==NULL)  return NULL;
  OAxisInitialize (This);
  return          This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OAxisDelete (
 OAxis This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OAxisClear       (This);
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void OAxisInitialize (
 OAxis This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  This->minimumValue = 0.;
  This->maximumValue = 0.;
  This->autoscale    = 0;
  This->logscale     = 0;
  This->title        = NULL;
  This->binNumber    = 0;

  This->tickn      = 0;
  This->mag        = 0;
  This->tick_up    = 1;

  for(count=0;count<N__TICKS;count++)
    {
      This->tickValues[count] = 0.;
      sprintf(This->tickLabels[count],"%s","");
    }
}
/***************************************************************************/
void OAxisClear (
 OAxis This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete      (This->title);
  This->title        = NULL;
  This->minimumValue = 0.;
  This->maximumValue = 0.;
  This->autoscale    = 0;
  This->logscale     = 0;
  This->binNumber    = 0;
  This->tickn        = 0;
  This->mag          = 0;
  This->tick_up      = 1;
  for(count=0;count<N__TICKS;count++)
    {
      This->tickValues[count] = 0.;
      sprintf(This->tickLabels[count],"%s","");
    }
}
/***************************************************************************/
void OAxisCopy (
 OAxis This 
,OAxis a_from
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_from==NULL) return;
  if(a_from==This) return;

  OAxisClear         (This);

  This->minimumValue = a_from->minimumValue;
  This->maximumValue = a_from->maximumValue;
  This->autoscale    = a_from->autoscale;
  This->logscale     = a_from->logscale;
  This->binNumber    = a_from->binNumber;

  CStringDelete      (This->title);
  This->title        = CStringDuplicate(a_from->title);

  This->mag          = a_from->mag;
  This->tick_up      = a_from->tick_up;

  This->tickn        = a_from->tickn;
  for(count=0;count<N__TICKS;count++)
    {
      This->tickValues[count] = a_from->tickValues[count];
      strcpy(This->tickLabels[count],a_from->tickLabels[count]);
    }
}
/***************************************************************************/
int OAxisSetAttributesF (
 OAxis This    
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      changed;
/*.........................................................................*/
  va_start    (args,a_format);
  OAxisSetAttributesV    (This,&changed,a_format,args);
  va_end      (args);
  return      changed;
}
/***************************************************************************/
void OAxisGetAttributesF (
 OAxis This    
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
/*.........................................................................*/
  va_start    (args,a_format);
  OAxisGetAttributesV    (This,a_format,args);
  va_end      (args);
}
/***************************************************************************/
void OAxisSetAttributesV (
 OAxis This    
,int* a_changed 
,char* a_format 
,va_list  a_args  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  itema;
/*.........................................................................*/
  if(a_changed!=NULL) *a_changed = 0;
  if(This==NULL)     return;
  if(a_format==NULL) return;
  for(itema=a_format;*itema!='\0';itema++)
    { 
      int    ival;
      double dval;
      int    bval;
      char*  sval;
      double d;
      int    status;
      switch(*itema)
        {
  	  case 'm': dval        = This->minimumValue;
                    This->minimumValue  = va_arg(a_args,double);
                    if( (a_changed!=NULL) && (*a_changed==0)) 
		      *a_changed = This->minimumValue==dval ? 0 : 1;
                    break;
  	  case 'x': dval        = This->maximumValue;
                    This->maximumValue  = va_arg(a_args,double);
                    if( (a_changed!=NULL) && (*a_changed==0)) 
		      *a_changed = This->maximumValue==dval ? 0 : 1;
                    break;
  	  case 'a': bval        = This->autoscale;
                    This->autoscale = va_arg(a_args,int);
                    if( (a_changed!=NULL) && (*a_changed==0)) 
		      *a_changed = This->autoscale==bval ? 0 : 1;
                    break;
  	  case 'l': bval        = This->logscale;
                    This->logscale = va_arg(a_args,int);
                    if( (a_changed!=NULL) && (*a_changed==0)) 
		      *a_changed = This->logscale==bval ? 0 : 1;
                    break;
  	  case 't': 
                    sval          = va_arg(a_args,char*);
                    if( (a_changed!=NULL) && (*a_changed==0)) 
		      *a_changed = CStringCompare(sval,This->title)==1 ? 0 : 1;
                    CStringDelete (This->title);
                    This->title   = CStringDuplicate(sval);
                    break;
  	  case 'n': ival        = This->binNumber;
                    This->binNumber = va_arg(a_args,int);
                    if( (a_changed!=NULL) && (*a_changed==0)) 
		      *a_changed = This->binNumber==ival ? 0 : 1;
                    break;
/* min, max are given by string */
  	  case 'd': dval        = This->minimumValue;
                    sval        = va_arg(a_args,char*);
                    d           = CStringConvertToDouble (sval,&status);
                    This->minimumValue   = status==1 ? d : dval;
                    if( (a_changed!=NULL) && (*a_changed==0)) 
		      *a_changed  = This->minimumValue==dval ? 0 : 1;
                    break;
  	  case 'u': dval        = This->maximumValue;
                    sval        = va_arg(a_args,char*);
                    d           = CStringConvertToDouble(sval,&status);
                    This->maximumValue   = status==1 ? d : dval;
                    if( (a_changed!=NULL) && (*a_changed==0)) 
		      *a_changed  = This->maximumValue==dval ? 0 : 1;
                    break;
          default:
	            ReportError ("OAxisSetAttributesV",CErrorCase);
                    break;
        }
    }
}
/***************************************************************************/
void OAxisGetAttributesV (
 OAxis This    
,char* a_format 
,va_list  a_args  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
/*.........................................................................*/
  if(a_format==NULL) return;
  for(itema=a_format;*itema!='\0';itema++)
    { 
      switch(*itema)
        {
  	  case 'm': *(va_arg(a_args,double *)) = This==NULL ? 0.:This->minimumValue;break;
  	  case 'x': *(va_arg(a_args,double *)) = This==NULL ? 0.:This->maximumValue;break;
  	  case 'a': *(va_arg(a_args,int    *)) = This==NULL ?  1:This->autoscale;break;
  	  case 'l': *(va_arg(a_args,int    *)) = This==NULL ?  0:This->logscale;break;
  	  case 't': *(va_arg(a_args,char  **)) = This==NULL ?  0:This->title;break;
  	  case 'n': *(va_arg(a_args,   int *)) = This==NULL ?  0:This->binNumber;break;
  	  case 'g': *(va_arg(a_args,   int *)) = This==NULL ?  0:This->mag;break;
          default:
                    ReportError ("OAxisGetAttributesV",CErrorCase);
                    break;
        }
    }
}
/***************************************************************************/
int OAxisVerifyNumberOfBins (
 OAxis       This   
,int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  if(This->binNumber<=0) 
    {
      This->binNumber = a_number;
      return 1;
    }
  return 0;
}
/***************************************************************************/
void OAxisVerifyRange (
 OAxis This   
,double a_min 
,double a_max 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->minimumValue<This->maximumValue) return;
  This->minimumValue = a_min;
  This->maximumValue = a_max;
}
/***************************************************************************/
int OAxisAdjust (
 OAxis       This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;

  if((This->minimumValue>=This->maximumValue) || (This->autoscale==1))
    {
      AdjustAxis (&(This->minimumValue),&(This->maximumValue),This->binNumber,This->logscale);
    }

  if((This->logscale==1)&&(This->minimumValue<=0.0)) return 0;
  if((This->logscale==1)&&(This->maximumValue<=0.0)) return 0;

  return 1;
}
/***************************************************************************/
static void AdjustAxis ( 
 double* a_low  
,double* a_high 
,int a_nbins     
,int a_log 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i;
  int axis = 0;
  double step, w, mag, range;
  double mylow, myhigh;
#define N_NICE 4
  static double nice[N_NICE] = { 1.0,2.0,2.5,5.0 };
/*.........................................................................*/
  if (*a_low > *a_high) 
    {
      double  low;
      low     = *a_low;
      *a_low  = *a_high;
      *a_high = low;
    }
  else if (*a_low == *a_high) 
    {
      *a_low = -1.0;
      *a_high = 1.0;
      return;
    }
  
  if (a_nbins <= 0) 
    {
      axis    = 1;
      a_nbins = 10;
    }
  
  /*
   * Round the "bin width" to a nice number.
   * If this is being done for an axis (ie a_nbins was 0), then
   * we don't have to go > *a_high.
   */
  w = (*a_high - *a_low)/((double)a_nbins);
  mag = floor(log10(w));  
  i = 0;
  do
    {
      step   = nice[i] * pow(10.0,mag);

      mylow  = floor(*a_low/step) * step;
      myhigh = axis==1 ? ceil(*a_high/step) * step : mylow + step * a_nbins ;
      
      i++;
      if (i>=N_NICE) 
	{
	  i = 0;
	  mag++;
	}
    }
  while ( ( (axis==1) && myhigh < *a_high) || ( (axis==0) && myhigh <= *a_high) );
  
  range = myhigh - mylow;
  
  /*
   * we now have decided on a range. Try to move a_low/a_high a little
   *  to end up on a nice number.
   *
   * first check if either end is near 0.0
   */
  if ((a_log==0) && (*a_low >= 0.0) && 
      (( (axis==1) && (range>=*a_high) ) || ( (axis==0) && (range>*a_high) )) )
    {
      *a_low = 0.0;
      *a_high = range;
      return;
    }
  if ( (( (axis==1) && (*a_high<=0.0) ) || ( (axis==0) && (*a_high<0.0) )) 
       && (-range<=*a_low))
    {
      *a_high = 0.0;
      *a_low = -range;
      return;
    }
  
  /*
   * try to round *a_low.
   */
  /* correction */
  if( (a_log==1) && (*a_low<=0.0)) *a_low  = 1.0;
  
  i   = N_NICE-1;
  mag = myhigh != 0.0 ? ceil(log10(fabs(myhigh))) : ceil(log10(fabs(mylow)));
  
  do
    {
      step   = nice[i] * pow(10.0,mag);
      
      mylow  = floor(*a_low/step) * step;
      myhigh = mylow + range;
      
      i--;
      if (i<0) 
	{
	  i = N_NICE-1;
	  mag--;
	}
    } 
  while (( (a_log==1) && (mylow  <= 0.0)     ) || 
	 ( (axis==1)  && (myhigh < *a_high)  ) || 
	 ( (axis==0)  && (myhigh <= *a_high) ) );
  
  *a_low  = mylow;
  *a_high = myhigh;
}
/***************************************************************************/
void OAxisComputeTicks (
 OAxis This  
)
/***************************************************************************/
/*
  Controlled by :
    This->minimumValue, This->maximumValue, This->logscale
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double magnitude,y,yr,startTick,tickSize;
  double pmag;
  char   pstr[10] = "";
/* need include <float.h> which does not exist on some system
#define NUM_FUZZ DBL_EPSILON*4
*/
#define NUM_FUZZ 0.01
/*.........................................................................*/
  if(This==NULL) return;

  This->tickn = 0;
  This->mag    = 0;

  if (This->minimumValue >= This->maximumValue) return;

  if (This->logscale==0)
    {
      tickSize  = CalculateTicks (This->maximumValue - This->minimumValue, &magnitude);
      startTick = ceil( This->minimumValue / tickSize) * tickSize;

      if (fabs(magnitude) <= 3.0)
        pmag = 0.0;
      else 
        pmag = startTick != 0.0 ? floor(log10(fabs(startTick))) : magnitude;
	  	       	  
      sprintf(pstr,"%%1.%df",DBL_INT MAXIMUM((pmag-magnitude),0.0));

      y = startTick;
      while (y <= This->maximumValue*(1.0+NUM_FUZZ))
	{
	  if (This->tickn >= N__TICKS)
	    {
  	      ReportError ("OAxisComputeTicks",CErrorValue);
              This->tickn = 0;
              This->mag    = 0;
	      return;
	    }
	       
	  yr = floor(y/pow(10.0,magnitude) + 0.5);
          This->tickValues[This->tickn] = yr * pow(10.0,magnitude);
          sprintf(This->tickLabels[This->tickn],pstr,yr*pow(10.0,magnitude-pmag));
          This->tickn++;
          y += tickSize;
	}
      if (fabs(magnitude) <= 3.0) magnitude = 0.0;
    }
  else
    {
      int    i;
      int    nLogTicks;
      double logTicks[5];
      float  maglow, maghigh, magrng, magStep;

      if (This->minimumValue <=0.) return;

      maghigh =  ceil(log10(This->maximumValue));
      maglow  = floor(log10(This->minimumValue));
      magrng  = maghigh - maglow;
	  
      if (magrng <=3)
	{
	  nLogTicks   = 3;
	  logTicks[0] = 1.0;
	  logTicks[1] = 2.0;
	  logTicks[2] = 5.0;
	  magStep     = 1.0;
	}
      else 
	{
	  nLogTicks   = 1;
	  logTicks[0] = 1.0;
          magStep     = magrng <= 7 ? 1.0 : 2.0;
	}
	       
      pmag = (nLogTicks == 3 && (fabs(maglow)>3 || fabs(maghigh)>3)) ? maglow : 0.0;

      magnitude = maglow;
      i         = 0;
      while ((y=logTicks[i]*pow(10.0,magnitude)) < This->maximumValue*(1.0+NUM_FUZZ))
        {
	  if (This->tickn >= N__TICKS)
	    {
  	      ReportError ("OAxisComputeTicks",CErrorValue);
              This->tickn = 0;
              This->mag    = 0;
	      return;
	    }
	  if (y >= This->minimumValue)
	    {
              This->tickValues[This->tickn] = log10(y);
	      /*
	       * be careful: there is a bug in the NeXT (s)printf 
	       *   routine when you do, eg. printf("%1.0g",0.01);
	      */
	      if ((magnitude-pmag) > 4 || (magnitude-pmag) < -3)
   	        strcpy(pstr,"%1.0e");
	      else
		sprintf(pstr,"%%1.%df",DBL_INT ((magnitude-pmag)>0?0.:-(magnitude-pmag)));
  	      sprintf(This->tickLabels[This->tickn],pstr,y*pow(10.0,-pmag));
  	      This->tickn++;
	    }
	       
	  i++;
	  if (i>=nLogTicks)
	    {
              i = 0;
	      magnitude += magStep;
	    }
	}
     }

  This->mag = DBL_INT pmag;

}
/***************************************************************************/
static double CalculateTicks (
 double     a_size      
,double* a_magnitude 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#define MIN_TICKS 4
  static double goodTicks[] = {10.0, 5.0, 4.0, 2.0, 1.0};
  double tickSize;
  int	 tickIndex;
/*.........................................................................*/
  if (a_size <= 0.0) 
    {
      ReportError ("CalculateTicks",CErrorValue);
      a_size = fabs(a_size);
      if (a_size == 0.0) a_size = 1.0;
    }
     
  *a_magnitude = floor(log10(a_size));
  if (a_size/pow(10.0,*a_magnitude) < MIN_TICKS) (*a_magnitude)--;

  /* now fit the max number of ticks into this range */

  for (tickIndex = 0;
       a_size / (tickSize=goodTicks[tickIndex]*pow(10.0,*a_magnitude) ) < MIN_TICKS;
       tickIndex++);

  if (tickIndex == 0) (*a_magnitude)++;
     
  return tickSize;
}
/***************************************************************************/
int OAxisGetNumberOfBins (
 OAxis This    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->binNumber;
}
/***************************************************************************/
double OAxisGetMinimumValue (
 OAxis This    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return This->minimumValue;
}
/***************************************************************************/
double OAxisGetMaximumValue (
 OAxis This    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  return This->maximumValue;
}
/***************************************************************************/
int OAxisGetNumberOfTicks (
 OAxis This    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->tickn;
}
/***************************************************************************/
double OAxisGetIthTickValue (
 OAxis This    
,int a_count   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  if((a_count<0)||(a_count>=This->tickn)) return 0.;
  return This->tickValues[a_count];
}
/***************************************************************************/
char* OAxisGetIthTickLabel (
 OAxis This    
,int a_count   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  if((a_count<0)||(a_count>=This->tickn)) return NULL;
  return This->tickLabels[a_count];
}
/***************************************************************************/
char* OAxisGetTitle (
 OAxis This    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->title;
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
  CErrorHandle ("Ho","OAxis",a_name,a_code);
}
/***************************************************************************/
void OAxisSetAttributes (
 OAxis  This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char**   words;
  int      wordn;
/*.........................................................................*/
  if(This==NULL) return;
  words    = CStringGetWords (a_string," ",&wordn);
  for(count=0;count<wordn;count++) 
    {
      if(words[count]!=NULL)
	{
	       if( (strcmp(words[count],"min")==0) && (count+1<wordn))                
	    {
	      double value;
              int    status;
	      value  = CStringConvertToDouble(words[count+1],&status);
	      if(status==0) 
		{
		  CWarnF("In:\n%s\n %s not a number.\n",a_string,words[count+1]);
		  break;
		}
              This->minimumValue = value;
	      count++;
	    }
	  else if( (strcmp(words[count],"max")==0) && (count+1<wordn))                
	    {
	      double value;
              int    status;
	      value  = CStringConvertToDouble(words[count+1],&status);
	      if(status==0) 
		{
		  CWarnF("In:\n%s\n %s not a number.\n",a_string,words[count+1]);
		  break;
		}
              This->maximumValue = value;
	      count++;
	    }
	  else if( (strcmp(words[count],"auto")==0) && (count+1<wordn))                
	    {
	      int    value;
              int    status;
	      value  = CStringConvertToBoolean(words[count+1],&status);
	      if(status==0) 
		{
		  CWarnF("In:\n%s\n %s not a boolean.\n",a_string,words[count+1]);
		  break;
		}
              This->autoscale = value;
	      count++;
	    }
	  else if( (strcmp(words[count],"log")==0) && (count+1<wordn))                
	    {
	      int    value;
              int    status;
	      value  = CStringConvertToBoolean(words[count+1],&status);
	      if(status==0) 
		{
		  CWarnF("In:\n%s\n %s not a boolean.\n",a_string,words[count+1]);
		  break;
		}
              This->logscale = value;
	      count++;
	    }
	  else if( (strcmp(words[count],"title")==0) && (count+1<wordn))                
	    {
	      CStringDelete (This->title);
	      This->title   = CStringDuplicate(words[count+1]);
	      count++;
	    }
	  else if( (strcmp(words[count],"nbin")==0) && (count+1<wordn))                
	    {
	      int    value;
              int    status;
	      value  = (int)CStringConvertToLong(words[count+1],&status);
	      if(status==0) 
		{
		  CWarnF("In:\n%s\n %s not a number.\n",a_string,words[count+1]);
		  break;
		}
              This->binNumber = value;
	      count++;
	    }
	}
   
    } 
  CTextDelete (wordn,words);
}
