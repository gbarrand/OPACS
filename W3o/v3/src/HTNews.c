/*			NEWS ACCESS				HTNews.c
**			===========
**
** History:
**	26 Sep 90	Written TBL
**	29 Nov 91	Downgraded to C, for portable implementation.
**         Mar 95       Newsreader Enhancements (in progress) -B. Swetland
*/

/*
** WARNING: This code is under development.  The only thing good about it
** right now is that it works (most of the time). Read at your own risk.
**     -- BJS
*
* - fixed &gt and &lt's with ;'s
* - added xhdr patch by MH 
*
*/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "HTNews.h"
#include "HTString.h"
#include "HTAlert.h"
#include "HTTCP.h"


#define NEWS_PORT 119		/* See rfc977 */
#define APPEND			/* Use append methods */
#define MAX_CHUNK	40	/* Largest number of articles in one window */
#define CHUNK_SIZE	20	/* Number of articles for quick display */

#ifndef DEFAULT_NEWS_HOST
#define DEFAULT_NEWS_HOST "news"
#endif
#ifndef SERVER_FILE
#define SERVER_FILE "/usr/local/lib/rn/server"
#endif

#define FAST_THRESHOLD 100	/* Above this, read IDs fast */
#define CHOP_THRESHOLD 50	/* Above this, chop off the rest */

#include "HTUtils.h"		/* Coding convention macros */
#include "HTNet.h"
#include "HTMLNew.h"
#include "HTParse.h"
#include "HTFormat.h"

struct _HTStructured 
{
  CONST HTStructuredClass *	isa;
  /* ... */
};

/*#define TRACE 1 */

#define LINE_LENGTH 512		   /* Maximum length of line of ARTICLE etc */
#define GROUP_NAME_LENGTH    256   /* Maximum length of group name */


/* Thread Chain Structure */
typedef struct NEWSART {
    struct NEWSART *prev, *next, *prevt, *nextt;    /* Article List pointers */
    char *FirstRef, *LastRef;                       /* Thread List pointers */
    int num;                                        /* Article Header Info */
    char *ID;
    char *SUBJ;
    char *FROM;
} NewsArt;

/*	Module-wide variables
*/
PRIVATE NewsArt *CurrentArt = NULL;
PRIVATE NewsArt *FirstArt = NULL;
PRIVATE NewsArt *LastArt = NULL;
PRIVATE char *NewsGroup = NULL;
PRIVATE int Count = 0;
PRIVATE int GroupFirst = 0;
PRIVATE int GroupLast = 0;
PRIVATE int ReadLast = 0;
PRIVATE int ReadFirst = 0;

PUBLIC char * HTNewsHost = NULL;  /*Barrand*/
PRIVATE int s;					/* Socket for NewsHost */
PRIVATE char response_text[LINE_LENGTH+1];	/* Last response */

PRIVATE ConfigView = 1;                         /* view format configure */

/* PRIVATE HText *	HT;	*/		/* the new hypertext */
PRIVATE HTStructured * target;			/* The output sink */
PRIVATE HTStructuredClass targetClass;		/* Copy of fn addresses */
PRIVATE HTParentAnchor *node_anchor;		/* Its anchor */
PRIVATE int	diagnostic;			/* level: 0=none 2=source */

#define PUTC(c) (*targetClass.put_character)(target, c)
#define PUTS(s) (*targetClass.put_string)(target, s)
#define START(e) (*targetClass.start_element)(target, e, 0, 0)
#define END(e) (*targetClass.end_element)(target, e)


/* Internal Utility Functions ----------------------------------------------*/

/*	Case insensitive string comparisons
**	-----------------------------------
**
** On entry,
**	template must be already un upper case.
**	unknown may be in upper or lower or mixed case to match.
*/
PRIVATE HTBoolean match ARGS2 (CONST char *,unknown, CONST char *,template_)
{
    CONST char * u = unknown;
    CONST char * t = template_;
    for (;*u && *t && (TOUPPER(*u)==*t); u++, t++) /* Find mismatch or end */ ;
    return (HTBoolean)(*t==0);		/* OK if end of template */
}

/*	Find Author's name in mail address
**	----------------------------------
**
** On exit,
**	THE EMAIL ADDRESS IS CORRUPTED
**
** For example, returns "Tim Berners-Lee" if given any of
**	" Tim Berners-Lee <tim@online.cern.ch> "
**  or	" tim@online.cern.ch ( Tim Berners-Lee ) "
*/
PRIVATE char * author_name ARGS1 (char *,email)
{
    char *s;
    char *e = NULL;
    
    if ((s=strchr(email,'(')) && (e=strchr(email, ')')))
        if (e>s) {
	    *e=0;		      /* Chop off everything after the ')'  */
	    return HTStrip(s+1);      /* Remove leading and trailing spaces */
	}
	
    if ((s=strchr(email,'<')) && (e=strchr(email, '>')))
        if (e>s) {
	    strcpy(s, e+1);	      /* Remove <...> */
	    return HTStrip(email);    /* Remove leading and trailing spaces */
	}
	
    return HTStrip(email);	      /* Default to the whole thing */
}


/* compare two strings w/out crashing SGIs, etc */
static int strmatch(char *s1, char *s2)
{
    if(!s1 || !s2) return 0;

    while(*s1){
	if(!*s2) return 0;
	if(*s1 != *s2) return 0;
	s1++; s2++;
    }
    return 1;
}

/* Free the Article List */
void freeart(NewsArt *art)
{
    if(art->ID) free(art->ID);
    if(art->SUBJ) free(art->SUBJ);
    if(art->FROM) free(art->FROM);
    if(art->FirstRef) free(art->FirstRef);
    if(art->LastRef) free(art->LastRef);
    free(art);
}

PRIVATE void *ClearArtList NOARGS
{
    NewsArt *temp,*tnext,*temp2,*tnext2;
    
    temp = FirstArt;
    while(temp) {
	tnext = temp->nextt;
	if(temp->next){
	    temp2 = temp->next;
	    while(temp2){
		tnext2 = temp2->next;
		freeart(temp2);
		temp2 = tnext2;
	    }
	}
	freeart(temp);
	temp = tnext;
    }

    FirstArt = NULL;
    LastArt = NULL;
    CurrentArt = NULL;

    GroupLast = GroupFirst = ReadLast = ReadFirst = 0;

    if(NewsGroup) free(NewsGroup);
    NewsGroup = NULL;
    Count = 0;
    return NULL;
}

/* Allocate a New Article Entry */
PRIVATE NewsArt *NewArt NOARGS
{
    NewsArt *temp;

    if( !(temp = (NewsArt *) malloc(sizeof(NewsArt))) )
	outofmem(__FILE__, "NewArt");

    /* wipe potentially unused fields */
    temp->LastRef = NULL;
    temp->FirstRef = NULL;

    temp->ID = NULL;
    temp->FROM = NULL;
    temp->SUBJ = NULL;

    return temp;
}

