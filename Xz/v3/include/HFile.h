/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef HFile_h
#define HFile_h

typedef struct _HFileRecord *HFile;

#ifdef __cplusplus
extern "C"{
#endif
void      HFileClearClass                    ();
HFile*    HFileGetIdentifiers                ();
HFile     HFileGetIdentifier                 (char*);
int       HFileIsValid                       (HFile);
HFile     HFileGetFirstIdentifier            ();
HFile     HFileMake                          ();
HFile     HFileOpenZEBRA_FileForReading      (char*,char*);
void      HFileDelete                        (HFile);
void      HFileUpdate                        (HFile);
char*     HFileGetName                       (HFile);
int       HFileGetType                       (HFile);
int       HFileSetDirectory                  (HFile,char*);
void      HFileBringDirectoryInMemory        (HFile);
int       HFileGetAttribute                  (HFile,char*,void*,void*,int*);
int       HFileDoMethod                      (HFile,char*,void*,int,char**,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindHFile(0); /**/
#endif

#endif  /*HFile_h*/
