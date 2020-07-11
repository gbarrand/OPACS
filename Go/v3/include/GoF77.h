/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef GoF77_h
#define GoF77_h

#ifdef __CI_SPECIFIC__
#define HAS_NOT_U
#endif

#ifndef HAS_NOT_U
#define goaraz goaraz_
#define goaset goaset_
#define gocset gocset_
#define goocre goocre_
#define goview goview_

#define gopgon gopgon_
#define goplin goplin_
#define gopmar gopmar_
#define gopseg gopseg_
#define goptxt goptxt_

#define gogshp gogshp_
#define goglin goglin_
#endif

#include <FString.h>
#include <ONode.h>
#ifdef __cplusplus
extern "C"{
#endif
void GoF77_ClearClass ();
void GoF77_SetNode    (ONode);

/*F77*/
void goaraz ();
void goaset (int*,int*,int*,int*,int*,int*);
void gocset (FString,int*);
void goocre (FString,int*);
void goview ();
void gopgon (int*,float*,float*,float*);
void goplin (int*,float*,float*,float*);
void gopmar (int*,float*,float*,float*);
void gopseg (int*,float*,float*,float*);
void goptxt (FString,int*,float*,float*,float*,float*,float*);
void gogshp (int*,float*,float*,float*);
void goglin (int*,float*);
/*F77*/
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindGoF77(0); /**/
#endif

#endif /*GoF77_h*/
