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

#ifdef __cplusplus
extern "C"{
#endif
static void DoFiles   (char*);
static void DoFile    (char*,char*);
static int  GetOldNew (char*,char**,char**);
#ifdef __cplusplus
}
#endif

static char what[] = "@(#)opatch v3";
/***************************************************************************/
int main (
 int a_argn
,char** a_args
)
/***************************************************************************/
{
/*.........................................................................*/
  what[0] = '@'; /*c++ no warning.*/
  if(a_argn<=1) 
    {
      CInfo ("\
Two arguments required:\n\
  opatch <file of modifications>\n\
  opatch <file of modifications> <file to modify>.\n");
      return EXIT_SUCCESS;
    }

  if(a_argn==2)
    {
      DoFiles (a_args[1]);
    }

  if(a_argn==3)
    {
      DoFile  (a_args[1],a_args[2]);
    }

  return       EXIT_SUCCESS;
}
/***************************************************************************/
static void DoFiles (
 char* a_patch
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** ulines;
  int    ulinen;
/*.........................................................................*/
  ulines      = CFileLoadText (a_patch,&ulinen);
  if(ulines==NULL) return;
 
  for(count=0;count<ulinen;count++) 
    { 
      if( (ulines[count]!=NULL) && (strncmp(ulines[count],"#",1)==0) ) 
        {
/*          printf("%s\n",ulines[count]);*/
        }
      else 
        { char** words;
          int    nword;
          words  = CStringGetWords (ulines[count]," ",&nword);
          if(nword>=3) 
            {
                   if(strcmp(words[0],"file")==0)
		{
                  rename (words[1],words[2]);
                }
            }
          CTextDelete (nword,words);
        }
    }

  CTextDelete   (ulinen,ulines);
}
/***************************************************************************/
static void DoFile (
 char* a_patch
,char* a_fname 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char** lines;
  int    linen;
  char** ulines;
  int    ulinen;
  int    modified;
/*.........................................................................*/
  ulines      = CFileLoadText (a_patch,&ulinen);
  if(ulines==NULL) return;
 
  lines       = CFileLoadText (a_fname,&linen);
  if(lines==NULL)  return;

  modified    = 0;
  for(count=0;count<ulinen;count++) 
    { 
      if(CStringHasOnlySpaces(ulines[count])==1) continue;
      if(strncmp(ulines[count],"#",1)==0)
        {
/*          printf("%s\n",ulines[count]);*/
        }
      else if(strncmp(ulines[count],"warnifword",10)==0)
        {
	  char*         string;
          string        = CStringGetFirstNotSpacePosition (ulines[count]+10);
	  if(string!=NULL)
	    {
	       int count;
	      char*         word;
	      word          = CStringGetFirstWord (string);
	      for(count=0;count<linen;count++) 
		{
		  if(CStringGetFirstWordPosition(lines[count],word)!=NULL)
		    {
		      printf("%s\n%s\n",string+strlen(word),lines[count]);
		    }
		}
	      CStringDelete (word);
	    }
        }
      else if(strncmp(ulines[count],"warnif",6)==0)
        {
	  char*         string;
          string        = CStringGetFirstNotSpacePosition (ulines[count]+6);
	  if(string!=NULL)
	    {
	       int count;
	      char*         word;
	      word          = CStringGetFirstWord (string);
	      for(count=0;count<linen;count++) 
		{
		  if(strstr(lines[count],word)!=NULL)
		    {
		      printf("%s\n%s\n",string+strlen(word),lines[count]);
		    }
		}
	      CStringDelete (word);
	    }
        }
      else 
        { 
	  char* sold;
	  char* snew;
	  if(GetOldNew (ulines[count],&sold,&snew)==1)
            {
                   if(strncmp(ulines[count],"word",4)==0)
		{
                  if(CTextReplaceWord (linen,lines,sold,snew)==1) modified = 1;
                }
              else if(strncmp(ulines[count],"string",6)==0)
		{
                  if(CTextReplaceString(linen,lines,sold,snew)==1) modified = 1;
                }
            }
	  else
	    {
	      CWarnF ("Bad syntax in:\n%s\n",ulines[count]);
	    }
          CStringDelete (sold);
          CStringDelete (snew);
        }
    }

  if(modified==1)
    {
      CInfoF         ("%s modified\n",a_fname);
      CFileSafeguard (a_fname);
      CFileSaveText  (a_fname,linen,lines);
    }

  CTextDelete   (linen,lines);
  CTextDelete   (ulinen,ulines);
}
/***************************************************************************/
static int GetOldNew (
 char*  a_string
,char** a_old
,char** a_new
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string = NULL;
  char* pos;
/*.........................................................................*/
  if(a_old!=NULL) *a_old = NULL;
  if(a_new!=NULL) *a_new = NULL;
  if(a_string==NULL) return 0;

       if(strncmp(a_string,"word"  ,4)==0) 
    string = CStringDuplicate(a_string + 4);
  else if(strncmp(a_string,"string",6)==0) 
    string = CStringDuplicate(a_string + 6);
  else 
    return 0;

  pos    = strchr(string,'\'');
  if(pos==NULL)
    {
      char** words;
      int    wordn;
      words  = CStringGetWords (string," ",&wordn);
      if(wordn==2) 
	{
	  if(a_old!=NULL) *a_old = CStringDuplicate(words[0]);
	  if(a_new!=NULL) *a_new = CStringDuplicate(words[1]);
	  CTextDelete   (wordn,words);
	  CStringDelete (string);
	  return        1;
	}
      else
        {
	  CTextDelete   (wordn,words);
	  CStringDelete (string);
	  return        0;
	}
    }
  else
    {
      char*     begin_new;
      char*     begin_old;

      begin_old = pos + 1;
      pos       = strchr(begin_old,'\'');
      if(pos==NULL) 
	{
	  CStringDelete (string);
	  return 0;
	}
      *pos      = '\0';

      pos       = strchr(pos+1,'\'');
      if(pos==NULL)
	{
	  CStringDelete (string);
	  return 0;
	}

      begin_new = pos + 1;
      pos       = strchr(begin_new,'\'');
      if(pos==NULL)
	{
	  CStringDelete (string);
	  return 0;
	}
      *pos      = '\0';
         
      if(a_old!=NULL) *a_old = CStringDuplicate(begin_old);
      if(a_new!=NULL) *a_new = CStringDuplicate(begin_new);

    }

  CStringDelete (string);
  return 1;
}
