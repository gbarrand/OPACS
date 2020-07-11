/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CiStat_h
#define CiStat_h

#ifdef __cplusplus
extern "C"{
#endif
static  int              CiLoaded                    ();

static  void             CiParserStartup             ();
static  CiVariable       CiVariableGetReference      (CiPointer);
static  CiStatement      CiTextCompile               (CiPointer);
static  void             CiVariableDelete            (CiVariable);
static  CiValue          CiVariableGetValue          (CiVariable);
static  void             CiValuePSet                 (CiValue,CiPointer);
static  double           CiValueCastToDouble         (CiValue);
static  CiValue          CiStatementExecute          (CiStatement);
static  CiRoutine        CiFunctionGetBind           (CiPointer);
static  CiSymbol         CiSymbolGetReference        (CiPointer,CHasher);
static  void             CiStatementDelete           (CiStatement);
static  CiFunctionSwitch CiFunctionSwitchNew         (CiIRoutine);
static  void             CiDo                        (CiPointer);
static  CiFunction       CiFunctionBind              (CiPointer,CiRoutine);
static  void             CiParserCleanup             ();
static  CiPath           CiPathNew                   (CiPointer);
static  void             CiParserDeclareErrorHandler (CiRoutine);
static  void             CiBindClass                 (char*,CiRoutine);
static  CiRoutine        CiSetPrintFunction          (CiRoutine);

static  CiRoutine        CiBindmath                  (char*);
static  CiRoutine        CiBindstdarg                (char*);
static  CiRoutine        CiBindstdio                 (char*);
static  CiRoutine        CiBindstdlib                (char*);
static  CiRoutine        CiBindstring                (char*);
static  CiRoutine        CiBindtime                  (char*);
static  CiRoutine        CiBindCi                    (char*);
#ifdef __cplusplus
}
#endif

#endif /*CiStat_h*/
