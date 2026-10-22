/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#जइ !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# जइ defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# अण्णहा
#  include <gtk/gtkwidget.h>
# endif
#अण्णहा
# जइ defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

प्रकारनिर्देश गणना
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

प्रकारनिर्देश संरचना BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    unsigned पूर्णाङ्क	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    पूर्णाङ्क			x;
    पूर्णाङ्क			y;
    unsigned पूर्णाङ्क	state;		// Button/Modifier key state
# अण्णहा
#  जइ !defined(FEAT_GUI_MSWIN)
    Widget		target;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_मूलम्;
    Position		y_मूलम्;
    पूर्णाङ्क			state;		// Button/Modifier key state
#  अण्णहा
    HWND		target;
    HWND		balloon;
    पूर्णाङ्क			x;
    पूर्णाङ्क			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# जइ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    शून्य		(*सन्देशः_CB)(संरचना BalloonEvalStruct *, पूर्णाङ्क);
    शून्य		*clientData;	// For callback
#endif

    पूर्णाङ्क			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    पूर्णाङ्क			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*सन्देशः;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    शून्य		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    पूर्णाङ्क			x;
    पूर्णाङ्क			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
