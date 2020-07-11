#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <CPrinter.h>
#include <CMemory.h>

#include <OBatch.h>

#ifdef __cplusplus
extern "C"{
#endif
static void Submit     (char*);
static void TreatLine  (OBatch);
static void EndCommand (OBatch);
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
  time_t t1,t2;
/*.........................................................................*/
  CInfo   ("HELLO\n");
  time    (&t1);
  CInfoF  ("argn = %d\n",a_argn);
  if (a_argn >=2)
    {
      Submit (a_args[1]);
    }
  else
    {
      CInfo  ("default command is ls \n");
      Submit ("ls");
    }
  time   (&t2);
  CInfoF ("ELASPSE= %d\n",t2-t1);

  OBatchClearClass ();

 {char*      string;
  string     = CMemoryGetStatus();
  if(string!=NULL) CInfoF ("%s\n",string);}

  return     EXIT_SUCCESS;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void Submit (
 char* a_command
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  /*  OBatch ospawn;*/
/*.........................................................................*/
  CInfo ("======== BEGIN ===================\n");
  OBatchCreate (a_command,TreatLine,EndCommand);
  /*ospawn = */OBatchCreate ("ls;sleep 5;echo '---------------';ls",TreatLine,EndCommand);

/*  OBatchKillProcess (ospawn);*/

  while (OBatchGetExtentSize()!=0) OBatchSurveyExtent();

  CInfo ("======== END ===================\n");
}
/***************************************************************************/
static void TreatLine (
 OBatch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CInfoF ("READ [%s]>>>%s",OBatchGetCommand(This),OBatchGetString(This));
}
/***************************************************************************/
static void EndCommand (
 OBatch This
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CInfo  ("READ FINISH \n");
  This   = NULL;
}





