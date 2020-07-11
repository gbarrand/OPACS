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
  command  = string [< > filename]
  pipeline = list of command  separated by |
  list     = list of pipeline separated by [; & && || ] and could be ended by [ ; & |& ]
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <CMemory.h>
#include <CStream.h>
#include <CFile.h>
#include <CString.h>
#include <CPrinter.h>
#include <CText.h>
#include <CError.h>
#include <CList.h>
#include <OShell.h>
#include <OType.h>

#include <OProcessP.h>

#ifdef __cplusplus
extern "C"{
#endif
static int   DoFlow                       (OProcess,char*);
static int   DoPipe                       (OProcess,char*,char**);
static int   DoCommand                    (OProcess,char*);
static char* GetPipe                      (OProcess);

static int   PutStringInPipe              (OProcess,char*);
static void  ConvertPipeStringsToHandles  (OProcess);
static char* GetSubstring                 (char*,char);
static int   SetVariable                  (OProcess,char*);
static void  ReportError                  (char*,int);
#ifdef __cplusplus
}
#endif

#define NotFound    (-1)
static struct 
{
  int      argn;
  char**   args;
} Class = {0,NULL};
/***************************************************************************/
char** OProcessGetArguments (
 int* a_argn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=NULL) *a_argn = Class.argn;
  return Class.args;
}
/***************************************************************************/
OProcess OProcessCreate (
 void* a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProcess            This;
/*.........................................................................*/
  if(a_osh==NULL)  return NULL;
  This             = (OProcess)CMemoryAllocateBlock (sizeof(OProcessRecord));
  if(This==NULL)   return NULL;
  This->osh        = a_osh;
  This->set_x      = 0;
  This->set_v      = 0;
  This->pipeHands  = NULL;
  This->pipeActive = 0;
  This->argn       = 0;
  This->args       = NULL;
  This->varn       = 0;
  This->vars       = NULL;
  This->valn       = 0;
  This->vals       = NULL;
  This->command    = NULL;
  This->ifStatus   = 0;
  This->elseStatus = 0;
  return           This;
}
/***************************************************************************/
void OProcessDelete (
  OProcess This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)      return;
  This->command       = NULL;
  OProcessPutHandles (This,NULL);
  CTextDelete         (This->argn,This->args);
  This->argn          = 0;
  This->args          = NULL;
  CTextDelete         (This->varn,This->vars);
  This->varn          = 0;
  This->vars          = NULL;
  CTextDelete         (This->valn,This->vals);
  This->valn          = 0;
  This->vals          = NULL;
  This->ifStatus      = 0;
  This->elseStatus    = 0;
  This->pipeActive    = 0;
  CMemoryFreeBlock    (This);
}
/***************************************************************************/
void OProcessExecuteFile (
 OProcess This
,char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  file;
  char*  buff = NULL;
/*.........................................................................*/
  if(This==NULL)       return;
  file                 = CFileOpenForReading (a_fname,NULL);
  if(file==NULL)       return; 
  buff                 = CStreamRead (file,NULL);
  if(buff==NULL)       return;
  fclose               (file);
  OProcessExecuteLines (This,buff);
  CMemoryFreeBlock     (buff);
}
/***************************************************************************/
int OProcessExecuteLines (
 OProcess This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  char*  line;
  int    retval = EXIT_SUCCESS;
/*.........................................................................*/
  if(This==NULL) return retval;
  if( (a_string==NULL) || (*a_string=='\0')) return retval;
  line = string = CStringDuplicate(a_string);
  This->ifStatus   = 0;
  This->elseStatus = 0;
  while(1)
    {
      char*  pos;
      pos    = strchr(line,'\n');
      if(pos==NULL) 
	{
	  retval = DoFlow (This,line);
	  break;
	}
      else
	{ 
	  *pos   = '\0';
	  retval = DoFlow (This,line);
          if(retval<=OProcessExit) break;
	  line   = pos + 1;
	}                     
    }
  CStringDelete (string);
  if(retval<=OProcessExit) return retval;
  if((This->ifStatus==100) || (This->ifStatus==101)) /*After a "then"*/
    {
      if(This->elseStatus!=0)
	{
	  CWarn  ("A line containning \"fi\" is expected.\n");
	  retval = OProcessExit;
	}
    }
  return        retval;
}
/***************************************************************************/
static int DoFlow (
 OProcess This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  int    retval = EXIT_SUCCESS;
  char*  pos;
/*.........................................................................*/
  if(This==NULL) return retval;
  if( (a_string==NULL) || (*a_string=='\0')) return retval;
  if(CStringBeginWith(a_string,'#')==1) return retval; 

  string = CStringDuplicate(a_string);

  pos    = CStringGetFirstNotSpacePosition (string);

  if( (string!=NULL) && (*string!='\0'))
    {
      /*printf ("debug : %s %d %d\n",string,This->ifStatus,This->elseStatus);*/
      if( (pos!=NULL) && (strncmp(pos,"if ",3)==0) )
	{
	  retval = OProcessExecute (This,pos+3);
	  if(retval<=OProcessExit) 
	    {
	    }
	  else
	    {
	      if(retval==EXIT_SUCCESS) 
		{
		  This->ifStatus   = 11;
		  This->elseStatus = 11;
		}
	      else
		{
		  This->ifStatus   = 10;
		  This->elseStatus = 11;
		}
	    }
	}                     
      else if((This->ifStatus==10) || (This->ifStatus==11))
	{
	  if( (pos!=NULL) && (strcmp(pos,"then")==0) )
	    {
	      if(This->ifStatus==10)
		{
		  This->ifStatus = 100;
		}
	      else if(This->ifStatus==11)
		{
		  This->ifStatus = 101;
		}
	    }
	  else
	    {
	      CWarnF  ("A line containning \"then\" is expected in place of :\n%s\n",string);
	      retval  = OProcessExit;
	    }
	}                     
      else if((This->ifStatus==100) || (This->ifStatus==101)) /*After a "then".*/
	{
	  if( (pos!=NULL) && (strcmp(pos,"else")==0) )
	    {
	      This->elseStatus = 10;
	    }
	  else if( (pos!=NULL) && (strcmp(pos,"fi")==0) )
	    {
	      This->ifStatus   = 0;
	      This->elseStatus = 0;
	    }
	  else if( (This->ifStatus==100) && (This->elseStatus==10) ) 
	    {
	      retval = OProcessExecute (This,string);
	    }
	  else if( (This->ifStatus==101) && (This->elseStatus==11) ) 
	    {
	      retval = OProcessExecute (This,string);
	    }
	}                     
      else /*Not in an if.*/
 	{
	  retval = OProcessExecute (This,string);
	}
    }

  CStringDelete (string);

  return        retval;
}
/***************************************************************************/
int OProcessExecute (
 OProcess   This
,char* a_string
)
/***************************************************************************/
/*
  Execute a list of pipe separated by ';' or '&&'.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  char*  pos;
  int    retval = EXIT_SUCCESS;
/*.........................................................................*/
  if(This==NULL)                     return retval;
  if( (a_string==NULL) || (*a_string=='\0')) return retval;
