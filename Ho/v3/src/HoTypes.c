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

/*Co*/
#include <CList.h>
#include <OProcess.h>
#include <OType.h>
#include <CoTypes.h>
/*Ho*/
#include <OHistogram.h>
#include <OTuple.h>
#include <OCut.h>
#include <Ho.h>

#include <HoTypes.h>

#ifdef __cplusplus
extern "C"{
#endif
static OHandle* LoadTuple (char*,char*);
#ifdef __cplusplus
}
#endif

/***************************************************************************/
void HoSetTypes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("OHistogram")!=NULL) return; /*done*/

  CoSetTypes               ();

  otype                          = OTypeCreate ("OHistogram");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OHistogramGetIdentifiers);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OHistogramGetAttribute);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OHistogramSetAttribute);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OHistogramMake);
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)OHistogramDelete);
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)OHistogramDoMethod);
  OTypeSetClearClassFunction     (otype,OHistogramClearClass);
  OTypeAddNewProperty            (otype,"identifier",OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty            (otype,"name"      ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"dimension" ,OPropertyInteger   ,NULL);
  OTypeAddNewProperty            (otype,"title"     ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"xAxisTitle"        ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"xAxisNumberOfBins" ,OPropertyInteger,NULL);
  OTypeAddNewProperty            (otype,"xAxisMinimum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"xAxisMaximum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"yAxisTitle"        ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"yAxisNumberOfBins" ,OPropertyInteger,NULL);
  OTypeAddNewProperty            (otype,"yAxisMinimum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"yAxisMaximum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"zAxisTitle"        ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"zAxisNumberOfBins" ,OPropertyInteger,NULL);
  OTypeAddNewProperty            (otype,"zAxisMinimum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"zAxisMaximum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"monitoringMode"    ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"profile"           ,OPropertyBoolean,NULL);
  OTypeAddNewProperty            (otype,"vAxisTitle"        ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"bins"              ,OPropertyArrayOfDoubles,NULL);
  OTypeAddNewMethodProperty      (otype,"fill"                 ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"fillRandom"           ,OPropertyVoid,NULL);
  OTypeAddNewMethodProperty      (otype,"clear"                ,OPropertyVoid,NULL);
  
  otype                          = OTypeCreate ("OScatter");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OScatterGetIdentifiers);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OScatterGetAttribute);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OScatterSetAttribute);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OScatterMake);
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)OScatterDelete);
  OTypeSetClearClassFunction     (otype,OScatterClearClass);
  OTypeAddNewProperty            (otype,"identifier",OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty            (otype,"name"      ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"dimension" ,OPropertyInteger   ,NULL);
  OTypeAddNewProperty            (otype,"title"     ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"xAxisMinimum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"xAxisMaximum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"yAxisMinimum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"yAxisMaximum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"zAxisMinimum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"zAxisMaximum"      ,OPropertyDouble,NULL);
  OTypeAddNewProperty            (otype,"numberOfEntries"   ,OPropertyInteger   ,NULL);
  OTypeAddNewProperty            (otype,"points"            ,OPropertyArrayOfDoubles,NULL);
  
  otype                          = OTypeCreate ("OTuple");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OTupleGetIdentifiers);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OTupleGetAttribute);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OTupleSetAttribute);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OTupleMake);
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)OTupleDelete);
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)OTupleDoMethod);
  OTypeSetClearClassFunction     (otype,OTupleClearClass);
  OTypeAddNewProperty            (otype,"identifier"      ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty            (otype,"name"            ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"title"           ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"fileName"        ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"numberOfColumns" ,OPropertyInteger   ,NULL);
  OTypeAddNewProperty            (otype,"numberOfRows"    ,OPropertyInteger   ,NULL);
  OTypeAddNewProperty            (otype,"labels"          ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewProperty            (otype,"values"          ,OPropertyArrayOfDoubles,NULL);
  OTypeAddNewMethodProperty      (otype,"getInformations"                 ,OPropertyString,NULL);

  otype                          = OTypeCreate ("OCut");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OCutGetIdentifiers);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OCutGetAttribute);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OCutSetAttribute);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OCutMake);
  OTypeSetClearClassFunction     (otype,OCutClearClass);
  OTypeAddNewProperty            (otype,"identifier" ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty            (otype,"name"       ,OPropertyString,NULL);
  OTypeAddNewProperty            (otype,"script"     ,OPropertyString,NULL);

  otype                          = OTypeCreate ("Ho");
  OTypeSetClearClassFunction     (otype,HoClearClass);

  OTypeAddLoader                 ("otuple",LoadTuple);
  OTypeAddLoader                 ("otu"   ,LoadTuple);
  OTypeAddLoader                 ("mo"    ,LoadTuple);

}
/***************************************************************************/
static OHandle* LoadTuple (
 char* a_name 
,char* a_filter
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OTuple    This;
  OHandle*    hands;
/*.........................................................................*/
  This      = OTupleLoadFile (a_name);
  if(This==NULL) return NULL;
  hands     = (OHandle*)CListCreate(1);
  if(hands!=NULL) 
    hands[0] = OHandleCreate (OTypeGetIdentifier("OTuple"),This);
  a_filter = NULL;
  return    hands;
}
