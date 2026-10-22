/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#যদি !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_VERSION(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  অন্যথা
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# অন্যথা
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
বাহ্য Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
স্থির BalloonEval *current_beval = রিক্ত;
# endif

# ifdef FEAT_GUI_GTK
স্থির শূন্য addEventHandler(GtkWidget *, BalloonEval *);
স্থির শূন্য removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
স্থির gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  অন্যথা
স্থির gint target_event_cb(GtkWidget *, GdkEvent *, gpointer);
স্থির gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
স্থির শূন্য pointer_event(BalloonEval *, পূর্ণাঙ্ক, পূর্ণাঙ্ক, unsigned);
স্থির শূন্য চাবি_event(BalloonEval *, unsigned, পূর্ণাঙ্ক);
স্থির gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   যদি GTK_CHECK_VERSION(3,0,0)
স্থির gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   অন্যথা
স্থির gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# অন্যথা
স্থির শূন্য addEventHandler(Widget, BalloonEval *);
স্থির শূন্য removeEventHandler(BalloonEval *);
স্থির শূন্য pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
স্থির শূন্য pointerEvent(BalloonEval *, XEvent *);
স্থির শূন্য timerRoutine(XtPointer, XtIntervalId *);
# endif
স্থির শূন্য cancelBalloon(BalloonEval *);
স্থির শূন্য requestBalloon(BalloonEval *);
স্থির শূন্য drawBalloon(BalloonEval *);
স্থির শূন্য undrawBalloon(BalloonEval *beval);
স্থির শূন্য createBalloonEvalWindow(BalloonEval *);

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
    শূন্য	*target,
    char_u	*mesg,
    শূন্য	(*mesgCB)(BalloonEval *, পূর্ণাঙ্ক),
    শূন্য	*clientData)
{
# ifndef FEAT_GUI_GTK
    বর্ণ	*display_name;	    // get from gui.dpy
    পূর্ণাঙ্ক		screen_অঙ্ক;
    বর্ণ	*p;
# endif
    BalloonEval	*beval;

    যদি (mesg != রিক্ত && mesgCB != রিক্ত)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	প্রত্যাগম রিক্ত;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    যদি (beval != রিক্ত)
    {
# ifdef FEAT_GUI_GTK
	beval->target = GTK_WIDGET(target);
# অন্যথা
	beval->target = (Widget)target;
	beval->appContext = XtWidgetToApplicationContext((Widget)target);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->বার্তা);
	beval->বার্তা = mesg == রিক্ত ? রিক্ত : vim_strsave(mesg);
	beval->বার্তা_CB = mesgCB;
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
	যদি (p != রিক্ত)
	    screen_অঙ্ক = atoi(++p);
	অন্যথা
	    screen_অঙ্ক = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_অঙ্ক);
	beval->screen_height = DisplayHeight(gui.dpy, screen_অঙ্ক);
# endif
    }

    প্রত্যাগম beval;
}

# যদি defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    শূন্য
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   অন্যথা
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  অন্যথা
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    যদি (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->বার্তা);
    vim_free(beval);
}
# endif

    শূন্য
gui_mch_enable_beval_area(BalloonEval *beval)
{
    যদি (beval != রিক্ত)
	addEventHandler(beval->target, beval);
}

    শূন্য
gui_mch_disable_beval_area(BalloonEval *beval)
{
    যদি (beval != রিক্ত)
	removeEventHandler(beval);
}

# যদি defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(শূন্য)
{
    প্রত্যাগম current_beval;
}
# endif
#endif // !FEAT_GUI_MSWIN

#যদি defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# যদি !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    শূন্য
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->বার্তা);
    beval->বার্তা = mesg == রিক্ত ? রিক্ত : vim_strsave(mesg);
    যদি (beval->বার্তা != রিক্ত)
	drawBalloon(beval);
    অন্যথা
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#endif // FEAT_NETBEANS_INTG || FEAT_EVAL

