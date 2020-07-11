bin=[-.$(CONFIG)]
mgr=[-.mgr]
src=[-.src]
app=[-.app]
inlib=@COROOT:[mgr]inlib.com
make_so=@COROOT:[mgr]maklibs.com
G3o_CPPINCLUDES=COROOT:[include],GOROOT:[include],[-.include]
G3o_CPPDEFINES=$(CPPDEFINES)
GL_CPPINCLUDES=GLOROOT:[usr.include]
GL_CPPDEFINES=NOTHING
APP_CPPINCLUDES=NOTHING
APP_CCDEFINES=NOTHING
libc=COROOT:[mgr]libc/opt
libm=COROOT:[mgr]libm/opt
libf77=COROOT:[mgr]libf77/opt
libgeant=cern$library:geant321/lib
libpacklib=cern$library:packlib/lib
libkernlib=cern$library:kernlib/lib
libCo=COROOT:[$(CONFIG)]libCo/opt
libGo=GOROOT:[$(CONFIG)]libGo/opt
libGL=GLOROOT:[$(CONFIG)]libGLo/opt
libGLU=GLOROOT:[$(CONFIG)]libGLUo/opt
libG3o=G3OROOT:[$(CONFIG)]libG3o/opt
libG3oEXPO=G3OROOT:[$(CONFIG)]libG3oEXPO/opt


