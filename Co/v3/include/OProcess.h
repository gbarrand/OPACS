/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OProcess_h
#define OProcess_h

typedef struct _OProcessRecord *OProcess;

#define OProcessExit  -1   /*Take it negative; EXIT_FAILURE is large positive on some system (VMS).*/

#include <CList.h>
#include <OHandle.h>
#ifdef __cplusplus
extern "C"{
#endif
char**    OProcessGetArguments                  (int*);
OProcess  OProcessCreate                        (void*);
void      OProcessDelete                        (OProcess);
int       OProcessExecuteLines                  (OProcess,char*);
void      OProcessExecuteFile                   (OProcess,char*);
int       OProcessExecute                       (OProcess,char*);
void      OProcessPutHandles                   (OProcess,OHandle*);
OHandle*  OProcessGetHandles                   (OProcess);
void      OProcessPutString                     (OProcess,char*);
char*     OProcessGetString                     (OProcess,char*);
void      OProcessPutTypeAndIdentifier          (OProcess,void*,void*);
void      OProcessSetMode                       (OProcess,int,int);
void*     OProcessGetShell                      (OProcess);
char*     OProcessGetCommand                    (OProcess);
int       OProcessIsPipeActive                  (OProcess);
char*     OProcessGetAndConvertHandlesToString (OProcess);
int       OProcessPreparePipe                   (OProcess,char*);
CList     OProcessGetTypedObjects               (OProcess,char*,char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOProcess(0); /**/
#endif

#endif  /*OProcess_h*/



