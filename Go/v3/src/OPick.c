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
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif
 
#include <CMemory.h>
#include <CString.h>
#include <CVector.h>

#include <OPickP.h>

/***************************************************************************/
OPick OPickMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPick         This;
/*.........................................................................*/
  This           = (OPickRecord*)CMemoryAllocateBlock(sizeof(OPickRecord));
  if(This==NULL) return NULL;
  This->user     = NULL;
  This->x        = 0.;
  This->y        = 0.;
  This->z        = 0.;
  return         This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OPickDelete (
 OPick This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  CMemoryFreeBlock  (This);
}
/***************************************************************************/
void OPickSetPoint (
 OPick This 
,double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->x = a_x;
  This->y = a_y;
  This->z = a_z;
}
/***************************************************************************/
void OPickGetPoint (
 OPick This 
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL) return;
  if(a_x!=NULL) *a_x = This->x;
  if(a_y!=NULL) *a_y = This->y;
  if(a_z!=NULL) *a_z = This->z;
}
/***************************************************************************/
void OPickSetUserData (
 OPick This 
,void* a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)  return;
  This->user = a_user;
}
/***************************************************************************/
void* OPickGetUserData (
 OPick This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return  This->user;
}
/***************************************************************************/
double OPickGetDistance (
 OPick This 
,OPick a_end   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)  return 0.;
  if(a_end==NULL) return 0.;
  return     CVectorGetDistance (This->x,This->y,This->z,a_end->x,a_end->y,a_end->z);
}
/***************************************************************************/
char* OPickGetInformations (
 OPick This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return CStringDuplicate("");
  return CStringCreateF (1024,
" Loaded point:\n\
  x     (wc)    : %g\n\
  y     (wc)    : %g\n\
  z     (wc)    : %g\n\
  theta (degree): %g\n\
  phi   (degree): %g",This->x,This->y,This->z,
                      CVectorGetPolarAngle (This->x,This->y,This->z) * 180./M_PI,
                      CVectorGetAzimuth   (This->x,This->y,This->z) * 180./M_PI);
}



