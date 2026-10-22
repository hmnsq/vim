/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ಯದಿ !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# ಯದಿ defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# ಅನ್ಯಥಾ
#  include <gtk/gtkwidget.h>
# endif
#ಅನ್ಯಥಾ
# ಯದಿ defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    unsigned ಪೂರ್ಣಾಙ್ಕ	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    ಪೂರ್ಣಾಙ್ಕ			x;
    ಪೂರ್ಣಾಙ್ಕ			y;
    unsigned ಪೂರ್ಣಾಙ್ಕ	state;		// Button/Modifier key state
# ಅನ್ಯಥಾ
#  ಯದಿ !defined(FEAT_GUI_MSWIN)
    Widget		target;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_ಬೇರು;
    Position		y_ಬೇರು;
    ಪೂರ್ಣಾಙ್ಕ			state;		// Button/Modifier key state
#  ಅನ್ಯಥಾ
    HWND		target;
    HWND		balloon;
    ಪೂರ್ಣಾಙ್ಕ			x;
    ಪೂರ್ಣಾಙ್ಕ			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# ಯದಿ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    ಶೂನ್ಯ		(*ಸಂದೇಶ_CB)(ಸಂರಚನಾ BalloonEvalStruct *, ಪೂರ್ಣಾಙ್ಕ);
    ಶೂನ್ಯ		*clientData;	// For callback
#endif

    ಪೂರ್ಣಾಙ್ಕ			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    ಪೂರ್ಣಾಙ್ಕ			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*ಸಂದೇಶ;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    ಶೂನ್ಯ		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    ಪೂರ್ಣಾಙ್ಕ			x;
    ಪೂರ್ಣಾಙ್ಕ			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
