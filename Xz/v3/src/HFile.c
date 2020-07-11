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
#include <stdarg.h>
#include <stdio.h>

#include <math.h>

#include <CMemory.h>
#include <CString.h>
#include <CFile.h>
#include <CPrinter.h>
#include <CText.h>
#include <CList.h>
#include <CSystem.h>
#include <FString.h>

#include <HF77.h>
#include <HBook.h>

#include <HFile.h>

typedef struct _HFileRecord
{
  char* name;
  int   unit;
  int   type;
  int   offset;
  int   length;
} HFileRecord;

#ifdef __cplusplus
extern "C"{
#endif
static void FreeRecord       (HFile);
static int  GetIdentifier    (HFile,char*);
static int  IsSameAsPrevious (char*);
#ifdef __cplusplus
}
#endif

typedef unsigned long Ulong;
#define NotFound   (-1)

#define HFILE_FILE 1
#define HFILE_GS   2
#define HFILE_PAWC 3

#define HBOOK_FILE 1
#define HBOOK_GS   2
#define HBOOK_PAWC 3

static struct {
  int        init;
  int        unit;
  HFile*     extent;
  char*      prev;
} Class = {0,10,NULL,NULL};
/***************************************************************************/
void HFileClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy    ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent    = NULL;
  HBookClearClass ();
  CStringDelete   (Class.prev);
  Class.prev      = NULL;
}
/***************************************************************************/
HFile* HFileGetIdentifiers (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return Class.extent;
}
/***************************************************************************/
int HFileIsValid (
 HFile This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (CListGetEntryIndex((CList)Class.extent,This,NULL) == NotFound ? 0 : 1);
}
/***************************************************************************/
HFile HFileGetIdentifier (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* pos;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return NULL;
  pos = strchr (a_name,'/');
  if( (pos!=NULL) && (strncmp(a_name,"HFile",5)==0) )
    {
      int        status;
      HFile      identifier;
      identifier = (HFile)CStringConvertToLong(pos+1,&status);
      if(status==0) /*Not a number, then assume object given by a string.*/
	{
	  return (HFile)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,pos+1);
	}
      else
	{
	  if(HFileIsValid(identifier)==0)
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
      return (HFile)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
    }
}
/***************************************************************************/
static int GetIdentifier (
 HFile This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
/*.........................................................................*/
  if(a_name==NULL) return 0;
  name   = strncmp(a_name,"//",2)==0 ? a_name + 2 : a_name;
  return ( (name!=NULL) && (This->name!=NULL) && (strcmp(name,This->name)==0) ) ? 1 : 0;
}
/***************************************************************************/
HFile HFileGetFirstIdentifier (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (Class.extent==NULL) || (*(Class.extent)=='\0') ) return NULL;
  return *(Class.extent);
}
/***************************************************************************/
HFile HFileMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  HFile     This;
/*.........................................................................*/
  This         = (HFile) CMemoryAllocateBlock(sizeof(HFileRecord));  
  if(This==NULL)    return NULL;

  CListAddEntry ((CList*)&(Class.extent),This);

  This->name   = NULL;
  This->unit   = 0;
  This->type   = 0;
  This->offset = 0;
  This->length = 0;

  return       This;
}
/***************************************************************************/
HFile HFileOpenZEBRA_FileForReading (
 char* a_dname 
,char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  HFile This,oThis;
  FString  fdname;
  int   ldname;
/*.........................................................................*/
  HBookInitialize (0);
  if(Class.init==0)
    {
      HFile pawc;
      pawc  = HFileMake ();
      if(pawc==NULL) return NULL;
      pawc->name    = CStringDuplicate ("PAWC");
      pawc->type    = HBOOK_PAWC;
      CStringDelete (Class.prev);
      Class.prev    = CStringDuplicate("//PAWC");
      Class.init    = 1;
    }

  This           = HFileMake();
  if(This==NULL) return NULL;

  if(a_fname!=NULL) 
    {
      FILE*    file;
      char*    fname;
      int      status;
      FString  fstr;
      int      lstr;
      char*    dname;
      int      mode = 0;
      
      file     = CFileOpenForReading(a_fname,&fname);
      if(file==NULL) 
	{
	  HFileDelete(This);
	  return NULL;
	}
      fclose    (file);

      CFileLink (&fname);

      fstr      = FStringCreateFromC (fname,&lstr);

      if(a_dname==NULL) 
	{ 
	  dname              = CStringDuplicate(a_fname);
	  CFileTrimDirectory (dname);
	  CFileTrimType      (dname);
	}
      else
	{
	  dname              = CStringDuplicate(a_dname);
	}

      oThis         = HFileGetIdentifier (dname);
      if(oThis!=NULL)     
	{
	  HFileDelete(This);
	  return oThis;
	}

      fdname                 = FStringCreateFromC (dname,&ldname);
      status                 = ohopen    (&(Class.unit),fstr,&lstr,fdname,&ldname,&mode);
      if(status!=0)          
	{
	  This->name   = CStringDuplicate (dname);
	  This->type   = HBOOK_FILE;
	  This->unit   = Class.unit;
	  Class.unit++;
	}
      CStringDelete     (dname);

      CMemoryFreeBlock  (fname);
      FStringDelete     (fstr);
      FStringDelete     (fdname);

    } 
  else if(a_dname!=NULL) /*else a_fname*/
    {  
      int length = 0;
      int offset = 0;

      oThis         = HFileGetIdentifier (a_dname);
      if(oThis!=NULL)     
	{
	  HFileDelete(This);
	  return oThis;
	}

      fdname      = FStringCreateFromC (a_dname,&ldname);
      if(CSystemIsKindOf("VMS",NULL)==1) 
	offset = ohgmap(fdname,&ldname,&length);
      else
	offset = ohmmap(fdname,&ldname);
      if(offset==0)
	{
	  FStringDelete    (fdname);
	  HFileDelete   (This);
	  return        NULL;
	}
      This->name   = CStringDuplicate (a_dname);
      This->unit   = 0;
      This->type   = HBOOK_GS;
      This->offset = offset;
      This->length = length;
      if(CSystemIsKindOf("VMS",NULL)==1) 
	ohgr (fdname,&ldname,&This->offset);
      else
	ohmr (fdname,&ldname,&This->offset);
      FStringDelete    (fdname);

    }  /*endif a_fname*/

  HFileBringDirectoryInMemory (This);

  return      This;
}
/***************************************************************************/
void HFileDelete (
 HFile This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)     return;
  if( (This->name!=NULL) && (strcmp(This->name,"PAWC")==0) ) return; /*Do not want to delete HFile associated to PAWC.*/
  CListRemoveEntry  ((CList)Class.extent,This);
  FreeRecord       (This);
}
/***************************************************************************/
static void FreeRecord (
 HFile This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;

  HBookInitialize (0);
  if(This->name!=NULL)
    {
      FString   fdname;
      int       ldname;
      fdname    = FStringCreateFromC (This->name,&ldname);
      if(This->type==HBOOK_FILE) 
        {
          ohrend (&(This->unit),fdname,&ldname); 
        }
      else if(This->type==HBOOK_GS)
        {
	  if(CSystemIsKindOf("VMS",NULL)==1) 
	    ohgunm (&(This->offset),&(This->length));
	  else
	    ohmunm (&(This->offset));
          ohrend        (&(This->unit),fdname,&ldname); 
        }
      FStringDelete    (fdname);
    }

  CStringDelete (This->name);
  This->name   = NULL;

  CMemoryFreeBlock      (This);
}
/***************************************************************************/
char* HFileGetName (
 HFile This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->name;
}
/***************************************************************************/
int HFileGetType (
 HFile This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->type;
}
/***************************************************************************/
void HFileUpdate (
 HFile This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString       fdname;
  int        ldname;
/*.........................................................................*/
  if(This==NULL) return;
  if(This->type!=HBOOK_GS) return;
  HBookInitialize  (0);
  fdname     = FStringCreateFromC (This->name,&ldname);
  ohrend     (&(This->unit),fdname,&ldname); 
  if(CSystemIsKindOf("VMS",NULL)==1) 
    ohgr (fdname,&ldname,&This->offset);
  else
    ohmr (fdname,&ldname,&This->offset);
  FStringDelete  (fdname);
  HFileBringDirectoryInMemory (This);
}
/***************************************************************************/
int HFileSetDirectory (
 HFile This
,char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  if(strncmp(a_string,"//",2)!=0) 
    {
      CWarnF("%s: must start by //.\n",a_string);
      return 0;
    }
  if(strncmp(This->name,a_string+2,strlen(This->name))!=0)
    {
      CWarnF("Directory\n%s\ndo not belong to %s\n",a_string,This->name);
      return 0;
    }
  return HBookSetDirectory (a_string);
}
/***************************************************************************/
void HFileBringDirectoryInMemory (
 HFile This
)
/***************************************************************************/
/* Try to avoid too much hrin.                                             */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*   pwd;
  int     same;
