$ name = f$environment("DEFAULT")+f$parse (p1,,,"NAME")
$ arch = f$getsyi ("ARCH_NAME")
$ library/ext=*/out='name'o  'p1'
$ library/cro=symb/out='name'.lis 'p1'
$ if p2 .nes. ""
$ then
$     link/share/map='name'/full/noexe 'name'o,'p2'
$ else
$     link/share/map='name'/full/noexe 'name'o
$ endif
$ mapana = p6
$ if p3 .nes. "" then p3 = "-s=''p3'"
$ if p4 .eqs. "." then p4 = ""
$ if p4 .nes. "" then p4 = "-e=''p4'"
$ mapana 'name' 'p3' 'p4'
$ answer = ""
$! inquire /nopunct answer "Change ''name's.mar ? [n/y]"
$ if answer .eqs. "" then answer = "no"
$ if f$extract (0, 1, answer) .eqs. "n" then goto nowmac
$ def/user sys$input sys$command
$ edf 'name's.mar
$nowmac:
$ if arch .eqs. "VAX" then mac/nolist/obj='name's 'name's
$ answer = ""
$! inquire /nopunct answer "Change ''name'.opt ? [n/y]"
$ if answer .eqs. "" then answer = "no"
$ if f$extract (0, 1, answer) .eqs. "n" then goto nowlink
$ def/user sys$input sys$command
$ edf 'name'.opt
$nowlink:
$ opt = "/notrace"
$ if p5 .eqs. "debug" then opt = "/trace/debug"
$ if p2 .nes. ""
$ then
$     link'opt'/share/exe='name's-
        /map='name's/full -
        'name'o,'name'/opt,'p2'
$ else
$     link'opt'/share/exe='name's-
        /map='name's/full -
        'name'o,'name'/opt
$ endif
$ del 'name'.lis;*
$ del 'name'.map;*
$ del 'name'.opt;*
$ del 'name'o.obj;*
$ del 'name's.map;*
$ if arch .eqs. "VAX" then del 'name's.mar;*
$ if arch .eqs. "VAX" then del 'name's.obj;*
$ image = f$parse (name,,,"NAME")
$ define 'image's 'name's
$ exit
