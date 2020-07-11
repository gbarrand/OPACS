/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
#define DEBUG
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#include <CPrinter.h>
#include <CList.h>
#include <CString.h>
#include <CText.h>
#include <CFile.h>
#include <OType.h>
#include <CoTypes.h>

#include <OColormap.h>
#include <OCamera.h>
#include <OContextmap.h>
#include <Go.h>

#include <GoTypes.h>

#ifdef __cplusplus
extern "C"{
#endif
static OHandle*   ONodeSetRefs  (OType,OIdentifier,OProperty,char* ,OHandle*);
static OPrimitive SolveRef      (char*,OType,OHandle*);
static void       ONodeGetRefs  (OType,OIdentifier,OProperty,char**,OHandle**);
#ifdef __cplusplus
}
#endif

/***************************************************************************/
void GoSetTypes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("OContext")!=NULL) return; /*done*/

  CoSetTypes               ();

  otype                          = OTypeCreate ("OContext");
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OContextSetAttribute);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OContextGetAttribute);
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)OContextDoMethod);
  OTypeAddNewClassProperty       (otype,"color"          ,OPropertyString        ,NULL);
  OTypeAddNewClassProperty       (otype,"lineWidth"      ,OPropertyInteger       ,NULL);
  OTypeAddNewClassProperty       (otype,"markerSize"     ,OPropertyInteger       ,NULL);
  OTypeAddNewClassProperty       (otype,"lineStyle"      ,OPropertyString        ,NULL);
  OTypeAddNewClassProperty       (otype,"markerStyle"    ,OPropertyString        ,NULL);
  OTypeAddNewClassProperty       (otype,"areaStyle"      ,OPropertyString        ,NULL);
  OTypeAddNewClassProperty       (otype,"modeling"       ,OPropertyString        ,NULL);
  OTypeAddNewClassProperty       (otype,"painting"       ,OPropertyString        ,NULL);
  OTypeAddNewClassMethodProperty (otype,"setDefaults"    ,OPropertyVoid          ,NULL);

  otype                          = OTypeCreate ("OPrimitive");
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OPrimitiveSetAttribute);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OPrimitiveGetAttribute);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OPrimitiveMake);
  OTypeSetClearClassFunction     (otype,OPrimitiveClearClass);
  OTypeAddNewProperty            (otype,"identifier"     ,OPropertyUnsignedLong  ,NULL);
  OTypeAddNewProperty            (otype,"label"          ,OPropertyInteger       ,NULL);
  OTypeAddNewProperty            (otype,"type"           ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"colorIndex"     ,OPropertyInteger       ,NULL);
  OTypeAddNewProperty            (otype,"lineWidth"      ,OPropertyInteger       ,NULL);
  OTypeAddNewProperty            (otype,"markerSize"     ,OPropertyInteger       ,NULL);
  OTypeAddNewProperty            (otype,"lineStyle"      ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"markerStyle"    ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"areaStyle"      ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"numberOfPoints" ,OPropertyInteger       ,NULL);
  OTypeAddNewProperty            (otype,"points"         ,OPropertyArrayOfDoubles,NULL);

  otype                          = OTypeCreate ("ONode");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)ONodeGetIdentifiers);
  OTypeSetDeleteFunction         (otype,(OTypeDeleteFunction)ONodeDelete);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)ONodeGetAttribute);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)ONodeSetAttribute);
  OTypeSetSetReferencesFunction  (otype,(OTypeSetReferencesFunction)ONodeSetRefs);
  OTypeSetGetReferencesFunction  (otype,(OTypeGetReferencesFunction)ONodeGetRefs);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)ONodeMake);
  OTypeSetClearClassFunction     (otype,ONodeClearClass);
  OTypeAddNewProperty            (otype,"identifier"   ,OPropertyUnsignedLong   ,NULL);
  OTypeAddNewProperty            (otype,"name"         ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"highlight"    ,OPropertyBoolean        ,NULL);
  OTypeAddNewProperty            (otype,"pickable"     ,OPropertyBoolean        ,NULL);
  OTypeAddNewProperty            (otype,"visible"      ,OPropertyBoolean        ,NULL);
  OTypeAddNewProperty            (otype,"projected"    ,OPropertyBoolean        ,NULL);
  OTypeAddNewProperty            (otype,"cameraAligned",OPropertyBoolean        ,NULL);
  OTypeAddNewProperty            (otype,"color"        ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"colorIndex"   ,OPropertyInteger        ,NULL);
  OTypeAddNewProperty            (otype,"lineWidth"    ,OPropertyInteger        ,NULL);
  OTypeAddNewProperty            (otype,"markerSize"   ,OPropertyInteger        ,NULL);
  OTypeAddNewProperty            (otype,"lineStyle"    ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"markerStyle"  ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"areaStyle"    ,OPropertyString         ,NULL);
  OTypeAddNewReferencesProperty  (otype,"primitives"   ,OPropertyArrayOfStrings ,NULL);

  otype                          = OTypeCreate ("OColormap");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OColormapGetIdentifiers);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OColormapSetAttribute);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OColormapGetAttribute);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OColormapMake);
  OTypeSetClearClassFunction     (otype,OColormapClearClass);
  OTypeAddNewProperty            (otype,"identifier"      ,OPropertyUnsignedLong  ,NULL);
  OTypeAddNewProperty            (otype,"name"            ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"size"            ,OPropertyInteger       ,NULL);
  OTypeAddNewProperty            (otype,"rgbs"            ,OPropertyArrayOfDoubles,NULL);
  OTypeAddNewProperty            (otype,"names"           ,OPropertyArrayOfStrings,NULL);

  otype                          = OTypeCreate ("OCamera");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OCameraGetIdentifiers);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OCameraSetAttribute);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OCameraGetAttribute);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OCameraMake);
  OTypeSetClearClassFunction     (otype,OCameraClearClass);