/*.........................................................................*/
  if(This==NULL) return;
  pwd       = HBookGetDirectoryName();
  if(pwd==NULL)  return;
  if(strncmp(This->name,pwd+2,strlen(This->name))!=0)     /*not current directory*/
    { char*             string;
      string            = CStringCreateF(strlen(This->name)+2,"//%s",This->name);
      HBookSetDirectory (string);
      CStringDelete     (string);
    }
  same          = IsSameAsPrevious(pwd);
  CStringDelete (pwd);
       if(This->type==HBOOK_FILE) 
    {
      if(same==0) HBookBringDirectoryInMemory ();
    }
  else if(This->type==HBOOK_GS) 
    {
      HBookBringDirectoryInMemory ();
      HBookSetDirectory           ("//PAWC");
    }
  else if(This->type==HBOOK_PAWC) 
    {
    }
}
/***************************************************************************/
int HFileGetAttribute (
 HFile  This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* types[3] = {"file","global_section","pawc"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL) return 0;
  if(a_name==0)  return 0;
  if(a_addr==0)  return 0; 

       if(strcmp(a_name,"identifier")==0) *((Ulong*)a_addr)    = (Ulong)This;
  else if(strcmp(a_name,"name")==0)       *((char**)a_addr)    = This->name;
  else if(strcmp(a_name,"unit")==0)       *((int*)a_addr)      = This->unit;
  else if(strcmp(a_name,"type")==0) 
    {
           if(This->type==HFILE_FILE) *((char**)a_addr) = types[0];
      else if(This->type==HFILE_GS)   *((char**)a_addr) = types[1];
      else if(This->type==HFILE_PAWC) *((char**)a_addr) = types[2];
      else 
	{
          CInfoF("HFileGetAttribute: bad type for %s.\n",This->name);
          *((char**)a_addr) = NULL;
	}
    }
  else 
    {
      CInfoF("HFileGetAttribute: unknown property %s.\n",a_name);
      return 0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
int HFileDoMethod (
 HFile  This
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

  if(strcmp(a_name,"bringDirectoryInMemory")==0) 
    HFileBringDirectoryInMemory(This);
  else 
    {
      CInfoF("HBookDoMethod: %s is not a class routine property.\n",a_name);
      return 0;
    }
  a_user = NULL;
  a_argn = 0;
  a_args = NULL;
  a_addr = NULL;
  return 1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int IsSameAsPrevious (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if( (Class.prev!=NULL) && (strcmp(a_name,Class.prev)==0) ) return 1;
  CStringDelete (Class.prev);
  Class.prev   = CStringDuplicate(a_name);
  return        0;
}
