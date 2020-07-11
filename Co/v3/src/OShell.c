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
#include <stdarg.h>

#include <CMemory.h>
#include <CString.h>
#include <CPrinter.h>
#include <CError.h>
#include <CFile.h>
#include <CList.h>

#include <OProcess.h>

typedef unsigned long Ulong;

#include <OShellP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  FreeRecord           (OShell);
static int   GetIdentifier        (OShell,char*);
static int   GetCommandIdentifier (OCommand,char*);
static void  WriteHtml            (OCommand);
static void  ReportError          (char*,int);
#ifdef __cplusplus
}
#endif

#define NotFound    (-1)

static struct 
{
  OShell*  extent;
  FILE* file;
} Class = {NULL,NULL};
/***************************************************************************/
void OShellClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy       ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent       = NULL;
  OCommandClearClass ();
}
/***************************************************************************/
OShell* OShellGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
OShell OShellGetIdentifier (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OShell)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OShell  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OShellGetName(This),a_name);
}
/***************************************************************************/
int OShExist (
 OShell This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
OShell OShellCreate (
 char*  a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShell          This;
/*.........................................................................*/
  if(  (a_name==NULL) || (*a_name=='\0')) 
    {
      ReportError("OShellCreate",CErrorValue);
      return NULL;
    }
  This             = (OShell)CMemoryAllocateBlock (sizeof(OShellRecord));
  if(This==NULL)   return NULL;
  CListAddEntry    ((CList*)&(Class.extent),This);
  This->name       = CStringDuplicate(a_name);
  This->cmds       = NULL;
  return           This;
}
/***************************************************************************/
void OShellDelete (
  OShell This 
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
static void FreeRecord (
 OShell This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy     ((CList)(This->cmds),(CListVisitEntryFunction)OCommandDelete);
  This->cmds       = NULL;
  CStringDelete    (This->name);
  This->name       = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void OShellAddCommand (
 OShell This 
,OCommand a_cmd
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)  return;
  if(a_cmd==NULL) return;
  CListAddEntry  ((CList*)&(This->cmds),a_cmd);
}
/***************************************************************************/
void OShellAddNewCommand (
 OShell This 
,char*    a_name 
,OShellExecuteCommandFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OShellAddCommand (This,OCommandCreate(a_name,(OCommandExecuteFunction)a_proc));
}
/***************************************************************************/
char* OShellGetName (
 OShell This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
void OShellProduceHTML_File (
 OShell This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE* file;
/*.........................................................................*/
  if(This==NULL) return;
  if( (a_name==NULL) || (*a_name=='\0')) return;

  file           = CFileOpenForWriting (a_name);
  if(file==NULL) return;

  fprintf       (file,"\
<TITLE>Commands</TITLE>\n\
<!***************************************************************************>\n\
<A NAME=\"Commands\" ><H3>Commands</H3></A>\n\
<!***************************************************************************>\n\
<P>\n\
  osh commands.\n\
<P>\n\
<!--------------------------------------------------------------------------->\n\
<P>Other documentation\n\
<UL>\n");

  Class.file    = file;
  CListVisit    ((CList)This->cmds,(CListVisitEntryFunction)WriteHtml);

  fprintf       (file,"</UL>\n");
  fprintf       (file,"<HR>\n");

  fclose        (file);
  CInfoF        ("File %s produced.\n",a_name);
}
/***************************************************************************/
static void WriteHtml (
 OCommand a_cmd
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  fprintf (Class.file,"<LI><A HREF=\"%s\">%s</A>\n",OCommandGetName(a_cmd),OCommandGetName(a_cmd));
}
/***************************************************************************/
OCommand OShellGetCommandIdentifier (
 OShell   This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OCommand)CListFindEntryBackward ((CList)This->cmds,(CListFindEntryFunction)GetCommandIdentifier,a_name);
}
/***************************************************************************/
static int GetCommandIdentifier (
 OCommand  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OCommandGetName(This),a_name);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void OShellExecuteF (
 OShell This
,int    a_length
,char*  a_format
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  va_list       args;
  char*         string;
/*.........................................................................*/
  va_start      (args,a_format);
  string        = CStringCreateV (a_length,a_format,args);
  va_end        (args);
  OShellExecute (This,string);
  CStringDelete (string);
}
/***************************************************************************/
void OShellExecute (
 OShell   This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProcess       oexec;
/*.........................................................................*/
  if(This==NULL) return;
  oexec                = OProcessCreate (This);
  OProcessExecuteLines (oexec,a_string);
  OProcessDelete       (oexec);
}
/***************************************************************************/
void OShellExecuteFile (
 OShell   This
,char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProcess  oexec;
/*.........................................................................*/
  oexec               = OProcessCreate (This);
  OProcessExecuteFile (oexec,a_fname);
  OProcessDelete      (oexec);
}
/***************************************************************************/
int OShellGetAttribute (
 OShell   This
,char* a_name
,void* a_user
,void* a_addr 
,int*  a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0) *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"name")==0)       *((char**)a_addr)  = This->name;
  else 
    {
      CInfoF ("OShellGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
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
  CErrorHandle ("Co","OShell",a_name,a_code);
}
