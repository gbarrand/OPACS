/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#if defined(HAS_HO) && defined(HAS_XZ)

#include <stdlib.h>

/*Co*/
#include <CPrinter.h>
#include <CList.h>
#include <CText.h>
#include <OShell.h>
#include <OType.h>

/*Ho*/
#include <OHistogram.h>

/*Wo*/
#include <HoXz.h>

#include <HoXzCommands.h>

/***************************************************************************/
void HoXzAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("saveInHBOOK_File")!=NULL) return; /*done*/

  OShellAddNewCommand (a_osh,"Wo/saveInHBOOK_File",HoXzExecute_saveInHBOOK_File);

}
/***************************************************************************/
int HoXzExecute_saveInHBOOK_File (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> saveInHBOOK_File - <string:(file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram* histograms = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }
 
  histograms = (OHistogram*) OProcessGetTypedObjects  (a_process,a_args[1],"OHistogram");

  HoXzProduceRZ_File  (a_args[2],(CList)histograms,1);

  CListDelete         ((CList)histograms);

  OProcessPutHandles (a_process,NULL);

  return              EXIT_SUCCESS;
}

#else /*HAS_HO && HAS_XZ*/
#include <stddef.h>
#endif /*HAS_HO && HAS_XZ*/
