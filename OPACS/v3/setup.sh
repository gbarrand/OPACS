# Usage :
#     sh> . ./setup.sh
#
#set -x
root=/projects
#
vers=v3
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Make environment (needed if building new applications) :
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
COROOT=$root/Co/$vers;export COROOT
W3OROOT=$root/W3o/$vers;export W3OROOT
HTMLOROOT=$root/HTMLo/$vers;export HTMLOROOT
HOROOT=$root/Ho/$vers;export HOROOT
GLOROOT=$root/GLo/$vers;export GLOROOT
GOROOT=$root/Go/$vers;export GOROOT
XXROOT=$root/Xx/$vers;export XXROOT
XOROOT=$root/Xo/$vers;export XOROOT
XZROOT=$root/Xz/$vers;export XZROOT
G3OROOT=$root/G3o/$vers;export G3OROOT
WOROOT=$root/Wo/$vers;export WOROOT
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Optional connection to other packages :
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# methods : optional, necessary if using Ci, Cm.
if [ -z "${METHODSCONFIG}" ] ; then
. $root/methods/v3r1/mgr/setup.sh
fi
# Ci : optional, could be used in Ho, Wo.
CSETROOT=$root/CSet/v2r5;export CSETROOT
CIROOT=$root/Ci/v5r2;export CIROOT
# Cm : optional, could be used by Ho, Xz.
CMROOT=$root/Cm/v7r3;export CMROOT
CMMGR=$CMROOT/mgr;export CMMGR
CMDOMAIN="LAL";export CMDOMAIN
# CERN : optional, used by Xz, G3o.
CERNLIB=/cern/pro/lib;export CERNLIB
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Run time environment (do not depend on upper ROOT variables) :
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
#  OPATH         : optional.
#  COPATH        : optional.
#  COSECURED     : optional.
#  COVERSION     : optional.
#  COSYSKIND     : optional.
#  GOVERSION     : optional.
#  GOPATH        : optional.
#  XOFITSSWAP    : optional.
#  WOVERSION     : optional. 
#  WOENVIRONMENT : optional.
#  WOPATH        : optional.
#
# Ci
CISTDINCLUDE=${CIROOT}/usr/include;export CISTDINCLUDE
CIPATH="${COROOT}/include ${XXROOT}/include ${WOROOT}/include ${WOROOT}/usr ${XOROOT}/include ${HOROOT}/include ${GOROOT}/include";export CIPATH
#
#!!!!!!!!!!!!!!!!!!!!!!!!!!
#!!! Depend on OCONFIG. !!!
#!!!!!!!!!!!!!!!!!!!!!!!!!!
#
if [ -z "${OCONFIG}" ] ; then
OCONFIG=`uname`;export OCONFIG
fi
#
# Xz, G3o
# Add Xz/$vers/${OCONFIG} so that phan.rz could be found.
OPATH="${COROOT}/usr ${WOROOT}/usr ${HOROOT}/usr ${GOROOT}/usr ${XOROOT}/usr ${XZROOT}/usr ${G3OROOT}/usr ${XZROOT}/${OCONFIG}";export OPATH
# JAVA
if [ -z "${CLASSPATH}" ] ; then
  CLASSPATH=../${OCONFIG};export CLASSPATH
else
  status=1;(echo $CLASSPATH | grep "../${OCONFIG}" >  /dev/null  ) && status=0
  if [ $status != 0 ] ; then
    CLASSPATH=${CLASSPATH}:../${OCONFIG};export CLASSPATH
  fi
fi
#
# AIX
#
if test `uname` = AIX
then
MALLOCTYPE="3.1";export MALLOCTYPE
fi
#
# Set LD_LIBRARY_PATH for Linux, IRIX, IRIX64
#
 os=`uname`
if [ $os = "Linux"  -o   $os = "IRIX"  -o  $os = "IRIX64" ] ; then
#  To find shared libs.
#  If you are superuser use the tool ldconfig to
# declare the "o" shared libs to the system and then
# avoid the use of LD_LIBRARY_PATH.
string="${COROOT}/${OCONFIG}:${W3OROOT}/${OCONFIG}:${HTMLOROOT}/${OCONFIG}:${HOROOT}/${OCONFIG}:${GLOROOT}/${OCONFIG}:${GOROOT}/${OCONFIG}:${XXROOT}/${OCONFIG}:${XOROOT}/${OCONFIG}:${WOROOT}/${OCONFIG}"
if [ -z "${LD_LIBRARY_PATH}" ] ; then
  LD_LIBRARY_PATH=$string;export LD_LIBRARY_PATH
else
  status=1;(echo $LD_LIBRARY_PATH | grep "$string" >& /dev/null  ) && status=0
  if [ $status != 0 ] ; then
    LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$string;export LD_LIBRARY_PATH
  fi
fi
unset string
fi
unset os
#
# Exit :
unset root
unset vers



