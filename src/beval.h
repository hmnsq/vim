/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# اگر defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# وگرنه
#  include <gtk/gtkwidget.h>
# endif
#وگرنه
# اگر defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

تعریفنوع شمارش
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

تعریفنوع ساختار BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    unsigned صحیح	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    صحیح			x;
    صحیح			y;
    unsigned صحیح	state;		// Button/Modifier key state
# وگرنه
#  اگر !defined(FEAT_GUI_MSWIN)
    Widget		target;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_ریشه;
    Position		y_ریشه;
    صحیح			state;		// Button/Modifier key state
#  وگرنه
    HWND		target;
    HWND		balloon;
    صحیح			x;
    صحیح			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# اگر !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    تهی		(*پیام_CB)(ساختار BalloonEvalStruct *, صحیح);
    تهی		*clientData;	// For callback
#endif

    صحیح			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    صحیح			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*پیام;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    تهی		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    صحیح			x;
    صحیح			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
