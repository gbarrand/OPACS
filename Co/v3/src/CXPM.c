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

#include <CMemory.h>
#include <CStream.h>
#include <CFile.h>
#include <CString.h>
#include <CText.h>
#include <CError.h>

#include <CXPM.h>

typedef unsigned char        Uchar;
typedef unsigned long        Ulong;
typedef unsigned int         Uint;
#define MINIMUM(a,b)            ((a)<(b)?a:b)

#ifdef __cplusplus
extern "C"{
#endif
static void ReportError (char*,int);
#ifdef __cplusplus
}
#endif

#define LINE_SIZE 2
/***************************************************************************/
Uchar* CXPM_GetImageFromStream (
 FILE* a_file
,Uint* a_width
,Uint* a_height
,int* a_colorn
,char*** a_colors
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  char*        lineb;
  char**       lines = NULL;
  int          linen = 0;
  int          iline = 0;
  char*        line;
  char*        pos;
  char**       words;
  int          nword;
  unsigned int width  = 0;
  unsigned int height = 0;
  Uchar*       pixmap = NULL;
  int          ncpix  = 0;
  int          ncolor = 0;
  char**       colors = NULL;
  char*        chars  = NULL;
  int          icolor,beg;
  int          end   = 0;
  Uchar*       cvt   = NULL;
  Uchar*       data  = NULL;
  int          size  = 0;
  int          found,success;
/*.........................................................................*/
  if(a_width!=NULL)  *a_width     = 0;
  if(a_height!=NULL) *a_height    = 0;
  if(a_colorn!=NULL) *a_colorn    = 0;
  if(a_colors!=NULL) *a_colors    = NULL;

  if(a_file==NULL)  return NULL;

  lineb        = CStreamGetStrings (a_file,'\0',&linen,&lines);
  if( (lineb==NULL) || 
      (linen==0)    || 
      (lines==NULL) 
     ) 
    {
      CMemoryFreeBlock(lines);
      CMemoryFreeBlock(lineb);
      return NULL;
    }

  if(strstr(lines[0],"XPM")==NULL) 
    {
      CMemoryFreeBlock(lines);
      CMemoryFreeBlock(lineb);
      return NULL;
    }

  found = 0;
  for(iline=0;iline<linen;iline++) 
    {
      if(lines[iline][0]=='"')  
	{
	  found = 1;
	  break;
	}
    }

  if(found==1)  
    {
  
      line         = lines[iline]+1;
      pos          = strstr(line,"\"");
      if(pos!=NULL) 
	{
	  *pos            = '\0';

	  words           = CStringGetWords(line," ",&nword);
	  if(nword==4)    
	    {

	      width           = (unsigned int)CStringConvertToLong (words[0],NULL);
	      height          = (unsigned int)CStringConvertToLong (words[1],NULL);
	      ncolor          = (int)CStringConvertToLong(words[2],NULL);
	      ncpix           = (int)CStringConvertToLong(words[3],NULL);
	      
	      if( (width!=0)    && 
		  (height!=0)   && 
		  (ncolor!=0)   && 
		  (ncolor<=256) && 
		  (ncpix!=0)
		 ) 
		{

		  chars           = (char*)  CMemoryAllocateBlock(ncolor*sizeof(char));
		  cvt             = (Uchar*) CMemoryAllocateBlock(256 * sizeof(Uchar));
		  size            = width * height;
		  pixmap = data   = (Uchar*) CMemoryAllocateBlock(size * sizeof(Uchar));
		  if( (chars==NULL) || 
		      (cvt==NULL)   || 
		      (pixmap==NULL) 
		     ) 
		    {
		      chars  = NULL;
		      cvt    = NULL;
		      pixmap = NULL;
		    }
		  else 
		    {
		      
		      for(count=0;count<ncolor;count++) 
			{
			  chars [count]  = '\0';
			}
		      for(count=0;count<256;   count++) cvt[count]    = '\0';
		      for(count=0;count<size;  count++) pixmap[count] = '\0';
		      
		      /* read colors */
		      found = 0;
		      for(iline=iline+1;iline<linen;iline++) 
			{
			  if(lines[iline][0]=='"')  
			    {
			      found = 1;
			      break;
			    }
			}
		      if(found==1) 
			{
			  beg    = iline;
			  end    = MINIMUM(beg+ncolor,linen);
			  icolor = 0;
			  for(count=beg;count<end;count++)
			    { 
			      success = 0;
			      if(lines[count][0]=='"') 
				{

				  line = lines[count]+1;
				  pos  = strstr(line,"\"");
				  if(pos!=NULL) 
				    {
				      *pos            = '\0';
				      
				      pos             = strstr(line," c ");
				      if(pos!=NULL) 
					{
					  char*           posw;
					  pos            += 3;
					  pos             = pos+strspn(pos," ");
					  posw            = strchr(pos,' ');
					  if(posw!=NULL)  *posw = '\0';

					  chars[icolor]      = line[0];           
					  cvt[chars[icolor]] = (Uchar)icolor;
					  CTextAddLine     (&icolor,&colors,pos);     /*icolor incremented*/

					  success = 1;
					  
					} /*endif pos*/
				    } /*endif pos*/
				} /*endif lines[count]*/

			      if(success==0) ReportError("CXPM_GetImageFromStream",CErrorFormat);

			    } /*endfor*/
			}  /*endif found*/

		      if(icolor!=ncolor) 
			{
			  ReportError   ("CXPM_GetImageFromStream",CErrorFormat);
			  ncolor  = icolor;
			}

/* read data */
		      found = 0;
		      for(iline=end+1;iline<linen;iline++) 
			{
			  if(lines[iline][0]=='"')  
			    {
			      found = 1;
			      break;
			    }
			}
		      if(found==1) 
			{
			  for(count=iline;count<linen;count++)
			    { char c;
			      int  i;
			      if(lines[count][0]=='"') 
				{
				  line = lines[count]+1;
				  pos  = strstr(line,"\"");
				  if(pos!=NULL) 
				    {
				      *pos            = '\0';
     
				      for(i=0;(c=line[i])!='\0';i+=ncpix)
					{
					  *data = cvt[c];                         
					  data++;
					}

				    } /*endif pos*/
				} /*endif lines[count]*/
			    }
			}  /*endif found*/


		    } /*endif CMemoryAllocateBlock*/
		} /*endif width,height,ncolor*/

	    } /*endif nword*/
	  CTextDelete (nword,words);
	  
	} /*endif pos*/
    } /*endif LINE_SIZE*/

  CMemoryFreeBlock (lines);
  CMemoryFreeBlock (lineb);

  CMemoryFreeBlock (chars);
  CMemoryFreeBlock (cvt);

  if(a_width!=NULL)  *a_width        = width;
  if(a_height!=NULL) *a_height       = height;
  if(a_colorn!=NULL) *a_colorn       = ncolor;
  if(a_colors!=NULL) *a_colors       = colors;

  return          pixmap;
}
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Co","CXPM",a_name,a_code);
}
