/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CFile_h
#define CFile_h

#ifndef NO_C_H
#include <stdio.h>   
#endif

#ifdef __cplusplus
extern "C"{
#endif
void            CFileClearClass              ();
void            CFileAddPath                 (char*);
void            CFileAddPathVariable         (char*);
char*           CFileBuildTemporaryName      (char*);
int             CFileIsValid                 (char*);
int             CFileIsValidInPathList       (char*,char**);
FILE*           CFileOpenForReading          (char*,char**);
FILE*           CFileOpenTemporaryForWriting (char*,char**);
FILE*           CFileOpenForWritingF         (int,char*,...);
FILE*           CFileOpenForWriting          (char*);
FILE*           CFileOpenForAppending        (char*);
void            CFileSafeguardF              (int,char*,...);
void            CFileSafeguard               (char*);
char*           CFileLoadStrings             (char*,char,int*,char***);
char**          CFileLoadText                (char*,int*);
void            CFileSaveText                (char*,int,char**);
void            CFileLink                    (char**);
void            CFileBindToF77_Unit          (char*,int);
int             CFileProduceFromTemplate     (char*,char*,char*,char*);
char*           CFileGetName                 (char*);
char*           CFileGetType                 (char*);
char*           CFileGetDirectory            (char*);
void            CFileTrimDirectory           (char*);
void            CFileTrimType                (char*);
void            CFileTrimName                (char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCFile(0); /**/
#endif

#endif  /*CFile_h*/



