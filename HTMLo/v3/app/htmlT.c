#include <stdio.h>
#include <stdlib.h>

#ifdef HAS_HTML

#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <CFile.h>

#include <HTML.h>

#ifdef __cplusplus
extern "C"{
#endif 
static void AnchorCbk      (Widget,XtPointer,XtPointer);
#ifdef __cplusplus
}
#endif

#define HREF_START "htmlT.htm"

static XtAppContext appContext = NULL;
static Widget whtml = NULL;

typedef char* Ligne;
static struct {
  char* text;
} Class = {NULL};
/***************************************************************************/
int main (
 int   a_argn
,char* a_args[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Widget top,shell;
  Arg    args[2];
/*.........................................................................*/
 {char* senv;
  senv  = getenv ("XENVIRONMENT");
  if(!senv || (!strstr(senv,"htmlT.xrm") && !strstr(senv,"HTMLT.XRM") ) )
    {
      printf ("set XENVIRONMNET variable to htmlT.xrm.\n\
  csh> setenv XENVIRONMENT \"../usr/htmlT.xrm\"\n\
  sh>  XENVIRONMENT=\"../usr/htmlT.xrm\";export XENVRONMENT\n\
  NT>  set XENVIRONMENT=..\\usr\\htmlT.xrm\n\
  VMS$ define/nolog XENVIRONMENT [-.usr]htmlT.xrm\n");
      exit   (EXIT_FAILURE);
    }}

 {
#if XtSpecificationRelease == 4
  Cardinal argn;
  argn     = (Cardinal)a_argn;
#else
  int      argn;
  argn     = a_argn;
#endif
  top      = XtAppInitialize(&appContext,"htmlT",NULL,(Cardinal)0,&argn,a_args,NULL,NULL,(Cardinal)0);}
  
/*Add WMClose does not works on ApplicationShell.*/
  shell            = XtAppCreateShell ("xoShellT","Shell",topLevelShellWidgetClass,XtDisplay(top),args,0); 

  whtml            = XtCreateManagedWidget("whtml",htmlWidgetClass,shell,args,0);

  XtAddCallback    (whtml,WbNanchorCallback,AnchorCbk,NULL);

 {FILE* file;
  file  = CFileOpen (HREF_START,NULL);
  if(file)
    {
      Class.text      = CFileRead (file,NULL);
      fclose          (file);
      HTMLSetText     (whtml,Class.text,NULL,NULL,0,NULL,NULL);
    }
  else
    {
      printf ("Can't open %s file.\nYou must start htmlT.exe in usr directory.\n",HREF_START);
    }}

  XtRealizeWidget        (shell);

  XtMapWidget            (shell);

  XtAppMainLoop          (appContext);

  return          1;
}
/***************************************************************************/
static void AnchorCbk (
 Widget This 
,XtPointer a_tag
,XtPointer a_data
)
/***************************************************************************/
/*  from code of (Mosaic-2.2/src/gui.c)mo_load_window_text                 */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char*       href;
  char*       tref;
/*.........................................................................*/
  href        = ((WbAnchorCallbackData*)a_data)->href;
  tref        = ((WbAnchorCallbackData*)a_data)->text; 
 
  printf ("href:%s tref:%s\n",href,tref);

  This   = NULL;
  a_tag  = NULL;
  a_data = NULL;
}
#else
int main() {printf ("C macro HAS_HTML not defined.\n");return 0;}
#endif
