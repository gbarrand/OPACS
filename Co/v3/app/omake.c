/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <CPrinter.h>
#include <OType.h>

#include <CManager.h>

static char what[] = "@(#)omake v3";
/******************************************************************************/
int main (
 int a_argn 
,char** a_args 
)
/******************************************************************************/
{
/*.........................................................................*/
  what[0] = '@'; /*c++ no warning.*/
  if( (a_argn>=2) && (strcmp(a_args[1],"-all")==0) )
    {
      CManagerProduceMakefile     ("Make.odb",a_argn,a_args);
      CManagerProduceNMakefile    ("Make.odb",a_argn,a_args);
      CManagerProduceMMS_File     ("Make.odb",a_argn,a_args);
    }
  else if( (a_argn>=2) && (strcmp(a_args[1],"-nmake")==0) )
    {
      CManagerProduceNMakefile    ("Make.odb",a_argn,a_args);
    }
  else if( (a_argn>=2) && (strcmp(a_args[1],"-make")==0) )
    {
      CManagerProduceMakefile     ("Make.odb",a_argn,a_args);
    }
  else if( (a_argn>=2) && (strcmp(a_args[1],"-mms")==0) )
    {
      CManagerProduceMMS_File     ("Make.odb",a_argn,a_args);
    }
  else
    {
      CInfo ("\
At least one argument required :\n\
  -make\n\
    To build UNIX make file : This.mk.\n\
  -nmake\n\
    To build NT make file : This.nmk.\n\
  -mms\n\
    To build VMS make file : This.mms.\n\
  -all\n\
    To build This.mk, This.nmk, This.mms files.\n\
");
      return EXIT_SUCCESS;
    }

  OTypeClearClass             ();
  return                      EXIT_SUCCESS;
}
