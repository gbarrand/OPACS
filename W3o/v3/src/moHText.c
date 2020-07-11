/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define bcopy_(source,dest,count)  memcpy(dest,source,count)   /*Barrand*/
#define CWarn  printf

#include <moHTextP.h>

typedef char HTBoolean;                /* Logical value */

#ifdef __cplusplus
extern "C"{
#endif
HText* HText_new (void);
void   HText_free (HText*);
void   HText_beginAppend (HText*);
void   HText_endAppend (HText*);
void   HText_doAbort (HText*);
void   HText_clearOutForNewContents (HText *);
void   HText_appendCharacter (HText*,char);
void   HText_appendText (HText*,char*);
void   HText_appendBlock (HText*,char*,int);
void   HText_appendParagraph (HText*);
void   HText_beginAnchor (HText*,char*);
void   HText_endAnchor (HText*);
void   HText_dump (HText*);
char*  HText_getText (HText *);
char** HText_getPtrToText (HText*);
int    HText_getTextLength (HText*);
HTBoolean   HText_select (HText*);
static void new_chunk (HText *);
#ifdef __cplusplus
}
#endif

#define MO_BUFFER_SIZE 8192

HText* HTMainText = NULL;                  /* Equivalent of main window */
/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
HText *HText_new (void)
{
  HText *htObj = (HText *)malloc (sizeof (HText));

  htObj->expandedAddress = NULL;
  htObj->simpleAddress = NULL;
  htObj->htmlSrc = NULL;
  htObj->htmlSrcHead = NULL;
  htObj->srcalloc = 0;
  htObj->srclen = 0;

  /* Free the struct but not the text, as it will be handled
     by Mosaic proper -- apparently. */
  if (HTMainText)
    free (HTMainText);

  HTMainText = htObj;

  return htObj;
}

void HText_free (HText *self)
{
  CWarn ("debug: HText_free.\n");
  if (self)
    {
      if (self->htmlSrcHead)
        free (self->htmlSrcHead);
      free (self);
    }
  return;
}

void HText_beginAppend (HText *text)
{
  HTMainText = text;
  return;
}

void HText_endAppend (HText *text)
{
  if (text)
    {
      HText_appendCharacter (text, '\0');
    }
  HTMainText = text;
  return;
}

void HText_doAbort (HText *self)
{
  /* Clean up -- we want to free htmlSrc here because htmlSrcHead
     doesn't get assigned until hack_htmlsrc, and by the time we
     reach that, this should never be called. */
  if (self)
    {
      if (self->htmlSrc)
        free (self->htmlSrc);
      self->htmlSrc = NULL;
      self->htmlSrcHead = NULL;
      self->srcalloc = 0;
      self->srclen = 0;
    }
  return;
}

void HText_clearOutForNewContents (HText *self)
{
  if (self)
    {
      if (self->htmlSrc)
        free (self->htmlSrc);
      self->htmlSrc = NULL;
      self->htmlSrcHead = NULL;
      self->srcalloc = 0;
      self->srclen = 0;
    }
  return;
}

void HText_appendCharacter (HText *text, char ch)
{
  if (text->srcalloc < text->srclen + 1)
    new_chunk (text);
  
  text->htmlSrc[text->srclen++] = ch;

  return;
}

void HText_appendText (HText *text, char *str)
{
  int len;

  if (!str)
    return;

  len = strlen (str);

  while (text->srcalloc < text->srclen + len + 1)
    new_chunk (text);

  bcopy_ (str, (text->htmlSrc + text->srclen), len);

  text->srclen += len;
  text->htmlSrc[text->srclen] = '\0';

  return;
}

void HText_appendBlock (HText *text, char *data, int len)
{
  if (!data)
    return;
  
  while (text->srcalloc < text->srclen + len + 1)
    new_chunk (text);

  bcopy_ (data, (text->htmlSrc + text->srclen), len);

  text->srclen += len;
  text->htmlSrc[text->srclen] = '\0';

  return;
}

void HText_appendParagraph (HText *text)
{
  /* Boy, talk about a misnamed function. */
  char *str = " <p> \n";

  HText_appendText (text, str);

  return;
}

void HText_beginAnchor (HText *text, char *anc)
{
  HText_appendText (text, "<A HREF=\"");
  HText_appendText (text, anc);
  HText_appendText (text, "\">");
  return;
}

void HText_endAnchor (HText * text)
{
  HText_appendText (text, "</A>");
  return;
}

void HText_dump (HText*me)
{
  me = 0;
  return;
}

char *HText_getText (HText *me)
{
  if (me)
    return me->htmlSrc;
  else
    return NULL;
}

char **HText_getPtrToText (HText *me)
{
  if (me)
    return &(me->htmlSrc);
  else
    return NULL;
}

int HText_getTextLength (HText *me)
{
  if (me)
    return me->srclen;
  else
    return 0;
}

HTBoolean HText_select (HText *text)
{
  text = 0;
  return 0;
}

static void new_chunk (HText *text)
{
  if (text->srcalloc == 0)
    {
      text->htmlSrc = (char *)malloc (MO_BUFFER_SIZE);
      text->htmlSrc[0] = '\0';
    }
  else
    {
      text->htmlSrc = (char *)realloc
        (text->htmlSrc, text->srcalloc + MO_BUFFER_SIZE);
    }

  text->srcalloc += MO_BUFFER_SIZE;

  return;
}
