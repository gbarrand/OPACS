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
#include <string.h>
#include <stdlib.h>

#include <CMath.h>
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE* file;
  int   number = 1000;
  int   count;
/*.........................................................................*/
  if(a_argn==2) {
    if(sscanf(a_args[1],"%d",&number)!=1) {
      printf ("%s not a number.\n",a_args[1]);
      return EXIT_FAILURE;
    }
  }

  file = fopen("gauss.otu","w");
  if(file==NULL) return;

  fprintf           (file,"\"gauss\"\n");
  fprintf           (file,"\"x\" \"y\"\n");

  for(count=0;count<number;count++) {
    double            xval,yval,weight;
    xval              = CMathGetRandomGauss();
    yval              = CMathGetRandomGauss();
    weight            = 1.;
    fprintf           (file,"%g %g\n",xval,yval);
  }

  fclose(file);

  a_argn = 0;
  a_args = NULL;
  return EXIT_SUCCESS;
}

