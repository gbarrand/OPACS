/*              MosaicHTML text object                       HTMosaicHTML.h
**              -----------------
**
**
*/

#ifndef HTMosaicHTML_H
#define HTMosaicHTML_H

#include "HTStream.h"
#include "HTAnchor.h"

#ifdef __cplusplus
extern "C"{
#endif
HTStream* HTMosaicHTMLPresent PARAMS((
        HTPresentation *        pres,
        HTParentAnchor *        anchor,
        HTStream *              sink, 
        HTFormat                format_in,
        int                     compressed));
#ifdef __cplusplus
}
#endif

#endif
