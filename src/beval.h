/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#എങ്കിൽ !defined(BEVAL__H) && defined(FEAT_BEVAL)
#define BEVAL__H

#ifdef FEAT_GUI_GTK
# എങ്കിൽ defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# അല്ലെങ്കിൽ
#  include <gtk/gtkwidget.h>
# endif
#അല്ലെങ്കിൽ
# എങ്കിൽ defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#endif

തരനിർവചനം എണ്ണൽ
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

തരനിർവചനം ഘടന BalloonEvalStruct
{
#ifdef FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*target;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    unsigned പൂർണ്ണസംഖ്യ	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    പൂർണ്ണസംഖ്യ			x;
    പൂർണ്ണസംഖ്യ			y;
    unsigned പൂർണ്ണസംഖ്യ	state;		// Button/Modifier key state
# അല്ലെങ്കിൽ
#  എങ്കിൽ !defined(FEAT_GUI_MSWIN)
    Widget		target;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_വേര്;
    Position		y_വേര്;
    പൂർണ്ണസംഖ്യ			state;		// Button/Modifier key state
#  അല്ലെങ്കിൽ
    HWND		target;
    HWND		balloon;
    പൂർണ്ണസംഖ്യ			x;
    പൂർണ്ണസംഖ്യ			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# എങ്കിൽ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_width;	// screen width in pixels
    Dimension		screen_height;	// screen height in pixels
# endif
    ശൂന്യം		(*സന്ദേശം_CB)(ഘടന BalloonEvalStruct *, പൂർണ്ണസംഖ്യ);
    ശൂന്യം		*clientData;	// For callback
#endif

    പൂർണ്ണസംഖ്യ			ts;		// tabstop setting for this buffer
#ifdef FEAT_VARTABS
    പൂർണ്ണസംഖ്യ			*vts;		// vartabstop setting for this buffer
#endif
    char_u		*സന്ദേശം;		// allocated: current text
#ifdef FEAT_GUI_MSWIN
    ശൂന്യം		*tofree;
#endif
#ifdef FEAT_GUI_HAIKU
    പൂർണ്ണസംഖ്യ			x;
    പൂർണ്ണസംഖ്യ			y;
#endif
} BalloonEval;

#define EVAL_OFFSET_X 15 // displacement of beval topleft corner from pointer
#define EVAL_OFFSET_Y 10

#ifdef FEAT_BEVAL_GUI
# include "gui_beval.pro"
#endif

#endif // BEVAL__H and FEAT_BEVAL_GUI
