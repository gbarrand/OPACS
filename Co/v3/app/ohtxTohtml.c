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

#include <CFile.h>
#include <CPrinter.h>
#include <CStream.h>
#include <CString.h>
#include <CText.h>
#include <OType.h>

#ifdef __cplusplus
extern "C"{
#endif
#ifdef __cplusplus
}
#endif

/***************************************************************************/
int main (
 int a_narg
,char** a_args
)
/***************************************************************************/
{
  char** lines;
  int    linen;
  char*  name;
  FILE*  file;
   int count;
  int    subtopic,list,example;
/*.........................................................................*/
  if(a_narg!=2) return EXIT_FAILURE;  

  lines  = CFileLoadText (a_args[1],&linen);
  if(lines==NULL)  return EXIT_FAILURE;

  name   = CFileGetName (a_args[1]);

  file   = CFileOpenForWritingF (strlen(name)+5,"%s.htm",name);
  if(file==NULL) 
    {
      CWarnF("Can't open %s.htm file.\n",name);
      CStringDelete (name);
      return EXIT_FAILURE;
    }
  CStringDelete (name);

  CStreamInitializeStatus ();

  CStreamPrintF (file,"<TITLE>%s</TITLE>\n",name);

  subtopic = 0;
  list     = 0;
  example  = 0;
  for(count=0;count<linen;count++) 
    { 
      char* line;
      line  = lines[count];
      if(subtopic==1)
	{
	  if(strncmp(line,"endtopic",8)==0) 
	    {
	      subtopic      = 0;
	      CStreamPrintF(file,"</UL>\n");
	      CStreamPrintF (file,"<HR>\n");
	    }
	  else
	    {
	      char** words;
	      int    wordn;
	      words  = CStringGetWords (line," ",&wordn);
	      if(wordn==1)
		{
		  char*              title;
		  title              = CStringDuplicate (words[0]);
		  CStringReplacePart (&title,"_"," ");
		  CStreamPrintF      (file,"<LI><A HREF=\"#%s\">%s</A>\n",line,title);
		  CStringDelete      (title);
		}
	      else if(wordn>=2)
		{
		  char*              fname;
		  fname              = CStringDuplicate (words[1]);
		  CStringReplacePart (&fname,".ohtx",".htm");
		  CStreamPrintF      (file,"<LI><A HREF=\"%s\">%s</A>\n",fname,words[0]);
		  CStringDelete      (fname);
		}
	      CTextDelete   (wordn,words);
	    }
	}
      else if(list==1)
	{
	  if(strncmp(line,"endlist",7)==0) 
	    {
	      list   = 0;
	      CStreamPrintF(file,"</UL>\n");
	    }
	  else if(strncmp(line," -",2)==0) 
	    {
	      CStreamPrintF(file,"<LI> %s\n",line+2);
	    }
	  else
	    {
	      CStreamPrintF(file,"%s\n",line);
	    }
	}
      else if(example==1)
	{
	  if(strncmp(line,"endexa",6)==0) 
	    {
	      example = 0;
	      CStreamPrintF(file,"</PRE>\n");
	    }
	  else
	    {
	      char*              string;
              string             = CStringDuplicate (line);
	      CStringReplacePart (&string,"<","&lt;");
	      CStringReplacePart (&string,">","&gt;");
	      CStringReplacePart (&string,"<","&lt;");
	      CStringReplacePart (&string,">","&gt;");
	      CStreamPrintF      (file,"%s\n",string);
	      CStringDelete      (string);
	    }
	}
      else
	{
	  if(line[0]=='!') 
	    {
	      CStreamPrintF(file,"<%s>\n",line);
	    }
	  else if(strncmp(line,"------------------------------------",36)==0) 
	    {
	      CStreamPrintF(file,"<P>\n%s\n<P>\n",line);
	    }
          else if(CStringHasOnlySpaces(line)!=0)
	    {
	      CStreamPrintF(file,"<P>\n");
	    }
	  else if(strncmp(line,"begtopic",8)==0) 
	    {
	      char* topic;
	      char* title;
	      topic              = CStringGetFirstWord (line+8);
	      title              = CStringDuplicate (topic);
	      CStringReplacePart (&title,"_"," ");
	      CStreamPrintF      (file,"<A NAME=\"%s\"><H3>%s</H3></A>\n",topic,title);
	      CStringDelete      (title);
	      CStringDelete      (topic);
	    }
	  else if(strncmp(line,"endtopic",8)==0) 
	    {
	      CStreamPrintF(file,"<HR>\n");
	    }
	  else if(strncmp(line,"subtopic",8)==0) 
	    {
	      CStreamPrintF(file,"<P>Other documentations :\n");
	      CStreamPrintF(file,"<UL>\n");
	      subtopic = 1;
	    }
	  else if(strncmp(line,"beglist",7)==0) 
	    {
	      CStreamPrintF(file,"<UL>\n");
	      list = 1;
	    }
	  else if(strncmp(line,"begexa",6)==0) 
	    {
	      CStreamPrintF(file,"<PRE>\n");
	      example = 1;
	    }
	  else 
	    {
	      CStreamPrintF(file,"%s\n",line);
	    }
	}
    }



  CTextDelete    (linen,lines);

  fclose         (file);

  OTypeClearClass ();
  return          EXIT_SUCCESS;
}



