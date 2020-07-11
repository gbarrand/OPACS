#ifndef XPMHASH_h
#define XPMHASH_h

#include <xpmhtml.h>

/*Barrand: file introduced to build AIX sheared libraries.*/

#ifdef __cplusplus /*barrand: __cplusplus*/
extern "C" {
#endif
extern int          xpmHashTableInit (xpmHashTable * table);
extern void         xpmHashTableFree (xpmHashTable * table);
extern xpmHashAtom* xpmHashSlot      (xpmHashTable * table, char *s);
extern int          xpmHashIntern    (xpmHashTable * table, char *tag, void *data);
#ifdef __cplusplus
}
#endif

#endif
