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
#include <stdio.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CString.h>
#include <CText.h>
#include <CStream.h>
#include <CFile.h>
#include <OType.h>
#include <CoTypes.h>

#include <OModule.h>

#include <CManager.h>

typedef struct _MakeRec*  Make;

typedef struct _MakeRec
{
  char*    package;
  char*    has;
  char*    libs;
  char*    incMake;
  char*    incNMake;
  char*    incMMS;
  int      hasn;
  char**   hass;
  int      haslibn;
  char**   haslibs;
} MakeRec;

#ifdef __cplusplus
extern "C"{
#endif
static void        ProduceFile            (char*,int,char**,int);
static void        SetTypes               ();
static OIdentifier MakeCreate             (OType,int,char**,char**);
static void        MakeUNIX_Makefile      ();
static void        MakeUNIX_File          (char*,char*,char*,char*,char*,int,char**,FILE*);
static void        MakeNT_Makefile        ();
static void        MakeNT_File            (char*,char*,char*,char*,char*,int,FILE*);
static void        MakeVMS_Makefile       ();
static void        MakeVMS_File           (char*,char*,char*,char*,char*,FILE*);
static int         HasLibrary             (char*);
static int         IsTaken                (OModule);
static void        TreatHasProperty       ();
#ifdef __cplusplus
}
#endif

#define NotFound      (-1)
#define TYPE_UNIX     0
#define TYPE_VMS      1
#define TYPE_NT       2

static struct {
  Make  make;
  char* has;
  char* odb;
} Class = {NULL,NULL,NULL};
/******************************************************************************/
void CManagerProduceMakefile (
 char*  a_odb
,int    a_argn
,char** a_args
)
/******************************************************************************/
{
/*.........................................................................*/
  ProduceFile (a_odb,a_argn,a_args,TYPE_UNIX);
}
/******************************************************************************/
void CManagerProduceMMS_File (
 char*  a_odb
,int    a_argn
,char** a_args
)
/******************************************************************************/
{
/*.........................................................................*/
  ProduceFile (a_odb,a_argn,a_args,TYPE_VMS);
}
/******************************************************************************/
void CManagerProduceNMakefile (
 char*  a_odb
,int    a_argn
,char** a_args
)
/******************************************************************************/
{
/*.........................................................................*/
  ProduceFile (a_odb,a_argn,a_args,TYPE_NT);
}
/******************************************************************************/
static void ProduceFile (
 char*  a_odb
,int    a_argn
,char** a_args
,int    a_type
)
/******************************************************************************/
{
  int count;
/*.........................................................................*/
  CStringDelete (Class.odb);
  Class.odb     = CStringDuplicate (a_odb);
  for(count=0;count<a_argn;count++)
    {
      if((strcmp(a_args[count],"-has")==0) && (count!=a_argn-1) )
	{
	  CStringDelete (Class.has);
	  Class.has     = CStringDuplicate (a_args[count+1]);
	  count++;
	}
    }
  SetTypes               ();
  OTypeLoadFile          (a_odb,NULL);
       if(a_type==TYPE_UNIX)
    MakeUNIX_Makefile    ();
  else if(a_type==TYPE_NT)
    MakeNT_Makefile      ();
  else
    MakeVMS_Makefile     ();
  OModuleClearClass      ();
  if(Class.make!=NULL)
    {
      CStringDelete          (Class.make->package);
      CStringDelete          (Class.make->has);
      CStringDelete          (Class.make->libs);
      CStringDelete          (Class.make->incMake);
      CStringDelete          (Class.make->incNMake);
      CStringDelete          (Class.make->incMMS);
      CTextDelete            (Class.make->hasn   ,Class.make->hass);
      CTextDelete            (Class.make->haslibn,Class.make->haslibs);
      Class.make->package    = NULL;
      Class.make->has        = NULL;
      Class.make->libs       = NULL;
      Class.make->incMake    = NULL;
      Class.make->incNMake   = NULL;
      Class.make->incMMS     = NULL;
      Class.make->hasn       = 0;
      Class.make->hass       = NULL;
      Class.make->haslibn    = 0;
      Class.make->haslibs    = NULL;
      CMemoryFreeBlock       (Class.make);
      Class.make             = NULL;
    }
  CStringDelete (Class.has);
  CStringDelete (Class.odb);
  Class.has     = NULL;
  Class.odb     = NULL;
}
/******************************************************************************/
static void SetTypes (
)
/******************************************************************************/
{
  OType otype;
/*.........................................................................*/
  if(OTypeGetIdentifier("Make")!=NULL) return; /*done*/

  CoSetTypes                  ();

  otype                       = OTypeCreate ("Make");
  OTypeSetConstructFunction    (otype,(OTypeConstructFunction)MakeCreate);

  otype                       = OTypeCreate ("OModule");
  OTypeSetMakeFunction         (otype,(OTypeMakeFunction)OModuleMake);
  OTypeSetSetAttributeFunction (otype,(OTypeSetAttributeFunction)OModuleSetAttribute);
  OTypeSetClearClassFunction   (otype,OModuleClearClass);
  OTypeAddNewProperty          (otype,"identifier" ,OPropertyUnsignedLong,NULL);
  OTypeAddNewProperty          (otype,"name"       ,OPropertyString,NULL);
  OTypeAddNewProperty          (otype,"type"       ,OPropertyString,NULL);
  OTypeAddNewProperty          (otype,"language"   ,OPropertyString,NULL);
  OTypeAddNewProperty          (otype,"extension"  ,OPropertyString,NULL);
  OTypeAddNewProperty          (otype,"inLib"      ,OPropertyString,NULL);
  OTypeAddNewProperty          (otype,"directory"  ,OPropertyString,NULL);
  OTypeAddNewProperty          (otype,"beforeLibraries"  ,OPropertyBoolean,NULL);
  OTypeAddNewProperty          (otype,"distributed"      ,OPropertyBoolean,NULL);
  OTypeAddNewProperty          (otype,"isANSI"           ,OPropertyBoolean,NULL);
  OTypeAddNewProperty          (otype,"incs"             ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewProperty          (otype,"objs"             ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewProperty          (otype,"libs"             ,OPropertyArrayOfStrings,NULL);
  OTypeAddNewProperty          (otype,"ifhas"            ,OPropertyArrayOfStrings,NULL);

}
/***************************************************************************/
static OIdentifier MakeCreate (
 OType  This
,int    a_parn
,char** a_pars
,char** a_vals
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count;
  char*  package  = NULL;
  char*  has      = NULL;
  char*  libs     = NULL;
  char*  incMake  = NULL;
  char*  incNMake = NULL;
  char*  incMMS   = NULL;
/*.........................................................................*/
  if(Class.make!=NULL) return Class.make;

  for(count=0;count<a_parn;count++)
    {
      char*  string;
      string = a_pars[count];
           if( (string!=NULL) && (strcmp(string,"package")==0)  ) package  = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"has")==0)      ) has      = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"libs")==0)     ) libs     = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"incMake")==0)  ) incMake  = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"incNMake")==0) ) incNMake = a_vals[count];
      else if( (string!=NULL) && (strcmp(string,"incMMS")==0)   ) incMMS   = a_vals[count];
    }
  if(package==NULL)
    {
      CWarn  ("Property \"package\" not given for object \"Make\".\n");
      return NULL;
    }

  Class.make = (Make) CMemoryAllocateBlock(sizeof(MakeRec));
  if(Class.make==NULL) return NULL;
 
  Class.make->package  = CStringDuplicate(package);
  Class.make->has      = CStringDuplicate(has);
  Class.make->libs     = CStringDuplicate(libs);
  Class.make->incMake  = CStringDuplicate(incMake);
  Class.make->incNMake = CStringDuplicate(incNMake);
  Class.make->incMMS   = CStringDuplicate(incMMS);
  CStringReplacePart   (&(Class.make->libs),"\n"," ");
  Class.make->hasn     = 0;
  Class.make->hass     = NULL;
  Class.make->haslibn  = 0;
  Class.make->haslibs  = NULL;

  CTextAddLine        (&(Class.make->hasn),&(Class.make->hass),package);
 {char*               string; 
  string              = CStringCreateF (3+strlen(package),"lib%s",package);
  CTextAddLine        (&(Class.make->haslibn),&(Class.make->haslibs),string);
  CStringDelete       (string);}

  This       = NULL;
  return     Class.make;
}
/******************************************************************************/
/******************************************************************************/
/******** VMS *****************************************************************/
/******************************************************************************/
/******************************************************************************/
static void MakeVMS_Makefile (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      count;
  FILE*    file    = NULL;
  OModule* liba    = NULL;
  OModule* itema   = NULL;
  char*    package = NULL;
  char*    prefix  = NULL;
  int      lprefix;
/*.........................................................................*/
  if(Class.make==NULL) return;

  package      = Class.make->package;
  prefix       = CStringDuplicate   (package);
  CStringRaise (prefix);
  lprefix      = strlen(prefix);
/*dirbin       = CStringCreateF(lprefix+3,"%sBIN",prefix);*/

/* produce Makefile */
  CFileSafeguard ("This.mms");
  file           = CFileOpenForWriting ("This.mms");
  if(file==NULL) return;

  CStreamPrintF(file,"\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
!  File produced by the Co/omake tool\n\
! using file Make.odb.\n");


  if(Class.make->incMMS!=NULL)
    {
     CStreamPrintF(file,"\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
%s\n",Class.make->incMMS);
    }

  if(Class.has!=NULL)
    {
      CStringDelete   (Class.make->has);
      Class.make->has = CStringDuplicate (Class.has);
      CStreamPrintF(file,"\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
!  The soft linked packages has been taken\n\
! from the command line option '-has'.\n\
! It had the value :\n\
!    %s\n",Class.has);
    }
  else if(Class.make->has!=NULL)
    {
     CStreamPrintF(file,"\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
!  The soft linked packages has been taken\n\
! from the property 'has' of the \n\
! 'Make' object of file %s.\n\
! It had the value :\n\
!    %s\n",Class.odb,Class.make->has);
    }

  TreatHasProperty ();

  CStreamPrintF(file,"\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
HAS_CCFLAGS = ");
  for(count=0;count<Class.make->hasn;count++)
    {
      CStreamPrintF(file,"\\\n$(%s_CCFLAGS)",Class.make->hass[count]);
    }
  CStreamPrintF(file,"\n");
  CStreamPrintF(file,"\
HAS_CXXFLAGS = ");
  for(count=0;count<Class.make->hasn;count++)
    {
      CStreamPrintF(file,"\\\n$(%s_CXXFLAGS)",Class.make->hass[count]);
    }
  CStreamPrintF(file,"\n");
  CStreamPrintF(file,"\
HAS_CPPINCLUDES = ");
  for(count=0;count<Class.make->hasn;count++)
    {
      if(count==0)
	CStreamPrintF(file,"\\\n $(%s_CPPINCLUDES)",Class.make->hass[count]);
      else
	CStreamPrintF(file,"\\\n,$(%s_CPPINCLUDES)",Class.make->hass[count]);
    }
  CStreamPrintF(file,"\n");
  CStreamPrintF(file,"\
HAS_CPPDEFINES = ");
  for(count=0;count<Class.make->hasn;count++)
    {
      if(count==0)
	CStreamPrintF(file,"\\\n $(%s_CPPDEFINES)",Class.make->hass[count]);
      else
	CStreamPrintF(file,"\\\n,$(%s_CPPDEFINES)",Class.make->hass[count]);
    }
  CStreamPrintF(file,"\n");
  CStreamPrintF(file,"\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
all   ::\n\
\t@ if f$search(\"[-]$(CONFIG).DIR\") .eqs. \"\" then cre/dir $(bin)\n\
clean ::\n\
\tif f$search(\"$(bin)*.obj;*\") .nes. \"\" then delete $(bin)*.obj;*\n\
\tif f$search(\"$(bin)*.exe;*\") .nes. \"\" then delete $(bin)*.exe;*\n\
\tif f$search(\"$(bin)*.olb;*\") .nes. \"\" then delete $(bin)*.olb;*\n\
\tif f$search(\"$(bin)*.opt;*\") .nes. \"\" then delete $(bin)*.opt;*\n\
rmlib ::\n\
\tif f$search(\"$(bin)*.olb;*\") .nes. \"\" then delete $(bin)*.olb;*\n\
rmexe ::\n\
\tif f$search(\"$(bin)*.exe;*\") .nes. \"\" then delete $(bin)*.exe;*\n\n");

/*macros liba*/
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  if(OModuleIsSharedLibrary(*itema,"VMS")==1)
	    {
	      CStreamPrintF(file,"%s_target = $(bin)%s.$(olb_or_exe)\n",OModuleGetName(*itema),OModuleGetName(*itema));
	    }
	  else
  	    CStreamPrintF(file,"%s_target = $(bin)%s.olb\n",OModuleGetName(*itema),OModuleGetName(*itema));
	}
    }

/*targets*/
  CStreamPrintF(file,"\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  CStreamPrintF(file,"all :: ");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsApplication(*itema)==1) && 
          (OModuleIsDistributed(*itema)==1) && 
	  (OModuleIsBeforeLibraries(*itema)==1)  )
        CStreamPrintF(file,"\\\n$(bin)%s.exe ",OModuleGetName(*itema));
    }
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  CStreamPrintF(file,"\\\n$(%s_target) ",OModuleGetName(*itema));
	}
    }
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsApplication(*itema)==1) && 
          (OModuleIsDistributed(*itema)==1) && 
	  (OModuleIsBeforeLibraries(*itema)==0)  )
        CStreamPrintF(file,"\\\n$(bin)%s.exe ",OModuleGetName(*itema));
    }
  CStreamPrintF(file,"\n\t@ write sys$output \"%s : all ok.\" \n",package);
  CStreamPrintF(file,"\nlibs :: ");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  CStreamPrintF(file,"\\\n$(%s_target) ",OModuleGetName(*itema));
	}
    }
  CStreamPrintF(file,"\n\t@ write sys$output \"%s : libs ok.\" \n",package);
  CStreamPrintF(file,"\napps :: ");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1) 
        CStreamPrintF(file,"\\\n$(bin)%s.exe ",OModuleGetName(*itema));
    }
  CStreamPrintF(file,"\n\t@ write sys$output \"%s : apps ok.\" \n",package);
  CStreamPrintF(file,"!.............................................\n\
rmexeo :\n");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1) 
	CStreamPrintF(file,"\tif f$search(\"$(bin)%s.obj;*\") .nes. \"\" then del $(bin)%s.obj;*\n",
		      OModuleGetName(*itema),OModuleGetName(*itema));
    }
  CStreamPrintF(file,"!.............................................\n");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1)
	{
	  CStreamPrintF(file,"%s : $(bin)%s.exe\n",OModuleGetName(*itema),OModuleGetName(*itema));
	  CStreamPrintF(file,"\t@ write sys$output \"%s : %s ok.\"\n",package,OModuleGetName(*itema));
	}
    }

