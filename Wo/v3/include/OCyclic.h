/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OCyclic_h
#define OCyclic_h
 
typedef struct _OCyclicRecord  *OCyclic;

#ifdef __cplusplus
extern "C"{
#endif
void     OCyclicClearClass     ();
OCyclic* OCyclicGetIdentifiers ();
OCyclic  OCyclicGetIdentifier  (char*);
int      OCyclicIsValid        (OCyclic);
OCyclic  OCyclicMake           ();
OCyclic  OCyclicCreate         (char*,char*,int,char*);
void     OCyclicDelete         (OCyclic);
void     OCyclicStop           (OCyclic);
void     OCyclicStart          (OCyclic);
void     OCyclicSetName        (OCyclic,char*);
void     OCyclicSetWidgetName  (OCyclic,char*);
void     OCyclicSetDelay       (OCyclic,int);
char*    OCyclicGetName        (OCyclic);
void     OCyclicSetScript      (OCyclic,char*);
int      OCyclicSetAttribute   (OCyclic,char*,void*,void*,int);
int      OCyclicGetAttribute   (OCyclic,char*,void*,void*,int*);
int      OCyclicDoMethod       (OCyclic,char*,void*,int,char**,void*,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOCyclic(0);  /**/
#endif

#endif  /*OCyclic_h*/
