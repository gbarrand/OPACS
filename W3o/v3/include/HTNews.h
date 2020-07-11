/*                                  Network News Transfer protocol module for the WWW library
                                          HTNEWS
                                             
 */
/* History:
**      26 Sep 90       Written TBL in Objective-C
**      29 Nov 91       Downgraded to C, for portable implementation.
*/

#ifndef HTNEWS_H
#define HTNEWS_H

#include "HTAccess.h"
#include "HTAnchor.h"

extern HTProtocol HTNews;
extern char* HTNewsHost;

#ifdef __cplusplus
extern "C"{
#endif
void HTSetNewsHost PARAMS((CONST char *value));
CONST char * HTGetNewsHost NOPARAMS;
#ifdef __cplusplus
}
#endif

#endif /* HTNEWS_H */
