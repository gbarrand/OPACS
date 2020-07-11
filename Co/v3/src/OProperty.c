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

#include <CError.h>
#include <CPrinter.h>
#include <CMemory.h>
#include <CString.h>
#include <CText.h>
#include <CList.h>
#include <CLkStat.h>

#include <OProperty.h>

typedef struct _OPropertyRecord
{
  LkEntryRec links;
  char*      name;
  int        type;
  int        length;
  char*      format;
  union     
    {
      void* p;
      int   i;
    }        userData;
  int        head;
  int        isClass;
  int        isMethod;
  int        isReferences;
} OPropertyRecord;
 
#ifdef __cplusplus
extern "C"{
#endif
static void      ClearRecord        (OProperty);
static void      ClearEntry         (LkEntry);
static OProperty GetObjectFromEntry (LkEntry);
static void      ReportError        (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
typedef char* OString;
#define MAXIMUM(a,b)                     ((a)>(b)?a:b)
#define MINIMUM(a,b)                     ((a)<(b)?a:b)

static struct {
  LkListRec     extent;
  OProperty*    identifiers;
  int           variableInteger;
  unsigned long variableUnsignedLong;
  double        variableDouble;
  float         variableFloat;
  char*         variableString;  
  int           variableVoid;
  int           variableBoolean;
  int*          variableArrayOfIntegers;
  Ulong*        variableArrayOfUnsignedLongs;
  float*        variableArrayOfFloats;
  double*       variableArrayOfDoubles;
  char**        variableArrayOfStrings;
  int*          ints;
  double*       doubles;
  Ulong*        ulongs;
  int           stringn;
  OString*      strings;
} Class = {
{NULL,NULL},NULL,
0,0L,0.,0.,NULL,0,0,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,0,NULL};
/***************************************************************************/
void OPropertyClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  LkListClear       (&(Class.extent),ClearEntry);
  LkListInit        (&(Class.extent));
  CListDelete       ((CList)Class.identifiers);
  Class.identifiers = NULL;;
  CMemoryFreeBlock  (Class.ints);
  Class.ints        = NULL;
  CMemoryFreeBlock  (Class.doubles);
  Class.doubles     = NULL;
  CMemoryFreeBlock  (Class.ulongs);
  Class.ulongs      = NULL;
  CTextDelete       (Class.stringn,Class.strings);
  Class.stringn     = 0;
  Class.strings     = NULL;
}
/***************************************************************************/
OProperty OPropertyCreate (
 char*     a_name
,int       a_type
,void*     a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty           This;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) {ReportError("OPropCreate",CErrorValue);return NULL;}

  This               = (OProperty)CMemoryAllocateBlock (sizeof(OPropertyRecord));
  if(This==NULL)     return NULL;

  LkEntryInit        (&(This->links),&(Class.extent));

  This->name         = CStringDuplicate (a_name);
  This->userData.p   = a_user;
  This->type         = a_type;
  This->isClass      = 0;
  This->isMethod     = 0;
  This->isReferences = 0;

  switch(a_type)
    {
    case OPropertyVoid:
      This->length  = 4;   /*Size of "void".*/
      This->format  = CStringDuplicate ("%d"); /*Not used.*/
      break;
    case OPropertyBoolean:
      This->length  = 5;   /*Size of "false".*/
      This->format  = CStringDuplicate ("%d"); /*Not used.*/
      break;
    case OPropertyArrayOfIntegers:
    case OPropertyInteger:
      This->length  = 8;
      This->format  = CStringDuplicate ("%d");
      break;
    case OPropertyArrayOfUnsignedLongs:
    case OPropertyUnsignedLong:
      This->length  = 12;
      This->format  = CStringDuplicate ("%lu");
      break;
    case OPropertyArrayOfFloats:
    case OPropertyFloat:
    case OPropertyArrayOfDoubles:
    case OPropertyDouble:
      This->length  = 12;
      This->format  = CStringDuplicate ("%g");
      break;
    case OPropertyArrayOfStrings:
    case OPropertyString:
      This->length  = 12;
      This->format  = CStringDuplicate ("%s");
      break;
    default:
      This->length  = 8;
      This->format  = CStringDuplicate ("xxxxxxxx");
      break;
    }

  return        This;
}
/***************************************************************************/
OProperty OPropertyCreateMethod (
 char*     a_name
,int       a_type
,void*     a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty           This;
/*.........................................................................*/
  This           = OPropertyCreate (a_name,a_type,a_user);
  if(This==NULL) return NULL;
  This->isMethod = 1;
  return         This;
}
/***************************************************************************/
OProperty OPropertyCreateClass (
 char*     a_name
,int       a_type
,void*     a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty           This;
/*.........................................................................*/
  This           = OPropertyCreate (a_name,a_type,a_user);
  if(This==NULL) return NULL;
  This->isClass  = 1;
  return         This;
}
/***************************************************************************/
OProperty OPropertyCreateClassMethod (
 char*     a_name
,int       a_type
,void*     a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty           This;
/*.........................................................................*/
  This            = OPropertyCreate (a_name,a_type,a_user);
  if(This==NULL)  return NULL;
  This->isClass   = 1;
  This->isMethod  = 1;
  return          This;
}
/***************************************************************************/
OProperty OPropertyCreateReferences (
 char*     a_name
,int       a_type
,void*     a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty           This;
/*.........................................................................*/
  This               = OPropertyCreate (a_name,a_type,a_user);
  if(This==NULL)     return NULL;
  This->isReferences = 1;
  return             This;
}
/***************************************************************************/
void OPropertySetFormat (
 OProperty This 
,int   a_length
,char* a_format
)
/***************************************************************************/
/*
  Dangerous to change format for OPropertyString due to 
 CStringCreateF(strlen) in OPropertyPrintVariableInString.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->type==OPropertyString) return;
  if( (a_length<=0) || (a_format==NULL) || (*a_format=='\0')) return;
  This->length  = a_length;
  CStringDelete (This->format);
  This->format  = CStringDuplicate (a_format);
}
/***************************************************************************/
void OPropertyDelete (
  OProperty This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  LkEntryDisconnect (&(This->links));
  ClearRecord       (This);
  CMemoryFreeBlock  (This);
}
/***************************************************************************/
static void ClearEntry (
 LkEntry This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  ClearRecord    (GetObjectFromEntry(This));
}
/***************************************************************************/
static OProperty GetObjectFromEntry (
 LkEntry This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  y0 = NULL;
  char*  x0;
  int    offset; 
/*.........................................................................*/
  x0     = (char*) & ((OProperty)y0)->links;
  offset = x0-y0;
  return (OProperty) ((char*) This - offset );
}
/***************************************************************************/
static void ClearRecord (
 OProperty This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete    (This->name);
  CStringDelete    (This->format);
  This->name       = NULL;
  This->type       = OPropertyInteger;
  This->length     = 0;
  This->format     = NULL;
  This->userData.p = NULL;
}
/***************************************************************************/
int OPropertyIsListOfReferences (
 OProperty This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->isReferences;
}
/***************************************************************************/
char* OPropertyGetName (
 OProperty This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
int OPropertyGetType (
 OProperty This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return OPropertyInteger;
  return This->type;
}
/***************************************************************************/
int OPropertyIsClass (
 OProperty This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->isClass;
}
/***************************************************************************/
int OPropertyIsMethod (
 OProperty This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->isMethod;
}
/***************************************************************************/
void* OPropertyGetUserData (
 OProperty This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->userData.p;
}
/***************************************************************************/
int OPropertyGetUserDataInteger (
 OProperty This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->userData.i;
}
/***************************************************************************/
void OPropertySetUserDataInteger (
 OProperty This
,int a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->userData.i = a_user;
}
/***************************************************************************/
void OPropertyPutHTML_InStream (
 OProperty This
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* type = NULL;
  FILE* file;
/*.........................................................................*/
  if(This==NULL) return;

  file = va_arg(a_args,FILE*);

       if(This->type==OPropertyInteger)              type = CStringDuplicate("int");
  else if(This->type==OPropertyDouble)               type = CStringDuplicate("double");
  else if(This->type==OPropertyUnsignedLong)         type = CStringDuplicate("ulong");
  else if(This->type==OPropertyFloat)                type = CStringDuplicate("float");
  else if(This->type==OPropertyString)               type = CStringDuplicate("string");
  else if(This->type==OPropertyVoid)                 type = CStringDuplicate("void");
  else if(This->type==OPropertyBoolean)              type = CStringDuplicate("boolean");
  else if(This->type==OPropertyArrayOfIntegers)      type = CStringDuplicate("int[]");
  else if(This->type==OPropertyArrayOfDoubles)       type = CStringDuplicate("double[]");
  else if(This->type==OPropertyArrayOfUnsignedLongs) type = CStringDuplicate("ulong[]");
  else if(This->type==OPropertyArrayOfStrings)       type = CStringDuplicate("string[]");
  else                                               type = CStringDuplicate("?");

  fprintf (file,"\
\n<PRE>\n\
name = %s\n\
is class = %s\n\
is method = %s\n\
type = %s\n\
length = %d\n\
format = %s\n\
</PRE>\n",
	   This->name,
           This->isClass==1 ? "yes" : "no",
           This->isMethod==1 ? "yes" : "no",
	   type,
	   This->length,
	   This->format);

  CStringDelete (type);

}
/***************************************************************************/
char* OPropertyGetHeader (
 OProperty This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int   lname,length,lspace;
  char* name;
/*.........................................................................*/
  if(This==NULL) return NULL;
  This->head = 0;
  lname      = strlen  (This->name);
  length     = MAXIMUM (lname,This->length);
  lspace     = lname < length ? length - lname : 0;
  name       = CStringCreate(length);
  sprintf    (name+lspace,"%s",This->name);
  This->head = length;
  return     name;
}
/***************************************************************************/
void* OPropertyGetVariable (
 OProperty  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return NULL;
  Class.variableInteger              = 0;
  Class.variableUnsignedLong         = 0L;
  Class.variableDouble               = 0.;
  Class.variableFloat                = 0.;
  Class.variableString               = NULL; 
  Class.variableVoid                 = 0;
  Class.variableBoolean              = 0;
  Class.variableArrayOfIntegers      = NULL;
  Class.variableArrayOfUnsignedLongs = NULL;
  Class.variableArrayOfFloats        = NULL;
  Class.variableArrayOfDoubles       = NULL;
  Class.variableArrayOfStrings       = NULL;
       if(This->type==OPropertyInteger)              return &(Class.variableInteger);
  else if(This->type==OPropertyUnsignedLong)         return &(Class.variableUnsignedLong);
  else if(This->type==OPropertyDouble)               return &(Class.variableDouble);
  else if(This->type==OPropertyFloat)                return &(Class.variableFloat);
  else if(This->type==OPropertyString)               return &(Class.variableString);
  else if(This->type==OPropertyVoid)                 return &(Class.variableInteger);
  else if(This->type==OPropertyBoolean)              return &(Class.variableVoid);
  else if(This->type==OPropertyArrayOfIntegers)      return &(Class.variableArrayOfIntegers);
  else if(This->type==OPropertyArrayOfFloats)        return &(Class.variableArrayOfFloats);
  else if(This->type==OPropertyArrayOfDoubles)       return &(Class.variableArrayOfDoubles);
  else if(This->type==OPropertyArrayOfUnsignedLongs) return &(Class.variableArrayOfUnsignedLongs);
  else if(This->type==OPropertyArrayOfStrings)       return &(Class.variableArrayOfStrings);
  return NULL;
}
/***************************************************************************/
char* OPropertyConvertVariableToString (
 OProperty This
,void* a_addr
,int   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string = NULL;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  if(a_addr==NULL) 
    {
      return CStringDuplicate("(nil)");
    }
       if(This->type==OPropertyInteger)      
    string = CStringCreateF (64,"%d"  ,*((int*)a_addr));
  else if(This->type==OPropertyDouble)       
    string = CStringCreateF (64,"%g"  ,*((double*)a_addr));
  else if(This->type==OPropertyUnsignedLong) 
    string = CStringCreateF (64,"%lu" ,*((unsigned long*)a_addr));
  else if(This->type==OPropertyFloat)        
    string = CStringCreateF (64,"%g"  ,*((float*)a_addr));
  else if(This->type==OPropertyString)       
    {
      if( (a_addr==NULL) || (*((char**)a_addr)==NULL) )
	{
	  string = CStringDuplicate ("(nil)");
	}
      else
	{
	  string = CStringCreateF (strlen(*((char**)a_addr)) ,"%s"  , *((char**)a_addr) );
	}
    }
  else if(This->type==OPropertyVoid)      
    string = CStringDuplicate("void");
  else if(This->type==OPropertyBoolean)      
    string = CStringDuplicate( (*((int*)a_addr))==1 ? "true" : "false"  );
  else if(This->type==OPropertyArrayOfIntegers)
    {
      if( (a_addr!=NULL) && (a_number!=0) )
        {
	  int*    list;
          list    = *((int**)a_addr);
          string  = (char*)CMemoryAllocateBlock (64 * a_number * sizeof(char));
          if(string!=NULL)
	    {
	       int count;
	      int pos     = 0;
	      for(count=0;count<a_number;count++)
		{
		  pos += CStringPrintF (string+pos,"%d\n",list[count]);
		}
	      string[strlen(string)-1] = '\0';
	    }
	}
    }
  else if(This->type==OPropertyArrayOfDoubles)
    {
      if( (a_addr!=NULL) && (a_number!=0) )
        {
	  double* list;
          list    = *((double**)a_addr);
          string  = (char*)CMemoryAllocateBlock (64 * a_number * sizeof(char));
          if(string!=NULL)
	    {
	       int count;
	      int pos     = 0;
	      for(count=0;count<a_number;count++)
		{
		  pos += CStringPrintF (string+pos,"%g\n",list[count]);
		}
	      string[strlen(string)-1] = '\0';
	    }
	}
    }
  else if(This->type==OPropertyArrayOfUnsignedLongs)
    {
      if( (a_addr!=NULL) && (a_number!=0) )
        {
	  Ulong*  list;
          list    = *((Ulong**)a_addr);
          string  = (char*)CMemoryAllocateBlock (64 * a_number * sizeof(char));
          if(string!=NULL)
	    {
	      int count;
	      int pos     = 0;
	      for(count=0;count<a_number;count++)
		{
		  pos += CStringPrintF (string+pos,"%lu\n",list[count]);
		}
	      string[strlen(string)-1] = '\0';
	    }
	}
    }
  else if(This->type==OPropertyArrayOfStrings)
    {
      if( (a_addr!=NULL) && (a_number!=0) )
        {
	  char**  list;
          list    = *((OString**)a_addr);
          string  = CTextConvertToString (a_number,list,"\n");
	}
    }
  CStringTrim (string);
  return      string;
}
/***************************************************************************/
int OPropertySetVariableFromString (
 OProperty This
,char* a_string
,void* a_addr
,int*  a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int status;
/*.........................................................................*/
  if(a_number!=NULL)  *a_number = 0;
  if(This==NULL)     return 0;
  if(a_addr==NULL)   return 0;
  if( (a_string!=NULL) && (strcmp(a_string,"(nil)")==0) ) return 1;
       if(This->type==OPropertyInteger)      
    {
      int   value;
      value = (int)CStringConvertToLong    (a_string,&status);
      if(status==0)
        {
	  CWarnF ("OPropertySetVariableFromString : Can't convert %s to integer.\n",a_string);
	  return 0;
	}
      *((int*)a_addr)  = value;
    }
  else if(This->type==OPropertyDouble)       
    {
      double value;
      value  = CStringConvertToDouble  (a_string,&status);
      if(status==0)
        {
	  CWarnF ("OPropertySetVariableFromString : Can't convert %s to double.\n",a_string);
	  return 0;
	}
      *((double*)a_addr)  = value;
    }
  else if(This->type==OPropertyUnsignedLong) 
    {
      Ulong value;
      value = CStringConvertToLong    (a_string,&status);
      if(status==0)
        {
	  CWarnF ("OPropertySetVariableFromString : Can't convert %s to unsigned long.\n",a_string);
	  return 0;
	}
      *((Ulong*)a_addr)  = value;
    }
  else if(This->type==OPropertyFloat)        
    {
      float value;
      value = (float)CStringConvertToDouble  (a_string,&status);
      if(status==0)
        {
	  CWarnF ("OPropertySetVariableFromString : Can't convert %s to float.\n",a_string);
	  return 0;
	}
      *((float*)a_addr)  = value;
    }
  else if(This->type==OPropertyString)
    {
      *((char**) a_addr)  = a_string;
    }
  else if(This->type==OPropertyVoid)
    {
      *((int*) a_addr)  = 0;
    }
  else if(This->type==OPropertyBoolean)      
    {
      int   value;
      value = (int)CStringConvertToBoolean (a_string,&status);
      if(status==0)
        {
	  CWarnF ("OPropertySetVariableFromString : Can't convert %s to boolean.\n",a_string);
	  return 0;
	}
      *((int*)a_addr)  = value;
    }
  else if(This->type==OPropertyArrayOfIntegers)
    {
      int               count  = 0;
      CMemoryFreeBlock  (Class.ints);
      Class.ints        = NULL;
      if(a_string!=NULL)
	{
	  int    size   = 0;
	  int    sblock = 32;
	  char*  begin;
	  begin  = a_string;
	  while(1)
	    {  
	      long   d;
	      char*  end;
	      d      = strtol(begin,&end,10);
	      if(begin==end) break; 
	      if(count>=size)
		{
		  Class.ints   = Class.ints!=NULL ? 
		      (int*)CMemoryChangeBlockSize(Class.ints, (size+sblock) * sizeof(int)) :
		      (int*)CMemoryAllocateBlock  (                  sblock  * sizeof(int));
		  if(Class.ints==NULL) 
		    {
		      *((int**)a_addr) = NULL;
		      return 0;
		    }
		  size  += sblock;
		}
	      Class.ints[count] = (int)d;
	      count++;
	      begin = end;
	    }
	}
      
      if(a_number!=NULL) *a_number = count;
      *((int**)a_addr) = Class.ints;
    }
  else if(This->type==OPropertyArrayOfDoubles)
    {
      int           count  = 0;
      CMemoryFreeBlock      (Class.doubles);
      Class.doubles = NULL;
      if(a_string!=NULL)
	{
	  int    size   = 0;
	  int    sblock = 32;
	  char*  begin;
	  begin  = a_string;
	  while(1)
	    {  
	      double d;
	      char*  end;
	      d      = strtod(begin,&end);
	      if(begin==end) break; 
	      if(count>=size)
		{
		  Class.doubles   = Class.doubles!=NULL ? 
		    (double*)CMemoryChangeBlockSize(Class.doubles, (size+sblock) * sizeof(double)) :
		    (double*)CMemoryAllocateBlock  (                     sblock  * sizeof(double));
		  if(Class.doubles==NULL) 
		    {
		      *((double**)a_addr) = NULL;
		      return 0;
		    }
		  size  += sblock;
		}
	      Class.doubles[count] = d;
	      count++;
	      begin = end;
	    }
	}
      if(a_number!=NULL) *a_number = count;
      *((double**)a_addr) = Class.doubles;
    }
  else if(This->type==OPropertyArrayOfUnsignedLongs)
    {
      int           count  = 0;
      CMemoryFreeBlock      (Class.ulongs);
      Class.ulongs = NULL;
      if(a_string!=NULL)
	{
	  int    size   = 0;
	  int    sblock = 32;
	  char*  begin;
	  begin  = a_string;
	  while(1)
	    {  
	      Ulong d;
	      char*  end;
	      d      = strtol(begin,&end,10);
	      if(begin==end) break; 
	      if(count>=size)
		{
		  Class.ulongs   = Class.ulongs!=NULL ? 
		    (Ulong*)CMemoryChangeBlockSize(Class.ulongs, (size+sblock) * sizeof(Ulong)) :
		    (Ulong*)CMemoryAllocateBlock  (                    sblock  * sizeof(Ulong));
		  if(Class.ulongs==NULL) 
		    {
		      *((Ulong**)a_addr) = NULL;
		      return 0;
		    }
		  size  += sblock;
		}
	      Class.ulongs[count] = d;
	      count++;
	      begin = end;
	    }
	}
      if(a_number!=NULL) *a_number = count;
      *((Ulong**)a_addr) = Class.ulongs;
    }
  else if(This->type==OPropertyArrayOfStrings)
    {
      CTextDelete   (Class.stringn,Class.strings);
      Class.stringn = 0;
      Class.strings = NULL;
      if( (a_string!=NULL) && (strchr(a_string,'\n')!=NULL)) 
	Class.strings = CTextCreateFromString (a_string,&Class.stringn);
      else 
	Class.strings = CStringGetWords      (a_string," ",&Class.stringn);
/*
      printf         ("debug : %d %lu\n",Class.stringn,Class.strings);
      CTextDump      (Class.stringn,Class.strings);
*/
      if(a_number!=NULL)  *a_number = Class.stringn;
      *((OString**)a_addr) = Class.strings;
    }
  return 1;
}
/***************************************************************************/
char* OPropertyPrintVariableInString (
 OProperty This
,void* a_addr
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char  format[128];
  char* string = NULL;
  char* name = NULL;
/*.........................................................................*/
  if(This==NULL)         return NULL;
  if(This->format==NULL) return NULL;
  sprintf  (format,"%s",This->format);
  if(a_addr==NULL) 
    {
      string = CStringDuplicate("(nil)");
    }
  else
    {
            if(This->type==OPropertyInteger)      string = CStringCreateF   (64,format,*((int*)a_addr));
       else if(This->type==OPropertyDouble)       string = CStringCreateF   (64,format,*((double*)a_addr));
       else if(This->type==OPropertyUnsignedLong) string = CStringCreateF   (64,format,*((unsigned long*)a_addr));
       else if(This->type==OPropertyFloat)        string = CStringCreateF   (64,format,*((double*)a_addr));
       else if(This->type==OPropertyVoid)         string = CStringDuplicate ("void");
       else if(This->type==OPropertyBoolean)      string = CStringDuplicate ( (*((int*)a_addr))==1 ? "true" : "false" );
       else if(This->type==OPropertyString)       
	 {
	   if( (a_addr==NULL) || (*((char**)a_addr)==NULL) )
	     {
	       string = CStringDuplicate ("(nil)");
	     }
	   else
	     {
	       string = CStringCreateF (strlen(*((char**)a_addr)),format, *((char**)a_addr) );
	     }
	 }
    }
  CStringTrim (string);
  if(string!=NULL)
    {
      int           lstring,length,lspace;
      lstring       = strlen(string);
      length        = This->head;
      if(lstring < length)
        {
	  lspace        = length - lstring;
	  name          = CStringCreate(length);
	  sprintf       (name+lspace,"%s",string);
	  name[length]  = '\0';
	  CStringDelete (string);
	}
      else
        {
	  name          = string;
	  name[length]  = '\0';
	}
    }
  return        name;
}
/***************************************************************************/
char* OPropertyPrintArrayInString (
 OProperty  This
,void*  a_addr
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*    name;
  char*    string = NULL;
  char     format[64];
/*.........................................................................*/
  if(This==NULL)         return NULL;
  if(This->format==NULL) return NULL;
  name    = OPropertyGetName(This);
  sprintf ( format,"%s %%d : %s",name,This->format);
  if(a_addr==NULL) 
    {
      return CStringCreateF(strlen(name)+10,"\n%s : (nil)",name);
    }
  if(a_number!=0) CStringConcatenate (&string,"\n");
  for(count=0;count<a_number;count++)
    { 
      char*         str = NULL; 
           if(This->type==OPropertyArrayOfIntegers)       
	str = CStringCreateF (128,format,count+1,(*(int**)   a_addr)[count]);
      else if(This->type==OPropertyArrayOfDoubles)        
	str = CStringCreateF (128,format,count+1,(*(double**)a_addr)[count]);
      else if(This->type==OPropertyArrayOfUnsignedLongs)  
	str = CStringCreateF (128,format,count+1,(*(Ulong**) a_addr)[count]);
      else if(This->type==OPropertyArrayOfStrings)        
	str = CStringCreateF (2048,format,count+1,(*(char***) a_addr)[count]);
      CStringConcatenate    (&string,str);
      if(count!=a_number-1) CStringConcatenate (&string,"\n");
      CStringDelete (str);
    }      
  return  string;
}
/***************************************************************************/
int OPropertyIsArray (
 OProperty This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  if(This->type==OPropertyArrayOfIntegers)       return 1;
  if(This->type==OPropertyArrayOfDoubles)        return 1;
  if(This->type==OPropertyArrayOfUnsignedLongs)  return 1;
  if(This->type==OPropertyArrayOfStrings)        return 1;
  return 0;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
OProperty* OPropertyGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int         number,count;
/*.........................................................................*/
  number            = LkListCount (&(Class.extent));
  CListDelete       ((CList)Class.identifiers);
  Class.identifiers = (OProperty*)CListCreate(number);
  count             = 0;
 {LkIterator        iterator;
  LkEntry           entry;
  iterator          = LkIteratorNew (&(Class.extent));
  while ((entry=LkIteratorNext(iterator))!=NULL)
    { 
      Class.identifiers[count] = GetObjectFromEntry(entry);
      count++;
    }
  LkIteratorDelete (iterator);}
  return           Class.identifiers;
}
/***************************************************************************/
int OPropertyGetAttribute (
 OProperty  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number) *a_number = 0;
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0)         *((Ulong*)a_addr) = (Ulong)This;
  else if(strcmp(a_name,"name")==0)               *((char**)a_addr) = OPropertyGetName (This);
  else if(strcmp(a_name,"isClass")==0)            *((int*)a_addr)   = OPropertyIsClass(This);
  else if(strcmp(a_name,"isMethod")==0)           *((int*)a_addr)   = OPropertyIsMethod(This);
  else if(strcmp(a_name,"isListOfReferences")==0) *((int*)a_addr)   = OPropertyIsListOfReferences (This);
  else 
    {
      CInfoF ("OPropertyGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
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
  CErrorHandle ("Co","OProperty",a_name,a_code);
}
#include <CLkStat.ic>
