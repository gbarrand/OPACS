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
#include <stddef.h>

#include <CPrinter.h>
#include <CMath.h>
#include <CError.h>

#include <CVector.h>

#ifdef __cplusplus
extern "C"{
#endif
static void ReportError (char*,int);
#ifdef __cplusplus
}
#endif

/***************************************************************************/
void CVectorDump (
 double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CDumpF ("%g %g %g\n",a_x,a_y,a_z);
}
/***************************************************************************/
double CVectorGetNorme (
 double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return sqrt(a_x * a_x + a_y * a_y + a_z * a_z);
}
/***************************************************************************/
double CVectorNormalize (
 double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double length;
/*.........................................................................*/
  if( (a_x==NULL) || (a_y==NULL) || (a_z==NULL) ) return 0.;
  length         = CVectorGetNorme (*a_x,*a_y,*a_z);
  if(length==(double)0.) {ReportError ("CVectorNormalize",CErrorValue);length=1.;}
  *a_x   = (*a_x) / length;
  *a_y   = (*a_y) / length;
  *a_z   = (*a_z) / length;
  return length;
}
/***************************************************************************/
double CVectorGetDotProduct (
 double a_1_x
,double a_1_y
,double a_1_z
,double a_2_x
,double a_2_y
,double a_2_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (a_1_x * a_2_x + a_1_y * a_2_y + a_1_z * a_2_z);
}
/***************************************************************************/
double CVectorGetCosine (
 double a_1_x
,double a_1_y
,double a_1_z
,double a_2_x
,double a_2_y
,double a_2_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double norme_1,norme_2,dot;
/*.........................................................................*/
  norme_1 = CVectorGetNorme (a_1_x,a_1_y,a_1_z);
  if(norme_1==(double)0.) {ReportError ("CVectorGetCosine",CErrorValue);norme_1=1.;}
  norme_2 = CVectorGetNorme (a_2_x,a_2_y,a_2_z);
  if(norme_2==(double)0.) {ReportError ("CVectorGetCosine",CErrorValue);norme_2=1.;}
  dot     = CVectorGetDotProduct   (a_1_x,a_1_y,a_1_z,a_2_x,a_2_y,a_2_z);
  return  dot/(norme_1 * norme_2);
}
/***************************************************************************/
double CVectorGetDistance (
 double a_1_x
,double a_1_y
,double a_1_z
,double a_2_x
,double a_2_y
,double a_2_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double x,y,z;
/*.........................................................................*/
  x = a_2_x - a_1_x;
  y = a_2_y - a_1_y;
  z = a_2_z - a_1_z;
  return sqrt(x * x + y * y + z * z);
}
/***************************************************************************/
void CVectorMultiply (
 double a_1_x
,double a_1_y
,double a_1_z
,double a_2_x
,double a_2_y
,double a_2_z
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = a_1_y * a_2_z - a_1_z * a_2_y;
  if(a_y!=NULL) *a_y = a_1_z * a_2_x - a_1_x * a_2_z;
  if(a_z!=NULL) *a_z = a_1_x * a_2_y - a_1_y * a_2_x;
}
/***************************************************************************/
double CVectorGetPolarAngle (
 double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return   CMathGetArcTangent (sqrt(a_x * a_x + a_y * a_y),a_z) ;
}
/***************************************************************************/
double CVectorGetAzimuth (
 double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_z = 0.;
  return CMathGetArcTangent (a_y,a_x);
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
  CErrorHandle ("Co","CVector",a_name,a_code);
}
