/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OCommand_h
#define OCommand_h

typedef struct _OCommandRecord *OCommand;

typedef int(*OCommandExecuteFunction)(int,char**,void*);

#ifdef __cplusplus
extern "C"{
#endif
void                     OCommandClearClass         ();
OCommand*                OCommandGetIdentifiers     ();
OCommand                 OCommandGetIdentifier      (char*);
int                      OCommandIsValid            (OCommand);
OCommand                 OCommandCreate             (char*,OCommandExecuteFunction);
void                     OCommandDelete             (OCommand);
char*                    OCommandGetGroup           (OCommand);
char*                    OCommandGetName            (OCommand);
OCommandExecuteFunction  OCommandGetExecuteFunction (OCommand);
int                      OCommandGetAttribute       (OCommand,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOCommand(0); /**/
#endif

#endif  /*OCommand_h*/



