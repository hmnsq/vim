/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#𐤀𐤌 !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# 𐤀𐤌 defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# وإلا
#  include <gtk/gtkwidget.h>
# endif
#وإلا
# 𐤀𐤌 defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

تعريفنوع تعداد
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

تعريفنوع بنية BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    unsigned صحيح	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    صحيح			x;
    صحيح			y;
    unsigned صحيح	state;		// Button/Modifier key state
# وإلا
#  𐤀𐤌 !defined(FEAT_GUI_MSWIN)
    Widget		target;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_root;
    Position		y_root;
    صحيح			state;		// Button/Modifier key state
#  وإلا
    HWND		target;
    HWND		balloon;
    صحيح			x;
    صحيح			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# 𐤀𐤌 !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    فراغ		(*msgCB)(بنية BalloonEvalStruct *, صحيح);
    فراغ		*clientData;	// For callback
#endif

    صحيح			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    صحيح			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*msg;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    فراغ		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    صحيح			x;
    صحيح			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
