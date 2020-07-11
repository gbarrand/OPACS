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

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <CMemory.h>
#include <CSystem.h>
#include <CString.h>
#include <CPrinter.h>
#include <CError.h>

#include <CStream.h>

#ifdef __cplusplus
extern "C"{
#endif
static void TreatContinuator   (int*,char***,char);
static void ReportError        (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long        Ulong;

#define MINIMUM(a,b)         ((a)<(b)?a:b)

#define MAX_STRING 2048
static struct 
{
  int    status;
  char   string[MAX_STRING+1];
} Class = {1};
/***************************************************************************/
void CStreamInitializeBuffer (
 FILE* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.string[MAX_STRING] = '\0';
  This = NULL;
}
/***************************************************************************/
char* CStreamGetString (
 FILE* This 
)
/***************************************************************************/
/*
  On some system (NT) editors when saving binary files 
 put \r\n at place of \n ; we then look for \r\n.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int length;
/*.........................................................................*/
  if(This==NULL)                                 return NULL;
  if(fgets (Class.string,MAX_STRING,This)==NULL) return NULL;
  length              = strlen(Class.string);
  if( (Class.string[length-2]=='\r') && (Class.string[length-1]=='\n') ) 
    {
      Class.string[length-2] = '\0';
      length--;
      length--;
    }
  else if(Class.string[length-1]=='\n') 
    {
      Class.string[length-1] = '\0';
      length--;
    }
  return              Class.string;
}
/***************************************************************************/
void CStreamDump (
 FILE* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* strloop;
/*.........................................................................*/
  if(This==NULL) return;
  rewind         (This);
  CStreamInitializeBuffer (This);
  while((strloop=CStreamGetString(This))!=NULL) CPrinterPut (strloop);
}
/***************************************************************************/
long CStreamGetSize (
 FILE* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0L;
  if(fseek(This, 0L, SEEK_END)!=0)  /* end of file*/
    {
      CWarn ("Co/CStream.c/CStreamGetSize : fseek failure.\n");
      return 0L;
    }
  return   ftell(This);
}
/***************************************************************************/
void CStreamAppend (
 FILE* This 
,FILE* a_from 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_from==NULL) return;
/* go at the end of This*/
  if(fseek(This, 0L, SEEK_END)!=0)
    {
      CWarn ("Co/CStream.c/CStreamAppend : fseek failure.\n");
      return;
    }
 {char* strloop;
  CStreamInitializeBuffer (a_from);
  while((strloop=CStreamGetString(This))!=NULL)
    {
      fprintf (This,"%s\n",strloop);
    }}
}
/***************************************************************************/
char* CStreamRead (
 FILE* This 
,Ulong* a_size
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*      buff;
  Ulong      size;
  size_t     offset;
  size_t     sblock = 2048;
  size_t     filesize;
/*.........................................................................*/
  if(a_size!=NULL) *a_size   = 0L;
  if(This==NULL) return NULL;

/* try fast read */
  buff           = CStreamReadOnceThrough (This,&size);
  if(buff!=NULL) 
    {
#ifdef DEBUG
      printf ("debug : CStreamRead : size %lu.\n",size);
#endif
      if(a_size!=NULL) *a_size=size;
      return buff;
    }

/* read at once failed; try to read by little block */
  filesize         = (size_t)CStreamGetSize(This);
#ifdef DEBUG
  printf ("debug : CStreamRead : filesize %lu.\n",filesize);
#endif
  if(filesize==0L) 
    {
      ReportError ("CStreamRead",CErrorFailure);
      return      NULL;
    }
  rewind           (This);
  buff             = NULL;
  offset           = 0;
  while(1)
    { 
      size_t retval;
      buff = buff!=NULL ? 
	     (char*)CMemoryChangeBlockSize(buff,(offset+sblock)*sizeof(char)) :
	     (char*)CMemoryAllocateBlock(sblock*sizeof(char));
      if(buff==NULL) 
	{
	  ReportError    ("CStreamRead",CErrorFailure);
	  return NULL;
	}
/*
      for(count=0;count<sblock;count++)  { *(buff+offset+count) = ' ';}
*/
      if ((retval=fread((char *)(buff+offset),sblock, (size_t)1, This))!=(size_t)1)  
        {
#ifdef DEBUG
	  printf ("debug : CStreamRead : fread failed : %lu : %d.\n",retval,ferror(This));
#endif
          if (feof(This)!=0) /* eof */
            { 
	      size_t         sbuff;
              sbuff          = offset+sblock;
              size           = MINIMUM(filesize,sbuff);
              if(size==sbuff)
                { /* add one byte to be able to add \n if file contain lines */
                  buff = (char*) CMemoryChangeBlockSize(buff,(sbuff+1) * sizeof(char));
                }
              buff    [size] = '\0'; 
#ifdef DEBUG
	      printf ("debug : CStreamRead : size %lu.\n",size);
#endif
              if(a_size!=NULL) *a_size = size;
              return           buff;
            }
          else 
            { 
              ReportError      ("CStreamRead",CErrorFailure);
              CMemoryFreeBlock (buff);
              return           NULL;
            }
        }
      else
        {
#ifdef DEBUG
	  printf ("debug : CStreamRead : fread ok.\n");
#endif
          offset += sblock;
        }
    }
}
/***************************************************************************/
char* CStreamReadOnceThrough (
 FILE* This 
,Ulong* a_size
)
/***************************************************************************/
/*
  On NT read once through does not work quite well.   
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  long      filesize;
  char*     buff;
  int       nitem;
/*.........................................................................*/
  if(a_size!=NULL) *a_size    = 0L;
  if(This==NULL)  return NULL;
  filesize   = CStreamGetSize(This);
  if(filesize==0L) return NULL;
 /* add one byte to be able to add \n if file contains lines */
  buff       = (char*)CMemoryAllocateBlock((size_t)( (filesize+1) * sizeof(char)));
  if(buff==NULL)  return NULL;
  rewind     (This);
  nitem      = fread((char *)buff, (size_t)filesize, (size_t)1, This);
  if(nitem!=1)
    {
      ReportError      ("CStreamReadOnceThrough",CErrorIO);
#ifdef DEBUG
      printf ("debug : CStreamReadOnceThrough : fread of %d bytes failed : %d.\n",filesize,nitem);
#endif
      CMemoryFreeBlock (buff);
      return           NULL;
    }
  buff[filesize]  = '\0';
  if(a_size!=NULL) *a_size = filesize;
  return         buff;
}  
/***************************************************************************/
char* CStreamGetStrings (
 FILE* This
,char a_char
,int* a_linen
,char*** a_lines
)
/***************************************************************************/
/* 
  Free with:
   char*  lineb; 
   int    linen; 
   char** lines;
   lineb    = CStreamGetStrings(file,'\\',&linen,&lines);        
   CMemoryFreeBlock (lines); 
   CMemoryFreeBlock (lineb);

  On some system (NT) editors (edit, emacs) when saving binary files 
 put \r\n at place of \n ; we then look for \r\n.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int    count;
  Ulong  filesize;
  char*  buff;
  char** lines;
  int    linen;
  int    iline;
  Ulong  fsize_1;
/*.........................................................................*/
  if(a_linen!=NULL) *a_linen  = 0;
  if(a_lines!=NULL) *a_lines  = NULL;
  if(This==NULL) return NULL;
  rewind    (This);

  buff      = CStreamRead (This,&filesize);
  if(buff==NULL) return NULL;

  if(buff[filesize-1]!='\n') 
    {
      buff[filesize] = '\n';
      filesize++;
    }

