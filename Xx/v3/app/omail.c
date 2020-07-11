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
#include <stdio.h>
#include <stdlib.h>

#include <CPrinter.h>
#include <CMemory.h>

#include <XMailbox.h>

#ifdef __cplusplus
extern "C"{
#endif
int  main (int a_cmd_narg,char** a_cmd_args);
static int  Read ();
#ifdef __cplusplus
}
#endif

static char what[] = "@(#)omail v3";

static struct 
{
  Display* disp;
} Class = {NULL};
/***************************************************************************/
int main (
 int a_narg
,char** a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int     count;
  int     cr,cw;
  Window  rmbx,wmbx;
 
  char    prompt[80];
  char    string[80];
/*.........................................................................*/
  what[0] = '@'; /*c++ no warning.*/
  if (a_narg <=1)
    {
      CInfo("omail arguments format:\n\
    -    omail -r 'rmbx' : .create rmbx \n\
                           .wait and print received messages\n\
    -    omail -w 'wmbx' : .wait creation of wmbx by an other processus\n\
                           .send typed message to wmbx (type stop to exit)\n\
    -    omail -r 'rmbx' -w 'wmbx'  : .wait creation of wmbx by an other processus\n\
                                      .create rmbx\n\
                                      .wait something\n\
                                      .send typed answer\n\
    -    omail -w 'wmbx' -r 'rmbx'  : .wait creation of wmbx by an other processus\n\
                                      .create rmbx\n\
                                      .send typed message\n\
                                      .wait answer\n\
    Note that you can enter window ident for 'wmbx'\n\
    ex: omail -w 11534337 -r my_mailbox\n\
");
      exit(EXIT_SUCCESS);
    }

  Class.disp = XOpenDisplay("");
  if(Class.disp==NULL) 
    {
      CWarn("Unable to open X display.\n");
      exit(EXIT_SUCCESS);
    }
  CInfo("Connected to X display.\n");

  cr   = cw = 0;
  wmbx = 0L;
  for(count=0;count<a_narg;count++)
    {
      if(strcmp(a_args[count],"-r")==0)
        {
          if(count+1<a_narg) cr = count+1;
        }
      else if(strcmp(a_args[count],"-w")==0)
        {
          if(count+1<a_narg) 
            {
             cw = count+1;
             if(sscanf(a_args[cw],"%lu",&wmbx)!=1)  wmbx = 0L;
            }
        }
    }

  if( (cr==0) && (cw==0) ) 
    {
      CWarn("Bad arguments.\n");
      for(count=0;count<a_narg;count++)
	CWarnF("%s\n",a_args[count]);
      exit(EXIT_SUCCESS);
    }

  if(cr!=0)
    {
      rmbx = XMailboxCreate (a_args[cr],Class.disp);
      if(rmbx==0L) 
	{
	  CWarnF("Unable to create mailbox %s.\n",a_args[cr]);
	  exit(EXIT_SUCCESS);
	}
      CInfoF("Mailbox %s created. Address:%d\n",a_args[cr],rmbx);
    }
  
  if(cw!=0) 
    {
      if(wmbx==0L)
        {
          CInfoF("Try to connect to %s...\n",a_args[cw]);
          while(1)
	    {
	      wmbx = XMailboxGetIdentifier(a_args[cw],Class.disp);
	      if(wmbx!=0L) break;
	      CInfo("Try again...\n");
	    }
          CInfoF("Connected to %s\n",a_args[cw]);
          sprintf(prompt,"Send to %s> ",a_args[cw]);
        }
      else
        {
          XMailboxCreate (NULL,Class.disp); /* to exec InitializeClass of XMailbox */
          sprintf(prompt,"Send to %lu> ",wmbx);
        }
    }
    
  if((cr!=0)&&(cw==0))
    {
      while(1)
	{
	  if(Read()==0) break;
	}
    }
  else if((cr==0)&&(cw!=0))
    {
      while(1)
	{
	  printf  (prompt);
	  gets    (string);
	  if(XMailboxSendString(wmbx,string)==0) 
	    {
	      CWarnF("unable to send:%s\n",string);
	      break;
	    }
	  if( (string!=NULL) && (strcmp(string,"stop")==0) ) break;
	}
    }
  else if((cr!=0)&&(cw!=0)&&(cr<cw))
    {
      while(1)
	{
	  if(Read()==0) break;
	  printf  (prompt);
	  gets    (string);
	  if(XMailboxSendString(wmbx,string)==0) break;
	  if( (string!=NULL) && (strcmp(string,"stop")==0) ) break;
	}
    }
  else if((cr!=0)&&(cw!=0)&&(cr>cw))
    {
      while(1)
	{
	  printf  (prompt);
	  gets    (string);
	  if(XMailboxSendString (wmbx,string)==0) break;
	  if( (string!=NULL) && (strcmp(string,"stop")==0) ) break;
	  if(Read()==0) break;
	}
    }

  XCloseDisplay (Class.disp);

  return EXIT_SUCCESS;
}

/***************************************************************************/
static int Read (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  string   = XMailboxWaitMessage ();
  if(string==NULL) return 0;
  if(strcmp(string,"stop")==0) 
    {
      CMemoryFreeBlock(string);
      return 0;
    }
  CInfoF   ("%s\n",string);
  CMemoryFreeBlock (string);
  return   1;
}