/*.exe*/
  CStreamPrintF(file,"\
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1)
	{
	  char*        name;
	  int          objn;
	  char**       objs;
	  int          libn;
	  char**       libs;
	  char*        lang;
	  char*        extension;
	  char*        slk;
	  lang         = OModuleGetLanguage(*itema);
	  extension    = OModuleGetExtension(*itema);
	  CStreamPrintF(file,"\
!.............................................\n");
	  if(CStringCompare(lang,"c++")==1)
	    {
	      slk = CStringDuplicate("$(CXXLD)$(CXXLDFLAGS)");
	    }
	  else if(CStringCompare(lang,"f77")==1)
	    {
	      slk = CStringDuplicate("$(F77LD)$(F77LDFLAGS)");
	    }
	  else
	    {
	      slk = CStringDuplicate("$(CCLD)$(CCLDFLAGS)");
	    }
	  name         = OModuleGetName(*itema);
	  CStreamPrintF  (file,"$(bin)%s.exe : $(bin)%s.obj ",name,name);
	  objs         = OModuleGetOBJ_Files(*itema,&objn);
	  libs         = OModuleGetLibraries(*itema,&libn);
	  for(count=0;count<objn;count++) 
	    {
	      char*         fname;
	      fname         = OModuleGetName(OModuleGetIdentifier(objs[count]));
	      CStreamPrintF (file,"\\\n$(bin)%s.obj ",fname);
	    }
	  for(count=0;count<libn;count++) 
	    {
	      if( (OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==1) && (HasLibrary(libs[count])==1) )
		{
		  /*assume library Xxx of pack Pack exists,
		    and that macro PackXxx exists saying where is the library */
		  CStreamPrintF (file,"\\\n$(%s_target) ",libs[count]);
		}
	    } 
	  CStreamPrintF (file,"\n\t%s/exe=$(bin)%s.exe $(bin)%s.obj",slk,name,name);
	  for(count=0;count<objn;count++) 
	    {
	      char*         fname;
	      fname         = OModuleGetName(OModuleGetIdentifier(objs[count]));
	      CStreamPrintF (file,"\\\n,$(bin)%s.obj",fname);
	    }
	  for(count=0;count<libn;count++) 
	    {
              if(HasLibrary(libs[count])==1)
		{
		  CStreamPrintF (file,"\\\n,$(%s)",libs[count]);
		}
	    } 
	  
	  if(CStringCompare(lang,"c")==1) 
	    CStreamPrintF(file,"\\\n$(CCLDEND)");
	  else if(CStringCompare(lang,"c++")==1) 
	    CStreamPrintF(file,"\\\n$(CXXLDEND)");
	  else
	    CStreamPrintF(file,"\\\n$(F77LDEND)");

	  CStreamPrintF(file,"\n");
	  
	  if(CStringCompare(lang,"c")==1) 
	    {
	      CStreamPrintF(file,"\n$(bin)%s.obj   : $(app)%s.%s\n",name,name,extension);
	      CStreamPrintF(file,"\t$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CCDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)%s.obj $(app)%s.%s \n",name,name,extension);
	    } 
	  else if(CStringCompare(lang,"c++")==1) 
	    {
	      CStreamPrintF(file,"\n$(bin)%s.obj   : $(app)%s.%s\n",name,name,extension);
	      CStreamPrintF(file,"\t$(CXX)$(CXXFLAGS)$(HAS_CXXFLAGS)/include=($(HAS_CPPINCLUDES),$(APP_CPPINCLUDES))/define=($(APP_CXXDEFINES),$(HAS_CPPDEFINES))/obj=$(bin)%s.obj $(app)%s.%s\n",name,name,extension);
	    } 
	  else if(CStringCompare(lang,"f77")==1) 
	    {
	      CStreamPrintF(file,"\n$(bin)%s.obj   : $(app)%s.%s\n",name,name,extension);
	      CStreamPrintF(file,"\t$(F77)$(F77FLAGS)/obj=$(bin)%s.obj $(app)%s.%s\n",name,name,extension);
	    } 
	  CStringDelete   (slk);
	}
    } 

