/*                                                                 String handling for libwww
                                         STRINGS
                                             
   Case-independent string comparison and allocations with copies etc
   
 */
#ifndef HTSTRING_H
#define HTSTRING_H

#include "HTUtils.h"

extern int WWW_TraceFlag;       /* Global flag for all W3 trace */

extern CONST char * HTLibraryVersion;   /* String for help screen etc */

#ifdef __cplusplus
extern "C"{
#endif
/*

Case-insensitive string comparison

   The usual routines (comp instead of cmp) had some problem.
   
 */
int strcasecomp  PARAMS((CONST char *a, CONST char *b));
int strncasecomp PARAMS((CONST char *a, CONST char *b, int n));

/*

Malloced string manipulation

 */
#define StrAllocCopy(dest, src) HTSACopy (&(dest), src)
#define StrAllocCat(dest, src)  HTSACat  (&(dest), src)
char * HTSACopy PARAMS((char **dest, CONST char *src));
char * HTSACat  PARAMS((char **dest, CONST char *src));

/*

Next word or quoted string

 */
char * HTNextField PARAMS((char** pstr));
#ifdef __cplusplus
}
#endif


#endif
