/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OCi_h
#define OCi_h
 
#ifdef __cplusplus
extern "C"{
#endif
void    OCiClearClass       ();
void    OCiExecuteScript    (char*);
void    OCiExecuteFile      (char*);
void    OCiVerifySyntax     (char*);
void    OCiGenerateCallback (char*);
void    OCiSetSTD_Print     ();
#ifdef __cplusplus
}
#endif

#endif  /*OCi_h*/
