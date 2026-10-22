/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#எனில் !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# எனில் defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# இல்லையெனில்
#  include <gtk/gtkwidget.h>
# endif
#இல்லையெனில்
# எனில் defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

வகைவரை எண்ணிக்கை
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

வகைவரை கட்டமைப்பு BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    unsigned முழு	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    முழு			x;
    முழு			y;
    unsigned முழு	state;		// Button/Modifier key state
# இல்லையெனில்
#  எனில் !defined(FEAT_GUI_MSWIN)
    Widget		target;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_வேர்;
    Position		y_வேர்;
    முழு			state;		// Button/Modifier key state
#  இல்லையெனில்
    HWND		target;
    HWND		balloon;
    முழு			x;
    முழு			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# எனில் !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    வெறுமை		(*செய்தி_CB)(கட்டமைப்பு BalloonEvalStruct *, முழு);
    வெறுமை		*clientData;	// For callback
#endif

    முழு			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    முழு			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*செய்தி;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    வெறுமை		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    முழு			x;
    முழு			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
