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
#include <CString.h>
#include <CText.h>
#include <CStream.h>
#include <CFile.h>
#include <CTime.h>
#include <CList.h>
#include <CError.h>
#include <CSystem.h>
#include <OType.h>

#include <OProcedure.h>
#include <OProcedureL.h>
#include <OLoop.h>

#include <XWidget.h>

#include <OClass.h>
#include <OWidget.h>
#include <OResource.h>
#include <OPart.h>
#include <OPiece.h>
#include <OInterpreter.h>

#include <OInterfaceP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void    FreeRecord       (OInterface);
static int     GetIdentifier    (OInterface,char*);
static int     IsModified       (OInterface,void*);
static void    SetNotChanged    (OInterface);
static void    DestroyCallback  (Widget,XtPointer,XtPointer);
static void    DestroyWidgets   (OInterface);
static void    DestroyInterface (char*);
static char*   GetStringWords   (char*,char*,int*,char***);
static OPiece  SolveReferences  (char*,OType,OHandle*);
static int     Realize1         (OHandle*,char*);
static int     VerifyInterface  (OInterface);
static int     SetInterface     (OInterface,OHandle*);
static void    Realize2         (OInterface);
static void    SetResources1    (int,OPiece*);
static void    SetResources2    (int,OPiece*);
static int     IsResource       (char*);
static int     IsChild          (Widget,Widget,OInterface,OInterface*);
static FILE*   OpenTemporary    (char**);
static void    ReportError      (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;

#define NotFound   (-1)

static struct 
{
  OInterface*  extent;
  char**       packs;
  OTrace       verbose;
  char**       ouis;
  int          ouin;
  WidgetClass  wclassShell;
} Class = {NULL,NULL,OTraceNone,NULL,0,NULL};
/***************************************************************************/
void OInterfaceClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy      ((CList)Class.packs,(CListVisitEntryFunction)CMemoryFreeBlock);
  Class.packs       = NULL;

  CListDestroy      ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent      = NULL;
  Class.wclassShell = NULL;

  OPieceClearClass  ();
}
/***************************************************************************/
OInterface* OInterfaceGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
char* OInterfaceGetVersion (
)
/***************************************************************************/
/*
  Return the hard coded Wo version number.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char WOVERSION[] = "v3";
/*.........................................................................*/
  return WOVERSION;
}
/***************************************************************************/
void OInterfaceAddPackage (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return;
  CListAddEntry       ((CList*)&Class.packs,(void*)CStringDuplicate(a_name));
  CListRemoveDuplicates ((CList)Class.packs,(CListMatchEntryFunction)CStringCompare,1);
}
/***************************************************************************/
char** OInterfaceGetPackages (
 int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = CListGetSize ((CList)Class.packs);
  return Class.packs;
}
/***************************************************************************/
int OInterfaceIsPackageKnown (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return 0;
  return ((CListGetEntryIndex((CList)Class.packs,(void*)a_name,(CListMatchEntryFunction)CStringCompare) == NotFound) ? 0:1); 
}
/***************************************************************************/
void OInterfaceSetTrace (
  OTrace a_mode
)
/***************************************************************************/
{
/*.........................................................................*/
  Class.verbose = a_mode;
}
/***************************************************************************/
OInterface OInterfaceGetIdentifier  (
  char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OInterface)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OInterface  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OInterfaceGetName(This),a_name);
}
/***************************************************************************/
void OInterfaceVerifyScriptSyntaxOfExtent (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListVisit ((CList)Class.extent,(CListVisitEntryFunction)OInterfaceVerifySyntaxOfScripts);
}
/***************************************************************************/
void OInterfaceSetExtentNotModified (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListVisit ((CList)Class.extent,(CListVisitEntryFunction)SetNotChanged);
}
/***************************************************************************/
static void SetNotChanged (
 OInterface This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  OInterfaceMark (This,0);
}
/***************************************************************************/
int OInterfaceIsExtentModified (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListFindEntry((CList)Class.extent,(CListFindEntryFunction)IsModified,NULL)!=NULL ? 1 : 0);
}
/***************************************************************************/
static int IsModified (
 OInterface   This
,void* a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  a_user = NULL;
  return OInterfaceIsModified(This);
}
/***************************************************************************/
void OInterfaceSaveExtent (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListVisit ((CList)Class.extent,(CListVisitEntryFunction)OInterfaceSave);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
OInterface OInterfaceMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OInterface This;
/*.........................................................................*/
  This               = (OInterface)CMemoryAllocateBlock (sizeof(OInterfaceRecord));
  if(This==NULL)     return NULL;
  CListAddEntry      ((CList*)&(Class.extent),This);

  This->name            = NULL;
  This->fileName        = NULL;
  This->prefix          = NULL;
  This->packs           = NULL;
  This->version         = NULL;
  This->resources       = NULL;
  This->modified        = 0;
  This->volatil         = 0;
  This->createCallback  = NULL;
  This->destroyCallback = NULL;
  This->parent          = NULL;

  return This;
}
/***************************************************************************/
OInterface OInterfaceCreate (
 char* a_name 
,char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OInterface This;
/*.........................................................................*/
  This              = OInterfaceMake();
  if(This==NULL)    return NULL;
  This->name        = CStringDuplicate(a_name);
  This->fileName    = CStringDuplicate(a_fname);
  return            This;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OInterfaceIsValid (
 OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
void OInterfaceDelete (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  CListRemoveEntry  ((CList)Class.extent,This);
  FreeRecord        (This);
}
/***************************************************************************/
static void FreeRecord (
 OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/*destroyCallback*/
  if(This->destroyCallback!=NULL) OScriptExecute (This->destroyCallback);
  CStringDelete (This->name);
  CStringDelete (This->fileName);
  CStringDelete (This->prefix);
  CStringDelete (This->parent);
  CStringDelete (This->createCallback);
  CStringDelete (This->destroyCallback);
  CStringDelete (This->packs);
  CStringDelete (This->version);
  CStringDelete (This->resources);
  This->name            = NULL;
  This->fileName        = NULL;
  This->prefix          = NULL;
  This->parent          = NULL;
  This->createCallback  = NULL;
  This->destroyCallback = NULL;
  This->packs           = NULL;
  This->version         = NULL;
  This->resources       = NULL;
  CMemoryFreeBlock      (This);
}
/***************************************************************************/
void OInterfaceSetName (
 OInterface This 
,char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete  (This->name);
  This->name     = CStringDuplicate (a_name);
}
/***************************************************************************/
void OInterfaceSetResources (
 OInterface This 
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)       return;
  if(a_value==NULL)    return;
  CStringDelete   (This->resources);
  This->resources = CStringDuplicate (a_value);
}
/***************************************************************************/
void OInterfaceSetPackages (
 OInterface This 
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete  (This->packs);
  This->packs    = CStringDuplicate (a_value);
  OInterfaceMark (This,True);
}
/***************************************************************************/
void OInterfaceSetParent (
 OInterface This 
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete  (This->parent);
  This->parent   = CStringDuplicate (a_value);
  OInterfaceMark (This,True);
}
/***************************************************************************/
void OInterfaceSetVersion (
 OInterface This 
,char* a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CStringDelete  (This->version);
  This->version  = CStringDuplicate (a_value);
  OInterfaceMark (This,True);
}
/***************************************************************************/
void OInterfaceMark (
 OInterface This 
,int a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->modified = a_mark;
}
/***************************************************************************/
void OInterfaceSetVolatile (
 OInterface This 
,int a_mark 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->volatil = a_mark;
}
/***************************************************************************/
int OInterfaceIsModified (
 OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->modified;
}
/***************************************************************************/
char* OInterfaceGetPrefix (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->prefix;
}
/***************************************************************************/
char* OInterfaceGetParent (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->parent;
}
/***************************************************************************/
void OInterfaceSetPrefix (
 OInterface This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(CStringIsWord(a_string)==0) 
    {
      ReportError ("OInterfaceSetPrefix",CErrorFormat);
      return;
    }
  CStringDelete (This->prefix);
  This->prefix = CStringDuplicate (a_string);
  OInterfaceMark (This,True);
}
/***************************************************************************/
char* OInterfaceGetName (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
char* OInterfaceGetFileName (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->fileName;
}
/***************************************************************************/
void OInterfaceSetFileName (
 OInterface This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  if(CStringCompare(This->fileName,a_string)==1) return;
  CStringDelete  (This->fileName);
  This->fileName = CStringDuplicate(a_string);
  OInterfaceMark (This,True);
}
/***************************************************************************/
char* OInterfaceGetCreateCallbackScript (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->createCallback;
}
/***************************************************************************/
char* OInterfaceGetDestroyCallbackScript (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->destroyCallback;
}
/***************************************************************************/
void OInterfaceSetCreateCallbackScript (
 OInterface This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_string==NULL) return;
  if(CStringCompare(This->createCallback,a_string)==1) return;
  CStringDelete        (This->createCallback);
  This->createCallback = CStringDuplicate(a_string);
  OInterfaceMark       (This,True);
}
/***************************************************************************/
void OInterfaceSetDestroyCallbackScript (
 OInterface This 
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  if(a_string==NULL) return;
  if(CStringCompare(This->destroyCallback,a_string)==1) return;
  CStringDelete         (This->destroyCallback);
  This->destroyCallback = CStringDuplicate(a_string);
  OInterfaceMark        (This,True);
}
/***************************************************************************/
char* OInterfaceGetInformations (
 OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int   length;
  char* string;
/*.........................................................................*/
  if(This==NULL) return NULL;

  length     = (This->name==NULL     ? 5 : strlen (This->name)) + 
               (This->fileName==NULL ? 5 : strlen (This->fileName)) + 
	       strlen (OInterfaceGetPrefix(This)) + 100 ;

  string     = CStringCreateF (length,
"\
     name:%s\n\
     file:%s\n\
   prefix:%s\n\
 modified:%d\n\
", 
                   This->name==NULL     ? "(nil)" : This->name,
                   This->fileName==NULL ? "(nil)" : This->fileName,
                   OInterfaceGetPrefix(This),
                   OInterfaceIsModified(This));

  return           string;
}
/***************************************************************************/
void OInterfaceVerifySyntaxOfScripts (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList items,itema;
/*.........................................................................*/
  if(This==NULL) return;
  items = XWidgetGetLeaves (XWidgetGetTop());
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if((OInterface)OWidgetGetFile(*itema)==This) 
	OWidgetVerifySyntaxOfCallbacks (*itema);
    }
  CListDelete ((CList)items);
}
/***************************************************************************/
void OInterfaceMapShells (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  WidgetList   items,itema;
/*.........................................................................*/
  if(This==NULL) return;
  itema   = items = XWidgetGetLeaves (XWidgetGetTop());
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( ((OInterface)OWidgetGetFile(*itema)==This) && 
	  (OWidgetIsShell(*itema)==1) ) 
	{
          XWidgetMap(*itema);
	}
    }
  CListDelete ((CList)items);
}
/***************************************************************************/
static void DestroyWidgets (
 OInterface This 
)
/***************************************************************************/
/*  Must not be executed inside XtDispatchEvent (then in a callBack).       
    This routine need the immediate execution in XtDestroyWidget of the
   destroyCallback.                                                        */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        found;
