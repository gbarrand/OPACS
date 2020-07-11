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
#include <string.h>   

#include <math.h>

#include <CPrinter.h>
#include <CList.h>
#include <CError.h>

#include <CMath.h>

#ifdef __cplusplus
extern "C"{
#endif
static void ReportError (char*,int);
#ifdef __cplusplus
}
#endif

#define DBL_INT       (int)(float)
#define CHECK_DIV(a_var,a_pro) {if((a_var)==0.) {ReportError(a_pro,CErrorValue);(a_var)=1.;}}

static struct {
  double       randomMaximum;
  unsigned int seed;
} Class = {(double)RAND_MAX,1};
/***************************************************************************/
void CMathGetDigitsOfDouble (
 double  a_value  
,int*    a_fdigit 
,int*    a_sdigit 
,int*    a_mag    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double mag;
/*.........................................................................*/
  if(a_value==0.0)
    {
      if(a_fdigit!=NULL) *a_fdigit            = 0;
      if(a_sdigit!=NULL) *a_sdigit            = 0;
      if(a_mag!=NULL)    *a_mag               = 0;
    }
  else 
    {
      mag                = floor(log10(fabs(a_value)));
      if(a_fdigit!=NULL) *a_fdigit = DBL_INT (a_value/pow(10.0,mag)); 
      if(a_sdigit!=NULL) *a_sdigit = DBL_INT ((a_value - (*a_fdigit) * pow(10.0,mag))/pow(10.0,mag-1));
      if(a_mag!=NULL)    *a_mag    = DBL_INT mag;
    }
}
/***************************************************************************/
double CMathGetGauss (
 double a_x 
,double a_height 
,double a_mean  
,double a_sigma 
)
/***************************************************************************/
/*
   For a normalized gaussian take : 
     a_height = 1./ (a_sigma * sqrt(2. * M_PI ))
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double x;
/*.........................................................................*/
  CHECK_DIV  (a_sigma,"CMathGetGauss")
  x       = (a_x-a_mean)/fabs(a_sigma); 
  return  a_height * exp (-0.5 * x * x);
}
/***************************************************************************/
double CMathGetBreitWigner (
 double a_x 
,double a_height   
,double a_center   
,double a_width    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double  x;
/*.........................................................................*/
  CHECK_DIV  (a_width,"CMathGetBreitWigner")
  x       = 2. * (a_x - a_center)/a_width;
  return  a_height/(1. + x * x);
}
/***************************************************************************/
double CMathGetPolynomial (
 double a_x 
,int a_number 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  va_list  args;
  double   value,x;
/*.........................................................................*/
  va_start    (args,a_number);
  value       = 0.;
  x           = 1.;
  for(count=0;count<a_number;count++)
    {
      value += va_arg(args,double) * x;
      x     *= a_x;
    }
  va_end      (args);
  return      value;
}
/***************************************************************************/
double CMathGetArcTangent (
 double a_x
,double a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
       if((a_x==(double)0.) && (a_y==(double)0.))
    {
      ReportError   ("CMathGetArcTangent",CErrorValue);
      return  0.;
    }
  else
    {
      return atan2 (a_x,a_y);
    }
}
/***************************************************************************/
double CMathGetRandomFlat (
)
/***************************************************************************/
/*
  Shot random numbers according a flat distribution.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double value;
/*.........................................................................*/
  value  = (double)rand();
  value /= (double)RAND_MAX;
  return value;
}
/***************************************************************************/
double CMathGetRandomGauss (
)
/***************************************************************************/
/*
  Shot random numbers according a gaussian distribution of mean 0 and sigma 1.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double v1,v2,r,fac;
/*.........................................................................*/
  do {
    v1 = 2.0 * CMathGetRandomFlat() - 1.0;
    v2 = 2.0 * CMathGetRandomFlat() - 1.0;
    r = v1*v1 + v2*v2;
  } while ( r > 1.0 );
  fac = sqrt(-2.0*log(r)/r);
  return v2*fac;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
CList CMathGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static void* list[2] = {&Class,NULL};
/*.........................................................................*/
  return (CList)list;
}
/***************************************************************************/
int CMathSetAttribute (
 void*  This
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

       if(strcmp(a_name,"randomMaximum")==0)       
    {
      double        value;
      value         = *((double*)a_addr);
      if(value==0.) Class.randomMaximum = (double)RAND_MAX;
      else          Class.randomMaximum = value;
    }
  else if(strcmp(a_name,"seed")==0)       
    {
      Class.seed   = *((unsigned int*)a_addr);
      srand        (Class.seed);
    } 
  else if(strcmp(a_name,"random")==0)
    {}  
  else 
    {
      CInfoF ("CMathSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  This     = NULL;       
  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int CMathGetAttribute (
 void* This
,char* a_name
,void* a_user
,void* a_addr 
,int*  a_number
)
/***************************************************************************/
/* 
   This==NULL means we want infos on the class.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"randomMaximum")==0)       
    *((double*)a_addr) = Class.randomMaximum;
  else if(strcmp(a_name,"seed")==0)       
    *((int*)a_addr)    = Class.seed;
  else if(strcmp(a_name,"random")==0)       
    {
      double             value;
      value              = CMathGetRandomFlat();
      value             *= Class.randomMaximum;
      *((double*)a_addr) = value;
    }
  else 
    {
      CInfoF ("CMathGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  This   = NULL;
  a_user = NULL;
  return 1;
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
  CErrorHandle ("Co","CMath",a_name,a_code);
}
