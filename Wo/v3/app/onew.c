/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>
#include <string.h>

/*begin Want_h*/
#include <WoWo.h>
/*end Want_h*/

#include <CPrinter.h>

static char what[] = "@(#)onew v3";
/***************************************************************************/
int main (
 int a_argn
,char** a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  what[0] = '@'; /*c++ no warning.*/
  if((a_argn!=2) && (a_argn!=3))
    {
      CWarn ("\
 One or two arguments expected :\n\
  UNIX> onew <app name> [<template dir name>]\n\
 Examples :\n\
  UNIX> onew Xxx\n\
to build a little application on Motif.\n\
  UNIX> onew Xxx $WOROOT/usr/template/Xaw/\n\
to build a little application on Xaw.\n\
  UNIX> onew Xxx $WOROOT/usr/template/Example/\n\
to build a little application on Motif with graphic examples.\n\
");
      return EXIT_FAILURE;
    }


/*begin Want_c*/
#include <WoWo.ic>
/*end Want_c*/

  if(a_argn==2) OShellExecuteF (WoGetShellInterpreter(),
                                22+strlen(a_args[1]),
                                "do Wo makeApplication %s",
				a_args[1]);
  else          OShellExecuteF (WoGetShellInterpreter(),
                                23+strlen(a_args[1])+strlen(a_args[2]),
                                "do Wo makeApplication %s %s",
				a_args[1],a_args[2]);
  WoClearClass  ();

  return        EXIT_SUCCESS;
}
