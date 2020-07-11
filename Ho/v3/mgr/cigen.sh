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
$CIBIN/cigen.exe -p -h OHistogram.h -h OScatter.h -h OTuple.h -h OCut.h -h OFit.h -f Ho.h -n CiBindHo -o ../gen/Ho_B.c
$CIBIN/cigen.exe -p -f HoTypes.h -n CiBindHoTypes -o ../gen/HoTypes_B.c
$CIBIN/cigen.exe -p -h stdarg.h -f OAxis.h -n CiBindOAxis -o ../gen/OAxis_B.c
$CIBIN/cigen.exe -p -f OCut.h -n CiBindOCut -o ../gen/OCut_B.c
$CIBIN/cigen.exe -p -f OFit.h -n CiBindOFit -o ../gen/OFit_B.c
$CIBIN/cigen.exe -p -h OAxis.h -f OFunction.h -n CiBindOFunction -o ../gen/OFunction_B.c
$CIBIN/cigen.exe -p -h OAxis.h -f OHistogram.h -n CiBindOHistogram -o ../gen/OHistogram_B.c
$CIBIN/cigen.exe -p -h OAxis.h -f OScatter.h -n CiBindOScatter -o ../gen/OScatter_B.c
$CIBIN/cigen.exe -p -f OTuple.h -n CiBindOTuple -o ../gen/OTuple_B.c
$CIBIN/cigen.exe -p -f OStatement.h -n CiBindOStatement -o ../gen/OStatement_B.c
cd ../mgr
/bin/rm -f ../src/HoBinds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/HoBinds.c ; done
cd ../mgr
