/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/StringDefs.h>

#include <CMemory.h>
#include <CString.h>
#include <CList.h>
#include <CError.h>
#include <CPrinter.h>
#include <CFile.h>
#include <CGIF.h>
#include <CXPM.h>
#include <CText.h>

#include <XWidget.h>

#include <OResource.h>
#include <XtwTools.h>

#ifdef __cplusplus
extern "C"{
#endif
static char**       GetWords                  (char*,int*);
static Pixmap       ConvertStringToPixmap     (char*,Widget);
static char*        ConvertPixmapToString     (Pixmap);
static KeySym       ConvertStringToKeySymbol  (char*);
static char*        ConvertKeySymbolToString  (KeySym);
static Cursor       ConvertStringToCursor     (char*,Widget);
static char*        ConvertCursorToString     (Cursor);
static XFontStruct* ConvertStringToFontStruct (char*,Widget);
static char*        ConvertFontStructToString (XFontStruct*);
static Font         ConvertStringToFont       (char*,Widget);
static char*        ConvertFontToString       (Font);
static void         ReportError               (char*,int);
#ifdef __cplusplus
}
#endif

typedef struct _OPixmapElementRecord
{
  char*    name;
  Display* display;
  Pixmap   pixmap;
} OPixmapElementRecord;

typedef struct _OKeySymElementRecord
{
  char*      name;
  KeySym     keySym;
} OKeySymElementRecord;

typedef struct _OCursorElementRecord
{
  char*      name;
  Cursor     cursor;
} OCursorElementRecord;

typedef struct _OFontStructElementRecord
{
  char*        name;
  XFontStruct* fontStruct;
} OFontStructElementRecord;

typedef struct _OFontElementRecord
{
  char*  name;
  Font   font;
} OFontElementRecord;

typedef unsigned char        Uchar;
typedef unsigned long        Ulong;

#define NotFound         (-1)
#define StringNotNull(s) ((s)==NULL?"":(s))

