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
#include <CString.h>
#include <CText.h>
#include <CPrinter.h>
#include <CSystem.h>
#include <CError.h>
#include <CStream.h>

#include <CFile.h>

#ifdef __cplusplus
extern "C"{
#endif
static char* GetEndDirectory    (char*);
static char* GetDirectory       (char*);
static void  GetEnvironment     (char**);
static FILE* OpenFileForReading (char*,char**,char**);
static void  ReportError        (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long        Ulong;

#define MINIMUM(a,b)         ((a)<(b)?a:b)
#define NotFound (-1)

#define MAX_STRING 2048
static struct 
{
  int    vpathn;
  char** vpaths;
  int    pathn;
  char** paths;
  char   name[7];
  char   tmpname[128];
  char   string[MAX_STRING+1];
} Class = {0,NULL,0,NULL,"tmpxxx"};
/***************************************************************************/
void CFileClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CTextDelete  (Class.vpathn,Class.vpaths);
  Class.vpathn = 0;
  Class.vpaths = NULL;
  CTextDelete  (Class.pathn,Class.paths);
  Class.pathn  = 0;
  Class.paths  = NULL;
}
/***************************************************************************/
void CFileAddPath (
 char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(a_name==NULL) return;
  string = GetDirectory (a_name);
  if(CTextGetLineIndex(Class.pathn,Class.paths,string)==NotFound)
    {
      CTextAddAllocatedLine (&Class.pathn,&Class.paths,string);
    }
  else
    {
      CStringDelete (string);
    }
}
/***************************************************************************/
void CFileAddPathVariable (
 char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name==NULL) return;
  if(CTextGetLineIndex(Class.vpathn,Class.vpaths,a_name)!=NotFound) return;
  CTextAddLine (&Class.vpathn,&Class.vpaths,a_name);
}
/***************************************************************************/
int CFileIsValid (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*    fp;
  char*    fname;
/*..... .......................................................................*/
  if(a_fname==NULL) return 0;
  fname          = CStringDuplicate(a_fname);
  GetEnvironment (&fname);
  fp             = fopen(fname,"rb");
  CStringDelete  (fname);
  if(fp==NULL)   return 0;
  fclose         (fp);
  return         1;
}
/***************************************************************************/
int CFileIsValidInPathList (
 char* a_string 
,char** a_fullName
)
/***************************************************************************/
/* 
  Use list of path to search the file.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*        fp;
/*.........................................................................*/
  fp           = OpenFileForReading (a_string,a_fullName,NULL);
  if(fp==NULL) return 0;
  fclose       (fp);
  return       1;
}
/***************************************************************************/
void CFileSafeguard (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*                fname;
  char*                directory;
/*.........................................................................*/
  if(a_fname==NULL) return;
  if(CFileIsValid(a_fname)==0) return;

  directory            = CFileGetDirectory (a_fname);
  fname                = CStringDuplicate  (a_fname);
  CFileTrimDirectory   (fname);

  if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      if(directory==NULL)
	CSystemExecuteF   (strlen(a_fname)+strlen(fname)+7,"cp %s '#%s'",a_fname,fname);
      else
	CSystemExecuteF   (strlen(a_fname)+strlen(directory)+strlen(fname)+7,"cp %s '%s#%s'",a_fname,directory,fname);
    }
  else if(CSystemIsKindOf("NT",NULL)==1)
    {
      if(directory==NULL)
	CSystemExecuteF   (strlen(a_fname)+strlen(fname)+7,"copy %s #%s",a_fname,fname);
      else
	CSystemExecuteF   (strlen(a_fname)+strlen(directory)+strlen(fname)+7,"copy %s %s#%s",a_fname,directory,fname);
    }
  else if(CSystemIsKindOf("VMS",NULL)==1)
    {
      char*           fname;
      char*           spos;
      fname           = CStringDuplicate(a_fname);
      spos            = strrchr(fname,';');
      if(spos!=NULL)  *spos   = '\0';
      CSystemExecuteF (strlen(a_fname)+strlen(fname)+11,"$ copy %s %sold",a_fname,fname);
      CStringDelete   (fname);
    }

  CStringDelete        (directory);
  CStringDelete        (fname);
}
/***************************************************************************/
void CFileSafeguardF (
 int   a_length
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list    args;
  char*      string;
/*..... .......................................................................*/
  if(a_format==NULL) return;
  va_start           (args,a_format);
  string             = CStringCreateV (a_length,a_format,args);
  va_end             (args);
  CFileSafeguard     (string);
  CStringDelete      (string);
}
/***************************************************************************/
FILE* CFileOpenForReading (
 char* a_string 
,char** a_fullName
)
/***************************************************************************/
/* 
  Use list of path to search the file. The file is open in reading mode.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  fp;
  char*  fname;
/*.........................................................................*/
  fp = OpenFileForReading (a_string,a_fullName,&fname);
  if(fp==NULL) 
    {
      CWarnF ("Can't open %s for reading :\n check : path, environment variables, existence, quotas, protections, format.\n",fname);
    }
  CStringDelete (fname);
  return        fp;
}
/***************************************************************************/
FILE* CFileOpenForWritingF (
 int   a_length
,char* a_format 
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list    args;
  char*      string;
  FILE*      fp;
/*..... .......................................................................*/
  if(a_format==NULL) return NULL;
  va_start           (args,a_format);
  string             = CStringCreateV (a_length,a_format,args);
  va_end             (args);
  fp                 = CFileOpenForWriting (string);
  CStringDelete      (string);
  return             fp;
}
/***************************************************************************/
FILE* CFileOpenForWriting (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*   fp;
  char*   fname;
/*..... .......................................................................*/
  if(a_fname==NULL) return NULL;
  fname          = CStringDuplicate(a_fname);
  GetEnvironment (&fname);
  remove         (fname); /*For NT.*/
  fp             = fopen(fname,"wb");
  CStringDelete  (fname);
  if(fp==NULL)
    {
      CWarnF("Can't open %s for writing :\n check : path, environment variables, existence, quotas, protections, format.\n",a_fname);
    }
  return        fp;
}
/***************************************************************************/
FILE* CFileOpenForAppending (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*   fp;
  char*   fname;
/*..... .......................................................................*/
  if(a_fname==NULL) return NULL;
  fname          = CStringDuplicate(a_fname);
  GetEnvironment (&fname);
  fp             = fopen(fname,"ab");
  CStringDelete  (fname);
  if(fp==NULL)
    {
      CWarnF("Can't open %s for appending :\n check : path, environment variables, existence, quotas, protections, format.\n",a_fname);
    }
  return        fp;
}
/***************************************************************************/
char* CFileBuildTemporaryName (
 char* a_dir
)
/***************************************************************************/
/*
  For UNIX a_dir must be a string ended with '/'. Exa : /tmp/ .
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*..... ..................................................................*/
  strcpy (Class.name,"tmpxxx");
  for(count=0;count<1000;count++)
    {
      FILE*    file = NULL;
      sprintf  (Class.name+3,"%03d",count);
      if(a_dir!=NULL) sprintf  (Class.tmpname,"%s%s",a_dir,Class.name);
      else   	      sprintf  (Class.tmpname,"%s",Class.name);
      file            = fopen (Class.tmpname,"rb");
      if(file==NULL)  return Class.tmpname; /*File does not exists.*/
      fclose (file);
    }
  return           NULL;
}
/***************************************************************************/
FILE* CFileOpenTemporaryForWriting (
 char*  a_dir
,char** a_name
)
/***************************************************************************/
/*
  For UNIX a_dir must be a string ended with '/'. Exa : /tmp/ .
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
  FILE* file;
/*..... .......................................................................*/
  if(a_name!=NULL) *a_name = NULL;
  name             = CFileBuildTemporaryName(a_dir);
  if(name==NULL)   return NULL;
  remove           (name); /*For NT.*/
  file             = fopen (name,"wb+");
  if(a_name!=NULL) *a_name = name;
  return           file;
}
/***************************************************************************/
char** CFileLoadText (
 char* a_fname
,int* a_linen
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  file;
  char** lines;
/*.........................................................................*/
  if(a_linen!=NULL) *a_linen      = 0;
  file           = CFileOpenForReading (a_fname,NULL);
  if(file==NULL) return NULL; 
  lines          = CStreamGetText (file,a_linen);
  fclose         (file);
  return         lines;
}
/***************************************************************************/
void CFileSaveText (
 char* a_fname
,int a_linen
,char** a_lines 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  file;
/*.........................................................................*/
  if(a_fname==NULL) return;
  remove           (a_fname); /*For NT.*/
  file              = fopen (a_fname,"wb");
  CStreamPutText    (file,a_linen,a_lines);
  if(file!=NULL)    fclose (file);
}
/***************************************************************************/
void CFileLink (
 char** a_fname
)
/***************************************************************************/
/* 
   if *a_fname contains upper case RZOPEN is unable to open.
   I do a temporary lower case named link to /tmp
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* fname;
  char* string = NULL;
  char* name;
/*.........................................................................*/
  if(a_fname==NULL)           return;
  if(*a_fname=='\0')          return;
  if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      if(CStringHasUpperCase(*a_fname)==0) return;

      string                 = CStringCreateF (strlen(*a_fname)+10,"ln -f -s %s ",*a_fname);

      name                   = CStringDuplicate(*a_fname);
      CFileTrimDirectory     (name);

      if(strcmp(*a_fname,name)!=0) /*some directory path in *a_fname*/
	{
	  CStringLower           (name);
	  fname                  = CStringCreateF (strlen(name)+5,"/tmp/%s",name);
	}
      else /*no directory in path name but some upper case in file name*/
	{
	  CStringLower           (name);
	  fname                  = CStringDuplicate(name);
	}

      CStringConcatenate     (&string,fname);
      
      remove                 (fname);
      CSystemExecute         (string);
      
      CStringDelete          (name);
      CStringDelete          (string);
      
      CStringDelete          (*a_fname);
      *a_fname               = fname;
    }
}
/***************************************************************************/
void CFileBindToF77_Unit (
 char* a_fname
,int a_unit 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_fname==NULL)  return;
  if(a_unit<=0)      return;

  if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      char* sf77;
      char* string = NULL;
      sf77  = CSystemAsk ("f77UnitFormat");
      if(sf77!=NULL)
	{
	  string         = CStringCreateF (strlen(sf77)+64,sf77,a_unit);
	}
      if(string!=NULL)     
	{
	  remove          (string);
	  CSystemExecuteF (strlen(a_fname)+strlen(string)+10,"ln -f -s %s %s",a_fname,string);
	}
      if(CFileIsValid(string)==0)
	{
	  CWarnF         ("Can't bind file %s \nto FORTRAN unit %d.\n",a_fname,a_unit);
	}
      CStringDelete  (string);
    }
  else if(CSystemIsKindOf("VMS",NULL)==1)
    {
      CSystemExecuteF       (strlen(a_fname)+64,"$ define/job for%3.3d %s",a_unit,a_fname);
    }
  else
    {
      CWarnF         ("Bind file %s \nto FORTRAN unit %d.\n",a_fname,a_unit);
    }
}
/***************************************************************************/
char* CFileLoadStrings (
 char* a_fname
,char a_char
,int* a_linen
,char*** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  file;
  char*  buff;
/*.........................................................................*/
  if(a_linen!=NULL) *a_linen      = 0;
  if(a_lines!=NULL) *a_lines      = NULL;
  file           = CFileOpenForReading (a_fname,NULL);
  if(file==NULL) return NULL; 
  buff           = CStreamGetStrings (file,a_char,a_linen,a_lines);
  fclose         (file);
  return         buff;
}
/***************************************************************************/
int CFileProduceFromTemplate (
 char*   a_fname
,char*   a_template_fname
,char*   a_name   
,char*   a_template_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    status;
  char*  lineb;
  char** lines;
  int    linen;
/*.........................................................................*/
  status = 0;
  lineb  = CFileLoadStrings (a_template_fname,'\0',&linen,&lines);
  if( (lineb!=NULL) && (linen!=0) && (lines!=NULL) ) 
    {
      FILE*          pfile;
      CFileSafeguard (a_fname);
      pfile          = CFileOpenForWriting (a_fname);
      if(pfile!=NULL)
	{
	  if( (a_template_name!=NULL) && (a_name!=NULL) && (strcmp(a_template_name,a_name)!=0) )
	    {
	      int count;
	      for(count=0;count<linen;count++) 
		{ 
		  if(strstr(lines[count],a_template_name)!=NULL)
		    {
		      char*         string;
		      string        = CStringDuplicate(lines[count]);
		      CStringReplacePart   (&string,a_template_name,a_name);
		      if(fprintf(pfile,"%s\n",string)<0) ReportError("CFileCreateFrTemplate",CErrorIO);
		      CStringDelete (string);
		    }
		  else
		    {
		      if(fprintf(pfile,"%s\n",lines[count])<0) ReportError("CFileCreateFrTemplate",CErrorIO);
		    }
		}
	    }
	  fclose (pfile);
	  CInfo  ("File ");CInfo (a_fname);CInfo  (" created.\n");
	}
      status = 1;
    }
  CMemoryFreeBlock (lines);
  CMemoryFreeBlock (lineb);
  return           status;
}
/***************************************************************************/
char* CFileGetDirectory (
 char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* This;
  char* pos = NULL;
/*..... .......................................................................*/
  This          = CStringDuplicate (a_fname);
  pos           = GetEndDirectory (This);
  if(pos==NULL) {CStringDelete(This);return NULL;}
  *(pos+1)      = '\0';
  return        This;
}
/***************************************************************************/
char* CFileGetName (
 char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* This;
/*..... .......................................................................*/
  This                 = CStringDuplicate (a_fname);
  CFileTrimDirectory   (This);
  CFileTrimType        (This);
  return               This;
}
/***************************************************************************/
char* CFileGetType (
 char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* This;
/*..... .......................................................................*/
  This               = CStringDuplicate (a_fname);
  CFileTrimDirectory (This);
  CFileTrimName      (This);
  return             This;
}
/***************************************************************************/
void CFileTrimDirectory (
 char* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos = NULL;
/*..... .......................................................................*/
  pos    = GetEndDirectory (This);
  if(pos==NULL) return;
  strcpy (This,pos+1);
}
/***************************************************************************/
static char* GetEndDirectory (
 char* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* syskind;
  char* pos = NULL;
/*..... .......................................................................*/
  if(This==NULL) return NULL;
       if(CSystemIsKindOf("UNIX",&syskind)==1)
    {
      pos = strrchr (This,'/');
    }
  else if(CSystemIsKindOf("NT",&syskind)==1)
    {
      pos = strrchr (This,'\\');
    }
  else if(CSystemIsKindOf("VMS",&syskind)==1)
    {
                    pos = strrchr (This,']');
      if(pos==NULL) pos = strrchr (This,':');
      if(pos==NULL) pos = strrchr (This,'>');
    }
  else
    {
      CWarnF ("GetEndDirectory : system %s not treated.\n",syskind);
    }
  return pos;
}
/***************************************************************************/
void CFileTrimType (
 char* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*..... .......................................................................*/
  if(This==NULL) return;
  pos     = strstr (This,".");
  if(pos==NULL) return;
  *pos    = '\0';
}
/***************************************************************************/
void CFileTrimName (
 char* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*..... .......................................................................*/
  if(This==NULL) return;
  pos           = strstr (This,".");
  if(pos!=NULL) strcpy  (This,pos+1);
  else          *This = '\0';
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static char* GetDirectory (
 char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
  char* string;
/*.........................................................................*/
  if(a_name==NULL) return NULL;
  name                     = getenv(a_name);
  string                   = CStringDuplicate (name!=NULL ? name : a_name);
  GetEnvironment           (&string);
  CStringTrim              (string);
  if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      int    length;
      length = strlen(string);
      if( (length!=0) && string[length-1]!='/') CStringConcatenate (&string,"/");
    }
  else if(CSystemIsKindOf("NT",NULL)==1)
    {
      int    length;
      length = strlen(string);
      if( (length!=0) && string[length-1]!='\\') CStringConcatenate (&string,"\\");
    }
  return string;
}
/***************************************************************************/
static void GetEnvironment (
 char** This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if(This==NULL)  return;
  if(*This=='\0') return;
#ifdef DEBUG
  printf ("debug : Co/src/CFile.c/GetEnvironment : begin : %s.\n",*This);
#endif
  if(CSystemIsKindOf("UNIX",NULL)==1) 
    {
      while((pos=strstr(*This,"$"))!=NULL)
	{
	  char* env;
	  char* end;
	  char* val;
	  env   = CStringDuplicate(pos);
	  end   = strstr(env,"/");
	  if(end!=NULL) *end = '\0';
	  val     = getenv (env+1);
	  if(val!=NULL) CStringReplacePart (This,env,val);
	  else          CWarnF ("Environment variable %s not defined.\n",env+1);
	  CStringDelete (env);
	  if(val==NULL) break; /*Stop looping if env variable not found.*/
	}
    }
  else if(CSystemIsKindOf("NT",NULL)==1) 
    {
      int found = 0;
      while((pos=strstr(*This,"$"))!=NULL)
	{
	  char* env;
	  char* end;
	  char* val;
	  env   = CStringDuplicate(pos);
	  end   = strstr(env,"/");
	  if(end!=NULL) *end = '\0';
	  val     = getenv (env+1);
	  if(val!=NULL)       
	    {
	      CStringReplacePart (This,env,val);
	      found       = 1;
	    }
	  else          CWarnF ("Environment variable %s not defined.\n",env+1);
	  CStringDelete (env);
	  if(val==NULL) break; /*Stop looping if env variable not found.*/
	}
      if (found==1)
	{
	  CStringReplacePart (This,"/","\\");
	}
#ifdef DEBUG
      printf ("debug : Co/src/CFile.c/GetEnvironment : end : %s.\n",*This);
#endif
    }
  else if(CSystemIsKindOf("VMS",NULL)==1) 
    {
      int found = 0;
      while((pos=strstr(*This,"$"))!=NULL)
	{
	  char* env;
	  char* end;
	  char* val;
	  env   = CStringDuplicate(pos);
	  end   = strstr(env,"/");
	  if(end!=NULL) *end = '\0';
	  val     = getenv (env+1);
	  if(val!=NULL)       
	    {
	      CStringReplacePart (This,env,val);
	      found       = 1;
	    }
	  else          CWarnF ("Environment variable %s not defined.\n",env+1);
	  CStringDelete (env);
	  if(val==NULL) break; /*Stop looping if env variable not found.*/
	}
      if (found==1)
	{
	  CStringReplacePart (This,"/",":");
	}
    }
}
/***************************************************************************/
static FILE* OpenFileForReading (
 char* a_string 
,char** a_fullName
,char** a_fname
)
/***************************************************************************/
/* 
  Use list of path to search the file. The file is open in reading mode.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  FILE*  fp;
  char*  ename;
  char*  fname;
/*.........................................................................*/
  if(a_fullName!=NULL) *a_fullName = NULL;
  if(a_fname!=NULL)    *a_fname    = NULL;
  if( (a_string==NULL) || (*a_string=='\0') ) return NULL;