/*.........................................................................*/
  if(This==NULL) return;
/*delete items*/
  found = 1;
  while(found==1)
    { 
       WidgetList   items,itema;
       found        = 0;
       items        = XWidgetGetLeaves (XWidgetGetTop()); /*Must returned Widget with "core.being_destroy==False".*/
       for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
	 {
	   if((OInterface)OWidgetGetFile(*itema)==This)
	     {
	       OWidgetDelete (*itema,False);
	       found           = 1;
	       break;
	     }
	 }
       CListDelete ((CList)items);
    }
}
/***************************************************************************/
static void DestroyCallback (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CTextAddLine (&(Class.ouin),&(Class.ouis),OInterfaceGetName((OInterface)OWidgetGetFile(This)));
  a_tag  = NULL;
  a_data = NULL;
}
/***************************************************************************/
void OInterfaceDestroy (
 OInterface This 
)
/***************************************************************************/
/*  Must not be executed inside XtDispatchEvent (then in a callBack).       
    This routine need the immediate execution in XtDestroyWidget of the
   destroyCallback. 
   &osh> reload fname 
   -                                                                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XtCallbackProc old_proc;
  char*          name;
/*.........................................................................*/
  if(This==NULL) return;
  if(XWidgetGetCallData(NULL)!=NULL) return;/*todo: find a better way to know if we are in XtDispatchEvent.*/
  name                       = CStringDuplicate(This->name);
  old_proc                   = OWidgetSetDestroyFunction (DestroyCallback);
  DestroyInterface           (name);
  OWidgetSetDestroyFunction  (old_proc);
  CStringDelete              (name);
}
/***************************************************************************/
static void DestroyInterface (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OInterface    oui;
  char** ouis;
  int    ouin;
/*.........................................................................*/
  if(a_name==NULL)                return;
  oui                             = OInterfaceGetIdentifier(a_name);
  if(oui==NULL)                   return;
  CTextInitialize                 (&(Class.ouin),&(Class.ouis));
  DestroyWidgets                  (oui);
  OInterfaceDelete                (oui);
  CTextRemoveDuplicates           (&(Class.ouin),&(Class.ouis));
  if(Class.ouin==0)               return;
  ouis                            = CTextDuplicate(Class.ouin,Class.ouis,&ouin);
  CTextDelete                     (Class.ouin,Class.ouis);
  for(count=0;count<ouin;count++) DestroyInterface (ouis[count]);
  CTextDelete                     (ouin,ouis);
}
/***************************************************************************/
static int   IsResource (
 char* a_string   
)
/***************************************************************************/
/* check if string has the form : yyyy[*|.]res_name:value                  */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_string==NULL)       return 0;
  return              (strstr(a_string,":")!=NULL ? 1 : 0);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static char* GetStringWords (
 char* a_string 
,char* a_limiter 
,int* a_linen
,char*** a_lines
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  char*  token;
  int    linen = 0;
  char** lines = NULL;
/*.........................................................................*/
  *a_linen = 0;
  *a_lines = NULL;
  if( (a_string==NULL) || (*a_string=='\0') )  return NULL;
  if(a_limiter==NULL)                 return NULL;

  string      = CStringDuplicate(a_string);
  if(string==NULL) return NULL;

  token       = string;
  while(1)
    { 
      char* pos;
      pos = strstr (token,a_limiter);
      if(pos!=NULL)
        {
          *pos = 0;
          if(*token!='\0') 
	    {
	      CListAddEntry ((CList*)&lines,token);
	      linen++;
	    }
          token = pos + strlen(a_limiter);          
        }
      else /*last word*/
        {
          if(*token!='\0') 
	    {
	      CListAddEntry ((CList*)&lines,token);
	      linen++;
	    }
          break;
        }
    }

  *a_linen = linen;
  *a_lines = lines;
  
  return   string;
}
/***************************************************************************/
int OInterfaceValidate (
 OInterface  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return 0;

/*Check packages*/
  if(This->packs!=NULL)
    { 
      char*  packs = NULL;
      int    wordn;
      int    wordi;
      char** words;
      char*  wordb;
      wordb  = GetStringWords(This->packs," ",&wordn,&words);
      for(wordi=0;wordi<wordn;wordi++)
	{
	  if(OInterfaceIsPackageKnown(words[wordi])==0)
	    {
	      CWarnF           ("Can't load %s file:\n %s package unknown.\n",This->fileName,words[wordi]);
	      CStringDelete    (packs);
	      CMemoryFreeBlock (words);
	      CMemoryFreeBlock (wordb);
	      return 0;
	    }
	  CStringConcatenate (&packs,words[wordi]);
	  CStringConcatenate (&packs," ");
	}
      OInterfaceSetPackages (This,packs);
      CStringDelete         (packs);
      CMemoryFreeBlock      (words);
      CMemoryFreeBlock      (wordb);
    }

  return 1;
}
/***************************************************************************/
int OInterfaceSetAttribute (
 OInterface  This
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

       if(strcmp(a_name,"identifier")==0)          {}
  else if(strcmp(a_name,"dataBaseName")==0)        OInterfaceSetName                  (This,*((char**)a_addr));
  else if(strcmp(a_name,"dataBaseFile")==0)        OInterfaceSetFileName              (This,*((char**)a_addr));
  else if(strcmp(a_name,"prefix")==0)              OInterfaceSetPrefix                (This,*((char**)a_addr));
  else if(strcmp(a_name,"packages")==0) 	   OInterfaceSetPackages              (This,*((char**)a_addr));
  else if(strcmp(a_name,"version")==0)             OInterfaceSetVersion               (This,*((char**)a_addr));
  else if(strcmp(a_name,"parent")==0)              OInterfaceSetParent                (This,*((char**)a_addr));
  else if(strcmp(a_name,"createCallback")==0)      OInterfaceSetCreateCallbackScript  (This,*((char**)a_addr));
  else if(strcmp(a_name,"destroyCallback")==0)     OInterfaceSetDestroyCallbackScript (This,*((char**)a_addr));
  else if(strcmp(a_name,"resources")==0)           OInterfaceSetResources             (This,*((char**)a_addr));
  else if(strcmp(a_name,"modified")==0)            OInterfaceMark                     (This,*((int*)a_addr));
  else 
    {
      CInfoF ("OInterfaceSetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int OInterfaceGetAttribute (
 OInterface    This
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

       if(strcmp(a_name,"identifier")==0)          *((Ulong*)a_addr)    = (Ulong)This;
  else if(strcmp(a_name,"dataBaseName")==0)        *((char**)a_addr)    = This->name;
  else if(strcmp(a_name,"dataBaseFile")==0)        *((char**)a_addr)    = This->fileName;
  else if(strcmp(a_name,"prefix")==0)              *((char**)a_addr)    = This->prefix;
  else if(strcmp(a_name,"packages")==0)            *((char**)a_addr)    = This->packs;
  else if(strcmp(a_name,"version")==0)             *((char**)a_addr)    = This->version;
  else if(strcmp(a_name,"parent")==0)              *((char**)a_addr)    = This->parent;
  else if(strcmp(a_name,"createCallback")==0)      *((char**)a_addr)    = This->createCallback;
  else if(strcmp(a_name,"destroyCallback")==0)     *((char**)a_addr)    = This->destroyCallback;
  else if(strcmp(a_name,"resources")==0)           *((char**)a_addr)    = This->resources;
  else if(strcmp(a_name,"modified")==0)            *((int*)a_addr)      = This->modified;
  else 
    {
      CInfoF("OInterfaceGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int OInterfaceDoMethod (
 OInterface    This
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
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;

       if(strcmp(a_name,"verifySyntaxOfScripts")==0) 
    {
      OInterfaceVerifySyntaxOfScripts(This);
    }
  else if(strcmp(a_name,"mapShells")==0) 
    {
      OInterfaceMapShells(This);
    }
  else if(strcmp(a_name,"getInformations")==0)
    {
      *((char**)a_addr) = OInterfaceGetInformations (This);
      return            FREE_BLOCK;
    }
  else 
    {
      CInfoF("OInterfaceDoMethod: %s is not a routine property.\n",a_name);
      return 0;
    }

  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  a_addr = NULL;
  return 1;
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
  CErrorHandle ("Wo","OInterface",a_name,a_code);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void* OPieceConstruct (
 void*  a_type
,int    a_parn
,char** a_pars
,char** a_vals
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  char*  name      = NULL;
  char*  type      = NULL;
  char*  children  = NULL;
  char*  dataBaseName  = NULL;
/*char*  dataBaseFile  = NULL;*/
  int    resn = 0;
  int*   ress = NULL;
  OPiece opiece;
/*.........................................................................*/
  if(a_parn==0) return NULL;
  ress      = (int*) CMemoryAllocateBlock (a_parn * sizeof (int));
  if(ress==NULL) return NULL;
  resn      = 0;
  for(count=0;count<a_parn;count++)
    {
      char*  string;
      string = a_pars[count];
           if( (string!=NULL) && (strcmp(string,"name")==0) )          name         = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"type")==0) )          type         = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"children")==0) )      children     = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"dataBaseName")==0) )  dataBaseName = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"dataBaseFile")==0) )  
	{
	  /*dataBaseFile = a_vals[count];*/
	}
      else
        { /*Assume it is a resource.*/
	  ress[resn] = count;
          resn++;
	}
    }
  if(OPartGetIdentifier(name)!=NULL)
    { 
/*    CWarnF   ("Widget %s already exist;\nignore second load.\n",name);*/
      CMemoryFreeBlock (ress);
      return   NULL;
    }
  if(name==NULL)
    {
      CWarn    ("Found a Widget without name.\n");
      CMemoryFreeBlock (ress);
      return   NULL;
    }
  if(type==NULL)
    {
      CWarnF   ("Widget %s has no type.\n",name);
      CMemoryFreeBlock (ress);
      return   NULL;
    }
  if(dataBaseName==NULL)
    {
      CWarnF   ("Widget %s has no dataBaseName.\n",name);
      CMemoryFreeBlock (ress);
      return   NULL;
    }
  if(OClassGetIdentifier(type)==NULL)
    {
      CWarnF           ("Widget %s has an unknown type %s.\n",name,type);
      CMemoryFreeBlock (ress);
      return           NULL;
    }
  opiece                    = OPieceCreate (name,type,NULL);
  OPieceSetStringOfChildren (opiece,children);
  OPieceSetDataBaseName     (opiece,dataBaseName);
  for(count=0;count<resn;count++) OPieceAddResource (opiece,a_pars[ress[count]],a_vals[ress[count]]);
  CMemoryFreeBlock   (ress);
  a_type             = NULL;
  return             (void*)opiece;
}
/***************************************************************************/
CList OPieceSetReferences (
 void*   a_type
,void*   a_obj
,void*   a_prop
,char*   a_string
,CList  a_hands
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  name;
  OHandle* hands = NULL;
/*.........................................................................*/
  CListConcatenate ((CList*)&hands,(CList)a_hands);

  name = OPropertyGetName((OProperty)a_prop);
  if( (a_obj!=NULL) && (name!=NULL) && (strcmp(name,"children")==0) )
    {
       int count;
      int    wordn;
      char** words;
      char*  wordb;
      OType  type;

      type       = OTypeGetIdentifier ("OWidget");

      wordb      = GetStringWords(a_string," ",&wordn,&words);
      for(count=0;count<wordn;count++)
	{
	  char*   pos;
          OPiece  opiece;
	  pos     = strchr(words[count],'<');
	  if(pos!=NULL) 
	    {
	      *pos  = 0;
	      if(OInterfaceGetIdentifier(pos+1)==NULL)  /*Load sub file.*/
		{
		  OHandle*        shands;
		  shands           = OTypeGetExtentFromFile (pos+1,"odb",NULL);
		  CListConcatenate ((CList*)&hands,(CList)shands);
		  CListDelete      ((CList)shands);
		}
	    }
          opiece = SolveReferences (words[count],type,(OHandle*)hands); /*Solve ref on total object list.*/
          if(opiece!=NULL)
            {
	      if(OPieceSetParent(opiece,(OPiece)a_obj)==0) 
                {
		  CWarnF ("For %s; can't set child %s.\n",OPieceGetName((OPiece)a_obj),words[count]);
		}
	    }
	  else
	    {
	      CWarnF ("For %s; can't solve reference %s.\n",OPieceGetName((OPiece)a_obj),words[count]);
	    }
	}
      CMemoryFreeBlock       (words);
      CMemoryFreeBlock       (wordb);
    }


  a_type = NULL;
  return (CList)hands;
}
/***************************************************************************/
static OPiece SolveReferences (
 char*  a_string
,OType  a_type
,OHandle* a_hands
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHandle* itema;
/*.........................................................................*/
  if(a_hands==NULL) return NULL;
  for(itema=a_hands;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      OType type;
      type  = (OType)OHandleGetType  (*itema);
      if(type==a_type)
        {
	  OIdentifier identifier;
	  identifier    = (OIdentifier)OHandleGetIdentifier (*itema);
	  if(CStringCompare(a_string,OPieceGetName((OPiece)identifier))==1)
	    {
              return (OPiece)identifier;
	    }
	}
    }
  return NULL;
}
/***************************************************************************/
void OInterfaceLoadFile (
 char* a_name
,char* a_parent
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OHandle* hands;
/*.........................................................................*/
  hands              = OTypeGetExtentFromFile(a_name,"odb",NULL);
  OInterfaceRealize  ((CList)hands,a_parent);
  CListDestroy       ((CList)hands,(CListVisitEntryFunction)OHandleDelete);
}
/***************************************************************************/
void OInterfaceRealize (
 CList a_hands
,char*  a_parent
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Realize1((OHandle*)a_hands,a_parent)==0)
    { /*Abort. Deleting objects.*/
      OHandle* itema;
      for(itema=(OHandle*)a_hands;(itema!=NULL) && (*itema!=NULL);itema++)
	{
	  OTypeDeleteObject ((OType)OHandleGetType(*itema),OHandleGetIdentifier(*itema));
	}
    }
}
/***************************************************************************/
static int Realize1 (
 OHandle* a_hands
,char* a_parent
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType       otype_oui;
  OInterface* ouis = NULL;
/*.........................................................................*/
  if(a_hands==NULL) return 0;

  otype_oui = OTypeGetIdentifier ("OInterface");

 {OHandle* itema;
  for(itema=a_hands;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if((OType)OHandleGetType(*itema)==otype_oui)
	{
	  OInterface  oui;
	  oui  = (OInterface)OHandleGetIdentifier(*itema);
	  if(SetInterface(oui,a_hands)==0)  
	    {
	      CListDelete ((CList)ouis);
	      return 0;
	    }
	  CListAddEntry ((CList*)&ouis,(void*)oui);
	}
    }}

  if(ouis==NULL)
    {
      CWarn          ("Can't find an OInterface object.\n");
      CListDelete    ((CList)ouis);
      return         0;
    }

  /*For first OInterface.*/
  if(a_parent!=NULL)  OInterfaceSetParent (*ouis,a_parent);
  Realize2            (*ouis); 

  OPieceClearClass    ();
  CListDelete         ((CList)ouis);
  return              1;
}
/***************************************************************************/
static int VerifyInterface (
 OInterface This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;

 {OInterface* itema;  /*Look if already loaded.*/
  for(itema=Class.extent;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (CStringCompare(OInterfaceGetName(*itema),This->name)==1) && (*itema!=This))
	{
	  return 0;
	}
    }}

  if( (This->name==NULL) || (*This->name=='\0') )
    {
      CWarn  ("OInterface object has no name.\n");
      return 0;
    }

  if( (This->fileName==NULL) || (*This->fileName=='\0'))
    {
      CWarnF ("OInterface object %s has no dataBaseFile.\n",This->name);
      return 0;
    }

