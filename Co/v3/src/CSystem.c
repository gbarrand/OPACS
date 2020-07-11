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
#include <stdio.h>
#include <string.h>

#include <CMemory.h>
#include <CString.h>
#include <CPrinter.h>

#include <CSystem.h>

#ifdef __sgi /*ifdef_system*/   /*sigsetjmp : pb to link with fortran.*/
#ifdef __cplusplus
extern "C"{
#endif
void sigsetjmp () {CWarn("Co/CSystem.c/sigsetjmp : dummy.\n");} 
#ifdef __cplusplus
}
#endif
#endif

static struct 
{
  char string[32];
} Class = {""};
/***************************************************************************/
int CSystemIsKindOf (
 char*  a_string   
,char** a_sys
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static  char kind[] = "      ";
  static  int  init   = 0;
/*.........................................................................*/
  if(a_sys!=NULL) *a_sys = NULL;
  if(init==0) 
    {
      char* senv;
      senv  = getenv("COSYSKIND");
      if(senv==NULL)
	{
	  if(fopen("/tmp","rb")!=NULL)
	    {
	      strcpy (kind,"UNIX");
	      init   = 1;
	    }
	  else if(fopen("SYS$SYSTEM:SYSBOOT.EXE;1","rb")!=NULL)
	    {
	      strcpy (kind,"VMS");
	      init   = 1;
	    }
	  else if(fopen("C:\\MSDEV\\INCLUDE\\WINNT.H","rb")!=NULL)
	    {
	      strcpy (kind,"NT");
	      init   = 1;
	    }
	  else
	    {
	      CWarn  ("Environment variable COSYSKIND not defined.\n Modify Co/mgr/requirements and do a pack_config.\n");
	      return 0;
	    }
	}
      else
	{
	  strcpy (kind,senv);
	  init   = 1;
	}
    }
  if(a_sys!=NULL) *a_sys = kind;
  if( (a_string==NULL) || (*a_string=='\0') ) return 0;
  if(strcmp(a_string,kind)==0) return 1;
  return 0;
}
/***************************************************************************/
int CSystemIsSecured (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (getenv("COSECURED")==NULL ? 0 : 1);
}
/***************************************************************************/
void CSystemExecute (
 char* a_string   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*   senv;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0')) return;
  senv     = getenv(a_string);
  system   (senv!=NULL ? senv : a_string);
}
/***************************************************************************/
void CSystemExecuteF (
 int   a_number
,char* a_format
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*    string;
  va_list  args;
/*.........................................................................*/
  if(a_number<0)     return;
  if(a_format==NULL) return;
  string             = (char*)CMemoryAllocateBlock( (a_number+1) * sizeof(char));
  if(string==NULL)   return;
  string[a_number]   = '\0';
  va_start           (args,a_format);
  vsprintf           (string,a_format,args);
  va_end             (args);
  if(string[a_number]!='\0') 
    {
      CWarn ("Co/CSystem.c/CSystemExecuteF : string overflow. A crash is expected.\n");
    }
  CSystemExecute           (string);
  CMemoryFreeBlock           (string);
}
/***************************************************************************/
char* CSystemAsk (
 char* a_what			 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_what==NULL) return NULL;
  if(strcmp(a_what,"listCommand")==0)
    {
           if(CSystemIsKindOf("UNIX",NULL)==1)
	{
	  strcpy (Class.string,"ls");
	  return Class.string;
	}
      else if(CSystemIsKindOf("NT",NULL)==1)
	{
	  strcpy (Class.string,"dir");
	  return Class.string;
	}
      else if(CSystemIsKindOf("VMS",NULL)==1)
	{
	  strcpy (Class.string,"dir");
	  return Class.string;
	}
      else
	{
	  CWarn  ("Co/CSystemGetCommand : Unknown operating system.\n");
	  return NULL;
	}
    }
  else if(strcmp(a_what,"makeCommand")==0)
    {
           if(CSystemIsKindOf("UNIX",NULL)==1)
	{
	  strcpy (Class.string,"make");
	  return Class.string;
	}
      else if(CSystemIsKindOf("NT",NULL)==1)
	{
	  strcpy (Class.string,"nmake");
	  return Class.string;
	}
      else if(CSystemIsKindOf("VMS",NULL)==1)
	{
	  strcpy (Class.string,"mms");
	  return Class.string;
	}
      else
	{
	  CWarn  ("Co/CSystemGetCommand : Unknown operating system.\n");
	  return NULL;
	}
    }
  else if(strcmp(a_what,"remoteShellCommand")==0)
    {
           if(CSystemIsKindOf("UNIX",NULL)==1)
	{
#ifdef __hpux
	  strcpy (Class.string,"remsh");
#else
	  strcpy (Class.string,"rsh");
#endif
	  return Class.string;
	}
      else if(CSystemIsKindOf("NT",NULL)==1)
	{
	  strcpy (Class.string,"rsh");
	  return Class.string;
	}
      else if(CSystemIsKindOf("VMS",NULL)==1)
	{
	  strcpy (Class.string,"rsh");
	  return Class.string;
	}
      else
	{
	  CWarn  ("Co/CSystemGetCommand : Unknown operating system.\n");
	  return NULL;
	}
    }
  else if(strcmp(a_what,"f77UnitFormat")==0)
    {
           if(CSystemIsKindOf("UNIX",NULL)==1)
	{
#ifdef __hpux
	  strcpy (Class.string,"ftn%d");
#else
	  strcpy (Class.string,"fort.%d");
#endif
	  return Class.string;
	}
      else if(CSystemIsKindOf("NT",NULL)==1)
	{
	  strcpy (Class.string,"fort.%d");
	  return Class.string;
	}
      else if(CSystemIsKindOf("VMS",NULL)==1)
	{
	  strcpy (Class.string,"fort.%d");
	  return Class.string;
	}
      else
	{
	  CWarn  ("Co/CSystemGetCommand : Unknown operating system.\n");
	  return NULL;
	}
    }
  else if(strcmp(a_what,"kind")==0)
    {
           if(CSystemIsKindOf("UNIX",NULL)==1)
	{
	  strcpy (Class.string,"UNIX");
	  return Class.string;
	}
      else if(CSystemIsKindOf("NT",NULL)==1)
	{
	  strcpy (Class.string,"NT");
	  return Class.string;
	}
      else if(CSystemIsKindOf("VMS",NULL)==1)
	{
	  strcpy (Class.string,"VMS");
	  return Class.string;
	}
      else
	{
	  CWarn  ("Co/CSystemGetCommand : Unknown operating system.\n");
	  return NULL;
	}
    }
  else 
    {
      CWarnF ("Co/CSystemGetCommand : Unknown keyword %s.\n",a_what);
      return NULL;
    }
}
/***************************************************************************/
int CSystemGetAttribute (
 void* This
,char* a_name
,void* a_user
,void* a_addr 
,int*  a_number
)
/***************************************************************************/
/* 
   This==NULL means we want infos on the class.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_name==NULL)   return 0;
  if(a_addr==NULL)   return 0; 

       if(strcmp(a_name,"listCommand")==0)       
    {
      *((char**)a_addr) = CSystemAsk ("listCommand");
    }
  else if(strcmp(a_name,"makeCommand")==0)       
    {
      *((char**)a_addr) = CSystemAsk ("makeCommand");
    }
  else if(strcmp(a_name,"remoteShellCommand")==0)       
    {
      *((char**)a_addr) = CSystemAsk ("remoteShellCommand");
    }
  else if(strcmp(a_name,"kind")==0)       
    {
      *((char**)a_addr) = CSystemAsk ("kind");
    }
  else 
    {
      CInfoF ("CSystemGetAttribute : unknown property %s.\n",a_name);
      return 0;
    }

  This   = NULL;
  a_user = NULL;
  return 1;
}
/***************************************************************************/
void CSystemGenerateCallback (
 char* a_string   
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0')) return;
/* CFilePrintSF ("\
{\n\
 system(\"%s\");\n\
 a_widget = NULL;\n\
 a_tag    = NULL;\n\
 a_reason = NULL;\n\
}\n",a_string);
*/
}
