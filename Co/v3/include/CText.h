/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CText_h
#define CText_h

#ifdef __cplusplus
extern "C"{
#endif
void   CTextInitialize                  (int*,char***);
void   CTextDelete                      (int,char**);
char** CTextCreateFromString            (char*,int*);
char** CTextDuplicate                   (int,char**,int*);
void   CTextAddLine                     (int*,char***,char*);
void   CTextAddAllocatedLine            (int*,char***,char*);
void   CTextPrependLine                 (int*,char***,char*);
void   CTextConcatenate                 (int*,char***,int,char**);
void   CTextClean                       (int*,char***);
void   CTextRemoveDuplicates            (int*,char***);
int    CTextGetLineIndex                (int,char**,char*);
char*  CTextGetIthLine                  (int,char**,int);
void   CTextSort                        (int,char**);
void   CTextTrim                        (int,char**);
char*  CTextConvertToString             (int,char**,char*);
char*  CTextConvertToStringWithNewLines (int,char**);
void   CTextDump                        (int,char**);
int    CTextReplaceString               (int,char**,char*,char*);
int    CTextReplaceWord                 (int,char**,char*,char*);
int    CTextSortByText                  (int,char**,int,char**);
int    CTextCountString                 (int,char**,char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCText(0); /**/
#endif

#endif  /*CText_h*/