/* Add an Article to the thread chain */
PRIVATE void AddArtTop ARGS1(CONST NewsArt *, add)
{
    NewsArt *temp;

    /* Easy case ... */
    if(!FirstArt){
	add->prev = NULL;
	add->next = NULL;
	add->prevt = NULL;
	add->nextt = NULL;
	FirstArt = add;
	LastArt = add;
	return;
    }

    /* If threaded, try to find some more of me ... */
    if(add->FirstRef){
	for(temp = LastArt; temp; temp = temp->prevt)
	    if( strmatch(add->FirstRef,temp->FirstRef) 
		|| strmatch(add->FirstRef,temp->ID) ) break;
    } else { 
	for(temp = LastArt; temp; temp = temp->prevt)
	    if(strmatch(add->ID,temp->FirstRef)) break;
    }

    /* If we found a thread point ... */
    if(temp){
	/* follow the thread on down... */
	while(temp->next) temp = temp->next;

	add->prevt = add->nextt = NULL;
	add->next = NULL;
	add->prev = temp;
	
	temp->next = add;
    } else {
	/* Otherwise, tack it onto the back of the list */
	add->prev = NULL;
	add->next = NULL;

	add->prevt = LastArt;
	add->nextt = NULL;
	LastArt->nextt = add;
	LastArt = add;
    }
}




/*	Start anchor element
**	--------------------
*/
PRIVATE void start_anchor ARGS1(CONST char *,  href)
{
  PUTS ("<A HREF=\"");
  PUTS (href);
  PUTS ("\">");
}

/*	Paste in an Anchor
**	------------------
**
**
** On entry,
**	HT 	has a selection of zero length at the end.
**	text 	points to the text to be put into the file, 0 terminated.
**	addr	points to the hypertext refernce address,
**		terminated by white space, comma, NULL or '>' 
*/
PRIVATE void write_anchor ARGS2(CONST char *,text, CONST char *,addr)
{
    char href[LINE_LENGTH+1];
		
    {
    	CONST char * p;
	strcpy(href,"news:");
	for(p=addr; *p && (*p!='>') && !WHITE(*p) && (*p!=','); p++);
        strncat(href, addr, p-addr);	/* Make complete hypertext reference */
    }
    
    start_anchor(href);
    PUTS(text);
    PUTS("</A>");
}


/*	Write list of anchors
**	---------------------
**
**	We take a pointer to a list of objects, and write out each,
**	generating an anchor for each.
**
** On entry,
**	HT 	has a selection of zero length at the end.
**	text 	points to a comma or space separated list of addresses.
** On exit,
**	*text	is NOT any more chopped up into substrings.
*/
PRIVATE void write_anchors ARGS1 (char *,text)
{
    char * start = text;
    char * end;
    char c;
    for (;;) {
        for(;*start && (WHITE(*start)); start++);  /* Find start */
	if (!*start) return;			/* (Done) */
        for(end=start; *end && (*end!=' ') && (*end!=','); end++);/* Find end */
	if (*end) end++;	/* Include comma or space but not NULL */
	c = *end;
	*end = 0;
	write_anchor(start, start);
	*end = c;
	start = end;			/* Point to next one */
    }
}

/*	Abort the connection					abort_socket
**	--------------------
*/
PRIVATE void abort_socket NOARGS
{
    if (TRACE) fprintf(stderr,
	    "HTNews: EOF on read, closing socket %d\n", s);
    NETCLOSE(s);	/* End of file, close socket */
    PUTS("Network Error: connection lost");
    PUTC('\n');
    s = -1;		/* End of file on response */
    return;
}



PUBLIC CONST char * HTGetNewsHost NOARGS
{
	return HTNewsHost;
}

PUBLIC void HTSetNewsHost ARGS1(CONST char *, value)
{
	StrAllocCopy(HTNewsHost, value);
}

/*	Initialisation for this module
**	------------------------------
**
**	We pick up the NewsHost name from
**
**	1.	Environment variable NNTPSERVER
**	2.	File SERVER_FILE
**	3.	Compilation time macro DEFAULT_NEWS_HOST
**	4.	Default to "news"
*/
PRIVATE HTBoolean initialized = NO;
PRIVATE HTBoolean initialize NOARGS
{
  /*   Get name of Host
   */
  if (getenv("NNTPSERVER")) 
    {
      StrAllocCopy(HTNewsHost, (char *)getenv("NNTPSERVER"));
      if (TRACE) fprintf(stderr, "HTNews: NNTPSERVER defined as `%s'\n",
                         HTNewsHost);
    } 
  else 
    {
      char server_name[256];
      FILE* fp = fopen(SERVER_FILE, "r");
      if (fp) 
        {
          if (fscanf(fp, "%s", server_name)==1) 
            {
              StrAllocCopy(HTNewsHost, server_name);
              if (TRACE) fprintf(stderr,
                                 "HTNews: File %s defines news host as `%s'\n",
                                 SERVER_FILE, HTNewsHost);
	    }
          fclose(fp);
	}
    }
  if (!HTNewsHost) 
    HTNewsHost = DEFAULT_NEWS_HOST;
  
  s = -1;		/* Disconnected */
  
  return YES;
}



/*	Send NNTP Command line to remote host & Check Response
**	------------------------------------------------------
**
** On entry,
**	command	points to the command to be sent, including CRLF, or is null
**		pointer if no command to be sent.
** On exit,
**	Negative status indicates transmission error, socket closed.
**	Positive status is an NNTP status.
*/

PRIVATE int newswrite ARGS1(CONST char *, msg)
{
    int status;
    if( (status = NETWRITE(s, msg, strlen(msg))) <0){
	if (TRACE) 
	    fprintf(stderr, "HTNews: Unable to send command. Disconnecting.\n");
	NETCLOSE(s);
	s = -1;
	return status;
    } /* if bad status */
    return status; /*Barrand*/
}


PRIVATE int response ARGS1(CONST char *,command)
{
  int result;    
  char * p = response_text;
  if (command) 
    {
      int status;
      int length = strlen(command);
      if (TRACE) 
        fprintf(stderr, "NNTP command to be sent: %s", command);
      status = NETWRITE(s, command, length);
      if (status<0)
        {
          if (TRACE) fprintf(stderr,
                             "HTNews: Unable to send command. Disconnecting.\n");
          NETCLOSE(s);
          s = -1;
          return status;
	} /* if bad status */
    } /* if command to be sent */
  
  for(;;) 
    {  
      if (((*p++=HTGetCharacter ()) == LF)
          || (p == &response_text[LINE_LENGTH])) 
        {
          *p++=0;				/* Terminate the string */
          if (TRACE) fprintf(stderr, "NNTP Response: %s\n", response_text);
          sscanf(response_text, "%d", &result);
          return result;	    
	} /* if end of line */
      
      if (*(p-1) < 0) 
        {
          if (TRACE) fprintf(stderr,
                             "HTNews: EOF on read, closing socket %d\n", s);
          NETCLOSE(s);	/* End of file, close socket */
          return s = -1;	/* End of file on response */
	}
    } /* Loop over characters */
}

