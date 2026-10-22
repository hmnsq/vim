/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر !defined(BEVAL__H) && defined(FEAT_BEVAL)
#تعریف_کرو BEVAL__H

#اگر_تعریف_شدہ FEAT_GUI_GTK
# اگر defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# ورنہ
#  include <gtk/gtkwidget.h>
# endif
#ورنہ
# اگر defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#اگر_ختم

قسمتعریف شمار
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

قسمتعریف ساخت BalloonEvalStruct
{
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*ہدف;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    غیرعلامتی صحیح	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    صحیح			x;
    صحیح			y;
    غیرعلامتی صحیح	state;		// Button/Modifier key state
# ورنہ
#  اگر !defined(FEAT_GUI_MSWIN)
    Widget		ہدف;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_جڑ;
    Position		y_جڑ;
    صحیح			state;		// Button/Modifier key state
#  ورنہ
    HWND		ہدف;
    HWND		balloon;
    صحیح			x;
    صحیح			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# اگر !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_چوڑائی;	// screen width in pixels
    Dimension		screen_اونچائی;	// screen height in pixels
# endif
    خالی		(*پیغام_CB)(ساخت BalloonEvalStruct *, صحیح);
    خالی		*client_ڈیٹا;	// For callback
#اگر_ختم

    صحیح			ts;		// tabstop setting for this buffer
#اگر_تعریف_شدہ FEAT_VARTABS
    صحیح			*vts;		// vartabstop setting for this buffer
#اگر_ختم
    char_u		*پیغام;		// allocated: current text
#اگر_تعریف_شدہ FEAT_GUI_MSWIN
    خالی		*tofree;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_HAIKU
    صحیح			x;
    صحیح			y;
#اگر_ختم
} BalloonEval;

#تعریف_کرو EVAL_انحراف_X 15 // displacement of beval topleft corner from pointer
#تعریف_کرو EVAL_انحراف_Y 10

#اگر_تعریف_شدہ FEAT_BEVAL_GUI
# include "gui_beval.pro"
#اگر_ختم

#اگر_ختم // BEVAL__H and FEAT_BEVAL_GUI
