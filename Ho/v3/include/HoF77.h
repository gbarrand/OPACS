/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef HoF77_h
#define HoF77_h

#ifndef HAS_NOT_U
#define ohcurr ohcurr_
#define oh1cre oh1cre_
#define oh1fil oh1fil_
#define ohdump ohdump_
#define ohdele ohdele_
#define oscurr oscurr_
#define os2cre os2cre_
#define os2fil os2fil_
#define osallo osallo_
#define osdump osdump_
#define osdele osdele_
#endif

#include <FString.h>
#ifdef __cplusplus
extern "C"{
#endif
/*F77*/
void ohcurr (FString,int*);
void oh1cre (FString,int*,FString,int*,int*,float*,float*);
void oh1fil (float*,float*);
void ohdump ();
void ohdele ();
void oscurr (FString,int*);
void os2cre (FString,int*,FString,int*,float*,float*,FString,int*,float*,float*);
void os2fil (float*,float*);
void osallo (int*);
void osdump ();
void osdele ();
/*F77*/
#ifdef __cplusplus
}
#endif

#endif /*HoF77_h*/
