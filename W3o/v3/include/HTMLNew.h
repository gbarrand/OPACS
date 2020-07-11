#ifndef HTMLNew_H
#define HTMLNew_H

#include "HTUtils.h"
#include "HTAnchor.h"
#include "HTMLDTD.h"

#ifdef SHORT_NAMES
#define HTMLPresentation        HTMLPren
#endif

extern CONST HTStructuredClass HTMLPresentation;

/*      Names for selected internal representations:
*/
typedef enum _HTMLCharacterSet {
        HTML_ISO_LATIN1,
        HTML_NEXT_CHARS,
        HTML_PC_CP950
} HTMLCharacterSet;

#ifdef __cplusplus
extern "C"{
#endif
HTStructured* HTML_new PARAMS((
        HTParentAnchor * anchor,
        HTFormat        format_out,
        HTStream *      target));

void HTMLUseCharacterSet PARAMS((HTMLCharacterSet i));

/*

Record error message as a hypertext object

   The error message should be marked as an error so that it can be
   reloaded later. This implementation just throws up an error message
   and leaves the document unloaded.
   
 */
/* On entry,
**      sink    is a stream to the output device if any
**      number  is the HTTP error number
**      message is the human readable message.
** On exit,
**      a retrun code like HT_LOADED if object exists else 60; 0
*/

int HTLoadError PARAMS((
        HTStream *      sink,
        int             number,
        CONST char *    message));
#ifdef __cplusplus
}
#endif

#endif
