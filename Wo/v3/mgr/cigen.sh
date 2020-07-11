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
cp $COROOT/include/OShell.h .
#
# Wo
#
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f OEvent.h -n CiBindOEvent -o ../gen/OEvent_B.c
$CIBIN/cigen.exe -p -f OCyclic.h -n CiBindOCyclic -o ../gen/OCyclic_B.c
$CIBIN/cigen.exe -p -f OInterpreter.h -n CiBindOInterpreter -o ../gen/OInterpreter_B.c
$CIBIN/cigen.exe -p -h CList.h -h OTrace.h -f OInterface.h -n CiBindOInterface -o ../gen/OInterface_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f OWidget.h -n CiBindOWidget -o ../gen/OWidget_B.c
$CIBIN/cigen.exe -p -h X11/Xlib.h -h OTrace.h -h OShell.h -f Wo.h -n CiBindWo -o ../gen/Wo_B.c
$CIBIN/cigen.exe -p -f WoTypes.h -n CiBindWoTypes -o ../gen/WoTypes_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f OClass.h -n CiBindOClass -o ../gen/OClass_B.c
$CIBIN/cigen.exe -p -f OPart.h -n CiBindOPart -o ../gen/OPart_B.c
$CIBIN/cigen.exe -p -h stdarg.h -f OPiece.h -n CiBindOPiece -o ../gen/OPiece_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f OResource.h -n CiBindOResource -o ../gen/OResource_B.c
$CIBIN/cigen.exe -p -f OTrace.h -n CiBindOTrace -o ../gen/OTrace_B.c
#$CIBIN/cigen.exe -p -f OCi.h -n CiBindOCi -o ../gen/OCi_B.c
#$CIBIN/cigen.exe -p -h CList.h -f HoXz.h -n CiBindHoXz -o ../gen/HoXz_B.c
/bin/rm -f CList.h
/bin/rm -f FString.h
/bin/rm -f OShell.h
cd ../mgr
/bin/rm -f ../src/WoBinds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/WoBinds.c ; done
cd ../mgr
#
# X11
#
/bin/rm -f ../gen/*
$CIBIN/cigen.exe -p -f X11/Xlib.h -n CiBindXlib -o ../gen/Xlib_B.c
$CIBIN/cigen.exe -p -h X11/Xlib.h -f X11/Xresource.h -n CiBindXresource -o ../gen/Xres_B.c
$CIBIN/cigen.exe -p -h X11/Xlib.h -f X11/Xutil.h -n CiBindXutil -o ../gen/Xutil_B.c
$CIBIN/cigen.exe -p -h X11/Xlib.h -h X11/Xresource.h -f X11/Intrinsic.h -n CiBindIntrinsic -o ../gen/Intrins_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f X11/Shell.h -n CiBindShell -o ../gen/Shell_B.c
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f X11/Vendor.h -n CiBindVendor -o ../gen/Vendor_B.c
/bin/rm -f ../src/X11Binds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/X11Binds.c ; done
cd ../mgr
#
# Xm
#
/bin/rm -f ../gen/*
cat <<end > ../gen/Xm_B.c
#ifdef HAS_XM
end
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f Xm/Xm.h -n CiBindXm -o x.lis
cat x.lis >> ../gen/Xm_B.c
cat <<end >> ../gen/Xm_B.c
#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
end
/bin/rm -f x.lis

cat <<end > ../gen/Form_B.c
#ifdef HAS_XM
end
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f Xm/Form.h -n CiBindForm -o x.lis
cat x.lis >> ../gen/Form_B.c
cat <<end >> ../gen/Form_B.c
#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
end
/bin/rm -f x.lis

cat <<end > ../gen/List_B.c
#ifdef HAS_XM
end
$CIBIN/cigen.exe -p -h Xm/Xm.h -f Xm/List.h -n CiBindList -o x.lis
cat x.lis >> ../gen/List_B.c
cat <<end >> ../gen/List_B.c
#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
end
/bin/rm -f x.lis

cat <<end > ../gen/RowCol_B.c
#ifdef HAS_XM
end
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f Xm/RowColumn.h -n CiBindRowColumn -o x.lis
cat x.lis >> ../gen/RowCol_B.c
cat <<end >> ../gen/RowCol_B.c
#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
end
/bin/rm -f x.lis

cat <<end > ../gen/Scale_B.c
#ifdef HAS_XM
end
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f Xm/Scale.h -n CiBindScale -o x.lis
cat x.lis >> ../gen/Scale_B.c
cat <<end >> ../gen/Scale_B.c
#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
end
/bin/rm -f x.lis

cat <<end > ../gen/Text_B.c
#ifdef HAS_XM
end
$CIBIN/cigen.exe -p -h Xm/Xm.h -f Xm/Text.h -n CiBindText -o x.lis
cat x.lis >> ../gen/Text_B.c
cat <<end >> ../gen/Text_B.c
#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
end
/bin/rm -f x.lis

cat <<end > ../gen/ToggleB_B.c
#ifdef HAS_XM
end
$CIBIN/cigen.exe -p -h X11/Intrinsic.h -f Xm/ToggleB.h -n CiBindToggleB -o x.lis
cat x.lis >> ../gen/ToggleB_B.c
cat <<end >> ../gen/ToggleB_B.c
#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
end
/bin/rm -f x.lis

cat <<end > ../gen/XmTools_B.c
#ifdef HAS_XM
end
$CIBIN/cigen.exe -p -h stdarg.h -h stdio.h -h Xm/Xm.h -f XmTools.h -n CiBindXmTools -o x.lis
cat x.lis >> ../gen/XmTools_B.c
cat <<end >> ../gen/XmTools_B.c
#else /*HAS_XM*/
#include <stddef.h>
#endif /*HAS_XM*/
end
/bin/rm -f x.lis
/bin/rm -f ../src/XmBinds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/XmBinds.c ; done
cd ../mgr
#
# gl
#
/bin/rm -f ../gen/*
$CIBIN/cigen.exe -p -f GL/gl.h  -n CiBindgl  -o ../gen/gl_B.c
$CIBIN/cigen.exe -p -f GL/glu.h -n CiBindglu -o ../gen/glu_B.c
$CIBIN/cigen.exe -p -h X11/Xlib.h -h X11/Xutil.h -h GL/gl.h -f GL/glx.h -n CiBindglx -o ../gen/glx_B.c
/bin/rm -f ../src/GL_Binds.c
cd ../gen
files=`ls *_B.c`
for file in $files ; do cat $file >> ../src/GL_Binds.c ; done
cd ../mgr
#
#
#
