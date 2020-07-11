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
#include <stdio.h>
#include <math.h>

#include <CMemory.h>
#include <CTime.h>
#include <CString.h>
#include <CSystem.h>
#include <CPrinter.h>
#include <CText.h>
#include <CList.h>
#include <CStream.h>
#include <CFile.h>
#include <CPS.h>
#include <CBatch.h>
#include <CManager.h>
#include <OProcess.h>
#include <OType.h>
#include <OHandle.h>
#include <OHTML.h>
#include <CoTypes.h>

#include <CoCommands.h>

#ifdef __cplusplus
extern "C"{
#endif
static int    Collect             (int*,OIdentifier**,OType,OProcess,char*,char*,char*,char*);
static int    GetComparator       (char*);
static char*  AskPassword         (char*);
static void   TreatCarriageReturn (char**);
#ifdef __cplusplus
}
#endif

static struct 
{
  char*  password;
} Class = {NULL};
/***************************************************************************/
void CoAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("echo")!=NULL) return; /*done*/

  CoSetTypes ();

/*UNIX like commands.*/
  OShellAddNewCommand (a_osh,"Co/."       ,CoExecute_dot);
  OShellAddNewCommand (a_osh,"Co/echo"    ,CoExecute_echo);
  OShellAddNewCommand (a_osh,"Co/exit"    ,CoExecute_exit);
  OShellAddNewCommand (a_osh,"Co/cat"     ,CoExecute_cat);
  OShellAddNewCommand (a_osh,"Co/rm"      ,CoExecute_rm);
  OShellAddNewCommand (a_osh,"Co/printf"  ,CoExecute_printf);
  OShellAddNewCommand (a_osh,"Co/sort"    ,CoExecute_sort);
  OShellAddNewCommand (a_osh,"Co/test"    ,CoExecute_test);
  OShellAddNewCommand (a_osh,"Co/date"    ,CoExecute_date);
  OShellAddNewCommand (a_osh,"Co/printenv",CoExecute_printenv);
  OShellAddNewCommand (a_osh,"Co/expr"    ,CoExecute_expr);

