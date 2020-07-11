/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OColormap_h
#define OColormap_h
 
typedef struct _OColormapRecord  *OColormap;

#ifdef __cplusplus
extern "C"{
#endif
void           OColormapClearClass              ();
OColormap*     OColormapGetIdentifiers          ();
char**         OColormapGetNames                (int*);
OColormap      OColormapGetIdentifier           (char*);
OColormap      OColormapGetIdentifierFromString (char*);
OColormap      OColormapGetIdentifierFromRGBs   (int,double*,double*,double*);
OColormap      OColormapMake                    ();
OColormap      OColormapCreate                  (char*,int,double*,double*,double*);
void           OColormapSetName                 (OColormap,char*);
char*          OColormapGetName                 (OColormap);
void           OColormapDelete                  (OColormap);
int            OColormapIsValid                 (OColormap);
int            OColormapGetSize                 (OColormap);
void           OColormapGetRGBs                 (OColormap,int*,double**,double**,double**);
void           OColormapDump                    (OColormap);
void           OColormapSetIthRGB               (OColormap,int,double,double,double);
void           OColormapGetIthRGB               (OColormap,int,double*,double*,double*);
int            OColormapGetRGB_Index            (OColormap,double,double,double);
void           OColormapSetColorNames           (OColormap,int,char**);
int            OColormapGetColorNameIndex       (OColormap,char*);
char*          OColormapGetIthColorName         (OColormap,int);
int            OColormapGetValueColorMapping    (OColormap,char*,double**,int**);
int            OColormapSetAttribute            (OColormap,char*,void*,void*,int);
int            OColormapGetAttribute            (OColormap,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOColormap(0); /**/
#endif

#endif  /*OColormap_h*/
