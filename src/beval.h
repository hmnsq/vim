/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#якщо !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# якщо defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# інакше
#  include <gtk/gtkwidget.h>
# endif
#інакше
# якщо defined(FEAT_GUI_X11)
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
    unsigned ціле	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    ціле			x;
    ціле			y;
    unsigned ціле	state;		// Button/Modifier key state
# інакше
#  якщо !defined(FEAT_GUI_MSWIN)
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
    ціле			state;		// Button/Modifier key state
#  інакше
    HWND		target;
    HWND		balloon;
    ціле			x;
    ціле			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# якщо !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    void		(*msgCB)(struct BalloonEvalStruct *, ціле);
    void		*clientData;	// For callback
#endif

    ціле			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    ціле			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*msg;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    void		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    ціле			x;
    ціле			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
