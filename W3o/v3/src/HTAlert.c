/*	Displaying messages and getting input for LineMode Browser
**	==========================================================
**
**	REPLACE THIS MODULE with a GUI version in a GUI environment!
**
** History:
**	   Jun 92 Created May 1992 By C.T. Barker
**	   Feb 93 Simplified, portablised (ha!) TBL
**
*/


#include "HTString.h"
#include "HTAlert.h"

#ifdef __cplusplus
extern "C"{
#endif
void mo_gui_notify_progress (char *);
int  mo_gui_check_icon      (int);
void mo_gui_clear_icon      (void);
int prompt_for_yes_or_no (char *);
char *prompt_for_string (char *);
char *prompt_for_password (char *);
#ifdef __cplusplus
}
#endif

PUBLIC void HTAlert ARGS1(CONST char *, Msg)
{
  mo_gui_notify_progress (Msg);
  return;
}

PUBLIC void HTProgress ARGS1(CONST char *, Msg)
{
  mo_gui_notify_progress (Msg);
  return;
}

PUBLIC int HTCheckActiveIcon ARGS1(int, twirl)
{
  int ret;

  ret = mo_gui_check_icon (twirl);
  return(ret);
}

PUBLIC void HTClearActiveIcon NOARGS
{
  mo_gui_clear_icon ();
  return;
}

PUBLIC HTBoolean HTConfirm ARGS1(CONST char *, Msg)
{
  if (prompt_for_yes_or_no (Msg))
    return(YES);
  else
    return(NO);
}

PUBLIC char * HTPrompt ARGS2(CONST char *, Msg, CONST char *, deflt)
{
  char *Tmp = prompt_for_string (Msg);
  char *rep = 0;

  StrAllocCopy (rep, (Tmp && *Tmp) ? Tmp : deflt);
  return rep;
}

PUBLIC char * HTPromptPassword ARGS1(CONST char *, Msg)
{
  char *Tmp = prompt_for_password (Msg);

  return Tmp;
}