/* Setup our networking */

PRIVATE int OpenNNTP NOARGS 
{
    /* CONNECTING to news host */
    char url[1024];
    int status;

    sprintf (url, "lose://%s/", HTNewsHost);

    if (TRACE)
	fprintf (stderr, "News: doing HTDoConnect on '%s'\n", url);

    status = HTDoConnect (url, "NNTP", NEWS_PORT, &s);

    if (TRACE)
	fprintf (stderr, "News: Done DoConnect; status %d\n", status);

    if (status == HT_INTERRUPTED) {
	/* Interrupt cleanly. */
	return 3;
    }

    if (status < 0) {
	NETCLOSE(s);
	s = -1;
	if (TRACE) 
	    fprintf(stderr, "HTNews: Unable to connect to news host.\n");
	return 2;

    } else {
	if (TRACE) 
	    fprintf(stderr, "HTNews: Connected to news host %s.\n",HTNewsHost);
	HTInitInput(s);		/* set up buffering */
	if ((response(NULL) / 100) !=2) {
	    NETCLOSE(s);
	    s = -1;
	    
	    return 1;
	}
    }
    return 0;
} 


/* Interface with news-gui.c and various others... */
void NNTPconfig(int viewtype)
{
    ConfigView = viewtype;
}

/* this is VERY non-reentrant.... */
static char qline[LINE_LENGTH+1];

char *NNTPgetquoteline(char *art)
{
    char *p;
    int i,status ;

    if (!initialized)
	initialized = initialize();
    if (!initialized){
	if(TRACE) fprintf(stderr,"No init?\n");
	HTProgress ("Could not set up news connection.");
	return NULL;
    }
    
    if(s < 0) {
	HTProgress("Attempting to connect to news server");
	if(OpenNNTP()){
	    if(TRACE) fprintf(stderr,"No OpenNNTP?\n");
	    HTProgress ("Could not connect to news server.");
	    return NULL;
	}
    }

    if(art){
	/* FLUSH!!! */
	HTInitInput(s);
	sprintf(qline, "BODY <%s>%c%c", art, CR, LF);
	status = response(qline);
	
	if (status != 222) return NULL;
    }

    qline[0] = '>';
    qline[1] = ' ';

    for(p = &qline[2],i=0;;p++,i++){
	*p = HTGetCharacter();

	if (*p==(char)EOF) {
	    abort_socket();	/* End of file, close socket */
	    return NULL;	/* End of file on response */
	}

	if(*p == '\n'){
	    *++p = 0;
	    break;
	}

	if(i == LINE_LENGTH-4){
	    *p = 0;
	    break;
	}
    }
    
    if(qline[2]=='.' && qline[3] < ' ') return NULL;
    return qline;
}
 
int NNTPgetarthdrs(char *art,char **ref, char **grp, char **subj, char **from)
{
    int status, done;
    char *aname,*p;
    char line[LINE_LENGTH+1];
    char buffer[LINE_LENGTH+1];

    *ref = *grp = *subj = *from = NULL;
 
    if (!initialized)
	initialized = initialize();
    if (!initialized){
	if(TRACE) fprintf(stderr,"No init?\n");
	HTProgress ("Could not set up news connection.");
	return HT_NOT_LOADED;	/* FAIL */
    }
    
    if(s < 0) {
	HTProgress("Attempting to connect to news server");
	if(OpenNNTP()){
	    if(TRACE) fprintf(stderr,"No OpenNNTP?\n");
	    HTProgress ("Could not connect to news server.");
	    return HT_NOT_LOADED;	/* FAIL */
	}
    }

    /* FLUSH!!! */
    HTInitInput(s);
    sprintf(buffer, "HEAD <%s>%c%c", art, CR, LF);
    status = response(buffer);
	
    if (status == 221) {	/* Head follows - parse it:*/
	
	p = line;				/* Write pointer */
	done = NO;
	while(!done){
	    char ch = *p++ = HTGetCharacter ();
	    if (ch==(char)EOF) {
		abort_socket();	/* End of file, close socket */
		return HT_OK;	/* End of file on response */ /*Barrand : add HT_OK*/
	    }
	    
	    if ((ch == LF)
		|| (p == &line[LINE_LENGTH]) ) {
		
		*--p=0;		/* Terminate  & chop LF*/
		p = line;		/* Restart at beginning */
		if (TRACE) fprintf(stderr, "G %s\n", line);
		switch(line[0]) {
		    
		case '.':
		    done = (line[1]<' ');	/* End of article? */
		    break;
		    
		case 'S':
		case 's':
		    if (match(line, "SUBJECT:"))
			StrAllocCopy(*subj, line+9);/* Save subject */
		    break;
		    
		case 'R':
		case 'r':
		    if (match(line, "REFERENCES:")) {
			p = line + 12;
			StrAllocCopy(*ref,p+1);
		    }
		    break;
		    
		case 'N':
		case 'n':
		    if (match(line, "NEWSGROUPS:")) {
			p = line + 11;
			StrAllocCopy(*grp,p+1);
		    }
		    break;
		    
		case 'f':
		case 'F':
		    if (match(line, "FROM:")) {
			aname = author_name(strchr(line,':')+1);
			if (aname && *aname){
			    StrAllocCopy(*from, aname);
			    p = *from + strlen(*from) - 1;
			    if (*p==LF) *p = 0;	/* Chop off newline */
			} else {
			    StrAllocCopy(*from, "Unknown");
			}
		    }
		    break;
		    
		} /* end switch on first character */
		
		p = line;		/* Restart at beginning */
	    } /* if end of line */
	} /* Loop over characters */
    } /* If good response */
    return HT_OK;
}
 
int NNTPpost(char *from, char *subj, char *ref, char *groups, char *msg)
{
    char buf[1024];
  
    if (!initialized)
	initialized = initialize();
    if (!initialized){
	if(TRACE) fprintf(stderr,"No init?\n");
	HTProgress ("Could not set up news connection.");
	return HT_NOT_LOADED;	/* FAIL */
    }
    
    if(s < 0) {
	HTProgress("Attempting to connect to news server");
	if(OpenNNTP()){
	    if(TRACE) fprintf(stderr,"No OpenNNTP?\n");
	    HTProgress ("Could not connect to news server.");
	    return HT_NOT_LOADED;	/* FAIL */
	}
    }
    
    if(response("POST\r\n") != 340) {
	HTProgress("Server does not allow posting.");
	return 0;
    }

    HTProgress("Posting your article...");
    sprintf(buf,"From: %s\r\n",from);
    newswrite(buf);
    sprintf(buf,"Subject: %s\r\n",subj);
    newswrite(buf);
    if(ref){
	sprintf(buf,"References: %s\r\n",ref);
	newswrite(buf);
    }
    sprintf(buf,"Newsgroups: %s\r\n",groups);
    newswrite(buf);
    sprintf(buf,"X-Newsreader: NCSA Mosaic\r\n\r\n");
    newswrite(buf);
    newswrite(msg);
    if(response("\r\n.\r\n") != 240)
	HTProgress("Article was not posted.");
    else
	HTProgress("Article was posted successfully.");

    return HT_OK;
}


