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
$CIBIN/cigen.exe -p -h X11/Xlib.h -f OColormapX.h -n CiBindOColormapX -o ../gen/OColormapX_B.c
$CIBIN/cigen.exe -p -h X11/Xlib.h -f OImage.h -n CiBindOImage -o ../gen/OImage_B.c
$CIBIN/cigen.exe -p -h CList.h -f OPlotter.h -n CiBindOPlotter -o ../gen/OPlotter_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f Xo.h -n CiBindXo -o ../gen/Xo_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -h CList.h -h XWidget.h -f XoCamera.h -n CiBindXoCamera -o ../gen/XoCamera_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XoColormap.h -n CiBindXoColormap -o ../gen/XoColormap_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XoEyes.h -n CiBindXoEyes -o ../gen/XoEyes_B.c
$CIBIN/cigen.exe -p -f XoF77.h -n CiBindXoF77 -o ../gen/XoF77_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XoShape.h -n CiBindXoShape -o ../gen/XoShape_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -h XWidget.h -f XoGraph.h -n CiBindXoGraph -o ../gen/XoGraph_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XoHelp.h -n CiBindXoHelp -o ../gen/XoHelp_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XoImage.h -n CiBindXoImage -o ../gen/XoImage_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XoLabel.h -n CiBindXoLabel -o ../gen/XoLabel_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XoMosaic.h -n CiBindXoMosaic -o ../gen/XoMosaic_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XoPlotter.h -n CiBindXoPlotter -o ../gen/XoPlotter_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f XoRheostat.h -n CiBindXoRheostat -o ../gen/XoRheostat_B.c
$CIBIN/cigen.exe -p -f XoTypes.h -n CiBindXoTypes -o ../gen/XoTypes_B.c
/bin/rm -f CList.h
cd ../mgr
/bin/rm -f ../src/XoBinds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/XoBinds.c ; done
cd ../mgr
