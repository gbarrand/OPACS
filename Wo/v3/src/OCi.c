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
#include <string.h>

#include <Ci.h>

/*Co*/
#include <CPrinter.h>
#include <CFile.h>
#include <CString.h>
#include <CText.h>
#include <CMemory.h>
#include <CSystem.h>

/*Wo*/
#include <OCi.h>

/***************************************************************************/
void OCiSetSTD_Print (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CiSetPrintFunction ((CiRoutine)vprintf);
}
/***************************************************************************/
void OCiClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CiSetPrintFunction ((CiRoutine)vprintf);
  CiParserCleanup    (); /*Ci used in OCyclic*/
}
/***************************************************************************/
void OCiExecuteScript (
 char* a_string   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  if(CSystemIsSecured()==1)
    {
      if(
	 (CStringHasWord(a_string,"system")==1) ||
	 (CStringHasWord(a_string,"remove")==1) ||
	 (CStringHasWord(a_string,"rename")==1) 
        ) 
	{
	  CWarnF ("Use of \"system, remove, rename,...\" routines refused in script:\n%s\n",a_string);
	  return;
	}
    }
  CiDo      (a_string);
}
/***************************************************************************/
void OCiExecuteFile (
 char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    linen = 0;
  char** lines = NULL;
  char*  lineb = NULL;
  char*  string = NULL;
/*.........................................................................*/
  lineb            = CFileLoadStrings (a_fname,'\0',&linen,&lines);
  string           = CTextConvertToString (linen,lines,"\n");
  OCiExecuteScript (string);
  CStringDelete    (string);
  CMemoryFreeBlock (lines);
  CMemoryFreeBlock (lineb);
}
/***************************************************************************/
void OCiVerifySyntax (
 char* a_string   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  CiStatementDelete (CiTextCompile(a_string));
}
/***************************************************************************/
void OCiGenerateCallback (
 char* a_string   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return;
/*CFilePrintSF ("\
{\n\
%s\n\
 a_widget = 0;\n\
 a_tag    = 0;\n\
 a_reason = 0;\n\
}\n",a_string);
*/
}

