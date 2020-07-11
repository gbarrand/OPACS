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
#define DEBUG
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#include <CPrinter.h>
#include <CList.h>
#include <CString.h>
#include <CText.h>
#include <OMatrix.h>
#include <OType.h>

#include <GoTypes.h>
#include <OCamera.h>
#include <OContextmap.h>
#include <Go.h>

#include <GoCommands.h>

#ifdef __cplusplus
extern "C"{
#endif
static void   MultiplyMatrix (char*,OCamera,OMatrix);
static ONode  GetNode        (char*);
#ifdef __cplusplus
}
#endif


/***************************************************************************/
void GoAddCommands (
 OShell a_osh
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OCommandGetIdentifier("represent")!=NULL) return; /*done*/

  GoSetTypes ();

  OShellAddNewCommand (a_osh,"Go/represent"          ,GoExecute_represent);
  OShellAddNewCommand (a_osh,"Go/getCameraNodes"     ,GoExecute_getCameraNodes);
  OShellAddNewCommand (a_osh,"Go/viewNode"           ,GoExecute_viewNode);
  OShellAddNewCommand (a_osh,"Go/unviewNode"         ,GoExecute_unviewNode);
  OShellAddNewCommand (a_osh,"Go/isCameraEmpty"      ,GoExecute_isCameraEmpty);
  OShellAddNewCommand (a_osh,"Go/normalizeNode"      ,GoExecute_normalizeNode);
  OShellAddNewCommand (a_osh,"Go/rotateCamera"       ,GoExecute_rotateCamera);
  OShellAddNewCommand (a_osh,"Go/translateCamera"    ,GoExecute_translateCamera);
  OShellAddNewCommand (a_osh,"Go/scaleCamera"        ,GoExecute_scaleCamera);
  OShellAddNewCommand (a_osh,"Go/resetCamera"        ,GoExecute_resetCamera);
  OShellAddNewCommand (a_osh,"Go/doCameraPick"       ,GoExecute_doCameraPick);
  OShellAddNewCommand (a_osh,"Go/copyCameraMatrix"   ,GoExecute_copyCameraMatrix);
  OShellAddNewCommand (a_osh,"Go/translateNode"      ,GoExecute_translateNode);
  OShellAddNewCommand (a_osh,"Go/rotateNode"         ,GoExecute_rotateNode);
  OShellAddNewCommand (a_osh,"Go/addPrimitiveToNode" ,GoExecute_addPrimitiveToNode);
  OShellAddNewCommand (a_osh,"Go/addStringToNode"    ,GoExecute_addStringToNode);
  OShellAddNewCommand (a_osh,"Go/addHTML_PageToNode" ,GoExecute_addHTML_PageToNode);

}
/***************************************************************************/
int GoExecute_represent (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> represent -
  osh> represent <string:oid>
  osh> represent <string:oid> [<string:ONode parent>, nil]
  osh> represent <string:oid> [<string:ONode parent>, nil] <string:OContextmap>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType       otypeONode;
  int         handn;
  OHandle*    hands;
  int         rhandn      = 0;
  OHandle*    rhands      = NULL;
  ONode       oparent     = NULL;
  OContextmap ocontextmap = NULL;
/*.........................................................................*/
  if( (a_argn!=2) && (a_argn!=3) && (a_argn!=4) )
    {
      CWarnF              ("One or two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  if( (a_argn>=3) && (strcmp(a_args[2],"nil")!=0) )
    {
      oparent = ONodeGetIdentifier(a_args[2]);
      if(oparent==NULL) 
	{
	  CWarnF ("ONode %s is unknown in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  OProcessPutHandles (a_process,NULL);
	  return EXIT_FAILURE;
	}
    }

  if(a_argn==4)
    {
      ocontextmap = OContextmapGetIdentifier (a_args[3]);
      if(ocontextmap==NULL)
	{
	  CWarnF ("OContextmap %s is unknown in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	  OProcessPutHandles (a_process,NULL);
	  return EXIT_FAILURE;
	}
    }


  otypeONode = OTypeGetIdentifier ("ONode");
  if(otypeONode==NULL)
    {
      CWarnF              ("Type ONode is unknown in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return              EXIT_FAILURE;
    }

  if(OProcessPreparePipe(a_process,a_args[1])==0) return EXIT_FAILURE;

  OContextmapSetDefaultContext  (ocontextmap);
  OContextmapResetHits          (ocontextmap);

  hands  = OProcessGetHandles   (a_process);
  handn  = CListGetSize          ((CList)hands);
  rhands = (OHandle*)CListCreate (handn);
  if(rhands!=NULL)
    {
      int count;
      for(count=0;count<handn;count++)
	{ 
	  OType                  otype;
	  OTypeRepresentFunction proc;
	  otype                  = (OType)OHandleGetType(hands[count]);
	  proc                   = OTypeGetRepresentFunction(otype);
	  if(proc!=NULL)
	    {
	      OIdentifier                 oid;
	      ONode                       onode;
	      oid                         = (OIdentifier)OHandleGetIdentifier(hands[count]);
              OContextmapSetObjectContext (ocontextmap,otype,oid);
	      onode                       = (ONode) proc(oid,otype);
	      if(onode!=NULL)
		{
		  rhands[rhandn] = OHandleCreate ((OType)otypeONode,(OIdentifier)onode);
		  rhandn++;
		  if(oparent!=NULL) ONodeAddChild (oparent,onode);
		}
	    }
	}
    }
  CListLimit          ((CList)rhands,rhandn);
  OProcessPutHandles (a_process,rhands);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_getCameraNodes (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> getCameraNodes <string:OCamera>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OType     type;
  OCamera   ocamera;
  OHandle* hands = NULL;
  ONode*    itema;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  ocamera = OCameraGetIdentifier(a_args[1]);
  if(ocamera==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  type = OTypeGetIdentifier("ONode");
  if(type==NULL) 
    {
      CWarnF("Type ONode is unknown in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  itema  = OCameraGetNodes   (ocamera);
  for(;(itema!=NULL) && (*itema!=NULL);itema++) 
    {
      CListAddEntry ((CList*)&hands,OHandleCreate(type,*itema));
    }

  OProcessPutHandles  (a_process,hands);
  return        EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_viewNode (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/*****************************************************************6**********/
/*
  osh> viewNode -              <string:OCamera>
  osh> viewNode <string:ONode> -
  osh> viewNode <string:ONode> <string:OCamera>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera* cameras = NULL;
  ONode*   nodes   = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  nodes   = (ONode*)   OProcessGetTypedObjects (a_process,a_args[1],"ONode");
  cameras = (OCamera*) OProcessGetTypedObjects (a_process,a_args[2],"OCamera");

 {OCamera* ca;
  for(ca = cameras;(ca!=NULL) && (*ca!=NULL);ca++)
    {
      OCameraViewNodes (*ca,nodes);
    }}

  CListDelete         ((CList)cameras);
  CListDelete         ((CList)nodes);

  OProcessPutHandles (a_process,NULL);

  return              EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_unviewNode (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/*****************************************************************6**********/
/*
  osh> unviewNode -              <string:OCamera>
  osh> unviewNode <string:ONode> -
  osh> unviewNode <string:ONode> <string:OCamera>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera* cameras = NULL;
  ONode*   nodes   = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  nodes   = (ONode*)   OProcessGetTypedObjects (a_process,a_args[1],"ONode");
  cameras = (OCamera*) OProcessGetTypedObjects (a_process,a_args[2],"OCamera");

 {OCamera* ca;
  ONode*   na;
  for(ca = cameras;(ca!=NULL) && (*ca!=NULL);ca++)
    {
      for(na = nodes;(na!=NULL) && (*na!=NULL);na++)
	{
	  OCameraUnviewNode (*ca,*na);
	}
    }}

  CListDelete         ((CList)cameras);
  CListDelete         ((CList)nodes);

  OProcessPutHandles (a_process,NULL);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_isCameraEmpty (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> isCameraEmpty <string:OCamera>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera ocamera;
/*.........................................................................*/
  if(a_argn!=2) 
    {
      CWarnF("One argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  ocamera = OCameraGetIdentifier(a_args[1]);
  if(ocamera==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  return OCameraIsEmpty(ocamera)==1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
/***************************************************************************/
int GoExecute_normalizeNode (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/*****************************************************************6**********/
/*
  osh> normalizeNode -              <string:OCamera>
  osh> normalizeNode <string:ONode> <string:OCamera>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera   ocamera;
  ONode*    nodes   = NULL;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  ocamera = OCameraGetIdentifier(a_args[2]);
  if(ocamera==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  nodes                  = (ONode*) OProcessGetTypedObjects  (a_process,a_args[1],"ONode");
  OCameraNormalizeMatrix (ocamera,nodes);
  CListDelete            ((CList)nodes);

  OProcessPutHandles    (a_process,NULL);

  return                 EXIT_SUCCESS;
}
/********************************************a*******************************/
int GoExecute_rotateCamera (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> rotateCamera <string:OCamera> [x,y,z] <double:angle>
  osh> rotateCamera <string:OCamera> [x,y,z] <double:angle> [camera, scene, highlight]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera camera;
  OMatrix matrix = NULL;
  double  value;
  int     status;
/*.........................................................................*/
  if( (a_argn!=4) && (a_argn!=5) )
    {
      CWarnF("Three or four arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  camera = OCameraGetIdentifier(a_args[1]);
  if(camera==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  value       = CStringConvertToDouble(a_args[3],&status); 
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  value       = value * M_PI / 180.; 
       if(strcmp(a_args[2],"x")==0) matrix  = OMatrixCreate (OMatrixRotationX,value);
  else if(strcmp(a_args[2],"y")==0) matrix  = OMatrixCreate (OMatrixRotationY,value);
  else if(strcmp(a_args[2],"z")==0) matrix  = OMatrixCreate (OMatrixRotationZ,value);
  else 
    {
      CWarnF("Axe %s unknown (x, y, z expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
    }
  MultiplyMatrix (a_argn==4 ? CStringNULL : a_args[4],camera,matrix);
  OMatrixDelete  (matrix);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_translateCamera (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> translateCamera <string:OCamera> <double:x> <double:y> <double:z>
  osh> translateCamera <string:OCamera> <double:x> <double:y> <double:z> [camera, scene, highlight]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera camera;
  OMatrix matrix = NULL;
  double  x,y,z;
  int     status;
/*.........................................................................*/
  if( (a_argn!=5) && (a_argn!=6) )
    {
      CWarnF("Four or five arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  camera = OCameraGetIdentifier(a_args[1]);
  if(camera==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  x = CStringConvertToDouble(a_args[2],&status); 
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  y = CStringConvertToDouble(a_args[3],&status); 
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  z = CStringConvertToDouble(a_args[4],&status); 
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  matrix            = OMatrixCreate (OMatrixTranslation,x,y,z);
  MultiplyMatrix    (a_argn==5 ? CStringNULL : a_args[5],camera,matrix);
  OMatrixDelete     (matrix);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_scaleCamera (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> scaleCamera <string:OCamera> <double:factor>
  osh> scaleCamera <string:OCamera> <double:factor> [camera, scene, highlight]
  osh> scaleCamera <string:OCamera> <double:x> <double:y> <double:z>
  osh> scaleCamera <string:OCamera> <double:x> <double:y> <double:z>  [camera, scene, highlight]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera camera;
  int     status;
/*.........................................................................*/
  if((a_argn!=3)  && (a_argn!=4) && (a_argn!=5) && (a_argn!=6)) 
    {
      CWarnF("Two to five arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  camera = OCameraGetIdentifier(a_args[1]);
  if(camera==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }


  if( (a_argn==3) || (a_argn==4) )
    {
      OMatrix           matrix = NULL;
      double            value;
      value             = CStringConvertToDouble(a_args[2],&status); 
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      matrix            = OMatrixCreate (OMatrixScale,value,value,value);
      MultiplyMatrix    (a_argn==3 ? CStringNULL : a_args[3],camera,matrix);
      OMatrixDelete     (matrix);
    }
  else if( (a_argn==5) || (a_argn==6) )
    {
      OMatrix           matrix = NULL;
      double            x,y,z;
      x                 = CStringConvertToDouble(a_args[2],&status); 
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      y                 = CStringConvertToDouble(a_args[3],&status); 
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      z                 = CStringConvertToDouble(a_args[4],&status); 
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      matrix            = OMatrixCreate (OMatrixScale,x,y,z);
      MultiplyMatrix    (a_argn==5 ? CStringNULL : a_args[5],camera,matrix);
      OMatrixDelete     (matrix);
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_resetCamera (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> resetCamera <string:OCamera>
  osh> resetCamera <string:OCamera> [camera, scene, highlight, context]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera  ocamera;
  ONode* list;
  ONode* itema;
/*.........................................................................*/
  if((a_argn!=2) && (a_argn!=3)) 
    {
      CWarnF("One or tow arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  
  ocamera = OCameraGetIdentifier(a_args[1]);
  if(ocamera==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  if(a_argn==2)
    {
      OCameraRestoreDefaultUpVectorAndVRP (ocamera);
      OCameraRestoreDefaultMatrix         (ocamera);
      list                 = ONodeCollect(OCameraGetNodes(ocamera),OCollectHighlighted);
      for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) 
	ONodeAssignMatrixToIdentity (*itema);
      CListDelete           ((CList)list);
    }
  else if(strcmp(a_args[2],"camera")==0)
    {
      OCameraRestoreDefaultUpVectorAndVRP    (ocamera);
    }
  else if(strcmp(a_args[2],"scene")==0)
    {
      OCameraRestoreDefaultMatrix   (ocamera);
    }
  else if(strcmp(a_args[2],"highlight")==0)
    { 
      list                 = ONodeCollect(OCameraGetNodes(ocamera),OCollectHighlighted);
      for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) 
	ONodeAssignMatrixToIdentity (*itema);
      CListDelete           ((CList)list);
    }
  else if(strcmp(a_args[2],"context")==0)
    {
      OContextRestoreDefaultValues (OContextGetStaticInstance());
    }
  else
    {
      CWarnF       ("Bad option %s (camera, scene, highlight expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return       EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_doCameraPick (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> doCameraPick <string:OCamera> [align, center, distance, infos]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera  ocamera;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  ocamera = OCameraGetIdentifier(a_args[1]);
  if(ocamera==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return EXIT_FAILURE;
    }

  if(strcmp(a_args[2],"align")==0)
    {
      OCameraAlignMatrixOnPick  (ocamera,OCameraGetPick(ocamera,0),OCameraGetPick(ocamera,1));
      OProcessPutHandles       (a_process,NULL);
    }
  else if(strcmp(a_args[2],"center")==0)
    {
      OCameraCenterMatrixOnPick (ocamera,OCameraGetPick(ocamera,0));
      OProcessPutHandles       (a_process,NULL);
    }
  else if(strcmp(a_args[2],"distance")==0)
    {
      OPick      pick1,pick2;
      pick1      = OCameraGetPick(ocamera,0);
      pick2      = OCameraGetPick(ocamera,1);
      if( (pick1!=NULL) && (pick2!=NULL) )
	{
          OProcessPutString  (a_process,CStringCreateF(64,"%g",OPickGetDistance(pick1,pick2)));
	}
      else
	{
	  OProcessPutHandles (a_process,NULL);
	}
    }
  else if(strcmp(a_args[2],"infos")==0)
    {
      OPick      pick;
      pick       = OCameraGetPick(ocamera,0);
      if(pick!=NULL)
	{
          OProcessPutString   (a_process,OPickGetInformations(pick));
	}
      else
	{
	  OProcessPutHandles (a_process,NULL);
	}
    }
  else
    {
      CWarnF ("Bad option %s (align, center, distance, infos expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OProcessPutHandles (a_process,NULL);
      return       EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_copyCameraMatrix (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> copyCameraMatrix <string:OCamera> <string:to OCamera> 
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OCamera  ocamera,ocamera2;
/*.........................................................................*/
  if(a_argn!=3)
    {
      CWarnF ("Two arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  ocamera = OCameraGetIdentifier(a_args[1]);
  if(ocamera==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  ocamera2 = OCameraGetIdentifier(a_args[2]);
  if(ocamera2==NULL) 
    {
      CWarnF ("OCamera %s is unknown in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  
  OCameraSetMatrix (ocamera2,OCameraGetMatrix(ocamera));

  return EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_translateNode (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> translateNode <string:ONode> <double:x> <double:y> <double:z>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode   node;
  OMatrix matrix = NULL;
  double  x,y,z;
  int     status;
/*.........................................................................*/
  if(a_argn!=5)
    {
      CWarnF("Four arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  node = ONodeGetIdentifier(a_args[1]);
  if(node==NULL) 
    {
      CWarnF ("ONode %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  x = CStringConvertToDouble(a_args[2],&status); 
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  y = CStringConvertToDouble(a_args[3],&status); 
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[3],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  z = CStringConvertToDouble(a_args[4],&status); 
  if(status==0) 
    {
      CWarnF ("Argument %s not a number in :\n%s\n",a_args[4],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  matrix              = OMatrixCreate (OMatrixTranslation,x,y,z);
  ONodeMultiplyMatrix (node,matrix);
  OMatrixDelete       (matrix);

  return EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_rotateNode (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> rotateNode <string:ONode> [x,y,z] <double:angle in degree> ...
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode   node;
  OMatrix matrix = NULL;
  OMatrix rotate = NULL;
  int     status = 1;
  int     count;
/*.........................................................................*/
  if( (a_argn!=4) && (a_argn!=6) && (a_argn!=8) )
    {
      CWarnF("3, 5 or 7 arguments expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  node = ONodeGetIdentifier(a_args[1]);
  if(node==NULL)
    {
      CWarnF ("ONode %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  for (count=2;count<a_argn;count+=2)
    { 
      char    axis;
      double  angle;

      axis = a_args[count][0];
      if( (axis!='x') && (axis!='y') && (axis!='z') )
	{
	  CWarnF ("Argument %s not a rotation axis [x,y,z] in :\n%s\n",
		  a_args[count],OProcessGetCommand(a_process));
	  break;
	}
      angle = CStringConvertToDouble(a_args[count+1],&status); 
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",a_args[count+1],OProcessGetCommand(a_process));
	  break;
	}
      
      angle *= M_PI/180.;
      if     (axis=='x')
	rotate            = OMatrixCreate(OMatrixRotationX,angle);
      else if(axis=='y')
	rotate            = OMatrixCreate(OMatrixRotationY,angle);
      else
	rotate            = OMatrixCreate(OMatrixRotationZ,angle);

      if (matrix==NULL)
	matrix            = rotate;
      else
	{
	  OMatrix                  temp;
          temp                     = OMatrixDuplicate(matrix);
	  OMatrixMultiplyAndAssign (matrix,temp,rotate);
	  OMatrixDelete            (temp);
	  OMatrixDelete            (rotate);
	}
    }
      
  if (status==1)    ONodeMultiplyMatrix   (node,matrix);

  if (matrix!=NULL) OMatrixDelete  (matrix);
  
  if (status==1)    return EXIT_SUCCESS;
  else              return EXIT_FAILURE;
}
/***************************************************************************/
int GoExecute_addPrimitiveToNode (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> addPrimitiveToNode <string:ONode> [lines,markers,segments] [list of <double:x> <double:y> <double:z>]
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode      node;
  int        number,pointn,count;
  OPointList points;
/*.........................................................................*/
  if(a_argn<=2)
    {
      CWarnF("At least two argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  node = ONodeGetIdentifier(a_args[1]);
  if(node==NULL) 
    {
      CWarnF ("ONode %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  number = a_argn-3;
  pointn = number/3;
  if(3 * pointn != number)
    {
      CWarnF ("3 * n (%d given) real numbers expected in :\n%s\n",number,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  points = OPointListCreate (pointn);

  for(count=0;count<pointn;count++)
    {
      char*   string;
      double  x,y,z;
      int     status;
      string  = a_args[3 + 3 * count];
      x       = CStringConvertToDouble(string,&status); 
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",string,OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      string  = a_args[3 + 3 * count + 1];
      y       = CStringConvertToDouble(string,&status); 
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",string,OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      string  = a_args[3 + 3 * count + 2];
      z       = CStringConvertToDouble(string,&status); 
      if(status==0) 
	{
	  CWarnF ("Argument %s not a number in :\n%s\n",string,OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
      OPointListSetIthEntry   (points,count,x,y,z);
    }

  if(strcmp(a_args[2],"lines")==0)  
    {
      GoAddLinesToNode (node,points);
      OPointListDelete (points);
    }
  else if(strcmp(a_args[2],"markers")==0)  
    {
      GoAddMarkersToNode (node,points);
      OPointListDelete (points);
    }
  else if(strcmp(a_args[2],"segments")==0)  
    {
      GoAddSegmentsToNode (node,points);
      OPointListDelete (points);
    }
  else
    {
      CWarnF ("Invalid keyword %s (lines, markers, segments expected) in :\n%s\n",a_args[2],OProcessGetCommand(a_process));
      OPointListDelete (points);
      return EXIT_FAILURE;
    }      
  
  return           EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_addStringToNode (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> addStringToNode <string:ONode> <string:text> <double:x> <double:y> <double:z> <double:height> <double:width-stretch>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode      node;
  OMatrix    matrix;
  int        number,status;
  char*      string;
  double     height        =  10.0;
  double     width_stretch =   1.0;
  double     x,y,z;
/*.........................................................................*/
  if(a_argn<=5)
    {
      CWarnF("At least 5 argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  node = ONodeGetIdentifier(a_args[1]);
  if(node==NULL)
    {
      CWarnF ("ONode %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  number = 3;
  string = a_args[number++];
  x      = CStringConvertToDouble(string,&status);
  if(status==0)
    {
      CWarnF ("Argument %s not a number for x in :\n%s\n",string,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  string = a_args[number++];
  y      = CStringConvertToDouble(string,&status);
  if(status==0)
    {
      CWarnF ("Argument %s not a number for y in :\n%s\n",string,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  string = a_args[number++];
  z      = CStringConvertToDouble(string,&status);
  if(status==0)
    {
      CWarnF ("Argument %s not a number for z in :\n%s\n",string,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  if(number<a_argn)
    {
      string  = a_args[number++];
      height  = CStringConvertToDouble(string,&status);
      if(status==0)
	{
	  CWarnF ("Argument %s not a number for height in :\n%s\n",string,OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
    }
  if(number<a_argn)
    {
      string = a_args[number++];
      width_stretch  = CStringConvertToDouble(string,&status);
      if(status==0)
	{
	  CWarnF ("Argument %s not a number for width-stretch in :\n%s\n",
		  string,OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
    }
      
  string               = a_args[2];
  matrix               = OMatrixCreate (OMatrixScale,height * 0.8 * width_stretch,height,1.);
  GoAddStringToNode    (node,string,x,y,z,matrix);
  OMatrixDelete        (matrix);

  return               EXIT_SUCCESS;
}
/***************************************************************************/
int GoExecute_addHTML_PageToNode (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
  osh> addHTML_PageToNode <string:ONode> <string:text> <double:x> <double:y> <double:z> <double:height> <double:width-stretch>
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  ONode      node;
  OMatrix    matrix;
  int        number,status;
  char*      page;
  char*      string;
  double     height        =  10.0;
  double     width_stretch =   1.0;
  double     x,y,z;
/*.........................................................................*/
  if(a_argn<=5)
    {
      CWarnF("At least 5 argument expected in :\n%s\n",OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  node = ONodeGetIdentifier(a_args[1]);
  if(node==NULL)
    {
      CWarnF ("ONode %s is unknown in :\n%s\n",a_args[1],OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }

  page = OProcessGetString(a_process,a_args[2]);

  number = 3;
  string = a_args[number++];
  x      = CStringConvertToDouble(string,&status);
  if(status==0)
    {
      CWarnF ("Argument %s not a number for x in :\n%s\n",string,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  string = a_args[number++];
  y      = CStringConvertToDouble(string,&status);
  if(status==0)
    {
      CWarnF ("Argument %s not a number for y in :\n%s\n",string,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  string = a_args[number++];
  z      = CStringConvertToDouble(string,&status);
  if(status==0)
    {
      CWarnF ("Argument %s not a number for z in :\n%s\n",string,OProcessGetCommand(a_process));
      return EXIT_FAILURE;
    }
  if(number<a_argn)
    {
      string  = a_args[number++];
      height  = CStringConvertToDouble(string,&status);
      if(status==0)
	{
	  CWarnF ("Argument %s not a number for height in :\n%s\n",string,OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
    }
  if(number<a_argn)
    {
      string = a_args[number++];
      width_stretch  = CStringConvertToDouble(string,&status);
      if(status==0)
	{
	  CWarnF ("Argument %s not a number for width-stretch in :\n%s\n",
		  string,OProcessGetCommand(a_process));
	  return EXIT_FAILURE;
	}
    }
      
  matrix               = OMatrixCreate (OMatrixScale,height * 0.8 * width_stretch,height,1.);
  GoAddStringToNode    (node,page,x,y,z,matrix);
  OMatrixDelete        (matrix);

  return               EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static ONode GetNode (
 char*   a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int   status;
  ONode onode;
/*.........................................................................*/
  if(a_string==NULL) return NULL;
  onode = (ONode)CStringConvertToLong(a_string,&status);
  if(status==1)   return onode;
  return ONodeGetIdentifier (a_string);
}
/***************************************************************************/
static void MultiplyMatrix (
 char*   a_what
,OCamera a_camera
,OMatrix a_matrix
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_camera==NULL) return;
  if(a_matrix==NULL) return;
  if(a_what==NULL)
    {
      OCameraMultiplyMatrix (a_camera,a_matrix);
    }
  else if(strcmp(a_what,"camera")==0)
    {
      OCameraMultiplyUpVectorAndVRP  (a_camera,a_matrix);
    }
  else if(strcmp(a_what,"scene")==0)
    {
      OCameraMultiplyMatrix (a_camera,a_matrix);
    }
  else if(strcmp(a_what,"highlight")==0)
    { 
      ONode* list;
      ONode* itema;
      list                 = ONodeCollect(OCameraGetNodes(a_camera),OCollectHighlighted);
      for(itema=list;(itema!=NULL) && (*itema!=NULL);itema++) 
	ONodeMultiplyMatrix (*itema,a_matrix);
      CListDelete           ((CList)list);
    }
  else
    {
      CWarnF("Bad option %s.[camera, scene, highlight] expected.\n",a_what);
    }
}
