/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#যদি !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# যদি defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# অন্যথা
#  include <gtk/gtkwidget.h>
# endif
#অন্যথা
# যদি defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

প্রকারনির্দেশ গণনা
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

প্রকারনির্দেশ সংরচনা BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    unsigned পূর্ণাঙ্ক	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    পূর্ণাঙ্ক			x;
    পূর্ণাঙ্ক			y;
    unsigned পূর্ণাঙ্ক	state;		// Button/Modifier key state
# অন্যথা
#  যদি !defined(FEAT_GUI_MSWIN)
    Widget		target;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_মূল;
    Position		y_মূল;
    পূর্ণাঙ্ক			state;		// Button/Modifier key state
#  অন্যথা
    HWND		target;
    HWND		balloon;
    পূর্ণাঙ্ক			x;
    পূর্ণাঙ্ক			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# যদি !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    শূন্য		(*বাৰ্তা_CB)(সংরচনা BalloonEvalStruct *, পূর্ণাঙ্ক);
    শূন্য		*clientData;	// For callback
#endif

    পূর্ণাঙ্ক			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    পূর্ণাঙ্ক			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*বাৰ্তা;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    শূন্য		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    পূর্ণাঙ্ক			x;
    পূর্ণাঙ্ক			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
