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
#include <stdio.h>

#define CWarn printf

#include <momo.h>

#ifdef __cplusplus
extern "C"{
#endif
int   mo_post_mailto_win        (char*,char*);
char* mo_check_for_proxy        (char*);
int   mo_gui_check_icon         (int);
void  mo_gui_clear_icon         (void);
void  mo_gui_notify_progress    (char*);
void  rename_binary_file        (char*);
void  application_user_feedback (char *);
char* prompt_for_string         (char*);
char* prompt_for_password       (char*);
int   prompt_for_yes_or_no      (char*);
#ifdef __cplusplus
}
#endif

#define MO_VERSION_STRING "2.2"
char* HTAppName                 = "NCSA Mosaic for the X Window System";
char* HTAppVersion              = MO_VERSION_STRING;
char* global_xterm_str          = "";  /* required for HTAccess.c now */
int   use_default_extension_map = 0;
char* global_extension_map      = "";
char* personal_extension_map    = "";
int   use_default_type_map      = 0;
int   tweak_gopher_types        = 0;
int   have_hdf                  = 0;
int   twirl_increment           = 0;
int   loading_inlined_images    = 0;
int   force_dump_to_file        = 0;   /* hook to force dumping binary data straight to file named by... */
char* force_dump_filename       = NULL;   /* this filename. */
char* machine_with_domain       = "";
char* uncompress_program        = "";
char* gunzip_program            = "";
int   binary_transfer           = 0;
char* global_type_map           = "";
char* personal_type_map         = "";

char* HTReferer                 = NULL;  /*gui.c*/

static struct {
  mo_notify_function notifyProc;
} Class = {NULL};
/***************************************************************************/
void mo_set_notify_proc (
 mo_notify_function a_proc
)
/***************************************************************************/
/* This routine does not exists in Mosaic-2.2                              */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.notifyProc = a_proc;
}
/****************************************************************************
 * name:    mo_tmpnam
 * purpose: Make a temporary, unique filename.
 * inputs:  
 *   none
 * returns: 
 *   The new temporary filename.
 * remarks: 
 *   We call tmpnam() to get the actual filename, and use the value
 *   of Rdata.tmp_directory, if any, for the directory.
 ****************************************************************************/
#ifndef L_tmpnam
#define L_tmpnam 32
#endif
char *mo_tmpnam (void)
{
  char *tmp = (char *)malloc (sizeof (char) * L_tmpnam);
  tmpnam (tmp);
  return tmp;
}
char* mo_check_for_proxy (char* access)
{
  access = NULL;
  return NULL;
}
int mo_gui_check_icon (int twirl)
{
  twirl = 0;
  return 0;
}
void mo_gui_clear_icon (void)
{
}
void mo_gui_notify_progress (char *msg)
{
  if(Class.notifyProc) Class.notifyProc(msg);
  else                 printf("%s\n",msg);
}
int mo_post_mailto_win (char *to_address, char *subject)
{
  CWarn ("mo_post_mailto_win: dummy.\n"); 
  to_address = NULL;
  subject    = NULL;
  return 0;
}
void rename_binary_file (char *fnam)
{
  fnam = NULL;
  CWarn ("rename_binary_file: dummy.\n"); 
}
void application_user_feedback (char *str)
{
  str = NULL;
  CWarn ("application_user_feed: dummy.\n"); 
}
char *prompt_for_string (char *questionstr)
{
  questionstr = NULL;
  CWarn ("prompt_for_string: dummy.\n"); 
  return "";
}
char *prompt_for_password (char *questionstr)
{
  questionstr = NULL;
  CWarn ("prompt_for_password: dummy.\n"); 
  return "";
}

int prompt_for_yes_or_no (char *questionstr)
{
  questionstr = NULL;
  CWarn ("prompt_for_yes_or_no: dummy.\n"); 
  return 0;
}
