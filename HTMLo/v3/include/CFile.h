#ifndef CFile_h
#define CFile_h

#ifndef NO_C_H
#include <stdio.h>   
#endif

#ifdef __cplusplus
extern "C"{
#endif
FILE*           CFileOpen            (char*,char**);
char*           CFileRead            (FILE*,unsigned long*);
long            CFileGetSize         (FILE*);
#ifdef __cplusplus
}
#endif


#endif  /*CFile_h*/



