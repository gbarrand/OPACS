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

/*HAS_HO*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

/*Co*/
#include <CPrinter.h>
#include <CMemory.h>
#include <CString.h>
#include <CError.h>
#include <CList.h>
#include <CText.h>
#include <OMatrix.h>

/*Ho*/
#include <OHistogram.h>
#include <OFunction.h>
#include <OScatter.h>

/*Go*/
#include <OColormap.h>
#include <OShape.h>
#include <Go.h>

#include <OPlotterP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void       FreeRecord           (OPlotter);
static int        GetIdentifier        (OPlotter,char*);
static void       CreateNodes          (OPlotter);
static void       RepresentPlot        (OPlotter,int,OHistogram*,int,OFunction*,int,OScatter*);
static void       RepresentAxis        (ONode,OAxis,OLayout,OAxisIndex,double,double);
static double     VerifyLog            (double,double,double,int);
static void       MultiplyPoints       (OMatrix,OPointList);
static OHistogram GetBinFromString     (char*,int*,int*);
static void       EraseAxes            (OPlotter);
static int        GetBinColor          (OPainting,double,double,double);
static int        GetValueIndex        (double,int,double*,int*);
static void       ReportError          (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned int Uint;

#define ONumber(arr)  (sizeof(arr)/sizeof(arr[0]))
#define NotFound       (-1)

#define DBL_INT                 (int)(float)
#define MINIMUM(a,b)            ((a)<(b)?a:b)
#define MAXIMUM(a,b)            ((a)>(b)?a:b)

#define ONE_OBJECT  10000

static struct 
{
  OPlotter*  extent;
  OColormap  grad_ocmap;
  int        grad_beg;
  int        grad_ncolor;
} Class = {NULL,NULL,0,0};
/***************************************************************************/
void OPlotterClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy     ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent     = NULL;
}
/***************************************************************************/
OPlotter* OPlotterGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OPlotter OPlotterGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OPlotter)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OPlotter  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OPlotterGetName(This),a_name);
}
/****************************************************************************/
OPlotter OPlotterMake (
)
/****************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OPlotter This;
/*..........................................................................*/
  This                  = (OPlotter)CMemoryAllocateBlock (sizeof(OPlotterRecord));
  if(This==NULL)        return NULL;
  CListAddEntry         ((CList*)&(Class.extent),This);
  This->name            = NULL;
  This->title           = NULL;
  This->textScale       = 1.;
  This->layout          = OLayoutXV;
  This->layoutAutomated = 1;
  This->xyModeling      = OXY_ModelingRandom;
/* rendering */
  This->wallContext           = CStringDuplicate("color wheat modeling  solid");
  This->gridContext           = CStringDuplicate("color black lineStyle dotted");
  This->textContext           = CStringDuplicate("color black");
  This->histogramContext            = CStringDuplicate("color brown lineStyle solid areaStyle solid markerStyle dot painting shadow");
  This->secondHistogramContext      = CStringDuplicate("color brown lineStyle solid areaStyle solid markerStyle dot painting shadow");
  This->functionContext       = CStringDuplicate("color brown lineStyle solid areaStyle solid painting shadow");
  This->secondFunctionContext = CStringDuplicate("color brown lineStyle solid areaStyle solid painting shadow");
  This->scatterContext        = CStringDuplicate("color brown lineStyle solid markerStyle dot modeling cloud");
  This->secondScatterContext  = CStringDuplicate("color brown lineStyle solid markerStyle dot modeling cloud");
  This->colormap            = NULL;
  This->valueColorMapping   = NULL;
  This->gridsVisible        = 1;
  This->wallsVisible        = 1;
  This->pickable            = 0;
  This->drawTitle           = 1;
  This->titlePosition       = 0;
  This->barsVisible         = 0;
  for(count=0;count<OAxisMaximumNumber;count++) This->axis[count] = OAxisMake();
  This->value            = OAxisMake();
  OAxisSetAttributesF    (This->value,"a",1); /*Like OHistogram, OFunction.*/

  This->goplot          = NULL;
  This->gobins          = NULL;
  This->gofunc          = NULL;
  This->goback          = NULL;
  This->gogrid          = NULL;
  This->gotitle         = NULL;
  This->gobar           = NULL;
  This->goColorMapping  = NULL;
  for(count=0;count<OAxisMaximumNumber;count++) This->goaxis[count] = NULL;
  This->goaxisv         = NULL;

  This->sendRebuildProc = NULL;
  This->sendRebuildArg  = NULL;
  This->sendRebuildDone = 0;

  return                This;
}
/****************************************************************************/
OPlotter OPlotterCreate (
 char* a_name  
)
/****************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPlotter This;
/*..........................................................................*/
  This              = OPlotterMake();
  if(This==NULL)    return NULL;
  This->name        = CStringDuplicate(a_name);
  CreateNodes       (This);
  return            This;
}
/***************************************************************************/
void OPlotterSetColorGradient (
 void* a_cmap
,int a_beg
,int a_ncolor
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.grad_ocmap  = (OColormap)a_cmap;
  Class.grad_beg    = a_beg;
  Class.grad_ncolor = a_ncolor;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OPlotterIsValid (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
void OPlotterDelete (
  OPlotter This 
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
char* OPlotterGetName (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
static void FreeRecord (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;

  if(ONodeIsValid(This->goplot)==1)  /*A ONodeClearClass (exa : from OCameraClearClass) could have already cleared nodes.*/
    {
      ONode*        items;
      items         = ONodeGetLeavesForward (This->goplot);
      CListDestroy  ((CList)items,(CListVisitEntryFunction)ONodeDelete);
    }
  This->goplot  = NULL;
  This->gobins  = NULL;
  This->gofunc  = NULL;
  This->goback  = NULL;
  This->gogrid  = NULL;
  This->gotitle = NULL;
  This->goColorMapping = NULL;
  for(count=0;count<OAxisMaximumNumber;count++) This->goaxis[count] = NULL;
  This->goaxisv = NULL;

  CStringDelete (This->name);
  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      OAxisDelete       (This->axis[count]);
      This->axis[count] = NULL;
    }    
  OAxisDelete       (This->value);
  CStringDelete     (This->title);
  CStringDelete     (This->wallContext);
  CStringDelete     (This->gridContext);
  CStringDelete     (This->textContext);
  CStringDelete     (This->histogramContext);
  CStringDelete     (This->secondHistogramContext);
  CStringDelete     (This->functionContext);
  CStringDelete     (This->secondFunctionContext);
  CStringDelete     (This->scatterContext);
  CStringDelete     (This->secondScatterContext);
  CStringDelete     (This->valueColorMapping);
  This->valueColorMapping = NULL;
  This->name              = NULL;
  This->value             = NULL;
  This->title             = NULL;
  This->wallContext       = NULL;
  This->gridContext       = NULL;
  This->textContext       = NULL;
  This->histogramContext       = NULL;
  This->secondHistogramContext = NULL;
  This->functionContext        = NULL;
  This->secondFunctionContext  = NULL;
  This->scatterContext         = NULL;
  This->secondScatterContext   = NULL;
  This->colormap               = NULL;
  CMemoryFreeBlock             (This);
}
/***************************************************************************/
int OPlotterGetDimension (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NotFound;
       if(This->layout==OLayoutXV)    return 1;
  else if(This->layout==OLayoutXY)    return 2;
  else if(This->layout==OLayoutXYV)   return 2;
  else if(This->layout==OLayoutXYZ)   return 3;
  else return NotFound;
}
/***************************************************************************/
void OPlotterSetLayout (
 OPlotter This 
,OLayout a_type 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_type==This->layout) return;
  This->layout                  = a_type;
  ONodeRemovePrimitives         (This->goback);
  ONodeRemovePrimitives         (This->gogrid);
  ONodeRemovePrimitives         (This->gotitle);
  ONodeRemovePrimitives         (This->goColorMapping);
  EraseAxes                     (This);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetLayoutAutomated (
 OPlotter This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_value==This->layoutAutomated) return;
  This->layoutAutomated         = a_value;
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetGridsVisible (
 OPlotter This 
,int a_draw
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_draw==This->gridsVisible) return;
  This->gridsVisible            = a_draw;
  ONodeRemovePrimitives         (This->gogrid);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
int OPlotterAreGridsVisible (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return This->gridsVisible;
}
/***************************************************************************/
int OPlotterGetPickability (
 OPlotter This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->pickable;
}
/***************************************************************************/
void OPlotterSetPickability (
 OPlotter This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->pickable = a_value;
}
/***************************************************************************/
void OPlotterSetWallsVisible (
 OPlotter This 
,int a_draw
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_draw==This->wallsVisible) return;
  This->wallsVisible            = a_draw;
  ONodeRemovePrimitives         (This->goback);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetTitleVisible (
 OPlotter This 
,int a_draw
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_draw==This->drawTitle) return;
  This->drawTitle               = a_draw;
  ONodeRemovePrimitives         (This->gotitle);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetTitlePosition (
 OPlotter This 
,int a_pos
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_pos==This->titlePosition) return;
  This->titlePosition           = a_pos;
  ONodeRemovePrimitives         (This->gotitle);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetBarsVisible (
 OPlotter This 
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_value==This->barsVisible) return;
  This->barsVisible  = a_value;
  OPlotterRequireRepresentation    (This);
}
/***************************************************************************/
int OPlotterAreBarsVisible (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->barsVisible;
}
/***************************************************************************/
int OPlotterAreWallsVisible (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return This->wallsVisible;
}
/***************************************************************************/
OLayout OPlotterGetLayout (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OLayoutXV;
  return This->layout;
}
/***************************************************************************/
void OPlotterSetXY_Modeling (
 OPlotter This 
,OXY_Modeling a_type 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_type==This->xyModeling)  return;
  This->xyModeling              = a_type;
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
OXY_Modeling OPlotterGetXY_Modeling (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OXY_ModelingRandom;
  return This->xyModeling;
}
/***************************************************************************/
void OPlotterSetGridContext (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->gridContext)==1) return;
  CStringDelete                 (This->gridContext);
  This->gridContext             = CStringDuplicate(a_string);
  ONodeRemovePrimitives         (This->gogrid);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetWallContext (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->wallContext)==1) return;
  CStringDelete                 (This->wallContext);
  This->wallContext             = CStringDuplicate(a_string);
  ONodeRemovePrimitives         (This->goback);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetTextContext (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->textContext)==1) return;
  CStringDelete                 (This->textContext);
  This->textContext             = CStringDuplicate(a_string);
  ONodeRemovePrimitives         (This->gotitle);
  EraseAxes                     (This);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetHistogramContext (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->histogramContext)==1) return;
  CStringDelete   (This->histogramContext);
  This->histogramContext   = CStringDuplicate(a_string);
  OPlotterRequireRepresentation  (This);
}
/***************************************************************************/
char* OPlotterGetHistogramContext (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->histogramContext;
}
/***************************************************************************/
void OPlotterSetSecondHistogramContext (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->secondHistogramContext)==1) return;
  CStringDelete   (This->secondHistogramContext);
  This->secondHistogramContext  = CStringDuplicate(a_string);
  OPlotterRequireRepresentation  (This);
}
/***************************************************************************/
char* OPlotterGetSecondHistogramContext (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->secondHistogramContext;
}
/***************************************************************************/
void OPlotterSetFunctionContext (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->functionContext)==1) return;
  CStringDelete   (This->functionContext);
  This->functionContext  = CStringDuplicate(a_string);
  OPlotterRequireRepresentation  (This);
}
/***************************************************************************/
void OPlotterSetSecondFunctionContext (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->secondFunctionContext)==1) return;
  CStringDelete   (This->secondFunctionContext);
  This->secondFunctionContext = CStringDuplicate(a_string);
  OPlotterRequireRepresentation  (This);
}
/***************************************************************************/
void OPlotterSetScatterContext (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->scatterContext)==1) return;
  CStringDelete   (This->scatterContext);
  This->scatterContext  = CStringDuplicate(a_string);
  OPlotterRequireRepresentation  (This);
}
/***************************************************************************/
void OPlotterSetSecondScatterContext (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->secondScatterContext)==1) return;
  CStringDelete   (This->secondScatterContext);
  This->secondScatterContext = CStringDuplicate(a_string);
  OPlotterRequireRepresentation  (This);
}
/***************************************************************************/
void OPlotterSetColormap (
 OPlotter This  
,void* a_colormap
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->colormap==a_colormap) return;
  This->colormap                = (OColormap)a_colormap;
  ONodeRemovePrimitives         (This->goColorMapping);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetValueColorMapping (
 OPlotter This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringCompare(a_string,This->valueColorMapping)==1) return;
  CStringDelete                 (This->valueColorMapping);
  This->valueColorMapping       = CStringDuplicate(a_string);
  ONodeRemovePrimitives         (This->goColorMapping);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
void OPlotterSetTitle (
 OPlotter This 
,char*  a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if( (a_string!=NULL)    && 
      (This->title!=NULL) && 
      (strcmp(a_string,This->title)==0)
     ) return;
  CStringDelete                 (This->title);
  This->title                   = CStringDuplicate (a_string);
  ONodeRemovePrimitives         (This->gotitle);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
char* OPlotterGetTitle (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->title;
}
/***************************************************************************/
void OPlotterSetTextScale (
 OPlotter This 
,double a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_value==This->textScale) return;
  This->textScale               = a_value;
  ONodeRemovePrimitives         (This->gotitle);
  EraseAxes                     (This);
  OPlotterRequireRepresentation (This);
}
/***************************************************************************/
double OPlotterGetTextScale (
 OPlotter This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1.;
  return This->textScale;
}
/***************************************************************************/
void OPlotterSetIthAxisAttributesF (
 OPlotter This  
,Uint a_axis     
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  int      changed = 0;
/*.........................................................................*/
  if(This==NULL) return; 
  va_start    (args,a_format);
       if(a_axis==OAxisV)                      
    OAxisSetAttributesV (This->value     ,&changed,a_format,args);
  else if(OAxisVerifyIndex((OAxisIndex)a_axis)==0) 
    OAxisSetAttributesV (This->axis[0]    ,&changed,a_format,args);
  else 
    OAxisSetAttributesV (This->axis[a_axis],&changed,a_format,args);
  va_end      (args);
  if(changed==1) 
    {
      ONodeRemovePrimitives          (This->gogrid);
      if(a_axis==OAxisV)                      
	{
	  ONodeDestroyChildren       (This->goaxisv);
	  ONodeRemovePrimitives      (This->goaxisv);
	}
      else if(OAxisVerifyIndex((OAxisIndex)a_axis)==1) 
	{
	  ONodeDestroyChildren       (This->goaxis[a_axis]);
	  ONodeRemovePrimitives      (This->goaxis[a_axis]);
	}
      OPlotterRequireRepresentation  (This);
    }
}
/***************************************************************************/
void OPlotterGetIthAxisAttributesF (
 OPlotter This  
,Uint a_axis     
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
/*.........................................................................*/
  if(This==NULL) return;
  va_start  (args,a_format);
       if(a_axis==OAxisV)
    OAxisGetAttributesV (This->value     ,a_format,args);
  else if(OAxisVerifyIndex((OAxisIndex)a_axis)==0) 
    OAxisGetAttributesV (NULL            ,a_format,args);
  else 
    OAxisGetAttributesV (This->axis[a_axis],a_format,args);
  va_end    (args);
}
/***************************************************************************/
void* OPlotterGetRootNode (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->goplot;
}
/***************************************************************************/
void OPlotterSetRequireRepresentationFunction (
 OPlotter This 
,OPlotterSendFunction a_proc
,void* a_arg
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_proc!=NULL)
    {
      This->sendRebuildProc = a_proc;
      This->sendRebuildArg  = a_arg;
    }
  This->sendRebuildDone = a_value;
}
/***************************************************************************/
int OPlotterIsRebuildDone (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->sendRebuildDone;
}
/***************************************************************************/
void OPlotterRequireRepresentation (
 OPlotter This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                  return;
  if(This->sendRebuildDone==1)    return;
  if(This->sendRebuildProc==NULL) return;            

#ifdef DEBUG
  printf ("debug : OPlotterRequireRepresentation : %s\n",This->name==NULL ? "(nil)" : This->name);
#endif

  if(This->sendRebuildProc(This->sendRebuildArg)==0) 
    {
      ReportError("OPlotterRequireRepresentation",CErrorIO);
      return;
    }
  This->sendRebuildDone     = 1;
}
/***************************************************************************/
void OPlotterReceiveRepresentationRequest (
 OPlotter This 
,CList a_hists
,CList a_funcs
,CList a_scats
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)               return;
  if(This->sendRebuildDone==0) return;

