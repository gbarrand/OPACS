#!/bin/csh -f

set name=$1

set e = ( "s/template/${name}/g" )

sed -e ${e} ${OPACSROOT}/src/template.c    >${name}.c
sed -e ${e} ${OPACSROOT}/src/template.odb  >${name}.odb
sed -e ${e} ${OPACSROOT}/src/template.html >${name}.html

