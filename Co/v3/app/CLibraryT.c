/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>
#include <stdio.h>

#include <CPrinter.h>
#include <CLibrary.h>

int main()
{

  void*      regLib  = NULL;
  void*      csetLib = NULL;
  void*      ciLib   = NULL;

  regLib       = CLibraryOpen ("CSETSHREG");
  if(regLib==NULL)  return 1;
  csetLib      = CLibraryOpen ("CSETSHLIB");
  if(csetLib==NULL) return 1;
  ciLib        = CLibraryOpen ("CISHLIB");
  if(ciLib==NULL)   return 1;

  CInfo ("Libs open.\n");

  CLibraryGetSymbol (ciLib,"CiParserStartup");
  CLibraryGetSymbol (ciLib,"CiVariableGetReference");
  CLibraryGetSymbol (ciLib,"CiTextCompile");
  CLibraryGetSymbol (ciLib,"CiVariableDelete");
  CLibraryGetSymbol (ciLib,"CiVariableGetValue");
  CLibraryGetSymbol (ciLib,"CiValuePSet");
  CLibraryGetSymbol (ciLib,"CiValueCastToDouble");
  CLibraryGetSymbol (ciLib,"CiStatementExecute");
  CLibraryGetSymbol (ciLib,"CiFunctionGetBind");
  CLibraryGetSymbol (ciLib,"CiSymbolGetReference");
  CLibraryGetSymbol (ciLib,"CiStatementDelete");
  CLibraryGetSymbol (ciLib,"CiFunctionSwitchNew");

  CInfo ("Symbols found.\n");

  return  EXIT_SUCCESS;
}