/*Class properties.*/
  OTypeAddNewClassProperty       (otype,"current"            ,OPropertyString        ,NULL);
/*Object properties.*/
  OTypeAddNewProperty            (otype,"identifier"         ,OPropertyUnsignedLong  ,NULL);
  OTypeAddNewProperty            (otype,"name"               ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"colormap"           ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"width"              ,OPropertyInteger       ,NULL);
  OTypeAddNewProperty            (otype,"height"             ,OPropertyInteger       ,NULL);
  OTypeAddNewProperty            (otype,"backgroundColor"    ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"highlightContext"   ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"edgeColor"          ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"buffering"          ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"depthTreatment"     ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"projection"         ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"projectionAngle"    ,OPropertyDouble        ,NULL);
  OTypeAddNewProperty            (otype,"viewLimits"         ,OPropertyArrayOfDoubles,NULL);
  OTypeAddNewProperty            (otype,"viewPoint"          ,OPropertyArrayOfDoubles,NULL);
  OTypeAddNewProperty            (otype,"upVector"           ,OPropertyArrayOfDoubles,NULL);
  OTypeAddNewProperty            (otype,"specialProjection"  ,OPropertyString        ,NULL);
  OTypeAddNewProperty            (otype,"xcompress"          ,OPropertyDouble        ,NULL);
  OTypeAddNewProperty            (otype,"fisheye_a"          ,OPropertyDouble        ,NULL);
  OTypeAddNewProperty            (otype,"rhoz_phi1"          ,OPropertyDouble        ,NULL);
  OTypeAddNewProperty            (otype,"v_rho_max"          ,OPropertyDouble        ,NULL);
  OTypeAddNewProperty            (otype,"v_z_max"            ,OPropertyDouble        ,NULL);
  OTypeAddNewProperty            (otype,"v_k"                ,OPropertyDouble        ,NULL);

  otype                          = OTypeCreate ("OContextmap");
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)OContextmapGetIdentifiers);
  OTypeSetSetAttributeFunction   (otype,(OTypeSetAttributeFunction)OContextmapSetAttribute);
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)OContextmapGetAttribute);
  OTypeSetMakeFunction           (otype,(OTypeMakeFunction)OContextmapMake);
  OTypeSetRepresentFunction      (otype,(OTypeRepresentFunction)GoRepresentOContextmap);
  OTypeSetClearClassFunction     (otype,OContextmapClearClass);
