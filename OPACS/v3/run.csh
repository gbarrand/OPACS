# Usage :
#     csh> source run.csh
#
set root=/projects
set vers=v3
#
# Linux
#
if ( `uname` == Linux ) then
setenv LD_PRELOAD $root/Wo/$vers/${OCONFIG}/libWoJAVA.so:$root/Wo/$vers/${OCONFIG}/libWoXm.so:$root/Wo/$vers/${OCONFIG}/libWoGL.so:$root/Wo/$vers/${OCONFIG}/libWo.so:$root/Xo/$vers/${OCONFIG}/libXo.so:$root/Go/$vers/${OCONFIG}/libGo.so:$root/GLo/$vers/${OCONFIG}/libGLUo.so:$root/GLo/$vers/${OCONFIG}/libGLXo.so:$root/GLo/$vers/${OCONFIG}/libGLo.so:$root/Xx/$vers/${OCONFIG}/libXx.so:$root/Ho/$vers/${OCONFIG}/libHo.so:$root/Co/$vers/${OCONFIG}/libCo.so:$root/W3o/$vers/${OCONFIG}/libW3o.so:$root/HTMLo/$vers/${OCONFIG}/libHTMLo.so:/usr/X11R6/lib/libXaw.so:/usr/X11R6/lib/libXm.so.2.0:/usr/X11R6/lib/libXmu.so:/usr/X11R6/lib/libXt.so:/usr/X11R6/lib/libXext.so:/usr/X11R6/lib/libX11.so:/usr/lib/libm.so
#:/usr/local/JDK/rt-1.1.3/lib/i686/green_threads/libjava.so
#:/usr/local/lib/netscape/libc.so.5.0.9  
#:/usr/local/JDK/jdk-1.1.3/lib/i686/green_threads/libjava.so
#:/usr/local/JDK/jdk-1.1.3/lib/i686/green_threads/libc.so.5
#:${HOME}/Linux/lib/libgcc.so
endif
#
# OSF1
#
if ( `uname` == OSF1 ) then
setenv _RLD_LIST $root/Wo/$vers/${OCONFIG}/libWoJAVA.so:$root/Wo/$vers/${OCONFIG}/libWoXm.so:$root/Wo/$vers/${OCONFIG}/libWoGL.so:$root/Wo/$vers/${OCONFIG}/libWo.so:$root/Xo/$vers/${OCONFIG}/libXo.so:$root/Go/$vers/${OCONFIG}/libGo.so:$root/GLo/$vers/${OCONFIG}/libGLUo.so:$root/GLo/$vers/${OCONFIG}/libGLXo.so:$root/GLo/$vers/${OCONFIG}/libGLo.so:$root/Xx/$vers/${OCONFIG}/libXx.so:$root/Ho/$vers/${OCONFIG}/libHo.so:$root/Co/$vers/${OCONFIG}/libCo.so:$root/W3o/$vers/${OCONFIG}/libW3o.so:$root/HTMLo/$vers/${OCONFIG}/libHTMLo.so:/usr/shlib/libXaw.so:/usr/shlib/libXm.so:/usr/shlib/libXmu.so:/usr/shlib/libXt.so:/usr/shlib/libXext.so:/usr/shlib/libX11.so:DEFAULT
endif
# Exit :
unset root
unset vers










