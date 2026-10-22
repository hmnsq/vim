/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#ਜੇ !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  ਨਹੀਂਤਾਂ
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# ਨਹੀਂਤਾਂ
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
ਬਾਹਰੀ Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
ਸਥਿਰ BalloonEval *current_beval = ਨਿਰਾ;
# endif

# ifdef FEAT_GUI_GTK
ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_Event_Handler(GtkWidget *, BalloonEval *);
ਸਥਿਰ ਖਾਲੀ removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
ਸਥਿਰ gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  ਨਹੀਂਤਾਂ
ਸਥਿਰ gint ਨਿਸ਼ਾਨਾ_event_cb(GtkWidget *, GdkEvent *, gpointer);
ਸਥਿਰ gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
ਸਥਿਰ ਖਾਲੀ ਸੂਚਕ_event(BalloonEval *, ਪੂਰਨ, ਪੂਰਨ, ਬਿਨਾਨਿਸ਼ਾਨੀ);
ਸਥਿਰ ਖਾਲੀ ਕੁੰਜੀ_event(BalloonEval *, ਬਿਨਾਨਿਸ਼ਾਨੀ, ਪੂਰਨ);
ਸਥਿਰ gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
ਸਥਿਰ gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   ਨਹੀਂਤਾਂ
ਸਥਿਰ gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_Event_Handler(Widget, BalloonEval *);
ਸਥਿਰ ਖਾਲੀ removeEventHandler(BalloonEval *);
ਸਥਿਰ ਖਾਲੀ ਸੂਚਕ_Event_EH(Widget, Xt_ਸੂਚਕ, XEvent *, Boolean *);
ਸਥਿਰ ਖਾਲੀ ਸੂਚਕ_Event(BalloonEval *, XEvent *);
ਸਥਿਰ ਖਾਲੀ timerRoutine(Xt_ਸੂਚਕ, XtIntervalId *);
# endif
ਸਥਿਰ ਖਾਲੀ cancelBalloon(BalloonEval *);
ਸਥਿਰ ਖਾਲੀ requestBalloon(BalloonEval *);
ਸਥਿਰ ਖਾਲੀ drawBalloon(BalloonEval *);
ਸਥਿਰ ਖਾਲੀ undrawBalloon(BalloonEval *beval);
ਸਥਿਰ ਖਾਲੀ create_Balloon_Eval_ਖਿੜਕੀ(BalloonEval *);

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
    ਖਾਲੀ	*ਨਿਸ਼ਾਨਾ,
    char_u	*mesg,
    ਖਾਲੀ	(*mesgCB)(BalloonEval *, ਪੂਰਨ),
    ਖਾਲੀ	*client_ਅੰਕੜੇ)
{
# ifndef FEAT_GUI_GTK
    ਅੱਖਰ	*display_name;	    // get from gui.dpy
    ਪੂਰਨ		screen_ਅੰਕ;
    ਅੱਖਰ	*p;
# endif
    BalloonEval	*beval;

    ਜੇ (mesg != ਨਿਰਾ && mesgCB != ਨਿਰਾ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_ਸੁਨੇਹਾ_and_callback);
	ਵਾਪਸ ਨਿਰਾ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ਜੇ (beval != ਨਿਰਾ)
    {
# ifdef FEAT_GUI_GTK
	beval->ਨਿਸ਼ਾਨਾ = GTK_WIDGET(ਨਿਸ਼ਾਨਾ);
# ਨਹੀਂਤਾਂ
	beval->ਨਿਸ਼ਾਨਾ = (Widget)ਨਿਸ਼ਾਨਾ;
	beval->appContext = Xt_Widget_ਮੰਜ਼ਿਲ_Application_Context((Widget)ਨਿਸ਼ਾਨਾ);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->ਸੁਨੇਹਾ);
	beval->ਸੁਨੇਹਾ = mesg == ਨਿਰਾ ? ਨਿਰਾ : vim_strsave(mesg);
	beval->ਸੁਨੇਹਾ_CB = mesgCB;
	beval->client_ਅੰਕੜੇ = client_ਅੰਕੜੇ;

	/*
	 * Set up event handler which will keep its eyes on the pointer,
	 * and when the pointer rests in a certain spot for a given time
	 * interval, show the beval.
	 */
	ਜੋੜੋ_Event_Handler(beval->ਨਿਸ਼ਾਨਾ, beval);
	create_Balloon_Eval_ਖਿੜਕੀ(beval);

# ifndef FEAT_GUI_GTK
	/*
	 * Now create and save the screen width and height. Used in drawing.
	 */
	display_name = DisplayString(gui.dpy);
	p = strrchr(display_name, '.');
	ਜੇ (p != ਨਿਰਾ)
	    screen_ਅੰਕ = atoi(++p);
	ਨਹੀਂਤਾਂ
	    screen_ਅੰਕ = 0;
	beval->screen_ਚੌੜਾਈ = Display_ਚੌੜਾਈ(gui.dpy, screen_ਅੰਕ);
	beval->screen_ਉਚਾਈ = Display_ਉਚਾਈ(gui.dpy, screen_ਅੰਕ);
# endif
    }

    ਵਾਪਸ beval;
}

