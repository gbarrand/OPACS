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

#include <CPrinter.h>
#include <CString.h>
#include <CFile.h>
#include <CSystem.h>

#include <CBatch.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    SendToDCL_ThroughUCX    (char*,char*,char*,char*,char*);
static void    SendToDCL_ThroughDECNET (char*,char*,char*,char*);
static void    SendToUNIX_SH           (char*,char*,char*);
static void    SendToUNIX_CSH          (char*,char*,char*);
#ifdef __cplusplus
}
#endif

static struct 
{
  char*    display;
  CBatchAskPasswordFunction askPassword;
} Class = {NULL,NULL};
/***************************************************************************/
void CBatchClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete     (Class.display);
  Class.display     = NULL;
  Class.askPassword = NULL;
}
/***************************************************************************/
void CBatchSetDisplay (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete (Class.display);
  Class.display = CStringDuplicate(a_string);
}
/***************************************************************************/
void CBatchSetAskPasswordFunction (
 CBatchAskPasswordFunction a_askPassword
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.askPassword = a_askPassword;
}
/***************************************************************************/
void CBatchSendToUNIX_SH (
 char* a_node
,char* a_string
)
/***************************************************************************/
/*
      unix_node:command
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_node==NULL)   || (*a_node=='\0') )   return;
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      SendToUNIX_SH (a_string,a_node,Class.display);
    }
  else
    {
      CWarn ("Spawning to a unix node from this system not implemented.\n");
    }
}
/***************************************************************************/
void CBatchSendToUNIX_CSH (
 char* a_node
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_node==NULL)   || (*a_node=='\0') )   return;
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      SendToUNIX_CSH (a_string,a_node,Class.display);
    }
  else
    {
      CWarn ("Spawning to a unix node from this system not implemented.\n");
    }
}
/***************************************************************************/
void CBatchSendToVMS_ThroughUCX (
 char* a_node
,char* a_string
)
/***************************************************************************/
/*
      vms_node:command
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_node==NULL)   || (*a_node=='\0') )   return;
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      char* user;
      if(Class.askPassword==NULL) 
	{
	  CWarn ("Co/CBatch.c/CBatchSendToVMS_ThroughUCX : no AskPasswordProc declared.\n");
	  return;
	}

      user  = getenv ("USER");
      if(user==NULL)
	{
	  CWarn ("Co/CBatch.c/CBatchSendToVMS_ThroughUCX : environment variable USER not defined.\n");
	}
      else
	{
	  char*         prompt;
	  char*         password;
	  prompt        = CStringCreateF (strlen(a_node)+strlen(user)+10,"%s %s password",a_node,user);
	  password      = Class.askPassword(prompt);
	  CStringDelete (prompt);
	  SendToDCL_ThroughUCX  (a_string,a_node,Class.display,user,password);
	}
    }
  else
    {
      CWarn ("Spawning to a vms node from this system not implemented.\n");
    }
}
/***************************************************************************/
void CBatchSendToVMS_ThroughDECNET (
 char* a_decnet
,char* a_node
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_decnet==NULL) || (*a_decnet=='\0') ) return;
  if( (a_node==NULL)   || (*a_node=='\0')   ) return;
  if( (a_string==NULL) || (*a_string=='\0') ) return;
  if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      SendToDCL_ThroughDECNET (a_string,a_node,Class.display,a_decnet);
    }
  else
    {
      CWarn ("Spawning to a vms node from this system not implemented.\n");
    }
}
/***************************************************************************/
static void SendToDCL_ThroughUCX (
 char* a_string
,char* a_host
,char* a_display
,char* a_user
,char* a_password
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* tmpname;
  FILE* file;
  char* home;
  char* fname;
  char* corsh;
/*.........................................................................*/
  if( (a_string==NULL)   || (*a_string=='\0') )   return;
  if( (a_host==NULL)     || (*a_host=='\0')   )   return;
  if( (a_user==NULL)     || (*a_user=='\0')   )   return;
  if( (a_password==NULL) || (*a_password=='\0') ) return;

  home = getenv("HOME");
  if(home==NULL)
    {
      CWarn ("Co/CBatch.c/SendToDCL_ThroughUCX : environment variable USER not defined.\n");
      return;
    }
  corsh  = CSystemAsk ("remoteShellCommand");
  if(corsh==NULL) return;

  tmpname   = CFileBuildTemporaryName ("/tmp/");
  file      = CFileOpenForWriting  (tmpname);
  if(file==NULL) return;
  if(a_display!=NULL)        
    { 
      char* node;
      char* pos;
      node  = CStringDuplicate(a_display);
      pos   = strstr(node,":");
      if(pos!=NULL) {*pos = '\0';}
      fprintf (file,"$ set display/create/node=%s/transport=tcpip\n",node);                    
      CStringDelete (node);
    }
  fprintf (file,"$ %s\n",a_string);
  fprintf (file,"$ pur out.com\n");
  fprintf (file,"$ exit\n");
  fclose  (file);

  fname     = CStringCreateF (strlen(home)+7,"%s/.netrc",home);
  if(CFileIsValid(fname)==1) {CSystemExecuteF (2*strlen(fname)+7,"mv %s %sbak",fname,fname);}
  file      = CFileOpenForWriting  (fname);
  if(file!=NULL) 
    {
      fprintf    (file,"machine %s login %s password %s\n",a_host,a_user,a_password);
      fprintf    (file,"macdef init\n");
      fprintf    (file,"put %s out.com\n",tmpname);
      fprintf    (file,"quit\n");
      fprintf    (file,"\n");
      fclose     (file);
      CSystemExecuteF (strlen(fname)+2*strlen(a_host)+strlen(corsh)+44,
		  "chmod go-rw %s;ftp %s;(%s %s -n '@out.com' &) ",fname,a_host,corsh,a_host);
      remove     (tmpname);
      remove     (fname);
    } /*endif file netrc*/

  CStringDelete (fname);

}
/***************************************************************************/
static void SendToDCL_ThroughDECNET (
 char* a_string
,char* a_host
,char* a_display
,char* a_decnet
)
/***************************************************************************/
/*  
    a_host:   vms node.
    a_decnet: ultrix node that have decnet.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* tmpname;
  FILE* file;
  char* corsh;
/*.........................................................................*/
  if( (a_string==NULL)   || (*a_string=='\0') )   return;
  if( (a_host==NULL)     || (*a_host=='\0')   )   return;
  if( (a_decnet==NULL)   || (*a_decnet=='\0') )   return;

  corsh  = CSystemAsk ("remoteShellCommand");
  if(corsh==NULL) return;

  tmpname   = CFileBuildTemporaryName ("/tmp/");
  file      = CFileOpenForWriting  (tmpname);
  if(file==NULL) return;

  if(a_display!=NULL)        
    {
      char* node;
      char* pos;
      node  = CStringDuplicate(a_display);
      pos   = strstr(node,":");
      if(pos!=NULL) {*pos = '\0';}
      fprintf (file,"$ set display/create/node=%s/transport=tcpip\n",node);
      CStringDelete (node);
    }
  fprintf (file,"$ %s\n",a_string);
  fprintf (file,"$ pur out.com\n");
  fprintf (file,"$ exit\n");
  fclose  (file);

  CSystemExecuteF (4*strlen(tmpname)+2*strlen(a_decnet)+strlen(a_host)+strlen(corsh)+128,
"(rcp %s %s:%s);\
%s %s -n \"csh -c '((dcp -S %s %s::out.com);/bin/rm -f %s) >&/dev/null &' \"",
tmpname,a_decnet,tmpname,
corsh,a_decnet,tmpname,a_host,tmpname);
  remove  (tmpname);

