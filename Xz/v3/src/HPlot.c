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

#include <FString.h>

#include <HF77.h>

#include <HPlot.h>
/***************************************************************************/
void HPlotDrawHistogram (
 int a_id
,char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           id;
  int           lopt;
  FString       fopt;
/*.........................................................................*/
  id            = a_id;
  fopt          = FStringCreateFromC(a_opt,&lopt);
  ohplot        (&id,fopt,&lopt);
  FStringDelete (fopt);
}
/***************************************************************************/
void HPlotSetParameter (
 char* a_opt
,double a_val
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lopt;
  FString    fopt;
  float      val;
/*.........................................................................*/
  fopt        = FStringCreateFromC(a_opt,&lopt);
  val         = a_val;
  ohpset      (fopt,&lopt,&val);
  FStringDelete (fopt);
}
/***************************************************************************/
void HPlotSetOption (
 char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           lopt;
  FString       fopt;
/*.........................................................................*/
  fopt          = FStringCreateFromC(a_opt,&lopt);
  ohpopt        (fopt,&lopt);
  FStringDelete (fopt);
}
/***************************************************************************/
void HPlotSetTitle (
 char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           lopt;
  FString       fopt;
/*.........................................................................*/
  fopt          = FStringCreateFromC(a_opt,&lopt);
  ohptit        (fopt,&lopt);
  FStringDelete (fopt);
}
/***************************************************************************/
void HPlotSetZones (
 int a_nx
,int a_ny
,int a_first
,char* a_opt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           nx,ny,first;
  int           lopt;
  FString       fopt;
/*.........................................................................*/
  nx            = a_nx;
  ny            = a_ny;
  first         = a_first;
  fopt          = FStringCreateFromC(a_opt,&lopt);
  ohpzon        (&nx,&ny,&first,fopt,&lopt);
  FStringDelete (fopt);
}
/***************************************************************************/
void HPlotGetPictureSize (
 double* a_nx
,double* a_ny
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  float         xsiz,ysiz;
/*.........................................................................*/
  ohpsiz        (&xsiz,&ysiz);
  if(a_nx!=NULL) *a_nx = (double)xsiz;
  if(a_ny!=NULL) *a_ny = (double)ysiz;
}
/***************************************************************************/
void HPlotSetAxesTitle (
 char* a_x
,char* a_y
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int           lx;
  FString       fx;
  int           ly;
  FString       fy;
/*.........................................................................*/
  fx            = FStringCreateFromC(a_x,&lx);
  fy            = FStringCreateFromC(a_y,&ly);
  ohpax         (fx,&lx,fy,&ly);
  FStringDelete (fx);
  FStringDelete (fy);
}
