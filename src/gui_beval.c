/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#எனில் !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_VERSION(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  இல்லையெனில்
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# இல்லையெனில்
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
வெளி Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
நிலையான BalloonEval *current_beval = வெற்று;
# endif

# ifdef FEAT_GUI_GTK
நிலையான வெறுமை addEventHandler(GtkWidget *, BalloonEval *);
நிலையான வெறுமை removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
நிலையான gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  இல்லையெனில்
நிலையான gint target_event_cb(GtkWidget *, GdkEvent *, gpointer);
நிலையான gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
நிலையான வெறுமை pointer_event(BalloonEval *, முழு, முழு, unsigned);
நிலையான வெறுமை சாவி_event(BalloonEval *, unsigned, முழு);
நிலையான gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   எனில் GTK_CHECK_VERSION(3,0,0)
நிலையான gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   இல்லையெனில்
நிலையான gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# இல்லையெனில்
நிலையான வெறுமை addEventHandler(Widget, BalloonEval *);
நிலையான வெறுமை removeEventHandler(BalloonEval *);
நிலையான வெறுமை pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
நிலையான வெறுமை pointerEvent(BalloonEval *, XEvent *);
நிலையான வெறுமை timerRoutine(XtPointer, XtIntervalId *);
# endif
நிலையான வெறுமை cancelBalloon(BalloonEval *);
நிலையான வெறுமை requestBalloon(BalloonEval *);
நிலையான வெறுமை drawBalloon(BalloonEval *);
நிலையான வெறுமை undrawBalloon(BalloonEval *beval);
நிலையான வெறுமை createBalloonEvalWindow(BalloonEval *);

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
    வெறுமை	*target,
    char_u	*mesg,
    வெறுமை	(*mesgCB)(BalloonEval *, முழு),
    வெறுமை	*clientData)
{
# ifndef FEAT_GUI_GTK
    எழுத்து	*display_name;	    // get from gui.dpy
    முழு		screen_இலக்கம்;
    எழுத்து	*p;
# endif
    BalloonEval	*beval;

    எனில் (mesg != வெற்று && mesgCB != வெற்று)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	திரும்பு வெற்று;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    எனில் (beval != வெற்று)
    {
# ifdef FEAT_GUI_GTK
	beval->target = GTK_WIDGET(target);
# இல்லையெனில்
	beval->target = (Widget)target;
	beval->appContext = XtWidgetToApplicationContext((Widget)target);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->செய்தி);
	beval->செய்தி = mesg == வெற்று ? வெற்று : vim_strsave(mesg);
	beval->செய்தி_CB = mesgCB;
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
	எனில் (p != வெற்று)
	    screen_இலக்கம் = atoi(++p);
	இல்லையெனில்
	    screen_இலக்கம் = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_இலக்கம்);
	beval->screen_height = DisplayHeight(gui.dpy, screen_இலக்கம்);
# endif
    }

    திரும்பு beval;
}

# எனில் defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    வெறுமை
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   இல்லையெனில்
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  இல்லையெனில்
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    எனில் (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->செய்தி);
    vim_free(beval);
}
# endif

    வெறுமை
gui_mch_enable_beval_area(BalloonEval *beval)
{
    எனில் (beval != வெற்று)
	addEventHandler(beval->target, beval);
}

    வெறுமை
gui_mch_disable_beval_area(BalloonEval *beval)
{
    எனில் (beval != வெற்று)
	removeEventHandler(beval);
}

# எனில் defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(வெறுமை)
{
    திரும்பு current_beval;
}
# endif
#endif // !FEAT_GUI_MSWIN

#எனில் defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# எனில் !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    வெறுமை
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->செய்தி);
    beval->செய்தி = mesg == வெற்று ? வெற்று : vim_strsave(mesg);
    எனில் (beval->செய்தி != வெற்று)
	drawBalloon(beval);
    இல்லையெனில்
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#endif // FEAT_NETBEANS_INTG || FEAT_EVAL

