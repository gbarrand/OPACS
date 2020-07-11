/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OType_h
#define OType_h

typedef struct _OTypeRecord* OType;
typedef void*                OIdentifier;

#include <OProperty.h>
#include <OHandle.h>

typedef OIdentifier*(*OTypeGetIdentifiersFunction)(OType);
typedef OIdentifier (*OTypeMakeFunction)(OType);
typedef OIdentifier (*OTypeConstructFunction)(OType,int,char**,char**);
typedef int         (*OTypeValidateFunction)(OIdentifier);
typedef void        (*OTypeDeleteFunction)(OIdentifier);
typedef void        (*OTypeDestroyFunction)(OIdentifier);
typedef int         (*OTypeSetPropertyFunction)(OIdentifier,OType,OProperty,void*,int);
typedef int         (*OTypeSetAttributeFunction)(OIdentifier,char*,void*,void*,int);
typedef int         (*OTypeGetPropertyFunction)(OIdentifier,OType,OProperty,void*,int*);
typedef int         (*OTypeGetAttributeFunction)(OIdentifier,char*,void*,void*,int*);
typedef OHandle*    (*OTypeSetReferencesFunction)(OType,OIdentifier,OProperty,char*,OHandle*);
typedef void        (*OTypeGetReferencesFunction)(OType,OIdentifier,OProperty,char**,OHandle**);
typedef void*       (*OTypeRepresentFunction)(OIdentifier,OType);
typedef void        (*OTypeClearClassFunction)();
typedef int         (*OTypeDoPropertyFunction)(OIdentifier,OType,OProperty,int,char**,void*,int*);
typedef int         (*OTypeDoMethodFunction)(OIdentifier,char*,void*,int,char**,void*,int*);
typedef OHandle*    (*OTypeLoadFunction)(char*,char*);

#define IF_EQ       1
#define IF_NE       2
#define IF_LE       3
#define IF_LT       4
#define IF_GE       5
#define IF_GT       6
#define IF_IN       7
#define IF_NI       8

#ifdef __cplusplus
extern "C"{
#endif
void                    OTypeClearClass                          ();
void                    OTypeProduceHTML_File                    (char*);
OType                   OTypeGetIdentifier                       (char*);
OType*                  OTypeGetIdentifiers                      ();
int                     OTypeIsValid                             (OType);
void*                   OTypeOpenODB_FileForWriting              (char*);
int                     OTypeGenerateLabel                       (int);
void                    OTypeAddLoader                           (char*,OTypeLoadFunction);  
OType                   OTypeCreate                              (char*);
void                    OTypeDelete                              (OType);
void                    OTypeSetGetIdentifiersFunction           (OType,OTypeGetIdentifiersFunction);
void                    OTypeSetMakeFunction                     (OType,OTypeMakeFunction);
void                    OTypeSetConstructFunction                (OType,OTypeConstructFunction);
void                    OTypeSetValidateFunction                 (OType,OTypeValidateFunction);
void                    OTypeSetDeleteFunction                   (OType,OTypeDeleteFunction);
void                    OTypeSetDestroyFunction                  (OType,OTypeDestroyFunction);
void                    OTypeSetSetPropertyFunction              (OType,OTypeSetPropertyFunction);
void                    OTypeSetGetPropertyFunction              (OType,OTypeGetPropertyFunction);
void                    OTypeSetDoPropertyFunction               (OType,OTypeDoPropertyFunction);
void                    OTypeSetDoMethodFunction                 (OType,OTypeDoMethodFunction);
void                    OTypeSetSetAttributeFunction             (OType,OTypeSetAttributeFunction);
void                    OTypeSetGetAttributeFunction             (OType,OTypeGetAttributeFunction);
void                    OTypeSetSetReferencesFunction            (OType,OTypeSetReferencesFunction);
void                    OTypeSetGetReferencesFunction            (OType,OTypeGetReferencesFunction);
void                    OTypeSetRepresentFunction                (OType,OTypeRepresentFunction);
void                    OTypeSetClearClassFunction               (OType,OTypeClearClassFunction);
void                    OTypeSetUserData                         (OType,void*);
void                    OTypeAddProperty                         (OType,OProperty);
OProperty               OTypeAddNewProperty                      (OType,char*,int,void*);
OProperty               OTypeAddNewMethodProperty                (OType,char*,int,void*);
OProperty               OTypeAddNewClassProperty                 (OType,char*,int,void*);
OProperty               OTypeAddNewClassMethodProperty           (OType,char*,int,void*);
OProperty               OTypeAddNewReferencesProperty            (OType,char*,int,void*);
OProperty               OTypeAddNewPropertyUserDataInteger       (OType,char*,int,int);
OTypeClearClassFunction OTypeGetClearClassFunction               (OType);
OTypeMakeFunction       OTypeGetMakeFunction                     (OType);
OProperty               OTypeGetPropertyIdentifier               (OType,char*);
char*                   OTypeGetName                             (OType);
OTypeRepresentFunction  OTypeGetRepresentFunction                (OType);
void*                   OTypeGetUserData                         (OType);
void                    OTypeLoadFile                            (char*,char*);
OHandle*                OTypeGetExtentFromFile                   (char*,char*,char*);
OHandle*                OTypeLoadODB                             (char*,char*);
OIdentifier*            OTypeGetObjectIdentifiers                (OType);
OIdentifier*            OTypeCollectObjects                      (OType,int,OIdentifier*,OProperty,int,void*,int,int*);
char*                   OTypeDumpObjectsInString                 (OType,int,OIdentifier*,int,OProperty*);
OIdentifier             OTypeMakeObject                          (OType);
OIdentifier             OTypeConstructObject                     (OType,int,char**,char**);
OIdentifier             OTypeGetObjectIdentifier                 (OType,char*);
char*                   OTypeGetObjectName                       (OType,OIdentifier);
int                     OTypeIsObjectValid                       (OType,OIdentifier);
void                    OTypeDeleteObject                        (OType,OIdentifier);
void                    OTypeDestroyObject                       (OType,OIdentifier);
int                     OTypeSetObjectProperty                   (OType,OIdentifier,OProperty,void*,int);
int                     OTypeGetObjectProperty                   (OType,OIdentifier,OProperty,void*,int*);
OHandle*                OTypeSetObjectReferences                 (OType,OIdentifier,OProperty,char* ,OHandle*);
void                    OTypeGetObjectReferences                 (OType,OIdentifier,OProperty,char**,OHandle**);
int                     OTypeSetObjectPropertyFromString         (OType,OIdentifier,OProperty,char*);
char*                   OTypeGetAndConvertObjectPropertyToString (OType,OIdentifier,OProperty);
int                     OTypeDoObjectProperty                    (OType,OIdentifier,OProperty,int,char**,void*,int*);
int                     OTypeSelectObject                        (OType,OIdentifier,OProperty,int,void*,int);
void                    OTypePutObjectInODB_Stream               (OType,OIdentifier,void*);
char*                   OTypeConvertTypeAndIdentifierToString    (OType,OIdentifier);
OType                   OTypeConvertStringToTypeAndIdentifier    (char*,OIdentifier*);
int                     OTypeSetAttribute                        (OType,char*,void*,void*,int);
int                     OTypeGetAttribute                        (OType,char*,void*,void*,int*);
OType*                  OTypeGetTypesFromHandles                (OHandle*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOType(0); /**/
#endif

#endif  /*OType_h*/



