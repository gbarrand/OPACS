/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdarg.h>

#include <CMemory.h>

#include <CList.h>

#define NotFound (-1)
/***************************************************************************/
CList CListCreate (
 int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList         This;
/*.........................................................................*/
  if(a_number<0) return NULL;
  This           = (CList)CMemoryAllocateBlock((a_number+1)*sizeof(CEntry));
  if(This==NULL) return NULL;
  This[a_number] = NULL;
  return         This;
}
/***************************************************************************/
CList CListCreateFromTree (
 CEntry    a_top
,CListCountLeafFunction a_count_proc
,CListVisitLeafFunction a_list_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int               count;
  CList            This;
/*.........................................................................*/
  if(a_count_proc==NULL) return NULL;
  if(a_list_proc==NULL)  return NULL;
  count             = 0;
  a_count_proc      (a_top,&count);
  This              = CListCreate(count);
  if(This==NULL)    return NULL;
  count             = 0;
  a_list_proc       (a_top,&count,This);
  return            This;
} 
/***************************************************************************/
void CListDelete (
 CList This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CMemoryFreeBlock (This);
}
/***************************************************************************/
int CListGetSize (
 CList This 
)
/***************************************************************************/
/*
 Description :
   Returns the size of a null ended list.
 Arguments :
  This : null ended list concerned.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return 0;
  count = 0;
  while(This[count]!=NULL) 
    {
      count++;
    }
  return count;
}
/***************************************************************************/
int CListIsEmpty (
 CList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return 1;
  if(This[0]==NULL) return 1;
  return 0;
}
/***************************************************************************/
void CListLimit (
 CList This 
,int a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  This[a_number] = NULL;
}
/***************************************************************************/
void CListDestroy (
 CList This 
,CListVisitEntryFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList itema;
/*.........................................................................*/
  if(This==NULL) return;
  if(a_proc!=NULL)
    {
      for(itema=This;*itema!=NULL;itema++) a_proc(*itema);
    }
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void CListVisit (
 CList This
,CListVisitEntryFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList itema;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_proc==NULL) return;
  for(itema=This;*itema!=NULL;itema++) a_proc(*itema);
}
/***************************************************************************/
void CListVisitE (
 CList This
,CListVisitEntryV_Function a_proc
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList  itema;
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_proc==NULL) return;
  for(itema=This;*itema!=NULL;itema++)
    {
      va_list  args;
      va_start (args,a_proc);
      a_proc   (*itema,args);
      va_end   (args);
    }
}
/***************************************************************************/
CList CListExtract (
 CList This
,CListTestEntryFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList  list = NULL;
  CList  itema;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  if(a_proc==NULL) return NULL;
  for(itema=This;*itema!=NULL;itema++)
    {
      if( a_proc(*itema) != 0) CListAddEntry (&list,*itema);
    }
  return list;
}
/***************************************************************************/
CList CListExtractE (
 CList This
,CListTestEntryV_Function a_proc
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList  list = NULL;
  CList  itema;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  if(a_proc==NULL) return NULL;
  for(itema=This;*itema!=NULL;itema++)
    {
      va_list args;
      va_start (args,a_proc);
      if( a_proc(*itema,args) !=0) CListAddEntry (&list,*itema);
      va_end (args);
    }
  return list;
}
/***************************************************************************/
CEntry CListFindEntry (
 CList   This
,CListFindEntryFunction a_proc
,void*    a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList itema;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  if(a_proc==NULL) return NULL;
  for(itema=This;*itema!=NULL;itema++)
    { 
      if(a_proc(*itema,a_user)!=0) return (*itema);
    }
  return NULL;
}
/***************************************************************************/
CEntry CListFindEntryBackward (
 CList   This
,CListFindEntryFunction a_proc
,void*    a_user
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    count,number;
  CList itema;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  if(a_proc==NULL) return NULL;
  number = CListGetSize (This);
  itema  = This + number - 1;
  for(count=0;count<number;count++)
    { 
      if(a_proc(*itema,a_user)!=0) return (*itema);
      itema--;
    }
  return NULL;
}
/***************************************************************************/
CEntry CListSearchEntry (
 CList   This
,CListSearchEntryFunction a_proc
,...
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList itema;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  if(a_proc==NULL) return NULL;
  for(itema=This;*itema!=NULL;itema++)
    { 
      int      status;
      va_list  args;
      va_start (args,a_proc);
      status   = a_proc(*itema,args);
      va_end   (args);
      if(status!=0) return (*itema);
    }
  return NULL;
}
/***************************************************************************/
CEntry CListGetIthEntry (
 CList   This
,int a_index
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int    number;
/*.........................................................................*/
  if(This==NULL)   return NULL;
  number = CListGetSize (This);
  if(number<=0)                         return NULL;
  if((a_index<0) || (a_index>=number) ) return NULL; 
  return This[a_index];
}
/***************************************************************************/
CList CListDuplicate (
 CList This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  CList     list;
  int        count,number;
/*.........................................................................*/
  if(This==NULL)    return NULL;
  number         = CListGetSize (This);
  list           = CListCreate    (number);
  if(list==NULL) return NULL;
  for(count=0;count<number;count++) list[count] = This[count];
  return       list;
}
/***************************************************************************/
int CListCompare (
 CList This
,CList a_to
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        number1,number2,count;
/*.........................................................................*/
  if( (This==NULL) && (a_to!=NULL) ) return 0;
  if( (This!=NULL) && (a_to==NULL) ) return 0;
  if( (This==NULL) && (a_to==NULL) ) return 1;
  if(This==a_to)     return 1;
  number1 = CListGetSize   (This);
  number2 = CListGetSize   (a_to);
  if(number1!=number2) return 0;
  for(count=0;count<number1;count++) 
    {if(This[count]!=a_to[count]) return 0;}
  return   1;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int CListGetEntryIndex (
 CList This
,CEntry a_entry
,CListMatchEntryFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL)    return NotFound;
  if(a_entry==NULL) return NotFound;
  count = 0;
  while(This[count]!=NULL) 
    {
      if(a_proc!=NULL)
        {
	  if(a_proc(This[count],a_entry)!=0) return count;
	}
      else
        {
	  if(This[count]==a_entry) return count;
	}
      count++;
    }
  return NotFound;
}
/***************************************************************************/
void CListRemoveEntry (
 CList This 
,CEntry a_entry
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int   pos;
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_entry==NULL) return;
  pos   = CListGetEntryIndex(This,a_entry,NULL);
  if(pos==NotFound) return;
  count = pos + 1;
  while(This[count]!=NULL)
   {
     This[count-1] = This[count];
     count++;
   }
  This[count-1] = NULL;
}
/***************************************************************************/
void CListRemoveDuplicates (
 CList This
,CListMatchEntryFunction a_proc
,int a_free
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  count     = 0;
  while(This[count]!=NULL)
    { 
      while(1)
        { 
	   int icount;
          int     found;
	  found   = 0;
          icount  = count+1;
	  while(This[icount]!=NULL)
	    {
              if(a_proc!=NULL)
                {
		  if(a_proc(This[icount],This[count])!=0)
		    {
		      found = 1;
		      break;
		    }
		}
              else
                {
		  if(This[icount]==This[count]) 
		    {
		      found = 1;
		      break;
		    }
		}
              icount++;
	    }      
	  if(found==0) break;
/*remove entry This[icount] */
          if(a_free!=0) CMemoryFreeBlock (This[icount]);
	  icount++;
	  while(This[icount]!=NULL)
	    {
	      This[icount-1] = This[icount];
	      icount++;
	    }
	  This[icount-1] = NULL;
        }
      count++;
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void CListAddEntryFunction (
 CList* This 
,CEntryFunction a_entry
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListAddEntry (This,(CEntry)a_entry);  /*Should produce a warning with some compiler.*/
}
/***************************************************************************/
void CListAddEntry (
 CList* This 
,CEntry a_entry
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)         return;
  if(a_entry==NULL)      return;
  if(*This==NULL)        
    {
      *This           = CListCreate(1);
      if(*This==NULL) return;
      (*This)[0]      = a_entry;
    }
  else
    {
      int               number;
      number            = CListGetSize(*This);
      *This             = (CList)CMemoryChangeBlockSize(*This,(number+2)*sizeof(CEntry));
      if(*This==NULL)   return;
      (*This)[number]   = a_entry;
      (*This)[number+1] = NULL;
    }
}
/***************************************************************************/
void CListPrependEntry (
 CList* This
,CEntry a_entry
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)    return;
  if(a_entry==NULL) return;
  if(*This==NULL)  
    {
      *This           = CListCreate(1);
      if(*This==NULL) return;
      (*This)[0]      = a_entry;
    }
  else
    { 
       int count;
      int               number;
      number            = CListGetSize(*This);
      *This             = (CList)CMemoryChangeBlockSize(*This,(number+2)*sizeof(CEntry));
      if(*This==NULL)   return;
      for(count=number+1;count>=1;count--) (*This)[count] = (*This)[count-1];
      (*This)[0]        = a_entry;
    }
}
/***************************************************************************/
void CListConcatenate (
 CList* This 
,CList a_list
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return;
  if(a_list==NULL) return;
  if(*This==NULL)        
    {
      *This = CListDuplicate(a_list);
    }
  else
    {
       int count = 0;
      int          number,itemn;
      CList        list;
      number       = CListGetSize(*This);
      itemn        = CListGetSize(a_list);
      *This        = (CList)CMemoryChangeBlockSize(*This,(number+itemn+1)*sizeof(CEntry));
      if(*This==NULL) return;
      list         = (*This) + number;
      for(count=0;count<itemn;count++)
	{
	  *list = a_list[count];
	  list++;
	}
      *list       = NULL;
    }
}
