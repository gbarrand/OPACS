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

#include <CMemory.h>
#include <CPrinter.h>
#include <CFile.h>

#include <CFITS.h>

/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*         file;
  char*         string;
/*.........................................................................*/
  if(a_argn<=1)
    {
      CInfo ("\
Usage:\n\
CFITST.exe <fits file>\n\
");
      return EXIT_SUCCESS;
    }

  file              = CFileOpenForReading  (a_args[1],NULL);
  if(file!=NULL)
    {
      CFITS_DumpHeaderInStream   (file);
      fclose            (file);
    }

  CFileClearClass   ();

  string            = CMemoryGetStatus();
  if((string!=NULL) && (*string!='\0')) CInfoF ("%s\n",string);

  return            EXIT_SUCCESS;
}

