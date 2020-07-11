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
#include <stdlib.h>

#include <OType.h>

#include <OHistogram.h>
#include <CoCommands.h>

#include <HoCommands.h>
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShell     osh;
  int        count;
  OHistogram hist;
/*.........................................................................*/
  osh           = OShellCreate ("osh");
  CoAddCommands (osh);
  HoAddCommands (osh);

  hist          = OHistogramCreate ("mon_histo",1,"x",10,0.,10.);
  OHistogramSetTitle (hist,"Hello");
  for(count=0;count<1000;count++)
    {
      double            value;
      value             = (double)rand();
      value            /= ((double)RAND_MAX);
      value            *= 10.;
      OHistogramFillOneDimensional  (hist,value,1.);
    }
  OHistogramDump          (hist);

  OShellExecute           (osh,"\
load HoT.odb\n\
collect OHistogram where name eq mon_histo | dump -\n\
collect OHistogram where name ne mon_histo | dump -\n\
collect OHistogram | save - x.odb ;cat x.odb;rm x.odb\n\
collect OCut  | dump - script");

  OShellExecute           (osh,"\
load naine.odb;load naine.otu\n\
collect OTuple | dump - name\n\
collect OTuple | dump - title\n\
collect OTuple | dump - labels");


  OTypeClearClass ();

  a_argn = 0;
  a_args = NULL;
  return EXIT_SUCCESS;
}




