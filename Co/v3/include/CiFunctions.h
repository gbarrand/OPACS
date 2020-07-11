/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CiFunctions_h
#define CiFunctions_h

typedef struct _CHasherRec*          CHasher;

typedef struct _CiVariableRec*       CiVariable;
typedef struct _CiStatementRec*      CiStatement;
typedef struct _CiValueRec*          CiValue;
typedef char*                        CiPointer;
typedef struct _CiSymbolRec*         CiSymbol;
typedef struct _CiFunctionSwitchRec* CiFunctionSwitch;
typedef struct _CiFunctionRec*       CiFunction;
typedef struct _CiPathRec*           CiPath;
typedef void*                        CiPrimValue;

#define CI_ELLIPSIS
typedef void   (*CiRoutine)(CI_ELLIPSIS);
typedef int    (*CiIRoutine)(CI_ELLIPSIS);

typedef   void             (*CiParserStartupFunction)             ();
typedef   CiVariable       (*CiVariableGetReferenceFunction)      (CiPointer);
typedef   CiStatement      (*CiTextCompileFunction)               (CiPointer);
typedef   void             (*CiVariableDeleteFunction)            (CiVariable);
typedef   CiValue          (*CiVariableGetValueFunction)          (CiVariable);
typedef   void             (*CiValuePSetFunction)                 (CiValue,CiPointer);
typedef   double           (*CiValueCastToDoubleFunction)         (CiValue);
typedef   CiValue          (*CiStatementExecuteFunction)          (CiStatement);
typedef   CiRoutine        (*CiFunctionGetBindFunction)           (CiPointer);
typedef   CiSymbol         (*CiSymbolGetReferenceFunction)        (CiPointer,CHasher);
typedef   void             (*CiStatementDeleteFunction)           (CiStatement);
typedef   CiFunctionSwitch (*CiFunctionSwitchNewFunction)         (CiIRoutine);
typedef   void             (*CiDoFunction)                        (CiPointer);
typedef   CiFunction       (*CiFunctionBindFunction)              (CiPointer,CiRoutine);
typedef   void             (*CiParserCleanupFunction)             ();
typedef   CiPath           (*CiPathNewFunction)                   (CiPointer);
typedef   void             (*CiParserDeclareErrorHandlerFunction) (CiRoutine);
typedef   void             (*CiBindClassFunction)                 (char*,CiRoutine);
typedef   CiRoutine        (*CiSetPrintFunctionFunction)          (CiRoutine);

typedef   CiRoutine        (*CiBindFunction)                      (char*);

typedef struct _CiFunctionsRecord
{
  CiParserStartupFunction             CiParserStartup;
  CiVariableGetReferenceFunction      CiVariableGetReference;
  CiTextCompileFunction               CiTextCompile;
  CiVariableDeleteFunction            CiVariableDelete;
  CiVariableGetValueFunction          CiVariableGetValue;
  CiValuePSetFunction                 CiValuePSet;
  CiValueCastToDoubleFunction         CiValueCastToDouble;
  CiStatementExecuteFunction          CiStatementExecute;
  CiFunctionGetBindFunction           CiFunctionGetBind;
  CiSymbolGetReferenceFunction        CiSymbolGetReference;
  CiStatementDeleteFunction           CiStatementDelete;
  CiFunctionSwitchNewFunction         CiFunctionSwitchNew;
  CiDoFunction                        CiDo;
  CiFunctionBindFunction              CiFunctionBind;
  CiParserCleanupFunction             CiParserCleanup;
  CiPathNewFunction                   CiPathNew;
  CiParserDeclareErrorHandlerFunction CiParserDeclareErrorHandler;
  CiBindClassFunction                 CiBindClass;
  CiSetPrintFunctionFunction          CiSetPrintFunction;
/*C binding.*/
  CiBindFunction                      CiBindmath;
  CiBindFunction                      CiBindstdarg;
  CiBindFunction                      CiBindstdio;
  CiBindFunction                      CiBindstdlib;
  CiBindFunction                      CiBindstring;
  CiBindFunction                      CiBindtime;
  CiBindFunction                      CiBindCi;
} CiFunctionsRecord;

#define CiNULL {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}

#endif  /*CiFunctions_h*/

