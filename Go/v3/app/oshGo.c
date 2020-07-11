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
#include <stdio.h>
#include <string.h>

#include <CPrinter.h>
#include <OType.h>
#include <CoCommands.h>

#include <GoCommands.h>
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShell  osh;
/*.........................................................................*/
  osh           = OShellCreate ("osh");
  CoAddCommands (osh);
  GoAddCommands (osh);

  if(a_argn==2)  
    {
      OShellExecuteFile (osh,a_args[1]);
    }
  else
    {
      char      string[128];
      OProcess  oprocess;
      oprocess  = OProcessCreate (osh);
      while(1)
	{
	  printf        ("osh> ");
	  gets          (string);
	  if(strcmp(string,"exit")==0) break;
	  OProcessExecute (oprocess,string);
	}
      OProcessDelete (oprocess);
    }

  OTypeClearClass ();

  a_argn = 0;
  a_args = NULL;
  return EXIT_SUCCESS;
}
