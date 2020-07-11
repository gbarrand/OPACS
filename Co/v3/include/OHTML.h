/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OHTML_h
#define OHTML_h
 
#ifdef __cplusplus
extern "C"{
#endif
void    OHTML_ClearClass        ();
char*   OHTML_ParseLocator      (char*,char**);
char*   OHTML_GetTopic          (char*,int,char***,int*,char***,int*);
char*   OHTML_GoBackward        ();
void    OHTML_DumpTopic         (char*);
void    OHTML_ProduceLATEX_File (char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOHTML(0); /**/
#endif

#endif  /*OHTML_h*/
