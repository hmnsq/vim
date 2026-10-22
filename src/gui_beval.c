/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

#اگر defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#اگر !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_VERSION(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  وگرنه
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# وگرنه
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
بیرونی Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
ایستا BalloonEval *current_beval = پوچ;
# endif

# ifdef FEAT_GUI_GTK
ایستا تهی addEventHandler(GtkWidget *, BalloonEval *);
ایستا تهی removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
ایستا gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  وگرنه
ایستا gint target_event_cb(GtkWidget *, GdkEvent *, gpointer);
ایستا gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
ایستا تهی pointer_event(BalloonEval *, صحیح, صحیح, unsigned);
ایستا تهی کلید_event(BalloonEval *, unsigned, صحیح);
ایستا gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   اگر GTK_CHECK_VERSION(3,0,0)
ایستا gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   وگرنه
ایستا gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# وگرنه
ایستا تهی addEventHandler(Widget, BalloonEval *);
ایستا تهی removeEventHandler(BalloonEval *);
ایستا تهی pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
ایستا تهی pointerEvent(BalloonEval *, XEvent *);
ایستا تهی timerRoutine(XtPointer, XtIntervalId *);
# endif
ایستا تهی cancelBalloon(BalloonEval *);
ایستا تهی requestBalloon(BalloonEval *);
ایستا تهی drawBalloon(BalloonEval *);
ایستا تهی undrawBalloon(BalloonEval *beval);
ایستا تهی createBalloonEvalWindow(BalloonEval *);

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
    تهی	*target,
    char_u	*mesg,
    تهی	(*mesgCB)(BalloonEval *, صحیح),
    تهی	*clientData)
{
# ifndef FEAT_GUI_GTK
    نویسه	*display_name;	    // get from gui.dpy
    صحیح		screen_رقم;
    نویسه	*p;
# endif
    BalloonEval	*beval;

    اگر (mesg != پوچ && mesgCB != پوچ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	بازگشت پوچ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    اگر (beval != پوچ)
    {
# ifdef FEAT_GUI_GTK
	beval->target = GTK_WIDGET(target);
# وگرنه
	beval->target = (Widget)target;
	beval->appContext = XtWidgetToApplicationContext((Widget)target);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->پیام);
	beval->پیام = mesg == پوچ ? پوچ : vim_strsave(mesg);
	beval->پیام_CB = mesgCB;
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
	اگر (p != پوچ)
	    screen_رقم = atoi(++p);
	وگرنه
	    screen_رقم = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_رقم);
	beval->screen_height = DisplayHeight(gui.dpy, screen_رقم);
# endif
    }

    بازگشت beval;
}

# اگر defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    تهی
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   وگرنه
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  وگرنه
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    اگر (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->پیام);
    vim_free(beval);
}
# endif

    تهی
gui_mch_enable_beval_area(BalloonEval *beval)
{
    اگر (beval != پوچ)
	addEventHandler(beval->target, beval);
}

    تهی
gui_mch_disable_beval_area(BalloonEval *beval)
{
    اگر (beval != پوچ)
	removeEventHandler(beval);
}

# اگر defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(تهی)
{
    بازگشت current_beval;
}
# endif
#endif // !FEAT_GUI_MSWIN

#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# اگر !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    تهی
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->پیام);
    beval->پیام = mesg == پوچ ? پوچ : vim_strsave(mesg);
    اگر (beval->پیام != پوچ)
	drawBalloon(beval);
    وگرنه
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#endif // FEAT_NETBEANS_INTG || FEAT_EVAL

