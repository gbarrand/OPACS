/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OLoop_h
#define OLoop_h
 
#define LOOP_BEGIN_PCD(a_list) {if(a_list){OProcedure* itema;itema=(OProcedure*)(a_list);while(*itema!=NULL) {{
#define LOOP_END_PCD           }itema++;}}}

#define LOOP_BEGIN_PCD_LINE(a_text,a_pcd) \
 { int lineloop;int begloop,endloop;char* strloop;\
  begloop=OProcedureGetBeginPosition(a_pcd);endloop=OProcedureGetEndPosition(a_pcd); \
  for(lineloop =begloop;lineloop<=endloop;lineloop++)  \
    {strloop = a_text[lineloop];if(!strloop) continue;{
#define LOOP__END_PCD_LINE   }}}

#define NOTCOMMENT if( (strloop!=NULL) && (*strloop=='!')) continue;
#define NOTEMPTY   if( (strloop==NULL) || (*strloop=='\0') ) continue;

#endif  /*OLoop_h*/
