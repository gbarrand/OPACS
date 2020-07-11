/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef ociargs_h
#define ociargs_h

void   OAppDummy1  (char* a,int b);
void   OAppDummy2  (char* a,double b);
void   OAppDummy3  (char* a,char* b);
void   OAppDummy4  (char* a,int b,int c,int d);
void   OAppDummy5  (char* a,int b,int c,int d,int e,int f);
int    OAppDummy6  (char* a,int b,int c);
int    OAppDummy7  (char* a,char* b);
void   OAppDummy8  (char* a,char* b,char* c);
void   OAppDummy9  (char* a,char* b,double c,double d);
void   OAppDummy10 (char* a,double b,double c);
void   OAppDummy11 (char* a,double b,double c,double d,double e);
void   OAppDummy12 (char* a,int b,int c);
void   OAppDummy13 (short a);
void   OAppDummy14 (char* a,double b,double c,double d,double e);
void   OAppDummy15 (double a,double b);
void   OAppDummy16 (double a,double b,double c,double d);
void   OAppDummy17 (int a,short b);
void   OAppDummy18 (char* a,char* b,double c,double d,int e);
void   OAppDummy19 (char* a,char* b,double c);
void   OAppDummy20 (char* a,char* b,char* c,double d,double e);
void   OAppDummy21 (char* a,char* b,short c);
void   OAppDummy22 (char* a,char* b,int c,int d,int e,int f,int g,double h,double i);
void   OAppDummy23 (char* a,int b,int c,int d,double e,double f);
void   OAppDummy24 (char* a,int b,double c);
void   OAppDummy25 (char* a,double b,int c);
int    OAppDummy28 (int a,double b);
int    OAppDummy29 (char* a,int b,char* c,double d);
void   OAppDummy30 (char* a,char* b,double c,int d);
void   OAppDummy31 (double a);
int    OAppDummy32 (int a,double* b,int c,int d,double e);
int    OAppDummy33 (char* a,int b,int c,double d,char* e);
void   OAppDummy34 (char* a,char* b,char* c,int d,char* e,int f,double g,double h);
char*  OAppDummy35 (char* a,char* b,char* c,int d,char* e,int f,double g,double h,char* i,int j,double k,double l);
void   OAppDummy36 (char* a,char* b,double c,double d,double e);
double OAppDummy37 (double a,int b,double c);
double OAppDummy38 (double a,int b,double c,double d);
double OAppDummy39 (double a,int b,double c,double d,double e);
double OAppDummy40 (double a,int b,double c,double d,double e,double f);
/*OMatrixCreate.*/
char*  OAppDummy42 (int a,double b);
char*  OAppDummy43 (int a,double b,double c,double d);
char*  OAppDummy44 (int a,double b,double c,double d,double e,
                                  double f,double g,double h,double i,
                                  double j,double k,double l,double m,
                                  double o,double p,double q,double r);
char*  OAppDummy45 (int a,double b,double c,double d,double e,double f,double g);

char*  OAppDummy46 (char* a,char* b,char* c,double d,char* e,char* f,double g);
char*  OAppDummy47 (char* a,double b);
void   OAppDummy48 (char*** a,int* b,int** c,int* d);
void   OAppDummy49 (int* a,char** b,char** c,char* d);
char*  OAppDummy50 (char* a,int b,char* c,char* d);
double OAppDummy51 (char* a,int b,double c);
void   OAppDummy52 (char* a,int b,double c,double d);
void   OAppDummy53 (char* a,char* b,char* c,int d,int e);
void   OAppDummy54 (char* a,char* b,char* c,int d,char* e);
void   OAppDummy55 (char* a,char* b,char* c,long d);
void   OAppDummy56 (char* a,int b,long c);
void   OAppDummy57 (char* a,unsigned long,unsigned long,unsigned long,char*);
void   OAppDummy58 (char* a,char* b,char* c,int d,int   e);
void   OAppDummy59 (char* a,char* b,char* c,int d,char* e);
char*  OAppDummy60 (char* a,int b,char* c,int d,double e,double f);
char*  OAppDummy61 (char* a,int b,char* c,int d,double e,double f,char* g,int h,double i,double j);

char*  OAppDummy62 (char*,char*,char*,char*);
char*  OAppDummy63 (char*,char*,double);
void*  OAppDummy64 (void*, void*, void*, void*, void*);
void*  OAppDummy65 (int,void*, void*, void*, void*, void*);
void   OAppDummy66 (void*, void*, int, int);
double OAppDummy67 (double,int,double,int);
double OAppDummy68 (double,int,int,double);
void*  OAppDummy69 (void*,int);
void*  OAppDummy70 (void*,char*);

#endif  /*ociargs_h*/
