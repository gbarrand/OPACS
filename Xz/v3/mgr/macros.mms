bin=[-.$(CONFIG)]
mgr=[-.mgr]
src=[-.src]
app=[-.app]
inlib=@COROOT:[mgr]inlib.com
make_so=@COROOT:[mgr]maklibs.com
Xz_CPPFLAGS=/prefix=(except=gethostname)
Xz_CPPINCLUDES=COROOT:[include],XXROOT:[include],[-.include]
Xz_CPPDEFINES=$(CPPDEFINES)
CERN_CPPINCLUDES=NOTHING
CERN_CPPDEFINES=NOTHING
APP_CPPINCLUDES=NOTHING
APP_CCDEFINES=HAS_X
libc=COROOT:[mgr]libc/opt
libm=COROOT:[mgr]libm/opt
libf77=COROOT:[mgr]libf77/opt
libX11=COROOT:[mgr]X11/opt
libXext=COROOT:[mgr]Xext/opt
libXt=COROOT:[mgr]Xt/opt
libXmu=COROOT:[mgr]Xmu/opt
libgraflib=cern$library:graflib/lib
libgrafX11=cern$library:grafx11/lib
libpacklib=cern$library:packlib/lib
libkernlib=cern$library:kernlib/lib
libCo=COROOT:[$(CONFIG)]libCo/opt
libXx=XXROOT:[$(CONFIG)]libXx/opt
libXz=XZROOT:[$(CONFIG)]libXz/opt
libXzPAWC=XZROOT:[$(CONFIG)]libXzPAWC/opt

