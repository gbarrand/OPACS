/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoBind_h
#define WoBind_h

#ifdef __cplusplus
extern "C"{
#endif
CiRoutine CiBindOClass(char*);
CiRoutine CiBindOCyclic(char*);
CiRoutine CiBindOEvent(char*);
CiRoutine CiBindOInterface(char*);
CiRoutine CiBindOInterpreter(char*);
CiRoutine CiBindOPart(char*);
CiRoutine CiBindOPiece(char*);
CiRoutine CiBindOResource(char*);
CiRoutine CiBindOTrace(char*);
CiRoutine CiBindOWidget(char*);
CiRoutine CiBindWo(char*);
CiRoutine CiBindWoTypes(char*);
#ifdef __cplusplus
}
#endif


#endif  /*WoBind_h*/
