/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef XzBind_h
#define XzBind_h

#ifdef __cplusplus
extern "C"{
#endif
CiRoutine CiBindHBook(char*);
CiRoutine CiBindHF77(char*);
CiRoutine CiBindHFile(char*);
CiRoutine CiBindHPlot(char*);
CiRoutine CiBindKuip(char*);
CiRoutine CiBindXzPlotter(char*);
CiRoutine CiBindXzTypes(char*);
#ifdef __cplusplus
}
#endif


#endif  /*XzBind_h*/
