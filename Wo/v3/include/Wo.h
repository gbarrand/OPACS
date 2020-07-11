/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef Wo_h
#define Wo_h
 
#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

#include <OTrace.h>
#include <OShell.h>
#ifdef __cplusplus
extern "C"{
#endif
typedef void(*WoInitializeClassFunction)(Display*);
typedef void(*WoVoidFunction)();

void     WoSetXt                             (XtAppContext,Widget);
void     WoStartup                           (int,char**);
void     WoClearClass                        ();
int      WoIsInitialized                     ();
void     WoSetInterfaceFile                  (char*);
char*    WoGetInterfaceFile                  ();
char*    WoGetVersion                        ();
char**   WoGetArguments                      (int*);
void     WoSetTrace                          (OTrace);
OTrace   WoGetTrace                          ();
void     WoSetInitializeClassFunction        (WoInitializeClassFunction);
void     WoSetExitFunction                   (WoVoidFunction);
int      WoProcessEvents                     ();
void     WoFlushEvents                       ();
int      WoDispatchEvent                     (void*);
int      WoGetStopFlag                       ();
void     WoSetStopFlag                       ();
void     WoSetPasteScript                    (char*);
int      WoWaitClientMessage                 ();
void     WoSendExit                          ();
OShell   WoGetShellInterpreter               ();
void     WoExecuteSystemScript               (char*);
void     WoExecuteShellScript                (char*);
void     WoExecuteShellScriptInSameContext   (char*);
void     WoExecuteShellFileInSameContext     (char*);
OProcess WoGetGlobalProcess                  ();
void     WoAddClearClassFunction             (WoVoidFunction);
int      WoSetAttribute                      (void*,char*,void*,void*,int);
int      WoGetAttribute                      (void*,char*,void*,void*,int*);
int      WoDoMethod                          (void*,char*,void*,int,char**,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindWo(0);  /**/
#endif

#endif  /*Wo_h*/
