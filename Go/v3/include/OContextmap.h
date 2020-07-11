#ifndef Contextmap_h
#define Contextmap_h

typedef enum {
  OPackingUndefined = 0,
  OPackingHorizontal,
  OPackingVertical
} OPacking;

typedef struct _OContextmapRecord *OContextmap;

#define ICON_MARKER             (1<<0)
#define ICON_LINE               (1<<1)
#define ICON_CIRCLE             (1<<2)
#define ICON_SQUARE             (1<<3)
#define ICON_DIAMOND            (1<<4)

#include <OType.h>
#ifdef __cplusplus
extern "C"{
#endif
void          OContextmapClearClass              ();
OContextmap*  OContextmapGetIdentifiers          ();
OContextmap   OContextmapGetIdentifier           (char*);
void          OContextmapSetCurrent              (OContextmap);
OContextmap   OContextmapGetCurrent              ();
double        OContextmapGetBaseLineStretch      ();
double        OContextmapGetLabelWidthStretch    ();

OContextmap   OContextmapMake                    ();
void          OContextmapDelete                  (OContextmap);
int           OContextmapIsValid                 (OContextmap);
void          OContextmapSetName                 (OContextmap,char*);
char*         OContextmapGetName                 (OContextmap);
void          OContextmapSetObjectContext        (OContextmap,OType,OIdentifier);
int           OContextmapSetAttribute            (OContextmap,char*,void*,void*,int);
int           OContextmapGetAttribute            (OContextmap,char*,void*,void*,int*);
void          OContextmapResetHits               (OContextmap);
void          OContextmapSetDefaultContext       (OContextmap);
void          OContextmapSetLabelContext         (OContextmap);
void          OContextmapSetIthContext           (OContextmap,int);
void          OContextmapSetPacking              (OContextmap,OPacking);
OPacking      OContextmapGetPacking              (OContextmap);
int           OContextmapGetContextNumber        (OContextmap);
int           OContextmapGetIconTypes            (OContextmap);
double        OContextmapGetIconSize             (OContextmap);
char*         OContextmapGetTitle                (OContextmap);
int           OContextmapShowHitsOnly            (OContextmap);
int           OContextmapGetIthContextHitNumber  (OContextmap,int);

void          OContextmapGetLabelPosition        (OContextmap,double*,double*,double*);
char*         OContextmapGetContextLabel         (OContextmap,int);
double        OContextmapGetLabelHeight          (OContextmap);
OMatrix       OContextmapGetLabelRotationMatrix  (OContextmap);
OPacking      OContextmapGetLabelPacking         (OContextmap);
                                    
char**        OPackingGetNames                   (int*);
OPacking      OPackingGetIdentifier              (char*);
char*         OPackingGetName                    (OPacking);
#ifdef __cplusplus
}
#endif

#endif/*Contextmap_h*/

