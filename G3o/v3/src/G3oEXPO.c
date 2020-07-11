/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

/*Co*/
#include <CFile.h>
#include <CMemory.h>
#include <CPrinter.h>
#include <CList.h>
#include <CString.h>
#include <OMatrix.h>
#include <OType.h>
#include <CoCommands.h>

/*Go*/
#include <GoCommands.h>
#include <OColormap.h>
#include <Go.h>

/*G3o*/
#include <GEANT.h>
#include <GoF77.h>
#include <G3oCommands.h>
#include <G3oTypes.h>

#include <G3oEXPO.h>

#ifndef HAS_NOT_U
#define g3oini g3oini_
#define nxtevt nxtevt_
#define g3oevt g3oevt_
#define ibkget ibkget_
#endif

#ifdef __cplusplus
extern "C"{
#endif
void  g3oini ();
void  nxtevt ();
void  g3oevt (int*);
int   ibkget (int*,int*);

static void    ClearClass            ();
static int     Execute_event         (int,char**,OProcess);
static int     Execute_opt           (int,char**,OProcess);

static OIdentifier* GetTOWE_Identifiers   (OType);
static int          GetTOWE_Property      (OIdentifier,OType,OProperty,void*,int*);
static int          SetTOWE_Property      (OIdentifier,OType,OProperty,void*,int);
static ONode        RepresentTOWE         (OIdentifier);

static int          GetEventProperty      (OIdentifier,OType,OProperty,void*,int*);
static int          DoEventMethod         (OIdentifier,OType,OProperty,int,char**,void*,int*);
#ifdef __cplusplus
}
#endif

static struct {
  CList extent;
  int    rep;
  double norm;
} Class = {NULL,0,1.};
/***************************************************************************/
void G3oEXPO_Initialize (
 void* a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("TOWE")!=NULL) return; /*done*/

  CoAddCommands              ((OShell)a_osh);
  GoAddCommands              ((OShell)a_osh);
  G3oAddCommands             ((OShell)a_osh);

/*G3oT types.*/
  otype                         = OTypeCreate ("TOWE");
  OTypeSetGetIdentifiersFunction (otype,GetTOWE_Identifiers);
  OTypeSetGetPropertyFunction    (otype,GetTOWE_Property);
  OTypeSetSetPropertyFunction    (otype,SetTOWE_Property);
  OTypeSetRepresentFunction      (otype,(OTypeRepresentFunction)RepresentTOWE);
  OTypeSetClearClassFunction     (otype,ClearClass);
/*Class properties.*/
  OTypeAddNewClassProperty       (otype,"norme"          ,OPropertyDouble       ,NULL);
  OTypeAddNewClassProperty       (otype,"representation" ,OPropertyString       ,NULL);
/*Object properties.*/
  OTypeAddNewProperty           (otype,"identifier" ,OPropertyUnsignedLong ,NULL);
  OTypeAddNewProperty           (otype,"layer"      ,OPropertyInteger      ,NULL);
  OTypeAddNewProperty           (otype,"slice"      ,OPropertyInteger      ,NULL);
  OTypeAddNewProperty           (otype,"tower"      ,OPropertyInteger      ,NULL);
  OTypeAddNewProperty           (otype,"energy"     ,OPropertyDouble       ,NULL);

  otype                          = OTypeCreate ("Event");
  OTypeSetGetPropertyFunction     (otype,GetEventProperty);
  OTypeSetDoPropertyFunction      (otype,DoEventMethod);
