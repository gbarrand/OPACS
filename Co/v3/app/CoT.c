/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <CPrinter.h>
#include <CSystem.h>
#include <CString.h>
#include <CTime.h>
#include <OType.h>

#include <CoCommands.h>

/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShell  osh;
/*.........................................................................*/
  osh               = OShellCreate ("osh");
  CoAddCommands (osh);

  CInfoF    ("OShell identifier:%lu\n",osh);
  CInfoF    ("OShell identifier:%lu\n",OShellCreate("xxx"));


  OShellExecuteF    (osh,5+64,"echo %d",123);
 {char*  string;
  string = CSystemAsk("listCommand");
  if(string!=NULL) OShellExecuteF  (osh,6+strlen(string),"spawn %s",string);}

  OShellExecute     (osh,"\
set -v;set +x\n\
collect OShell | dump -\n\
osh 'collect OShell where name eq xxx'\n\
");
  OShellExecute     (osh,"\
isValid OCommand/isValid && echo 'isValid is a known command'\n\
collect OCommand  | dump -\n\
collect OType | dump - name properties\n\
stdout '-------------------------------'\n\
collect OType where name eq O* | collect OType where name eq *x* | dump - name properties\n\
stdout '-------------------------------'\n\
collect OType where name in 'OMatrix\\nCMemory' | dump - name\n\
stdout '-------------------------------'\n\
cat CoT.osh >x.lis;cat x.lis;rm x.lis\n\
help 'CoT.htm'");
  OShellExecute     (osh,"\
  spawn UNIX sh     as2 /usr/bin/X11/xclock\n\
  spawn UNIX csh    as2 /usr/bin/X11/xclock\n\
  spawn VMS  DECNET lalux0 lalaxs 'run sys$system:decw$mail'\n\
 # Require a DECNET proxy login for lalux0 on VMS node lalaxs.\n\
 # axs is not a decnet name.\n\
 # spawn VMS  DECNET lalux0 lalaxs create/terminal/detached\n\
 # Require a DECNET proxy login for lalux3 on VMS node lalaxs.\n\
 #spawn VMS  UCX    <password>  axs 'run sys$system:decw$mail'\n\
  load CoT.odb;collect OLUT | dump -;collect OMatrix33 | dump -\n\
");

  OShellExecute (osh," xxx=2;echo $xxx");

 {char*         date;
  time_t        at;
  date          = CStringDuplicate(CTimeGetDate());
  at            = CTimeParseDate(date);
  CInfoF        ("%s\n%s\n",date,ctime(&at));
  CStringDelete (date);}

  OTypeClearClass ();

  a_argn = 0;
  a_args = NULL;
  return EXIT_SUCCESS;
}
