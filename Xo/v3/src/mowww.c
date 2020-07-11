/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
   This file is the same than W3o/src/mosaic.c
 bracketted by HAS_MOSAIC
*/
#ifdef HAS_MOSAIC

#ifdef HAS_HTML

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define bcopy(source, dest, count)  memcpy(dest, source, count)

#include <CPrinter.h>

#include <momo.h>
#include <moHTextP.h>
extern HText* HTMainText;         /* W3o/moHText.c*/  /* Equivalent of main window */

/*libwww2/HTParse*/
#define PARSE_ACCESS            16
#define PARSE_HOST               8
#define PARSE_PATH               4
#define PARSE_ANCHOR             2
#define PARSE_PUNCTUATION        1
#define PARSE_ALL               31

#include <mowww.h>

#ifdef HAS_MOSAIC_IMG

#include <X11/StringDefs.h>

#ifndef HOME_PAGE_DEFAULT
/* This must be a straight string as it is included into a struct; 
   no tricks. */
#define HOME_PAGE_DEFAULT \
  "http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/NCSAMosaicHome.html"
#endif /* not HOME_PAGE_DEFAULT */

#ifndef GLOBAL_EXTENSION_MAP
#define GLOBAL_EXTENSION_MAP "/usr/local/lib/mosaic/mime.types"
#endif
#ifndef GLOBAL_TYPE_MAP
#define GLOBAL_TYPE_MAP "/usr/local/lib/mosaic/mailcap"
#endif

#include <xresources.h>
#endif /*HAS_MOSAIC_IMG*/

#ifdef __cplusplus
extern "C"{
#endif
char* HTParse               (char*,char*,int);
int   HTLoadAbsolute        (char*);
char* getcwd                (char*,size_t);

static char* doit           (char*,char**);
#ifdef __cplusplus
}
#endif

#ifdef HAS_MOSAIC_IMG
Display* dsp    = NULL;
int      Vclass = 0;
AppData  Rdata  = {0,0};  /*Barrand*/
#endif /*HAS_MOSAIC_IMG*/

/*Exported to libwww2.*/
extern int   binary_transfer;     /*W3o/momo.c*/
extern int   force_dump_to_file;  /*W3o/momo.c*/
extern char* force_dump_filename; /*W3o/momo.c*/
int   interrupted          = 0;
/***************************************************************************/
void mo_set_globals (
 void*    a_widget
,void*    a_htmlw
,mo_notify_function a_proc
)
/***************************************************************************/
/* This routine does not exists in Mosaic-2.2                              */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
#ifdef HAS_MOSAIC_IMG
  if(a_widget!=NULL)
    { 
      XVisualInfo    vinfo, *vptr; 
      int            cnt;
      dsp            = XtDisplay((Widget)a_widget);
/*Vclass*/ /*From Mosaic-2.2/src/gui.c*/
      vinfo.visualid = XVisualIDFromVisual (DefaultVisual(dsp,DefaultScreen(dsp)));
      vptr           = XGetVisualInfo      (dsp, VisualIDMask, &vinfo, &cnt);
#if defined(__cplusplus) || defined(c_plusplus)
      Vclass         = vptr->c_class;
#else
      Vclass         = vptr->class;
#endif
      XFree          ((char *)vptr);
/*Rdata*/
      XtVaGetApplicationResources ((Widget)a_widget, (XtPointer)&Rdata, resources, XtNumber (resources), NULL);
    }
  mo_register_image_resolution (a_htmlw);

#else
  a_widget         = NULL;
  a_htmlw          = NULL;
#endif /*HAS_MOSAIC_IMG*/
  mo_set_notify_proc (a_proc);
}
/****************************************************************************
 * name:    hack_htmlsrc (PRIVATE)
 * purpose: Do really nasty things to a stream of HTML that just got
 *          pulled over from a server.
 * inputs:  
 *   - none (global HTMainText is assumed to contain current
 *           HText object)
 * returns: 
 *   - HTMainText->htmlSrc (char *)
 * remarks: 
 *   This is ugly but it gets the job done.
 *   The job is this:
 *     - Filter out repeated instances of <PLAINTEXT>.
 *     - Discover if <PLAINTEXT> has been improperly applied
 *       to the head of an HTML document (we discover HTML documents
 *       by checking to see if a <TITLE> element is on the next line).
 *     - Same as above but for <HEAD> and <HTML>.
 *   We advance the character pointer HTMainText->htmlSrc by the
 *   appropriate remark to make adjustments, and keep the original
 *   head of the allocated block of text in HTMainText->htmlSrcHead.
 ****************************************************************************/
