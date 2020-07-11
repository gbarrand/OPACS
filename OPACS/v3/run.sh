# Usage :
#     sh> . ./run.csh
#
root=/projects
vers=v3
#
# Linux
if test `uname` = Linux
then
LD_PRELOAD=$root/Wo/$vers/${OCONFIG}/libWoJAVA.so:$root/Wo/$vers/${OCONFIG}/libWoXm.so:$root/Wo/$vers/${OCONFIG}/libWoGL.so:$root/Wo/$vers/${OCONFIG}/libWo.so:$root/Xo/$vers/${OCONFIG}/libXo.so:$root/Go/$vers/${OCONFIG}/libGo.so:$root/GLo/$vers/${OCONFIG}/libGLUo.so:$root/GLo/$vers/${OCONFIG}/libGLXo.so:$root/GLo/$vers/${OCONFIG}/libGLo.so:$root/Xx/$vers/${OCONFIG}/libXx.so:$root/Ho/$vers/${OCONFIG}/libHo.so:$root/Co/$vers/${OCONFIG}/libCo.so:$root/W3o/$vers/${OCONFIG}/libW3o.so:$root/HTMLo/$vers/${OCONFIG}/libHTMLo.so:/usr/X11R6/lib/libXaw.so:/usr/X11R6/lib/libXm.so.2.0:/usr/X11R6/lib/libXmu.so:/usr/X11R6/lib/libXt.so:/usr/X11R6/lib/libXext.so:/usr/X11R6/lib/libX11.so:/usr/lib/libm.so;export LD_PRELOAD
fi
#
# OSF1
if test `uname` = OSF1
then
_RLD_LIST=$root/Wo/$vers/${OCONFIG}/libWoJAVA.so:$root/Wo/$vers/${OCONFIG}/libWoXm.so:$root/Wo/$vers/${OCONFIG}/libWoGL.so:$root/Wo/$vers/${OCONFIG}/libWo.so:$root/Xo/$vers/${OCONFIG}/libXo.so:$root/Go/$vers/${OCONFIG}/libGo.so:$root/GLo/$vers/${OCONFIG}/libGLUo.so:$root/GLo/$vers/${OCONFIG}/libGLXo.so:$root/GLo/$vers/${OCONFIG}/libGLo.so:$root/Xx/$vers/${OCONFIG}/libXx.so:$root/Ho/$vers/${OCONFIG}/libHo.so:$root/Co/$vers/${OCONFIG}/libCo.so:$root/W3o/$vers/${OCONFIG}/libW3o.so:$root/HTMLo/$vers/${OCONFIG}/libHTMLo.so:/usr/shlib/libXaw.so:/usr/shlib/libXm.so:/usr/shlib/libXmu.so:/usr/shlib/libXt.so:/usr/shlib/libXext.so:/usr/shlib/libX11.so:DEFAULT;export _RLD_LIST
fi
#
# Exit :
unset root
unset vers






