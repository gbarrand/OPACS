/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <string.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <OTrace.h>

#define ONumber(arr)  (sizeof(arr) / sizeof(arr[0]))

static struct {
  char*   name;
  OTrace  ref;
} verbose_mode [] = {
  {"none"       ,OTraceNone},
  {"widget"     ,OTraceWidget},
  {"resource"   ,OTraceResource},
  {"file"       ,OTraceFile},
  {"event"      ,OTraceEvent},
  {"start"      ,OTraceStartup},
  {"represent"  ,OTraceRepresent},
  {"message"    ,OTraceMessage}
};  
/***************************************************************************/
char** OTraceGetNames (
 int* a_number
)
/***************************************************************************/
/*
  CMemoryFreeBlock returned list when used
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  static char* verboseMode[8] = {
"none",
"widget",
"resource",
"file",
"event",
"start",
"represent",
"message"};
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 8;
  return verboseMode;
}
/***************************************************************************/
OTrace OTraceGetIdentifier (
 char* a_string 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  if(a_string==NULL) return OTraceNone;
  for(count=0;count<ONumber(verbose_mode);count++)
    if(strcmp(a_string,verbose_mode[count].name)==0) 
      return verbose_mode[count].ref;
  CWarnF ("%s not a OTrace.\n",a_string);
  return OTraceNone;
}
/***************************************************************************/
char* OTraceGetName (
 OTrace This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  unsigned int count;
/*.........................................................................*/
  for(count=0;count<ONumber(verbose_mode);count++)
    if(This==verbose_mode[count].ref) 
      return verbose_mode[count].name;
  CWarnF ("%d not a OTrace.\n",This);
  return NULL;
}
