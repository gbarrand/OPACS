/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef GF77_h
#define GF77_h

#ifdef __CI_SPECIFIC__
#define HAS_NOT_U
#endif

/* C calling f77 */
#ifndef HAS_NOT_U
/*PACKLIB*/
#define uhtoc  uhtoc_
/* GEANT */
#define grun   grun_
#define gtrigc gtrigc_
/* GEANG */
#define gpvolu gpvolu_
#define gsatt  gsatt_
#define gfatt  gfatt_
#define gdnson gdnson_
#define gdson  gdson_
#define gfmoth gfmoth_
#define gphits gphits_
#define gpdigi gpdigi_
#define gfnhit gfnhit_
#define gfndig gfndig_
/* ogeant.f */
#define oginit oginit_
#define ogrgeo ogrgeo_
#define ogwgeo ogwgeo_
#define ogrevt ogrevt_
#define ogvolt ogvolt_
#define ogevbg ogevbg_
#define ogvonm ogvonm_
#define ogmoth ogmoth_
#define ogvoln ogvoln_
#define ogvolv ogvolv_
#define ogvold ogvold_
#define ogkinn ogkinn_
#define ogkinv ogkinv_
#define ogkind ogkind_
#define oghitv oghitv_
#define ogdetn ogdetn_
#define ogdetv ogdetv_
#define ogdigv ogdigv_
#define ogdeto ogdeto_
#define ogvern ogvern_
#define ogverv ogverv_
#define ogverd ogverd_
#endif

#ifdef __cplusplus
extern "C"{
#endif
/*PACKLIB*/
void  uhtoc  (int*,int*,void*,int*);
/* GEANT */
void  grun    ();
void  gtrigc  ();
void  gphits  (void*,void*);
void  gpdigi  (void*,void*);
/* GEANG */
void  gpvolu (int*);
void  gsatt  (void*,void*,int*);
void  gfatt  (void*,void*,int*);
void  gdnson (void*,int*,int*);
void  gdson  (int*,void*,void*);
void  gfmoth (void*,void*,int*);
void  gfnhit (void*,void*,int*);
void  gfndig (void*,void*,int*);
/* ogeantt.f */
void  oginit ();
void  ogrgeo (int*,void*,int*,int*);
void  ogwgeo (int*,void*,int*,int*);
int   ogvolt (void*);
int   ogevbg ();
void  ogmoth (void*,int*,int*,int*);
void  ogvonm (int*,void*);
int   ogvoln ();
void  ogvolv (int*,int*,int*,float*,void*,float*);
int   ogvold (void*,int*,int*,int*);
int   ogkinn ();
void  ogkinv (int*,int*,int*,float*,void*);
void  ogkind (int*);
int   oghitv (void*,void*,int*,int*,int*,int*,float*,int*);
int   ogdetn ();
void  ogdetv (int*,int*,int*,float*,void*,int*);
int   ogdigv (void*,void*,int*,int*,int*,int*,int*);
void  ogdeto (void*,void*,int*,int*);
int   ogvern ();
void  ogverv (int*,int*,int*,float*,void*);
void  ogverd (int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindGF77(0); /**/
#endif

#endif  /*GF77_h*/











