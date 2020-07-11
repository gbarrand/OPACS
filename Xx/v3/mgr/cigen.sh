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
cp $COROOT/include/FString.h .
$CIBIN/cigen.exe -p -h stdarg.h -h X11/Xlib.h -h X11/Xresource.h -f XDisplay.h -n CiBindXDisplay -o ../gen/XDisplay_B.c
$CIBIN/cigen.exe -p -h X11/Xlib.h -f XMailbox.h -n CiBindXMailbox -o ../gen/XMailbox_B.c
$CIBIN/cigen.exe -p -h X11/Xlib.h -f XSelection.h -n CiBindXSelection -o ../gen/XSelection_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -h XDisplay.h -f XWidget.h -n CiBindXWidget -o ../gen/XWidget_B.c
$CIBIN/cigen.exe -p -h FString.h -f XxF77.h -n CiBindXxF77 -o ../gen/XxF77_B.c
$CIBIN/cigen.exe -p -f XxTypes.h -n CiBindXxTypes -o ../gen/XxTypes_B.c
/bin/rm -f FString.h
cd ../mgr
/bin/rm -f ../src/XxBinds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/XxBinds.c ; done
cd ../mgr
