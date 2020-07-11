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
#include <stdlib.h> /*for getenv*/

#include <CMemory.h>
#include <CCharacter.h>
#include <CError.h>
#include <CPrinter.h>
#include <CSystem.h>

#include <CString.h>

#ifdef __cplusplus
extern "C"{
#endif
static char* PrintV         (char*,va_list);
static char* Concatenate    (char*,char*);
static char* Substitute     (char*,char*,char*,int*);
static char* ChangeWord     (char*,char*,char*,int*);
static void  Copy           (char*,char*,int);
static void  ReportError    (char*,int);
#ifdef __cplusplus
}
#endif

#define MAX_STR    2048
static struct {
  char* string;
} Class = {NULL};
/***************************************************************************/
void CStringClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock      (Class.string);
  Class.string = NULL;
}
/***************************************************************************/
char* CStringDuplicate (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
  int   length;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  length           = strlen(This);
  string           = (char*)CMemoryAllocateBlock((length+1)*sizeof(char));
  if(string==NULL) return NULL;
  string[length]   = '\0';
  return           strcpy(string,This);
}
/***************************************************************************/
char* CStringCreate (
 int a_length
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
   int count;
/*.........................................................................*/
  if(a_length<=0) a_length = 0;
  string      = (char*)CMemoryAllocateBlock((a_length+1)*sizeof(char));
  if(string==NULL) return NULL;
  string[a_length] = '\0';
  for(count=0;count<a_length;count++) string[count] = ' ';
  return      string;
}
/***************************************************************************/
void CStringDelete (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock (This);
}
/***************************************************************************/
int CStringCompare (
 char* This
,char* a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL) && (a_to==NULL) ) return 1; 
  if( (This!=NULL) && (a_to==NULL) ) return 0; 
  if( (This==NULL) && (a_to!=NULL) ) return 0; 
  if( This ==  a_to) return 1; 
  return (strcmp(This,a_to)==0 ? 1: 0); 
}
/***************************************************************************/
int CStringHasOnlySpaces (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
/*.........................................................................*/
  if(This==NULL) return 1;
  for(itema=This;*itema!='\0';itema++)
    {
      if((*itema)!=' ') return 0;
    }
  return 1;
}
/***************************************************************************/
char* CStringGetFirstWord (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
  int   lspace;
  char* pos;
/*.........................................................................*/
  if(This==NULL) return NULL;
  lspace        = strspn(This," ");
  string        = CStringDuplicate(This+lspace);
  pos           = strchr(string,' ');
  if(pos!=NULL) *pos = '\0';
  return        string;
}
/***************************************************************************/
char* CStringGetSecondWord (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
  int   lspace;
  char* string;
/*.........................................................................*/
  if(This==NULL) return NULL;

  pos      = This;

  lspace   = strspn(pos," ");
  pos      = strchr(pos+lspace,' ');
  if(pos==NULL) return NULL;

  lspace   = strspn(pos," ");
  string   = CStringDuplicate(pos+lspace);
  pos      = strchr(string,' ');
  if(pos!=NULL)  *pos = '\0';

  return   string;
}
/***************************************************************************/
char* CStringGetThirdWord (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
  int   lspace;
  char* string;
/*.........................................................................*/
  if(This==NULL) return NULL;

  pos      = This;

  lspace   = strspn(pos," ");
  pos      = strchr(pos+lspace,' ');
  if(pos==NULL) return NULL;

  lspace   = strspn(pos," ");
  pos      = strchr(pos+lspace,' ');
  if(pos==NULL) return NULL;

  lspace   = strspn(pos," ");
  string   = CStringDuplicate(pos+lspace);
  pos      = strchr(string,' ');
  if(pos!=NULL)  *pos = '\0';

  return   string;
}
/***************************************************************************/
int CStringIsWord (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
/*.........................................................................*/
  if(This==NULL)  return 0;
  if(*This=='\0') return 0; /*0 length string*/
  if(CCharacterIsDigit(This[0])==1) return 0;  /* do not accecpt word starting by a digit */
  for(itema=This;*itema!='\0';itema++)
    {
       if( (CCharacterIsDigit (*itema)==0)       &&
           (CCharacterIsAlphanumeric(*itema)==0) &&
           ((*itema)!='_')  
       )
        return 0;
    }
  return 1;
}
/***************************************************************************/
int CStringRaise (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
  int   changed = 0;
/*.........................................................................*/
  if(This==NULL) return 0;
  for(itema=This;*itema!='\0';itema++)
    { char   c;
      c      = *itema;
      *itema = CCharacterConvertToUpperCase(c);
      if(c!=(*itema)) changed = 1;
    }
  return changed;
}
/***************************************************************************/
int CStringLower (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
  int   changed = 0;
/*.........................................................................*/
  if(This==NULL) return 0;
  for(itema=This;*itema!='\0';itema++)
    { char   c;
      c      = *itema;
      *itema = CCharacterConvertToLowerCase(c);
      if(c!=(*itema)) changed = 1;
    }
  return changed;
}
/***************************************************************************/
int CStringHasUpperCase (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
/*.........................................................................*/
  if(This==NULL) return 0;
  for(itema=This;*itema!='\0';itema++)
    {
      if(CCharacterIsUpperCase(*itema)==1) return 1;
    }
  return 0;
}
/***************************************************************************/
static void Copy (
 char* This
,char* a_from
,int a_length
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_from==NULL) return;
  for(count=0;count<a_length;count++) *(This+count) = *(a_from+count);
}
/***************************************************************************/
void CStringTrim (
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
  for(count=length-1;count>=0;count--)  {if(This[count]!=' ') {pos = count+1;break;}}
  if(pos==-1) This[0]   = '\0'; 
  else        This[pos] = '\0';
}
/***************************************************************************/
char* CStringGetLastNotSpacePosition (
 char* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int  length;
/*.........................................................................*/
  if(This==NULL) return NULL;
  length      = strlen(This);
  for(count=length-1;count>=0;count--)  
    {
      if(This[count]!=' ') 
	{
	  return This+count;
	}
    }
  return NULL;
}
/***************************************************************************/
char* CStringGetFirstNotSpacePosition (
 char* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
/*.........................................................................*/
  if(This==NULL) return NULL;
  for(itema=This;*itema!='\0';itema++)
    {
      if(*itema!=' ') return itema;
    }
  return NULL;
}
/***************************************************************************/
int CStringRemoveDelimiters (
 char* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char s_quote = '\''; /* 39 */
  char d_quote = '"';  /* 34 */
  int  length;
/*.........................................................................*/
  if(This==NULL) return 0;
  length    = strlen(This);
  if( (length >=2)  &&  (
      ( ((This)[0]==s_quote) && ((This)[length-1]==s_quote)) ||
      ( ((This)[0]==d_quote) && ((This)[length-1]==d_quote)) ))
    {  int count;
      for(count=1;count<length-1;count++)
	{
          This[count-1] = This[count];
        }
      This      [length-2] = '\0';
      return    1;
    }
  return 0;
}
/***************************************************************************/
char* CStringCreateF (
 int   a_number
,char* a_format
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  char*    This;
/*.........................................................................*/
  if(a_format==NULL) return NULL;
  va_start (args,a_format);
  This     = CStringCreateV (a_number,a_format,args);
  va_end   (args);
  return   This;
}
/***************************************************************************/
char* CStringCreateV (
 int     a_number
,char*   a_format
,va_list a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*              This = NULL;
/*.........................................................................*/
  if(a_format==NULL) return NULL;
  if(a_number<0)     return NULL;
  This               = CStringCreate(a_number);
  if(This==NULL)     return NULL;
  vsprintf           (This,a_format,a_args);
  if(This[a_number]!='\0') 
    {
      ReportError ("CStringCreateV",CErrorFlow);
    }
  return             This;
}
/***************************************************************************/
char* CStringCreateFormatted (
 char* a_format
,...
)
/***************************************************************************/
/*
  Let for backward compatibilty. Use now CStringCreateF.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list  args;
  char*    This;
/*.........................................................................*/
  if(a_format==NULL) return NULL;
  va_start (args,a_format);
  This     = CStringCreateV (MAX_STR,a_format,args);
  va_end   (args);
  return   This;
}
/***************************************************************************/
static char* PrintV (
 char* a_format 
,va_list a_args
)
/***************************************************************************/
/* 
   DANGEROUS: This routine uses a buffer with a size fixed to 2048 bytes.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_format==NULL)          return NULL;
  if(Class.string==NULL)      
    {
      Class.string = (char*)CMemoryAllocateBlock(MAX_STR * sizeof(char));
      if(Class.string==NULL) return NULL;
    }
  Class.string[MAX_STR-1]     = '\0';
  vsprintf                    (Class.string,a_format,a_args);
  if(Class.string[MAX_STR-1]!='\0') 
    {
      ReportError("CStringPrintSFV",CErrorFlow);
      return NULL;
    }
  return                      Class.string;
}
/***************************************************************************/
int CStringPrintF (
 char* This 
,char* a_format
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list            args;
  int                count;
/*.........................................................................*/
  if(This==NULL)          return 0;
  if(a_format==NULL)      return 0;
#ifdef COSPRINTF     /*some system have non ansi definition of sprintf */
  va_start           (args,a_format);
  PrintV             (a_format,args);
  va_end             (args);
  count              = strlen(Class.string);
  strcpy             (This,Class.string);
#else
  va_start           (args,a_format);
  count              = vsprintf (This,a_format,args);
  va_end             (args);
#endif  
  return             count;
}
/***************************************************************************/
int CStringMatchPattern (
 char* This   
,char* a_pattern 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int      wcount;
  int      lpattern,lstring;
  char*    string;
  char*    token;
  int      ok,match;
  int      wnumber;
#define MAX_WORDS 100
  char*    words[MAX_WORDS]; 
/*.........................................................................*/
  if ( (a_pattern==NULL) && (This==NULL) ) return 1;
  if ( (a_pattern==NULL) && (This!=NULL) ) return 0;
  if ( (a_pattern!=NULL) && (This==NULL) ) return 0;
  lpattern  = strlen(a_pattern);
  lstring   = strlen(This);
  if ((lpattern==0)&&(lstring==0)) return 1;
  if ((lpattern==0)&&(lstring!=0)) return 1;
  if ((lpattern!=0)&&(lstring==0)) return 0;
/* pattern is * */
  if(strcmp(a_pattern,"*")==0) return 1;
  wcount = 0;
  for(count=0;count<lpattern;count++) {if(a_pattern[count]=='*') wcount++;}
/* no wildcard */
  if(wcount==0)
    {
      return (strcmp(a_pattern,This)==0 ? 1 : 0 );
    }

/* complex pattern */
  token    = string = CStringDuplicate(a_pattern);
  wnumber  = 0;
  while(1)
    { char* pos;
      pos   = strstr (token,"*");
      if(pos!=NULL)
        {
          *pos = '\0';
          if(*token!='\0') 
            {
              if(wnumber>=MAX_WORDS) {ReportError("CStringMatchPattern",CErrorFlow);} 
              else                   {words[wnumber] = token;wnumber++;}
            }
        token = pos + 1;
        }
      else /*last word*/
        {
          if(*token!='\0') 
            {
              if(wnumber>=MAX_WORDS) {ReportError("CStringMatchPattern",CErrorFlow);} 
              else                   {words[wnumber] = token;wnumber++;}
            }
          break;
        }
    }
/* check that at least one word is not empty */
  ok = 0;
  for(count=0;count<wnumber;count++)
    { 
      if(*(words[count])!='\0') {ok = 1;break;}
    }
  if(ok==0) {CStringDelete(string);return 1;} /* only wildcards */

/* loop on words */
  match    = 1;
  token    = This;
  for(count=0;count<wnumber;count++)
    { int   lword;
      lword = strlen(words[count]); 
      if(lword>0) 
        { 
	  char* pos;
	  if(count==0)
	    {
	      if(a_pattern[0]!='*') /*Begin of pattern (words[0]) and This must match.*/
		{
		  if(strncmp(token,words[count],lword)!=0) 
		    {
		      match = 0; /*Different.*/
		      break;      
		    }
		  token = token + lword;
		  continue;
		}
	    }
	  pos                = strstr (token,words[count]);
	  if(pos==NULL)      {match=0;break;}
	  if((count==(wnumber-1)) && (a_pattern[lpattern-1]!='*') ) /*Last word.*/
	    {
	      if(strcmp(This+lstring-lword,words[count])!=0) match = 0; /*Compare last word and end of This.*/
	      break;
	    }
	  else
	    {
	      token = pos + lword;
	    }
        }
    }
  CStringDelete (string);
/*printf ("debug:match:%d:%s|%s|\n",match,This,a_pattern);*/
  return        match;
}
/***************************************************************************/
int CStringIsPrintable (
 char* This 
,char* a_char 
,int* a_pos 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
  int   pos;
/*.........................................................................*/
  if(a_char!=NULL) *a_char   = '\0';
  if(a_pos!=NULL)  *a_pos    = 0;
  if(This==NULL) return 1;
  pos   = 0;
  for(itema=This;*itema!='\0';itema++)
    {
      if(CCharacterIsPrintable(*itema)==0) 
        { 
          if(a_char!=NULL) *a_char = *itema;
          if(a_pos!=NULL)  *a_pos  = pos;
          return  0;
        }
      pos++;
    }
  if(a_pos!=NULL) *a_pos  = 0;
  return  1;
}
/***************************************************************************/
void CStringInitialize (
 char* This 
,char a_char 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
/*.........................................................................*/
  if(This==NULL) return;
  for(itema=This;*itema!='\0';itema++)
    {
      *itema = a_char;
    }
}
/***************************************************************************/
int CStringBeginWith (
 char* This 
,char a_char 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
/*.........................................................................*/
  if(This==NULL) return 0;
  for(itema=This;*itema!='\0';itema++)
    {
      if(*itema==' ')    continue;
      if(*itema==a_char) return 1;
      return 0;
    }
  return 0;
}
/****************************************************************************/
int CStringCountLines (
 char* This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* itema;
  int  nline;
/*.........................................................................*/
  if(This==NULL) return 0;
  nline = 1;
  for(itema=This;*itema!='\0';itema++)
    {
      if((*itema)=='\n') nline++;
    }
  return nline;
}
/***************************************************************************/
long CStringConvertToLong (
 char* This
,int* a_status
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  long   value = 0L;
  char*  s;
/*.........................................................................*/
  if(a_status!=NULL) *a_status = 0;
  if(This==NULL)     return 0L;
  value              = strtol(This,&s,10);
  if(s==This)        return 0L;
  if(a_status!=NULL) *a_status = 1;
  return             value;
}
/***************************************************************************/
double CStringConvertToDouble (
 char* This
,int*  a_status
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double value = 0.;
  char*  s;
/*.........................................................................*/
  if(a_status!=NULL) *a_status = 0;
  if(This==NULL)     return 0.;
  value              = strtod(This,&s);
  if(s==This)        return 0.;
  if(a_status!=NULL) *a_status = 1;
  return        value;
}
/***************************************************************************/
int CStringIsLong (
 char* This
,long* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  long   value = 0L;
  char*  s;
/*.........................................................................*/
  if(a_value!=NULL) *a_value = 0L;
  if(This==NULL)    return 0;
  value             = strtol (This,&s,10);
  if(a_value!=NULL) *a_value = value;
  return            (s==This ? 0 : 1);
}
/****************************************************************************/
double* CStringConvertToDoubles (
 char* a_string 
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*   end;
  char*   begin;
  int     number;
  double* list;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_string==NULL) return NULL;
  number = 0;
  list   = NULL;
  begin  = a_string;
  count  = 0;
  while(1)
    {  
      double d;
      d      = strtod(begin,&end);
      if(begin==end) break; 
      if(count>=number)
        {
	  list      = number!=0 ? 
	              (double*)CMemoryChangeBlockSize (list,(number+100) * sizeof(double)) : 
		      (double*) CMemoryAllocateBlock(100*sizeof(double));
          if(list==NULL) return NULL;
	  number   += 100;
	}
      list[count] = d;
      count++;
      begin = end;
    }
  if(a_number!=NULL) *a_number = count;
  return list;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int CStringReplaceWord (
 char** This  
,char* a_old     
,char* a_new     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int status;
/*.........................................................................*/
  *This = ChangeWord (*This,a_old,a_new,&status);
  return status;
}
/***************************************************************************/
static char* ChangeWord (
 char* This  
,char* a_old     
,char* a_new     
,int*  a_status
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** list = NULL;
  char*  cstring;
  char*  string;
  int    lold,lnew;
  char*  nstring;
  char*  ostring;
  int    nlist,length;
/*.........................................................................*/
  if(a_status!=NULL) *a_status = 0;
  if(This==NULL)     return This;
  if(a_old==NULL)    return This;
  if(a_new==NULL)    return This;
  if(*a_old=='\0')   return This;

  lold      = strlen(a_old);

/* get list of a_old position */
  string     = cstring = CStringDuplicate(This);
  nlist      = 0;
  while(1)
    { char               char_bef,char_aft;
      char*              pos;
      pos                = strstr (string,a_old);
      if(pos==NULL)      break;
      char_bef           = (pos==string ? '\0' : *(pos-1));
      char_aft           = *(pos+lold);
      if( (CCharacterIsSeparator(char_bef)==1) && 
	  (CCharacterIsSeparator(char_aft)==1) )
        {
          *pos               = '\0';
          list               = nlist!=0 ? 
	                       (char**)CMemoryChangeBlockSize(list,(nlist+1)*sizeof(char*)) :
			       (char**) CMemoryAllocateBlock(sizeof(char*));
          if(list==NULL)     return This;
          list[nlist]        = pos;
          nlist++;
        }
      string = pos+lold;
    }
  if(nlist==0) {CStringDelete (cstring);return This;}

  lnew             = strlen(a_new);
  length           = strlen(This) + nlist * (lnew-lold);
  string           = nstring = (char*)CMemoryAllocateBlock((length+1)*sizeof(char));
  if(string==NULL) return This;
  string[length]   = '\0';
  ostring          = cstring;
  for(count=0;count<nlist;count++)
    { 
      length   = strlen(ostring);
      strncpy  (string,ostring,length);
      string  += length;
      ostring += length;
      strncpy  (string,a_new,lnew);
      string  += lnew;
      ostring += lold;      
    }
  length           = strlen(ostring);
  strncpy          (string,ostring,length);
  *(string+length) = '\0';
  if(a_status!=NULL) *a_status = 1;

  CMemoryFreeBlock      (list);
  CStringDelete (cstring);

  if(nstring!=This) CMemoryFreeBlock(This);

  return        nstring;
}
/***************************************************************************/
char* CStringGetFirstWordPosition (
 char* This  
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  int    lstring;
/*.........................................................................*/
  if(This==NULL)      return NULL;
  if(a_string==NULL)  return This;
  if(*a_string=='\0') return This;

  lstring    = strlen(a_string);

/* get list of a_string position */
  string     = This;
  while(1)
    { char          char_bef,char_aft;
      char*         pos;
      pos           = strstr (string,a_string);
      if(pos==NULL) return NULL;
      char_bef      = (pos==string ? '\0' : *(pos-1));
      char_aft      = *(pos+lstring);
      if( (CCharacterIsSeparator(char_bef)==1) && 
	  (CCharacterIsSeparator(char_aft)==1) )
        {
          return pos;
        }
      string = pos + lstring;
    }
}
/***************************************************************************/
int CStringHasWord (
 char* This  
,char* a_word     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  cstring;
  char*  string;
  int    lold;
  int    found;
/*.........................................................................*/
  if(This==NULL)     return 0;
  if(a_word==NULL)   return 0;
  if(*a_word=='\0')  return 0;

  lold       = strlen(a_word);

/* get list of a_word position */
  found      = 0;
  string     = cstring = CStringDuplicate(This);
  while(1)
    { char               char_bef,char_aft;
      char*              pos;
      pos                = strstr (string,a_word);
      if(pos==NULL)      break;
      char_bef           = (pos==string ? 0:*(pos-1));
      char_aft           = *(pos+lold);
      if( (CCharacterIsSeparator(char_bef)==1) && 
	  (CCharacterIsSeparator(char_aft)==1) )
        {
          *pos   = '\0';
	  CInfoF ("%s\n",This);
	  found  = 1;
        }
      string = pos+lold;
    }

  CStringDelete (cstring);
  return        found;
}
/***************************************************************************/
void CStringConcatenate (
 char** This
,char* a_from 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  *This = Concatenate (*This,a_from);
}
/***************************************************************************/
static char* Concatenate (
 char* This
,char* a_from 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_from==NULL) return This;
  if(This==NULL)
    {
      This = CStringDuplicate(a_from);
    }
  else if(*This=='\0')
    {
      CMemoryFreeBlock (This);
      This             = CStringDuplicate(a_from);
    }
  else
    { char*            string;
      int              lfrom,lto,length;
      lto              = strlen(This);
      lfrom            = strlen(a_from);
      length           = lto+lfrom;
      string           = (char *) CMemoryChangeBlockSize (This,(size_t)((length+1)*sizeof(char)));
      if(string==NULL) return This;
      string[length]   = '\0';
      This             = string;
      strncpy          (This+lto,a_from,lfrom);
      This[lto+lfrom]  = '\0';
    }
  return This;
}
/***************************************************************************/
int CStringReplacePart (
 char** This
,char* a_old     
,char* a_new     
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int status;
/*.........................................................................*/
  *This = Substitute (*This,a_old,a_new,&status);
  return status;
}
/***************************************************************************/
static char* Substitute (
 char* This
,char* a_old     
,char* a_new     
,int*  a_status
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int length,lold,lnew;
/*.........................................................................*/
  if(a_status!=NULL) *a_status = 0;
  if(This==NULL)     return This;
  if(a_old==NULL)    return This;
  if(a_new==NULL)    return This;
  if(*This=='\0')  return This;
  if(*a_old=='\0') return This;
  length      = strlen(This);
  lold        = strlen(a_old);
  lnew        = strlen(a_new);
  if(lnew<=lold)
    {  int count;
      count = 0;
      while(1)
        { 
          if(count>length-lold) break;
          if(strncmp(This+count,a_old,lold)!=0)  {count ++;continue;}
	  if(a_status!=NULL) *a_status = 1;
          strncpy          (This+count       ,a_new           ,lnew);
/* overlapping copy 
          strncpy          (This+count+lnew  ,This+count+lold ,length-count-lold);
*/
          Copy             (This+count+lnew  ,This+count+lold ,length-count-lold);
          count           += lnew;
          length          -= lold-lnew; 
          This[length]     = '\0';
        }
      return This;
    }
  else
    {  int count,icount;
      int        lstring;
      char*      string;
      lstring    = length;
      string     = This;
      icount     = 0;
      count      = 0;
      while(1)
        {
          if(count>lstring-lold) break;
          if(strncmp(string+count,a_old,lold)!=0)  {count ++;icount++;continue;}
	  if(a_status!=NULL) *a_status = 1;
          lstring += lnew-lold;
          if(string==This)
            {
              string            = (char *) CMemoryAllocateBlock  ((size_t)((lstring+1)*sizeof(char)));
              if(string==NULL)  return This;
	      string[lstring]   = '\0';
              strncpy           (string ,This ,count);
            }
          else
            {
              string            = (char *) CMemoryChangeBlockSize (string,(size_t)((lstring+1)*sizeof(char)));
              if(string==NULL)  return This;
	      string[lstring]   = '\0';
            }
          strncpy          (string+count       ,a_new            ,lnew);
          strncpy          (string+count+lnew  ,This+icount+lold ,length-icount-lold);
          count           += lnew;
          icount          += lold;
          string[lstring]  = '\0';
        }
      if(string!=This)  CMemoryFreeBlock (This);
      return string;
    }
}
/***************************************************************************/
int CStringConvertToBoolean (
 char* a_string
,int* a_status
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) 
    {
      if(a_status!=NULL) *a_status = 0;
      return 0;
    }
  if(a_status!=NULL) *a_status = 1;
       if(strcmp(a_string,"1")==0)     return 1;
  else if(strcmp(a_string,"true")==0)  return 1;
  else if(strcmp(a_string,"True")==0)  return 1;
  else if(strcmp(a_string,"yes")==0)   return 1;
  else if(strcmp(a_string,"Yes")==0)   return 1;
  else if(strcmp(a_string,"on")==0)    return 1;
  else if(strcmp(a_string,"On")==0)    return 1;

  else if(strcmp(a_string,"0")==0)     return 0;
  else if(strcmp(a_string,"false")==0) return 0;
  else if(strcmp(a_string,"False")==0) return 0;
  else if(strcmp(a_string,"no")==0)    return 0;
  else if(strcmp(a_string,"No")==0)    return 0;
  else if(strcmp(a_string,"off")==0)   return 0;
  else if(strcmp(a_string,"Off")==0)   return 0;
  else 
    {
      if(a_status!=NULL) *a_status = 0;
      return 0;
    }
}
/*****************************************************************************/
char** CStringGetWords (
 char* a_string 
,char* a_limiter 
,int* a_number
)
/***************************************************************************/
/*
  In a_string get tokens between a_limiter.
  Example :
    #include <CString.h>
    #include <CText.h>
    ...
    char** words;
    int    wordn;
    words  = CStringGetWords ("aaa bbb ccc"," ",&wordn);
    if(wordn>=1) printf ("%s\n",words[0]);
    ...
    CTextDelete (wordn,words);
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*  string;
  char*  token;
  int    iline;
  char** list  = NULL;
  int    nline = 0;
/*.........................................................................*/
  if(a_number!=NULL) *a_number      = 0;
  if( (a_string==NULL) || (*a_string=='\0') )  return NULL;
  if(a_limiter==NULL) return NULL;

  string                       = CStringDuplicate(a_string);
  if(string==NULL)             return NULL;

  nline                        = 16;
  list                         = (char**)CMemoryAllocateBlock(nline*sizeof(char*));
  if(list==NULL)               return NULL;
  iline                        = 0;

  token                        = string;
  while(1)
    { char* pos;
      pos = strstr (token,a_limiter);
      if(pos!=NULL)
        {
          *pos = '\0';
          if(*token!='\0') 
            {
              if(iline>=nline) 
                { 
                  nline    +=16;
                  list      = (char**)CMemoryChangeBlockSize(list,nline*sizeof(char*));
                  if(list==NULL) return NULL;
                }
              list[iline]      = token;
              iline++;
            }
          token = pos + strlen(a_limiter);          
        }
      else /*last word*/
        {
          if(*token!='\0') 
            {
              if(iline>=nline) 
                {
                  nline    += 16;
                  list      = (char**)CMemoryChangeBlockSize(list,nline*sizeof(char*));
                  if(list==NULL) return NULL;
                }
              list[iline]      = token;
              iline++;
            }
          break;
        }
    }

  for(count=0;count<iline;count++) list[count] = CStringDuplicate(list[count]);
  CMemoryFreeBlock (string);

  if(iline==0)  
    {
      CMemoryFreeBlock   (list);
      if(a_number!=NULL) *a_number = 0;
      return             NULL;
    }
  else
    {
      if(a_number!=NULL) *a_number = iline;
      return             list;
    }
}
/***************************************************************************/
double CStringGetHersheyLength (
 char*  a_string 
,int    a_greek
,double a_offset
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  itema;
  double length,width = 0.;
#define HERSHEY__MAX_POLY   4
#define HERSHEY__MAX_POINT  160 /* 40 * 4 */
  int    max_point[HERSHEY__MAX_POLY];
  double xp[HERSHEY__MAX_POINT],yp[HERSHEY__MAX_POINT];
  int    number;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return 0.;
  length = 0.;
  for(itema=a_string;*itema!='\0';itema++) { 
    if (a_greek==1)
      CCharacterGetGreekFontPoints  (*itema,&number,max_point,xp,yp,&width);
    else
      CCharacterGetASCII_FontPoints (*itema,&number,max_point,xp,yp,&width);
    length                         += width + a_offset;
  }
  return length;
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
  CErrorHandle ("Co","CString",a_name,a_code);
}
