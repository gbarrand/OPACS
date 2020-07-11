/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>
#include <string.h>

#include <CMemory.h>
#include <CString.h>
#include <CText.h>
#include <CPrinter.h>
#include <CStream.h>
#include <CFile.h>
#include <CError.h>
#include <CList.h>

#include <OTupleP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void   InitializeClass ();
static void   FreeRecord      (OTuple);
static int    GetIdentifier   (OTuple,char*);
static char** GetStrings      (char*,int*);
static void   ReportError     (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
typedef char*         OString;
#define XtNumber(arr)  (sizeof(arr)/sizeof(arr[0]))
#define MINIMUM(a,b)   ((a)<(b)?a:b)
#define MAXIMUM(a,b)   ((a)>(b)?a:b)
#define NotFound       (-1)
#define DVOID__POS     -1.

#define TVAL(This,a_row,a_col) ((This)->values[a_row * (This)->cols + a_col])

static struct 
{
  OTuple* extent;
} Class = {NULL};
/***************************************************************************/
void OTupleClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy     ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent     = NULL;
}
/***************************************************************************/
OTuple* OTupleGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OTuple OTupleGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"OTuple",6)==0) )
    {
      int        status;
      OTuple     identifier;
      identifier = (OTuple)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return (OTuple)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,pos+1);
	}
      else
	{
	  if(OTupleIsValid(identifier)==0)
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
      return (OTuple)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
    }
}
/***************************************************************************/
static int GetIdentifier (
 OTuple  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OTupleGetName(This),a_name);
}
/***************************************************************************/
void OTupleDelete (
 OTuple This 
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
/***************************************************************************/
/***************************************************************************/
OTuple OTupleMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OTuple       This;
/*.........................................................................*/
  This            = (OTuple)CMemoryAllocateBlock (sizeof(OTupleRecord));
  if(This==NULL)      return NULL;
  CListAddEntry    ((CList*)&(Class.extent),This);
  This->name      = NULL;
  This->fname     = NULL;
  This->title     = NULL;
  This->labels    = NULL;
  This->labeln    = 0;
  This->cols      = 0;
  This->rows      = 0;
  This->crow      = 0;
  This->valuen      = 0;
  This->values      = NULL;

  return           This;
}
/***************************************************************************/
OTuple OTupleCreate (
 char* a_name 
,int a_cols 
,int a_rows 
,char* a_title  
,int a_labeln
,char** a_labels
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OTuple       This;
/*.........................................................................*/
  This         = OTupleMake ();
  if(This==NULL)   return NULL;

  This->name   = CStringDuplicate(a_name);
  This->fname  = NULL;
  This->title  = ( (a_title==NULL) || (*a_title=='\0')) ? 
                 CStringDuplicate("untitled"):CStringDuplicate(a_title);
  if(a_labels==NULL) {  
    int count;
    This->labels    = NULL;
    This->labeln    = 0;
    for(count=0;count<a_cols;count++)
      CTextAddLine (&This->labeln,&This->labels,"dummy");
  } else {
    This->labels    = CTextDuplicate(a_labeln,a_labels,&This->labeln);
  }
  CTextTrim        (This->labeln,This->labels);

  This->cols       = a_cols;
  This->rows       = a_rows;
  This->crow       = 0;
  This->valuen       = a_cols * a_rows;
  This->values       = NULL;
  if(This->valuen==0) return This;
  This->values       = (double*)CMemoryAllocateBlock(This->valuen*sizeof(double));
  if(This->values==NULL)  This->valuen = 0;
  return           This;
}
/***************************************************************************/
int OTupleReadFileHeader (
 char*   a_fname 
,char**  a_name
,int*    a_coln
,char*** a_labels
,char*** a_formats
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     count;
  FILE*   file;
  char*   fname;
  int     foundTitle   = 0;
  int     foundLabels  = 0;
  int     foundFormats = 0;
  char*   strloop;
  char*   title   = NULL;
  int     coln    = 0;
  int     labeln  = 0;
  char**  labels  = NULL;
  int     formatn = 0;
  char**  formats = NULL;
  int     status  = 0;
/*.........................................................................*/
  if(a_name!=NULL)    *a_name    = NULL;
  if(a_coln!=NULL)    *a_coln    = 0;
  if(a_labels!=NULL)  *a_labels  = NULL;
  if(a_formats!=NULL) *a_formats = NULL;
  file = CFileOpenForReading (a_fname,&fname);
  if(file!=NULL) {
    CStreamInitializeBuffer (file);
    while((strloop=CStreamGetString(file))!=NULL) {
      if(foundTitle==0) { /*First line : title. Mendatory.*/
	char**      words = NULL;
	int         wordn = 0;
	words       = GetStrings (strloop,&wordn);
	title       = (wordn!=1) ?  NULL : CStringDuplicate(words[0]);
	CTextDelete (wordn,words);
	foundTitle  = 1;
      } else if(foundLabels==0) { /*Second line : labels. Mendatory. Evaluate column number.*/
	char** words  = NULL;
	int    wordn  = 0;
	words  = GetStrings (strloop,&wordn);
	coln   = wordn;
	for(count=0;count<coln;count++) {
	  if(*(words[count])=='\0') CTextAddLine(&labeln,&labels,"dummy");
	  else                      CTextAddLine(&labeln,&labels,words[count]);
	}
	CTextDelete (wordn,words);
	foundLabels = 1;
      } else if(foundFormats==0) { /*Third line : formats. Optionnal.*/
	char** words   = NULL;
	int    wordn   = 0;
	words  = CStringGetWords (strloop," ",&wordn);
	if(wordn>=coln) {
	  for(count=0;count<coln;count++) { 
	    if(strchr(words[count],'%')==NULL) {	
	      CTextDelete (formatn,formats);
	      formats     = NULL;
	      formatn     = 0;
	      break;
	    } else {
	      CTextAddLine(&formatn,&formats,words[count]);
	    }
	  }
	}
	CTextDelete(wordn,words);
	foundFormats = 1;
      }    
      if( (foundTitle==1) && (foundLabels==1) && (foundFormats==1) ) break; 
    }
  }
  CStringDelete           (fname);
  if( (title==NULL) || (*title=='\0') ) {
    CWarn ("OTupleReadFileHeader : bad title\n");
  } else if(coln==0) {
    CWarn ("OTupleReadFileHeader : no labels given\n");
  } else {
    status = 1;
  }
  if(a_name!=NULL)    *a_name    = title;
  if(a_coln!=NULL)    *a_coln    = coln;
  if(a_labels!=NULL)  *a_labels  = labels;
  if(a_formats!=NULL) *a_formats = formats;
  fclose (file);
  return status;
}
/***************************************************************************/
OTuple OTupleLoadFile (
  char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    status,coln; 
  char*  title;
  char** labels  = NULL;
  char** formats = NULL;
  OTuple otuple  = NULL;
/*.........................................................................*/
  status = OTupleReadFileHeader (a_fname,&title,&coln,&labels,&formats);
  if(status==0) {
    CWarnF("OTupleLoadFile : %s file has a bad header.\n",a_fname);
  } else { /*Create OTuple.*/
    char* fname;
    FILE* file;
    file  = CFileOpenForReading (a_fname,&fname);
    if(file!=NULL) {
      int                rown;
      CFileTrimDirectory (fname);
      CFileTrimType      (fname);
      rown               = 1000;   
      otuple             = OTupleCreate (fname,coln,rown,title,coln,labels);
      OTupleSetFileName  (otuple,a_fname);
      if(otuple!=NULL) { /*Read data.*/
	char*  strloop;
	int    skipn = 3;
	int    count = 0;
	char** words = NULL;
	int    wordn = 0;
	int    rowi  = 0;
	int    coli;
        if(formats==NULL) skipn = 2;
	CStreamInitializeBuffer (file);
	while((strloop=CStreamGetString(file))!=NULL) {
	  count++;
	  if(count<=skipn) continue;	  
	  words = CStringGetWords (strloop," ",&wordn);
	  if(coln!=wordn) {
	    CWarnF       ("OTupleLoadFile : bad item number (%d expected) in line :\n%s\n",coln,strloop,coln);
	    OTupleDelete (otuple);
	    otuple       = NULL;
	    break;
	  }
	  if(rowi>=rown) {
	    OTupleAddRows (otuple,100);
	    rown         += 100;
	  }
	  for(coli=0;coli<coln;coli++) { 
	    if(sscanf (words[coli],"%le",&TVAL(otuple,rowi,coli))!=1) {
	      CWarnF       ("OTupleLoadFile : %s not a number in :\n%s\n",words[coli],strloop);
	      OTupleDelete (otuple);
	      otuple       = NULL;
	      break;
	    }
          }
	  rowi++; /*One line added.*/
	  CTextDelete (wordn,words);	  
	  if(otuple==NULL) break;
	}
	OTupleLimitRows (otuple,rowi);
      }
      fclose        (file);
    }
    CStringDelete (fname);
  }
  CStringDelete (title);
  CTextDelete   (coln,labels);
  CTextDelete   (coln,formats);
  return        otuple;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OTupleIsValid (
 OTuple This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
char* OTupleGetName (
 OTuple This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OTupleSetName (
 OTuple This 
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->name);
  This->name    = CStringDuplicate(a_name);
}
/***************************************************************************/
void OTupleSetTitle (
 OTuple This 
,char* a_title
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->title);
  This->title  = ( (a_title==NULL) || (*a_title=='\0')) ? 
                 CStringDuplicate("untitled"):CStringDuplicate(a_title);
}
/***************************************************************************/
char* OTupleGetTitle (
 OTuple This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->title;
}
/***************************************************************************/
void OTupleInitialize (
 OTuple This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
/*.........................................................................*/
  if(This==NULL)          return;
  if(This->values==NULL)  return;
  for(count=0;count<This->valuen;count++) This->values[count] = 0.;
}
/***************************************************************************/
void OTupleSetIthColumnLabel (
 OTuple This 
,int a_col 
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_name==NULL) return;
  if(OTupleVerifyColumnIndex(This,a_col)==0) return;
  CStringDelete       (This->labels[a_col]);
  This->labels[a_col] = (*a_name!='\0') ? CStringDuplicate(a_name) : CStringDuplicate("dummy");
  CStringTrim         (This->labels[a_col]);
}
/***************************************************************************/
void OTupleSetLabels (
 OTuple This 
,int    a_linen
,char** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)               return;
  if(a_linen!=This->cols) {CWarnF("%d labels expected.\n",This->cols);return;}
  CTextDelete      (This->labeln,This->labels);
  This->labels   = NULL;
  This->labeln   = 0;
  This->labels   = CTextDuplicate (a_linen,a_lines,&This->labeln);
}
/***************************************************************************/
static void FreeRecord (
 OTuple This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete    (This->name);
  CStringDelete    (This->fname);
  CStringDelete    (This->title);
  CTextDelete      (This->labeln,This->labels);
  This->name       = NULL;
  This->fname      = NULL;
  This->title      = NULL;
  This->labels     = NULL;
  This->labeln     = 0;

  This->rows       = 0;
  This->cols       = 0;
  This->crow       = 0;

  CMemoryFreeBlock (This->values);
  This->valuen     = 0;
  This->values     = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void OTupleClear (
 OTuple This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return;
  FreeRecord           (This);
}
/***************************************************************************/
int OTupleIsEmpty (
 OTuple This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 1;
  return (This->values==NULL ? 1 : 0);
}
/***************************************************************************/
void OTupleCopy (
 OTuple This
,OTuple a_from
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_from==NULL)   return;
  if(a_from==This)   return;

  FreeRecord           (This);

  This->fname        = CStringDuplicate(a_from->fname);
  This->title        = CStringDuplicate(a_from->title);
  This->labels       = CTextDuplicate (a_from->labeln,a_from->labels,&This->labeln);

  This->rows         = a_from->rows;
  This->cols         = a_from->cols;
  This->crow         = a_from->crow;

  This->valuen        = a_from->valuen;
  if(a_from->values!=NULL)
    {
      This->values         = (double*)CMemoryAllocateBlock(a_from->valuen*sizeof(double));
      for(count=0;count<This->valuen;count++) 
        {
          This->values[count] = a_from->values[count];      
        }
    }

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OTupleFillRow (
 OTuple This 
,double* a_data 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
/* assume This exist */
  if(This->crow>=This->rows) OTupleAddRows (This,100);
  for(count=0;count<This->cols;count++)
     TVAL(This,This->crow,count) = a_data[count];
  This->crow++;
}
/***************************************************************************/
void OTupleFillRowWithFloats (
 OTuple This 
,float* a_data 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
/* assume This exist */
  if(This->crow>=This->rows) OTupleAddRows (This,100);
  for(count=0;count<This->cols;count++)
    TVAL(This,This->crow,count) = (double)(a_data[count]);
  This->crow++;
}
/***************************************************************************/
void OTupleEndFilling (
 OTuple This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OTupleLimitRows (This,This->crow);
}
/***************************************************************************/
void OTupleSetColumns (
 OTuple This
,int a_cols 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)              return;
  if(a_cols<0)           return;
  if(a_cols==This->cols) return;

  CTextDelete      (This->labeln,This->labels);
  This->labels   = NULL;
  This->labeln   = 0;
  for(count=0;count<a_cols;count++) CTextAddLine (&This->labeln,&This->labels,"dummy");

  This->cols      = a_cols;
  This->crow      = 0;

  This->valuen     = This->cols * This->rows;
  CMemoryFreeBlock        (This->values);
  This->values      = NULL;
  if(This->valuen==0) return;
  This->values      = (double*)CMemoryAllocateBlock(This->valuen*sizeof(double));
  if(This->values==NULL) This->valuen = 0;
}
/***************************************************************************/
void OTupleSetRows (
 OTuple This 
,int a_rows 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_rows<0) return;
  This->rows     = a_rows;
  This->valuen     = This->cols * This->rows;
  if(This->valuen==0) 
    {
      CMemoryFreeBlock(This->values);
      This->values = NULL;
      return;
    }
  This->values = This->values!=NULL ? 
                 (double*)CMemoryChangeBlockSize(This->values,This->valuen*sizeof(double)) :
                 (double*)CMemoryAllocateBlock(This->valuen*sizeof(double));
  if(This->values==NULL) This->valuen = 0;
