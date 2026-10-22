/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ಆದರೆ !defined(BEVAL__H) && defined(FEAT_BEVAL)
#ವ್ಯಾಖ್ಯಾನಿಸು BEVAL__H

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
# ಆದರೆ defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  include <gtk/gtkwidget.h>
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ BalloonEvalStruct
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*ಗುರಿ;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    ಪೂರ್ಣಾಂಕ			x;
    ಪೂರ್ಣಾಂಕ			y;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	state;		// Button/Modifier key state
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ !defined(FEAT_GUI_MSWIN)
    Widget		ಗುರಿ;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_ಬೇರು;
    Position		y_ಬೇರು;
    ಪೂರ್ಣಾಂಕ			state;		// Button/Modifier key state
#  ಇಲ್ಲದಿದ್ದರೆ
    HWND		ಗುರಿ;
    HWND		balloon;
    ಪೂರ್ಣಾಂಕ			x;
    ಪೂರ್ಣಾಂಕ			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# ಆದರೆ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_ಅಗಲ;	// screen width in pixels
    Dimension		screen_ಎತ್ತರ;	// screen height in pixels
# endif
    ಶೂನ್ಯ		(*ಸಂದೇಶ_CB)(ರಚನೆ BalloonEvalStruct *, ಪೂರ್ಣಾಂಕ);
    ಶೂನ್ಯ		*client_ದತ್ತಾಂಶ;	// For callback
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ			ts;		// tabstop setting for this buffer
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಪೂರ್ಣಾಂಕ			*vts;		// vartabstop setting for this buffer
#ಆದರೆ_ಅಂತ್ಯ
    char_u		*ಸಂದೇಶ;		// allocated: current text
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
    ಶೂನ್ಯ		*tofree;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_HAIKU
    ಪೂರ್ಣಾಂಕ			x;
    ಪೂರ್ಣಾಂಕ			y;
#ಆದರೆ_ಅಂತ್ಯ
} BalloonEval;

#ವ್ಯಾಖ್ಯಾನಿಸು EVAL_ಸ್ಥಾನಾಂತರ_X 15 // displacement of beval topleft corner from pointer
#ವ್ಯಾಖ್ಯಾನಿಸು EVAL_ಸ್ಥಾನಾಂತರ_Y 10

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
# include "gui_beval.pro"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ // BEVAL__H and FEAT_BEVAL_GUI