# ਜੇ defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    ਖਾਲੀ
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   ਨਹੀਂਤਾਂ
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  ਨਹੀਂਤਾਂ
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    ਜੇ (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->ਸੁਨੇਹਾ);
    vim_free(beval);
}
# endif

    ਖਾਲੀ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ਜੇ (beval != ਨਿਰਾ)
	ਜੋੜੋ_Event_Handler(beval->ਨਿਸ਼ਾਨਾ, beval);
}

    ਖਾਲੀ
gui_mch_disable_beval_area(BalloonEval *beval)
{
    ਜੇ (beval != ਨਿਰਾ)
	removeEventHandler(beval);
}

# ਜੇ defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(ਖਾਲੀ)
{
    ਵਾਪਸ current_beval;
}
# endif
#ਜੇ_ਅੰਤ // !FEAT_GUI_MSWIN

#ਜੇ defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# ਜੇ !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    ਖਾਲੀ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->ਸੁਨੇਹਾ);
    beval->ਸੁਨੇਹਾ = mesg == ਨਿਰਾ ? ਨਿਰਾ : vim_strsave(mesg);
    ਜੇ (beval->ਸੁਨੇਹਾ != ਨਿਰਾ)
	drawBalloon(beval);
    ਨਹੀਂਤਾਂ
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#ਜੇ_ਅੰਤ // FEAT_NETBEANS_INTG || FEAT_EVAL

#ਜੇ !defined(FEAT_GUI_MSWIN)
# ਜੇ defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    ਖਾਲੀ
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->ਸੁਨੇਹਾ);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_Event_Handler(GtkWidget *ਨਿਸ਼ਾਨਾ, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(ਨਿਸ਼ਾਨਾ), "beval", controller);
    gtk_widget_ਜੋੜੋ_controller(ਨਿਸ਼ਾਨਾ, controller);
#  ਨਹੀਂਤਾਂ
    /*
     * Connect to the generic "event" signal instead of the individual
     * signals for each event type, because the former is emitted earlier.
     * This allows us to catch events independently of the signal handlers
     * in gui_gtk_x11.c.
     */
    g_signal_connect(G_OBJECT(ਨਿਸ਼ਾਨਾ), "event",
		     G_CALLBACK(ਨਿਸ਼ਾਨਾ_event_cb),
		     beval);
    /*
     * Nasty:  Key press events go to the main window thus the drawing area
     * will never see them.  This means we have to connect to the main window
     * as well in order to catch those events.
     */
    ਜੇ (gtk_socket_id == 0 && gui.mainwin != ਨਿਰਾ
	    && gtk_widget_is_ancestor(ਨਿਸ਼ਾਨਾ, gui.mainwin))
    {
	gtk_widget_ਜੋੜੋ_events(gui.mainwin,
			      GDK_LEAVE_NOTIFY_ਨਕਾਬ);

	g_signal_connect(G_OBJECT(gui.mainwin), "event",
			 G_CALLBACK(mainwin_event_cb),
			 beval);
    }
#  endif
}

    ਸਥਿਰ ਖਾਲੀ
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*ਨਿਸ਼ਾਨਾ = beval->ਨਿਸ਼ਾਨਾ;
    GtkEventController	*controller;

    controller = g_object_get_ਅੰਕੜੇ(G_OBJECT(ਨਿਸ਼ਾਨਾ), "beval");
    ਜੇ (controller != ਨਿਰਾ)
    {
	gtk_widget_remove_controller(ਨਿਸ਼ਾਨਾ, controller);
	g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(ਨਿਸ਼ਾਨਾ), "beval", ਨਿਰਾ);
    }
