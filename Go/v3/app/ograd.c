/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <stdlib.h>

#include <CColor.h>
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double   dbin;
   int count;
#define GRAD_NCOLOR 10
#define GRAD_MIN    240.  /* hue of blue    */
#define GRAD_MAX    360.  /* hue of red     */
/*.........................................................................*/
  dbin      = (GRAD_MAX - GRAD_MIN) / GRAD_NCOLOR;
  for(count=0;count<GRAD_NCOLOR;count++)
    {  
      double          h,s,b;
      h               = GRAD_MIN + dbin * count;
      s               = 1.;
      b               = 1.;
      CColorConvertHSB_ToRGB  (&h,&s,&b);
      printf          ("%g %g %g\\\n",h,s,b);
    }
  a_argn = 0;
  a_args = NULL;
  return EXIT_SUCCESS;
}
