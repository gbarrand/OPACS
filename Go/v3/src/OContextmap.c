/*
#define debug
*/
#ifdef debug
#define DEBUG(s) printf s
#else
#define DEBUG(s)
#endif

#include <stdarg.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

/*Co*/
#include <CMemory.h>
#include <CFile.h>
#include <CPrinter.h>
#include <CList.h>
#include <CMath.h>
#include <CString.h>
#include <CText.h>
#include <OMatrix.h>
#include <OType.h>
#include <CError.h>
#include <OColor.h>

/*Go*/
#include <Go.h>

typedef char*         OString;

#include <OContextmapP.h>

typedef unsigned long Ulong;
typedef unsigned char Uchar;

#ifdef __cplusplus
extern "C"{
#endif
static void      InitializeClass         ();
static void      FreeRecord              (OContextmap);
static int       GetIdentifier           (OContextmap,char*);
static void      FreeContexts            (OContextmap);
static int       CreateContexts          (OContextmap,int,char**);
static void      UpdateContexts          (OContextmap);
static void      AddIconName             (OContextmap,OString);
static int       IsIconValid             (OContextmap,int);

static void      *OContextmapRepresent   (OIdentifier,OType);

static int       IconGetSize             ();
static OString*  IconGetNames            (int*);
static OString   IconGetName             (int);

static OMatrix   CreateRotationMatrixFromScript (OString);
static void      ReportError                    (char*,int);
#ifdef __cplusplus
}
#endif

#define ICON_TYPE_MAX           5 
#define ICON_DEFAULT            ICON_MARKER 

#define NotFound                (-1)
#define DBL_INT                 (int)(float)
#define CHECK_DIV(a_var,a_pro)  {if((a_var)==0.) {ReportError(a_pro,CErrorValue);(a_var)=1.;}}
#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)
#define OContextmapName(This)   (This->name!=NULL ? This->name:"(nil)")
#define ONumber(arr)            (sizeof(arr) / sizeof(arr[0]))
#define STRING_DELETE(s)        {CStringDelete(s); s=NULL;}
#define TEXT_DELETE(l,s)        {CTextDelete(l,s); s=NULL;}
#define MATRIX_DELETE(m)        {OMatrixDelete(m); m=NULL;}
#define MEMORY_FREE(m)          {CMemoryFreeBlock(m); m=NULL;}

static char* IconTypeName[] = {"marker","line","circle","square","diamond"};