#اگر !defined(FEAT_GUI_MSWIN)
# اگر defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    تهی
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->پیام);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    ایستا تهی
addEventHandler(GtkWidget *target, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_set_data(G_OBJECT(target), "beval", controller);
    gtk_widget_add_controller(target, controller);
#  وگرنه
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
    اگر (gtk_socket_id == 0 && gui.mainwin != پوچ
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

    ایستا تهی
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*target = beval->target;
    GtkEventController	*controller;

    controller = g_object_get_data(G_OBJECT(target), "beval");
    اگر (controller != پوچ)
    {
	gtk_widget_remove_controller(target, controller);
	g_object_set_data(G_OBJECT(target), "beval", پوچ);
    }
#  وگرنه
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->target),
					 FUNC2GENERIC(target_event_cb),
					 beval);

    اگر (gtk_socket_id == 0 && gui.mainwin != پوچ
	    && gtk_widget_is_ancestor(beval->target, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    ایستا gboolean
event_cb(GtkEventController *controller UNUSED, GdkEvent *event, BalloonEval *beval)
{
    صحیح type = gdk_event_get_event_type(event);

    انتخاب (type)
    {
	حالت GDK_ENTER_NOTIFY:
	حالت GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*ریشه;
	    graphene_point_t	p;
	    graphene_point_t	result;
	    دوگانه		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    ریشه = GTK_WIDGET(gtk_widget_get_ریشه(beval->target));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    اگر (gtk_widget_compute_point(ریشه, beval->target, &p, &result))
	    {
		x = result.x;
		y = result.y;
	    }

	    pointer_event(beval, (صحیح)x, (صحیح)y,
		    gdk_event_get_modifier_state(event));
	    بشکن;
	}
	حالت GDK_LEAVE_NOTIFY:
	    اگر (gdk_crossing_event_get_mode(event) == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    بشکن;
	حالت GDK_BUTTON_PRESS:
	حالت GDK_SCROLL:
	    cancelBalloon(beval);
	    بشکن;
	حالت GDK_کلید_PRESS:
	حالت GDK_کلید_RELEASE:
	    کلید_event(beval, gdk_کلید_event_get_keyval(event),
		    type == GDK_کلید_PRESS);
	    بشکن;
	پیشفرض:
	    بشکن;
    }

    // Always pass through events?
    بازگشت FALSE;
}
#  وگرنه
    ایستا gint
target_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    انتخاب (event->type)
    {
	حالت GDK_ENTER_NOTIFY:
	    pointer_event(beval, (صحیح)event->crossing.x,
				 (صحیح)event->crossing.y,
				 event->crossing.state);
	    بشکن;
	حالت GDK_MOTION_NOTIFY:
	    اگر (event->motion.is_hint)
	    {
		صحیح		x;
		صحیح		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   اگر GTK_CHECK_VERSION(3,0,0)
		{
		    GdkWindow * ثابت win = gtk_widget_get_window(widget);
		    GdkDisplay * ثابت dpy = gdk_window_get_display(win);
#    اگر GTK_CHECK_VERSION(3,20,0)
		    GdkSeat * ثابت seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * ثابت dev = gdk_seat_get_pointer(seat);
#    وگرنه
		    GdkDeviceManager * ثابت mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * ثابت dev = gdk_device_manager_get_client_pointer(mngr);
#    endif
		    gdk_window_get_device_position(win, dev , &x, &y, &state);
		}
#   وگرنه
		gdk_window_get_pointer(widget->window, &x, &y, &state);
#   endif
		pointer_event(beval, x, y, (unsigned صحیح)state);
	    }
	    وگرنه
	    {
		pointer_event(beval, (صحیح)event->motion.x,
				     (صحیح)event->motion.y,
				     event->motion.state);
	    }
	    بشکن;
	حالت GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    اگر (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    بشکن;
	حالت GDK_BUTTON_PRESS:
	حالت GDK_SCROLL:
	    cancelBalloon(beval);
	    بشکن;
	حالت GDK_کلید_PRESS:
	    کلید_event(beval, event->کلید.keyval, TRUE);
	    بشکن;
	حالت GDK_کلید_RELEASE:
	    کلید_event(beval, event->کلید.keyval, FALSE);
	    بشکن;
	پیشفرض:
	    بشکن;
    }

    بازگشت FALSE; // continue emission
}

    ایستا gint
mainwin_event_cb(GtkWidget *widget UNUSED, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    انتخاب (event->type)
    {
	حالت GDK_کلید_PRESS:
	    کلید_event(beval, event->کلید.keyval, TRUE);
	    بشکن;
	حالت GDK_کلید_RELEASE:
	    کلید_event(beval, event->کلید.keyval, FALSE);
	    بشکن;
	حالت GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    اگر (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    بشکن;
	پیشفرض:
	    بشکن;
    }

    بازگشت FALSE; // continue emission
}
#  endif

    ایستا تهی
pointer_event(BalloonEval *beval, صحیح x, صحیح y, unsigned state)
{
    صحیح distance;

    distance = ABS(x - beval->x) + ABS(y - beval->y);

    اگر (distance <= 4)
	بازگشت;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    اگر (!(state & ((صحیح)GDK_BUTTON1_MASK | (صحیح)GDK_BUTTON2_MASK
		    | (صحیح)GDK_BUTTON3_MASK)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	اگر (state & (صحیح)GDK_ALT_MASK)
#  وگرنه
	اگر (state & (صحیح)GDK_MOD1_MASK)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    اگر (beval->پیام_CB != پوچ)
	    {
		beval->showState = ShS_PENDING;
		(*beval->پیام_CB)(beval, state);
	    }
	}
	وگرنه
	{
	    beval->timerID = g_timeout_add((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    ایستا تهی
کلید_event(BalloonEval *beval, unsigned keyval, صحیح is_keypress)
{
    اگر (beval->showState == ShS_SHOWING && beval->پیام_CB != پوچ)
    {
	انتخاب (keyval)
	{
#  ifdef USE_GTK4
	    حالت GDK_کلید_Shift_L:
	    حالت GDK_کلید_Shift_R:
#  وگرنه
	    حالت GDK_Shift_L:
	    حالت GDK_Shift_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->پیام_CB)(beval, (is_keypress)
						   ? (صحیح)GDK_SHIFT_MASK : 0);
		بشکن;
#  ifdef USE_GTK4
	    حالت GDK_کلید_Control_L:
	    حالت GDK_کلید_Control_R:
#  وگرنه
	    حالت GDK_Control_L:
	    حالت GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->پیام_CB)(beval, (is_keypress)
						 ? (صحیح)GDK_CONTROL_MASK : 0);
		بشکن;
	    پیشفرض:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		اگر (is_keypress)
		    cancelBalloon(beval);
		بشکن;
	}
    }
    وگرنه
	cancelBalloon(beval);
}

    ایستا gboolean
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

    بازگشت FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   اگر GTK_CHECK_VERSION(3,0,0)
    ایستا gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 data UNUSED)
{
    GtkStyleContext *context = پوچ;
    gint width = -1, height = -1;

    اگر (widget == پوچ)
	بازگشت TRUE;

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

    بازگشت FALSE;
}
#   وگرنه
    ایستا gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer data UNUSED)
{
    gtk_paint_flat_box(widget->style, widget->window,
		       GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    بازگشت FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# وگرنه // !FEAT_GUI_GTK

    ایستا تهی
addEventHandler(Widget target, BalloonEval *beval)
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | کلید_Press_Mask |
			کلید_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}

    ایستا تهی
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | کلید_Press_Mask |
			کلید_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    ایستا تهی
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

    ایستا تهی
pointerEvent(BalloonEval *beval, XEvent *event)
{
    Position	distance;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    انتخاب (event->type)
    {
	حالت EnterNotify:
	حالت MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    اگر (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    اگر (delta < 0)
		delta = -delta;
	    distance += delta;
	    اگر (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		اگر (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		وگرنه اگر (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_ریشه = event->xmotion.x_ریشه;
		    beval->y_ریشه = event->xmotion.y_ریشه;
		    cancelBalloon(beval);
		    اگر (beval->پیام_CB != پوچ)
		    {
			beval->showState = ShS_PENDING;
			(*beval->پیام_CB)(beval, beval->state);
		    }
		}
		وگرنه
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_ریشه = event->xmotion.x_ریشه;
		    beval->y_ریشه = event->xmotion.y_ریشه;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    بشکن;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	حالت کلید_Press:
	    اگر (beval->showState == ShS_SHOWING && beval->پیام_CB != پوچ)
	    {
		Modifiers   modifier;
		کلید_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		اگر (keysym == XK_Shift_L || keysym == XK_Shift_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->پیام_CB)(beval, ShiftMask);
		}
		وگرنه اگر (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->پیام_CB)(beval, ControlMask);
		}
		وگرنه
		    cancelBalloon(beval);
	    }
	    وگرنه
		cancelBalloon(beval);
	    بشکن;

	حالت کلید_Release:
	    اگر (beval->showState == ShS_SHOWING && beval->پیام_CB != پوچ)
	    {
		Modifiers modifier;
		کلید_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		اگر ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->پیام_CB)(beval, 0);
		}
		وگرنه اگر ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->پیام_CB)(beval, 0);
		}
		وگرنه
		    cancelBalloon(beval);
	    }
	    وگرنه
		cancelBalloon(beval);
	    بشکن;

	حالت LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		اگر (event->xcrossing.mode == NotifyNormal)
		    cancelBalloon(beval);
		بشکن;

	حالت ButtonPress:
		cancelBalloon(beval);
		بشکن;

	پیشفرض:
	    بشکن;
    }
}

    ایستا تهی
timerRoutine(XtPointer dx, XtIntervalId *id UNUSED)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)پوچ;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    ایستا تهی
requestBalloon(BalloonEval *beval)
{
    اگر (beval->showState == ShS_PENDING)
	بازگشت;

    // Determine the beval to display
    اگر (beval->پیام_CB != پوچ)
    {
	beval->showState = ShS_PENDING;
	(*beval->پیام_CB)(beval, beval->state);
    }
    وگرنه اگر (beval->پیام != پوچ)
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
    ایستا تهی
set_printable_label_text(GtkLabel *label, char_u *text)
{
    char_u	    *convbuf = پوچ;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    unsigned صحیح    len;
    صحیح		    charlen;
    صحیح		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    اگر (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, text, پوچ);
	اگر (convbuf != پوچ)
	    text = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    برای (p = text; *p != NUL; p += charlen)
    {
	اگر ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	وگرنه
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    اگر (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    اگر (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    وگرنه اگر (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    وگرنه
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    اگر (buf != پوچ)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  اگر GTK_CHECK_VERSION(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   اگر PANGO_VERSION_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  وگرنه
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != پوچ) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	اگر (pixel != INVALCOLOR)
#  اگر GTK_CHECK_VERSION(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  وگرنه
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (unsigned long)pixel, &color);
#  endif

	pdest = buf;
	p = text;
	وقتی (*p != NUL)
	{
	    // Be quick for ASCII
	    اگر ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    وگرنه
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		اگر (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		اگر (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    صحیح	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    اگر (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    وگرنه اگر (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    وگرنه
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    اگر (pixel != INVALCOLOR)
		    {
#  اگر GTK_CHECK_VERSION(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   اگر PANGO_VERSION_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  وگرنه
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->start_index = pdest - buf;
			attr->end_index   = pdest - buf + outlen;
			pango_attr_list_درج(attr_list, attr);
#  اگر GTK_CHECK_VERSION(3,0,0)
#   اگر PANGO_VERSION_CHECK(1,38,0)
			attr_alpha->start_index = pdest - buf;
			attr_alpha->end_index   = pdest - buf + outlen;
			pango_attr_list_درج(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		وگرنه
		{
		    انجامبده
			*pdest++ = *p++;
		    وقتی (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_set_text(label, (ثابت نویسه *)buf);
    vim_free(buf);

    gtk_label_set_attributes(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    ایستا تهی
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    صحیح		    natural_width = 0;

    اگر (beval->پیام == پوچ)
	بازگشت;

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->پیام);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    پوچ, &natural_width, پوچ, پوچ);

    rect.width = (natural_width > 0 ? natural_width : 1) + gui.char_width * 3;
    rect.height = gui.char_height;

    gtk_popover_set_pointing_to(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  وگرنه
    ایستا تهی
drawBalloon(BalloonEval *beval)
{
    اگر (beval->پیام == پوچ)
	بازگشت;

    GtkRequisition	requisition;
    صحیح		screen_w;
    صحیح		screen_h;
    صحیح		screen_x;
    صحیح		screen_y;
    صحیح		x;
    صحیح		y;
    صحیح		x_offset = EVAL_OFFSET_X;
    صحیح		y_offset = EVAL_OFFSET_Y;
    PangoLayout	*layout;

#   اگر !GTK_CHECK_VERSION(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->target);
    gtk_window_set_screen(GTK_WINDOW(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_of_win(beval->target, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   اگر !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->پیام);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_WORD_CHAR
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
#   وگرنه
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
#   endif
    pango_layout_set_width(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.رقم_cols * gui.char_width,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   اگر GTK_CHECK_VERSION(3,0,0)
    gtk_widget_get_preferred_size(beval->balloonShell, &requisition, پوچ);
#   وگرنه
    gtk_widget_size_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_window_get_origin(gtk_widget_get_window(beval->target), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    اگر (x + x_offset + requisition.width > screen_x + screen_w)
	y_offset += 15;
    اگر (y + y_offset + requisition.height > screen_y + screen_h)
	y_offset = -requisition.height - EVAL_OFFSET_Y;

    // Sanitize values
    x = CLAMP(x + x_offset, 0,
	    MAX(0, screen_x + screen_w - requisition.width));
    y = CLAMP(y + y_offset, 0,
	    MAX(0, screen_y + screen_h - requisition.height));

    // Show the balloon
#   اگر GTK_CHECK_VERSION(3,0,0)
    gtk_window_move(GTK_WINDOW(beval->balloonShell), x, y);
#   وگرنه
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
    ایستا تهی
undrawBalloon(BalloonEval *beval)
{
    اگر (beval->balloonShell != پوچ)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  وگرنه
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    ایستا تهی
cancelBalloon(BalloonEval *beval)
{
    اگر (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    اگر (beval->timerID != 0)
    {
	g_source_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    ایستا تهی
createBalloonEvalWindow(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_set_parent(beval->balloonShell, beval->target);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_set_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(پوچ);

    gtk_label_set_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_چپ);
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_set_child(GTK_POPOVER(popover), beval->balloonLabel);
}
#  وگرنه
    ایستا تهی
createBalloonEvalWindow(BalloonEval *beval)
{
    beval->balloonShell = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_transient_for(GTK_WINDOW(beval->balloonShell), GTK_WINDOW(gui.mainwin));

    gtk_widget_set_app_paintable(beval->balloonShell, TRUE);
    gtk_window_set_resizable(GTK_WINDOW(beval->balloonShell), FALSE);
    gtk_widget_set_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_set_border_width(GTK_CONTAINER(beval->balloonShell), 4);

#   اگر GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), پوچ);
#   وگرنه
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), پوچ);
#   endif
    beval->balloonLabel = gtk_label_new(پوچ);

    gtk_label_set_سطر_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_چپ);
#   اگر GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_VERSION(3,14,0)
    GValue align_val = G_مقدار_INIT;
    g_مقدار_init(&align_val, G_TYPE_FLOAT);
    g_مقدار_set_float(&align_val, 0.5);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_مقدار_unset(&align_val);
#   وگرنه
    gtk_misc_set_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_set_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_add(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# وگرنه // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    ایستا تهی
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    اگر (beval->پیام == پوچ)
	بازگشت;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    اگر (beval->پیام_CB != پوچ)
	s = XmStringCreateLtoR((نویسه *)beval->پیام, XmFONTLIST_DEFAULT_TAG);
    وگرنه
	s = XmStringCreateLocalized((نویسه *)beval->پیام);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	اگر (fl == پوچ)
	{
	    XmStringFree(s);
	    بازگشت;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_offset << 1;
    h += gui.border_offset << 1;
    Xt_Va_Set_مقادیر(beval->balloonLabel, XmNlabelString, s, پوچ);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_ریشه + EVAL_OFFSET_X;
    ty = beval->y_ریشه + EVAL_OFFSET_Y;
    اگر ((tx + w) > beval->screen_width)
	tx = beval->screen_width - w;
    اگر ((ty + h) > beval->screen_height)
	ty = beval->screen_height - h;
    Xt_Va_Set_مقادیر(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    پوچ);
    // Set tooltip colors
    {
	Arg args[2];

	args[0].name = XmNbackground;
	args[0].مقدار = gui.tooltip_bg_pixel;
	args[1].name = XmNforeground;
	args[1].مقدار = gui.tooltip_fg_pixel;
	Xt_Set_مقادیر(beval->balloonLabel, &args[0], Xt_عدد(args));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    ایستا تهی
undrawBalloon(BalloonEval *beval)
{
    اگر (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = پوچ;
}

    ایستا تهی
cancelBalloon(BalloonEval *beval)
{
    اگر (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    اگر (beval->timerID != (XtIntervalId)پوچ)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = (XtIntervalId)پوچ;
    }
    beval->showState = ShS_NEUTRAL;
}


    ایستا تهی
createBalloonEvalWindow(BalloonEval *beval)
{
    Arg		args[12];
    صحیح		n;

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
