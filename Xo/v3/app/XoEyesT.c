/* $XConsortium: xeyes.c,v 1.15 91/07/18 16:46:26 rws Exp $ */
/*
 * Copyright 1991 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
#include <stdio.h> 
#include <stdlib.h> 

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <XoEyes.h>

#include <eyes.h>
#include <eyesmask.h>

#ifdef __cplusplus
extern "C"{
#endif
static void usage ();
static void quit  (Widget,XEvent*,String*,Cardinal*);
#ifdef __cplusplus
}
#endif


/* Command line options table.  Only resources are entered here...there is a
   pass over the remaining options after XtParseCommand is let loose. */

static XrmOptionDescRec options[] = {
{"-outline",	"*eyes.outline",	XrmoptionSepArg,	NULL},
{"-center",	"*eyes.center",		XrmoptionSepArg,	NULL},
{"-backing",	"*eyes.backingStore",	XrmoptionSepArg,	NULL},
{"-shape",	"*eyes.shapedWindow",	XrmoptionNoArg,		"TRUE"},
{"+shape",	"*eyes.shapedWindow",	XrmoptionNoArg,		"FALSE"},
};

static XtActionsRec actions[] = {
    {"quit",	quit}
};

static Atom wm_delete_window;

int main(int argc,char** argv)
{
    XtAppContext app_context;
    Widget toplevel;
    Arg arg[2];
    Cardinal i;
    
    toplevel = XtAppInitialize(&app_context, "XoEyesT", 
			       options, XtNumber(options), &argc, argv,
			       NULL, arg, (Cardinal) 0);
    if (argc != 1) usage();
    XtAppAddActions(app_context, actions, XtNumber(actions));
    XtOverrideTranslations
	(toplevel, XtParseTranslationTable ("<Message>WM_PROTOCOLS: quit()"));
    
    i = 0;
    XtSetArg (arg[i], XtNiconPixmap, 
	      XCreateBitmapFromData (XtDisplay(toplevel),
				     XtScreen(toplevel)->root,
				     (char *)eyes_bits, eyes_width, eyes_height));
    i++;
    XtSetArg (arg[i], XtNiconMask, 
	      XCreateBitmapFromData (XtDisplay(toplevel),
				     XtScreen(toplevel)->root,
				     (char *)eyesmask_bits,
				     eyesmask_width, eyesmask_height));
    i++;
    XtSetValues (toplevel, arg, i);

    (void) XtCreateManagedWidget ("xoEyesT", xoEyesWidgetClass, toplevel, NULL, 0);
    XtRealizeWidget (toplevel);
    wm_delete_window = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW",
				   False);
    (void) XSetWMProtocols (XtDisplay(toplevel), XtWindow(toplevel),
                            &wm_delete_window, 1);
    XtAppMainLoop(app_context);
    return EXIT_SUCCESS;
}

/* Exit with message describing command line format */

static void usage()
{
    fprintf(stderr,
"usage: xeyes\n");
    fprintf (stderr, 
"       [-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]] [-display [{host}]:[{vs}]]\n");
    fprintf(stderr,
"       [-fg {color}] [-bg {color}] [-bd {color}] [-bw {pixels}]");
    fprintf(stderr, " [-shape | +shape]");
    fprintf(stderr, "\n");
    fprintf(stderr,
"       [-outline {color}] [-center {color}] [-backing {backing-store}]\n");
    exit(1);
}

/*ARGSUSED*/
static void quit(Widget w,XEvent* event,String* params,Cardinal* num_params)
{
    printf ("debug:quit\n");
    if( (event->type == ClientMessage) && 
	((unsigned long)event->xclient.data.l[0] != wm_delete_window)
	) 
      {
	XBell(XtDisplay(w), 0);
      } else {
	XtDestroyApplicationContext(XtWidgetToApplicationContext(w));
	exit(0);
      }
   params     = NULL;
   num_params = 0;
}
