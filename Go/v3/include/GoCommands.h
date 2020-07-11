/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef GoCommands_h
#define GoCommands_h

#include <OShell.h>
#ifdef __cplusplus
extern "C"{
#endif
void GoAddCommands              (OShell);

int GoExecute_represent         (int,char**,OProcess);
int GoExecute_getCameraNodes    (int,char**,OProcess);
int GoExecute_viewNode          (int,char**,OProcess);
int GoExecute_unviewNode        (int,char**,OProcess);
int GoExecute_isCameraEmpty     (int,char**,OProcess);
int GoExecute_normalizeNode     (int,char**,OProcess);

int GoExecute_rotateCamera      (int,char**,OProcess);
int GoExecute_translateCamera   (int,char**,OProcess);
int GoExecute_scaleCamera       (int,char**,OProcess);
int GoExecute_resetCamera       (int,char**,OProcess);

int GoExecute_doCameraPick      (int,char**,OProcess);
int GoExecute_copyCameraMatrix  (int,char**,OProcess);

int GoExecute_translateNode     (int,char**,OProcess);
int GoExecute_rotateNode        (int,char**,OProcess);
int GoExecute_addPrimitiveToNode(int,char**,OProcess);
int GoExecute_addStringToNode   (int,char**,OProcess);
int GoExecute_addHTML_PageToNode(int,char**,OProcess);
#ifdef __cplusplus
}
#endif

#endif  /*GoCommands_h*/



