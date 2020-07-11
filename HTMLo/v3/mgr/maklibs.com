$   ON ERROR     THEN $ GOTO EXIT
$   ON CONTROL_Y THEN $ GOTO EXIT
$!   set ver
$   if f$trnlnm ("COROOT") .eqs. "" 
$   then 
$     write sys$output "Define COROOT environment variable with :"
$     write sys$output "$ define/nolog/translation=(concealed) COROOT <disk>:[<root>.Co.v3.]"
$     goto exit
$   endif
$   if f$search("COROOT:[mgr]share.com") .eqs. "" 
$   then 
$     write sys$output "COROOT:[mgr]share.com not found."
$     goto exit
$   endif
$!   config = f$getsyi("ARCH_NAME")
$   libname = p1
$   package = p2
$   config  = p3
$   if f$search("COROOT:[''config']mapana.exe") .eqs. "" 
$   then 
$     write sys$output "COROOT:[''config']mapana.exe not found."
$     goto exit
$   endif
$   set def [-.'config']
$   dirbin  = package + "ROOT:[" + config + "]"
$   dexe    = libname + ".EXE;*"
$   if f$search(dexe) .nes. "" then delete 'dexe'
$   dopt    = libname + ".OPT;*"
$   if f$search(dopt) .nes. "" then delete 'dopt'
$   copy [-.mgr]'libname'EXE.opt *
$   define/nolog 'libname' 'dirbin''libname'.exe
$   deassign 'libname'
$   mapana = "$COROOT:[''config']mapana.exe"
$   @COROOT:[mgr]share "''libname'" "''libname'exe/opt" "" "" "debug" "''mapana'"
$   define/nolog 'libname' 'dirbin''libname'.exe
$   dsopt    = libname + "S.OPT;*"
$   if f$search(dsopt) .nes. "" then delete 'dsopt'
$   dsopt    = libname + "EXE.OPT;*"
$   if f$search(dsopt) .nes. "" then delete 'dsopt'
$   libs    = libname + "S.EXE;"
$   lib     = libname + ".EXE"
$   copy    'libs' 'lib'
$   dsexe   = libname + "S.EXE;*"
$   if f$search(dsexe) .nes. "" then delete 'dsexe'
$EXIT:
$   set def [-.mgr]
$   EXIT
