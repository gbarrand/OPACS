/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CString.h>
#include <CText.h>
#include <CList.h>
#include <CError.h>

#include <OColormapP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  FreeRecord       (OColormap);
static int   GetIdentifier    (OColormap,char*);
static void  AddName          (OColormap,va_list);
static void  SetNumber        (OColormap,int);
static void  ReportError      (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
typedef unsigned char Uchar;
typedef char* OString;

#define NotFound           (-1)

static struct 
{
  OColormap*  extent;
  char**  list;
} Class = {NULL,NULL};
/***************************************************************************/
void OColormapClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy     ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent    = NULL;
  CListDelete      ((CList)Class.list);
  Class.list      = NULL;
}
/***************************************************************************/
OColormap* OColormapGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
char** OColormapGetNames (
 int*  a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDelete   ((CList)Class.list);
  CListVisitE    ((CList)Class.extent,(CListVisitEntryV_Function)AddName,&(Class.list));
  if(a_number!=NULL) *a_number = CListGetSize((CList)Class.list);
  return       (char**)Class.list;
}
/***************************************************************************/
static void AddName (
 OColormap   This
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList*       lista;
/*.........................................................................*/
  lista         = va_arg(a_args,CList*);
  CListAddEntry  (lista,OColormapGetName(This));
}
/***************************************************************************/
OColormap OColormapGetIdentifier (
 char* a_name    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OColormap)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OColormap This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OColormapGetName(This),a_name);
}
/***************************************************************************/
OColormap OColormapGetIdentifierFromRGBs (
 int a_ncolor
,double* a_reds
,double* a_greens
,double* a_blues
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_reds==NULL) || (a_greens==NULL) || (a_blues==NULL) ) return NULL;
  if(Class.extent!=NULL)
    {
      OColormap* itema;
      for(itema=Class.extent;(itema!=NULL) && (*itema!=NULL);itema++)
	{ 
          OColormap cmap;
	  int   equal;
	  equal = 1;
          cmap  = *itema;
	  if(a_ncolor!=cmap->number)
	    {
	      equal = 0;
	    }
	  else
	    {
	       int count;
	      for(count=0;count<cmap->number;count++) 
		{
		  if(  (a_reds  [count] != cmap->reds  [count])  ||
		       (a_greens[count] != cmap->greens[count])  ||
		       (a_blues [count] != cmap->blues [count])
		     )
		    {
		      equal = 0;
		      break;
		    }
		}
	    }
	  if(equal==1)
	    {
	      return cmap;
	    }
       	}
    }
  return NULL;
}
/***************************************************************************/
OColormap OColormapGetIdentifierFromString (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OColormap ocmap;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return NULL;
  ocmap = OColormapGetIdentifier (a_string);  
  if(ocmap==NULL) CWarnF("Unknown OColormap %s.\n",a_string);
  return ocmap;
}
/******************************************************************************/
OColormap OColormapMake (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OColormap    This;
/*............................................................................*/
  This           = (OColormap) CMemoryAllocateBlock (sizeof(OColormapRecord));
  if(This==NULL) return NULL;
  CListAddEntry  ((CList*)&(Class.extent),This);
  This->name     = NULL;
  This->number   = 0;
  This->reds     = NULL;
  This->greens   = NULL;
  This->blues    = NULL;
  This->namen    = 0;
  This->names    = NULL;
  return         This;
}
/******************************************************************************/
OColormap OColormapCreate (
 char* a_name    
,int a_ncolor
,double* a_reds
,double* a_greens
,double* a_blues
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OColormap    This;
   int count;
  double*  reds;
  double*  greens;
  double*  blues;
/*............................................................................*/
  if((a_ncolor<=0) || (a_reds==NULL) || (a_greens==NULL) || (a_blues==NULL) ) return NULL;

  reds                 = (double*)CMemoryAllocateBlock(a_ncolor * sizeof(double));
  greens               = (double*)CMemoryAllocateBlock(a_ncolor * sizeof(double));
  blues                = (double*)CMemoryAllocateBlock(a_ncolor * sizeof(double));
  if( (reds==NULL) || (greens==NULL) || (blues==NULL) ) 
    {
      CMemoryFreeBlock (reds);
      CMemoryFreeBlock (greens);
      CMemoryFreeBlock (blues);
      return   NULL;
    }

  This                     = OColormapMake();
  if(This==NULL) return NULL;

  This->name               = CStringDuplicate(a_name);
  This->number             = a_ncolor;
  This->reds               = reds;
  This->greens             = greens;
  This->blues              = blues;
  for(count=0;count<This->number;count++)
    {
      This->reds   [count]  = a_reds   [count];
      This->greens [count]  = a_greens [count];
      This->blues  [count]  = a_blues  [count];
    }
 
  return      This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OColormapDelete (
 OColormap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  CListRemoveEntry  ((CList)Class.extent,This);
  FreeRecord       (This);
}
/***************************************************************************/
int OColormapIsValid (
 OColormap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
void OColormapSetName (
 OColormap This 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->name);
  This->name    = CStringDuplicate (a_name);
}
/***************************************************************************/
char* OColormapGetName (
 OColormap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return    This->name;
}
/***************************************************************************/
int OColormapGetSize (
 OColormap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->number;
}
/***************************************************************************/
void OColormapGetRGBs (
 OColormap This 
,int* a_ncolor
,double** a_reds
,double** a_greens
,double** a_blues
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_ncolor!=NULL) *a_ncolor = 0;
  if(a_reds!=NULL)   *a_reds   = NULL;
  if(a_greens!=NULL) *a_greens = NULL;
  if(a_blues!=NULL)  *a_blues = NULL;
  if(This==NULL)    return;
  if(a_ncolor!=NULL) *a_ncolor = This->number;
  if(a_reds!=NULL)   *a_reds   = This->reds;
  if(a_greens!=NULL) *a_greens = This->greens;
  if(a_blues!=NULL)  *a_blues  = This->blues;
}
/***************************************************************************/
void OColormapDump (
 OColormap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  CDumpF ("name : %s\n",This->name!=NULL ? This->name : "(nil)");
  for(count=0;count<This->number;count++)
    {  
       CDumpF ("red : %15g, green : %15g, blue : %15g\n",
	       This->reds[count],
	       This->greens[count],
	       This->blues[count]);
    }
}
/***************************************************************************/
void OColormapSetIthRGB (
 OColormap This 
,int a_index 
,double a_red
,double a_green
,double a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                                return;
  if((a_index<0)||(a_index>=This->number)) return;
  This->reds   [a_index] = a_red;
  This->greens [a_index] = a_green;
  This->blues  [a_index] = a_blue;
}
/***************************************************************************/
void OColormapGetIthRGB (
 OColormap This 
,int a_index 
,double* a_red
,double* a_green
,double* a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_red!=NULL)   *a_red   = 0.;
  if(a_green!=NULL) *a_green = 0.;
  if(a_blue!=NULL)  *a_blue  = 0.;
  if(This==NULL) return;
  if((a_index<0)||(a_index>=This->number)) return;
  if(a_red!=NULL)   *a_red   = This->reds  [a_index];
  if(a_green!=NULL) *a_green = This->greens[a_index];
  if(a_blue!=NULL)  *a_blue  = This->blues [a_index];
}
/***************************************************************************/
int OColormapGetRGB_Index (
 OColormap This
,double a_red
,double a_green
,double a_blue
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return NotFound;
  for(count=0;count<This->number;count++) 
    {
      if((a_red   == This->reds  [count])  &&
	 (a_green == This->greens[count])  &&
	 (a_blue  == This->blues [count])
	 )
	{
	  return count;
	}
    }
  return NotFound;
}
/***************************************************************************/
void OColormapSetColorNames (
 OColormap This
,int    a_namen
,char** a_names
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CTextDelete (This->namen,This->names);
  This->names = CTextDuplicate (a_namen,a_names,&(This->namen));
}
/***************************************************************************/
int OColormapGetColorNameIndex (
 OColormap This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int value;
/*.........................................................................*/
  if(This==NULL) return NotFound;
  value = CTextGetLineIndex (This->namen,This->names,a_string);
  if(value==NotFound)  
    { /*Look if a_string is a number.*/
      int   status;
      value = (int)CStringConvertToLong (a_string,&status);
      if(status==0) 
	{
	  value  = NotFound;
	}
    }
  return value;
}
/***************************************************************************/
char* OColormapGetIthColorName (
 OColormap This
,int a_index
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(This==NULL) return NULL;
  string = CTextGetIthLine (This->namen,This->names,a_index);
  if(string==NULL)
    {
      static  char s[32];
      sprintf (s,"%d",a_index);
      return  s;
    }
  else
    {
      return string;
    }				     
}
/***************************************************************************/
int OColormapGetValueColorMapping (
 OColormap This
,char*    a_string
,double** a_values
,int**    a_colors
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char**  words;
  int     wordn;
  int     number,count;
  double* values = NULL;
  int*    colors = NULL;
/*.........................................................................*/
  if(a_values!=NULL) *a_values = NULL;
  if(a_colors!=NULL) *a_colors = NULL;
  if(This==NULL)     return 0;
  if(a_string==NULL) return 0;  
  words      = CStringGetWords (a_string," ",&wordn);
  number     = wordn/2;
  if(2*number+1!=wordn)
    {
      CWarnF ("An odd number (%d given) of words is expected in :\n%s\n",
	      wordn,a_string);
      CTextDelete (wordn,words);
      return 0;
    }
  if(number!=0)
    {
      values = (double*)CMemoryAllocateBlock ((number+1) * sizeof(double));
      colors = (int*)   CMemoryAllocateBlock ((number+1) * sizeof(int));
      if( (values==NULL) || (colors==NULL) )
	{
          CMemoryFreeBlock (values);
	  CMemoryFreeBlock (colors);
	  CTextDelete      (wordn,words);
	  return           0;
	}
      values[0] = 0.;
      colors[0] = OColormapGetColorNameIndex (This,words[0]);
      for(count=0;count<number;count++)
        {
	  int           status;
	  values[1+count] = CStringConvertToDouble (words[1+2*count],&status);
	  if(status==0)
	    {
	      CWarnF           ("Can't convert %s to double in :\n%s\n",words[1+2*count],a_string);
	      CMemoryFreeBlock (values);
	      CMemoryFreeBlock (colors);
	      CTextDelete      (wordn,words);
	      return           0;
	    }
	  colors[1+count] = OColormapGetColorNameIndex (This,words[1+2*count+1]);
	}
    }
  CTextDelete (wordn,words);
  if(a_values!=NULL) *a_values = values;
  if(a_colors!=NULL) *a_colors = colors;
  return number+1;
}
/***************************************************************************/
int OColormapGetAttribute (
 OColormap  This
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

       if(strcmp(a_name,"identifier")==0) *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"name")==0)       *((char**)a_addr)  = This->name;
  else if(strcmp(a_name,"size")==0)       *((int*)a_addr)    = This->number;
  else if(strcmp(a_name,"rgbs")==0) 
    {
      int       number  = 0;
      double*   doubles = NULL;
      if(This->number!=0)
        {
	  doubles = (double*)CMemoryAllocateBlock( (This->number * 3) * sizeof(double));
	  number        = 0;
	  if(doubles!=NULL)
            {
	        int count;
	      for(count=0;count<This->number;count++)
		{
		  doubles[number] = This->reds  [count];number++;
		  doubles[number] = This->greens[count];number++;
		  doubles[number] = This->blues [count];number++;
		}
	    }
	}
      if(a_number!=NULL) *a_number = number;
      *((double**)a_addr) = doubles;
      return FREE_BLOCK;
    }
  else if(strcmp(a_name,"names")==0)
    {
      if(a_number!=NULL) *a_number = This->namen;
      *((OString**)a_addr) = This->names;
      return 1;
    }
  else 
    {
      CInfoF("OColormapGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OColormapSetAttribute (
 OColormap  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0)  {}
  else if(strcmp(a_name,"name")==0)        OColormapSetName      (This,*((char**)a_addr));
  else if(strcmp(a_name,"size")==0)        {}
  else if(strcmp(a_name,"rgbs")==0) 
    {
      int        rgbn;
         int count;
      double*    list;
      rgbn       = a_number/3;
      SetNumber (This,rgbn);
      list       = *((double**)a_addr);
      for(count=0;count<rgbn;count++) 
	{
	  This->reds   [count] = *list;list++;
	  This->greens [count] = *list;list++;
	  This->blues  [count] = *list;list++;
	}
    }
  else if(strcmp(a_name,"names")==0) 
    {
      OString*    list;
      list        = *((OString**)a_addr);
      OColormapSetColorNames (This,a_number,list);
    }
  else 
    {
      CInfoF ("OColormapSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void FreeRecord (
 OColormap This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock (This->name);
  CMemoryFreeBlock (This->reds);
  CMemoryFreeBlock (This->greens);
  CMemoryFreeBlock (This->blues);
  CTextDelete      (This->namen,This->names);
  This->name       = NULL;
  This->reds       = NULL;
  This->greens     = NULL;
  This->blues      = NULL;
  This->number     = 0;
  This->namen      = 0;
  This->names      = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
static void SetNumber (
 OColormap  This
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  CMemoryFreeBlock      (This->reds);
  CMemoryFreeBlock      (This->greens);
  CMemoryFreeBlock      (This->blues);
  This->reds    = NULL;
  This->greens  = NULL;
  This->blues   = NULL;
  This->number  = 0;
  if(a_number<=0) return;
  This->reds    = (double*)CMemoryAllocateBlock(a_number * sizeof(double));
  This->greens  = (double*)CMemoryAllocateBlock(a_number * sizeof(double));
  This->blues   = (double*)CMemoryAllocateBlock(a_number * sizeof(double));
  This->number  = a_number;
  if( (This->reds==NULL) || (This->greens==NULL) || (This->blues==NULL) ) 
    {
      CMemoryFreeBlock     (This->reds);
      CMemoryFreeBlock     (This->greens);
      CMemoryFreeBlock     (This->blues);
      This->reds    = NULL;
      This->greens  = NULL;
      This->blues   = NULL;
      This->number  = 0;
    }
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
  CErrorHandle ("Go","OColormap",a_name,a_code);
}


