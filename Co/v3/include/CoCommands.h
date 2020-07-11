/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CoCommands_h
#define CoCommands_h

#include <OShell.h>
#ifdef __cplusplus
extern "C"{
#endif
void CoAddCommands (OShell);

int CoExecute_dot                   (int,char**,OProcess);
int CoExecute_echo                  (int,char**,OProcess);
int CoExecute_exit                  (int,char**,OProcess);
int CoExecute_cat                   (int,char**,OProcess);
int CoExecute_rm                    (int,char**,OProcess);
int CoExecute_printf                (int,char**,OProcess);
int CoExecute_sort                  (int,char**,OProcess);
int CoExecute_test                  (int,char**,OProcess);
int CoExecute_date                  (int,char**,OProcess);
int CoExecute_printenv              (int,char**,OProcess);
int CoExecute_expr                  (int,char**,OProcess);

int CoExecute_create                (int,char**,OProcess);
int CoExecute_load                  (int,char**,OProcess);
int CoExecute_collect               (int,char**,OProcess);
int CoExecute_delete                (int,char**,OProcess);
int CoExecute_destroy               (int,char**,OProcess);
int CoExecute_save                  (int,char**,OProcess);
int CoExecute_dump                  (int,char**,OProcess);
int CoExecute_set                   (int,char**,OProcess);
int CoExecute_get                   (int,char**,OProcess);
int CoExecute_do                    (int,char**,OProcess);
int CoExecute_isValid               (int,char**,OProcess);
int CoExecute_isNotValid            (int,char**,OProcess);
int CoExecute_count                 (int,char**,OProcess);
int CoExecute_empty                 (int,char**,OProcess);
int CoExecute_clear                 (int,char**,OProcess);

int CoExecute_osh                   (int,char**,OProcess);
int CoExecute_isCommand             (int,char**,OProcess);
int CoExecute_makeHTML              (int,char**,OProcess);
int CoExecute_buildFileName         (int,char**,OProcess);
int CoExecute_addFilePath           (int,char**,OProcess);
int CoExecute_help                  (int,char**,OProcess);
int CoExecute_spawn                 (int,char**,OProcess);
int CoExecute_getTopic              (int,char**,OProcess);
int CoExecute_makeMakefile          (int,char**,OProcess);

int CoExecute_getStringLength       (int,char**,OProcess);
int CoExecute_matchString           (int,char**,OProcess);
int CoExecute_prependString         (int,char**,OProcess);
int CoExecute_lowerString           (int,char**,OProcess);
int CoExecute_raiseString           (int,char**,OProcess);
int CoExecute_getStringWord         (int,char**,OProcess);
int CoExecute_removeLastStringWord  (int,char**,OProcess);
int CoExecute_replaceStringPart     (int,char**,OProcess);
int CoExecute_cutStringBegin        (int,char**,OProcess);
int CoExecute_removeDuplicates      (int,char**,OProcess);
int CoExecute_getLine               (int,char**,OProcess);
int CoExecute_convertToString       (int,char**,OProcess);
int CoExecute_putStringInPipe       (int,char**,OProcess);

int CoExecute_stdout                (int,char**,OProcess);
int CoExecute_calculate             (int,char**,OProcess);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCoCommands(0); /**/
#endif

#endif  /*CoCommands_h*/