static struct 
{
  int           init;
  OContextmap*  extent;
  OContextmap   current;
  double        labelHeight;
  double        labelWidthStretch;
  double        baseLineStretch;
  OPacking      labelPacking;
  OString       labelContextScript;
  double        iconSizeStretch;
  OString       labelRotationScript;
  OMatrix       labelRotationMatrix;
} Class = {     0          
	       ,NULL
	       ,NULL
	       ,10.0
	       ,0.8
	       ,1.3
	       ,OPackingHorizontal
	       ,NULL
	       ,0.8
	       ,NULL
	       ,NULL
};
/***************************************************************************/
static void InitializeClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==1) return; /* already done */

  Class.labelContextScript = CStringDuplicate("color black"); 

  Class.init               = 1;
}
/***************************************************************************/
void OContextmapClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==0) return;

  STRING_DELETE    (Class.labelContextScript);
  STRING_DELETE    (Class.labelRotationScript);
  MATRIX_DELETE    (Class.labelRotationMatrix);

  CListDestroy     ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent     = NULL;

  Class.init       = 0;
}
/***************************************************************************/
OContextmap* OContextmapGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OContextmap OContextmapGetIdentifier (
 char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"OContextmap",11)==0) )
    {
      int        status;
      OContextmap    identifier;
      identifier = (OContextmap)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return (OContextmap)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,pos+1);
	}
      else
	{
	  if(OContextmapIsValid(identifier)==0)
	    {
	      return NULL;
	    }
	  else
	    {
	      return identifier;
	    }
	}
    }
  else
    {
      return (OContextmap)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
    }
}
/***************************************************************************/
static int GetIdentifier (
 OContextmap This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OContextmapGetName(This),a_name);
}
/***************************************************************************/
int OContextmapIsValid (
 OContextmap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
OContextmap OContextmapGetCurrent (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.current;
}
/***************************************************************************/
void OContextmapSetCurrent (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.current = This;
}
/***************************************************************************/
double OContextmapGetBaseLineStretch (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.baseLineStretch;
}
/***************************************************************************/
double OContextmapGetLabelWidthStretch (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.labelWidthStretch;
}
/***************************************************************************/
OContextmap OContextmapMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OContextmap This;
/*.........................................................................*/
  InitializeClass          ();
  This                     = (OContextmap) CMemoryAllocateBlock (sizeof(OContextmapRecord));
  if(This==NULL)           return NULL;

  CListAddEntry            ((CList*)&(Class.extent),This);

  This->name                 = NULL;
  This->referenceType        = NULL;
  This->referenceProperty    = NULL;
  This->referencePropertyType= 0;
  This->defaultContextScript = NULL;
  This->defaultLabel         = NULL;
  This->labelPacking         = OPackingUndefined;
  This->title                = NULL;
  This->labelContextScript   = NULL;
  This->labelHeight          = 0.;
  This->labelPositionX       = 0.;
  This->labelPositionY       = 0.;
  This->labelPositionZ       = 0.;
  This->labelRotationScript  = NULL;
  This->labelRotationMatrix  = NULL;
  This->iconSizeStretch      = 0.;
  This->iconTypes            = ICON_DEFAULT;
  This->showHitsOnly         = 0;

  This->contextn             = 0;
  This->contexts             = NULL;
  This->hits                 = NULL;
  This->contextsScript       = NULL;

  This->valuen               = 0;
  This->values               = NULL;
  This->labeln               = 0;
  This->labels               = NULL;

  return This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OContextmapDelete (
 OContextmap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  CListRemoveEntry ((CList)Class.extent,This);
  FreeRecord       (This);
}
/***************************************************************************/
void OContextmapSetObjectContext (
 OContextmap  This
,OType        a_type
,OIdentifier  a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  double value;
  int    found;
/*.........................................................................*/
  if(This==NULL)                  return;  
  if(This->values==NULL)          return;
  if(a_type!=This->referenceType) return;

  if(This->referencePropertyType==OPropertyDouble)
    {
      OTypeGetObjectProperty (a_type,a_obj,This->referenceProperty,(void*)&value,0);
    }
  else if(This->referencePropertyType==OPropertyInteger)
    {
      int                    ivalue;
      OTypeGetObjectProperty (a_type,a_obj,This->referenceProperty,(void*)&ivalue,0);
      value                  = (double)ivalue;
    }
  found = 0;
  for (count=0;count<This->valuen;count++)
    {
      if (value < This->values[count])
	{
	  DEBUG( ("OContextmapSetContext : OType %s : prop %s : value %g : context %g %s\n",
		  OTypeGetName     (This->referenceType),
		  OPropertyGetName (This->referenceProperty),
		  value,This->values[count],This->contextsScript[count]) );
	  found = 1;
	  break;
	}
    }
  if( (found==1) && (count<This->contextn) )
    {
      OContextmapSetIthContext (This,count);
      This->hits[count]++;
    }
  else
    {
      OContextmapSetDefaultContext (This);
    }

}
/***************************************************************************/
int OContextmapGetAttribute (
 OContextmap  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

  /* class properties */
  if(This==NULL)
    {
       if(strcmp(a_name,"current")==0) 
	 {
	   OContextmap          contextmap;
	   contextmap           = OContextmapGetCurrent();
	   if(contextmap==NULL) return 0;
	   *((char**)a_addr)    = CStringCreateF   (9+64,"OContextmap/%lu",contextmap);
	   return               FREE_BLOCK;
	 }
       else if(strcmp(a_name,"labelHeight")==0)        
	 {
	   *((double*)a_addr)    = Class.labelHeight;
	 }
       else if(strcmp(a_name,"labelWidthStretch")==0)        
	 {
	   *((double*)a_addr)    = Class.labelWidthStretch;
	 }
       else if(strcmp(a_name,"labelPacking")==0)        
	 {
	   *((OString*)a_addr)   = OPackingGetName(Class.labelPacking);
	 }
       else if(strcmp(a_name,"baseLineStretch")==0)        
	 {
	   *((double*)a_addr)    = Class.baseLineStretch;
	 }
       else if(strcmp(a_name,"iconSizeStretch")==0)        
	 {
	   *((double*)a_addr)    = Class.iconSizeStretch;
	 }
       else if(strcmp(a_name,"labelContext")==0)             
	 {
	   *((OString*)a_addr)   = Class.labelContextScript;
	 }
       else if(strcmp(a_name,"labelRotation")==0)
	 {
	   *((OString*)a_addr)   = Class.labelRotationScript;
	 }
       else 
	 {
	   CInfoF("OContextmapGetAttribute: %s is not a class property.\n",a_name);
	   return 0;
	 }
       return   1;
    }

  /* object properties */ 
  if     (strcmp(a_name,"identifier")==0)       
    {
      *((Ulong*)a_addr)    = (Ulong)This;
    }
  else if(strcmp(a_name,"name")==0)             
    {
      *((char**)a_addr)    = This->name;
    }
  else if(strcmp(a_name,"referenceType")==0)
    {
      *((OString*)a_addr)  = OTypeGetName(This->referenceType);
    }
  else if(strcmp(a_name,"referenceProperty")==0)
    {
      *((OString*)a_addr)  = OPropertyGetName(This->referenceProperty);
    }
  else if(strcmp(a_name,"defaultContext")==0)        
    {
      *((OString*)a_addr)  = This->defaultContextScript;
    }
  else if(strcmp(a_name,"labelPacking")==0)        
    {
      *((OString*)a_addr)  = OPackingGetName(This->labelPacking);
    }
  else if(strcmp(a_name,"title")==0)        
    {
      *((OString*)a_addr)  = This->title;
    }
  else if(strcmp(a_name,"defaultLabel")==0)        
    {
      *((OString*)a_addr)  = This->defaultLabel;
    }
  else if(strcmp(a_name,"labelContext")==0)        
    {
      *((OString*)a_addr)  = This->labelContextScript;
    }
  else if(strcmp(a_name,"labelHeight")==0)        
    {
      *((double*)a_addr)   = This->labelHeight;
    }
  else if(strcmp(a_name,"labelRotation")==0)
    {
      *((OString*)a_addr)  = This->labelRotationScript;
    }
  else if(strcmp(a_name,"iconSizeStretch")==0)        
    {
      *((double*)a_addr)   = This->iconSizeStretch;
    }
  else if(strcmp(a_name,"iconTypes")==0)        
    {
      int      count;
      int      number = 0;
      OString* list   = NULL;
      for(count=0;count<IconGetSize();count++)
	{
	  if(IsIconValid(This,count))
	    CTextAddLine(&number,&list,IconGetName(count));
	}
      if(a_number!=NULL)   *a_number = number;
      *((OString**)a_addr) = list;
      return FREE_BLOCKS;
    }
  else if(strcmp(a_name,"labelPosition")==0) 
    {
      int     number = 0;
      double* doubles;
      doubles = (double*)CMemoryAllocateBlock( 3 * sizeof(double));
      if(doubles!=NULL)
	{
	  number     = 3;
	  doubles[0] = This->labelPositionX;
	  doubles[1] = This->labelPositionY;
	  doubles[2] = This->labelPositionZ;
	}
      if(a_number!=NULL) *a_number = number;
      *((double**)a_addr) = doubles;
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"showHitsOnly")==0)        
    {
      *((int*)a_addr)   = This->showHitsOnly;
    }
  else if(strcmp(a_name,"hits")==0) 
    {
      if(a_number!=NULL) *a_number = This->contextn;
      *((int**)a_addr) = This->hits;
    }
  else if(strcmp(a_name,"values")==0) 
    {
      if(a_number!=NULL)  *a_number = This->valuen;
      *((double**)a_addr) = This->values;
    }
  else if(strcmp(a_name,"contexts")==0) 
    {
      if(a_number!=NULL)   *a_number = This->contextn;
      *((OString**)a_addr) = This->contextsScript;
    }
  else if(strcmp(a_name,"labels")==0) 
    {
      if(a_number!=NULL) *a_number = This->labeln;
      *((OString**)a_addr) = This->labels;
    }
  else 
    {
      CInfoF("OContextmapGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OContextmapSetAttribute (
 OContextmap  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

  /* class properties */
  if(This==NULL)
    {
       if(strcmp(a_name,"current")==0) 
	 {
	   OContextmap contextmap;
	   contextmap  = OContextmapGetIdentifier(*((OString*)a_addr));
	   if(contextmap!=NULL)
	     {
	       OContextmapSetCurrent         (contextmap);
	       OContextmapSetDefaultContext  (contextmap);
	       OContextmapResetHits          (contextmap);
	       DEBUG( ("Current OContextmap now: %s\n",contextmap->name) );
	     }
	   else 
	     {
	       CWarnF("OContextmapSetAttribute: Unknown OContextmap %s.\n",*((OString*)a_addr));
	       return 0;
	     }
	 }
       else if(strcmp(a_name,"labelHeight")==0)        
	 {
	   Class.labelHeight   = *((double*)a_addr);
	 }
       else if(strcmp(a_name,"labelWidthStretch")==0)        
	 {
	   Class.labelWidthStretch  = *((double*)a_addr);
	 }
       else if(strcmp(a_name,"labelPacking")==0)        
	 {
	   Class.labelPacking       = OPackingGetIdentifier(*((OString*)a_addr));
	 }
       else if(strcmp(a_name,"baseLineStretch")==0)        
	 {
	   Class.baseLineStretch    = *((double*)a_addr);
	 }
       else if(strcmp(a_name,"iconSizeStretch")==0)        
	 {
	   Class.iconSizeStretch    = *((double*)a_addr);
	 }
       else if(strcmp(a_name,"labelContext")==0)             
	 {
	   STRING_DELETE            (Class.labelContextScript);
	   Class.labelContextScript = CStringDuplicate (*((OString*)a_addr));
	 }
       else if(strcmp(a_name,"labelRotation")==0)
	 {
	   OString       script;
	   OMatrix       matrix;
	   STRING_DELETE (Class.labelRotationScript);
	   MATRIX_DELETE (Class.labelRotationMatrix);
	   script        = *((OString*)a_addr);
	   matrix        = CreateRotationMatrixFromScript(script);
 	   if(matrix==NULL)
	     {
	       CInfoF ("OContextmapSetAttribute: property %s : %s invalid\n",a_name, script);
	       return 0;
	     }
	   Class.labelRotationMatrix  = matrix;
	   Class.labelRotationScript  = CStringDuplicate (script);
	 }
       else 
	 {
	   CInfoF ("OContextmapSetAttribute: %s is not a class property.\n",a_name);
	   return 0;
	 }
       a_user   = NULL;
       a_number = 0;
       return   1;
    }

  /* object properties */ 
  if     (strcmp(a_name,"identifier")==0)  
    {
    }
  else if(strcmp(a_name,"name")==0)        
    {
      OContextmapSetName        (This,*((OString*)a_addr));
    }
  else if(strcmp(a_name,"referenceType")==0)
    {                               
      This->referenceType = OTypeGetIdentifier(*((OString*)a_addr));
      if (This->referenceType==NULL)
	{
	  CWarnF ("OContextmapSetAttribute: %s : otype %s is unknown.\n",
		  a_name,*((OString*)a_addr));
	  return 0;
	}
    }
  else if(strcmp(a_name,"referenceProperty")==0)
       {
	 OProperty                   oprop;
	 int                         opropType;
	 This->referenceProperty     = NULL;
	 This->referencePropertyType = 0;
	 if (This->referenceType==NULL)
	   {
	     CWarnF ("OContextmapSetAttribute: %s : otype must be set first.\n",
		     a_name,*((OString*)a_addr));
	     return 0;
	   }
	 oprop = OTypeGetPropertyIdentifier(This->referenceType,*((OString*)a_addr));
	 if(oprop==NULL)
	   {
	     CWarnF ("OContextmapSetAttribute: %s : %s is not a property of %s.\n",
		     a_name,*((OString*)a_addr),OTypeGetName(This->referenceType));
	     return 0;
	   }
	 opropType = OPropertyGetType(oprop);
	 if( (opropType!=OPropertyDouble) &&
	     (opropType!=OPropertyInteger) )
	     {
	       CWarnF ("OContextmapSetAttribute: %s : property %s must be double or integer.\n",
		       a_name,*((OString*)a_addr));
	       return 0;
	     }
	 This->referenceProperty     = oprop;
	 This->referencePropertyType = opropType;
       }
  else if(strcmp(a_name,"defaultContext")==0)        
    {
      STRING_DELETE              (This->defaultContextScript);
      This->defaultContextScript = CStringDuplicate (*((OString*)a_addr));
      UpdateContexts             (This);
    }
  else if(strcmp(a_name,"defaultLabel")==0)        
    {
      STRING_DELETE      (This->defaultLabel);
      This->defaultLabel = CStringDuplicate (*((OString*)a_addr));
    }
  else if(strcmp(a_name,"labelPacking")==0)        
    {
      OContextmapSetPacking (This,OPackingGetIdentifier(*((OString*)a_addr)));
    }
  else if(strcmp(a_name,"title")==0)        
    {
      STRING_DELETE     (This->title);
      This->title       = CStringDuplicate (*((OString*)a_addr));
    }
  else if(strcmp(a_name,"labelContext")==0)        
    {
      STRING_DELETE            (This->labelContextScript);
      This->labelContextScript = CStringDuplicate (*((OString*)a_addr));
    }
  else if(strcmp(a_name,"labelHeight")==0)        
    {
      This->labelHeight = *((double*)a_addr);
    }
  else if(strcmp(a_name,"labelPosition")==0) 
    {
      if(a_number!=3)
	{
	  CWarnF("OContextmapSetAttribute:For property %s of obj %s 3 values are requested.\n",
		 a_name,OContextmapName(This));
	  return 0;
	}
      else
	{
	  double*              list;
	  list                 = *((double**)a_addr);
	  This->labelPositionX = list[0];
	  This->labelPositionY = list[1];
	  This->labelPositionZ = list[2];
	}
    }
  else if(strcmp(a_name,"labelRotation")==0)
    {
      OString       script;
      OMatrix       matrix;
      STRING_DELETE (This->labelRotationScript);
      MATRIX_DELETE (This->labelRotationMatrix);
      script        = *((OString*)a_addr);
      matrix        = CreateRotationMatrixFromScript(script);
      if(matrix==NULL)
	{
	  CInfoF ("OContextmapSetAttribute: property %s : %s invalid\n",a_name, script);
	  return 0;
	}
      This->labelRotationMatrix  = matrix;
      This->labelRotationScript  = CStringDuplicate (script);
    }
  else if(strcmp(a_name,"iconSizeStretch")==0)        
    {
      This->iconSizeStretch = *((double*)a_addr);
    }
  else if(strcmp(a_name,"iconTypes")==0)        
    {
      int             count;
      OString*        list = *((OString**)a_addr);
      This->iconTypes = 0;
      for(count=0;count<a_number;count++)
	AddIconName   (This,list[count]);
    }
  else if(strcmp(a_name,"showHitsOnly")==0)
    {
      This->showHitsOnly = *((int*)a_addr);
    }
  else if(strcmp(a_name,"hits")==0) 
    {
    }
  else if(strcmp(a_name,"values")==0)      
    {
      double*                list;
      int                    count;
      DEBUG                  ( ("set %s: number=%d\n",a_name,a_number) );
      CMemoryFreeBlock       (This->values);
      This->values           = NULL;
      This->valuen           = 0;
      if(a_number<=0)        return 1;
      This->values           = (double*)CMemoryAllocateBlock( a_number * sizeof(double));
      if(This->values==NULL) return 0;
      This->valuen           = a_number;
      OContextmapResetHits   (This);
      list                   = *((double**)a_addr);
      for(count=0;count<a_number;count++)
	{
	  This->values[count] = list[count];
	  DEBUG ( ("values[%2d] = %g\n",count,This->values[count]) );
	}
    }
  else if(strcmp(a_name,"contexts")==0)
    {
      OString*             list;
      list                 = *((OString**)a_addr);
      if(CreateContexts(This,a_number,list)==0) return 0;
      UpdateContexts       (This);
    }
  else if(strcmp(a_name,"labels")==0)
    {
      OString*     list;
      CTextDelete  (This->labeln,This->labels);
      list         = *((OString**)a_addr);
      This->labels = CTextDuplicate(a_number,list,&(This->labeln));
    }
  else 
    {
      CInfoF ("OContextmapSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  return   1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OContextmapSetName (
 OContextmap This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  STRING_DELETE (This->name);
  This->name    = CStringDuplicate (a_name);
}
/***************************************************************************/
char* OContextmapGetName (
 OContextmap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OContextmapSetDefaultContext (
 OContextmap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if (This==NULL)       return;
  OContextInitialize    (OContextGetStaticInstance());
  OContextSetFromScript (OContextGetStaticInstance(),This->defaultContextScript);
}
/***************************************************************************/
void OContextmapResetHits (
 OContextmap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
/*.........................................................................*/
  if (This==NULL)       return;
  if (This->hits==NULL) return;
  for (count=0;count<This->contextn;count++) This->hits[count] = 0;
}
/***************************************************************************/
void OContextmapSetLabelContext (
 OContextmap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if (This==NULL)    return;
  OContextInitialize (OContextGetStaticInstance());
  if     (This->labelContextScript!=NULL)
    OContextSetFromScript (OContextGetStaticInstance(),This->labelContextScript);
  else if(Class.labelContextScript!=NULL) 
    OContextSetFromScript (OContextGetStaticInstance(),Class.labelContextScript);
}
/***************************************************************************/
void OContextmapSetIthContext(
 OContextmap This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)           return;
  if(This->contexts==NULL) return;
  OContextCopy(OContextGetStaticInstance(),This->contexts[a_value]);
}
/***************************************************************************/
void OContextmapSetPacking (
 OContextmap This 
,OPacking a_packing 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_packing==This->labelPacking) return;
  This->labelPacking = a_packing;
}
/***************************************************************************/
OPacking OContextmapGetPacking (
 OContextmap This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OPackingHorizontal;
  return This->labelPacking;
}
/***************************************************************************/
OMatrix OContextmapGetLabelRotationMatrix (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || (This->labelRotationMatrix==NULL) )
    return Class.labelRotationMatrix;
  else
    return This->labelRotationMatrix;
}
/***************************************************************************/
int OContextmapGetContextNumber (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->contextn;
}
/***************************************************************************/
int OContextmapGetIconTypes (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return ICON_DEFAULT;
  return This->iconTypes;
}
/***************************************************************************/
int OContextmapShowHitsOnly (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->showHitsOnly;
}
/***************************************************************************/
int OContextmapGetIthContextHitNumber (
 OContextmap This
,int a_count
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return 0;
  if(This->hits==NULL) return 0;
  if( (a_count<0) || (a_count>=This->contextn) ) return 0;
  return This->hits[a_count];
}
/***************************************************************************/
char* OContextmapGetTitle (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->title;
}
/***************************************************************************/
void OContextmapGetLabelPosition (
 OContextmap This
,double* a_x
,double* a_y
,double* a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL) return;
  if(a_x!=NULL) *a_x = This->labelPositionX;
  if(a_y!=NULL) *a_y = This->labelPositionY;
  if(a_z!=NULL) *a_z = This->labelPositionZ;
}
/***************************************************************************/
double OContextmapGetLabelHeight (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || (This->labelHeight==0.) )
    return Class.labelHeight;
  else
    return This->labelHeight;
}
/***************************************************************************/
OPacking OContextmapGetLabelPacking (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if((This==NULL) || (This->labelPacking==OPackingUndefined) )
    return Class.labelPacking;
  else
    return This->labelPacking;
}
/***************************************************************************/
double OContextmapGetIconSize (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) || (This->iconSizeStretch==0.) )
    return Class.iconSizeStretch * OContextmapGetLabelHeight(This);
  else
    return This->iconSizeStretch * OContextmapGetLabelHeight(This);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void FreeRecord (
 OContextmap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;

  STRING_DELETE    (This->name);
  STRING_DELETE    (This->defaultLabel);
  STRING_DELETE    (This->defaultContextScript);
  STRING_DELETE    (This->title);
  STRING_DELETE    (This->labelContextScript);
  STRING_DELETE    (This->labelRotationScript);
  MATRIX_DELETE    (This->labelRotationMatrix);

  FreeContexts     (This);

  CMemoryFreeBlock (This->values);
  This->values     = NULL;
  This->valuen     = 0;
  CTextDelete      (This->labeln,This->labels);
  This->labeln     = 0;
  This->labels     = NULL;

  CMemoryFreeBlock (This);
}
/***************************************************************************/
char* OContextmapGetContextLabel (
 OContextmap This
,int a_index
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* label = NULL;
/*.........................................................................*/
  if(This==NULL) return NULL;
  label    = CStringCreate (256);
  if(label==NULL) return NULL;
  label[0] = '\0'; /* empty */
  if(This->labels==NULL)
    {
      if(This->defaultLabel!=NULL) 
	{
	  OString pos;
	  pos     = strchr(This->defaultLabel,'%');
	  if( (This->values==NULL) || (a_index<0) || (a_index>=This->valuen) ||
	      (pos==NULL) )
	    strcpy(label,This->defaultLabel);
	  else if(strchr(pos+1,'%')==NULL)
	    sprintf(label,This->defaultLabel,This->values[a_index]);
	  else if(a_index==0)
	    sprintf(label,This->defaultLabel,0.,This->values[a_index]);
	  else
	    sprintf(label,This->defaultLabel,This->values[a_index-1],This->values[a_index]);
	}
    }
  else if( (a_index>=0) && (a_index<This->labeln) ) 
    { /* replace one or two format specifiers within the label */
      OString pos;
      pos     = strchr(This->labels[a_index],'%');
      if( (This->values==NULL) || (a_index<0) || (a_index>=This->valuen) ||
	  (pos==NULL) )
	strcpy(label,This->labels[a_index]);
      else if(strchr(pos+1,'%')==NULL)
	sprintf(label,This->labels[a_index],This->values[a_index]);
      else if(a_index==0)
	sprintf(label,This->labels[a_index],0.,This->values[a_index]);
      else
	sprintf(label,This->labels[a_index],
		    This->values[a_index-1],This->values[a_index]);
    }
 return label;
}
/***************************************************************************/
static int CreateContexts (
 OContextmap This
,int         a_number
,char**      a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
/*.........................................................................*/
  if(This==NULL)       return 0;
  FreeContexts         (This);
  if (a_number<=0)     return 0;
  This->contexts       = (OContext*)CMemoryAllocateBlock( a_number * sizeof(OContext));
  This->hits           =      (int*)CMemoryAllocateBlock( a_number * sizeof(int));
  This->contextsScript =  (OString*)CMemoryAllocateBlock( a_number * sizeof(OString));
  if(
     (This->contexts==NULL)       || 
     (This->hits==NULL)           || 
     (This->contextsScript==NULL) 
     )
    {
      MEMORY_FREE (This->contexts);
      MEMORY_FREE (This->hits);
      MEMORY_FREE (This->contextsScript);
      return      0;
    }
  This->contextn = a_number;
  for(count=0;count<This->contextn;count++)
    {
      This->contexts[count]       = OContextMake();
      This->hits[count]           = 0;
      This->contextsScript[count] = CStringDuplicate(a_list[count]);
    }
  return 1;
}
/***************************************************************************/
static void UpdateContexts (
 OContextmap This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
/*.........................................................................*/
  if(This==NULL)           return;
  if(This->contexts==NULL) return;
  for(count=0;count<This->contextn;count++)
    {
      OContext              context;
      context               = This->contexts[count];
      OContextInitialize    (context);
      OContextSetFromScript (context,This->defaultContextScript);
      OContextSetFromScript (context,This->contextsScript[count]);
    }
}
/***************************************************************************/
static void FreeContexts (
 OContextmap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
/*.........................................................................*/
  if(This==NULL)           return;
  if(This->contexts==NULL) return;
  for(count=0;count<This->contextn;count++)
    {
      OContextDelete(This->contexts[count]);
    }
  CMemoryFreeBlock (This->contexts);
  This->contexts   = NULL;
  MEMORY_FREE      (This->hits);
  TEXT_DELETE      (This->contextn,This->contextsScript);
  This->contextn   = 0;
}
/***************************************************************************/
static void AddIconName (
 OContextmap This 
,OString a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_name==NULL) return;
  for(count=0;count<ICON_TYPE_MAX;count++)
    {
      if (strcmp(a_name,IconGetName(count))==0)
	{
	  This->iconTypes |= 1<<count;
	  return;
	}
    }
  CWarnF("%s is not a valid Icon type.\n",a_name);
}
/***************************************************************************/
static int IsIconValid (
 OContextmap This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return (This->iconTypes & (1<<a_value));
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static struct {
  char*      string;
  OPacking value;
} OPackingTypeList [] = {
  {"undefined",    OPackingUndefined},
  {"horizontal",   OPackingHorizontal},
  {"vertical",     OPackingVertical}
};  
/***************************************************************************/
char** OPackingGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* packings[3] = {"undefined","horizontal","vertical"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 3;
  return packings;
}
/***************************************************************************/
OPacking OPackingGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return OPackingHorizontal;
  for(count=0;count<ONumber(OPackingTypeList);count++)
    if(strcmp(a_string,OPackingTypeList[count].string)==0) 
      return OPackingTypeList[count].value;
  CWarnF("%s not an OPacking.\n",a_string);
  return OPackingHorizontal;
}
/***************************************************************************/
char* OPackingGetName (
 OPacking a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OPackingTypeList);count++)
    if(a_value==OPackingTypeList[count].value) return OPackingTypeList[count].string;
  CWarnF("%d not an OPacking.\n",a_value);
  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int IconGetSize (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return ICON_TYPE_MAX;
}
/***************************************************************************/
static OString* IconGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = ICON_TYPE_MAX;
  return IconTypeName;
}
/***************************************************************************/
static OString IconGetName (
 int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if((a_value>=0) && (a_value<ICON_TYPE_MAX))
    {
      return IconTypeName[a_value];
    }
  CWarnF("%d not an Icon index.\n",a_value);
  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static OMatrix CreateRotationMatrixFromScript (
 OString  a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OMatrix  matrix = NULL;
  OMatrix  rotate = NULL;
  int      count;
  int      wordn;
  int      status = 1;
  OString* words;
/*.........................................................................*/

  if(a_string==0) return NULL;
  
  words = CStringGetWords (a_string," ",&wordn);

  if(wordn<=0 || (wordn%2)!=0)
    {
      CWarnF ("Even number of arguments expected in %s instead of %d\n",a_string,wordn);
      CTextDelete  (wordn,words);
      return       NULL;
    }

  for (count=0;count<wordn;count+=2)
    { 
      char    axis;
      double  angle;

      axis = words[count][0];
      if(axis!='x' && axis!='y' && axis!='z')
	{
	  CWarnF ("Argument %s is not a rotation axis [x,y,z]\n",words[count]);
	  break;
	}
      angle = CStringConvertToDouble(words[count+1],&status); 
      if(status==0) 
	{
	  CWarnF ("Argument %s is not a wordn\n",words[count]);
	  break;
	}
      
      angle *= M_PI/180.;
      if     (axis=='x')
	rotate            = OMatrixCreate(OMatrixRotationX,angle);
      else if(axis=='y')
	rotate            = OMatrixCreate(OMatrixRotationY,angle);
      else
	rotate            = OMatrixCreate(OMatrixRotationZ,angle);
      if (matrix==NULL)
	matrix            = rotate;
      else
	{
	  OMatrixMultiplyMatricesAndAssign (matrix,1,rotate);
	  OMatrixDelete                    (rotate);
	}
    }
      
  CTextDelete (wordn,words);
  if (status==1)    return matrix;
  MATRIX_DELETE (matrix);

  return NULL;
}
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Go","OContextmap",a_name,a_code);
}
