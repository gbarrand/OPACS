/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*#define DEBUG*/

#include <string.h>
#include <stdlib.h>

#include <CString.h>
#include <CFile.h>
#include <CMath.h>
#include <CPS.h>
#include <CBatch.h>
#include <CMemory.h>
#include <CSystem.h>
#include <OShell.h>
#include <OType.h>
#include <OHTML.h>
#include <OLUT.h>
#include <OMatrix33.h>
#include <OMatrix.h>
#include <OBatch.h>

#include <CoTypes.h>

/***************************************************************************/
void CoSetTypes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("CMemory")!=NULL) return; /*done*/

  otype                           = OTypeCreate ("CSystem");
  OTypeSetGetAttributeFunction    (otype,CSystemGetAttribute);
  OTypeAddNewClassProperty        (otype,"listCommand"       ,OPropertyString ,NULL);
  OTypeAddNewClassProperty        (otype,"makeCommand"       ,OPropertyString ,NULL);
  OTypeAddNewClassProperty        (otype,"remoteShellCommand",OPropertyString ,NULL);
  OTypeAddNewClassProperty        (otype,"kind"              ,OPropertyString ,NULL);

  otype                           = OTypeCreate ("CMemory");
  OTypeSetGetAttributeFunction    (otype,CMemoryGetAttribute);
  OTypeAddNewClassProperty        (otype,"byteBalance"            ,OPropertyInteger        ,NULL);
  OTypeAddNewClassProperty        (otype,"numberOfAllocations"    ,OPropertyInteger        ,NULL);
  OTypeAddNewClassProperty        (otype,"maximumAllocatedBytes"  ,OPropertyInteger        ,NULL);

  otype                           = OTypeCreate ("CMath");
  OTypeSetGetAttributeFunction    (otype,CMathGetAttribute);
  OTypeSetSetAttributeFunction    (otype,CMathSetAttribute);
  OTypeAddNewClassProperty        (otype,"randomMaximum"          ,OPropertyDouble       ,NULL);
  OTypeAddNewClassProperty        (otype,"seed"                   ,OPropertyInteger      ,NULL);
  OTypeAddNewClassProperty        (otype,"random"                 ,OPropertyDouble       ,NULL);

  otype                           = OTypeCreate ("CPS");
  OTypeSetGetAttributeFunction    (otype,CPS_GetAttribute);
  OTypeSetSetAttributeFunction    (otype,CPS_SetAttribute);
  OTypeSetDoMethodFunction        (otype,CPS_DoMethod);
  OTypeSetClearClassFunction      (otype,CPS_ClearClass);
  OTypeAddNewClassMethodProperty  (otype,"close"           ,OPropertyVoid         ,NULL);
  OTypeAddNewClassProperty        (otype,"fileName"        ,OPropertyString       ,NULL);
  OTypeAddNewClassProperty        (otype,"colorScheme"     ,OPropertyString       ,NULL);
  OTypeAddNewClassProperty        (otype,"orientation"     ,OPropertyString       ,NULL);
  OTypeAddNewClassProperty        (otype,"backgroundDrawn" ,OPropertyBoolean      ,NULL);
  OTypeAddNewClassProperty        (otype,"bitsPerPixel"    ,OPropertyInteger      ,NULL);
  OTypeAddNewClassProperty        (otype,"lineWidth"       ,OPropertyInteger      ,NULL);

  otype                           = OTypeCreate ("CString");
  OTypeSetClearClassFunction      (otype,CStringClearClass);

  otype                           = OTypeCreate ("CFile");
  OTypeSetClearClassFunction      (otype,CFileClearClass);

  otype                           = OTypeCreate ("CBatch");
  OTypeSetClearClassFunction      (otype,CBatchClearClass);

  otype                           = OTypeCreate ("OHTML");
  OTypeSetClearClassFunction      (otype,OHTML_ClearClass);

  otype                           = OTypeCreate ("OMatrix");
  OTypeSetSetAttributeFunction    (otype,(OTypeSetAttributeFunction)OMatrixSetAttribute);
  OTypeSetMakeFunction            (otype,(OTypeMakeFunction)OMatrixMake);
  OTypeSetDeleteFunction          (otype,(OTypeDeleteFunction)OMatrixDelete);
  OTypeAddNewProperty             (otype,"values",OPropertyArrayOfDoubles,NULL);

  otype                           = OTypeCreate ("OMatrix33");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)OMatrix33GetIdentifiers);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)OMatrix33GetAttribute);
  OTypeSetDoMethodFunction        (otype,(OTypeDoMethodFunction)OMatrix33DoMethod);
  OTypeSetSetAttributeFunction    (otype,(OTypeSetAttributeFunction)OMatrix33SetAttribute);
  OTypeSetMakeFunction            (otype,(OTypeMakeFunction)OMatrix33Make);
  OTypeSetDeleteFunction          (otype,(OTypeDeleteFunction)OMatrix33Delete);
  OTypeSetClearClassFunction      (otype,OMatrix33ClearClass);
  OTypeAddNewClassMethodProperty  (otype,"testClass"  ,OPropertyVoid  ,NULL);
  OTypeAddNewProperty             (otype,"identifier" ,OPropertyUnsignedLong   ,NULL);
  OTypeAddNewProperty             (otype,"name"       ,OPropertyString         ,NULL);
  OTypeAddNewProperty             (otype,"values"     ,OPropertyArrayOfIntegers,NULL);
  OTypeAddNewProperty             (otype,"scale"      ,OPropertyInteger        ,NULL);
  OTypeAddNewMethodProperty       (otype,"testObject" ,OPropertyInteger  ,NULL);

  otype                           = OTypeCreate ("OLUT");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)OLUT_GetIdentifiers);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)OLUT_GetAttribute);
  OTypeSetSetAttributeFunction    (otype,(OTypeSetAttributeFunction)OLUT_SetAttribute);
  OTypeSetMakeFunction            (otype,(OTypeMakeFunction)OLUT_Make);
  OTypeSetDeleteFunction          (otype,(OTypeDeleteFunction)OLUT_Delete);
  OTypeSetClearClassFunction      (otype,OLUT_ClearClass);
  OTypeAddNewProperty             (otype,"identifier"    ,OPropertyUnsignedLong,NULL);
  OTypeAddNewProperty             (otype,"name"          ,OPropertyString,NULL);
  OTypeAddNewProperty             (otype,"type"          ,OPropertyString,NULL);
  OTypeAddNewProperty             (otype,"minimum"       ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"maximum"       ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"numberOfBins"  ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"size"          ,OPropertyInteger,NULL);

  otype                           = OTypeCreate ("OProperty");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)OPropertyGetIdentifiers);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)OPropertyGetAttribute);
  OTypeAddNewProperty             (otype,"identifier"                ,OPropertyUnsignedLong,NULL);
  OTypeAddNewProperty             (otype,"name"                      ,OPropertyString,NULL);
  OTypeAddNewProperty             (otype,"isClass"                   ,OPropertyBoolean,NULL);
  OTypeAddNewProperty             (otype,"isMethod"                  ,OPropertyBoolean,NULL);
  OTypeAddNewProperty             (otype,"isListOfReferences"        ,OPropertyBoolean,NULL);

  otype                           = OTypeCreate ("OType");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)OTypeGetIdentifiers);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)OTypeGetAttribute);
  OTypeSetSetAttributeFunction    (otype,(OTypeSetAttributeFunction)OTypeSetAttribute);
  OTypeAddNewClassProperty        (otype,"dumpMemory"          ,OPropertyBoolean ,NULL);
  OTypeAddNewProperty             (otype,"identifier"     ,OPropertyUnsignedLong,NULL);
  OTypeAddNewProperty             (otype,"name"           ,OPropertyString,NULL);
  OTypeAddNewProperty             (otype,"properties"     ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewProperty             (otype,"hasGetIdentifiersFunction" ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasMakeFunction"           ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasLoadBeginFunction"      ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasConstructFunction"      ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasValidateFunction"       ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasDeleteFunction"         ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasDestroyFunction"        ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasSetPropertyFunction"    ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasGetPropertyFunction"    ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasSetAttributeFunction"   ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasGetAttributeFunction"   ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasRepresentFunction"      ,OPropertyInteger,NULL);
  OTypeAddNewProperty             (otype,"hasClearClassFunction"     ,OPropertyInteger,NULL);

  otype                           = OTypeCreate ("OShell");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)OShellGetIdentifiers);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)OShellGetAttribute);
  OTypeSetClearClassFunction      (otype,OShellClearClass);
  OTypeAddNewProperty             (otype,"identifier"  ,OPropertyUnsignedLong,NULL);
  OTypeAddNewProperty             (otype,"name"        ,OPropertyString,NULL);

  otype                           = OTypeCreate ("OCommand");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)OCommandGetIdentifiers);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)OCommandGetAttribute);
  OTypeAddNewProperty             (otype,"identifier"   ,OPropertyUnsignedLong,NULL);
  OTypeAddNewProperty             (otype,"name"         ,OPropertyString,NULL);
  OTypeAddNewProperty             (otype,"group"        ,OPropertyString,NULL);

  otype                           = OTypeCreate ("OBatch");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)OBatchGetIdentifiers);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)OBatchGetAttribute);
  OTypeSetClearClassFunction      (otype,OBatchClearClass);
  OTypeAddNewProperty             (otype,"identifier"    ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty             (otype,"command"       ,OPropertyString,NULL);
  OTypeAddNewProperty             (otype,"pid"           ,OPropertyInteger   ,NULL);

  OTypeAddLoader                  ("odb",OTypeLoadODB);

}

