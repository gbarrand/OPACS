/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
/*
  time_t time(time_t*);

  tm*    gmtime(time_t*)
  tm*    localtime(time_t*)
  time_t mktime(tm*)     is the invert of localtime
  
  char*  asctime(tm*)
  char*  ctime  (time_t*)  = char* asctime(localtime(time_t*))
*/

#include <stddef.h>
#include <time.h>
#include <string.h>

#include <CTime.h>
#include <CPrinter.h>
#include <CString.h>
#include <CText.h>

static struct 
{
  time_t start;
} Class = {0};
/***************************************************************************/
void CTimeStart (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   time_t d;
/*.........................................................................*/
   Class.start = time(&d);
}
/***************************************************************************/
int CTimeGetElapsedPeriod (
)
/***************************************************************************/
/*
  Return elapsed time since CTimeStart in second.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   time_t d,t;
/*.........................................................................*/
   t = time(&d);
   return (int)(t - Class.start);
}
/***************************************************************************/
char* CTimeGetDate (
)
/***************************************************************************/
/*
  Return local date.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  string;
  time_t d;
/*.........................................................................*/
  time       (&d);
  string     = ctime(&d);
  string[24] = '\0';
  return     string;
}
/***************************************************************************/
time_t CTimeParseDate (
 char* a_date
)
/***************************************************************************/
/*
  Date is assumed to be a local date.
    "wday mon mday hour:min:sec year"
  Example : 
    Tue Jun 10 14:54:12 1997
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*  date;
  int    wordn;
  char** words;
  static struct tm stime;
  int    status;
  char*  month;
  char*  day;
/*.........................................................................*/
  if(a_date==NULL)   return (time_t)(-1);
  date               = CStringDuplicate (a_date);
  CStringReplacePart (&date,":"," ");
  words              = CStringGetWords (date," ",&wordn);
  if(wordn!=7) 
    {
      CWarnF        ("Seven fields expected in :\n%s\n",a_date);
      CTextDelete   (wordn,words);
      CStringDelete (date);
      return        (time_t)(-1);
    }

  day = words[0];
       if(strcmp(day,"Sun")==0) stime.tm_wday = 0;
  else if(strcmp(day,"Mon")==0) stime.tm_wday = 1;
  else if(strcmp(day,"Tue")==0) stime.tm_wday = 2;
  else if(strcmp(day,"Wed")==0) stime.tm_wday = 3;
  else if(strcmp(day,"Thu")==0) stime.tm_wday = 4;
  else if(strcmp(day,"Fri")==0) stime.tm_wday = 5;
  else if(strcmp(day,"Sat")==0) stime.tm_wday = 6;
  else
    {
      CWarnF        ("Word %s not a day in :\n%s\n",day,a_date);
      CTextDelete   (wordn,words);
      CStringDelete (date);
      return        (time_t)(-1);
    }

  month = words[1];
       if(strcmp(month,"Jan")==0) stime.tm_mon = 0;
  else if(strcmp(month,"Feb")==0) stime.tm_mon = 1;
  else if(strcmp(month,"Mar")==0) stime.tm_mon = 2;
  else if(strcmp(month,"Apr")==0) stime.tm_mon = 3;
  else if(strcmp(month,"May")==0) stime.tm_mon = 4;
  else if(strcmp(month,"Jun")==0) stime.tm_mon = 5;
  else if(strcmp(month,"Jul")==0) stime.tm_mon = 6;
  else if(strcmp(month,"Aug")==0) stime.tm_mon = 7;
  else if(strcmp(month,"Sep")==0) stime.tm_mon = 8;
  else if(strcmp(month,"Oct")==0) stime.tm_mon = 9;
  else if(strcmp(month,"Nov")==0) stime.tm_mon = 10;
  else if(strcmp(month,"Dec")==0) stime.tm_mon = 11;
  else
    {
      CWarnF        ("Word %s not a month in :\n%s\n",month,a_date);
      CTextDelete   (wordn,words);
      CStringDelete (date);
      return        (time_t)(-1);
    }

  stime.tm_mday = (int)CStringConvertToLong(words[2],&status);
  if(status==0) 
    {
      CWarnF        ("Word %s not a number in :\n%s\n",words[2],a_date);
      CTextDelete   (wordn,words);
      CStringDelete (date);
      return        (time_t)(-1);
    }
  stime.tm_hour = (int)CStringConvertToLong(words[3],&status);
  if(status==0) 
    {
      CWarnF        ("Word %s not a number in :\n%s\n",words[3],a_date);
      CTextDelete   (wordn,words);
      CStringDelete (date);
      return        (time_t)(-1);
    }
  stime.tm_min = (int)CStringConvertToLong(words[4],&status);
  if(status==0) 
    {
      CWarnF        ("Word %s not a number in :\n%s\n",words[4],a_date);
      CTextDelete   (wordn,words);
      CStringDelete (date);
      return        (time_t)(-1);
    }
  stime.tm_sec = (int)CStringConvertToLong(words[5],&status);
  if(status==0) 
    {
      CWarnF        ("Word %s not a number in :\n%s\n",words[5],a_date);
      CTextDelete   (wordn,words);
      CStringDelete (date);
      return        (time_t)(-1);
    }
  stime.tm_year  = (int)CStringConvertToLong(words[6],&status);
  stime.tm_year -= 1900;
  if(status==0) 
    {
      CWarnF        ("Word %s not a number in :\n%s\n",words[6],a_date);
      CTextDelete   (wordn,words);
      CStringDelete (date);
      return        (time_t)(-1);
    }

  stime.tm_yday  = -1;
  stime.tm_isdst =  1;  /*gmtime 0, localtime 1.*/

  CTextDelete    (wordn,words);
  CStringDelete  (date);
  return         mktime(&stime);
}
/***************************************************************************/
char* CTimeConvertToString (
 time_t a_time
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  time_t d;
  char*  string;
/*.........................................................................*/
  d          = a_time;
  string     = ctime(&d);
  string[24] = '\0';
  return     string;
}
