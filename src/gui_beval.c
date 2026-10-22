/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#જો !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_VERSION(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  નહીંતર
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# નહીંતર
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
બાહ્ય Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
સ્થિર BalloonEval *current_beval = ખાલી;
# endif

# ifdef FEAT_GUI_GTK
સ્થિર શૂન્ય addEventHandler(GtkWidget *, BalloonEval *);
સ્થિર શૂન્ય removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
સ્થિર gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  નહીંતર
સ્થિર gint target_event_cb(GtkWidget *, GdkEvent *, gpointer);
સ્થિર gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
સ્થિર શૂન્ય pointer_event(BalloonEval *, પૂર્ણાંક, પૂર્ણાંક, unsigned);
સ્થિર શૂન્ય ચાવી_event(BalloonEval *, unsigned, પૂર્ણાંક);
સ્થિર gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   જો GTK_CHECK_VERSION(3,0,0)
સ્થિર gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   નહીંતર
સ્થિર gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# નહીંતર
સ્થિર શૂન્ય addEventHandler(Widget, BalloonEval *);
સ્થિર શૂન્ય removeEventHandler(BalloonEval *);
સ્થિર શૂન્ય pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
સ્થિર શૂન્ય pointerEvent(BalloonEval *, XEvent *);
સ્થિર શૂન્ય timerRoutine(XtPointer, XtIntervalId *);
# endif
સ્થિર શૂન્ય cancelBalloon(BalloonEval *);
સ્થિર શૂન્ય requestBalloon(BalloonEval *);
સ્થિર શૂન્ય drawBalloon(BalloonEval *);
સ્થિર શૂન્ય undrawBalloon(BalloonEval *beval);
સ્થિર શૂન્ય createBalloonEvalWindow(BalloonEval *);

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
    શૂન્ય	*target,
    char_u	*mesg,
    શૂન્ય	(*mesgCB)(BalloonEval *, પૂર્ણાંક),
    શૂન્ય	*clientData)
{
# ifndef FEAT_GUI_GTK
    અક્ષર	*display_name;	    // get from gui.dpy
    પૂર્ણાંક		screen_અંક;
    અક્ષર	*p;
# endif
    BalloonEval	*beval;

    જો (mesg != ખાલી && mesgCB != ખાલી)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	પાછા ખાલી;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    જો (beval != ખાલી)
    {
# ifdef FEAT_GUI_GTK
	beval->target = GTK_WIDGET(target);
# નહીંતર
	beval->target = (Widget)target;
	beval->appContext = XtWidgetToApplicationContext((Widget)target);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->સંદેશ);
	beval->સંદેશ = mesg == ખાલી ? ખાલી : vim_strsave(mesg);
	beval->સંદેશ_CB = mesgCB;
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
	જો (p != ખાલી)
	    screen_અંક = atoi(++p);
	નહીંતર
	    screen_અંક = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_અંક);
	beval->screen_height = DisplayHeight(gui.dpy, screen_અંક);
# endif
    }

    પાછા beval;
}

# જો defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    શૂન્ય
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   નહીંતર
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  નહીંતર
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    જો (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->સંદેશ);
    vim_free(beval);
}
# endif

    શૂન્ય
gui_mch_enable_beval_area(BalloonEval *beval)
{
    જો (beval != ખાલી)
	addEventHandler(beval->target, beval);
}

    શૂન્ય
gui_mch_disable_beval_area(BalloonEval *beval)
{
    જો (beval != ખાલી)
	removeEventHandler(beval);
}

# જો defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(શૂન્ય)
{
    પાછા current_beval;
}
# endif
#endif // !FEAT_GUI_MSWIN

#જો defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# જો !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    શૂન્ય
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->સંદેશ);
    beval->સંદેશ = mesg == ખાલી ? ખાલી : vim_strsave(mesg);
    જો (beval->સંદેશ != ખાલી)
	drawBalloon(beval);
    નહીંતર
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#endif // FEAT_NETBEANS_INTG || FEAT_EVAL

