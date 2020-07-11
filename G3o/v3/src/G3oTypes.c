/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
static char what[] = "@(#)G3o v3";

#include <string.h>

/*Co*/
#include <CPrinter.h>
#include <CList.h>
#include <OType.h>
#include <CString.h>
#include <CMemory.h>
#include <CText.h>
#include <FString.h>

/*Go*/
#include <ONode.h>
#include <GoF77.h>
#include <GoTypes.h>

/*G3o*/
#include <GF77.h>
#include <GEANT.h>
#include <G3oTypes.h>

#ifdef __cplusplus
extern "C"{
#endif
static OHandle* LoadGeant   (char*,char*);
static OHandle* LoadGeantRZ (char*,char*);
static OHandle* LoadGeantRA (char*,char*);
static OHandle* LoadGeantRX (char*,char*);
static OHandle* LoadGeantFZ (char*,char*);
static OHandle* LoadGeantFA (char*,char*);
static OHandle* LoadGeantFX (char*,char*);

static void    ClearClass   ();

static int          DoEVENT_Method      (OIdentifier,OType,OProperty,int,char**,void*,int*);

static OIdentifier* GetVOLU_Identifiers (OType);
static int          GetVOLU_Property    (OIdentifier,OType,OProperty,void*,int*);
static ONode        RepresentVOLU       (OIdentifier,OType);

static OIdentifier* GetVERT_Identifiers (OType);
static ONode        RepresentVERT       (OIdentifier);
static int          GetVERT_Property    (OIdentifier,OType,OProperty,void*,int*);

static OIdentifier* GetKINE_Identifiers (OType);
static ONode        RepresentKINE       (OIdentifier);
static int          GetKINE_Property    (OIdentifier,OType,OProperty,void*,int*);

static OIdentifier* GetUDET_Identifiers (OType);
static int          GetUDET_Property    (OIdentifier,OType,OProperty,void*,int*);
static int          DoUDET_Method       (OIdentifier,OType,OProperty,int,char**,void*,int*);

static OIdentifier* GetHITS_Identifiers (OType);
static ONode        RepresentHITS       (OIdentifier,OType);
static int          GetHITS_Property    (OIdentifier,OType,OProperty,void*,int*);

static OIdentifier* GetDIGI_Identifiers (OType);
static ONode        RepresentDIGI       (OIdentifier,OType);
static int          GetDIGI_Property    (OIdentifier,OType,OProperty,void*,int*);

static char*        ConvertHollerithToString     (int);
#ifdef __cplusplus
}
#endif

typedef char* OString;

static struct {
  CList     extent;
  OProperty propName;
  int*      ints;
  double*   doubles;
  int       stringn;
  char**    strings;
  char*     types[6];
} Class = {NULL,NULL,NULL,NULL,0,NULL,
{"gamma",
 "electron",
 "neutral",
 "hadron",
 "muon",
 "geantino"}};
/***************************************************************************/
void G3oSetTypes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("VOLU")!=NULL) return; /*done*/

  GoSetTypes                          ();

  otype                          = OTypeCreate ("EVENT");
  OTypeSetDoPropertyFunction      (otype,DoEVENT_Method);
/*Class properties.*/
  OTypeAddNewClassMethodProperty (otype,"read",OPropertyVoid,NULL);


  otype                               = OTypeCreate ("VOLU");
  OTypeSetGetIdentifiersFunction      (otype,GetVOLU_Identifiers);
  OTypeSetGetPropertyFunction         (otype,GetVOLU_Property);
  OTypeSetRepresentFunction           (otype,(OTypeRepresentFunction)RepresentVOLU);
  OTypeSetClearClassFunction          (otype,ClearClass);
  OTypeAddNewProperty                 (otype,"identifier" ,OPropertyInteger         ,NULL);
  OTypeAddNewPropertyUserDataInteger  (otype,"name"       ,OPropertyString          ,1);
  OTypeAddNewPropertyUserDataInteger  (otype,"numed"      ,OPropertyInteger         ,2);
  OTypeAddNewPropertyUserDataInteger  (otype,"shape"      ,OPropertyString          ,3);
  OTypeAddNewPropertyUserDataInteger  (otype,"parn"       ,OPropertyInteger         ,4);
  OTypeAddNewPropertyUserDataInteger  (otype,"pars"       ,OPropertyArrayOfDoubles  ,5);

  otype                               = OTypeCreate ("UDET");
  OTypeSetGetIdentifiersFunction      (otype,GetUDET_Identifiers);
  OTypeSetGetPropertyFunction         (otype,GetUDET_Property);
  OTypeSetDoPropertyFunction          (otype,DoUDET_Method);