/*liba*/
  for(liba=OModuleGetIdentifiers();(liba!=NULL) && (*liba!=NULL);liba++)
    { 
      char*  name;
      name   = OModuleGetName(*liba);
      if( (OModuleIsLibrary(*liba)==1) && (HasLibrary(name)==1) )
	{
	  int    libn;
	  char** libs;
	  libs   = OModuleGetLibraries(*liba,&libn);
	  if(OModuleIsSharedLibrary(*liba,"VMS")==1)
	    {
	      CStreamPrintF(file,"$(bin)%s.exe : $(bin)%s.olb ",name,name);
	      for(count=0;count<libn;count++)  /* here we set or remove librairie dependency. */
		{
		  if( (OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==1) && (HasLibrary(libs[count])==1) )
		    {
		      CStreamPrintF(file,"\\\n$(%s_target) ",libs[count]);
		    }
		}
	      CStreamPrintF(file,"\n\t$(make_so) %s %s $(CONFIG)\n",name,package);
	      CStreamPrintF(file,"\tif f$search(\"$(bin)%s.opt;*\") .nes. \"\" then delete $(bin)%s.opt;*\n",name,name);
	      CStreamPrintF(file,"\topen/write file $(bin)%s.opt\n",name);
	      CStreamPrintF(file,"\twrite file \"%sROOT:[$(CONFIG)]%s/share\"\n",prefix,name);
	      CStreamPrintF(file,"\tclose file\n");
	    }
	  CStreamPrintF(file,"\n$(bin)%s.olb : ",name);
	  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
	    {
	      if( (OModuleIsInLibrary(*itema,name)==1) && (IsTaken(*itema)==1))
		{
		  char*         fname;
		  fname         = OModuleGetName(*itema);
		  CStreamPrintF (file,"\\\n$(bin)%s.olb(%s) ",name,fname);
		}
	    }
	  for(count=0;count<libn;count++)  /* here we set or remove librairie dependency. */
	    {
	      if( (OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==1) && (HasLibrary(libs[count])==1) )
		{
		  CStreamPrintF(file,"\\\n$(%s_target) ",libs[count]);
		}
	    }
	  CStreamPrintF(file,"\n\tif f$search(\"$(bin)%s.opt;*\") .nes. \"\" then delete $(bin)%s.opt;*\n",name,name);
	  CStreamPrintF(file,"\topen/write file $(bin)%s.opt\n",name);
	  CStreamPrintF(file,"\twrite file \"%sROOT:[$(CONFIG)]%s/lib\"\n",prefix,name);
	  CStreamPrintF(file,"\tclose file\n");
	}
    }

  for(liba=OModuleGetIdentifiers();(liba!=NULL) && (*liba!=NULL);liba++)
    { 
      char*  name;
      name   = OModuleGetName(*liba);
      if( (OModuleIsLibrary(*liba)==1) && (HasLibrary(name)==1) )
	{
	  CStreamPrintF(file,"\
!.............................................\n\
! %s dependencies \n\
!.............................................\n\
",name);
	  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
	    {
	      if( (OModuleIsSourceFile(*itema)==1) && 
		  (OModuleIsInLibrary(*itema,name)==1) &&
		  (IsTaken(*itema)==1)
		  ) 
		{
		  MakeVMS_File (name,
				 OModuleGetName(*itema),
				 OModuleGetLanguage(*itema),
				 OModuleGetExtension(*itema),
				 OModuleGetDirectory(*itema),file);
		} 
	    }
	}
    }

/*files not in libraries*/
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsSourceFile(*itema)==1) && 
	  (OModuleIsInLibrary(*itema,"none")==1)
	 ) 
	{
	  MakeVMS_File  (NULL,
			 OModuleGetName(*itema),
			 OModuleGetLanguage(*itema),
			 OModuleGetExtension(*itema),
			 OModuleGetDirectory(*itema),file);
	} 
    }

  CStreamPrintF(file,"\n");

  CStringDelete (prefix);

  fclose        (file);
  CInfo         ("File This.mms created.\n");
}
/******************************************************************************/
static void MakeVMS_File (
 char*   a_lib
,char*   a_name
,char*   a_lang
,char*   a_extension
,char*   a_dir
,FILE*   a_file
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(strcmp(a_lang,"c")==0) 
    { 
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"$(bin)%s.olb(%s) : %s%s.%s\n",a_lib,a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)%s.obj %s%s.%s\n",a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(inlib) $(bin)%s.olb $(bin)%s.obj\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\tdel $(bin)%s.obj;*\n",a_name);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)%s.obj : $(app)%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)%s.obj $(app)%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"c++")==0) 
    { 
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"$(bin)%s.olb(%s) : %s%s.%s\n",a_lib,a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CXX)$(CXXFLAGS)$(HAS_CXXFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)%s.obj %s%s.%s\n",a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(inlib) $(bin)%s.olb $(bin)%s.obj\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\tdel $(bin)%s.obj;*\n",a_name);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)%s.obj : $(app)%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CXX)$(CXXFLAGS)$(HAS_CXXFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)%s.obj $(app)%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"f77")==0) 
    {
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"$(bin)%s.olb(%s) : %s%s.%s\n",a_lib,a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(F77)$(F77FLAGS)/obj=$(bin)%s.obj %s%s.%s\n",a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(inlib) $(bin)%s.olb $(bin)%s.obj\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\tdel $(bin)%s.obj;*\n",a_name);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)%s.obj : $(app)%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(F77)$(F77FLAGS)/obj=$(bin)%s.obj $(app)%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"pf77")==0) 
    {
      if(a_lib!=NULL) 
	CStreamPrintF (a_file,"$(bin)%s.olb(%s) : $(bin)%s.f\n",a_lib,a_name,a_name);
      else 
	CStreamPrintF (a_file,"$(bin)%s.obj : $(bin)%s.f\n",a_name,a_name);
      CStreamPrintF (a_file,"\t$(F77)$(F77FLAGS)/obj=$(bin)%s.obj $(bin)%s.f\n",a_name,a_name);
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"\t$(inlib) $(bin)%s.olb $(bin)%s.obj\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\tdel $(bin)%s.obj;*\n",a_name);
	}
      CStreamPrintF (a_file,"$(bin)%s.f : %s%s.car\n",a_name,a_dir,a_name);
      CStreamPrintF (a_file,"\t$(mgr)patchyf77.com\n");
    } 
  else if(strcmp(a_lang,"pc")==0) 
    {
      if(a_lib!=NULL) 
	CStreamPrintF (a_file,"$(bin)%s.olb(%s) : $(bin)%s.c\n",a_lib,a_name,a_name);
      else 
	CStreamPrintF (a_file,"$(bin)%s.obj : $(bin)%s.c\n",a_name,a_name);
      CStreamPrintF (a_file,"\t$(CC)$(CFLAGS)$(HAS_CCFLAGS)/include=($(HAS_CPPINCLUDES))/define=($(HAS_CPPDEFINES))/obj=$(bin)%s.obj $(bin)%s.c\n",a_name,a_name);
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"\t$(inlib) $(bin)%s.olb $(bin)%s.obj\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\tdel $(bin)%s.obj;*\n",a_name);
	}
      CStreamPrintF (a_file,"$(bin)%s.c : %s%s.car\n",a_name,a_dir,a_name);
      CStreamPrintF (a_file,"\t$(mgr)patchycc.com\n");
    } 

  CStreamPrintF (a_file,"\n");
}
/******************************************************************************/
/******************************************************************************/
/******** UNIX ****************************************************************/
/******************************************************************************/
/******************************************************************************/
static void MakeUNIX_Makefile (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      count;
  char*    package = NULL;
  FILE*    file    = NULL;
  OModule* liba    = NULL;
  OModule* itema   = NULL;
/*.........................................................................*/
  if(Class.make==NULL) return;

  package         = Class.make->package;

  CFileSafeguard  ("This.mk");
  file            = CFileOpenForWriting ("This.mk");
  if(file==NULL)  return;
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
#  File produced by the Co/omake tool\n\
# using file Make.odb.\n");


  if(Class.make->incMake!=NULL)
    {
     CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
%s\n",Class.make->incMake);
    }

  if(Class.has!=NULL) 
    {
      CStringDelete   (Class.make->has);
      Class.make->has = CStringDuplicate (Class.has);
      CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
#  The soft linked packages has been taken\n\
# from the command line option '-has'.\n\
# It had the value :\n\
#    %s\n",Class.has);
    }
  else if(Class.make->has!=NULL)
    {
     CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
#  The soft linked packages has been taken\n\
# from the property 'has' of the \n\
# 'Make' object of file %s.\n\
# It had the value :\n\
#    %s\n",Class.odb,Class.make->has);
    }

  TreatHasProperty ();

  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
HAS_CPPFLAGS = ");
  for(count=0;count<Class.make->hasn;count++)
    {
      CStreamPrintF(file,"\\\n $(%s_CPPFLAGS)",Class.make->hass[count]);
    }
  CStreamPrintF(file,"\n");
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
all   :: mkdir\n\n\
clean ::\n\
\t/bin/rm -f $(bin)/*.o;/bin/rm -f $(bin)/*.exe;/bin/rm -f $(bin)/*.class;/bin/rm -f $(bin)/*.a;/bin/rm -f $(bin)/*.so;/bin/rm -f $(bin)/*.sl\n\
rmlib ::\n\
\t/bin/rm -f $(bin)/*.a\n\
rmo   ::\n\
\t/bin/rm -f $(bin)/*.o\n\
rmexe ::\n\
\t/bin/rm -f $(bin)/*.exe;/bin/rm -f $(bin)/*.class\n\n\
mkdir :\n\
\t@if test -d $(bin) ; then exit ; else mkdir $(bin) ; echo \"$(bin) created.\" ; fi\n\
\n");

/*macros liba*/
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  if(OModuleIsSharedLibrary(*itema,"UNIX")==1)
	    {
	      CStreamPrintF(file,"%s_target = $(bin)/%s.$(a_or_so)\n",OModuleGetName(*itema),OModuleGetName(*itema));
	    }
	  else
  	    CStreamPrintF(file,"%s_target = $(bin)/%s.a\n",OModuleGetName(*itema),OModuleGetName(*itema));
	}
    }

