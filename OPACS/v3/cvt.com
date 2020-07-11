$! Usage :
$!   VMS$ @cvt *.odb;*
$! The cvt.com itself should be converted :
$!   VMS$ @cvt cvt.com
$!
$ loop:
$ file = f$search(P1)
$ file = file - f$parse(file,,,"VERSION")
$ if file.eqs."" then exit
$ write sys$output file
$ convert/fdl=sys$input 'file' 'file'
RECORD
        FORMAT                  stream_lf
$ goto loop