/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OTouch_h
#define OTouch_h

typedef struct _OTouchRecord*  OTouch;

#ifdef __cplusplus
extern "C"{
#endif
void      OTouchClearClass        ();
OTouch*   OTouchGetIdentifiers    ();
OTouch    OTouchMake              ();
void      OTouchDelete            (OTouch);
void      OTouchDump              (OTouch);
char*     OTouchGetDate           (OTouch);
char*     OTouchGetType           (OTouch);
char*     OTouchGetComment        (OTouch);
char*     OTouchGetConfigure      (OTouch);
char*     OTouchGetLibs           (OTouch);
char*     OTouchGetApps           (OTouch);
char*     OTouchGetIfapp          (OTouch);
char**    OTouchGetRemove         (OTouch,int*);
char**    OTouchGetFiles          (OTouch,int*);
int       OTouchSetAttribute      (OTouch,char*,void*,void*,int);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOTouch(0); /**/
#endif

#endif  /*OTouch_h*/



