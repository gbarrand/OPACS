/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <string.h>
#include <OType.h>

/*Xo*/
#include <XWidget.h>

/*Wo*/
#include <OClass.h>
#include <OInterface.h>
#include <OWidget.h>
#include <OPiece.h>
#include <OInterpreter.h>
#include <OCyclic.h>

#include <Wo.h>

#include <WoTypes.h>

#ifdef __cplusplus
extern "C"{
#endif
static void DeleteWidget (Widget);
#ifdef __cplusplus
}
#endif

/***************************************************************************/
void WoSetTypes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("OClass")!=NULL) return; /*done*/

  otype                          = OTypeCreate ("OClass");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OClassGetIdentifiers);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OClassGetAttribute);
  OTypeSetClearClassFunction     (otype,OClassClearClass);
  OTypeAddNewProperty            (otype,"identifier"     ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty            (otype,"name"           ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"group"          ,OPropertyString,NULL);

  otype                          = OTypeCreate ("OInterpreter");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OInterpreterGetIdentifiers);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OInterpreterGetAttribute);
  OTypeSetClearClassFunction     (otype,OInterpreterClearClass);
  OTypeAddNewProperty            (otype,"identifier"    ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty            (otype,"name"          ,OPropertyString,NULL);

  otype                          = OTypeCreate ("OCyclic");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OCyclicGetIdentifiers);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OCyclicGetAttribute);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OCyclicSetAttribute);
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)OCyclicDoMethod);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OCyclicMake);
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)OCyclicDelete);
  OTypeSetClearClassFunction     (otype,OCyclicClearClass);
  OTypeAddNewProperty            (otype,"identifier"    ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty            (otype,"name"          ,OPropertyString  ,NULL);
  OTypeAddNewProperty            (otype,"widget"        ,OPropertyString  ,NULL);
  OTypeAddNewProperty            (otype,"script"        ,OPropertyString  ,NULL);
  OTypeAddNewProperty            (otype,"delay"         ,OPropertyInteger ,NULL);
  OTypeAddNewMethodProperty      (otype,"start"         ,OPropertyVoid    ,NULL);
  OTypeAddNewMethodProperty      (otype,"stop"          ,OPropertyVoid    ,NULL);

  otype                          = OTypeCreate ("OInterface");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OInterfaceGetIdentifiers);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OInterfaceMake);
  OTypeSetValidateFunction       (otype,(OTypeValidateFunction)OInterfaceValidate);
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)OInterfaceDelete);
  OTypeSetDestroyFunction        (otype,(OTypeDestroyFunction)OInterfaceDestroy);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OInterfaceSetAttribute);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OInterfaceGetAttribute);
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)OInterfaceDoMethod);
  OTypeSetClearClassFunction     (otype,OInterfaceClearClass);
  OTypeAddNewProperty            (otype,"identifier"           ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty            (otype,"dataBaseName"         ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"dataBaseFile"         ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"packages"             ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"version"              ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"parent"               ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"prefix"               ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"createCallback"       ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"destroyCallback"      ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"resources"            ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"modified"             ,OPropertyBoolean,NULL);
  OTypeAddNewMethodProperty      (otype,"verifySyntaxOfScripts",OPropertyVoid ,NULL);
  OTypeAddNewMethodProperty      (otype,"mapShells"            ,OPropertyVoid ,NULL);
  OTypeAddNewMethodProperty      (otype,"getInformations"      ,OPropertyString,NULL);

  otype                          = OTypeCreate ("OWidget");
  OTypeSetConstructFunction      (otype,(OTypeConstructFunction)OPieceConstruct);
  OTypeSetSetReferencesFunction  (otype,(OTypeSetReferencesFunction)OPieceSetReferences);
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)OPieceDelete);
  OTypeAddNewReferencesProperty  (otype,"children" ,OPropertyArrayOfStrings,NULL);

  otype                          = OTypeGetIdentifier ("Widget");
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)DeleteWidget);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OWidgetSetAttribute);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OWidgetGetAttribute);
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)OWidgetDoMethod);
  OTypeSetConstructFunction      (otype,(OTypeConstructFunction)OWidgetConstruct);
  OTypeAddNewClassProperty       (otype,"filter"             ,OPropertyString,NULL);
  OTypeAddNewClassProperty       (otype,"thisValue"          ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"type"               ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"resources"          ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewMethodProperty      (otype,"putPageInPS"        ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"addCallback"        ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"addEventHandler"    ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"saveHierarchy"      ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"markFile"           ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"markResource"       ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"unmarkResource"     ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"duplicate"          ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"duplicateHierarchy" ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"setResource"        ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"setAndMarkResource" ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"getResource"        ,OPropertyString,NULL);

  otype                          = OTypeCreate ("Wo");
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)WoSetAttribute);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)WoGetAttribute);
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)WoDoMethod);
/*OTypeSetClearClassFunction     (otype,WoClearClass);*/ /*Never do this.*/
  OTypeAddNewClassProperty      (otype,"version"                 ,OPropertyString,NULL);
  OTypeAddNewClassProperty      (otype,"packages"                ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewClassProperty      (otype,"traces"                  ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewClassProperty      (otype,"shells"                  ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewClassProperty      (otype,"trace"                   ,OPropertyString  ,NULL);
  OTypeAddNewClassProperty      (otype,"interfaceFile"           ,OPropertyString  ,NULL);
  OTypeAddNewClassProperty      (otype,"pasteScript"             ,OPropertyString  ,NULL);
  OTypeAddNewClassProperty      (otype,"overflyHelp"             ,OPropertyBoolean ,NULL);
  OTypeAddNewClassProperty      (otype,"stopFlag"                ,OPropertyBoolean ,NULL);
  OTypeAddNewClassMethodProperty(otype,"sendExit"                ,OPropertyVoid ,NULL);
  OTypeAddNewClassMethodProperty(otype,"sendExitOnly"            ,OPropertyVoid ,NULL);
  OTypeAddNewClassMethodProperty(otype,"processEvents"           ,OPropertyVoid ,NULL);
  OTypeAddNewClassMethodProperty(otype,"flushEvents"             ,OPropertyVoid ,NULL);
  OTypeAddNewClassMethodProperty(otype,"makeApplication"         ,OPropertyVoid ,NULL);
  OTypeAddNewClassMethodProperty(otype,"makeApplicationMakefile" ,OPropertyVoid ,NULL);
  OTypeAddNewClassMethodProperty(otype,"loadInterface"           ,OPropertyVoid ,NULL);
  OTypeAddNewClassMethodProperty(otype,"removeInterface"         ,OPropertyVoid ,NULL);
  OTypeAddNewClassMethodProperty(otype,"saveInterface"           ,OPropertyVoid ,NULL);

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void DeleteWidget (
 Widget This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OWidgetDelete (This,False);
}


