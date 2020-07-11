#!/bin/sh -f
#
# Usage :
#   UNIX> ./run.sh <task>
#  Linux> ./run.sh oshow.exe
#
opacs=$COROOT/$OCONFIG:$HOROOT/$OCONFIG:$HTMLOROOT/$OCONFIG:$W3OROOT/$OCONFIG:$XOROOT/$OCONFIG:$GLOROOT/$OCONFIG:$GOROOT/$OCONFIG:$XXROOT/$OCONFIG:$WOROOT/$OCONFIG
#
if test `uname` = "Linux"
then
LD_LIBRARY_PATH=$opacs;export LD_LIBRARY_PATH
fi
#
if test `uname` = "SunOS"
then
LD_LIBRARY_PATH=$opacs;export LD_LIBRARY_PATH
fi
#
#echo $#
if test $# = "1"
then
echo run
$WOROOT/$OCONFIG/$1
fi
if test $# = "2"
then
printenv $LD_LIBRARY_PATH
$WOROOT/$OCONFIG/$1 $2
fi
#
