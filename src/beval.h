/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#जर !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# जर defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# अन्यथा
#  include <gtk/gtkwidget.h>
# endif
#अन्यथा
# जर defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

प्रकारव्याख्या गणना
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

प्रकारव्याख्या संरचना BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    अचिन्हित पूर्णांक	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    पूर्णांक			x;
    पूर्णांक			y;
    अचिन्हित पूर्णांक	state;		// Button/Modifier key state
# अन्यथा
#  जर !defined(FEAT_GUI_MSWIN)
    Widget		target;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_मूळ;
    Position		y_मूळ;
    पूर्णांक			state;		// Button/Modifier key state
#  अन्यथा
    HWND		target;
    HWND		balloon;
    पूर्णांक			x;
    पूर्णांक			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# जर !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    शून्य		(*संदेश_CB)(संरचना BalloonEvalStruct *, पूर्णांक);
    शून्य		*clientData;	// For callback
#endif

    पूर्णांक			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    पूर्णांक			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*संदेश;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    शून्य		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    पूर्णांक			x;
    पूर्णांक			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
