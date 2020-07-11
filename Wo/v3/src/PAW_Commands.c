/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifdef HAS_PAW

#include <stdlib.h>

/*Co*/
#include <CPrinter.h>

/*Xz*/
#include <Kuip.h>

#include <PAW_Commands.h>

#ifndef HAS_NOT_U
#define opawc  opawc_
#define paw    paw_
#define kwodef kwodef_
#endif

#ifdef __cplusplus
extern "C"{
#endif
void opawc  (int*,int*);
void paw    (int*,int*);
void kwodef ();
#ifdef __cplusplus
}
#endif

/***************************************************************************/
void PAW_AddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("PAW")!=NULL) return; /*done*/

  OShellAddNewCommand  (a_osh,"Wo/PAW",PAW_Execute_PAW);

}
/***************************************************************************/
int PAW_Execute_PAW (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  Initialize PAW.
  Example :
     osh> PAW
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int NPAW,NKUIP,IWTYP;
/*.........................................................................*/
  if(okited()==1)
    {
      CWarn   (" Sorry, can't execute the PAW command (and then the paw routine) \nbecause KUIP is already initialized.\n");
      return  EXIT_FAILURE;
    }

/* Routine paw do an HLIMIT and a KUINIT without checking that it has already been done ! 
   Then KuipInitialize or HBookInitialize must be done AFTER calling paw routine. */

  opawc   (&NPAW,&NKUIP);
  paw     (&NPAW,&IWTYP);

  kwodef  ();
	  
  return  EXIT_SUCCESS;
}

#else /*HAS_PAW*/
#include <stddef.h>
#endif /*HAS_PAW*/
