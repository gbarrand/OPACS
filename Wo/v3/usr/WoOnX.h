#ifndef WoOnX_h
#define WoOnX_h

#include <CString.h>
#include <XWidget.h>
#include <OWidget.h>
#include <Xm/Text.h>

#define CStringNew          CStringDuplicate
#define CStringPrintf       CStringCreateFormatted

#define OWidgetGetReference OWidgetGetIdentifier
#define OWidgetGetResource  OWidgetGetAndConvertResourceToString
#define OWidgetSetResource  OWidgetSetResourceFromString

#endif  /*WoOnX_h*/
