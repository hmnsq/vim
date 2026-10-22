/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#nếu !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# nếu defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# không_thì
#  include <gtk/gtkwidget.h>
# endif
#không_thì
# nếu defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

typedef enum
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

typedef struct BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    unsigned số_nguyên	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    số_nguyên			x;
    số_nguyên			y;
    unsigned số_nguyên	state;		// Button/Modifier key state
# không_thì
#  nếu !defined(FEAT_GUI_MSWIN)
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
    số_nguyên			state;		// Button/Modifier key state
#  không_thì
    HWND		target;
    HWND		balloon;
    số_nguyên			x;
    số_nguyên			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# nếu !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    void		(*msgCB)(struct BalloonEvalStruct *, số_nguyên);
    void		*clientData;	// For callback
#endif

    số_nguyên			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    số_nguyên			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*msg;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    void		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    số_nguyên			x;
    số_nguyên			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