/* take a url and return the news article for it if its in the news cache */
NewsArt *is_news_url(char *s)
{
    NewsArt *art, *art2;

    if((strlen(s) > 5) && !strncmp("news:", s, 5)){
	s = &s[5];
	/* check the obvious */
	if(CurrentArt && strmatch(s,CurrentArt->ID)) return CurrentArt;
	for(art = FirstArt; art; art = art->nextt){
	    if(strmatch(s,art->ID)) return art;
	    if(art->next)
		for(art2 = art->next; art2; art2 = art2->next)
		    if(strmatch(s,art2->ID)) return art2;
	}
    }
    return NULL;
}

/* These are called by their gui_news_* counterparts in gui-news.c */
void news_prevt(char *url)
{	
    NewsArt *art;

    if( (art = (NewsArt*)is_news_url(url) )) {
	while(art->prev) art = art->prev;
	if(art->prevt){
	    sprintf(url,"news:%s",art->prevt->ID);
	    return;
	}
    }   
    url[0] = 0;
}

void news_nextt(char *url)
{
    NewsArt *art;

    if( (art = (NewsArt*)is_news_url(url)) ){
	while(art->prev) art = art->prev;
	if(art->nextt){
	    sprintf(url,"news:%s",art->nextt->ID);
	    return;
	}
    }
    url[0] = 0;
}

void news_prev(char *url)
{
    NewsArt *art;

    if( (art = (NewsArt*)is_news_url(url)) ){
	if(art->prev){
	    sprintf(url,"news:%s",art->prev->ID);
	    return;
	} 
    }
    url[0] = 0;
}

void news_next(char *url)
{
    NewsArt *art;

    if( (art = (NewsArt*)is_news_url(url)) ) {
	if(art->next){
	    sprintf(url,"news:%s",art->next->ID);
	    return;
	}
    }
    url[0] = 0;
}

void news_index(char *url)
{
    if(NewsGroup && is_news_url(url))
 	sprintf(url,"news:%s",NewsGroup);
    else
	url[0] = 0;
}


void printmenu(NewsArt *art)
{
    char line[LINE_LENGTH+1];
    PUTS("<H2>");
    if(art->prev){
	sprintf(line,"<A HREF=\"%s\">[PREV]</A>  ",art->prev->ID);
	PUTS(line);
    } else {
	PUTS("[PREV]  ");
    }
    if(art->next){
	sprintf(line,"<A HREF=\"%s\">[NEXT]</A>  ",art->next->ID);
	PUTS(line);
    } else {
	PUTS("[NEXT]  ");
    }
    
    while(art->prev) art = art->prev;
    
    if(art->prevt){
	sprintf(line,"<A HREF=\"%s\">[PREV Thread]</A>  ",art->prevt->ID);
	PUTS(line);
    } else {
	PUTS("[PREV Thread]  ");
    }
    if(art->nextt){
	sprintf(line,"<A HREF=\"%s\">[NEXT Thread]</A>  ",art->nextt->ID);
	PUTS(line);
    } else {
	PUTS("[NEXT Thread]  ");
    }
    sprintf(line,"<A HREF=\"news:%s\">[Index]</A>  ",NewsGroup);
    PUTS(line);
    PUTS("</H2>");
}
		
