/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#إذا !defined(BEVAL__H) && defined(FEAT_BEVAL)
#تعریف_کرو BEVAL__H

#جے_تعریف_شدہ FEAT_GUI_GTK
# إذا defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# وإلا
#  include <gtk/gtkwidget.h>
# endif
#وإلا
# إذا defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#جے_انت

تعريفنوع تعداد
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

تعريفنوع بنية BalloonEvalStruct
{
#جے_تعریف_شدہ FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*نشانہ;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    بنانشانی صحيح	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    صحيح			x;
    صحيح			y;
    بنانشانی صحيح	state;		// Button/Modifier key state
# وإلا
#  إذا !defined(FEAT_GUI_MSWIN)
    Widget		نشانہ;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_جڑ;
    Position		y_جڑ;
    صحيح			state;		// Button/Modifier key state
#  وإلا
    HWND		نشانہ;
    HWND		balloon;
    صحيح			x;
    صحيح			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# إذا !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_چوڑائی;	// screen width in pixels
    Dimension		screen_اچائی;	// screen height in pixels
# endif
    فراغ		(*پیغام_CB)(بنية BalloonEvalStruct *, صحيح);
    فراغ		*client_انکڑے;	// For callback
#جے_انت

    صحيح			ts;		// tabstop setting for this buffer
#جے_تعریف_شدہ FEAT_VARTABS
    صحيح			*vts;		// vartabstop setting for this buffer
#جے_انت
    char_u		*پیغام;		// allocated: current text
#جے_تعریف_شدہ FEAT_GUI_MSWIN
    فراغ		*tofree;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_HAIKU
    صحيح			x;
    صحيح			y;
#جے_انت
} BalloonEval;

#تعریف_کرو EVAL_وتھ_X 15 // displacement of beval topleft corner from pointer
#تعریف_کرو EVAL_وتھ_Y 10

#جے_تعریف_شدہ FEAT_BEVAL_GUI
# include "gui_beval.pro"
#جے_انت

#جے_انت // BEVAL__H and FEAT_BEVAL_GUI