/*targets*/
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  CStreamPrintF(file,"all :: mkdir ");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsApplication(*itema)==1) && 
          (OModuleIsDistributed(*itema)==1) && 
	  (OModuleIsBeforeLibraries(*itema)==1)  )
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==1) 
	    CStreamPrintF(file,"\\\n$(bin)/%s.class ",OModuleGetName(*itema));
	  else
	    CStreamPrintF(file,"\\\n$(bin)/%s.exe ",OModuleGetName(*itema));
	}
    }
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  CStreamPrintF(file,"\\\n$(%s_target) ",OModuleGetName(*itema));
	}
    }
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsApplication(*itema)==1) && 
          (OModuleIsDistributed(*itema)==1) && 
	  (OModuleIsBeforeLibraries(*itema)==0)  )
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==1) 
	    CStreamPrintF(file,"\\\n$(bin)/%s.class ",OModuleGetName(*itema));
	  else
	    CStreamPrintF(file,"\\\n$(bin)/%s.exe ",OModuleGetName(*itema));
	}
    }
  CStreamPrintF(file,"\n\t@echo \"%s : all ok.\" \n",package);
  CStreamPrintF(file,"\nlibs :: mkdir ");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  CStreamPrintF(file,"\\\n$(%s_target) ",OModuleGetName(*itema));
	}
    }
  CStreamPrintF(file,"\n\t@echo \"%s : libs ok.\" \n",package);
  CStreamPrintF(file,"\napps :: mkdir ");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1)
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==1) 
  	    CStreamPrintF(file,"\\\n$(bin)/%s.class ",OModuleGetName(*itema));
	  else
  	    CStreamPrintF(file,"\\\n$(bin)/%s.exe ",OModuleGetName(*itema));
	}
    }
  CStreamPrintF(file,"\n\t@echo \"%s : apps ok.\" \n",package);
  CStreamPrintF(file,"#--------------------------------------------\n\
rmexeo :\n");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1) 
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==0) 
	    CStreamPrintF(file,"\t/bin/rm -f $(bin)/%s.o\n",OModuleGetName(*itema));
	}
    }
  CStreamPrintF(file,"#--------------------------------------------\n");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  CStreamPrintF(file,"%s : $(%s_target)\n",OModuleGetName(*itema),OModuleGetName(*itema));
	  CStreamPrintF(file,"\t@echo \"%s : %s ok.\"\n",package,OModuleGetName(*itema));
	}
      else if(OModuleIsApplication(*itema)==1)
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==1) 
   	    CStreamPrintF(file,"%s : $(bin)/%s.class\n",OModuleGetName(*itema),OModuleGetName(*itema));
	  else
   	    CStreamPrintF(file,"%s : $(bin)/%s.exe\n",OModuleGetName(*itema),OModuleGetName(*itema));
	  CStreamPrintF(file,"\t@echo \"%s : %s ok.\"\n",package,OModuleGetName(*itema));
	}
    }

/*.exe*/
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1)
	{
	  char*        name;
	  int          objn;
	  char**       objs;
	  int          libn;
	  char**       libs;
	  char*        lang;
	  char*        extension;
	  char*        slk = NULL;
	  lang         = OModuleGetLanguage(*itema);
	  extension    = OModuleGetExtension(*itema);
	  name         = OModuleGetName(*itema);
	  CStreamPrintF(file,"\
#--------------------------------------------\n");
	  if(CStringCompare(lang,"c++")==1)
	    {
	      slk = CStringDuplicate("$(CXXLD) $(CXXLDFLAGS) $(HAS_CPPFLAGS)");
	    }
	  else if(CStringCompare(lang,"f77")==1)
	    {
	      slk = CStringDuplicate("$(F77LD) $(F77LDFLAGS)");
	    }
	  else if(CStringCompare(lang,"java")==1) 
	    {
	      CStreamPrintF (file,"$(bin)/%s.class : $(app)/%s.java\n",name,name);
	      CStreamPrintF (file,"\tjavac $(app)/%s.java\n",name);
	      CStreamPrintF (file,"\t/bin/mv $(app)/%s.class $(bin)/.\n",name);
	      continue;
	    } 
	  else
	    {
	      slk = CStringDuplicate("$(CCLD) $(CCLDFLAGS)");
	    }
	  CStreamPrintF  (file,"$(bin)/%s.exe : $(bin)/%s.o ",name,name);
	  objs         = OModuleGetOBJ_Files(*itema,&objn);
	  libs         = OModuleGetLibraries(*itema,&libn);
	  for(count=0;count<objn;count++) 
	    {
	      char*         fname;
	      fname         = OModuleGetName(OModuleGetIdentifier(objs[count]));
	      CStreamPrintF (file,"\\\n$(bin)/%s.o ",fname);
	    }
	  for(count=0;count<libn;count++) 
	    {
	      if( (OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==1) && (HasLibrary(libs[count])==1) )
		{
		  /*assume library Xxx of pack Pack exists,
		    and that macro PackXxx exists saying where is the library */
		  CStreamPrintF (file,"\\\n$(%s_target) ",libs[count]);
		}
	    } 
	  CStreamPrintF (file,"\n\t%s -o $(bin)/%s.exe $(bin)/%s.o ",slk,name,name);
	  for(count=0;count<objn;count++) 
	    {
	      char*         fname;
	      fname         = OModuleGetName(OModuleGetIdentifier(objs[count]));
	      CStreamPrintF (file,"\\\n$(bin)/%s.o ",fname);
	    }
	  for(count=0;count<libn;count++) 
	    {
              if(HasLibrary(libs[count])==1)
		{
		  CStreamPrintF (file,"\\\n$(%s) ",libs[count]);
		}
	    } 
	  
	  if(CStringCompare(lang,"c")==1) 
	    CStreamPrintF(file,"\\\n$(CCLDEND) ");
	  else if(CStringCompare(lang,"c++")==1) 
	    CStreamPrintF(file,"\\\n$(CXXLDEND) ");
	  else
	    CStreamPrintF(file,"\\\n$(F77LDEND) ");

	  CStreamPrintF(file,"\n");
	  
	  if(CStringCompare(lang,"c")==1) 
	    {
	      CStreamPrintF(file,"\n$(bin)/%s.o   : $(app)/%s.%s\n",name,name,extension);
	      CStreamPrintF(file,"\t$(CC) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/%s.o $(app)/%s.%s \n",name,name,extension);
	    } 
	  else if(CStringCompare(lang,"c++")==1) 
	    {
	      CStreamPrintF(file,"\n$(bin)/%s.o   : $(app)/%s.%s\n",name,name,extension);
	      CStreamPrintF(file,"\t$(CXX) $(CXXFLAGS) $(APP_CXXFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/%s.o $(app)/%s.%s\n",name,name,extension);
	    } 
	  else if(CStringCompare(lang,"f77")==1) 
	    {
	      CStreamPrintF(file,"\n$(bin)/%s.o   : $(app)/%s.%s\n",name,name,extension);
	      CStreamPrintF(file,"\t$(F77) $(F77FLAGS) -c -o $(bin)/%s.o $(app)/%s.%s\n",name,name,extension);
	    } 
	  CStringDelete   (slk);
	}
    } 