/*card version*/
/*
  if(OInterfaceGetVersion()!=NULL)
    {
      if(This->version!=NULL)
	{
	  if(strcmp(This->version,OInterfaceGetVersion())!=0)
	    {
	      CWarnF 
	      ("In file %s;\n for OInterface %s, property \"version\" do not match Wo version %s.\n",
	      This->fileName,This->name,OInterfaceGetVersion());
	    }
	}
      else
	{ 
	  CWarnF ("In file %s;\n for OInterface %s, property \"version\" not found or badly written.\n",
	  This->fileName,This->name);
	}
    }
*/

/*prefix*/
  if(This->prefix==NULL)
    {
      CWarnF ("In file %s;\n\
 for OInterface %s, property \"prefix\" not found.\n\
 Continue with prefix \"xxx_\".\n",This->fileName,This->name);
      OInterfaceSetPrefix (This,"xxx_"); 
    }
  if(CStringIsWord(This->prefix)==0) 
    {
      CWarnF ("In file %s;\n\
 for OInterface %s, property \"prefix\" %s has not syntax of a C variable.\n\
 Continue with prefix \"xxx_\".\n",This->fileName,This->name,This->prefix);
      OInterfaceSetPrefix (This,"xxx_"); 
    }

  return 1;
}
/***************************************************************************/
static int SetInterface (
 OInterface This
,OHandle* a_hands
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;

  if(VerifyInterface(This)==0) return 0;

/*All seems ok. Continue...*/

/*createCallback*/
  if(This->createCallback!=NULL) 
    {
      if(OScriptIsBackgroundScript(This->createCallback,NULL)==0) /*Do it now.*/
	{
	  OWidgetRequireScriptExecution (XWidgetGetTop(),This->createCallback,NULL,0);
	}
    }


  XDisplayPutStringInResourceDatabase (XtDisplay(XWidgetGetTop()),This->resources);

 {OType        otype_widget;
  OHandle*     itema;
  otype_widget = OTypeGetIdentifier ("OWidget");
  for(itema=(OHandle*)a_hands;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if((OType)OHandleGetType(*itema)==otype_widget)
	{
	  OPiece opiece;
	  opiece = (OPiece)OHandleGetIdentifier(*itema);
          if(CStringCompare(OPieceGetDataBaseName(opiece),This->name)==1)
	    {
	      OPieceSetFile (opiece,This);
	    }
	}
    }}

  return 1;
}
/***************************************************************************/
static void Realize2 (
 OInterface   This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget      fshell,parent;
  OPiece      oshell,oroot,oparent;
  OPiece      opiece;
  OPiece*     pieces;
  int         piecen;
   int count;
/*.........................................................................*/
  if(This==NULL)  return;

/* connect widget that has no parent on:
  - ApplicationShell,OverrideShell,TopLevelShell,TransientShell: set as child of XWidgetGetTop()
  - Dialog: set as child of first (application,dialog) shell
  - other types:
     if(a_widget==NULL)
       . warnings and not connected
     else
       . connect to a_widget and set orphan volatilChild to True 
*/
/* OPieceExtent list contain only new OPiece to create */
  oroot      = OPieceCreate ("otop","ApplicationShell",XWidgetGetTop());
  if(oroot==NULL) return;
  fshell     = OWidgetGetFirstShell();
  oshell     = fshell!=NULL   ? OPieceCreate (XtName(fshell)  ,OWidgetGetType(fshell)  ,fshell)   : (OPiece)NULL;
  parent     = XWidgetGetIdentifier (OInterfaceGetParent(This));

  oparent    = parent!=NULL ? OPieceCreate (XtName(parent),OWidgetGetType(parent),parent) : (OPiece)NULL;

  OPieceSetParent (oshell ,oroot);
  OPieceSetParent (oparent,oroot);

  if(Class.wclassShell==NULL)
    {
      Class.wclassShell = (WidgetClass)OClassGetWidgetClass (OClassGetIdentifier("Shell"));
    }

  CListVisitE    ((CList)OPieceGetIdentifiers(),(CListVisitEntryV_Function)OPieceConnect,oroot,oshell,oparent,Class.wclassShell);

  pieces         = OPieceGetLeaves (oroot,&piecen);

  SetResources1  (piecen,pieces);

  for(count=0;count<piecen;count++)
    {
      opiece = pieces[count];
      if( (opiece==oroot) || (opiece==oshell) || (opiece==oparent) )  continue;
      OPieceBuildWidget(opiece);
    }

/* compound widget create intermediate Xt items: init their OPart */
  XWidgetVisitHierarchyForward (XWidgetGetTop(),(XWidgetVisitFunction)OWidgetBuildPrivatePart);

  if(Class.verbose==OTraceStartup) 
    {CInfoF("Elapsed:%d:create items\n",CTimeGetElapsedPeriod());CTimeStart();}

  SetResources2 (piecen,pieces);

  if(Class.verbose==OTraceStartup) 
    {CInfoF("Elapsed:%d:analyse resources\n",CTimeGetElapsedPeriod());CTimeStart();}

  for(count=0;count<piecen;count++)
    { 
      opiece         = pieces[count];
      if( (opiece==oroot) || (opiece==oshell) || (opiece==oparent) )  continue;
      XWidgetRealize ((Widget)OPieceGetWidget(opiece));
    }

  if(Class.verbose==OTraceStartup) 
    {CInfoF("Elapsed:%d:realize items\n",CTimeGetElapsedPeriod());CTimeStart();}

  XWidgetVisitHierarchyForward (XWidgetGetTop(),OWidgetSetWindowManagerCloseAction);

/*createCallback*/
  if(This->createCallback!=NULL) 
    {
      if(OScriptIsBackgroundScript(This->createCallback,NULL)==1) /*Background, need that top widget be realized.*/
	{
	  OWidgetRequireScriptExecution (XWidgetGetTop(),This->createCallback,NULL,0);
	}
    }

  for(count=0;count<piecen;count++)
    { 
      Widget xwidget;
      char*  string;
      opiece = pieces[count];
      if( (opiece==oroot) || (opiece==oshell) || (opiece==oparent) )  continue;
      xwidget                       = (Widget)OPieceGetWidget         (opiece);
      string                        = OWidgetGetAndConvertResourceToString  (xwidget,"createCallback");
      OWidgetRequireScriptExecution (xwidget,string,NULL,0);
      CStringDelete                 (string);
      OInterfaceMark                ((OInterface)OPieceGetFile(opiece),0);
    }
  CMemoryFreeBlock (pieces);

  if(Class.verbose==OTraceStartup) 
    {CInfoF("Elapsed:%d:send createCallbacks\n",CTimeGetElapsedPeriod());CTimeStart();}

/* done in upper loop
  OInterfaceMark   (This,0);
*/

}
/***************************************************************************/
static void SetResources1 (
 int   a_wuin
,OPiece* a_wuis
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*    tmpname;
  FILE*    file;
   int count;
/*.........................................................................*/
  if( (a_wuin==0) || (a_wuis==NULL) ) return;

  file     = OpenTemporary (&tmpname);
  if(file==NULL) 
    {
      if(tmpname!=NULL) CWarnF("Can't open temporary file %s.\n",tmpname);
      else              CWarnF("Can't open temporary file.\n");
      return;
    }

  for(count=0;count<a_wuin;count++)
    {
      int    resn;
      char** rnms;
      char** rvls;
      int    resi;
      OPiece   wui;
      char*  name;
      wui    = a_wuis[count];
      name   = OPieceGetName(wui);
      resn   = OPieceGetResources (wui,&rnms,&rvls);
      for(resi=0;resi<resn;resi++)
	{
	  char*      string;
	  char*      value;
	  
          string     = rnms[resi];
          value      = rvls[resi];

	  if( /*Resources that involved pbs treated in SetResources2 after Xt widget creation.*/
	     (strcmp(string,"mnemonic")==0)     ||  /*KeySym*/
             (OResourceIsCursor        (NULL,string)==1)      ||
             (OResourceIsMultiLine     (NULL,string)==1)      ||
	     (OResourceIsItemList      (NULL,string)==1)      ||
	     (OResourceIsPixmap        (NULL,string)==1)      ||
	     (OResourceIsAttachment    (NULL,string)==1)      ||
	     (OResourceIsReferenceList (NULL,string)==1)      ||
	     (OResourceIsFontList      (NULL,string)==1)      ||
	     (OResourceIsFontStruct    (NULL,string)==1)      ||
	     (OResourceIsFont          (NULL,string)==1)         
	     )
	    {
/*
	      printf ("debug:Wo*%s.%s:%s\n",name,string,value);
*/
	    }
	  else  /*Widget resources without trouble, to be marked and be sent to xrm.*/
	    {
	      fprintf       (file,"Wo*%s.%s:%s\n",name,string,value); /* force App name on resources (else pb with HPVUE) */
	      CStringDelete (rvls[resi]);  /*Flag this resource to be marked in PbResources.*/
	      rvls[resi]    = NULL;
	    }
	}
      
    }      

 {char*              sxrm;
  sxrm               = CStreamRead (file,NULL);
  XDisplayPutStringInResourceDatabase (XtDisplay(XWidgetGetTop()),sxrm);
/*  printf ("debug:----------\n%s\n",sxrm);*/
  CStringDelete      (sxrm);}

  fclose        (file);
  remove        (tmpname);	      
}
/***************************************************************************/
static void SetResources2 (
 int   a_wuin
,OPiece* a_wuis
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if( (a_wuin==0) || (a_wuis==NULL) ) return;

  for(count=0;count<a_wuin;count++)
    {
      int    resn;
      char** rnms;
      char** rvls;
      int    resi;
      OPiece wui;
      char*  name;
      OPart  opart;
      Widget widget;
      wui             = a_wuis[count];
      name            = OPieceGetName(wui);
      resn            = OPieceGetResources (wui,&rnms,&rvls);
      opart           = OPartGetIdentifier(name);
      widget          = (Widget)OPartGetWidget(opart);
      XResourceChangesInitialize  ();
      for(resi=0;resi<resn;resi++)
	{
	  char*      string;
	  char*      value;
	  XtResource res;
	  int        rcount;
	  
          string     = rnms[resi];
          value      = rvls[resi];

	  rcount     = XWidgetGetResources  (widget,string,&res);
	  if(rcount==NotFound) 
	    {
	      CWarnF("For widget %s; %s not a resource of %s\n",XtName(widget),string,XWidgetGetClassName(widget));
	    }
	  else if(value!=NULL)
	    {
/*	      printf ("debug::::%s.%s(%s):%s\n",XtName(widget),res.resource_name,res.resource_type,value);*/
	      OWidgetAssignResource   (widget,res.resource_name,res.resource_type,value);
	      OPartMarkIthResource    (opart,rcount,1);
	      OInterfaceMark          ((OInterface)OPieceGetFile(wui),1);
	    }
	  else
	    {
	      OPartMarkIthResource    (opart,rcount,1);
	      OInterfaceMark          ((OInterface)OPieceGetFile(wui),1);
	    }
	}
      XResourceChangesCommit ();
      OPieceDeleteResources  (wui);
    }      

}
/***************************************************************************/
void OInterfaceSave (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*      tmpname;
  FILE*      file;
  Widget     target;
  WidgetList items,itema;
/*.........................................................................*/
  if(This==NULL)         return;
  if(This->volatil==1)   return;
  if(This->modified==0)  return;

  file           = OpenTemporary (&tmpname);
  if(file==NULL) 
    {
      if(tmpname!=NULL) CWarnF("Can't open temporary file %s.\n",tmpname);
      else              CWarnF("Can't open temporary file.\n");
      return;
    }
  CStreamInitializeStatus ();

  target           = XWidgetGetTarget ();
  XWidgetSetTarget (NULL);

  CStreamPrintF(file,"\
!----------------------------\n\
begin OInterface\n");
  if(OInterfaceGetVersion()!=NULL)  
    CStreamPrintF(file," version = %s\n",OInterfaceGetVersion());
  if(This->prefix!=NULL) 
    CStreamPrintF(file," prefix = %s\n",This->prefix);
  if(This->parent!=NULL)  
    CStreamPrintF(file," parent = %s\n",This->parent);

/*packs*/
  if(This->packs!=NULL)
    { 
      char*              string;   
      string             = CStringDuplicate(This->packs);
      CStringReplacePart (&string,"\n","\\\n");
      CStreamPrintF      (file," packages = %s\n",string);
      CStringDelete      (string);
    }

/*createCallback*/
  if(This->createCallback!=NULL)
    { 
      char*              string;   
      string             = CStringDuplicate(This->createCallback);
      CStringReplacePart (&string,"\n","\\\n");
      CStreamPrintF      (file," createCallback = %s\n",string);
      CStringDelete      (string);
    }
  
/*destroyCallback*/
  if(This->destroyCallback!=NULL)
    { 
      char*              string;   
      string             = CStringDuplicate(This->destroyCallback);
      CStringReplacePart (&string,"\n","\\\n");
      CStreamPrintF      (file," destroyCallback = %s\n",string);
      CStringDelete      (string);
    }
  
/*resource not belonging directly to widget */
  if(This->resources!=NULL) 
    {
      char*              string;   
      string             = CStringDuplicate(This->resources);
      CStringReplacePart (&string,"\n","\\\n");
      CStreamPrintF      (file," resources = %s\n",string);
      CStringDelete      (string);
    }

  CStreamPrintF(file,"end\n");

/*Widgets*/
  items = XWidgetGetLeaves (XWidgetGetTop());
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      char* name;
      name  = XtName(*itema);
      if( (name!=NULL)  && 
	  (*name!='\0') && 
	  ((OInterface)OWidgetGetFile(*itema)==This) && 
	  (OWidgetIsVolatile(*itema)==0) 
	 )
        { 
	  char*      type;
	  char**     list;
	  int        nline;
          WidgetList childa;
	  char*      schildren = NULL;
	  type       = OWidgetGetType(*itema);

	  CStreamPrintF(file,"\
!----------------------------\n\
begin OWidget\n\
 name = %s\n\
 type = %s\n\
",name,type);

/* Loop over all widget. 
   For example in PulldownMenu contained in PulldownMenu the second RowColumn 
   appens to be child of a widget that is a parent of the first RowColumn !!!
*/
          childa = items;
	  while(*childa!=NULL)
	    {
	      int        status;
	      OInterface coui;
	      char*      string;   
	      status     = IsChild (*childa,*itema,This,&coui);
	      if(status==1)
		{
		  string             = CStringCreateF (strlen(XtName(*childa))+1,"%s ",XtName(*childa));
		  CStringConcatenate (&schildren,string);
		  CStringDelete      (string);
		}
	      else if(status==2)
		{
		  if(OInterfaceGetName(coui)!=NULL)
		    {
		      string        = CStringCreateF (strlen(XtName(*childa))+strlen(OInterfaceGetName(coui))+2,
						      "%s<%s ",XtName(*childa),OInterfaceGetName(coui));
		      CStringConcatenate (&schildren,string);
		      CStringDelete      (string);
		    }
		}
	      else if(status==3)
		{
		  CWarnF("Widget %s has a bad oui\n",XtName(*childa));
		}
              childa++;
	    }
	  if(schildren!=NULL) CStreamPrintF(file," children = %s\n",schildren);
          CStringDelete (schildren);

	  list    = OWidgetGetMarkedResources (*itema,&nline,1);
	  for(count=0;count<nline;count++)
	    { 
	      char ch;int pos;
	      if( (CStringIsPrintable (list[count],&ch,&pos)==0) && (ch!='\n')) 
		{
		  CWarnF("Character %d not printable at %d in :%s\n",ch,pos,list[count]);
		}
	      CStreamPrintF(file,"%s\n",list[count]); 
	      CMemoryFreeBlock (list[count]);
	    }
	  CMemoryFreeBlock (list);

	  CStreamPrintF(file,"end\n");
	}    
    }
  CListDelete ((CList)items);

  This->modified = 0;

  if(CStreamGetStatus()==1)
   {
     long           filesize;
     char           *buff;
     filesize       = CStreamGetSize(file);
     rewind         (file);
     buff           = (char*)CMemoryAllocateBlock((size_t)(filesize*sizeof(char)));
     if(buff!=NULL) 
       {
	 if (fread(buff, (size_t)filesize, 1, file)!=1)  
	   {
	     ReportError ("OInterfaceSave",CErrorIO);
	   }
	 else if(This->fileName!=NULL)
	   { 
	     FILE*          file;
	     CFileSafeguard (This->fileName);
	     file           = CFileOpenForWriting  (This->fileName);
	     if(file!=NULL) 
	       {
		 if (fwrite(buff, (size_t)filesize, 1, file)!=1)  ReportError ("OInterfaceSave",CErrorIO);
		 fclose         (file);
		 CInfoF         ("File %s created.\n",This->fileName);
	       }
	   }
	 CMemoryFreeBlock (buff);
       }
   }
  
  fclose        (file);
  remove        (tmpname);

  XWidgetSetTarget (target);
}
/***************************************************************************/
static int IsChild (
 Widget This
,Widget a_widget
,OInterface a_oui
,OInterface* a_coui
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  String name;
/*.........................................................................*/
  if(a_coui!=NULL) *a_coui = NULL;
  if( OWidgetGetParent (This) != a_widget) return 0;
  name    = XtName (This);
  if( (name==NULL) || (*name=='\0') ) return 0; 
  *a_coui = (OInterface) OWidgetGetFile (This);
  if(OWidgetIsVolatileChild(This)==1)
    {
      return 0;
    }
  else if(*a_coui==a_oui)
    {
      return 1;
    }
  else if(OInterfaceIsValid(*a_coui)==1) /* child in an other file */
    {
      return 2;
    }
  else
    {
      return 3;
    }
}
/***************************************************************************/
static FILE* OpenTemporary (
 char** a_tmpname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE* file;
/*.........................................................................*/
  file  = CFileOpenTemporaryForWriting (NULL,a_tmpname);
  if(file==NULL) 
    {
      if(CSystemIsKindOf("UNIX",NULL)==1) {
	file = CFileOpenTemporaryForWriting ("/tmp/",a_tmpname);
      }
    }
  return file;
}
