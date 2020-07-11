#!/bin/sh -f
# Usage :
#     UNXI> edit the root variable.
#     UNIX> chmod a+x cvsu.sh
#     UNIX> cvsu.sh
#
CVSROOT=/projects/cvs;export CVSROOT
root=/projects
#
OPACS_vers=v3
W3o_vers=v3
HTMLo_vers=v3
Co_vers=v3
Ho_vers=v3
Xx_vers=v3
Wo_vers=v3
GLo_vers=v3
Go_vers=v3
Xo_vers=v3
Xz_vers=v3
G3o_vers=v3
Mo_vers=v3
G4o_vers=v3
OnX_vers=v7
#
packs='OPACS W3o HTMLo Co Ho Xx Wo GLo Go Xo Xz G3o Mo G4o OnX'
#
for pack in $packs
do 
  echo "------ Looking $pack ------"
  vers=${pack}_vers
  s=`set | grep ${vers} | awk '{i=index($0,"=");s=substr($0,i+1,2);print s;}'`
  vers=`echo $s | awk '{print $1;}'`
  cd ${root}/${pack}/${vers}
  cvs -n update
done
# Exit :
unset root
unset vers