#எனில் !defined(FEAT_GUI_MSWIN)
# எனில் defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    வெறுமை
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->செய்தி);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    நிலையான வெறுமை
addEventHandler(GtkWidget *target, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_set_data(G_OBJECT(target), "beval", controller);
    gtk_widget_add_controller(target, controller);
#  இல்லையெனில்
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
    எனில் (gtk_socket_id == 0 && gui.mainwin != வெற்று
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

    நிலையான வெறுமை
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*target = beval->target;
    GtkEventController	*controller;

    controller = g_object_get_data(G_OBJECT(target), "beval");
    எனில் (controller != வெற்று)
    {
	gtk_widget_remove_controller(target, controller);
	g_object_set_data(G_OBJECT(target), "beval", வெற்று);
    }
#  இல்லையெனில்
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->target),
					 FUNC2GENERIC(target_event_cb),
					 beval);

    எனில் (gtk_socket_id == 0 && gui.mainwin != வெற்று
	    && gtk_widget_is_ancestor(beval->target, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    நிலையான gboolean
event_cb(GtkEventController *controller UNUSED, GdkEvent *event, BalloonEval *beval)
{
    முழு type = gdk_event_get_event_type(event);

    தேர்வு (type)
    {
	நிலை GDK_ENTER_NOTIFY:
	நிலை GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*வேர்;
	    graphene_point_t	p;
	    graphene_point_t	result;
	    இரட்டை		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    வேர் = GTK_WIDGET(gtk_widget_get_வேர்(beval->target));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    எனில் (gtk_widget_compute_point(வேர், beval->target, &p, &result))
	    {
		x = result.x;
		y = result.y;
	    }

	    pointer_event(beval, (முழு)x, (முழு)y,
		    gdk_event_get_modifier_state(event));
	    நிறுத்து;
	}
	நிலை GDK_LEAVE_NOTIFY:
	    எனில் (gdk_crossing_event_get_mode(event) == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    நிறுத்து;
	நிலை GDK_BUTTON_PRESS:
	நிலை GDK_SCROLL:
	    cancelBalloon(beval);
	    நிறுத்து;
	நிலை GDK_சாவி_PRESS:
	நிலை GDK_சாவி_RELEASE:
	    சாவி_event(beval, gdk_சாவி_event_get_keyval(event),
		    type == GDK_சாவி_PRESS);
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
    }

    // Always pass through events?
    திரும்பு FALSE;
}
#  இல்லையெனில்
    நிலையான gint
target_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    தேர்வு (event->type)
    {
	நிலை GDK_ENTER_NOTIFY:
	    pointer_event(beval, (முழு)event->crossing.x,
				 (முழு)event->crossing.y,
				 event->crossing.state);
	    நிறுத்து;
	நிலை GDK_MOTION_NOTIFY:
	    எனில் (event->motion.is_hint)
	    {
		முழு		x;
		முழு		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   எனில் GTK_CHECK_VERSION(3,0,0)
		{
		    GdkWindow * மாறிலி win = gtk_widget_get_window(widget);
		    GdkDisplay * மாறிலி dpy = gdk_window_get_display(win);
#    எனில் GTK_CHECK_VERSION(3,20,0)
		    GdkSeat * மாறிலி seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * மாறிலி dev = gdk_seat_get_pointer(seat);
#    இல்லையெனில்
		    GdkDeviceManager * மாறிலி mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * மாறிலி dev = gdk_device_manager_get_client_pointer(mngr);
#    endif
		    gdk_window_get_device_position(win, dev , &x, &y, &state);
		}
#   இல்லையெனில்
		gdk_window_get_pointer(widget->window, &x, &y, &state);
#   endif
		pointer_event(beval, x, y, (unsigned முழு)state);
	    }
	    இல்லையெனில்
	    {
		pointer_event(beval, (முழு)event->motion.x,
				     (முழு)event->motion.y,
				     event->motion.state);
	    }
	    நிறுத்து;
	நிலை GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    எனில் (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    நிறுத்து;
	நிலை GDK_BUTTON_PRESS:
	நிலை GDK_SCROLL:
	    cancelBalloon(beval);
	    நிறுத்து;
	நிலை GDK_சாவி_PRESS:
	    சாவி_event(beval, event->சாவி.keyval, TRUE);
	    நிறுத்து;
	நிலை GDK_சாவி_RELEASE:
	    சாவி_event(beval, event->சாவி.keyval, FALSE);
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
    }

    திரும்பு FALSE; // continue emission
}

    நிலையான gint
mainwin_event_cb(GtkWidget *widget UNUSED, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    தேர்வு (event->type)
    {
	நிலை GDK_சாவி_PRESS:
	    சாவி_event(beval, event->சாவி.keyval, TRUE);
	    நிறுத்து;
	நிலை GDK_சாவி_RELEASE:
	    சாவி_event(beval, event->சாவி.keyval, FALSE);
	    நிறுத்து;
	நிலை GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    எனில் (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
    }

    திரும்பு FALSE; // continue emission
}
#  endif

    நிலையான வெறுமை
pointer_event(BalloonEval *beval, முழு x, முழு y, unsigned state)
{
    முழு distance;

    distance = ABS(x - beval->x) + ABS(y - beval->y);

    எனில் (distance <= 4)
	திரும்பு;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    எனில் (!(state & ((முழு)GDK_BUTTON1_MASK | (முழு)GDK_BUTTON2_MASK
		    | (முழு)GDK_BUTTON3_MASK)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	எனில் (state & (முழு)GDK_ALT_MASK)
#  இல்லையெனில்
	எனில் (state & (முழு)GDK_MOD1_MASK)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    எனில் (beval->செய்தி_CB != வெற்று)
	    {
		beval->showState = ShS_PENDING;
		(*beval->செய்தி_CB)(beval, state);
	    }
	}
	இல்லையெனில்
	{
	    beval->timerID = g_timeout_add((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    நிலையான வெறுமை
சாவி_event(BalloonEval *beval, unsigned keyval, முழு is_keypress)
{
    எனில் (beval->showState == ShS_SHOWING && beval->செய்தி_CB != வெற்று)
    {
	தேர்வு (keyval)
	{
#  ifdef USE_GTK4
	    நிலை GDK_சாவி_Shift_L:
	    நிலை GDK_சாவி_Shift_R:
#  இல்லையெனில்
	    நிலை GDK_Shift_L:
	    நிலை GDK_Shift_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->செய்தி_CB)(beval, (is_keypress)
						   ? (முழு)GDK_SHIFT_MASK : 0);
		நிறுத்து;
#  ifdef USE_GTK4
	    நிலை GDK_சாவி_Control_L:
	    நிலை GDK_சாவி_Control_R:
#  இல்லையெனில்
	    நிலை GDK_Control_L:
	    நிலை GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->செய்தி_CB)(beval, (is_keypress)
						 ? (முழு)GDK_CONTROL_MASK : 0);
		நிறுத்து;
	    இயல்பு:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		எனில் (is_keypress)
		    cancelBalloon(beval);
		நிறுத்து;
	}
    }
    இல்லையெனில்
	cancelBalloon(beval);
}

    நிலையான gboolean
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

    திரும்பு FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   எனில் GTK_CHECK_VERSION(3,0,0)
    நிலையான gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 data UNUSED)
{
    GtkStyleContext *context = வெற்று;
    gint width = -1, height = -1;

    எனில் (widget == வெற்று)
	திரும்பு TRUE;

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

    திரும்பு FALSE;
}
#   இல்லையெனில்
    நிலையான gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer data UNUSED)
{
    gtk_paint_flat_box(widget->style, widget->window,
		       GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    திரும்பு FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# இல்லையெனில் // !FEAT_GUI_GTK

    நிலையான வெறுமை
addEventHandler(Widget target, BalloonEval *beval)
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | சாவி_Press_Mask |
			சாவி_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}

    நிலையான வெறுமை
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | சாவி_Press_Mask |
			சாவி_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    நிலையான வெறுமை
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

    நிலையான வெறுமை
pointerEvent(BalloonEval *beval, XEvent *event)
{
    Position	distance;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    தேர்வு (event->type)
    {
	நிலை EnterNotify:
	நிலை MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    எனில் (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    எனில் (delta < 0)
		delta = -delta;
	    distance += delta;
	    எனில் (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		எனில் (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		இல்லையெனில் எனில் (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_வேர் = event->xmotion.x_வேர்;
		    beval->y_வேர் = event->xmotion.y_வேர்;
		    cancelBalloon(beval);
		    எனில் (beval->செய்தி_CB != வெற்று)
		    {
			beval->showState = ShS_PENDING;
			(*beval->செய்தி_CB)(beval, beval->state);
		    }
		}
		இல்லையெனில்
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_வேர் = event->xmotion.x_வேர்;
		    beval->y_வேர் = event->xmotion.y_வேர்;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    நிறுத்து;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	நிலை சாவி_Press:
	    எனில் (beval->showState == ShS_SHOWING && beval->செய்தி_CB != வெற்று)
	    {
		Modifiers   modifier;
		சாவி_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		எனில் (keysym == XK_Shift_L || keysym == XK_Shift_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->செய்தி_CB)(beval, ShiftMask);
		}
		இல்லையெனில் எனில் (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->செய்தி_CB)(beval, ControlMask);
		}
		இல்லையெனில்
		    cancelBalloon(beval);
	    }
	    இல்லையெனில்
		cancelBalloon(beval);
	    நிறுத்து;

	நிலை சாவி_Release:
	    எனில் (beval->showState == ShS_SHOWING && beval->செய்தி_CB != வெற்று)
	    {
		Modifiers modifier;
		சாவி_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		எனில் ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->செய்தி_CB)(beval, 0);
		}
		இல்லையெனில் எனில் ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->செய்தி_CB)(beval, 0);
		}
		இல்லையெனில்
		    cancelBalloon(beval);
	    }
	    இல்லையெனில்
		cancelBalloon(beval);
	    நிறுத்து;

	நிலை LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		எனில் (event->xcrossing.mode == NotifyNormal)
		    cancelBalloon(beval);
		நிறுத்து;

	நிலை ButtonPress:
		cancelBalloon(beval);
		நிறுத்து;

	இயல்பு:
	    நிறுத்து;
    }
}

    நிலையான வெறுமை
timerRoutine(XtPointer dx, XtIntervalId *id UNUSED)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)வெற்று;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    நிலையான வெறுமை
requestBalloon(BalloonEval *beval)
{
    எனில் (beval->showState == ShS_PENDING)
	திரும்பு;

    // Determine the beval to display
    எனில் (beval->செய்தி_CB != வெற்று)
    {
	beval->showState = ShS_PENDING;
	(*beval->செய்தி_CB)(beval, beval->state);
    }
    இல்லையெனில் எனில் (beval->செய்தி != வெற்று)
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
    நிலையான வெறுமை
set_printable_label_text(GtkLabel *label, char_u *text)
{
    char_u	    *convbuf = வெற்று;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    unsigned முழு    len;
    முழு		    charlen;
    முழு		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    எனில் (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, text, வெற்று);
	எனில் (convbuf != வெற்று)
	    text = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    க்கு (p = text; *p != NUL; p += charlen)
    {
	எனில் ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	இல்லையெனில்
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    எனில் (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    எனில் (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    இல்லையெனில் எனில் (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    இல்லையெனில்
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    எனில் (buf != வெற்று)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  எனில் GTK_CHECK_VERSION(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   எனில் PANGO_VERSION_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  இல்லையெனில்
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != வெற்று) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	எனில் (pixel != INVALCOLOR)
#  எனில் GTK_CHECK_VERSION(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  இல்லையெனில்
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (unsigned long)pixel, &color);
#  endif

	pdest = buf;
	p = text;
	போது (*p != NUL)
	{
	    // Be quick for ASCII
	    எனில் ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    இல்லையெனில்
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		எனில் (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		எனில் (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    முழு	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    எனில் (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    இல்லையெனில் எனில் (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    இல்லையெனில்
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    எனில் (pixel != INVALCOLOR)
		    {
#  எனில் GTK_CHECK_VERSION(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   எனில் PANGO_VERSION_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  இல்லையெனில்
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->start_index = pdest - buf;
			attr->end_index   = pdest - buf + outlen;
			pango_attr_list_செருகு(attr_list, attr);
#  எனில் GTK_CHECK_VERSION(3,0,0)
#   எனில் PANGO_VERSION_CHECK(1,38,0)
			attr_alpha->start_index = pdest - buf;
			attr_alpha->end_index   = pdest - buf + outlen;
			pango_attr_list_செருகு(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		இல்லையெனில்
		{
		    செய்
			*pdest++ = *p++;
		    போது (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_set_text(label, (மாறிலி எழுத்து *)buf);
    vim_free(buf);

    gtk_label_set_attributes(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    நிலையான வெறுமை
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    முழு		    natural_width = 0;

    எனில் (beval->செய்தி == வெற்று)
	திரும்பு;

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->செய்தி);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    வெற்று, &natural_width, வெற்று, வெற்று);

    rect.width = (natural_width > 0 ? natural_width : 1) + gui.char_width * 3;
    rect.height = gui.char_height;

    gtk_popover_set_pointing_to(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  இல்லையெனில்
    நிலையான வெறுமை
drawBalloon(BalloonEval *beval)
{
    எனில் (beval->செய்தி == வெற்று)
	திரும்பு;

    GtkRequisition	requisition;
    முழு		screen_w;
    முழு		screen_h;
    முழு		screen_x;
    முழு		screen_y;
    முழு		x;
    முழு		y;
    முழு		x_offset = EVAL_OFFSET_X;
    முழு		y_offset = EVAL_OFFSET_Y;
    PangoLayout	*layout;

#   எனில் !GTK_CHECK_VERSION(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->target);
    gtk_window_set_screen(GTK_WINDOW(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_of_win(beval->target, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   எனில் !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->செய்தி);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_WORD_CHAR
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
#   இல்லையெனில்
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
#   endif
    pango_layout_set_width(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.இலக்கம்_cols * gui.char_width,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   எனில் GTK_CHECK_VERSION(3,0,0)
    gtk_widget_get_preferred_size(beval->balloonShell, &requisition, வெற்று);
#   இல்லையெனில்
    gtk_widget_size_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_window_get_origin(gtk_widget_get_window(beval->target), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    எனில் (x + x_offset + requisition.width > screen_x + screen_w)
	y_offset += 15;
    எனில் (y + y_offset + requisition.height > screen_y + screen_h)
	y_offset = -requisition.height - EVAL_OFFSET_Y;

    // Sanitize values
    x = CLAMP(x + x_offset, 0,
	    MAX(0, screen_x + screen_w - requisition.width));
    y = CLAMP(y + y_offset, 0,
	    MAX(0, screen_y + screen_h - requisition.height));

    // Show the balloon
#   எனில் GTK_CHECK_VERSION(3,0,0)
    gtk_window_move(GTK_WINDOW(beval->balloonShell), x, y);
#   இல்லையெனில்
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
    நிலையான வெறுமை
undrawBalloon(BalloonEval *beval)
{
    எனில் (beval->balloonShell != வெற்று)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  இல்லையெனில்
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    நிலையான வெறுமை
cancelBalloon(BalloonEval *beval)
{
    எனில் (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    எனில் (beval->timerID != 0)
    {
	g_source_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    நிலையான வெறுமை
createBalloonEvalWindow(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_set_parent(beval->balloonShell, beval->target);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_set_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(வெற்று);

    gtk_label_set_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_இடது);
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_set_child(GTK_POPOVER(popover), beval->balloonLabel);
}
#  இல்லையெனில்
    நிலையான வெறுமை
createBalloonEvalWindow(BalloonEval *beval)
{
    beval->balloonShell = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_transient_for(GTK_WINDOW(beval->balloonShell), GTK_WINDOW(gui.mainwin));

    gtk_widget_set_app_paintable(beval->balloonShell, TRUE);
    gtk_window_set_resizable(GTK_WINDOW(beval->balloonShell), FALSE);
    gtk_widget_set_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_set_border_width(GTK_CONTAINER(beval->balloonShell), 4);

#   எனில் GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), வெற்று);
#   இல்லையெனில்
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), வெற்று);
#   endif
    beval->balloonLabel = gtk_label_new(வெற்று);

    gtk_label_set_வரி_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_இடது);
#   எனில் GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_VERSION(3,14,0)
    GValue align_val = G_மதிப்பு_INIT;
    g_மதிப்பு_init(&align_val, G_TYPE_FLOAT);
    g_மதிப்பு_set_float(&align_val, 0.5);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_மதிப்பு_unset(&align_val);
#   இல்லையெனில்
    gtk_misc_set_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_set_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_add(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# இல்லையெனில் // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    நிலையான வெறுமை
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    எனில் (beval->செய்தி == வெற்று)
	திரும்பு;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    எனில் (beval->செய்தி_CB != வெற்று)
	s = XmStringCreateLtoR((எழுத்து *)beval->செய்தி, XmFONTLIST_DEFAULT_TAG);
    இல்லையெனில்
	s = XmStringCreateLocalized((எழுத்து *)beval->செய்தி);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	எனில் (fl == வெற்று)
	{
	    XmStringFree(s);
	    திரும்பு;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_offset << 1;
    h += gui.border_offset << 1;
    Xt_Va_Set_மதிப்புகள்(beval->balloonLabel, XmNlabelString, s, வெற்று);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_வேர் + EVAL_OFFSET_X;
    ty = beval->y_வேர் + EVAL_OFFSET_Y;
    எனில் ((tx + w) > beval->screen_width)
	tx = beval->screen_width - w;
    எனில் ((ty + h) > beval->screen_height)
	ty = beval->screen_height - h;
    Xt_Va_Set_மதிப்புகள்(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    வெற்று);
    // Set tooltip colors
    {
	Arg args[2];

	args[0].name = XmNbackground;
	args[0].மதிப்பு = gui.tooltip_bg_pixel;
	args[1].name = XmNforeground;
	args[1].மதிப்பு = gui.tooltip_fg_pixel;
	Xt_Set_மதிப்புகள்(beval->balloonLabel, &args[0], Xt_எண்(args));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    நிலையான வெறுமை
undrawBalloon(BalloonEval *beval)
{
    எனில் (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = வெற்று;
}

    நிலையான வெறுமை
cancelBalloon(BalloonEval *beval)
{
    எனில் (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    எனில் (beval->timerID != (XtIntervalId)வெற்று)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = (XtIntervalId)வெற்று;
    }
    beval->showState = ShS_NEUTRAL;
}


    நிலையான வெறுமை
createBalloonEvalWindow(BalloonEval *beval)
{
    Arg		args[12];
    முழு		n;

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
