/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OShell_h
#define OShell_h

typedef struct _OShellRecord *OShell;

#include <OProcess.h>
typedef int(*OShellExecuteCommandFunction)(int,char**,OProcess);

#include <OCommand.h>
#ifdef __cplusplus
extern "C"{
#endif
void     OShellClearClass           ();
OShell*  OShellGetIdentifiers       ();
OShell   OShellGetIdentifier        (char*);
OShell   OShellCreate               (char*);
void     OShellDelete               (OShell);
void     OShellProduceHTML_File     (OShell,char*);
char*    OShellGetName              (OShell);
void     OShellAddCommand           (OShell,OCommand);
void     OShellAddNewCommand        (OShell,char*,OShellExecuteCommandFunction);
OCommand OShellGetCommandIdentifier (OShell,char*);
void     OShellExecute              (OShell,char*);
void     OShellExecuteF             (OShell,int,char*,...);
void     OShellExecuteFile          (OShell,char*);
int      OShellGetAttribute         (OShell,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOShell(0); /**/
#endif

#endif  /*OShell_h*/



