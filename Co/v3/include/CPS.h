/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CPS_h
#define CPS_h

typedef int(*CPS_GetRGB_Function)(unsigned int,unsigned int,double*,double*,double*);

#define CPS_Color      0
#define CPS_Grey       1
#define CPS_BlackWhite 2

#ifdef __cplusplus
extern "C"{
#endif
void  CPS_ClearClass              ();
void  CPS_SetColorScheme          (int);
void  CPS_SetOrientation          (int);
void  CPS_SetBackgroundDrawn      (int);
void  CPS_SetBitsPerPixel         (int);
void  CPS_SetLineWidth            (int);
void* CPS_GetStream               ();
char* CPS_GetFileName             ();
void  CPS_OpenFileForWriting      (char*);
void  CPS_CloseStream             ();
void  CPS_PutInStreamF            (char*,...);
void  CPS_PutPageScaleInStream    (double,double);
void  CPS_PutImageInStream        (unsigned int,unsigned int,CPS_GetRGB_Function);
void  CPS_PutSaveStateInStream    ();
void  CPS_PutRestoreStateInStream ();
void  CPS_PutTranslationInStream  (double,double);
void  CPS_PutScaleInStream        (double,double);
void  CPS_PutBeginPageInStream    ();
void  CPS_PutEndPageInStream      ();
void  CPS_PutRGB_InStream         (double,double,double);
void  CPS_PutLineWidthInStream    (int);
void  CPS_PutMarkerSizeInStream   (double);
void  CPS_PutBackgroundInStream   (double,double,double,double,double);
void  CPS_PutFrameInStream        (double,double,double,double,double);
int   CPS_SetAttribute            (void*,char*,void*,void*,int);
int   CPS_GetAttribute            (void*,char*,void*,void*,int*);
int   CPS_DoMethod                (void*,char*,void*,int,char**,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCPS(0); /**/
#endif

#endif  /*CPS_h*/
