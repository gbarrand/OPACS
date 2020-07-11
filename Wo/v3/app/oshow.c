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
#include <WoFNAL.h>
#include <WoPAW.h> /*Need HAS_XZ and HAS_PAW flags.*/
/*end Want_h*/

#ifdef HAS_G3O
#include <G3oEXPO.h>
#endif /*HAS_G3O*/

#include <CPrinter.h>

static char what[] = "@(#)oshow v3";
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
  if(a_argn<=1)
    {
      CInfo ("Usage:\n\
oshow.exe <odb file>\n\
");
      return EXIT_FAILURE;
    }

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
#include <WoFNAL.ic>
#include <WoPAW.ic>
/*end Want_c*/

#ifdef HAS_G3O  
  G3oEXPO_Initialize (WoGetShellInterpreter());  /*Define env for G3o example of a GEANT simulation.*/
#endif /*HAS_G3O*/

  WoSetInterfaceFile (a_args[1]);
  WoStartup          (a_argn,a_args);
  WoProcessEvents    ();
  WoClearClass       ();

  return             EXIT_SUCCESS;
}
