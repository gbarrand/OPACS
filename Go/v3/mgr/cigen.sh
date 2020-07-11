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
cp $COROOT/include/CList.h .
cp $COROOT/include/FString.h .
cp $COROOT/include/OMatrix.h .
cp $COROOT/include/OColor.h .
$CIBIN/cigen.exe -p -h OPointList.h -h ONode.h -f Go.h -n CiBindGo -o ../gen/Go_B.c
$CIBIN/cigen.exe -p -h FString.h -h ONode.h -f GoF77.h -n CiBindGoF77 -o ../gen/GoF77_B.c
$CIBIN/cigen.exe -p -f GoTypes.h -n CiBindGoTypes -o ../gen/GoTypes_B.c
$CIBIN/cigen.exe -p -h OCamera.h -f OCamDAWN.h -n CiBindOCamDAWN -o ../gen/OCamDAWN_B.c
$CIBIN/cigen.exe -p -h OCamera.h -f OCamVRML.h -n CiBindOCamVRML -o ../gen/OCamVRML_B.c
$CIBIN/cigen.exe -p -h ONode.h -h OMatrix.h -h OColormap.h -h OPick.h -f OCamera.h -n CiBindOCamera -o ../gen/OCamera_B.c
$CIBIN/cigen.exe -p -f OColormap.h -n CiBindOColormap -o ../gen/OColormap_B.c
$CIBIN/cigen.exe -p -h OColor.h -h stdarg.h -f OContext.h -n CiBindOContext -o ../gen/OContext_B.c
$CIBIN/cigen.exe -p -h stdarg.h -h CList.h -h OMatrix.h -h OPrimitive.h -f ONode.h -n CiBindONode -o ../gen/ONode_B.c
$CIBIN/cigen.exe -p -f OPick.h -n CiBindOPick -o ../gen/OPick_B.c
$CIBIN/cigen.exe -p -f OPointList.h -n CiBindOPointList -o ../gen/OPointList_B.c
$CIBIN/cigen.exe -p -h OContext.h -f OPrimitive.h -n CiBindOPrimitive -o ../gen/OPrimitive_B.c
/bin/rm -f CList.h
/bin/rm -f FString.h
/bin/rm -f OMatrix.h
/bin/rm -f OColor.h
cd ../mgr
/bin/rm -f ../src/GoBinds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/GoBinds.c ; done
cd ../mgr