/*Class properties.*/
  OTypeAddNewClassMethodProperty      (otype,"declareHitsAndDigis" ,OPropertyVoid ,NULL);
/*Objects properties.*/
  OTypeAddNewProperty                 (otype,"identifier" ,OPropertyInteger        ,NULL);
  OTypeAddNewPropertyUserDataInteger  (otype,"name"       ,OPropertyString         ,1);
  OTypeAddNewPropertyUserDataInteger  (otype,"uset"       ,OPropertyString         ,2);
  OTypeAddNewPropertyUserDataInteger  (otype,"voln"       ,OPropertyInteger        ,3);
  OTypeAddNewPropertyUserDataInteger  (otype,"vols"       ,OPropertyArrayOfStrings ,4);

  otype                               = OTypeCreate ("VERT");
  OTypeSetGetIdentifiersFunction      (otype,GetVERT_Identifiers);
  OTypeSetGetPropertyFunction         (otype,GetVERT_Property);
  OTypeSetRepresentFunction           (otype,(OTypeRepresentFunction)RepresentVERT);
  OTypeAddNewProperty                 (otype,"identifier" ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewPropertyUserDataInteger  (otype,"x"          ,OPropertyDouble       ,1);
  OTypeAddNewPropertyUserDataInteger  (otype,"y"          ,OPropertyDouble       ,2);
  OTypeAddNewPropertyUserDataInteger  (otype,"z"          ,OPropertyDouble       ,3);

  otype                               = OTypeCreate ("KINE");
  OTypeSetGetIdentifiersFunction      (otype,GetKINE_Identifiers);
  OTypeSetGetPropertyFunction         (otype,GetKINE_Property);
  OTypeSetRepresentFunction           (otype,(OTypeRepresentFunction)RepresentKINE);
  OTypeAddNewProperty                 (otype,"identifier" ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewPropertyUserDataInteger  (otype,"type"       ,OPropertyString       ,1);
  OTypeAddNewPropertyUserDataInteger  (otype,"energy"     ,OPropertyDouble       ,2);
  OTypeAddNewPropertyUserDataInteger  (otype,"tracking"   ,OPropertyInteger      ,3);

  OTypeAddLoader                ("geant"   ,LoadGeant);
  OTypeAddLoader                ("geant_rz",LoadGeantRZ);
  OTypeAddLoader                ("geant_ra",LoadGeantRA);
  OTypeAddLoader                ("geant_rx",LoadGeantRX);
  OTypeAddLoader                ("geant_fz",LoadGeantFZ);
  OTypeAddLoader                ("geant_fa",LoadGeantFA);
  OTypeAddLoader                ("geant_fx",LoadGeantFX);

}
/***************************************************************************/
static void ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDelete        (Class.extent);
  Class.extent       = NULL;
  CMemoryFreeBlock   (Class.doubles);
  Class.doubles      = NULL;
  CMemoryFreeBlock   (Class.ints);
  Class.ints         = NULL;
  CTextDelete          (Class.stringn,Class.strings);
  Class.strings      = NULL;
  Class.stringn      = 0;
  Class.propName     = NULL;
  what[0] = '@'; /*c++ no warning.*/
}
/***************************************************************************/
static int DoEVENT_Method (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
,int a_argn
,char** a_args
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  name;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  name = OPropertyGetName (a_prop);
  if(name==NULL) return 0;

  if(strcmp(name,"read")==0)
    {
      GEANT_ReadEvent(70);
    }
  else 
    {
      CInfoF ("DoEVENT_Method: unknown routine property %s.\n",name);
      return 0;
    }

  a_obj  = NULL;
  a_addr = NULL;
  return 1;
}
/***************************************************************************/
static OIdentifier* GetVOLU_Identifiers (
 OType a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int       objn;
/*.........................................................................*/
  objn               = ogvoln ();
  if(objn<=0)        return NULL;
  CListDelete         (Class.extent);
  Class.extent       = CListCreate(objn);
  if(Class.extent==NULL)  return NULL;
  for(count=0;count<objn;count++) Class.extent[count] = (OIdentifier)(count+1);
  a_type             = NULL;
  return             Class.extent;
}
/***************************************************************************/
static int GetVOLU_Property (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  name      = OPropertyGetName (a_prop);
  if(name==NULL) return 0;
       if(strcmp(name,"identifier")==0)
    {
      *((int*)a_addr) = (int)a_obj;
    }
  else
    {
      int        ivo,iprop,ival;
      float      rval;
      FString    sval;
      float      rarr[50];
      sval       = FStringCreate (4);
      ivo        = (int)a_obj;
      iprop      = OPropertyGetUserDataInteger (a_prop);
      ogvolv     (&ivo,&iprop,&ival,&rval,sval,rarr);
      if((iprop==1) || (iprop==3))
        {
          static  char   sname[5] = "XXXX";
          char*             cname;
          cname             = FStringConvertToC (sval,4);
          CStringTrim       (cname);
          strcpy            (sname,cname);
          CStringDelete     (cname);
          *((char**)a_addr) = sname;
        }
      else if((iprop==2) || (iprop==4))
        {
          *((int*)a_addr)   = ival;
        }
      else if(iprop==5)
        {
          int       number;
          number    = ival;
          if(number>0) 
	    {
	      CMemoryFreeBlock      (Class.doubles);
	      Class.doubles = (double*)CMemoryAllocateBlock( number * sizeof(double));
	      if(Class.doubles!=NULL)
		{
		    int count;
		  for(count=0;count<number;count++) Class.doubles[count] = (double)rarr[count];
		  if(a_number!=NULL) *a_number = number;
		}
	      *((double**)a_addr) = Class.doubles;
	    }
	  else  /*It is possible, in GEANT, that a volume has been given without specifying parameters.*/
	    {
	      *((double**)a_addr) = NULL;
	    }
        }
      FStringDelete (sval);
    }
  return 1;
}
/***************************************************************************/
static ONode RepresentVOLU (
 OIdentifier a_obj
,OType This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode onode;
  char*   NAME = NULL;
/*.........................................................................*/
  if(This==NULL)  return NULL;
  if(Class.propName==NULL)
    {
      Class.propName = OTypeGetPropertyIdentifier (This,"name");
      if(Class.propName==NULL) 
	{
	  CPrinterPut (" RepVOLU: name not a property of VOLU.\n");
	  return NULL;
	}
    }

  OTypeGetObjectProperty   (This,a_obj,Class.propName,(void*)&NAME,NULL);

  GEANT_SetVolumeDrawingAttribute    ("*","SEEN",GVOL_INV_SON_INV);
  GEANT_SetVolumeVisibility          (NAME);

  onode           = ONodeCreateF   (64,"VOLU/%d",(int)a_obj);
  GoF77_SetNode   (onode);
  GEANT_RepresentVolume          (GEANT_GetTopVolume(),0,NULL,NULL);

  return            onode;
}
/***************************************************************************/
static OIdentifier* GetKINE_Identifiers (
 OType a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int       objn;
/*.........................................................................*/
  objn               = ogkinn();
  if(objn<=0)        return NULL;
  CListDelete         (Class.extent);
  Class.extent       = CListCreate(objn);
  if(Class.extent==NULL)  return NULL;
  for(count=0;count<objn;count++) Class.extent[count] = (OIdentifier)(count+1);
  a_type             = NULL;
  return             Class.extent;
}
/***************************************************************************/
static int GetKINE_Property (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
  int   iobj,iprop,ival;
  float rval;
  FString  sval;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  name      = OPropertyGetName (a_prop);
  if(name==NULL) return 0;

  iobj      = (int)a_obj;

  sval      = FStringCreate (20);

       if(strcmp(name,"identifier")==0)
    {
      *((int*)a_addr) = (int)a_obj;
    }
  else if(strcmp(name,"type")==0)
    {
                                /*12345678901234567890*/
      static  char  napart[21] = "                    ";
      char*             cval;
      iprop             = 1;
      ogkinv            (&iobj,&iprop,&ival,&rval,sval);
      cval              = FStringConvertToC (sval,20);
      CStringTrim       (cval);
      strcpy            (napart,cval);
      CStringDelete     (cval);
      *((char**)a_addr) = napart;
    }
  else if(strcmp(name,"energy")==0)
    {
      iprop              = 2;
      ogkinv             (&iobj,&iprop,&ival,&rval,sval);
      *((double*)a_addr) = (double)rval;
    }
  else if(strcmp(name,"tracking")==0)
    {
      iprop              = 3;
      ogkinv             (&iobj,&iprop,&ival,&rval,sval);
      *((int*)a_addr)    = ival;
    }
  else 
    {
      CInfoF ("GetKINE_Property: unknown property %s.\n",name);
      return 0;
    }

  FStringDelete        (sval);
  return 1;
}
/***************************************************************************/
static ONode RepresentKINE (
 OIdentifier a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode           onode;
/*.........................................................................*/
  onode                = ONodeCreateF   (64,"KINE/%d",(int)a_obj);
  GoF77_SetNode        (onode);
  GEANT_RepresentTrack ((int)a_obj);
  return               onode;
}
/***************************************************************************/
static OIdentifier* GetVERT_Identifiers (
 OType a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int       objn;
/*.........................................................................*/
  objn               = ogvern();
  if(objn<=0)        return NULL;
  CListDelete        (Class.extent);
  Class.extent       = CListCreate(objn);
  if(Class.extent==NULL)  return NULL;
  for(count=0;count<objn;count++) Class.extent[count] = (OIdentifier)(count+1);
  a_type             = NULL;
  return             Class.extent;
}
/***************************************************************************/
static int GetVERT_Property (
 OIdentifier a_obj
,OType       This
,OProperty   a_prop
,void*       a_addr 
,int*        a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*    name;
  int      iobj,iprop,ival;
  float    rval;
  FString  sval;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  name      = OPropertyGetName (a_prop);
  if(name==NULL) return 0;

  iobj      = (int)a_obj;

  sval      = FStringCreate (20);

       if(strcmp(name,"identifier")==0)
    {
      *((int*)a_addr) = (int)a_obj;
    }
  else if(strcmp(name,"x")==0)
    {
      iprop              = 1;
      ogverv             (&iobj,&iprop,&ival,&rval,sval);
      *((double*)a_addr) = (double)rval;
    }
  else if(strcmp(name,"y")==0)
    {
      iprop              = 2;
      ogverv             (&iobj,&iprop,&ival,&rval,sval);
      *((double*)a_addr) = (double)rval;
    }
  else if(strcmp(name,"z")==0)
    {
      iprop              = 3;
      ogverv             (&iobj,&iprop,&ival,&rval,sval);
      *((double*)a_addr) = (double)rval;
    }
  else 
    {
      CInfoF ("GetVERT_Property: unknown property %s.\n",name);
      return 0;
    }

  FStringDelete (sval);
  return        1;
}
/***************************************************************************/
static ONode RepresentVERT (
 OIdentifier a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode           onode;
/*.........................................................................*/
  onode                 = ONodeCreateF   (64,"VERT/%d",(int)a_obj);
  GoF77_SetNode         (onode);
  GEANT_RepresentVertex ((int)a_obj);
  return                onode;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static OIdentifier* GetUDET_Identifiers (
 OType a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int       objn;
/*.........................................................................*/
  objn               = ogdetn ();
  if(objn<=0)        return NULL;
  CListDelete         (Class.extent);
  Class.extent       = CListCreate(objn);
  if(Class.extent==NULL)  return NULL;
  for(count=0;count<objn;count++) Class.extent[count] = (OIdentifier)(count+1);
  a_type             = NULL;
  return             Class.extent;
}
/***************************************************************************/
static int GetUDET_Property (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  name      = OPropertyGetName (a_prop);
  if(name==NULL) return 0;

  if(a_obj==NULL) return 0;

       if(strcmp(name,"identifier")==0)
    {
      *((int*)a_addr) = (int)a_obj;
    }
  else
    {
      int        iobj,iprop,ival;
      float      rval;
      FString    sval;
      int        iarr[50];
      sval       = FStringCreate (4);
      iobj       = (int)a_obj;
      iprop      = OPropertyGetUserDataInteger (a_prop);
      ogdetv     (&iobj,&iprop,&ival,&rval,sval,iarr);
      if( (iprop==1) || (iprop==2))
        {
          static  char   sname[5] = "XXXX";
          char*             cname;
          cname             = FStringConvertToC (sval,4);
          CStringTrim       (cname);
          strcpy            (sname,cname);
          CStringDelete     (cname);
          *((char**)a_addr) = sname;
        }
      else if(iprop==3)
        {
          *((int*)a_addr)   = ival;
        }
      else if(iprop==4)
        {
          int       number;
          number    = ival;
          if(number>0) 
	    {
	      int           count;
	      CTextDelete     (Class.stringn,Class.strings);
	      Class.strings = (OString*)CMemoryAllocateBlock(ival * sizeof(OString));
	      Class.stringn = ival;
	      for(count=0;count<ival;count++)
		{
		  int         iname;
		  int         lname = 4;
		  char*       cname;
		  iname       = iarr[count];
		  uhtoc       (&iname,&lname,sval,&lname);
		  cname       = FStringConvertToC (sval,4);
		  CStringTrim (cname);
		  Class.strings[count] = cname;
		}
	      if(a_number!=NULL)  *a_number = Class.stringn;
	      *((OString**)a_addr) = Class.strings;
	    }
	  else
	    {
	      if(a_number!=NULL)  *a_number = 0;
	      *((OString**)a_addr) = NULL;
	    }
        }
      else 
	{
	  CInfoF("GetUDET_Property: %d bad user property.\n",iprop);
	  return 0;
	 }
      FStringDelete (sval);
    }
  return 1;
}
/***************************************************************************/
static int DoUDET_Method (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
,int a_argn
,char** a_args
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  name      = OPropertyGetName (a_prop);
  if(name==NULL) return 0;

  if(a_obj==NULL) 
    {
      if(strcmp(name,"declareHitsAndDigis")==0) 
	{
	  G3oDeclareUDETsHitsAndDIGIs ();
	}
      else 
	{
	  CInfoF("UDET_ExecProperty: %s is not a class routine property.\n",name);
	  return 0;
	 }
      return 1;
    }

  a_argn = 0;
  a_args = NULL;
  a_addr = NULL;
  return 0;
}
/***************************************************************************/
void G3oDeclareUDETsHitsAndDIGIs (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int     objn;
  OType   type;
  OProperty   prop_name,prop_uset;
/*.........................................................................*/
  objn                = ogdetn ();
  if(objn<=0)         return;

  type                = OTypeGetIdentifier ("UDET");
  if(type==NULL)      return;
  prop_name           = OTypeGetPropertyIdentifier (type,"name");
  if(prop_name==NULL) return;
  prop_uset           = OTypeGetPropertyIdentifier (type,"uset");
  if(prop_uset==NULL) return;

  for(count=0;count<objn;count++) 
    {
      OIdentifier obj;
      char*  NAME = NULL;
      char*  USET = NULL;
      char*  name;

      obj                    = (OIdentifier)(count+1);
      OTypeGetObjectProperty (type,obj,prop_uset,(void*)&NAME,NULL);
      USET                   = CStringDuplicate(NAME);
      OTypeGetObjectProperty (type,obj,prop_name,(void*)&NAME,NULL);

      name                   = CStringCreateF (strlen(USET)+strlen(NAME)+6,"HITS_%s_%s",USET,NAME);
      if(OTypeGetIdentifier(name)==NULL) 
	{
	  OType                              otype;
	  otype                              = OTypeCreate (name);
	  OTypeSetGetIdentifiersFunction     (otype,(OTypeGetIdentifiersFunction)GetHITS_Identifiers);
	  OTypeSetRepresentFunction          (otype,(OTypeRepresentFunction)RepresentHITS);
	  OTypeSetGetPropertyFunction        (otype,(OTypeGetPropertyFunction)GetHITS_Property);
	  OTypeAddNewProperty                (otype,"identifier"     ,OPropertyInteger ,NULL);
	  OTypeAddNewPropertyUserDataInteger (otype,"voln"  ,OPropertyInteger         ,1);
	  OTypeAddNewPropertyUserDataInteger (otype,"vols"  ,OPropertyArrayOfIntegers ,2);
	  OTypeAddNewPropertyUserDataInteger (otype,"parn"  ,OPropertyInteger         ,3);
	  OTypeAddNewPropertyUserDataInteger (otype,"pars"  ,OPropertyArrayOfDoubles  ,4);
	}
      CStringDelete          (name);

      name                   = CStringCreateF (strlen(USET)+strlen(NAME)+6,"DIGI_%s_%s",USET,NAME);
      if(OTypeGetIdentifier(name)==NULL) 
	{
	  OType                              otype;
	  otype                              = OTypeCreate (name);
	  OTypeSetGetIdentifiersFunction     (otype,(OTypeGetIdentifiersFunction)GetDIGI_Identifiers);
	  OTypeSetRepresentFunction          (otype,(OTypeRepresentFunction)RepresentDIGI);
	  OTypeSetGetPropertyFunction        (otype,(OTypeGetPropertyFunction)GetDIGI_Property);
	  OTypeAddNewProperty                (otype,"identifier" ,OPropertyInteger ,NULL);
	  OTypeAddNewPropertyUserDataInteger (otype,"voln"  ,OPropertyInteger         ,1);
	  OTypeAddNewPropertyUserDataInteger (otype,"vols"  ,OPropertyArrayOfIntegers ,2);
	  OTypeAddNewPropertyUserDataInteger (otype,"parn"  ,OPropertyInteger         ,3);
	  OTypeAddNewPropertyUserDataInteger (otype,"pars"  ,OPropertyArrayOfIntegers ,4);
	}
      CStringDelete          (name);

      CStringDelete          (USET);

    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static OIdentifier* GetHITS_Identifiers (
  OType This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int       objn;
  char*     uset;
  char*     udet;
  char*     pos;
/*.........................................................................*/
  uset               = CStringDuplicate(OTypeGetName(This)+5);
  pos                = strchr(uset,'_');
  if(pos==NULL)      {CStringDelete(uset);return NULL;}
  *pos               = '\0';
  udet               = pos + 1;

  objn               = GEANT_GetDetectorNumberOfHits (uset,udet);
  CStringDelete      (uset);

  if(objn<=0)        return NULL;
  CListDelete        (Class.extent);
  Class.extent       = CListCreate(objn);
  if(Class.extent==NULL)  return NULL;
  for(count=0;count<objn;count++) Class.extent[count] = (OIdentifier)(count+1);
  return             Class.extent;
}
/***************************************************************************/
static int GetHITS_Property (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  name      = OPropertyGetName (a_prop);
  if(name==NULL) return 0;
       if(strcmp(name,"identifier")==0)
    {
      *((int*)a_addr) = (int)a_obj;
    }
  else
    {
      char*   uset;
      char*   udet;
      char*   pos;
      int     vols[50];
      float   hits[50];
      int     voln,hitn;
      
      uset               = CStringDuplicate(OTypeGetName(This)+5);
      pos                = strchr(uset,'_');
      if(pos==NULL)      
	{
	  CStringDelete(uset);
	  return 0;
	}
      *pos               = '\0';
      udet               = pos + 1;

      if(GEANT_GetDetectorIthHit (uset,udet,(int)a_obj,&voln,vols,&hitn,hits)==1)
	{
	  int   iprop;
	  iprop = OPropertyGetUserDataInteger (a_prop);
	  if(iprop==1)
	    {
	      *((int*)a_addr) = voln;
	    }
	  else if(iprop==2)
	    {
	      int       number;
	      number    = voln;
	      if(number>0) 
		{
		  CMemoryFreeBlock   (Class.ints);
		  Class.ints = (int*)CMemoryAllocateBlock( number * sizeof(int));
		  if(Class.ints!=NULL)
		    {
		        int count;
		      for(count=0;count<number;count++) Class.ints[count] = vols[count];
		      if(a_number!=NULL) *a_number = number;
		    }
		  *((int**)a_addr) = Class.ints;
		}
	      else
		{
		  *((int**)a_addr) = NULL;
		}
	    }
	  else if(iprop==3)
	    {
	      *((int*)a_addr) = hitn;
	    }
	  else if(iprop==2)
	    {
	      int       number;
	      number    = hitn;
	      if(number>0) 
		{
		  CMemoryFreeBlock      (Class.doubles);
		  Class.doubles = (double*)CMemoryAllocateBlock( number * sizeof(double));
		  if(Class.doubles!=NULL)
		    {
		        int count;
		      for(count=0;count<number;count++) Class.doubles[count] = (double)hits[count];
		      if(a_number!=NULL) *a_number = number;
		    }
		  *((double**)a_addr) = Class.doubles;
		}
	      else
		{
		  *((double**)a_addr) = NULL;
		}
	    }
	}
      CStringDelete (uset);
    }
  return 1;
}
/***************************************************************************/
static ONode RepresentHITS (
 OIdentifier a_obj
,OType  a_type
)
/***************************************************************************/
/*Assume that a VOLU exists with name UDET                                 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*   uset;
  char*   udet;
  char*   pos;

  int     vols[50];
  float   hits[50];
  int     voln,hitn;
  ONode onode = NULL;
/*.........................................................................*/
  uset               = CStringDuplicate(OTypeGetName(a_type)+5);
  pos                = strchr(uset,'_');
  if(pos==NULL)      {CStringDelete(uset);return NULL;}
  *pos               = '\0';
  udet               = pos + 1;

  if(GEANT_GetDetectorIthHit (uset,udet,(int)a_obj,&voln,vols,&hitn,hits)==1)
    {
      int                             dvols[50];
      char*                           name;
      GEANT_GetDetectorVolumes        (uset,udet,NULL,dvols);
      onode                           = ONodeCreateF (64,"%s/%d",OTypeGetName(a_type),(int)a_obj);
      GEANT_SetVolumeDrawingAttribute ("*","SEEN",GVOL_INV_SON_INV);
      name                            = ConvertHollerithToString(dvols[voln-1]); /*Assume that last volume is the hit volume !!!*/
      GEANT_SetVolumeVisibility       (name);
      CStringDelete                   (name);
      GoF77_SetNode                   (onode);
      if(GEANT_RepresentVolume(GEANT_GetTopVolume(),voln,vols,dvols)==0)
	{
	  GEANT_SetVolumeDrawingAttribute ("*","SEEN",GVOL_VIS_SON_VIS);
	  if(GEANT_RepresentVolume (GEANT_GetTopVolume(),voln,vols,dvols)==0)
	    {
               int count;
              CWarnF ("Can't draw volume:\n");
              for(count=0;count<voln;count++)
                {
                  name          = ConvertHollerithToString(dvols[count]);
                  CWarnF        ("%s %d\n",name,vols[count]);
                  CStringDelete (name);
                }
	    }
	}
    }

  CStringDelete (uset);

  return        onode;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static OIdentifier* GetDIGI_Identifiers (
  OType This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int       objn;
  char*     uset;
  char*     udet;
  char*     pos;
/*.........................................................................*/
  uset               = CStringDuplicate(OTypeGetName(This)+5);
  pos                = strchr(uset,'_');
  if(pos==NULL)      {CStringDelete(uset);return NULL;}
  *pos               = '\0';
  udet               = pos + 1;

  objn               = GEANT_GetDetectorNumberOfDIGIs (uset,udet);
  CStringDelete      (uset);

  if(objn<=0)            return NULL;
  CListDelete            (Class.extent);
  Class.extent           = CListCreate(objn);
  if(Class.extent==NULL) return NULL;
  for(count=0;count<objn;count++) Class.extent[count] = (OIdentifier)(count+1);
  return                 Class.extent;
}
/***************************************************************************/
static int GetDIGI_Property (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  name      = OPropertyGetName (a_prop);
  if(name==NULL) return 0;
       if(strcmp(name,"identifier")==0)
    {
      *((int*)a_addr) = (int)a_obj;
    }
  else
    {
      char*   uset;
      char*   udet;
      char*   pos;
      int     vols[50];
      int     digs[50];
      int     voln,dign;
      
      uset    = CStringDuplicate(OTypeGetName(This)+5);
      pos     = strchr(uset,'_');
      if(pos==NULL)      
	{
	  CStringDelete(uset);
	  return 0;
	}
      *pos    = '\0';
      udet    = pos + 1;

      if(GEANT_GetDetectorIthDIGI (uset,udet,(int)a_obj,&voln,vols,&dign,digs)==1)
	{
	  int   iprop;
	  iprop = OPropertyGetUserDataInteger (a_prop);
	  if(iprop==1)
	    {
	      *((int*)a_addr) = voln;
	    }
	  else if(iprop==2)
	    {
	      int       number;
	      number    = voln;
	      if(number>0) 
		{
		  CMemoryFreeBlock   (Class.ints);
		  Class.ints = (int*)CMemoryAllocateBlock( number * sizeof(int));
		  if(Class.ints!=NULL)
		    {
		        int count;
		      for(count=0;count<number;count++) Class.ints[count] = vols[count];
		      if(a_number!=NULL) *a_number = number;
		    }
		  *((int**)a_addr) = Class.ints;
		}
	      else
		{
		  *((int**)a_addr) = NULL;
		}
	    }
	  else if(iprop==3)
	    {
	      *((int*)a_addr) = dign;
	    }
	  else if(iprop==4)
	    {
	      int       number;
	      number    = dign;
	      if(number>0) 
		{
		  CMemoryFreeBlock   (Class.ints);
		  Class.ints = (int*)CMemoryAllocateBlock( number * sizeof(int));
		  if(Class.ints!=NULL)
		    {
		        int count;
		      for(count=0;count<number;count++) Class.ints[count] = digs[count];
		      if(a_number!=NULL) *a_number = number;
		    }
		  *((int**)a_addr) = Class.ints;
		}
	      else
		{
		  *((int**)a_addr) = NULL;
		}
	    }
	}
      CStringDelete (uset);
    }
  return 1;
}
/***************************************************************************/
static ONode RepresentDIGI (
 OIdentifier a_obj
,OType  a_type
)
/***************************************************************************/
/*Assume that a VOLU exists with name UDET                                 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*   uset;
  char*   udet;
  char*   pos;

  int     vols[50];
  int     digs[50];
  int     voln,dign;
  ONode   onode = NULL;
/*.........................................................................*/
  uset               = CStringDuplicate(OTypeGetName(a_type)+5);
  pos                = strchr(uset,'_');
  if(pos==NULL)      {CStringDelete(uset);return NULL;}
  *pos               = '\0';
  udet               = pos + 1;

  if(GEANT_GetDetectorIthDIGI (uset,udet,(int)a_obj,&voln,vols,&dign,digs)==1)
    {
      int                             dvoln;
      int                             dvols[50];
      char*                           name;
      GEANT_GetDetectorVolumes        (uset,udet,&dvoln,dvols);
      /*
      {int count;
      printf ("debug : RepresentDIGI : %s %s %d\n",uset,udet,dvoln);
      for(count=0;count<dvoln;count++) 
	{
	  name                            = ConvertHollerithToString(dvols[count]); 
	  printf ("debug : RepresentDIGI : %d %s\n",count,name);
	  CStringDelete                   (name);
	}
      printf ("debug : RepresentDIGI : indexes : %s %s %d\n",uset,udet,voln);
      for(count=0;count<voln;count++) 
	{
	  printf ("debug : RepresentDIGI : %d %d\n",count,vols[count]);
	}
      }
      */

      onode                           = ONodeCreateF (64,"%s/%d",OTypeGetName(a_type),(int)a_obj);
      GEANT_SetVolumeDrawingAttribute ("*","SEEN",GVOL_INV_SON_INV);
/*Assume that last volume is the hit volume !!!*/
      name                            = ConvertHollerithToString(dvols[voln-1]); 
      /*      printf ("debug : RepresentDIGI : %s\n",name);*/
      GEANT_SetVolumeVisibility       (name);
      CStringDelete                   (name);
      GoF77_SetNode                   (onode);
      if(GEANT_RepresentVolume(GEANT_GetTopVolume(),voln,vols,dvols)==0)
	{
	  GEANT_SetVolumeDrawingAttribute ("*","SEEN",GVOL_VIS_SON_VIS);
	  if(GEANT_RepresentVolume        (GEANT_GetTopVolume(),voln,vols,dvols)==0)
	    {
               int count;
              CWarnF ("Can't draw volume:\n");
              for(count=0;count<voln;count++)
                {
                  name          = ConvertHollerithToString(dvols[count]);
                  CWarnF        ("%s %d\n",name,vols[count]);
                  CStringDelete (name);
                }
	    }
	}
    }

  CStringDelete (uset);

  return        onode;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static OHandle* LoadGeant (
 char* a_name 
,char* a_filter
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int type = 0;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
       if( (strstr(a_name,".rz")!=NULL) || (strstr(a_name,".RZ")!=NULL) ) type = GFILE_RZ;
  else if( (strstr(a_name,".ra")!=NULL) || (strstr(a_name,".RA")!=NULL) ) type = GFILE_RA;
  else if( (strstr(a_name,".rx")!=NULL) || (strstr(a_name,".RX")!=NULL) ) type = GFILE_RX;
  else if( (strstr(a_name,".fz")!=NULL) || (strstr(a_name,".FZ")!=NULL) ) type = GFILE_FZ;
  else if( (strstr(a_name,".fa")!=NULL) || (strstr(a_name,".FA")!=NULL) ) type = GFILE_FA;
  else if( (strstr(a_name,".fx")!=NULL) || (strstr(a_name,".FX")!=NULL) ) type = GFILE_FX;
  else
    {
      CWarnF ("%s: bad file type.\n",a_name);
      return NULL; 
    }
  if(type!=0) GEANT_LoadGeometry (type,a_name,70);
  a_filter = NULL;
  return   NULL;
}
/***************************************************************************/
static OHandle* LoadGeantRZ (
 char* a_name 
,char* a_filter
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  GEANT_LoadGeometry (GFILE_RZ,a_name,70);
  a_filter = NULL;
  return   NULL;
}
/***************************************************************************/
static OHandle* LoadGeantRA (
 char* a_name 
,char* a_filter
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  GEANT_LoadGeometry (GFILE_RA,a_name,70);
  a_filter = NULL;
  return   NULL;
}
/***************************************************************************/
static OHandle* LoadGeantRX (
 char* a_name 
,char* a_filter
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  GEANT_LoadGeometry (GFILE_RX,a_name,70);
  a_filter = NULL;
  return   NULL;
}
/***************************************************************************/
static OHandle* LoadGeantFZ (
 char* a_name 
,char* a_filter
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  GEANT_LoadGeometry (GFILE_FZ,a_name,70);
  a_filter = NULL;
  return   NULL;
}
/***************************************************************************/
static OHandle* LoadGeantFA (
 char* a_name 
,char* a_filter
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  GEANT_LoadGeometry (GFILE_FA,a_name,70);
  a_filter = NULL;
  return   NULL;
}
/***************************************************************************/
static OHandle* LoadGeantFX (
 char* a_name 
,char* a_filter
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  GEANT_LoadGeometry (GFILE_FX,a_name,70);
  a_filter = NULL;
  return   NULL;
}
/***************************************************************************/
static char* ConvertHollerithToString (
 int a_hollerith
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString         sval;
  int          iname;
  int          lname = 4;
  char*        cname;
/*.........................................................................*/
  sval          = FStringCreate (4);
  iname         = a_hollerith;
  uhtoc         (&iname,&lname,sval,&lname);
  cname         = FStringConvertToC (sval,4);
  CStringTrim   (cname);
  FStringDelete (sval);
  return        cname;
}