/*get number of lines*/
  linen = 0;
  for(count=0;count<filesize;count++) 
    {
      if(*(buff+count)=='\n') 
	{
	  linen++;
	}
    }
  if(linen==0) 
    {
      CMemoryFreeBlock (buff);
      return           NULL;
    }

  lines = (char**) CMemoryAllocateBlock( (linen+1) * sizeof(char*) );  /*linen + last \n*/
  if(lines==NULL) 
    {
      CMemoryFreeBlock (buff);
      return           NULL;
    }
  iline        = 0;
  lines[iline] = buff;
  iline++;
  fsize_1      = filesize - 1;
  for(count=0;count<filesize;count++) 
    {
      /*For files where editors (exa on NT) have put \r\n at end of lines.*/
      if( (count!=fsize_1) && (*(buff+count)=='\r')  && (*(buff+count+1)=='\n') )
        {
          *(buff+count)   = '\0';      /*replace \r by \0 */
          lines[iline]    = buff+count+2;
          iline++;
	  count++;
        }
      else if(*(buff+count)=='\n') 
        {
          *(buff+count) = '\0';      /*replace \n by \0 */
          lines[iline]  = buff+count+1;
          iline++;
        }
    }

  lines[linen] = NULL;  /*null ended list*/

  TreatContinuator (&linen,&lines,a_char);

  if(a_linen!=NULL) *a_linen     = linen;
  if(a_lines!=NULL) *a_lines     = lines;

  return       buff;
}
/***************************************************************************/
char** CStreamGetText (
 FILE* This
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*  lineb;
  char** lines;
  int    linen;
/*.........................................................................*/
  lineb     = CStreamGetStrings(This,'\0',&linen,&lines);
  for(count=0;count<linen;count++) lines[count] = CStringDuplicate(lines[count]);
  CMemoryFreeBlock  (lineb);
  if(a_number!=NULL) *a_number = linen;
  return    lines;
}
/***************************************************************************/
void CStreamPutText (
 FILE*  This
,int    a_linen
,char** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_lines==NULL) return;
  for(count=0;count<a_linen;count++) 
    {
      if(a_lines[count]==NULL)
        {
          if(fprintf (This,"\n")<0) ReportError ("CStreamPutText",CErrorIO);
        }
      else
        {
          if(fprintf (This,"%s\n",a_lines[count])<0) ReportError ("CStreamPutText",CErrorIO);
        }
    }
}
/***************************************************************************/
void CStreamInitializeStatus (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.status = 1;
}
/***************************************************************************/
int CStreamGetStatus (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.status;
}
/***************************************************************************/
void CStreamPrintF (
 FILE* a_file
,char* a_format
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
/*.........................................................................*/
  if(a_format==NULL) return;
  if(a_file==NULL)   return;
  va_start           (args,a_format);
  if(vfprintf        (a_file,a_format,args)<0) 
    {
      ReportError          ("CStreamPrintSF",CErrorIO);
      Class.status   = 0;
    }
  va_end             (args);
}
/***************************************************************************/
static void TreatContinuator (
 int*    a_linen
,char*** a_lines
,char    a_char
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char** lines = NULL;
  int    linen = 0;
/*.........................................................................*/
  if(a_lines==NULL)  return;
  if(*a_lines==NULL) return;
  if(a_char=='\0')   return;

  linen = 0; 
  lines = (char**)CMemoryAllocateBlock( (*a_linen+1) * sizeof(char*));
  if(lines!=NULL)
    {
       int count;
      for(count=0;count<*a_linen;count++) 
	{
	  if(linen==0)
	    {
	      lines[linen] = (*a_lines)[count];
	      linen++;
	    }
	  else 
	    { 
	      char*    pvl;
	      int      length;
	      char*    eol;
	      pvl           = lines[linen-1];
	      if(pvl==NULL) ReportError("TreatContinuator",CErrorValue);
	      length   = strlen(pvl);
	      eol      = pvl + length - 1; /* */
	      if( (length!=0) && (eol!=NULL) && (*eol==a_char))
		{ char*  pos;
		  *eol   = '\n';
		  eol++;
		  pos    = (*a_lines)[count];
		  while(*pos!='\0')
		    {
		      *eol = *pos;
		      eol++;
		      pos++;
		    }
		  *eol = '\0';              
		}
	      else
		{
		  lines[linen] = (*a_lines)[count];
		  linen++;
		}
	    }
	}
      lines[linen] = NULL;
    }

  CMemoryFreeBlock (*a_lines);

  *a_linen = linen;
  *a_lines = lines;
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
  CErrorHandle ("Co","CStream",a_name,a_code);
}
