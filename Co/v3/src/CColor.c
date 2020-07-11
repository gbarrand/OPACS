/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stddef.h>
#include <math.h>

#include <CError.h>

#ifdef __cplusplus
extern "C"{
#endif
static void ReportError (char*,int);
#ifdef __cplusplus
}
#endif

#include <CColor.h>

#define DBL_INT       (int)(float)
/***************************************************************************/
void CColorConvertHSB_ToRGB (
 double* a_1
,double* a_2
,double* a_3
)
/***************************************************************************/
/* h in [0.,360.]                                                          */
/* s in [0.,1.]                                                            */
/* b in [0.,1.]                                                            */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double h,s,b,f,p,q,t;
  int    i; 
/*.........................................................................*/
  if( (a_1==NULL) || (a_2==NULL) || (a_3==NULL) ) return;
  h = *a_1;
  s = *a_2;
  b = *a_3;
  if(s==0.)
    {
      if(h==999.)
        {
          *a_1  = b;
          *a_2  = b;
          *a_3  = b;
          return;
        }
      else
        {
          ReportError ("CColorConvertHSB_ToRGB",CErrorValue);
          return;
        }
    }
  else
    {
      if(h==360.) h = 0.;
      h /= 60.;
      i = DBL_INT floor(h);
      f = h-i;
      p = b * (1.-s);
      q = b * (1.-s*f);
      t = b * (1.-s*(1.-f));
      switch(i)
        {
     	  case 0: 
                 *a_1  = b;
		 *a_2  = t;
		 *a_3  = p;
		 return;
     	  case 1:
                 *a_1  = q;
		 *a_2  = b;
		 *a_3  = p;
		 return;
     	  case 2:
                 *a_1  = p;
		 *a_2  = b;
		 *a_3  = t;
		 return;
     	  case 3:
                 *a_1  = p;
		 *a_2  = q;
		 *a_3  = b;
		 return;
     	  case 4:
                 *a_1  = t;
		 *a_2  = p;
		 *a_3  = b;
		 return;
     	  case 5:
                 *a_1  = b;
		 *a_2  = p;
		 *a_3  = q;
		 return;
     	  default: return;
        }
    }
}
/***************************************************************************/
double CColorConvertRGB_ToGrey (
 double a_red
,double a_green
,double a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (0.30 * a_red + 0.59 * a_green + 0.11 * a_blue);
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
  CErrorHandle ("Co","CColor",a_name,a_code);
}
