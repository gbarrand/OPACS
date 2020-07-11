/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CoBind_h
#define CoBind_h

#ifdef __cplusplus
extern "C"{
#endif
CiRoutine CiBindCiDummy (char*); 

CiRoutine CiBindCBatch(char*);
CiRoutine CiBindCCharacter(char*);
CiRoutine CiBindCColor(char*);
CiRoutine CiBindCError(char*);
CiRoutine CiBindCFITS(char*);
CiRoutine CiBindCFile(char*);
CiRoutine CiBindCGIF(char*);
CiRoutine CiBindCImageshort(char*);
CiRoutine CiBindCJPEG(char*);
CiRoutine CiBindCLibrary(char*);
CiRoutine CiBindCList(char*);
CiRoutine CiBindCManager(char*);
CiRoutine CiBindCMath(char*);
CiRoutine CiBindCMemory(char*);
CiRoutine CiBindCOption(char*);
CiRoutine CiBindCPS(char*);
CiRoutine CiBindCPrinter(char*);
CiRoutine CiBindCStream(char*);
CiRoutine CiBindCString(char*);
CiRoutine CiBindCSystem(char*);
CiRoutine CiBindCTIFF(char*);
CiRoutine CiBindCText(char*);
CiRoutine CiBindCTime(char*);
CiRoutine CiBindCVector(char*);
CiRoutine CiBindCXPM(char*);
CiRoutine CiBindCoCommands(char*);
CiRoutine CiBindCoTypes(char*);
CiRoutine CiBindFString(char*);
CiRoutine CiBindOBatch(char*);
CiRoutine CiBindOCommand(char*);
CiRoutine CiBindOHTML(char*);
CiRoutine CiBindOHandle(char*);
CiRoutine CiBindOLUT(char*);
CiRoutine CiBindOMatrix(char*);
CiRoutine CiBindOMatrix33(char*);
CiRoutine CiBindOModule(char*);
CiRoutine CiBindOProcedure(char*);
CiRoutine CiBindOProcedureL(char*);
CiRoutine CiBindOProcess(char*);
CiRoutine CiBindOProperty(char*);
CiRoutine CiBindOShell(char*);
CiRoutine CiBindOTouch(char*);
CiRoutine CiBindOType(char*);
CiRoutine CiBindOColor(char*);
#ifdef __cplusplus
}
#endif

#endif  /*CoBind_h*/
