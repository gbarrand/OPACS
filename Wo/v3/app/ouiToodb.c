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

#include <CStream.h>
#include <CFile.h>
#include <CMemory.h>
#include <CPrinter.h>
#include <CError.h>
#include <CString.h>
#include <CStream.h>
#include <CList.h>
#include <OType.h>

#include <OProcedure.h>
#include <OProcedureL.h>
#include <OLoop.h>

#include <OPiece.h>
#include <OInterfaceP.h>

#ifdef __cplusplus
extern "C"{
#endif
static OInterface ParseOUi    (char*,char*,FILE*);
static char*      GetWords    (char*,char*,int*,char***);
static void       SaveODB     (OInterface);
static int        IsResource  (char*);
static char*      ParseLine   (char*,char**,char**,char**);
static int        IsChild     (OPiece,OPiece);
static void       ReportError (char*,int);
#ifdef __cplusplus
}
#endif

static char what[] = "@(#)ouiToodb v3";

static struct {
 int    xresn;
 char** xress; 
} Class = {0,NULL};
/***************************************************************************/
int main (
 int a_narg
,char** a_args
)
/***************************************************************************/
{
  FILE* fp;
  char* fname;
  OInterface   oui;
/*.........................................................................*/
  if(a_narg!=2) return EXIT_FAILURE;  

  fp = CFileOpenForReading (a_args[1],NULL);
  if(fp==NULL)            
    {
      CInfoF ("Could not open %s.\n",a_args[1]);
      return EXIT_FAILURE;
    }

  fname                   = CStringDuplicate (a_args[1]);
  CFileTrimDirectory      (fname);
  CFileTrimType           (fname);
  CStringConcatenate      (&fname,".odb");                 

  oui                     = ParseOUi  (a_args[1],fname,fp);
  CStringDelete           (fname);

  SaveODB                 (oui);
  
  fclose                  (fp);

  OInterfaceDelete        (oui);
  OPieceClearClass        ();
  OInterfaceClearClass    ();

  OTypeClearClass         ();
  return                  EXIT_SUCCESS;
}
/***************************************************************************/
static OInterface ParseOUi (
 char* a_name 
,char* a_fname 
,FILE* a_file 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*     lineb;
  char**    lines;
  int       linen;

  OProcedureList     procs;
  char*     word1;
  char*     word2;

  int       wordn;
  int       wordi;
  char**    words;
  char*     wordb;

  char*      string;
  OInterface oui;
  int        found;
/*.........................................................................*/
  if(a_file==NULL)     return NULL;

  lineb               = CStreamGetStrings (a_file,'\\',&linen,&lines);
  if( (lineb==NULL) || (linen==0) || (lines==NULL) ) 
    {
      CWarnF   ("File %s is empty.\n",a_name);
      CMemoryFreeBlock (lines);
      CMemoryFreeBlock (lineb);
      return   NULL;
    }

  oui = OInterfaceCreate (a_name,a_fname);
  if(oui==NULL)            
    {
      ReportError     ("ParseOUi",CErrorFailure);
      CMemoryFreeBlock  (lines);
      CMemoryFreeBlock  (lineb);
      return    NULL;
    }

/*packages*/
  for(count=0;count<linen;count++)
    {
      string = lines[count];      /*012345678901234567890123456789*/
      if( (string!=NULL) && (strncmp(string,"packages:",9)==0) 
        )
        { 
          char* packs = NULL;
	  wordb = GetWords(string+9," ",&wordn,&words);
          for(wordi=0;wordi<wordn;wordi++)
            {
              CStringConcatenate (&packs,words[wordi]);
              CStringConcatenate (&packs," ");
	    }
	  CStringDelete (oui->packs);
          oui->packs    = packs;
	  CMemoryFreeBlock      (words);
	  CMemoryFreeBlock      (wordb);
        }
    }

/*createCallback*/
  for(count=0;count<linen;count++)
    {
      string = lines[count]; 
      if( (string!=NULL) && (strncmp(string,"init_script:",12)==0)
        )
        { 
          CStringDelete       (oui->createCallback);
          oui->createCallback    = CStringDuplicate(string+12);
        }
    }

/*generic*/
  oui->prefix = NULL;
  found        = 0;
  for(count=0;count<linen;count++)
    {
      word1  = CStringGetFirstWord  (lines[count]);
      word2  = CStringGetSecondWord (lines[count]);
      if( (word1!=NULL) && (word2!=NULL) && (strcmp(word1,"generic")==0) )
        {
          oui->prefix   = CStringDuplicate (word2); 
          CStringDelete (word1);
          CStringDelete (word2);
          found = 1;
          break; 
        }
      CStringDelete (word1);
      CStringDelete (word2);
    }
  if(found==0) 
    {
      CWarnF
 ("In file %s, line \"generic\" not found.\n\
Continue with generic \"xxx_\".\n",a_name);
      oui->prefix = CStringDuplicate ("xxx_"); 
    }
  if(CStringIsWord(oui->prefix)==0) 
    {
      CWarnF
 ("In file %s, generic \"%s\" will not produce names \n\
with syntax of a C variable.\n\
Continue with generic \"xxx_\".\n",a_name,oui->prefix);
      CStringDelete (oui->prefix);
      oui->prefix  = CStringDuplicate ("xxx_"); 
    }

/*parent*/
  oui->parent  = NULL;
  for(count=0;count<linen;count++)
    {
      word1  = CStringGetFirstWord  (lines[count]);
      word2  = CStringGetSecondWord (lines[count]);
      if( (word1!=NULL) && (word2!=NULL) && (strcmp(word1,"parent")==0) )
        {
          oui->parent   = CStringDuplicate (word2); 
          CStringDelete (word1);
          CStringDelete (word2);
          break; 
        }
      CStringDelete (word1);
      CStringDelete (word2);
    }

/*store X resource*/
  procs  = OProcedureListCreate (linen,lines,"xresource","endxresource");
  count  = 0;
  LOOP_BEGIN_PCD(procs)
    count += OProcedureGetEndPosition(*itema)-OProcedureGetBeginPosition(*itema)+1;
  LOOP_END_PCD
  Class.xress     = count!=0 ? (char**)CMemoryAllocateBlock(count * sizeof(char*)) : NULL;
  count           = 0;
  if(Class.xress!=NULL)  
    {
      LOOP_BEGIN_PCD(procs)
	LOOP_BEGIN_PCD_LINE(lines,*itema)
	  NOTCOMMENT
	  NOTEMPTY 
	    if(CStringHasOnlySpaces(strloop)==0)
               {
		 Class.xress[count] = CStringDuplicate(strloop);
		 count++;
	       }
          LOOP__END_PCD_LINE 
      LOOP_END_PCD
    }
  Class.xresn  = count;
  if(count==0) 
    {
      CMemoryFreeBlock    (Class.xress);
      Class.xress = NULL;
      Class.xresn = 0;
    }
  OProcedureListDelete (procs);

/*get items*/
  procs = OProcedureListCreate (linen,lines,"widget","endwidget");

/*create items*/
  LOOP_BEGIN_PCD(procs)
      wordb = GetWords(lines[OProcedureGetTitlePosition(*itema)]," ",&wordn,&words);
      if(wordn<3) 
        {
	  CWarnF        ("Bad line syntax in %s:\n%s\n",OInterfaceGetName(oui),lines[OProcedureGetTitlePosition(*itema)]);
	  CMemoryFreeBlock      (words);
          CMemoryFreeBlock      (wordb);
	  OInterfaceDelete      (oui);
	  OProcedureListDelete  (procs);
	  CMemoryFreeBlock      (lines);
	  CMemoryFreeBlock      (lineb);
	  return        NULL;
	}
        { OPiece               widget;
          widget             = OPieceCreate (words[1],words[2],NULL);
          if(widget==NULL)         
            {
	      CMemoryFreeBlock      (words);
	      CMemoryFreeBlock      (wordb);
    	      OInterfaceDelete      (oui);
	      OProcedureListDelete  (procs);
	      CMemoryFreeBlock      (lines);
	      CMemoryFreeBlock      (lineb);
	      return        NULL;
	    }
          OPieceSetFile         (widget,oui);
          OProcedureSetUserData (*itema,(void*)widget);
	  CMemoryFreeBlock      (words);
          CMemoryFreeBlock      (wordb);
	}
  LOOP_END_PCD

/*build hierarchy*/
  LOOP_BEGIN_PCD(procs)
      OPiece            widgetloop;
      widgetloop       = (OPiece) OProcedureGetUserData(*itema);
      if(widgetloop!=NULL) 
	{
	  LOOP_BEGIN_PCD_LINE(lines,*itema)
	    NOTEMPTY 
	    NOTCOMMENT 
	    wordb = GetWords(strloop," ",&wordn,&words);
	    if(wordn>=2)
	      {
		if(strcmp(words[0],"child")==0)
		  { 
		    OPiece   widget;
		    if( (wordn==3) && (strcmp(words[2],oui->name)!=0) )
		      { /*child in an other wtree file*/
			 widget          = OPieceCreate (words[1],NULL,NULL);
			 OPieceSetDataBaseName (widget,words[2]);
		      }
		    widget = OPieceGetIdentifier (words[1]);
		    if(widget==NULL) 
		      { /* not fatal */
			CWarnF("In file %s; widget %s not found\n",OInterfaceGetName(oui),words[1]);
		      }
		    else if(OPieceAddChild(widgetloop,widget)==0) 
                      {
			CMemoryFreeBlock      (words);
			CMemoryFreeBlock      (wordb);
			OInterfaceDelete      (oui);
			OProcedureListDelete  (procs);
			CMemoryFreeBlock      (lines);
			CMemoryFreeBlock      (lineb);
			return        NULL;
		      }
		  }
	      }
	    CMemoryFreeBlock      (words);
            CMemoryFreeBlock      (wordb);
	  LOOP__END_PCD_LINE 
	}
  LOOP_END_PCD

  OProcedureListDelete (procs);
  CMemoryFreeBlock    (lines);
  CMemoryFreeBlock    (lineb);

  return      oui;
}
/***************************************************************************/
static void SaveODB (
  OInterface This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*      tmpname;
  FILE*      file;
  OPiece*      items;
  OPiece*      itema;
/*.........................................................................*/
  if(This==NULL) return;
  if(This->volatil==1) return;

  file            = CFileOpenTemporaryForWriting (NULL,&tmpname);
  if(file==NULL)  {CWarnF ("Can't open temporary file.\n");return;}

  CStreamInitializeStatus ();

  CStreamPrintF(file,"\
!----------------------------\n\
begin OInterface\n\
 version = %s\n\
 prefix = %s\n\
",getenv("WOVERSION")!=NULL ? getenv("WOVERSION") : "v3" ,This->prefix);
  if(This->parent!=NULL)
    {
      CStreamPrintF(file," parent = %s\n",This->parent);
    }

/*packs*/
  if(This->packs!=NULL)
    { 
      CStreamPrintF(file," packages = %s\n",This->packs);
    }

/*createCallback*/
  if(This->createCallback!=NULL)
    { 
      char*         string;   
      string        = CStringDuplicate(This->createCallback);
      CStringReplacePart   (&string,"\n","\\\n");
      CStreamPrintF   (file," createCallback = %s\n",string);
      CStringDelete (string);
    }
  
 {char* string = NULL;
  if(Class.xresn!=0)
    { 
      for(count=0;count<Class.xresn;count++)
	{ 
	  if(IsResource(Class.xress[count])==0)
	    {
	      CWarnF             ("For OInterface %s; strange resource not saved:\n%s|\n",This->name,Class.xress[count]);
	      CStringDelete      (Class.xress[count]);
	      Class.xress[count] = NULL;
	    }
	  else
	    {
	      char* sline;
	      char* swidget;
	      char* sres;
	      char* sval;
	      sline = ParseLine (Class.xress[count],&swidget,&sres,&sval);
	      if( (sline==NULL) ||  (OPieceGetIdentifier(swidget)==NULL) )
		{
		  CStringConcatenate (&string,Class.xress[count]);
		  CStringConcatenate (&string,"\\\n");
		  CStringDelete     (Class.xress[count]);
		  Class.xress[count] = NULL;
		}
	      CStringDelete    (sline);
	    }
	}
    }
  if(string!=NULL) 
    {
      string[strlen(string)-2] = 0;
      CStreamPrintF(file," resources = %s\n",string);
    }
  CStringDelete  (string);}

  CStreamPrintF(file,"end\n");

/*Widgets*/
  items = (OPiece*)OPieceGetIdentifiers ();
  for(itema=items;(itema!=NULL) && (*itema!=NULL);itema++)
    {
      char* name;
      name = OPieceGetName(*itema);
      if( (name!=NULL) && (*name!='\0') && ((OInterface)OPieceGetFile(*itema)==This)  )
        { 
	  char*      type;
          OPiece*    childa;
	  char*      schildren = NULL;
	  type       = OPieceGetType(*itema);

	  CStreamPrintF(file,"\
!----------------------------\n\
begin OWidget\n\
 name = %s\n\
 type = %s\n\
",name,type);

/* Loop over all widget. 
   For example in MOTIF PulldownMenu contained in PulldownMenu the second RowColumn 
   appens to be child of a widget that is a parent of the first RowColumn !!!
*/

/*          childa = items; */
          childa = OPieceGetChildren (*itema);
	  while((childa!=NULL) && (*childa!=NULL) )
	    {
	      int     status;
	      char*   string;   
	      status  = IsChild (*childa,*itema);
	      if(status==1)
		{
		  string              = CStringCreateF (strlen(OPieceGetName(*childa))+1,"%s ",OPieceGetName(*childa));
		  CStringConcatenate  (&schildren,string);
		  CStringDelete       (string);
		}
	      else if(status==2)
		{
		  string              = CStringCreateF (strlen(OPieceGetName(*childa))+strlen(OPieceGetDataBaseName(*childa))+2,
				  	"%s<%s ",OPieceGetName(*childa),OPieceGetDataBaseName(*childa));
		  CStringConcatenate  (&schildren,string);
		  CStringDelete       (string);
		}
	      else if(status==3)
		{
		  CWarnF("Widget %s has a bad oui\n",OPieceGetName(*childa));
		}
              childa++;
	    }
	  if(schildren!=NULL) CStreamPrintF(file," children = %s\n",schildren);
          CStringDelete (schildren);

/*resources*/
	  if(Class.xresn!=0)
	    { 
	      for(count=0;count<Class.xresn;count++)
		{ 
		  if(Class.xress[count]!=NULL)
		    {
		      if(IsResource(Class.xress[count])==0)
			{
			  CWarnF ("For OInterface %s; strange resource not saved:\n%s\n",This->name,Class.xress[count]);
			}
		      else
			{
			  char* sline;
			  char* swidget;
			  char* sres;
			  char* sval;
			  sline            = ParseLine (Class.xress[count],&swidget,&sres,&sval);
			  if( (swidget!=NULL) && (strcmp(name,swidget)==0) )
			    {
			      char*             string;
			      string            = CStringDuplicate (sval);
			      CStringReplacePart       (&string,"\n","\\\n");
			      CStreamPrintF       (file," %s = %s\n",sres,string);
			      CStringDelete     (string);
			      CStringDelete     (Class.xress[count]);
			      Class.xress[count] = NULL;
			    }
			  CStringDelete    (sline);
			}
		    }
		}
	    }

	  CStreamPrintF(file,"end\n");
	}    
    }

  CMemoryFreeBlock   (Class.xress);
  Class.xress = NULL;
  Class.xresn = 0;

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
	       }
	   }
	 CMemoryFreeBlock (buff);
       }
   }
  
  fclose        (file);
  remove        (tmpname);

}
/***************************************************************************/
static char* GetWords (
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
    { char* pos;
      pos = strstr (token,a_limiter);
      if(pos!=NULL)
        {
          *pos = 0;
          if(*token!='\0') {CListAddEntry ((CList*)&lines,token);linen++;}
          token = pos + strlen(a_limiter);          
        }
      else /*last word*/
        {
          if(*token!='\0') {CListAddEntry ((CList*)&lines,token);linen++;}
          break;
        }
    }

  *a_linen = linen;
  *a_lines = lines;
  
  return   string;
}
/***************************************************************************/
static int IsChild (
 OPiece   This
,OPiece   a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OPieceGetParent(This)!=a_widget) return 0;
  if(OPieceGetDataBaseName(This)!=NULL) return 2;
  return 1;
}
/***************************************************************************/
static int IsResource (
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
static char* ParseLine (
 char* a_string   
,char** a_widget   
,char** a_resource 
,char** a_value    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
  char* pos;
/*.........................................................................*/
  *a_widget           = NULL;
  *a_resource         = NULL;
  *a_value            = NULL;
  if(a_string==NULL)       return NULL;
  string              = CStringDuplicate(a_string);
  pos                 = strstr(string,":");
  if(pos==NULL)            {CStringDelete(string);return NULL;}
  *pos                = 0;
  *a_value            = pos + 1;    
  pos                 = strrchr(string,'.');
  if(pos==NULL)            {CStringDelete(string);*a_value = NULL;return NULL;}
  *pos                = 0;
  *a_resource         = pos + 1;    
  pos                 = strrchr(string,'.');
  if(pos!=NULL) 
    {
      *pos                = 0;
      *a_widget           = pos + 1;    
    }
  else
    {
      pos                 = strrchr(string,'*');
      if(pos!=NULL)
        {
          *pos                = 0;
          *a_widget           = pos + 1;    
        }
      else
        {
          *a_widget = string;
        }
    }
  CStringTrim         (*a_widget);
  CStringTrim         (*a_resource);
  CStringTrim         (*a_value);
  return              string; /* to free */
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
  CErrorHandle ("Wo","oui2odb",a_name,a_code);
}
