/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OLUT_h
#define OLUT_h
 
typedef struct _OLUT_Record *OLUT;

#define OLUT_Identity  0
#define OLUT_Linear 1
#define OLUT_Log    2

#ifdef __cplusplus
extern "C"{
#endif
void              OLUT_ClearClass      ();
OLUT*             OLUT_GetIdentifiers  ();
OLUT              OLUT_GetIdentifier   (char*);
OLUT              OLUT_Make            ();
OLUT              OLUT_Create          (char*,int);
void              OLUT_Delete          (OLUT);
int               OLUT_IsValid         (OLUT);
char*             OLUT_GetName         (OLUT);
void              OLUT_SetName         (OLUT,char*);
void              OLUT_Initialize      (OLUT,int,int,int,int);
unsigned char*    OLUT_GetList         (OLUT);
int               OLUT_GetSize         (OLUT);
int               OLUT_GetMinimum      (OLUT);
int               OLUT_GetMaximum      (OLUT);
int               OLUT_GetNumberOfBins (OLUT);
int               OLUT_GetType         (OLUT);
void              OLUT_SetType         (OLUT,int);
void              OLUT_SetMinimum      (OLUT,int);
void              OLUT_SetMaximum      (OLUT,int);
void              OLUT_SetNumberOfBins (OLUT,int);
void              OLUT_SetSize         (OLUT,int);
int               OLUT_SetAttribute    (OLUT,char*,void*,void*,int);
int               OLUT_GetAttribute    (OLUT,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOLUT(0); /**/
#endif

#endif  /*OLUT_h*/
