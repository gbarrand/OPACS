#!/bin/sh -f
# Example : dlldef.sh libCo
#  produces ../mgr/libCo.def
#set -x
############################################
if test $# -lt 1 ; then echo "Give name of library" ; exit 1 ; fi 
libname=$1
bindir=../OSF1
liba=$bindir/$libname.a
############################################
##### build libs ###########################
############################################
if test -f $liba ; then echo "" ; else echo "Library $liba not found " ; exit 1 ; fi 

if test `uname` = "OSF1" ; then
/bin/rm -f $libdef
cd $bindir
libdef=$libname.def
cat << END > $libdef
LIBRARY $libname
EXPORTS 
END
/bin/nm $liba | grep '| T |' | awk '{print " "$1;}' >> $libdef
#/bin/nm $liba | grep '| G |' | awk '{print " "$1;}' >> $libdef
cd ../mgr
/bin/mv $bindir/$libdef .
echo "$libdef built."
fi
