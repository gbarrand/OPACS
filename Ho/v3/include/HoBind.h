/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef HoBind_h
#define HoBind_h

#ifdef __cplusplus
extern "C"{
#endif
CiRoutine CiBindHo(char*);
CiRoutine CiBindHoTypes(char*);
CiRoutine CiBindOAxis(char*);
CiRoutine CiBindOCut(char*);
CiRoutine CiBindOFit(char*);
CiRoutine CiBindOFunction(char*);
CiRoutine CiBindOHistogram(char*);
CiRoutine CiBindOScatter(char*);
CiRoutine CiBindOStatement(char*);
CiRoutine CiBindOTuple(char*);
#ifdef __cplusplus
}
#endif


#endif  /*HoBind_h*/