static char *hack_htmlsrc (void)
{
  if (!HTMainText)
    return NULL;

  if (!HTMainText->htmlSrc)
    {
      HTMainText->htmlSrcHead = NULL;
      return NULL;
    }

  /* Keep pointer to real head of htmlSrc memory block. */
  HTMainText->htmlSrcHead = HTMainText->htmlSrc;
  
  if (HTMainText->htmlSrc && HTMainText->srclen > 30)
    {
      if (!strncmp (HTMainText->htmlSrc, "<plaintext>", 11) ||
          !strncmp (HTMainText->htmlSrc, "<PLAINTEXT>", 11))
        {
          if (!strncmp (HTMainText->htmlSrc + 11, "<plaintext>", 11) ||
              !strncmp (HTMainText->htmlSrc + 11, "<PLAINTEXT>", 11))
            {
              HTMainText->htmlSrc += 11;
            }
          else if (!strncmp (HTMainText->htmlSrc + 11, "\n<plaintext>", 12) ||
                   !strncmp (HTMainText->htmlSrc + 11, "\n<PLAINTEXT>", 12))
            {
              HTMainText->htmlSrc += 12;
            }
          else if (!strncmp (HTMainText->htmlSrc + 11, "\n<title>", 8) ||
                   !strncmp (HTMainText->htmlSrc + 11, "\n<TITLE>", 8))
            {
              HTMainText->htmlSrc += 12;
            }
          else if (!strncmp (HTMainText->htmlSrc + 11, "\n<HEAD>", 7) ||
                   !strncmp (HTMainText->htmlSrc + 11, "\n<head>", 7) ||
                   !strncmp (HTMainText->htmlSrc + 11, "\n<HTML>", 7) ||
                   !strncmp (HTMainText->htmlSrc + 11, "\n<html>", 7) ||
                   !strncmp (HTMainText->htmlSrc + 11, "\n<BASE",  6) ||
                   !strncmp (HTMainText->htmlSrc + 11, "\n<base",  6))
            {
              HTMainText->htmlSrc += 12;
            }
        }
      if (!strncmp (HTMainText->htmlSrc, 
                    "<TITLE>Document</TITLE>\n<PLAINTEXT>", 35))
        {
          if (!strncmp (HTMainText->htmlSrc + 35, "\n<title>", 8) ||
              !strncmp (HTMainText->htmlSrc + 35, "\n<TITLE>", 8))
            {
              HTMainText->htmlSrc += 36;
            }
        }
    }
  return HTMainText->htmlSrc;
}
/****************************************************************************
 * name:    mo_pull_er_over
 * purpose: Given a URL, pull 'er over.
 * inputs:  
 *   - char       *url: The URL to pull over.
 *   - char **texthead: Return pointer to head of allocated block.
 * returns: 
 *   Text to display (char *).
 * remarks: 
 *   
 ****************************************************************************/
char *mo_pull_er_over (char *url, char **texthead)
{
  char *rv;

  if (binary_transfer)
    {
      force_dump_to_file = 1;
      force_dump_filename = mo_tmpnam();
    }
  rv = doit (url, texthead);
  if (binary_transfer)
    {
      force_dump_to_file = 0;
      force_dump_filename = NULL;
    }
  return rv;
}
/****************************************************************************
 * name:    doit (PRIVATE)
 * purpose: Given a URL, go fetch information.
 * inputs:  
 *   - char       *url: The URL to fetch.
 *   - char **texthead: Return pointer for the head of the allocated
 *                      text block (which may be different than the
 *                      return text intended for display).
 * returns: 
 *   The text intended for display (char *).
 ****************************************************************************/