/*liba*/
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
# libraries \n\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  CStreamPrintF(file,".PRECIOUS : ");
  for(liba=OModuleGetIdentifiers();(liba!=NULL) && (*liba!=NULL);liba++)
    { 
      char*  name;
      name   = OModuleGetName(*liba);
      if( (OModuleIsLibrary(*liba)==1) && (HasLibrary(name)==1) )
	{
	  CStreamPrintF(file,"$(bin)/%s.a ",name);
	}
    }
  CStreamPrintF(file,"\n\
#--------------------------------------------\n");

  for(liba=OModuleGetIdentifiers();(liba!=NULL) && (*liba!=NULL);liba++)
    { 
      char*  name;
      name   = OModuleGetName(*liba);
      if( (OModuleIsLibrary(*liba)==1) && (HasLibrary(name)==1) )
	{
	  int    libn;
	  char** libs;
	  libs   = OModuleGetLibraries(*liba,&libn);
	  if(OModuleIsSharedLibrary(*liba,"UNIX")==1)
	    {
	      CStreamPrintF(file,"$(bin)/%s.$(so) : $(bin)/%s.a ",name,name);
	      for(count=0;count<libn;count++)  /* here we set or remove librairie dependency. */
		{
		  if( (OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==1) && 
                      (HasLibrary(libs[count])==1) 
		    )
		    {
		      CStreamPrintF(file,"\\\n$(%s_target) ",libs[count]);
		    }
		}
	      CStreamPrintF(file,"\n\t$(make_so) %s $(bin) \"\\\n",name);
	      for(count=0;count<libn;count++) 
		{
		  /*		      if(OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==0)*/
		  {
		    CStreamPrintF (file,"$(%s) \\\n",libs[count]);
		  }
		} 
	      CStreamPrintF(file,"\"\n");
	    }
	  CStreamPrintF(file,"\n$(bin)/%s.a : ",name);
	  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
	    {
	      if( (OModuleIsInLibrary(*itema,name)==1) && (IsTaken(*itema)==1))
		{
		  char*         fname;
		  char*         lang;
		  fname         = OModuleGetName(*itema);
		  lang          = OModuleGetLanguage(*itema);
		  if(strcmp(lang,"java")==0) 
		    CStreamPrintF (file,"\\\n$(bin)/%s.a(%sStubs.o) ",name,fname);
		  else
		    CStreamPrintF (file,"\\\n$(bin)/%s.a(%s.o) ",name,fname);
		}
	    }
	  for(count=0;count<libn;count++)  /* here we set or remove librairie dependency. */
	    {
	      if( (OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==1) && (HasLibrary(libs[count])==1) )
		{
		  CStreamPrintF(file,"\\\n$(%s_target) ",libs[count]);
		}
	    }
	  CStreamPrintF(file,"\n\tif [ -f /bin/ranlib ] ; then /bin/ranlib $(bin)/%s.a ; fi\n\n",name);
	}
    }

  for(liba=OModuleGetIdentifiers();(liba!=NULL) && (*liba!=NULL);liba++)
    { 
      char*  name;
      name   = OModuleGetName(*liba);
      if( (OModuleIsLibrary(*liba)==1) && (HasLibrary(name)==1) )
	{
	  CStreamPrintF(file,"\
#--------------------------------------------\n\
# %s dependencies \n\
#--------------------------------------------\n\
",name);
	  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
	    {
	      if( (OModuleIsSourceFile(*itema)==1) && 
		  (OModuleIsInLibrary(*itema,name)==1) &&
		  (IsTaken(*itema)==1)
		  ) 
		{
		  int           incn;
		  char**        incs;
		  incs          = OModuleGetIncludeFiles(*itema,&incn);
		  MakeUNIX_File (name,
				 OModuleGetName(*itema),
				 OModuleGetLanguage(*itema),
				 OModuleGetExtension(*itema),
				 OModuleGetDirectory(*itema),
				 incn,incs,
				 file);
		} 
	    }
	}
    }

