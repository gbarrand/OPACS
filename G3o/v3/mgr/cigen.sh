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
$CIBIN/cigen.exe -p -f G3oTypes.h -n CiBindG3oTypes -o ../gen/G3oTypes_B.c
$CIBIN/cigen.exe -p -f GEANT.h -n CiBindGEANT -o ../gen/GEANT_B.c
$CIBIN/cigen.exe -p -f GF77.h -n CiBindGF77 -o ../gen/GF77_B.c
cd ../mgr
/bin/rm -f ../src/G3oBinds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/G3oBinds.c ; done
cd ../mgr