static char *doit (char *url, char **texthead)
{
  char *msg;
  int rv;
  extern char *use_this_url_instead;

  /* Hmmmmmmm... */
  if (HTMainText)
    {
      free (HTMainText);
      HTMainText = NULL;
    }
  rv = HTLoadAbsolute (url);
  if (rv == 1)
    {
      char *txt = hack_htmlsrc ();
      if (HTMainText)
        *texthead = HTMainText->htmlSrcHead;
      else
        *texthead = NULL;
      return txt;
    }
  else if (rv == -1)
    {
      interrupted = 1;
      *texthead = NULL;
      return NULL;
    }

  /* Return proper error message if we experienced redirection. */
  if (use_this_url_instead) url = use_this_url_instead;
  msg = (char *)malloc ((strlen (url) + 200) * sizeof (char));
  sprintf (msg, 
"<H1>ERROR</H1> Requested document (URL %s) \
could not be accessed.<p>The information \
server either is not accessible or is refusing \
to serve the document to you.<p>", url);
  *texthead = msg;
  return msg;
}
/****************************************************************************
 * name:    mo_url_canonicalize
 * purpose: Turn a URL into its canonical form, based on the previous
 *          URL in this context (if appropriate).  
 *          INTERNAL ANCHORS ARE STRIPPED OFF.
 * inputs:  
 *   - char    *url: URL to canonicalize.
 *   - char *oldurl: The previous URL in this context.
 * returns: 
 *   The canonical representation of the URL.
 * remarks: 
 *   All we do is call HTParse.
 ****************************************************************************/
char *mo_url_canonicalize (char *url, char *oldurl)
{
  /* We LOSE anchor information. */
  return HTParse (url, oldurl,
                  PARSE_ACCESS | PARSE_HOST | PARSE_PATH |
                  PARSE_PUNCTUATION);
}
/****************************************************************************
 * name:    mo_url_canonicalize_keep_anchor
 * purpose: Turn a URL into its canonical form, based on the previous
 *          URL in this context (if appropriate).  
 *          INTERNAL ANCHORS ARE *NOT* STRIPPED OFF.
 * inputs:  
 *   - char    *url: URL to canonicalize.
 *   - char *oldurl: The previous URL in this context.
 * returns: 
 *   The canonical representation of the URL.
 * remarks: 
 *   All we do is call HTParse.
 ****************************************************************************/
char *mo_url_canonicalize_keep_anchor (char *url, char *oldurl)
{
  char *rv;
  /* We KEEP anchor information already present in url,
     but NOT in oldurl. */
  oldurl = HTParse (oldurl, "", PARSE_ACCESS | PARSE_HOST | PARSE_PATH |
                    PARSE_PUNCTUATION);
  rv = HTParse (url, oldurl,
                PARSE_ACCESS | PARSE_HOST | PARSE_PATH |
                PARSE_PUNCTUATION | PARSE_ANCHOR);
  /* We made a new copy of oldurl, so free the new copy. */
  free (oldurl);
  return rv;
}
/* Simply loop through a string and convert all newlines to spaces. */
/* We now remove leading whitespace as well */
char *mo_convert_newlines_to_spaces (char *str)
{
  int i,l;
  char *tptr;

  if (!str)
    return NULL;

  l =strlen(str);
  for (i = 0; i < l; i++)
    if (str[i] == '\n')
      str[i] = ' ';

  tptr = str;
  while ((*tptr != '\0')&&(isspace((int)(*tptr))))
	tptr++;

  if (tptr != str)
  {
	bcopy(tptr, str, (strlen(tptr) + 1));
  }

  return str;
}
/****************************************************************************
 * name:    mo_url_extract_anchor
 * purpose: Given a URL (presumably in canonical form), extract
 *          the internal anchor, if any.
 * inputs:  
 *   - char *url: URL to use.
 * returns: 
 *   Internal anchor, if one exists in the URL; else NULL.
 * remarks: 
 *   
 ****************************************************************************/
char *mo_url_extract_anchor (char *url)
{
  return HTParse (url, "", PARSE_ANCHOR);
}
/****************************************************************************
 * name:    mo_pull_er_over_virgin
 * purpose: Given a URL, pull 'er over in such a way that no format
 *          handling takes place and the data gets dumped in the filename
 *          of the calling routine's choice.
 * inputs:  
 *   - char  *url: The URL to pull over.
 *   - char *fnam: Filename in which to dump the received data.
 * returns: 
 *   mo_succeed on success; mo_fail otherwise.
 * remarks: 
 *   This routine is called when we know there's data out there we
 *   want to get and we know we just want it dumped in a file, no
 *   questions asked, by the WWW library.  Appropriate global flags
 *   are set to make this happen.
 *   This must be made cleaner.
 ****************************************************************************/
