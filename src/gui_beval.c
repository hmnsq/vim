/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#ଯଦି !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_VERSION(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  ଅନ୍ୟଥା
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# ଅନ୍ୟଥା
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
ବାହ୍ୟ Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
ସ୍ଥିର BalloonEval *current_beval = ଖାଲି;
# endif

# ifdef FEAT_GUI_GTK
ସ୍ଥିର ଶୂନ୍ୟ addEventHandler(GtkWidget *, BalloonEval *);
ସ୍ଥିର ଶୂନ୍ୟ removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
ସ୍ଥିର gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  ଅନ୍ୟଥା
ସ୍ଥିର gint target_event_cb(GtkWidget *, GdkEvent *, gpointer);
ସ୍ଥିର gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
ସ୍ଥିର ଶୂନ୍ୟ pointer_event(BalloonEval *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, unsigned);
ସ୍ଥିର ଶୂନ୍ୟ ଚାବି_event(BalloonEval *, unsigned, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   ଯଦି GTK_CHECK_VERSION(3,0,0)
ସ୍ଥିର gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   ଅନ୍ୟଥା
ସ୍ଥିର gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ addEventHandler(Widget, BalloonEval *);
ସ୍ଥିର ଶୂନ୍ୟ removeEventHandler(BalloonEval *);
ସ୍ଥିର ଶୂନ୍ୟ pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
ସ୍ଥିର ଶୂନ୍ୟ pointerEvent(BalloonEval *, XEvent *);
ସ୍ଥିର ଶୂନ୍ୟ timerRoutine(XtPointer, XtIntervalId *);
# endif
ସ୍ଥିର ଶୂନ୍ୟ cancelBalloon(BalloonEval *);
ସ୍ଥିର ଶୂନ୍ୟ requestBalloon(BalloonEval *);
ସ୍ଥିର ଶୂନ୍ୟ drawBalloon(BalloonEval *);
ସ୍ଥିର ଶୂନ୍ୟ undrawBalloon(BalloonEval *beval);
ସ୍ଥିର ଶୂନ୍ୟ createBalloonEvalWindow(BalloonEval *);

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
    ଶୂନ୍ୟ	*target,
    char_u	*mesg,
    ଶୂନ୍ୟ	(*mesgCB)(BalloonEval *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
    ଶୂନ୍ୟ	*clientData)
{
# ifndef FEAT_GUI_GTK
    ଅକ୍ଷର	*display_name;	    // get from gui.dpy
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		screen_ଅଙ୍କ;
    ଅକ୍ଷର	*p;
# endif
    BalloonEval	*beval;

    ଯଦି (mesg != ଖାଲି && mesgCB != ଖାଲି)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ଯଦି (beval != ଖାଲି)
    {
# ifdef FEAT_GUI_GTK
	beval->target = GTK_WIDGET(target);
# ଅନ୍ୟଥା
	beval->target = (Widget)target;
	beval->appContext = XtWidgetToApplicationContext((Widget)target);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->ସନ୍ଦେଶ);
	beval->ସନ୍ଦେଶ = mesg == ଖାଲି ? ଖାଲି : vim_strsave(mesg);
	beval->ସନ୍ଦେଶ_CB = mesgCB;
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
	ଯଦି (p != ଖାଲି)
	    screen_ଅଙ୍କ = atoi(++p);
	ଅନ୍ୟଥା
	    screen_ଅଙ୍କ = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_ଅଙ୍କ);
	beval->screen_height = DisplayHeight(gui.dpy, screen_ଅଙ୍କ);
# endif
    }

    ଫେରନ୍ତୁ beval;
}

# ଯଦି defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    ଶୂନ୍ୟ
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   ଅନ୍ୟଥା
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  ଅନ୍ୟଥା
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    ଯଦି (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->ସନ୍ଦେଶ);
    vim_free(beval);
}
# endif

    ଶୂନ୍ୟ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ଯଦି (beval != ଖାଲି)
	addEventHandler(beval->target, beval);
}

    ଶୂନ୍ୟ
gui_mch_disable_beval_area(BalloonEval *beval)
{
    ଯଦି (beval != ଖାଲି)
	removeEventHandler(beval);
}

# ଯଦି defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ current_beval;
}
# endif
#endif // !FEAT_GUI_MSWIN

