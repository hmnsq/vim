/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#જો !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# જો defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# નહીંતર
#  include <gtk/gtkwidget.h>
# endif
#નહીંતર
# જો defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

પ્રકારવ્યાખ્યા ગણના
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

પ્રકારવ્યાખ્યા સંરચના BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    unsigned પૂર્ણાંક	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    પૂર્ણાંક			x;
    પૂર્ણાંક			y;
    unsigned પૂર્ણાંક	state;		// Button/Modifier key state
# નહીંતર
#  જો !defined(FEAT_GUI_MSWIN)
    Widget		target;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_મૂળ;
    Position		y_મૂળ;
    પૂર્ણાંક			state;		// Button/Modifier key state
#  નહીંતર
    HWND		target;
    HWND		balloon;
    પૂર્ણાંક			x;
    પૂર્ણાંક			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# જો !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    શૂન્ય		(*સંદેશ_CB)(સંરચના BalloonEvalStruct *, પૂર્ણાંક);
    શૂન્ય		*clientData;	// For callback
#endif

    પૂર્ણાંક			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    પૂર્ણાંક			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*સંદેશ;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    શૂન્ય		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    પૂર્ણાંક			x;
    પૂર્ણાંક			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
