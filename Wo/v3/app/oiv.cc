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

#ifdef HAS_INVENTOR
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodekits/SoAppearanceKit.h>
#include <Inventor/nodekits/SoShapeKit.h>
#include <XWidget.h>
#include <OEvent.h>
#endif 

/*begin Want_h*/
#include <WoWo.h>
#include <WoXtw.h>
#include <WoXm.h>
#include <WoXo.h>
#include <WoHTML.h>
#include <WoXz.h>
#include <WoDeX.h>
#include <WoCi.h>
#include <WoPacksCi.h>
#include <Wotcl.h>
#include <WoHoXz.h>
#include <WoXaw.h>
#include <WoG3o.h>
#include <WoFNAL.h>
#include <WoPAW.h> /*Need HAS_XZ and HAS_PAW flags.*/
/*end Want_h*/

#include <CPrinter.h>

static char what[] = "@(#)oiv v3";
/***************************************************************************/
int main (
 int a_argn
,char** a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
#ifdef HAS_INVENTOR
  Widget       topWidget;
  XtAppContext appContext;
#endif
/*.........................................................................*/
  what[0] = '@'; /*c++ no warning.*/
  if(a_argn<=1)
    {
      CInfo ("Usage:\n\
oiv.exe <odb file>\n\
");
      return EXIT_FAILURE;
    }

/*begin Want_c*/
#include <WoWo.ic>
#include <WoXtw.ic>
#include <WoXm.ic>
#include <WoXo.ic>
#include <WoHTML.ic>
#include <WoXz.ic>
#include <WoDeX.ic>
#include <WoCi.ic>
#include <WoPacksCi.ic>
#include <Wotcl.ic>
#include <WoHoXz.ic>
#include <WoXaw.ic>
#include <WoG3o.ic>
#include <WoFNAL.ic>
#include <WoPAW.ic>
/*end Want_c*/

  WoSetInterfaceFile (a_args[1]);
  WoStartup          (a_argn,a_args);

#ifdef HAS_INVENTOR
  topWidget          = XWidgetGetTop();
  appContext         = XtWidgetToApplicationContext(topWidget);
  SoXt::init         (topWidget);
  // A little scene graph.
  SoSeparator* root=new SoSeparator;
  root->ref();

  SoOrthographicCamera* camera=new SoOrthographicCamera;
  root->addChild(camera);

  SoRotationXYZ*  rotation = new SoRotationXYZ();
  rotation->axis  = SoRotationXYZ::Y;
  rotation->angle = M_PI/4.5;
  root->addChild  (rotation);

  SoMaterial*     cubeMat = new SoMaterial;
  cubeMat->ambientColor.setValue (1.,0.,0.);
  root->addChild  (cubeMat);
  SoCube*         cube=new SoCube;
  root->addChild  (cube);

  SoAppearanceKit* appearanceKit = new SoAppearanceKit;
  root->addChild(appearanceKit);
  SoMaterial* theMaterial = (SoMaterial*)appearanceKit->getPart("material",TRUE);
  if(theMaterial==NULL) 
    {
      CWarn ("material not found.\n");
    }
  else
    theMaterial->ambientColor.setValue (0.,1.,0.);
  SoCylinder*     cylinder=new SoCylinder;
  root->addChild  (cylinder);

  //To test that getPart works.
  SoShapeKit* shapeKit = new SoShapeKit;
  SoMaterial* material = (SoMaterial*)shapeKit->getPart("appearance.material",TRUE);
  if(material==NULL) 
    {
      CWarn ("appearance.material not found.\n");
    }
  else
    {
      material->ambientColor.setValue (0.,0.,1.);
    }
  SoTransform* transform = (SoTransform*)shapeKit->getPart("transform",TRUE);
  if(transform==NULL) 
    {
      CWarn ("appearance.transform not found.\n");
    }
  else
    {
      transform->rotation.setValue(SbRotation(SbVec3f(1.,1.,1.),M_PI/4.));
      //transform->center.setValue(1.,1.,1.);
      transform->translation.setValue(1.,1.,1.);
    }
  root->addChild  (shapeKit);
  SoCylinder*     cylinder2 = new SoCylinder;
  cylinder2->radius.setValue(0.5);
  root->addChild  (cylinder2);

  // View the scene in an Examiner Viewer
  //SoXtExaminerViewer* myViewer=new SoXtExaminerViewer(myWindow);
  SoXtExaminerViewer *myViewer=new SoXtExaminerViewer(NULL);
  myViewer->setSceneGraph(root);
  myViewer->viewAll();
  myViewer->show();
#endif

#ifdef HAS_INVENTOR
  while(1)
    { 
      XEvent          xevent;
      XtAppNextEvent  (appContext,&xevent);
      if ( (xevent.xany.type==ClientMessage) && 
           (xevent.xclient.data.l[0]==OEventExit) )
        { 
          WoDispatchEvent (&xevent);
          break;
	}
      else if(WoDispatchEvent(&xevent)!=0)
	continue;
      else if(SoXt::dispatchEvent(&xevent)!=0)
	continue;
      XtDispatchEvent(&xevent);
    }
#ifdef SoINVENTOR_HEP
  root->unref();
  delete myViewer;
  SoXt::clear();
#endif
#else
  WoProcessEvents    ();
#endif

  WoClearClass       ();

  return             EXIT_SUCCESS;
}
