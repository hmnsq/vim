/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#إذا !defined(BEVAL__H) && defined(FEAT_BEVAL)
#تعریف_کړه BEVAL__H

#که_تعریف_شوی FEAT_GUI_GTK
# إذا defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# وإلا
#  include <gtk/gtkwidget.h>
# endif
#وإلا
# إذا defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#د_که_پای

تعريفنوع تعداد
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

تعريفنوع بنية BalloonEvalStruct
{
#که_تعریف_شوی FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*موخه;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    بینښه صحيح	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    صحيح			x;
    صحيح			y;
    بینښه صحيح	state;		// Button/Modifier key state
# وإلا
#  إذا !defined(FEAT_GUI_MSWIN)
    Widget		موخه;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_ریښه;
    Position		y_ریښه;
    صحيح			state;		// Button/Modifier key state
#  وإلا
    HWND		موخه;
    HWND		balloon;
    صحيح			x;
    صحيح			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# إذا !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_پلنوالی;	// screen width in pixels
    Dimension		screen_لوړوالی;	// screen height in pixels
# endif
    فراغ		(*پیغام_CB)(بنية BalloonEvalStruct *, صحيح);
    فراغ		*client_معلومات;	// For callback
#د_که_پای

    صحيح			ts;		// tabstop setting for this buffer
#که_تعریف_شوی FEAT_VARTABS
    صحيح			*vts;		// vartabstop setting for this buffer
#د_که_پای
    char_u		*پیغام;		// allocated: current text
#که_تعریف_شوی FEAT_GUI_MSWIN
    فراغ		*tofree;
#د_که_پای
#که_تعریف_شوی FEAT_GUI_HAIKU
    صحيح			x;
    صحيح			y;
#د_که_پای
} BalloonEval;

#تعریف_کړه EVAL_دننه_واټن_X 15 // displacement of beval topleft corner from pointer
#تعریف_کړه EVAL_دننه_واټن_Y 10

#که_تعریف_شوی FEAT_BEVAL_GUI
# include "gui_beval.pro"
#د_که_پای

#د_که_پای // BEVAL__H and FEAT_BEVAL_GUI