#જો !defined(FEAT_GUI_MSWIN)
# જો defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    શૂન્ય
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->સંદેશ);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    સ્થિર શૂન્ય
addEventHandler(GtkWidget *target, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_set_data(G_OBJECT(target), "beval", controller);
    gtk_widget_add_controller(target, controller);
#  નહીંતર
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
    જો (gtk_socket_id == 0 && gui.mainwin != ખાલી
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

    સ્થિર શૂન્ય
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*target = beval->target;
    GtkEventController	*controller;

    controller = g_object_get_data(G_OBJECT(target), "beval");
    જો (controller != ખાલી)
    {
	gtk_widget_remove_controller(target, controller);
	g_object_set_data(G_OBJECT(target), "beval", ખાલી);
    }
#  નહીંતર
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->target),
					 FUNC2GENERIC(target_event_cb),
					 beval);

    જો (gtk_socket_id == 0 && gui.mainwin != ખાલી
	    && gtk_widget_is_ancestor(beval->target, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    સ્થિર gboolean
event_cb(GtkEventController *controller UNUSED, GdkEvent *event, BalloonEval *beval)
{
    પૂર્ણાંક type = gdk_event_get_event_type(event);

    પસંદગી (type)
    {
	સ્થિતિ GDK_ENTER_NOTIFY:
	સ્થિતિ GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*મૂળ;
	    graphene_point_t	p;
	    graphene_point_t	result;
	    દ્વિગુણ		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    મૂળ = GTK_WIDGET(gtk_widget_get_મૂળ(beval->target));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    જો (gtk_widget_compute_point(મૂળ, beval->target, &p, &result))
	    {
		x = result.x;
		y = result.y;
	    }

	    pointer_event(beval, (પૂર્ણાંક)x, (પૂર્ણાંક)y,
		    gdk_event_get_modifier_state(event));
	    તોડો;
	}
	સ્થિતિ GDK_LEAVE_NOTIFY:
	    જો (gdk_crossing_event_get_mode(event) == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    તોડો;
	સ્થિતિ GDK_BUTTON_PRESS:
	સ્થિતિ GDK_SCROLL:
	    cancelBalloon(beval);
	    તોડો;
	સ્થિતિ GDK_ચાવી_PRESS:
	સ્થિતિ GDK_ચાવી_RELEASE:
	    ચાવી_event(beval, gdk_ચાવી_event_get_keyval(event),
		    type == GDK_ચાવી_PRESS);
	    તોડો;
	મૂળભૂત:
	    તોડો;
    }

    // Always pass through events?
    પાછા FALSE;
}
#  નહીંતર
    સ્થિર gint
target_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    પસંદગી (event->type)
    {
	સ્થિતિ GDK_ENTER_NOTIFY:
	    pointer_event(beval, (પૂર્ણાંક)event->crossing.x,
				 (પૂર્ણાંક)event->crossing.y,
				 event->crossing.state);
	    તોડો;
	સ્થિતિ GDK_MOTION_NOTIFY:
	    જો (event->motion.is_hint)
	    {
		પૂર્ણાંક		x;
		પૂર્ણાંક		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   જો GTK_CHECK_VERSION(3,0,0)
		{
		    GdkWindow * અચલ win = gtk_widget_get_window(widget);
		    GdkDisplay * અચલ dpy = gdk_window_get_display(win);
#    જો GTK_CHECK_VERSION(3,20,0)
		    GdkSeat * અચલ seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * અચલ dev = gdk_seat_get_pointer(seat);
#    નહીંતર
		    GdkDeviceManager * અચલ mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * અચલ dev = gdk_device_manager_get_client_pointer(mngr);
#    endif
		    gdk_window_get_device_position(win, dev , &x, &y, &state);
		}
#   નહીંતર
		gdk_window_get_pointer(widget->window, &x, &y, &state);
#   endif
		pointer_event(beval, x, y, (unsigned પૂર્ણાંક)state);
	    }
	    નહીંતર
	    {
		pointer_event(beval, (પૂર્ણાંક)event->motion.x,
				     (પૂર્ણાંક)event->motion.y,
				     event->motion.state);
	    }
	    તોડો;
	સ્થિતિ GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    જો (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    તોડો;
	સ્થિતિ GDK_BUTTON_PRESS:
	સ્થિતિ GDK_SCROLL:
	    cancelBalloon(beval);
	    તોડો;
	સ્થિતિ GDK_ચાવી_PRESS:
	    ચાવી_event(beval, event->ચાવી.keyval, TRUE);
	    તોડો;
	સ્થિતિ GDK_ચાવી_RELEASE:
	    ચાવી_event(beval, event->ચાવી.keyval, FALSE);
	    તોડો;
	મૂળભૂત:
	    તોડો;
    }

    પાછા FALSE; // continue emission
}

    સ્થિર gint
mainwin_event_cb(GtkWidget *widget UNUSED, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    પસંદગી (event->type)
    {
	સ્થિતિ GDK_ચાવી_PRESS:
	    ચાવી_event(beval, event->ચાવી.keyval, TRUE);
	    તોડો;
	સ્થિતિ GDK_ચાવી_RELEASE:
	    ચાવી_event(beval, event->ચાવી.keyval, FALSE);
	    તોડો;
	સ્થિતિ GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    જો (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    તોડો;
	મૂળભૂત:
	    તોડો;
    }

    પાછા FALSE; // continue emission
}
#  endif

    સ્થિર શૂન્ય
pointer_event(BalloonEval *beval, પૂર્ણાંક x, પૂર્ણાંક y, unsigned state)
{
    પૂર્ણાંક distance;

    distance = ABS(x - beval->x) + ABS(y - beval->y);

    જો (distance <= 4)
	પાછા;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    જો (!(state & ((પૂર્ણાંક)GDK_BUTTON1_MASK | (પૂર્ણાંક)GDK_BUTTON2_MASK
		    | (પૂર્ણાંક)GDK_BUTTON3_MASK)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	જો (state & (પૂર્ણાંક)GDK_ALT_MASK)
#  નહીંતર
	જો (state & (પૂર્ણાંક)GDK_MOD1_MASK)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    જો (beval->સંદેશ_CB != ખાલી)
	    {
		beval->showState = ShS_PENDING;
		(*beval->સંદેશ_CB)(beval, state);
	    }
	}
	નહીંતર
	{
	    beval->timerID = g_timeout_add((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    સ્થિર શૂન્ય
ચાવી_event(BalloonEval *beval, unsigned keyval, પૂર્ણાંક is_keypress)
{
    જો (beval->showState == ShS_SHOWING && beval->સંદેશ_CB != ખાલી)
    {
	પસંદગી (keyval)
	{
#  ifdef USE_GTK4
	    સ્થિતિ GDK_ચાવી_Shift_L:
	    સ્થિતિ GDK_ચાવી_Shift_R:
#  નહીંતર
	    સ્થિતિ GDK_Shift_L:
	    સ્થિતિ GDK_Shift_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->સંદેશ_CB)(beval, (is_keypress)
						   ? (પૂર્ણાંક)GDK_SHIFT_MASK : 0);
		તોડો;
#  ifdef USE_GTK4
	    સ્થિતિ GDK_ચાવી_Control_L:
	    સ્થિતિ GDK_ચાવી_Control_R:
#  નહીંતર
	    સ્થિતિ GDK_Control_L:
	    સ્થિતિ GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->સંદેશ_CB)(beval, (is_keypress)
						 ? (પૂર્ણાંક)GDK_CONTROL_MASK : 0);
		તોડો;
	    મૂળભૂત:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		જો (is_keypress)
		    cancelBalloon(beval);
		તોડો;
	}
    }
    નહીંતર
	cancelBalloon(beval);
}

    સ્થિર gboolean
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

    પાછા FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 data UNUSED)
{
    GtkStyleContext *context = ખાલી;
    gint width = -1, height = -1;

    જો (widget == ખાલી)
	પાછા TRUE;

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

    પાછા FALSE;
}
#   નહીંતર
    સ્થિર gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer data UNUSED)
{
    gtk_paint_flat_box(widget->style, widget->window,
		       GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    પાછા FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# નહીંતર // !FEAT_GUI_GTK

    સ્થિર શૂન્ય
addEventHandler(Widget target, BalloonEval *beval)
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | ચાવી_Press_Mask |
			ચાવી_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}

    સ્થિર શૂન્ય
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | ચાવી_Press_Mask |
			ચાવી_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    સ્થિર શૂન્ય
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

    સ્થિર શૂન્ય
pointerEvent(BalloonEval *beval, XEvent *event)
{
    Position	distance;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    પસંદગી (event->type)
    {
	સ્થિતિ EnterNotify:
	સ્થિતિ MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    જો (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    જો (delta < 0)
		delta = -delta;
	    distance += delta;
	    જો (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		જો (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		નહીંતર જો (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_મૂળ = event->xmotion.x_મૂળ;
		    beval->y_મૂળ = event->xmotion.y_મૂળ;
		    cancelBalloon(beval);
		    જો (beval->સંદેશ_CB != ખાલી)
		    {
			beval->showState = ShS_PENDING;
			(*beval->સંદેશ_CB)(beval, beval->state);
		    }
		}
		નહીંતર
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_મૂળ = event->xmotion.x_મૂળ;
		    beval->y_મૂળ = event->xmotion.y_મૂળ;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    તોડો;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	સ્થિતિ ચાવી_Press:
	    જો (beval->showState == ShS_SHOWING && beval->સંદેશ_CB != ખાલી)
	    {
		Modifiers   modifier;
		ચાવી_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		જો (keysym == XK_Shift_L || keysym == XK_Shift_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->સંદેશ_CB)(beval, ShiftMask);
		}
		નહીંતર જો (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->સંદેશ_CB)(beval, ControlMask);
		}
		નહીંતર
		    cancelBalloon(beval);
	    }
	    નહીંતર
		cancelBalloon(beval);
	    તોડો;

	સ્થિતિ ચાવી_Release:
	    જો (beval->showState == ShS_SHOWING && beval->સંદેશ_CB != ખાલી)
	    {
		Modifiers modifier;
		ચાવી_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		જો ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->સંદેશ_CB)(beval, 0);
		}
		નહીંતર જો ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->સંદેશ_CB)(beval, 0);
		}
		નહીંતર
		    cancelBalloon(beval);
	    }
	    નહીંતર
		cancelBalloon(beval);
	    તોડો;

	સ્થિતિ LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		જો (event->xcrossing.mode == NotifyNormal)
		    cancelBalloon(beval);
		તોડો;

	સ્થિતિ ButtonPress:
		cancelBalloon(beval);
		તોડો;

	મૂળભૂત:
	    તોડો;
    }
}

    સ્થિર શૂન્ય
timerRoutine(XtPointer dx, XtIntervalId *id UNUSED)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)ખાલી;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    સ્થિર શૂન્ય
requestBalloon(BalloonEval *beval)
{
    જો (beval->showState == ShS_PENDING)
	પાછા;

    // Determine the beval to display
    જો (beval->સંદેશ_CB != ખાલી)
    {
	beval->showState = ShS_PENDING;
	(*beval->સંદેશ_CB)(beval, beval->state);
    }
    નહીંતર જો (beval->સંદેશ != ખાલી)
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
    સ્થિર શૂન્ય
set_printable_label_text(GtkLabel *label, char_u *text)
{
    char_u	    *convbuf = ખાલી;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    unsigned પૂર્ણાંક    len;
    પૂર્ણાંક		    charlen;
    પૂર્ણાંક		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    જો (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, text, ખાલી);
	જો (convbuf != ખાલી)
	    text = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    માટે (p = text; *p != NUL; p += charlen)
    {
	જો ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	નહીંતર
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    જો (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    જો (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    નહીંતર જો (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    નહીંતર
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    જો (buf != ખાલી)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  જો GTK_CHECK_VERSION(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   જો PANGO_VERSION_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  નહીંતર
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != ખાલી) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	જો (pixel != INVALCOLOR)
#  જો GTK_CHECK_VERSION(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  નહીંતર
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (unsigned long)pixel, &color);
#  endif

	pdest = buf;
	p = text;
	જ્યારે (*p != NUL)
	{
	    // Be quick for ASCII
	    જો ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    નહીંતર
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		જો (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		જો (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    પૂર્ણાંક	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    જો (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    નહીંતર જો (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    નહીંતર
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    જો (pixel != INVALCOLOR)
		    {
#  જો GTK_CHECK_VERSION(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   જો PANGO_VERSION_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  નહીંતર
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->start_index = pdest - buf;
			attr->end_index   = pdest - buf + outlen;
			pango_attr_list_ઉમેરો(attr_list, attr);
#  જો GTK_CHECK_VERSION(3,0,0)
#   જો PANGO_VERSION_CHECK(1,38,0)
			attr_alpha->start_index = pdest - buf;
			attr_alpha->end_index   = pdest - buf + outlen;
			pango_attr_list_ઉમેરો(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		નહીંતર
		{
		    કરો
			*pdest++ = *p++;
		    જ્યારે (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_set_text(label, (અચલ અક્ષર *)buf);
    vim_free(buf);

    gtk_label_set_attributes(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    સ્થિર શૂન્ય
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    પૂર્ણાંક		    natural_width = 0;

    જો (beval->સંદેશ == ખાલી)
	પાછા;

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->સંદેશ);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    ખાલી, &natural_width, ખાલી, ખાલી);

    rect.width = (natural_width > 0 ? natural_width : 1) + gui.char_width * 3;
    rect.height = gui.char_height;

    gtk_popover_set_pointing_to(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  નહીંતર
    સ્થિર શૂન્ય
drawBalloon(BalloonEval *beval)
{
    જો (beval->સંદેશ == ખાલી)
	પાછા;

    GtkRequisition	requisition;
    પૂર્ણાંક		screen_w;
    પૂર્ણાંક		screen_h;
    પૂર્ણાંક		screen_x;
    પૂર્ણાંક		screen_y;
    પૂર્ણાંક		x;
    પૂર્ણાંક		y;
    પૂર્ણાંક		x_offset = EVAL_OFFSET_X;
    પૂર્ણાંક		y_offset = EVAL_OFFSET_Y;
    PangoLayout	*layout;

#   જો !GTK_CHECK_VERSION(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->target);
    gtk_window_set_screen(GTK_WINDOW(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_of_win(beval->target, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   જો !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->સંદેશ);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_WORD_CHAR
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
#   નહીંતર
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
#   endif
    pango_layout_set_width(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.અંક_cols * gui.char_width,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   જો GTK_CHECK_VERSION(3,0,0)
    gtk_widget_get_preferred_size(beval->balloonShell, &requisition, ખાલી);
#   નહીંતર
    gtk_widget_size_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_window_get_origin(gtk_widget_get_window(beval->target), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    જો (x + x_offset + requisition.width > screen_x + screen_w)
	y_offset += 15;
    જો (y + y_offset + requisition.height > screen_y + screen_h)
	y_offset = -requisition.height - EVAL_OFFSET_Y;

    // Sanitize values
    x = CLAMP(x + x_offset, 0,
	    MAX(0, screen_x + screen_w - requisition.width));
    y = CLAMP(y + y_offset, 0,
	    MAX(0, screen_y + screen_h - requisition.height));

    // Show the balloon
#   જો GTK_CHECK_VERSION(3,0,0)
    gtk_window_move(GTK_WINDOW(beval->balloonShell), x, y);
#   નહીંતર
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
    સ્થિર શૂન્ય
undrawBalloon(BalloonEval *beval)
{
    જો (beval->balloonShell != ખાલી)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  નહીંતર
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    સ્થિર શૂન્ય
cancelBalloon(BalloonEval *beval)
{
    જો (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    જો (beval->timerID != 0)
    {
	g_source_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    સ્થિર શૂન્ય
createBalloonEvalWindow(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_set_parent(beval->balloonShell, beval->target);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_set_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(ખાલી);

    gtk_label_set_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ડાબું);
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_set_child(GTK_POPOVER(popover), beval->balloonLabel);
}
#  નહીંતર
    સ્થિર શૂન્ય
createBalloonEvalWindow(BalloonEval *beval)
{
    beval->balloonShell = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_transient_for(GTK_WINDOW(beval->balloonShell), GTK_WINDOW(gui.mainwin));

    gtk_widget_set_app_paintable(beval->balloonShell, TRUE);
    gtk_window_set_resizable(GTK_WINDOW(beval->balloonShell), FALSE);
    gtk_widget_set_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_set_border_width(GTK_CONTAINER(beval->balloonShell), 4);

#   જો GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), ખાલી);
#   નહીંતર
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), ખાલી);
#   endif
    beval->balloonLabel = gtk_label_new(ખાલી);

    gtk_label_set_લીટી_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ડાબું);
#   જો GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_VERSION(3,14,0)
    GValue align_val = G_કિંમત_INIT;
    g_કિંમત_init(&align_val, G_TYPE_FLOAT);
    g_કિંમત_set_float(&align_val, 0.5);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_કિંમત_unset(&align_val);
#   નહીંતર
    gtk_misc_set_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_set_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_add(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# નહીંતર // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    સ્થિર શૂન્ય
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    જો (beval->સંદેશ == ખાલી)
	પાછા;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    જો (beval->સંદેશ_CB != ખાલી)
	s = XmStringCreateLtoR((અક્ષર *)beval->સંદેશ, XmFONTLIST_DEFAULT_TAG);
    નહીંતર
	s = XmStringCreateLocalized((અક્ષર *)beval->સંદેશ);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	જો (fl == ખાલી)
	{
	    XmStringFree(s);
	    પાછા;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_offset << 1;
    h += gui.border_offset << 1;
    Xt_Va_Set_કિંમતો(beval->balloonLabel, XmNlabelString, s, ખાલી);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_મૂળ + EVAL_OFFSET_X;
    ty = beval->y_મૂળ + EVAL_OFFSET_Y;
    જો ((tx + w) > beval->screen_width)
	tx = beval->screen_width - w;
    જો ((ty + h) > beval->screen_height)
	ty = beval->screen_height - h;
    Xt_Va_Set_કિંમતો(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    ખાલી);
    // Set tooltip colors
    {
	Arg args[2];

	args[0].name = XmNbackground;
	args[0].કિંમત = gui.tooltip_bg_pixel;
	args[1].name = XmNforeground;
	args[1].કિંમત = gui.tooltip_fg_pixel;
	Xt_Set_કિંમતો(beval->balloonLabel, &args[0], Xt_સંખ્યા(args));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    સ્થિર શૂન્ય
undrawBalloon(BalloonEval *beval)
{
    જો (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = ખાલી;
}

    સ્થિર શૂન્ય
cancelBalloon(BalloonEval *beval)
{
    જો (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    જો (beval->timerID != (XtIntervalId)ખાલી)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = (XtIntervalId)ખાલી;
    }
    beval->showState = ShS_NEUTRAL;
}


    સ્થિર શૂન્ય
createBalloonEvalWindow(BalloonEval *beval)
{
    Arg		args[12];
    પૂર્ણાંક		n;

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
