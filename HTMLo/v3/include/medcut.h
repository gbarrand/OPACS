#ifndef medcut_h
#define medcut_h

#include <stdio.h>
#include <X11/Xlib.h>

/*Barrand. To build AIX sheared libs.*/
#ifdef __cplusplus
extern "C"{
#endif
void           MedianCut (unsigned char *,int*,int*,XColor*,int,int);
unsigned char* ReadGIF   (FILE*,int,int*);
#ifdef __cplusplus
}
#endif

#endif  /*medcut_h*/