#  ਨਹੀਂਤਾਂ
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->ਨਿਸ਼ਾਨਾ),
					 FUNC2GENERIC(ਨਿਸ਼ਾਨਾ_event_cb),
					 beval);

    ਜੇ (gtk_socket_id == 0 && gui.mainwin != ਨਿਰਾ
	    && gtk_widget_is_ancestor(beval->ਨਿਸ਼ਾਨਾ, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    ਸਥਿਰ gboolean
event_cb(GtkEventController *controller ਅਣਵਰਤਿਆ, GdkEvent *event, BalloonEval *beval)
{
    ਪੂਰਨ type = gdk_event_get_event_type(event);

    ਚੋਣ (type)
    {
	ਹਾਲਤ GDK_ENTER_NOTIFY:
	ਹਾਲਤ GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*ਜੜ੍ਹ;
	    graphene_ਬਿੰਦੂ_t	p;
	    graphene_ਬਿੰਦੂ_t	ਨਤੀਜਾ;
	    ਦੁੱਗਣਾ		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    ਜੜ੍ਹ = GTK_WIDGET(gtk_widget_get_ਜੜ੍ਹ(beval->ਨਿਸ਼ਾਨਾ));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    ਜੇ (gtk_widget_compute_ਬਿੰਦੂ(ਜੜ੍ਹ, beval->ਨਿਸ਼ਾਨਾ, &p, &ਨਤੀਜਾ))
	    {
		x = ਨਤੀਜਾ.x;
		y = ਨਤੀਜਾ.y;
	    }

	    ਸੂਚਕ_event(beval, (ਪੂਰਨ)x, (ਪੂਰਨ)y,
		    gdk_event_get_modifier_state(event));
	    ਰੋਕੋ;
	}
	ਹਾਲਤ GDK_LEAVE_NOTIFY:
	    ਜੇ (gdk_ਪਾਰ_event_get_ਬਹੁਲਕ(event) == GDK_ਪਾਰ_ਸਾਧਾਰਨ)
		cancelBalloon(beval);
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_BUTTON_PRESS:
	ਹਾਲਤ GDK_SCROLL:
	    cancelBalloon(beval);
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_ਕੁੰਜੀ_PRESS:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_RELEASE:
	    ਕੁੰਜੀ_event(beval, gdk_ਕੁੰਜੀ_event_get_keyval(event),
		    type == GDK_ਕੁੰਜੀ_PRESS);
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }

    // Always pass through events?
    ਵਾਪਸ FALSE;
}
#  ਨਹੀਂਤਾਂ
    ਸਥਿਰ gint
ਨਿਸ਼ਾਨਾ_event_cb(GtkWidget *widget, GdkEvent *event, gpointer ਅੰਕੜੇ)
{
    BalloonEval *beval = (BalloonEval *)ਅੰਕੜੇ;

    ਚੋਣ (event->type)
    {
	ਹਾਲਤ GDK_ENTER_NOTIFY:
	    ਸੂਚਕ_event(beval, (ਪੂਰਨ)event->ਪਾਰ.x,
				 (ਪੂਰਨ)event->ਪਾਰ.y,
				 event->ਪਾਰ.state);
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_MOTION_NOTIFY:
	    ਜੇ (event->motion.is_hint)
	    {
		ਪੂਰਨ		x;
		ਪੂਰਨ		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
		{
		    Gdk_ਖਿੜਕੀ * ਅਚਲ win = gtk_widget_get_ਖਿੜਕੀ(widget);
		    GdkDisplay * ਅਚਲ dpy = gdk_ਖਿੜਕੀ_get_display(win);
#    ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,20,0)
		    GdkSeat * ਅਚਲ seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * ਅਚਲ dev = gdk_seat_get_ਸੂਚਕ(seat);
#    ਨਹੀਂਤਾਂ
		    GdkDeviceManager * ਅਚਲ mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * ਅਚਲ dev = gdk_device_manager_get_client_ਸੂਚਕ(mngr);
#    endif
		    gdk_ਖਿੜਕੀ_get_device_position(win, dev , &x, &y, &state);
		}
#   ਨਹੀਂਤਾਂ
		gdk_ਖਿੜਕੀ_get_ਸੂਚਕ(widget->ਖਿੜਕੀ_2, &x, &y, &state);
#   endif
		ਸੂਚਕ_event(beval, x, y, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)state);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਸੂਚਕ_event(beval, (ਪੂਰਨ)event->motion.x,
				     (ਪੂਰਨ)event->motion.y,
				     event->motion.state);
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    ਜੇ (event->ਪਾਰ.ਬਹੁਲਕ == GDK_ਪਾਰ_ਸਾਧਾਰਨ)
		cancelBalloon(beval);
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_BUTTON_PRESS:
	ਹਾਲਤ GDK_SCROLL:
	    cancelBalloon(beval);
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_ਕੁੰਜੀ_PRESS:
	    ਕੁੰਜੀ_event(beval, event->ਕੁੰਜੀ.keyval, TRUE);
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_ਕੁੰਜੀ_RELEASE:
	    ਕੁੰਜੀ_event(beval, event->ਕੁੰਜੀ.keyval, FALSE);
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }

    ਵਾਪਸ FALSE; // continue emission
}

    ਸਥਿਰ gint
mainwin_event_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, GdkEvent *event, gpointer ਅੰਕੜੇ)
{
    BalloonEval *beval = (BalloonEval *)ਅੰਕੜੇ;

    ਚੋਣ (event->type)
    {
	ਹਾਲਤ GDK_ਕੁੰਜੀ_PRESS:
	    ਕੁੰਜੀ_event(beval, event->ਕੁੰਜੀ.keyval, TRUE);
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_ਕੁੰਜੀ_RELEASE:
	    ਕੁੰਜੀ_event(beval, event->ਕੁੰਜੀ.keyval, FALSE);
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    ਜੇ (event->ਪਾਰ.ਬਹੁਲਕ == GDK_ਪਾਰ_ਸਾਧਾਰਨ)
		cancelBalloon(beval);
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }

    ਵਾਪਸ FALSE; // continue emission
}
#  endif

    ਸਥਿਰ ਖਾਲੀ
