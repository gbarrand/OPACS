#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
 
/*Below are the "wanted" packages header files.*/
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
/*end Want_h*/

#ifdef HAS_XO  /*examples Xo*/
#include <OType.h>
#include <Go.h>
#endif /*HAS_XO*/

/*Some prototypes.*/
#ifdef __cplusplus
extern "C"{
#endif
static int     Execute_hello   (int,char**,OProcess);
static void    PrintHello      ();

#ifdef HAS_XO  /*examples Xo*/
static OIdentifier* GetCubeIdentifiers ();
static int     GetCubeAttribute   (OIdentifier,char*,void*,void*,int*);
static ONode   RepresentCube      (OIdentifier);
#endif /*HAS_XO*/

#ifdef __cplusplus
}
#endif
/***************************************************************************/
int main (
 int    a_argn
,char** a_args
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
/*Below are the "wanted" packages c files.*/
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
/*end Want_c*/

/*Example of declaration of a new osh command.*/
/*Commands are added to the default OShell interpreter, created in WoWo.c*/
  OShellAddNewCommand ((OShell)WoGetShellInterpreter(),"OApp/hello",Execute_hello);  
/*The "OApp/.." permits to classify commands.*/

#ifdef HAS_CI /*examples Ci*/
/*Example of a routine declaration to Ci.*/
  CiDo           ("void PrintHello();");
  CiFunctionBind ("PrintHello",(CiRoutine)PrintHello); 
#endif

#ifdef HAS_XO /*examples Xo*/
/*
  You will be able to type (for example from Wo/osh panel):

   osh> collect Cube | represent - | viewNode - my_camera
   osh> collect ONode | delete -
   osh> set OContext color blue;collect Cube where size le 1. | represent - | viewNode - my_camera
   osh> collect ONode | delete -
   osh> set OContext color red; collect Cube where size gt 1. | represent - | viewNode - my_camera

*/
/*Declaration of the type "Cube" and its properties to the type manager.*/
 {OType               otype;
  otype               = OTypeCreate ("Cube");
/*Declare the Cube extent routine.*/
  OTypeSetGetIdentifiersFunction (otype,(OTypeGetIdentifiersFunction)GetCubeIdentifiers); 
/*Declare the Cube get value property routine.*/
  OTypeSetGetAttributeFunction   (otype,(OTypeGetAttributeFunction)GetCubeAttribute); 
/*Declare the Cube representation routine.*/
  OTypeSetRepresentFunction      (otype,(OTypeRepresentFunction)RepresentCube);   
/*Last argument is let to the user.*/
  OTypeAddNewProperty           (otype,"identifier",OPropertyUnsignedLong,NULL); 
  OTypeAddNewProperty           (otype,"name"      ,OPropertyString      ,NULL); 
  OTypeAddNewProperty           (otype,"size"      ,OPropertyDouble      ,NULL); 
  OTypeAddNewProperty           (otype,"x"         ,OPropertyDouble      ,NULL);}
#endif

  WoStartup        (a_argn,a_args); /*Initialize Wo, load OApp.odb file.*/
  WoProcessEvents  ();              /*Main loop. It calls WoDispatchEvent, XtDispatchEvent.*/
  WoClearClass     ();             

  return           EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static int Execute_hello (
 int    a_argn
,char** a_args
,OProcess  a_process
)
/***************************************************************************/
/*
   Example of a user defined command.
   The last argument is the pointer to the command execution context.
  It permits to retrieve/put handlers in the pipe.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPrinterPut ("Hello from a user defined command.\n");
  a_process = NULL;  /*To avoid C++ compilation warning.*/
  a_argn    = 0;     /*To avoid C++ compilation warning.*/
  a_args    = NULL;  /*To avoid C++ compilation warning.*/
  return    EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void PrintHello (
)
/***************************************************************************/
/* Example of routine visible from C interpreter.     
   To trigger this routine from a push button         
   put, on its activateCallback resource, the script: 
   Ci>{OAppHello();}                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CPrinterPut ("Hello\n");
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
#ifdef HAS_XO /*examples Xo*/

/*Object oriented C declaration of the type Cube.*/
typedef struct _CubeRec *Cube;

typedef struct _CubeRec
{
  char*  name;
  double size;
  double x,y,z;
} CubeRec;

/*Create some static instances of the type Cube.*/
static CubeRec cube1 = {"Cube_1",0.5,0.0,0.0,0.0};
static CubeRec cube2 = {"Cube_2",1.0,0.25,0.25,0.25};
static CubeRec cube3 = {"Cube_3",1.5,1.0,1.0,1.0};
static CubeRec cube4 = {"Cube_4",2.0,3.0,3.0,3.0};
static Cube cubes[5] = {&cube1,&cube2,&cube3,&cube4,NULL};
/***************************************************************************/
static OIdentifier* GetCubeIdentifiers (
)
/***************************************************************************/
/*
  Routine that return the null ended list of the Cube instance addresses.
  It is declared to the type manager with the routine OTypeSetGetIdentifiersFunction.
  It is mainly used by the osh collect command.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  return (OIdentifier*)cubes;
}
/***************************************************************************/
static int GetCubeAttribute (
 OIdentifier This
,char*  a_name
,void*  a_user
,void*  a_addr 
,int*   a_number
)
/***************************************************************************/
/*
  Routine to retrieve property values for a Cube instance.
  It is declared to the type manager with the routine OTypeSetGetAttributeFunction.
  It is used, for example, by the osh dump command, and by the osh 
 collect command to do selection over the instances.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if(This==NULL)    return 0;
  if(a_name==NULL)  return 0;
  if(a_addr==NULL)  return 0; 

       if(strcmp(a_name,"identifier")==0)  *((unsigned long*)a_addr)  = (unsigned long)This;
  else if(strcmp(a_name,"name")==0) *((char**)a_addr)  = ((Cube)This)->name;
  else if(strcmp(a_name,"size")==0) *((double*)a_addr) = ((Cube)This)->size;
  else if(strcmp(a_name,"x")==0)    *((double*)a_addr) = ((Cube)This)->x;
  else if(strcmp(a_name,"y")==0)    *((double*)a_addr) = ((Cube)This)->y;
  else if(strcmp(a_name,"z")==0)    *((double*)a_addr) = ((Cube)This)->z;
  else 
    {
      CPrinterPutF ("GetCubeAttribute: unknown property %s.\n",a_name);
      return       0;
    }

  a_user = NULL;
  return 1;
}
/***************************************************************************/
static ONode RepresentCube (
 OIdentifier This
)
/***************************************************************************/
/*
  Routine to represent a Cube instance.
  It create a hierachy of ONodes. In principle the top ONode is returned.
  It is used by the osh represent command.
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Cube    cube;
  ONode   onode;
/*.........................................................................*/
  cube           = (Cube)This;
  onode          = ONodeCreateF (64,"Cube/%lu",cube);
  GoAddBoxToNode (onode,cube->x,cube->y,cube->z,cube->size,cube->size,cube->size);
  return         onode;
}
#endif /*HAS_XO*/
