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

#include <CString.h>
#include <OHistogram.h>
#include <OScatter.h>

#include <HoF77.h>

static struct 
{
  OHistogram ohistogram;
  OScatter   oscatter;
} Class = {NULL,NULL};
/***************************************************************************/
void HoF77_ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.ohistogram = NULL;
  Class.oscatter   = NULL;
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void ohcurr (
 FString a_name
,int* a_lname
)
/******************************************************************************/
/*
  Set current OHistogram.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         cname;
/*.........................................................................*/
  cname            = FStringConvertToC (a_name,*a_lname);
  Class.ohistogram = OHistogramGetIdentifier (cname);
  CStringDelete    (cname);
}
/******************************************************************************/
void oh1cre (
 FString a_name
,int* a_lname
,FString a_xt
,int* a_lxt
,int* a_xn
,float* a_xm
,float* a_xx
)
/******************************************************************************/
/*
  Create a one dimensional OHistogram.
  Example :
      CALL OH1CRE('hist_1',6,'X',1,20,0.,20.)     
      DO 10 I=1,1000
        X = 20. * CRANDO()
        CALL OH1FIL(X,1.)
10    CONTINUE
      CALL OHDUMP()
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         cname;
  char*         cxt;
/*.........................................................................*/
  cname            = FStringConvertToC (a_name,*a_lname);
  cxt              = FStringConvertToC (a_xt,*a_lxt);
  Class.ohistogram = OHistogramCreate  (cname,1,cxt,*a_xn,(double)(*a_xm),(double)(*a_xx));
  CStringDelete    (cname);
  CStringDelete    (cxt);
}
/******************************************************************************/
void oh1fil (
 float* a_x
,float* a_w
)
/******************************************************************************/
/*
  Fill current one dimensional OHistogram.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OHistogramFillOneDimensional  (Class.ohistogram,(double)(*a_x),(double)(*a_w));
}
/******************************************************************************/
void ohdump (
)
/******************************************************************************/
/*
  Dump current OHistogram.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OHistogramDump (Class.ohistogram);
}
/******************************************************************************/
void ohdele (
)
/******************************************************************************/
/*
  Delete current OHistogram.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OHistogramDelete (Class.ohistogram);
  Class.ohistogram = NULL;
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void oscurr (
 FString a_name
,int* a_lname
)
/******************************************************************************/
/*
  Set current OScatter.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         cname;
/*.........................................................................*/
  cname            = FStringConvertToC (a_name,*a_lname);
  Class.oscatter = OScatterGetIdentifier (cname);
  CStringDelete    (cname);
}
/******************************************************************************/
void os2cre (
 FString a_name
,int* a_lname
,FString a_xt
,int* a_lxt
,float* a_xm
,float* a_xx
,FString a_yt
,int* a_lyt
,float* a_ym
,float* a_yx
)
/******************************************************************************/
/*
  Create a two dimensional OScatter.
  Example :
      CALL OS2CRE('scat',4,'X',1,0.,20.,
     +                     'Y',1,0.,1.)     
      CALL OSALLO(100)
      DO 10 I=1,100
        X = 20. * CRANDO()
        Y = CRANDO()
        CALL OS2FIL(X,Y)
10    CONTINUE
      CALL OSDUMP()
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         cname;
  char*         cxt;
  char*         cyt;
/*.........................................................................*/
  cname            = FStringConvertToC (a_name,*a_lname);
  cxt              = FStringConvertToC (a_xt,*a_lxt);
  cyt              = FStringConvertToC (a_yt,*a_lyt);
  Class.oscatter   = OScatterCreate  (cname,2,
				      cxt,(double)(*a_xm),(double)(*a_xx),
				      cyt,(double)(*a_ym),(double)(*a_yx));
  CStringDelete    (cname);
  CStringDelete    (cxt);
  CStringDelete    (cyt);
}
/******************************************************************************/
void osdump (
)
/******************************************************************************/
/*
  Dump current OScatter.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OScatterDump (Class.oscatter);
}
/******************************************************************************/
void osdele (
)
/******************************************************************************/
/*
  Delete current OScatter.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OScatterDelete (Class.oscatter);
  Class.oscatter = NULL;
}
/******************************************************************************/
void osallo (
 int* a_number
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OScatterAllocatePoints (Class.oscatter,*a_number);
}
/******************************************************************************/
void os2fil (
 float* a_x
,float* a_y
)
/******************************************************************************/
/*
  Fill current two dimensional OScatter.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OScatterFillTwoDimensional  (Class.oscatter,(double)(*a_x),(double)(*a_y));
}




