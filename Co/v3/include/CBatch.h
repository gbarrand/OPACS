/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef CBatch_h
#define CBatch_h

#ifdef __cplusplus
extern "C"{
#endif
typedef char*(*CBatchAskPasswordFunction)(char*);

void  CBatchClearClass              ();
void  CBatchSetDisplay              (char*);
void  CBatchSetAskPasswordFunction  (CBatchAskPasswordFunction);
void  CBatchSendToUNIX_SH           (char*,char*);
void  CBatchSendToUNIX_CSH          (char*,char*);
void  CBatchSendToVMS_ThroughUCX    (char*,char*);
void  CBatchSendToVMS_ThroughDECNET (char*,char*,char*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindCBatch(0); /**/
#endif

#endif  /*CBatch_h*/
