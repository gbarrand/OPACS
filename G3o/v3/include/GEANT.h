/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef GEANT_h
#define GEANT_h

#define GVOL_VIS_SON_VIS       1        /*Default*/
#define GVOL_INV_SON_VIS       0
#define GVOL_INV_SON_INV      -1
#define GVOL_VIS_SON_INV      -2

#define GFILE_RZ              10
#define GFILE_RA              11
#define GFILE_RX              12
#define GFILE_FZ              20
#define GFILE_FA              21
#define GFILE_FX              22

#ifdef __cplusplus
extern "C"{
#endif
void   GEANT_SetInitialized            ();
void   GEANT_Initialize                ();
void   GEANT_LoadGeometry              (int,char*,int);
void   GEANT_SaveGeometry              (int,char*,int);
void   GEANT_ReadEvent                 (int);
int    GEANT_ProduceEvent              ();
void   GEANT_ClearEvent                ();
char** GEANT_GetVolumeNames            (int*);
void   GEANT_PrintVolumes              ();
char*  GEANT_GetTopVolume              ();
int    GEANT_GetVolumeNumberOfSons     (char*);
char*  GEANT_GetVolumeIthSon           (char*,int);
char** GEANT_GetVolumeUpperVolumes     (char*,int*);
void   GEANT_SetVolumeVisibility       (char*);
void   GEANT_SetVolumeDrawingAttribute (char*,char*,int);
int    GEANT_GetVolumeDrawingAttribute (char*,char*);
int    GEANT_RepresentVolume           (char*,int,int*,int*);
int    GEANT_GetDetectorNumberOfHits   (char*,char*);
int    GEANT_GetDetectorIthHit         (char*,char*,int,int*,int*,int*,float*);
void   GEANT_GetDetectorVolumes        (char*,char*,int*,int*);
int    GEANT_GetDetectorNumberOfDIGIs  (char*,char*);
int    GEANT_GetDetectorIthDIGI        (char*,char*,int,int*,int*,int*,int*);
void   GEANT_RepresentTrack            (int);
void   GEANT_RepresentVertex           (int);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindGEANT(0); /**/
#endif

#endif  /*GEANT_h*/
