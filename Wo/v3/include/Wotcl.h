/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef Wotcl_h
#define Wotcl_h

#ifdef HAS_TCL

#include <OInterface.h>
#include <OInterpreter.h>

#include <tcl.h>

#ifdef __cplusplus
extern "C"{
#endif
static void ExecuteTclScript (char*);
#ifdef __cplusplus
}
#endif

/***************************************************************************/
static void ExecuteTclScript (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static Tcl_Interp* interp = NULL; 
/*.........................................................................*/
  if(interp==NULL)
    {
      interp      = Tcl_CreateInterp ();   /**/
      if(interp==NULL) return;
    }
  Tcl_Eval         (interp,a_string); /**/
}

#endif /*HAS_TCL*/

#endif /*Wotcl_h*/
