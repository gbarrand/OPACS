/*		Plain text object		HTWrite.c
**		=================
**
**	This version of the stream object just writes to a socket.
**	The socket is assumed open and left open.
**
**	Bugs:
**		strings written must be less than buffer size.
*/
#include <stdlib.h>

#include "HTPlain.h"
#include "HTString.h"
#include "HTUtils.h"
#include "HText.h"
#include "HTFile.h"
#include "HTCompressed.h"

/*		HTML Object
**		-----------
*/

struct _HTStream {
	CONST HTStreamClass *	isa;

	HText * 		text;
        int compressed;
};

/* #define TRACE 1 */

/*_________________________________________________________________________
**
**			A C T I O N 	R O U T I N E S
*/

/*	Character handling
**	------------------
*/

PRIVATE void HTPlain_put_character ARGS2(HTStream *, me, char, c)
{
    HText_appendCharacter(me->text, c);
}



/*	String handling
**	---------------
**
*/
PRIVATE void HTPlain_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    HText_appendText(me->text, s);
}


PRIVATE void HTPlain_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    HText_appendBlock (me->text, s, l);
}



/*	Free an HTML object
**	-------------------
**
**	Note that the SGML parsing context is freed, but the created object is not,
**	as it takes on an existence of its own unless explicitly freed.
*/
PRIVATE void HTPlain_free ARGS1(HTStream *, me)
{
  if (me->compressed != COMPRESSED_NOT)
    {
      if (TRACE)
        fprintf 
          (stderr, 
           "[HTPlain_free] OK, we're going to decompress HText\n");
      HTCompressedHText (me->text, me->compressed, 1);
    }

  free(me);
}

/*	End writing
*/

PRIVATE void HTPlain_end_document ARGS1(HTStream *, me)
{
  HText_endAppend(me->text);
}

PRIVATE void HTPlain_handle_interrupt ARGS1(HTStream *, me)
{
  HText_doAbort(me->text);
}



/*		Structured Object Class
**		-----------------------
*/
PUBLIC CONST HTStreamClass HTPlain =
{		
	"SocketWriter",
	HTPlain_free,
	HTPlain_end_document,
	HTPlain_put_character, 	HTPlain_put_string, HTPlain_write,
        HTPlain_handle_interrupt
}; 


/*		New object
**		----------
*/
PUBLIC HTStream* HTPlainPresent ARGS5(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,	
	HTStream *,		sink,
        HTFormat,               format_in,
        int,                    compressed)
{
  HTStream* me = (HTStream*)malloc(sizeof(*me));
  me->isa = &HTPlain;       
  
  if (TRACE)
    fprintf (stderr, "[HTPlainPresent] here we are; format_in is '%s' and compressed is %d\n", HTAtom_name (format_in), compressed);
  
  me->text = HText_new();
  me->compressed = compressed;

  HText_beginAppend(me->text);
  if (me->compressed == COMPRESSED_NOT)
    HText_appendText(me->text, "<PLAINTEXT>\n");

  pres = NULL;
  anchor = NULL;
  sink = NULL;
  return (HTStream*) me;
}
