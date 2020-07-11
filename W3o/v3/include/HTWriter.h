/*  */

/*              Unix File or Socket Writer                      HTWriter.c
**              --------------------------
**
**      This version of the stream object just writes to a socket.
**      The socket is assumed open and closed afterward.
**
**      There are two versions (identical on ASCII machines)
**      one of which converts to ASCII on output.
**
**      Bugs:
**              strings written must be less than buffer size.
*/

#ifndef HTWRITE_H
#define HTWRITE_H

#include "HTStream.h"

#ifdef __cplusplus
extern "C"{
#endif
HTStream * HTWriter_new PARAMS((int soc));
HTStream * HTASCIIWriter PARAMS((int soc));
#ifdef __cplusplus
}
#endif

#endif
