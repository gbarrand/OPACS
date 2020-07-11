/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OInterpreter_h
#define OInterpreter_h

typedef struct _OInterpreterRecord* OInterpreter;

#ifdef __cplusplus
extern "C"{
#endif

typedef void(*OInterpreterVoidFunction)();
typedef void(*OInterpreterDoFunction)(char*);

void             OInterpreterClearClass      ();
OInterpreter*    OInterpreterGetIdentifiers  ();
OInterpreter     OInterpreterMake            ();
OInterpreter     OInterpreterCreate          (char*,OInterpreterDoFunction,OInterpreterVoidFunction,OInterpreterDoFunction,OInterpreterDoFunction);
OInterpreter     OInterpreterAddInterpreter  (char*,OInterpreterDoFunction);
char*            OInterpreterGetName         (OInterpreter);
int              OInterpreterGetAttribute    (OInterpreter,char*,void*,void*,int*);

int              OScriptIsBackgroundScript   (char*,char**);
void             OScriptExecute              (char*);
void             OScriptVerifySyntax         (char*);
void             OScriptGenerateCallbackCode (char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOInterpreter(0);  /**/
#endif

#endif  /*OInterpreter_h*/
