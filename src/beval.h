/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ఒకవేళ !defined(BEVAL__H) && defined(FEAT_BEVAL)
#నిర్వచించు BEVAL__H

#నిర్వచితమైతే FEAT_GUI_GTK
# ఒకవేళ defined(USE_GTK3) || defined(USE_GTK4)
#  include <gtk/gtk.h>
# లేకపోతే
#  include <gtk/gtkwidget.h>
# endif
#లేకపోతే
# ఒకవేళ defined(FEAT_GUI_X11)
#  include <X11/Intrinsic.h>
# endif
#అయితే_ముగింపు

రకనిర్వచనం గణన
{
    ShS_NEUTRAL,			// nothing showing or pending
    ShS_PENDING,			// data requested from debugger
    ShS_UPDATE_PENDING,			// switching information displayed
    ShS_SHOWING				// the balloon is being displayed
} BeState;

రకనిర్వచనం నిర్మాణం BalloonEvalStruct
{
#నిర్వచితమైతే FEAT_BEVAL_GUI
# ifdef FEAT_GUI_GTK
    GtkWidget		*లక్ష్యం;	// widget we are monitoring
    GtkWidget		*balloonShell;
    GtkWidget		*balloonLabel;
    గుర్తులేని పూర్ణాంకం	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    పూర్ణాంకం			x;
    పూర్ణాంకం			y;
    గుర్తులేని పూర్ణాంకం	state;		// Button/Modifier key state
# లేకపోతే
#  ఒకవేళ !defined(FEAT_GUI_MSWIN)
    Widget		లక్ష్యం;		// widget we are monitoring
    Widget		balloonShell;
    Widget		balloonLabel;
    XtIntervalId	timerID;	// timer for run
    BeState		showState;	// tells us what's currently going on
    XtAppContext	appContext;	// used in event handler
    Position		x;
    Position		y;
    Position		x_మూలం;
    Position		y_మూలం;
    పూర్ణాంకం			state;		// Button/Modifier key state
#  లేకపోతే
    HWND		లక్ష్యం;
    HWND		balloon;
    పూర్ణాంకం			x;
    పూర్ణాంకం			y;
    BeState		showState;	// tells us what's currently going on
#  endif
# endif
# ఒకవేళ !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)
    Dimension		screen_వెడల్పు;	// screen width in pixels
    Dimension		screen_ఎత్తు;	// screen height in pixels
# endif
    శూన్య		(*సందేశం_CB)(నిర్మాణం BalloonEvalStruct *, పూర్ణాంకం);
    శూన్య		*client_దత్తాంశం;	// For callback
#అయితే_ముగింపు

    పూర్ణాంకం			ts;		// tabstop setting for this buffer
#నిర్వచితమైతే FEAT_VARTABS
    పూర్ణాంకం			*vts;		// vartabstop setting for this buffer
#అయితే_ముగింపు
    char_u		*సందేశం;		// allocated: current text
#నిర్వచితమైతే FEAT_GUI_MSWIN
    శూన్య		*tofree;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_HAIKU
    పూర్ణాంకం			x;
    పూర్ణాంకం			y;
#అయితే_ముగింపు
} BalloonEval;

#నిర్వచించు EVAL_స్థానభ్రంశం_X 15 // displacement of beval topleft corner from pointer
#నిర్వచించు EVAL_స్థానభ్రంశం_Y 10

#నిర్వచితమైతే FEAT_BEVAL_GUI
# include "gui_beval.pro"
#అయితే_ముగింపు

#అయితే_ముగింపు // BEVAL__H and FEAT_BEVAL_GUI