#যদি !defined(FEAT_GUI_MSWIN)
# যদি defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    শূন্য
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->বার্তা);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    স্থির শূন্য
addEventHandler(GtkWidget *target, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_set_data(G_OBJECT(target), "beval", controller);
    gtk_widget_add_controller(target, controller);
#  অন্যথা
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
    যদি (gtk_socket_id == 0 && gui.mainwin != রিক্ত
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

    স্থির শূন্য
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*target = beval->target;
    GtkEventController	*controller;

    controller = g_object_get_data(G_OBJECT(target), "beval");
    যদি (controller != রিক্ত)
    {
	gtk_widget_remove_controller(target, controller);
	g_object_set_data(G_OBJECT(target), "beval", রিক্ত);
    }
#  অন্যথা
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->target),
					 FUNC2GENERIC(target_event_cb),
					 beval);

    যদি (gtk_socket_id == 0 && gui.mainwin != রিক্ত
	    && gtk_widget_is_ancestor(beval->target, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    স্থির gboolean
event_cb(GtkEventController *controller UNUSED, GdkEvent *event, BalloonEval *beval)
{
    পূর্ণাঙ্ক type = gdk_event_get_event_type(event);

    বিকল্প (type)
    {
	পক্ষ GDK_ENTER_NOTIFY:
	পক্ষ GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*মূল;
	    graphene_point_t	p;
	    graphene_point_t	result;
	    দ্বিগুণ		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    মূল = GTK_WIDGET(gtk_widget_get_মূল(beval->target));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    যদি (gtk_widget_compute_point(মূল, beval->target, &p, &result))
	    {
		x = result.x;
		y = result.y;
	    }

	    pointer_event(beval, (পূর্ণাঙ্ক)x, (পূর্ণাঙ্ক)y,
		    gdk_event_get_modifier_state(event));
	    ভঙ্গ;
	}
	পক্ষ GDK_LEAVE_NOTIFY:
	    যদি (gdk_crossing_event_get_mode(event) == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    ভঙ্গ;
	পক্ষ GDK_BUTTON_PRESS:
	পক্ষ GDK_SCROLL:
	    cancelBalloon(beval);
	    ভঙ্গ;
	পক্ষ GDK_চাবি_PRESS:
	পক্ষ GDK_চাবি_RELEASE:
	    চাবি_event(beval, gdk_চাবি_event_get_keyval(event),
		    type == GDK_চাবি_PRESS);
	    ভঙ্গ;
	স্বভাব:
	    ভঙ্গ;
    }

    // Always pass through events?
    প্রত্যাগম FALSE;
}
#  অন্যথা
    স্থির gint
target_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    বিকল্প (event->type)
    {
	পক্ষ GDK_ENTER_NOTIFY:
	    pointer_event(beval, (পূর্ণাঙ্ক)event->crossing.x,
				 (পূর্ণাঙ্ক)event->crossing.y,
				 event->crossing.state);
	    ভঙ্গ;
	পক্ষ GDK_MOTION_NOTIFY:
	    যদি (event->motion.is_hint)
	    {
		পূর্ণাঙ্ক		x;
		পূর্ণাঙ্ক		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   যদি GTK_CHECK_VERSION(3,0,0)
		{
		    GdkWindow * ধ্রুব win = gtk_widget_get_window(widget);
		    GdkDisplay * ধ্রুব dpy = gdk_window_get_display(win);
#    যদি GTK_CHECK_VERSION(3,20,0)
		    GdkSeat * ধ্রুব seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * ধ্রুব dev = gdk_seat_get_pointer(seat);
#    অন্যথা
		    GdkDeviceManager * ধ্রুব mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * ধ্রুব dev = gdk_device_manager_get_client_pointer(mngr);
#    endif
		    gdk_window_get_device_position(win, dev , &x, &y, &state);
		}
#   অন্যথা
		gdk_window_get_pointer(widget->window, &x, &y, &state);
#   endif
		pointer_event(beval, x, y, (unsigned পূর্ণাঙ্ক)state);
	    }
	    অন্যথা
	    {
		pointer_event(beval, (পূর্ণাঙ্ক)event->motion.x,
				     (পূর্ণাঙ্ক)event->motion.y,
				     event->motion.state);
	    }
	    ভঙ্গ;
	পক্ষ GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    যদি (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    ভঙ্গ;
	পক্ষ GDK_BUTTON_PRESS:
	পক্ষ GDK_SCROLL:
	    cancelBalloon(beval);
	    ভঙ্গ;
	পক্ষ GDK_চাবি_PRESS:
	    চাবি_event(beval, event->চাবি.keyval, TRUE);
	    ভঙ্গ;
	পক্ষ GDK_চাবি_RELEASE:
	    চাবি_event(beval, event->চাবি.keyval, FALSE);
	    ভঙ্গ;
	স্বভাব:
	    ভঙ্গ;
    }

    প্রত্যাগম FALSE; // continue emission
}

    স্থির gint
mainwin_event_cb(GtkWidget *widget UNUSED, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    বিকল্প (event->type)
    {
	পক্ষ GDK_চাবি_PRESS:
	    চাবি_event(beval, event->চাবি.keyval, TRUE);
	    ভঙ্গ;
	পক্ষ GDK_চাবি_RELEASE:
	    চাবি_event(beval, event->চাবি.keyval, FALSE);
	    ভঙ্গ;
	পক্ষ GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    যদি (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    ভঙ্গ;
	স্বভাব:
	    ভঙ্গ;
    }

    প্রত্যাগম FALSE; // continue emission
}
#  endif

    স্থির শূন্য
pointer_event(BalloonEval *beval, পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, unsigned state)
{
    পূর্ণাঙ্ক distance;

    distance = ABS(x - beval->x) + ABS(y - beval->y);

    যদি (distance <= 4)
	প্রত্যাগম;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    যদি (!(state & ((পূর্ণাঙ্ক)GDK_BUTTON1_MASK | (পূর্ণাঙ্ক)GDK_BUTTON2_MASK
		    | (পূর্ণাঙ্ক)GDK_BUTTON3_MASK)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	যদি (state & (পূর্ণাঙ্ক)GDK_ALT_MASK)
#  অন্যথা
	যদি (state & (পূর্ণাঙ্ক)GDK_MOD1_MASK)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    যদি (beval->বার্তা_CB != রিক্ত)
	    {
		beval->showState = ShS_PENDING;
		(*beval->বার্তা_CB)(beval, state);
	    }
	}
	অন্যথা
	{
	    beval->timerID = g_timeout_add((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    স্থির শূন্য
চাবি_event(BalloonEval *beval, unsigned keyval, পূর্ণাঙ্ক is_keypress)
{
    যদি (beval->showState == ShS_SHOWING && beval->বার্তা_CB != রিক্ত)
    {
	বিকল্প (keyval)
	{
#  ifdef USE_GTK4
	    পক্ষ GDK_চাবি_Shift_L:
	    পক্ষ GDK_চাবি_Shift_R:
#  অন্যথা
	    পক্ষ GDK_Shift_L:
	    পক্ষ GDK_Shift_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->বার্তা_CB)(beval, (is_keypress)
						   ? (পূর্ণাঙ্ক)GDK_SHIFT_MASK : 0);
		ভঙ্গ;
#  ifdef USE_GTK4
	    পক্ষ GDK_চাবি_Control_L:
	    পক্ষ GDK_চাবি_Control_R:
#  অন্যথা
	    পক্ষ GDK_Control_L:
	    পক্ষ GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->বার্তা_CB)(beval, (is_keypress)
						 ? (পূর্ণাঙ্ক)GDK_CONTROL_MASK : 0);
		ভঙ্গ;
	    স্বভাব:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		যদি (is_keypress)
		    cancelBalloon(beval);
		ভঙ্গ;
	}
    }
    অন্যথা
	cancelBalloon(beval);
}

    স্থির gboolean
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

    প্রত্যাগম FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   যদি GTK_CHECK_VERSION(3,0,0)
    স্থির gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 data UNUSED)
{
    GtkStyleContext *context = রিক্ত;
    gint width = -1, height = -1;

    যদি (widget == রিক্ত)
	প্রত্যাগম TRUE;

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

    প্রত্যাগম FALSE;
}
#   অন্যথা
    স্থির gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer data UNUSED)
{
    gtk_paint_flat_box(widget->style, widget->window,
		       GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    প্রত্যাগম FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# অন্যথা // !FEAT_GUI_GTK

    স্থির শূন্য
addEventHandler(Widget target, BalloonEval *beval)
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | চাবি_Press_Mask |
			চাবি_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}

    স্থির শূন্য
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | চাবি_Press_Mask |
			চাবি_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    স্থির শূন্য
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

    স্থির শূন্য
pointerEvent(BalloonEval *beval, XEvent *event)
{
    Position	distance;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    বিকল্প (event->type)
    {
	পক্ষ EnterNotify:
	পক্ষ MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    যদি (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    যদি (delta < 0)
		delta = -delta;
	    distance += delta;
	    যদি (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		যদি (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		অন্যথা যদি (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_মূল = event->xmotion.x_মূল;
		    beval->y_মূল = event->xmotion.y_মূল;
		    cancelBalloon(beval);
		    যদি (beval->বার্তা_CB != রিক্ত)
		    {
			beval->showState = ShS_PENDING;
			(*beval->বার্তা_CB)(beval, beval->state);
		    }
		}
		অন্যথা
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_মূল = event->xmotion.x_মূল;
		    beval->y_মূল = event->xmotion.y_মূল;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    ভঙ্গ;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	পক্ষ চাবি_Press:
	    যদি (beval->showState == ShS_SHOWING && beval->বার্তা_CB != রিক্ত)
	    {
		Modifiers   modifier;
		চাবি_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		যদি (keysym == XK_Shift_L || keysym == XK_Shift_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->বার্তা_CB)(beval, ShiftMask);
		}
		অন্যথা যদি (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->বার্তা_CB)(beval, ControlMask);
		}
		অন্যথা
		    cancelBalloon(beval);
	    }
	    অন্যথা
		cancelBalloon(beval);
	    ভঙ্গ;

	পক্ষ চাবি_Release:
	    যদি (beval->showState == ShS_SHOWING && beval->বার্তা_CB != রিক্ত)
	    {
		Modifiers modifier;
		চাবি_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		যদি ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->বার্তা_CB)(beval, 0);
		}
		অন্যথা যদি ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->বার্তা_CB)(beval, 0);
		}
		অন্যথা
		    cancelBalloon(beval);
	    }
	    অন্যথা
		cancelBalloon(beval);
	    ভঙ্গ;

	পক্ষ LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		যদি (event->xcrossing.mode == NotifyNormal)
		    cancelBalloon(beval);
		ভঙ্গ;

	পক্ষ ButtonPress:
		cancelBalloon(beval);
		ভঙ্গ;

	স্বভাব:
	    ভঙ্গ;
    }
}

    স্থির শূন্য
timerRoutine(XtPointer dx, XtIntervalId *id UNUSED)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)রিক্ত;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    স্থির শূন্য
requestBalloon(BalloonEval *beval)
{
    যদি (beval->showState == ShS_PENDING)
	প্রত্যাগম;

    // Determine the beval to display
    যদি (beval->বার্তা_CB != রিক্ত)
    {
	beval->showState = ShS_PENDING;
	(*beval->বার্তা_CB)(beval, beval->state);
    }
    অন্যথা যদি (beval->বার্তা != রিক্ত)
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
    স্থির শূন্য
set_printable_label_text(GtkLabel *label, char_u *text)
{
    char_u	    *convbuf = রিক্ত;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    unsigned পূর্ণাঙ্ক    len;
    পূর্ণাঙ্ক		    charlen;
    পূর্ণাঙ্ক		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    যদি (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, text, রিক্ত);
	যদি (convbuf != রিক্ত)
	    text = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    প্রতি (p = text; *p != NUL; p += charlen)
    {
	যদি ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	অন্যথা
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    যদি (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    যদি (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    অন্যথা যদি (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    অন্যথা
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    যদি (buf != রিক্ত)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  যদি GTK_CHECK_VERSION(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   যদি PANGO_VERSION_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  অন্যথা
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != রিক্ত) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	যদি (pixel != INVALCOLOR)
#  যদি GTK_CHECK_VERSION(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  অন্যথা
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (unsigned long)pixel, &color);
#  endif

	pdest = buf;
	p = text;
	যাবত্ (*p != NUL)
	{
	    // Be quick for ASCII
	    যদি ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    অন্যথা
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		যদি (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		যদি (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    পূর্ণাঙ্ক	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    যদি (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    অন্যথা যদি (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    অন্যথা
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    যদি (pixel != INVALCOLOR)
		    {
#  যদি GTK_CHECK_VERSION(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   যদি PANGO_VERSION_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  অন্যথা
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->start_index = pdest - buf;
			attr->end_index   = pdest - buf + outlen;
			pango_attr_list_ঢোকাও(attr_list, attr);
#  যদি GTK_CHECK_VERSION(3,0,0)
#   যদি PANGO_VERSION_CHECK(1,38,0)
			attr_alpha->start_index = pdest - buf;
			attr_alpha->end_index   = pdest - buf + outlen;
			pango_attr_list_ঢোকাও(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		অন্যথা
		{
		    কুরু
			*pdest++ = *p++;
		    যাবত্ (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_set_text(label, (ধ্রুব বর্ণ *)buf);
    vim_free(buf);

    gtk_label_set_attributes(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    স্থির শূন্য
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    পূর্ণাঙ্ক		    natural_width = 0;

    যদি (beval->বার্তা == রিক্ত)
	প্রত্যাগম;

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->বার্তা);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    রিক্ত, &natural_width, রিক্ত, রিক্ত);

    rect.width = (natural_width > 0 ? natural_width : 1) + gui.char_width * 3;
    rect.height = gui.char_height;

    gtk_popover_set_pointing_to(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  অন্যথা
    স্থির শূন্য
drawBalloon(BalloonEval *beval)
{
    যদি (beval->বার্তা == রিক্ত)
	প্রত্যাগম;

    GtkRequisition	requisition;
    পূর্ণাঙ্ক		screen_w;
    পূর্ণাঙ্ক		screen_h;
    পূর্ণাঙ্ক		screen_x;
    পূর্ণাঙ্ক		screen_y;
    পূর্ণাঙ্ক		x;
    পূর্ণাঙ্ক		y;
    পূর্ণাঙ্ক		x_offset = EVAL_OFFSET_X;
    পূর্ণাঙ্ক		y_offset = EVAL_OFFSET_Y;
    PangoLayout	*layout;

#   যদি !GTK_CHECK_VERSION(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->target);
    gtk_window_set_screen(GTK_WINDOW(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_of_win(beval->target, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   যদি !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->বার্তা);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_WORD_CHAR
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
#   অন্যথা
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
#   endif
    pango_layout_set_width(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.অঙ্ক_cols * gui.char_width,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   যদি GTK_CHECK_VERSION(3,0,0)
    gtk_widget_get_preferred_size(beval->balloonShell, &requisition, রিক্ত);
#   অন্যথা
    gtk_widget_size_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_window_get_origin(gtk_widget_get_window(beval->target), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    যদি (x + x_offset + requisition.width > screen_x + screen_w)
	y_offset += 15;
    যদি (y + y_offset + requisition.height > screen_y + screen_h)
	y_offset = -requisition.height - EVAL_OFFSET_Y;

    // Sanitize values
    x = CLAMP(x + x_offset, 0,
	    MAX(0, screen_x + screen_w - requisition.width));
    y = CLAMP(y + y_offset, 0,
	    MAX(0, screen_y + screen_h - requisition.height));

    // Show the balloon
#   যদি GTK_CHECK_VERSION(3,0,0)
    gtk_window_move(GTK_WINDOW(beval->balloonShell), x, y);
#   অন্যথা
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
    স্থির শূন্য
undrawBalloon(BalloonEval *beval)
{
    যদি (beval->balloonShell != রিক্ত)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  অন্যথা
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    স্থির শূন্য
cancelBalloon(BalloonEval *beval)
{
    যদি (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    যদি (beval->timerID != 0)
    {
	g_source_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    স্থির শূন্য
createBalloonEvalWindow(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_set_parent(beval->balloonShell, beval->target);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_set_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(রিক্ত);

    gtk_label_set_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_বাম);
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_set_child(GTK_POPOVER(popover), beval->balloonLabel);
}
#  অন্যথা
    স্থির শূন্য
createBalloonEvalWindow(BalloonEval *beval)
{
    beval->balloonShell = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_transient_for(GTK_WINDOW(beval->balloonShell), GTK_WINDOW(gui.mainwin));

    gtk_widget_set_app_paintable(beval->balloonShell, TRUE);
    gtk_window_set_resizable(GTK_WINDOW(beval->balloonShell), FALSE);
    gtk_widget_set_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_set_border_width(GTK_CONTAINER(beval->balloonShell), 4);

#   যদি GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), রিক্ত);
#   অন্যথা
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), রিক্ত);
#   endif
    beval->balloonLabel = gtk_label_new(রিক্ত);

    gtk_label_set_সারি_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_বাম);
#   যদি GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_VERSION(3,14,0)
    GValue align_val = G_মান_INIT;
    g_মান_init(&align_val, G_TYPE_FLOAT);
    g_মান_set_float(&align_val, 0.5);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_মান_unset(&align_val);
#   অন্যথা
    gtk_misc_set_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_set_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_add(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# অন্যথা // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    স্থির শূন্য
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    যদি (beval->বার্তা == রিক্ত)
	প্রত্যাগম;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    যদি (beval->বার্তা_CB != রিক্ত)
	s = XmStringCreateLtoR((বর্ণ *)beval->বার্তা, XmFONTLIST_DEFAULT_TAG);
    অন্যথা
	s = XmStringCreateLocalized((বর্ণ *)beval->বার্তা);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	যদি (fl == রিক্ত)
	{
	    XmStringFree(s);
	    প্রত্যাগম;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_offset << 1;
    h += gui.border_offset << 1;
    Xt_Va_Set_মানসমূহ(beval->balloonLabel, XmNlabelString, s, রিক্ত);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_মূল + EVAL_OFFSET_X;
    ty = beval->y_মূল + EVAL_OFFSET_Y;
    যদি ((tx + w) > beval->screen_width)
	tx = beval->screen_width - w;
    যদি ((ty + h) > beval->screen_height)
	ty = beval->screen_height - h;
    Xt_Va_Set_মানসমূহ(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    রিক্ত);
    // Set tooltip colors
    {
	Arg args[2];

	args[0].name = XmNbackground;
	args[0].মান = gui.tooltip_bg_pixel;
	args[1].name = XmNforeground;
	args[1].মান = gui.tooltip_fg_pixel;
	Xt_Set_মানসমূহ(beval->balloonLabel, &args[0], Xt_সংখ্যা(args));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    স্থির শূন্য
undrawBalloon(BalloonEval *beval)
{
    যদি (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = রিক্ত;
}

    স্থির শূন্য
cancelBalloon(BalloonEval *beval)
{
    যদি (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    যদি (beval->timerID != (XtIntervalId)রিক্ত)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = (XtIntervalId)রিক্ত;
    }
    beval->showState = ShS_NEUTRAL;
}


    স্থির শূন্য
createBalloonEvalWindow(BalloonEval *beval)
{
    Arg		args[12];
    পূর্ণাঙ্ক		n;

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