static struct 
{
  int                 pixmapn;
  OPixmapElementRecord*     pixmaps;
  int                 keySymn;
  OKeySymElementRecord*     keySyms;
  int                 cursorn;
  OCursorElementRecord*     cursors;
  int                 fontStructn;
  OFontStructElementRecord* fontStructs;
  int                 fontn;
  OFontElementRecord*       fonts;
} Class = {0,NULL,0,NULL,0,NULL,0,NULL,0,NULL};
/***************************************************************************/
void XtwClearClass (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  XDisplayClearClass ();
  for(count=0;count<Class.pixmapn;count++)
    {
      CStringDelete (Class.pixmaps[count].name);
      XFreePixmap   (Class.pixmaps[count].display,Class.pixmaps[count].pixmap);
    }
  CMemoryFreeBlock      (Class.pixmaps);
  Class.pixmaps = NULL;
  Class.pixmapn = 0;

  for(count=0;count<Class.keySymn;count++)
    {
      CStringDelete    (Class.keySyms[count].name);
      /*Free KeySym ?????*/
    }
  CMemoryFreeBlock      (Class.keySyms);
  Class.keySyms = NULL;
  Class.keySymn = 0;

  for(count=0;count<Class.cursorn;count++)
    {
      CStringDelete    (Class.cursors[count].name);
      /*Do not free .cursor that come from an Xt converter.*/
    }
  CMemoryFreeBlock      (Class.cursors);
  Class.cursors = NULL;
  Class.cursorn = 0;

  for(count=0;count<Class.fontStructn;count++)
    {
      CStringDelete  (Class.fontStructs[count].name);
      /*Do not free .fontStruct that come from an Xt converter.*/
    }
  CMemoryFreeBlock          (Class.fontStructs);
  Class.fontStructs = NULL;
  Class.fontStructn = 0;

  for(count=0;count<Class.fontn;count++)
    {
      CStringDelete  (Class.fonts[count].name);
      /*Do not free .font that come from the an Xt converter.*/
    }
  CMemoryFreeBlock    (Class.fonts);
  Class.fonts = NULL;
  Class.fontn = 0;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
char* XtwConvertResourceValueToString (
 Widget    a_widget 
,char*     a_name 
,char*     a_type 
,XtArgVal* a_value  
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* string;
/*.........................................................................*/
  if(strcmp(a_type,XtRString)==0)
    { 
      return CStringDuplicate(StringNotNull(*((String*)a_value)));
    }

  if(strcmp(a_type,XtRPixel)==0)
    { 
      string = XDisplayConvertPixelToColorName (XtDisplay(a_widget),*((Pixel*)a_value));
      return CStringDuplicate(string!=NULL ? string : "");
    }

  if( (strcmp(a_name,"list")==0) && 
      (strcmp(a_type,XtRPointer)==0) 
     ) /*Xaw:List*/
    {
      int               linen;
      Arg               args[1];
      XtSetArg          (args[0],"numberStrings",&linen);
      XtGetValues       (a_widget,args,1);
      return            CTextConvertToString (linen,*((char***)a_value),"\n");
    }

  if(strcmp(a_type,"KeySym")==0)
    { 
      char*        string;
      string       = CStringDuplicate (ConvertKeySymbolToString(*((KeySym*)a_value)));
      return       (string!=NULL ? string : CStringDuplicate(""));
    }

  if(OResourceIsCursor(a_type,NULL)==1)
    { 
      char*        string;
      string       = CStringDuplicate (ConvertCursorToString(*((Cursor*)a_value)));
      return       (string!=NULL ? string : CStringDuplicate(""));
    }

  if(OResourceIsPixmap(a_type,NULL)==1)
    { 
      char*        string;
      string       = CStringDuplicate (ConvertPixmapToString(*((Pixmap*)a_value)));
      return       (string!=NULL ? string : CStringDuplicate(""));
    }


  if(OResourceIsFontStruct(a_type,NULL)==1)
    { 
      char*        string;
      string       = CStringDuplicate (ConvertFontStructToString(*((XFontStruct**)a_value)));
      return       (string!=NULL ? string : CStringDuplicate(""));
    }


  if(OResourceIsFont(a_type,NULL)==1)
    { 
      char*        string;
      string       = CStringDuplicate (ConvertFontToString(*((Font*)a_value)));
      return       (string!=NULL ? string : CStringDuplicate(""));
    }

  if(OResourceIsReferenceList(a_type,NULL)==1)
    {
      if(strcmp(a_type,XtRWidgetList)==0)
        {
          string = WidgetListConvertToString (*((WidgetList*)a_value));
          return (string!=NULL ? string : CStringDuplicate(""));
        }
      else
        {
          return ( XWidgetIsValid(*((Widget*)a_value))==True ? 
		   CStringDuplicate(XtName(*((Widget*)a_value))) : CStringDuplicate(""));
        }
    }
  a_name = NULL;
  return NULL;
}
/***************************************************************************/
int XtwConvertStringToResourceChange (
 Widget a_widget 
,char* a_name  
,char* a_type  
,char* a_value 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(a_widget==NULL) return 0;
  if(a_name==NULL)   return 0;
  if(a_type==NULL)   return 0;
  if(a_value==NULL)  return 0;
  if(XResourceChangesSetWidget(a_widget)==0) return 0;

  if(strcmp(a_name,"geometry")==0)
    {
/*a local copy of string is not done in the widget, do it here*/
      XResourceChangesAddEntry  (a_name,(XtArgVal)XtNewString(a_value),NULL);  
      XResourceChangesFlush     ();
/*unrealize/realize Shell widget to take into account change of geometry resource*/
      XtUnrealizeWidget (a_widget);
      XtRealizeWidget   (a_widget);
      XtMapWidget       (a_widget);
      return            1;
    }

  if( (strcmp(a_name,"list")==0) && 
      (strcmp(a_type,XtRPointer)==0)
     ) /*Xaw:List*/
    {
/*a local copy of the list is not done in the widget, do it here*/
      char**            lines;
      int               linen;
      lines             = GetWords (a_value,&linen);
      XResourceChangesAddEntry ("numberStrings",(XtArgVal)linen,NULL);
      XResourceChangesAddEntry (a_name         ,(XtArgVal)lines,NULL);  
      XResourceChangesFlush    ();
      return                   1;
    }

  if(strcmp(a_type,XtRString)==0) /* if we pass by XtConvert, pb with string at same address but with different content */
    {
/*
    printf                   ("debug:Xtw:%s.%s(%s):%s\n",XtName(a_widget),a_name,a_type,a_value);
*/
/*here we assume that a local copy of string is done (not the case for XtNgeometry of Shell).*/
      XResourceChangesAddEntry (a_name,(XtArgVal)a_value,NULL);
      XResourceChangesFlush    ();
      return                   1;
    }

  if(strcmp(a_type,"KeySym")==0)
    { 
      KeySym       keySym;
      keySym       = ConvertStringToKeySymbol (a_value);
      if(keySym!=0L)
        {
          XResourceChangesAddEntry (a_name,(XtArgVal)keySym,NULL);
          XResourceChangesFlush    ();
        }
      return 1;
    }

  if(OResourceIsCursor(a_type,NULL)==1)
    { 
      Cursor       cursor;
      cursor       = ConvertStringToCursor (a_value,a_widget);
      if(cursor!=0L)
        {
          XResourceChangesAddEntry  (a_name,(XtArgVal)cursor,NULL);
          XResourceChangesFlush  ();
        }
      return 1;
    }

  if(OResourceIsPixmap(a_type,NULL)==1)
    { 
      Pixmap       pixmap;
      pixmap       = ConvertStringToPixmap (a_value,a_widget);
      if(pixmap!=0L)
        {
          XResourceChangesAddEntry  (a_name,(XtArgVal)pixmap,NULL);
          XResourceChangesFlush  ();
        }
      return 1;
    }

  if(OResourceIsFontStruct(a_type,NULL)==1)
    { 
      XFontStruct*     fontStruct;
      fontStruct       = ConvertStringToFontStruct (a_value,a_widget);
      if(fontStruct!=NULL)
        {
          XResourceChangesAddEntry  (a_name,(XtArgVal)fontStruct,NULL);
          XResourceChangesFlush  ();
        }
      return 1;
    }

  if(OResourceIsFont(a_type,NULL)==1)
    { 
      Font     font;
      font       = ConvertStringToFont (a_value,a_widget);
      if(font!=0L)
        {
          XResourceChangesAddEntry  (a_name,(XtArgVal)font,NULL);
          XResourceChangesFlush  ();
        }
      return 1;
    }

  if(OResourceIsReferenceList(a_type,NULL)==1)
    {
      if(strcmp(a_type,XtRWidgetList)==0)
        {
          WidgetList    list;
          list          = WidgetListCreateFromString (a_value);
          XResourceChangesAddEntry (a_name,(XtArgVal)list,(XResourceChangeFunction)CMemoryFreeBlock);
          XResourceChangesFlush ();
          return        1;
        }
      else
        {
          XResourceChangesAddEntry (a_name,(XtArgVal)XWidgetGetIdentifier(a_value),NULL);
          XResourceChangesFlush ();
          return        1;
        }
    }

  return 0;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static char** GetWords (
 char* a_string
,int* a_number
)
/***************************************************************************/
/*
  string is a list of widget separated by "\n" or "\\n"
  from CTextCreateFromString
*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char** list = NULL;
  int    nitem;
  char*  string;
  int    length,pos,count;
/*.........................................................................*/
  if(a_number!=NULL) *a_number      = 0;
  if(a_string==NULL)  return NULL;
  length         = strlen(a_string);
  if(length<=0)  return NULL;

  string         = CStringDuplicate(a_string);
  if(string==NULL)    return NULL;

  pos            = 0;
  nitem          = 0;
  list           = NULL;
  length        ++;
  for(count=0;count<length;count++) 
    {
      if( (string[count]=='\\') || (string[count]=='\n') || (string[count]==0) ) 
        { char    shift_one;
          char*   item;
          shift_one      = string[count]=='\n' ? 1 : 0;
          string [count] = 0;
          item           = (char*)XtNewString(string+pos);
          pos            = shift_one==1 ? count+1:count+2;
          if(item!=NULL)
            {
               list = nitem!=0 ? (char**)realloc((char *)list,(nitem+1)*sizeof(char*)) :
                                 (char**)malloc(sizeof(char*));
               if(list==NULL)   return NULL;
               list[nitem] = item;
               nitem++;
            }
        }
    }
  CMemoryFreeBlock  (string);
  if(a_number!=NULL) *a_number = nitem;
  return    list;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static Pixmap ConvertStringToPixmap (
 char* a_fname
,Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Pixmap  pixmap = 0L;
/*.........................................................................*/
  if(a_fname==NULL)  return 0L;
  if(a_widget==NULL) return 0L;

 { int count;  /*Look if already loaded.*/
  for(count=0;count<Class.pixmapn;count++)
    {
      if(strcmp(a_fname,Class.pixmaps[count].name)==0) 
	return Class.pixmaps[count].pixmap;
    }}

  if(!XtIsWidget(a_widget)) return 0L;

/* xbm ??? */
  pixmap = XWidgetLoadPixmapFile (a_widget,a_fname);
  if(pixmap==0L) /*Xmu routine unable to find file. Try to get the full path name, with CFile routines, and retry.*/
    { 
      char*    fname = NULL;
      FILE*    file;
      file     = CFileOpenForReading (a_fname,&fname);
      if(file!=NULL) fclose (file);     
      if(fname!=NULL)
        {
	  pixmap = XWidgetLoadPixmapFile (a_widget,fname);
        }
      else
	{
	  pixmap = 0L;  
	}
      CStringDelete (fname);
    }

  if(pixmap==0L)
    { 
      FILE*     file;
      unsigned int width,height;
      int       ncolor = 0;
      Uchar*    data   = NULL;     
      char**    colors = NULL;
      double*   red   = NULL;
      double*   green = NULL;
      double*   blue  = NULL;
      GC        gc;
      Ulong*    pixels = NULL;
      Display*  display;
       int count;
      
      display  = XtDisplay (a_widget);

      file     = CFileOpenForReading (a_fname,NULL);
/* xpm ??? */
      data     = CXPM_GetImageFromStream (file,&width,&height,&ncolor,&colors);
      if(data!=NULL)
        {
          if(file!=NULL) fclose     (file);     
          if( (ncolor!=0) && (colors!=NULL) ) 
	    {
	      pixels      = (Ulong*) CMemoryAllocateBlock(ncolor * sizeof(Ulong));
	      if(pixels!=NULL)
		{
		  for(count=0;count<ncolor;count++)
		    pixels[count] = XWidgetConvertStringToPixel (a_widget,colors[count]);
		}
	    }
          CTextDelete(ncolor,colors);
        }
/* gif ??? */
      else
        {
          CTextDelete (ncolor,colors);
          data        = CGIF_GetImageFromStream (file,&width,&height,&ncolor,&red,&green,&blue);
          if(file!=NULL)   fclose     (file);     
	  if(ncolor!=0)
            {
	      pixels = (Ulong*)CMemoryAllocateBlock(ncolor*sizeof(Ulong));
	      if(pixels!=NULL)
		{
		  for(count=0;count<ncolor;count++)
		    { int    alloc;
		      XDisplayGetPixelFromRGB (display,red[count],green[count],blue[count],&(pixels[count]),&alloc);
		    }
		}
	    }
          CMemoryFreeBlock (red);
          CMemoryFreeBlock (green);
          CMemoryFreeBlock (blue);
        }

      if(data!=NULL) 
	{
	  gc         = XCreateGC (display, XDefaultRootWindow(display),0L,NULL);
	  if(gc!=NULL)
            { 
	      XImage*                image;
	      pixmap                 = XCreatePixmap (display,XDefaultRootWindow(display),
						      width,height,XDisplayGetDepth(display));

	      image                  = XCreateImage  (display,
                                        DefaultVisualOfScreen (DefaultScreenOfDisplay(display)),
                                        XDisplayGetDepth(display),
                                        ZPixmap,0,
                                        NULL,
                                        width,height,8,0); /*create an XImage adapted to screen*/
	      if(image!=NULL)
                { 
		  image->data            = (char*) CMemoryAllocateBlock (image->height * image->bytes_per_line * sizeof(char));
                  image->f.destroy_image = XImageDelete;
		  image->obdata          = CStringDuplicate("icon");
		  if(image->data!=NULL)
                    {  int row,col;
		      char*  itema;
		      itema  = (char*)data;
		      for (row=0;row<image->height;row++) 
			{ 
			  for (col=0;col<image->width;col++) 
			    {
			      XPutPixel (image,col,row,
					 ( (pixels!=NULL) && (*itema<ncolor))? pixels[*itema] : *itema);
			      itema++;
			    }
			}
		      XPutImage (display,pixmap,gc,image,0,0,0,0,width,height);
		    }
		  XDestroyImage (image);
		}

	    }
	  XFreeGC    (display,gc);
	}
      CMemoryFreeBlock   (pixels);
      CMemoryFreeBlock   (data);
    }

  if(pixmap!=0L)
    {
      Class.pixmaps = Class.pixmapn==0 ? 
                        (OPixmapElementRecord*)CMemoryAllocateBlock(sizeof(OPixmapElementRecord)) :
                        (OPixmapElementRecord*)CMemoryChangeBlockSize((char *)(Class.pixmaps),(Class.pixmapn+1)*sizeof(OPixmapElementRecord));
      if(Class.pixmaps==NULL) 
	{
	  Class.pixmapn = 0;
	  return 0L;
	}
      Class.pixmaps[Class.pixmapn].name       = CStringDuplicate(a_fname);
      Class.pixmaps[Class.pixmapn].display    = XtDisplay(a_widget);
      Class.pixmaps[Class.pixmapn].pixmap     = pixmap;
      Class.pixmapn++;
    }

  return pixmap;
}
/***************************************************************************/
static char* ConvertPixmapToString (
 Pixmap a_pixmap
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_pixmap==0L) return NULL;

  for(count=0;count<Class.pixmapn;count++)
    {
      if(a_pixmap == Class.pixmaps[count].pixmap) 
	return Class.pixmaps[count].name;
    }

  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static KeySym ConvertStringToKeySymbol (
 char* a_string
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  KeySym keySym;
/*.........................................................................*/
  if(a_string==NULL) return NoSymbol;

 { int count;  /*Look if already loaded.*/
  for(count=0;count<Class.keySymn;count++)
    {
      if(strcmp(a_string,Class.keySyms[count].name)==0) 
	return Class.keySyms[count].keySym;
    }}

  keySym = XStringToKeysym(a_string);

  if(keySym!=NoSymbol)
    {
      Class.keySyms = Class.keySymn==0 ? 
                        (OKeySymElementRecord*)CMemoryAllocateBlock(sizeof(OKeySymElementRecord)) :
                        (OKeySymElementRecord*)CMemoryChangeBlockSize((char *)(Class.keySyms),(Class.keySymn+1)*sizeof(OKeySymElementRecord));
      if(Class.keySyms==NULL) 
	{
	  Class.keySymn = 0;
	  return NoSymbol;
	}
      Class.keySyms[Class.keySymn].name       = CStringDuplicate(a_string);
      Class.keySyms[Class.keySymn].keySym     = keySym;
      Class.keySymn++;
    }

  return keySym;
}
/***************************************************************************/
static char* ConvertKeySymbolToString (
 KeySym a_keySym
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_keySym==0L) return NULL;

  for(count=0;count<Class.keySymn;count++)
    {
      if(a_keySym == Class.keySyms[count].keySym) 
	return Class.keySyms[count].name;
    }

  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static Cursor ConvertStringToCursor (
 char* a_string
,Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Cursor cursor;
/*.........................................................................*/
  if(a_string==NULL) return 0L;
  if(a_widget==NULL) return 0L;

 { int count;  /*Look if already loaded.*/
  for(count=0;count<Class.cursorn;count++)
    {
      if(strcmp(a_string,Class.cursors[count].name)==0) 
	return Class.cursors[count].cursor;
    }}

  cursor = XWidgetConvertStringToCursor (a_widget,a_string); /*From an Xt converter.*/

  if(cursor!=0L)
    {
      Class.cursors = Class.cursorn==0 ? 
                        (OCursorElementRecord*)CMemoryAllocateBlock(sizeof(OCursorElementRecord)) :
                        (OCursorElementRecord*)CMemoryChangeBlockSize((char *)(Class.cursors),(Class.cursorn+1)*sizeof(OCursorElementRecord));
      if(Class.cursors==NULL) 
	{
	  Class.cursorn = 0;
	  return 0L;
	}
      Class.cursors[Class.cursorn].name   = CStringDuplicate(a_string);
      Class.cursors[Class.cursorn].cursor = cursor;
      Class.cursorn++;
    }

  return cursor;
}
/***************************************************************************/
static char* ConvertCursorToString (
 Cursor a_cursor
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_cursor==0L) return NULL;

  for(count=0;count<Class.cursorn;count++)
    {
      if(a_cursor == Class.cursors[count].cursor) 
	return Class.cursors[count].name;
    }

  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static XFontStruct* ConvertStringToFontStruct (
 char* a_string
,Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  XFontStruct*   fontStruct;
/*.........................................................................*/
  if(a_string==NULL)  return NULL;
  if(a_widget==NULL)  return NULL;

 { int count;  /*Look if already loaded.*/
  for(count=0;count<Class.fontStructn;count++)
    {
      if(strcmp(a_string,Class.fontStructs[count].name)==0) 
	return Class.fontStructs[count].fontStruct;
    }}

/*fontStruct      = XLoadQueryFont (a_display,a_string);  */
  fontStruct      = XWidgetConvertStringToFontStruct (a_widget,a_string);  /*From an Xt converter.*/
  if(fontStruct==NULL) return NULL;

  if(fontStruct!=NULL)
    {
      Class.fontStructs = Class.fontStructn==0 ? 
	        (OFontStructElementRecord*)CMemoryAllocateBlock(sizeof(OFontStructElementRecord)) :
                (OFontStructElementRecord*)CMemoryChangeBlockSize((char *)(Class.fontStructs),(Class.fontStructn+1)*sizeof(OFontStructElementRecord));
      if(Class.fontStructs==NULL) 
	{
	  Class.fontStructn = 0;
	  return NULL;
	}
      Class.fontStructs[Class.fontStructn].name       = CStringDuplicate(a_string);
      Class.fontStructs[Class.fontStructn].fontStruct = fontStruct;
      Class.fontStructn++;
    }

  return fontStruct;
}
/***************************************************************************/
static char* ConvertFontStructToString (
 XFontStruct* a_fontStruct
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_fontStruct==NULL) return NULL;

  for(count=0;count<Class.fontStructn;count++)
    {
      if(a_fontStruct == Class.fontStructs[count].fontStruct) 
	return Class.fontStructs[count].name;
    }

  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static Font ConvertStringToFont (
 char* a_string
,Widget a_widget
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  Font   font;
/*.........................................................................*/
  if(a_string==NULL)  return 0L;
  if(a_widget==NULL)  return 0L;

 { int count;  /*Look if already loaded.*/
  for(count=0;count<Class.fontn;count++)
    {
      if(strcmp(a_string,Class.fonts[count].name)==0) 
	return Class.fonts[count].font;
    }}

  font     = XWidgetConvertStringToFont (a_widget,a_string);  /*From an Xt converter.*/
  if(font==0L) return 0L;

  if(font!=0L)
    {
      Class.fonts = Class.fontn==0 ? 
	        (OFontElementRecord*)CMemoryAllocateBlock(sizeof(OFontElementRecord)) :
                (OFontElementRecord*)CMemoryChangeBlockSize((char *)(Class.fonts),(Class.fontn+1)*sizeof(OFontElementRecord));
      if(Class.fonts==NULL) 
	{
	  Class.fontn = 0;
	  return 0L;
	}
      Class.fonts[Class.fontn].name = CStringDuplicate(a_string);
      Class.fonts[Class.fontn].font = font;
      Class.fontn++;
    }

  return font;
}
/***************************************************************************/
static char* ConvertFontToString (
 Font a_font
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
   int count;
/*.........................................................................*/
  if(a_font==0L) return NULL;

  for(count=0;count<Class.fontn;count++)
    {
      if(a_font == Class.fonts[count].font) 
	return Class.fonts[count].name;
    }

  return NULL;
}
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Wo","XtwTools",a_name,a_code);
}
