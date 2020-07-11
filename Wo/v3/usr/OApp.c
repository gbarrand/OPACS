#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
 
/*Below are the "wanted" packages header files.*/
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
/***************************************************************************/
int main (
 int    a_argn
,char** a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/*Below are the "wanted" packages c files.*/
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

  WoStartup        (a_argn,a_args); /*Initialize Wo, load OApp.odb file.*/
  WoProcessEvents  ();              /*Main loop. It calls WoDispatchEvent, XtDispatchEvent.*/
  WoClearClass     ();             

  return           EXIT_SUCCESS;
}
