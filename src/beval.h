/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#إذا !defined(BEVAL__H) && defined(FEAT_BEVAL)
#تعریف_کن BEVAL__H

#اگر_تعریف_شده FEAT_GUI_GTK
# إذا defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# وإلا
#  include <gtk/gtkwidget.h>
# endif
#وإلا
# إذا defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#پایان_اگر

تعريفنوع تعداد
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

تعريفنوع بنية BalloonEvalStruct
{
#اگر_تعریف_شده FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*هدف;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    بیعلامت صحيح	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    صحيح			x;
    صحيح			y;
    بیعلامت صحيح	state;		// Button/Modifier key state
# وإلا
#  إذا !defined(FEAT_GUI_MSWIN)
    Widget		هدف;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_ریشه;
    Position		y_ریشه;
    صحيح			state;		// Button/Modifier key state
#  وإلا
    HWND		هدف;
    HWND		balloon;
    صحيح			x;
    صحيح			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# إذا !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_عرض;	// screen width in pixels
    Dimension		screen_ارتفاع;	// screen height in pixels
# endif
    فراغ		(*پیام_CB)(بنية BalloonEvalStruct *, صحيح);
    فراغ		*client_داده;	// For callback
#پایان_اگر

    صحيح			ts;		// tabstop setting for this buffer
#اگر_تعریف_شده FEAT_VARTABS
    صحيح			*vts;		// vartabstop setting for this buffer
#پایان_اگر
    char_u		*پیام;		// allocated: current text
#اگر_تعریف_شده FEAT_GUI_MSWIN
    فراغ		*tofree;
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_HAIKU
    صحيح			x;
    صحيح			y;
#پایان_اگر
} BalloonEval;

#تعریف_کن EVAL_جابه_جایی_X 15 // displacement of beval topleft corner from pointer
#تعریف_کن EVAL_جابه_جایی_Y 10

#اگر_تعریف_شده FEAT_BEVAL_GUI
# include "gui_beval.pro"
#پایان_اگر

#پایان_اگر // BEVAL__H and FEAT_BEVAL_GUI