/* new rows not inititalised */
}
/***************************************************************************/
void OTupleAddRows (
 OTuple This 
,int a_rows 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_rows<=0) return;
  This->rows     += a_rows;
  This->valuen     = This->cols * This->rows;
  if(This->valuen==0) 
    {
      CMemoryFreeBlock(This->values);
      This->values = NULL;
      return;
    }
  This->values = This->values!=NULL ? 
                 (double*)CMemoryChangeBlockSize(This->values,This->valuen*sizeof(double)) :
                 (double*)CMemoryAllocateBlock(This->valuen*sizeof(double));
  if(This->values==NULL) This->valuen = 0;
/* new rows not inititalised */
}
/***************************************************************************/
void OTupleLimitRows (
 OTuple This 
,int a_rows 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)              return;
  if(a_rows<0)           return;
  if(a_rows>This->rows)  return;
  if(a_rows==This->rows) return;
  This->rows      = a_rows;
  This->valuen     = This->cols * This->rows;
  if(This->valuen==0) 
    {
      CMemoryFreeBlock(This->values);
      This->values = NULL;
      return;
    }
  This->values = This->values!=NULL ? 
                 (double*)CMemoryChangeBlockSize(This->values,This->valuen*sizeof(double)) :
		 (double*)CMemoryAllocateBlock(This->valuen*sizeof(double));
  if(This->values==NULL) This->valuen = 0;
