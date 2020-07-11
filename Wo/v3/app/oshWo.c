/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*begin Want_h*/
#include <WoWo.h>
#include <WoXtw.h>
#include <WoXm.h>
#include <WoXo.h>
#include <WoHTML.h>
#include <WoXz.h>
#include <WoDeX.h>
#include <WoCi.h>
#include <WoPacksCi.h>
#include <Wotcl.h>
#include <WoHoXz.h>
#include <WoXaw.h>
#include <WoG3o.h>
/*end Want_h*/

#ifdef HAS_G3O
#include <G3oEXPO.h>
#endif /*HAS_G3O*/

/*Co*/
#include <CPrinter.h>
#include <CMemory.h>
#include <CFile.h>
#include <CText.h>
#include <COption.h>
#include <CString.h>
#include <CSystem.h>
#include <CError.h>
#include <OHTML.h>

/*Xx*/
#include <XDisplay.h>
#include <XMailbox.h>
#include <XWidget.h>

/*Wo*/
#include <OEvent.h>
#include <OWidget.h>
#include <Wo.h>

static char what[] = "@(#)oshWo v3";
/***************************************************************************/
int main (
 int a_argn
,char** a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    argn;
  char** args;
/*.........................................................................*/
  what[0] = '@'; /*c++ no warning.*/
/*begin Want_c*/
#include <WoWo.ic>
#include <WoXtw.ic>
#include <WoXm.ic>
#include <WoXo.ic>
#include <WoHTML.ic>
#include <WoXz.ic>
#include <WoDeX.ic>
#include <WoCi.ic>
#include <WoPacksCi.ic>
#include <Wotcl.ic>
#include <WoHoXz.ic>
#include <WoXaw.ic>
#include <WoG3o.ic>
/*end Want_c*/

#ifdef HAS_G3O  
  G3oEXPO_Initialize (WoGetShellInterpreter());  /*Define env for G3o example of a GEANT simulation.*/
#endif /*HAS_G3O*/

  WoSetInterfaceFile ("nil");
  WoStartup          (a_argn,a_args);

  args = WoGetArguments (&argn);
  if(argn==2)  
    {
      OShellExecuteFile ((OShell)WoGetShellInterpreter(),args[1]);
    }
  else
    {
      char                    string[128];
      OProcess                oprocess;
      oprocess                = OProcessCreate ((OShell)WoGetShellInterpreter());
      CPrinterSetPutFunction   (NULL); /*output on stdio.*/
      CPrinterSetPutV_Function (NULL);
      while(1)
	{
	  printf        ("osh> ");
	  gets          (string);
	  if(strcmp(string,"exit")==0) break;
	  OProcessExecute (oprocess,string);
	}
      OProcessDelete (oprocess);
    }

  WoClearClass ();

  return EXIT_SUCCESS;

}