#ifdef DEBUG
  printf ("debug : OPlotterReceiveRepresentationRequest : %s.\n",This->name==NULL ? "(nil)" : This->name);
#endif

  OPlotterRepresent     (This,a_hists,a_funcs,a_scats);

  This->sendRebuildDone = 0;
}
/***************************************************************************/
void OPlotterDoRepresentation (
 OPlotter This 
,CList a_hists
,CList a_funcs
,CList a_scats
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OPlotterRepresent (This,a_hists,a_funcs,a_scats);
}
/***************************************************************************/
void OPlotterErase (
 OPlotter  This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  ONodeDestroyChildren (This->gobins);
  ONodeDestroyChildren (This->gofunc);
}
/***************************************************************************/
void OPlotterClear (
 OPlotter  This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
#ifdef DEBUG
  printf ("debug : RepresentPlot : %s : clear ONodes.\n",This->name==NULL ? "(nil)" : This->name);
#endif
  ONodeRemovePrimitives    (This->goback);
  ONodeRemovePrimitives    (This->gogrid);
  ONodeRemovePrimitives    (This->gotitle);
  ONodeRemovePrimitives    (This->gobar);
  ONodeRemovePrimitives    (This->goColorMapping);
  EraseAxes                (This);
  OPlotterErase            (This);
}
/***************************************************************************/
void OPlotterRepresent (
 OPlotter  This 
,CList a_hists
,CList a_funcs
,CList a_scats
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        histn,scatn,funcn;
  OHistogram major_hist;
  OFunction  major_func;
  OScatter   major_scat;

  char*     title;

  int       nbin;
  double    min,max;
  int       islog;
  int       isauto;
/*.........................................................................*/
  if(This==NULL) return;

  histn = CListGetSize (a_hists);
  scatn = CListGetSize (a_scats);
  funcn = CListGetSize (a_funcs);

  if( (histn!=0) || 
      (scatn!=0) || 
      (funcn!=0) 
     ) 
    {
/* priority to map plot axes and build plot title is: 
  - first hist
  - first function */

      major_hist = histn>0 && (a_hists!=NULL) ? (OHistogram)(a_hists[0]) : (OHistogram)NULL ;
      major_func = funcn>0 && (a_funcs!=NULL) ? (OFunction)(a_funcs[0])  : (OFunction)NULL ;
      major_scat = scatn>0 && (a_scats!=NULL) ? (OScatter)(a_scats[0])   : (OScatter)NULL ;

      if(major_hist!=NULL)
	{ 
/* major hist force plot policy */
	  if(This->layoutAutomated==1)
	    {
	      if(OHistogramGetDimension(major_hist)==1) 
		{
		  OPlotterSetLayout (This,OLayoutXV);
		}
	      else
		{
		  /*OPlotterSetLayout (This,OLayoutXYV);*/ /*Lego need too much resources to be the default.*/
		    OPlotterSetLayout (This,OLayoutXY);
		}
	    }
	  switch(OPlotterGetDimension(This))
	    { 
	    case 2:
	      /* plot axes from histo */
	      OHistogramGetIthAxisAttributesF  (major_hist,OAxisX,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF    (This,OAxisX,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks                (This->axis[0]);
	      
	      OHistogramGetIthAxisAttributesF  (major_hist,OAxisY,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF    (This,OAxisY,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks                (This->axis[1]);

	      OHistogramAdjustAxisOfValues     (major_hist,0);
	      OHistogramGetIthAxisAttributesF  (major_hist,OAxisV,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF    (This,OAxisV,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks                (This->value);
	      /* plot title */
	      OPlotterSetTitle                 (This,OHistogramGetTitle(major_hist));
	      break;
          case 1:
	      /* plot axes from histo */
	      OHistogramGetIthAxisAttributesF  (major_hist,OAxisX,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF    (This,OAxisX,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks                (This->axis[0]);
	      
	      OHistogramAdjustAxisOfValues     (major_hist,0);
	      OHistogramGetIthAxisAttributesF  (major_hist,OAxisV,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF    (This,OAxisV,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks                (This->value);
	      /* plot title */
	      OPlotterSetTitle                 (This,OHistogramGetTitle(major_hist));
	      break;
	    default:break;
	    }
	}
      
      else  if(major_scat!=NULL)
	{ 
	  int pdim,count;
	  /* plot binded to some OScatter: take first one to map plot axes */
	  if(This->layoutAutomated==1)
	    {
	      if(OScatterGetDimension(major_scat)==2) 
		OPlotterSetLayout (This,OLayoutXY);
	      else if(OScatterGetDimension(major_scat)==3) 
		OPlotterSetLayout (This,OLayoutXYZ);
	      else 
		OPlotterSetLayout (This,OLayoutXY);
	    }	  
	  pdim = OPlotterGetDimension(This);
	  for(count=0;count<pdim;count++)	  
	    { 
	      OAxisVerifyNumberOfBins       (OScatterGetIthAxis(major_scat,(OAxisIndex)count),20);
	      OAxisVerifyRange              (OScatterGetIthAxis(major_scat,(OAxisIndex)count),0.,1.);
	      
	      OScatterGetIthAxisAttributesF (major_scat,(OAxisIndex)count,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF (This,(OAxisIndex)count,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks             (This->axis[count]);
	    }	      
	  OPlotterSetTitle    (This,OScatterGetTitle(major_scat));
	}
      
      else  if(major_func!=NULL)
	{ /* plot binded to some functions: take first one to map plot axes */
	  int   fdim;
	  fdim  = OFunctionGetDimension(major_func);
	  /* major hist type force plot policy */
	  if(This->layoutAutomated==1)
	    {
	      if(fdim==1)
		{
		  OPlotterSetLayout (This,OLayoutXV);
		}
	      else if(fdim==2) 
		{
		  OPlotterSetLayout (This,OLayoutXYV);
		}
	      else
		{ 
		  OPlotterSetLayout (This,OLayoutXV);
		}
	    }
	  switch(OPlotterGetDimension(This))
	    { 
	    case 2:
	      OAxisVerifyNumberOfBins        (OFunctionGetIthAxis(major_func,OAxisX),20);
	      OAxisVerifyNumberOfBins        (OFunctionGetIthAxis(major_func,OAxisY),20);
	      OAxisVerifyRange               (OFunctionGetIthAxis(major_func,OAxisX),0.,1.);
	      OAxisVerifyRange               (OFunctionGetIthAxis(major_func,OAxisY),0.,1.);
	      /* plot axes from func */
	      OFunctionGetIthAxisAttributesF (major_func,OAxisX,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF  (This,OAxisX,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks              (This->axis[0]);
	      
	      OFunctionGetIthAxisAttributesF (major_func,OAxisY,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF  (This,OAxisY,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks              (This->axis[1]);
	      
	      OFunctionAdjustAxisOfValues    (major_func);
	      OFunctionGetIthAxisAttributesF (major_func,OAxisV,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF  (This,OAxisV,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks              (This->value);
	      /* plot title */
	      title            = OFunctionGetScript (major_func);
	      OPlotterSetTitle (This,title);
	      break;
	    case 1:
	      OAxisVerifyNumberOfBins        (OFunctionGetIthAxis(major_func,OAxisX),100);
	      OAxisVerifyRange               (OFunctionGetIthAxis(major_func,OAxisX),0.,1.);
	      /* plot axes from func */
	      OFunctionGetIthAxisAttributesF (major_func,OAxisX,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF  (This,OAxisX,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks              (This->axis[0]);
	      
	      OFunctionAdjustAxisOfValues    (major_func);
	      OFunctionGetIthAxisAttributesF (major_func,OAxisV,"nmxtla",&nbin,&min,&max,&title,&islog,&isauto);
	      OPlotterSetIthAxisAttributesF  (This,OAxisV,"nmxtla",nbin,min,max,title,islog,isauto);
	      OAxisComputeTicks              (This->value);
	      /* plot title */
	      title            = OFunctionGetScript (major_func);
	      OPlotterSetTitle (This,title);
	      break;
	    default:break;
	    }
	}
    }

  RepresentPlot (This,histn,(OHistogram*)a_hists,funcn,(OFunction*)a_funcs,scatn,(OScatter*)a_scats);
}
/****************************************************************************/
static void CreateNodes (
 OPlotter This
)
/****************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
  int   lname;
/*..........................................................................*/
  if(This==NULL) return;

  name                = This->name!=NULL ? CStringDuplicate(This->name) : CStringCreateF(16,"%lu",This);
  lname               = strlen(name);
  ONodeSetPickability (This->goplot = ONodeCreateF (lname+5,"%s:plot",name),0);

/* background controlled by : This->wallsVisible, This->layout, This->wallContext.*/
  ONodeSetPickability (This->goback = ONodeCreateF (lname+4,"%s:box",name),0);
  ONodeAddChild       (This->goplot,This->goback);

/* grid controlled by : This->layout, This->gridsVisible, This->gridContext, ticks.*/
  ONodeSetPickability (This->gogrid = ONodeCreateF (lname+5,"%s:grid",name),0);
  ONodeAddChild       (This->goplot,This->gogrid);

  ONodeSetPickability (This->gobar = ONodeCreateF (lname+6,"%s:bar",name),0);
  ONodeAddChild       (This->goplot,This->gobar);

/* title controlled by : This->title, This->drawTitle, This->titlePosition, This->layout, This->textScale, This->textContext.*/
  ONodeSetPickability (This->gotitle = ONodeCreateF (lname+6,"%s:title",name),0);
  ONodeAddChild       (This->goplot,This->gotitle);

/* Axes controlled by : This->layout, This->textScale, This->textContext.*/
  ONodeSetPickability (This->goaxis[OAxisX] = ONodeCreateF (lname+6,"%s:axe_x",name),0);
  ONodeAddChild       (This->goplot,This->goaxis[OAxisX]);

  ONodeSetPickability (This->goaxis[OAxisY] = ONodeCreateF (lname+6,"%s:axe_y",name),0);
  ONodeAddChild       (This->goplot,This->goaxis[OAxisY]);

  ONodeSetPickability (This->goaxis[OAxisZ] = ONodeCreateF (lname+6,"%s:axe_z",name),0);
  ONodeAddChild       (This->goplot,This->goaxis[OAxisZ]);

  ONodeSetPickability (This->goaxisv = ONodeCreateF (lname+6,"%s:axe_v",name),0);
  ONodeAddChild       (This->goplot,This->goaxisv);

  ONodeSetPickability (This->gobins  = ONodeCreateF (lname+5,"%s:bins",name),0);
  ONodeAddChild       (This->goplot,This->gobins);

  ONodeSetPickability (This->gofunc  = ONodeCreateF (lname+5,"%s:func",name),0);
  ONodeAddChild       (This->goplot,This->gofunc);

/* ColorMapping controlled by : This->colormap, This->valueColorMapping.*/
  ONodeSetPickability (This->goColorMapping = ONodeCreateF (lname+13,"%s:colorMapping",name),0);
  ONodeAddChild       (This->goplot,This->goColorMapping);

  CStringDelete       (name);
}
/***************************************************************************/
static  void RepresentPlot (
 OPlotter    This      
,int         a_histn
,OHistogram* a_hists
,int         a_funcn
,OFunction*  a_funcs
,int         a_scatn
,OScatter*   a_scats
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        count;
  char*      name;
  int        lname;
  int        pdim;
  OPointList points = NULL;
  double     rule_height;
  double     XMIN,XMAX,YMIN,YMAX,ZMIN,ZMAX,VMIN,VMAX;
  double     xmin,xmax,ymin,ymax,zmin,zmax,vmin,vmax;
  double     dx,dy,dz;
  double     xx,yy,zz;
  double     dval;
  int        xlog,ylog,zlog,vlog;
  int        ihist,ifunc,iscat;
  int        drawColorMapping = 0;
/*.........................................................................*/
  if(This==NULL)     return;

  if( (a_histn==0) &&
      (a_scatn==0) &&
      (a_funcn==0) 
     ) /*Nothing to draw, delete ONodes.*/
    {
      OPlotterClear      (This);
      return;
    }

  ONodeRemovePrimitives    (This->gobar);
  ONodeRemovePrimitives    (This->goColorMapping);
  OPlotterErase            (This);

  pdim                     = OPlotterGetDimension(This);

  OPlotterGetIthAxisAttributesF       (This,OAxisX,"mxl",&XMIN,&XMAX,&xlog);
  OPlotterGetIthAxisAttributesF       (This,OAxisY,"mxl",&YMIN,&YMAX,&ylog);
  OPlotterGetIthAxisAttributesF       (This,OAxisZ,"mxl",&ZMIN,&ZMAX,&zlog);
  OPlotterGetIthAxisAttributesF       (This,OAxisV,"mxl",&VMIN,&VMAX,&vlog);

  if(  (XMIN>=XMAX)              ||
       ((pdim>=2)&&(YMIN>=YMAX)) ||
       ((pdim>=3)&&(ZMIN>=ZMAX)) ||
       (((This->layout==OLayoutXV) || (This->layout==OLayoutXYV)) && (VMIN>=VMAX) ) 
    )
    {
      return;
    }

  xmin = XMIN;
  xmax = XMAX;
  if(xlog==1) 
    {
      if((xmin<=0.) || (xmax<=0.) )
        {
	  xlog = 0;
        }
      else
	{
	  xmin = log10(xmin);
	  xmax = log10(xmax);
	}
    }

  ymin = YMIN;
  ymax = YMAX;
  if(ylog==1) 
    {
      if((ymin<=0.) || (ymax<=0.) )
        {
          ylog = 0;
        }
      else
	{
	  ymin = log10(ymin);
	  ymax = log10(ymax);
	}
    }

  zmin = ZMIN;
  zmax = ZMAX;
  if(zlog==1) 
    {
      if((zmin<=0.) || (zmax<=0.) )
        {
          zlog = 0;
        }
      else
	{
	  zmin = log10(zmin);
	  zmax = log10(zmax);
	}
    }

  vmin = VMIN;
  vmax = VMAX;
  if(vlog==1) 
    {
      if((vmin<=0.) || (vmax<=0.) )
        {
          vlog = 0;
        }
      else
	{
	  vmin = log10(vmin);
	  vmax = log10(vmax);
	}
    }

  dx              = xmax - xmin;
  dy              = ymax - ymin;
  dz              = zmax - zmin;

  dval            = vmax - vmin;
  if(dval<=0.)    dval = 1.;	  

  OContextRestoreDefaultValues       (OContextGetStaticInstance());

  name            = OPlotterGetName(This);
  name            = name!=NULL ? CStringDuplicate(name) : CStringCreateF(9+64,"OPlotter/%lu",This);
  lname           = strlen(name);

  if( (ONodeGetPrimitives(This->goback)==NULL) && (This->wallsVisible==1) )
    {
#define PLANE_OFFSET 0.01
      if ( (This->layout==OLayoutXV) || (This->layout==OLayoutXY) )
	{
	  OContextRestoreDefaultValues (OContextGetStaticInstance());
	  OContextSetFromScript        (OContextGetStaticInstance(),This->wallContext);
	  GoAddGridToNode              (This->goback,
					0.,0.,-PLANE_OFFSET,
					1.,0.,0.,
					0.,1.,0.,
					1,1,
					1.,1.);
	}
      else if ( (This->layout==OLayoutXYV) || (This->layout==OLayoutXYZ) )
	{ 
	  OContextRestoreDefaultValues (OContextGetStaticInstance());
	  OContextSetFromScript        (OContextGetStaticInstance(),This->wallContext);
	  /* plan x-y */
	  GoAddGridToNode  (This->goback,
			    0.,0.,0./*-PLANE_OFFSET*/,
			    1.,0.,0.,
			    0.,1.,0.,
			    1,1,
			    1.,1.);
	  /* plan z-y at x = 1. */
	  GoAddGridToNode  (This->goback,
			    1.+PLANE_OFFSET,0.,0.,
			    0.,0.,1.,
			    0.,1.,0.,
			    1,1,
			    1.,1.+PLANE_OFFSET);
	  /* plan x-z at y = 1. */
	  GoAddGridToNode  (This->goback,
			    0.,1.+PLANE_OFFSET,0.,
			    1.,0.,0.,
			    0.,0.,1.,
			    1,1,
			    1.+PLANE_OFFSET,1.);
	}
    } /* endif wallsVisible */

/* grid */
  if( (ONodeGetPrimitives(This->gogrid)==NULL) && (This->gridsVisible==1) )
    {
      if( (This->layout==OLayoutXYV) || (This->layout==OLayoutXYZ) )
	{
	  int    pos = 0;
	  int    ntick;
	  OAxis  axe;
	  double min,step;
	  if (This->layout==OLayoutXYV) 
	    {
	      axe   = This->value;
	      min   = vmin;
	      step  = dval;
	    }
	  else /*take OAxisZ*/
	    {
	      axe   = This->axis[2];
	      min   = zmin;
	      step  = dz;
	    }
	  OContextRestoreDefaultValues (OContextGetStaticInstance());
	  OContextSetFromScript        (OContextGetStaticInstance(),This->gridContext);
	  ntick                        = OAxisGetNumberOfTicks(axe);
	  points                       = OPointListCreate(4 * ntick);
	  for(count=0;count<ntick;count++)
	    {
	      yy                 = (OAxisGetIthTickValue(axe,count)-min)/step;
	      if((yy>=0.)&&(yy<=1.))
		{
		  OPointListSetIthEntry    (points,pos,0.,1.,yy);pos++;
		  OPointListSetIthEntry    (points,pos,1.,1.,yy);pos++;
		  OPointListSetIthEntry    (points,pos,1.,1.,yy);pos++;
		  OPointListSetIthEntry    (points,pos,1.,0.,yy);pos++;
		}
	    }
	  OPointListLimit       (points,pos);
	  GoAddSegmentsToNode   (This->gogrid,points);
	  OPointListDelete      (points);
	}
    }
  OContextRestoreDefaultValues (OContextGetStaticInstance());

  if (This->layout==OLayoutXV)  /*pdim==1*/
    {

      for(ihist=a_histn-1;ihist>=0;ihist--) 
	{
	  OHistogram histloop;
	  histloop   = a_hists[ihist];

	  if(pdim==OHistogramGetDimension(histloop)) 
	    {
	      int       xnbin;

	      OHistogramGetIthAxisAttributesF (histloop,OAxisX,"n",&xnbin);
	      if(xnbin!=0)          
		{
		  double    XDBIN;
		  double    bmin,bmax;
		  int       ibin;
		  double    val_prev = 0.;
		  char*     hname;
		  int       lhname;
                  OModeling binModel;
                  OPainting binPaint;
		  ONode     gobin = NULL;

		  XDBIN              = (XMAX - XMIN)/xnbin;

		  OHistogramGetBinSumOfWeightsRange (histloop,&bmin,&bmax);
		  hname              = OHistogramGetName (histloop);
		  hname              = hname!=NULL ? CStringDuplicate(hname) : CStringCreateF(11+64,"OHistogram/%lu",histloop);
		  lhname             = strlen(hname);

		  if(ihist==0)	     OContextSetFromScript (OContextGetStaticInstance(),This->histogramContext);
		  else               OContextSetFromScript (OContextGetStaticInstance(),This->secondHistogramContext);

                  binModel           = OContextGetModeling(OContextGetStaticInstance());
		  points             = binModel==OModelingWireFrame ? OPointListCreate (4) : OPointListCreate (5);

                  binPaint           = OContextGetPainting(OContextGetStaticInstance());

		  if(This->pickable==0)
		    {
		      gobin               = ONodeCreateF       (lname+lhname+1,"%s:%s",name,hname);
		      ONodeSetPickability (gobin,0);
		      ONodeAddChild       (This->gobins,gobin);
		    }

		  for(ibin=0;ibin<xnbin;ibin++)
		    { 
		      double               val,xe;
		      int                  nent;
		      nent                 = OHistogramGetBinNumberOfEntries(histloop,ibin);

		      /*if(nent<=0) continue;*/  /*Need all bins modeled for fitting.*/

		      val                  = OHistogramGetBinSumOfWeights (histloop,ibin);

 		      OContextSetColorIndex (OContextGetStaticInstance(),
					     GetBinColor(binPaint,val,bmin,bmax));

		      xx                   = XMIN + ibin * XDBIN;
		      xe                   = xx + XDBIN;

		      val                  = VerifyLog (val,vmin,dval,vlog);
		      xx                   = VerifyLog (xx ,xmin,dx  ,xlog);
		      xe                   = VerifyLog (xe ,xmin,dx  ,xlog);

		      val                  = nent<=0 ? 0. : val;
		      val                  = val > 1. ? 1. : val;
		      if((xx >=0.)&&(xx <=1.)  &&
			 (xe >=0.)&&(xe <=1.)  
			 ) 
			{
			  if(This->pickable==1)
			    {
			      gobin            = ONodeCreateF       (lname+lhname+32,"%s:%s:%d",name,hname,ibin);
			      ONodeAddChild    (This->gobins,gobin);
			    }
			  if(binModel==OModelingWireFrame)
			    {
			      val              = val < 0. ? 0. : val;
			      OPointListSetIthEntry  (points,0,xx,val_prev,0.);
			      OPointListSetIthEntry  (points,1,xx,val ,0.);
			      OPointListSetIthEntry  (points,2,xe,val ,0.);
			      if(ibin==xnbin-1)
				OPointListSetIthEntry  (points,3,xe,0.  ,0.);
			      else
				OPointListSetIthEntry  (points,3,xe,val ,0.);
			      GoAddLinesToNode    (gobin,points);
			      val_prev         = val;
			    }
			  else 
			    {
			      if(val>=0.) 
				{
				  OPointListSetIthEntry  (points,0,xx,0.  ,0.);
				  OPointListSetIthEntry  (points,1,xe,0.  ,0.);
				  OPointListSetIthEntry  (points,2,xe,val ,0.);
				  OPointListSetIthEntry  (points,3,xx,val ,0.);
				  OPointListSetIthEntry  (points,4,xx,0.  ,0.);
				  if(binModel==OModelingSolid)
				    GoAddPolygonToNode     (gobin,points); 
				  else 
				    GoAddLinesToNode    (gobin,points);
				}
			    }
			}
		    }
		  OPointListDelete (points);

		  if( (This->barsVisible==1) && (ihist==0) )
		    { 
		      OContextSetColorIndex (OContextGetStaticInstance(),OColorBlack);
		      for(ibin=0;ibin<xnbin;ibin++)
			{ double               val,xe,bar_height;
			  double               ex,edx,bar_min,bar_max;
			  
			  /*if(OHistogramGetBinNumberOfEntries(histloop,ibin)<=0) continue;*/ /*Need all bins modeled for fitting.*/
			  
			  val                  = OHistogramGetBinSumOfWeights    (histloop,ibin);
			  bar_height           = OHistogramGetBinBar             (histloop,ibin);
			  
			  bar_min              = val - bar_height /2.;
			  bar_max              = val + bar_height /2.;
			  
			  xx                   = XMIN + ibin * XDBIN;
			  xe                   = xx + XDBIN;
			  
			  val                  = VerifyLog (val ,vmin,dval,vlog);
			  xx                   = VerifyLog (xx  ,xmin,dx  ,xlog);
			  xe                   = VerifyLog (xe  ,xmin,dx  ,xlog);
			  
			  bar_min                 = VerifyLog (bar_min,vmin,dval,vlog);
			  bar_max                 = VerifyLog (bar_max,vmin,dval,vlog);
			  
			  val                  = val > 1. ? 1. : val;
			  ex                   = (xe+xx)/2.;
			  if( (ex >=0.)&&(ex <=1.) ) 
			    { 
			      int              ipt;
			      edx              = 0.3 * (xe-xx);
			      ipt              = 0;
			      points           = OPointListCreate (6);
			      if(bar_min >1.)  /* do nothing */
				{}
			      else  if(bar_max <0.)  /* do nothing */
				{}
			      else  if(bar_min <0.) /* bottom out of histo */
				{ 
				  if(bar_max >1.) /* top also out of histo */
				    { 
				      OPointListSetIthEntry  (points,ipt,ex    ,  0.,0.);ipt++;
				      OPointListSetIthEntry  (points,ipt,ex    ,  1.,0.);ipt++;
				    }
				  else
				    { 
				      OPointListSetIthEntry  (points,ipt,ex    ,  0.,0.);ipt++;
				      OPointListSetIthEntry  (points,ipt,ex    ,bar_max,0.);ipt++;
				      OPointListSetIthEntry  (points,ipt,ex-edx,bar_max,0.);ipt++;
				      OPointListSetIthEntry  (points,ipt,ex+edx,bar_max,0.);ipt++;
				    }
				}
			      else  if(bar_max >1.) /* top out of histo */
				{ 
				  OPointListSetIthEntry  (points,ipt,ex-edx,bar_min,0.);ipt++;
				  OPointListSetIthEntry  (points,ipt,ex+edx,bar_min,0.);ipt++;
				  OPointListSetIthEntry  (points,ipt,ex    ,bar_min,0.);ipt++;
				  OPointListSetIthEntry  (points,ipt,ex    ,  1.,0.);ipt++;
				}
			      else /* all error bar in histo */
				{
				  OPointListSetIthEntry  (points,ipt,ex-edx,bar_min,0.);ipt++;
				  OPointListSetIthEntry  (points,ipt,ex+edx,bar_min,0.);ipt++;
				  OPointListSetIthEntry  (points,ipt,ex    ,bar_min,0.);ipt++;
				  OPointListSetIthEntry  (points,ipt,ex    ,bar_max,0.);ipt++;
				  OPointListSetIthEntry  (points,ipt,ex-edx,bar_max,0.);ipt++;
				  OPointListSetIthEntry  (points,ipt,ex+edx,bar_max,0.);ipt++;
				}
			      OPointListLimit     (points,ipt);
			      GoAddLinesToNode    (This->gobar,points);
			      OPointListDelete    (points);
			    }
			}
		    }
		  
		  CStringDelete(hname);
		} /*endif xnbin*/

	    } /* endif pdim */

	} /* end for */

    } 
  else if (This->layout==OLayoutXYV)  /*pdim==2*/
    {

      for(ihist=a_histn-1;ihist>=0;ihist--) 
	{
	  OHistogram    histloop;
	  histloop = a_hists[ihist];
	  
	  if(pdim==OHistogramGetDimension(histloop)) 
	    {
	      int           xnbin,ynbin;
	      char*         hname;
	      int           lhname;
	      double        XDBIN,YDBIN;
	      double        bmin,bmax;
	      int           ibin,jbin;
              OPainting     binPaint;
	      int           colorn = 0;
	      int*          colors = NULL;
	      double*       values = NULL;
	      
	      hname              = OHistogramGetName (histloop);
	      hname              = hname!=NULL ? 
		                    CStringDuplicate(hname) : 
		                    CStringCreateF(11+64,"OHistogram/%lu",histloop);
	      lhname             = strlen(hname);
	      
	      OHistogramGetIthAxisAttributesF       (histloop,OAxisX,"n",&xnbin);
	      OHistogramGetIthAxisAttributesF       (histloop,OAxisY,"n",&ynbin);

	      if(ihist==0)	  
		OContextSetFromScript (OContextGetStaticInstance(),This->histogramContext);
	      else  	          
		OContextSetFromScript (OContextGetStaticInstance(),This->secondHistogramContext);

	      OContextSetModeling (OContextGetStaticInstance(),OModelingSolid);
	      binPaint            = OContextGetPainting(OContextGetStaticInstance());

	      colorn              = OColormapGetValueColorMapping (This->colormap,
								   This->valueColorMapping,
								   &values,&colors);

	      if( (xnbin!=0)  && (ynbin!=0) ) 
		{
		  OHistogramGetBinSumOfWeightsRange    (histloop,&bmin,&bmax);
		  XDBIN              = (XMAX - XMIN)/xnbin;
		  YDBIN              = (YMAX - YMIN)/ynbin;

		  for(jbin=ynbin-1;jbin>=0;jbin--)
		    {
		      for(ibin=xnbin-1;ibin>=0;ibin--)
			{ 
			  double               val,xe,ye;
			  int                  icx,icy,icz;

			  if(OHistogramGetBinNumberOfEntries(histloop,ibin,jbin)<=0) continue;

			  val                  = OHistogramGetBinSumOfWeights (histloop,ibin,jbin);
			  if(binPaint==OPaintingShadow)
			    {
			      icx = OColorDimgrey;
			      icy = OColorGrey;
			      icz = OColorWhite;
			    }
			  else if( (This->xyModeling==OXY_ModelingColor) && (colorn>0))
			    {
			      icx = icy = icz = GetValueIndex (val,colorn,values,colors);
			    }
			  else
			    {
			      icx = icy = icz = GetBinColor (binPaint,val,bmin,bmax);
			    }
			  
			  xx                   = XMIN + ibin * XDBIN;
			  xe                   = xx + XDBIN;
			  yy                   = YMIN + jbin * YDBIN;
			  ye                   = yy + YDBIN;

			  val                  = VerifyLog (val,vmin,dval,vlog);
			  xx                   = VerifyLog (xx ,xmin,dx  ,xlog);
			  xe                   = VerifyLog (xe ,xmin,dx  ,xlog);
			  yy                   = VerifyLog (yy ,ymin,dy  ,ylog);
			  ye                   = VerifyLog (ye ,ymin,dy  ,ylog);
			  val                  = val > 1. ? 1. : val;
			  
			  if((xx>=0.)&&(xx<=1.)  &&
			     (xe>=0.)&&(xe<=1.)  &&
			     (yy>=0.)&&(yy<=1.)  &&
			     (ye>=0.)&&(ye<=1.)  &&
			     (val>=0.)
			     ) 
			    { 
			      ONode                 gobin;
			      gobin                 = ONodeCreateF (lname+lhname+64,"%s:%s:%d:%d",name,hname,ibin,jbin);
			      ONodeAddChild         (This->gobins,gobin);
			      GoAddColoredBoxToNode (gobin,xx,yy,0.,xe-xx,ye-yy,val,icx,icy,icz);
			    }
			  
			}
		    }
		  
		} /*endif xnbin ynbin*/
	      CStringDelete    (hname);
	      CMemoryFreeBlock (values);
	      CMemoryFreeBlock (colors);
	    } /* endif pdim */
	  
	} /* end for hist */
            
    } 
  else if (This->layout==OLayoutXY)  /*pdim==2*/
    {
      
      for(ihist=a_histn-1;ihist>=0;ihist--) 
	{
	  OHistogram  histloop;
	  histloop = a_hists[ihist];
	  
	  if(pdim==OHistogramGetDimension(histloop)) 
	    {
	      if(This->xyModeling==OXY_ModelingRandom)/*Draw for each bins a number of random points proportiannal to bins range.*/
		{

		  int       xnbin,ynbin;
		  
		  OHistogramGetIthAxisAttributesF       (histloop,OAxisX,"n",&xnbin);
		  OHistogramGetIthAxisAttributesF       (histloop,OAxisY,"n",&ynbin);
		  if( (xnbin!=0) && (ynbin!=0) ) 
		    {
		      char*      hname;
		      int        lhname;
		      double     XDBIN,YDBIN;
		      int        ibin,jbin;
		      OPointList points;
		      double     bmin,bmax,range;

		      hname              = OHistogramGetName (histloop);
		      hname              = hname!=NULL ? CStringDuplicate(hname) : CStringCreateF(11+64,"OHistogram/%lu",histloop);
                      lhname             = strlen(hname);

		      XDBIN              = (XMAX - XMIN)/xnbin;
		      YDBIN              = (YMAX - YMIN)/ynbin;
		      
		      if(ihist==0)	   OContextSetFromScript (OContextGetStaticInstance(),This->histogramContext);
		      else  	           OContextSetFromScript (OContextGetStaticInstance(),This->secondHistogramContext);

		      OHistogramGetBinSumOfWeightsRange    (histloop,&bmin,&bmax);
                      range              = bmax - bmin;

		      OPlotterSetPickability  (This,1);

		      for(jbin=ynbin-1;jbin>=0;jbin--)
			{
			  for(ibin=xnbin-1;ibin>=0;ibin--)
			    { 
			      double               val;
			      int                  npt;
			      int                  ipt;
			      int                  nmin = 1;
			      int                  nmax = 50;
				  
			      if(OHistogramGetBinNumberOfEntries (histloop,ibin,jbin)<=0) continue;

			      val                  = OHistogramGetBinSumOfWeights    (histloop,ibin,jbin);
				  
			      /* If range is 0. ; then all bins that have entries have same values. Draw one point. */

			      npt                  = range>0. ? (int)((val - bmin) * (nmax-nmin) /range + nmin) : 1;

			      points               = OPointListCreate (npt);
			      ipt                  = 0;
			      
			      for(count=0;count<npt;count++) 
				{
		    
				  xx                   = XDBIN * (((double)rand())/RAND_MAX);
				  xx                   = XMIN + ibin * XDBIN + xx;
		    
				  yy                   = YDBIN * (((double)rand())/RAND_MAX);
				  yy                   = YMIN + jbin * YDBIN + yy;
		    
				  xx                   = VerifyLog (xx ,xmin,dx  ,xlog);
				  yy                   = VerifyLog (yy ,ymin,dy  ,ylog);
				  
				  if((xx>=0.)&&(xx<=1.)  &&
				     (yy>=0.)&&(yy<=1.)  
				     ) 
				    { 
				      OPointListSetIthEntry      (points,ipt,xx    ,yy  ,0.);ipt++;
				    }
				  
				} /*endfor count*/
				  
			      OPointListLimit           (points,ipt);

			      if(ipt>0)
				{
				  ONode              gobin;
				  gobin              = ONodeCreateF (lname+lhname+64,"%s:%s:%d:%d",name,hname,ibin,jbin);
				  ONodeAddChild      (This->gobins,gobin);
				  GoAddMarkersToNode (gobin,points);
				}
				  
			      OPointListDelete   (points);
			      
			    }
			}

		      CStringDelete(hname);

		    } /*endif xnbin ynbin*/
		  
		  
		}
	      else if(This->xyModeling==OXY_ModelingBox) /*Draw box of size proportionnal to bin value.*/
		{
		  
		  int       xnbin,ynbin;
		  
		  OHistogramGetIthAxisAttributesF       (histloop,OAxisX,"n",&xnbin);
		  OHistogramGetIthAxisAttributesF       (histloop,OAxisY,"n",&ynbin);
		  if( (xnbin!=0) && (ynbin!=0) ) 
		    {
		      char*      hname;
		      int        lhname;
		      double     XDBIN,YDBIN;
		      int        ibin,jbin;
		      OPointList points;
		      double     bmin,bmax,range;
		      OModeling  binModel;
		      double     ax,bx,ay,by;

		      hname              = OHistogramGetName (histloop);
		      hname              = hname!=NULL ? CStringDuplicate(hname) : CStringCreateF(11+64,"OHistogram/%lu",histloop);
                      lhname             = strlen(hname);

		      XDBIN              = (XMAX - XMIN)/xnbin;
		      YDBIN              = (YMAX - YMIN)/ynbin;
		      points             = OPointListCreate (5);
		      
		      if(ihist==0)	 OContextSetFromScript (OContextGetStaticInstance(),This->histogramContext);
		      else  	         OContextSetFromScript (OContextGetStaticInstance(),This->secondHistogramContext);

		      OHistogramGetBinSumOfWeightsRange    (histloop,&bmin,&bmax);
                      range              = bmax - bmin;

		      binModel           = OContextGetModeling(OContextGetStaticInstance());

		      OPlotterSetPickability  (This,1);

		      if(range>0.) 
			{		  
                          ax     = (XDBIN - 0.) /range;
                          bx     =  XDBIN - ax * bmax; 
                          ay     = (YDBIN - 0.) /range;
                          by     =  YDBIN - ay * bmax; 
			}
		      else /*If range is 0. ; then all bins that have entries have same values. Draw box XDBIN * YDBIN.*/
			{
                          ax     = 0.;
                          bx     = XDBIN;
                          ay     = 0.;
                          by     = YDBIN;
			}

		      for(jbin=ynbin-1;jbin>=0;jbin--)
			{
			  for(ibin=xnbin-1;ibin>=0;ibin--)
			    { 
			      double               val,xe,ye,size;

			      if(OHistogramGetBinNumberOfEntries (histloop,ibin,jbin)<=0) continue;

			      val                  = OHistogramGetBinSumOfWeights (histloop,ibin,jbin);
			      
			      size                 = ax * val + bx;
			      xx                   = XMIN + ibin * XDBIN + (XDBIN - size)/2.;
			      xe                   = xx + size;
			      size                 = ay * val + by;
			      yy                   = YMIN + jbin * YDBIN + (YDBIN - size)/2.;
			      ye                   = yy + size;
				  
			      xx                   = VerifyLog (xx ,xmin,dx  ,xlog);
			      xe                   = VerifyLog (xe ,xmin,dx  ,xlog);
			      yy                   = VerifyLog (yy ,ymin,dy  ,ylog);
			      ye                   = VerifyLog (ye ,ymin,dy  ,ylog);
				  
			      if((xx>=0.)&&(xx<=1.)  &&
				 (xe>=0.)&&(xe<=1.)  &&
				 (yy>=0.)&&(yy<=1.)  &&
				 (ye>=0.)&&(ye<=1.)  
				 ) 
				{ 
				  ONode                 gobin;
				  gobin                 = ONodeCreateF (lname+lhname+64,"%s:%s:%d:%d",name,hname,ibin,jbin);
				  ONodeAddChild         (This->gobins,gobin);
				  OPointListSetIthEntry (points,0,xx    ,yy  ,0.);
				  OPointListSetIthEntry (points,1,xe    ,yy  ,0.);
				  OPointListSetIthEntry (points,2,xe    ,ye  ,0.);
				  OPointListSetIthEntry (points,3,xx    ,ye  ,0.);
				  OPointListSetIthEntry (points,4,xx    ,yy  ,0.);
				  if(binModel==OModelingSolid)
				    GoAddPolygonToNode     (gobin,points); 
				  else 
				    GoAddLinesToNode    (gobin,points);
				}
			    }
			}
		      
		      OPointListDelete (points);
		      CStringDelete    (hname);
		    } /*endif xnbin ynbin*/
		  
		}
	      else if(This->xyModeling==OXY_ModelingColor) /*Draw box of bin size colored according a gradient.*/
		{
		  
		  int       xnbin,ynbin;
		  
		  OHistogramGetIthAxisAttributesF       (histloop,OAxisX,"n",&xnbin);
		  OHistogramGetIthAxisAttributesF       (histloop,OAxisY,"n",&ynbin);
		  if( (xnbin!=0) && (ynbin!=0) ) 
		    {
		      char*      hname;
		      int        lhname;
		      double     XDBIN,YDBIN;
		      int        ibin,jbin;
		      OPointList points;
		      double     bmin,bmax;
                      int        colorn;
		      int*       colors = NULL;
		      double*    values = NULL;
		      
		      hname              = OHistogramGetName (histloop);
		      hname              = hname!=NULL ? CStringDuplicate(hname) : CStringCreateF(11+64,"OHistogram/%lu",histloop);
                      lhname             = strlen(hname);


		      XDBIN              = (XMAX - XMIN)/xnbin;
		      YDBIN              = (YMAX - YMIN)/ynbin;
		      points             = OPointListCreate (4);
		      
		      if(ihist==0)	 
			OContextSetFromScript (OContextGetStaticInstance(),This->histogramContext);
		      else  	         
			OContextSetFromScript (OContextGetStaticInstance(),This->secondHistogramContext);

		      OHistogramGetBinSumOfWeightsRange    (histloop,&bmin,&bmax);
                      colorn             = OColormapGetValueColorMapping (This->colormap,
									  This->valueColorMapping,
									  &values,&colors);
		      drawColorMapping   = 1;

		      OPlotterSetPickability  (This,1);
#ifdef DEBUG
		      printf ("debug : RepresentPlot : %s : XY_ModelingColor : xnbin %d ynbin %d range.\n",hname,xnbin,ynbin);
#endif
		      for(jbin=ynbin-1;jbin>=0;jbin--)
			{
			  for(ibin=xnbin-1;ibin>=0;ibin--)
			    { 
			      double               val,xe,ye;
			      int                  icolor = 0;

			      if(OHistogramGetBinNumberOfEntries (histloop,ibin,jbin)<=0) continue;

			      val                  = OHistogramGetBinSumOfWeights (histloop,ibin,jbin);
			      if(colorn<=1)
				{
				  icolor = GetBinColor(OPaintingGradient,val,bmin,bmax);
				}
			      else
				{
				  icolor = GetValueIndex (val,colorn,values,colors);
				}

			      OContextSetColorIndex(OContextGetStaticInstance(),icolor);

			      xx                   = XMIN + ibin * XDBIN;
			      xe                   = xx + XDBIN;

			      yy                   = YMIN + jbin * YDBIN;
			      ye                   = yy + YDBIN;
				  
			      xx                   = VerifyLog (xx ,xmin,dx  ,xlog);
			      xe                   = VerifyLog (xe ,xmin,dx  ,xlog);
			      yy                   = VerifyLog (yy ,ymin,dy  ,ylog);
			      ye                   = VerifyLog (ye ,ymin,dy  ,ylog);
				  
			      if((xx>=0.)&&(xx<=1.)  &&
				 (xe>=0.)&&(xe<=1.)  &&
				 (yy>=0.)&&(yy<=1.)  &&
				 (ye>=0.)&&(ye<=1.)  
				 ) 
				{ 
				  double                dx,dy;
				  ONode                 gobin;
				      
				  dx                    = xe-xx;
				  dy                    = ye-yy;

				  gobin                 = ONodeCreateF (lname+lhname+64,"%s:%s:%d:%d",name,hname,ibin,jbin);
				  ONodeAddChild         (This->gobins,gobin);
				  OPointListSetIthEntry (points,0,xx ,yy ,0.);
				  OPointListSetIthEntry (points,1,xe ,yy ,0.);
				  OPointListSetIthEntry (points,2,xe ,ye ,0.);
				  OPointListSetIthEntry (points,3,xx ,ye ,0.);
				  GoAddPolygonToNode    (gobin,points); 
				}
			    }
			}
		      
		      CMemoryFreeBlock  (values);
		      CMemoryFreeBlock  (colors);
		      OPointListDelete  (points);
		      CStringDelete     (hname);
		    } /*endif xnbin ynbin*/
		  
		}
	      else if(This->xyModeling==OXY_ModelingText) /*Draw box and text for number of entries.*/
		{
		  
		  int       xnbin,ynbin;
		  
		  OHistogramGetIthAxisAttributesF       (histloop,OAxisX,"n",&xnbin);
		  OHistogramGetIthAxisAttributesF       (histloop,OAxisY,"n",&ynbin);
		  if( (xnbin!=0) && (ynbin!=0) ) 
		    {
		      char*      hname;
		      int        lhname;
		      double     XDBIN,YDBIN;
		      int        ibin,jbin;
		      OPointList points;

		      hname              = OHistogramGetName (histloop);
		      hname              = hname!=NULL ? CStringDuplicate(hname) : CStringCreateF(11+64,"OHistogram/%lu",histloop);
                      lhname             = strlen(hname);

		      XDBIN              = (XMAX - XMIN)/xnbin;
		      YDBIN              = (YMAX - YMIN)/ynbin;
		      points             = OPointListCreate (5);
		      
		      if(ihist==0)	 OContextSetFromScript (OContextGetStaticInstance(),This->histogramContext);
		      else  	         OContextSetFromScript (OContextGetStaticInstance(),This->secondHistogramContext);

		      OPlotterSetPickability  (This,1);

		      for(jbin=ynbin-1;jbin>=0;jbin--)
			{
			  for(ibin=xnbin-1;ibin>=0;ibin--)
			    { 
			      double               val,xe,ye;

			      if(OHistogramGetBinNumberOfEntries (histloop,ibin,jbin)<=0) continue;

			      val                  = OHistogramGetBinSumOfWeights (histloop,ibin,jbin);
		  
			      xx                   = XMIN + ibin * XDBIN;
			      xe                   = xx + XDBIN;
			      yy                   = YMIN + jbin * YDBIN;
			      ye                   = yy + YDBIN;
		  
			      xx                   = VerifyLog (xx ,xmin,dx  ,xlog);
			      xe                   = VerifyLog (xe ,xmin,dx  ,xlog);
			      yy                   = VerifyLog (yy ,ymin,dy  ,ylog);
			      ye                   = VerifyLog (ye ,ymin,dy  ,ylog);
			      
			      if((xx>=0.)&&(xx<=1.)  &&
				 (xe>=0.)&&(xe<=1.)  &&
				 (yy>=0.)&&(yy<=1.)  &&
				 (ye>=0.)&&(ye<=1.)  
				 ) 
				{ 
				  char                 sval[32];
				  double               dx,dy;
				  ONode                gobin;

				  dx                   = xe-xx;
				  dy                   = ye-yy;
		      
				  sprintf              (sval,"%g",val);
				  gobin                = ONodeCreateF (lname+lhname+64,"%s:%s:%d:%d",name,hname,ibin,jbin);
				  ONodeAddChild        (This->gobins,gobin);
				  OPointListSetIthEntry      (points,0,xx    ,yy  ,0.);
				  OPointListSetIthEntry      (points,1,xe    ,yy  ,0.);
				  OPointListSetIthEntry      (points,2,xe    ,ye  ,0.);
				  OPointListSetIthEntry      (points,3,xx    ,ye  ,0.);
				  OPointListSetIthEntry      (points,4,xx    ,yy  ,0.);
				  GoAddLinesToNode           (gobin,points);
				  GoAddTextFieldToNode       (gobin,sval,xx+0.1*dx,yy+0.1*dy,0.,dx * 0.8,dy * 0.8);
				}
			      
			      
			    }
			}
		      
		      OPointListDelete (points);
		      CStringDelete    (hname);
		    } /*endif xnbin ynbin*/
		  
		} /* endif */
	      
	    } /* endif pdim */
	  
	} /* end for hist */
      
      for(iscat=a_scatn-1;iscat>=0;iscat--) 
	{
	  OScatter    scatloop;
	  scatloop = a_scats[iscat];

	  if(pdim==OScatterGetDimension(scatloop)) 
	    {

		  char*                scatname;
		  int                  lscatname;
		  int                  npoint;
		  int                  ipoint = 0;
                  OModeling            scatterModeling;
		  int                  one_node;

		  scatname             = OScatterGetName (scatloop);
		  scatname           = scatname!=NULL ? CStringDuplicate(scatname) : CStringCreateF(9+64,"OScatter/%lu",scatloop);
		  lscatname            = strlen(scatname);
		  npoint               = OScatterGetNumberOfEntries (scatloop);
  
		  if(iscat==0)	     OContextSetFromScript (OContextGetStaticInstance(),This->scatterContext);
		  else  	     OContextSetFromScript (OContextGetStaticInstance(),This->secondScatterContext);

                  scatterModeling    = OContextGetModeling(OContextGetStaticInstance());

		  one_node           = npoint>ONE_OBJECT ? 1 : 0;
		  points             = NULL;
		  if( (one_node==1)  || (scatterModeling==OModelingWireFrame) || (scatterModeling==OModelingSegments) )
		    {
		      points               = OPointListCreate (npoint);
		      ipoint               = 0;
		    }
		  if(one_node==0)
		    {
		      OPlotterSetPickability  (This,1);
		    }
		  for(count=0;count<npoint;count++)
		    {
		      OScatterGetIthPoint           (scatloop,count,&xx,&yy);
		      if(xlog==1)             
			{
			  if(xx<=0.) continue;
			  xx   = log10(xx);
			}
		      xx                   = (xx   - xmin)/dx;
		      if(ylog==1)
			{
			  if(yy<=0.) continue;
			  yy   = log10(yy);
			}
		      yy                   = (yy   - ymin)/dy;
		      if((xx>=0.)&&(xx<=1.)&&(yy>=0.)&&(yy<=1.)) 
			{
			  if(one_node==1)  
			    {
			      OPointListSetIthEntry      (points,ipoint,xx,yy,0.);
			      ipoint++;
			    } 
			  else 
			    {
			      ONode             gopoint;
			      gopoint           = ONodeCreateF (lname+lscatname+32,"%s:%s:%d",name,scatname,count);
			      ONodeAddChild     (This->gobins,gopoint);
			      GoAddMarkerToNode (gopoint,xx,yy,0.);
			      if( (scatterModeling==OModelingWireFrame) || (scatterModeling==OModelingSegments) )
				{
				  OPointListSetIthEntry      (points,ipoint,xx,yy,0.);
				  ipoint++;
				} 
			    }
			}
		    }
		  if(one_node==1)  
		    {
		      ONode                gocurve;
		      OPointListLimit      (points,ipoint);
		      ONodeSetPickability  (gocurve = ONodeCreateF(lname+lscatname+1,"%s:%s",name,scatname),0);
		      ONodeAddChild        (This->gobins,gocurve);
		      if(scatterModeling==OModelingWireFrame)
			{
			  GoAddLinesToNode          (gocurve,points);
			}
		      else if(scatterModeling==OModelingSegments)
			{
			  GoAddSegmentsToNode       (gocurve,points);
			}
		      else
			{
			  GoAddMarkersToNode        (gocurve,points);
			}
		      OPointListDelete          (points);
		    } 
		  else
		    {
		      if(scatterModeling==OModelingWireFrame)
			{
			  ONode                gocurve;
			  OPointListLimit      (points,ipoint);
			  ONodeSetPickability  (gocurve = ONodeCreateF(lname+lscatname+7,"%s:%s:curve",name,scatname),0);
			  ONodeAddChild        (This->gobins,gocurve);
			  GoAddLinesToNode     (gocurve,points);
			  OPointListDelete     (points);
			} 
		      else if(scatterModeling==OModelingSegments)
			{
			  ONode                gocurve;
			  OPointListLimit      (points,ipoint);
			  ONodeSetPickability  (gocurve = ONodeCreateF(lname+lscatname+7,"%s:%s:curve",name,scatname),0);
			  ONodeAddChild        (This->gobins,gocurve);
			  GoAddSegmentsToNode  (gocurve,points);
			  OPointListDelete     (points);
			} 
		    }
		  CStringDelete(scatname);
		  
		} /* endif pdim */
	  
	} /* end for scat */
      
    } /* end plot type */

  else if (This->layout==OLayoutXYZ)  /*pdim==3*/
    {
      
      for(iscat=a_scatn-1;iscat>=0;iscat--) 
	{
	  OScatter    scatloop;
	  scatloop = a_scats[iscat];

	  if(pdim==OScatterGetDimension(scatloop)) 
	    {

		  char*                scatname;
		  int                  lscatname;
		  int                  npoint;
		  int                  ipoint= 0;
                  OModeling            scatterModeling;
		  int                  one_node;

		  scatname             = OScatterGetName (scatloop);
		  scatname           = scatname!=NULL ? CStringDuplicate(scatname) : CStringCreateF(9+64,"OScatter/%lu",scatloop);
		  lscatname            = strlen(scatname);
		  npoint               = OScatterGetNumberOfEntries (scatloop);
		  
		  if(iscat==0)	       OContextSetFromScript (OContextGetStaticInstance(),This->scatterContext);
		  else  	       OContextSetFromScript (OContextGetStaticInstance(),This->secondScatterContext);

                  scatterModeling    = OContextGetModeling(OContextGetStaticInstance());

		  one_node           = npoint>ONE_OBJECT ? 1 : 0;
		  points             = NULL;
		  if( (one_node==1)  || (scatterModeling==OModelingWireFrame) || (scatterModeling==OModelingSegments) )
		    {
		      points = OPointListCreate (npoint);
		      ipoint = 0;
		    }
		  if(one_node==0) 
		    {
		      OPlotterSetPickability  (This,1);
		    }
		  for(count=0;count<npoint;count++)
		    {
		      OScatterGetIthPoint (scatloop,count,&xx,&yy,&zz);
		      if(xlog==1)
			{
			  if(xx<=0.) continue;
			  xx   = log10(xx);
			}
		      xx                   = (xx   - xmin)/dx;
		      if(ylog==1)
			{
			  if(yy<=0.) continue; 
			  yy   = log10(yy);
			}
		      yy                   = (yy   - ymin)/dy;
		      if(zlog==1)
			{
			  if(zz<=0.) continue;
			  zz   = log10(zz);
			}
		      zz                   = (zz   - zmin)/dz;
		      if( (xx>=0.)&&(xx<=1.) && 
			  (yy>=0.)&&(yy<=1.) &&
			  (zz>=0.)&&(zz<=1.)
			 ) 
			{
			  if(one_node==1)  
			    {
			      OPointListSetIthEntry      (points,ipoint,xx,yy,zz);
			      ipoint++;
			    } 
			  else 
			    {
			      ONode             gopoint;
			      gopoint           = ONodeCreateF (lname+lscatname+32,"%s:%s:%d",name,scatname,count);
			      ONodeAddChild     (This->gobins,gopoint);
			      GoAddMarkerToNode (gopoint,xx,yy,zz);
			      if( (scatterModeling==OModelingWireFrame) || (scatterModeling==OModelingSegments) )
				{
				  OPointListSetIthEntry (points,ipoint,xx,yy,zz);
				  ipoint++;
				} 
			    }
			}
		    }
		  if(one_node==1)  
		    {
		      ONode                gocurve;
		      OPointListLimit      (points,ipoint);
		      ONodeSetPickability  (gocurve = ONodeCreateF(lname+lscatname+1,"%s:%s",name,scatname),0);
		      ONodeAddChild        (This->gobins,gocurve);
		      if(scatterModeling==OModelingWireFrame)
			{
			  GoAddLinesToNode          (gocurve,points);
			}
		      else if(scatterModeling==OModelingSegments)
			{
			  GoAddSegmentsToNode       (gocurve,points);
			}
		      else
			{
			  GoAddMarkersToNode        (gocurve,points);
			}
		      OPointListDelete          (points);
		    } 
		  else 
		    {
		      if(scatterModeling==OModelingWireFrame)
			{
			  ONode                   gocurve;
			  OPointListLimit         (points,ipoint);
			  ONodeSetPickability     (gocurve = ONodeCreateF(lname+lscatname+7,"%s:%s:curve",name,scatname),0);
			  ONodeAddChild           (This->gobins,gocurve);
			  GoAddLinesToNode        (gocurve,points);
			  OPointListDelete        (points);
			} 
		      else if(scatterModeling==OModelingSegments)
			{
			  ONode                gocurve;
			  OPointListLimit      (points,ipoint);
			  ONodeSetPickability  (gocurve = ONodeCreateF(lname+lscatname+7,"%s:%s:curve",name,scatname),0);
			  ONodeAddChild        (This->gobins,gocurve);
			  GoAddSegmentsToNode  (gocurve,points);
			  OPointListDelete     (points);
			} 
		    }
		  CStringDelete(scatname);
		  
		} /* endif pdim */
	  
	} /* end for scat */
      
    } /* end plot type */

/* functions */
  for(ifunc=a_funcn-1;ifunc>=0;ifunc--) 
    {
      int    fdim;
      char*  fname;
      int    lfname;
      OFunction  funcloop;
      funcloop = a_funcs[ifunc];
      fdim     = OFunctionGetDimension   (funcloop);
      fname    = OFunctionGetName  (funcloop);
      fname    = fname!=NULL ? CStringDuplicate(fname) : CStringCreateF(10+64,"OFunction/%lu",funcloop);
      lfname   = strlen(fname);
      if(fdim!=pdim)
        {
	}
      else if(fdim==1)
        { 
	  int    ibin,ipt;
          double df;
          int    nbpt;
          OFunctionGetIthAxisAttributesF (funcloop,OAxisX,"n",&nbpt);
          nbpt         = nbpt <=0 ? 100 : nbpt;
          df           = (XMAX-XMIN)/nbpt;      
          points       = OPointListCreate (nbpt+1);
          ipt          = 0;
          for(ibin=0;ibin<nbpt+1;ibin++)
            { double xx,val;
              xx                   = XMIN + ibin * df;
              val                  = OFunctionEvaluateF (funcloop,1,xx);
              val                  = VerifyLog (val,vmin,dval,vlog);
              xx                   = VerifyLog (xx ,xmin,dx  ,xlog);

              val                  = val > 1. ? 1. : val;
              val                  = val < 0. ? 0. : val;
              if((xx >=0.)&&(xx<=1.) 
                )
                {
                  OPointListSetIthEntry     (points,ipt,xx,val,0.);ipt++;
                }
            }
	  if(ifunc==0)	 OContextSetFromScript (OContextGetStaticInstance(),This->functionContext);
	  else  	 OContextSetFromScript (OContextGetStaticInstance(),This->secondFunctionContext);

	 {ONode                gofunc;
          ONodeSetPickability  (gofunc = ONodeCreateF (lname+lfname+1,"%s:%s",name,fname),0);
	  ONodeAddChild        (This->gofunc,gofunc);
          OPointListLimit      (points,ipt);
          GoAddLinesToNode     (gofunc,points);}
          OPointListDelete     (points);
        }      
      else if(fdim==2) 
        {
	  int       ibin,jbin,nx,ny;
	  double    dfx,dfy;
	  double    *vprev;
	  OPointList   ptsx;  /* border x */
	  int       posx = 0;
	  OPointList   ptsy;  /* border y */
	  int       posy = 0;
	  OFunctionGetIthAxisAttributesF (funcloop,OAxisX,"n",&nx);
	  OFunctionGetIthAxisAttributesF (funcloop,OAxisY,"n",&ny);
	  nx        = nx <=0 ? 20 : nx;
	  ny        = ny <=0 ? 20 : ny;
	  dfx       = (XMAX-XMIN)/nx;
	  dfy       = (YMAX-YMIN)/ny;
	  vprev     = (double*)CMemoryAllocateBlock(2*nx*sizeof(double));
	  if(vprev!=NULL) 
	    {
	      ONode       gofunc;
	      OPainting binPaint;

	      points    = OPointListCreate (4);
	      ptsx      = OPointListCreate (nx+1+2);
	      ptsy      = OPointListCreate (ny+1+2);
	      
	      ONodeSetPickability (gofunc = ONodeCreateF (lname+lfname+1,"%s:%s",name,fname),0);
	      ONodeAddChild       (This->gofunc,gofunc);

	      if(ifunc==0)	  OContextSetFromScript (OContextGetStaticInstance(),This->functionContext);
	      else  	          OContextSetFromScript (OContextGetStaticInstance(),This->secondFunctionContext);

	      binPaint            = OContextGetPainting(OContextGetStaticInstance());

	      for(jbin=ny-1;jbin>=0;jbin--)
	       { 
		 double o1 = XMAX,o4 = XMAX;
		 for(ibin=nx-1;ibin>=0;ibin--)
		   { 
		     double              xx,yy,xe,ye,val1,val2,val3,val4;
		     xx                  = XMIN + ibin * dfx;
		     yy                  = YMIN + jbin * dfy;
		     xe                  = xx + dfx;
		     ye                  = yy + dfy;
		     
		     val1                  =  OFunctionEvaluateF (funcloop,2,xx,yy);
		     OContextSetColorIndex (OContextGetStaticInstance(),GetBinColor (binPaint,val1,VMIN,VMAX));
		     
		     if(ibin==nx-1)
		       val2                =  OFunctionEvaluateF (funcloop,2,xe,yy);
		     else
		       val2                = o1;
		     if(jbin==ny-1)
		       {
			 if(ibin==nx-1)
			   val3                  =  OFunctionEvaluateF (funcloop,2,xe,ye);
			 else
			   val3                  = o4;
			 val4                  =  OFunctionEvaluateF (funcloop,2,xx,ye);
		       }
		     else
		       {
			 val3                  = vprev[2*ibin+1];
			 val4                  = vprev[2*ibin];
		       }
		     vprev[2*ibin]       = val1;
		     vprev[2*ibin+1]     = val2;
		     o4                  = val4;
		     o1                  = val1;

		     val1                  = VerifyLog (val1,vmin,dval,vlog);
		     val2                  = VerifyLog (val2,vmin,dval,vlog);
		     val3                  = VerifyLog (val3,vmin,dval,vlog);
		     val4                  = VerifyLog (val4,vmin,dval,vlog);
		     xx                  = VerifyLog (xx,xmin,dx,xlog);
		     xe                  = VerifyLog (xe,xmin,dx,xlog);
		     yy                  = VerifyLog (yy,ymin,dy,ylog);
		     ye                  = VerifyLog (ye,ymin,dy,ylog);
		     
		     val1                  = val1 > 1. ? 1. : val1;
		     val2                  = val2 > 1. ? 1. : val2;
		     val3                  = val3 > 1. ? 1. : val3;
		     val4                  = val4 > 1. ? 1. : val4;
		     
		     val1                  = val1 < 0. ? 0. : val1;
		     val2                  = val2 < 0. ? 0. : val2;
		     val3                  = val3 < 0. ? 0. : val3;
		     val4                  = val4 < 0. ? 0. : val4;

		     if((xx>=0.)&&(xx<=1.)   &&
			(xe>=0.)&&(xe<=1.)   &&
			(yy>=0.)&&(yy<=1.)   &&
			(ye>=0.)&&(ye<=1.)  
			) 
		       {
			 OPointListSetIthEntry      (points,0,xx,yy,val1); 
			 OPointListSetIthEntry      (points,1,xe,yy,val2); 
			 OPointListSetIthEntry      (points,2,xe,ye,val3); 
			 OPointListSetIthEntry      (points,3,xx,ye,val4); 
			 GoAddPolygonToNode         (gofunc,points);
			 if(ibin==0) 
			   {
			     if(jbin==ny-1) 
			       {
				 OPointListSetIthEntry (ptsy,posy,xx,ye,0.);posy++;
				 OPointListSetIthEntry (ptsy,posy,xx,ye,val1);posy++;
				 OPointListSetIthEntry (ptsy,posy,xx,yy,val1);posy++;
			       }
			     else if(jbin==0)
			       {
				 OPointListSetIthEntry (ptsy,posy,xx,yy,val1);posy++;
				 OPointListSetIthEntry (ptsy,posy,xx,yy,0.);posy++;
			       }
			     else
			       {
				 OPointListSetIthEntry (ptsy,posy,xx,yy,val1);posy++;
			       }
			   } 
			 if(jbin==0) 
			   {
			     if(ibin==nx-1) 
			       {
				 OPointListSetIthEntry (ptsx,posx,xe,yy,0.);posx++;
				 OPointListSetIthEntry (ptsx,posx,xe,yy,val1);posx++;
				 OPointListSetIthEntry (ptsx,posx,xx,yy,val1);posx++;
			       }
			     else if(ibin==0)
			       {
				 OPointListSetIthEntry (ptsx,posx,xx,yy,val1);posx++;
				 OPointListSetIthEntry (ptsx,posx,xx,yy,0.);posx++;
			       }
			     else
			       {
				 OPointListSetIthEntry (ptsx,posx,xx,yy,val1);posx++;
			       }
			   } 
			 
		       }
		   }
	       }
	     
	     {
	      ONode goborder;
	      /* border x */
	      ONodeSetPickability    (goborder = ONodeCreateF (lname+lfname+9,"%s:%s:xborder",name,fname),0);
	      ONodeAddChild          (This->gofunc,goborder);
	      OContextSetColorIndex  (OContextGetStaticInstance(),OColorBrown);
	      OPointListLimit        (ptsx,posx);
	      GoAddPolygonToNode     (goborder,ptsx);
	     /* border y */
	      ONodeSetPickability    (goborder = ONodeCreateF (lname+lfname+9,"%s:%s:yborder",name,fname),0);
	      ONodeAddChild          (This->gofunc,goborder);
	      OContextSetColorIndex  (OContextGetStaticInstance(),OColorBrown);
	      OPointListLimit        (ptsy,posy);
 	      OPointListReverse      (ptsy);
	      GoAddPolygonToNode     (goborder,ptsy);
	    }
	     
	      OPointListDelete       (points);
	      OPointListDelete       (ptsx);
	      OPointListDelete       (ptsy);
	      CMemoryFreeBlock       (vprev);
	    } /*endif vprev*/
	}      

      CStringDelete(fname);
      
    } /* end for func */

  if( (ONodeGetPrimitives(This->gogrid)==NULL) && (This->gridsVisible==1) )
    {
      if(This->layout==OLayoutXV)
	{
	  int   pos = 0;
	  int   number;
	  
	  OContextRestoreDefaultValues (OContextGetStaticInstance());
	  OContextSetFromScript        (OContextGetStaticInstance(),This->gridContext);
	  points              = OPointListCreate(2 * (OAxisGetNumberOfTicks(This->axis[0]) + OAxisGetNumberOfTicks(This->value))  );
	  number              = OAxisGetNumberOfTicks(This->axis[0]);
	  for(count=0;count<number;count++)
	    { 
	      xx = (OAxisGetIthTickValue(This->axis[0],count)-xmin)/dx;
	      if((xx>=0.)&&(xx<=1.))
		{
		  OPointListSetIthEntry    (points,pos,xx, 0.,0.);pos++;
		  OPointListSetIthEntry    (points,pos,xx, 1.,0.);pos++;
		} 
	    }
	  number = OAxisGetNumberOfTicks(This->value);
	  for(count=0;count<number;count++)
	    {
	      yy = (OAxisGetIthTickValue(This->value,count)-vmin)/dval;
	      if((yy>=0.)&&(yy<=1.))
		{
		  OPointListSetIthEntry    (points,pos,0.,yy,0.);pos++;
		  OPointListSetIthEntry    (points,pos,1.,yy,0.);pos++;
		} 
	    }
	  OPointListLimit       (points,pos);
	  GoAddSegmentsToNode   (This->gogrid,points);
	  OPointListDelete      (points);
	}
      else if(This->layout==OLayoutXY)
	{
	  int   pos = 0;
	  int   number;
	  OContextRestoreDefaultValues (OContextGetStaticInstance());
	  OContextSetFromScript        (OContextGetStaticInstance(),This->gridContext);
	  points = OPointListCreate(2 * (OAxisGetNumberOfTicks(This->axis[0]) + OAxisGetNumberOfTicks(This->axis[1]))  );
	  number = OAxisGetNumberOfTicks(This->axis[0]);
	  for(count=0;count<number;count++)
	    { 
	      xx = (OAxisGetIthTickValue(This->axis[0],count)-xmin)/dx;
	      if((xx>=0.)&&(xx<=1.))
		{
		  OPointListSetIthEntry    (points,pos,xx, 0.,0.);pos++;
		  OPointListSetIthEntry    (points,pos,xx, 1.,0.);pos++;
		} 
	    }
	  number = OAxisGetNumberOfTicks(This->axis[1]);
	  for(count=0;count<number;count++)
	    {
	      yy = (OAxisGetIthTickValue(This->axis[1],count)-ymin)/dy;
	      if((yy>=0.)&&(yy<=1.))
		{
		  OPointListSetIthEntry    (points,pos,0.,yy,0.);pos++;
		  OPointListSetIthEntry    (points,pos,1.,yy,0.);pos++;
		} 
	    }
	  OPointListLimit       (points,pos);
	  GoAddSegmentsToNode   (This->gogrid,points);
	  OPointListDelete      (points);
	}
    }
  OContextRestoreDefaultValues (OContextGetStaticInstance());
  OContextSetFromScript        (OContextGetStaticInstance(),This->textContext);

  rule_height  = 0.1;

/* axe x */
  RepresentAxis (This->goaxis[OAxisX],This->axis[0],This->layout,OAxisX,rule_height,This->textScale);

/* axe y */
  RepresentAxis (This->goaxis[OAxisY],This->axis[1],This->layout,OAxisY,rule_height,This->textScale);

/* axe z */
  RepresentAxis (This->goaxis[OAxisZ],This->axis[2],This->layout,OAxisZ,rule_height,This->textScale);

/* value */
  RepresentAxis (This->goaxisv,This->value,This->layout,OAxisV,rule_height,This->textScale);
      
  if( (ONodeGetPrimitives(This->gotitle)==NULL) && (This->drawTitle==1) )
    {
      double vx = 0.,vy= 0.,vz = 0.;
      double TXx,TXy,TXz;
      double TYx,TYy,TYz;
      TXx    = 1.;
      TXy    = 0.;
      TXz    = 0.;
      TYx    = 0.;
      TYy    = 1.;
      TYz    = 0.;
      if(This->layout==OLayoutXV)
	{ 
	  vx  = 0.5;
	  vy  = This->titlePosition==0 ? 1.+rule_height/4. : -2.5 * rule_height;
	  vz  = 0.;
	  TXx = 1.;
	  TXy = 0.;
	  TXz = 0.;
	  TYx = 0.;
	  TYy = 1.;
	  TYz = 0.;
	}
      else if(This->layout==OLayoutXY)
	{ 
	  vx  = 0.5;
	  vy  = This->titlePosition==0 ? 1.+rule_height/4. : -2.5 * rule_height;
	  vz  = 0.;
	  TXx = 1.;
	  TXy = 0.;
	  TXz = 0.;
	  TYx = 0.;
	  TYy = 1.;
	  TYz = 0.;
	}
      else if((This->layout==OLayoutXYV) || (This->layout==OLayoutXYZ)) 
	{
	  vx  = -0.1;
	  vy  = -0.1;
	  vz  = -0.1;
	  TXx =  1.;
	  TXy = -1.;
	  TXz =  0.;
	  TYx =  0.;
	  TYy =  0.;
	  TYz =  1.;
	}
     {int                      cameraAligned = 1;
      double                   text_size;
      text_size                = rule_height * This->textScale/2.5;
      if( (cameraAligned==1) && ((This->layout==OLayoutXYV) || (This->layout==OLayoutXYZ)) ) 
	{ 
	  vx  = -0.1;
	  vy  = -0.1;
	  vz  = -0.1;
	  TXx =  1.;
	  TXy =  0.;
	  TXz =  0.;
	  TYx =  0.;
	  TYy =  1.;
	  TYz =  0.;
	  ONodeSetPickability   (This->gotitle,1);
	  ONodeSetCameraAligned (This->gotitle,1);
	  ONodeSetHookPoint     (This->gotitle,vx,vy,vz);
	}
      else
	{
	  ONodeSetPickability   (This->gotitle,0);
	  ONodeSetCameraAligned (This->gotitle,0);
	}
      GoAddAlignedStringToNode (This->gotitle,
				This->title,
				vx,vy,vz,
				TXx,TXy,TXz,
				TYx,TYy,TYz,
				text_size,'C','B');}
    }

  if( (ONodeGetPrimitives(This->goColorMapping)==NULL) && (drawColorMapping==1) )
    {
      int        colorn;
      int*       colors = NULL;
      double*    values = NULL;
      colorn     = OColormapGetValueColorMapping (This->colormap,This->valueColorMapping,&values,&colors);
      points     = OPointListCreate (4);
      if(colorn>0)
	{
	  double     width  = 0.15;
	  double     margin = 0.05;
	  double     height;
	  double     xx = 1.01;
	  double     yy = 0.;
	  height     = (1. - margin * (colorn + 1))/colorn;
	  OContextRestoreDefaultValues (OContextGetStaticInstance());
	  for(count=0;count<colorn;count++)
	    {
	      OContextSetColorIndex (OContextGetStaticInstance(),colors[count]);
	      OPointListSetIthEntry (points,0,xx          ,yy + margin          ,0.);
	      OPointListSetIthEntry (points,1,xx + width  ,yy + margin          ,0.);
	      OPointListSetIthEntry (points,2,xx + width  ,yy + margin + height ,0.);
	      OPointListSetIthEntry (points,3,xx          ,yy + margin + height ,0.);
	      GoAddPolygonToNode    (This->goColorMapping,points); 
	      OContextSetColorIndex (OContextGetStaticInstance(),OColorBlack);
	      if(values!=NULL)
		{
		  char*          string = NULL;
		  if(count==0)
		    {
		      string = CStringCreateF (1+64,"<%g",values[1]);
		    }
		  else if(count==colorn-1)
		    {
		      string = CStringCreateF (2+64,"%g<=",values[count]);
		    }
		  else
		    {
		      string = CStringCreateF (4+2*64,"%g<= <%g",values[count],values[count+1]);
		    }
		  if(string!=NULL) 
		    {
		      int       length;
		      double    text_size;
		      length    = strlen(string);
		      text_size = MINIMUM(width / length,margin);
		      GoAddAlignedStringToNode (This->goColorMapping,
						string,
						xx + width/2.,yy+0.01,0.,
						1.,0.,0.,
						0.,1.,0.,
						text_size * 0.9,'C','B');
		    }
		  CStringDelete  (string);
		}
	      yy += height + margin;
	    }
	}
      CMemoryFreeBlock  (values);
      CMemoryFreeBlock  (colors);
      OPointListDelete  (points);
    }

 CStringDelete (name);
}
/***************************************************************************/
static void RepresentAxis (
 ONode      a_goaxis
,OAxis      a_axis         
,OLayout    a_frameType        
,OAxisIndex a_type        
,double     a_height      
,double     a_scale
)
/***************************************************************************/
/* if a_height < 0. tick are downward                                      */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        number;
  int        mag;
  OMatrix    matrix,rotz,roty,trans;
  double     TXx,TXy,TXz;
  double     TYx,TYy,TYz;
  double     vx,vy,vz;
  int        tick_up = 1;
  char*      title;
  double     text_size;
  int        cameraAligned = 1;
/*.........................................................................*/
  if(a_axis==NULL)  return;
  if(ONodeGetPrimitives(a_goaxis)!=NULL) return; /*done*/

  vx     = 0.;
  vy     = 0.;
  vz     = 0.;

  TXx    = 0.;
  TXy    = 0.;
  TXz    = 0.;
  TYx    = 0.;
  TYy    = 0.;
  TYz    = 0.;

  if((a_type==OAxisX)&&((a_frameType==OLayoutXV)||(a_frameType==OLayoutXY) ))
    {
      matrix = OMatrixCreate(OMatrixIdentity);
      TXx    = 1.;
      TXy    = 0.;
      TXz    = 0.;
      TYx    = 0.;
      TYy    = 1.;
      TYz    = 0.;
    }
  else if(  ((a_type==OAxisV)&&(a_frameType==OLayoutXV))  ||
            ((a_type==OAxisY)&&(a_frameType==OLayoutXY))  )
    {
      rotz          = OMatrixCreate   (OMatrixRotationZ,M_PI_2);
      roty          = OMatrixCreate   (OMatrixRotationY,M_PI);
      matrix        = OMatrixMultiply (roty,rotz);
      OMatrixDelete (roty);
      OMatrixDelete (rotz);
      TXx    = 1.;
      TXy    = 0.;
      TXz    = 0.;
      TYx    = 0.;
      TYy    = 1.;
      TYz    = 0.;
    }

  else if((a_type==OAxisX) && ( (a_frameType==OLayoutXYV) || (a_frameType==OLayoutXYZ)))
    {
      matrix       = OMatrixCreate(OMatrixRotationX,M_PI_2);
      tick_up      = 0;
      if(cameraAligned==1)
        {
	  TXx    = 1.;
	  TXy    = 0.;
	  TXz    = 0.;
	  TYx    = 0.;
	  TYy    = 1.;
	  TYz    = 0.;
	}
      else
	{
	  TXx    = 1.;
	  TXy    = 0.;
	  TXz    = 0.;
	  TYx    = 0.;
	  TYy    = 0.;
	  TYz    = 1.;
	}
    }
  else if((a_type==OAxisY) && ( (a_frameType==OLayoutXYV) || (a_frameType==OLayoutXYZ))) 
    {
      rotz           = OMatrixCreate   (OMatrixRotationZ, M_PI_2);
      roty           = OMatrixCreate   (OMatrixRotationY, M_PI_2);
      matrix         = OMatrixMultiply (roty,rotz);
      OMatrixDelete  (roty);
      OMatrixDelete  (rotz);
      tick_up        = 0;
      if(cameraAligned==1)
        {
	  TXx    = 1.;
	  TXy    = 0.;
	  TXz    = 0.;
	  TYx    = 0.;
	  TYy    = 1.;
	  TYz    = 0.;
	}
      else
	{
	  TXx    = 0.;
	  TXy    =-1.;
	  TXz    = 0.;
	  TYx    = 0.;
	  TYy    = 0.;
	  TYz    = 1.;
	}
    }
  else if(  ( (a_type==OAxisV)&&(a_frameType==OLayoutXYV) ) ||
            ( (a_type==OAxisZ)&&(a_frameType==OLayoutXYZ) ) 
         )
    {
      roty         = OMatrixCreate(OMatrixRotationY,-M_PI_2);
      rotz         = OMatrixCreate(OMatrixRotationZ,-M_PI_2);
      trans        = OMatrixCreate(OMatrixTranslation,0.,1.,0.);
      matrix       = OMatrixMultiplyMatrices (3,trans,rotz,roty);
      OMatrixDelete  (trans);
      OMatrixDelete  (roty);
      OMatrixDelete  (rotz);
      tick_up      = 0;
      if(cameraAligned==1)
        {
	  TXx    = 1.;
	  TXy    = 0.;
	  TXz    = 0.;
	  TYx    = 0.;
	  TYy    = 1.;
	  TYz    = 0.;
	}
      else
	{
	  TXx    = 1.;
	  TXy    = 0.;
	  TXz    = 0.;
	  TYx    = 0.;
	  TYy    = 0.;
	  TYz    = 1.;
	}
    }
  else 
    {
      return;
    }

  GoAddArrowToNode      (a_goaxis,matrix,0);

  text_size  = a_height * 0.35 * a_scale; /* original 0.5 */
  number     = OAxisGetNumberOfTicks(a_axis);
  if(number>=2)
    { 
      int         count;
      double      range,min,max,xx,yy;
      int         is_log;
      OPointList  points;
      double      twidth,xxnxt;
      OAxisGetAttributesF (a_axis,"mxl",&min,&max,&is_log);
      if(is_log==1)
        {
          if((min<=0.) || (max<=0.) )
            {
	      is_log = 0;
            }
	  else
	    {
	      min = log10(min);
	      max = log10(max);
	    }
        }
      range  = max - min;

      points = OPointListCreate(2);
      for(count=0;count<number;count++)
        { 
          xx                 = (OAxisGetIthTickValue(a_axis,count)-min)/range;
          yy                 = tick_up==1 ? a_height/4. : -a_height/4.;
          if((xx>=0.)&&(xx<=1.))
            {
              OPointListSetIthEntry (points,0,xx, 0.,0.);
              OPointListSetIthEntry (points,1,xx, yy,0.);
              MultiplyPoints        (matrix,points);
              GoAddLinesToNode      (a_goaxis,points);
            }
        }
      OPointListDelete (points);

/* get text tick size */
      twidth = 10000.;
      xx     = (OAxisGetIthTickValue(a_axis,0)-min)/range;
      for(count=0;count<number-1;count++)
        { 
          xxnxt  = (OAxisGetIthTickValue(a_axis,count+1)-min)/range;
          twidth = MINIMUM (twidth,(xxnxt-xx));
          xx     = xxnxt;
        }
      twidth  = MINIMUM (twidth,text_size);
      twidth *= 0.8;

      yy      = - a_height * .9;   /* original .5 */
      for(count=0;count<number;count++)
        { 
          xx  = (OAxisGetIthTickValue(a_axis,count)-min)/range;
          if((xx>=0.)&&(xx<=1.))
            {
              OMatrixMultiplyVector (matrix,xx,yy,0.,&vx,&vy,&vz);
	      if( (cameraAligned==1) && ((a_frameType==OLayoutXYV) || (a_frameType==OLayoutXYZ)))
		{
		  char*                name;
		  ONode                node; 
		  name                 = ONodeGetName (a_goaxis);
		  node                 = OShapeAlignedStringCreateF (
					  OAxisGetIthTickLabel(a_axis,count),
					  vx,vy,vz,
					  TXx,TXy,TXz,
					  TYx,TYy,TYz,
					  twidth,'C','B',
					  OContextGetStaticInstance(),
					  strlen(name)+6+32,"%s:tick_%d",name,count);
		/*ONodeSetPickability  (node,0);*/
		  ONodeAddChild        (a_goaxis,node);
		}
	      else
		{
		  GoAddAlignedStringToNode (a_goaxis,
					    OAxisGetIthTickLabel(a_axis,count),
					    vx,vy,vz,
					    TXx,TXy,TXz,
					    TYx,TYy,TYz,
					    twidth,'C','B');
		}
            }
        }
    }

/* draw magnitude */
  OAxisGetAttributesF (a_axis,"g",&mag);
  if(mag!=0)
    { 
      char*                    string;
      OMatrixMultiplyVector    (matrix,1.0,- 1.8 * a_height,0.,&vx,&vy,&vz);
      string                   = mag>=0 ? CStringCreateF(4+16,"x10+%d",mag) : 
	                                  CStringCreateF(4+16,"x10-%d",abs(mag));
      if( (cameraAligned==1) && ((a_frameType==OLayoutXYV) || (a_frameType==OLayoutXYZ)) )
	{
	  char*                name;
	  ONode                node; 
	  name                 = ONodeGetName (a_goaxis);
	  node                 = OShapeAlignedStringCreateF (
                                  string,		
				  vx,vy,vz,
				  TXx,TXy,TXz,
				  TYx,TYy,TYz,
				  text_size*0.8,'C','B',
				  OContextGetStaticInstance(),
				  strlen(name)+4,"%s:mag",name);
	/*ONodeSetPickability  (node,0);*/
	  ONodeAddChild        (a_goaxis,node);
	}
      else
	{
	  GoAddAlignedStringToNode (a_goaxis,
				    string,
				    vx,vy,vz,
				    TXx,TXy,TXz,
				    TYx,TYy,TYz,
				    text_size*0.8,'C','B');
	}
      CStringDelete            (string);
    }

/* draw title */
  OAxisGetAttributesF (a_axis,"t",&title);
  if( (title!=NULL) && (*title!='\0'))
    { 
	  OMatrixMultiplyVector (matrix,.5,- 1.7 * a_height,0.,&vx,&vy,&vz);

          if((a_type==OAxisX)&&((a_frameType==OLayoutXV)||(a_frameType==OLayoutXY)))
            {
              TXx         = 1.;
              TXy         = 0.;
              TXz         = 0.;
              TYx         = 0.;
              TYy         = 1.;
              TYz         = 0.;
	      GoAddAlignedStringToNode (a_goaxis,
					title,
					vx,vy,vz,
					TXx,TXy,TXz,
					TYx,TYy,TYz,
					text_size,'C','B');
            }
          else if(  ((a_type==OAxisV)&&(a_frameType==OLayoutXV))  ||
                    ((a_type==OAxisY)&&(a_frameType==OLayoutXY))  )
            {
              TXx         = 0.;
              TXy         = 1.;
              TXz         = 0.;
              TYx         =-1.;
              TYy         = 0.;
              TYz         = 0.;
              GoAddAlignedStringToNode (a_goaxis,
					title,
					vx,vy,vz,TXx,
					TXy,TXz,TYx,
					TYy,TYz,
					text_size,'C','B');
            }
          else if((a_type==OAxisX) && ((a_frameType==OLayoutXYV) || (a_frameType==OLayoutXYZ)))
            {
	      if(cameraAligned==1)
		{
		  char*         name;
		  ONode         node; 
		  TXx           = 1.;
		  TXy           = 0.;
		  TXz           = 0.;
		  TYx           = 0.;
		  TYy           = 1.;
		  TYz           = 0.;
		  name          = ONodeGetName (a_goaxis);
		  node          = OShapeAlignedStringCreateF (
					   title,
					   vx,vy,vz,
					   TXx,TXy,TXz,
					   TYx,TYy,TYz,
					   text_size,'C','B',
					   OContextGetStaticInstance(),
					   strlen(name)+6,"%s:title",name);
	        /*ONodeSetPickability   (node,0);*/
		  ONodeAddChild (a_goaxis,node);
		}
	      else
		{
		  TXx         = 1.;
		  TXy         = 0.;
		  TXz         = 0.;
		  TYx         = 0.;
		  TYy         = 0.;
		  TYz         = 1.;
		  GoAddAlignedStringToNode  (a_goaxis,
					     title,
					     vx,vy,vz,
					     TXx,TXy,TXz,
					     TYx,TYy,TYz,
					     text_size,'C','B');
		}
            }
          else if((a_type==OAxisY) && ( (a_frameType==OLayoutXYV) || (a_frameType==OLayoutXYZ) ) )
            {
	      if(cameraAligned==1)
		{
		  char*         name;
		  ONode         node; 
		  TXx           = 1.;
		  TXy           = 0.;
		  TXz           = 0.;
		  TYx           = 0.;
		  TYy           = 1.;
		  TYz           = 0.;
		  name          = ONodeGetName (a_goaxis);
		  node          = OShapeAlignedStringCreateF (
				   title,
				   vx,vy,vz,
				   TXx,TXy,TXz,
				   TYx,TYy,TYz,
				   text_size,'C','B',
				   OContextGetStaticInstance(),
				   strlen(name)+6,"%s:title",name);
		/*ONodeSetPickability      (node,0);*/
		  ONodeAddChild (a_goaxis,node);
		}
	      else
		{
		  TXx         = 0.;
		  TXy         =-1.;
		  TXz         = 0.;
		  TYx         = 0.;
		  TYy         = 0.;
		  TYz         = 1.;
		  GoAddAlignedStringToNode (a_goaxis,
					    title,
					    vx,vy,vz,
					    TXx,TXy,TXz,
					    TYx,TYy,TYz,
					    text_size,'C','B');
		}
            }
          else if(  (a_type==OAxisV)&&(a_frameType==OLayoutXYV) ||
		    (a_type==OAxisZ)&&(a_frameType==OLayoutXYZ)
		  )
            {
	      if(cameraAligned==1)
		{
		  char*                    name;
		  ONode                    node; 
		  TXx                      = 0.;
		  TXy                      = 1.;
		  TXz                      = 0.;
		  TYx                      =-1.;
		  TYy                      = 0.;
		  TYz                      = 0.;
		  OMatrixMultiplyVector    (matrix,.5,- 4.0 * a_height,0.,&vx,&vy,&vz);
		  name                     = ONodeGetName (a_goaxis);
		  node                     = OShapeAlignedStringCreateF (
					      title,
					      vx,vy,vz,
					      TXx,TXy,TXz,
					      TYx,TYy,TYz,
					      text_size,'C','B',
					      OContextGetStaticInstance(),
					      strlen(name)+6,"%s:title",name);
		/*ONodeSetPickability      (node,0);*/
		  ONodeAddChild            (a_goaxis,node);
		}
	      else
		{
		  TXx         = 0.;
		  TXy         = 0.;
		  TXz         = 1.;
		  TYx         =-1.;
		  TYy         = 0.;
		  TYz         = 0.;
		  GoAddAlignedStringToNode (a_goaxis,
					    title,
					    vx,vy,vz,
					    TXx,TXy,TXz,
					    TYx,TYy,TYz,
					    text_size,'C','B');
		}
            }
    }

  OMatrixDelete (matrix);
}
/***************************************************************************/
static double VerifyLog (
 double     a_val  
,double     a_min  
,double     a_dx   
,int     a_log  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double val;
/*.........................................................................*/
  if(a_log==1) 
    val = a_val>0.? log10(a_val) : -FLT_MAX; /*-HUGE_VAL does not work on some system (exa: ULTRIX)*/
  else
    val = a_val;
  val          = (val - a_min)/a_dx;
  return       val;
}
/***************************************************************************/
static void MultiplyPoints (
 OMatrix This 
,OPointList a_list 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int number;
/*.........................................................................*/
  if(This==NULL) return;
  if(a_list==NULL) return;
  number = OPointListGetSize(a_list);
  for(count=0;count<number;count++) 
    {
      double             x,y,z;
      double             nx,ny,nz;
      OPointListGetIthEntry       (a_list,count,&x,&y,&z);
      OMatrixMultiplyVector       (This,x,y,z,&nx,&ny,&nz);
      OPointListSetIthEntry    (a_list,count,nx,ny,nz);
    }
}
/***************************************************************************/
static void EraseAxes (
 OPlotter This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
/*.........................................................................*/
  for(count=0;count<OAxisMaximumNumber;count++) 
    {
      ONodeDestroyChildren  (This->goaxis[count]);
      ONodeRemovePrimitives (This->goaxis[count]);
    }
  ONodeDestroyChildren  (This->goaxisv);
  ONodeRemovePrimitives (This->goaxisv);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static struct {
  char*   string;
  OLayout value;
} OLayoutList [] = {
  {"xv",    OLayoutXV},
  {"xy",    OLayoutXY},
  {"xyv",   OLayoutXYV},
  {"xyz",   OLayoutXYZ}
};  
/***************************************************************************/
char** OLayoutGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* Layout[4] = {"xv","xy","xyv","xyz"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 4;
  return Layout;
}
/***************************************************************************/
OLayout OLayoutGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OLayoutList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OLayoutList[count].string)==0) ) 
      return OLayoutList[count].value;
  CWarnF("%s not an OLayout.\n",a_string);
  return OLayoutXV;
}
/***************************************************************************/
char* OLayoutGetName (
 OLayout a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OLayoutList);count++)
    if(a_value==OLayoutList[count].value) return OLayoutList[count].string;
  CWarnF("%d not an OLayout.\n",a_value);
  return NULL;
}
/***************************************************************************/
int OLayoutIsValid (
 OLayout a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OLayoutList);count++)
    if(a_value==OLayoutList[count].value) return 1;
  return 0;
}
/***************************************************************************/
static struct {
  char*        string;
  OXY_Modeling value;
} OXY_ModelingList [] = {
  {"random", OXY_ModelingRandom},
  {"box",    OXY_ModelingBox},
  {"text",   OXY_ModelingText},
  {"color",   OXY_ModelingColor}
};  
/***************************************************************************/
char** OXY_ModelingGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* XY_Modeling[4] = {"random","box","text","color"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 4;
  return XY_Modeling;
}
/***************************************************************************/
OXY_Modeling OXY_ModelingGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OXY_ModelingList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OXY_ModelingList[count].string)==0) ) 
      return OXY_ModelingList[count].value;
  CWarnF("%s not an OXY_Modeling.\n",a_string);
  return OXY_ModelingRandom;
}
/***************************************************************************/
char* OXY_ModelingGetName (
 OXY_Modeling a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OXY_ModelingList);count++)
    if(a_value==OXY_ModelingList[count].value) return OXY_ModelingList[count].string;
  CWarnF("%d not an OXY_Modeling.\n",a_value);
  return NULL;
}
/***************************************************************************/
int OXY_ModelingIsValid (
 OXY_Modeling a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OXY_ModelingList);count++)
    if(a_value==OXY_ModelingList[count].value) return 1;
  return 0;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
char* OPlotterGetInformations (
 OPlotter This  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHistogram    hist;
  int      nbin;
  int*     is;
  int*     js;
  char*    sinfos;
  int      count;
  int      pos;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  hist             = (OHistogram)OPlotterGetHighlightedBins   (This,&nbin,&is,&js);
  if(hist==NULL)   return NULL;
  if(nbin==0)      return NULL;

  sinfos         = NULL;
  if(OHistogramGetDimension(hist)==1)
    { 
      sinfos  = (char*)CMemoryAllocateBlock( nbin * 64 * sizeof(char));
      if(sinfos==NULL) return NULL;
      pos     = CStringPrintF (sinfos,"");
      for(count=0;count<nbin;count++)
        {
           if(This->barsVisible==0)
             {
               pos += CStringPrintF
                      (sinfos+pos,"%-4d %-10g : %g\n",is[count],
	               OHistogramGetIthAxisBinMinimum(hist,OAxisX,is[count]),OHistogramGetBinSumOfWeights(hist,is[count]));
             }
           else
             {
               pos += CStringPrintF
                      (sinfos+pos,"%-4d %-10g : %-10g %g\n",is[count],
                       OHistogramGetIthAxisBinMinimum(hist,OAxisX,is[count]),
                       OHistogramGetBinSumOfWeights(hist,is[count]),OHistogramGetBinBar(hist,is[count]));
             }
        }
    }
/*debug  else if(OHistGetType(hist)==OHistScatter)
    {
      sinfos = (char*)CMemoryAllocateBlock( nbin * 64 * sizeof(char));
      if(sinfos==NULL) return NULL;
      pos    = CStringPrintF (sinfos,"");
      for(count=0;count<nbin;count++)
          { double xx,yy;
            OHistScatterPoint (hist,is[count],&xx,&yy);
            pos += CStringPrintF (sinfos+pos,"%-10g %-10g\n",xx,yy);
          }
    } */
  else if(OHistogramGetDimension(hist)==2)
    {
      sinfos = (char*)CMemoryAllocateBlock( nbin  * 64 * sizeof(char));
      if(sinfos==NULL) return NULL;
      pos    = CStringPrintF (sinfos,"");
      for(count=0;count<nbin;count++)
          {
            pos += CStringPrintF 
                   (sinfos+pos,"%-4d %-10g : %-4d %-10g:%g\n",is[count],
		    OHistogramGetIthAxisBinMinimum(hist,OAxisX,is[count]),
		    js[count],OHistogramGetIthAxisBinMinimum(hist,OAxisY,js[count]),
		    OHistogramGetBinSumOfWeights(hist,is[count],js[count]));
          }
    }
  CMemoryFreeBlock (is);
  CMemoryFreeBlock (js);
  return sinfos;
}
/***************************************************************************/
void* OPlotterGetHighlightedBins (
 OPlotter This
,int*  a_nbin
,int** a_is
,int** a_js
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      binn;
  int*     is;
  int*     js;
  ONode* items;
  ONode* list;
  ONode* itema;
  OHistogram    hist;
/*.........................................................................*/
  if(a_nbin!=NULL) *a_nbin = 0;
  if(a_is!=NULL)   *a_is   = NULL;
  if(a_js!=NULL)   *a_js   = NULL;
  if(This==NULL)   return NULL;

  items      = ONodeGetLeavesForward (This->gobins);
  list       = ONodeCollect          (items,OCollectHighlighted);
  CListDelete ((CList)items);

  hist       = NULL;
  binn       = 0;
  is         = NULL;
  js         = NULL;
/* get hist and bins min,max */
  for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      OHistogram    new_hist;
      int           ibin,jbin;
      new_hist      = GetBinFromString (ONodeGetName(*itema),&ibin,&jbin);
      if(new_hist==NULL) continue;
      if( (hist!=NULL) && (new_hist!=hist)) continue; /* ensure same histo */
      hist             = new_hist;
      if((OHistogramGetDimension(hist)==1)&&(ibin!=NotFound))
        {
          is = binn!=0 ? 
	       (int*)CMemoryChangeBlockSize((char*)is,(binn+1)*sizeof(int)) : 
	       (int*)CMemoryAllocateBlock(sizeof(int));
          if(is==NULL)  return NULL;
          is[binn] = ibin;
          binn++;
        }
/*debug else if((OHistGetType(hist)==OHistScatter)&&(ibin!=NotFound))
        {
          is = binn!=0 ? 
	       (int*)CMemoryChangeBlockSize(is,(binn+1)*sizeof(int)):
	       (int*)CMemoryAllocateBlock(sizeof(int));
          if(is==NULL) return NULL;
          is[binn] = ibin;
          binn++;
        } */
      else if((OHistogramGetDimension(hist)==2)&&(ibin!=NotFound)&&(jbin!=NotFound))
        {
          is = binn!=0 ? 
	       (int*)CMemoryChangeBlockSize(is,(binn+1)*sizeof(int)) : 
	       (int*)CMemoryAllocateBlock(sizeof(int));
          js = binn!=0 ? 
	       (int*)CMemoryChangeBlockSize(js,(binn+1)*sizeof(int)) : 
	       (int*)CMemoryAllocateBlock(sizeof(int));
          if( (is==NULL) || (js==NULL) )  
	    {
	      CMemoryFreeBlock (is);
	      CMemoryFreeBlock (js);
	      return   NULL;
	    }
          is[binn] = ibin;
          js[binn] = jbin;
          binn++;
        }
     }


  if(a_nbin!=NULL) *a_nbin = binn;
  if(a_is!=NULL)   *a_is   = is;
  else             CMemoryFreeBlock (is);
  if(a_js!=NULL)   *a_js   = js;
  else             CMemoryFreeBlock (js);

  CListDelete ((CList)list);
  return      (void*)hist;
}
/***************************************************************************/
void OPlotterHighlightBins (
 OPlotter This
,void* a_hist
,int  a_binn
,int* a_is
,int* a_js
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    dim;
  ONode* items;
  ONode* itema;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_hist==NULL) return;
  dim   = OHistogramGetDimension((OHistogram)a_hist);
  items = ONodeGetLeavesForward (This->gobins);
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      OHistogram hist;
      int        ibin,jbin,count,found;
      hist       = GetBinFromString (ONodeGetName(*itema),&ibin,&jbin);
      if(hist!=(OHistogram)a_hist) continue;
      found      = 0;
      for(count=0;count<a_binn;count++)
	{
	  if( ( (dim==1) && (a_is[count]==ibin) ) ||
	      ( (dim==2) && (a_is[count]==ibin) && (a_js[count]==jbin) ) 
	      )
	    {
	      found = 1;
	      break;
	    }
	}
      if(found==1) ONodeSetHighlight (*itema,1);
    }
  CListDelete ((void**)items);
}
/***************************************************************************/
static OHistogram GetBinFromString (
 char* a_string 
,int* a_ibin
,int* a_jbin
)
/***************************************************************************/
/* 3d object name is :
    plot:name:ibin[:jbin]
    plot:box,grid,axe_x,axe_y,axe_v,title                              */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char** words;
  int    nword;
  OHistogram  hist;
/*.........................................................................*/
  *a_ibin = NotFound;
  *a_jbin = NotFound;
  if(a_string==NULL) return NULL;  
  words      = CStringGetWords (a_string,":",&nword);
  if(nword>=3) 
    {
      int  status;
      hist = OHistogramGetIdentifier(words[1]);

      *a_ibin = (int)CStringConvertToLong(words[2],&status);
      if(status==0)  *a_ibin = NotFound;

      if(nword==4)
	{
	  *a_jbin = (int)CStringConvertToLong(words[3],&status);
	  if(status==0)  *a_jbin = NotFound;
	}

    }
  else
    {
      hist = NULL;
    }
  CTextDelete (nword,words);
  return   hist;
}
/***************************************************************************/
int OPlotterGetAttribute (
 OPlotter  This
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
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"name")==0) *((char**)a_addr)  = This->name;
  else 
    {
      CInfoF ("OPlotterGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OPlotterDoMethod (
 OPlotter This
,char*  a_name
,void*  a_user
,int    a_argn
,char** a_args
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)  return 0;

  if(This==NULL)
    {
      CInfoF("OPlotterDoMethod: %s is not a class routine property.\n",a_name);
      return 0;
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  if(strcmp(a_name,"getInformations")==0)
    {
      *((char**)a_addr) = OPlotterGetInformations (This);
      return            FREE_BLOCK;
    }
  else 
    {
      CInfoF ("OPlotterDoMethod: %s is not a routine property.\n",a_name);
      return 0;
    }
}
/***************************************************************************/
static int GetBinColor (
 OPainting a_aspect
,double a_value 
,double a_min 
,double a_max
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_aspect==OPaintingLUT)
    {
      return DBL_INT a_value;
    }
  else if(a_aspect==OPaintingGradient)
    { 
      double       grey;
      if (a_max<=a_min)           return OColorBrown;
      if(Class.grad_ocmap==NULL)  return OColorBrown;

      grey         = 1. - (a_max - a_value) / (a_max-a_min);

      return (int)(Class.grad_beg + (Class.grad_ncolor - 1) * grey);
    }
  else
    {
      return OContextGetColorIndex (OContextGetStaticInstance());
    }
}
/***************************************************************************/
static int GetValueIndex (
 double   a_value
,int      a_colorn
,double*  a_values
,int*     a_colors
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int icolor = 0;
/*.........................................................................*/
  if(a_colorn<=0) return 0;
  if(a_value<a_values[1])
    {
      icolor = a_colors[0];
    }
  else
    {
      int   count,found;
      found = 0;
      for(count=1;count<a_colorn-1;count++)
	{
	  if( (a_values[count]<=a_value) && (a_value<a_values[count+1]) )
	    {
	      icolor = a_colors[count];
	      found  = 1;
	      break;
	    }
	}
      if(found==0)
	{
	  icolor = a_colors[a_colorn-1];
	}
    }
  return icolor;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Xo","OPlotter",a_name,a_code);
}
