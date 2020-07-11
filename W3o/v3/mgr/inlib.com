$ ON ERROR     THEN $ GOTO EXIT
$ ON CONTROL_Y THEN $ GOTO EXIT
$!############################################
$ IF P2 .NES. ""
$ THEN
$   libname = p1
$   objname = p2
$   IF F$SEARCH(libname) .EQS. "" THEN library/create 'libname'
$   library/replace 'libname' 'p2'
$ ELSE
$   write sys$output "Give name of library and object files"
$ ENDIF
$!############################################
$EXIT:
$   EXIT