/*files not in libraries*/
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsSourceFile(*itema)==1) && 
	  (OModuleIsInLibrary(*itema,"none")==1)
	 ) 
	{
	  int           incn;
	  char**        incs;
	  incs          = OModuleGetIncludeFiles(*itema,&incn);
	  MakeUNIX_File (NULL,
			 OModuleGetName(*itema),
			 OModuleGetLanguage(*itema),
			 OModuleGetExtension(*itema),
			 OModuleGetDirectory(*itema),
			 incn,incs,
			 file);
	} 
    }

  CStreamPrintF(file,"\n");

  fclose        (file);
  CInfo         ("File This.mk created.\n");
}
/******************************************************************************/
static void MakeUNIX_File (
 char*   a_lib
,char*   a_name
,char*   a_lang
,char*   a_extension
,char*   a_dir
,int     a_incn
,char**  a_incs
,FILE*   a_file
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int count;
/*.........................................................................*/
  if(strcmp(a_lang,"c")==0)
    { 
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"$(bin)/%s.a(%s.o) : %s/%s.%s\n",a_lib,a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/%s.o %s/%s.%s\n",a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\tar cr $(bin)/%s.a $(bin)/%s.o\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%s.o\n",a_name);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)/%s.o : $(app)/%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/%s.o $(app)/%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"c++")==0) 
    { 
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"$(bin)/%s.a(%s.o) : %s/%s.%s\n",a_lib,a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CXX) $(CXXFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/%s.o %s/%s.%s\n",a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\tar cr $(bin)/%s.a $(bin)/%s.o\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%s.o\n",a_name);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)/%s.o : $(app)/%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CXX) $(CXXFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/%s.o $(app)/%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"java")==0) 
    { 
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"$(bin)/%s.a(%sStubs.o) : %s/%s.java\n",a_lib,a_name,a_dir,a_name);
	  CStreamPrintF (a_file,"\tjavac %s/%s.java\n",a_dir,a_name);
	  CStreamPrintF (a_file,"\t/bin/mv %s/%s.class .\n",a_dir,a_name);
	  CStreamPrintF (a_file,"\tjavah -o ../include/%sStubs.h %s\n",a_name,a_name);
	  CStreamPrintF (a_file,"\tjavah -o $(bin)/%sStubs.c -stubs %s\n",a_name,a_name);
	  CStreamPrintF (a_file,"\t$(CC) $(CFLAGS) $(CCCC) $(HAS_CPPFLAGS) -c -o $(bin)/%sStubs.o $(bin)/%sStubs.c\n",a_name,a_name);
	  CStreamPrintF (a_file,"\tar cr $(bin)/%s.a $(bin)/%sStubs.o\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%sStubs.o $(bin)/%sStubs.c\n",a_name,a_name);
	  CStreamPrintF (a_file,"\t/bin/mv %s.class $(bin)/.\n",a_name);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)/%s.class : $(app)/%s.java\n",a_name,a_name);
	  CStreamPrintF (a_file,"\tjavac $(app)/%s.java\n",a_name);
	  CStreamPrintF (a_file,"\t/bin/mv %s.class $(bin)/.\n",a_name);
	}
    } 
  else if(strcmp(a_lang,"f77")==0) 
    {
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"$(bin)/%s.a(%s.o) : %s/%s.%s ",a_lib,a_name,a_dir,a_name,a_extension);
	  if(a_incn>0)
	    {	
	      for(count=0;count<a_incn;count++)
		{
		  CStreamPrintF(a_file,"\\\n$(inc)/%s ",a_incs[count]);
		}
	    }
	  CStreamPrintF (a_file,"\n");
	  CStreamPrintF (a_file,"\t$(F77) $(F77FLAGS) -c -o $(bin)/%s.o %s/%s.%s\n",a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\tar cr $(bin)/%s.a $(bin)/%s.o\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%s.o\n",a_name);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)/%s.o : $(app)/%s.%s ",a_name,a_name,a_extension);
	  if(a_incn>0)
	    {	
	      for(count=0;count<a_incn;count++)
		{
		  CStreamPrintF(a_file,"\\\n$(inc)/%s ",a_incs[count]);
		}
	    }
	  CStreamPrintF (a_file,"\n");
	  CStreamPrintF (a_file,"\t$(F77) $(F77FLAGS) -c -o $(bin)/%s.o $(app)/%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"pf77")==0) 
    {
      if(a_lib!=NULL) 
	CStreamPrintF (a_file,"$(bin)/%s.a(%s.o) : $(bin)/%s.f\n",a_lib,a_name,a_name);
      else 
	CStreamPrintF (a_file,"$(bin)/%s.o : $(bin)/%s.f\n",a_name,a_name);
      CStreamPrintF (a_file,"\t$(F77) $(F77FLAGS) -c -o $(bin)/%s.o $(bin)/%s.f\n",a_name,a_name);
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"\tar cr $(bin)/%s.a $(bin)/%s.o\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%s.o\n",a_name);
	}
      CStreamPrintF (a_file,"$(bin)/%s.f : %s/%s.car\n",a_name,a_dir,a_name);
      CStreamPrintF (a_file,"\t$(mgr)/patchyf77.sh\n");
    } 
  else if(strcmp(a_lang,"pc")==0) 
    {
      if(a_lib!=NULL) 
	CStreamPrintF (a_file,"$(bin)/%s.a(%s.o) : $(bin)/%s.c\n",a_lib,a_name,a_name);
      else 
	CStreamPrintF (a_file,"$(bin)/%s.o : $(bin)/%s.c\n",a_name,a_name);
      CStreamPrintF (a_file,"\t$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/%s.o $(bin)/%s.c\n",a_name,a_name);
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"\tar cr $(bin)/%s.a $(bin)/%s.o\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%s.o\n",a_name);
	}
      CStreamPrintF (a_file,"$(bin)/%s.c : %s/%s.car\n",a_name,a_dir,a_name);
      CStreamPrintF (a_file,"\t$(mgr)/patchycc.sh\n");
    } 
  if(strcmp(a_lang,"rc")==0)
    { 
      if(a_lib!=NULL)
	{
	  CStreamPrintF (a_file,"$(bin)/%s.o : $(app)/%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\techo 'static int dummy = 0;' > $(bin)/%s.c\n",a_name);
	  CStreamPrintF (a_file,"\t$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/%s.o $(bin)/%s.c\n",a_name,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%s.c\n",a_name);
	  CStreamPrintF (a_file,"\tar cr $(bin)/%s.a $(bin)/%s.o\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%s.o\n",a_name);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)/%s.o : $(app)/%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\techo 'static int dummy = 0;' > $(bin)/%s.c\n",a_name);
	  CStreamPrintF (a_file,"\t$(CC) $(CFLAGS) $(HAS_CPPFLAGS) -c -o $(bin)/%s.o $(bin)/%s.c\n",a_name,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%s.c\n",a_name);
	}
    } 

  CStreamPrintF (a_file,"\n");
}
/******************************************************************************/
/******************************************************************************/
/******** NT ******************************************************************/
/******************************************************************************/
/******************************************************************************/
static void MakeNT_Makefile (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      count;
  char*    package = NULL;
  FILE*    file    = NULL;
  OModule* liba    = NULL;
  OModule* itema   = NULL;
/*.........................................................................*/
  if(Class.make==NULL) return;

  package         = Class.make->package;

  CFileSafeguard  ("This.nmk");
  file            = CFileOpenForWriting ("This.nmk");
  if(file==NULL)  return;
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
#  File produced by the Co/omake tool\n\
# using file Make.odb.\n");

  if(Class.make->incNMake!=NULL)
    {
     CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
%s\n",Class.make->incNMake);
    }

  if(Class.has!=NULL) 
    {
      CStringDelete   (Class.make->has);
      Class.make->has = CStringDuplicate (Class.has);
      CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
#  The soft linked packages has been taken\n\
# from the command line option '-has'.\n\
# It had the value :\n\
#    %s\n",Class.has);
    }
  else if(Class.make->has!=NULL)
    {
     CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
#  The soft linked packages has been taken\n\
# from the property 'has' of the \n\
# 'Make' object of file %s.\n\
# It had the value :\n\
#    %s\n",Class.odb,Class.make->has);
    }

  TreatHasProperty ();

  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
HAS_CPPFLAGS = ");
  for(count=0;count<Class.make->hasn;count++)
    {
      CStreamPrintF(file,"\\\n $(%s_CPPFLAGS)",Class.make->hass[count]);
    }
  CStreamPrintF(file,"\n");
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
all   :: mkdir\n\n\
clean ::\n\
\tif exist $(bin)\\*.obj del $(bin)\\*.obj\n\
\tif exist $(bin)\\*.exe del $(bin)\\*.exe\n\
\tif exist $(bin)\\*.class del $(bin)\\*.class\n\
\tif exist $(bin)\\*.lib del $(bin)\\*.lib\n\
\tif exist $(bin)\\*.dll del $(bin)\\*.dll\n\
\tif exist $(bin)\\*.exp del $(bin)\\*.exp\n\
\tif exist $(bin)\\*.pdb del $(bin)\\*.pdb\n\
\tif exist $(bin)\\*.ilk del $(bin)\\*.ilk\n\
rmlib ::\n\
\tif exist $(bin)\\*.lib del $(bin)\\*.lib\n\
rmo   ::\n\
\tif exist $(bin)\\*.obj del $(bin)\\*.obj\n\
rmexe ::\n\
\tif exist $(bin)\\*.exe del $(bin)\\*.exe\n\n\
\tif exist $(bin)\\*.class del $(bin)\\*.class\n\
mkdir :\n\
\t@if not exist $(bin) mkdir $(bin)\n\
\n");

/*macros liba*/
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  if(OModuleIsSharedLibrary(*itema,"NT")==1)
	    {
	      CStreamPrintF(file,"%s_target = $(bin)\\%s.$(lib_or_dll)\n",OModuleGetName(*itema),OModuleGetName(*itema));
	    }
	  else
  	    CStreamPrintF(file,"%s_target = $(bin)\\%s.lib\n",OModuleGetName(*itema),OModuleGetName(*itema));
	}
    }

/*targets*/
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  CStreamPrintF(file,"all :: mkdir ");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsApplication(*itema)==1) && 
          (OModuleIsDistributed(*itema)==1) && 
	  (OModuleIsBeforeLibraries(*itema)==1)  )
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==1) 
	    CStreamPrintF(file,"\\\n$(bin)\\%s.class ",OModuleGetName(*itema));
	  else
	    CStreamPrintF(file,"\\\n$(bin)\\%s.exe ",OModuleGetName(*itema));
	}
    }
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  CStreamPrintF(file,"\\\n$(%s_target) ",OModuleGetName(*itema));
	}
    }
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsApplication(*itema)==1) && 
          (OModuleIsDistributed(*itema)==1) && 
	  (OModuleIsBeforeLibraries(*itema)==0)  )
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==1) 
	    CStreamPrintF(file,"\\\n$(bin)\\%s.class ",OModuleGetName(*itema));
	  else
	    CStreamPrintF(file,"\\\n$(bin)\\%s.exe ",OModuleGetName(*itema));
	}
    }
  CStreamPrintF(file,"\n\t@echo \"%s : all ok.\" \n",package);
  CStreamPrintF(file,"\nlibs :: mkdir ");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsLibrary(*itema)==1) && (HasLibrary(OModuleGetName(*itema))==1) ) 
	{
	  CStreamPrintF(file,"\\\n$(%s_target) ",OModuleGetName(*itema));
	}
    }
  CStreamPrintF(file,"\n\t@echo \"%s : libs ok.\" \n",package);
  CStreamPrintF(file,"\napps :: mkdir ");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1) 
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==1) 
  	    CStreamPrintF(file,"\\\n$(bin)\\%s.class ",OModuleGetName(*itema));
	  else
  	    CStreamPrintF(file,"\\\n$(bin)\\%s.exe ",OModuleGetName(*itema));
	}
    }
  CStreamPrintF(file,"\n\t@echo \"%s : apps ok.\" \n",package);
  CStreamPrintF(file,"#--------------------------------------------\n\
rmexeo :\n");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1) 
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==0) 
	    CStreamPrintF(file,"\tif exist $(bin)\\%s.obj del $(bin)\\%s.obj\n",OModuleGetName(*itema),OModuleGetName(*itema));
	}
    }
  CStreamPrintF(file,"#--------------------------------------------\n");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1)
	{
	  if(CStringCompare(OModuleGetLanguage(*itema),"java")==1) 
   	    CStreamPrintF(file,"%s : $(bin)\\%s.class\n",OModuleGetName(*itema),OModuleGetName(*itema));
	  else
   	    CStreamPrintF(file,"%s : $(bin)\\%s.exe\n",OModuleGetName(*itema),OModuleGetName(*itema));
	  CStreamPrintF(file,"\t@echo \"%s : %s ok.\"\n",package,OModuleGetName(*itema));
	}
    }

