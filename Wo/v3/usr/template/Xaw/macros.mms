bin=[]
mgr=[]
src=[]
app=[]
inlib=@COROOT:[mgr]inlib.com
make_so=@COROOT:[mgr]maklibs.com
OApp_CPPINCLUDES=[]
OApp_CPPDEFINES=$(CPPDEFINES)
OApp_Wo_CPPINCLUDES=COROOT:[include],XXROOT:[include],WOROOT:[include]
OApp_Wo_CPPDEFINES=NOTHING
APP_CPPINCLUDES=NOTHING
APP_CCDEFINES=NOTHING
libWo=WOROOT:[$(CONFIG)]libWo/opt
libc=COROOT:[mgr]libc/opt
libm=COROOT:[mgr]libm/opt
libX11=COROOT:[mgr]X11/opt
libXext=COROOT:[mgr]Xext/opt
libXt=COROOT:[mgr]Xt/opt
libXmu=COROOT:[mgr]Xmu/opt
libCo=COROOT:[$(CONFIG)]libCo/opt
libXx=XXROOT:[$(CONFIG)]libXx/opt
OApp_Xm_CPPINCLUDES=NOTHING
OApp_Xm_CPPDEFINES=HAS_XM
libWoXm=WOROOT:[$(CONFIG)]libWoXm/opt
libXm=COROOT:[mgr]Xm/opt
OApp_Xaw_CPPINCLUDES=NOTHING
OApp_Xaw_CPPDEFINES=HAS_XAW
libWoXaw=WOROOT:[$(CONFIG)]libWoXaw/opt
libXaw=COROOT:[mgr]Xaw/opt
OApp_Xo_CPPINCLUDES=XOROOT:[include],GOROOT:[include],GLOROOT:[usr.include],HOROOT:[include]
OApp_Xo_CPPDEFINES=HAS_XO
libWoGL=WOROOT:[$(CONFIG)]libWoGL/opt
libXo=XOROOT:[$(CONFIG)]libXo/opt
libHo=HOROOT:[$(CONFIG)]libHo/opt
libGL=GLOROOT:[$(CONFIG)]libGLo/opt
libGLU=GLOROOT:[$(CONFIG)]libGLUo/opt
libGLX=GLOROOT:[$(CONFIG)]libGLXo/opt
libGo=GOROOT:[$(CONFIG)]libGo/opt
OApp_HTML_CPPINCLUDES=NOTHING
OApp_HTML_CPPDEFINES=HAS_HTML
libwww=W3OROOT:[$(CONFIG)]libw3o/opt
libhtmlw=HTMLOROOT:[$(CONFIG)]libHTMLo/opt
OApp_Xz_CPPINCLUDES=XZROOT:[include],HOROOT:[include]
OApp_Xz_CPPDEFINES=HAS_XZ,HAS_HO
libWoHoXz=WOROOT:[$(CONFIG)]libWoHoXz/opt
libXz=XZROOT:[$(CONFIG)]libXz/opt
libXzPAWC=XZROOT:[$(CONFIG)]libXzPAWC/opt
libgraflib=cern$library:graflib/lib
libgrafX11=cern$library:grafx11/lib
libpacklib=cern$library:packlib/lib
libkernlib=cern$library:kernlib/lib
libmathlib=cern$library:mathlib/lib
OApp_PAW_CPPINCLUDES=NOTHING
OApp_PAW_CPPDEFINES=HAS_PAW
libWoPAW=WOROOT:[$(CONFIG)]libWoPAW/opt
libpawlib=cern$library:pawlib/lib
OApp_G3o_CPPINCLUDES=G3OROOT:[include]
OApp_G3o_CPPDEFINES=HAS_G3O
libf77=COROOT:[mgr]libf77/opt
libgeant=cern$library:geant321/lib
libG3o=G3OROOT:[$(CONFIG)]libG3o/opt
libG3oEXPO=G3OROOT:[$(CONFIG)]libG3oEXPO/opt
OApp_Ci_CPPINCLUDES=CSETROOT:[src],CIROOT:[src]
OApp_Ci_CPPDEFINES=HAS_CI
libWoCi=WOROOT:[$(CONFIG)]libWoCi/opt
libCi=COROOT:[$(CONFIG)]Ci/opt
libCSet=COROOT:[$(CONFIG)]CSet/opt
OApp_FNAL_CPPINCLUDES=NOTHING
OApp_FNAL_CPPDEFINES=HAS_FNAL
libWoFNAL=WOROOT:[$(CONFIG)]libWoFNAL/opt
libFNAL=WOROOT:[$(CONFIG)]FNAL/opt
OApp_tcl_CPPINCLUDES=NOTHING
OApp_tcl_CPPDEFINES=HAS_TCL
libtcl=WOROOT:[$(CONFIG)]tcl/opt


