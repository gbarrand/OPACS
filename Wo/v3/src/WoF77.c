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

#include <CString.h>
#include <CPrinter.h>
#include <CError.h>
#include <OShell.h>

#include <Wo.h>

#include <WoF77.h>

#ifdef __cplusplus
extern "C"{
#endif
static void ReportError (char*,int);
#ifdef __cplusplus
}
#endif

typedef void(*VoidFunction)();

/******************************************************************************/
void wofile (
 FString a_string
,int* a_lstring
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         cstring;
/*.........................................................................*/
  cstring              = FStringConvertToC   (a_string,*a_lstring);
  WoSetInterfaceFile   (cstring);
  CStringDelete        (cstring);
}
/******************************************************************************/
int worun (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  WoStartup          (0,NULL);
  WoProcessEvents    ();
  WoClearClass       ();
  return             EXIT_SUCCESS;
}
/******************************************************************************/
void wocmd (
 FString a_string
,int* a_lstring
,VoidFunction a_proc
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  cstring;
/*.........................................................................*/
  cstring              = FStringConvertToC   (a_string,*a_lstring);
  OShellAddNewCommand  (WoGetShellInterpreter(),cstring,(OShellExecuteCommandFunction)a_proc);
  CStringDelete        (cstring);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Wo","WoF77",a_name,a_code);
}
