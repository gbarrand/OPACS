/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OInterface_h
#define OInterface_h
 
typedef struct _OInterfaceRecord   *OInterface;

#include <CList.h>
#include <OTrace.h>
#ifdef __cplusplus
extern "C"{
#endif
void        OInterfaceClearClass                 ();
OInterface* OInterfaceGetIdentifiers             ();
void        OInterfaceAddPackage                 (char*);
char**      OInterfaceGetPackages                (int*);
int         OInterfaceIsPackageKnown             (char*);
char*       OInterfaceGetVersion                 ();
void        OInterfaceSetTrace                   (OTrace);
OInterface  OInterfaceGetIdentifier              (char*);
void        OInterfaceVerifyScriptSyntaxOfExtent ();
void        OInterfaceSaveExtent                 ();
int         OInterfaceIsExtentModified           ();
void        OInterfaceSetExtentNotModified       ();
OInterface  OInterfaceMake                       ();
OInterface  OInterfaceCreate                     (char*,char*);
int         OInterfaceValidate                   (OInterface);
int         OInterfaceIsValid                    (OInterface);
void        OInterfaceDelete                     (OInterface);
void        OInterfaceSetName                    (OInterface,char*);
char*       OInterfaceGetName                    (OInterface);
void        OInterfaceMark                       (OInterface,int);
int         OInterfaceIsModified                 (OInterface);
void        OInterfaceSetVolatile                (OInterface,int);
char*       OInterfaceGetPrefix                  (OInterface);
void        OInterfaceSetPrefix                  (OInterface,char*);
void        OInterfaceSetResources               (OInterface,char*);
char*       OInterfaceGetParent                  (OInterface);
void        OInterfaceSetFileName                (OInterface,char*);
char*       OInterfaceGetFileName                (OInterface);
void        OInterfaceSetPackages                (OInterface,char*);
void        OInterfaceSetVersion                 (OInterface,char*);
void        OInterfaceSetParent                  (OInterface,char*);
char*       OInterfaceGetCreateCallbackScript    (OInterface);
void        OInterfaceSetCreateCallbackScript    (OInterface,char*);
char*       OInterfaceGetDestroyCallbackScript   (OInterface);
void        OInterfaceSetDestroyCallbackScript   (OInterface,char*);
char*       OInterfaceGetInformations            (OInterface);
void        OInterfaceVerifySyntaxOfScripts      (OInterface);
void        OInterfaceSave                       (OInterface);
void        OInterfaceMapShells                  (OInterface);
void        OInterfaceDestroy                    (OInterface);
int         OInterfaceSetAttribute               (OInterface,char*,void*,void*,int);
int         OInterfaceGetAttribute               (OInterface,char*,void*,void*,int*);
int         OInterfaceDoMethod                   (OInterface,char*,void*,int,char**,void*,int*);
void        OInterfaceLoadFile                   (char*,char*);
void        OInterfaceRealize                    (CList,char*);
  
void*       OPieceConstruct                      (void*,int,char**,char**);
CList       OPieceSetReferences                  (void*,void*,void*,char*,CList);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOInterface(0);  /**/
#endif

#endif  /*OInterface_h*/