mo_status mo_pull_er_over_virgin (char *url, char *fnam)
{
  int rv;

  /* Force dump to file. */
  force_dump_to_file  = 1;
  force_dump_filename = fnam;

  rv = HTLoadAbsolute (url);
  if (rv == 1)
    {
      force_dump_to_file = 0;
      return mo_succeed;
    }
  else if (rv == -1)
    {
      force_dump_to_file = 0;
      interrupted = 1;
      return mo_fail;
    }
  else
    {
      force_dump_to_file = 0;
      return mo_fail;
    }
}
/****************************************************************************
 * name:    mo_cache_data
 * purpose: Cache a piece of data associated with a given URL.
 * inputs:  
 *   - char  *url: The URL.
 *   - void *info: The piece of data to cache (currently either
 *                 an ImageInfo struct for an image named as SRC
 *                 in an IMG tag, or the filename corresponding to the
 *                 local copy of a remote HDF file).
 *   - int   type: The type of data to cache (currently either
 *                 0 for an ImageInfo struct or 1 for a local name).
 * returns: 
 *   mo_succeed, unless something goes badly wrong
 * remarks: 
 *   We do *not* do anything to the URL.  If there is a target
 *   anchor in it, fine with us.  This means the target anchor
 *   should have been stripped out someplace else if it needed to be.
 ****************************************************************************/
mo_status mo_cache_data (char *url, void *info, int type)
/*barrand: image caching system not imported.*/
{
  url    = NULL;
  info   = NULL;
  type   = 0;
  return mo_fail;
}
/****************************************************************************
 * name:    mo_fetch_cached_image_data
 * purpose: Retrieve a piece of cached data associated with a URL.
 * inputs:  
 *   - char *url: The URL.
 * returns: 
 *   The piece of cached data (void *).
 * remarks: 
 *   We do *not* do anything to the URL.  If there is a target
 *   anchor in it, fine with us.  This means the target anchor
 *   should have been stripped out someplace else if it needed to be.
 ****************************************************************************/
void* mo_fetch_cached_image_data (char *url)
/*barrand: image caching system not imported.*/
{
  url    = NULL;
  return NULL;
}
/* Touched code. */
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void mo_gui_done_with_icon (void)
{
}
int mo_here_we_are_son (char *url)
{
  url = 0;
  return 1;
}
void application_user_feed (char *str)
{
  str = 0;
  CWarn ("application_user_feed: dummy.\n"); 
}

#else /*HAS_HTML*/

#include <CPrinter.h>
#include <mosaic.h>
int   interrupted          = 0;
char* mo_pull_er_over (char *url, char **texthead)
{
  CWarn    ("mo_pull_er_over: dummy.\n");
  url      = 0;
  texthead = 0;
  return   0;
}
char* mo_url_canonicalize_keep_anchor (char *url, char *oldurl)
{
  CWarn    ("mo_url_canonicalize_keep_anchor: dummy.\n");
  url    = 0;
  oldurl = 0;
  return 0;
}
char* mo_url_canonicalize (char *url, char *oldurl)
{
  CWarn    ("mo_url_canonicalize: dummy.\n");
  url    = 0;
  oldurl = 0;
  return 0;
}
char* mo_convert_newlines_to_spaces (char *str)
{
  CWarn  ("mo_convert_newlines_to_spaces: dummy.\n");
  str    = 0;
  return 0;
}
char* mo_url_extract_anchor (char *url)
{
  CWarn    ("mo_url_extract_anchor: dummy.\n");
  url    = 0;
  return 0;
}
mo_status mo_pull_er_over_virgin (char *url, char *fnam)
{
  CWarn  ("mo_pull_er_over_virgin: dummy.\n");
  url    = NULL;
  fnam   = NULL;
  return 0;
}
mo_status mo_cache_data (char *url, void *info, int type)
{
  CWarn  ("mo_cache_data: dummy.\n");
  url    = NULL;
  info   = NULL;
  type   = 0;
  return mo_fail;
}
void* mo_fetch_cached_image_data (char *url)
{
  CWarn  ("mo_fetch_cached_image_data: dummy.\n");
  url    = NULL;
  return NULL;
}
void mo_gui_done_with_icon (void)
{
  CWarn    ("mo_gui_done_with_icon: dummy.\n");
}
int mo_here_we_are_son (char *url)
{
  CWarn    ("mo_here_we_are_son: dummy.\n");
  url = 0;
  return 1;
}
#endif /*HAS_HTML*/


#else 
#include <stddef.h>
#endif /*HAS_MOSAIC*/
