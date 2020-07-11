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

/*Co*/
#include <OShell.h>
#include <OType.h>

/*Go*/
#include <OColormap.h>
#include <Go.h>
#include <OCamera.h>

/*G3o*/
#include <G3oEXPO.h>

/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OShell     osh;
  OCamera    camera;
/*.........................................................................*/
  osh                 = OShellCreate ("osh");
  G3oEXPO_Initialize  (osh);

  
  OShellExecute       (osh,"load X11.odb");
  OShellExecute       (osh,"load G3oT.odb");

  camera              = OCameraGetIdentifier ("my_camera");
  OCameraSetColormap  (camera,OColormapGetIdentifier("ocolormap_X"));

  OShellExecute       (osh,"\n\
do Event initialize\n\
#Produce some ONode\n\
#Detector\n\
set OContext color lightblue;collect VOLU where name eq ECAL | represent - | viewNode - my_camera\n\
#set OContext color lightblue;collect VOLU where name eq TOWE | represent - | viewNode - my_camera\n\
set OContext color grey;do Go representFrame FRAME 10. | viewNode - my_camera\n\
#\n\
do Event getNext\n\
#Event\n\
collect KINE | represent - | viewNode - my_camera\n\
#set OContext lineStyle solid lineWidth 10 color magenta\n\
#set TOWE representation herisson;set TOWE norme 100.;collect TOWE | represent - | viewNode - my_camera\n\
do UDET declareHitsAndDigis\n\
set OContext color green;collect HITS_ECAL_TOWE | represent - | viewNode - my_camera\n\
set OContext lineStyle solid color magenta\n\
set TOWE representation herisson;set TOWE norme 200.;collect TOWE | represent - | viewNode - my_camera\n\
");
  
/*Produce a PostScript file.*/
  OCameraOpenPS_FileForWriting   ("out.ps");

/*Page 1.*/
  OCameraPutScenePageInPS_Stream (camera);

/*Page 2.*/
  OShellExecute                  (osh,"rotateCamera my_camera y -90.");
  OCameraPutScenePageInPS_Stream (camera);

/*Page 3.*/
  OShellExecute                  (osh,"resetCamera my_camera;rotateCamera my_camera x 45.;rotateCamera my_camera y 45.");
  OCameraPutScenePageInPS_Stream (camera);

/*Page 4.*/
  OCameraSetBlackAndWhite        (camera,1);
  OCameraPutScenePageInPS_Stream (camera);

/*Page 5.*/
  OCameraSetBlackAndWhite        (camera,2);
  OCameraPutScenePageInPS_Stream (camera);

  OCameraClosePS_Stream          ();
  
  OTypeClearClass                ();

  a_argn              = 0;
  a_args              = NULL;
  return              EXIT_SUCCESS;
}
