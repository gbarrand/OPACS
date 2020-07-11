#!/bin/sh -f
#set -x
############################################
if test $# -ge 2 ; then
libname=$1
bindir=$2
liba=$bindir/$libname.a
############################################
##### build libs ###########################
############################################
if test -f $liba ; then

if test `uname` = "OSF1" ; then
#  Run time environment variable _RLD_LIST 
# equal to a colon separated list of so libs.
cd $bindir
rpath=`pwd`
rpath=`echo $rpath | sed -e 's%/tmp_mnt%%'`
libso=$rpath/$libname.so
/bin/rm -f $libso
/bin/rm -f so_locations
ld -expect_unresolved '*' -shared -o $libso -all $liba -rpath $rpath
cd ../mgr
echo "$libso built."
fi

if test `uname` = "IRIX" ; then
#  Run time environment variable _RLD_LIST  
# equal to a colon separated list of so libs.
cd $bindir
rpath=`pwd`
rpath=`echo $rpath | sed -e 's%/tmp_mnt%%'`
libso=$rpath/$libname.so
/bin/rm -f $libso
/bin/rm -f so_locations
ld -shared -o $libso -all $liba
cd ../mgr
echo "$libso built."
fi

if test `uname` = "HP-UX" ; then
#  Binaries must be loaded with the option +s.
#  Run time environment variable SHLIB_PATH 
# equal to a colon separated list of directories.
#set -x
cd $bindir
rpath=`pwd`
rpath=`echo $rpath | sed -e 's%/tmp_mnt%%'`
libso=$rpath/$libname.sl
/bin/rm -f $libso
# get .o files
if test ! -d tmp ; then mkdir tmp ; fi
cd tmp;/bin/rm -f *.o;ar x ../$liba;cd ..
objs=`ls tmp/*.o`
sys=`uname -r`
name=`echo $sys | awk '{name=substr($0,1,1);print name;}'`
if test $name = "A" ; then
ld -b -a shared -o $libso $objs
else
ld -b +h $libso -a shared -o $libso $objs
fi
/bin/rm -f tmp/*.o
/bin/rmdir -f tmp
cd ../mgr
echo "$libso built."
fi

if test `uname` = "Linux" ; then
#  Run time environment variable LD_LIBRARY_PATH 
# equal to a colon separated list of directories.
#  If you are superuser, use the tool ldconfig to 
# declare the created shared lib to the system and then 
# avoid the use of LD_LIBRARY_PATH.
#set -x
cd $bindir
rpath=`pwd`
rpath=`echo $rpath | sed -e 's%/tmp_mnt%%'`
libso=$rpath/$libname.so
/bin/rm -f $libso
ld -shared -o $libso --whole-archive $liba
cd ../mgr
echo "$libso built."
fi

if test `uname` = "Darwin" ; then
#  Run time environment variable LD_LIBRARY_PATH 
# equal to a colon separated list of directories.
#  If you are superuser, use the tool ldconfig to 
# declare the created shared lib to the system and then 
# avoid the use of LD_LIBRARY_PATH.
#set -x
cd $bindir
# get .o files
if test ! -d tmp ; then mkdir tmp ; fi
cd tmp;/bin/rm -f *.o;ar x ../$liba;cd ..
objs=`ls tmp/*.o`
#rpath=`pwd`
#rpath=`echo $rpath | sed -e 's%/tmp_mnt%%'`
#libso=$rpath/$libname.dylib
libso=$libname.dylib
/bin/rm -f $libso
#cc -dynamiclib -flat_namespace -undefined suppress -o $libso $objs
cc -dynamiclib -twolevel_namespace -undefined error -dynamic -single_module -o $libso $objs
/bin/rm -f tmp/*.o
/bin/rm -Rf tmp
cd ../mgr
echo "$libso built."
fi

fi
############################################
else
echo 'Give name of binaray directory and library'
fi
