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

#include <CPrinter.h>

#ifdef HAS_TCL

#include <tcl.h>
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Tcl_Interp* interp; 
/*.........................................................................*/
  if(a_argn<=1)
    {
      CInfo ("\
Examples:\n\
  UNIX> tclT.exe 'puts \"Hello world.\"'\n\
");
      return 0;
    }

  interp           = Tcl_CreateInterp ();  
  Tcl_Eval         (interp,a_args[1]);
  Tcl_DeleteInterp (interp);
  return           0;
}
#else
int main() {CWarn ("C macro HAS_TCL not defined.\n");return 0;}
#endif

