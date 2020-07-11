@echo off
rem # Usage :
rem #     NT> call setup.bat
rem #
set root=f:
rem #
set vers=v3
rem #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
rem # Make environment (needed if building new applications) :
rem #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
set path=%path%;c:\msdev\bin
set include=c:\msdev\include
set lib=c:\msdev\lib
rem #
set COROOT=%root%\Co\%vers%
set W3OROOT=%root%\W3o\%vers%
set HTMLOROOT=%root%\HTMLo\%vers%
set HOROOT=%root%\Ho\%vers%
set GLOROOT=%root%\GLo\%vers%
set GOROOT=%root%\Go\%vers%
set XXROOT=%root%\Xx\%vers%
set XOROOT=%root%\Xo\%vers%
set XZROOT=%root%\Xz\%vers%
set G3OROOT=%root%\G3o\%vers%
set G4OROOT=%root%\G4o\%vers%
set WOROOT=%root%\Wo\%vers%
rem # Wo+Ci
set CSETROOT=\lal\CSet\v2r3
set CIROOT=\lal\Ci\v5r1
rem # CERN
set CERNLIB=\cern\pro\lib
rem #!!!!!!!!!!!!!!!!!!!!!!!
rem # Run time environment :
rem #!!!!!!!!!!!!!!!!!!!!!!!
rem #  OPATH         : optional.
rem #  COPATH        : optional.
rem #  COSECURED     : optional.
rem #  COVERSION     : optional.
rem #  COSYSKIND     : optional.
rem #  GOVERSION     : optional.
rem #  GOPATH        : optional.
rem #  XOFITSSWAP    : optional.
rem #  WOVERSION     : optional. 
rem #  WOENVIRONMENT : optional.
rem #  WOPATH        : optional.
rem #
set OPATH=%root%\Co\%vers%\usr %root%\Ho\%vers%\usr %root%\Go\%vers%\usr %root%\Xo\%vers%\usr %root%\Xz\%vers%\usr %root%\G3o\%vers%\usr %root%\Wo\%vers%\usr
rem # Ci
set CISTDINCLUDE=\lal\Ci\v5r1\usr\include
set CIPATH=%root%\Co\%vers%\include %root%\Xx\%vers%\include %root%\Wo\%vers%\include %root%\Wo\%vers%\usr %root%\Xo\%vers%\include %root%\Ho\%vers%\include %root%\Go\%vers%\include
rem #!!!!!!!!!!!!!!!!!!!
rem # Depend on config :
rem #!!!!!!!!!!!!!!!!!!!
set configure=NT
doskey  omake = %root%\Co\%vers%\%configure%\omake.exe $1
doskey  oshow = %root%\Wo\%vers%\%configure%\oshow.exe $1
doskey  oapplication  = %root%\Wo\%vers%\%configure%\oshow.exe NewApp.odb
set OPATH=%root%\Co\%vers%\usr %root%\Ho\%vers%\usr %root%\Go\%vers%\usr %root%\Xo\%vers%\usr %root%\Xz\%vers%\usr %root%\G3o\%vers%\usr %root%\Wo\%vers%\usr %root%\Xz\%vers%\%configure%
rem # dll path
rem # eXcursion X11, Xm libs are not dll, then GLo, Xx, Xo, Wo libs are not dll.
set PATH=%PATH%;%root%\Co\%vers%\%configure%;%root%\Ho\%vers%\%configure%
rem #

