bin=[-.$(CONFIG)]
mgr=[-.mgr]
src=[-.src]
app=[-.app]
inlib=@COROOT:[mgr]inlib.com
make_so=@COROOT:[mgr]maklibs.com
Ho_CPPINCLUDES=COROOT:[include],[-.include]
Ho_CPPDEFINES=$(CPPDEFINES)
APP_CPPINCLUDES=NOTHING
APP_CCDEFINES=NOTHING
libc=COROOT:[mgr]libc/opt
libm=COROOT:[mgr]libm/opt
libCo=COROOT:[$(CONFIG)]libCo/opt
libHo=HOROOT:[$(CONFIG)]libHo/opt