/*	Read in an Article					read_article
**	------------------
**
**
**	Note the termination condition of a single dot on a line by itself.
**	RFC 977 specifies that the line "folding" of RFC850 is not used, so we
**	do not handle it here.
**
** On entry,
**	s	Global socket number is OK
**	HT	Global hypertext object is ready for appending text
*/       
PRIVATE void read_article ARGS1 (char *, artID)
{

    char line[LINE_LENGTH+1];
    char *references=NULL;			/* Hrefs for other articles */
    char *newsgroups=NULL;			/* Newsgroups list */
    char *p = line;
    HTBoolean done = NO;

    NewsArt *art,*art2;
    int l;

    l = strlen(artID)-3; /* ">\n\r" should be stripped outside !!! */
    for(art = FirstArt; art; art = art -> nextt){
	if(!strncmp(art->ID,artID,l)) break;
	if(art->next){
	    for(art2 = art->next; art2; art2 = art2->next)
		if(!strncmp(art2->ID,artID,l)) break;
	    if(art2) {
		art = art2;
		break;
	    }
	}
    }

    if(art) CurrentArt = art;
    else CurrentArt = NULL;


/*	Read in the HEADer of the article:
**
**	The header fields are either ignored, or formatted and put into the
**	 Text.
*/
    if(art){
	printmenu(art);
	PUTS("<HR>");
    }

    (*targetClass.start_element)(target, HTML_ADDRESS, 0, 0);
    while(!done){
	char ch = *p++ = HTGetCharacter ();
	if (ch==(char)EOF) {
	    abort_socket();	/* End of file, close socket */
	    return;		/* End of file on response */
	}
	if ((ch == LF) || (p == &line[LINE_LENGTH])) {
	    *--p=0;				/* Terminate the string */
	    if (TRACE) fprintf(stderr, "H %s\n", line);
	    
	    if (line[0]=='.') {	
		if (line[1]<' ') {		/* End of article? */
		    done = YES;
		    break;
		}
		
	    } else if (line[0]<' ') {
		break;		/* End of Header? */
	    } else if (match(line, "SUBJECT:")) {
		END(HTML_ADDRESS);
		START(HTML_TITLE);			/** Uuugh! @@@ */
		PUTS(line+8);
		END(HTML_TITLE);
		START(HTML_ADDRESS);
		(*targetClass.start_element)(target, HTML_H1 , 0, 0);
		PUTS(line+8);
		(*targetClass.end_element)(target, HTML_H1);
		END(HTML_ADDRESS);
		(*targetClass.start_element)(target, HTML_ADDRESS , 0, 0);
	    } else if (match(line, "DATE:")
		       || match(line, "FROM:")
		       || match(line, "ORGANIZATION:")) {
		strcat(line, "\n");
		PUTS(strchr(line,':')+1);
	    } else if (match(line, "NEWSGROUPS:")) {
		StrAllocCopy(newsgroups, HTStrip(strchr(line,':')+1));
		
	    } else if (match(line, "REFERENCES:")) {
		StrAllocCopy(references, HTStrip(strchr(line,':')+1));
		
	    } /* end if match */
	    p = line;			/* Restart at beginning */
	} /* if end of line */
    } /* Loop over characters */
    (*targetClass.end_element)(target, HTML_ADDRESS);
    
    if (newsgroups || references) {
	(*targetClass.start_element)(target, HTML_DL , 0, 0);
	if (newsgroups) {
	    (*targetClass.start_element)(target, HTML_DT , 0, 0);
	    PUTS("Newsgroups:");
	    (*targetClass.start_element)(target, HTML_DD , 0, 0);
	    write_anchors(newsgroups);
	    free(newsgroups);
	}
	
	if (references) {
	    (*targetClass.start_element)(target, HTML_DT , 0, 0);
	    PUTS("References:");
	    (*targetClass.start_element)(target, HTML_DD , 0, 0);
	    write_anchors(references);
	    free(references);
	}
	(*targetClass.end_element)(target, HTML_DL);
    }
    PUTS("<HR>\n");
    
/*	Read in the BODY of the Article:
*/
    (*targetClass.start_element)(target, HTML_PRE , 0, 0);

    p = line;
    while(!done){
	char ch = *p++ = HTGetCharacter ();
	if (ch==(char)EOF) {
	    abort_socket();	/* End of file, close socket */
	    return;		/* End of file on response */
	}
	if ((ch == LF) || (p == &line[LINE_LENGTH])) {
	    *p++=0;				/* Terminate the string */
	    if (TRACE) fprintf(stderr, "B %s", line);
	    if (line[0]=='.') {
		if (line[1]<' ') {		/* End of article? */
		    done = YES;
		    break;
		} else {			/* Line starts with dot */
		    PUTS(&line[1]);	/* Ignore first dot */
		}
	    } else {

/*	Normal lines are scanned for buried references to other articles.
**	Unfortunately, it will pick up mail addresses as well!
*/
		char *l = line;
		char *p;
		int f; /* ':' flag */
#ifdef OLF
		while (p=strchr(l, '<')) {
		    char *q  = strchr(p,'>');
		    char *at = strchr(p, '@');
		    if (q && at && at<q) {
		        char c = q[1];
			q[1] = 0;		/* chop up */
			*p = 0;
			PUTS(l);
			*p = '<'; 		/* again */
			*q = 0;
			start_anchor(p+1);
			*q = '>'; 		/* again */
			PUTS(p);
                        PUTS("</A>");
			q[1] = c;		/* again */
			l=q+1;
		    } else break;		/* line has unmatched <> */
		} 
#endif
		for(f=1,p=line;*p;p++){
		    if(*p == '<'){
			if(strchr(p,'>')){
			    f = 0;
			} else {
			    *p = 0;
			    PUTS(l);
			    PUTS("&lt;");
			    l=p+1; /* step over */
			}
		    }
#ifdef VERYCOOL
			f = 0;
			/* am I a url */
			for(pp=p+1;*pp > ' ';pp++){
			    if(*pp == ':') f++;
			    if(*pp == '>'){
				if(f){
				    PUTS("<A HREF=\"");
				    p=l=pp+1;
				    *pp = 0;
				    PUTS(p+1);
				    PUTS("\">&lt;");
				    PUTS(p+1);
				    PUTS("&gt;</A>");
				}
			    }
			}				    

#endif	      
		    if(*p == '>'){
			if(f){
			    *p = 0;
			    PUTS(l);
			    PUTS("&gt;");
			    l=p+1; /* step over */
			}
		    }
		}
		PUTS(l);	/* Last bit of the line */
	    } /* if not dot */

	    p = line;				/* Restart at beginning */
	} /* if end of line */
    } /* Loop over characters */
    
    (*targetClass.end_element)(target, HTML_PRE);
    if(art){
	PUTS("<HR>");
	printmenu(art);
    }
}


/*	Read in a List of Newsgroups
**	----------------------------
*/
/*
**	Note the termination condition of a single dot on a line by itself.
**	RFC 977 specifies that the line "folding" of RFC850 is not used, so we
**	do not handle it here.
*/        
PRIVATE void read_list NOARGS
{

    char line[LINE_LENGTH+1];
    
    char group[LINE_LENGTH];
    int first, last;
    char postable;

    char *p;
    HTBoolean done = NO;
    
/*	Read in the HEADer of the article:
**
**	The header fields are either ignored, or formatted and put into the
**	Text.
*/
    (*targetClass.start_element)(target, HTML_H1 , 0, 0);
    PUTS( "Newsgroups");
    (*targetClass.end_element)(target, HTML_PRE);
    p = line;
    (*targetClass.start_element)(target, HTML_MENU , 0, 0);
    while(!done){
	char ch = *p++ = HTGetCharacter ();
	if (ch==(char)EOF) {
	    abort_socket();	/* End of file, close socket */
	    return;		/* End of file on response */
	}
	if ((ch == LF) || (p == &line[LINE_LENGTH])) {
	    *p++=0;				/* Terminate the string */
	    if (TRACE) fprintf(stderr, "B %s", line);
    	    (*targetClass.start_element)(target, HTML_LI , 0, 0);
	    if (line[0]=='.') {
		if (line[1]<' ') {		/* End of article? */
		    done = YES;
		    break;
		} else {			/* Line starts with dot */
		    PUTS( &line[1]);
		}
	    } else {

/*	Normal lines are scanned for references to newsgroups.
*/
		if (sscanf(line, "%s %d %d %c", 
			   group, &last, &first, &postable)==4)
		    sprintf(line,"<A HREF=\"news:%s\">%s</A> (%d) %s\n",
			    group,group,
			    (last<first)?0:(last-first),
			    postable=='y'?" ":" not postable ");
		PUTS(line);
	    } /* if not dot */
	    p = line;			/* Restart at beginning */
	} /* if end of line */
    } /* Loop over characters */
    (*targetClass.end_element)(target, HTML_MENU);
}

