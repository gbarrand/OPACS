/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <X11/Intrinsic.h>

#include <CString.h>
#include <FString.h>

#include <XMailbox.h>

#include <XxF77.h>

static struct 
{
  Display* display;
  XMailbox mbx;
} Class = {NULL,0L};
/***************************************************************************/
int xdpope (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  Class.display = XOpenDisplay ("");
  return         (Class.display==NULL ? 0 : 1);
}
/***************************************************************************/
void xdpclo (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(Class.display==NULL) return;
  XCloseDisplay (Class.display);
}
/***************************************************************************/
int xmbope (
 FString a_string
,int* a_lstring
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*         cstring;
/*.........................................................................*/
  if(Class.display==NULL) return 0;
  cstring             = FStringConvertToC   (a_string,*a_lstring);
  Class.mbx          = XMailboxGetIdentifier(cstring,Class.display);
  CStringDelete       (cstring);
  return              (Class.mbx==0L ? 0 : 1);
}
/***************************************************************************/
void xmbsen (
 FString a_string
,int* a_lstring
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*          cstring;
/*.........................................................................*/
  cstring        = FStringConvertToC   (a_string,*a_lstring);
  XMailboxSendString (Class.mbx,cstring);
  CStringDelete  (cstring);
}


