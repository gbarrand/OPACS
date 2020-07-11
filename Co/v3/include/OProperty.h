/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OProperty_h
#define OProperty_h

typedef struct _OPropertyRecord*  OProperty;

#define   OPropertyInteger                0
#define   OPropertyUnsignedLong           1
#define   OPropertyFloat                  2
#define   OPropertyDouble                 3
#define   OPropertyString                 4
#define   OPropertyVoid                   5
#define   OPropertyBoolean                6
#define   OPropertyArrayOfIntegers       10
#define   OPropertyArrayOfUnsignedLongs  11
#define   OPropertyArrayOfFloats         12
#define   OPropertyArrayOfDoubles        13
#define   OPropertyArrayOfStrings        14

#ifndef NO_C_H
#include <stdarg.h>
#endif
#ifdef __cplusplus
extern "C"{
#endif
void        OPropertyClearClass              ();
OProperty   OPropertyCreate                  (char*,int,void*);
OProperty   OPropertyCreateMethod            (char*,int,void*);
OProperty   OPropertyCreateClass             (char*,int,void*);
OProperty   OPropertyCreateClassMethod       (char*,int,void*);
OProperty   OPropertyCreateReferences        (char*,int,void*);
void        OPropertyDelete                  (OProperty);
void        OPropertySetFormat               (OProperty,int,char*);
int         OPropertyIsListOfReferences      (OProperty);
char*       OPropertyGetName                 (OProperty);
int         OPropertyGetType                 (OProperty);
int         OPropertyIsClass                 (OProperty);
int         OPropertyIsMethod                (OProperty);
void        OPropertySetUserDataInteger      (OProperty,int);
void*       OPropertyGetUserData             (OProperty);
int         OPropertyGetUserDataInteger      (OProperty);
void        OPropertyPutHTML_InStream        (OProperty,va_list);
char*       OPropertyGetHeader               (OProperty);
char*       OPropertyConvertVariableToString (OProperty,void*,int);
void*       OPropertyGetVariable             (OProperty);
int         OPropertySetVariableFromString   (OProperty,char*,void*,int*);
char*       OPropertyPrintVariableInString   (OProperty,void*);
char*       OPropertyPrintArrayInString      (OProperty,void*,int);
int         OPropertyIsArray                 (OProperty);

OProperty*  OPropertyGetIdentifiers          ();
int         OPropertyGetAttribute            (OProperty,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOProperty(0); /**/
#endif

#endif  /*OProperty_h*/