/*printf ("debug : line : %s|\n",a_string);*/
  if(This->set_v!=0) CInfoF ("%s\n",a_string);

  if(CStringBeginWith(a_string,'#')==1) return retval; 

  string  = CStringDuplicate(a_string);

  if( (string!=NULL) && (*string!='\0'))
    {
      char*  list;
      char*  sout;
      int    found;
      char*  subs;
      char   sdollar[16];

      CTextDelete (This->argn,This->args);
      This->argn  = 0;
      This->args  = NULL;

/*treat 'xxx'*/
      while(1)
	{
	  found = 0;
	  subs  = GetSubstring  (string,'\'');
	  if(subs!=NULL)
	    {
	      sprintf              (sdollar,"$$$%d",This->argn);
	      CStringReplacePart   (&string,subs,sdollar);
	      subs[strlen(subs)-1] = '\0';
	      CTextAddLine         (&(This->argn),&(This->args),subs+1);
	      CStringDelete        (subs);
	      found                = 1;
	    }
	  if(found==0) break;
	}
      
/*treat `xxx`*/
      while(1)
	{
	  found = 0;
	  subs  = GetSubstring  (string,'`');
	  if(subs!=NULL)
	    {
	      sprintf              (sdollar,"$_$%d",This->argn);
	      CStringReplacePart   (&string,subs,sdollar);
	      subs[strlen(subs)-1] = '\0';
	      CTextAddLine         (&(This->argn),&(This->args),subs+1);
	      CStringDelete        (subs);
	      found                = 1;
	    }
	  if(found==0) break;
	}

      list = string;
      
     {int do_next = 1;
      while(1)
	{
	  pos   = strchr(list,'&');
	  if( (pos!=NULL) && (*(pos+1)=='&')) 
	    {
	      *pos = '\0';
	      if(do_next==1)
		{
		  retval           = DoPipe     (This,list,&sout);
		  if(sout!=NULL)   
		    {
		      CPrinterPut (sout);
		      CPrinterPut("\n");
		    }
		  CStringDelete    (sout);
		  if(retval<=OProcessExit)   break;
		  if(retval!=EXIT_SUCCESS) do_next = 0; /*Take care of f77 cmds and VMS where EXIT_FAILURE is a big number.*/
		}
	      else 
		{
		  do_next = 1;
		}
	      list = pos + 2;
	    }
	  else
	    { 
	      pos   = strchr(list,';');
	      if(pos==NULL) 
		{
		  if(do_next==1)
                    {
		      retval         = DoPipe (This,list,&sout);
		      if(sout!=NULL) 
			{
			  CPrinterPut (sout);
			  CPrinterPut("\n");
			}
		      CStringDelete  (sout);
		    }
		  else 
		    {
		      do_next = 1;
		    }
		  break;
		}
	      else
		{ 
		  *pos             = '\0';
		  if(do_next==1)
                    {
		      retval           = DoPipe     (This,list,&sout);
		      if(sout!=NULL)         
			{
			  CPrinterPut (sout);
			  CPrinterPut("\n");
			}
		      CStringDelete    (sout);
		      if(retval<=OProcessExit) break;
		    }
		  else 
		    {
		      do_next = 1;
		    }
		  list = pos + 1;
		}                     
	    }                     
	}}

      CTextDelete   (This->argn,This->args);
      This->argn  = 0;
      This->args  = NULL;
    }

  CStringDelete (string);
  return        retval;
}
/***************************************************************************/
void OProcessPutHandles (
 OProcess  This
,OHandle* a_hands
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CListDestroy     ((CList)This->pipeHands,(CListVisitEntryFunction)OHandleDelete);
  This->pipeHands = a_hands; /*No copy is done*/
}
/***************************************************************************/
OHandle* OProcessGetHandles (
 OProcess This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->pipeHands;
}
/***************************************************************************/
void OProcessPutString (
 OProcess This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHandle*       hands;
/*.........................................................................*/
  if(This==NULL)  return;
  hands           = (OHandle*)CListCreate(1);
  if(hands==NULL) return;
  hands[0]        = OHandleCreate(NULL,a_string);  
  OProcessPutHandles (This,hands);
}
/***************************************************************************/
char* OProcessGetString (
 OProcess This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHandle*     hands;
/*.........................................................................*/
  if(This==NULL) return NULL;
  if( (a_string!=NULL) && (strcmp(a_string,"-")!=0) ) return a_string;
  hands           = OProcessGetHandles(This);
  if(hands==NULL) return NULL;
  if(OHandleGetType(hands[0])!=NULL) return NULL; /*Not a string.*/
  return     (char*)OHandleGetIdentifier(hands[0]);
}
/***************************************************************************/
void OProcessPutTypeAndIdentifier (
 OProcess This
,void* a_type
,void* a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHandle*       hands;
/*.........................................................................*/
  if(This==NULL)      return;
  hands               = (OHandle*)CListCreate(1);
  if(hands==NULL)     return;
  hands[0]            = OHandleCreate(a_type,a_obj);
  OProcessPutHandles (This,hands);
}
/***************************************************************************/
void OProcessSetMode (
 OProcess This
,int a_mode
,int a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
       if(a_mode==1) This->set_v = a_value;
  else if(a_mode==2) This->set_x = a_value;
}
/***************************************************************************/
void* OProcessGetShell (
 OProcess This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->osh;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int DoPipe (
 OProcess  This
,char*  a_string
,char** a_sout
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  char*  pipe;
  char*  sout = NULL;
  int    retval = EXIT_SUCCESS;
/*.........................................................................*/
  if(a_sout!=NULL) *a_sout = NULL;
  if(This==NULL) return retval;
  if( (a_string==NULL) || (*a_string=='\0') ) return retval;
/*printf ("debug : pipe : %s|\n",a_string);*/
  if(CStringBeginWith(a_string,'#')==1) return retval; 
  string           = CStringDuplicate(a_string);
  pipe             = string;
  This->pipeActive = 0;
  while(1)
    {
      char*  pos;
      pos    = strchr(pipe,'|');
      if(pos==NULL) 
	{
	  retval = DoCommand (This,pipe);
	  break;
	}
      else
	{ 
	  *pos   = '\0';
	  retval = DoCommand (This,pipe);
          if(retval<=OProcessExit) break;
	  pipe             = pos + 1;
	  This->pipeActive = 1;
	}                     
    }
  CStringDelete (string);

  sout               = GetPipe(This);

  This->pipeActive   = 0;
  OProcessPutHandles (This,NULL); /*Clear pipe.*/

  if(a_sout!=NULL)   *a_sout       = sout;
  else               CStringDelete (sout);
  return             retval; 
}
/***************************************************************************/
static int DoCommand (
 OProcess This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  char*  fname  = NULL;
  FILE*  file   = NULL;
  int    retval = EXIT_SUCCESS;
/*.........................................................................*/
  if(This==NULL)     return retval;
  if(a_string==NULL) return retval;
/*printf ("debug : cmd : %s|\n",a_string);*/
  string        = CStringDuplicate(a_string);
  if( (string!=NULL) && (*string!='\0'))
    {
      char* pos;
      char* next;  
      next  = string;
      pos   = strchr (string,'<');
      if(pos!=NULL)
        {
          char*  name;
	  FILE*  fin;
          *pos           = '\0';
	  next           = strchr (pos+1,'>');
	  if(next!=NULL) *next = '\0';
	  name           = CStringDuplicate(pos+1);
	  CStringTrim    (name);
	  fin            = CFileOpenForReading (name,NULL);
	  if(fin!=NULL)
            {
	      char*          buff;
	      unsigned long  filesize;
	      buff           = CStreamRead (fin,&filesize);
              fclose         (fin);
	      OProcessPutString (This,buff);
	    }
	  CStringDelete  (name);
	  if(next!=NULL) *next = '>';
	}
      pos   = next!=NULL ? strchr (next,'>') : CStringNULL;
      if(pos!=NULL)
        {
          *pos        = '\0';
	  pos++;
          if((*pos)=='>')
            {
	      fname       = CStringDuplicate(pos+1);
	      CStringTrim (fname);
	      file        = CFileOpenForAppending (fname);
	    }
	  else
            {
	      fname       = CStringDuplicate(pos);
	      CStringTrim (fname);
	      file        = CFileOpenForWriting   (fname);
	    }
	  if(file==NULL) 
	    {
	      CPrinterPutF ("Co/src/OProcess.c/DoCommand :\nCan't open %s (check : path,existence,quotas,protections).\n",fname);
	    }
	}
    }
  CStringTrim (string);
  if( (string!=NULL) && (*string!='\0'))
    {
      int    argi;
      int    argn;
      char** args;
      args   = CStringGetWords (string," ",&argn);
/*Args that are return of other commands.*/
      for(argi=0;argi<argn;argi++)
        {
	  if(args[argi]!=NULL)
            {
	      char* pos;
              pos   = strstr(args[argi],"$$$"); /*dollar*/
              if(pos!=NULL)
                {
		  int           count;
		  char*         s;
		  count         = (int)strtol(pos+3,&s,10);
		  if(s!=pos+3)
                    {
		      char*         sval;
		      *pos          = '\0';
                      sval          = CStringDuplicate(args[argi]);
                      CStringConcatenate    (&sval,This->args[count]);
                      CStringConcatenate    (&sval,s);
		      CStringDelete (args[argi]);
		      args[argi]    = sval;
		    }
		} 
              pos   = strstr(args[argi],"$_$"); /*Return of other command.*/
              if(pos!=NULL)
                {
		  int           count;
		  char*         s;
		  count         = (int)strtol(pos+3,&s,10);
		  if(s!=pos+3)
                    {
		      char*         sval;
		      char*         sout;
		      *pos          = '\0';
                      sval          = CStringDuplicate(args[argi]);
		      retval        = DoPipe (This,This->args[count],&sout);
                      CStringConcatenate    (&sval,sout);
                      CStringConcatenate    (&sval,s);
		      CStringDelete (sout);
		      CStringDelete (args[argi]);
		      args[argi]    = sval;
		    }
		} 
	    }
	}
/*Args that are value of shell variables.*/
      for(argi=0;argi<argn;argi++)
        {
	  if( (args[argi]!=NULL) && (*(args[argi])=='$') )
            {
	      char* pos;
              pos   = args[argi]+1;
              if(pos!=NULL)
                {
		  int count;
		  int found = 0;
		  for(count=0;count<This->varn;count++)
		    {
		      if(strcmp(pos,This->vars[count])==0) 
			{
			  found=1;
			  break;
			}
		    }
		  if(found==1)
                    {
		      CStringDelete (args[argi]);
		      args[argi]    = CStringDuplicate(This->vals[count]);
		    }
		} 
	    }
	}
/*Exec cmd.*/
      if( (argn!=0) && (args!=NULL) )
        {
	  char*  scmd;
	  scmd   = CTextConvertToString (argn,args," ");
	  if(This->set_x!=0)  CInfoF ("%s\n",scmd);
	  if(SetVariable(This,scmd)==0)
	    {
	      OCommand cmd;
	      cmd      = OShellGetCommandIdentifier ((OShell)OProcessGetShell(This),args[0]);
	      if(cmd!=NULL)  
		{
		  OCommandExecuteFunction proc;
		  This->command           = scmd;
		  proc                    = OCommandGetExecuteFunction (cmd);
		  if(proc!=NULL) 
		    {
		      Class.argn    = argn;
		      Class.args    = args;
		      if(CTextCountString(argn,args,"-")>1)
			{
			  CWarnF  ("One pipe symbol expected in :\n%s\n",scmd);
			  retval  = OProcessExit;
			}
		      else
			{
			  retval  = proc (argn,args,This);
			}
		    }
		  This->command  = NULL;
		}
	      else
		{
		  CWarnF  ("Unknown command %s in :\n%s\n",args[0],scmd);
		  retval = OProcessExit;
		}
	    }
	  CStringDelete (scmd);
	}
      CTextDelete (argn,args);
    }
  CStringDelete (string);

  CStringDelete (fname);
  if(file!=NULL)
    {
      char*          sout;
      sout           = GetPipe(This);
      if(sout!=NULL) fprintf (file,"%s",sout);
      fclose         (file);
      CStringDelete  (sout);
      OProcessPutHandles  (This,NULL); /*Clear pipe.*/
    }

  return        retval;
}
/***************************************************************************/
char* OProcessGetCommand (
 OProcess  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->command;
}
/***************************************************************************/
int OProcessIsPipeActive (
 OProcess  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->pipeActive;
}
/***************************************************************************/
char* OProcessGetAndConvertHandlesToString (
 OProcess  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string = NULL;
  OHandle* itema;
/*.........................................................................*/
  if(This==NULL) return NULL;
  if(This->pipeHands==NULL) return NULL;
  itema = This->pipeHands;
  while(*itema!=NULL)
    {
      if(OHandleGetType(*itema)!=NULL) 
	{
	  char* shand;
          shand = OTypeConvertTypeAndIdentifierToString 
	          ((OType)OHandleGetType(*itema),
                   OHandleGetIdentifier(*itema));
	  CStringConcatenate    (&string,shand);
	  CStringConcatenate    (&string,"\n");
	  CStringDelete (shand);
	}
      else if(OHandleGetIdentifier(*itema)!=NULL) 
	{
	  CStringConcatenate (&string,(char*)OHandleGetIdentifier(*itema));
	  CStringConcatenate (&string,"\n");
	}
      itema++;
    }          
  if(string!=NULL)
    {
      int              length;
      length           = strlen(string);
      string[length-1] = '\0'; /*Remove last \n*/ 
    }
  return           string;
}
/***************************************************************************/
static int SetVariable (
 OProcess This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
/*.........................................................................*/
  if(This==NULL)     return 0;
  if(a_string==NULL) return 0;
/*printf ("debug : SetVariable : %s|\n",a_string);*/
  string      = CStringDuplicate(a_string);
  if( (string!=NULL) && (*string!='\0'))
    {
      char* pos;
      pos   = strchr (string,'=');
      if(pos!=NULL)
        {
	  char*       value;
          *pos        = '\0';
	  value       = pos+1;
	  pos         = strchr (string,' ');
	  if( (pos==NULL) && (value!=NULL) )
	    {
	      int   count;
	      count = CTextGetLineIndex (This->varn,This->vars,string);
              if(count!=NotFound)
                {
		  CStringDelete     (This->vals[count]);
		  This->vals[count] = CStringDuplicate(value);
		}
	      else
                {
		  CTextAddLine (&(This->varn),&(This->vars),string);
		  CTextAddLine (&(This->valn),&(This->vals),value);
		}
	      CStringDelete (string);
	      return        1;
	    }
	}
    }
  CStringDelete (string);
  return        0;
}
/***************************************************************************/
static char* GetPipe (
 OProcess  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string = NULL;
  OHandle* itema;
/*.........................................................................*/
  if(This==NULL) return NULL;
  for(itema = This->pipeHands;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OHandleGetType(*itema)!=NULL) 
	{
	  char*         shand;
	  shand         = OTypeConvertTypeAndIdentifierToString 
	                  ((OType)OHandleGetType(*itema),OHandleGetIdentifier(*itema));
	  CStringConcatenate    (&string,shand);
	  CStringConcatenate    (&string,"\n");
	  CStringDelete (shand);
	}
      else if(OHandleGetIdentifier(*itema)!=NULL) 
	{
	  CStringConcatenate (&string,(char*)OHandleGetIdentifier(*itema));
	  CStringConcatenate (&string,"\n");
	}
    }          
  if(string!=NULL)
    {
      int              length;
      length           = strlen(string);
      string[length-1] = '\0'; /*Remove last \n*/ 
    }
  return           string;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OProcessPreparePipe (
 OProcess This
,char*    a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return 0;
  if(a_string==NULL) return 0;
  if(strcmp(a_string,"-")==0)
    {
      ConvertPipeStringsToHandles (This);
    }
  else
    {
      if(PutStringInPipe(This,a_string)==0) return 0;
    }    
  return 1;
}
/***************************************************************************/
static int PutStringInPipe (
 OProcess This
,char*    a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int status;
/*.........................................................................*/
  status = 1;
  if(a_string==NULL)
    {
      status = 0;
    }
  else
    {
      OType       otype;
      OIdentifier identifier;
      otype       = OTypeConvertStringToTypeAndIdentifier (a_string,&identifier);
      if(otype!=NULL)  /*Admit class identifier.*/
	{
	  OHandle* hands;
	  hands     = (OHandle*)CListCreate(1);
	  if(hands!=NULL)
	    {
	      hands[0] = OHandleCreate(otype,identifier);
	      hands[1] = NULL;
	      OProcessPutHandles (This,hands);
	    }
	  else
	    {
	      status = 0;
	    }
	}
      else
	{
	  CWarnF ("Unknown object %s in :\n%s\n",a_string,OProcessGetCommand(This));
	  status = 0;
	}
    }
  if(status==0) OProcessPutHandles (This,NULL);
  return status;
}
/***************************************************************************/
static void ConvertPipeStringsToHandles (
 OProcess This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  int    linen = 0;
  char** lines = NULL;
/*.........................................................................*/
  string = OProcessGetString(This,"-");
  if(string==NULL) return;

  lines  = CTextCreateFromString (string,&linen);

  if( (linen!=0) && (lines!=NULL) )
    {
      int       handn = 0;
      OHandle* hands = NULL;
      hands  = (OHandle*)CListCreate(linen);
      handn  = 0;
      if(hands!=NULL)
	{
	  int count;
	  for(count=0;count<linen;count++)
	    {
	      OType       otype;
	      OIdentifier identifier;
	      otype       = OTypeConvertStringToTypeAndIdentifier (lines[count],&identifier);
	      if( (otype!=NULL) && (identifier!=NULL) )
		{
		  hands[handn] = OHandleCreate(otype,identifier);
		  handn++;
		}
	      else
		{
		  CWarnF("Unknown object %s.\n",lines[count]);
		}
	    }	
	  hands[handn] = NULL;
	  OProcessPutHandles (This,hands);
	}
      else
        {
	  OProcessPutHandles (This,NULL);
	}
    }
  else
    {
      OProcessPutHandles (This,NULL);
    }

  CTextDelete (linen,lines);
}
/***************************************************************************/
CList OProcessGetTypedObjects (
 OProcess This
,char* a_string
,char* a_type
)
/*****************************************************************6**********/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList list = NULL;
  OType type;
/*.........................................................................*/
  if(a_string==NULL) return NULL;
  if(a_type==NULL)   return NULL;
  type = OTypeGetIdentifier(a_type);
  if(type==NULL) 
    {
      CWarnF ("Type %s is unknown in :\n%s\n",a_type,OProcessGetCommand(This));
      return NULL;
    }
  if(strcmp(a_string,"-")==0)
    {
      OHandle* handa;
      if(OProcessPreparePipe(This,a_string)==0) return NULL;
      for(handa = OProcessGetHandles(This);(handa!=NULL) && (*handa!=NULL);handa++)
	{
	  OType  otype;
	  otype = (OType)OHandleGetType(*handa);
	  if(otype==type)
	    {
	      CListAddEntry (&list,OHandleGetIdentifier(*handa));
	    }
	}
    }
  else
    {
      int    count;
      int    linen = 0;
      char** lines = NULL;
      lines  = CTextCreateFromString (a_string,&linen);
      for(count=0;count<linen;count++) 
	{
	  OIdentifier  oid;
	  oid          = OTypeGetObjectIdentifier (type,lines[count]);
	  if(oid!=NULL)
	    {
	      CListAddEntry (&list,oid);
	    }
	  else
	    {
	      OType otype;
	      otype = OTypeConvertStringToTypeAndIdentifier (lines[count],&oid);
	      if( (otype==type) && (oid!=NULL) )
		{
		  CListAddEntry (&list,oid);
		}
	    }
	}
      CTextDelete  (linen,lines);
    }
  return list;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static char* GetSubstring (
 char* a_string
,char  a_sep
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* beg;
  char* end;
  char* string;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return NULL;
  beg      = strchr(a_string,a_sep);
  if(beg==NULL) return NULL;
  end      = strchr(beg+1,a_sep);
  if(end==NULL) return NULL;
  string   = CStringDuplicate(beg);
  end      = strchr(string+1,a_sep);
  *(end+1) = '\0';
  return   string;
}
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
  CErrorHandle ("Co","OProcess",a_name,a_code);
}