/*.exe*/
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if(OModuleIsApplication(*itema)==1)
	{
	  char*        name;
	  int          objn;
	  char**       objs;
	  int          libn;
	  char**       libs;
	  char*        lang;
	  char*        extension;
	  char*        slk;
          int          isANSI;
	  lang         = OModuleGetLanguage(*itema);
	  extension    = OModuleGetExtension(*itema);
          isANSI       = OModuleIsANSI(*itema);
	  name          = OModuleGetName(*itema);
	  CStreamPrintF(file,"\
#--------------------------------------------\n");
	  if(CStringCompare(lang,"c++")==1)
	    {
	      slk = CStringDuplicate("$(CXXLD) $(CXXLDFLAGS)");
	    }
	  else if(CStringCompare(lang,"f77")==1)
	    {
	      slk = CStringDuplicate("$(F77LD) $(F77LDFLAGS)");
	    }
	  else if(CStringCompare(lang,"java")==1) 
	    {
	      CStreamPrintF (file,"$(bin)\\%s.class : $(app)\\%s.java\n",name,name);
	      CStreamPrintF (file,"\tjavac $(app)\\%s.java\n",name);
	      CStreamPrintF (file,"\tcopy $(app)\\%s.class $(bin)\\.\n",name);
	      CStreamPrintF (file,"\tdel $(app)\\%s.class\n",name);
	      continue;
	    } 
	  else
	    {
	      slk = CStringDuplicate("$(CCLD) $(CCLDFLAGS)");
	    }
	  CStreamPrintF (file,"$(bin)\\%s.exe : $(bin)\\%s.obj ",name,name);
	  objs          = OModuleGetOBJ_Files(*itema,&objn);
	  libs          = OModuleGetLibraries(*itema,&libn);
	  for(count=0;count<objn;count++) 
	    {
	      char*         fname;
	      fname         = OModuleGetName(OModuleGetIdentifier(objs[count]));
	      CStreamPrintF (file,"\\\n$(bin)\\%s.obj ",fname);
	    }
	  for(count=0;count<libn;count++) 
	    {
	      if( (OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==1) && (HasLibrary(libs[count])==1) )
		{
		  /*assume library Xxx of pack Pack exists,
		    and that macro PackXxx exists saying where is the library */
		  CStreamPrintF (file,"\\\n$(%s_target) ",libs[count]);
		}
	    } 
	  CStreamPrintF (file,"\n\t%s $(CCLDO)$(bin)\\%s.exe $(bin)\\%s.obj ",slk,name,name);
	  for(count=0;count<objn;count++) 
	    {
	      char*         fname;
	      fname         = OModuleGetName(OModuleGetIdentifier(objs[count]));
	      CStreamPrintF (file,"\\\n$(bin)\\%s.obj ",fname);
	    }
	  for(count=0;count<libn;count++) 
	    {
              if(HasLibrary(libs[count])==1)
		{
		  CStreamPrintF (file,"\\\n$(%s) ",libs[count]);
		}
	    } 
	  
	  if(CStringCompare(lang,"c")==1) 
	    CStreamPrintF(file,"\\\n$(CCLDEND) ");
	  else if(CStringCompare(lang,"c++")==1) 
	    CStreamPrintF(file,"\\\n$(CXXLDEND) ");
	  else
	    CStreamPrintF(file,"\\\n$(F77LDEND) ");

	  CStreamPrintF(file,"\n");
	  
	  if(CStringCompare(lang,"c")==1) 
	    {
	      CStreamPrintF(file,"\n$(bin)\\%s.obj   : $(app)\\%s.%s\n",name,name,extension);
	      if(isANSI==1)
		CStreamPrintF(file,
			      "\t$(CC) $(CCANSI) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) /c /Fo$(bin)\\%s.obj $(app)\\%s.%s\n",
			      name,name,extension);
	      else
		CStreamPrintF(file,
			      "\t$(CC) $(CCNOTANSI) $(CFLAGS) $(APP_CCFLAGS) $(HAS_CPPFLAGS) /c /Fo$(bin)\\%s.obj $(app)\\%s.%s\n",
			      name,name,extension);
	    } 
	  else if(CStringCompare(lang,"c++")==1) 
	    {
	      CStreamPrintF(file,"\n$(bin)\\%s.obj   : $(app)\\%s.%s\n",name,name,extension);
	      CStreamPrintF(file,"\t$(CXX) $(CXXFLAGS) $(APP_CXXFLAGS) $(HAS_CPPFLAGS) /c /Fo$(bin)\\%s.obj /Tp$(app)\\%s.%s\n",name,name,extension);
	    } 
	  else if(CStringCompare(lang,"f77")==1) 
	    {
	      CStreamPrintF(file,"\n$(bin)\\%s.obj   : $(app)\\%s.%s\n",name,name,extension);
	      CStreamPrintF(file,"\t$(F77) $(F77FLAGS) /c Fo$(bin)\\%s.obj $(app)\\%s.%s\n",name,name,extension);
	    } 
	  CStringDelete   (slk);
	}
    } 

