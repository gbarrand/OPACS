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
#include <WoPAW.h> 
/*Need HAS_XZ and HAS_PAW flags.*/
#include <WoJAVA.h> 
/*end Want_h*/

#ifdef __cplusplus
extern "C"{
#endif
#include <WoStubs.h>
#ifdef __cplusplus
}
#endif

typedef struct HWo*               WoStub;
typedef struct Hjava_lang_String* JString;
/***************************************************************************/
void Wo_Startup (
 WoStub a_stub
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
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
#include <WoJAVA.ic>
/*end Want_c*/
  WoStartup (0,NULL);
  a_stub    = NULL;
}
/***************************************************************************/
void Wo_ProcessEvents (
 WoStub a_stub
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  WoProcessEvents ();
  a_stub          = NULL;
}
/***************************************************************************/
void Wo_ClearClass (
 WoStub a_stub
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  WoClearClass ();
  a_stub       = NULL;
}
/***************************************************************************/
void Wo_SetInterfaceFile (
 WoStub  a_stub
,JString a_fileName
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*              string = NULL;
/*.........................................................................*/
  string             = allocCString(a_fileName);
  WoSetInterfaceFile (string);
  if(string!=NULL)   free(string);
  a_stub             = NULL;
  a_fileName         = NULL;
}
