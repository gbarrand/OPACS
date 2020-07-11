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

/*Co*/
#include <CPrinter.h>
#include <OShell.h>

/*Wo*/
#include <OCi.h>

#include <CiCommands.h>
/***************************************************************************/
void CiAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("doCiScript")!=NULL) return; /*done*/

  OShellAddNewCommand (a_osh,"Wo/doCiScript"         ,CiExecute_doCiScript);
  OShellAddNewCommand (a_osh,"Wo/doCiFile"           ,CiExecute_doCiFile);
  OShellAddNewCommand (a_osh,"Wo/parseCiScript"      ,CiExecute_parseCiScript);
}
/***************************************************************************/
int CiExecute_doCiScript (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> doCiScript <string:script>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  OCiExecuteScript   (a_args[1]);
  return EXIT_SUCCESS;
}
/***************************************************************************/
int CiExecute_doCiFile (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> doCiFile <string:file>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  OCiExecuteFile (a_args[1]);
  return EXIT_SUCCESS;
}
/***************************************************************************/
int CiExecute_parseCiScript (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> parseCiScript <string:script>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  OCiVerifySyntax (a_args[1]);
  return EXIT_SUCCESS;
}
