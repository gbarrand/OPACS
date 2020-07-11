# Usage :
#     csh> source setup.csh
#
#set root=/lal
set root=/barrand/install/OPACS
set vers=/v3
#set vers=
#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Make environment (needed if building new applications) :
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
setenv COROOT    $root/Co${vers}
setenv W3OROOT   $root/W3o${vers}
setenv HTMLOROOT $root/HTMLo${vers}
setenv HOROOT    $root/Ho${vers}
setenv GLOROOT   $root/GLo${vers}
setenv GOROOT    $root/Go${vers}
setenv XXROOT    $root/Xx${vers}
setenv XOROOT    $root/Xo${vers}
setenv XZROOT    $root/Xz${vers}
setenv G3OROOT   $root/G3o${vers}
setenv WOROOT    $root/Wo${vers}
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Optional connection to other packages :
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# methods : optional, necessary if using Ci, Cm.
#if ( "$?METHODSCONFIG" == 0 ) then
#  source $root/methods/v3r1/mgr/setup.csh
#endif
# Ci : optional, could be used in Ho, Wo.
setenv CSETROOT     $root/CSet/v2r5
setenv CIROOT       $root/Ci/v5r2
# Cm : optional, could be used by Ho, Xz.
setenv CMROOT       $root/Cm/v7r4
setenv CMMGR        $CMROOT/mgr
setenv CMDOMAIN     "LAL"
# CERN : optional, used by Xz, G3o.
setenv CERNLIB   /cern/pro/lib
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
setenv CISTDINCLUDE ${CIROOT}/usr/include
setenv CIPATH "${COROOT}/include ${XXROOT}/include ${WOROOT}/include ${WOROOT}/usr ${XOROOT}/include ${HOROOT}/include ${GOROOT}/include"
#
#!!!!!!!!!!!!!!!!!!!!!!!!!!
#!!! Depend on OCONFIG. !!!
#!!!!!!!!!!!!!!!!!!!!!!!!!!
#
if ( $?OCONFIG ) then
else
setenv OCONFIG `uname`
endif
#
# G3o, Xz
# Add Xz${vers}/${OCONFIG} so that phan.rz could be found. 
setenv OPATH "${COROOT}/usr ${WOROOT}/usr ${HOROOT}/usr ${GOROOT}/usr ${XOROOT}/usr ${XZROOT}/usr ${G3OROOT}/usr ${XZROOT}/${OCONFIG}"
# JAVA : to compile. 
#  Use run.csh at run time.
if ( $?CLASSPATH ) then
  echo $CLASSPATH | grep "../${OCONFIG}" >& /dev/null
  if ( $status != 0 ) then
    setenv CLASSPATH ${CLASSPATH}:../${OCONFIG}
  endif
else
  setenv CLASSPATH ../${OCONFIG}
endif
#
# AIX
#
if ( `uname` == AIX ) then
setenv MALLOCTYPE  "3.1"
endif
#
# Set LD_LIBRARY_PATH for Linux, IRIX, IRIX64
#
set string="${COROOT}/${OCONFIG}:${W3OROOT}/${OCONFIG}:${HTMLOROOT}/${OCONFIG}:${HOROOT}/${OCONFIG}:${GLOROOT}/${OCONFIG}:${GOROOT}/${OCONFIG}:${XXROOT}/${OCONFIG}:${XOROOT}/${OCONFIG}:${WOROOT}/${OCONFIG}:/usr/X11R6/lib"
set os=`uname`
if ( $os == "Linux"  || $os == "IRIX" || $os == "IRIX64" )  then
if ( $?LD_LIBRARY_PATH ) then
  echo $LD_LIBRARY_PATH | grep "$string" >& /dev/null
  if ( $status != 0 ) then
    setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$string
  endif
else
  setenv LD_LIBRARY_PATH $string
endif
endif

if ( $os == "Darwin"  )  then
if ( $?DYLD_LIBRARY_PATH ) then
  echo $DYLD_LIBRARY_PATH | grep "$string" >& /dev/null
  if ( $status != 0 ) then
    setenv DYLD_LIBRARY_PATH ${DYLD_LIBRARY_PATH}:$string
  endif
else
  setenv DYLD_LIBRARY_PATH $string
endif
endif

unset string
unset os
#
# Aliases
#
alias  omake        "$COROOT/${OCONFIG}/omake.exe"
alias  oshow        "$WOROOT/${OCONFIG}/oshow.exe"
alias  oapplication "oshow NewApp.odb"
alias  onew         "$WOROOT/${OCONFIG}/onew.exe"
# Exit :
unset root
unset vers
