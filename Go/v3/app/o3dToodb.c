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
#include <stdlib.h>
#include <string.h>

#include <CFile.h>
#include <CMemory.h>
#include <CError.h>
#include <CPrinter.h>
#include <CList.h>
#include <OMatrix.h>
#include <OType.h>

#include <CoCommands.h>
#include <GoCommands.h>

#include <ONode.h>

#include <OMatrixP.h> /* For ReadMatrix.*/
 
#ifdef __cplusplus
extern "C"{
#endif
static ONode*   LoadO3D     (char*);
static ONode    Read2r0     (FILE*);
static ONode    Read3r0     (FILE*);
static int      ReadPoints  (FILE*,double*,double*,double*);
static int      ReadMatrix  (OMatrix,FILE*);
static void     ReportError (char*,int);
#ifdef __cplusplus
}
#endif
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_argn!=3)
    {
      CInfo ("\
Give input and output filename\n\
  UNIX> o3d2odb.exe in.o3d out.odb\n\
");
    }
  else
    {
      OShell      osh;
      ONode* items;

      osh      = OShellCreate ("osh");
      CoAddCommands   (osh);
      GoAddCommands   (osh);

      items               = (ONode*) LoadO3D (a_args[1]);
      OShellExecuteF      (osh,22+strlen(a_args[2]),"collect ONode | write %s",a_args[2]);
      CListDelete         ((CList)items);
      
      OTypeClearClass     ();

    }
  return              EXIT_SUCCESS;
}
/***************************************************************************/
static ONode* LoadO3D (
 char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*     file;
  char      string[80];
  ONode*  items = NULL;
/*.........................................................................*/
  file      = CFileOpenForReading(a_name,NULL);
  if(file==NULL) return NULL;

  rewind    (file);

/* get version */
  if(fgets(string,79,file)!=NULL)
    { 
      ONode item;
      int     lstring;
      lstring = strlen(string);
      if(string[lstring-1]=='\n') string[lstring-1] = 0;
      if(strncmp(string,"OnX",3)==0) 
	{
	  while(1)
            {
              item = Read3r0(file);
              if(item==NULL) break;
	      CListAddEntry ((CList*)&items,item);
	    }
	}
      else                        
	{
	  rewind (file);
	  while(1)
            {
              item = Read2r0(file);
              if(item==NULL) break;
	      CListAddEntry ((CList*)&items,item);
	    }
	}
    }

  fclose (file);

  return items;
}
/***************************************************************************/
static ONode Read2r0 (
 FILE* a_file
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lname;
  char       name[80];
  ONode    This;
  OMatrix    matrix;
/*.........................................................................*/
  if(fgets(name,79,a_file)==NULL) return NULL;
  lname = strlen(name);
  if(name[lname-1]=='\n') name[lname-1] = 0;

  This                = ONodeCreate (name);
  if(This==NULL)           return NULL;

  matrix              = OMatrixMake();

  if(ReadMatrix(matrix,a_file)==1) 
    {
      int  nline,nvertex;
      if(OMatrixIsIdentity(matrix)==0) 
	{
	  ONodeSetMatrix (This,matrix);
	}
      if(fscanf(a_file,"%d %d\n",&nline,&nvertex)!=2) 
	{
	  ReportError ("Read2r0",CErrorFormat);
	}
      else
	{
           int count;
          int           pointn;
	  double*       xs;
	  double*       ys;
	  double*       zs;
	  OContext         atb;
	  atb           = OContextMake();
	  xs            = (double*)CMemoryAllocateBlock(nvertex*sizeof(double));
	  ys            = (double*)CMemoryAllocateBlock(nvertex*sizeof(double));
	  zs            = (double*)CMemoryAllocateBlock(nvertex*sizeof(double));
	  for(count=0;count<nline;count++) 
	    { int                 color,type,sline,width;
	      
	      if(fscanf(a_file,"%d %d %d %d \n",&color,&type,&sline,&width)!=4) 
		{ReportError ("Read2r0",CErrorFormat);break;}
	  
	      pointn              = ReadPoints (a_file,xs,ys,zs);

              type--;
              sline--;
              if(type==4)         type = (int)OPrimitivePolygon; /*GPrimGoneEdge no more supported*/

	      OContextSetF            (atb,"cwl",color,width,sline);

	      ONodeAddPrimitive      (This,OPrimitiveCreate(type,atb,pointn,xs,ys,zs));

	    }
	  OContextDelete     (atb);
	  CMemoryFreeBlock       (xs);
	  CMemoryFreeBlock       (ys);
	  CMemoryFreeBlock       (zs);
	}
    }

  OMatrixDelete       (matrix); 
  return              This;
}
/***************************************************************************/
static ONode Read3r0 (
 FILE* a_file
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int        lname;
  char       name[80];
  ONode    This;
  OMatrix    matrix;
/*.........................................................................*/
  if(fgets(name,79,a_file)==NULL) return NULL;
  lname = strlen(name);
  if(name[lname-1]=='\n') name[lname-1] = 0;

  This              = ONodeCreate(name);
  if(This==NULL)    return NULL;

  matrix            = OMatrixMake();

  if(ReadMatrix (matrix,a_file)==1) 
    {
      int        nline,nvertex;
      if(OMatrixIsIdentity(matrix)==0) 
	{
	  ONodeSetMatrix (This,matrix);
	}
      if(fscanf(a_file,"%d %d\n",&nline,&nvertex)!=2) 
        {
	  ReportError ("Read3r0",CErrorFormat);
        }
      else 
	{
	   int count;
          int           pointn;
	  double*       xs;
	  double*       ys;
	  double*       zs;
	  OContext          atb;
	  atb           = OContextMake();
	  xs            = (double*)CMemoryAllocateBlock(nvertex*sizeof(double));
	  ys            = (double*)CMemoryAllocateBlock(nvertex*sizeof(double));
	  zs            = (double*)CMemoryAllocateBlock(nvertex*sizeof(double));
	  for(count=0;count<nline;count++) 
	    { int                 color,type,sline,smark,sarea,width;
	      
	      if(fscanf(a_file,"%d %d %d %d %d %d \n",&color,&type,&sline,&smark,&sarea,&width)!=6) 
		{ReportError("Read3r0",CErrorFormat);break;}
	      
              type--;
              sline--;
              smark--;
              sarea--;
              if(type==4)         type = (int)OPrimitivePolygon; /*GPrimGoneEdge no more supported*/

	      pointn              = ReadPoints (a_file,xs,ys,zs);

	      OContextSetF            (atb,"cwlma",color,width,sline,smark,sarea);
	      ONodeAddPrimitive      (This,OPrimitiveCreate(type,atb,pointn,xs,ys,zs));

	    }
	  OContextDelete     (atb);
	  CMemoryFreeBlock       (xs);
	  CMemoryFreeBlock       (ys);
	  CMemoryFreeBlock       (zs);
	}
    }

  OMatrixDelete       (matrix); 
  return              This;
}
/***************************************************************************/
static int ReadPoints (
 FILE* a_file 
,double* a_xs
,double* a_ys
,double* a_zs
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
  int number;
/*.........................................................................*/
  if(a_file==NULL)   return 0;
  if( (a_xs==NULL) || (a_ys==NULL) || (a_zs==NULL) ) return 0;
  if(fscanf(a_file,"%d\n",&number)!=1) {ReportError ("ReadPoints",CErrorFormat);return 0;}
  for(count=0;count<number;count++) 
    {
      if(fscanf (a_file,"%le %le %le \n",&(a_xs[count]),
                                        &(a_ys[count]),
                                        &(a_zs[count]))!=3) 
	{
	  ReportError ("ReadPoints",CErrorFormat);
	  return 0;
	}
    }
  return number;
}
/***************************************************************************/
static int ReadMatrix (
 OMatrix This 
,FILE* a_file 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(This==NULL)   return 0;
  if(a_file==NULL) return 0;
  if(fscanf (a_file,"%le %le %le %le \n",&(This->e00),&(This->e01),&(This->e02),&(This->e03))!=4) 
    {CWarn("OnX(ReadMatrix):fscanf error:1 row\n");return 0;}
  if(fscanf (a_file,"%le %le %le %le \n",&(This->e10),&(This->e11),&(This->e12),&(This->e13))!=4) 
    {CWarn("OnX(ReadMatrix):fscanf error:2 row\n");return 0;}
  if(fscanf (a_file,"%le %le %le %le \n",&(This->e20),&(This->e21),&(This->e22),&(This->e23))!=4) 
    {CWarn("OnX(ReadMatrix):fscanf error:3 row\n");return 0;}
  if(fscanf (a_file,"%le %le %le %le \n",&(This->e30),&(This->e31),&(This->e32),&(This->e33))!=4) 
    {CWarn("OnX(ReadMatrix):fscanf error:4 row\n");return 0;}
  return 1;
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
  CErrorHandle ("Go","ONode",a_name,a_code);
}
