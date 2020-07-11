/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <CPrinter.h>
#include <CMemory.h>

#include <OPointListP.h>

#define MINIMUM(a,b) ((a)<(b)?a:b)
/***************************************************************************/
OPointList OPointListMake (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPointList This;
/*.........................................................................*/
  This                 = (OPointListRecord*)CMemoryAllocateBlock(sizeof(OPointListRecord));
  if(This==NULL)       return NULL;
  This->number         = 0;
  This->xs             = NULL;
  This->ys             = NULL;
  This->zs             = NULL;
  return               This;
}
/***************************************************************************/
OPointList OPointListCreate (
 int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPointList           This;
/*.........................................................................*/
  This                 = OPointListMake ();
  if(This==NULL)       return NULL;
  OPointListInitialize (This,a_number);
  return               This;
}
/***************************************************************************/
void OPointListDelete (
 OPointList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  OPointListClear  (This);
  CMemoryFreeBlock (This);
}
/***************************************************************************/
void OPointListInitialize (
 OPointList This 
,int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  This->number = 0;
  This->xs     = NULL;
  This->ys     = NULL;
  This->zs     = NULL;
  if(a_number<=0) return;
  This->xs     = (double*)CMemoryAllocateBlock(a_number*sizeof(double));
  This->ys     = (double*)CMemoryAllocateBlock(a_number*sizeof(double));
  This->zs     = (double*)CMemoryAllocateBlock(a_number*sizeof(double));
  if( (This->xs==NULL) || (This->ys==NULL) || (This->zs==NULL) )
    {
      CMemoryFreeBlock (This->xs);
      CMemoryFreeBlock (This->ys);
      CMemoryFreeBlock (This->zs);
      This->xs = NULL;
      This->ys = NULL;
      This->zs = NULL;
      return;
    }
  This->number      = a_number;
}
/***************************************************************************/
void OPointListClear (
 OPointList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  CMemoryFreeBlock     (This->xs);
  CMemoryFreeBlock     (This->ys);
  CMemoryFreeBlock     (This->zs);
  This->xs     = NULL;
  This->ys     = NULL;
  This->zs     = NULL;
  This->number = 0;
}
/***************************************************************************/
void OPointListAddEntry (
 OPointList This 
,double a_x
,double a_y
,double a_z
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)  return;
  if(This->number==0)   
    { 
      This->xs = (double*)CMemoryAllocateBlock(sizeof(double));
      This->ys = (double*)CMemoryAllocateBlock(sizeof(double));
      This->zs = (double*)CMemoryAllocateBlock(sizeof(double));
    }
  else                      
    {
      This->xs = (double*)CMemoryChangeBlockSize(This->xs,(This->number+1)*sizeof(double));
      This->ys = (double*)CMemoryChangeBlockSize(This->ys,(This->number+1)*sizeof(double));
      This->zs = (double*)CMemoryChangeBlockSize(This->zs,(This->number+1)*sizeof(double));
    }
  if( (This->xs==NULL) || (This->ys==NULL) || (This->zs==NULL) )
    {
      CMemoryFreeBlock (This->xs);
      CMemoryFreeBlock (This->ys);
      CMemoryFreeBlock (This->zs);
      This->xs = NULL;
      This->ys = NULL;
      This->zs = NULL;
      This->number = 0;
      return;
    }
  This->number              ++;
  This->xs[This->number-1]  = a_x;
  This->ys[This->number-1]  = a_y;
  This->zs[This->number-1]  = a_z;
}
/***************************************************************************/
void OPointListDump (
 OPointList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return;
  CDumpF ("number : %d\n",This->number);
  for(count=0;count<This->number;count++) 
    CDumpF ("%g %g %g\n",This->xs[count],This->ys[count],This->zs[count]);
}
/***************************************************************************/
void OPointListLimit (
 OPointList This 
,int a_number 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if(a_number<=0)    {OPointListClear (This);return;}
  if(a_number>=This->number) return;
  This->number       = a_number;
}
/***************************************************************************/
OPointList OPointListDuplicate (
 OPointList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OPointList list;
/*.........................................................................*/
  if(This==NULL) return NULL;
  list        = OPointListCreate (This->number);
  OPointListCopy (list,This);
  return      list;
}
/***************************************************************************/
void OPointListCopy (
 OPointList This 
,OPointList a_from 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int number;
/*.........................................................................*/
  if(This==NULL)        return;
  if(a_from==NULL)      return;
  if(a_from==This) return;
  number = MINIMUM(This->number,a_from->number);
  for(count=0;count<number;count++) 
    {
      This->xs[count] = a_from->xs[count];
      This->ys[count] = a_from->ys[count];
      This->zs[count] = a_from->zs[count];
    }
}
/***************************************************************************/
void OPointListReverse (
 OPointList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int end,half;
/*.........................................................................*/
  if(This==NULL) return;
  end   = This->number - 1;
  half  = This->number/2;
  for(count=0;count<half;count++) 
    { double                x,y,z;
      int                   index;
      index                 = end-count;
      x                     = This->xs[count];
      y                     = This->ys[count];
      z                     = This->zs[count];
      This->xs[count]       = This->xs[index];
      This->ys[count]       = This->ys[index];
      This->zs[count]       = This->zs[index];
      This->xs[index]       = x;
      This->ys[index]       = y;
      This->zs[index]       = z;
    }
}
/***************************************************************************/
void OPointListSetIthEntry (
 OPointList This 
,int a_number 
,double a_x 
,double a_y 
,double a_z 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if((a_number<0)||(a_number>=This->number)) return;
  This->xs[a_number] = a_x;
  This->ys[a_number] = a_y;
  This->zs[a_number] = a_z;
}
/***************************************************************************/
int OPointListGetSize (
 OPointList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0;
  return This->number;
}
/***************************************************************************/
void OPointListGetIthEntry (
 OPointList This 
,int a_count 
,double* a_x 
,double* a_y 
,double* a_z 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_x!=NULL) *a_x = 0.;
  if(a_y!=NULL) *a_y = 0.;
  if(a_z!=NULL) *a_z = 0.;
  if(This==NULL)                                return;
  if((a_count<0)||(a_count>=This->number)) return;
  if(a_x!=NULL) *a_x = This->xs[a_count];
  if(a_y!=NULL) *a_y = This->ys[a_count];
  if(a_z!=NULL) *a_z = This->zs[a_count];
}
/***************************************************************************/
double OPointListGetIthEntryX (
 OPointList This 
,int a_count 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  if((a_count<0)||(a_count>=This->number)) return 0.;
  return This->xs[a_count];
}
/***************************************************************************/
double OPointListGetIthEntryY (
 OPointList This 
,int a_count 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  if((a_count<0)||(a_count>=This->number)) return 0.;
  return This->ys[a_count];
}
/***************************************************************************/
double OPointListGetIthEntryZ (
 OPointList This 
,int a_count 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return 0.;
  if((a_count<0)||(a_count>=This->number)) return 0.;
  return This->zs[a_count];
}
/***************************************************************************/
int OPointListCompare (
 OPointList This 
,OPointList a_list 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(This==NULL) return 0;
  if(a_list==NULL) return 0;
  if(This->number!=a_list->number) return 0;
  for(count=0;count<This->number;count++) 
    {
      if(  (This->xs[count] != a_list->xs[count])  ||
           (This->ys[count] != a_list->ys[count])  ||
           (This->zs[count] != a_list->zs[count])  
        )
        return 0;
    }
  return 1;
}
/***************************************************************************/
double* OPointListGetListOfX (
 OPointList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->xs;
}
/***************************************************************************/
double* OPointListGetListOfY (
 OPointList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->ys;
}
/***************************************************************************/
double* OPointListGetListOfZ (
 OPointList This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return NULL;
  return This->zs;
}
/***************************************************************************/
void OPointListCopyEntry (
 OPointList This 
,int a_to
,int a_from
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL) return;
  if((a_to<0)  ||(a_to  >=This->number)) return;
  if((a_from<0)||(a_from>=This->number)) return;
  This->xs[a_to] = This->xs[a_from];
  This->ys[a_to] = This->ys[a_from];
  This->zs[a_to] = This->zs[a_from];
}
