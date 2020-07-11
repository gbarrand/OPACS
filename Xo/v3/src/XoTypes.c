/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#define HAS_XOPLOTTER

#include <stddef.h>

#include <OType.h>

#include <OImage.h>

#ifdef HAS_XOPLOTTER
#include <OPlotter.h>
#endif /*HAS_XOPLOTTER*/

#include <XoTypes.h>

/***************************************************************************/
void XoSetTypes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("OImage")!=NULL) return; /*done*/

  otype                           = OTypeCreate ("OImage");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)OImageGetIdentifiers);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)OImageGetAttribute);
  OTypeSetDeleteFunction          (otype,(OTypeDeleteFunction)OImageDelete);
  OTypeSetClearClassFunction      (otype,OImageClearClass);
  OTypeAddNewProperty             (otype,"name"      ,OPropertyString,NULL);
  OTypeAddNewProperty             (otype,"fileName"  ,OPropertyString,NULL);
  OTypeAddNewProperty             (otype,"fileType"  ,OPropertyString,NULL);

#ifdef HAS_XOPLOTTER
  otype                           = OTypeCreate ("OPlotter");
  OTypeSetGetIdentifiersFunction  (otype,(OTypeGetIdentifiersFunction)OPlotterGetIdentifiers);
  OTypeSetGetAttributeFunction    (otype,(OTypeGetAttributeFunction)OPlotterGetAttribute);
  OTypeSetDoMethodFunction        (otype,(OTypeDoMethodFunction)OPlotterDoMethod);
  OTypeAddNewProperty             (otype,"name"      ,OPropertyString,NULL);
  OTypeAddNewMethodProperty       (otype,"getInformations"        ,OPropertyString,NULL);
#endif /*HAS_XOPLOTTER*/

}
