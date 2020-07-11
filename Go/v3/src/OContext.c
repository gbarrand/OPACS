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
#include <stdio.h>
 
#include <CMemory.h>
#include <CPrinter.h>
#include <CError.h>
#include <CString.h>
#include <CText.h>

#include <OContextP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void SetDefaults ();
static void ReportError (char*,int);
#ifdef __cplusplus
}
#endif

#define ONumber(arr)  (sizeof(arr) / sizeof(arr[0]))
#define NotFound (-1)

static struct 
{
  OContextRecord  contextRecord;
} Class = {{0,1,1,OLineStyleSolid,OMarkerStyleDot,OAreaStyleSolid,OModelingWireFrame,OPaintingColor,OFontLatin}};
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
OContext OContextGetStaticInstance (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return &(Class.contextRecord);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
OContext OContextMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OContext               This;
/*.........................................................................*/
  This               = (OContext)CMemoryAllocateBlock (sizeof(OContextRecord));
  if(This==NULL)          return NULL;
  This->color        = 0;
  This->lineWidth    = 1;
  This->markerSize   = 1;
  This->lineStyle    = OLineStyleSolid;
  This->markStyle    = OMarkerStyleDot;
  This->areaStyle    = OAreaStyleSolid;
  This->modeling     = OModelingWireFrame;
  This->painting     = OPaintingColor;
  This->font         = OFontLatin;
  return             This;
}
/***************************************************************************/
void OContextInitialize (
  OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  This->color        = 0;
  This->lineWidth    = 1;
  This->markerSize   = 1;
  This->lineStyle    = OLineStyleSolid;
  This->markStyle    = OMarkerStyleDot;
  This->areaStyle    = OAreaStyleSolid;
  This->modeling     = OModelingWireFrame;
  This->painting     = OPaintingColor;
  This->font         = OFontLatin;
}
/***************************************************************************/
void OContextCopy (
 OContext This
,OContext a_from
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_from==NULL)   return;
  This->color        = a_from->color;
  This->lineWidth    = a_from->lineWidth;
  This->markerSize   = a_from->markerSize;
  This->lineStyle    = a_from->lineStyle;
  This->markStyle    = a_from->markStyle;
  This->areaStyle    = a_from->areaStyle;
  This->modeling     = a_from->modeling;
  This->painting     = a_from->painting;
  This->font         = a_from->font;
}
/***************************************************************************/
void OContextDelete (
 OContext This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock  (This);
}
/***************************************************************************/
OContext OContextDuplicate (
 OContext This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OContext      item;
/*.........................................................................*/
  if(This==NULL) return NULL;
  item      = OContextMake();
  OContextCopy  (item,This);
  return    item;
}
/***************************************************************************/
void OContextSetColorIndex (
 OContext This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->color = a_value;
}
/***************************************************************************/
int OContextGetColorIndex (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return    This->color;
}
/***************************************************************************/
void OContextSetLineWidth (
 OContext This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->lineWidth = a_value;
}
/***************************************************************************/
int OContextGetLineWidth (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return    This->lineWidth;
}
/***************************************************************************/
void OContextSetMarkerSize (
 OContext This
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->markerSize = a_value;
}
/***************************************************************************/
int OContextGetMarkerSize (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return    This->markerSize;
}
/***************************************************************************/
void OContextSetLineStyle (
 OContext This
,OLineStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->lineStyle = a_value;
}
/***************************************************************************/
OLineStyle OContextGetLineStyle (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OLineStyleSolid;
  return    This->lineStyle;
}
/***************************************************************************/
void OContextSetMarkerStyle (
 OContext This
,OMarkerStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->markStyle = a_value;
}
/***************************************************************************/
OMarkerStyle OContextGetMarkerStyle (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OMarkerStyleDot;
  return    This->markStyle;
}
/***************************************************************************/
void OContextSetAreaStyle (
 OContext This
,OAreaStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->areaStyle = a_value;
}
/***************************************************************************/
OAreaStyle OContextGetAreaStyle (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OAreaStyleSolid;
  return    This->areaStyle;
}
/***************************************************************************/
void OContextSetModeling (
 OContext This
,OModeling a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->modeling = a_value;
}
/***************************************************************************/
OModeling OContextGetModeling (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OModelingWireFrame;
  return    This->modeling;
}
/***************************************************************************/
void OContextSetPainting (
 OContext This
,OPainting a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->painting = a_value;
}
/***************************************************************************/
OPainting OContextGetPainting (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OPaintingColor;
  return    This->painting;
}
/***************************************************************************/
void OContextSetFont (
 OContext This
,OFont a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->font = a_value;
}
/***************************************************************************/
OFont OContextGetFont (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OFontLatin;
  return    This->font;
}
/***************************************************************************/
void OContextSetF (
 OContext This
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list       args;
/*.........................................................................*/
  va_start      (args,a_format);
  OContextSetV  (This,a_format,args);
  va_end        (args);
}
/***************************************************************************/
void OContextGetF (
 OContext This
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list       args;
/*.........................................................................*/
  va_start      (args,a_format);
  OContextGetV  (This,a_format,args);
  va_end        (args);
}
/***************************************************************************/
void OContextRestoreDefaultValues (
 OContext This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  This->color        = 0;
  This->lineWidth    = 1;
  This->markerSize   = 1;
  This->lineStyle    = OLineStyleSolid;
  This->markStyle    = OMarkerStyleDot;
  This->areaStyle    = OAreaStyleSolid;
  This->modeling     = OModelingWireFrame;
  This->painting     = OPaintingColor;
  This->font         = OFontLatin;
}
/***************************************************************************/
void OContextSetV (
 OContext This
,char* a_format 
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*    chara;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_format==NULL) return;
  for(chara=a_format;*chara!='\0';chara++)
    { 
      switch(*chara)
        {
  	  case 'c': This->color        = va_arg(a_args,int);           break;
  	  case 'w': This->lineWidth    = va_arg(a_args,int);           break;
  	  case 's': This->markerSize   = va_arg(a_args,int);           break;
  	  case 'l': This->lineStyle    = va_arg(a_args,OLineStyle);    break;
  	  case 'm': This->markStyle    = va_arg(a_args,OMarkerStyle);    break;
  	  case 'a': This->areaStyle    = va_arg(a_args,OAreaStyle);    break;
  	  case 'o': This->modeling     = va_arg(a_args,OModeling); break;
  	  case 'p': This->painting     = va_arg(a_args,OPainting); break;
  	  case 'f': This->font         = va_arg(a_args,OFont); break;
  	  case 'r': /*reset*/
                    This->color        = 0;
                    This->lineWidth    = 1;
                    This->markerSize   = 1;
                    This->lineStyle    = OLineStyleSolid;
                    This->markStyle    = OMarkerStyleDot;
                    This->areaStyle    = OAreaStyleSolid;
	            This->modeling     = OModelingWireFrame;
	            This->painting     = OPaintingColor;
	            This->font         = OFontLatin;
                    break;
          default:
	            ReportError("OContextSetV",CErrorCase);
	            return;
        }
    }
}
/***************************************************************************/
void OContextGetV (
 OContext This
,char* a_format 
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*    chara;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_format==NULL) return;
  for(chara=a_format;*chara!='\0';chara++)
    { 
      switch(*chara)
        {
  	  case 'c': *(va_arg(a_args,int        *))   = This->color;       break;
  	  case 'l': *(va_arg(a_args,OLineStyle *))   = This->lineStyle;   break;
  	  case 'm': *(va_arg(a_args,OMarkerStyle *)) = This->markStyle;   break;
  	  case 'a': *(va_arg(a_args,OAreaStyle *))   = This->areaStyle;   break;
  	  case 'w': *(va_arg(a_args,int *))          = This->lineWidth;   break;
  	  case 's': *(va_arg(a_args,int *))          = This->markerSize;  break;
  	  case 'o': *(va_arg(a_args,OModeling*))     = This->modeling;break;
  	  case 'p': *(va_arg(a_args,OPainting*))     = This->painting;break;
  	  case 'f': *(va_arg(a_args,OFont*))         = This->font;break;
          default:
                    ReportError ("OContextGetV",CErrorCase);
                    return;
        }
    }
}
/***************************************************************************/
void OContextSetFromScript (
 OContext  This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char**   words;
  int      wordn;
/*.........................................................................*/
  if(This==NULL) return;
  words    = CStringGetWords (a_string," ",&wordn);
  for(count=0;count<wordn;count++) 
    {
      if(words[count]!=NULL)
	{
	       if( (strcmp(words[count],"color")==0) && (count+1<wordn))                
	    {
	      OContextSetColorIndex        (This,OColorGetIdentifier(words[count+1]));
	      count++;
	    }
	  else if( (strcmp(words[count],"lineStyle")==0) && (count+1<wordn))                
	    {
	      OContextSetLineStyle    (This,OLineStyleGetIdentifier(words[count+1]));
	      count++;
	    }
	  else if( (strcmp(words[count],"markerStyle")==0) && (count+1<wordn))                
	    {
	      OContextSetMarkerStyle    (This,OMarkerStyleGetIdentifier(words[count+1]));
	      count++;
	    }
	  else if( (strcmp(words[count],"areaStyle")==0) && (count+1<wordn))                
	    {
	      OContextSetAreaStyle    (This,OAreaStyleGetIdentifier(words[count+1]));
	      count++;
	    }
	  else if( (strcmp(words[count],"modeling")==0) && (count+1<wordn))                
	    {
	      OContextSetModeling (This,OModelingGetIdentifier(words[count+1]));
	      count++;
	    }
	  else if( (strcmp(words[count],"painting")==0) && (count+1<wordn))                
	    {
	      OContextSetPainting (This,OPaintingGetIdentifier(words[count+1]));
	      count++;
	    }
	  else if( (strcmp(words[count],"font")==0) && (count+1<wordn))                
	    {
	      OContextSetFont (This,OFontGetIdentifier(words[count+1]));
	      count++;
	    }
	  else if( (strcmp(words[count],"lineWidth")==0) && (count+1<wordn))                
	    {
              int          value;
              int          status;
	      value        = (int)CStringConvertToLong(words[count+1],&status);
	      if(status==0)  {CWarnF ("%s: %s not a number.\n",a_string,words[count+1]);break;}
	      OContextSetLineWidth (This,value);
	      count++;
	    }
	  else if( (strcmp(words[count],"markerSize")==0) && (count+1<wordn))                
	    {
              int          value;
              int          status;
	      value        = (int)CStringConvertToLong(words[count+1],&status);
	      if(status==0)  {CWarnF ("%s: %s not a number.\n",a_string,words[count+1]);break;}
	      OContextSetMarkerSize (This,value);
	      count++;
	    }
	  else if(strcmp(words[count],"setDefaults")==0)
	    {
	      OContextRestoreDefaultValues           (This);
	    }
	  else
	    {
	      CWarnF  ("OContextSetFromScript: bad option %s.\n",words[count]);
	    }
	}
   
    } 
  CTextDelete (wordn,words);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static struct {
  char*      string;
  OLineStyle value;
} OLineStyleList [] = {
  {"solid",       OLineStyleSolid},
  {"dashed",      OLineStyleDashed},
  {"dotted",      OLineStyleDotted},
  {"dash_dotted", OLineStyleDashDotted}
};  
static struct {
  char*      string;
  OMarkerStyle value;
} OMarkerStyleList [] = {
  {"dot",         OMarkerStyleDot},
  {"plus",        OMarkerStylePlus},
  {"asterisk",    OMarkerStyleAsterisk},
  {"circle",      OMarkerStyleCircle},
  {"cross",       OMarkerStyleCross},
  {"star",        OMarkerStyleStar}
};  
static struct {
  char*      string;
  OAreaStyle value;
} OAreaStyleList [] = {
  {"solid",         OAreaStyleSolid},
  {"hatched",       OAreaStyleHatched},
  {"checker",       OAreaStyleChecker}
};  
/***************************************************************************/
char** OLineStyleGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* lineStyle[4] = {"solid","dashed","dotted","dash_dotted"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 4;
  return lineStyle;
}
/***************************************************************************/
OLineStyle OLineStyleGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0')) return OLineStyleSolid; 
  for(count=0;count<ONumber(OLineStyleList);count++)
    {
      if(strcmp(a_string,OLineStyleList[count].string)==0) 
	return OLineStyleList[count].value;
    }
  CWarnF  ("%s not a OLineStyle.\n",a_string);
  return  OLineStyleSolid;
}
/***************************************************************************/
char* OLineStyleGetName (
 OLineStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OLineStyleList);count++)
    if(a_value==OLineStyleList[count].value) 
      return OLineStyleList[count].string;
  CWarnF("%d not a OLineStyle.\n",a_value);
  return NULL;
}
/***************************************************************************/
char** OMarkerStyleGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* markStyle[6] = {"dot","plus","asterisk","circle","cross","star"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 6;
  return markStyle;
}
/***************************************************************************/
OMarkerStyle OMarkerStyleGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0'))  return OMarkerStyleDot;
  for(count=0;count<ONumber(OMarkerStyleList);count++)
    {
      if(strcmp(a_string,OMarkerStyleList[count].string)==0) 
	return OMarkerStyleList[count].value;
    }
  CWarnF ("%s not a OMarkerStyle.\n",a_string);
  return OMarkerStyleDot;
}
/***************************************************************************/
char* OMarkerStyleGetName (
 OMarkerStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OMarkerStyleList);count++)
    if(a_value==OMarkerStyleList[count].value) return OMarkerStyleList[count].string;
  CWarnF("%d not a OMarkerStyle.\n",a_value);
  return NULL;
}
/***************************************************************************/
char** OAreaStyleGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* areaStyle[3] = {"solid","hatched","checker"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 3;
  return areaStyle;
}
/***************************************************************************/
OAreaStyle OAreaStyleGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0'))  return OAreaStyleSolid; 
  for(count=0;count<ONumber(OAreaStyleList);count++)
    {
      if(strcmp(a_string,OAreaStyleList[count].string)==0) 
	return OAreaStyleList[count].value;
    }
  CWarnF ("%s not a OAreaStyle.\n",a_string);
  return OAreaStyleSolid;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static struct {
  char*         string;
  OModeling value;
} OModelingTypeList [] = {
  {"wire_frame",    OModelingWireFrame},
  {"solid",         OModelingSolid},
  {"cloud",         OModelingCloud},
  {"segments",      OModelingSegments}
};  
static struct {
  char*       string;
  OPainting value;
} OPaintingTypeList [] = {
  {"color"   ,    OPaintingColor},
  {"shadow"  ,    OPaintingShadow},
  {"lut"     ,    OPaintingLUT},
  {"gradient",    OPaintingGradient}
};  
static struct {
  char*       string;
  OFont       value;
} OFontTypeList [] = {
  {"latin"   ,    OFontLatin},
  {"greek"   ,    OFontGreek}
};  
/***************************************************************************/
char* OAreaStyleGetName (
 OAreaStyle a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OAreaStyleList);count++)
    if(a_value==OAreaStyleList[count].value) return OAreaStyleList[count].string;
  CWarnF("%d not a OAreaStyle.\n",a_value);
  return NULL;
}
/***************************************************************************/
char** OModelingGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* modelingType[4] = {"wire_frame","solid","cloud","segments"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 4;
  return modelingType;
}
/***************************************************************************/
OModeling OModelingGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0'))  return OModelingWireFrame;
  for(count=0;count<ONumber(OModelingTypeList);count++)
    {
      if(strcmp(a_string,OModelingTypeList[count].string)==0) 
	return OModelingTypeList[count].value;
    }
  CWarnF  ("%s not a OModeling.\n",a_string);
  return  OModelingWireFrame;
}
/***************************************************************************/
char* OModelingGetName (
 OModeling a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OModelingTypeList);count++)
    if(a_value==OModelingTypeList[count].value) return OModelingTypeList[count].string;
  CWarnF("%d not a OModeling.\n",a_value);
  return NULL;
}
/***************************************************************************/
char** OPaintingGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* binAspect[4] = {"color","shadow","lut","gradient"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 4;
  return binAspect;
}
/***************************************************************************/
OPainting OPaintingGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OPaintingTypeList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OPaintingTypeList[count].string)==0) ) 
      return OPaintingTypeList[count].value;
  CWarnF("%s not an OPainting.\n",a_string);
  return OPaintingShadow;
}
/***************************************************************************/
char* OPaintingGetName (
 OPainting a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OPaintingTypeList);count++)
    if(a_value==OPaintingTypeList[count].value) return OPaintingTypeList[count].string;
  CWarnF("%d not an OPainting.\n",a_value);
  return NULL;
}
/***************************************************************************/
char** OFontGetNames (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* font[2] = {"latin","greek"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 2;
  return font;
}
/***************************************************************************/
OFont OFontGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OFontTypeList);count++)
    if( (a_string!=NULL) && (strcmp(a_string,OFontTypeList[count].string)==0) ) 
      return OFontTypeList[count].value;
  CWarnF("%s not an OFont.\n",a_string);
  return OFontLatin;
}
/***************************************************************************/
char* OFontGetName (
 OFont a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(OFontTypeList);count++)
    if(a_value==OFontTypeList[count].value) return OFontTypeList[count].string;
  CWarnF("%d not an OFont.\n",a_value);
  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OContextSetAttribute (
 OContext  This
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

       if(strcmp(a_name,"color")==0)       Class.contextRecord.color      = OColorGetIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"lineWidth")==0)   Class.contextRecord.lineWidth  = *((int*)a_addr);
  else if(strcmp(a_name,"markerSize")==0)  Class.contextRecord.markerSize = *((int*)a_addr);
  else if(strcmp(a_name,"lineStyle")==0)   Class.contextRecord.lineStyle  = OLineStyleGetIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"markerStyle")==0) Class.contextRecord.markStyle  = OMarkerStyleGetIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"areaStyle")==0)   Class.contextRecord.areaStyle  = OAreaStyleGetIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"modeling")==0)    Class.contextRecord.modeling   = OModelingGetIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"painting")==0)    Class.contextRecord.painting   = OPaintingGetIdentifier(*((char**)a_addr));
  else if(strcmp(a_name,"font")==0)        Class.contextRecord.font       = OFontGetIdentifier(*((char**)a_addr));
  else 
    {
      CInfoF ("OContextSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  This     = NULL;
  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int OContextGetAttribute (
 OContext  This
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

  else if(strcmp(a_name,"color")==0) 
    {
      static char name[32]; 
      sprintf (name,"%d",OContextGetColorIndex(&Class.contextRecord));
      *((char**)a_addr) = name;
    }
  else if(strcmp(a_name,"lineWidth")==0)   *((int*)a_addr)    = OContextGetLineWidth                       (&Class.contextRecord);
  else if(strcmp(a_name,"markerSize")==0)  *((int*)a_addr)    = OContextGetMarkerSize                      (&Class.contextRecord);
  else if(strcmp(a_name,"lineStyle")==0)   *((char**)a_addr)  = OLineStyleGetName(OContextGetLineStyle     (&Class.contextRecord));
  else if(strcmp(a_name,"markerStyle")==0) *((char**)a_addr)  = OMarkerStyleGetName(OContextGetMarkerStyle (&Class.contextRecord));
  else if(strcmp(a_name,"areaStyle")==0)   *((char**)a_addr)  = OAreaStyleGetName(OContextGetAreaStyle     (&Class.contextRecord));
  else if(strcmp(a_name,"modeling")==0)    *((char**)a_addr)  = OModelingGetName(OContextGetModeling       (&Class.contextRecord));
  else if(strcmp(a_name,"painting")==0)    *((char**)a_addr)  = OPaintingGetName(OContextGetPainting       (&Class.contextRecord));
  else if(strcmp(a_name,"font")==0)        *((char**)a_addr)  = OFontGetName(OContextGetFont               (&Class.contextRecord));
  else 
    {
      CInfoF("OContextGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  This   = NULL;
  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OContextDoMethod (
 OContext  This
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

  if(strcmp(a_name,"setDefaults")==0)
    {
      SetDefaults();
    }
  else 
    {
      CInfoF ("OContextDoMethod: %s not a class routine property.\n",a_name);
      return 0;
    }

  This     = NULL;
  a_user   = NULL;
  a_argn   = 0;
  a_args   = NULL;
  a_addr   = NULL;
  return   1;
}
/***************************************************************************/
static void SetDefaults (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OContextRestoreDefaultValues (OContextGetStaticInstance());
}
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
  CErrorHandle ("Go","OContext",a_name,a_code);
}
