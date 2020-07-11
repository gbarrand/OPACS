/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OMatrix33_h
#define OMatrix33_h

typedef struct _OMatrix33Record *OMatrix33;

#ifdef __cplusplus
extern "C"{
#endif
void       OMatrix33ClearClass     ();
OMatrix33* OMatrix33GetIdentifiers ();
OMatrix33  OMatrix33GetIdentifier  (char*);
OMatrix33  OMatrix33Make           ();
OMatrix33  OMatrix33Create         (char*);
void       OMatrix33Delete         (OMatrix33);
char*      OMatrix33GetName        (OMatrix33);
void       OMatrix33SetName        (OMatrix33,char*);
void       OMatrix33Initialize     (OMatrix33,int,int,int,int,int,int,int,int,int,int);
int*       OMatrix33GetElements    (OMatrix33);
int        OMatrix33GetScale       (OMatrix33);
int        OMatrix33SetAttribute   (OMatrix33,char*,void*,void*,int);
int        OMatrix33GetAttribute   (OMatrix33,char*,void*,void*,int*);
int        OMatrix33DoMethod       (OMatrix33,char*,void*,int,char**,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOMatrix33(0); /**/
#endif

#endif  /*OMatrix33_h*/
