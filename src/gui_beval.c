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

#ಯದಿ defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#ಯದಿ !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_VERSION(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  ಅನ್ಯಥಾ
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# ಅನ್ಯಥಾ
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
ಸ್ಥಿರ BalloonEval *current_beval = ರಿಕ್ತ;
# endif

# ifdef FEAT_GUI_GTK
ಸ್ಥಿರ ಶೂನ್ಯ addEventHandler(GtkWidget *, BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
ಸ್ಥಿರ gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  ಅನ್ಯಥಾ
ಸ್ಥಿರ gint target_event_cb(GtkWidget *, GdkEvent *, gpointer);
ಸ್ಥಿರ gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
ಸ್ಥಿರ ಶೂನ್ಯ pointer_event(BalloonEval *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, unsigned);
ಸ್ಥಿರ ಶೂನ್ಯ ಕೀಲಿ_event(BalloonEval *, unsigned, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   ಯದಿ GTK_CHECK_VERSION(3,0,0)
ಸ್ಥಿರ gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   ಅನ್ಯಥಾ
ಸ್ಥಿರ gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ಶೂನ್ಯ addEventHandler(Widget, BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ removeEventHandler(BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
ಸ್ಥಿರ ಶೂನ್ಯ pointerEvent(BalloonEval *, XEvent *);
ಸ್ಥಿರ ಶೂನ್ಯ timerRoutine(XtPointer, XtIntervalId *);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ cancelBalloon(BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ requestBalloon(BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ drawBalloon(BalloonEval *);
ಸ್ಥಿರ ಶೂನ್ಯ undrawBalloon(BalloonEval *beval);
ಸ್ಥಿರ ಶೂನ್ಯ createBalloonEvalWindow(BalloonEval *);

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
    ಶೂನ್ಯ	*target,
    char_u	*mesg,
    ಶೂನ್ಯ	(*mesgCB)(BalloonEval *, ಪೂರ್ಣಾಙ್ಕ),
    ಶೂನ್ಯ	*clientData)
{
# ifndef FEAT_GUI_GTK
    ವರ್ಣ	*display_name;	    // get from gui.dpy
    ಪೂರ್ಣಾಙ್ಕ		screen_ಅಂಕೆ;
    ವರ್ಣ	*p;
# endif
    BalloonEval	*beval;

    ಯದಿ (mesg != ರಿಕ್ತ && mesgCB != ರಿಕ್ತ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ಯದಿ (beval != ರಿಕ್ತ)
    {
# ifdef FEAT_GUI_GTK
	beval->target = GTK_WIDGET(target);
# ಅನ್ಯಥಾ
	beval->target = (Widget)target;
	beval->appContext = XtWidgetToApplicationContext((Widget)target);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->ಸಂದೇಶ);
	beval->ಸಂದೇಶ = mesg == ರಿಕ್ತ ? ರಿಕ್ತ : vim_strsave(mesg);
	beval->ಸಂದೇಶ_CB = mesgCB;
	beval->clientData = clientData;

	/*
	 * Set up event handler which will keep its eyes on the pointer,
	 * and when the pointer rests in a certain spot for a given time
	 * interval, show the beval.
	 */
	addEventHandler(beval->target, beval);
	createBalloonEvalWindow(beval);

# ifndef FEAT_GUI_GTK
	/*
	 * Now create and save the screen width and height. Used in drawing.
	 */
	display_name = DisplayString(gui.dpy);
	p = strrchr(display_name, '.');
	ಯದಿ (p != ರಿಕ್ತ)
	    screen_ಅಂಕೆ = atoi(++p);
	ಅನ್ಯಥಾ
	    screen_ಅಂಕೆ = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_ಅಂಕೆ);
	beval->screen_height = DisplayHeight(gui.dpy, screen_ಅಂಕೆ);
# endif
    }

    ಪ್ರತ್ಯಾಗಮ beval;
}

# ಯದಿ defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
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
#   ಅನ್ಯಥಾ
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  ಅನ್ಯಥಾ
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    ಯದಿ (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->ಸಂದೇಶ);
    vim_free(beval);
}
# endif

    ಶೂನ್ಯ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ಯದಿ (beval != ರಿಕ್ತ)
	addEventHandler(beval->target, beval);
}

    ಶೂನ್ಯ
gui_mch_disable_beval_area(BalloonEval *beval)
{
    ಯದಿ (beval != ರಿಕ್ತ)
	removeEventHandler(beval);
}

# ಯದಿ defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ current_beval;
}
# endif
#endif // !FEAT_GUI_MSWIN

#ಯದಿ defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# ಯದಿ !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    ಶೂನ್ಯ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->ಸಂದೇಶ);
    beval->ಸಂದೇಶ = mesg == ರಿಕ್ತ ? ರಿಕ್ತ : vim_strsave(mesg);
    ಯದಿ (beval->ಸಂದೇಶ != ರಿಕ್ತ)
	drawBalloon(beval);
    ಅನ್ಯಥಾ
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#endif // FEAT_NETBEANS_INTG || FEAT_EVAL

#ಯದಿ !defined(FEAT_GUI_MSWIN)
# ಯದಿ defined(FEAT_BEVAL_TIP)
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
addEventHandler(GtkWidget *target, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_set_data(G_OBJECT(target), "beval", controller);
    gtk_widget_add_controller(target, controller);
#  ಅನ್ಯಥಾ
    /*
     * Connect to the generic "event" signal instead of the individual
     * signals for each event type, because the former is emitted earlier.
     * This allows us to catch events independently of the signal handlers
     * in gui_gtk_x11.c.
     */
    g_signal_connect(G_OBJECT(target), "event",
		     G_CALLBACK(target_event_cb),
		     beval);
    /*
     * Nasty:  Key press events go to the main window thus the drawing area
     * will never see them.  This means we have to connect to the main window
     * as well in order to catch those events.
     */
    ಯದಿ (gtk_socket_id == 0 && gui.mainwin != ರಿಕ್ತ
	    && gtk_widget_is_ancestor(target, gui.mainwin))
    {
	gtk_widget_add_events(gui.mainwin,
			      GDK_LEAVE_NOTIFY_MASK);

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
    GtkWidget		*target = beval->target;
    GtkEventController	*controller;

    controller = g_object_get_data(G_OBJECT(target), "beval");
    ಯದಿ (controller != ರಿಕ್ತ)
    {
	gtk_widget_remove_controller(target, controller);
	g_object_set_data(G_OBJECT(target), "beval", ರಿಕ್ತ);
    }
#  ಅನ್ಯಥಾ
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->target),
					 FUNC2GENERIC(target_event_cb),
					 beval);

    ಯದಿ (gtk_socket_id == 0 && gui.mainwin != ರಿಕ್ತ
	    && gtk_widget_is_ancestor(beval->target, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    ಸ್ಥಿರ gboolean
event_cb(GtkEventController *controller UNUSED, GdkEvent *event, BalloonEval *beval)
{
    ಪೂರ್ಣಾಙ್ಕ type = gdk_event_get_event_type(event);

    ವಿಕಲ್ಪ (type)
    {
	ಪಕ್ಷ GDK_ENTER_NOTIFY:
	ಪಕ್ಷ GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*ಬೇರು;
	    graphene_point_t	p;
	    graphene_point_t	result;
	    ದ್ವಿಗುಣ		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    ಬೇರು = GTK_WIDGET(gtk_widget_get_ಬೇರು(beval->target));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    ಯದಿ (gtk_widget_compute_point(ಬೇರು, beval->target, &p, &result))
	    {
		x = result.x;
		y = result.y;
	    }

	    pointer_event(beval, (ಪೂರ್ಣಾಙ್ಕ)x, (ಪೂರ್ಣಾಙ್ಕ)y,
		    gdk_event_get_modifier_state(event));
	    ಭಙ್ಗ;
	}
	ಪಕ್ಷ GDK_LEAVE_NOTIFY:
	    ಯದಿ (gdk_crossing_event_get_mode(event) == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    ಭಙ್ಗ;
	ಪಕ್ಷ GDK_BUTTON_PRESS:
	ಪಕ್ಷ GDK_SCROLL:
	    cancelBalloon(beval);
	    ಭಙ್ಗ;
	ಪಕ್ಷ GDK_ಕೀಲಿ_PRESS:
	ಪಕ್ಷ GDK_ಕೀಲಿ_RELEASE:
	    ಕೀಲಿ_event(beval, gdk_ಕೀಲಿ_event_get_keyval(event),
		    type == GDK_ಕೀಲಿ_PRESS);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }

    // Always pass through events?
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#  ಅನ್ಯಥಾ
    ಸ್ಥಿರ gint
target_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    ವಿಕಲ್ಪ (event->type)
    {
	ಪಕ್ಷ GDK_ENTER_NOTIFY:
	    pointer_event(beval, (ಪೂರ್ಣಾಙ್ಕ)event->crossing.x,
				 (ಪೂರ್ಣಾಙ್ಕ)event->crossing.y,
				 event->crossing.state);
	    ಭಙ್ಗ;
	ಪಕ್ಷ GDK_MOTION_NOTIFY:
	    ಯದಿ (event->motion.is_hint)
	    {
		ಪೂರ್ಣಾಙ್ಕ		x;
		ಪೂರ್ಣಾಙ್ಕ		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   ಯದಿ GTK_CHECK_VERSION(3,0,0)
		{
		    GdkWindow * ಧ್ರುವ win = gtk_widget_get_window(widget);
		    GdkDisplay * ಧ್ರುವ dpy = gdk_window_get_display(win);
#    ಯದಿ GTK_CHECK_VERSION(3,20,0)
		    GdkSeat * ಧ್ರುವ seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * ಧ್ರುವ dev = gdk_seat_get_pointer(seat);
#    ಅನ್ಯಥಾ
		    GdkDeviceManager * ಧ್ರುವ mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * ಧ್ರುವ dev = gdk_device_manager_get_client_pointer(mngr);
#    endif
		    gdk_window_get_device_position(win, dev , &x, &y, &state);
		}
#   ಅನ್ಯಥಾ
		gdk_window_get_pointer(widget->window, &x, &y, &state);
#   endif
		pointer_event(beval, x, y, (unsigned ಪೂರ್ಣಾಙ್ಕ)state);
	    }
	    ಅನ್ಯಥಾ
	    {
		pointer_event(beval, (ಪೂರ್ಣಾಙ್ಕ)event->motion.x,
				     (ಪೂರ್ಣಾಙ್ಕ)event->motion.y,
				     event->motion.state);
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    ಯದಿ (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    ಭಙ್ಗ;
	ಪಕ್ಷ GDK_BUTTON_PRESS:
	ಪಕ್ಷ GDK_SCROLL:
	    cancelBalloon(beval);
	    ಭಙ್ಗ;
	ಪಕ್ಷ GDK_ಕೀಲಿ_PRESS:
	    ಕೀಲಿ_event(beval, event->ಕೀಲಿ.keyval, TRUE);
	    ಭಙ್ಗ;
	ಪಕ್ಷ GDK_ಕೀಲಿ_RELEASE:
	    ಕೀಲಿ_event(beval, event->ಕೀಲಿ.keyval, FALSE);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE; // continue emission
}

    ಸ್ಥಿರ gint
mainwin_event_cb(GtkWidget *widget UNUSED, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    ವಿಕಲ್ಪ (event->type)
    {
	ಪಕ್ಷ GDK_ಕೀಲಿ_PRESS:
	    ಕೀಲಿ_event(beval, event->ಕೀಲಿ.keyval, TRUE);
	    ಭಙ್ಗ;
	ಪಕ್ಷ GDK_ಕೀಲಿ_RELEASE:
	    ಕೀಲಿ_event(beval, event->ಕೀಲಿ.keyval, FALSE);
	    ಭಙ್ಗ;
	ಪಕ್ಷ GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    ಯದಿ (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE; // continue emission
}
#  endif

    ಸ್ಥಿರ ಶೂನ್ಯ
pointer_event(BalloonEval *beval, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, unsigned state)
{
    ಪೂರ್ಣಾಙ್ಕ distance;

    distance = ABS(x - beval->x) + ABS(y - beval->y);

    ಯದಿ (distance <= 4)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    ಯದಿ (!(state & ((ಪೂರ್ಣಾಙ್ಕ)GDK_BUTTON1_MASK | (ಪೂರ್ಣಾಙ್ಕ)GDK_BUTTON2_MASK
		    | (ಪೂರ್ಣಾಙ್ಕ)GDK_BUTTON3_MASK)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	ಯದಿ (state & (ಪೂರ್ಣಾಙ್ಕ)GDK_ALT_MASK)
#  ಅನ್ಯಥಾ
	ಯದಿ (state & (ಪೂರ್ಣಾಙ್ಕ)GDK_MOD1_MASK)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    ಯದಿ (beval->ಸಂದೇಶ_CB != ರಿಕ್ತ)
	    {
		beval->showState = ShS_PENDING;
		(*beval->ಸಂದೇಶ_CB)(beval, state);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    beval->timerID = g_timeout_add((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೀಲಿ_event(BalloonEval *beval, unsigned keyval, ಪೂರ್ಣಾಙ್ಕ is_keypress)
{
    ಯದಿ (beval->showState == ShS_SHOWING && beval->ಸಂದೇಶ_CB != ರಿಕ್ತ)
    {
	ವಿಕಲ್ಪ (keyval)
	{
#  ifdef USE_GTK4
	    ಪಕ್ಷ GDK_ಕೀಲಿ_Shift_L:
	    ಪಕ್ಷ GDK_ಕೀಲಿ_Shift_R:
#  ಅನ್ಯಥಾ
	    ಪಕ್ಷ GDK_Shift_L:
	    ಪಕ್ಷ GDK_Shift_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->ಸಂದೇಶ_CB)(beval, (is_keypress)
						   ? (ಪೂರ್ಣಾಙ್ಕ)GDK_SHIFT_MASK : 0);
		ಭಙ್ಗ;
#  ifdef USE_GTK4
	    ಪಕ್ಷ GDK_ಕೀಲಿ_Control_L:
	    ಪಕ್ಷ GDK_ಕೀಲಿ_Control_R:
#  ಅನ್ಯಥಾ
	    ಪಕ್ಷ GDK_Control_L:
	    ಪಕ್ಷ GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->ಸಂದೇಶ_CB)(beval, (is_keypress)
						 ? (ಪೂರ್ಣಾಙ್ಕ)GDK_CONTROL_MASK : 0);
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		ಯದಿ (is_keypress)
		    cancelBalloon(beval);
		ಭಙ್ಗ;
	}
    }
    ಅನ್ಯಥಾ
	cancelBalloon(beval);
}

    ಸ್ಥಿರ gboolean
timeout_cb(gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    beval->timerID = 0;
    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);

    ಪ್ರತ್ಯಾಗಮ FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   ಯದಿ GTK_CHECK_VERSION(3,0,0)
    ಸ್ಥಿರ gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 data UNUSED)
{
    GtkStyleContext *context = ರಿಕ್ತ;
    gint width = -1, height = -1;

    ಯದಿ (widget == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    context = gtk_widget_get_style_context(widget);
    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);

    gtk_style_context_save(context);

    gtk_style_context_add_class(context, "tooltip");
    gtk_style_context_set_state(context, GTK_STATE_FLAG_NORMAL);

    cairo_save(cr);
    gtk_render_frame(context, cr, 0, 0, width, height);
    gtk_render_background(context, cr, 0, 0, width, height);
    cairo_restore(cr);

    gtk_style_context_restore(context);

    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#   ಅನ್ಯಥಾ
    ಸ್ಥಿರ gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer data UNUSED)
{
    gtk_paint_flat_box(widget->style, widget->window,
		       GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    ಪ್ರತ್ಯಾಗಮ FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# ಅನ್ಯಥಾ // !FEAT_GUI_GTK

    ಸ್ಥಿರ ಶೂನ್ಯ
addEventHandler(Widget target, BalloonEval *beval)
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | ಕೀಲಿ_Press_Mask |
			ಕೀಲಿ_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | ಕೀಲಿ_Press_Mask |
			ಕೀಲಿ_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
pointerEventEH(
    Widget	w UNUSED,
    XtPointer	client_data,
    XEvent	*event,
    Boolean	*unused UNUSED)
{
    BalloonEval *beval = (BalloonEval *)client_data;
    pointerEvent(beval, event);
}


/*
 * The real event handler. Called by pointerEventEH() whenever an event we are
 * interested in occurs.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
pointerEvent(BalloonEval *beval, XEvent *event)
{
    Position	distance;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    ವಿಕಲ್ಪ (event->type)
    {
	ಪಕ್ಷ EnterNotify:
	ಪಕ್ಷ MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    ಯದಿ (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    ಯದಿ (delta < 0)
		delta = -delta;
	    distance += delta;
	    ಯದಿ (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		ಯದಿ (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		ಅನ್ಯಥಾ ಯದಿ (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
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
		    ಯದಿ (beval->ಸಂದೇಶ_CB != ರಿಕ್ತ)
		    {
			beval->showState = ShS_PENDING;
			(*beval->ಸಂದೇಶ_CB)(beval, beval->state);
		    }
		}
		ಅನ್ಯಥಾ
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_ಬೇರು = event->xmotion.x_ಬೇರು;
		    beval->y_ಬೇರು = event->xmotion.y_ಬೇರು;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    ಭಙ್ಗ;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	ಪಕ್ಷ ಕೀಲಿ_Press:
	    ಯದಿ (beval->showState == ShS_SHOWING && beval->ಸಂದೇಶ_CB != ರಿಕ್ತ)
	    {
		Modifiers   modifier;
		ಕೀಲಿ_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		ಯದಿ (keysym == XK_Shift_L || keysym == XK_Shift_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ಸಂದೇಶ_CB)(beval, ShiftMask);
		}
		ಅನ್ಯಥಾ ಯದಿ (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ಸಂದೇಶ_CB)(beval, ControlMask);
		}
		ಅನ್ಯಥಾ
		    cancelBalloon(beval);
	    }
	    ಅನ್ಯಥಾ
		cancelBalloon(beval);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ಕೀಲಿ_Release:
	    ಯದಿ (beval->showState == ShS_SHOWING && beval->ಸಂದೇಶ_CB != ರಿಕ್ತ)
	    {
		Modifiers modifier;
		ಕೀಲಿ_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		ಯದಿ ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ಸಂದೇಶ_CB)(beval, 0);
		}
		ಅನ್ಯಥಾ ಯದಿ ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ಸಂದೇಶ_CB)(beval, 0);
		}
		ಅನ್ಯಥಾ
		    cancelBalloon(beval);
	    }
	    ಅನ್ಯಥಾ
		cancelBalloon(beval);
	    ಭಙ್ಗ;

	ಪಕ್ಷ LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		ಯದಿ (event->xcrossing.mode == NotifyNormal)
		    cancelBalloon(beval);
		ಭಙ್ಗ;

	ಪಕ್ಷ ButtonPress:
		cancelBalloon(beval);
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
timerRoutine(XtPointer dx, XtIntervalId *id UNUSED)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)ರಿಕ್ತ;

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
    ಯದಿ (beval->showState == ShS_PENDING)
	ಪ್ರತ್ಯಾಗಮ;

    // Determine the beval to display
    ಯದಿ (beval->ಸಂದೇಶ_CB != ರಿಕ್ತ)
    {
	beval->showState = ShS_PENDING;
	(*beval->ಸಂದೇಶ_CB)(beval, beval->state);
    }
    ಅನ್ಯಥಾ ಯದಿ (beval->ಸಂದೇಶ != ರಿಕ್ತ)
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
set_printable_label_text(GtkLabel *label, char_u *text)
{
    char_u	    *convbuf = ರಿಕ್ತ;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    unsigned ಪೂರ್ಣಾಙ್ಕ    len;
    ಪೂರ್ಣಾಙ್ಕ		    charlen;
    ಪೂರ್ಣಾಙ್ಕ		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    ಯದಿ (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, text, ರಿಕ್ತ);
	ಯದಿ (convbuf != ರಿಕ್ತ)
	    text = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    ಪ್ರತಿ (p = text; *p != NUL; p += charlen)
    {
	ಯದಿ ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	ಅನ್ಯಥಾ
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    ಯದಿ (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    ಯದಿ (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    ಅನ್ಯಥಾ ಯದಿ (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    ಅನ್ಯಥಾ
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    ಯದಿ (buf != ರಿಕ್ತ)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  ಯದಿ GTK_CHECK_VERSION(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   ಯದಿ PANGO_VERSION_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  ಅನ್ಯಥಾ
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != ರಿಕ್ತ) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	ಯದಿ (pixel != INVALCOLOR)
#  ಯದಿ GTK_CHECK_VERSION(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  ಅನ್ಯಥಾ
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (unsigned long)pixel, &color);
#  endif

	pdest = buf;
	p = text;
	ಯಾವತ್ (*p != NUL)
	{
	    // Be quick for ASCII
	    ಯದಿ ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    ಅನ್ಯಥಾ
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		ಯದಿ (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		ಯದಿ (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    ಪೂರ್ಣಾಙ್ಕ	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    ಯದಿ (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    ಅನ್ಯಥಾ
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    ಯದಿ (pixel != INVALCOLOR)
		    {
#  ಯದಿ GTK_CHECK_VERSION(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   ಯದಿ PANGO_VERSION_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  ಅನ್ಯಥಾ
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->start_index = pdest - buf;
			attr->end_index   = pdest - buf + outlen;
			pango_attr_list_ಸೇರಿಸು(attr_list, attr);
#  ಯದಿ GTK_CHECK_VERSION(3,0,0)
#   ಯದಿ PANGO_VERSION_CHECK(1,38,0)
			attr_alpha->start_index = pdest - buf;
			attr_alpha->end_index   = pdest - buf + outlen;
			pango_attr_list_ಸೇರಿಸು(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		ಅನ್ಯಥಾ
		{
		    ಕುರು
			*pdest++ = *p++;
		    ಯಾವತ್ (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_set_text(label, (ಧ್ರುವ ವರ್ಣ *)buf);
    vim_free(buf);

    gtk_label_set_attributes(label, attr_list);
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
    ಪೂರ್ಣಾಙ್ಕ		    natural_width = 0;

    ಯದಿ (beval->ಸಂದೇಶ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->ಸಂದೇಶ);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    ರಿಕ್ತ, &natural_width, ರಿಕ್ತ, ರಿಕ್ತ);

    rect.width = (natural_width > 0 ? natural_width : 1) + gui.char_width * 3;
    rect.height = gui.char_height;

    gtk_popover_set_pointing_to(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  ಅನ್ಯಥಾ
    ಸ್ಥಿರ ಶೂನ್ಯ
drawBalloon(BalloonEval *beval)
{
    ಯದಿ (beval->ಸಂದೇಶ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    GtkRequisition	requisition;
    ಪೂರ್ಣಾಙ್ಕ		screen_w;
    ಪೂರ್ಣಾಙ್ಕ		screen_h;
    ಪೂರ್ಣಾಙ್ಕ		screen_x;
    ಪೂರ್ಣಾಙ್ಕ		screen_y;
    ಪೂರ್ಣಾಙ್ಕ		x;
    ಪೂರ್ಣಾಙ್ಕ		y;
    ಪೂರ್ಣಾಙ್ಕ		x_offset = EVAL_OFFSET_X;
    ಪೂರ್ಣಾಙ್ಕ		y_offset = EVAL_OFFSET_Y;
    PangoLayout	*layout;

#   ಯದಿ !GTK_CHECK_VERSION(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->target);
    gtk_window_set_screen(GTK_WINDOW(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_of_win(beval->target, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   ಯದಿ !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->ಸಂದೇಶ);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_WORD_CHAR
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
#   ಅನ್ಯಥಾ
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
#   endif
    pango_layout_set_width(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.ಅಂಕೆ_cols * gui.char_width,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   ಯದಿ GTK_CHECK_VERSION(3,0,0)
    gtk_widget_get_preferred_size(beval->balloonShell, &requisition, ರಿಕ್ತ);
#   ಅನ್ಯಥಾ
    gtk_widget_size_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_window_get_origin(gtk_widget_get_window(beval->target), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    ಯದಿ (x + x_offset + requisition.width > screen_x + screen_w)
	y_offset += 15;
    ಯದಿ (y + y_offset + requisition.height > screen_y + screen_h)
	y_offset = -requisition.height - EVAL_OFFSET_Y;

    // Sanitize values
    x = CLAMP(x + x_offset, 0,
	    MAX(0, screen_x + screen_w - requisition.width));
    y = CLAMP(y + y_offset, 0,
	    MAX(0, screen_y + screen_h - requisition.height));

    // Show the balloon
#   ಯದಿ GTK_CHECK_VERSION(3,0,0)
    gtk_window_move(GTK_WINDOW(beval->balloonShell), x, y);
#   ಅನ್ಯಥಾ
    gtk_widget_set_uposition(beval->balloonShell, x, y);
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
    ಯದಿ (beval->balloonShell != ರಿಕ್ತ)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  ಅನ್ಯಥಾ
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cancelBalloon(BalloonEval *beval)
{
    ಯದಿ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ಯದಿ (beval->timerID != 0)
    {
	g_source_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    ಸ್ಥಿರ ಶೂನ್ಯ
createBalloonEvalWindow(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_set_parent(beval->balloonShell, beval->target);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_set_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(ರಿಕ್ತ);

    gtk_label_set_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ಎಡ);
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_set_child(GTK_POPOVER(popover), beval->balloonLabel);
}
#  ಅನ್ಯಥಾ
    ಸ್ಥಿರ ಶೂನ್ಯ
createBalloonEvalWindow(BalloonEval *beval)
{
    beval->balloonShell = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_transient_for(GTK_WINDOW(beval->balloonShell), GTK_WINDOW(gui.mainwin));

    gtk_widget_set_app_paintable(beval->balloonShell, TRUE);
    gtk_window_set_resizable(GTK_WINDOW(beval->balloonShell), FALSE);
    gtk_widget_set_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_set_border_width(GTK_CONTAINER(beval->balloonShell), 4);

#   ಯದಿ GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), ರಿಕ್ತ);
#   ಅನ್ಯಥಾ
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), ರಿಕ್ತ);
#   endif
    beval->balloonLabel = gtk_label_new(ರಿಕ್ತ);

    gtk_label_set_ಸಾಲು_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ಎಡ);
#   ಯದಿ GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_VERSION(3,14,0)
    GValue align_val = G_ಮೌಲ್ಯ_INIT;
    g_ಮೌಲ್ಯ_init(&align_val, G_TYPE_FLOAT);
    g_ಮೌಲ್ಯ_set_float(&align_val, 0.5);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_ಮೌಲ್ಯ_unset(&align_val);
#   ಅನ್ಯಥಾ
    gtk_misc_set_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_set_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_add(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# ಅನ್ಯಥಾ // !FEAT_GUI_GTK

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

    ಯದಿ (beval->ಸಂದೇಶ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    ಯದಿ (beval->ಸಂದೇಶ_CB != ರಿಕ್ತ)
	s = XmStringCreateLtoR((ವರ್ಣ *)beval->ಸಂದೇಶ, XmFONTLIST_DEFAULT_TAG);
    ಅನ್ಯಥಾ
	s = XmStringCreateLocalized((ವರ್ಣ *)beval->ಸಂದೇಶ);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	ಯದಿ (fl == ರಿಕ್ತ)
	{
	    XmStringFree(s);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_offset << 1;
    h += gui.border_offset << 1;
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(beval->balloonLabel, XmNlabelString, s, ರಿಕ್ತ);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_ಬೇರು + EVAL_OFFSET_X;
    ty = beval->y_ಬೇರು + EVAL_OFFSET_Y;
    ಯದಿ ((tx + w) > beval->screen_width)
	tx = beval->screen_width - w;
    ಯದಿ ((ty + h) > beval->screen_height)
	ty = beval->screen_height - h;
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    ರಿಕ್ತ);
    // Set tooltip colors
    {
	Arg args[2];

	args[0].name = XmNbackground;
	args[0].ಮೌಲ್ಯ = gui.tooltip_bg_pixel;
	args[1].name = XmNforeground;
	args[1].ಮೌಲ್ಯ = gui.tooltip_fg_pixel;
	Xt_Set_ಮೌಲ್ಯಗಳು(beval->balloonLabel, &args[0], Xt_ಸಂಖ್ಯೆ(args));
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
    ಯದಿ (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cancelBalloon(BalloonEval *beval)
{
    ಯದಿ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ಯದಿ (beval->timerID != (XtIntervalId)ರಿಕ್ತ)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = (XtIntervalId)ರಿಕ್ತ;
    }
    beval->showState = ShS_NEUTRAL;
}


    ಸ್ಥಿರ ಶೂನ್ಯ
createBalloonEvalWindow(BalloonEval *beval)
{
    Arg		args[12];
    ಪೂರ್ಣಾಙ್ಕ		n;

    n = 0;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    beval->balloonShell = XtAppCreateShell("balloonEval", "BalloonEval",
		    overrideShellWidgetClass, gui.dpy, args, n);

    XmFontList fl;

    n = 0;
    fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
    XtSetArg(args[n], XmNforeground, gui.tooltip_fg_pixel); n++;
    XtSetArg(args[n], XmNbackground, gui.tooltip_bg_pixel); n++;
    XtSetArg(args[n], XmNfontList, fl); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    beval->balloonLabel = XtCreateManagedWidget("balloonLabel",
	    xmLabelWidgetClass, beval->balloonShell, args, n);
}

# endif // !FEAT_GUI_GTK
#endif // !FEAT_GUI_MSWIN

#endif // FEAT_BEVAL_GUI