/*liba*/
  CStreamPrintF(file,"\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
# libraries \n\
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  for(liba=OModuleGetIdentifiers();(liba!=NULL) && (*liba!=NULL);liba++)
    { 
      char*  name;
      name   = OModuleGetName(*liba);
      if( (OModuleIsLibrary(*liba)==1) && (HasLibrary(name)==1) )
	{
	  int    libn;
	  char** libs;
	  libs   = OModuleGetLibraries(*liba,&libn);
	  CStreamPrintF(file,"#--------------------------------------------\n");
	  if(OModuleIsSharedLibrary(*liba,"NT")==1)
	    {
	      CStreamPrintF(file,"$(bin)\\%s.dll : ",name);
	      for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
		{
		  if( (OModuleIsInLibrary(*itema,name)==1) && 
		      (IsTaken(*itema)==1))
		    {
		      CStreamPrintF (file,"\\\n$(bin)\\%s.obj ",OModuleGetName(*itema));
		    }
		}
	      for(count=0;count<libn;count++) 
		{
		  if( (OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==1) && (HasLibrary(libs[count])==1) )
		    {
		      CStreamPrintF (file,"\\\n$(%s_target) ",libs[count]);
		    }
		} 
	      CStreamPrintF(file,"\n\tif exist $(bin)\\%s.lib del $(bin)\\%s.lib \n",name,name);
	      CStreamPrintF(file,"\tif exist $(bin)\\%s.exp del $(bin)\\%s.exp \n",name,name);
	      CStreamPrintF(file,"\tlib.exe $(LIBFLAGS) /def:$(mgr)\\%s.def /out:$(bin)\\%s.lib\n",name,name);
	      CStreamPrintF(file,"\tif exist $(bin)\\%s.dll del $(bin)\\%s.dll \n",name,name);
	      CStreamPrintF(file,"\t$(LINK32) $(LINK32FLAGS) /out:$(bin)\\%s.dll $(bin)\\%s.exp ",name,name);
	      for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
		{
		  if( (OModuleIsInLibrary(*itema,name)==1) && 
		      (IsTaken(*itema)==1))
		    {
		      CStreamPrintF (file,"\\\n$(bin)\\%s.obj ",OModuleGetName(*itema));
		    }
		}
	      for(count=0;count<libn;count++) 
		{
		    CStreamPrintF (file,"\\\n$(%s) ",libs[count]);
		} 
	      CStreamPrintF(file,"\n");
	    }
	  CStreamPrintF(file,"\n$(bin)\\%s.lib : ",name);
	  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
	    {
	      if( (OModuleIsInLibrary(*itema,name)==1) && 
		  (IsTaken(*itema)==1))
		{
		  CStreamPrintF (file,"\\\n$(bin)\\%s.obj ",OModuleGetName(*itema));
		}
	    }
	  for(count=0;count<libn;count++)  /* here we set or remove librairie dependency. */
	    {
	      if( (OModuleIsLibrary(OModuleGetIdentifier(libs[count]))==1) && (HasLibrary(libs[count])==1) )
		{
		  CStreamPrintF(file,"\\\n$(%s_target) ",libs[count]);
		}
	    }
	  CStreamPrintF(file,"\n\tif exist $(bin)\\%s.lib del $(bin)\\%s.lib \n",name,name);
	  CStreamPrintF(file,"\tlib.exe @<<\n",name);
	  CStreamPrintF(file,"/nologo /out:$(bin)\\%s.lib\n",name);
	  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
	    {
	      if( (OModuleIsInLibrary(*itema,name)==1) && 
		  (IsTaken(*itema)==1))
		{
		  CStreamPrintF (file,"$(bin)\\%s.obj\n",OModuleGetName(*itema));
		}
	    }
	  CStreamPrintF(file,"<<\n");
	}
    }

  for(liba=OModuleGetIdentifiers();(liba!=NULL) && (*liba!=NULL);liba++)
    { 
      char*  name;
      name   = OModuleGetName(*liba);
      if( (OModuleIsLibrary(*liba)==1) && (HasLibrary(name)==1) )
	{
	  CStreamPrintF(file,"\
#--------------------------------------------\n\
# %s dependencies \n\
#--------------------------------------------\n\
",name);
	  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
	    {
	      if( (OModuleIsSourceFile(*itema)==1) && 
		  (OModuleIsInLibrary(*itema,name)==1) &&
		  (IsTaken(*itema)==1)
		  ) 
		{
		  MakeNT_File   (name,
				 OModuleGetName(*itema),
				 OModuleGetLanguage(*itema),
				 OModuleGetExtension(*itema),
				 OModuleGetDirectory(*itema),
				 OModuleIsANSI(*itema),
				 file);
		} 
	    }
	}
    }

/*files not in libraries*/
  for(itema=OModuleGetIdentifiers();(itema!=NULL) && (*itema!=NULL);itema++)
    {
      if( (OModuleIsSourceFile(*itema)==1) && 
	  (OModuleIsInLibrary(*itema,"none")==1)
	 ) 
	{
	  MakeNT_File   (NULL,
			 OModuleGetName(*itema),
			 OModuleGetLanguage(*itema),
			 OModuleGetExtension(*itema),
			 OModuleGetDirectory(*itema),
			 OModuleIsANSI(*itema),
			 file);
	} 
    }

  CStreamPrintF(file,"\n");

  fclose        (file);
  CInfo         ("File This.nmk created.\n");
}
/******************************************************************************/
static void MakeNT_File (
 char*   a_lib
,char*   a_name
,char*   a_lang
,char*   a_extension
,char*   a_dir
,int     a_isANSI
,FILE*   a_file
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(strcmp(a_lang,"c")==0) 
    { 
      if(a_lib!=NULL) 
	{
	  CStreamPrintF (a_file,"$(bin)\\%s.obj : %s\\%s.%s\n",a_name,a_dir,a_name,a_extension);
	  if(a_isANSI==1)
	    CStreamPrintF (a_file,
			   "\t$(CC) $(CCANSI) $(CFLAGS) $(HAS_CPPFLAGS) /c /Fo$(bin)\\%s.obj %s\\%s.%s\n",
			   a_name,a_dir,a_name,a_extension);
	  else
	    CStreamPrintF (a_file,
			   "\t$(CC) $(CCNOTANSI) $(CFLAGS) $(HAS_CPPFLAGS) /c /Fo$(bin)\\%s.obj %s\\%s.%s\n",
			   a_name,a_dir,a_name,a_extension);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)\\%s.obj : $(app)\\%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CC) $(CFLAGS) $(HAS_CPPFLAGS) /c /Fo$(bin)\\%s.obj $(app)\\%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"c++")==0) 
    { 
      if(a_lib!=NULL) 
	{
	  CStreamPrintF (a_file,"$(bin)\\%s.obj : %s\\%s.%s\n",a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CXX) $(CXXFLAGS) $(HAS_CPPFLAGS) /c /Fo$(bin)\\%s.obj /Tp%s\\%s.%s\n",a_name,a_dir,a_name,a_extension);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)\\%s.obj : $(app)\\%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(CXX) $(CXXFLAGS) $(HAS_CPPFLAGS) /c /Fo$(bin)\\%s.obj /Tp$(app)\\%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"rc")==0) 
    { 
      if(a_lib!=NULL) 
	{
	  CStreamPrintF (a_file,"$(bin)\\%s.obj : %s\\%s.%s\n",a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(RC) $(RCFLAGS) $(HAS_CPPFLAGS) /Fo$(bin)\\%s.obj %s\\%s.%s\n",a_name,a_dir,a_name,a_extension);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)\\%s.obj : $(app)\\%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(RC) $(RCFLAGS) $(HAS_CPPFLAGS) /Fo$(bin)\\%s.obj $(app)\\%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"java")==0) 
    { 
      if(a_lib!=NULL)
	{
	  /*
	  CStreamPrintF (a_file,"$(bin)\\%sStubs.obj : %s\\%s.java\n",a_name,a_dir,a_name);
	  CStreamPrintF (a_file,"\tjavac %s\\%s.java\n",a_dir,a_name);
	  CStreamPrintF (a_file,"\tcopy %s\\%s.class .\n",a_dir,a_name);
	  CStreamPrintF (a_file,"\tjavah -o ..\\include\\%sStubs.h %s\n",a_name,a_name);
	  CStreamPrintF (a_file,"\tjavah -o $(bin)\\%sStubs.c -stubs %s\n",a_name,a_name);
	  CStreamPrintF (a_file,"\t$(CC) $(CFLAGS) $(CCCC) $(HAS_CPPFLAGS) -c -o $(bin)/%sStubs.o $(bin)/%sStubs.c\n",a_name,a_name);
	  CStreamPrintF (a_file,"\tar cr $(bin)/%s.a $(bin)/%sStubs.o\n",a_lib,a_name);
	  CStreamPrintF (a_file,"\t/bin/rm -f $(bin)/%sStubs.o $(bin)/%sStubs.c\n",a_name,a_name);
	  CStreamPrintF (a_file,"\t/bin/mv %s.class $(bin)/.\n",a_name);
	  */
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)\\%s.class : $(app)\\%s.java\n",a_name,a_name);
	  CStreamPrintF (a_file,"\tjavac $(app)\\%s.java\n",a_name);
	  CStreamPrintF (a_file,"\tcopy %s.class $(bin)\\.\n",a_name);
	  CStreamPrintF (a_file,"\tdel %s.class\n",a_name);
	}
    } 
  else if(strcmp(a_lang,"f77")==0) 
    {
      if(a_lib!=NULL) 
	{
	  CStreamPrintF (a_file,"$(bin)\\%s.obj : %s\\%s.%s\n",a_name,a_dir,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(F77) $(F77FLAGS) /c /Fo$(bin)\\%s.obj %s\\%s.%s\n",a_name,a_dir,a_name,a_extension);
	}
      else
	{
	  CStreamPrintF (a_file,"$(bin)\\%s.obj : $(app)\\%s.%s\n",a_name,a_name,a_extension);
	  CStreamPrintF (a_file,"\t$(F77) $(F77FLAGS) /c /Fo$(bin)\\%s.obj $(app)\\%s.%s\n",a_name,a_name,a_extension);
	}
    } 
  else if(strcmp(a_lang,"pf77")==0) 
    {
      CStreamPrintF (a_file,"$(bin)\\%s.obj : $(bin)\\%s.f\n",a_name,a_name);
      CStreamPrintF (a_file,"\t$(F77) $(F77FLAGS) /c /Fo$(bin)\\%s.obj $(bin)\\%s.f\n",a_name,a_name);
      CStreamPrintF (a_file,"$(bin)\\%s.f : %s\\%s.car\n",a_name,a_dir,a_name);
      CStreamPrintF (a_file,"\t$(mgr)\\patchyf77.bat\n");
    } 
  else if(strcmp(a_lang,"pc")==0) 
    {
      CStreamPrintF (a_file,"$(bin)\\%s.obj : $(bin)\\%s.c\n",a_name,a_name);
      CStreamPrintF (a_file,"\t$(CC) $(CFLAGS) $(HAS_CPPFLAGS) /c /Fo$(bin)\\%s.obj $(bin)\\%s.c\n",a_name,a_name);
      CStreamPrintF (a_file,"$(bin)\\%s.c : %s\\%s.car\n",a_name,a_dir,a_name);
      CStreamPrintF (a_file,"\t$(mgr)\\patchycc.bat\n");
    } 

  CStreamPrintF (a_file,"\n");
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
static int HasLibrary (
 char* a_name
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*............................................................................*/
  if(Class.make==NULL) return 0;
  if (CTextGetLineIndex(Class.make->haslibn,Class.make->haslibs,a_name)==NotFound) return 0;
  return 1;
}
/******************************************************************************/
static int IsTaken (
 OModule a_module
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    hasn;
  char** hass;
  int count;
/*............................................................................*/
  if(Class.make==NULL) return 0;
  hass = OModuleGetHasFlags(a_module,&hasn);
  for(count=0;count<hasn;count++) {
    if (CTextGetLineIndex(Class.make->hasn,
			  Class.make->hass,
			  hass[count])==NotFound) return 0;
  }
  return 1;
}
/******************************************************************************/
static void TreatHasProperty (
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int      count;
  int      hasn;
  char**   hass;
  char**   libs;
  int      libn;
  char*    libhas = NULL;
  int      isgood = 0;
/*............................................................................*/
  hass             = CStringGetWords (Class.make->has ," ",&hasn);
  CTextConcatenate (&(Class.make->hasn),&(Class.make->hass),hasn,hass);
  CTextDelete      (hasn,hass);
  libs             = CStringGetWords (Class.make->libs," ",&libn);
  for(count=0;count<libn;count++)
    {
      char* pos;
      pos   = strchr(libs[count],':');
      if(pos!=NULL)
	{
	  int           hasi;
	  *pos          = '\0';
	  CStringDelete (libhas);
	  libhas        = CStringDuplicate (libs[count]);
	  isgood        = 0;
	  for(hasi=0;hasi<Class.make->hasn;hasi++)
	    {
	      if(strcmp(libhas,Class.make->hass[hasi])==0)	      
		{
		  isgood = 1;
		  break;
		}
	    } 
	}
      else if( (libhas!=NULL) && (isgood==1) )
	{
	  CTextAddLine (&(Class.make->haslibn),&(Class.make->haslibs),libs[count]);
	}
    } 
  CStringDelete         (libhas);
  CTextDelete           (libn,libs);
  CTextRemoveDuplicates (&(Class.make->haslibn),&(Class.make->haslibs));
 
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