/*Type manager commands.*/
  OShellAddNewCommand (a_osh,"Co/create"     ,CoExecute_create);
  OShellAddNewCommand (a_osh,"Co/load"       ,CoExecute_load);
  OShellAddNewCommand (a_osh,"Co/collect"    ,CoExecute_collect);
  OShellAddNewCommand (a_osh,"Co/delete"     ,CoExecute_delete);
  OShellAddNewCommand (a_osh,"Co/destroy"    ,CoExecute_destroy);
  OShellAddNewCommand (a_osh,"Co/save"       ,CoExecute_save);
  OShellAddNewCommand (a_osh,"Co/dump"       ,CoExecute_dump);
  OShellAddNewCommand (a_osh,"Co/set"        ,CoExecute_set);
  OShellAddNewCommand (a_osh,"Co/get"        ,CoExecute_get);
  OShellAddNewCommand (a_osh,"Co/do"         ,CoExecute_do);
  OShellAddNewCommand (a_osh,"Co/isValid"    ,CoExecute_isValid);
  OShellAddNewCommand (a_osh,"Co/isNotValid" ,CoExecute_isNotValid);
  OShellAddNewCommand (a_osh,"Co/count"      ,CoExecute_count);
  OShellAddNewCommand (a_osh,"Co/empty"      ,CoExecute_empty);
  OShellAddNewCommand (a_osh,"Co/clear"      ,CoExecute_clear);

  OShellAddNewCommand (a_osh,"Co/osh"                  ,CoExecute_osh);
  OShellAddNewCommand (a_osh,"Co/isCommand"            ,CoExecute_isCommand);
  OShellAddNewCommand (a_osh,"Co/makeHTML"             ,CoExecute_makeHTML);
  OShellAddNewCommand (a_osh,"Co/buildFileName"        ,CoExecute_buildFileName);
  OShellAddNewCommand (a_osh,"Co/addFilePath"          ,CoExecute_addFilePath);
  OShellAddNewCommand (a_osh,"Co/help"                 ,CoExecute_help);
  OShellAddNewCommand (a_osh,"Co/getTopic"             ,CoExecute_getTopic);
  OShellAddNewCommand (a_osh,"Co/spawn"                ,CoExecute_spawn);
  OShellAddNewCommand (a_osh,"Co/makeMakefile"         ,CoExecute_makeMakefile);

  /*On text.*/
  /*sort*/
  OShellAddNewCommand (a_osh,"Co/getStringLength"      ,CoExecute_getStringLength);
  OShellAddNewCommand (a_osh,"Co/matchString"          ,CoExecute_matchString);
  OShellAddNewCommand (a_osh,"Co/prependString"        ,CoExecute_prependString);
  OShellAddNewCommand (a_osh,"Co/lowerString"          ,CoExecute_lowerString);
  OShellAddNewCommand (a_osh,"Co/raiseString"          ,CoExecute_raiseString);
  OShellAddNewCommand (a_osh,"Co/getStringWord"        ,CoExecute_getStringWord);
  OShellAddNewCommand (a_osh,"Co/removeLastStringWord" ,CoExecute_removeLastStringWord);
  OShellAddNewCommand (a_osh,"Co/replaceStringPart"    ,CoExecute_replaceStringPart);
  OShellAddNewCommand (a_osh,"Co/cutStringBegin"       ,CoExecute_cutStringBegin);
  OShellAddNewCommand (a_osh,"Co/removeDuplicates"     ,CoExecute_removeDuplicates);
  OShellAddNewCommand (a_osh,"Co/getLine"              ,CoExecute_getLine);
  OShellAddNewCommand (a_osh,"Co/convertToString"      ,CoExecute_convertToString);
  OShellAddNewCommand (a_osh,"Co/putStringInPipe"      ,CoExecute_putStringInPipe);

  /*Else*/
  OShellAddNewCommand (a_osh,"Co/stdout"               ,CoExecute_stdout);
  OShellAddNewCommand (a_osh,"Co/calculate"            ,CoExecute_calculate);

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int CoExecute_dot (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> . <string:file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  OProcessExecuteFile (a_process,a_args[1]);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_echo (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> echo
  osh> echo <string:message>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if((a_argn!=1) && (a_argn!=2)) 
    {
      CWarnF("Zero or one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  string              = a_argn==1 ? CStringDuplicate("\n") : CStringDuplicate(a_args[1]);

  TreatCarriageReturn (&string);

  OProcessPutString   (a_process,string);

  return         EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_stdout (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> stdout
  osh> stdout <string:message>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if((a_argn!=1) && (a_argn!=2)) 
    {
      CWarnF("Zero or one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  string              = a_argn==1 ? CStringDuplicate("\n") : CStringDuplicate(a_args[1]);

  TreatCarriageReturn (&string);

  printf              ("%s\n",string);

  CStringDelete       (string);

  return         EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_exit (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> exit
  osh> exit <integer:status>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int retval = EXIT_SUCCESS;
/*.........................................................................*/
  if(a_argn==1)
    {
      retval = OProcessExit;
    }
  else if(a_argn==2)
    {
      int           status;
      retval        = OProcessExit - (int)labs(CStringConvertToLong(a_args[1],&status));
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
    }
  else
    {
      CWarnF("Zero or one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  return retval;
}
/***************************************************************************/
int CoExecute_cat (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> cat <string:file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string = NULL;
  char*          fname;
  FILE*          file;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString  (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(CSystemIsSecured()==1)
    {
      CWarnF            ("System is secured, can't execute :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  fname          = OProcessGetString   (a_process,a_args[1]);

#ifdef DEBUG
  printf ("debug : CoExecute_cat : try to read %s.\n",fname);
#endif

  file           = CFileOpenForReading (fname,NULL);
  if(file!=NULL)
    {
      unsigned long  filesize;
      string         = CStreamRead (file,&filesize);
      fclose         (file);
    }

#ifdef DEBUG
  printf ("debug : CoExecute_cat : end.\n");
#endif

  OProcessPutString  (a_process,string);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_rm (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> rm <string:file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(CSystemIsSecured()==1)
    {
      CWarnF ("System is secured, can't execute :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(remove(a_args[1])!=0)
    {
      CWarnF ("Can't remove file %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_printf (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> printf <string:format>
  osh> printf <string:format> <xxx:argument>,...
 To insert a character use \<octal> in the format.
 Some usefull octal codes are :
  \12 = carriage return
  \47 = '
  \44 = $
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* format;
  char* beg;
  char* string;
  int   argi;
/*.........................................................................*/
  if(a_argn<=1) 
    {
      CWarnF("At least one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  beg    = format = CStringDuplicate (a_args[1]);
  argi   = 2;
  string = NULL;
  while(1)
    {
      char* pos;
      char* pos1;
      char* pos2;
      int   doi;
      if(beg==NULL) break;
      pos1  = strchr(beg,'%');
      pos2  = strchr(beg,'\\');
      if( (pos1==NULL) && (pos2==NULL) )
	{ /*End*/
	  CStringConcatenate (&string,beg);
	  break;
	}
      if(pos1==NULL)
	{
	  pos = pos2;
	  doi = 2;
	}
      else if(pos2==NULL)
	{
	  pos = pos1;
	  doi = 1;
	}
      else
	{
	  if(pos1<pos2)
	    {
	      pos = pos1;
	      doi = 1;
	    }
	  else
	    {
	      pos = pos2;
	      doi = 2;
	    }
	}
      if(doi==1)
	{
	  if(argi>=a_argn)
	    {
	      CWarnF        ("Bad arg number for format (%s).\n\
At least %d arguments expected in :\n%s\n",format,a_argn+1,OProcessGetCommand(a_process));
	      CStringDelete (string);
	      string        = NULL;
	      break;
	    }
	  else
	    {
	      pos++;
	      if(*pos=='\0')
		{
		  CWarnF        ("Bad format (%s) in :\n%s\n",format,OProcessGetCommand(a_process));
		  CStringDelete (string);
		  string        = NULL;
		  break;
		}
	      if( (*pos=='s') ||
	          (*pos=='d') ||
	          (*pos=='g') 
		  )
		{
		  *(pos-1) = '\0';
		  CStringConcatenate (&string,beg);
		  CStringConcatenate (&string,a_args[argi]);
		  *(pos-1) = '%'; /* for better error message */ 
		  argi++;
		  beg = pos + 1;
		}
	      else if( (*pos=='l') && (*(pos+1)=='d') )
		{
		  *(pos-1) = '\0';
		  CStringConcatenate (&string,beg);
		  CStringConcatenate (&string,a_args[argi]);
		  *(pos-1) = '%';
		  argi++;
		  beg = pos + 2;
		}
	      else
		{
		  int  error = 1;
		  char last;
		  pos1 = pos;
		  while ( *pos1!='\0' &&
			  (*pos1=='.' || (*pos1>='0' && *pos1<='9') ||
			   *pos1=='l' || *pos1=='+'  || *pos1=='-') ) 
		    pos1++;
		  pos2  = pos1 + 1;
		  last  = *pos2;
		  *pos2 ='\0';
		  if( (*pos1=='s') )
		    {
		      char result[256];
		      sprintf(result,pos-1,a_args[argi]);
		      *(pos-1) = '\0';
		      CStringConcatenate (&string,beg);
		      CStringConcatenate (&string,result);
		      *(pos-1) = '%';
		      argi++;
		      beg   = pos2;
		      *beg  = last;
		      error = 0;
		    }
		  else if( (*pos1=='x') || (*pos1=='d') )
		    {
                      char*  s;
		      long   lvalue = 0;
		      lvalue = strtol(a_args[argi],&s,10);
		      if(s!=a_args[argi])
			{
			  char     result[64];
			  sprintf  (result,pos-1,lvalue);
			  *(pos-1) = '\0';
			  CStringConcatenate (&string,beg);
			  CStringConcatenate (&string,result);
			  *(pos-1) = '%';
			  argi++;
			  beg   = pos2;
			  *beg  = last;
			  error = 0;
			}
		    }
		  else if (*pos1=='g' || *pos1=='f') 
		    {
                      char*  s;
		      double value;
		      value  = strtod(a_args[argi],&s);
		      if(s!=a_args[argi])
			{
			  char     result[64];
			  sprintf  (result,pos-1,value);
			  *(pos-1) = '\0';
			  CStringConcatenate (&string,beg);
			  CStringConcatenate (&string,result);
			  *(pos-1) = '%';
			  argi++;
			  beg   = pos2;
			  *beg  = last;
			  error = 0;
			}
		    }
		  if (error==1)
		    {
		      CWarnF        ("Bad format (%s) in :\n%s\n",format,OProcessGetCommand(a_process));
		      CStringDelete (string);
		      string        = NULL;
		      break;
		    }
		} 
	    }
	}       
      else if(doi==2)      
	{
	  int   value;
	  char* s;
	  char  schar[8];
	  *pos  = '\0';
	  pos++;
	  value = (int)strtol(pos,&s,10);
	  if(s==pos)
	    {
	      CWarnF        ("Bad format (%s) in :\n%s\n",format,OProcessGetCommand(a_process));
	      CStringDelete (string);
	      string        = NULL;
	      break;
	    }
	  sprintf            (schar,"%d",value);
	  sscanf             (schar,"%o",&value);
	  sprintf            (schar,"%c",(char)value);
	  CStringConcatenate (&string,beg);
	  CStringConcatenate (&string,schar);
	  beg                = s;
	}
    }

  CStringDelete (format);

  TreatCarriageReturn   (&string);

  if(string!=NULL) 
    {
      OProcessPutString  (a_process,string);
      return             EXIT_SUCCESS;
    }
  else
    {
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }
}
/***************************************************************************/
int CoExecute_test (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> test <string> eq <string>
  osh> test <string> ne <string>
  osh> test <string> =  <string>
  osh> test <string> != <string>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=4) 
    {
      CWarnF("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

/*printf ("debug:%s:%s:\n",a_args[1],a_args[3]);*/

  if( (strcmp(a_args[2],"eq")==0) || 
      (strcmp(a_args[2],"=")==0) 
     )
    {
       if(CStringCompare(a_args[1],a_args[3])==1) return EXIT_SUCCESS;
       else                                       return EXIT_FAILURE;
    }
  else if(
	  (strcmp(a_args[2],"ne")==0) ||
	  (strcmp(a_args[2],"!=")==0)
	  )
    {
       if(CStringCompare(a_args[1],a_args[3])==1) return EXIT_FAILURE;
       else                                       return EXIT_SUCCESS;
    }
  else
    {
      CWarnF("Bad second argument %s in :\n%s\nMust be [eq, ne, =, !=].\n",
	     a_args[2],OProcessGetCommand(a_process));
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_date (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> date
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if (a_argn!=1) 
    {
      CWarnF("Zero argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  OProcessPutString (a_process,CStringDuplicate(CTimeGetDate()));

  a_args = NULL;
  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_printenv (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> printenv <string>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if (a_argn<2) 
    {
      CWarnF("One argument expected in:\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  OProcessPutString (a_process,CStringDuplicate(getenv(a_args[1])));

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_expr (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> expr <integer:number> <string:operation> <integer:number>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  long number_1,number_2;
  int  status;
/*.........................................................................*/
  if (a_argn!=4) 
    {
      CWarnF("Three arguments expected in:\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  number_1 = CStringConvertToLong(a_args[1],&status);
  if(status==0) 
    {
      CWarnF              ("Argument %s not a number in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }
  number_2 = CStringConvertToLong(a_args[3],&status);
  if(status==0) 
    {
      CWarnF              ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  if(strcmp(a_args[2],"+")==0)
    {
      OProcessPutString (a_process,CStringCreateF(64,"%ld",number_1 + number_2));
    }
  else if(strcmp(a_args[2],"-")==0)
    {
      OProcessPutString (a_process,CStringCreateF(64,"%ld",number_1 - number_2));
    }
  else if(strcmp(a_args[2],"*")==0)
    {
      OProcessPutString (a_process,CStringCreateF(64,"%ld",number_1 * number_2));
    }
  else if(strcmp(a_args[2],"/")==0)
    {
      if(number_2==0L)
        {
	  CWarnF              ("Can't divide by zero in :\n%s\n",OProcessGetCommand(a_process));
	  OProcessPutHandles  (a_process,NULL);
	  return              EXIT_FAILURE;
	}
      else
	{
	  OProcessPutString (a_process,CStringCreateF(64,"%ld",(long)(number_1/number_2)));
	}
    }
  else
    {
      CWarnF              ("Unknown operator %s (+, - , * , / expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles  (a_process,NULL);
      return              EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/********* Using OType *****************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int CoExecute_create (
 int    a_argn
,char** a_args
,OProcess a_process
)
/***************************************************************************/
/*
  osh> create <string:type>
  osh> create <string:type> {<string:property> <xxx:value>}
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OType       otype;
  int         argn;
  int         parn;
  char**      pars = NULL;
  char**      vals = NULL;
  OIdentifier obj  = NULL;
/*.........................................................................*/
  if(a_argn<2) 
    {
      CWarnF("At least one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString  (a_process,NULL);
      return EXIT_FAILURE;
    }

  argn = a_argn - 2;
  parn = argn/2;
  if((2*parn)!=argn)
    {
       CWarnF ("We must have an odd number of arguments in :\n%s\n",OProcessGetCommand(a_process));
       OProcessPutString  (a_process,NULL);
       return EXIT_FAILURE;
    }  

  otype = OTypeGetIdentifier (a_args[1]);
  if(otype==NULL) 
    {
       CWarnF ("Unknown type %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
       OProcessPutString  (a_process,NULL);
       return EXIT_FAILURE;
    }

  parn = 0;
  for(count=2;count<a_argn;count+=2)
    {
      CListAddEntry  ((CList*)&pars,a_args[count]);                            
      CListAddEntry  ((CList*)&vals,a_args[count+1]);
      parn          ++;
    }

  if(OTypeGetMakeFunction(otype)!=NULL)
    {
      obj = OTypeMakeObject (otype);
      for(count=0;count<parn;count++)  /*Set property that are not references.*/
	{
	  OProperty prop;
	  prop      = OTypeGetPropertyIdentifier (otype,pars[count]);
	  if(prop==NULL)
	    {
	      CWarnF ("%s is not a property of %s in :\n%s\n",pars[count],a_args[1],OProcessGetCommand(a_process));
	    }
	  else if( (OPropertyIsListOfReferences(prop)==0) &&
		  (OPropertyIsMethod(prop)==0)
		  )
	    {
	      OTypeSetObjectPropertyFromString (otype,obj,prop,vals[count]);
	    }
	}
    }
  else
    {
      obj = OTypeConstructObject (otype,parn,pars,vals);	
    }

  CListDelete        ((CList)pars);
  CListDelete        ((CList)vals);

  if(obj!=NULL)
    {
      OProcessPutString  (a_process,OTypeConvertTypeAndIdentifierToString (otype,obj));
    }
  else
    {
      OProcessPutString  (a_process,NULL);
    }

  return EXIT_FAILURE;
}
/***************************************************************************/
int CoExecute_load (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> load <string:file name>
  osh> load <string:file name> <string:file type>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* fname = NULL;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=3) )
    {
      CWarnF("One or two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  fname = OProcessGetString(a_process,a_args[1]);

  if(a_argn==2)
    {
      OProcessPutHandles (a_process,OTypeGetExtentFromFile(fname,NULL,NULL)); /*Type is taken from .suffix in file name.*/
    }
  else if(a_argn==3)
    {
      OProcessPutHandles (a_process,OTypeGetExtentFromFile(fname,a_args[2],NULL));
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_collect (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> collect <string:type> 
  osh> collect <string:type> where <string:property> {=,<>,>,<,>=,<=} <xxx:value>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int          argn;
  OType        wtype;
  int          handn = 0;
  OHandle*     hands = NULL;
  int          objn  = 0;
  OIdentifier* objs  = NULL;
  int          count;
/*.........................................................................*/
  if(a_argn<2)
    {
      CWarnF("At least one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }
  argn = (a_argn-2)/4;
  if(4*argn!=a_argn-2)
    {
      CWarnF("Bad argument number in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  wtype = OTypeGetIdentifier (a_args[1]);
  if(wtype==NULL) 
    {
      CWarnF("Unknown type %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,"-")==0) return EXIT_FAILURE;

  hands   = OProcessGetHandles(a_process);
  handn   = CListGetSize((CList)hands);
  if(handn!=0)
    {
      objs = (OIdentifier*)CListCreate(handn);
      if(objs!=NULL)
        {
	  objn = 0;
	  for(count=0;count<handn;count++)
	    {
	      if((OType)OHandleGetType(hands[count])==wtype)
                {
		  objs[objn] = OHandleGetIdentifier(hands[count]);
		  objn++;
		}
	    }
	  objs[objn] = NULL;
	}
    }
  else if(OProcessIsPipeActive(a_process)==0)
    {
      objs = CListDuplicate((CList)OTypeGetObjectIdentifiers(wtype));
      objn = CListGetSize((CList)objs);
    }
  
  for(count=2;count<a_argn;count+=4)
    {
      if(Collect(&objn,&objs,wtype,a_process,
		 a_args[count],a_args[count+1],a_args[count+2],a_args[count+3])==0) break;  
    }

  if(objn!=0)
    {
      hands  = (OHandle*)CListCreate(objn);
      if(hands!=NULL)
	{
	  handn  = objn;
	  for(count=0;count<objn;count++) hands[count] = OHandleCreate (wtype,objs[count]);
	}	
      OProcessPutHandles (a_process,hands);
    }
  else
    {
      OProcessPutHandles (a_process,NULL);
    }

  CListDelete ((CList)objs);

  return EXIT_SUCCESS;
}
/***************************************************************************/
static int Collect (
 int*          a_objn
,OIdentifier** a_objs
,OType         a_type
,OProcess      a_process
,char*         a_word
,char*         a_prop
,char*         a_cmp
,char*         a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty    oprop;
  int          status = 1;
/*.........................................................................*/
  oprop = OTypeGetPropertyIdentifier (a_type,a_prop);
  if(oprop==NULL) 
    {
      CWarnF      ("Unknown property %s for type %s in :\n%s\n",
		   a_prop,OTypeGetName(a_type),OProcessGetCommand(a_process));
      CListDelete ((CList)*a_objs);
      *a_objs     = NULL;
      *a_objn     = 0;
      status      = 0;
    }
  else if(OPropertyIsMethod(oprop)==0)
    {
      int   icmp;
      icmp  = GetComparator (a_cmp);
      if(icmp==0) 
	{
	  CWarnF      ("Unknown operator %s in :\n%s\n",a_cmp,OProcessGetCommand(a_process));
	  CListDelete ((CList)*a_objs);
	  *a_objs     = NULL;
	  *a_objn     = 0;
	  status      = 0;
	}
      else
	{
	  OProperty        otmp;
	  char*            sval;
	  int              number;
	  void*            variable;
	  if( (icmp==IF_IN) || (icmp==IF_NI) ) 
	    {
	      otmp                = OPropertyCreate  ("tmp",OPropertyArrayOfStrings ,NULL);
	      sval                = CStringDuplicate (a_value);
	      TreatCarriageReturn (&sval);
	    }
	  else
	    {
	      otmp  = oprop;
	      sval  = a_value;
	    }
	  variable = OPropertyGetVariable   (otmp);
	  if(OPropertySetVariableFromString(otmp,sval,variable,&number)==1)
	    {
	      int          robjn  = 0;
	      OIdentifier* robjs  = NULL;
	      robjs        = OTypeCollectObjects (
			     a_type,*a_objn,*a_objs,oprop,icmp,variable,number,&robjn);
	      CListDelete  ((CList)*a_objs);
	      *a_objn      = robjn;
	      *a_objs      = robjs;
	    }
	  else
	    {
	      CListDelete ((CList)*a_objs);
	      *a_objs     = NULL;
	      *a_objn     = 0;
              status      = 0;
	    }
	  if( (icmp==IF_IN) || (icmp==IF_NI) ) 
	    {
	      OPropertyDelete (otmp);
	      CStringDelete   (sval);
	    }
	}
    }
  a_word = NULL;
  return status;
}
/***************************************************************************/
int CoExecute_delete (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> delete -
  osh> delete <string:oid>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHandle* hands;
  OHandle* handa;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  hands = OProcessGetHandles(a_process);
  for(handa=hands;(handa!=NULL)&&(*handa!=NULL);handa++) 
    { 
      OTypeDeleteObject ((OType)OHandleGetType(*handa),(OIdentifier)OHandleGetIdentifier(*handa));
    }

  OProcessPutString  (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_destroy (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> destroy -
  osh> destroy <string:oid>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHandle* hands;
  OHandle* handa;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  hands = OProcessGetHandles(a_process);
  for(handa=hands;(handa!=NULL)&&(*handa!=NULL);handa++) 
    { 
      OTypeDestroyObject ((OType)OHandleGetType(*handa),(OIdentifier)OHandleGetIdentifier(*handa));
    }

  OProcessPutString  (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_save (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> save -            <string:file name>
  osh> save <string:oid> <string:file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*     file;
  OHandle* hands;
  OHandle* handa;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  file      = (FILE*)OTypeOpenODB_FileForWriting (a_args[2]);
  if(file==NULL) 
    {
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE; 
    }

  hands = OProcessGetHandles(a_process);
  for(handa=hands;(handa!=NULL)&&(*handa!=NULL);handa++) 
    { 
      OTypePutObjectInODB_Stream ( (OType)OHandleGetType (*handa),
				  (OIdentifier)OHandleGetIdentifier(*handa),file);
    }

  fclose       (file);

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_dump (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> dump -
  osh> dump -            {<string:property>,...}
  osh> dump <string:oid>
  osh> dump <string:oid> {<string:property>,...}
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int       fulldump;
  int       prop;
  char*     string = NULL;
  int       handn;
  OHandle* hands;
  int       otypei;
  int       otypen;
  OType*    otypes;
/*.........................................................................*/
  if(a_argn<=1) 
    {
      CWarnF("At least one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString  (a_process,string);
      return EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  prop          = 2;
  if(a_argn==2) fulldump = 1;
  else	        fulldump = 0;

  hands  = OProcessGetHandles       (a_process);
  handn  = CListGetSize              ((CList)hands);
  otypes = OTypeGetTypesFromHandles (hands,&otypen);
  for(otypei=0;otypei<otypen;otypei++)
    {
       int count;
      OType     otype;
      otype     = otypes[otypei];
      if(otype==NULL) /*Never go here !!!.*/
	{
	  for(count=0;count<handn;count++)
	    { 
	      if((OType)OHandleGetType(hands[count])==otype)
		{
		  CStringConcatenate (&string,(char*)OHandleGetIdentifier(hands[count]));
		  CStringConcatenate (&string,"\n");
		}
	    }
	}
      else
	{
	  int          status;
	  int          propi;
	  int          propn;
	  OProperty*   props;
	  status       = 1;
	  if(fulldump==0)
	    {
	      propn = a_argn - prop;
	      props = (OProperty*)CListCreate(propn);
	      propn = 0;
	      for(propi=prop;propi<a_argn;propi++)
		{
		  OProperty oprop;
		  oprop = OTypeGetPropertyIdentifier (otype,a_args[propi]);
		  if(oprop==NULL) 
		    {
		      CWarnF("Unknown property %s for type %s in :\n%s\n",
			     a_args[propi],OTypeGetName(otype),OProcessGetCommand(a_process));
		      status = 0;
		    }
		  else if(OPropertyIsMethod(oprop)==0)
		    {
		      props[propn] = oprop;
                      propn++;
		    }
		}
	    }
	  else
	    {
	      propn = 0;
	      props = NULL;
	    }
	  if(status==1)
	    {
	      int          objn;
	      OIdentifier* objs;
	      objs         = (OIdentifier*)CMemoryAllocateBlock(handn * sizeof(OIdentifier));
	      objn         = 0;
	      for(count=0;count<handn;count++)
		{ 
		  if(OHandleGetType(hands[count])==otype)
		    {
		      objs[objn] = OHandleGetIdentifier(hands[count]);
		      objn++;
		    }
		}
	      {char*                 stype;
	       stype                 = OTypeDumpObjectsInString (otype,objn,objs,propn,props);
	       CStringConcatenate    (&string,stype);
	       CStringConcatenate    (&string,"\n");
	       CStringDelete         (stype);}
	      CMemoryFreeBlock       (objs);
	    }
	  CMemoryFreeBlock (props);
	}
    }
  CMemoryFreeBlock (otypes);

  if(string!=NULL)
    {
      int    length;
      length = strlen(string);
      if(string[length-1]=='\n') string[length-1] = '\0';
    }
  OProcessPutString  (a_process,string);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_set (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> set +v
  osh> set -v
  osh> set +x
  osh> set -x

  osh> set -            {<string:property> <xxx:value>, ...}
  osh> set <string:oid> {<string:property> <xxx:value>, ...}
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int       propn;
  int       handn;
  OHandle* hands;
  int       otypei;
  int       otypen;
  OType*    otypes;
/*.........................................................................*/
  if((a_argn!=2) && (a_argn<4)) 
    {
      CWarnF("One or three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString  (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(a_argn==2)
    {
      OProcessPutString  (a_process,NULL);
           if(strcmp(a_args[1],"+v")==0) 
	 {
	   OProcessSetMode (a_process,1,0);
	   return             EXIT_SUCCESS;
	 }
      else if(strcmp(a_args[1],"-v")==0) 
	{
	  OProcessSetMode (a_process,1,1);
	  return             EXIT_SUCCESS;
	}
      else if(strcmp(a_args[1],"+x")==0) 
	{
	  OProcessSetMode (a_process,2,0);
	  return             EXIT_SUCCESS;
	}
      else if(strcmp(a_args[1],"-x")==0) 
	{
	  OProcessSetMode (a_process,2,1);
	  return             EXIT_SUCCESS;
	}
      else
	{
	  CWarnF             ("Bad argument %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
	  return             EXIT_FAILURE;
	}
    }
  
  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  propn = (a_argn - 2)/2;
  if(2*propn!=a_argn-2)
    {
      CWarnF ("An odd number of arguments is expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  hands  = OProcessGetHandles(a_process);
  handn  = CListGetSize((CList)hands);
  otypes = OTypeGetTypesFromHandles (hands,&otypen);
  for(otypei=0;otypei<otypen;otypei++)
    {
      int       propi;
      OType     otype;
      otype     = otypes[otypei];
      for(propi=0;propi<propn;propi++)
	{ 
	  OProperty oprop;
	  oprop     = OTypeGetPropertyIdentifier (otype,a_args[2+2*propi]);
	  if(oprop==NULL) 
	    {
	      CWarnF("Unknown property %s for type %s in :\n%s\n",
		     a_args[2+2*propi],OTypeGetName(otype),OProcessGetCommand(a_process));
	    }
	  else if(OPropertyIsMethod(oprop)==0)
	    {
	       int count;
	      for(count=0;count<handn;count++)
		{ 
		  if((OType)OHandleGetType(hands[count])==otype)
		    {
		      OTypeSetObjectPropertyFromString 
			(otype,OHandleGetIdentifier(hands[count]),oprop,a_args[2+2*propi+1]);
		    }
		}
	    }
	}
    }
  CMemoryFreeBlock (otypes);

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_get (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> get -            <string:property>
  osh> get <string:oid> <string:property>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*     prop;
  int       handn;
  OHandle* hands;
  int       otypei;
  int       otypen;
  OType*    otypes;
  char*     string = NULL;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  prop   = a_args[2];

  hands  = OProcessGetHandles       (a_process);
  handn  = CListGetSize              ((CList)hands);
  otypes = OTypeGetTypesFromHandles (hands,&otypen);
  for(otypei=0;otypei<otypen;otypei++)
    {
      OType     otype;
      OProperty oprop;
      otype     = otypes[otypei];
      oprop     = OTypeGetPropertyIdentifier (otype,prop);
      if(oprop==NULL) 
	{
	  CWarnF("Unknown property %s for type %s in :\n%s\n",
		 prop,OTypeGetName(otype),OProcessGetCommand(a_process));
	}
      else if(OPropertyIsMethod(oprop)==0)
	{
	   int count;
	  for(count=0;count<handn;count++)
	    { 
	      if((OType)OHandleGetType(hands[count])==otype)
		{
		  void*    variable;
		  int      number = 0;
		  int      status;
		  variable = OPropertyGetVariable (oprop);
		  status   = OTypeGetObjectProperty(otype,OHandleGetIdentifier(hands[count]),oprop,variable,&number);
		  if(status!=0)
		    {
		      char* sval;
		      sval  = OPropertyConvertVariableToString (oprop,variable,number);
		      if(sval!=NULL)
			{
			  CStringConcatenate (&string,sval);
			  CStringConcatenate (&string,"\n");
			  CStringDelete      (sval);
			}
		    }
		  CMemoryFreeVariable    (variable,number,status);
		}
	    }
	}
    }
  CMemoryFreeBlock (otypes);

  if(string!=NULL)
    {
      int    length;
      length = strlen(string);
      if(string[length-1]=='\n') string[length-1] = '\0';
    }
  OProcessPutString  (a_process,string);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_do (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> do -            <string:property>  [<string:argument>, ...]
  osh> do <string:oid> <string:property>  [<string:argument>, ...]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*     prop;
  int       handn;
  OHandle* hands;
  int       otypei;
  int       otypen;
  OType*    otypes;
  char*     string = NULL;
/*.........................................................................*/
  if(a_argn<3) 
    {
      CWarnF("More than two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  prop  = a_args[2];

  hands  = OProcessGetHandles(a_process);
  handn  = CListGetSize((CList)hands);
  otypes = OTypeGetTypesFromHandles (hands,&otypen);
  for(otypei=0;otypei<otypen;otypei++)
    {
      OType otype;
      OProperty oprop;
      otype = otypes[otypei];
      oprop = OTypeGetPropertyIdentifier (otype,prop);
      if(oprop==NULL) 
	{
	  CWarnF("Unknown property %s for type %s in :\n%s\n",
		 prop,OTypeGetName(otype),OProcessGetCommand(a_process));
	}
      else if(OPropertyIsMethod(oprop)==0)
	{
	  CWarnF("Property %s for type %s is not a routine in :\n%s\n",
		 prop,OTypeGetName(otype),OProcessGetCommand(a_process));
	}
      else if(OPropertyIsClass(oprop)==1)
	{
	  void*    variable;
	  int      number = 0;
	  int      status;
	  variable = OPropertyGetVariable (oprop);
	  status   = OTypeDoObjectProperty (otype,NULL,oprop,a_argn-3,a_args+3,variable,&number);
	  if(status!=0)
	    {
	      if(OPropertyGetType(oprop)!=OPropertyVoid)
		{
		  char*    sval;
		  sval     = OPropertyConvertVariableToString (oprop,variable,number);
		  if(sval!=NULL)
		    {
		      CStringConcatenate    (&string,sval);
		      CStringConcatenate    (&string,"\n");
		      CStringDelete         (sval);
		    }
		}
	    }
	  CMemoryFreeVariable    (variable,number,status);
	}
      else
	{
	   int count;
	  for(count=0;count<handn;count++)
	    { 
	      if((OType)OHandleGetType(hands[count])==otype)
		{
		  OIdentifier identifier;
		  void*       variable;
		  int         number = 0;
		  int         status;
		  variable    = OPropertyGetVariable   (oprop);
		  identifier  = (OIdentifier)OHandleGetIdentifier(hands[count]);
		  status      = OTypeDoObjectProperty (otype,identifier,oprop,a_argn-3,a_args+3,variable,&number);
		  if(status!=0)
		    {
		      if(OPropertyGetType(oprop)!=OPropertyVoid)
			{
			  char*    sval;
			  sval     = OPropertyConvertVariableToString (oprop,variable,number);
			  if(sval!=NULL)
			    {
			      CStringConcatenate    (&string,sval);
			      CStringConcatenate    (&string,"\n");
			      CStringDelete         (sval);
			    }
			}
		    }
		  CMemoryFreeVariable    (variable,number,status);
		}
	    }
	}
    }
  CMemoryFreeBlock    (otypes);

  if(string!=NULL)
    {
      int    length;
      length = strlen(string);
      if(string[length-1]=='\n') string[length-1] = '\0';
    }
  OProcessPutString  (a_process,string);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_isValid (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> isValid <string:oid>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType  otype;
  OIdentifier identifier;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  otype = OTypeConvertStringToTypeAndIdentifier (a_args[1],&identifier);
/*printf ("debug:isValid:%s:%lu %lu\n",a_args[1],otype,identifier);*/
  if( (otype==NULL) || (identifier==NULL) ) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_isNotValid (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> isNotValid <string:oid>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType  otype;
  OIdentifier identifier;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  otype = OTypeConvertStringToTypeAndIdentifier (a_args[1],&identifier);
/*printf ("debug:isValid:%s:%lu %lu\n",a_args[1],otype,identifier);*/
  if( (otype==NULL) || (identifier==NULL) ) return EXIT_SUCCESS;

  return EXIT_FAILURE;
}
/***************************************************************************/
int CoExecute_count (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> count
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*     string = NULL;
  int       handn;
  OHandle* hands;
/*.........................................................................*/
  if(a_argn!=1) 
    {
      CWarnF("Zero argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  hands  = OProcessGetHandles (a_process);
  handn  = CListGetSize        ((CList)hands);
  string = CStringCreateF      (64,"%d",handn);
  OProcessPutString            (a_process,string);

  a_args = NULL;
  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_empty (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> empty
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=1) 
    {
      CWarnF ("Zero argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  OProcessPutHandles (a_process,NULL);

  a_args = NULL;
  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_clear (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> clear <string:OType>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType                   otype;
  OTypeClearClassFunction routine;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  otype           = OTypeGetIdentifier (a_args[1]);
  if(otype==NULL) 
    {
      CWarnF("Unknown type %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  routine = OTypeGetClearClassFunction(otype);
  if(routine==NULL) 
    {
      CWarnF ("No ClearClassFunction declared for type %s in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  routine();    

  return EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int CoExecute_osh (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> osh <string:script>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  OShellExecute  ((OShell)OProcessGetShell(a_process),OProcessGetString(a_process,a_args[1]));

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_isCommand (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> isCommand <string:command>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  return OShellGetCommandIdentifier ((OShell)OProcessGetShell(a_process),a_args[1])!=NULL ? 
         EXIT_SUCCESS : EXIT_FAILURE;
}
/***************************************************************************/
int CoExecute_makeHTML (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> makeHTML <string:[cmds, types]>
  osh> makeHTML <string:[cmds, types]> <string:file name>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* fname;
/*.........................................................................*/
  if((a_argn!=2) && (a_argn!=3)) 
    {
      CWarnF ("One or two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  fname = a_argn==2 ? "out.htm" : a_args[2];
  if(strcmp(a_args[1],"cmds")==0)
    {
      OShellProduceHTML_File ((OShell)OProcessGetShell(a_process),fname);
    }
  else if(strcmp(a_args[1],"types")==0)
    {
      OTypeProduceHTML_File (fname);
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_buildFileName (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> buildFileName <string:directory>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string = NULL;
/*.........................................................................*/
  if( (a_argn!=1) && (a_argn!=2))  
    {
      CWarnF            ("Zero or one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  string = CFileBuildTemporaryName(a_argn==1 ? CStringNULL : a_args[1]);
  if(string==NULL) return EXIT_FAILURE;

  OProcessPutString (a_process,CStringDuplicate(string));

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_addFilePath (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> addFilePath <string:file path>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF            ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return            EXIT_FAILURE;
    }

#ifdef DEBUG
  printf ("debug : CoExecute_addFilePath : %s\n",a_args[1]);
#endif

  CFileAddPath (a_args[1]);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_help (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> help
  osh> help <string:topic>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if((a_argn!=1) && (a_argn!=2)) 
    {
      CWarnF              ("Zero or one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  if(a_argn==1)        
    {
      char*            string;
      string           = OHTML_GoBackward();
      OHTML_DumpTopic  (string);
      CStringDelete    (string);
    }
  else 
    {
      OHTML_DumpTopic  (OProcessGetString(a_process,a_args[1]));
    }

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_getTopic (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getTopic -
  osh> getTopic <string:topic>
 Example :
  osh> getTopic 'CoT.htm#CoT'
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char**        lines = NULL;
  int           nline = 0;
  char**        subs  = NULL;
  int           nsub  = 0;
  char*         string;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF              ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString   (a_process,NULL);
      return              EXIT_FAILURE;
    }

/* get topic */
  OHTML_GetTopic (OProcessGetString(a_process,a_args[1]),1,&lines,&nline,&subs,&nsub);

  string            = CTextConvertToString (nline,lines,"\n");
  CTextDelete       (nline,lines);
  CTextDelete       (nsub ,subs);
  OProcessPutString (a_process,string);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_makeMakefile (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> makeMakfile
  osh> makeMakfile <string:odb file>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_argn!=1) && (a_argn!=2) )
    {
      CWarnF ("Zero or one arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(a_argn==1)
    {
      CManagerProduceMakefile ("Make.odb",0,NULL);
    }
  else if(a_argn==2)
    {
      CManagerProduceMakefile (a_args[1],0,NULL);
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_spawn (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> spawn <string:command>
  osh> spawn UNIX sh     <string:node> <string:command>
  osh> spawn UNIX csh    <string:node> <string:command>
  osh> spawn VMS  UCX    <string:password> <string:node> <string:command>
  osh> spawn VMS  DECNET <string:decnet node>  <string:target node> <string:command>
 examples :
  osh> spawn '/usr/bin/X11/xclock'
  osh> spawn UNIX sh     lalas2 /usr/bin/X11/xclock
  osh> spawn UNIX csh    lalas2 /usr/bin/X11/xclock
  osh> spawn VMS  UCX    <password>  lalvs5 'run sys$system:decw$mail'
        Require a UCX proxy login on VMS node lalvs5.\n\
  osh> spawn VMS  UCX    <password>  lalvs5 create/terminal/detached
        Require a UCX proxy login on VMS node lalvs5.\n\
  osh> spawn VMS  DECNET lalux0 laldvx 'run sys$system:decw$mail'
        Require a DECNET proxy login for lalux0 on VMS node laldvx.
  osh> spawn VMS  DECNET lalux3 laldvx create/terminal/detached
        Require a DECNET proxy login for lalux3 on VMS node laldvx.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if((a_argn!=2) && (a_argn!=4) && (a_argn!=5) && (a_argn!=6)) 
    {
      CWarnF ("One, three or four arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(CSystemIsSecured()==1)
    {
      CWarnF ("System is secured, can't execute :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  CBatchSetDisplay             (getenv("DISPLAY"));
  CBatchSetAskPasswordFunction (AskPassword);

       if( (a_argn==2) )
    {
      CSystemExecute       (a_args[1]);
    }
  else if( (a_argn==5) && (strcmp(a_args[1],"UNIX")==0) && (strcmp(a_args[2],"sh")==0) )
    {
      CBatchSendToUNIX_SH  (a_args[3],a_args[4]);
    }
  else if( (a_argn==5) && (strcmp(a_args[1],"UNIX")==0) && (strcmp(a_args[2],"csh")==0) )
    {
      CBatchSendToUNIX_CSH (a_args[3],a_args[4]);
    }
  else if((a_argn==6) && (strcmp(a_args[1],"VMS")==0) && (strcmp(a_args[2],"UCX")==0) )
    {
      CStringDelete                (Class.password);
      Class.password               = CStringDuplicate(a_args[3]);
      CBatchSendToVMS_ThroughUCX   (a_args[4],a_args[5]);
      CStringDelete                (Class.password);
      Class.password               = NULL;
    }
  else if( (a_argn==6) && (strcmp(a_args[1],"VMS")==0) && (strcmp(a_args[2],"DECNET")==0) )
    {
      CBatchSendToVMS_ThroughDECNET  (a_args[3],a_args[4],a_args[5]);
    }
  else 
    {
      CWarnF ("Bad option %s or %s in :\n%s\n",a_args[1],a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/****** Text ***************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int CoExecute_getStringLength (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getStringLength -
  osh> getStringLength <string>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  int    linen = 0;
  char** lines = NULL;
  char*  string;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF            ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  lines = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  if(linen==0)
    {
      OProcessPutString (a_process,CStringDuplicate("0"));
    }
  else
    {
      for(count=0;count<linen;count++) 
	{
	  string        = CStringCreateF(64,"%d",strlen(lines[count]));
	  CStringDelete (lines[count]);
	  lines[count]  = string;
	}
      string            = CTextConvertToString (linen,lines,"\n");
      OProcessPutString (a_process,string);
    }

  CTextDelete       (linen,lines); 

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_sort (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> sort
  osh> sort -
  osh> sort <string:text>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    linen = 0;
  char** lines = NULL;
  char*  string;
/*.........................................................................*/
  if( (a_argn!=1) && (a_argn!=2) )
    {
      CWarnF("Zero or one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

       if(a_argn==1) lines = CTextCreateFromString (OProcessGetString(a_process,"-"),&linen);
  else if(a_argn==2) lines = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);

  CTextSort         (linen,lines); 
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_removeDuplicates (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> removeDuplicates -
  osh> removeDuplicates <string:text>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    linen = 0;
  char** lines = NULL;
  char*  string;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("Zero or one argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  lines                 = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  CTextRemoveDuplicates (&linen,&lines); 
  string                = CTextConvertToString (linen,lines,"\n");
  CTextDelete           (linen,lines); 
  OProcessPutString     (a_process,string);

  return                EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_matchString (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> matchString -             <string:pattern> [yes/no]
  osh> matchString <string:text> <string:pattern> [yes/no]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  int    linen = 0;
  char** lines = NULL;
  char*  string;
  int    match = 0;
/*.........................................................................*/
  if( (a_argn!=3) && (a_argn!=4) )
    {
      CWarnF            ("Two or three argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  if(a_argn==4)
    {
      int   status;
      match = CStringConvertToBoolean(a_args[3],&status);
      if(status==0) 
	{
	  CWarnF              ("Argument %s not a boolean in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	  OProcessPutHandles (a_process,NULL);
	  return              EXIT_FAILURE;
	}
      match = match==1 ? 0 : 1;
    }

  lines = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      if(CStringMatchPattern (lines[count],a_args[2])==match) 
	{
	  CStringDelete (lines[count]);
	  lines[count] = NULL;
	}
    }

  CTextClean        (&linen,&lines); 
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_getLine (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getLine -             <integer:line number>
  osh> getLine <string:text> <integer:line number>
  osh> getLine <string:text> last
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    linen = 0;
  char** lines = NULL;
  int    index;
  int    last = 0;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF            ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  if(strcmp(a_args[2],"last")==0) 
    {
      last = 1;
    } 
  else 
    {
      int   status;
      index = (int)CStringConvertToLong(a_args[2],&status);
      if(status==0) 
	{
	  CWarnF              ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  OProcessPutHandles (a_process,NULL);
	  return              EXIT_FAILURE;
	}
    }

  lines = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  if(last==1) 
    {
      if(linen>=1) 
	OProcessPutString (a_process,CStringDuplicate(lines[linen-1]));
      else
	OProcessPutString (a_process,NULL);
    }
  else if( (index>=1) && (index<=linen) ) 
    {
      OProcessPutString (a_process,CStringDuplicate(lines[index-1]));
    }
  else
    {
      OProcessPutString (a_process,NULL);
    }

  CTextDelete       (linen,lines); 

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_unmatchString (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> unmatchString -             <string:pattern>
  osh> unmatchString <string:text> <string:pattern>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  int    linen = 0;
  char** lines = NULL;
  char*  string;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF            ("Two argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  lines = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      if(CStringMatchPattern (lines[count],a_args[2])==1) 
	{
	  CStringDelete (lines[count]);
	  lines[count] = NULL;
	}
    }

  CTextClean        (&linen,&lines); 
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_prependString (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> prependString -             <string:prefix>
  osh> prependString <string:text> <string:prefix>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int    linen = 0;
  char** lines = NULL;
  char*  string;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF("Two argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  lines             = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      string        = CStringCreateF(strlen(a_args[2])+strlen(lines[count]),"%s%s",a_args[2],lines[count]);
      CStringDelete (lines[count]);
      lines[count]  = string;
    }
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_lowerString (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> lowerString -
  osh> lowerString <string:text>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  int    linen = 0;
  char** lines = NULL;
  char*  string;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  lines             = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      CStringLower  (lines[count]);
    }
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_raiseString (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> raiseString -
  osh> raiseString <string:text>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  int    linen = 0;
  char** lines = NULL;
  char*  string;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF ("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  lines             = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      CStringRaise (lines[count]);
    }
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_getStringWord (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getStringWord -             <integer:index>
  osh> getStringWord <string:text> <integer:index>
  osh> getStringWord <string:text> <integer:index> <string:separator>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  int    linen = 0;
  char** lines = NULL;
  char*  sindex;
  int    status;
  int    index = 0;
  char*  string;
/*.........................................................................*/
  if( (a_argn!=3) && (a_argn!=4) )
    {
      CWarnF              ("Two or three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  sindex = a_args[2];
  index  = (int)CStringConvertToLong(sindex,&status);
  if(status==0) 
    {
      CWarnF              ("Argument %s not a number in :\n%s\n",sindex,OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  lines             = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      int    wordn = 0;
      char** words = NULL;
      string = lines[count];
      if(a_argn==3)
	words     = CStringGetWords (string," ",&wordn);
      else
	words     = CStringGetWords (string,a_args[3],&wordn);
      if((index<1) || (index>wordn))
	{
	  CStringDelete (lines[count]);
          lines[count]  = CStringDuplicate("");
	}
      else
	{
	  CStringDelete (lines[count]);
          lines[count]  = CStringDuplicate(words[index-1]);
	}
      CTextDelete (wordn,words);
    }
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_removeLastStringWord (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> removeLastStringWord -        <string:separator>
  osh> removeLastStringWord <string> <string:separator>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      count;
  int      linen = 0;
  char**   lines = NULL;
  char*    string;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF              ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  lines = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      int    wordi;
      int    wordn = 0;
      char** words = NULL;
      words  = CStringGetWords (lines[count],a_args[2],&wordn);
      string = NULL;
      for(wordi=0;wordi<wordn-1;wordi++) 
	{
	  if(wordi!=0) CStringConcatenate (&string,a_args[2]);
	  CStringConcatenate (&string,words[wordi]);
	}
      CStringDelete (lines[count]);
      lines[count]  = string ? string : CStringDuplicate("");
      CTextDelete   (wordn,words);
    }
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_replaceStringPart (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> replaceStringPart -             <string:old part> <string:new part>
  osh> replaceStringPart <string:text> <string:old part> <string:new part>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  int    linen = 0;
  char** lines = NULL;
  char*  string;
  char*  sold = NULL;
  char*  snew = NULL;
/*.........................................................................*/
  if(a_argn!=4) 
    {
      CWarnF ("Three arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  sold                = CStringDuplicate (a_args[2]);
  snew                = CStringDuplicate (a_args[3]);
  TreatCarriageReturn (&sold);
  TreatCarriageReturn (&snew);

  lines               = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      string              = lines[count];
      CStringReplacePart  (&string,sold,snew);
      lines[count]        = string;
    }
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  CStringDelete     (sold);
  CStringDelete     (snew);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_cutStringBegin (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> cutStringBegin -             <integer:length to cut>
  osh> cutStringBegin <string:text> <integer:length to cut>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  int    linen = 0;
  char** lines = NULL;
  char*  string;
  int    length,status;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF            ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return            EXIT_FAILURE;
    }

  length = (int)CStringConvertToLong(a_args[2],&status);
  if(status==0) 
    {
      CWarnF              ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return              EXIT_FAILURE;
    }

  lines               = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      int           linel;
      linel         = strlen(lines[count]);          
      if(linel<=length)
	{
	  CStringDelete (lines[count]);
	  lines[count]  = CStringDuplicate("");
	}
      else
	{
	  string        = CStringCreateF(linel-length,"%s",lines[count]+length);
	  CStringDelete (lines[count]);
	  lines[count]  = string;
	}
    }
  string            = CTextConvertToString (linen,lines,"\n");
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  return            EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_convertToString (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> convertToString -             <string:separator>
  osh> convertToString <string:text> <string:separator>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  int    linen     = 0;
  char** lines     = NULL;
  char*  string    = NULL;
  char*  separator = NULL;
/*.........................................................................*/
  if(a_argn!=3) 
    {
      CWarnF ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutString (a_process,NULL);
      return EXIT_FAILURE;
    }

  separator           = CStringDuplicate (a_args[2]);
  TreatCarriageReturn (&separator);

  lines               = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);
  for(count=0;count<linen;count++) 
    {
      CStringConcatenate  (&string,lines[count]);
      if(count!=linen-1)  CStringConcatenate  (&string,separator);
    }
  CTextDelete       (linen,lines); 
  OProcessPutString (a_process,string);

  CStringDelete     (separator);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int CoExecute_putStringInPipe (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> putStringInPipe -
  osh> putStringInPipe <string:text>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int       linen = 0;
  char**    lines = NULL;
  OHandle*  hands = NULL;
/*.........................................................................*/
  if(a_argn!=2)
    {
      CWarnF("One arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  lines = CTextCreateFromString (OProcessGetString(a_process,a_args[1]),&linen);

  hands = (OHandle*)CListCreate(linen);
  if(hands!=NULL)
  {
    int count;
    for(count=0;count<linen;count++) 
      {
	OIdentifier  oid;
	OType        otype;
	otype        = OTypeConvertStringToTypeAndIdentifier (lines[count],&oid);
	if( (otype!=NULL) && (oid!=NULL) )
	  {
	    hands[count] = OHandleCreate (otype,oid);
	  }
      }
  }	
  CTextDelete         (linen,lines); 
  OProcessPutHandles (a_process,hands);

  return EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int CoExecute_calculate (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> calculate <double:number> [+, -, *, /, **] <double:number>
  osh> calculate <double:number> [add, sub, mul, div, pow] <double:number>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  double number_1,number_2;
  int    status;
/*.........................................................................*/
  if (a_argn!=4) 
    {
      CWarnF("Three arguments expected in:\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  number_1 = CStringConvertToDouble(a_args[1],&status);
  if(status==0) 
    {
      CWarnF              ("Argument %s not a number in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }
  number_2 = CStringConvertToDouble(a_args[3],&status);
  if(status==0) 
    {
      CWarnF              ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

       if( (strcmp(a_args[2],"+")==0) || (strcmp(a_args[2],"add")==0) )
    {
      OProcessPutString (a_process,CStringCreateF(64,"%g",number_1 + number_2));
    }
  else if( (strcmp(a_args[2],"-")==0) || (strcmp(a_args[2],"sub")==0) )
    {
      OProcessPutString (a_process,CStringCreateF(64,"%g",number_1 - number_2));
    }
  else if( (strcmp(a_args[2],"*")==0) || (strcmp(a_args[2],"mul")==0) )
    {
      OProcessPutString (a_process,CStringCreateF(64,"%g",number_1 * number_2));
    }
  else if( (strcmp(a_args[2],"**")==0) || (strcmp(a_args[2],"pow")==0) )
    {
      OProcessPutString (a_process,CStringCreateF(64,"%g",pow(number_1,number_2)));
    }
  else if( (strcmp(a_args[2],"/")==0) || (strcmp(a_args[2],"div")==0) )
    {
      if(number_2==(double)0.)
        {
	  CWarnF              ("Can't divide by zero in :\n%s\n",OProcessGetCommand(a_process));
	  OProcessPutHandles  (a_process,NULL);
	  return              EXIT_FAILURE;
	}
      else
	{
	  OProcessPutString (a_process,CStringCreateF(64,"%g",number_1/number_2));
	}
    }
  else
    {
      CWarnF              ("Unknown operator %s (+, - , * , /, **, add, sub, mul, div, pow expected) in :\n%s\n",
			   a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles  (a_process,NULL);
      return              EXIT_FAILURE;
    }


  return EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static char* AskPassword (
 char* a_prompt
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char pass[32];
/*.........................................................................*/
  sprintf    (pass,Class.password);
  a_prompt   = NULL; /*toavoid C++ warning.*/
  return     pass;
}
/***************************************************************************/
static int GetComparator (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0') ) return 0;
/* Can't work <,> are shell special characters
       if(strcmp(a_string,"=")==0)   return  IF_EQ;
  else if(strcmp(a_string,"<>")==0)  return  IF_NE;
  else if(strcmp(a_string,"<=")==0)  return  IF_LE;
  else if(strcmp(a_string,"<")==0)   return  IF_LT;
  else if(strcmp(a_string,">=")==0)  return  IF_GE;
  else if(strcmp(a_string,">")==0)   return  IF_GT;
*/
       if(strcmp(a_string,"eq")==0)   return  IF_EQ;
  else if(strcmp(a_string,"ne")==0)   return  IF_NE;
  else if(strcmp(a_string,"le")==0)   return  IF_LE;
  else if(strcmp(a_string,"lt")==0)   return  IF_LT;
  else if(strcmp(a_string,"ge")==0)   return  IF_GE;
  else if(strcmp(a_string,"gt")==0)   return  IF_GT;
  else if(strcmp(a_string,"in")==0)   return  IF_IN;
  else if(strcmp(a_string,"ni")==0)   return  IF_NI;
  return 0;
}
/***************************************************************************/
static void TreatCarriageReturn (
 char** a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringReplacePart (a_string,"\\\\n","$$$"); /*dollar*/
  CStringReplacePart (a_string,"\\n","\n");
  CStringReplacePart (a_string,"$$$","\\n");
}
