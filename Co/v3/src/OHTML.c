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
#include <CError.h>
#include <CList.h>

#include <OProcedure.h>
#include <OProcedureL.h>
#include <OLoop.h>

#include <OHTMLP.h>

#ifdef __cplusplus
extern "C"{
#endif
static void  InitializeClass       ();
static OHTML OHTML_Create           (char*);
static void  FreeRecord            (OHTML);
static int   GetIdentifier         (OHTML,char*);
static int   GetProcedureIdentifier (OProcedure,char*);
static OHTML OHTML_GetIdentifier     (char*);
static void  ProduceLATEX          (char*);
static void  ReportError           (char*,int);
#ifdef __cplusplus
}
#endif

#define NotFound   (-1)

static struct 
{
  OHTML* extent;
  int    linen;
  char** lines;
  OHTML  data;
  int    level;
} Class = {NULL,0,NULL,NULL,0};
/***************************************************************************/
void OHTML_ClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDestroy    ((CList)Class.extent,(CListVisitEntryFunction)FreeRecord);
  Class.extent   = NULL;

  CTextDelete      (Class.linen,Class.lines);
  Class.linen    = 0;
  Class.lines     = NULL;
}
/***************************************************************************/
static OHTML OHTML_GetIdentifier (
 char* a_name    
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OHTML)CListFindEntry ((CList)Class.extent,(CListFindEntryFunction)GetIdentifier,a_name);
}
/***************************************************************************/
static int GetIdentifier (
 OHTML This
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return CStringCompare(This->name,a_name);
}
/***************************************************************************/
static OHTML OHTML_Create (
 char* a_name 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OProcedureList pcds;
  char*   lineb;
  int     linen;
  char**  lines;
  OHTML   This = NULL;
/*.........................................................................*/
  if( (a_name==NULL) || 
      (*a_name=='\0')
     ) 
    {
      ReportError("OHTML_Create",CErrorValue);
      return NULL;
    }
  
  lineb          = CFileLoadStrings (a_name,'\0',&linen,&lines);
  if( (lineb==NULL) || 
      (linen==0)    || 
      (lines==NULL) 
     ) 
    {
      CMemoryFreeBlock(lines);
      CMemoryFreeBlock(lineb);
      return NULL;
    } 
  pcds     = OProcedureListCreate (linen,lines,"<A","<HR>");
  if(pcds==NULL)
    {
      CWarnF           ("No topic found in %s.\n",a_name);
      CMemoryFreeBlock (lines);
      CMemoryFreeBlock (lineb);
      return           NULL;
    }

  This           = (OHTML) CMemoryAllocateBlock (sizeof(OHTML_Record));
  if(This==NULL) 
    {
      OProcedureListDelete (pcds);
      CMemoryFreeBlock     (lines);
      CMemoryFreeBlock     (lineb);
      return               NULL;
    } 

  CListAddEntry   ((CList*)&(Class.extent),This);

  This->name     = CStringDuplicate(a_name);
  This->lineb    = lineb;
  This->lines    = lines;
  This->linen    = linen;
  This->pcds     = pcds;

  return         This;
}
/***************************************************************************/
char* OHTML_ParseLocator (
 char*  a_string     
,char** a_topic
)
/***************************************************************************/
/*
  topic locator follow URL convention:
     <file>#<topic>
     <file>
     #<topic>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* fileName;
  char* topic;
/*.........................................................................*/
  if(a_topic!=NULL)  *a_topic = NULL;
  if(a_string==NULL) return NULL;

  fileName = CStringDuplicate (a_string);
  topic    = strchr           (fileName,'#');
  if(topic==NULL) /*Only a file name.*/
    {
      CStringTrim (fileName);
      return      fileName;
    }
  *topic = '\0';
  topic  = CStringDuplicate(topic + 1);
  CStringTrim (fileName);
  CStringTrim (topic);
  if(*topic=='\0') 
    {
      CStringDelete (topic);
      topic         = NULL;
    }
  if(*fileName=='\0') 
    {
      CStringDelete (fileName);
      fileName      = NULL;
    }
  if(a_topic!=NULL) *a_topic = topic;
  return fileName;
}
/***************************************************************************/
char* OHTML_GetTopic (
 char*   a_locator     
,int     a_ascii     
,char*** a_topics    
,int*    a_topicn
,char*** a_subs 
,int*    a_subn
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char sret[128] = "";
  char*      topic;
  char*      fileName;
  OHTML      ohtml;
  char*      string;
  OProcedure pcd;
  int        begin;
  char*      pcdname;
/*.........................................................................*/
  if(a_topics!=NULL)  *a_topics = NULL;
  if(a_topicn!=NULL)  *a_topicn = 0;
  if(a_subs!=NULL)    *a_subs   = NULL;
  if(a_subn!=NULL)    *a_subn   = 0;
  if(a_locator==NULL) return NULL;

  fileName = OHTML_ParseLocator (a_locator,&topic);

  if( (topic!=NULL) && (fileName==NULL) ) /*Search in current ohtml.*/
    {
      ohtml   = Class.data;
      if(ohtml==NULL)
        {
          ReportError     ("OHTML_GetTopic",CErrorFormat);
          CStringDelete   (topic);
          CStringDelete   (fileName);
          return          NULL;
        }
      string = CStringCreateF 
        (strlen(((OHTML)ohtml)->name)+strlen(topic)+1,
	 "%s#%s",((OHTML)ohtml)->name,topic);
    }
  else if(fileName!=NULL)
    {  
       ohtml           = OHTML_GetIdentifier (fileName); /*Look if ohtml alredy loaded.*/
       if(ohtml==NULL) ohtml = OHTML_Create (fileName);  /*If not load it.*/
       Class.data    = ohtml;
       if(ohtml==NULL)       
         {
           CStringDelete   (topic);
           CStringDelete   (fileName);
           return          NULL;
         }
       if(topic==NULL) /*Get first topic.*/ 
	 {
	   char*          pos;
	   CStringDelete  (topic);
	   /*Assume name of procedure is:NAME="name"...*/
	   topic          = CStringDuplicate(OProcedureGetName(ohtml->pcds[0])+6);
	   pos            = strchr(topic,'"');
           if(pos!=NULL)  *pos = '\0';
	 }
       string = CStringDuplicate(a_locator);
    }
  else
    {
      CStringDelete   (topic);
      CStringDelete   (fileName);
      return          NULL;
    }

  CTextAddLine   (&Class.linen,&Class.lines,string);
  CStringDelete  (string);

  if(ohtml==NULL) 
    {
      CStringDelete   (topic);
      CStringDelete   (fileName);
      return          NULL;
    }

  pcdname       = CStringCreateF (strlen(topic)+7,"NAME=\"%s\"",topic);
  pcd           = (OProcedure) CListFindEntry ((CList)ohtml->pcds,(CListFindEntryFunction)GetProcedureIdentifier,pcdname);
  CStringDelete (pcdname);
  if(pcd==NULL) 
    {
      CWarnF          ("Topic %s not found.\n",topic);
      CStringDelete   (topic);
      CStringDelete   (fileName);
      return          NULL;
    }

  begin = 0;
  LOOP_BEGIN_PCD_LINE(ohtml->lines,pcd)
    if( (strloop!=NULL) && (strncmp(strloop,"<!",2)==0) ) continue;
    if(begin==0) 
      {
                                /*1234567890123456789012*/
             if( (strncmp(strloop,"<P>Other documentation",22)==0)
	       )
          {
	    begin = 1;
	  }
        else if( (a_ascii!=0) && (strncmp(strloop,"<UL>",4)==0) ) 
          {}
        else if( (a_ascii!=0) && (strncmp(strloop,"</UL>",5)==0) ) 
          {}
        else if( (a_ascii!=0) && (strncmp(strloop,"<PRE>",5)==0) ) 
          {}
        else if( (a_ascii!=0) && (strncmp(strloop,"</PRE>",6)==0) ) 
          {}
        else if( (a_ascii!=0) && (strncmp(strloop,"<P>",3)==0) ) 
          {
            CTextAddLine (a_topicn,a_topics,"");
          }
        else if( (a_ascii!=0) && (strncmp(strloop,"<LI>",4)==0) ) 
          {
            string             = CStringDuplicate(" -");
            CStringConcatenate (&string,strloop+4);
            CTextAddLine       (a_topicn,a_topics,string);
            CStringDelete      (string);
          }
        else 
          {
            CTextAddLine (a_topicn,a_topics,strloop);
          }
      }
    else
      {
        if(*strloop!='\0') /*extract subtopic name from: <LI><A HREF="#subtopic">sub topic</A> */
          { char*         pos;
            pos           = strstr(strloop,"#");
            if(pos!=NULL)
              {
		string          = CStringDuplicate(pos+1);
		pos             = strstr(string,"\">");
                if(pos!=NULL)
                  {
		    *pos     = '\0';
		    CTextAddLine (a_subn,a_subs,string);
		  }
		CStringDelete   (string);
	      }
          }
      }
  LOOP__END_PCD_LINE 

  if( (topic!=NULL) && (strlen(topic)>=(size_t)128)) 
    {
      CStringDelete (topic);
      CStringDelete (fileName);
      return        NULL;
    }

  CTextReplaceWord (*a_topicn,*a_topics,"&lt;","<");
  CTextReplaceWord (*a_topicn,*a_topics,"&gt;",">");
  CTextReplaceWord (*a_subn  ,*a_subs  ,"&lt;","<");
  CTextReplaceWord (*a_subn  ,*a_subs  ,"&gt;",">");

  strcpy        (sret,topic);

  CStringDelete (topic);
  CStringDelete (fileName);

  return        sret;
}
/***************************************************************************/
char* OHTML_GoBackward (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(Class.lines==NULL) return NULL;
  if(Class.linen<=1)    return NULL;
  string        = CStringDuplicate(Class.lines[Class.linen-2]);
  CStringDelete (Class.lines[Class.linen-2]);
  CStringDelete (Class.lines[Class.linen-1]);
  Class.linen -= 2;
  return        string;
}
/***************************************************************************/
/******** Terminal interface to ohtml **************************************/
/***************************************************************************/
/***************************************************************************/
void OHTML_DumpTopic (
 char* a_locator  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char**        lines  = NULL;
  int           linen = 0;
  char**        sub   = NULL;
  int           nsub  = 0;
/*.........................................................................*/
  OHTML_GetTopic (a_locator,1,&lines,&linen,&sub,&nsub);

  if( (linen!=0) && (lines!=NULL) )
    {
      CDump ("\n");
      CDump ("\n");
      for(count=0;count<linen;count++) CDumpF ("%s\n",lines[count]);
    }

  CTextDelete (linen,lines);
  CTextDelete (nsub,sub);
}
/***************************************************************************/
/******** Latex ************************************************************/
/***************************************************************************/
/***************************************************************************/
void OHTML_ProduceLATEX_File (
 char* a_locator 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_locator==NULL) return;
  Class.level = 0;
  ProduceLATEX  (a_locator);
}
/***************************************************************************/
static void ProduceLATEX (
 char* a_locator  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** lines  = NULL;
  int    linen = 0;
  char** sub   = NULL;
  int    nsub  = 0;
  char*  string;
  int    flist,fexa;
  static FILE*  file = NULL;
/*.........................................................................*/
  if(a_locator==NULL) return;
  if(strstr(a_locator,"nopaper")!=NULL) return; 

  string            = CStringDuplicate(OHTML_GetTopic (a_locator,0,&lines,&linen,&sub,&nsub));
  if(string==NULL)  return;
  if(lines==NULL)    {CStringDelete(string);return;}
  
       if(Class.level==0)
    { char*           fileName;
      CInfoF          ("build %s.tex...\n",string);
      fileName        = CStringCreateF (strlen(string)+4,"%s.tex",string);
      CFileSafeguard  (fileName);
      file            = CFileOpenForWriting (fileName);
      CStringDelete   (fileName);
      if(file==NULL)  return;
      CInfoF          ("\
You can build %s.ps with:\n\
 SYS> latex %s.tex\n\
that produce an %s.dvi.\n\
Do it twice to have table of content.\n\
 SYS> dvips %s.dvi\n\
that produce an %s.ps.\n\
",string,string,string,string,string);
    }

  CStringReplacePart(&string,"_"," ");
       if(Class.level==0)
    { char* ss;
      CStreamPrintF(file,"\\documentstyle{report}\n\\title{%s",string);
      for(count=0;count<linen;count++)
        {
          if( (CStringHasOnlySpaces(lines[count])==0) && 
              (strncmp(lines[count],"<P>",3)!=0)
	     )
            {
              CStreamPrintF(file,"\\\\\n%s",lines[count]); 
            }
        }
      CStreamPrintF(file,"}\n\\author{}\n");
      ss     = CTimeGetDate();
      ss[10] = '\0';
      CStreamPrintF(file,"\\date{%s %s}\n\n\n\\begin{document}\n\\maketitle\n",ss,ss+20);
    }
  else if(Class.level==1)
    {      
       CStreamPrintF(file,"\\chapter{%s}\n",string);
    }
  else if(Class.level>1)
    {
      CStreamPrintF(file,"\\section{%s}\n",string);
    }
  CStringDelete    (string);

/* print topic lines */
  if(Class.level>=1)
    {
      flist    = 0;
      fexa     = 0;
      for(count=0;count<linen;count++)
        { 
          string = CStringDuplicate (lines[count]);
               if( (string!=NULL) && (strncmp(string,"<P>",3)==0) )
            {
            }
          else if( (string!=NULL) && (strncmp(string,"<UL>",4)==0) ) /* list */
            {
              CStreamPrintF(file,"\\begin{itemize}\n");
              flist = 1;
            }
          else if( (string!=NULL) && (strncmp(string,"</UL>",5)==0) )
            {
              CStreamPrintF(file,"\\end{itemize}\n");
              flist    = 0;
            }
          else if( (string!=NULL) && (strncmp(string,"<PRE>",5)==0) ) /* example */
            {
              CStreamPrintF(file,"\\begin{verbatim}\n");
              fexa = 1;
            }
          else if( (string!=NULL) && (strncmp(string,"</PRE>",6)==0) )
            {
              CStreamPrintF(file,"\\end{verbatim}\n");
              fexa     = 0;
            }
          else
            {
	      if( (flist==1)     && 
		  (string!=NULL) && 
		  (strncmp(string,"<LI>",4)==0)
		 )
		{
                  CStringDelete   (string);
		  string          = CStringDuplicate(lines[count]+4);
		  CStreamPrintF (file,"\\item ");
		}
	      if(fexa==0)
		{
		  CStringReplacePart(&string,"\\","\\\\");
		  CStringReplacePart(&string,"#","\\#");
		  CStringReplacePart(&string,"$","\\$");
		  CStringReplacePart(&string,"%","\\%");
		  CStringReplacePart(&string,"&","\\&");
		  CStringReplacePart(&string,"~","\\~");
		  CStringReplacePart(&string,"_","\\_");
		  CStringReplacePart(&string,"^","\\^");
		  CStringReplacePart(&string,"{","\\{");
		  CStringReplacePart(&string,"}","\\}");
		  CStringReplacePart(&string,">","\\verb|>|");
		  CStringReplacePart(&string,"<","\\verb|<|");
		}
	      CStreamPrintF(file,"%s\n",string);
            }
          CStringDelete    (string);
        }
    }

  CTextDelete (linen,lines);

/* print subtopic list */
  Class.level ++;
  for(count=0;count<nsub;count++)
    {
      ProduceLATEX (sub[count]);
    }
  Class.level --;

  CTextDelete (nsub,sub);

       if(Class.level==0)
    {
      CStreamPrintF(file,"\\tableofcontents\n\\end{document}\n");
      fclose         (file);
    }

}
/***************************************************************************/
static void FreeRecord (
 OHTML This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringDelete         (This->name);
  OProcedureListDelete  (This->pcds);
  CMemoryFreeBlock      (This->lines);
  CMemoryFreeBlock      (This->lineb);
  This->name            = NULL;
  This->pcds            = NULL;
  This->lineb           = NULL;
  This->lines           = NULL;
  This->linen           = 0;
  CMemoryFreeBlock      (This);
}
/***************************************************************************/
static int GetProcedureIdentifier (
 OProcedure  a_pcd
,char* a_name  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name;
/*.........................................................................*/
  name   = OProcedureGetName(a_pcd);
  if( (a_name==NULL) || (name==NULL) ) return 0;
  return strncmp(name,a_name,strlen(a_name))==0 ? 1 : 0;
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
  CErrorHandle ("Co","OHTML",a_name,a_code);
}
