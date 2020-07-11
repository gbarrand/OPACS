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
#include <stdlib.h>
#include <string.h>

#include <CMemory.h>
#include <CString.h>
#include <CText.h>
#include <CFile.h>
#include <CError.h>
#include <CList.h>
#include <CPrinter.h>

#include <OStatement.h>

typedef enum {
  OCutEq,
  OCutNe,
  OCutLt,
  OCutLe,
  OCutGt,
  OCutGe
} OCutType;

#include <OCutP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void      FreeRecord        (OCut);
static int       GetIdentifier     (OCut,char*);
static int       CompareValue      (OCutType,double,double);
static OCutType  ParseScript       (char*,char**,double*);
static int       GetLineIdentifier (int,char**,char*);
static void      TrimString        (char*);
static void      ReportError       (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
#define NotFound (-1)

static struct 
{
  OCut*        extent;
  int          coln;
  double*      row;
} Class = {NULL,0,NULL};
/***************************************************************************/
void OCutClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OStatementClearClass ();
  CListDestroy     ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent    = NULL;
}
/***************************************************************************/
OCut* OCutGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OCut OCutGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"OCut",4)==0) )
    {
      int  status;
      OCut identifier;
      identifier = (OCut)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return (OCut)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,pos+1);
	}
      else
	{
	  if(OCutIsValid(identifier)==0)
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
      return (OCut)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
    }
}
/***************************************************************************/
static int GetIdentifier (
 OCut  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OCutGetName(This),a_name);
}
/***************************************************************************/
OCut OCutMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OCut This;
/*.........................................................................*/
  This           = (OCut)CMemoryAllocateBlock (sizeof(OCutRecord));
  if(This==NULL) return NULL;
  CListAddEntry  ((CList*)&(Class.extent),This);
  This->name    = NULL;
  This->script  = NULL;
  This->ostmt   = NULL;
  This->cutn    = 0;
  for(count=0;count<MAX_CUTS;count++)
    {
      This->cuts[count].col_name   = NULL;
      This->cuts[count].column     = NotFound;
      This->cuts[count].type       = OCutGe;
      This->cuts[count].value      = 0.;
    }
  return  This;
}
/***************************************************************************/
OCut OCutCreate (
 char*      a_name    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCut This;
/*.........................................................................*/
  This           = OCutMake();
  if(This==NULL) return NULL;
  This->name     = CStringDuplicate(a_name);
  return        This;
}
/***************************************************************************/
int OCutIsValid (
 OCut This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
void OCutDelete (
  OCut This 
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
static void FreeRecord (
 OCut This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  CStringDelete     (This->name);
  OStatementDelete  (This->ostmt);
  CStringDelete     (This->script);
  for(count=0;count<MAX_CUTS;count++) 
    {
      CStringDelete              (This->cuts[count].col_name);
      This->cuts[count].col_name = NULL;
    }
  This->name       = NULL;
  This->ostmt      = NULL;
  This->script     = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void OCutSetName (
 OCut This 
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
char* OCutGetName (
 OCut This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OCutSetScript (
 OCut       This    
,char*      a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)     return;
  CStringDelete      (This->script);
  This->script       = CStringDuplicate (a_string);
  This->cutn         = 0;
  OStatementDelete   (This->ostmt);
  This->ostmt        = NULL;
  if(This->script==NULL)    return;
  if(*(This->script)=='\0') return;

  if (OStatementGetSymbolIdentifier(This->script)!=NULL) /*Interpreted function.*/
    { 
      char*         string;
      string        = CStringCreateF (strlen(This->script)+2,"%s()",This->script); 
      This->ostmt   = OStatementCreate(string,0,NULL);
      CStringDelete (string);
    }
  else
    { char** words;
      int    nword;
      words = CStringGetWords (This->script,";",&nword);
      if(nword>MAX_CUTS)
        {
          ReportError ("OCutSetScript",CErrorFlow);
          This->cutn = MAX_CUTS;
        }
      else
	{
	  This->cutn = nword;
	}
      for(count=0;count<This->cutn;count++)
        {
          char*                      scol;
          This->cuts[count].type     = ParseScript (words[count],&scol,&(This->cuts[count].value));
          CStringDelete              (This->cuts[count].col_name);
          This->cuts[count].col_name = CStringDuplicate (scol);
          CMemoryFreeBlock           (scol);
        }
      CTextDelete (nword,words);
   }
}
/***************************************************************************/
char* OCutGetScript (
 OCut   This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->script;
}
/***************************************************************************/
void OCutSetColumns (
 OCut This 
,int a_number
,char** a_list
)
/***************************************************************************/
/* a_txt contains labels of tuple columns                                  */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return; 
  for(count=0;count<This->cutn;count++)
    { 
      This->cuts[count].column = NotFound;
      if(  (This->cuts[count].col_name!=NULL) && 
           (sscanf(This->cuts[count].col_name,"%d",&(This->cuts[count].column))!=1)
        )
        { 
          This->cuts[count].column = GetLineIdentifier (a_number,a_list,This->cuts[count].col_name);
        }
    }
}
/***************************************************************************/
double* OCutGetCurrentRow (
 int* a_cols 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_cols!=NULL) *a_cols = Class.coln;
  return  Class.row;
}
/***************************************************************************/
int OCutIsRowAccepted (
 OCut This 
,int a_cols 
,double* a_row 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return 1; 
  if(This->ostmt!=NULL) 
    {
      Class.coln = a_cols;
      Class.row  = a_row;
      return     (int)OStatementEvaluate(This->ostmt,0,NULL);
    }
  else
    {
      for(count=0;count<This->cutn;count++)
        {
          if((This->cuts[count].column>=0)&&(This->cuts[count].column<a_cols))
            { 
              if(CompareValue(This->cuts[count].type,This->cuts[count].value,
			      a_row[This->cuts[count].column])==0) return 0;
            }
        }
    }
  return     1;
}
/***************************************************************************/
static int CompareValue (
 OCutType a_type 
,double a_valcut  
,double a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  switch(a_type)
    {
      case OCutEq:return (a_value==a_valcut ? 1:0);
      case OCutNe:return (a_value!=a_valcut ? 1:0);
      case OCutLt:return (a_value< a_valcut ? 1:0);
      case OCutLe:return (a_value<=a_valcut ? 1:0);
      case OCutGt:return (a_value> a_valcut ? 1:0);
      case OCutGe:return (a_value>=a_valcut ? 1:0);
    }
  return 0;
}
/***************************************************************************/
static OCutType ParseScript (
 char* a_string 
,char** a_column 
,double* a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*      string;
  char*      pos;
  OCutType   cut;
  int        l,status;
  double     value;
/*.........................................................................*/
  if(a_column!=NULL) *a_column = NULL;
  if(a_value!=NULL)  *a_value  = 0.;
  if( (a_string==NULL) || (*a_string=='\0') ) return OCutLe;
  string    = CStringDuplicate(a_string);
  pos       = NULL;
  l         = 0;
       if( (pos = strstr (string,"=="))!=NULL) {cut = OCutEq;l=2;}
  else if( (pos = strstr (string,"eq"))!=NULL) {cut = OCutEq;l=2;}
  else if( (pos = strstr (string,"!="))!=NULL) {cut = OCutNe;l=2;}
  else if( (pos = strstr (string,"ne"))!=NULL) {cut = OCutNe;l=2;}
  else if( (pos = strstr (string,"<="))!=NULL) {cut = OCutLe;l=2;}
  else if( (pos = strstr (string,"le"))!=NULL) {cut = OCutLe;l=2;}
  else if( (pos = strstr (string,"<" ))!=NULL) {cut = OCutLt;l=1;}
  else if( (pos = strstr (string,"lt"))!=NULL) {cut = OCutLt;l=2;}
  else if( (pos = strstr (string,">="))!=NULL) {cut = OCutGe;l=2;}
  else if( (pos = strstr (string,"ge"))!=NULL) {cut = OCutGe;l=2;}
  else if( (pos = strstr (string,">" ))!=NULL) {cut = OCutGt;l=1;}
  else if( (pos = strstr (string,"gt"))!=NULL) {cut = OCutGt;l=2;}

  if(pos==NULL) 
    {
      CWarnF   ("Bad syntax in:\n%s\n\n",a_string);
      CMemoryFreeBlock (string);
      return   OCutLe;
    }

  value    = CStringConvertToDouble(pos+l,&status);
  if(status==0) 
    {
      CWarnF ("In %s, %s not a number.\n",string,pos+l);
      CMemoryFreeBlock (string);
      if(a_value!=NULL) *a_value = 0.;
      return OCutLe;
    }

  if(a_value!=NULL) *a_value = value;

/* cut column */
  *pos        = '\0';
  TrimString (string);
  if(a_column!=NULL) *a_column   = string;

  return      cut;
}
/***************************************************************************/
static int GetLineIdentifier (
 int a_number
,char** a_list
,char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_list==NULL) return NotFound;
  if(a_string==NULL) return NotFound;
  for(count=0;count<a_number;count++) 
    {
      if( (a_list[count]!=NULL) && (strcmp(a_string,a_list[count])==0) ) return count;
    }
  return NotFound;
}
/***************************************************************************/
static void TrimString (
 char* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count,pos;
  char c;
  int  length,found;
/*.........................................................................*/
  if(This==NULL) return;
/* trim begin */
  found = 0;
  pos   = 0;
  for(count=0;(c=This[count])!='\0';count++)
    {
      if(c!=' ') found = 1;
      if(found==1) 
        {
           This[pos] = c;
           pos++;
        }
    }
  This[pos] = '\0'; 
  
/* trim end */
  length      = pos;
  pos         = -1;
  for(count=length-1;count>=0;count--)  
    {
      if(This[count]!=' ') 
	{
	  pos = count+1;
	  break;
	}
    }
  if(pos==-1) This[0]   = 0; 
  else        This[pos] = 0;
}
/***************************************************************************/
int OCutGetAttribute (
 OCut  This
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
  else if(strcmp(a_name,"script")==0)     *((char**)a_addr)  = This->script;
  else 
    {
      CInfoF ("OCutGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OCutSetAttribute (
 OCut  This
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

       if(strcmp(a_name,"identifier")==0) {}
  else if(strcmp(a_name,"name")==0)       OCutSetName      (This,*((char**)a_addr));
  else if(strcmp(a_name,"script")==0)     OCutSetScript    (This,*((char**)a_addr));
  else 
    {
      CInfoF ("OCutSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
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
  CErrorHandle ("Ho","OCut",a_name,a_code);
}
