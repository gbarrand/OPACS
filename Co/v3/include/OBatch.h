#ifndef OBatch_h
#define OBatch_h

typedef struct _OBatchRecord *OBatch;

typedef void(*OBatchTreatmentFunction)(OBatch);
typedef void(*OBatchUserFunction)(void*,char*);

#ifdef __cplusplus
extern "C" {
#endif
void               OBatchClearClass           ();
OBatch*            OBatchGetIdentifiers       ();
int                OBatchGetExtentSize        ();
void               OBatchDeclareExtentInFDSET (void*);
void               OBatchSurveyExtent         ();
int                OBatchIsValid              (OBatch);
OBatch             OBatchCreate               (char*,OBatchTreatmentFunction,OBatchTreatmentFunction);
void               OBatchDelete               (OBatch);
char*              OBatchGetString            (OBatch);
char*              OBatchGetCommand           (OBatch);
int                OBatchGetChannel           (OBatch);
void               OBatchKillProcess          (OBatch);
int                OBatchSurvey               (OBatch);
int                OBatchGetAttribute         (OBatch,char*,void*,void*,int*);
void               OBatchSetUserData          (OBatch,void*);
void*              OBatchGetUserData          (OBatch);
void               OBatchSetUserFunction      (OBatch,OBatchUserFunction);
OBatchUserFunction OBatchGetUserFunction      (OBatch);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOBatch(0); /**/
#endif

#endif  /*OBatch_h*/
