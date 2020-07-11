/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <string.h>

#include <CMemory.h>
#include <CFile.h>
#include <CString.h>
#include <CText.h>
#include <FString.h>

#include <GF77.h>

#include <GEANT.h>

#ifdef __cplusplus
extern "C"{
#endif
static void EnableMothers (char*);
#ifdef __cplusplus
}
#endif

static struct {
  int   init;
} Class = {0};
/***************************************************************************/
void GEANT_SetInitialized (
)
/***************************************************************************/
/* To be able to disconnect the G3o GEANT initialization.                  */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.init = 1;
}
/***************************************************************************/
void GEANT_Initialize (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.init==1) return;
  oginit      ();
  Class.init = 1;
}
/***************************************************************************/
void GEANT_LoadGeometry (
 int   a_format
,char* a_fname
,int   a_unit
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  file;
  char*  fname;
  FString   fstr;
  int    lfstr;
  int    fmt;
  int    unit;
/*.........................................................................*/
  if(a_fname==NULL)    return;

  file                 = CFileOpenForReading(a_fname,&fname);
  if(file==NULL)       return;
  fclose               (file);

  fmt                  = a_format;
  unit                 = a_unit;

  if( (a_format==GFILE_FZ) ||
      (a_format==GFILE_FA) ||
      (a_format==GFILE_FX)
     )
    {
      CFileBindToF77_Unit (fname,unit);
    }
  else if( (a_format==GFILE_RZ) ||
	   (a_format==GFILE_RA) ||
	   (a_format==GFILE_RX)
	  )
    {
      CFileLink        (&fname);
    }

  GEANT_Initialize     ();
  fstr                 = FStringCreateFromC (fname,&lfstr);
  ogrgeo               (&fmt,fstr,&lfstr,&unit);
  FStringDelete        (fstr);

  CStringDelete        (fname);
}
/***************************************************************************/
void GEANT_SaveGeometry (
 int   a_format
,char* a_fname
,int   a_unit
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  fname;
  FString   fstr;
  int    lfstr;
  int    fmt;
  int    unit;
/*.........................................................................*/
  if(a_fname==NULL)         return;

  fmt                  = a_format;
  unit                 = a_unit;
  fname                = CStringDuplicate(a_fname);

  if( (a_format==GFILE_FZ) ||
      (a_format==GFILE_FA) ||
      (a_format==GFILE_FX)
     )
    {
      CFileBindToF77_Unit (fname,unit);
    }
  else if( (a_format==GFILE_RZ) ||
	   (a_format==GFILE_RA) ||
	   (a_format==GFILE_RX)
	  )
    {
      CFileLink        (&fname);
    }

  GEANT_Initialize     ();
  fstr                 = FStringCreateFromC (fname,&lfstr);
  ogwgeo               (&fmt,fstr,&lfstr,&unit);
  FStringDelete        (fstr);

  CStringDelete        (fname);

}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void GEANT_ReadEvent(
 int a_unit
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int unit;
/*.........................................................................*/
  unit                 = a_unit;
  GEANT_Initialize     ();
  ogrevt               (&unit);
}
/***************************************************************************/
int GEANT_ProduceEvent (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  GEANT_Initialize     ();
  return        ogevbg();
}
/***************************************************************************/
void GEANT_ClearEvent (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  GEANT_Initialize     ();
  gtrigc        ();
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
char** GEANT_GetVolumeNames (
 int*  a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int    number;
  FString   fname;
  char** list;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  number      = ogvoln ();
  if(number==0) return NULL;
  list        = (char**) CMemoryAllocateBlock (number * sizeof(char*));
  if(list==NULL)   return NULL;
  fname       = FStringCreate (4);
  for(count=0;count<number;count++)
    { 
      int            ivo;
      ivo            = count+1;
      ogvonm         (&ivo,fname);
      list[count]    = FStringConvertToC (fname,4);
    }
  FStringDelete   (fname);
  if(a_number!=NULL) *a_number = number;
  return       list;  
}
/***************************************************************************/
void GEANT_PrintVolumes (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        mode;
/*.........................................................................*/
  GEANT_Initialize  ();
  mode       = 0;
  gpvolu     (&mode);
}
/***************************************************************************/
char* GEANT_GetTopVolume (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char top_volume [5] = "XXXX";
  char* top;
  FString  fstr;
  int   lstr;
  int   stat;
/*.........................................................................*/
  GEANT_Initialize ();
  lstr          = 4;
  fstr          = FStringCreate (lstr);
  stat          = ogvolt     (fstr);
  top           = FStringConvertToC    (fstr,lstr);
  FStringDelete (fstr);
  strcpy        (top_volume,top);
  CStringDelete (top);
  return        stat!=0 ? top_volume : CStringNULL;
}
/***************************************************************************/
int GEANT_GetVolumeNumberOfSons (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString  fstr;
  int   lstr;
  int   nson,ndiv;
/*.........................................................................*/
  if(a_string==NULL) return 0;
  GEANT_Initialize   ();
  fstr           = FStringCreateFromC (a_string,&lstr);
  gdnson         (fstr,&nson,&ndiv);
  FStringDelete  (fstr);
  return         nson;
}
/***************************************************************************/
char* GEANT_GetVolumeIthSon (
 char* a_string
,int a_son
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString  fstr;
  int   lstr;
  int   son;
  FString  fson;
  char* cson;
/*.........................................................................*/
  if(a_string==NULL) return NULL;
  GEANT_Initialize     ();
  fstr          = FStringCreateFromC (a_string,&lstr);
  son           = a_son + 1;
  fson          = FStringCreate (4);
  gdson         (&son,fstr,fson);
  FStringDelete (fstr);
  cson          = FStringConvertToC (fson,4);
  FStringDelete (fson);
  return        cson;
}
/***************************************************************************/
char** GEANT_GetVolumeUpperVolumes (
 char* a_name
,int*  a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  FString   fname;
  int    lname;
  int    moths[64];
  int    mothn;
  int    mothx = 64;
  char** list;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  fname      = FStringCreateFromC (a_name,&lname);
  mothn      = 0;
  ogmoth     (fname,&mothn,moths,&mothx);
  FStringDelete (fname);
  if(mothn==0) return NULL;
  list       = (char**) CMemoryAllocateBlock (mothn * sizeof(char*));
  fname      = FStringCreate (4);
  for(count=0;count<mothn;count++)
    { 
      int            ivo;
      ivo            = moths[count];
      ogvonm         (&ivo,fname);
      list[count]    = FStringConvertToC (fname,4);
    }
  FStringDelete   (fname);
  if(a_number!=NULL) *a_number = mothn;
  return       list;  
}
/***************************************************************************/
void GEANT_SetVolumeVisibility (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int         seen;
/*.........................................................................*/
  if( (a_name==NULL) || (*a_name=='\0') ) return;
  seen   = GEANT_GetVolumeDrawingAttribute (a_name,"SEEN");
  /*     if(seen==GVOL_VIS_SON_VIS) GEANT_SetVolumeDrawingAttribute (a_name,"SEEN", GVOL_VIS_SON_VIS);
  else   if(seen==GVOL_VIS_SON_INV) GEANT_SetVolumeDrawingAttribute (a_name,"SEEN", GVOL_VIS_SON_INV);
  else*/ if(seen==GVOL_INV_SON_VIS) GEANT_SetVolumeDrawingAttribute (a_name,"SEEN", GVOL_VIS_SON_VIS);
  else   if(seen==GVOL_INV_SON_INV) GEANT_SetVolumeDrawingAttribute (a_name,"SEEN", GVOL_VIS_SON_INV);
  EnableMothers (a_name);
}
/***************************************************************************/
void GEANT_SetVolumeDrawingAttribute (
 char* a_name
,char* a_att
,int a_ival
)
/***************************************************************************/
/*
C.    *    1:  'WORK'   0=volume inactive   1=active volume            *
C.    *    2:  'SEEN'   0=unseen 1=seen -1,-2=tree unseen -3=tree opt. *
C.    *    3:  'LSTY'   line style 1,2,3,...                           *
C.    *    4:  'LWID'   line width 1,2,3,...                           *
C.    *    5:  'COLO'   color code 1,2,3,...                           *
C.    *    6:  'FILL'   fill area  0,1,2,...                           *
C.    *    7:  'SET '   set number associated to this volume           *
C.    *    8:  'DET '   detector number associated to this volume      *
C.    *    9:  'DTYP'   detector type (1,2)                            *
C.    *   10:  'NODE'   <>0=a node is created for 3D (PIONS,GMR,etc.)  *

 For 'SEEN':
    1  vis vis  (default)
    0  inv vis
   -1  inv inv
   -2  vis inv
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString       fname;
  int        lname;
  FString       fatt;
  int        latt;
  int        ival;
/*.........................................................................*/
  if(a_name==NULL) return;
  if(a_att==NULL)  return;
  GEANT_Initialize   ();
  fname       = FStringCreateFromC (a_name,&lname);
  fatt        = FStringCreateFromC (a_att ,&latt);
  ival        = a_ival;
  gsatt       (fname,fatt,&ival);
  FStringDelete  (fname);
  FStringDelete  (fatt);
}
/***************************************************************************/
int GEANT_GetVolumeDrawingAttribute (
 char* a_name
,char* a_att
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString       fname;
  int        lname;
  FString       fatt;
  int        latt;
  int        ival;
/*.........................................................................*/
  if(a_name==NULL) return 0;
  if(a_att==NULL)  return 0;
  GEANT_Initialize   ();
  fname       = FStringCreateFromC (a_name,&lname);
  fatt        = FStringCreateFromC (a_att ,&latt);
  gfatt       (fname,fatt,&ival);
  FStringDelete  (fname);
  FStringDelete  (fatt);
  return      ival;
}
/***************************************************************************/
int GEANT_RepresentVolume (
 char* a_string
,int   a_voln
,int*  a_vols
,int*  a_dvols
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString  fstr;
  int   lfstr;
  int   voln;
  int   vols[1];
  int   status;
/*.........................................................................*/
  if(a_string==NULL) return 0;
  GEANT_Initialize   ();
  fstr        = FStringCreateFromC (a_string,&lfstr);
  voln        = a_voln;
  status      = ogvold (fstr,&voln,voln==0 ? vols : a_vols,voln==0 ? vols : a_dvols);
  FStringDelete  (fstr);
  return      status;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int GEANT_GetDetectorNumberOfHits (
 char* a_set
,char* a_det
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString       fset,fdet;
  int        lset,ldet;
  int        val;
/*.........................................................................*/
  if(a_set==NULL) return 0;
  if(a_det==NULL) return 0;
  GEANT_Initialize   ();
  fset        = FStringCreateFromC (a_set,&lset);
  fdet        = FStringCreateFromC (a_det,&ldet);
  gfnhit      (fset,fdet,&val);
  FStringDelete  (fset);
  FStringDelete  (fdet);
  return      val;
}
/***************************************************************************/
int GEANT_GetDetectorIthHit (
 char*   a_set
,char*   a_det
,int     a_hit
,int*    a_voln
,int*    a_vols
,int*    a_hitn
,float*  a_hits
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString    fset,fdet;
  int        lset,ldet;
  int        hit,status,voln,hitn,itra;
/*.........................................................................*/
  if(a_voln!=NULL) *a_voln = 0;
  if(a_hitn!=NULL) *a_hitn = 0;
  if(a_set==NULL) return 0;
  if(a_det==NULL) return 0;
  GEANT_Initialize   ();
  fset        = FStringCreateFromC (a_set,&lset);
  fdet        = FStringCreateFromC (a_det,&ldet);
  hit         = a_hit;
  status      = oghitv (fset,fdet,&hit,&voln,a_vols,&hitn,a_hits,&itra);
  FStringDelete  (fset);
  FStringDelete  (fdet);
  if(a_voln!=NULL)  *a_voln = voln;
  if(a_hitn!=NULL)  *a_hitn = hitn;
  return      status;
}
/***************************************************************************/
int GEANT_GetDetectorNumberOfDIGIs (
 char* a_set
,char* a_det
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString       fset,fdet;
  int        lset,ldet;
  int        val;
/*.........................................................................*/
  if(a_set==NULL) return 0;
  if(a_det==NULL) return 0;
  GEANT_Initialize   ();
  fset        = FStringCreateFromC (a_set,&lset);
  fdet        = FStringCreateFromC (a_det,&ldet);
  gfndig      (fset,fdet,&val);
  FStringDelete  (fset);
  FStringDelete  (fdet);
  return      val;
}
/***************************************************************************/
int GEANT_GetDetectorIthDIGI (
 char*   a_set
,char*   a_det
,int     a_dig
,int*    a_voln
,int*    a_vols
,int*    a_dign
,int*    a_digs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString    fset,fdet;
  int        lset,ldet;
  int        dig,status,voln,dign;
/*.........................................................................*/
  if(a_voln!=NULL) *a_voln = 0;
  if(a_dign!=NULL) *a_dign = 0;
  if(a_set==NULL) return 0;
  if(a_det==NULL) return 0;
  GEANT_Initialize ();
  fset             = FStringCreateFromC (a_set,&lset);
  fdet             = FStringCreateFromC (a_det,&ldet);
  dig              = a_dig;
  status           = ogdigv (fset,fdet,&dig,&voln,a_vols,&dign,a_digs);
  FStringDelete    (fset);
  FStringDelete    (fdet);
  if(a_voln!=NULL) *a_voln = voln;
  if(a_dign!=NULL) *a_dign = dign;
  return      status;
}
/***************************************************************************/
void GEANT_GetDetectorVolumes (
 char*   a_set
,char*   a_det
,int*    a_number
,int*    a_vols
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FString    fset,fdet;
  int        lset,ldet;
  int        voln;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(a_set==NULL) return;
  if(a_det==NULL) return;
  GEANT_Initialize   ();
  fset        = FStringCreateFromC (a_set,&lset);
  fdet        = FStringCreateFromC (a_det,&ldet);
  ogdeto      (fset,fdet,&voln,a_vols);
  FStringDelete  (fset);
  FStringDelete  (fdet);
  if(a_number!=NULL) *a_number = voln;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void GEANT_RepresentTrack (
 int a_itra
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int              itra;
/*.........................................................................*/
  GEANT_Initialize ();
  itra             = a_itra;
  ogkind           (&itra);
}
/***************************************************************************/
void GEANT_RepresentVertex (
 int a_ivert
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int              ivert;
/*.........................................................................*/
  GEANT_Initialize ();
  ivert            = a_ivert;
  ogverd           (&ivert);
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void EnableMothers (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int    seen; 
  char** moths;
  int    mothn;
/*.........................................................................*/
  if(a_name==NULL) return;
  moths  = GEANT_GetVolumeUpperVolumes(a_name,&mothn);
  for(count=0;count<mothn;count++)
    { 
      char*  mother;
      mother = moths[count];
      seen   = GEANT_GetVolumeDrawingAttribute (mother,"SEEN");
      /*     if(seen==GVOL_VIS_SON_VIS) GEANT_SetVolumeDrawingAttribute (mother,"SEEN", GVOL_VIS_SON_VIS);
      else   if(seen==GVOL_INV_SON_VIS) GEANT_SetVolumeDrawingAttribute (mother,"SEEN", GVOL_INV_SON_VIS);
      else*/ if(seen==GVOL_VIS_SON_INV) GEANT_SetVolumeDrawingAttribute (mother,"SEEN", GVOL_VIS_SON_VIS);
      else   if(seen==GVOL_INV_SON_INV) GEANT_SetVolumeDrawingAttribute (mother,"SEEN", GVOL_INV_SON_VIS);
      EnableMothers (mother);
    }
  CTextDelete (mothn,moths);
}
