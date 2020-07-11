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
#include <stdlib.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CCharacter.h>
#include <CString.h>
#include <CText.h>

typedef int (*CompareFunction)(const void *, const void *);

#define NotFound (-1)

#ifdef __cplusplus
extern "C"{
#endif
static int   Compare     (char**,char**);
#ifdef __cplusplus
}
#endif

/***************************************************************************/
void CTextInitialize (
 int* a_linen
,char*** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_linen!=NULL) *a_linen = 0;
  if(a_lines!=NULL) *a_lines = NULL;
}
/***************************************************************************/
void CTextDelete (
 int a_linen
,char** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_lines==NULL) return;
  for(count=0;count<a_linen;count++) CMemoryFreeBlock(a_lines[count]);
  CMemoryFreeBlock (a_lines);
}
/***************************************************************************/
char** CTextCreateFromString (
 char* a_string
,int* a_linen
)
/***************************************************************************/
/*
  string is a list separated by "\n" or "\\n"
  free with: CTextDelete                                     
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    linen;
  char** lines = NULL;
  char*  string;
  int    length,pos,count;
/*.........................................................................*/
  if(a_linen!=NULL)  *a_linen = 0;
  if(a_string==NULL) return NULL;
  length             = strlen(a_string);
  if(length<=0)      return NULL;
  string             = CStringDuplicate(a_string);
  if(string==NULL)   return NULL;
  pos                = 0;
  linen              = 0;
  lines              = NULL;
  length             ++;
  for(count=0;count<length;count++) 
    {
      if( (string[count]=='\n') || (string[count]=='\0') || ( (string[count]=='\\') && (string[count+1]=='n') ) ) 
        { 
	  char          shift_one;
          char*         item;
          shift_one     = (string[count]=='\n' ? 1 : 0);
          string[count] = '\0';
          item          = CStringDuplicate(string+pos);
	  if(shift_one==1)
	    {
	      pos = count+1;
	    }
	  else
	    {
	      pos = count+2;
	      count++;
	    }
          if(item!=NULL)
            {
               lines = linen!=0 ? 
		       (char**)CMemoryChangeBlockSize((char*)lines,(linen+1)*sizeof(char*)) :
		       (char**)CMemoryAllocateBlock(sizeof(char*));
               if(lines==NULL)   return NULL;
               lines[linen] = item;
               linen++;
            }
        }
    }
  CMemoryFreeBlock  (string);
  if(a_linen!=NULL) *a_linen = linen;
  return            lines;
}
/***************************************************************************/
char** CTextDuplicate (
 int    a_linen
,char** a_lines
,int*   a_number
)
/***************************************************************************/
/*  free with: CTextDelete                                             */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** lines  = NULL;
/*.........................................................................*/
  if(a_number!=NULL) *a_number    = 0;
  if(a_lines==NULL)  return NULL;
  lines              = (char**)CMemoryAllocateBlock( a_linen * sizeof (char*));
  if(lines==NULL)    return NULL;
  for(count=0;count<a_linen;count++) lines[count] = CStringDuplicate(a_lines[count]);
  if(a_number!=NULL) *a_number = a_linen;
  return             lines;
}
/***************************************************************************/
void CTextAddLine (
 int* a_linen
,char*** a_lines
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char** lines;
/*.........................................................................*/
  if(a_string==NULL) return;
  if(a_linen==NULL) return;
  if(a_lines==NULL)   return;
  if(*a_lines==NULL)  
    {
      lines           = (char**)CMemoryAllocateBlock(sizeof(char*));
      if(lines==NULL) return;
      *a_lines        = lines;
      *a_linen        = 1;
      (*a_lines)[0]   = CStringDuplicate(a_string);
    }
  else
    {
      lines                 = (char**)CMemoryChangeBlockSize((char*)(*a_lines),((*a_linen) + 1)*sizeof(char*));
      if(lines==NULL)       return;
      *a_lines              = lines;
      (*a_lines)[*a_linen] = CStringDuplicate(a_string);
      (*a_linen)++;
    }
}
/***************************************************************************/
void CTextAddAllocatedLine (
 int* a_linen
,char*** a_lines
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char** lines;
/*.........................................................................*/
  if(a_string==NULL) return;
  if(a_linen==NULL) return;
  if(a_lines==NULL)   return;
  if(*a_lines==NULL)  
    {
      lines           = (char**)CMemoryAllocateBlock(sizeof(char*));
      if(lines==NULL) return;
      *a_lines        = lines;
      *a_linen        = 1;
      (*a_lines)[0]   = a_string;
    }
  else
    {
      lines                = (char**)CMemoryChangeBlockSize((char*)(*a_lines),((*a_linen) + 1)*sizeof(char*));
      if(lines==NULL)      return;
      *a_lines             = lines;
      (*a_lines)[*a_linen] = a_string;
      (*a_linen)++;
    }
}
/***************************************************************************/
void CTextPrependLine (
 int* a_linen
,char*** a_lines
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char** lines;
/*.........................................................................*/
  if(a_string==NULL) return;
  if(a_linen==NULL) return;
  if(a_lines==NULL)   return;
  if(*a_lines==NULL)  
    {
      lines           = (char**)CMemoryAllocateBlock(sizeof(char*));
      if(lines==NULL) return;
      *a_lines        = lines;
      *a_linen        = 1;
      (*a_lines)[0]   = CStringDuplicate(a_string);
    }
  else
    {  int count;
      lines                 = (char**)CMemoryChangeBlockSize((char*)(*a_lines),((*a_linen) + 1)*sizeof(char*));
      if(lines==NULL)       return;
      for(count=(*a_linen);count>=1;count--) lines[count] = lines[count-1];
      *a_lines              = lines;
      (*a_lines)[0]         = CStringDuplicate(a_string);
      (*a_linen)++;
    }
}
/***************************************************************************/
void CTextConcatenate (
 int*    a_linen
,char*** a_lines
,int     a_number
,char**  a_text
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** lines;
/*.........................................................................*/
  if(a_linen==NULL) return;
  if(a_lines==NULL) return;
  if( (a_text==NULL) || (a_number==0) ) return;
  lines = (*a_lines!=NULL) ? 
         (char**)CMemoryChangeBlockSize((char*)(*a_lines),(*a_linen + a_number) * sizeof(char*)) :
	 (char**) CMemoryAllocateBlock(                 (*a_linen + a_number) * sizeof(char*));
  if(lines==NULL) return;
  *a_lines        = lines;
  for(count=0;count<a_number;count++) 
    {
       (*a_lines)[*a_linen] = CStringDuplicate(a_text[count]);
       (*a_linen)++;
    }
}
/***************************************************************************/
void CTextClean (
 int* a_linen
,char*** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
   int rcount;
/*.........................................................................*/
  if(a_linen==NULL) return;
  if(a_lines==NULL)   return;
  rcount       = 0;
  for(count=0;count<*a_linen;count++) 
    {
      if((*a_lines)[count]!=NULL) 
	{
	  (*a_lines)[rcount] = (*a_lines)[count];
	  rcount++;
	}
    }
  if(rcount==0) 
    {
      CMemoryFreeBlock(*a_lines);
      *a_lines = NULL;
    }
  *a_linen    = rcount;
}
/***************************************************************************/
void CTextRemoveDuplicates (
 int* a_linen
,char*** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count,icount;
/*.........................................................................*/
  if(a_linen==NULL) return;
  if(a_lines==NULL)   return;
  for(count=1;count<*a_linen;count++) 
    { 
      for(icount=0;icount<count;icount++) 
        {
          if( ((*a_lines)[icount]!=NULL) && 
	      ((*a_lines)[count]!=NULL)  && 
	      (strcmp((*a_lines)[icount],(*a_lines)[count])==0)
	     )
           {
             CStringDelete     ((*a_lines)[icount]);
             (*a_lines)[icount] = NULL;                     
           }
        }
    }
  CTextClean (a_linen,a_lines);
}
/******************************************************************************/
int CTextGetLineIndex (
 int a_linen
,char** a_lines
,char* a_string
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_lines==NULL)  return NotFound;
  if(a_string==NULL) return NotFound;
  for(count=0;count<a_linen;count++) 
    { 
      if(CStringCompare(a_string,a_lines[count])==1) return count;
    }
  return NotFound;
}
/******************************************************************************/
char* CTextGetIthLine (
 int a_linen
,char** a_lines
,int a_index
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_lines==NULL)                       return NULL;
  if(a_linen<=0)                          return NULL;
  if( (a_index<0) || (a_index>=a_linen) ) return NULL;
  return a_lines[a_index];
}
/***************************************************************************/
void CTextSort (
 int a_linen
,char** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_lines==NULL) return; 
  if(a_linen<=1)    return;
  qsort  ((void *)a_lines,(size_t)a_linen,(size_t)sizeof(char*),(CompareFunction)Compare);
}
/***************************************************************************/
static int Compare (
 char** a_1 
,char** a_2 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_1==NULL) || (a_2==NULL) ) return 1;
  if(*a_1==NULL)     return -1;
  if(*a_2==NULL)     return 1;
  if((*a_1)==(*a_2)) return 0;
  return strcmp(*a_1,*a_2);
}
/***************************************************************************/
void CTextTrim (
 int a_linen
,char** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_lines==NULL)     return;
  for(count=0;count<a_linen;count++) CStringTrim (a_lines[count]);
}
/***************************************************************************/
char* CTextConvertToString (
 int a_linen
,char** a_lines
,char* a_limiter 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int   pos,length,length_limiter;
  char* string;
/*.........................................................................*/
  if(a_lines==NULL)     return NULL;
  if(a_limiter==NULL)  return NULL;
  if(a_linen<=0) return NULL;
  length_limiter  = strlen(a_limiter);
  length          = 0;
  for(count=0;count<a_linen;count++) 
    {
      if(a_lines[count]==NULL) continue;
      length += strlen(a_lines[count])+length_limiter;
    }
  if(length<=0) return NULL;
  string           = (char*)CMemoryAllocateBlock((length+1)*sizeof(char));
  if(string==NULL) return NULL;
  string[length]   = '\0';
  pos              = 0;
  for(count=0;count<a_linen;count++) 
    {
      if(a_lines[count]==NULL)   continue;
      length               = strlen(a_lines[count]);
      strncpy              (string+pos,a_lines[count] ,length);        pos += length;
      strncpy              (string+pos,a_limiter     ,length_limiter);pos += length_limiter;
    }
  string     [pos-length_limiter] = '\0';
  return     string;
}
/***************************************************************************/
char* CTextConvertToStringWithNewLines (
 int a_linen
,char** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CTextConvertToString (a_linen,a_lines,"\\n");
}
/***************************************************************************/
void CTextDump (
 int a_linen
,char** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_lines==NULL) return;
  for(count=0;count<a_linen;count++) CDumpF ("%s\n",a_lines[count]);
}
/***************************************************************************/
int CTextReplaceString (
 int a_linen
,char** a_lines
,char* a_from
,char* a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int status;
/*.........................................................................*/
  if(a_lines==NULL) return 0;
  if(a_from==NULL)  return 0;
  if(a_to==NULL)    return 0;
  status = 0;
  for(count=0;count<a_linen;count++) 
    {
      if(CStringReplacePart(&(a_lines[count]),a_from,a_to)==1) status = 1;
    }
  return status;
}
/***************************************************************************/
int CTextReplaceWord (
 int a_linen
,char** a_lines
,char* a_from
,char* a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int          status;
/*.........................................................................*/
  if(a_lines==NULL) return 0;
  if(a_from==NULL)  return 0;
  if(a_to==NULL)    return 0;
  status = 0;
  for(count=0;count<a_linen;count++) 
    {
      if(CStringReplaceWord(&(a_lines[count]),a_from,a_to)==1) status = 1;
    }
  return status;
}
/***************************************************************************/
int CTextSortByText (
 int     a_linen
,char**  a_lines
,int     a_number
,char**  a_text
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int    linen;
  char** lines;
/*.........................................................................*/
  if(a_lines==NULL)  return 0;
  if( (a_text==NULL) || (a_number==0) ) return 0;
  lines           = (char**)CMemoryAllocateBlock( a_linen * sizeof (char*));
  if(lines==NULL) return 0;
  linen           = 0;
  for(count=0;count<a_number;count++) 
    {
      int   index;
      index = CTextGetLineIndex (a_linen,a_lines,a_text[count]);
      if(index!=NotFound)
        {
	  lines [linen] = a_lines[index];
	  linen++;
	}
    }
  if(linen!=a_linen) 
    {
      CMemoryFreeBlock (lines);
      return 0;
    }
  for(count=0;count<a_linen;count++) 
    {
      a_lines[count] = lines [count];
    }
  CMemoryFreeBlock (lines);
  return 1;
}
/******************************************************************************/
int CTextCountString (
 int a_linen
,char** a_lines
,char* a_string
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count,number;
/*.........................................................................*/
  if(a_lines==NULL)  return 0;
  if(a_string==NULL) return 0;
  number = 0;
  for(count=0;count<a_linen;count++) 
    { 
      if(CStringCompare(a_string,a_lines[count])==1) number++;
    }
  return number;
}
