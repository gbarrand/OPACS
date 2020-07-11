/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef OPiece_h
#define OPiece_h
 
typedef struct _OPieceRecord  *OPiece;

#ifndef NO_C_H
#include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
void     OPieceClearClass          ();
OPiece*  OPieceGetIdentifiers      ();
OPiece   OPieceGetIdentifier       (char*);
OPiece   OPieceMake                ();
OPiece   OPieceCreate              (char*,char*,void*);
void     OPieceDelete              (OPiece);
int      OPieceSetParent           (OPiece,OPiece);
int      OPieceAddChild            (OPiece,OPiece);
OPiece*  OPieceGetChildren         (OPiece);
void     OPieceSetVolatile         (OPiece,int);
void     OPieceSetVolatileChild    (OPiece,int);
OPiece   OPieceGetParent           (OPiece);
void     OPieceSetFile             (OPiece,void*);
void     OPieceSetName             (OPiece,char*);
void     OPieceSetType             (OPiece,char*);
char*    OPieceGetType             (OPiece);
char*    OPieceGetName             (OPiece);
void*    OPieceGetFile             (OPiece);
void     OPieceSetDataBaseName     (OPiece,char*);
char*    OPieceGetDataBaseName     (OPiece);
int      OPieceIsVolatile          (OPiece);
void     OPieceSetStringOfChildren (OPiece,char*);
char*    OPieceGetStringOfChildren (OPiece);
void     OPieceAddResource         (OPiece,char*,char*);
int      OPieceGetResources        (OPiece,char***,char***);
void     OPieceDeleteResources     (OPiece);
void*    OPieceGetWidget           (OPiece);
void     OPieceBuildWidget         (OPiece);
void     OPieceConnect             (OPiece,va_list);
OPiece*  OPieceGetLeaves           (OPiece,int*);
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindOPiece(0);  /**/
#endif

#endif  /*OPiece_h*/
