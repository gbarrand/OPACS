/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OStatement_h
#define OStatement_h

typedef void* OStatement; 

typedef void (*OStatementInterpretedFunction)(void);
typedef void (*OStatementCiFunction)(void);

#ifdef __cplusplus
extern "C"{
#endif
void                           OStatementClearClass            ();
void                           OStatementSetFunctions          (OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction,OStatementCiFunction);
OStatement                     OStatementCreate                (char*,int,char**);
void                           OStatementDelete                (OStatement);
double                         OStatementEvaluate              (OStatement,int,double*);
OStatementInterpretedFunction  OStatementGetFunctionIdentifier (char*);
void*                          OStatementGetSymbolIdentifier   (char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOStatement(0); /**/
#endif

#endif  /*OStatement_h*/
