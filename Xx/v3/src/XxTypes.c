/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <OType.h>

#include <XWidget.h>

#include <XxTypes.h>

/***************************************************************************/
void XxSetTypes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("Widget")!=NULL) return; /*done*/

  otype                          = OTypeCreate ("Widget");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)XWidgetGetIdentifiers);
  OTypeSetSetAttributeFunction    (otype,(OTypeSetAttributeFunction)XWidgetSetAttribute);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)XWidgetGetAttribute);
  OTypeSetDoMethodFunction        (otype,(OTypeDoMethodFunction)XWidgetDoMethod);
  OTypeSetClearClassFunction      (otype,XWidgetClearClass);
/*Class properties.*/
  OTypeAddNewClassProperty       (otype,"this"                   ,OPropertyString,NULL);
  OTypeAddNewClassProperty       (otype,"thisParent"             ,OPropertyString,NULL);
  OTypeAddNewClassProperty       (otype,"thisShell"              ,OPropertyString,NULL);
  OTypeAddNewClassProperty       (otype,"thisLocalTarget"        ,OPropertyString,NULL);
  OTypeAddNewClassProperty       (otype,"target"                 ,OPropertyString,NULL);
  OTypeAddNewClassProperty       (otype,"eventType"              ,OPropertyInteger,NULL);
  OTypeAddNewClassProperty       (otype,"eventX"                 ,OPropertyInteger,NULL);
  OTypeAddNewClassProperty       (otype,"eventY"                 ,OPropertyInteger,NULL);
  OTypeAddNewClassProperty       (otype,"grabButton"             ,OPropertyInteger,NULL);
  OTypeAddNewClassProperty       (otype,"grabCursor"             ,OPropertyString,NULL);
  OTypeAddNewClassProperty       (otype,"grabbedX"               ,OPropertyInteger,NULL);
  OTypeAddNewClassProperty       (otype,"grabbedY"               ,OPropertyInteger,NULL);
  OTypeAddNewClassProperty       (otype,"grabbedWidth"           ,OPropertyInteger,NULL);
  OTypeAddNewClassProperty       (otype,"grabbedHeight"          ,OPropertyInteger,NULL);
  OTypeAddNewClassProperty       (otype,"grabbedWidget"          ,OPropertyString,NULL);
/*Object properties.*/
  OTypeAddNewProperty            (otype,"identifier"             ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty            (otype,"name"                   ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"class"                  ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"parent"                 ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"shell"                  ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"nextBrother"            ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"localTarget"            ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"children"               ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewProperty            (otype,"managedChildren"        ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewProperty            (otype,"cursor"                 ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"shape"                  ,OPropertyString,NULL);
  OTypeAddNewMethodProperty      (otype,"raise"                  ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"lower"                  ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"map"                    ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"unmap"                  ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"manage"                 ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"unmanage"               ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"popup"                  ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"popdown"                ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"iconify"                ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"uniconify"              ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"draw"                   ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"sendPixmapToRootWindow" ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"putPixmapInPS"          ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"move"                   ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"changeSize"             ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"callCallbacks"          ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"getInformations"        ,OPropertyString,NULL);
  OTypeAddNewMethodProperty      (otype,"grabRootWindowPart"     ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"grabWidget"             ,OPropertyVoid,NULL);
}