/*Class properties.*/
  OTypeAddNewClassProperty       (otype,"eventNumber"    ,OPropertyInteger      ,NULL);
  OTypeAddNewClassProperty       (otype,"runNumber"      ,OPropertyInteger      ,NULL);
  OTypeAddNewClassMethodProperty (otype,"initialize"     ,OPropertyVoid         ,NULL);
  OTypeAddNewClassMethodProperty (otype,"getNext"        ,OPropertyVoid         ,NULL);
  OTypeAddNewClassMethodProperty (otype,"get"            ,OPropertyVoid         ,NULL);

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static OIdentifier* GetTOWE_Identifiers (
 OType a_type
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int       objn;
/*.........................................................................*/
  objn               = GEANT_GetDetectorNumberOfHits ("ECAL","TOWE");
  if(objn<=0)        return NULL;
  CListDelete         (Class.extent);
  Class.extent       = CListCreate(objn);
  if(Class.extent==NULL)  return NULL;
  for(count=0;count<objn;count++) Class.extent[count] = (OIdentifier)(count+1);
  a_type             = NULL;
  return             Class.extent;
}
/***************************************************************************/
static int SetTOWE_Property (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  name;
/*.........................................................................*/
  if(This==NULL) return 0;
  name      = OPropertyGetName (a_prop);
  if(name==NULL) return 0;

  if(a_obj==NULL)
    {
       if(strcmp(name,"norme")==0)
	 {
	   Class.norm = *((double*)a_addr);
	 }
       else if(strcmp(name,"representation")==0)
	 {
	   char* string;
           string = *((char**)a_addr);
                if(strcmp(string,"geometric")==0) Class.rep = 0;
	   else if(strcmp(string,"herisson")==0)  Class.rep = 1;
	   else 
	     {
	       CWarnF ("SetTOWE_Property: bad option value %s.\n",string);
	       return 0;
	     }
	 }
       else 
	 {
	   CInfoF ("SetTOWE_Property: unknown class property %s.\n",name);
	   return 0;
	 }
      return 1;
    }

       if(strcmp(name,"identifier")==0)
    {
    }
  else if(strcmp(name,"layer")==0)
    {
    }
  else if(strcmp(name,"slice")==0)
    {
    }
  else if(strcmp(name,"tower")==0)
    {
    }
  else if(strcmp(name,"energy")==0)
    {
    }
  else 
    {
      CInfoF ("SetTOWE_Property: unknown property %s.\n",name);
      return 0;
    }

  a_number = 0;
  return   1;
}
/***************************************************************************/
static int GetTOWE_Property (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  name;
  int    iobj;
  int    vols[50];
  float  hits[50];
  int    voln,hitn;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  name      = OPropertyGetName (a_prop);
  if(name==NULL) return 0;

  if(a_obj==NULL)
    {
       if(strcmp(name,"norme")==0)
	 {
	   *((double*)a_addr) = Class.norm;
	 }
       else if(strcmp(name,"representation")==0)
	 {
	   static char* reps[2] = {"geometric","herisson"};
           if(Class.rep==1) 	   *((char**)a_addr) = reps[1];
           else                    *((char**)a_addr) = reps[0];
	 }
       else 
	 {
	   CInfoF ("GetTOWE_Property: unknown class property %s.\n",name);
	   return 0;
	 }
      return 1;
    }

  iobj      = (int)a_obj;

  GEANT_GetDetectorIthHit ("ECAL","TOWE",iobj,&voln,vols,&hitn,hits);

       if(strcmp(name,"identifier")==0)
    {
      *((int*)a_addr) = (int)a_obj;
    }
  else if(strcmp(name,"layer")==0)
    {
      *((int*)a_addr) = vols[0];
    }
  else if(strcmp(name,"slice")==0)
    {
      *((int*)a_addr) = vols[1];
    }
  else if(strcmp(name,"tower")==0)
    {
      *((int*)a_addr) = vols[2];
    }
  else if(strcmp(name,"energy")==0)
    {
      *((double*)a_addr) = hits[0];
    }
  else 
    {
      CInfoF ("GetTOWE_Property: unknown property %s.\n",name);
      return 0;
    }

  return 1;
}
/***************************************************************************/
static ONode RepresentTOWE (
 OIdentifier a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix matrix;
  int     vols[50];
  float   hits[50];
  int     voln,hitn;
  ONode gobject = NULL;
/*.........................................................................*/
  matrix = OMatrixCreate (OMatrixIdentity);

  if(GEANT_GetDetectorIthHit ("ECAL","TOWE",(int)a_obj,&voln,vols,&hitn,hits)==1)
    {
      int    layer,slice,tower;
      double z,phi,r;
       
      layer  = vols[0];
      slice  = vols[1];
      tower  = vols[2];

      z      = -200. + 10. * (slice-1);    /* z:   40 slices [-200.,200.] */
      phi    = (tower-1) * 10.;            /* phi: 36 towers [0.,360.] */
      r      = 100. + (layer-1) * 20.;     /* r:   3 layers  [100.,160.] */
          
/*
      printf ("debug:%d:%d %d %d:%g %g %g\n",voln,vols[0],vols[1],vols[2],z,phi,r);
      printf ("debug:%d %g:%d %g\n",hitn,hits[0],Class.rep,Class.norm);
*/

      if(Class.rep==0)
	{
	  gobject       = ONodeCreateF   (64,"TOWE/%d",(int)a_obj);
	  OMatrixInitialize   (matrix,OMatrixTranslation,0.,0.,z+5.);
	  GoAddGEANT_TUBS_ToNode (gobject,matrix,r,r+20.,5.,phi,phi+10.);
	}
      else if(Class.rep==1)
	{
	  double        angle,bx,by,bz,b,f,energy;
	  energy        = hits[0];
	  angle         = phi * M_PI/180.;
	  bx            = r * cos (angle);
	  by            = r * sin (angle);
	  bz            = z + 5.;
	  b             = 1. / sqrt (bx * bx + by * by);
	  f             = 1. + energy * b * Class.norm;
	  gobject       = ONodeCreateF   (64,"TOWE/%d",(int)a_obj);
	  GoAddLineToNode      (gobject,bx,by,bz,bx * f, by * f,bz);
	}
    }

  OMatrixDelete (matrix);

  return        gobject;
}
/***************************************************************************/
static int GetEventProperty (
 OIdentifier a_obj
,OType  This
,OProperty  a_prop
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

       if(strcmp(name,"eventNumber")==0)
    {
      int   iobj,iprop;
      iobj  = 1;
      iprop = 1;
      *((int*)a_addr) = ibkget(&iobj,&iprop);
    }
  else if(strcmp(name,"runNumber")==0)
    {
      int   iobj,iprop;
      iobj  = 1;
      iprop = 2;
      *((int*)a_addr) = ibkget(&iobj,&iprop);
    }
  else 
    {
      CInfoF ("GetEventProperty: unknown property %s.\n",name);
      return 0;
    }

  a_obj = NULL;
  return 1;
}
/***************************************************************************/
static int DoEventMethod (
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

  if(strcmp(name,"initialize")==0)
    {
      g3oini();
    }
  else if(strcmp(name,"getNext")==0)
    {
      nxtevt();
    }
  else if(strcmp(name,"get")==0)
    {
      int value,status;
      if(a_argn!=1)
	{
	  CWarnF ("One argument expected to execute property %s.\n",name);
	  return 0;
	}
      value = (int)CStringConvertToLong(a_args[0],&status);
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number.\n",a_args[0]);
	  return 0;
	}
      g3oevt (&value);
    }
  else 
    {
      CInfoF ("Event_ExecProperty: unknown routine property %s.\n",name);
      return 0;
    }

  a_obj  = NULL;
  a_addr = NULL;
  return 1;
}
/***************************************************************************/
static void ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDelete          (Class.extent);
  Class.extent        = NULL;
}