int parsexover(char *x, char **num, char **title, char **from,
	       char **date, char **msgid, char **ref, char **bytes, char **lines)
{
  *num = x;

  while(*x && *x != '\t') x++;      /* step to next tab */
  if(!*x) return 0;                 /* early end of string - bad record */
  *x = 0;                           /* terminate */
  x++;                              /* bump to start of next field */
  *title = x;

  while(*x && *x != '\t') x++;      /* step to next tab */
  if(!*x) return 0;                 /* early end of string - bad record */
  *x = 0;                           /* terminate */
  x++;                              /* bump to start of next field */
  *from = x;

  while(*x && *x != '\t') x++;      /* step to next tab */
  if(!*x) return 0;                 /* early end of string - bad record */
  *x = 0;                           /* terminate */
  x++;                              /* bump to start of next field */
  *date = x;

  while(*x && *x != '\t') x++;      /* step to next tab */
  if(!*x) return 0;                 /* early end of string - bad record */
  *x = 0;                           /* terminate */
  x++;                              /* bump to start of next field */
  *msgid = x;

  while(*x && *x != '\t') x++;      /* step to next tab */
  if(!*x) return 0;                 /* early end of string - bad record */
  *x = 0;                           /* terminate */
  x++;                              /* bump to start of next field */
  *ref = x;
  
  while(*x && *x != '\t') x++;      /* step to next tab */
  if(!*x) return 0;                 /* early end of string - bad record */
  *x = 0;                           /* terminate */
  x++;                              /* bump to start of next field */
  *bytes = x;

  while(*x && *x != '\t') x++;      /* step to next tab */
  if(!*x) return 0;                 /* early end of string - bad record */
  *x = 0;                           /* terminate */
  x++;                              /* bump to start of next field */
  *lines = x;

  while(*x && *x != '\t') x++;      /* step to next tab */
  *x = 0;                           /* terminate */

  return 1;
}

PRIVATE void XBuildArtList ARGS3(
  CONST char *,groupName,
  int,first_required,
  int,last_required
)
{
    NewsArt *art;
    char *p,*aname, *aref;

    char *num,*title,*date,*msgid,*ref,*bytes,*lines,*from;

    char buf[2048];

    int status, count, first, last;	/* Response fields */
					/* count is only an upper limit */

    if(TRACE) fprintf(stderr,"[%s]",response_text);
    sscanf(response_text, "%d %d %d %d", &status, &count, &first, &last);
    if(TRACE) fprintf(stderr,"Newsgroup status=%d, count=%d, (%d-%d)",
    			status, count, first, last);

    Count = 0;

    if(NewsGroup && !strncmp(NewsGroup,groupName,strlen(groupName))){
	last_required = last;
	first_required = ReadLast +1;
    } else {
	first_required = first;
	last_required = last;
	ClearArtList();

	StrAllocCopy(NewsGroup, groupName);
    }

    GroupFirst = first;
    GroupLast = last; 
    ReadLast = last;

    if (first_required<GroupFirst) 
 	first_required = GroupFirst;     
    if ((last_required==0) || (last_required > GroupLast)) 
	last_required = GroupLast;

    if(first_required > last_required) return;

    /* FLUSH!!! */
    HTInitInput(s);

    sprintf(buf, "XOVER %d-%d\r\n", first_required, last_required);
    if(response(buf) != 224) return; 

    for(;;){
	if(!(Count % 25) ) {
	    sprintf(buf, "Threading Article %d of %d",Count,count);
	    HTProgress (buf);
	}
	
	/* EOS test needed */
	for(p = buf;(*p = HTGetCharacter());p++){
	    if(*p=='\r' || *p=='\n'){
		*p = 0;
		break;
	    }
	    if(*p == (char)EOF){
		abort_socket();	/* End of file, close socket */
		return;		/* End of file on response */
	    }
	}

	if(buf[0]=='.') break; /* end of list */

	art = NewArt();

	parsexover(buf,&num,&title,&from,&date,&msgid,&ref,&bytes,&lines);

	art->num = atoi(num);
	
	Count++;
	     	
	StrAllocCopy(art->SUBJ, title);
	
	if(ref[0]){
	    p = ref;
	    aref = p;
	    if(*p=='<'){
		while(*aref && *aref!='>') aref++;
		aref++;
		*(aref-1) = 0;
		StrAllocCopy(art->FirstRef, p+1);
	    }
	    do aref++; while(*aref);
	    p = aref-1;
	    while(*p) {
		if(*p == '>'){
		    *p = 0;
		    while(*--p && *p != '<');
		    if(*p=='<')
			StrAllocCopy(art->LastRef,p+1);
		    break;
		}
		p--;
	    }
	}
	
	msgid++; /* Chop < */
	msgid[strlen(msgid)-1]=0;		/* Chop > */
	StrAllocCopy(art->ID, msgid);


	aname = author_name(from);
	if (aname && *aname){
	    StrAllocCopy(art->FROM, aname);
	    p = art->FROM + strlen(art->FROM) - 1;
	    if (*p==LF) *p = 0;	/* Chop off newline */
	} else {
	    StrAllocCopy(art->FROM, "Unknown");
	}
	AddArtTop(art);

    }
    
    HTProgress("Done Threading Articles");
}


