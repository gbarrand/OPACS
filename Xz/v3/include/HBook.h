/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef HBook_h
#define HBook_h

typedef void(*HBookUserFunction)(int,char*);
typedef int (*HBookConvertFunction)(char*);

#ifdef __cplusplus
extern "C"{
#endif
void              HBookClearClass                    ();
void              HBookInitialize                    (int);
HBookUserFunction HBookSetUserFunction               (HBookUserFunction);
char**            HBookGetSubDirectories             (int*);
char**            HBookGetEntries                    (int*);
void              HBookGetNextEntry                  (int*,char**,char**,char*);
char*             HBookGetDirectoryName              ();
int               HBookSetDirectory                  (char*);
void              HBookBringDirectoryInMemory        ();
void              HBookAddConverter                  (HBookConvertFunction);
int               HBookConvert                       (char*);
void              HBookGetHistogramInformations      (int,char**,int*,double*,double*,int*,double*,double*);
void              HBookGetTupleInformations          (int,char**,int*,int*,char***);
void              HBookCreateTuple                   (int,char*,int,int,char**);
void              HBookCreateOneDimensionalHistogram (int,char*,int,double,double);
void              HBookCreateTwoDimensionalHistogram (int,char*,int,double,double,int,double,double);
void              HBookCreateProfileHistogram        (int,char*,int,double,double,double,double);
int               HBookBuildIdentifier               (int);
int*              HBookGetIdentifiers                ();
void              HBookDelete                        (int);
int               HBookGetDimension                  (int);
void              HBookSetOption                     (int,char*);
void              HBookSetTitle                      (char*);
int               HBookSetAttribute                  (int,char*,void*,void*,int);
int               HBookGetAttribute                  (int,char*,void*,void*,int*);
int               HBookDoMethod                      (int,char*,void*,int,char**,void*,int*);
void*             HBookConstruct                     (void*,int,char**,char**);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindHBook(0); /**/
#endif

#endif  /*HBook_h*/
