/*              Plain text object                       HTPlain.h
**              -----------------
**
**
*/

#ifndef HTPLAIN_H
#define HTPLAIN_H

#include "HTStream.h"
#include "HTAnchor.h"

#ifdef __cplusplus
extern "C"{
#endif
HTStream* HTPlainPresent PARAMS((
        HTPresentation *        pres,
        HTParentAnchor *        anchor,
        HTStream *              sink, 
        HTFormat                format_in,
        int                     compressed));
#ifdef __cplusplus
}
#endif

#endif
