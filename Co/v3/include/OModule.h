/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OModule_h
#define OModule_h

typedef struct _OModuleRecord*  OModule;

#ifdef __cplusplus
extern "C"{
#endif
void      OModuleClearClass        ();
OModule   OModuleGetIdentifier     (char*);
OModule*  OModuleGetIdentifiers    ();
OModule   OModuleMake              ();
void      OModuleDelete            (OModule);
void      OModuleDump              (OModule);
char*     OModuleGetName           (OModule);
char*     OModuleGetLanguage       (OModule);
char*     OModuleGetExtension      (OModule);
char*     OModuleGetDirectory      (OModule);
char**    OModuleGetIncludeFiles   (OModule,int*);
char**    OModuleGetOBJ_Files      (OModule,int*);
char**    OModuleGetLibraries      (OModule,int*);
char**    OModuleGetHasFlags       (OModule,int*);
int       OModuleIsSourceFile      (OModule);
int       OModuleIsApplication     (OModule);
int       OModuleIsDistributed     (OModule);
int       OModuleIsANSI            (OModule);
int       OModuleIsLibrary         (OModule);
int       OModuleIsSharedLibrary   (OModule,char*);
int       OModuleIsInLibrary       (OModule,char*);
int       OModuleIsBeforeLibraries (OModule);
int       OModuleSetAttribute      (OModule,char*,void*,void*,int);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOModule(0); /**/
#endif

#endif  /*OModule_h*/



