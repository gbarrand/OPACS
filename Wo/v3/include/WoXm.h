/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#ifndef WoXm_h
#define WoXm_h

#ifdef HAS_XM

/*Co*/
#include <CPrinter.h> 
#include <CBatch.h> 

/*Xx*/
#include <XWidget.h>
#include <XxCommands.h>

/*Wo*/
#include <OInterface.h>
#include <OResource.h>
#include <OClass.h>
#include <OWidget.h>
#include <Wo.h>

/*Motif*/
#include <XmTools.h> 
#include <XmCommands.h>

/*extern WidgetClass xmProtocolObjectClass; Not on all systems.*/

extern WidgetClass xmArrowButtonWidgetClass;
extern WidgetClass xmArrowButtonGadgetClass;
extern WidgetClass xmBulletinBoardWidgetClass;
extern WidgetClass xmCascadeButtonWidgetClass;
extern WidgetClass xmCascadeButtonGadgetClass;
extern WidgetClass xmCommandWidgetClass;
extern WidgetClass xmDialogShellWidgetClass;
extern WidgetClass xmDrawingAreaWidgetClass;
extern WidgetClass xmDrawnButtonWidgetClass;
extern WidgetClass xmFileSelectionBoxWidgetClass;
extern WidgetClass xmFormWidgetClass;
extern WidgetClass xmFrameWidgetClass;
extern WidgetClass xmLabelWidgetClass;
extern WidgetClass xmLabelGadgetClass;
extern WidgetClass xmListWidgetClass;
extern WidgetClass xmMainWindowWidgetClass;
extern WidgetClass xmMenuShellWidgetClass;
extern WidgetClass xmMessageBoxWidgetClass;
extern WidgetClass xmPanedWindowWidgetClass;
extern WidgetClass xmPushButtonWidgetClass;
extern WidgetClass xmPushButtonGadgetClass;
extern WidgetClass xmRowColumnWidgetClass;
extern WidgetClass xmSashWidgetClass;
extern WidgetClass xmScaleWidgetClass;
extern WidgetClass xmScrollBarWidgetClass;
extern WidgetClass xmScrolledWindowWidgetClass;
extern WidgetClass xmSelectionBoxWidgetClass;
extern WidgetClass xmSeparatorGadgetClass;
extern WidgetClass xmSeparatorWidgetClass;
extern WidgetClass xmTextWidgetClass;
extern WidgetClass xmTextFieldWidgetClass;
extern WidgetClass xmToggleButtonWidgetClass;
extern WidgetClass xmToggleButtonGadgetClass;

#ifdef __cplusplus
extern "C"{
#endif
Widget XmCreateBulletinBoardDialog (Widget,String,ArgList,Cardinal);
Widget XmCreateErrorDialog         (Widget,String,ArgList,Cardinal);
Widget XmCreateFileSelectionDialog (Widget,String,ArgList,Cardinal);
Widget XmCreateFormDialog          (Widget,String,ArgList,Cardinal);
Widget XmCreateInformationDialog   (Widget,String,ArgList,Cardinal);
Widget XmCreateMenuBar             (Widget,String,ArgList,Cardinal);
Widget XmCreateMessageDialog       (Widget,String,ArgList,Cardinal);
Widget XmCreateOptionMenu          (Widget,String,ArgList,Cardinal);
Widget XmCreatePopupMenu           (Widget,String,ArgList,Cardinal);
Widget XmCreatePromptDialog        (Widget,String,ArgList,Cardinal);
Widget XmCreatePulldownMenu        (Widget,String,ArgList,Cardinal);
Widget XmCreateQuestionDialog      (Widget,String,ArgList,Cardinal);
Widget XmCreateRadioBox            (Widget,String,ArgList,Cardinal);
Widget XmCreateScrolledList        (Widget,String,ArgList,Cardinal);
Widget XmCreateSelectionDialog     (Widget,String,ArgList,Cardinal);
Widget XmCreateWarningDialog       (Widget,String,ArgList,Cardinal);
Widget XmCreateWorkingDialog       (Widget,String,ArgList,Cardinal);
Widget XmCreateScrolledText        (Widget,String,ArgList,Cardinal);

#ifndef __cplusplus
static 
#endif
void ExitWo ();
#ifdef __cplusplus
}
#endif

/***************************************************************************/
#ifndef __cplusplus
static 
#endif
void ExitWo (
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(OInterfaceIsExtentModified()==1) 
    {
      if(XmDialogAskForBoolean("Save modified interface ?")==True) OInterfaceSaveExtent(); /**/
    }
}


#endif /*HAS_XM*/

#endif /*WoXm_h*/
