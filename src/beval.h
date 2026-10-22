/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ਜੇ !defined(BEVAL__H) && defined(FEAT_BEVAL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEVAL__H

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
# ਜੇ defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# ਨਹੀਂਤਾਂ
#  include <gtk/gtkwidget.h>
# endif
#ਨਹੀਂਤਾਂ
# ਜੇ defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ BalloonEvalStruct
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*ਨਿਸ਼ਾਨਾ;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    ਪੂਰਨ			x;
    ਪੂਰਨ			y;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	state;		// Button/Modifier key state
# ਨਹੀਂਤਾਂ
#  ਜੇ !defined(FEAT_GUI_MSWIN)
    Widget		ਨਿਸ਼ਾਨਾ;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_ਜੜ੍ਹ;
    Position		y_ਜੜ੍ਹ;
    ਪੂਰਨ			state;		// Button/Modifier key state
#  ਨਹੀਂਤਾਂ
    HWND		ਨਿਸ਼ਾਨਾ;
    HWND		balloon;
    ਪੂਰਨ			x;
    ਪੂਰਨ			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# ਜੇ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_ਚੌੜਾਈ;	// screen width in pixels
    Dimension		screen_ਉਚਾਈ;	// screen height in pixels
# endif
    ਖਾਲੀ		(*ਸੁਨੇਹਾ_CB)(ਬਣਤਰ BalloonEvalStruct *, ਪੂਰਨ);
    ਖਾਲੀ		*client_ਅੰਕੜੇ;	// For callback
#ਜੇ_ਅੰਤ

    ਪੂਰਨ			ts;		// tabstop setting for this buffer
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਪੂਰਨ			*vts;		// vartabstop setting for this buffer
#ਜੇ_ਅੰਤ
    char_u		*ਸੁਨੇਹਾ;		// allocated: current text
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
    ਖਾਲੀ		*tofree;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_HAIKU
    ਪੂਰਨ			x;
    ਪੂਰਨ			y;
#ਜੇ_ਅੰਤ
} BalloonEval;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVAL_ਵਿੱਥ_X 15 // displacement of beval topleft corner from pointer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVAL_ਵਿੱਥ_Y 10

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
# include "gui_beval.pro"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ // BEVAL__H and FEAT_BEVAL_GUI
