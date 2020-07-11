/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CMemory.h>
#include <CString.h>
#include <CError.h>
#include <CList.h>
#include <CText.h>
#include <CPrinter.h>
#include <CStream.h>
#include <CFile.h>
#include <OProcedure.h>
#include <OProcedureL.h>
#include <OLoop.h>

#include <OType.h>

typedef struct _OTypeRecord
{
  char*                       name;
  OTypeGetIdentifiersFunction extentFunction;
  OTypeMakeFunction           makeFunction;
  OTypeConstructFunction      constructFunction;
  OTypeValidateFunction       validateFunction;
  OTypeDeleteFunction         deleteFunction;
  OTypeDestroyFunction        destroyFunction;
  OTypeSetPropertyFunction    setPropFunction;
  OTypeGetPropertyFunction    getPropFunction;
  OTypeSetAttributeFunction   setAttributeFunction;
  OTypeGetAttributeFunction   getAttributeFunction;
  OTypeSetReferencesFunction  setRefsFunction;
  OTypeGetReferencesFunction  getRefsFunction;
  OTypeRepresentFunction      repFunction;
  OTypeClearClassFunction     clearClassFunction;
  OTypeDoPropertyFunction     doPropFunction;
  OTypeDoMethodFunction       doMethodFunction;
  void*       user;
  OProperty*  props;
} OTypeRecord;
 
typedef struct _LoaderRecord*  Loader;

typedef struct _LoaderRecord
{
  char*              name;
  OTypeLoadFunction  proc;
} LoaderRecord;

