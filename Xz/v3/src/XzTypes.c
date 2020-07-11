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

#include <CPrinter.h>
#include <CList.h>
#include <OType.h>
#include <CoTypes.h>

#include <HBook.h>
#include <HFile.h>
#include <HF77.h>

#include <XzTypes.h>

#ifdef __cplusplus
extern "C"{
#endif
static OHandle* LoadFile (char*,char*);
#ifdef __cplusplus
}
#endif

/***************************************************************************/
void XzSetTypes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("HBook")!=NULL) return; /*done*/

  CoSetTypes               ();
 
  otype                          = OTypeCreate ("HBook");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)HBookGetIdentifiers);
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)HBookDelete);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)HBookGetAttribute);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)HBookSetAttribute);
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)HBookDoMethod);
  OTypeSetConstructFunction      (otype,(OTypeConstructFunction)HBookConstruct);
  OTypeSetClearClassFunction     (otype,HBookClearClass);
/*Class properties.*/
  OTypeAddNewClassProperty       (otype,"directory"              ,OPropertyString,NULL);
  OTypeAddNewClassProperty       (otype,"list"                   ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewClassProperty       (otype,"directories"            ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewClassProperty       (otype,"objects"                ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewClassMethodProperty (otype,"bringDirectoryInMemory" ,OPropertyVoid,NULL);
/*Object properties.*/
  OTypeAddNewProperty            (otype,"identifier"        ,OPropertyInteger,NULL);
  OTypeAddNewProperty            (otype,"name"              ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"title"             ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"dimension"         ,OPropertyInteger,NULL);
  OTypeAddNewProperty            (otype,"xAxisNumberOfBins" ,OPropertyInteger,NULL);
  OTypeAddNewProperty            (otype,"xAxisMinimum"      ,OPropertyDouble ,NULL);
  OTypeAddNewProperty            (otype,"xAxisMaximum"      ,OPropertyDouble ,NULL);
  OTypeAddNewProperty            (otype,"yAxisNumberOfBins" ,OPropertyInteger,NULL);
  OTypeAddNewProperty            (otype,"yAxisMinimum"      ,OPropertyDouble ,NULL);
  OTypeAddNewProperty            (otype,"yAxisMaximum"      ,OPropertyDouble ,NULL);
  OTypeAddNewProperty            (otype,"bins"              ,OPropertyArrayOfDoubles,NULL);
  OTypeAddNewMethodProperty      (otype,"fill"              ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"clear"             ,OPropertyVoid,NULL);

  otype                          = OTypeCreate ("HFile");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)HFileGetIdentifiers);
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)HFileDelete);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)HFileGetAttribute);
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)HFileDoMethod);
  OTypeSetClearClassFunction     (otype,HFileClearClass);
/*Object properties.*/
  OTypeAddNewProperty           (otype,"name"                   ,OPropertyString ,NULL);
  OTypeAddNewProperty           (otype,"unit"                   ,OPropertyInteger,NULL);
  OTypeAddNewProperty           (otype,"type"                   ,OPropertyString ,NULL);
  OTypeAddNewMethodProperty     (otype,"bringDirectoryInMemory" ,OPropertyVoid,NULL);

  OTypeAddLoader                ("hbook",LoadFile);
  OTypeAddLoader                ("hfile",LoadFile);
  
/*Initialize PAWC, ZUNIT.*/
  opawi();
 
}
/***************************************************************************/
static OHandle* LoadFile (
 char* a_name 
,char* a_filter
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  HFile     This;
  OHandle* hands;
/*.........................................................................*/
  This      = HFileOpenZEBRA_FileForReading (NULL,a_name);
  if(This==NULL) return NULL;
  hands     = (OHandle*)CListCreate(1);
  if(hands!=NULL) hands[0] = OHandleCreate (OTypeGetIdentifier("HFile"),This);
  a_filter  = NULL;
  return    hands;
}

