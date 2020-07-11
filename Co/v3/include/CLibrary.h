/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CLibrary_h
#define CLibrary_h

#ifdef __cplusplus
extern "C"{
#endif
void*    CLibraryOpen      (char*);
void*    CLibraryGetSymbol (void*,char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCLibrary(0); /**/
#endif

#endif  /*CLibrary_h*/
