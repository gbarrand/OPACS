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

#include <CPrinter.h>
#include <CFile.h>
#include <CString.h>
#include <CText.h>
#include <CMemory.h>

#ifdef __cplusplus
extern "C"{
#endif
static void DoHeader            (char*);
static void DoC                 (char*);
static void DoMethods           (char*);
static void DoReferenceCommands (char*);
static void DoCommands          (char*);
static void TreatHTML           (char**);
#ifdef __cplusplus
}
#endif

#define CATSH "http://www.lal.in2p3.fr/cgi-bin/SI/cat.sh?"

static char what[] = "@(#)odoc v3";
/***************************************************************************/
int main (
 int a_argn
,char** a_args
)
/***************************************************************************/
{
   int count;
  char** lines;
  int    linen;
/*.........................................................................*/
  what[0] = '@'; /*c++ no warning.*/
  if(a_argn!=2) 
    {
      CInfo ("\
  This (private) tool works on:\n\
 - hs.lis produced by hs.sh\n\
 - cs.lis produced by cs.sh\n\
  One arguments required:\n\
    odoc [h, c, methods, commands]\n");
      return EXIT_SUCCESS;
    }

  if(strcmp(a_args[1],"h")==0)
    {
      lines       = CFileLoadText ("hs.lis",&linen);
      if(lines==NULL)  return EXIT_SUCCESS; 
      for(count=0;count<linen;count++) 
	{ 
	  if(lines[count][0]=='#') continue;
	  DoHeader (lines[count]);
	}
      CTextDelete   (linen,lines);
    }
  else if(strcmp(a_args[1],"c")==0)
    {
      lines       = CFileLoadText ("cs.lis",&linen);
      if(lines==NULL)  return EXIT_SUCCESS; 
      for(count=0;count<linen;count++) 
	{ 
	  if(lines[count][0]=='#') continue;
	  DoC      (lines[count]);
	}
      CTextDelete   (linen,lines);
    }
  else if(strcmp(a_args[1],"methods")==0)
    {
      char*       package;
      lines       = CFileLoadText ("cs.lis",&linen);
      if(lines==NULL)  return EXIT_SUCCESS; 
     
      package     = lines[0] + strlen("#package ");

      printf("<TITLE>%s public methods</TITLE>\n",package);
      printf("<!***************************************************************************>\n");
      printf("<A NAME=\"%s_public_methods\" ><H3>%s public methods</H3></A>\n",package,package);
      printf("<!***************************************************************************>\n");
      /*printf("<!--------------------------------------------------------------------------->\n");*/
      printf("<P>Other documentation\n");
      printf("<UL>\n");
      for(count=0;count<linen;count++) 
	{ 
	  char*              fname;
	  if(lines[count][0]=='#') continue;
	  fname              = CFileGetName (lines[count]);
/*
	  fname              = CStringDuplicate (lines[count]);
	  CFileTrimDirectory (fname);
*/	  
	  printf("<LI><A HREF=\"#Public_methods_of_%s\">%s</A>\n",fname,fname);
	  CStringDelete (fname);
	}
      printf("</UL>\n");
      printf("<HR>\n");
      for(count=0;count<linen;count++) 
	{ 
	  if(lines[count][0]=='#') continue;
	  DoMethods (lines[count]);
	}
      CTextDelete   (linen,lines);
    }
  else if(strcmp(a_args[1],"commands")==0)
    {
      char*       package;
      lines       = CFileLoadText ("cs.lis",&linen);
      if(lines==NULL)  return EXIT_SUCCESS; 
     
      package     = lines[0] + strlen("#package ");

      printf("<TITLE>%s commands</TITLE>\n",package);
      printf("<!***************************************************************************>\n");
      printf("<A NAME=\"%s_commands\" ><H3>%s commands</H3></A>\n",package,package);
      printf("<!***************************************************************************>\n");
      /*printf("<!--------------------------------------------------------------------------->\n");*/
      printf("<P>Other documentation\n");
      printf("<UL>\n");
      for(count=0;count<linen;count++) 
	{ 
	  char*               fname;
	  if(lines[count][0]=='#') continue;
	  fname               = CFileGetName (lines[count]);
/*
	  printf("<LI><A HREF=\"#%s_commands\">%s</A>\n",fname,fname);
*/
	  DoReferenceCommands  (lines[count]);
	  CStringDelete        (fname);
	}
      printf("</UL>\n");
      printf("<HR>\n");
      for(count=0;count<linen;count++) 
	{ 
	  if(lines[count][0]=='#') continue;
	  DoCommands  (lines[count]);
	}
      CTextDelete   (linen,lines);
    }
  else
    {
      CInfoF ("Unknown option %s.\n",a_args[1]);
    }
  
    
 {char*            string;
  string           = CMemoryGetErrors();
  if(string!=NULL) CInfoF ("%s\n",string);}

  return       EXIT_SUCCESS;
}
/***************************************************************************/
static void DoHeader (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** lines;
  int    linen;
  int    length;
/*.........................................................................*/
  if(a_fname==NULL) return;
  length = strlen(a_fname);
  if( (strncmp(a_fname+length-2,".h",2)!=0) ||   
      (strncmp(a_fname+length-3,"P.h",3)==0) 
     ) /*Not a public header file.*/
    {
      return;
    }

  lines       = CFileLoadText (a_fname,&linen);
  if(lines==NULL)  return;

  printf ("File %s\n",a_fname);

  for(count=0;count<linen;count++) 
    { 
      char*  line;
      char*  pos;
      line   = CStringDuplicate(lines[count]);

      if( (strncmp(line,"static",6)!=0) &&
	  (strstr (line,"typedef")==NULL)
	 )
	{
	  pos    = CStringGetLastNotSpacePosition (line);
	  if((pos!=NULL) && (*pos==';') )
	    {
	      *pos = '\0';
	      pos  = CStringGetLastNotSpacePosition (line);
	      if( (pos!=NULL) && (*pos==')') )
		{
		  pos    = strchr (line,'(');
		  if(pos!=NULL)
		    {
		      printf ("%s\n",lines[count]);
		    }
		} 
	    }
	}
      CStringDelete (line);
    }

  printf ("\n\n");

  CTextDelete   (linen,lines);
}
/***************************************************************************/
static void DoC (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** lines;
  int    linen;
  int    found;
/*.........................................................................*/
  if(a_fname==NULL) return;

  lines       = CFileLoadText (a_fname,&linen);
  if(lines==NULL)  
    {
      printf ("File %s does not exist or is empty.\n",a_fname);
      return;
    }

  printf ("Public methods of %s :\n",a_fname);

  found = 0;
  for(count=0;count<linen;count++) 
    { 
      char*  line;
      line   = lines[count];
      if(found==0)
	{
	  char** words;
	  int    wordn;
	  words  = CStringGetWords (line," ",&wordn);
	  if( (wordn==3) || (wordn==4) ) /*Return type of routine could be in two words.*/
	    {
	      if( (strcmp(words[wordn-1],"(")==0) &&
	          (strncmp(words[0],"static",6)!=0)
		 )
		{
		  printf ("%s\n",line);
		  found  = 1;
		}
	    }
	  CTextDelete   (wordn,words);
	}
      else 
	{
	  printf ("%s\n",line);
	  if(strchr(line,')')!=NULL) 
	    {
	      found = 0;
	    }
	}
    }

  printf ("\n\n");

  CTextDelete   (linen,lines);
}
/***************************************************************************/
static void DoMethods (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** lines;
  int    linen;
  int    found;
  char*  fname;
/*.........................................................................*/
  if(a_fname==NULL) return;

  lines       = CFileLoadText (a_fname,&linen);
  if(lines==NULL)  
    {
      printf ("File %s does not exist or is empty.\n",a_fname);
      return;
    }

/*
  fname              = CStringDuplicate (a_fname);
  CFileTrimDirectory (fname);
*/
  fname              = CFileGetName (a_fname);

  printf("<!***************************************************************************>\n");
  printf("<A NAME=\"Public_methods_of_%s\" ><H3>Public methods of %s</H3></A>\n",fname,fname);
  printf("<!***************************************************************************>\n");
  /*printf("<!--------------------------------------------------------------------------->\n");*/
  printf("<P>Other documentation\n");
  printf("<UL>\n");
  found = 0;
  for(count=0;count<linen;count++) 
    { 
      char*  line;
      line   = lines[count];
      if(found==0)
	{
	  char** words;
	  int    wordn;
	  words  = CStringGetWords (line," ",&wordn);
	  if( (wordn==3) || (wordn==4) ) /*Return type of routine could be in two words.*/
	    {
	      if( (strcmp(words[wordn-1],"(")==0) &&
	          (strncmp(words[0],"static",6)!=0)
		 )
		{
		  char* name;
		  name  = words[wordn-2];
		  printf("<LI><A HREF=\"#%s\">%s</A>\n",name,name);
		  found  = 1;
		}
	    }
	  CTextDelete   (wordn,words);
	}
      else 
	{
	  if(strchr(line,')')!=NULL) 
	    {
	      found = 0;
	    }
	}
    }
  printf("</UL>\n");
  printf("<HR>\n");

  found = 0;
  for(count=0;count<linen;count++) 
    { 
      char*  line;
      line   = lines[count];
      if(found==0)
	{
	  char** words;
	  int    wordn;
	  words  = CStringGetWords (line," ",&wordn);
	  if( (wordn==3) || (wordn==4) ) /*Return type of routine could be in two words.*/
	    {
	      if( (strcmp(words[wordn-1],"(")==0) &&
	          (strncmp(words[0],"static",6)!=0)
		 )
		{
		  char* name;
		  name  = words[wordn-2];
		  printf("<!***************************************************************************>\n");
		  printf("<A NAME=\"%s\" ><H3>%s</H3></A>\n",name,name);
		  printf("<!***************************************************************************>\n");
		  printf("<PRE>\n");
		  printf("%s\n",line);
		  found    = 1;
		}
	    }
	  CTextDelete   (wordn,words);
	}
      else /*Find a header. Search doc.*/
	{
	  if(strncmp(line,"/*",2)==0) /*Skip.*/
	    {
	    }
	  else if(strncmp(line,"*/",2)==0) /*Skip.*/
	    {
	    }
	  else if(strncmp(line,"{",1)==0)
	    {
	      printf("</PRE>\n");
	      printf("<A HREF=\"%s%s\">Open %s.c</A>\n",CATSH,a_fname,fname);
 	      printf("<HR>\n");
	      found    = 0;
	    }
	  else
	    {
	      TreatHTML (&line);
	      printf ("%s\n",line);
	    }
	}
    }

  CTextDelete   (linen,lines);
  CStringDelete (fname);
}
/***************************************************************************/
static void DoReferenceCommands (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** lines;
  int    linen;
  int    found;
  char*  fname;
/*.........................................................................*/
  if(a_fname==NULL) return;

  lines       = CFileLoadText (a_fname,&linen);
  if(lines==NULL)  
    {
      printf ("File %s does not exist or is empty.\n",a_fname);
      return;
    }

  fname              = CFileGetName (a_fname);

  found = 0;
  for(count=0;count<linen;count++) 
    { 
      char*  line;
      line   = lines[count];
      if(found==0)
	{
	  char** words;
	  int    wordn;
	  words  = CStringGetWords (line," ",&wordn);
	  if(wordn==3) 
	    {
	      char* pos;
	      pos = strstr(words[1],"Execute_");
	      if( (strcmp(words[wordn-1],"(")==0) &&
	          (pos!=NULL)
		 )
		{
		  char* name;
		  name  = pos + strlen("Execute_");
		  printf("<LI><A HREF=\"#%s\">%s</A>\n",name,name);
		  found  = 1;
		}
	    }
	  CTextDelete   (wordn,words);
	}
      else 
	{
	  if(strchr(line,')')!=NULL) 
	    {
	      found = 0;
	    }
	}
    }

  CTextDelete   (linen,lines);
  CStringDelete (fname);
}
/***************************************************************************/
static void DoCommands (
 char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** lines;
  int    linen;
  int    found;
  char*  fname;
  int    beginDoc = 0;
/*.........................................................................*/
  if(a_fname==NULL) return;

  lines       = CFileLoadText (a_fname,&linen);
  if(lines==NULL)  
    {
      printf ("File %s does not exist or is empty.\n",a_fname);
      return;
    }

  fname              = CFileGetName (a_fname);

/*
  printf("<!***************************************************************************>\n");
  printf("<A NAME=\"%s_commands\" ><H3>%s_commands</H3></A>\n",fname,fname);
  printf("<!***************************************************************************>\n");
  printf("<P>Other documentation\n");
  printf("<UL>\n");
  found = 0;
  for(count=0;count<linen;count++) 
    { 
      char*  line;
      line   = lines[count];
      if(found==0)
	{
	  char** words;
	  int    wordn;
	  words  = CStringGetWords (line," ",&wordn);
	  if(wordn==3) 
	    {
	      char* pos;
	      pos = strstr(words[1],"Execute_");
	      if( (strcmp(words[wordn-1],"(")==0) &&
	          (pos!=NULL)
		 )
		{
		  char* name;
		  name  = pos + strlen("Execute_");
		  printf("<LI><A HREF=\"#%s\">%s</A>\n",name,name);
		  found  = 1;
		}
	    }
	  CTextDelete   (wordn,words);
	}
      else 
	{
	  if(strchr(line,')')!=NULL) 
	    {
	      found = 0;
	    }
	}
    }
  printf("</UL>\n");
  printf("<HR>\n");
*/

  found = 0;
  for(count=0;count<linen;count++) 
    { 
      char*  line;
      line   = lines[count];
      if(found==0)
	{
	  char** words;
	  int    wordn;
	  words  = CStringGetWords (line," ",&wordn);
	  if(wordn==3) 
	    {
	      char* pos;
	      pos = strstr(words[1],"Execute_");
	      if( (strcmp(words[wordn-1],"(")==0) &&
	          (pos!=NULL)
		 )
		{
		  char* name;
		  name  = pos + strlen("Execute_");
		  printf("<!***************************************************************************>\n");
		  printf("<A NAME=\"%s\" ><H3>%s</H3></A>\n",name,name);
		  printf("<!***************************************************************************>\n");
		  printf("<PRE>\n");
		  found    = 1;
		  beginDoc = 0;
		}
	    }
	  CTextDelete   (wordn,words);
	}
      else /*Find a header. Search doc.*/
	{
	  if(strncmp(line,"/*",2)==0)
	    {
	      beginDoc = 1;
	    }
	  else if(strncmp(line,"*/",2)==0)
	    {
	      printf("</PRE>\n");
	      printf("<A HREF=\"%s%s\">Open %s.c</A>\n",CATSH,a_fname,fname);
 	      printf("<HR>\n");
	      beginDoc = 0;
	      found    = 0;
	    }
	  else if(strncmp(line,"/*!!!",5)==0)
	    {
	      printf("</PRE>\n");
	      printf("<A HREF=\"%s%s\">Open %s.c</A>\n",CATSH,a_fname,fname);
 	      printf("<HR>\n");
	      beginDoc = 0;
	      found    = 0;
	    }
	  else if(beginDoc==1)
	    {
	      TreatHTML (&line);
	      printf ("%s\n",line);
	    }
	}
    }

  CTextDelete   (linen,lines);
  CStringDelete (fname);
}
/***************************************************************************/
static void TreatHTML (
 char** a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CStringReplacePart (a_string,"<","&lt;");
  CStringReplacePart (a_string,">","&gt;");
}