#ifdef __cplusplus
extern "C"{
#endif
static void    FreeRecord            (OType);
static void    FreeLoader            (Loader);
static int     GetIdentifier         (OType,char*);
static int     GetPropertyIdentifier (OProperty,char*);
static void    ReportError           (char*,int);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
typedef void(*ObjectFunction)(OIdentifier);
#define MAXIMUM(a,b)  ((a)>(b)?a:b)
#define NotFound      (-1)

static struct 
{
  OType*     extent;
  int        label;
  Loader*    loaders;
  int        dumpMemory;
} Class = {NULL,0,NULL,0};
/***************************************************************************/
void OTypeClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType* itema;
/*.........................................................................*/
  for(itema=Class.extent;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      OType  otype;
      otype  = *itema;
      if( (otype->clearClassFunction!=NULL) && 
	  (otype->clearClassFunction!=OTypeClearClass)) otype->clearClassFunction();
    }
  CListDestroy        ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent        = NULL;
  CListDestroy        ((CList)Class.loaders,(CListVisitEntryFunction)FreeLoader);
  Class.loaders       = NULL;
  OPropertyClearClass ();
  if(Class.dumpMemory==1)
    {
      char*            string;
      string           = CMemoryGetStatus();
      if(string!=NULL) CInfoF ("%s\n",string);
    }
  else
    {
      char*            string;
      string           = CMemoryGetErrors();
      if(string!=NULL) CInfoF ("%s\n",string);
    }
}
/***************************************************************************/
OType* OTypeGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
void* OTypeOpenODB_FileForWriting (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*     file;
/*.........................................................................*/
  file               = CFileOpenForWriting (a_name);
  if(file==NULL)     return NULL;
  OTypeGenerateLabel (1);
  return             (void*)file;
}
/***************************************************************************/
int OTypeGenerateLabel (
  int a_reset
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_reset!=0) Class.label = 0;
  else           Class.label++;
  return         Class.label;
}
/***************************************************************************/
void OTypeAddLoader (
 char* a_name
,OTypeLoadFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Loader This;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return;
  if(a_proc==NULL)               return;
  This           = (Loader) CMemoryAllocateBlock (sizeof(LoaderRecord));
  if(This==NULL) return;
  CListAddEntry  ((CList*)&(Class.loaders),This);
  This->name     = CStringDuplicate(a_name);
  This->proc     = a_proc;
}
/***************************************************************************/
static void FreeLoader (
 Loader This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete    (This->name);
  This->name       = NULL;
  This->proc       = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void OTypeProduceHTML_File (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*   file;
  OType* itema;
/*.........................................................................*/
  if(Class.extent==NULL) return;
  if( (a_name==NULL) || (*a_name=='\0') ) return;

  file      = CFileOpenForWriting (a_name);
  if(file==NULL) return;

  fprintf (file,"\
<TITLE>Types</TITLE>\n\
<!***************************************************************************>\n\
<A NAME=\"Types\" ><H3>Types</H3></A>\n\
<!***************************************************************************>\n\
<P>\n\
  Types\n\
<P>\n\
<!--------------------------------------------------------------------------->\n\
<P>Other documentation\n\
<UL>\n");

  for(itema=Class.extent;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      OType  otype;
      otype  = *itema;
      fprintf (file,"<LI><A HREF=\"%s\">%s</A>\n",otype->name,otype->name);
    }

  fprintf (file,"</UL>\n");
  fprintf (file,"<HR>\n");

  for(itema=Class.extent;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      OType  otype;
      otype  = *itema;
      fprintf (file,"\
<!***************************************************************************>\n\
<A NAME=\"%s\" ><H3>%s</H3></A>\n\
<!***************************************************************************>\n\
<P>\n\
  Properties of %s :\n\
",otype->name,otype->name,otype->name);
      if(otype->props==NULL)
	{
	  fprintf       (file,"<P>\n  No property defined.\n");
	}
      else
	{
	  CListVisitE   ((CList)otype->props,(CListVisitEntryV_Function)OPropertyPutHTML_InStream,file); 
	}
      fprintf       (file,"<HR>\n");
      
    }

  fclose (file);
  CInfoF ("File %s produced.\n",a_name);
}
/***************************************************************************/
int OTypeIsValid (
 OType This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
OType OTypeGetIdentifier (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OType)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OType  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OTypeGetName(This),a_name);
}
/***************************************************************************/
OType OTypeCreate (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType            This;
/*.........................................................................*/
  if(CStringIsWord(a_name)==0) 
    {
      ReportError("OTypeCreate",CErrorValue);
      return NULL;
    }

  This              = (OType)CMemoryAllocateBlock (sizeof(OTypeRecord));
  if(This==NULL)    return NULL;
  CListAddEntry     ((CList*)&(Class.extent),This);

  This->name           = CStringDuplicate(a_name);
  This->props          = NULL;
  This->extentFunction       = NULL;
  This->makeFunction         = NULL;
  This->constructFunction    = NULL;
  This->validateFunction     = NULL;
  This->deleteFunction       = NULL;
  This->destroyFunction      = NULL;
  This->setPropFunction      = NULL;
  This->getPropFunction      = NULL;
  This->setAttributeFunction = NULL;
  This->getAttributeFunction = NULL;
  This->setRefsFunction      = NULL;
  This->getRefsFunction      = NULL;
  This->repFunction          = NULL;
  This->clearClassFunction   = NULL;
  This->doPropFunction       = NULL;
  This->doMethodFunction     = NULL;
  This->user                = NULL;

  return               This;
}
/***************************************************************************/
void OTypeDelete (
  OType This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  CListRemoveEntry ((CList)Class.extent,This);
  FreeRecord       (This);
}
/***************************************************************************/
static void FreeRecord (
 OType This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy     ((CList)(This->props),(CListVisitEntryFunction)OPropertyDelete);
  This->props      = NULL;
  CStringDelete    (This->name);
  This->name       = NULL;
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void OTypeSetGetIdentifiersFunction (
 OType This
,OTypeGetIdentifiersFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->extentFunction  = a_proc;
}
/***************************************************************************/
void OTypeSetMakeFunction (
 OType This
,OTypeMakeFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->makeFunction  = a_proc;
}
/***************************************************************************/
void OTypeSetConstructFunction (
 OType This
,OTypeConstructFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->constructFunction  = a_proc;
}
/***************************************************************************/
void OTypeSetValidateFunction (
 OType This
,OTypeValidateFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->validateFunction  = a_proc;
}
/***************************************************************************/
void OTypeSetDeleteFunction (
 OType This
,OTypeDeleteFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->deleteFunction  = a_proc;
}
/***************************************************************************/
void OTypeSetDestroyFunction (
 OType This
,OTypeDestroyFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->destroyFunction  = a_proc;
}
/***************************************************************************/
void OTypeSetSetPropertyFunction (
 OType This
,OTypeSetPropertyFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->setPropFunction  = a_proc;
  This->setAttributeFunction   = NULL;
}
/***************************************************************************/
void OTypeSetDoPropertyFunction (
 OType This
,OTypeDoPropertyFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->doPropFunction  = a_proc;
  This->doMethodFunction   = NULL;
}
/***************************************************************************/
void OTypeSetSetReferencesFunction (
 OType This
,OTypeSetReferencesFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->setRefsFunction  = a_proc;
}
/***************************************************************************/
void OTypeSetGetReferencesFunction (
 OType This
,OTypeGetReferencesFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->getRefsFunction  = a_proc;
}
/***************************************************************************/
void OTypeSetGetPropertyFunction (
 OType This
,OTypeGetPropertyFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->getPropFunction      = a_proc;
  This->getAttributeFunction = NULL;
}
/***************************************************************************/
void OTypeSetSetAttributeFunction (
 OType This
,OTypeSetAttributeFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->setAttributeFunction  = a_proc;
  This->setPropFunction = NULL;
}
/***************************************************************************/
void OTypeSetGetAttributeFunction (
 OType This
,OTypeGetAttributeFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->getAttributeFunction  = a_proc;
  This->getPropFunction = NULL;
}
/***************************************************************************/
void OTypeSetDoMethodFunction (
 OType This
,OTypeDoMethodFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->doMethodFunction  = a_proc;
  This->doPropFunction = NULL;
}
/***************************************************************************/
void OTypeSetRepresentFunction (
 OType This
,OTypeRepresentFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->repFunction  = a_proc;
}
/***************************************************************************/
void OTypeSetClearClassFunction (
 OType This
,OTypeClearClassFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->clearClassFunction  = a_proc;
}
/***************************************************************************/
OTypeClearClassFunction OTypeGetClearClassFunction (
 OType This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->clearClassFunction;
}
/***************************************************************************/
OTypeMakeFunction OTypeGetMakeFunction (
 OType This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->makeFunction;
}
/***************************************************************************/
void OTypeSetUserData (
 OType This
,void* a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->user = a_user;
}
/***************************************************************************/
void OTypeAddProperty (
 OType This 
,OProperty a_prop
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_prop==NULL) return;
  CListAddEntry  ((CList*)&(This->props),a_prop);
}
/***************************************************************************/
OProperty OTypeAddNewProperty (
 OType This 
,char* a_name
,int   a_type
,void* a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty property;
/*.........................................................................*/
  if(This==NULL) return NULL;
  property         = OPropertyCreate(a_name,a_type,a_user);
  OTypeAddProperty (This,property);
  return           property;
}
/***************************************************************************/
OProperty OTypeAddNewReferencesProperty (
 OType This 
,char* a_name
,int   a_type
,void* a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty property;
/*.........................................................................*/
  if(This==NULL) return NULL;
  property         = OPropertyCreateReferences (a_name,a_type,a_user);
  OTypeAddProperty (This,property);
  return           property;
}
/***************************************************************************/
OProperty OTypeAddNewMethodProperty (
 OType This 
,char* a_name
,int   a_type
,void* a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty property;
/*.........................................................................*/
  if(This==NULL) return NULL;
  property         = OPropertyCreateMethod(a_name,a_type,a_user);
  OTypeAddProperty (This,property);
  return           property;
}
/***************************************************************************/
OProperty OTypeAddNewClassProperty (
 OType This 
,char* a_name
,int   a_type
,void* a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty property;
/*.........................................................................*/
  if(This==NULL) return NULL;
  property         = OPropertyCreateClass(a_name,a_type,a_user);
  OTypeAddProperty (This,property);
  return           property;
}
/***************************************************************************/
OProperty OTypeAddNewClassMethodProperty (
 OType This 
,char* a_name
,int   a_type
,void* a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty property;
/*.........................................................................*/
  if(This==NULL) return NULL;
  property         = OPropertyCreateClassMethod(a_name,a_type,a_user);
  OTypeAddProperty (This,property);
  return           property;
}
/***************************************************************************/
OProperty OTypeAddNewPropertyUserDataInteger (
 OType This 
,char* a_name
,int   a_type
,int   a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty property;
/*.........................................................................*/
  if(This==NULL) return NULL;
  property                    = OPropertyCreate(a_name,a_type,NULL);
  OPropertySetUserDataInteger (property,a_user);
  OTypeAddProperty            (This,property);
  return                      property;
}
/***************************************************************************/
char* OTypeGetName (
 OType This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
OTypeRepresentFunction OTypeGetRepresentFunction (
 OType This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->repFunction;
}
/***************************************************************************/
void* OTypeGetUserData (
 OType This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->user;
}
/***************************************************************************/
OProperty OTypeGetPropertyIdentifier (
 OType This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return (OProperty)CListFindEntryBackward ((CList)This->props,(CListFindEntryFunction)GetPropertyIdentifier,a_name);
}
/***************************************************************************/
static int GetPropertyIdentifier (
 OProperty  This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(OPropertyGetName(This),a_name);
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void OTypeLoadFile (
 char* a_name
,char* a_type
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*............................................................................*/
  CListDestroy ((CList)OTypeGetExtentFromFile(a_name,a_type,NULL),(CListVisitEntryFunction)OHandleDelete);
}
/******************************************************************************/
OHandle* OTypeGetExtentFromFile (
 char* a_name
,char* a_type
,char* a_filter
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* type = NULL;
  Loader* itema;
/*............................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') )  return NULL;
  if( (a_type!=NULL)  && (*a_type!='\0') )
    {
      type  = CStringDuplicate(a_type);     
    }
  else
    {
      type       = CFileGetType (a_name);
      if( (type==NULL) || (*type=='\0') ) 
	{
	  CStringDelete (type);
	  type          = CStringDuplicate("odb");
	}
    }
  for(itema=Class.loaders;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (type!=NULL) && (strcmp(type,(*itema)->name)==0) )
	{
	  OTypeLoadFunction  proc;
          proc               = (*itema)->proc;
	  CStringDelete      (type);
	  return             proc(a_name,a_filter);
	}
    }
  CWarnF  ("Can't find a loader for file %s of type %s.\n",a_name,a_type);
  CStringDelete (type);
  return NULL;
}
/******************************************************************************/
OHandle* OTypeLoadODB (
 char* a_name
,char* a_filter
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*    file;
  char*    fname;
  OHandle* hands = NULL;
  int      status = 1;
/*............................................................................*/
  file     = CFileOpenForReading (a_name,&fname);
  if(file!=NULL)    
    { 
      int            count;
      OProcedureList procs;
      int            linen   = 0;
      char**         lines   = NULL;
      char*          lineb   = NULL;
      int            rnumber = 0;
      OType*         rtypes  = NULL;
      OIdentifier*   roids   = NULL;
      OProperty*     rprops  = NULL;
      char**         rvals   = NULL;

      lineb       = CStreamGetStrings (file,'\\',&linen,&lines);
      fclose      (file);

      procs       = OProcedureListCreate (linen,lines,"begin","end");
      LOOP_BEGIN_PCD(procs)
	char*     line;
        char*     word1;
        char*     word2;
        line      = lines[OProcedureGetTitlePosition(*itema)];
        word1     = CStringGetFirstWord  (line);
        word2     = CStringGetSecondWord (line);
        if( (word1!=NULL) && (word2!=NULL) )  
	  { 
	    OType         type;
	    type          = OTypeGetIdentifier (word2);
	    if(type==NULL)
	      {
		CWarnF ("File %s not loaded :\n type %s is unknown.\n",a_name,word2);
		status = 0;
	      }
	    else if( (a_filter==NULL) || (strcmp(a_filter,word2)==0) )
	      {
		int           parn = 0;
		char**        pars = NULL;
		char**        vals = NULL;
		OIdentifier   obj  = NULL;
		OProperty     prop;
		LOOP_BEGIN_PCD_LINE  (lines,*itema)
		    char*         string;
		    NOTEMPTY 
		    NOTCOMMENT 
                    string      = CStringDuplicate(strloop);
		    if(string!=NULL)
		      {
			char* pos;
			pos   = strstr(string," = ");
			if(pos==NULL)
			  { 
			    CWarnF        ("File %s not loaded :\n \" = \" expected in :\n%s\n",a_name,strloop);
			    status        = 0;
			  }
			else
			  { 
			    char*          sprop;
			    *pos           = '\0';
			    sprop          = CStringDuplicate(string);                       
			    CStringTrim    (sprop);
			    CListAddEntry  ((CList*)&pars,CStringDuplicate(sprop));                            
			    CListAddEntry  ((CList*)&vals,CStringDuplicate(pos+3));
			    parn          ++;
			    CStringDelete (sprop);
			  }
		      }
		    CStringDelete (string);
		    if(status==0) break;
		LOOP__END_PCD_LINE 
		if(status==1)
		  {
		    CListAddEntry  ((CList*)&pars,CStringDuplicate("dataBaseName"));                            
		    CListAddEntry  ((CList*)&vals,CStringDuplicate(a_name));
		    parn          ++;
		    CListAddEntry  ((CList*)&pars,CStringDuplicate("dataBaseFile"));                            
		    CListAddEntry  ((CList*)&vals,CStringDuplicate(fname));
		    parn          ++;
		    if(type->makeFunction!=NULL)
		      {
			obj = OTypeMakeObject(type);
			if(obj!=NULL)
			  {
			    for(count=0;count<parn;count++)  /*Set property that are not references.*/
			      {
				prop   = OTypeGetPropertyIdentifier (type,pars[count]);
				if(prop==NULL)
				  {
				    if( (strcmp(pars[count],"dataBaseName")!=0) &&
					(strcmp(pars[count],"dataBaseFile")!=0) 
					)
				      {
					CWarnF ("File %s not loaded :\n %s is not a property of %s.\n",a_name,pars[count],word2);
				      }
				  }
				else if(OPropertyIsListOfReferences(prop)==0)
				  {
				    if(OTypeSetObjectPropertyFromString (type,obj,prop,vals[count])==0)
				      {
					CWarnF ("File %s not loaded :\n can't set a property.\n",a_name);
					status = 0;
					break;
				      }
				  }
			      }
			    if( (status==1) && (type->validateFunction!=NULL) )
			      {
				if(type->validateFunction(obj)==0)
				  {
				    /*No warning produced. Could be a well handled situation.*/
				    status = 0;
				  }
			      }
			  }
			else
			  {
			    CWarnF ("File %s not loaded :\n can't make an object.\n",a_name);
			    status = 0;
			  }
		      }
		    else
		      {
			obj = OTypeConstructObject (type,parn,pars,vals);	
			if(obj==NULL)
			  {
			    /*No warning produced. Could be a well handled situation.*/
			    status = 0;
			  }
		      }
		    if(obj!=NULL) 
		      {
			CListAddEntry ((CList*)&hands,OHandleCreate(type,obj));
			for(count=0;count<parn;count++)
			  {
			    prop   = OTypeGetPropertyIdentifier (type,pars[count]);
			    if(OPropertyIsListOfReferences(prop)==1)
			      {
				CListAddEntry          ((CList*)&rtypes,type);
				CListAddEntry          ((CList*)&roids  ,obj);
				CListAddEntry          ((CList*)&rprops,prop);
				CListAddEntry          ((CList*)&rvals ,CStringDuplicate(vals[count]));
				rnumber               ++;
			      }
			  }
		      }
		  }
		CListDestroy ((CList)pars,(CListVisitEntryFunction)CStringDelete);
		CListDestroy ((CList)vals,(CListVisitEntryFunction)CStringDelete);
	      }
	  } 
	else
	  {
	    CWarnF ("File %s not loaded :\n two words (begin <Type>) are expected in :\n%s\n",a_name,line);
	    status = 0;
	  }
        CStringDelete (word1);
        CStringDelete (word2);
	if(status==0) break;
      LOOP_END_PCD
      OProcedureListDelete    (procs);
      CMemoryFreeBlock        (lines);
      CMemoryFreeBlock        (lineb);
      /*Here hands contains objects for a_name file.*/
      if(status==1)
	{
	  for(count=0;count<rnumber;count++)   /*SetRef could load sub files.*/
	    {
	      OHandle*     shands;   /* shands = hands + new obj */
	      shands       = OTypeSetObjectReferences (rtypes[count],roids[count],rprops[count],rvals[count],hands);
	      CListDelete  ((CList)hands); 
	      hands        = shands;
	    }
	}
      CListDelete  ((CList)rtypes);
      CListDelete  ((CList)roids);
      CListDelete  ((CList)rprops);
      CListDestroy ((CList)rvals,(CListVisitEntryFunction)CStringDelete);

    } /*fi file*/
  else
    {
      status = 0;
    }

  CStringDelete (fname);
  
  if(status==0) 
    {
      if(hands!=NULL)
	{
	  OHandle* handa;
	  for(handa=hands;*handa!=NULL;handa++) 
	    {
	      OTypeDeleteObject ((OType)OHandleGetType(*handa),(OIdentifier)OHandleGetIdentifier(*handa));
	    }
	  CListDestroy ((CList)hands,(CListVisitEntryFunction)OHandleDelete);
	  hands        = NULL;
	}
    }

  return      hands;
}
/***************************************************************************/
OIdentifier* OTypeGetObjectIdentifiers (
 OType  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  if(This->extentFunction==NULL) return NULL;
  return This->extentFunction(This);
}
/***************************************************************************/
OIdentifier* OTypeCollectObjects (
 OType   This
,int     a_objn
,OIdentifier* a_objs
,OProperty   a_prop
,int     a_cmp
,void*   a_addr
,int     a_number
,int*    a_robjn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int             objn = 0;
  OIdentifier*         objs = NULL;
/*.........................................................................*/
  if(a_robjn!=NULL)    *a_robjn = 0;
  if(This==NULL)       return NULL;
  if( (a_objn!=0) && (a_objs!=NULL) )   
    {
      objs = (OIdentifier*)CListCreate(a_objn);
      if(objs!=NULL)
        {
	   int count;
	  objn = 0;
	  for(count=0;count<a_objn;count++)
	    {
              if(OTypeSelectObject(This,a_objs[count],a_prop,a_cmp,a_addr,a_number)==1)
                {
		  objs[objn] = a_objs[count];
                  objn++;
		}
	    }
	  objs[objn] = NULL;
	}
    }
  if(a_robjn!=NULL)    *a_robjn = objn;
  return          objs;
}
/***************************************************************************/
char* OTypeDumpObjectsInString (
 OType   This
,int     a_objn
,OIdentifier* a_objs
,int          a_propn
,OProperty*   a_props
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  OIdentifier  OBJ;
  int          found;
  char*        string;
  char*        name;
  int          propi;
  int          propn;
  OProperty*   props;
/*.........................................................................*/
  if(This==NULL) return NULL;
  if( (This->getPropFunction==NULL) && (This->getAttributeFunction==NULL) )
    {
      CWarnF ("For type %s; no GetProp or GetVal routine declared.\n",This->name);
      return NULL;
    }
  if(a_objn<=0)
    {
      CWarnF ("No instances for type %s.\n",This->name);
      return NULL;
    }
  if(This->props==NULL)
    {
      CWarnF ("Type %s has no property.\n",This->name);
      return NULL;
    }
  if( (a_propn==0) || (a_props==NULL) )
    {
      propn = CListGetSize((CList)This->props);
      props = (OProperty*)CListDuplicate((CList)This->props);
    }
  else
    {
      propn = a_propn;
      props = (OProperty*)CListDuplicate((CList)a_props);
    }
  /*Remove method properties.*/
  propi = 0;
  for(count=0;count<propn;count++)
    {
      if(OPropertyIsMethod(props[count])==0)
	{
	  props[propi] = props[count];
	  propi++;
	}
    }
  props[propi] = NULL;
  propn        = propi;
  /*Look if some objects are classes.*/
  found = 0;
  for(count=0;count<a_objn;count++)
    {
      OBJ = a_objs[count]; /*If OBJ==NULL, means class.*/
      if(OBJ==NULL) 
	{
	  found = 1;
	  break;
	}
    }
  if(found==1)  /*Dump class first.*/
    {
      string   = CStringCreateF (128,"Type %s informations :\n",This->name);
      for(propi=0;propi<propn;propi++)
	{
	  if( (OPropertyIsClass(props[propi])==1) &&
	      (OPropertyIsArray(props[propi])==0)
	     )
	    {
	      name               = OPropertyGetHeader (props[propi]);
	      CStringConcatenate (&string," ");
	      CStringConcatenate (&string,name);
	      CStringDelete      (name);
	    }
	}          
      CStringConcatenate (&string,"\n");
      OBJ = NULL;
	      
      for(propi=0;propi<propn;propi++)
	{
	  if( (OPropertyIsClass(props[propi])==1) &&
	      (OPropertyIsArray(props[propi])==0)
	     )
	    {
	      void*                  variable;
	      int                    status;
	      variable               = OPropertyGetVariable   (props[propi]);
	      status                 = OTypeGetObjectProperty (This,OBJ,props[propi],variable,NULL);
              if(status==0)          variable = NULL;
	      name                   = OPropertyPrintVariableInString (props[propi],variable);
	      CStringConcatenate     (&string," ");
	      CStringConcatenate     (&string,name);
	      CStringDelete          (name);
	      CMemoryFreeVariable    (variable,0,status);
	    }
	}          
      for(propi=0;propi<propn;propi++)
	{
	  if( (OPropertyIsClass(props[propi])==1) &&
	      (OPropertyIsArray(props[propi])==1)
	     )
	    {
	      int                    number   = 0;
	      void*                  variable;
	      int                    status;
	      variable               = OPropertyGetVariable   (props[propi]);
	      status                 = OTypeGetObjectProperty (This,OBJ,props[propi],variable,&number);
              if(status==0)          variable = NULL;
	      name                   = OPropertyPrintArrayInString (props[propi],variable,number);
	      CStringConcatenate     (&string,name);
	      CStringDelete          (name);
	      CMemoryFreeVariable    (variable,number,status);
	    }
	}
    }
  /*Look if some objects are not classes.*/
  found = 0;
  for(count=0;count<a_objn;count++)
    {
      OBJ = a_objs [count]; /*If OBJ==NULL, means class.*/
      if(OBJ!=NULL) 
	{
	  found = 1;
	  break;
	}
    }
  if(found==1)
    {
      string   = CStringCreateF (128,"%s informations (%d objects) :\n",This->name,a_objn);
      for(propi=0;propi<propn;propi++)
	{
	  if( (OPropertyIsClass(props[propi])==0) &&
	      (OPropertyIsArray(props[propi])==0)
	     )
	    {
	      name               = OPropertyGetHeader (props[propi]);
	      CStringConcatenate (&string," ");
	      CStringConcatenate (&string,name);
	      CStringDelete      (name);
	    }
	}          
      CStringConcatenate (&string,"\n");
      
      for(count=0;count<a_objn;count++)
	{
	  OBJ = a_objs [count]; /*If OBJ==NULL, means class.*/
	  if(OBJ==NULL) continue;

	  for(propi=0;propi<propn;propi++)
	    {
	      if( (OPropertyIsClass(props[propi])==0) &&
		 (OPropertyIsArray(props[propi])==0)
		 )
		{
		  void*                  variable;
		  int                    status;
		  variable               = OPropertyGetVariable   (props[propi]);
		  status                 = OTypeGetObjectProperty (This,OBJ,props[propi],variable,NULL);
		  if(status==0)          variable = NULL;
		  name                   = OPropertyPrintVariableInString (props[propi],variable);
		  CStringConcatenate     (&string," ");
		  CStringConcatenate     (&string,name);
		  CStringDelete          (name);
		  CMemoryFreeVariable    (variable,0,status);
		}
	    }          
	  for(propi=0;propi<propn;propi++)
	    {
	      if( (OPropertyIsClass(props[propi])==0) &&
		  (OPropertyIsArray(props[propi])==1)
		 )
		{
		  int                    number   = 0;
		  void*                  variable;
		  int                    status;
		  variable               = OPropertyGetVariable   (props[propi]);
		  status                 = OTypeGetObjectProperty (This,OBJ,props[propi],variable,&number);
		  if(status==0)          variable = NULL;
		  name                   = OPropertyPrintArrayInString (props[propi],variable,number);
		  CStringConcatenate     (&string,name);
		  CStringDelete          (name);
		  CMemoryFreeVariable    (variable,number,status);
		}
	    }
	  if(count!=a_objn-1) CStringConcatenate (&string,"\n");
	}
    }

  CListDelete((CList)props);

  return string;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
OIdentifier OTypeMakeObject (
 OType  This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return NULL;
  if(This->makeFunction!=NULL)
    {
      return This->makeFunction(This);
    }
  else
    {
      CWarnF ("For type %s; no NewProp routine declared.\n",This->name);
      return NULL;
    }
}
/***************************************************************************/
OIdentifier OTypeConstructObject (
 OType  This
,int    a_parn
,char** a_pars
,char** a_vals
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)          return NULL;
  if(This->constructFunction!=NULL) 
    {
      return This->constructFunction(This,a_parn,a_pars,a_vals);
    }
  else
    {
      CWarnF ("For type %s; no Construct routine declared.\n",This->name);
      return NULL;
    }
}
/***************************************************************************/
OIdentifier OTypeGetObjectIdentifier (
 OType This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OIdentifier*   itema;
  OProperty oprop;
/*.........................................................................*/
  if(This==NULL)                 return NULL;
  if( (a_name==NULL) || (*a_name=='\0')) return NULL;
  if( (This->getPropFunction==NULL) && (This->getAttributeFunction==NULL) )
    {
      CWarnF ("For type %s; no GetProp or GetVal routine declared.\n",This->name);
      return NULL;
    }
  oprop = OTypeGetPropertyIdentifier(This,"name");
  if(oprop==NULL)
    {
      CWarnF ("name is not a property of %s.\n",This->name);
      return NULL;
    }
  for(itema=OTypeGetObjectIdentifiers(This);(itema!=NULL) && (*itema!=NULL);itema++)
    {
      char*  value = NULL;
      if(OTypeGetObjectProperty(This,*itema,oprop,&value,NULL)!=0) /*Name, by convention, has never to be freed.*/
	{
	  if( (value!=NULL) && (strcmp(a_name,value)==0) ) 
	    {
	      return *itema;
	    }
	}
    } 
  return     NULL;
}
/***************************************************************************/
char* OTypeGetObjectName (
 OType This
,OIdentifier a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProperty oprop;
  char*     value = NULL;
/*.........................................................................*/
  if(This==NULL) return NULL;
  oprop = OTypeGetPropertyIdentifier(This,"name");
  if(oprop==NULL)
    {
      CWarnF ("name is not a property of %s.\n",This->name);
      return NULL;
    }
  if(OTypeGetObjectProperty(This,a_obj,oprop,&value,NULL)==0) return NULL; /*Name, by convention, has never to be freed.*/
  return        value;
}
/***************************************************************************/
int OTypeIsObjectValid (
 OType  This
,OIdentifier a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OIdentifier* itema;
/*.........................................................................*/
  if(This==NULL) return 0;
  for(itema=OTypeGetObjectIdentifiers(This);(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(*itema==a_obj) return 1;
    } 
  return 0;
}
/***************************************************************************/
void OTypeDeleteObject (
 OType  This
,OIdentifier a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)             return;
  if(This->deleteFunction!=NULL)
    {
      This->deleteFunction(a_obj);
    }
  else
    {
      CWarnF ("For type %s; no Delete routine declared.\n",This->name);
    }
}
/***************************************************************************/
void OTypeDestroyObject (
 OType  This
,OIdentifier a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)             return;
  if(This->destroyFunction!=NULL)
    {
      This->destroyFunction(a_obj);
    }
  else
    {
      CWarnF ("For type %s; no Destroy routine declared.\n",This->name);
    }
}
/***************************************************************************/
int OTypeSetObjectProperty (
 OType       This
,OIdentifier a_obj
,OProperty   a_prop
,void*       a_addr
,int         a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)              return 0;
       if(This->setPropFunction!=NULL)
    {
      return This->setPropFunction(a_obj,This,a_prop,a_addr,a_number);
    }
  else if(This->setAttributeFunction!=NULL)
    {
      return This->setAttributeFunction(a_obj,OPropertyGetName(a_prop),OPropertyGetUserData(a_prop),a_addr,a_number);
    }
  else
    {
      CWarnF ("For type %s; no SetProperty or SetAttribute routine declared.\n",This->name);
    }
  return 0;
}
/***************************************************************************/
int OTypeGetObjectProperty (
 OType  This
,OIdentifier a_obj
,OProperty  a_prop
,void*  a_addr
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
       if(This->getPropFunction!=NULL)
    {
      return This->getPropFunction(a_obj,This,a_prop,a_addr,a_number);
    }
  else if(This->getAttributeFunction!=NULL)
    {
      return This->getAttributeFunction(a_obj,OPropertyGetName(a_prop),OPropertyGetUserData(a_prop),a_addr,a_number);
    }
  else
    {
      CWarnF ("For type %s; no GetProperty or GetAttribute routine declared.\n",This->name);
    }
  return 0;
}
/***************************************************************************/
int OTypeDoObjectProperty (
 OType  This
,OIdentifier a_obj
,OProperty  a_prop
,int    a_argn
,char** a_args
,void*  a_addr
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
       if(This->doPropFunction!=NULL)
    {
      return This->doPropFunction (a_obj,This,a_prop,a_argn,a_args,a_addr,a_number);
    }
  else if(This->doMethodFunction!=NULL)
    {
      return This->doMethodFunction (a_obj,OPropertyGetName(a_prop),OPropertyGetUserData(a_prop),a_argn,a_args,a_addr,a_number);
    }
  else
    {
      CWarnF ("For type %s; no DoProperty or DoAttribute routine declared.\n",This->name);
    }
  return 0;
}
/***************************************************************************/
OHandle* OTypeSetObjectReferences (
 OType       This
,OIdentifier a_obj
,OProperty   a_prop
,char*       a_string
,OHandle*   a_hands
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  if(This->setRefsFunction!=NULL) 
    {
      return This->setRefsFunction(This,a_obj,a_prop,a_string,a_hands);
    }
  else
    {
      CWarnF ("For type %s; no SetRefs routine declared.\n",This->name);
      return NULL;
    }
}
/***************************************************************************/
void OTypeGetObjectReferences (
 OType       This
,OIdentifier a_obj
,OProperty   a_prop
,char**      a_string
,OHandle**   a_hands
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(This->getRefsFunction!=NULL) 
    {
      This->getRefsFunction (This,a_obj,a_prop,a_string,a_hands);
    }
  else
    {
      CWarnF ("For type %s; no GetRefs routine declared.\n",This->name);
    }
}
/***************************************************************************/
int OTypeSetObjectPropertyFromString (
 OType  This
,OIdentifier a_obj
,OProperty  a_prop
,char*  a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  void*  variable;
  int    number = 0;
/*.........................................................................*/
  if(This==NULL)  return 0;
  variable = OPropertyGetVariable   (a_prop);
  if(OPropertySetVariableFromString(a_prop,a_string,variable,&number)==0) return 0;
  return   OTypeSetObjectProperty (This,a_obj,a_prop,variable,number);
}
/***************************************************************************/
char* OTypeGetAndConvertObjectPropertyToString (
 OType  This
,OIdentifier a_obj
,OProperty  a_prop
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  void* variable;
  int   number = 0;
  int   status;
  char* string;
/*.........................................................................*/
  if(This==NULL)  return NULL;
  variable            = OPropertyGetVariable   (a_prop);
  status              = OTypeGetObjectProperty(This,a_obj,a_prop,variable,&number);
  if(status==0)       variable = NULL;
  string              = OPropertyConvertVariableToString (a_prop,variable,number);
  CMemoryFreeVariable (variable,number,status);
  return              string;
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
  CErrorHandle ("Co","OType",a_name,a_code);
}
/***************************************************************************/
int OTypeSelectObject (
 OType  This
,OIdentifier a_obj
,OProperty  a_prop
,int    a_cmp
,void*  a_addr
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int type;
/*.........................................................................*/
  if(This==NULL) return 0;
  type      = OPropertyGetType  (a_prop);
       if( (type==OPropertyInteger) || 
           (type==OPropertyBoolean)
	  )
    {
      int    VALUE = 0;
      int    value = *((int*)a_addr);
      if(OTypeGetObjectProperty(This,a_obj,a_prop,(void*)&VALUE,NULL)==0) return 0;
      if(
	 ((a_cmp==IF_EQ) && (VALUE==value)) ||
	 ((a_cmp==IF_NE) && (VALUE!=value)) ||
	 ((a_cmp==IF_LE) && (VALUE<=value)) ||
	 ((a_cmp==IF_LT) && (VALUE< value)) ||
	 ((a_cmp==IF_GE) && (VALUE>=value)) ||
	 ((a_cmp==IF_GT) && (VALUE> value)) 
	 ) return 1;
      return 0;
    }
  else if(type==OPropertyDouble)
    {
      double VALUE = 0.;
      double value = *((double*)a_addr);
      if(OTypeGetObjectProperty (This,a_obj,a_prop,(void*)&VALUE,NULL)==0) return 0;
      if(
	 ((a_cmp==IF_EQ) && (VALUE==value)) ||
	 ((a_cmp==IF_NE) && (VALUE!=value)) ||
	 ((a_cmp==IF_LE) && (VALUE<=value)) ||
	 ((a_cmp==IF_LT) && (VALUE< value)) ||
	 ((a_cmp==IF_GE) && (VALUE>=value)) ||
	 ((a_cmp==IF_GT) && (VALUE> value)) 
	 ) return 1;
    }
  else if( (type==OPropertyUnsignedLong) )
    {
      Ulong  VALUE = 0L;
      Ulong  value = *((Ulong*)a_addr);
      if(OTypeGetObjectProperty (This,a_obj,a_prop,(void*)&VALUE,NULL)==0) return 0;
      if(
	 ((a_cmp==IF_EQ) && (VALUE==value)) ||
	 ((a_cmp==IF_NE) && (VALUE!=value)) ||
	 ((a_cmp==IF_LE) && (VALUE<=value)) ||
	 ((a_cmp==IF_LT) && (VALUE< value)) ||
	 ((a_cmp==IF_GE) && (VALUE>=value)) ||
	 ((a_cmp==IF_GT) && (VALUE> value)) 
	 ) return 1;
    }
  else if(type==OPropertyFloat)
    {
      float  VALUE = 0.;
      float  value = *((float*)a_addr);
      if(OTypeGetObjectProperty (This,a_obj,a_prop,(void*)&VALUE,NULL)==0) return 0;
      if(
	 ((a_cmp==IF_EQ) && (VALUE==value)) ||
	 ((a_cmp==IF_NE) && (VALUE!=value)) ||
	 ((a_cmp==IF_LE) && (VALUE<=value)) ||
	 ((a_cmp==IF_LT) && (VALUE< value)) ||
	 ((a_cmp==IF_GE) && (VALUE>=value)) ||
	 ((a_cmp==IF_GT) && (VALUE> value)) 
	 ) return 1;
    }
  else if(type==OPropertyString)
    {
      char*  VALUE = NULL;
      int    status;
      int    retval = 0;
      status = OTypeGetObjectProperty (This,a_obj,a_prop,(void*)&VALUE,NULL);
      if(status==0) return 0;
      if(a_number!=0)
	{
	  char** value = *((char***)a_addr);
	  int    pos;
	  pos    = CTextGetLineIndex (a_number,value,VALUE);
	  if(
	     ((a_cmp==IF_IN) && (pos!=NotFound) ) ||
	     ((a_cmp==IF_NI) && (pos==NotFound) ) 
	     ) retval = 1;
	}
      else
	{
	  char*  value = *((char**)a_addr);
	  int    eq;
	  eq     = CStringMatchPattern (VALUE,value);
	  if(
	     ((a_cmp==IF_EQ) && (eq==1) ) ||
	     ((a_cmp==IF_NE) && (eq==0) ) 
	     ) retval = 1;
	}
      CMemoryFreeVariable ((void*)&VALUE,0,status);
      return              retval;
    }
  return 0;
}
/***************************************************************************/
void OTypePutObjectInODB_Stream (
 OType   This
,OIdentifier  a_obj
,void*   a_file
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  file;
  OProperty* itema;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_obj==NULL)  return;
  if(a_file==NULL) return;

  if( (This->getPropFunction==NULL) && (This->getAttributeFunction==NULL) )
    {
      CWarnF ("For type %s; no GetProp or GetVal routine declared.\n",This->name);
      return;
    }

  file    = (FILE*)a_file;
  fprintf (file,"begin %s\n",OTypeGetName(This));
  for(itema = This->props;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      char*   name;
      char*   string = NULL;
      name    = OPropertyGetName(*itema);
      if(OPropertyIsListOfReferences(*itema)==1)
	{
	  OTypeGetObjectReferences (This,a_obj,*itema,&string,NULL);
	}
      else if(OPropertyIsMethod(*itema)==1)
	{
	}
      else if( (name!=NULL) && (strcmp(name,"identifier")!=0) )
	{
	  string = OTypeGetAndConvertObjectPropertyToString (This,a_obj,*itema);
	}
      if(string!=NULL)
        {
	  CStringReplacePart    (&string,"\n","\\\n");
	  fprintf        (file," %s = %s\n",name,string);
	  CStringDelete  (string);
	}  
    }
  fprintf (file,"end\n");
/**/
  for(itema = This->props;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OPropertyIsListOfReferences(*itema)==1)
	{
          OHandle* hands;
	  OTypeGetObjectReferences (This,a_obj,*itema,NULL,&hands);
	  if(hands!=NULL)
	    {
	      OHandle* handa;
	      for(handa=hands;*handa!=NULL;handa++) 
		{
		  OTypePutObjectInODB_Stream 
		  ((OType)OHandleGetType(*handa),(OIdentifier)OHandleGetIdentifier(*handa),a_file);
		}
	    }
	  CListDestroy ((CList)hands,(CListVisitEntryFunction)OHandleDelete);
	}
    }
}
/***************************************************************************/
int OTypeSetAttribute (
 OType This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int    a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

  if(This==NULL)
    {
      if(strcmp(a_name,"dumpMemory")==0)   
	{
	  Class.dumpMemory = *((int*)a_addr);
	}
      else 
	{
	  CInfoF ("OTypeSetAttribute : %s is not a class property.\n",a_name);
	  return 0;
	}
      return 1;
    }

  a_user   = NULL;
  a_number = 0;
  return   1;
}
/***************************************************************************/
int OTypeGetAttribute (
 OType This
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
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

  if(This==NULL) 
    {
      if(strcmp(a_name,"dumpMemory")==0)   
	{
	  *((int*)a_addr) = Class.dumpMemory;
	}
      else 
	{
	  CInfoF ("OTypeGetAttribute : %s is not a class property.\n",a_name);
	  return 0;
	}
      return 1;
    }

       if(strcmp(a_name,"identifier")==0)                *((Ulong*)a_addr)  = (Ulong)This;
  else if(strcmp(a_name,"name")==0)                      *((char**)a_addr)  = OTypeGetName((OType)This);
  else if(strcmp(a_name,"hasGetIdentifiersFunction")==0) *((int*)a_addr)    = This->extentFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasMakeFunction")==0)           *((int*)a_addr)    = This->makeFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasConstructFunction")==0)      *((int*)a_addr)    = This->constructFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasValidateFunction")==0)       *((int*)a_addr)    = This->validateFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasDeleteFunction")==0)         *((int*)a_addr)    = This->deleteFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasDestroyFunction")==0)        *((int*)a_addr)    = This->destroyFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasSetPropertyFunction")==0)    *((int*)a_addr)    = This->setPropFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasGetPropertyFunction")==0)    *((int*)a_addr)    = This->getPropFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasSetAttributeFunction")==0)   *((int*)a_addr)    = This->setAttributeFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasGetAttributeFunction")==0)   *((int*)a_addr)    = This->getAttributeFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasRepresentFunction")==0)      *((int*)a_addr)    = This->repFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"hasClearClassFunction")==0)     *((int*)a_addr)    = This->clearClassFunction!=NULL ? 1 : 0;
  else if(strcmp(a_name,"properties")==0)
    {
      OProperty*    itema;
      int           stringn = 0;
      char**        strings = NULL;
      for(itema=This->props;(itema!=NULL) && (*itema!=NULL);itema++)
	{
	  char*                 string;
	  string                = CStringCreateF(10+64,"OProperty/%lu",(unsigned long)(*itema));
	  CTextAddAllocatedLine (&stringn,&strings,string);
	}
      *((char***)a_addr) = strings;
      if(a_number!=NULL) *a_number = stringn;
      return             FREE_BLOCKS;
    }
  else 
    {
      CInfoF("Unknown property %s of type OType.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
char* OTypeConvertTypeAndIdentifierToString (
 OType  This
,OIdentifier a_obj
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*   string;
/*.........................................................................*/
  if(This==NULL) return NULL;
  string  = (char*)CMemoryAllocateBlock ((strlen(This->name)+64)*sizeof(char));
  sprintf (string,"%s/%lu",This->name,(unsigned long)a_obj);
  return  string;
}
/***************************************************************************/
OType OTypeConvertStringToTypeAndIdentifier (
 char* a_string
,OIdentifier* a_oid
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  char*  pos;  
  OType  otype;
  OIdentifier identifier;
  int    status;
/*.........................................................................*/
  if(a_oid!=NULL) *a_oid = NULL;
  if( (a_string==NULL) || (*a_string=='\0')) return NULL;
  string = CStringDuplicate(a_string);
  pos    = strchr(string,'/');
  if(pos==NULL)  /*Could be a type identifier.*/ 
    {
      otype         = OTypeGetIdentifier (string);
      CStringDelete (string);
      return        otype;
    }
  *pos       = '\0';
  otype      = OTypeGetIdentifier (string);
  if(otype==NULL) 
    {
      CStringDelete (string);
      return        NULL;
    }
  identifier = (OIdentifier)CStringConvertToLong(pos+1,&status);
  if(status==0) /*Not a number, then assume object given by a string.*/
    {
      identifier = OTypeGetObjectIdentifier (otype,pos+1);
      if(identifier==NULL)
	{
	  otype = NULL;
	}
      else
        {
	  if(a_oid!=NULL) *a_oid = identifier;
	}      
    }
  else
    {
      if(OTypeIsObjectValid(otype,identifier)==0)
	{
	  otype = NULL;
	}
      else
	{
	  if(a_oid!=NULL) *a_oid = identifier;
	}
    }
  CStringDelete (string);
  return        otype;
}
/***************************************************************************/
OType* OTypeGetTypesFromHandles (
 OHandle* a_hands
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int       count;
  int       handn;
  int       otypen;
  OType*    otypes;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  handn  = CListGetSize((CList)a_hands);
  if( (handn==0) || (a_hands==NULL) ) return NULL;
  otypes = (OType*)CMemoryAllocateBlock(handn * sizeof(OType)); 
  if(otypes==NULL) return NULL;
  otypen = 0;
  for(count=0;count<handn;count++)
    { 
      OType  otype;
      otype  = (OType)OHandleGetType(a_hands[count]);
      if(otype!=NULL)
	{
	  int    otypei;
	  int    found;
	  found  = 0;
	  for(otypei=0;otypei<otypen;otypei++)
	    {
	      if(otype==otypes[otypei]) 
		{
		  found = 1;
		  break;
		}
	    }
	  if(found==0)
	    {
	      otypes[otypen] = otype;
	      otypen++;
	    }
	}
    }
  if(a_number!=NULL) *a_number = otypen;
  return otypes;
}
