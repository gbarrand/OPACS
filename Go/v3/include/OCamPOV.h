/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OCamPOV_h
#define OCamPOV_h
 
#include <OCamera.h>
#ifdef __cplusplus
extern "C"{
#endif
void OCameraOpenPOV_FileForWriting   (char*);
void OCameraClosePOV_Stream          ();
void OCameraPutSceneInPOV_Stream     (OCamera);
void OCameraProducePOV_File          (OCamera,char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOCamPOV(0); /**/
#endif

#endif  /*OCamPOV_h*/
