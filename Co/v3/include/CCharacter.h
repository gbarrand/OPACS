/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CCharacter_h
#define CCharacter_h

#ifdef __cplusplus
extern "C"{
#endif
char   CCharacterConvertToEBCDIC       (char);
char   CCharacterConvertToASCII        (char);
int    CCharacterIsUpperCase           (char);
int    CCharacterIsLowerCase           (char);
int    CCharacterIsDigit               (char);
int    CCharacterIsAlphanumeric        (char);
int    CCharacterIsPrintable           (char);
char   CCharacterConvertToLowerCase    (char);
char   CCharacterConvertToUpperCase    (char);
int    CCharacterIsSeparator           (char);
void   CCharacterGetGreekFontPoints    (char,int*,int*,double*,double*,double*);
void   CCharacterGetASCII_FontPoints   (char,int*,int*,double*,double*,double*);
#ifdef __cplusplus
}
#endif


#ifdef __CI_BIND__
CiBindCCharacter(0); /**/
#endif

#endif  /*CCharacter_h*/