ਸੂਚਕ_event(BalloonEval *beval, ਪੂਰਨ x, ਪੂਰਨ y, ਬਿਨਾਨਿਸ਼ਾਨੀ state)
{
    ਪੂਰਨ ਦੂਰੀ;

    ਦੂਰੀ = ABS(x - beval->x) + ABS(y - beval->y);

    ਜੇ (ਦੂਰੀ <= 4)
	ਵਾਪਸ;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    ਜੇ (!(state & ((ਪੂਰਨ)GDK_BUTTON1_ਨਕਾਬ | (ਪੂਰਨ)GDK_BUTTON2_ਨਕਾਬ
		    | (ਪੂਰਨ)GDK_BUTTON3_ਨਕਾਬ)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	ਜੇ (state & (ਪੂਰਨ)GDK_ALT_ਨਕਾਬ)
#  ਨਹੀਂਤਾਂ
	ਜੇ (state & (ਪੂਰਨ)GDK_MOD1_ਨਕਾਬ)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    ਜੇ (beval->ਸੁਨੇਹਾ_CB != ਨਿਰਾ)
	    {
		beval->showState = ShS_PENDING;
		(*beval->ਸੁਨੇਹਾ_CB)(beval, state);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    beval->timerID = g_timeout_ਜੋੜੋ((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਕੁੰਜੀ_event(BalloonEval *beval, ਬਿਨਾਨਿਸ਼ਾਨੀ keyval, ਪੂਰਨ is_keypress)
{
    ਜੇ (beval->showState == ShS_SHOWING && beval->ਸੁਨੇਹਾ_CB != ਨਿਰਾ)
    {
	ਚੋਣ (keyval)
	{
#  ifdef USE_GTK4
	    ਹਾਲਤ GDK_ਕੁੰਜੀ_ਖਿਸਕਾਓ_L:
	    ਹਾਲਤ GDK_ਕੁੰਜੀ_ਖਿਸਕਾਓ_R:
#  ਨਹੀਂਤਾਂ
	    ਹਾਲਤ GDK_ਖਿਸਕਾਓ_L:
	    ਹਾਲਤ GDK_ਖਿਸਕਾਓ_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->ਸੁਨੇਹਾ_CB)(beval, (is_keypress)
						   ? (ਪੂਰਨ)GDK_ਖਿਸਕਾਓ_ਨਕਾਬ : 0);
		ਰੋਕੋ;
#  ifdef USE_GTK4
	    ਹਾਲਤ GDK_ਕੁੰਜੀ_Control_L:
	    ਹਾਲਤ GDK_ਕੁੰਜੀ_Control_R:
#  ਨਹੀਂਤਾਂ
	    ਹਾਲਤ GDK_Control_L:
	    ਹਾਲਤ GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->ਸੁਨੇਹਾ_CB)(beval, (is_keypress)
						 ? (ਪੂਰਨ)GDK_CONTROL_ਨਕਾਬ : 0);
		ਰੋਕੋ;
	    ਮੂਲ:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		ਜੇ (is_keypress)
		    cancelBalloon(beval);
		ਰੋਕੋ;
	}
    }
    ਨਹੀਂਤਾਂ
	cancelBalloon(beval);
}

    ਸਥਿਰ gboolean
timeout_cb(gpointer ਅੰਕੜੇ)
{
    BalloonEval *beval = (BalloonEval *)ਅੰਕੜੇ;

    beval->timerID = 0;
    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);

    ਵਾਪਸ FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    GtkStyleContext *context = ਨਿਰਾ;
    gint ਚੌੜਾਈ = -1, ਉਚਾਈ = -1;

    ਜੇ (widget == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    context = gtk_widget_get_style_context(widget);
    ਚੌੜਾਈ = gtk_widget_get_allocated_ਚੌੜਾਈ(widget);
    ਉਚਾਈ = gtk_widget_get_allocated_ਉਚਾਈ(widget);

    gtk_style_context_save(context);

    gtk_style_context_ਜੋੜੋ_class(context, "tooltip");
    gtk_style_context_ਸੈੱਟ_state(context, GTK_STATE_FLAG_ਸਾਧਾਰਨ);

    cairo_save(cr);
    gtk_render_ਚੌਖਟਾ(context, cr, 0, 0, ਚੌੜਾਈ, ਉਚਾਈ);
    gtk_render_background(context, cr, 0, 0, ਚੌੜਾਈ, ਉਚਾਈ);
    cairo_restore(cr);

    gtk_style_context_restore(context);

    ਵਾਪਸ FALSE;
}
#   ਨਹੀਂਤਾਂ
    ਸਥਿਰ gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    gtk_paint_flat_box(widget->style, widget->ਖਿੜਕੀ_2,
		       GTK_STATE_ਸਾਧਾਰਨ, GTK_SHADOW_ਬਾਹਰਲਾ,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    ਵਾਪਸ FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# ਨਹੀਂਤਾਂ // !FEAT_GUI_GTK

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_Event_Handler(Widget ਨਿਸ਼ਾਨਾ, BalloonEval *beval)
{
    Xt_ਜੋੜੋ_Event_Handler(ਨਿਸ਼ਾਨਾ,
			ਸੂਚਕ_Motion_ਨਕਾਬ | Enter_ਖਿੜਕੀ_ਨਕਾਬ |
			Leave_ਖਿੜਕੀ_ਨਕਾਬ | Button_Press_ਨਕਾਬ | ਕੁੰਜੀ_Press_ਨਕਾਬ |
			ਕੁੰਜੀ_Release_ਨਕਾਬ,
			False,
			ਸੂਚਕ_Event_EH, (Xt_ਸੂਚਕ)beval);
}

    ਸਥਿਰ ਖਾਲੀ
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->ਨਿਸ਼ਾਨਾ,
			ਸੂਚਕ_Motion_ਨਕਾਬ | Enter_ਖਿੜਕੀ_ਨਕਾਬ |
			Leave_ਖਿੜਕੀ_ਨਕਾਬ | Button_Press_ਨਕਾਬ | ਕੁੰਜੀ_Press_ਨਕਾਬ |
			ਕੁੰਜੀ_Release_ਨਕਾਬ,
			False,
			ਸੂਚਕ_Event_EH, (Xt_ਸੂਚਕ)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੂਚਕ_Event_EH(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2,
    XEvent	*event,
    Boolean	*ਅਣਵਰਤਿਆ_2 ਅਣਵਰਤਿਆ)
{
    BalloonEval *beval = (BalloonEval *)client_ਅੰਕੜੇ_2;
    ਸੂਚਕ_Event(beval, event);
}


/*
 * The real event handler. Called by pointerEventEH() whenever an event we are
 * interested in occurs.
 */

    ਸਥਿਰ ਖਾਲੀ
ਸੂਚਕ_Event(BalloonEval *beval, XEvent *event)
{
    Position	ਦੂਰੀ;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    ਚੋਣ (event->type)
    {
	ਹਾਲਤ EnterNotify:
	ਹਾਲਤ MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    ਜੇ (delta < 0)
		delta = -delta;
	    ਦੂਰੀ = delta;
	    delta = event->xmotion.y - beval->y;
	    ਜੇ (delta < 0)
		delta = -delta;
	    ਦੂਰੀ += delta;
	    ਜੇ (ਦੂਰੀ > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		ਜੇ (beval->state & (Button1_ਨਕਾਬ|Button2_ਨਕਾਬ|Button3_ਨਕਾਬ))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		ਨਹੀਂਤਾਂ ਜੇ (beval->state & (Mod1_ਨਕਾਬ|Mod2_ਨਕਾਬ|Mod3_ਨਕਾਬ))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_ਜੜ੍ਹ = event->xmotion.x_ਜੜ੍ਹ;
		    beval->y_ਜੜ੍ਹ = event->xmotion.y_ਜੜ੍ਹ;
		    cancelBalloon(beval);
		    ਜੇ (beval->ਸੁਨੇਹਾ_CB != ਨਿਰਾ)
		    {
			beval->showState = ShS_PENDING;
			(*beval->ਸੁਨੇਹਾ_CB)(beval, beval->state);
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_ਜੜ੍ਹ = event->xmotion.x_ਜੜ੍ਹ;
		    beval->y_ਜੜ੍ਹ = event->xmotion.y_ਜੜ੍ਹ;
		    cancelBalloon(beval);
		    beval->timerID = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    ਰੋਕੋ;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	ਹਾਲਤ ਕੁੰਜੀ_Press:
	    ਜੇ (beval->showState == ShS_SHOWING && beval->ਸੁਨੇਹਾ_CB != ਨਿਰਾ)
	    {
		Modifiers   modifier;
		ਕੁੰਜੀ_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		ਜੇ (keysym == XK_ਖਿਸਕਾਓ_L || keysym == XK_ਖਿਸਕਾਓ_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ਸੁਨੇਹਾ_CB)(beval, ਖਿਸਕਾਓ_ਨਕਾਬ);
		}
		ਨਹੀਂਤਾਂ ਜੇ (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ਸੁਨੇਹਾ_CB)(beval, Control_ਨਕਾਬ);
		}
		ਨਹੀਂਤਾਂ
		    cancelBalloon(beval);
	    }
	    ਨਹੀਂਤਾਂ
		cancelBalloon(beval);
	    ਰੋਕੋ;

	ਹਾਲਤ ਕੁੰਜੀ_Release:
	    ਜੇ (beval->showState == ShS_SHOWING && beval->ਸੁਨੇਹਾ_CB != ਨਿਰਾ)
	    {
		Modifiers modifier;
		ਕੁੰਜੀ_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		ਜੇ ((keysym == XK_ਖਿਸਕਾਓ_L) || (keysym == XK_ਖਿਸਕਾਓ_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ਸੁਨੇਹਾ_CB)(beval, 0);
		}
		ਨਹੀਂਤਾਂ ਜੇ ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ਸੁਨੇਹਾ_CB)(beval, 0);
		}
		ਨਹੀਂਤਾਂ
		    cancelBalloon(beval);
	    }
	    ਨਹੀਂਤਾਂ
		cancelBalloon(beval);
	    ਰੋਕੋ;

	ਹਾਲਤ LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		ਜੇ (event->xcrossing.ਬਹੁਲਕ == Notify_ਸਾਧਾਰਨ)
		    cancelBalloon(beval);
		ਰੋਕੋ;

	ਹਾਲਤ ButtonPress:
		cancelBalloon(beval);
		ਰੋਕੋ;

	ਮੂਲ:
	    ਰੋਕੋ;
    }
}

    ਸਥਿਰ ਖਾਲੀ
timerRoutine(Xt_ਸੂਚਕ dx, XtIntervalId *id ਅਣਵਰਤਿਆ)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)ਨਿਰਾ;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    ਸਥਿਰ ਖਾਲੀ
requestBalloon(BalloonEval *beval)
{
    ਜੇ (beval->showState == ShS_PENDING)
	ਵਾਪਸ;

    // Determine the beval to display
    ਜੇ (beval->ਸੁਨੇਹਾ_CB != ਨਿਰਾ)
    {
	beval->showState = ShS_PENDING;
	(*beval->ਸੁਨੇਹਾ_CB)(beval, beval->state);
    }
    ਨਹੀਂਤਾਂ ਜੇ (beval->ਸੁਨੇਹਾ != ਨਿਰਾ)
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
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_printable_label_ਪਾਠ(GtkLabel *label, char_u *ਪਾਠ)
{
    char_u	    *convbuf = ਨਿਰਾ;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    len;
    ਪੂਰਨ		    charlen;
    ਪੂਰਨ		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    ਜੇ (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, ਪਾਠ, ਨਿਰਾ);
	ਜੇ (convbuf != ਨਿਰਾ)
	    ਪਾਠ = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    ਲਈ (p = ਪਾਠ; *p != NUL; p += charlen)
    {
	ਜੇ ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	ਨਹੀਂਤਾਂ
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    ਜੇ (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    ਜੇ (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    ਨਹੀਂਤਾਂ ਜੇ (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    ਨਹੀਂਤਾਂ
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    ਜੇ (buf != ਨਿਰਾ)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   ਜੇ PANGO_ਸੰਸਕਰਣ_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  ਨਹੀਂਤਾਂ
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != ਨਿਰਾ) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	ਜੇ (pixel != INVALCOLOR)
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  ਨਹੀਂਤਾਂ
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)pixel, &color);
#  endif

	pdest = buf;
	p = ਪਾਠ;
	ਜਦੋਂ (*p != NUL)
	{
	    // Be quick for ASCII
	    ਜੇ ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		ਜੇ (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		ਜੇ (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    ਪੂਰਨ	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    ਜੇ (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    ਜੇ (pixel != INVALCOLOR)
		    {
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   ਜੇ PANGO_ਸੰਸਕਰਣ_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  ਨਹੀਂਤਾਂ
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = pdest - buf;
			attr->ਸਿਰਾ_ਸੂਚਕਾਂਕ   = pdest - buf + outlen;
			pango_attr_list_ਪਾਓ(attr_list, attr);
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
#   ਜੇ PANGO_ਸੰਸਕਰਣ_CHECK(1,38,0)
			attr_alpha->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = pdest - buf;
			attr_alpha->ਸਿਰਾ_ਸੂਚਕਾਂਕ   = pdest - buf + outlen;
			pango_attr_list_ਪਾਓ(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਕਰੋ
			*pdest++ = *p++;
		    ਜਦੋਂ (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_ਸੈੱਟ_ਪਾਠ(label, (ਅਚਲ ਅੱਖਰ *)buf);
    vim_free(buf);

    gtk_label_ਸੈੱਟ_ਗੁਣ(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    ਸਥਿਰ ਖਾਲੀ
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    ਪੂਰਨ		    natural_ਚੌੜਾਈ = 0;

    ਜੇ (beval->ਸੁਨੇਹਾ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਸੈੱਟ_printable_label_ਪਾਠ(GTK_LABEL(beval->balloonLabel), beval->ਸੁਨੇਹਾ);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    ਨਿਰਾ, &natural_ਚੌੜਾਈ, ਨਿਰਾ, ਨਿਰਾ);

    rect.ਚੌੜਾਈ = (natural_ਚੌੜਾਈ > 0 ? natural_ਚੌੜਾਈ : 1) + gui.char_ਚੌੜਾਈ * 3;
    rect.ਉਚਾਈ = gui.char_ਉਚਾਈ;

    gtk_popover_ਸੈੱਟ_pointing_ਮੰਜ਼ਿਲ(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  ਨਹੀਂਤਾਂ
    ਸਥਿਰ ਖਾਲੀ
drawBalloon(BalloonEval *beval)
{
    ਜੇ (beval->ਸੁਨੇਹਾ == ਨਿਰਾ)
	ਵਾਪਸ;

    GtkRequisition	requisition;
    ਪੂਰਨ		screen_w;
    ਪੂਰਨ		screen_h;
    ਪੂਰਨ		screen_x;
    ਪੂਰਨ		screen_y;
    ਪੂਰਨ		x;
    ਪੂਰਨ		y;
    ਪੂਰਨ		x_ਵਿੱਥ = EVAL_ਵਿੱਥ_X;
    ਪੂਰਨ		y_ਵਿੱਥ = EVAL_ਵਿੱਥ_Y;
    PangoLayout	*layout;

#   ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->ਨਿਸ਼ਾਨਾ);
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_screen(GTK_ਖਿੜਕੀ(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_ਦਾ_win(beval->ਨਿਸ਼ਾਨਾ, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    ਸੈੱਟ_printable_label_ਪਾਠ(GTK_LABEL(beval->balloonLabel), beval->ਸੁਨੇਹਾ);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_ਸ਼ਬਦ_CHAR
    pango_layout_ਸੈੱਟ_wrap(layout, PANGO_WRAP_ਸ਼ਬਦ_CHAR);
#   ਨਹੀਂਤਾਂ
    pango_layout_ਸੈੱਟ_wrap(layout, PANGO_WRAP_ਸ਼ਬਦ);
#   endif
    pango_layout_ਸੈੱਟ_ਚੌੜਾਈ(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ * gui.char_ਚੌੜਾਈ,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_widget_get_preferred_ਮਾਪ(beval->balloonShell, &requisition, ਨਿਰਾ);
#   ਨਹੀਂਤਾਂ
    gtk_widget_ਮਾਪ_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_ਖਿੜਕੀ_get_origin(gtk_widget_get_ਖਿੜਕੀ(beval->ਨਿਸ਼ਾਨਾ), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    ਜੇ (x + x_ਵਿੱਥ + requisition.ਚੌੜਾਈ > screen_x + screen_w)
	y_ਵਿੱਥ += 15;
    ਜੇ (y + y_ਵਿੱਥ + requisition.ਉਚਾਈ > screen_y + screen_h)
	y_ਵਿੱਥ = -requisition.ਉਚਾਈ - EVAL_ਵਿੱਥ_Y;

    // Sanitize values
    x = CLAMP(x + x_ਵਿੱਥ, 0,
	    MAX(0, screen_x + screen_w - requisition.ਚੌੜਾਈ));
    y = CLAMP(y + y_ਵਿੱਥ, 0,
	    MAX(0, screen_y + screen_h - requisition.ਉਚਾਈ));

    // Show the balloon
#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_ਖਿੜਕੀ_move(GTK_ਖਿੜਕੀ(beval->balloonShell), x, y);
#   ਨਹੀਂਤਾਂ
    gtk_widget_ਸੈੱਟ_uposition(beval->balloonShell, x, y);
#   endif
    gtk_widget_show(beval->balloonShell);

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  endif

/*
 * Undraw a balloon.
 */
    ਸਥਿਰ ਖਾਲੀ
undrawBalloon(BalloonEval *beval)
{
    ਜੇ (beval->balloonShell != ਨਿਰਾ)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  ਨਹੀਂਤਾਂ
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    ਸਥਿਰ ਖਾਲੀ
cancelBalloon(BalloonEval *beval)
{
    ਜੇ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ਜੇ (beval->timerID != 0)
    {
	g_ਸਰੋਤ_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    ਸਥਿਰ ਖਾਲੀ
create_Balloon_Eval_ਖਿੜਕੀ(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_ਸੈੱਟ_ਮਾਪਾ(beval->balloonShell, beval->ਨਿਸ਼ਾਨਾ);
    gtk_popover_ਸੈੱਟ_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_ਸੈੱਟ_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_ਸੈੱਟ_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(ਨਿਰਾ);

    gtk_label_ਸੈੱਟ_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_ਸੈੱਟ_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ਖੱਬਾ);
    gtk_label_ਸੈੱਟ_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_ਸੈੱਟ_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_ਸੈੱਟ_ਬੱਚਾ(GTK_POPOVER(popover), beval->balloonLabel);
}
#  ਨਹੀਂਤਾਂ
    ਸਥਿਰ ਖਾਲੀ
create_Balloon_Eval_ਖਿੜਕੀ(BalloonEval *beval)
{
    beval->balloonShell = gtk_ਖਿੜਕੀ_new(GTK_ਖਿੜਕੀ_POPUP);
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_transient_for(GTK_ਖਿੜਕੀ(beval->balloonShell), GTK_ਖਿੜਕੀ(gui.mainwin));

    gtk_widget_ਸੈੱਟ_app_paintable(beval->balloonShell, TRUE);
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_resizable(GTK_ਖਿੜਕੀ(beval->balloonShell), FALSE);
    gtk_widget_ਸੈੱਟ_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(beval->balloonShell), 4);

#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), ਨਿਰਾ);
#   ਨਹੀਂਤਾਂ
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), ਨਿਰਾ);
#   endif
    beval->balloonLabel = gtk_label_new(ਨਿਰਾ);

    gtk_label_ਸੈੱਟ_ਸਤਰ_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_ਸੈੱਟ_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ਖੱਬਾ);
#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,16,0)
    gtk_label_ਸੈੱਟ_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_ਸੈੱਟ_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
    GValue align_val = G_ਮੁੱਲ_INIT;
    g_ਮੁੱਲ_init(&align_val, G_TYPE_FLOAT);
    g_ਮੁੱਲ_ਸੈੱਟ_float(&align_val, 0.5);
    g_object_ਸੈੱਟ_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_ਸੈੱਟ_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_ਮੁੱਲ_unset(&align_val);
#   ਨਹੀਂਤਾਂ
    gtk_misc_ਸੈੱਟ_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_ਸੈੱਟ_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_ਜੋੜੋ(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# ਨਹੀਂਤਾਂ // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    ਸਥਿਰ ਖਾਲੀ
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    ਜੇ (beval->ਸੁਨੇਹਾ == ਨਿਰਾ)
	ਵਾਪਸ;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    ਜੇ (beval->ਸੁਨੇਹਾ_CB != ਨਿਰਾ)
	s = XmStringCreateLtoR((ਅੱਖਰ *)beval->ਸੁਨੇਹਾ, Xm_FONTLIST_DEFAULT_ਟੈਗ);
    ਨਹੀਂਤਾਂ
	s = XmStringCreateLocalized((ਅੱਖਰ *)beval->ਸੁਨੇਹਾ);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	ਜੇ (fl == ਨਿਰਾ)
	{
	    XmStringFree(s);
	    ਵਾਪਸ;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_ਵਿੱਥ << 1;
    h += gui.border_ਵਿੱਥ << 1;
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(beval->balloonLabel, XmNlabelString, s, ਨਿਰਾ);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_ਜੜ੍ਹ + EVAL_ਵਿੱਥ_X;
    ty = beval->y_ਜੜ੍ਹ + EVAL_ਵਿੱਥ_Y;
    ਜੇ ((tx + w) > beval->screen_ਚੌੜਾਈ)
	tx = beval->screen_ਚੌੜਾਈ - w;
    ਜੇ ((ty + h) > beval->screen_ਉਚਾਈ)
	ty = beval->screen_ਉਚਾਈ - h;
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    ਨਿਰਾ);
    // Set tooltip colors
    {
	Arg ਦਲੀਲਾਂ[2];

	ਦਲੀਲਾਂ[0].name = XmNbackground;
	ਦਲੀਲਾਂ[0].ਮੁੱਲ = gui.tooltip_bg_pixel;
	ਦਲੀਲਾਂ[1].name = XmNforeground;
	ਦਲੀਲਾਂ[1].ਮੁੱਲ = gui.tooltip_fg_pixel;
	Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(beval->balloonLabel, &ਦਲੀਲਾਂ[0], Xt_ਸੰਖਿਆ(ਦਲੀਲਾਂ));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    ਸਥਿਰ ਖਾਲੀ
undrawBalloon(BalloonEval *beval)
{
    ਜੇ (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
cancelBalloon(BalloonEval *beval)
{
    ਜੇ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ਜੇ (beval->timerID != (XtIntervalId)ਨਿਰਾ)
    {
	Xt_Remove_ਸਮਾਂ_ਬਾਹਰਲਾ(beval->timerID);
	beval->timerID = (XtIntervalId)ਨਿਰਾ;
    }
    beval->showState = ShS_NEUTRAL;
}


    ਸਥਿਰ ਖਾਲੀ
create_Balloon_Eval_ਖਿੜਕੀ(BalloonEval *beval)
{
    Arg		ਦਲੀਲਾਂ[12];
    ਪੂਰਨ		n;

    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNallowShellResize, True); n++;
    beval->balloonShell = XtAppCreateShell("balloonEval", "BalloonEval",
		    overrideShellWidgetClass, gui.dpy, ਦਲੀਲਾਂ, n);

    XmFontList fl;

    n = 0;
    fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNforeground, gui.tooltip_fg_pixel); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbackground, gui.tooltip_bg_pixel); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNfontList, fl); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    beval->balloonLabel = XtCreateManagedWidget("balloonLabel",
	    xmLabelWidgetClass, beval->balloonShell, ਦਲੀਲਾਂ, n);
}

# endif // !FEAT_GUI_GTK
#ਜੇ_ਅੰਤ // !FEAT_GUI_MSWIN

#ਜੇ_ਅੰਤ // FEAT_BEVAL_GUI
