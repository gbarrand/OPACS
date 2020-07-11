/*  */

/*              HTML generator
*/

#ifndef HTMLGEN_H
#define HTMLGEN_H

#include "HTMLNew.h"
#include "HTStream.h"

#ifdef __cplusplus
extern "C"{
#endif
/* Subclass:
*/
/* extern CONST HTStructuredClass HTMLGeneration; */

/* Special Creation:
*/
HTStructured * HTMLGenerator PARAMS((HTStream * output));

HTStream * HTPlainToHTML PARAMS((
        HTPresentation *        pres,
        HTParentAnchor *        anchor,
        HTStream *              sink,
        HTFormat                format_in,
        int                     compressed));
#ifdef __cplusplus
}
#endif


#endif
