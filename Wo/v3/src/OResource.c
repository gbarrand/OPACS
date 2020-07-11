/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <string.h>

#include <CList.h>

#include <OResource.h>

static struct 
{
  OResourceGetOptionsFunction*            optionsProcs;
  OResourceConvertValueToStringFunction*  getProcs;
  OResourceConvertStringToChangeFunction* addProcs;
  OResourceGetCallbackDataFunction*       cbkValueProcs;
} Class = {NULL,NULL,NULL,NULL};
/***************************************************************************/
void OResourceClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CListDelete          ((CList)Class.optionsProcs);
  CListDelete          ((CList)Class.addProcs);
  CListDelete          ((CList)Class.getProcs);
  CListDelete          ((CList)Class.cbkValueProcs);
  Class.optionsProcs  = NULL;
  Class.addProcs      = NULL;
  Class.getProcs      = NULL;
  Class.cbkValueProcs = NULL;
}
/***************************************************************************/
void OResourceAddGetOptionsFunction (
 OResourceGetOptionsFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_proc==NULL)      return;
  CListAddEntryFunction ((CList*)&Class.optionsProcs,(CEntryFunction)a_proc);
  CListRemoveDuplicates ((CList)Class.optionsProcs,NULL,0);
}
/***************************************************************************/
void OResourceAddConvertValueToStringFunction (
 OResourceConvertValueToStringFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_proc==NULL)      return;
  CListAddEntryFunction ((CList*)&Class.getProcs,(CEntryFunction)a_proc);
  CListRemoveDuplicates ((CList)Class.getProcs,NULL,0);
}
/***************************************************************************/
void OResourceAddConvertStringToChangeFunction (
 OResourceConvertStringToChangeFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_proc==NULL)      return;
  CListAddEntryFunction ((CList*)&Class.addProcs,(CEntryFunction)a_proc);
  CListRemoveDuplicates ((CList)Class.addProcs,NULL,0);
}
/***************************************************************************/
void OResourceAddGetCallbackDataFunction (
 OResourceGetCallbackDataFunction a_proc
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_proc==NULL)      return;
  CListAddEntryFunction ((CList*)&Class.cbkValueProcs,(CEntryFunction)a_proc);
  CListRemoveDuplicates ((CList)Class.cbkValueProcs,NULL,0);
}
/***************************************************************************/
char** OResourceGetOptions (
 char* a_type
,int* a_number
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OResourceGetOptionsFunction* itema;
/*.........................................................................*/
  if(a_number!=NULL) *a_number = 0;
  if( (a_type==NULL) || (*a_type=='\0') ) return NULL;
  for(itema = Class.optionsProcs;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      char** list;
      list   = (*itema)(a_type,a_number);
      if(list!=NULL) return list;
    }
  if(a_number!=NULL) *a_number = 0;
  return NULL;
}
/***************************************************************************/
char* OResourceConvertValueToString (
 Widget    a_widget 
,char*     a_name 
,char*     a_type 
,XtArgVal* a_addr
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OResourceConvertValueToStringFunction* itema;
/*.........................................................................*/
  if(Class.getProcs==NULL) return NULL;
/*for(itema = Class.getProcs;(itema!=NULL) && (*itema!=NULL);itema++)*/
 {int   count,itemn;
  itemn = CListGetSize ((CList)Class.getProcs);
  for(count=itemn-1;count>=0;count--)
    {
      char*      string;
      itema      = Class.getProcs + count;
      string     = (*itema)(a_widget,a_name,a_type,a_addr);
      if(string!=NULL) return string;
    }}
  return NULL;
}
/***************************************************************************/
int OResourceConvertStringToChange (
 Widget   a_widget 
,char*    a_name 
,char*    a_type 
,char*    a_value
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OResourceConvertStringToChangeFunction* itema;
/*.........................................................................*/
  if(Class.addProcs==NULL) return 0;
/*for(itema = Class.addProcs;(itema!=NULL) && (*itema!=NULL);itema++)*/
 {int   count,itemn;
  itemn = CListGetSize ((CList)Class.addProcs);
  for(count=itemn-1;count>=0;count--)
    {
      int        status;
      itema      = Class.addProcs + count;
      status     = (*itema)(a_widget,a_name,a_type,a_value);
      if(status==1) return 1;
    }}
  return 0;
}
/***************************************************************************/
char* OResourceGetCallbackData (
 Widget   a_widget 
,void*    a_data
,char*    a_what
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  OResourceGetCallbackDataFunction* itema;
/*.........................................................................*/
  if(Class.cbkValueProcs==NULL) return NULL;
  for(itema = Class.cbkValueProcs;(itema!=NULL) && (*itema!=NULL);itema++)
    { 
      char*   xstring;
      xstring = (*itema)(a_widget,a_data,a_what);
      if(xstring!=NULL) return xstring;
    }
  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
int OResourceIsPixmap (
 char* a_type 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_type!=NULL)
    {
      if(strcmp(a_type,"Pixmap")==0)                 return 1;
      if(strcmp(a_type,"DynamicPixmap")==0)          return 1;
      if(strcmp(a_type,"Bitmap")==0)                 return 1;
      if(strcmp(a_type,"PrimForegroundPixmap")==0)   return 1;
      if(strcmp(a_type,"PrimHighlightPixmap")==0)    return 1;
      if(strcmp(a_type,"PrimTopShadowPixmap")==0)    return 1;
      if(strcmp(a_type,"PrimBottomShadowPixmap")==0) return 1;
      if(strcmp(a_type,"ManTopShadowPixmap")==0)     return 1;
      if(strcmp(a_type,"ManBottomShadowPixmap")==0)  return 1;
      if(strcmp(a_type,"ManHighlightPixmap")==0)     return 1;
      if(strcmp(a_type,"ManForegroundPixmap")==0)    return 1;
      if(strcmp(a_type,"BackgroundPixmap")==0)       return 1;
    }

  if(a_name!=NULL)
    {
/*Xaw*/
      if(strcmp(a_name,"icon")==0)                    return 1;
      if(strcmp(a_name,"leftBitmap")==0)              return 1;
      if(strcmp(a_name,"rightBitmap")==0)             return 1;
      if(strcmp(a_name,"bitmap")==0)                  return 1;
      if(strcmp(a_name,"fullPixmap")==0)              return 1;
      if(strcmp(a_name,"fullPixmapMask")==0)          return 1;
      if(strcmp(a_name,"emptyPixmap")==0)             return 1;
      if(strcmp(a_name,"emptyPixmapMask")==0)         return 1;
      if(strcmp(a_name,"thumb")==0)                   return 1;
      if(strcmp(a_name,"stipple")==0)                 return 1;
/*Xm*/
      if(strcmp(a_name,"labelPixmap")==0)             return 1;
      if(strcmp(a_name,"backgroundPixmap")==0)        return 1;
      if(strcmp(a_name,"borderPixmap")==0)            return 1;
      if(strcmp(a_name,"iconPixmap")==0)              return 1;
      if(strcmp(a_name,"cascadePixmap")==0)           return 1;
      if(strcmp(a_name,"labelInsensitivePixmap")==0)  return 1;
      if(strcmp(a_name,"symbolPixmap")==0)            return 1;
      if(strcmp(a_name,"armPixmap")==0)               return 1;
      if(strcmp(a_name,"selectPixmap")==0)            return 1; 
      if(strcmp(a_name,"selectInsensitivePixmap")==0) return 1;
      if(strcmp(a_name,"iconMask")==0)                return 1;
    }

  return 0;
}
/***************************************************************************/
int OResourceIsCursor (
 char* a_type 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_type!=NULL)
    {
      if(strcmp(a_type,"Cursor")==0)               return 1;
    }

  if(a_name!=NULL) 
    {
      if(strcmp (a_name,"cursor")==0)       return 1; /*Xaw*/
      if(strstr  (a_name,"Cursor")!=NULL)       return 1; /*Xaw*/
    }

  return 0;
}
/***************************************************************************/
int OResourceIsFontList (
 char* a_type 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_type!=NULL)
    {
      if(strcmp(a_type,"XmFontList")==0)               return 1;
      if(strcmp(a_type,"FontList")==0)                 return 1;
    }

  if(a_name!=NULL) 
    {
      if(strcmp(a_name,"fontList")==0)                 return 1;
      if(strcmp(a_name,"defaultFontList")==0)          return 1;
      if(strcmp(a_name,"textFontList")==0)             return 1;
      if(strcmp(a_name,"labelFontList")==0)            return 1;
      if(strcmp(a_name,"buttonFontList")==0)           return 1;
    }

  return 0;
}
/***************************************************************************/
int OResourceIsFont (
 char* a_type 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_type!=NULL)
    {
      if(strcmp(a_type,"Font")==0)       return 1;
    }

  if(a_name!=NULL) 
    {
      if(strcmp(a_name,"titleFont")==0)  return 1;
    }

  return 0;
}
/***************************************************************************/
int OResourceIsFontStruct (
 char* a_type 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_type!=NULL)
    {
      if(strcmp(a_type,"FontStruct")==0) return 1;
    }

  if(a_name!=NULL) 
    {
      if(strcmp(a_name,"font")==0)  return 1; /*Xaw*/
    }

  return 0;
}
/***************************************************************************/
int OResourceIsAttachment (
 char* a_type 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name!=NULL)  
    {
      if(strcmp(a_name,"topAttachment")==0)           return 1;
      if(strcmp(a_name,"topWidget")==0)               return 1;

      if(strcmp(a_name,"leftAttachment")==0)          return 1;
      if(strcmp(a_name,"leftWidget")==0)              return 1;

      if(strcmp(a_name,"rightAttachment")==0)         return 1;
      if(strcmp(a_name,"bottomAttachment")==0)        return 1;

      if(strcmp(a_name,"bottomWidget")==0)            return 1;
      if(strcmp(a_name,"rightWidget")==0)             return 1;

      if(strcmp(a_name,"topPosition")==0)             return 1;
      if(strcmp(a_name,"bottomPosition")==0)          return 1;
      if(strcmp(a_name,"leftPosition")==0)            return 1;
      if(strcmp(a_name,"rightPosition")==0)           return 1;

      if(strcmp(a_name,"topOffset")==0)               return 1;
      if(strcmp(a_name,"bottomOffset")==0)            return 1;
      if(strcmp(a_name,"leftOffset")==0)              return 1;
      if(strcmp(a_name,"rightOffset")==0)             return 1;
    }

  a_type = NULL;
  return 0;
}
/***************************************************************************/
int OResourceIsItemList (
 char* a_type 
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_name!=NULL)  
    {
      if(strcmp(a_name,"items")==0)            return 1;
      if(strcmp(a_name,"listItems")==0)        return 1;
      if(strcmp(a_name,"selectedItems")==0)    return 1;
      if(strcmp(a_name,"historyItems")==0)     return 1;
    }
  a_type = NULL;
  return 0;
}
/***************************************************************************/
int OResourceIsReferenceList (
 char* a_type
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_type!=NULL)
    {
      if(strcmp(a_type,"Window")==0)     return 1;
      if(strcmp(a_type,"Widget")==0)     return 1;
      if(strcmp(a_type,"MenuWidget")==0) return 1;
      if(strcmp(a_type,"WidgetList")==0) return 1;
    }

  if(a_name!=NULL) 
    {
/*Xaw*/
      if(strcmp(a_name,"fromVert")==0)     return 1;
      if(strcmp(a_name,"fromHoriz")==0)    return 1;
      if(strcmp(a_name,"treeParent")==0)   return 1;
      if(strcmp(a_name,"popupOnEntry")==0) return 1;
      if(strcmp(a_name,"textSource")==0)   return 1;
      if(strcmp(a_name,"textSink")==0)     return 1;
      if(strcmp(a_name,"radioGroup")==0)   return 1;

/* MenuWidget */
/*todo put this in WoXm.c*/  if(strcmp(a_name,"subMenuId")==0) return 1;
/* Window */
      if(strcmp(a_name,"topWidget")==0) return 1;
      if(strcmp(a_name,"bottomWidget")==0) return 1;
      if(strcmp(a_name,"leftWidget")==0) return 1;
      if(strcmp(a_name,"rightWidget")==0) return 1;

/* Xo  */
      if(strcmp(a_name,"transformationSensitiveWidgets")==0)   return 1;
      if(strcmp(a_name,"linkedWidgets")==0) return 1;
/* MenuWidget */
      if(strcmp(a_name,"menuHistory")==0) return 1;
      if(strcmp(a_name,"menuHelpWidget")==0) return 1;
/* Window */
      if(strcmp(a_name,"iconWindow")==0) return 1;
      if(strcmp(a_name,"windowGroup")==0) return 1;
      if(strcmp(a_name,"defaultButton")==0) return 1;
      if(strcmp(a_name,"cancelButton")==0) return 1;
      if(strcmp(a_name,"horizontalScrollBar")==0) return 1;
      if(strcmp(a_name,"verticalScrollBar")==0) return 1;
      if(strcmp(a_name,"commandWindow")==0) return 1;
      if(strcmp(a_name,"menuBar")==0) return 1;
      if(strcmp(a_name,"messageWindow")==0) return 1;
      if(strcmp(a_name,"workWindow")==0) return 1;
      if(strcmp(a_name,"clipWindow")==0) return 1;
/* Widget */
      if(strcmp(a_name,"transientFor")==0) return 1;
      if(strcmp(a_name,"referenceCamera")==0)  return 1;
/* WidgetList */
      if(strcmp(a_name,"children")==0)     return 1;
      if(strcmp(a_name,"postFromList")==0) return 1;
    }

  return 0;
}
/***************************************************************************/
int OResourceIsMultiLine (
 char* a_type
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_type!=NULL)
    {
      if(strcmp(a_type,"Callback")==0)     return 1;
      if(strcmp(a_type,"EventHandler")==0) return 1;
    }

  if(a_name!=NULL) 
    {
      if(strcmp(a_name,"value")==0)        return 1; /*Xm:  XmText: XtNvalue*/
      if(strcmp(a_name,"list")==0)         return 1; /*Xaw: List:   XtNlist*/
      if(strcmp(a_name,"getValue")==0)     return 1; /*Xaw: StripChart*/
      if(strstr (a_name,"Proc")!=NULL)     return 1; /*Xaw:Scrollbar:jumpProc callback,...*/
      if(strcmp(a_name,"callback")==0)     return 1; /*Xaw*/
      if(strstr (a_name,"Callback")!=NULL) return 1;
      if(strcmp(a_name,"eventHandler")==0) return 1;
    }

  return 0;
}