/* new rows not inititalised */
}
/***************************************************************************/
void OTupleFill (
 OTuple This 
,int a_row 
,int a_col 
,double a_data 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if((a_row<0)||(a_row>=This->rows))
    {
      ReportError ("OTupleFill",CErrorFlow);
      return;
    }
  else if((a_col<0)||(a_col>=This->cols)) 
    {
      ReportError ("OTupleFill",CErrorFlow);
      return;
    }
  TVAL(This,a_row,a_col) = a_data;
}
/***************************************************************************/
double OTupleGetValue (
 OTuple This 
,int a_row 
,int a_col 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return DVOID__POS;
  if((a_row<0)||(a_row>=This->rows))
    {
      ReportError ("OTupleGetValue",CErrorFlow);
      return DVOID__POS;
    }
  else if((a_col<0)||(a_col>=This->cols)) 
    {
      ReportError ("OTupleGetValue",CErrorFlow);
      return DVOID__POS;
    }
  return (double)TVAL(This,a_row,a_col);
}
/***************************************************************************/
double OTupleGetValueCareless (
 OTuple This 
,int a_row 
,int a_col 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (double)TVAL(This,a_row,a_col);
}
/***************************************************************************/
int OTupleGetColumnIndex (
 OTuple This 
,char*   a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return NotFound;
  if(a_name==NULL) return NotFound;
  for(count=0;count<This->labeln;count++)
    {  char* label;
       label = This->labels[count];
       if( (label!=NULL) && (strcmp(a_name,label)==0) ) return count;
    }
  return NotFound;
}
/***************************************************************************/
void OTupleSave (
 OTuple This 
,char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int row,col;
  FILE* file;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_fname==NULL) return;
  file      = CFileOpenForWriting  (a_fname);
  if(file==NULL) return;
  if(fprintf(file,"\"%s\"\n",This->name!=NULL  ? This->name : "nil")<0) 
    ReportError ("OTupleWrite",CErrorIO);

  for(col=0;col<This->labeln;col++) 
    {
      if(fprintf(file,"\"%s\"\n",This->labels[col])<0) 
	ReportError ("OTupleWrite",CErrorIO);
    }
  for(row=0;row<This->rows;row++) 
    {
      for(col=0;col<This->cols;col++) 
        if(fprintf (file,"%g ",TVAL(This,row,col))<0) 
	  ReportError ("OTupleWrite",CErrorIO);
      if(fprintf (file,"\n")<0) 
	ReportError ("OTupleWrite",CErrorIO);
    }
  fclose (file);
}
/***************************************************************************/
void OTupleDump (
 OTuple This 
,int    a_row  
,int    a_rows 
,int    a_col  
,int    a_cols 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int row,col;
  int nrow,ncol;
/*.........................................................................*/
  if(This==NULL) return;
  if(OTupleVerifyRowIndex    (This,a_row)==0) a_row = 0;
  if(OTupleVerifyColumnIndex (This,a_col)==0) a_col = 0;
  if(a_rows<0)                        a_rows = This->rows;
  if(a_cols<0)                        a_cols = This->cols;

  nrow = MINIMUM(a_row+a_rows,This->rows);
  ncol = MINIMUM(a_col+a_cols,This->cols);

  CDumpF("\"%s\"\n",This->name!=NULL  ? This->name : "(nil)");
  if(This->title!=NULL) CDumpF("\"%s\"\n",This->title);
  for(col=a_col;col<ncol;col++)
    CDumpF("\"%s\" ",This->labels[col]);
  CDump("\n");
  for(row=a_row;row<nrow;row++)
    { 
      for(col=a_col;col<ncol;col++)
        CDumpF("%g ",(double)TVAL(This,row,col));
      CDump ("\n");
    }
}
/***************************************************************************/
int OTupleVerifyColumnIndex (
 OTuple This 
,int  a_col 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  if((a_col<0)||(a_col>=This->cols)) return 0;
  return   1;
}
/***************************************************************************/
int OTupleVerifyRowIndex (
 OTuple This 
,int  a_row 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  if((a_row<0)||(a_row>=This->rows)) return 0;
  return   1;
}
/***************************************************************************/
void OTupleSetFileName (
 OTuple This   
,char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete (This->fname);
  This->fname   = CStringDuplicate(a_fname);
}
/***************************************************************************/
char* OTupleGetFileName (
 OTuple This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->fname;
}
/***************************************************************************/
char** OTupleGetLabels (
 OTuple This   
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)     return NULL;
  if(a_number!=NULL) *a_number    = This->labeln;
  return             This->labels;
}
/***************************************************************************/
char* OTupleGetIthLabel (
 OTuple This   
,int a_col  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  if(OTupleVerifyColumnIndex(This,a_col)==0) return NULL;
  return This->labels[a_col];
}
/***************************************************************************/
void OTupleGetColumnRange (
 OTuple This   
,int a_col  
,double* a_min 
,double* a_max 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  double val;
/*.........................................................................*/
  if(a_max!=NULL) *a_max = 0.;
  if(a_min!=NULL) *a_min = 0.;
  if(This==NULL) return;
  if(OTupleVerifyColumnIndex (This,a_col)==0) return;
  if(This->rows<=0) return;
  if(a_max!=NULL) *a_max = TVAL(This,0,a_col);
  if(a_min!=NULL) *a_min = TVAL(This,0,a_col);
  for (count=1;count<This->rows;count++) 
    {
      val        = TVAL(This,count,a_col);
      if(a_max!=NULL) *a_max = MAXIMUM (*a_max,val);
      if(a_min!=NULL) *a_min = MINIMUM (*a_min,val);
    }
}
/***************************************************************************/
int OTupleGetNumberOfRows (
 OTuple This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->rows;
}
/***************************************************************************/
int OTupleGetNumberOfColumns (
 OTuple This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->cols;
}
/***************************************************************************/
double* OTupleGetRowPointer (
 OTuple This   
,int a_row
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)                           return NULL;
  if(OTupleVerifyRowIndex (This,a_row)==0) return NULL;
  return                           &(This->values[a_row * This->cols]);
}
/***************************************************************************/
char* OTupleGetInformations (
 OTuple This   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
  char* labels;
/*.........................................................................*/
  string    = (char*)CMemoryAllocateBlock( 1024 *sizeof(char));  
  if(string==NULL) return NULL;
  string[0] = '\0';
  if(This==NULL) return string;
  labels  = CTextConvertToString  (This->labeln,This->labels,"\n");
/* bug on HP when dumping labels for gaccord */
  sprintf(string,
"  name   : %s\n\
  cols   : %d\n\
  rows   : %d\n\
  title  : %s\n\
  file   : %s\n\
 size(Mb): %g",
                       This->name!=NULL  ? This->name : "(nil)",
                       This->cols,
                       This->rows,
                       This->title!=NULL ? This->title : "(nil)",
                       This->fname!=NULL ? This->fname : "(nil)",
                       ((This->rows * This->cols * 8))/1000000.0);

  CMemoryFreeBlock (labels);

  return string;
}
/***************************************************************************/
int OTupleGetColumnIndexFromString (
 OTuple This 
,char* a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int status;
  int icol;
/*.........................................................................*/
  if(a_value==NULL) return NotFound;
  icol = (int) CStringConvertToLong (a_value,&status);
  if(status==0)
    icol = OTupleGetColumnIndex (This,a_value);
  else
    if(OTupleVerifyColumnIndex (This,icol)==0) icol = NotFound;
  return icol;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OTupleGetAttribute (
 OTuple  This
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

       if(strcmp(a_name,"identifier")==0)       *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"name")==0)             *((char**)a_addr)  = This->name;
  else if(strcmp(a_name,"title")==0)            *((char**)a_addr)  = This->title;
  else if(strcmp(a_name,"fileName")==0)         *((char**)a_addr)  = This->fname;
  else if(strcmp(a_name,"numberOfColumns")==0)  *((int*)a_addr)    = This->cols;
  else if(strcmp(a_name,"numberOfRows")==0)     *((int*)a_addr)    = This->rows;
  else if(strcmp(a_name,"labels")==0)     
    {
      int     stringn = 0;
      char**  strings = NULL;
      strings = CTextDuplicate(This->labeln,This->labels,&stringn);
      if(a_number!=NULL)  *a_number = stringn;
      *((OString**)a_addr) = strings;
      return FREE_BLOCKS;
    }
  else if(strcmp(a_name,"values")==0) 
    {
      int     number = 0;
      double* doubles;
      doubles = NULL;
      if(This->valuen!=0)
        {
	  doubles = (double*)CMemoryAllocateBlock( This->valuen * sizeof(double));
	  if(doubles!=NULL)
            {
	        int count;
	      number    = This->valuen;
	      for(count=0;count<number;count++) doubles[count] = This->values[count];
	    }
	}
      if(a_number!=NULL) *a_number = number;
      *((double**)a_addr) = doubles;
      return FREE_BLOCK;
    }
  else 
    {
      CInfoF ("OTupleGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OTupleSetAttribute (
 OTuple  This
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

       if(strcmp(a_name,"identifier")==0)        {}
  else if(strcmp(a_name,"name")==0)              OTupleSetName      (This,*((char**)a_addr));
  else if(strcmp(a_name,"title")==0)             OTupleSetTitle     (This,*((char**)a_addr));
  else if(strcmp(a_name,"fileName")==0)          OTupleSetFileName  (This,*((char**)a_addr));
  else if(strcmp(a_name,"numberOfColumns")==0)   OTupleSetColumns   (This,*((int*)a_addr));
  else if(strcmp(a_name,"numberOfRows")==0)      OTupleSetRows      (This,*((int*)a_addr));
  else if(strcmp(a_name,"labels")==0)            OTupleSetLabels    (This,a_number,*((OString**)a_addr));
  else if(strcmp(a_name,"values")==0) 
    {
      if( (a_number!=0) && (This->valuen!=0) )
        {
	  if(a_number!=This->valuen)
            {
              CWarnF
 ("OTupleSetAttribute: for %s data number mismatch:\ngiven:%d requested:%d.\n",
  This->name!=NULL ? This->name : "(nil)",a_number,This->valuen);
	      return 0;
	    }
	  else
            {
	        int count;
	      double*   list;
	      list      = *((double**)a_addr);
	      for(count=0;count<a_number;count++) This->values[count] = list[count];
	    }
	}
    }
  else 
    {
      CInfoF ("OTupleSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int OTupleDoMethod (
 OTuple This
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
      CInfoF("OTupleDoMethod: %s is not a class routine property.\n",a_name);
      return 0;
    }

  if(strcmp(a_name,"getInformations")==0)
    {
      *((OString*)a_addr) = OTupleGetInformations (This);
      return              FREE_BLOCK;
    }
  else 
    {
      CInfoF ("OTupleDoMethod: %s is not a routine property.\n",a_name);
      return 0;
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  a_addr = NULL;
  return 1;
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
static char** GetStrings (
 char* a_string 
,int* a_number
)
/***************************************************************************/
/* a_string = " fjalsd " " fsfs "                                          */
/*  free with: CTextDelete                                             */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned  int count,iline,nline;
  char*   string;
  char*   token;
  size_t  length;
  char**  list;
  int     in_string;
/*.........................................................................*/
  if(a_number!=NULL) *a_number      = 0;
  if(a_string==NULL)  return NULL;
  length         = strlen(a_string);
  if(length<=0)  return NULL;

  string         = CStringDuplicate(a_string);
  if(string==NULL)    return NULL;

  nline      = 16;
  list       = (char**)CMemoryAllocateBlock(nline*sizeof(char*));
  if(list==NULL)  return NULL;
  iline      = 0;

  token      = NULL;
  in_string  = 0;
  for(count=0;count<length;count++) 
    { char ch;
      ch   = string[count];
      if( (ch=='"') && (in_string==0) )
        {
          in_string = 1;
          token     = string + count + 1;
        }
      else if( (ch=='"') && (in_string==1) )
        { 
          in_string       = 0;
          string[count]   = '\0';
          if(token!=NULL) 
            {
              if(iline>=nline)  
                {
                  nline    += 16; 
                  list      = (char**)CMemoryChangeBlockSize(list,nline*sizeof(char*));
                  if(list==NULL) return NULL;
                }
              list[iline]      = (char*)token;
              iline++;
            }
          token           = NULL;
        }
    }

  for(count=0;count<iline;count++) list[count] = CStringDuplicate(list[count]);
  CMemoryFreeBlock (string);

  if(iline==0)  
    {
      CMemoryFreeBlock  (list);
      if(a_number!=NULL) *a_number = 0;
      return    NULL;
    }
  else
    {
      if(a_number!=NULL) *a_number = iline;
      return    list;
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
  CErrorHandle ("Ho","OTuple",a_name,a_code);
}
