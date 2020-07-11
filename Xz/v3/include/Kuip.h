/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef Kuip_h
#define Kuip_h

#ifdef __CI_SPECIFIC__
#define HAS_NOT_U
#endif

/* C calling f77 */
#ifndef HAS_NOT_U
#define okinit okinit_
#define okited okited_
#endif

#ifdef __cplusplus
extern "C"{
#endif
/*KUIP*/
char*  ku_getc();
char*  ku_gete();
char*  ku_getf();
char*  ku_getl();
char*  ku_gets();
int    ku_geti();
double ku_getr();
int    ku_exec(char*);

/*okuip.f*/
void   okinit (int*);
int    okited ();

/*Kuip.c*/
void   KuipInitialize    (int);
void   KuipExecuteScript (char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindKuip(0); /**/
#endif

#endif  /*Kuip_h*/





