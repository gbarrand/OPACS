$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!  Usage :
$!    VMS$ @setup
$! or :
$!    VMS$ @setup <config>
$!  Example :
$!    VMS$ @setup ALPHA
$!  <config> must be declared in the Config.mms files (Co/v3/mgr, HTMLo/v3/mgr, W3o/v3/mgr).
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ root = "dsksi:[barrand.projects"
$!
$ define/nolog/translation=(concealed) COROOT    "''root'.Co.v3.]"
$ define/nolog/translation=(concealed) W3OROOT   "''root'.W3o.v3.]"
$ define/nolog/translation=(concealed) HTMLOROOT "''root'.HTMLo.v3.]"
$ define/nolog/translation=(concealed) HOROOT    "''root'.Ho.v3.]"
$ define/nolog/translation=(concealed) GLOROOT   "''root'.GLo.v3.]"
$! Define GL so that compiler find #include <GL/gl.h> 
$ define/nolog                         GL        GLOROOT:[usr.include.gl]
$ define/nolog/translation=(concealed) GOROOT    "''root'.Go.v3.]"
$ define/nolog/translation=(concealed) XXROOT    "''root'.Xx.v3.]"
$ define/nolog/translation=(concealed) XOROOT    "''root'.Xo.v3.]"
$ define/nolog/translation=(concealed) XZROOT    "''root'.Xz.v3.]"
$ define/nolog/translation=(concealed) G3OROOT   "''root'.G3o.v3.]"
$ define/nolog/translation=(concealed) G4OROOT   "''root'.G4o.v3.]"
$ define/nolog/translation=(concealed) WOROOT    "''root'.Wo.v3.]"
$!
$! Wo+Ci
$! setenv CSETROOT     /lal/CSet/v2r3
$! setenv CIROOT       /lal/Ci/v5r1
$! setenv CIBIN        $CIROOT/$HOSTTYPE
$! setenv CISTDINCLUDE $CIROOT/usr/include
$! setenv CIPATH       "$COROOT/include $XXROOT/include $WOROOT/include $WOROOT/usr"
$! setenv CIPATH       "$CIPATH $XOROOT/include $HOROOT/include $GOROOT/include"
$!
$! Run time variables :
$!  OPATH         : optional.
$!  COPATH        : optional.
$!  COSECURED     : optional.
$!  COVERSION     : optional.
$!  COSYSKIND     : optional.
$!  GOVERSION     : optional.
$!  GOPATH        : optional.
$!  XOFITSSWAP    : optional.
$!  WOVERSION     : optional. 
$!  WOENVIRONMENT : optional.
$!  WOPATH        : optional.
$!
$ define/nolog OPATH "COROOT:[usr] HOROOT:[usr] GOROOT:[usr] XOROOT:[usr] XZROOT:[usr] G3OROOT:[usr] WOROOT:[usr]"
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!! Depend on config. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$ if p1 .nes. "" 
$ then
$  config = p1
$ else
$  config = f$getsyi("ARCH_NAME")
$ endif
$! write sys$output config
$!
$! Make environment :
$!
$! To build shared libs (used in mgr/lib<>Exe.opt) :
$ define/nolog COBIN    "COROOT:[''config']"
$ define/nolog HOBIN    "HOROOT:[''config']"
$ define/nolog W3OBIN   "W3OROOT:[''config']"
$ define/nolog HTMLOBIN "HTMLOROOT:[''config']"
$ define/nolog GLOBIN   "GLOROOT:[''config']"
$ define/nolog GOBIN    "GOROOT:[''config']"
$ define/nolog XXBIN    "XXROOT:[''config']"
$ define/nolog XOBIN    "XOROOT:[''config']"
$ define/nolog WOBIN    "WOROOT:[''config']"
$!
$! Run time :
$!
$! To activate shared libs :
$ define/nolog libCo    "COROOT:[''config']libCo.exe"
$ define/nolog libHo    "HOROOT:[''config']libHo.exe"
$ define/nolog libW3o   "W3OROOT:[''config']libW3o.exe"
$ define/nolog libHTMLo "HTMLOROOT:[''config']libHTMLo.exe"
$ define/nolog libGLo   "GLOROOT:[''config']libGLo.exe"
$ define/nolog libGLUo  "GLOROOT:[''config']libGLUo.exe"
$ define/nolog libGLPSo "GLOROOT:[''config']libGLPSo.exe"
$ define/nolog libGLXo  "GLOROOT:[''config']libGLXo.exe"
$ define/nolog libGo    "GOROOT:[''config']libGo.exe"
$ define/nolog libXx    "XXROOT:[''config']libXx.exe"
$ define/nolog libXo    "XOROOT:[''config']libXo.exe"
$ define/nolog libWo    "WOROOT:[''config']libWo.exe"
$ define/nolog libWoXm  "WOROOT:[''config']libWoXm.exe"
$ define/nolog libWoGL  "WOROOT:[''config']libWoGL.exe"
$ define/nolog libWoCi  "WOROOT:[''config']libWoCi.exe"
$ define/nolog libWoXaw "WOROOT:[''config']libWoXaw.exe"
$
$ omake        == "$COROOT:[''config']omake.exe"
$ oshow        == "$WOROOT:[''config']oshow.exe"
$ oapplication == "$WOROOT:[''config']oshow.exe NewApp.odb"
$ define/nolog OPATH "COROOT:[usr] HOROOT:[usr] GOROOT:[usr] XOROOT:[usr] XZROOT:[usr] G3OROOT:[usr] WOROOT:[usr] XZROOT:[''config']"
$!