#!/bin/sh -f
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Execute cigen
#  UNIX> sh cigen.sh
# Do a :
#  UNIX> grep 'Ci\> Error' ../gen/*.c
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
set -x
if test "$CIBIN" = ""        ; then echo "CIBIN not set"        ; exit 1 ; fi 
if test "$CISTDINCLUDE" = "" ; then echo "CISTDINCLUDE not set" ; exit 1 ; fi 
if test ! -d ../gen ; then mkdir ../gen ; fi
/bin/rm -f ../gen/*
cd ../include
$CIBIN/cigen.exe -f CiDummy.h -n CiBindCiDummy -o ../gen/CiDummy_B.c

$CIBIN/cigen.exe -p -f CBatch.h -n CiBindCBatch -o ../gen/CBatch_B.c
$CIBIN/cigen.exe -p -f CCharacter.h -n CiBindCCharacter -o ../gen/CCharacter_B.c
$CIBIN/cigen.exe -p -f CColor.h -n CiBindCColor -o ../gen/CColor_B.c
$CIBIN/cigen.exe -p -f CError.h -n CiBindCError -o ../gen/CError_B.c
$CIBIN/cigen.exe -p -h stdio.h -f CFITS.h -n CiBindCFITS -o ../gen/CFITS_B.c
$CIBIN/cigen.exe -p -h stdio.h -f CFile.h -n CiBindCFile -o ../gen/CFile_B.c
$CIBIN/cigen.exe -p -h stdio.h -f CGIF.h -n CiBindCGIF -o ../gen/CGIF_B.c
$CIBIN/cigen.exe -p -f CImageshort.h -n CiBindCImageshort -o ../gen/CImageshort_B.c
$CIBIN/cigen.exe -p -f CJPEG.h -n CiBindCJPEG -o ../gen/CJPEG_B.c
$CIBIN/cigen.exe -p -f CLibrary.h -n CiBindCLibrary -o ../gen/CLibrary_B.c
$CIBIN/cigen.exe -p -h stdarg.h -f CList.h -n CiBindCList -o ../gen/CList_B.c
$CIBIN/cigen.exe -p -f CManager.h -n CiBindCManager -o ../gen/CManager_B.c
$CIBIN/cigen.exe -p -f CMath.h -n CiBindCMath -o ../gen/CMath_B.c
$CIBIN/cigen.exe -p -h stdlib.h -f CMemory.h -n CiBindCMemory -o ../gen/CMemory_B.c
$CIBIN/cigen.exe -p -f COption.h -n CiBindCOption -o ../gen/COption_B.c
$CIBIN/cigen.exe -p -f CPS.h -n CiBindCPS -o ../gen/CPS_B.c
$CIBIN/cigen.exe -p -h stdarg.h -f CPrinter.h -n CiBindCPrinter -o ../gen/CPrinter_B.c
$CIBIN/cigen.exe -p -h stdio.h -f CStream.h -n CiBindCStream -o ../gen/CStream_B.c
$CIBIN/cigen.exe -p -h stdarg.h -f CString.h -n CiBindCString -o ../gen/CString_B.c
$CIBIN/cigen.exe -p -f CSystem.h -n CiBindCSystem -o ../gen/CSystem_B.c
$CIBIN/cigen.exe -p -h stdio.h -f CTIFF.h -n CiBindCTIFF -o ../gen/CTIFF_B.c
$CIBIN/cigen.exe -p -f CText.h -n CiBindCText -o ../gen/CText_B.c
$CIBIN/cigen.exe -p -h time.h -f CTime.h -n CiBindCTime -o ../gen/CTime_B.c
$CIBIN/cigen.exe -p -f CVector.h -n CiBindCVector -o ../gen/CVector_B.c
$CIBIN/cigen.exe -p -h stdio.h -f CXPM.h -n CiBindCXPM -o ../gen/CXPM_B.c
$CIBIN/cigen.exe -p -h OShell.h -f CoCommands.h -n CiBindCoCommands -o ../gen/CoCommands_B.c
$CIBIN/cigen.exe -p -f CoTypes.h -n CiBindCoTypes -o ../gen/CoTypes_B.c
$CIBIN/cigen.exe -p -f FString.h -n CiBindFString -o ../gen/FString_B.c
$CIBIN/cigen.exe -p -f OBatch.h -n CiBindOBatch -o ../gen/OBatch_B.c
$CIBIN/cigen.exe -p -f OCommand.h -n CiBindOCommand -o ../gen/OCommand_B.c
$CIBIN/cigen.exe -p -f OHTML.h -n CiBindOHTML -o ../gen/OHTML_B.c
$CIBIN/cigen.exe -p -h stdarg.h -f OHandle.h -n CiBindOHandle -o ../gen/OHandle_B.c
$CIBIN/cigen.exe -p -f OLUT.h -n CiBindOLUT -o ../gen/OLUT_B.c
$CIBIN/cigen.exe -p -f OMatrix.h -n CiBindOMatrix -o ../gen/OMatrix_B.c
$CIBIN/cigen.exe -p -f OMatrix33.h -n CiBindOMatrix33 -o ../gen/OMatrix33_B.c
$CIBIN/cigen.exe -p -f OModule.h -n CiBindOModule -o ../gen/OModule_B.c
$CIBIN/cigen.exe -p -f OProcedure.h -n CiBindOProcedure -o ../gen/OProcedure_B.c
$CIBIN/cigen.exe -p -h OProcedure.h -f OProcedureL.h -n CiBindOProcedureL -o ../gen/OProcedureL_B.c
$CIBIN/cigen.exe -p -h CList.h -h OHandle.h -f OProcess.h -n CiBindOProcess -o ../gen/OProcess_B.c
$CIBIN/cigen.exe -p -h stdarg.h -f OProperty.h -n CiBindOProperty -o ../gen/OProperty_B.c
$CIBIN/cigen.exe -p -h OProcess.h -h OCommand.h -f OShell.h -n CiBindOShell -o ../gen/OShell_B.c
$CIBIN/cigen.exe -p -f OTouch.h -n CiBindOTouch -o ../gen/OTouch_B.c
$CIBIN/cigen.exe -p -h OProperty.h -h OHandle.h -f OType.h -n CiBindOType -o ../gen/OType_B.c
$CIBIN/cigen.exe -p -f OColor.h -n CiBindOColor -o ../gen/OColor_B.c
cd ../mgr
/bin/rm -f ../src/CoBinds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/CoBinds.c ; done
cd ../mgr