/*look if a_string is an environment variable*/
  ename  = getenv(a_string);
  fname  = CStringDuplicate (ename!=NULL ? ename : a_string);
  GetEnvironment (&fname);

/*Try first current directory.*/
#ifdef DEBUG
  printf ("debug : try : %s\n",fname); 
#endif
  fp     = fopen(fname,"rb");
  if(fp!=NULL) 
    {
      if(a_fullName!=NULL) *a_fullName = CStringDuplicate(fname);
    }
  else 
    {
      CFileAddPathVariable ("OPATH");
      CFileAddPathVariable ("COPATH");
      for(count=0;count<Class.vpathn;count++)
	{
	  if(getenv(Class.vpaths[count])!=NULL)
	    {
	      char**   words;
	      int      wordn;
	      words    = CStringGetWords (getenv(Class.vpaths[count])," ",&wordn);
	      for(count=0;count<wordn;count++) 
		{
		  char*          string;
		  char*          name;
		  string         = GetDirectory (words[count]);
		  name           = CStringCreateF (strlen(string)+strlen(fname),"%s%s",string,fname);
		  CStringDelete  (string);
		  if(name==NULL) break;
#ifdef DEBUG
		  printf         ("debug : try : %s\n",name); 
#endif
		  fp             = fopen(name,"rb");
		  if(fp!=NULL)   
		    {
		      if(a_fullName!=NULL) 
			{
			  *a_fullName = name;
			} 
		      else 
			{
			  CStringDelete(name);
			}
		      break;
		    }
		  CStringDelete  (name);
		} 
	      CTextDelete (wordn,words);
	    }
	  if(fp!=NULL) break; 
	}
      if(fp==NULL) 
	{
	  if(Class.pathn!=0)
	    {	
	      for(count=Class.pathn-1;count>=0;count--)  /*Look last entry first.*/
		{ 
		  char*          name;
		  name           = CStringCreateF (strlen(Class.paths[count])+strlen(fname),
						   "%s%s",Class.paths[count],fname);
		  if(name==NULL) break;
#ifdef DEBUG
		  printf         ("debug : try : %s\n",name); 
#endif
		  fp             = fopen(name,"rb");
		  if(fp!=NULL)   
		    {
		      if(a_fullName!=NULL) 
			{
			  *a_fullName = name;
			} 
		      else 
			{
			  CStringDelete(name);
			}
		      break;
		    }
		  CStringDelete  (name);
		}
	    }
	}
    }
  if(a_fname!=NULL) *a_fname = CStringDuplicate(fname);
  CStringDelete (fname);
  return        fp;
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
  CErrorHandle ("Co","CFile",a_name,a_code);
}
