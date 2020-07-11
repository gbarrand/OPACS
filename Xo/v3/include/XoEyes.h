/*
* $XConsortium: Eyes.h,v 1.7 90/12/01 13:04:51 rws Exp $
*/

#ifndef XoEyes_h
#define XoEyes_h

#ifndef NO_X_H
#include <X11/Intrinsic.h>
#endif

/***********************************************************************
 *
 * Eyes Widget
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		pixel		White
 border		     BorderColor	pixel		Black
 borderWidth	     BorderWidth	int		1
 foreground	     Foreground		Pixel		Black
 outline	     Outline		Pixel		Black
 height		     Height		int		120
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 reverseVideo	     ReverseVideo	Boolean		False
 width		     Width		int		120
 x		     Position		int		0
 y		     Position		int		0

*/

#define XtNoutline	"outline"
#define XtNcenterColor	"center"

#define XtNshapedWindow	"shapedWindow"
#define XtCShapedWindow	"ShapedWindow"

typedef struct _EyesRec *EyesWidget;  /* completely defined in EyesPrivate.h */
typedef struct _EyesClassRec *EyesWidgetClass;    /* completely defined in EyesPrivate.h */


#ifdef __cplusplus
extern "C"{
#endif
extern WidgetClass xoEyesWidgetClass;
#ifdef __cplusplus
}
#endif

#ifdef __CI_BIND__
CiBindXoEyes(0); /**/
#endif

#endif /*XoEyes_h*/
