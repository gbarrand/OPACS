/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef glPS_h
#define glPS_h

#ifdef __cplusplus
extern "C"{
#endif
void glPS_OpenFileForWriting (char*);
void glPS_CloseStream        ();
void glPS_BeginPage          (int,int);
void glPS_EndPage            ();
void glPS_BeginView          (int,int);
void glPS_EndView            ();
#ifdef __cplusplus
}
#endif


#endif  /*glPS_h*/
