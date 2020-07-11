/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OClass_h
#define OClass_h

typedef struct _OClassRecord* OClass;

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
typedef Widget(*OClassCreateWidgetFunction)(Widget,char*,ArgList,Cardinal);
typedef void  (*OClassVisitWidgetFunction)(Widget);
typedef void  (*OClassVoidFunction)();

void                       OClassClearClass                ();
OClass*                    OClassGetIdentifiers            ();
OClass                     OClassMake                      ();
OClass                     OClassCreate                    (char*,WidgetClass,char*,char*,OClassCreateWidgetFunction,OClassVisitWidgetFunction);
OClass                     OClassDeclareWidgetClass        (char*,WidgetClass);
OClass                     OClassDeclareCompoundWidget     (char*,OClassCreateWidgetFunction,WidgetClass);
OClass                     OClassGetIdentifier             (char*);
int                        OClassIsShell                   (OClass);
char*                      OClassGetName                   (OClass);
char*                      OClassGetGroup                  (OClass);
char*                      OClassGetCreateFunctionName     (OClass);
OClassCreateWidgetFunction OClassGetCreateFunction         (OClass);
void                       OClassSetPS_Function            (OClass,OClassVisitWidgetFunction);
OClassVisitWidgetFunction  OClassGetPS_Function            (OClass);
void                       OClassSetTargetChangedFunction  (OClass,OClassVisitWidgetFunction);
OClassVisitWidgetFunction  OClassGetTargetChangedFunction  (OClass);
void                       OClassSetClearClassFunction     (OClass,OClassVoidFunction);
WidgetClass                OClassGetWidgetClass            (OClass);
char*                      OClassGetPublicHeaderFileName   (OClass);
int                        OClassGetAttribute              (OClass,char*,void*,void*,int*);
WidgetClass                OClassGetWidgetClassIdentifier  (char*);
int                        OClassIsWidgetClassValid        (WidgetClass);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOClass(0);  /**/
#endif

#endif  /*OClass_h*/
