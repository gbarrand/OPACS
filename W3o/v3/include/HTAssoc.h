/*                       ASSOCIATION LIST FOR STORING NAME-VALUE PAIRS
                                             
   Lookups from assosiation list are not case-sensitive.
   
 */

#ifndef HTASSOC_H
#define HTASSOC_H

#include "HTUtils.h"
#include "HTList.h"


#ifdef SHORT_NAMES
#define HTAL_new        HTAssocList_new
#define HTAL_del        HTAssocList_delete
#define HTAL_add        HTAssocList_add
#define HTAL_lup        HTAssocList_lookup
#endif /*SHORT_NAMES*/

typedef HTList HTAssocList;

typedef struct {
    char * name;
    char * value;
} HTAssoc;


#ifdef __cplusplus
extern "C"{
#endif
HTAssocList *HTAssocList_new NOPARAMS;
void HTAssocList_delete PARAMS((HTAssocList * alist));

void HTAssocList_add PARAMS((HTAssocList *       alist,
                                    CONST char *        name,
                                    CONST char *        value));

char *HTAssocList_lookup PARAMS((HTAssocList *   alist,
                                        CONST char *    name));
#ifdef __cplusplus
}
#endif

#endif /* not HTASSOC_H */
/*

   End of file HTAssoc.h.  */