#ଯଦି defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# ଯଦି !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    ଶୂନ୍ୟ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->ସନ୍ଦେଶ);
    beval->ସନ୍ଦେଶ = mesg == ଖାଲି ? ଖାଲି : vim_strsave(mesg);
    ଯଦି (beval->ସନ୍ଦେଶ != ଖାଲି)
	drawBalloon(beval);
    ଅନ୍ୟଥା
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#endif // FEAT_NETBEANS_INTG || FEAT_EVAL

#ଯଦି !defined(FEAT_GUI_MSWIN)
# ଯଦି defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    ଶୂନ୍ୟ
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->ସନ୍ଦେଶ);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    ସ୍ଥିର ଶୂନ୍ୟ
addEventHandler(GtkWidget *target, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_set_data(G_OBJECT(target), "beval", controller);
    gtk_widget_add_controller(target, controller);
#  ଅନ୍ୟଥା
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
    ଯଦି (gtk_socket_id == 0 && gui.mainwin != ଖାଲି
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

    ସ୍ଥିର ଶୂନ୍ୟ
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*target = beval->target;
    GtkEventController	*controller;

    controller = g_object_get_data(G_OBJECT(target), "beval");
    ଯଦି (controller != ଖାଲି)
    {
	gtk_widget_remove_controller(target, controller);
	g_object_set_data(G_OBJECT(target), "beval", ଖାଲି);
    }
#  ଅନ୍ୟଥା
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->target),
					 FUNC2GENERIC(target_event_cb),
					 beval);

    ଯଦି (gtk_socket_id == 0 && gui.mainwin != ଖାଲି
	    && gtk_widget_is_ancestor(beval->target, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    ସ୍ଥିର gboolean
event_cb(GtkEventController *controller UNUSED, GdkEvent *event, BalloonEval *beval)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = gdk_event_get_event_type(event);

    ବାଛ (type)
    {
	ସ୍ଥିତି GDK_ENTER_NOTIFY:
	ସ୍ଥିତି GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*ମୂଳ;
	    graphene_point_t	p;
	    graphene_point_t	result;
	    ଦ୍ୱିଗୁଣ		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    ମୂଳ = GTK_WIDGET(gtk_widget_get_ମୂଳ(beval->target));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    ଯଦି (gtk_widget_compute_point(ମୂଳ, beval->target, &p, &result))
	    {
		x = result.x;
		y = result.y;
	    }

	    pointer_event(beval, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y,
		    gdk_event_get_modifier_state(event));
	    ଭାଙ୍ଗ;
	}
	ସ୍ଥିତି GDK_LEAVE_NOTIFY:
	    ଯଦି (gdk_crossing_event_get_mode(event) == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GDK_BUTTON_PRESS:
	ସ୍ଥିତି GDK_SCROLL:
	    cancelBalloon(beval);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GDK_ଚାବି_PRESS:
	ସ୍ଥିତି GDK_ଚାବି_RELEASE:
	    ଚାବି_event(beval, gdk_ଚାବି_event_get_keyval(event),
		    type == GDK_ଚାବି_PRESS);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }

    // Always pass through events?
    ଫେରନ୍ତୁ FALSE;
}
#  ଅନ୍ୟଥା
    ସ୍ଥିର gint
target_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    ବାଛ (event->type)
    {
	ସ୍ଥିତି GDK_ENTER_NOTIFY:
	    pointer_event(beval, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event->crossing.x,
				 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event->crossing.y,
				 event->crossing.state);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GDK_MOTION_NOTIFY:
	    ଯଦି (event->motion.is_hint)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   ଯଦି GTK_CHECK_VERSION(3,0,0)
		{
		    GdkWindow * ଅଚଳ win = gtk_widget_get_window(widget);
		    GdkDisplay * ଅଚଳ dpy = gdk_window_get_display(win);
#    ଯଦି GTK_CHECK_VERSION(3,20,0)
		    GdkSeat * ଅଚଳ seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * ଅଚଳ dev = gdk_seat_get_pointer(seat);
#    ଅନ୍ୟଥା
		    GdkDeviceManager * ଅଚଳ mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * ଅଚଳ dev = gdk_device_manager_get_client_pointer(mngr);
#    endif
		    gdk_window_get_device_position(win, dev , &x, &y, &state);
		}
#   ଅନ୍ୟଥା
		gdk_window_get_pointer(widget->window, &x, &y, &state);
#   endif
		pointer_event(beval, x, y, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)state);
	    }
	    ଅନ୍ୟଥା
	    {
		pointer_event(beval, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event->motion.x,
				     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event->motion.y,
				     event->motion.state);
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    ଯଦି (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GDK_BUTTON_PRESS:
	ସ୍ଥିତି GDK_SCROLL:
	    cancelBalloon(beval);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GDK_ଚାବି_PRESS:
	    ଚାବି_event(beval, event->ଚାବି.keyval, TRUE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GDK_ଚାବି_RELEASE:
	    ଚାବି_event(beval, event->ଚାବି.keyval, FALSE);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ FALSE; // continue emission
}

    ସ୍ଥିର gint
mainwin_event_cb(GtkWidget *widget UNUSED, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    ବାଛ (event->type)
    {
	ସ୍ଥିତି GDK_ଚାବି_PRESS:
	    ଚାବି_event(beval, event->ଚାବି.keyval, TRUE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GDK_ଚାବି_RELEASE:
	    ଚାବି_event(beval, event->ଚାବି.keyval, FALSE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    ଯଦି (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ FALSE; // continue emission
}
#  endif

    ସ୍ଥିର ଶୂନ୍ୟ
pointer_event(BalloonEval *beval, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, unsigned state)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା distance;

    distance = ABS(x - beval->x) + ABS(y - beval->y);

    ଯଦି (distance <= 4)
	ଫେରନ୍ତୁ;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    ଯଦି (!(state & ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_BUTTON1_MASK | (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_BUTTON2_MASK
		    | (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_BUTTON3_MASK)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	ଯଦି (state & (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_ALT_MASK)
#  ଅନ୍ୟଥା
	ଯଦି (state & (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_MOD1_MASK)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    ଯଦି (beval->ସନ୍ଦେଶ_CB != ଖାଲି)
	    {
		beval->showState = ShS_PENDING;
		(*beval->ସନ୍ଦେଶ_CB)(beval, state);
	    }
	}
	ଅନ୍ୟଥା
	{
	    beval->timerID = g_timeout_add((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
ଚାବି_event(BalloonEval *beval, unsigned keyval, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_keypress)
{
    ଯଦି (beval->showState == ShS_SHOWING && beval->ସନ୍ଦେଶ_CB != ଖାଲି)
    {
	ବାଛ (keyval)
	{
#  ifdef USE_GTK4
	    ସ୍ଥିତି GDK_ଚାବି_Shift_L:
	    ସ୍ଥିତି GDK_ଚାବି_Shift_R:
#  ଅନ୍ୟଥା
	    ସ୍ଥିତି GDK_Shift_L:
	    ସ୍ଥିତି GDK_Shift_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->ସନ୍ଦେଶ_CB)(beval, (is_keypress)
						   ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_SHIFT_MASK : 0);
		ଭାଙ୍ଗ;
#  ifdef USE_GTK4
	    ସ୍ଥିତି GDK_ଚାବି_Control_L:
	    ସ୍ଥିତି GDK_ଚାବି_Control_R:
#  ଅନ୍ୟଥା
	    ସ୍ଥିତି GDK_Control_L:
	    ସ୍ଥିତି GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->ସନ୍ଦେଶ_CB)(beval, (is_keypress)
						 ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_CONTROL_MASK : 0);
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		ଯଦି (is_keypress)
		    cancelBalloon(beval);
		ଭାଙ୍ଗ;
	}
    }
    ଅନ୍ୟଥା
	cancelBalloon(beval);
}

    ସ୍ଥିର gboolean
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

    ଫେରନ୍ତୁ FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   ଯଦି GTK_CHECK_VERSION(3,0,0)
    ସ୍ଥିର gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 data UNUSED)
{
    GtkStyleContext *context = ଖାଲି;
    gint width = -1, height = -1;

    ଯଦି (widget == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;

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

    ଫେରନ୍ତୁ FALSE;
}
#   ଅନ୍ୟଥା
    ସ୍ଥିର gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer data UNUSED)
{
    gtk_paint_flat_box(widget->style, widget->window,
		       GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    ଫେରନ୍ତୁ FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# ଅନ୍ୟଥା // !FEAT_GUI_GTK

    ସ୍ଥିର ଶୂନ୍ୟ
addEventHandler(Widget target, BalloonEval *beval)
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | ଚାବି_Press_Mask |
			ଚାବି_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}

    ସ୍ଥିର ଶୂନ୍ୟ
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | ଚାବି_Press_Mask |
			ଚାବି_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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

    ସ୍ଥିର ଶୂନ୍ୟ
pointerEvent(BalloonEval *beval, XEvent *event)
{
    Position	distance;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    ବାଛ (event->type)
    {
	ସ୍ଥିତି EnterNotify:
	ସ୍ଥିତି MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    ଯଦି (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    ଯଦି (delta < 0)
		delta = -delta;
	    distance += delta;
	    ଯଦି (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		ଯଦି (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		ଅନ୍ୟଥା ଯଦି (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_ମୂଳ = event->xmotion.x_ମୂଳ;
		    beval->y_ମୂଳ = event->xmotion.y_ମୂଳ;
		    cancelBalloon(beval);
		    ଯଦି (beval->ସନ୍ଦେଶ_CB != ଖାଲି)
		    {
			beval->showState = ShS_PENDING;
			(*beval->ସନ୍ଦେଶ_CB)(beval, beval->state);
		    }
		}
		ଅନ୍ୟଥା
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_ମୂଳ = event->xmotion.x_ମୂଳ;
		    beval->y_ମୂଳ = event->xmotion.y_ମୂଳ;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    ଭାଙ୍ଗ;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	ସ୍ଥିତି ଚାବି_Press:
	    ଯଦି (beval->showState == ShS_SHOWING && beval->ସନ୍ଦେଶ_CB != ଖାଲି)
	    {
		Modifiers   modifier;
		ଚାବି_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		ଯଦି (keysym == XK_Shift_L || keysym == XK_Shift_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ସନ୍ଦେଶ_CB)(beval, ShiftMask);
		}
		ଅନ୍ୟଥା ଯଦି (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ସନ୍ଦେଶ_CB)(beval, ControlMask);
		}
		ଅନ୍ୟଥା
		    cancelBalloon(beval);
	    }
	    ଅନ୍ୟଥା
		cancelBalloon(beval);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ଚାବି_Release:
	    ଯଦି (beval->showState == ShS_SHOWING && beval->ସନ୍ଦେଶ_CB != ଖାଲି)
	    {
		Modifiers modifier;
		ଚାବି_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		ଯଦି ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ସନ୍ଦେଶ_CB)(beval, 0);
		}
		ଅନ୍ୟଥା ଯଦି ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->ସନ୍ଦେଶ_CB)(beval, 0);
		}
		ଅନ୍ୟଥା
		    cancelBalloon(beval);
	    }
	    ଅନ୍ୟଥା
		cancelBalloon(beval);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		ଯଦି (event->xcrossing.mode == NotifyNormal)
		    cancelBalloon(beval);
		ଭାଙ୍ଗ;

	ସ୍ଥିତି ButtonPress:
		cancelBalloon(beval);
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
timerRoutine(XtPointer dx, XtIntervalId *id UNUSED)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)ଖାଲି;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    ସ୍ଥିର ଶୂନ୍ୟ
requestBalloon(BalloonEval *beval)
{
    ଯଦି (beval->showState == ShS_PENDING)
	ଫେରନ୍ତୁ;

    // Determine the beval to display
    ଯଦି (beval->ସନ୍ଦେଶ_CB != ଖାଲି)
    {
	beval->showState = ShS_PENDING;
	(*beval->ସନ୍ଦେଶ_CB)(beval, beval->state);
    }
    ଅନ୍ୟଥା ଯଦି (beval->ସନ୍ଦେଶ != ଖାଲି)
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
    ସ୍ଥିର ଶୂନ୍ୟ
set_printable_label_text(GtkLabel *label, char_u *text)
{
    char_u	    *convbuf = ଖାଲି;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    charlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    ଯଦି (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, text, ଖାଲି);
	ଯଦି (convbuf != ଖାଲି)
	    text = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    ପାଇଁ (p = text; *p != NUL; p += charlen)
    {
	ଯଦି ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	ଅନ୍ୟଥା
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    ଯଦି (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    ଯଦି (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    ଅନ୍ୟଥା ଯଦି (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    ଅନ୍ୟଥା
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    ଯଦି (buf != ଖାଲି)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  ଯଦି GTK_CHECK_VERSION(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   ଯଦି PANGO_VERSION_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  ଅନ୍ୟଥା
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != ଖାଲି) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	ଯଦି (pixel != INVALCOLOR)
#  ଯଦି GTK_CHECK_VERSION(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  ଅନ୍ୟଥା
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (unsigned long)pixel, &color);
#  endif

	pdest = buf;
	p = text;
	ଯେତେବେଳେ (*p != NUL)
	{
	    // Be quick for ASCII
	    ଯଦି ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    ଅନ୍ୟଥା
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		ଯଦି (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		ଯଦି (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    ଯଦି (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    ଅନ୍ୟଥା ଯଦି (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    ଅନ୍ୟଥା
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    ଯଦି (pixel != INVALCOLOR)
		    {
#  ଯଦି GTK_CHECK_VERSION(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   ଯଦି PANGO_VERSION_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  ଅନ୍ୟଥା
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->start_index = pdest - buf;
			attr->end_index   = pdest - buf + outlen;
			pango_attr_list_ଭର୍ତ୍ତି(attr_list, attr);
#  ଯଦି GTK_CHECK_VERSION(3,0,0)
#   ଯଦି PANGO_VERSION_CHECK(1,38,0)
			attr_alpha->start_index = pdest - buf;
			attr_alpha->end_index   = pdest - buf + outlen;
			pango_attr_list_ଭର୍ତ୍ତି(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		ଅନ୍ୟଥା
		{
		    କର
			*pdest++ = *p++;
		    ଯେତେବେଳେ (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_set_text(label, (ଅଚଳ ଅକ୍ଷର *)buf);
    vim_free(buf);

    gtk_label_set_attributes(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    ସ୍ଥିର ଶୂନ୍ୟ
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    natural_width = 0;

    ଯଦି (beval->ସନ୍ଦେଶ == ଖାଲି)
	ଫେରନ୍ତୁ;

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->ସନ୍ଦେଶ);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    ଖାଲି, &natural_width, ଖାଲି, ଖାଲି);

    rect.width = (natural_width > 0 ? natural_width : 1) + gui.char_width * 3;
    rect.height = gui.char_height;

    gtk_popover_set_pointing_to(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  ଅନ୍ୟଥା
    ସ୍ଥିର ଶୂନ୍ୟ
drawBalloon(BalloonEval *beval)
{
    ଯଦି (beval->ସନ୍ଦେଶ == ଖାଲି)
	ଫେରନ୍ତୁ;

    GtkRequisition	requisition;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		screen_w;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		screen_h;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		screen_x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		screen_y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x_offset = EVAL_OFFSET_X;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y_offset = EVAL_OFFSET_Y;
    PangoLayout	*layout;

#   ଯଦି !GTK_CHECK_VERSION(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->target);
    gtk_window_set_screen(GTK_WINDOW(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_of_win(beval->target, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   ଯଦି !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->ସନ୍ଦେଶ);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_WORD_CHAR
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
#   ଅନ୍ୟଥା
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
#   endif
    pango_layout_set_width(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.ଅଙ୍କ_cols * gui.char_width,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   ଯଦି GTK_CHECK_VERSION(3,0,0)
    gtk_widget_get_preferred_size(beval->balloonShell, &requisition, ଖାଲି);
#   ଅନ୍ୟଥା
    gtk_widget_size_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_window_get_origin(gtk_widget_get_window(beval->target), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    ଯଦି (x + x_offset + requisition.width > screen_x + screen_w)
	y_offset += 15;
    ଯଦି (y + y_offset + requisition.height > screen_y + screen_h)
	y_offset = -requisition.height - EVAL_OFFSET_Y;

    // Sanitize values
    x = CLAMP(x + x_offset, 0,
	    MAX(0, screen_x + screen_w - requisition.width));
    y = CLAMP(y + y_offset, 0,
	    MAX(0, screen_y + screen_h - requisition.height));

    // Show the balloon
#   ଯଦି GTK_CHECK_VERSION(3,0,0)
    gtk_window_move(GTK_WINDOW(beval->balloonShell), x, y);
#   ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
undrawBalloon(BalloonEval *beval)
{
    ଯଦି (beval->balloonShell != ଖାଲି)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  ଅନ୍ୟଥା
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    ସ୍ଥିର ଶୂନ୍ୟ
cancelBalloon(BalloonEval *beval)
{
    ଯଦି (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ଯଦି (beval->timerID != 0)
    {
	g_source_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    ସ୍ଥିର ଶୂନ୍ୟ
createBalloonEvalWindow(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_set_parent(beval->balloonShell, beval->target);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_set_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(ଖାଲି);

    gtk_label_set_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ବାମ);
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_set_child(GTK_POPOVER(popover), beval->balloonLabel);
}
#  ଅନ୍ୟଥା
    ସ୍ଥିର ଶୂନ୍ୟ
createBalloonEvalWindow(BalloonEval *beval)
{
    beval->balloonShell = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_transient_for(GTK_WINDOW(beval->balloonShell), GTK_WINDOW(gui.mainwin));

    gtk_widget_set_app_paintable(beval->balloonShell, TRUE);
    gtk_window_set_resizable(GTK_WINDOW(beval->balloonShell), FALSE);
    gtk_widget_set_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_set_border_width(GTK_CONTAINER(beval->balloonShell), 4);

#   ଯଦି GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), ଖାଲି);
#   ଅନ୍ୟଥା
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), ଖାଲି);
#   endif
    beval->balloonLabel = gtk_label_new(ଖାଲି);

    gtk_label_set_ଧାଡ଼ି_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ବାମ);
#   ଯଦି GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_VERSION(3,14,0)
    GValue align_val = G_ମୂଲ୍ୟ_INIT;
    g_ମୂଲ୍ୟ_init(&align_val, G_TYPE_FLOAT);
    g_ମୂଲ୍ୟ_set_float(&align_val, 0.5);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_ମୂଲ୍ୟ_unset(&align_val);
#   ଅନ୍ୟଥା
    gtk_misc_set_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_set_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_add(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# ଅନ୍ୟଥା // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    ଯଦି (beval->ସନ୍ଦେଶ == ଖାଲି)
	ଫେରନ୍ତୁ;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    ଯଦି (beval->ସନ୍ଦେଶ_CB != ଖାଲି)
	s = XmStringCreateLtoR((ଅକ୍ଷର *)beval->ସନ୍ଦେଶ, XmFONTLIST_DEFAULT_TAG);
    ଅନ୍ୟଥା
	s = XmStringCreateLocalized((ଅକ୍ଷର *)beval->ସନ୍ଦେଶ);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	ଯଦି (fl == ଖାଲି)
	{
	    XmStringFree(s);
	    ଫେରନ୍ତୁ;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_offset << 1;
    h += gui.border_offset << 1;
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(beval->balloonLabel, XmNlabelString, s, ଖାଲି);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_ମୂଳ + EVAL_OFFSET_X;
    ty = beval->y_ମୂଳ + EVAL_OFFSET_Y;
    ଯଦି ((tx + w) > beval->screen_width)
	tx = beval->screen_width - w;
    ଯଦି ((ty + h) > beval->screen_height)
	ty = beval->screen_height - h;
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    ଖାଲି);
    // Set tooltip colors
    {
	Arg args[2];

	args[0].name = XmNbackground;
	args[0].ମୂଲ୍ୟ = gui.tooltip_bg_pixel;
	args[1].name = XmNforeground;
	args[1].ମୂଲ୍ୟ = gui.tooltip_fg_pixel;
	Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(beval->balloonLabel, &args[0], Xt_ସଂଖ୍ୟା(args));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
undrawBalloon(BalloonEval *beval)
{
    ଯଦି (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
cancelBalloon(BalloonEval *beval)
{
    ଯଦି (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ଯଦି (beval->timerID != (XtIntervalId)ଖାଲି)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = (XtIntervalId)ଖାଲି;
    }
    beval->showState = ShS_NEUTRAL;
}


    ସ୍ଥିର ଶୂନ୍ୟ
createBalloonEvalWindow(BalloonEval *beval)
{
    Arg		args[12];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

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