/*Class properties.*/
  OTypeAddNewClassProperty       (otype,"current"          ,OPropertyString         ,NULL);
  OTypeAddNewClassProperty       (otype,"labelHeight"      ,OPropertyDouble         ,NULL);
  OTypeAddNewClassProperty       (otype,"labelWidthStretch",OPropertyDouble         ,NULL);
  OTypeAddNewClassProperty       (otype,"baseLineStretch"  ,OPropertyDouble         ,NULL);
  OTypeAddNewClassProperty       (otype,"labelPacking"     ,OPropertyString         ,NULL);
  OTypeAddNewClassProperty       (otype,"labelContext"     ,OPropertyString         ,NULL);
  OTypeAddNewClassProperty       (otype,"labelRotation"    ,OPropertyString         ,NULL);
  OTypeAddNewClassProperty       (otype,"iconSizeStretch"  ,OPropertyDouble         ,NULL);
/*Object properties.*/                                                            
  OTypeAddNewProperty            (otype,"identifier"       ,OPropertyUnsignedLong   ,NULL);
  OTypeAddNewProperty            (otype,"name"             ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"referenceType"    ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"referenceProperty",OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"defaultContext"   ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"defaultLabel"     ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"labelPacking"     ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"title"            ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"labelContext"     ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"labelHeight"      ,OPropertyDouble         ,NULL);
  OTypeAddNewProperty            (otype,"labelPosition"    ,OPropertyArrayOfDoubles ,NULL);
  OTypeAddNewProperty            (otype,"labelRotation"    ,OPropertyString         ,NULL);
  OTypeAddNewProperty            (otype,"iconTypes"        ,OPropertyArrayOfStrings ,NULL);
  OTypeAddNewProperty            (otype,"iconSizeStretch"  ,OPropertyDouble         ,NULL);
  OTypeAddNewProperty            (otype,"showHitsOnly"     ,OPropertyBoolean        ,NULL);
  OTypeAddNewProperty            (otype,"hits"             ,OPropertyArrayOfIntegers,NULL);
  OTypeAddNewProperty            (otype,"values"           ,OPropertyArrayOfDoubles ,NULL);
  OTypeAddNewProperty            (otype,"contexts"         ,OPropertyArrayOfStrings ,NULL);
  OTypeAddNewProperty            (otype,"labels"           ,OPropertyArrayOfStrings ,NULL);
  
  otype                          = OTypeCreate ("Go");
  OTypeSetDoMethodFunction       (otype,(OTypeDoMethodFunction)GoDoMethod);
  OTypeAddNewClassMethodProperty (otype,"representFrame" ,OPropertyString,NULL);
  OTypeAddNewClassMethodProperty (otype,"representAxis"  ,OPropertyString,NULL);
  OTypeAddNewClassMethodProperty (otype,"representTRAP"  ,OPropertyString,NULL);
}
/***************************************************************************/
void GoLoadColormapX (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OColormapGetIdentifier("ocolormap_X")!=NULL) return;
  GoSetTypes    ();
  CFileAddPath  ("GOUSR");
  OTypeLoadFile ("X11.odb",NULL);
}
/***************************************************************************/
void GoLoadColormapGradient (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OColormapGetIdentifier("ocolormap_gradient")!=NULL) return;
  GoSetTypes    ();
  CFileAddPath  ("GOUSR");
  OTypeLoadFile ("Gradient.odb",NULL);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static OHandle* ONodeSetRefs (
 OType   a_type
,OIdentifier  a_obj
,OProperty   a_prop
,char*   a_string
,OHandle*  a_hands
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  name;
  OHandle* hands = NULL;
/*.........................................................................*/
  CListConcatenate ((CList*)&hands,(CList)a_hands);

  name = OPropertyGetName(a_prop);
  if( (a_obj!=NULL) && (name!=NULL) && (strcmp(name,"primitives")==0) )
    {
       int count;
      int    stringn;
      char** strings;
      OType  type;

      strings   = CTextCreateFromString (a_string,&stringn);
      type      = OTypeGetIdentifier("OPrimitive");
      for(count=0;count<stringn;count++)
	{
          OPrimitive prim;
          prim  = SolveRef (strings[count],type,a_hands);
          if(prim!=NULL)
            {
	      ONodeAddPrimitive ((ONode)a_obj,prim);
	    }
	  else
	    {
	      CWarnF("For %s; can't solve reference %s.\n",ONodeGetName((ONode)a_obj),strings[count]);
	    }
	}
      CTextDelete (stringn,strings);
    }

  a_type = NULL;
  return hands;
}
/***************************************************************************/
static OPrimitive SolveRef (
 char*  a_string
,OType  a_type
,OHandle* a_hands
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    ident,status;
  OHandle* itema;
/*.........................................................................*/
  if(a_hands==NULL) return NULL;
  ident  = (int)CStringConvertToLong(a_string,&status);
  if(status==0) return NULL;
  for(itema=a_hands;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      OType type;
      type  = (OType)OHandleGetType  (*itema);
      if(type==a_type)
        {
	  OIdentifier identifier;
	  identifier    = (OIdentifier)OHandleGetIdentifier (*itema);
	  if(ident==OPrimitiveGetLabel((OPrimitive)identifier))
	    {
              return (OPrimitive)identifier;
	    }
	}
    }
  return NULL;
}
/***************************************************************************/
static void ONodeGetRefs (
 OType   a_type
,OIdentifier  a_obj
,OProperty   a_prop
,char**  a_string
,OHandle** a_hands
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  name;
/*.........................................................................*/
  if(a_string!=NULL) *a_string = NULL;
  if(a_hands!=NULL)  *a_hands  = NULL;

  if(a_type==NULL)  return;
  if(a_obj==NULL)   return;
  if(a_prop==NULL)  return;

  name         = OPropertyGetName(a_prop);
  if(name==NULL)    return;

  if(strcmp(name,"primitives")==0)
    {
      if(a_string!=NULL)
        {
	  OPrimitive* itema;
	  char*  string = NULL;
	  for(itema=ONodeGetPrimitives((ONode)a_obj);(itema!=NULL) && (*itema!=NULL);itema++) 
            {
	      int           ident;
	      char*         sval;
	      ident         = OTypeGenerateLabel(0);
	      sval          = CStringCreateF (64,"%d\n",ident);
              CStringConcatenate    (&string,sval);
              CStringDelete (sval);
	      OPrimitiveSetLabel (*itema,ident);
	    }
          if(string!=NULL) string[strlen(string)-1] = 0;
	  *a_string = string;
	}
      if(a_hands!=NULL)
        {
	  OType   type;
	  type    = OTypeGetIdentifier("OPrimitive");
	  if(type!=NULL)
            {
	      OHandle*  hands = NULL;
	      OPrimitive*  itema;
	      for(itema=ONodeGetPrimitives((ONode)a_obj);(itema!=NULL) && (*itema!=NULL);itema++) 
		{
		  CListAddEntry ((CList*)&hands,OHandleCreate(type,*itema));
		}
	      *a_hands = hands;
	    }
	  else
            {
              CWarn ("Type OPrimitive is unknown.\n");
	    }
	}
    }
}

