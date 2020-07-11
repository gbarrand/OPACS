#ifndef OBatchP_h
#define OBatchP_h

#include <OBatch.h>

#define BUFFERSIZE     255

typedef struct _OBatchRecord 
{
  char*                   command;
  int                     pread;               /*pipe read*/
  int                     pwrite;              /*pipe write*/
  int                     pid;                 /*pid of process*/
  int                     end;                 /*end process flag*/
  OBatchTreatmentFunction treatment;           /*read value treatment*/
  OBatchTreatmentFunction posttreatment;       /*read value post treatment*/
  char                    line[BUFFERSIZE+1];  /*line read*/
  void*                   userData;
  OBatchUserFunction      userFunction;
} OBatchRecord;

#endif  /*OBatchP_h*/
