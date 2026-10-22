/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#ಆದರೆ !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  ಇಲ್ಲದಿದ್ದರೆ
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
ಬಾಹ್ಯ Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
ಸ್ಥಿರ BalloonEval *current_beval = ಖಾಲಿ;
# endif

# ifdef FEAT_GUI_GTK
ಸ್ಥಿರ ಶೂನ್ಯ ಕೂಡಿಸು_Event_Handler(GtkWidget *, BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
ಸ್ಥಿರ gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ gint ಗುರಿ_event_cb(GtkWidget *, GdkEvent *, gpointer);
ಸ್ಥಿರ gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
ಸ್ಥಿರ ಶೂನ್ಯ ಸೂಚಕ_event(BalloonEval *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಚಿಹ್ನೆರಹಿತ);
ಸ್ಥಿರ ಶೂನ್ಯ ಕೀಲಿ_event(BalloonEval *, ಚಿಹ್ನೆರಹಿತ, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
ಸ್ಥಿರ gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ ಕೂಡಿಸು_Event_Handler(Widget, BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ removeEventHandler(BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ ಸೂಚಕ_Event_EH(Widget, Xt_ಸೂಚಕ, XEvent *, Boolean *);
ಸ್ಥಿರ ಶೂನ್ಯ ಸೂಚಕ_Event(BalloonEval *, XEvent *);
ಸ್ಥಿರ ಶೂನ್ಯ timerRoutine(Xt_ಸೂಚಕ, XtIntervalId *);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ cancelBalloon(BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ requestBalloon(BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ drawBalloon(BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ undrawBalloon(BalloonEval *beval);
ಸ್ಥಿರ ಶೂನ್ಯ create_Balloon_Eval_ಕಿಟಕಿ(BalloonEval *);

/*
 * Create a balloon-evaluation area for a Widget.
 * There can be either a "mesg" for a fixed string or "mesgCB" to generate a
 * message by calling this callback function.
 * When "mesg" is not NULL it must remain valid for as long as the balloon is
 * used.  It is not freed here.
 * Returns a pointer to the resulting object (NULL when out of memory).
 */
    BalloonEval *
gui_mch_create_beval_area(
    ಶೂನ್ಯ	*ಗುರಿ,
    char_u	*mesg,
    ಶೂನ್ಯ	(*mesgCB)(BalloonEval *, ಪೂರ್ಣಾಂಕ),
    ಶೂನ್ಯ	*client_ದತ್ತಾಂಶ)
{
# ifndef FEAT_GUI_GTK
    ಅಕ್ಷರ	*display_name;	    // get from gui.dpy
    ಪೂರ್ಣಾಂಕ		screen_ಅಂಕೆ;
    ಅಕ್ಷರ	*p;
# endif
    BalloonEval	*beval;

    ಆದರೆ (mesg != ಖಾಲಿ && mesgCB != ಖಾಲಿ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_ಸಂದೇಶ_and_callback);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ಆದರೆ (beval != ಖಾಲಿ)
    {
# ifdef FEAT_GUI_GTK
	beval->ಗುರಿ = GTK_WIDGET(ಗುರಿ);
# ಇಲ್ಲದಿದ್ದರೆ
	beval->ಗುರಿ = (Widget)ಗುರಿ;
	beval->appContext = Xt_Widget_ಗಮ್ಯ_Application_Context((Widget)ಗುರಿ);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->ಸಂದೇಶ);
	beval->ಸಂದೇಶ = mesg == ಖಾಲಿ ? ಖಾಲಿ : vim_strsave(mesg);
	beval->ಸಂದೇಶ_CB = mesgCB;
	beval->client_ದತ್ತಾಂಶ = client_ದತ್ತಾಂಶ;

	/*
	 * Set up event handler which will keep its eyes on the pointer,
	 * and when the pointer rests in a certain spot for a given time
	 * interval, show the beval.
	 */
	ಕೂಡಿಸು_Event_Handler(beval->ಗುರಿ, beval);
	create_Balloon_Eval_ಕಿಟಕಿ(beval);

# ifndef FEAT_GUI_GTK
	/*
	 * Now create and save the screen width and height. Used in drawing.
	 */
	display_name = DisplayString(gui.dpy);
	p = strrchr(display_name, '.');
	ಆದರೆ (p != ಖಾಲಿ)
	    screen_ಅಂಕೆ = atoi(++p);
	ಇಲ್ಲದಿದ್ದರೆ
	    screen_ಅಂಕೆ = 0;
	beval->screen_ಅಗಲ = Display_ಅಗಲ(gui.dpy, screen_ಅಂಕೆ);
	beval->screen_ಎತ್ತರ = Display_ಎತ್ತರ(gui.dpy, screen_ಅಂಕೆ);
# endif
    }

    ಹಿಂತಿರುಗಿ beval;
}

# ಆದರೆ defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    ಶೂನ್ಯ
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   ಇಲ್ಲದಿದ್ದರೆ
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  ಇಲ್ಲದಿದ್ದರೆ
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    ಆದರೆ (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->ಸಂದೇಶ);
    vim_free(beval);
}
# endif

    ಶೂನ್ಯ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ಆದರೆ (beval != ಖಾಲಿ)
	ಕೂಡಿಸು_Event_Handler(beval->ಗುರಿ, beval);
}

    ಶೂನ್ಯ
gui_mch_disable_beval_area(BalloonEval *beval)
{
    ಆದರೆ (beval != ಖಾಲಿ)
	removeEventHandler(beval);
}

# ಆದರೆ defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ current_beval;
}
# endif
#ಆದರೆ_ಅಂತ್ಯ // !FEAT_GUI_MSWIN

#ಆದರೆ defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# ಆದರೆ !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    ಶೂನ್ಯ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->ಸಂದೇಶ);
    beval->ಸಂದೇಶ = mesg == ಖಾಲಿ ? ಖಾಲಿ : vim_strsave(mesg);
    ಆದರೆ (beval->ಸಂದೇಶ != ಖಾಲಿ)
	drawBalloon(beval);
    ಇಲ್ಲದಿದ್ದರೆ
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#ಆದರೆ_ಅಂತ್ಯ // FEAT_NETBEANS_INTG || FEAT_EVAL

#ಆದರೆ !defined(FEAT_GUI_MSWIN)
# ಆದರೆ defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    ಶೂನ್ಯ
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->ಸಂದೇಶ);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_Event_Handler(GtkWidget *ಗುರಿ, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(ಗುರಿ), "beval", controller);
    gtk_widget_ಕೂಡಿಸು_controller(ಗುರಿ, controller);
#  ಇಲ್ಲದಿದ್ದರೆ
    /*
     * Connect to the generic "event" signal instead of the individual
     * signals for each event type, because the former is emitted earlier.
     * This allows us to catch events independently of the signal handlers
     * in gui_gtk_x11.c.
     */
    g_signal_connect(G_OBJECT(ಗುರಿ), "event",
		     G_CALLBACK(ಗುರಿ_event_cb),
		     beval);
    /*
     * Nasty:  Key press events go to the main window thus the drawing area
     * will never see them.  This means we have to connect to the main window
     * as well in order to catch those events.
     */
    ಆದರೆ (gtk_socket_id == 0 && gui.mainwin != ಖಾಲಿ
	    && gtk_widget_is_ancestor(ಗುರಿ, gui.mainwin))
    {
	gtk_widget_ಕೂಡಿಸು_events(gui.mainwin,
			      GDK_LEAVE_NOTIFY_ಮುಖವಾಡ);

	g_signal_connect(G_OBJECT(gui.mainwin), "event",
			 G_CALLBACK(mainwin_event_cb),
			 beval);
    }
#  endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*ಗುರಿ = beval->ಗುರಿ;
    GtkEventController	*controller;

    controller = g_object_get_ದತ್ತಾಂಶ(G_OBJECT(ಗುರಿ), "beval");
    ಆದರೆ (controller != ಖಾಲಿ)
    {
	gtk_widget_remove_controller(ಗುರಿ, controller);
	g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(ಗುರಿ), "beval", ಖಾಲಿ);
    }
#  ಇಲ್ಲದಿದ್ದರೆ
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->ಗುರಿ),
					 FUNC2GENERIC(ಗುರಿ_event_cb),
					 beval);

    ಆದರೆ (gtk_socket_id == 0 && gui.mainwin != ಖಾಲಿ
	    && gtk_widget_is_ancestor(beval->ಗುರಿ, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    ಸ್ಥಿರ gboolean
event_cb(GtkEventController *controller ಬಳಸದ, GdkEvent *event, BalloonEval *beval)
{
    ಪೂರ್ಣಾಂಕ type = gdk_event_get_event_type(event);

    ಆಯ್ಕೆ (type)
    {
	ಸ್ಥಿತಿ GDK_ENTER_NOTIFY:
	ಸ್ಥಿತಿ GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*ಬೇರು;
	    graphene_ಬಿಂದು_t	p;
	    graphene_ಬಿಂದು_t	ಫಲಿತಾಂಶ;
	    ದ್ವಿಗುಣ		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    ಬೇರು = GTK_WIDGET(gtk_widget_get_ಬೇರು(beval->ಗುರಿ));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    ಆದರೆ (gtk_widget_compute_ಬಿಂದು(ಬೇರು, beval->ಗುರಿ, &p, &ಫಲಿತಾಂಶ))
	    {
		x = ಫಲಿತಾಂಶ.x;
		y = ಫಲಿತಾಂಶ.y;
	    }

	    ಸೂಚಕ_event(beval, (ಪೂರ್ಣಾಂಕ)x, (ಪೂರ್ಣಾಂಕ)y,
		    gdk_event_get_modifier_state(event));
	    ನಿಲ್ಲಿಸು;
	}
	ಸ್ಥಿತಿ GDK_LEAVE_NOTIFY:
	    ಆದರೆ (gdk_ದಾಟುವಿಕೆ_event_get_ಬಹುಲಕ(event) == GDK_ದಾಟುವಿಕೆ_ಸಾಮಾನ್ಯ)
		cancelBalloon(beval);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_BUTTON_PRESS:
	ಸ್ಥಿತಿ GDK_SCROLL:
	    cancelBalloon(beval);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_PRESS:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_RELEASE:
	    ಕೀಲಿ_event(beval, gdk_ಕೀಲಿ_event_get_keyval(event),
		    type == GDK_ಕೀಲಿ_PRESS);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }

    // Always pass through events?
    ಹಿಂತಿರುಗಿ FALSE;
}
#  ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ gint
ಗುರಿ_event_cb(GtkWidget *widget, GdkEvent *event, gpointer ದತ್ತಾಂಶ)
{
    BalloonEval *beval = (BalloonEval *)ದತ್ತಾಂಶ;

    ಆಯ್ಕೆ (event->type)
    {
	ಸ್ಥಿತಿ GDK_ENTER_NOTIFY:
	    ಸೂಚಕ_event(beval, (ಪೂರ್ಣಾಂಕ)event->ದಾಟುವಿಕೆ.x,
				 (ಪೂರ್ಣಾಂಕ)event->ದಾಟುವಿಕೆ.y,
				 event->ದಾಟುವಿಕೆ.state);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_MOTION_NOTIFY:
	    ಆದರೆ (event->motion.is_hint)
	    {
		ಪೂರ್ಣಾಂಕ		x;
		ಪೂರ್ಣಾಂಕ		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
		{
		    Gdk_ಕಿಟಕಿ * ಸ್ಥಿರಾಂಕ win = gtk_widget_get_ಕಿಟಕಿ(widget);
		    GdkDisplay * ಸ್ಥಿರಾಂಕ dpy = gdk_ಕಿಟಕಿ_get_display(win);
#    ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,20,0)
		    GdkSeat * ಸ್ಥಿರಾಂಕ seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * ಸ್ಥಿರಾಂಕ dev = gdk_seat_get_ಸೂಚಕ(seat);
#    ಇಲ್ಲದಿದ್ದರೆ
		    GdkDeviceManager * ಸ್ಥಿರಾಂಕ mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * ಸ್ಥಿರಾಂಕ dev = gdk_device_manager_get_client_ಸೂಚಕ(mngr);
#    endif
		    gdk_ಕಿಟಕಿ_get_device_position(win, dev , &x, &y, &state);
		}
#   ಇಲ್ಲದಿದ್ದರೆ
		gdk_ಕಿಟಕಿ_get_ಸೂಚಕ(widget->ಕಿಟಕಿ_2, &x, &y, &state);
#   endif
		ಸೂಚಕ_event(beval, x, y, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)state);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಸೂಚಕ_event(beval, (ಪೂರ್ಣಾಂಕ)event->motion.x,
				     (ಪೂರ್ಣಾಂಕ)event->motion.y,
				     event->motion.state);
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    ಆದರೆ (event->ದಾಟುವಿಕೆ.ಬಹುಲಕ == GDK_ದಾಟುವಿಕೆ_ಸಾಮಾನ್ಯ)
		cancelBalloon(beval);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_BUTTON_PRESS:
	ಸ್ಥಿತಿ GDK_SCROLL:
	    cancelBalloon(beval);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_PRESS:
	    ಕೀಲಿ_event(beval, event->ಕೀಲಿ.keyval, TRUE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_RELEASE:
	    ಕೀಲಿ_event(beval, event->ಕೀಲಿ.keyval, FALSE);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ FALSE; // continue emission
}

    ಸ್ಥಿರ gint
mainwin_event_cb(GtkWidget *widget ಬಳಸದ, GdkEvent *event, gpointer ದತ್ತಾಂಶ)
{
    BalloonEval *beval = (BalloonEval *)ದತ್ತಾಂಶ;

    ಆಯ್ಕೆ (event->type)
    {
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_PRESS:
	    ಕೀಲಿ_event(beval, event->ಕೀಲಿ.keyval, TRUE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_RELEASE:
	    ಕೀಲಿ_event(beval, event->ಕೀಲಿ.keyval, FALSE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    ಆದರೆ (event->ದಾಟುವಿಕೆ.ಬಹುಲಕ == GDK_ದಾಟುವಿಕೆ_ಸಾಮಾನ್ಯ)
		cancelBalloon(beval);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ FALSE; // continue emission
}
#  endif

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೂಚಕ_event(BalloonEval *beval, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಚಿಹ್ನೆರಹಿತ state)
{
    ಪೂರ್ಣಾಂಕ ದೂರ;

    ದೂರ = ABS(x - beval->x) + ABS(y - beval->y);

    ಆದರೆ (ದೂರ <= 4)
	ಹಿಂತಿರುಗಿ;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    ಆದರೆ (!(state & ((ಪೂರ್ಣಾಂಕ)GDK_BUTTON1_ಮುಖವಾಡ | (ಪೂರ್ಣಾಂಕ)GDK_BUTTON2_ಮುಖವಾಡ
		    | (ಪೂರ್ಣಾಂಕ)GDK_BUTTON3_ಮುಖವಾಡ)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	ಆದರೆ (state & (ಪೂರ್ಣಾಂಕ)GDK_ALT_ಮುಖವಾಡ)
#  ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (state & (ಪೂರ್ಣಾಂಕ)GDK_MOD1_ಮುಖವಾಡ)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    ಆದರೆ (beval->ಸಂದೇಶ_CB != ಖಾಲಿ)
	    {
		beval->showState = ShS_PENDING;
		(*beval->ಸಂದೇಶ_CB)(beval, state);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    beval->timerID = g_timeout_ಕೂಡಿಸು((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೀಲಿ_event(BalloonEval *beval, ಚಿಹ್ನೆರಹಿತ keyval, ಪೂರ್ಣಾಂಕ is_keypress)
{
    ಆದರೆ (beval->showState == ShS_SHOWING && beval->ಸಂದೇಶ_CB != ಖಾಲಿ)
    {
	ಆಯ್ಕೆ (keyval)
	{
#  ifdef USE_GTK4
	    ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ಸರಿಸು_L:
	    ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ಸರಿಸು_R:
#  ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ಥಿತಿ GDK_ಸರಿಸು_L:
	    ಸ್ಥಿತಿ GDK_ಸರಿಸು_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->ಸಂದೇಶ_CB)(beval, (is_keypress)
						   ? (ಪೂರ್ಣಾಂಕ)GDK_ಸರಿಸು_ಮುಖವಾಡ : 0);
		ನಿಲ್ಲಿಸು;
#  ifdef USE_GTK4
	    ಸ್ಥಿತಿ GDK_ಕೀಲಿ_Control_L:
	    ಸ್ಥಿತಿ GDK_ಕೀಲಿ_Control_R:
#  ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ಥಿತಿ GDK_Control_L:
	    ಸ್ಥಿತಿ GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->ಸಂದೇಶ_CB)(beval, (is_keypress)
						 ? (ಪೂರ್ಣಾಂಕ)GDK_CONTROL_ಮುಖವಾಡ : 0);
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		ಆದರೆ (is_keypress)
		    cancelBalloon(beval);
		ನಿಲ್ಲಿಸು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	cancelBalloon(beval);
}

    ಸ್ಥಿರ gboolean
timeout_cb(gpointer ದತ್ತಾಂಶ)
{
    BalloonEval *beval = (BalloonEval *)ದತ್ತಾಂಶ;

    beval->timerID = 0;
    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);

    ಹಿಂತಿರುಗಿ FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 ದತ್ತಾಂಶ ಬಳಸದ)
{
    GtkStyleContext *context = ಖಾಲಿ;
    gint ಅಗಲ = -1, ಎತ್ತರ = -1;

    ಆದರೆ (widget == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    context = gtk_widget_get_style_context(widget);
    ಅಗಲ = gtk_widget_get_allocated_ಅಗಲ(widget);
    ಎತ್ತರ = gtk_widget_get_allocated_ಎತ್ತರ(widget);

    gtk_style_context_save(context);

    gtk_style_context_ಕೂಡಿಸು_class(context, "tooltip");
    gtk_style_context_ಗಣ_state(context, GTK_STATE_FLAG_ಸಾಮಾನ್ಯ);

    cairo_save(cr);
    gtk_render_ಚೌಕಟ್ಟು(context, cr, 0, 0, ಅಗಲ, ಎತ್ತರ);
    gtk_render_background(context, cr, 0, 0, ಅಗಲ, ಎತ್ತರ);
    cairo_restore(cr);

    gtk_style_context_restore(context);

    ಹಿಂತಿರುಗಿ FALSE;
}
#   ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    gtk_paint_flat_box(widget->style, widget->ಕಿಟಕಿ_2,
		       GTK_STATE_ಸಾಮಾನ್ಯ, GTK_SHADOW_ಹೊರಹೋಗು,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    ಹಿಂತಿರುಗಿ FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# ಇಲ್ಲದಿದ್ದರೆ // !FEAT_GUI_GTK

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_Event_Handler(Widget ಗುರಿ, BalloonEval *beval)
{
    Xt_ಕೂಡಿಸು_Event_Handler(ಗುರಿ,
			ಸೂಚಕ_Motion_ಮುಖವಾಡ | Enter_ಕಿಟಕಿ_ಮುಖವಾಡ |
			Leave_ಕಿಟಕಿ_ಮುಖವಾಡ | Button_Press_ಮುಖವಾಡ | ಕೀಲಿ_Press_ಮುಖವಾಡ |
			ಕೀಲಿ_Release_ಮುಖವಾಡ,
			False,
			ಸೂಚಕ_Event_EH, (Xt_ಸೂಚಕ)beval);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->ಗುರಿ,
			ಸೂಚಕ_Motion_ಮುಖವಾಡ | Enter_ಕಿಟಕಿ_ಮುಖವಾಡ |
			Leave_ಕಿಟಕಿ_ಮುಖವಾಡ | Button_Press_ಮುಖವಾಡ | ಕೀಲಿ_Press_ಮುಖವಾಡ |
			ಕೀಲಿ_Release_ಮುಖವಾಡ,
			False,
			ಸೂಚಕ_Event_EH, (Xt_ಸೂಚಕ)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೂಚಕ_Event_EH(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2,
    XEvent	*event,
    Boolean	*ಬಳಸದ_2 ಬಳಸದ)
{
    BalloonEval *beval = (BalloonEval *)client_ದತ್ತಾಂಶ_2;
    ಸೂಚಕ_Event(beval, event);
}


/*
 * The real event handler. Called by pointerEventEH() whenever an event we are
 * interested in occurs.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೂಚಕ_Event(BalloonEval *beval, XEvent *event)
{
    Position	ದೂರ;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    ಆಯ್ಕೆ (event->type)
    {
	ಸ್ಥಿತಿ EnterNotify:
	ಸ್ಥಿತಿ MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    ಆದರೆ (delta < 0)
		delta = -delta;
	    ದೂರ = delta;
	    delta = event->xmotion.y - beval->y;
	    ಆದರೆ (delta < 0)
		delta = -delta;
	    ದೂರ += delta;
	    ಆದರೆ (ದೂರ > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		ಆದರೆ (beval->state & (Button1_ಮುಖವಾಡ|Button2_ಮುಖವಾಡ|Button3_ಮುಖವಾಡ))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (beval->state & (Mod1_ಮುಖವಾಡ|Mod2_ಮುಖವಾಡ|Mod3_ಮುಖವಾಡ))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_ಬೇರು = event->xmotion.x_ಬೇರು;
		    beval->y_ಬೇರು = event->xmotion.y_ಬೇರು;
		    cancelBalloon(beval);
		    ಆದರೆ (beval->ಸಂದೇಶ_CB != ಖಾಲಿ)
		    {
			beval->showState = ShS_PENDING;
			(*beval->ಸಂದೇಶ_CB)(beval, beval->state);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_ಬೇರು = event->xmotion.x_ಬೇರು;
		    beval->y_ಬೇರು = event->xmotion.y_ಬೇರು;
		    cancelBalloon(beval);
		    beval->timerID = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    ನಿಲ್ಲಿಸು;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	ಸ್ಥಿತಿ ಕೀಲಿ_Press:
	    ಆದರೆ (beval->showState == ShS_SHOWING && beval->ಸಂದೇಶ_CB != ಖಾಲಿ)
	    {
		Modifiers   modifier;
		ಕೀಲಿ_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		ಆದರೆ (keysym == XK_ಸರಿಸು_L || keysym == XK_ಸರಿಸು_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ಸಂದೇಶ_CB)(beval, ಸರಿಸು_ಮುಖವಾಡ);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ಸಂದೇಶ_CB)(beval, Control_ಮುಖವಾಡ);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    cancelBalloon(beval);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		cancelBalloon(beval);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ಕೀಲಿ_Release:
	    ಆದರೆ (beval->showState == ShS_SHOWING && beval->ಸಂದೇಶ_CB != ಖಾಲಿ)
	    {
		Modifiers modifier;
		ಕೀಲಿ_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		ಆದರೆ ((keysym == XK_ಸರಿಸು_L) || (keysym == XK_ಸರಿಸು_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ಸಂದೇಶ_CB)(beval, 0);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ಸಂದೇಶ_CB)(beval, 0);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    cancelBalloon(beval);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		cancelBalloon(beval);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		ಆದರೆ (event->xcrossing.ಬಹುಲಕ == Notify_ಸಾಮಾನ್ಯ)
		    cancelBalloon(beval);
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ButtonPress:
		cancelBalloon(beval);
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
timerRoutine(Xt_ಸೂಚಕ dx, XtIntervalId *id ಬಳಸದ)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)ಖಾಲಿ;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    ಸ್ಥಿರ ಶೂನ್ಯ
requestBalloon(BalloonEval *beval)
{
    ಆದರೆ (beval->showState == ShS_PENDING)
	ಹಿಂತಿರುಗಿ;

    // Determine the beval to display
    ಆದರೆ (beval->ಸಂದೇಶ_CB != ಖಾಲಿ)
    {
	beval->showState = ShS_PENDING;
	(*beval->ಸಂದೇಶ_CB)(beval, beval->state);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (beval->ಸಂದೇಶ != ಖಾಲಿ)
	drawBalloon(beval);
}

# ifdef FEAT_GUI_GTK
/*
 * Convert the string to UTF-8 if 'encoding' is not "utf-8".
 * Replace any non-printable characters and invalid bytes sequences with
 * "^X" or "<xx>" escapes, and apply SpecialKey highlighting to them.
 * TAB and NL are passed through unscathed.
 */
#  define IS_NONPRINTABLE(c) (((c) < 0x20 && (c) != TAB && (c) != NL) \
			      || (c) == DEL)
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_printable_label_ಪಠ್ಯ(GtkLabel *label, char_u *ಪಠ್ಯ)
{
    char_u	    *convbuf = ಖಾಲಿ;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    len;
    ಪೂರ್ಣಾಂಕ		    charlen;
    ಪೂರ್ಣಾಂಕ		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    ಆದರೆ (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, ಪಠ್ಯ, ಖಾಲಿ);
	ಆದರೆ (convbuf != ಖಾಲಿ)
	    ಪಠ್ಯ = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    ಗಾಗಿ (p = ಪಠ್ಯ; *p != NUL; p += charlen)
    {
	ಆದರೆ ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    ಆದರೆ (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    ಆದರೆ (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    ಇಲ್ಲದಿದ್ದರೆ
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    ಆದರೆ (buf != ಖಾಲಿ)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   ಆದರೆ PANGO_ಆವೃತ್ತಿ_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  ಇಲ್ಲದಿದ್ದರೆ
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != ಖಾಲಿ) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	ಆದರೆ (pixel != INVALCOLOR)
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  ಇಲ್ಲದಿದ್ದರೆ
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)pixel, &color);
#  endif

	pdest = buf;
	p = ಪಠ್ಯ;
	ಆಗ (*p != NUL)
	{
	    // Be quick for ASCII
	    ಆದರೆ ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		ಆದರೆ (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		ಆದರೆ (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    ಪೂರ್ಣಾಂಕ	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    ಆದರೆ (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    ಆದರೆ (pixel != INVALCOLOR)
		    {
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   ಆದರೆ PANGO_ಆವೃತ್ತಿ_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  ಇಲ್ಲದಿದ್ದರೆ
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->ಆರಂಭ_ಸೂಚ್ಯಂಕ = pdest - buf;
			attr->ಕೊನೆ_ಸೂಚ್ಯಂಕ   = pdest - buf + outlen;
			pango_attr_list_ಸೇರಿಸು(attr_list, attr);
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
#   ಆದರೆ PANGO_ಆವೃತ್ತಿ_CHECK(1,38,0)
			attr_alpha->ಆರಂಭ_ಸೂಚ್ಯಂಕ = pdest - buf;
			attr_alpha->ಕೊನೆ_ಸೂಚ್ಯಂಕ   = pdest - buf + outlen;
			pango_attr_list_ಸೇರಿಸು(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಮಾಡು
			*pdest++ = *p++;
		    ಆಗ (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_ಗಣ_ಪಠ್ಯ(label, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf);
    vim_free(buf);

    gtk_label_ಗಣ_ಗುಣಲಕ್ಷಣಗಳು(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    ಸ್ಥಿರ ಶೂನ್ಯ
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    ಪೂರ್ಣಾಂಕ		    natural_ಅಗಲ = 0;

    ಆದರೆ (beval->ಸಂದೇಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಣ_printable_label_ಪಠ್ಯ(GTK_LABEL(beval->balloonLabel), beval->ಸಂದೇಶ);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    ಖಾಲಿ, &natural_ಅಗಲ, ಖಾಲಿ, ಖಾಲಿ);

    rect.ಅಗಲ = (natural_ಅಗಲ > 0 ? natural_ಅಗಲ : 1) + gui.char_ಅಗಲ * 3;
    rect.ಎತ್ತರ = gui.char_ಎತ್ತರ;

    gtk_popover_ಗಣ_pointing_ಗಮ್ಯ(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ ಶೂನ್ಯ
drawBalloon(BalloonEval *beval)
{
    ಆದರೆ (beval->ಸಂದೇಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    GtkRequisition	requisition;
    ಪೂರ್ಣಾಂಕ		screen_w;
    ಪೂರ್ಣಾಂಕ		screen_h;
    ಪೂರ್ಣಾಂಕ		screen_x;
    ಪೂರ್ಣಾಂಕ		screen_y;
    ಪೂರ್ಣಾಂಕ		x;
    ಪೂರ್ಣಾಂಕ		y;
    ಪೂರ್ಣಾಂಕ		x_ಸ್ಥಾನಾಂತರ = EVAL_ಸ್ಥಾನಾಂತರ_X;
    ಪೂರ್ಣಾಂಕ		y_ಸ್ಥಾನಾಂತರ = EVAL_ಸ್ಥಾನಾಂತರ_Y;
    PangoLayout	*layout;

#   ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->ಗುರಿ);
    gtk_ಕಿಟಕಿ_ಗಣ_screen(GTK_ಕಿಟಕಿ(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_ದ_win(beval->ಗುರಿ, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    ಗಣ_printable_label_ಪಠ್ಯ(GTK_LABEL(beval->balloonLabel), beval->ಸಂದೇಶ);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_ಶಬ್ದ_CHAR
    pango_layout_ಗಣ_wrap(layout, PANGO_WRAP_ಶಬ್ದ_CHAR);
#   ಇಲ್ಲದಿದ್ದರೆ
    pango_layout_ಗಣ_wrap(layout, PANGO_WRAP_ಶಬ್ದ);
#   endif
    pango_layout_ಗಣ_ಅಗಲ(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * gui.char_ಅಗಲ,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_widget_get_preferred_ಗಾತ್ರ(beval->balloonShell, &requisition, ಖಾಲಿ);
#   ಇಲ್ಲದಿದ್ದರೆ
    gtk_widget_ಗಾತ್ರ_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_ಕಿಟಕಿ_get_origin(gtk_widget_get_ಕಿಟಕಿ(beval->ಗುರಿ), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    ಆದರೆ (x + x_ಸ್ಥಾನಾಂತರ + requisition.ಅಗಲ > screen_x + screen_w)
	y_ಸ್ಥಾನಾಂತರ += 15;
    ಆದರೆ (y + y_ಸ್ಥಾನಾಂತರ + requisition.ಎತ್ತರ > screen_y + screen_h)
	y_ಸ್ಥಾನಾಂತರ = -requisition.ಎತ್ತರ - EVAL_ಸ್ಥಾನಾಂತರ_Y;

    // Sanitize values
    x = CLAMP(x + x_ಸ್ಥಾನಾಂತರ, 0,
	    MAX(0, screen_x + screen_w - requisition.ಅಗಲ));
    y = CLAMP(y + y_ಸ್ಥಾನಾಂತರ, 0,
	    MAX(0, screen_y + screen_h - requisition.ಎತ್ತರ));

    // Show the balloon
#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_ಕಿಟಕಿ_move(GTK_ಕಿಟಕಿ(beval->balloonShell), x, y);
#   ಇಲ್ಲದಿದ್ದರೆ
    gtk_widget_ಗಣ_uposition(beval->balloonShell, x, y);
#   endif
    gtk_widget_show(beval->balloonShell);

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  endif

/*
 * Undraw a balloon.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
undrawBalloon(BalloonEval *beval)
{
    ಆದರೆ (beval->balloonShell != ಖಾಲಿ)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  ಇಲ್ಲದಿದ್ದರೆ
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cancelBalloon(BalloonEval *beval)
{
    ಆದರೆ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ಆದರೆ (beval->timerID != 0)
    {
	g_ಮೂಲ_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    ಸ್ಥಿರ ಶೂನ್ಯ
create_Balloon_Eval_ಕಿಟಕಿ(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_ಗಣ_ಪೋಷಕ(beval->balloonShell, beval->ಗುರಿ);
    gtk_popover_ಗಣ_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_ಗಣ_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_ಗಣ_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(ಖಾಲಿ);

    gtk_label_ಗಣ_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_ಗಣ_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ಎಡ);
    gtk_label_ಗಣ_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_ಗಣ_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_ಗಣ_ಮಗು(GTK_POPOVER(popover), beval->balloonLabel);
}
#  ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ ಶೂನ್ಯ
create_Balloon_Eval_ಕಿಟಕಿ(BalloonEval *beval)
{
    beval->balloonShell = gtk_ಕಿಟಕಿ_new(GTK_ಕಿಟಕಿ_POPUP);
    gtk_ಕಿಟಕಿ_ಗಣ_transient_for(GTK_ಕಿಟಕಿ(beval->balloonShell), GTK_ಕಿಟಕಿ(gui.mainwin));

    gtk_widget_ಗಣ_app_paintable(beval->balloonShell, TRUE);
    gtk_ಕಿಟಕಿ_ಗಣ_resizable(GTK_ಕಿಟಕಿ(beval->balloonShell), FALSE);
    gtk_widget_ಗಣ_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(beval->balloonShell), 4);

#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), ಖಾಲಿ);
#   ಇಲ್ಲದಿದ್ದರೆ
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), ಖಾಲಿ);
#   endif
    beval->balloonLabel = gtk_label_new(ಖಾಲಿ);

    gtk_label_ಗಣ_ಸಾಲು_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_ಗಣ_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ಎಡ);
#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,16,0)
    gtk_label_ಗಣ_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_ಗಣ_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
    GValue align_val = G_ಮೌಲ್ಯ_INIT;
    g_ಮೌಲ್ಯ_init(&align_val, G_TYPE_FLOAT);
    g_ಮೌಲ್ಯ_ಗಣ_float(&align_val, 0.5);
    g_object_ಗಣ_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_ಗಣ_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_ಮೌಲ್ಯ_unset(&align_val);
#   ಇಲ್ಲದಿದ್ದರೆ
    gtk_misc_ಗಣ_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_ಗಣ_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# ಇಲ್ಲದಿದ್ದರೆ // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    ಆದರೆ (beval->ಸಂದೇಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    ಆದರೆ (beval->ಸಂದೇಶ_CB != ಖಾಲಿ)
	s = XmStringCreateLtoR((ಅಕ್ಷರ *)beval->ಸಂದೇಶ, Xm_FONTLIST_DEFAULT_ಟ್ಯಾಗ್);
    ಇಲ್ಲದಿದ್ದರೆ
	s = XmStringCreateLocalized((ಅಕ್ಷರ *)beval->ಸಂದೇಶ);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	ಆದರೆ (fl == ಖಾಲಿ)
	{
	    XmStringFree(s);
	    ಹಿಂತಿರುಗಿ;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_ಸ್ಥಾನಾಂತರ << 1;
    h += gui.border_ಸ್ಥಾನಾಂತರ << 1;
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(beval->balloonLabel, XmNlabelString, s, ಖಾಲಿ);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_ಬೇರು + EVAL_ಸ್ಥಾನಾಂತರ_X;
    ty = beval->y_ಬೇರು + EVAL_ಸ್ಥಾನಾಂತರ_Y;
    ಆದರೆ ((tx + w) > beval->screen_ಅಗಲ)
	tx = beval->screen_ಅಗಲ - w;
    ಆದರೆ ((ty + h) > beval->screen_ಎತ್ತರ)
	ty = beval->screen_ಎತ್ತರ - h;
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    ಖಾಲಿ);
    // Set tooltip colors
    {
	Arg ವಾದಗಳು[2];

	ವಾದಗಳು[0].name = XmNbackground;
	ವಾದಗಳು[0].ಮೌಲ್ಯ = gui.tooltip_bg_pixel;
	ವಾದಗಳು[1].name = XmNforeground;
	ವಾದಗಳು[1].ಮೌಲ್ಯ = gui.tooltip_fg_pixel;
	Xt_ಗಣ_ಮೌಲ್ಯಗಳು(beval->balloonLabel, &ವಾದಗಳು[0], Xt_ಸಂಖ್ಯೆ(ವಾದಗಳು));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
undrawBalloon(BalloonEval *beval)
{
    ಆದರೆ (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cancelBalloon(BalloonEval *beval)
{
    ಆದರೆ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ಆದರೆ (beval->timerID != (XtIntervalId)ಖಾಲಿ)
    {
	Xt_Remove_ಸಮಯ_ಹೊರಹೋಗು(beval->timerID);
	beval->timerID = (XtIntervalId)ಖಾಲಿ;
    }
    beval->showState = ShS_NEUTRAL;
}


    ಸ್ಥಿರ ಶೂನ್ಯ
create_Balloon_Eval_ಕಿಟಕಿ(BalloonEval *beval)
{
    Arg		ವಾದಗಳು[12];
    ಪೂರ್ಣಾಂಕ		n;

    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNallowShellResize, True); n++;
    beval->balloonShell = XtAppCreateShell("balloonEval", "BalloonEval",
		    overrideShellWidgetClass, gui.dpy, ವಾದಗಳು, n);

    XmFontList fl;

    n = 0;
    fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNforeground, gui.tooltip_fg_pixel); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbackground, gui.tooltip_bg_pixel); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNfontList, fl); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    beval->balloonLabel = XtCreateManagedWidget("balloonLabel",
	    xmLabelWidgetClass, beval->balloonShell, ವಾದಗಳು, n);
}

# endif // !FEAT_GUI_GTK
#ಆದರೆ_ಅಂತ್ಯ // !FEAT_GUI_MSWIN

#ಆದರೆ_ಅಂತ್ಯ // FEAT_BEVAL_GUI
