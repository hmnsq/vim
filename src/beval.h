/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#إذا !defined(BEVAL__H) && defined(FEAT_BEVAL)
#وصف_ڪريو BEVAL__H

#جيڪڏهن_وصف_ٿيل FEAT_GUI_GTK
# إذا defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# وإلا
#  include <gtk/gtkwidget.h>
# endif
#وإلا
# إذا defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#جيڪڏهن_پورو

تعريفنوع تعداد
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

تعريفنوع بنية BalloonEvalStruct
{
#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*هدف;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    بنانشان صحيح	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    صحيح			x;
    صحيح			y;
    بنانشان صحيح	state;		// Button/Modifier key state
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
    Position		x_پاڙ;
    Position		y_پاڙ;
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
    Dimension		screen_ويڪر;	// screen width in pixels
    Dimension		screen_اوچائي;	// screen height in pixels
# endif
    فراغ		(*نياپو_CB)(بنية BalloonEvalStruct *, صحيح);
    فراغ		*client_معلومات;	// For callback
#جيڪڏهن_پورو

    صحيح			ts;		// tabstop setting for this buffer
#جيڪڏهن_وصف_ٿيل FEAT_VARTABS
    صحيح			*vts;		// vartabstop setting for this buffer
#جيڪڏهن_پورو
    char_u		*نياپو;		// allocated: current text
#جيڪڏهن_وصف_ٿيل FEAT_GUI_MSWIN
    فراغ		*tofree;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI_HAIKU
    صحيح			x;
    صحيح			y;
#جيڪڏهن_پورو
} BalloonEval;

#وصف_ڪريو EVAL_وٿي_X 15 // displacement of beval topleft corner from pointer
#وصف_ڪريو EVAL_وٿي_Y 10

#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_GUI
# include "gui_beval.pro"
#جيڪڏهن_پورو

#جيڪڏهن_پورو // BEVAL__H and FEAT_BEVAL_GUI
