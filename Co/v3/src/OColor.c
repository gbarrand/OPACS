/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <string.h>
#include <stdlib.h>

#include <CPrinter.h>
#include <CString.h>

#include <OColor.h>

static struct 
{
  int      number;
  char*    names [66]; /*Null ended list.*/
} Class = {65,
{
"aquamarine",        /*1*/
"mediumaquamarine",
"black",
"blue",
"cadetblue",
"cornflowerblue",
"darkslateblue",
"lightblue",
"lightsteelblue",
"mediumblue",        /*10*/
"mediumslateblue",
"midnightblue",
"navyblue",
"navy",
"skyblue",
"slateblue",
"steelblue",
"coral",
"cyan",
"firebrick",       /*20*/
"brown",
"gold",
"goldenrod",
"green",
"darkgreen",
"darkolivegreen",
"forestgreen",
"limegreen",
"mediumseagreen",
"mediumspringgreen", /*30*/
"palegreen",
"seagreen",
"springgreen",
"yellowgreen",
"darkslategrey",
"dimgrey",
"lightgrey",
"grey",
"khaki",
"magenta",        /*40*/
"maroon",
"orange",
"orchid",
"darkorchid",
"mediumorchid",
"pink",
"plum",
"red",
"indianred",
"mediumvioletred",  /*50*/
"orangered",
"violetred",
"salmon",
"sienna",
"tan",
"thistle",
"turquoise",
"darkturquoise",
"mediumturquoise",
"violet",        /*60*/
"blueviolet",
"wheat",
"white",
"yellow",
"greenyellow",
NULL}};
/***************************************************************************/
char** OColorGetNames (
 int* a_number
)
/***************************************************************************/
/* Return a CList.                                                         */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = Class.number;
  return  Class.names;
}
/***************************************************************************/
char* OColorGetName (
 OColor a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if((a_value<0)||(a_value>=Class.number)) {CWarnF("%d not an OColor.\n",a_value);return NULL;}
  return Class.names[a_value];
}
/***************************************************************************/
int OColorGetIdentifier (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int   color,status;
  char* string;
/*.........................................................................*/
  if( (a_string==NULL) || (*a_string=='\0'))  return OColorBlack;

  color  = (int)CStringConvertToLong(a_string,&status);
  if(status==1) return color;

  string = CStringDuplicate(a_string);
  if(string!=NULL)
    {
      int          count;
      CStringLower (string);
      for(count=0;count<Class.number;count++)
	{ 
	  if(strcmp(string,Class.names[count])==0) 
	    {
	      CStringDelete (string);  
	      return count;
	    }
	}
    }
  CStringDelete (string);  

  CWarnF ("%s not an OColor.\n",a_string);

  return OColorBlack;
}