PRIVATE void BuildArtList ARGS3(
  CONST char *,groupName,
  int,first_required,
  int,last_required
)
{
    NewsArt *art;
    char *p,*aname, *aref;
    HTBoolean done;

    char buffer[LINE_LENGTH];
    char line[LINE_LENGTH];
    int artno;				/* Article number WITHIN GROUP */
    int status, count, first, last;	/* Response fields */
					/* count is only an upper limit */

    int *artlist,c,i;

    if(TRACE) fprintf(stderr,"[%s]",response_text);
    sscanf(response_text, "%d %d %d %d", &status, &count, &first, &last);
    if(TRACE) fprintf(stderr,"Newsgroup status=%d, count=%d, (%d-%d)",
    			status, count, first, last);

    if(NewsGroup && !strncmp(NewsGroup,groupName,strlen(groupName))){
	last_required = last;
	first_required = ReadLast +1;
    } else {
	ClearArtList();
	StrAllocCopy(NewsGroup, groupName);
    }


    if (first_required<GroupFirst) 
	first_required = GroupFirst;           /* clip */
    if ((last_required==0) || (last_required > GroupLast)) 
	last_required = GroupLast;

/*	Read newsgroup using individual fields:
*/


    c = 0; 

    artlist = NULL;

    if(count){
	if(!(artlist = (int *) malloc(sizeof(int) * (count+2))))
	    outofmem(__FILE__, "BuildArtList");
	
	if(response("listgroup\r\n") != 211){
	    /* try XHDR if LISTGROUP fails */
	    /* thanks to Martin Hamilton for this bit 'o code...
               his choice of header, not mine */
	    sprintf(buffer, "xhdr anarchy-in-the-uk %d-%d\r\n", first, last);
 
	    if(response(buffer) != 221) {
		HTProgress("Cannot get article list from news server");    
		return;
	    }
	}

	/* read the list of available articles from the NNTP server */
	artlist[0]=0;
	while(c<(count+2)){
	    char ch = HTGetCharacter ();
	    
	    if (ch==(char)EOF) {
		abort_socket();	/* End of file, close socket */
		return;		/* End of file on response */
	    }
	    if(ch == '.') break;
	    if(ch == LF){
		if(TRACE) fprintf(stderr,"[%d]",artlist[c]);
		c++;
		artlist[c]=0;
	    } else {
		if (isdigit(ch))
		    artlist[c] = artlist[c]*10 + ch-'0';	   
	    } 
	} /* Loop over characters */
    }


/*    for(artno = last_required; artno >= first_required; artno--) {*/
    for(i=0;i<c;i++){
	artno = artlist[i];

	if(artno <= GroupLast) continue;
	/* FLUSH!!! */
	HTInitInput(s);
	sprintf(buffer, "HEAD %d%c%c", artno, CR, LF);
	status = response(buffer);
	
/*	fprintf(stderr,"%d:[%d]:%s\n",artno,status,buffer);*/

	if (status == 221) {	/* Head follows - parse it:*/
    
	    art = NewArt();
	    art->num = artno;
	    Count++;
	    
	    if(!(Count % 25) ) {
		sprintf(buffer, "Threading Article %d of %d",Count,count);
		HTProgress (buffer);
	    }
	    
	    if(!ReadFirst || artno<ReadFirst) ReadFirst = artno;
	    if(!ReadLast || artno>ReadLast) ReadLast = artno;
	    
	    p = line;				/* Write pointer */
	    done = NO;
	    while(!done){
		char ch = *p++ = HTGetCharacter ();
		if (ch==(char)EOF) {
		    abort_socket();	/* End of file, close socket */
		    return;		/* End of file on response */
		}
		
		if ((ch == LF)
		    || (p == &line[LINE_LENGTH]) ) {
		    
		    *--p=0;		/* Terminate  & chop LF*/
		    p = line;		/* Restart at beginning */
		    if (TRACE) fprintf(stderr, "G %s\n", line);
		    switch(line[0]) {
			
		    case '.':
			done = (line[1]<' ');	/* End of article? */
			break;
			
		    case 'S':
		    case 's':
			if (match(line, "SUBJECT:"))
			    StrAllocCopy(art->SUBJ, line+9);/* Save subject */
			break;
			
		    case 'R':
		    case 'r':
			if (match(line, "REFERENCES:")) {
			    p = line + 12;
			    aref = p;
			    if(*p=='<'){
				while(*aref && *aref!='>') aref++;
				aref++;
				*(aref-1) = 0;
				StrAllocCopy(art->FirstRef, p+1);
			    }
			    do aref++; while(*aref);
			    p = aref-1;
			    while(*p) {
				if(*p == '>'){
				    *p = 0;
				    while(*--p && *p != '<');
				    if(*p=='<')
					StrAllocCopy(art->LastRef,p+1);
				    break;
				}
				p--;
			    }
			}
			break;
			
		    case 'M':
		    case 'm':
			if (match(line, "MESSAGE-ID:")) {
			    char * addr = HTStrip(line+11) +1; /* Chop < */
			    addr[strlen(addr)-1]=0;		/* Chop > */
			    StrAllocCopy(art->ID, addr);
			}
			break;
			
		    case 'f':
		    case 'F':
			if (match(line, "FROM:")) {
			    aname = author_name(strchr(line,':')+1);
			    if (aname && *aname)
				{
                                    StrAllocCopy(art->FROM, aname);
                                    p = art->FROM + strlen(art->FROM) - 1;
                                    if (*p==LF) *p = 0;	/* Chop off newline */
				}
			    else
				{
                                    StrAllocCopy(art->FROM, "Unknown");
				}
			}
			break;
			
		    } /* end switch on first character */
		    
		    p = line;		/* Restart at beginning */
		} /* if end of line */
	    } /* Loop over characters */
	    AddArtTop(art);

/*	 indicate progress!   @@@@@@
*/
    
	} /* If good response */
    } /* Loop over article */	 

    if(artlist) free(artlist);

    GroupLast = last;
    GroupFirst = first;

    HTProgress("Done Threading Articles");
}


/*	Read in a Newsgroup
**	-------------------
**	Unfortunately, we have to ask for each article one by one if we
**	want more than one field.
**
*/
PRIVATE void read_group ARGS3(
  CONST char *,groupName,
  int,first,
  int,last
)
{
    NewsArt *art,*art2;

    char buffer[LINE_LENGTH];
    int i;			 /* Article number WITHIN GROUP */
					/* count is only an upper limit */

    
    if (!FirstArt) {
        PUTS( "\nNo articles in this group.\n");
	return;
    }

/*	Set window title
*/
    sprintf(buffer, "Newsgroup %s,  Articles %d-%d",
    		groupName, FirstArt->num, LastArt->num);
    START(HTML_TITLE);
    PUTS(buffer);
    END(HTML_TITLE);

/*	Read newsgroup using individual fields:
*/

/*      Link to earlier articles
*/

/*
   if (ReadFirst>GroupFirst) {
        int before;                    
        if (ReadFirst-MAX_CHUNK <= GroupFirst) before = GroupFirst;
        else before = ReadFirst-CHUNK_SIZE;
        sprintf(buffer, "%s/%d-%d", groupName, before, ReadFirst-1);
        if (TRACE) fprintf(stderr, "    Block before is %s\n", buffer);
        PUTS( " (");
        start_anchor(buffer);
        PUTS("Earlier articles");
        PUTS("</A>");
        PUTS( "...)\n");
    }
    if (ReadFirst==GroupFirst && ReadLast==GroupLast)
	PUTS("\nAll available articles in ");
    else PUTS( "\nArticles in ");
    PUTS(groupName);
*/
    START(HTML_OL);

    for(art = FirstArt; art; art = art->nextt){
	START(HTML_LI);
	if(!art->next) {
	    if(!ConfigView) PUTS("[1] ");

	    sprintf(buffer, "<STRONG>\"%s\"</STRONG> - %s", 
		    art->SUBJ, art->FROM);
	    if (art->ID) {
		write_anchor(buffer, art->ID);
	    } else {
		PUTS(buffer);
	    }
	} else {
	    for(i=0,art2=art;art2->next;art2=art2->next) i++;
/*	    if(art->FirstRef){
		if(match(art->SUBJ,"RE:"))
		    sprintf(buffer, "\"%s\" ---", &art->SUBJ[4]);
		else
		    sprintf(buffer, "\"%s\" ---", art->SUBJ);
		PUTS(buffer);
	    }
*/ 
	    
	    if(!ConfigView) {
		sprintf(buffer,"[%d] ",i+1);
		PUTS(buffer);
	    }

	    sprintf(buffer, "<STRONG>\"%s\"</STRONG> - %s", 
		    art->SUBJ, art->FROM);
	    if (art->ID) {
		write_anchor(buffer, art->ID);
	    } else {
		PUTS(buffer);
	    }

	    if(art->next && ConfigView){
		START(HTML_MENU);
		for(art2 = art->next; art2; art2 = art2->next){
		    START(HTML_LI);
/*		    sprintf(buffer, "\"%s\" - %s", art2->SUBJ, art2->FROM);*/
		    sprintf(buffer, "%s", art2->FROM);
		    if (art2->ID) {
			write_anchor(buffer, art2->ID);
		    } else {
			PUTS(buffer);
		    }
		}
		END(HTML_MENU);
	    }
	}
    }	       
    END(HTML_OL);
    START(HTML_P);

#ifdef HMMMM
/*	 indicate progress!   @@@@@@*/
/*      Link to later articles*/
    if (ReadLast<GroupLast) {
        int after;                      /* End of article after */
        after = ReadLast+CHUNK_SIZE;
        if (after==last) sprintf(buffer, "news:%s", groupName); /* original group */
        else sprintf(buffer, "news:%s/%d-%d", groupName, ReadLast+1, after);
        if (TRACE) fprintf(stderr, "    Block after is %s\n", buffer);
        PUTS( "<P> (");
        start_anchor(buffer);
        PUTS( "Later articles");
        PUTS("</A>");
        PUTS( "...)\n");
    }    
#endif
 first = 0;
 last = 0;
}




