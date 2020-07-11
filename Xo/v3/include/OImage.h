/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OImage_h
#define OImage_h

#ifndef NO_X_H
#include <X11/Xlib.h>
#endif
 
typedef struct _OImageRecord   *OImage;

#ifdef __cplusplus
extern "C"{
#endif
void          OImageClearClass           ();
OImage        OImageGetIdentifier        (char*);
OImage*       OImageGetIdentifiers       ();
OImage        OImageMake                 ();
OImage        OImageCreate               (Display*,char*);
OImage        OImageCreateFromRootWindow (Display*,int,int,int,int);
OImage        OImageCreateFromXBM_File   (Display*,char*);
OImage        OImageCreateFromGIF_File   (Display*,char*);
OImage        OImageCreateFromTIFF_File  (Display*,char*);
OImage        OImageCreateFromFITS_File  (Display*,char*,int,int,unsigned int,unsigned int,int);
OImage        OImageCreateFromBytes      (Display*,int,int,char*);
OImage        OImageCreateRotated        (Display*,char*,OImage);

int           OImageIsValid              (OImage);
void          OImageSetName              (OImage,char*);
void          OImageDelete               (OImage);
char*         OImageGetName              (OImage);
char*         OImageGetFileName          (OImage);
void          OImageSetFileType          (OImage,char*);
char*         OImageGetFileType          (OImage);
Display*      OImageGetDisplay          (OImage);
void*         OImageGetX11_Image         (OImage);
void          OImageSetX11_Image         (OImage,void*);
void          OImageGetRGBs              (OImage,int*,double**,double**,double**);
void*         OImageGetSizeAndBytes      (OImage,unsigned int*,unsigned int*);
int           OImageGetPixel             (OImage,int,int,long*,unsigned long*,double*,double*,double*);
int           OImageGetAttribute         (OImage,char*,void*,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOImage(0); /**/
#endif

#endif  /*OImage_h*/