/*
In /tmp/x.lis:
$ set display/create/node=193.48.100.66/transport=tcpip                                                        
$ run sys$system:decw$mail
$ pur out.com
$ exit

(rcp /tmp/x.lis lalux0:/tmp/x.lis);rsh lalux0 -n "csh -c '((dcp -S /tmp/x.lis lalaxs::out.com);)'"


*/
                  
}
/***************************************************************************/
static void SendToUNIX_SH (
 char* This
,char* a_host
,char* a_display
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL)   || (*This=='\0')   )       return;
  if( (a_host==NULL) || (*a_host=='\0') )
    {
      CSystemExecute (This);
    }
  else if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      char* corsh;
      corsh = CSystemAsk ("remoteShellCommand");
      if(corsh!=NULL)
	{
	  if(a_display!=NULL) 
	    {
	      CSystemExecuteF (strlen(corsh)+strlen(a_host)+strlen(a_display)+strlen(This)+128,
                         "%s %s -n \"sh -c '(DISPLAY=%s;export DISPLAY;(%s)) 1> /dev/null 2> /dev/null &' \"",
			 corsh,a_host,a_display,This);
	    }
	  else  
	    {
	      CSystemExecuteF (strlen(corsh)+strlen(a_host)+strlen(This)+64,
			 "%s %s -n \"sh -c '(%s) 1> /dev/null 2> /dev/null &' \"",corsh,a_host,This);
	    }
	}
    }
  else
    {
      CWarn ("Spawning from this system not implemented.\n");
    }
}
/***************************************************************************/
static void SendToUNIX_CSH (
 char* This
,char* a_host
,char* a_display
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (This==NULL)   || (*This=='\0')   )       return;
  if( (a_host==NULL) || (*a_host=='\0') )
    {
      CSystemExecute (This);
    }
  else if(CSystemIsKindOf("UNIX",NULL)==1)
    {
      char* corsh;
      corsh  = CSystemAsk ("remoteShellCommand");
      if(corsh!=NULL)
	{

	  if(a_display!=NULL)
	    {
	      CSystemExecuteF (strlen(corsh)+strlen(a_host)+strlen(a_display)+strlen(This)+128,
       "%s %s -n \"csh -c '(setenv DISPLAY %s;(%s)) >& /dev/null &' \"",corsh,a_host,a_display,This);
	    }
	  else
	    {
	      CSystemExecuteF (strlen(corsh)+strlen(a_host)+strlen(This)+64,
	      "%s %s -n \"csh -c '(%s) >& /dev/null &' \"",corsh,a_host,This);
	    }
	}
    }
  else
    {
      CWarn ("Spawning from this system not implemented.\n");
    }
}
