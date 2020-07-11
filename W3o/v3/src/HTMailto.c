/*			MAILTO WINDOW				HTMailTo.c
 **			=============
 ** Authors:
 **  Mike Peter Bretz (bretz@zdv.uni-tuebingen.de)
 **  Alan Braverman (alanb@ncsa.uiuc.edu)
 **
 ** History:
 **	07 Jul 94   First version  (MPB)
 **     07 Mar 95   Stuck it in NCSA Mosaic for X 2.6 (AMB)
 */
#include "HTString.h"
#include "HTAccess.h"
#include "HTUtils.h"
#include "HTMLNew.h"
#include "HTParse.h"
#include "HTFormat.h"
#include "HTAlert.h"

#include "HTMailto.h"

#ifdef __cplusplus
extern "C"{
#endif
int mo_post_mailto_win (char*,char*);
#ifdef __cplusplus
}
#endif

struct _HTStructured 
{
  CONST HTStructuredClass *	isa;
  /* ... */
};

/*	Module-wide variables
 */
PRIVATE int s;                                  /* Socket for FingerHost */
/*PRIVATE HTStructured * target;*/			/* The output sink */
/*PRIVATE HTStructuredClass targetClass;*/		/* Copy of fn addresses */

static HTGetMailtoKludgeInfoFunction getMailtoKludgeInfoProc = NULL;


/*	Initialisation for this module
 **	------------------------------
 */
PRIVATE HTBoolean initialized = NO;
PRIVATE HTBoolean initialize NOARGS
{
  s = -1;			/* Disconnected */
  return YES;
}


PUBLIC int HTSendMailTo ARGS4(
      CONST char *,     arg,
      HTParentAnchor *,	anAnchor,
      HTFormat,		format_out,
      HTStream*,	stream)
{
  char *mailtoURL     = NULL;
  char *mailtoSubject = NULL;

  if (TRACE) 
    fprintf(stderr, "HTMailto: Mailing to %s\n", arg);
  
  if (!initialized) 
    initialized = initialize();
  if (!initialized) 
    {
      HTProgress ((char *) 0);
      return HT_NOT_LOADED;
    }

  {
    CONST char * p1=arg;
    
    /*	We will ask for the document, omitting the host name & anchor.
     **
     **	Syntax of address is
     **		xxx@yyy			User xxx at site yyy (xxx is optional).
     */        
    if (!strncasecomp (arg, "mailto:", 7))
      p1 = arg + 7;		/* Skip "mailto:" prefix */
    
    if (!*arg) 
      {
	HTProgress ("Could not find email address");
	return HT_NOT_LOADED;	/* Ignore if no name */
      }

    if(getMailtoKludgeInfoProc)  /*Barrand*/
      getMailtoKludgeInfoProc(&mailtoURL,&mailtoSubject);  /*Routine in libhtmlw !!!*/
    else
      printf ("W3o/HTMailto/HTSendMailto: routine GetMailtoKludgeInfo not declared.\n");
    (void) mo_post_mailto_win(p1,mailtoSubject);
    return HT_LOADED;
  }
  anAnchor = NULL;
  format_out = NULL;
  stream = NULL;
}

void HTSetGetMailtoKludgeInfoProc(HTGetMailtoKludgeInfoFunction a_proc) /*Barrand avoid dependency of libwww over libhtmlw.*/
{
  getMailtoKludgeInfoProc = a_proc;
}

PUBLIC HTProtocol HTMailto = { "mailto", HTSendMailTo, NULL };
