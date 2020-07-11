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
$CIBIN/cigen.exe -p -f HBook.h -n CiBindHBook -o ../gen/HBook_B.c
$CIBIN/cigen.exe -p -f HF77.h -n CiBindHF77 -o ../gen/HF77_B.c
$CIBIN/cigen.exe -p -f HFile.h -n CiBindHFile -o ../gen/HFile_B.c
$CIBIN/cigen.exe -p -f HPlot.h -n CiBindHPlot -o ../gen/HPlot_B.c
$CIBIN/cigen.exe -p -f Kuip.h -n CiBindKuip -o ../gen/Kuip_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XzPlotter.h -n CiBindXzPlotter -o ../gen/XzPlotter_B.c
$CIBIN/cigen.exe -p -f XzTypes.h -n CiBindXzTypes -o ../gen/XzTypes_B.c
cd ../mgr
/bin/rm -f ../src/XzBinds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/XzBinds.c ; done
cd ../mgr