/*		Load by name					HTLoadNews
**		============
*/
PUBLIC int HTLoadNews ARGS4(
	CONST char *,		arg,
	HTParentAnchor *,	anAnchor,
	HTFormat,		format_out,
	HTStream*,		stream)
{
  char command[257];			/* The whole command */
  char groupName[GROUP_NAME_LENGTH];	/* Just the group name */
  int status;				/* tcp return */
  int retries;			/* A count of how hard we have tried */ 
  HTBoolean group_wanted;			/* Flag: group was asked for, not article */
  HTBoolean list_wanted;			/* Flag: group was asked for, not article */
  int first, last;			/* First and last articles asked for */
  
  int has_xover;
  diagnostic = (format_out == WWW_SOURCE);	/* set global flag */
  
  if (TRACE) 
    fprintf(stderr, "HTNews: Looking for %s\n", arg);
  
  if (!initialized) 
    initialized = initialize();
  if (!initialized) 
    {
      HTProgress ("Could not set up news connection.");
      return HT_NOT_LOADED;	/* FAIL */
    }
    
  {
    CONST char * p1=arg;
    
    /*	We will ask for the document, omitting the host name & anchor.
     **
     **	Syntax of address is
     **		xxx@yyy			Article
     **		<xxx@yyy>		Same article
     **		xxxxx			News group (no "@")
     **		group/n1-n2		Articles n1 to n2 in group
     */        
    group_wanted = (strchr(arg, '@')==0) && (strchr(arg, '*')==0);
    list_wanted  = (strchr(arg, '@')==0) && (strchr(arg, '*')!=0);
    
    /* p1 = HTParse(arg, "", PARSE_PATH | PARSE_PUNCTUATION); */
    /* Don't use HTParse because news: access doesn't follow traditional
       rules. For instance, if the article reference contains a '#',
       the rest of it is lost -- JFG 10/7/92, from a bug report */
    if (!strncasecomp (arg, "news:", 5))
      p1 = arg + 5;  /* Skip "news:" prefix */
    if (list_wanted) 
      {
        strcpy(command, "LIST ");
      } 
    else if (group_wanted) 
      {
        char * slash = strchr(p1, '/');
        strcpy(command, "GROUP ");
        first = 0;
        last = 0;
        if (slash) 
          {
            *slash = 0;
            strcpy(groupName, p1);
            *slash = '/';
            (void) sscanf(slash+1, "%d-%d", &first, &last);
          } 
        else 
          {
            strcpy(groupName, p1);
          }
        strcat(command, groupName);
      } 
    else 
      {
        strcpy(command, "ARTICLE ");
        if (strchr(p1, '<')==0) 
          strcat(command,"<");
        strcat(command, p1);
        if (strchr(p1, '>')==0) 
          strcat(command,">");
      }
    
    {
      char * p = command + strlen(command);
      *p++ = CR;		/* Macros to be correct on Mac */
      *p++ = LF;
      *p++ = 0;
    }
  } /* scope of p1 */
  
  if (!*arg) 
    {
      HTProgress ("Could not load data.");
      return HT_NOT_LOADED;			/* Ignore if no name */
    }
  
  /*	Make a hypertext object with an anchor list. */       
  node_anchor = anAnchor;
  target = HTML_new(anAnchor, format_out, stream);
  targetClass = *target->isa;	/* Copy routine entry points */
  
    	
  /*	Now, let's get a stream setup up from the NewsHost: */       
  for (retries=0; retries<2; retries++)
    {
      target = HTML_new(anAnchor, format_out, stream);
      targetClass = *target->isa;	/* Copy routine entry points */
      if (s < 0) 
	  if((status = OpenNNTP())){
	      char message[256];
	      switch(status){
	      case 1:
		  /* Couldn't get it. */
		  START(HTML_TITLE);
		  PUTS("Could Not Retrieve Information");
		  END(HTML_TITLE);
		  PUTS("Sorry, could not retrieve information.");
		  (*targetClass.end_document)(target);
		  (*targetClass.free)(target);
		  return HT_LOADED;
	      case 2:
		  if (retries<=1) {
		      /* Since we reallocate on each retry, free here. */
		      (*targetClass.end_document)(target);
		      (*targetClass.free)(target);
		      continue;
		  }
		  HTProgress ("Could not access news host.");
		  sprintf(message,"\nCould not access news host %s.  Try setting environment variable <code>NNTPSERVER</code> to the name of your news host, and restart Mosaic.", HTNewsHost);
	    
		  PUTS(message);
		  (*targetClass.end_document)(target);
		  (*targetClass.free)(target);
		  return HT_LOADED;
	      case 3:
		  HTProgress ("Connection interrupted.");
		  (*targetClass.handle_interrupt)(target);	    

		  return HT_INTERRUPTED;
	      }
	  }

      status = response("XOVER\r\n");
      
      if(status != 500) has_xover = 1; 
      else has_xover = 0;
      /* FLUSH!!! */
      HTInitInput(s);
      
      status = response(command);

      if (TRACE)
	  fprintf (stderr, "News: Sent '%s', status %d\n", command, status);
      if (status < 0) 
	  break;
      if ((status/ 100) !=2) {
	  PUTS(response_text);
	  (*targetClass.end_document)(target);
	  (*targetClass.free)(target);
	  NETCLOSE(s);
	  s = -1;
	  continue;	/* Try again */
      }
	  
      /*	Load a group, article, etc
       */
      
      if (list_wanted) 
	  read_list();
      else if (group_wanted){ 
	  if(has_xover) XBuildArtList(groupName, first, last);
	  else BuildArtList(groupName, first, last);
	  read_group(groupName, first, last);
      } else { 
	  read_article(&command[9]);
      }
      (*targetClass.end_document)(target);
      (*targetClass.free)(target);
      return HT_LOADED;
      
    } /* Retry loop */
  
  return HT_LOADED;
}

PUBLIC HTProtocol HTNews = { "news", HTLoadNews, NULL };













