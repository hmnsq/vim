/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#यदि !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_VERSION(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  अन्यथा
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# अन्यथा
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
बाह्य Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
स्थिर BalloonEval *current_beval = रिक्त;
# endif

# ifdef FEAT_GUI_GTK
स्थिर शून्य addEventHandler(GtkWidget *, BalloonEval *);
स्थिर शून्य removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
स्थिर gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  अन्यथा
स्थिर gint target_event_cb(GtkWidget *, GdkEvent *, gpointer);
स्थिर gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
स्थिर शून्य pointer_event(BalloonEval *, पूर्णाङ्क, पूर्णाङ्क, unsigned);
स्थिर शून्य किल्ली_event(BalloonEval *, unsigned, पूर्णाङ्क);
स्थिर gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   यदि GTK_CHECK_VERSION(3,0,0)
स्थिर gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   अन्यथा
स्थिर gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# अन्यथा
स्थिर शून्य addEventHandler(Widget, BalloonEval *);
स्थिर शून्य removeEventHandler(BalloonEval *);
स्थिर शून्य pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
स्थिर शून्य pointerEvent(BalloonEval *, XEvent *);
स्थिर शून्य timerRoutine(XtPointer, XtIntervalId *);
# endif
स्थिर शून्य cancelBalloon(BalloonEval *);
स्थिर शून्य requestBalloon(BalloonEval *);
स्थिर शून्य drawBalloon(BalloonEval *);
स्थिर शून्य undrawBalloon(BalloonEval *beval);
स्थिर शून्य createBalloonEvalWindow(BalloonEval *);

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
    शून्य	*target,
    char_u	*mesg,
    शून्य	(*mesgCB)(BalloonEval *, पूर्णाङ्क),
    शून्य	*clientData)
{
# ifndef FEAT_GUI_GTK
    वर्ण	*display_name;	    // get from gui.dpy
    पूर्णाङ्क		screen_अंक;
    वर्ण	*p;
# endif
    BalloonEval	*beval;

    यदि (mesg != रिक्त && mesgCB != रिक्त)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	प्रत्यागम रिक्त;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    यदि (beval != रिक्त)
    {
# ifdef FEAT_GUI_GTK
	beval->target = GTK_WIDGET(target);
# अन्यथा
	beval->target = (Widget)target;
	beval->appContext = XtWidgetToApplicationContext((Widget)target);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->संदेश);
	beval->संदेश = mesg == रिक्त ? रिक्त : vim_strsave(mesg);
	beval->संदेश_CB = mesgCB;
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
	यदि (p != रिक्त)
	    screen_अंक = atoi(++p);
	अन्यथा
	    screen_अंक = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_अंक);
	beval->screen_height = DisplayHeight(gui.dpy, screen_अंक);
# endif
    }

    प्रत्यागम beval;
}

# यदि defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    शून्य
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   अन्यथा
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  अन्यथा
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    यदि (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->संदेश);
    vim_free(beval);
}
# endif

    शून्य
gui_mch_enable_beval_area(BalloonEval *beval)
{
    यदि (beval != रिक्त)
	addEventHandler(beval->target, beval);
}

    शून्य
gui_mch_disable_beval_area(BalloonEval *beval)
{
    यदि (beval != रिक्त)
	removeEventHandler(beval);
}

# यदि defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(शून्य)
{
    प्रत्यागम current_beval;
}
# endif
#endif // !FEAT_GUI_MSWIN

#यदि defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# यदि !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    शून्य
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->संदेश);
    beval->संदेश = mesg == रिक्त ? रिक्त : vim_strsave(mesg);
    यदि (beval->संदेश != रिक्त)
	drawBalloon(beval);
    अन्यथा
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#endif // FEAT_NETBEANS_INTG || FEAT_EVAL

#यदि !defined(FEAT_GUI_MSWIN)
# यदि defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    शून्य
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->संदेश);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    स्थिर शून्य
addEventHandler(GtkWidget *target, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_set_data(G_OBJECT(target), "beval", controller);
    gtk_widget_add_controller(target, controller);
#  अन्यथा
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
    यदि (gtk_socket_id == 0 && gui.mainwin != रिक्त
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

    स्थिर शून्य
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*target = beval->target;
    GtkEventController	*controller;

    controller = g_object_get_data(G_OBJECT(target), "beval");
    यदि (controller != रिक्त)
    {
	gtk_widget_remove_controller(target, controller);
	g_object_set_data(G_OBJECT(target), "beval", रिक्त);
    }
#  अन्यथा
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->target),
					 FUNC2GENERIC(target_event_cb),
					 beval);

    यदि (gtk_socket_id == 0 && gui.mainwin != रिक्त
	    && gtk_widget_is_ancestor(beval->target, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    स्थिर gboolean
event_cb(GtkEventController *controller UNUSED, GdkEvent *event, BalloonEval *beval)
{
    पूर्णाङ्क type = gdk_event_get_event_type(event);

    विकल्प (type)
    {
	पक्ष GDK_ENTER_NOTIFY:
	पक्ष GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*मूळ;
	    graphene_point_t	p;
	    graphene_point_t	result;
	    द्विगुण		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    मूळ = GTK_WIDGET(gtk_widget_get_मूळ(beval->target));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    यदि (gtk_widget_compute_point(मूळ, beval->target, &p, &result))
	    {
		x = result.x;
		y = result.y;
	    }

	    pointer_event(beval, (पूर्णाङ्क)x, (पूर्णाङ्क)y,
		    gdk_event_get_modifier_state(event));
	    भङ्ग;
	}
	पक्ष GDK_LEAVE_NOTIFY:
	    यदि (gdk_crossing_event_get_mode(event) == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    भङ्ग;
	पक्ष GDK_BUTTON_PRESS:
	पक्ष GDK_SCROLL:
	    cancelBalloon(beval);
	    भङ्ग;
	पक्ष GDK_किल्ली_PRESS:
	पक्ष GDK_किल्ली_RELEASE:
	    किल्ली_event(beval, gdk_किल्ली_event_get_keyval(event),
		    type == GDK_किल्ली_PRESS);
	    भङ्ग;
	स्वभाव:
	    भङ्ग;
    }

    // Always pass through events?
    प्रत्यागम FALSE;
}
#  अन्यथा
    स्थिर gint
target_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    विकल्प (event->type)
    {
	पक्ष GDK_ENTER_NOTIFY:
	    pointer_event(beval, (पूर्णाङ्क)event->crossing.x,
				 (पूर्णाङ्क)event->crossing.y,
				 event->crossing.state);
	    भङ्ग;
	पक्ष GDK_MOTION_NOTIFY:
	    यदि (event->motion.is_hint)
	    {
		पूर्णाङ्क		x;
		पूर्णाङ्क		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   यदि GTK_CHECK_VERSION(3,0,0)
		{
		    GdkWindow * ध्रुव win = gtk_widget_get_window(widget);
		    GdkDisplay * ध्रुव dpy = gdk_window_get_display(win);
#    यदि GTK_CHECK_VERSION(3,20,0)
		    GdkSeat * ध्रुव seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * ध्रुव dev = gdk_seat_get_pointer(seat);
#    अन्यथा
		    GdkDeviceManager * ध्रुव mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * ध्रुव dev = gdk_device_manager_get_client_pointer(mngr);
#    endif
		    gdk_window_get_device_position(win, dev , &x, &y, &state);
		}
#   अन्यथा
		gdk_window_get_pointer(widget->window, &x, &y, &state);
#   endif
		pointer_event(beval, x, y, (unsigned पूर्णाङ्क)state);
	    }
	    अन्यथा
	    {
		pointer_event(beval, (पूर्णाङ्क)event->motion.x,
				     (पूर्णाङ्क)event->motion.y,
				     event->motion.state);
	    }
	    भङ्ग;
	पक्ष GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    यदि (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    भङ्ग;
	पक्ष GDK_BUTTON_PRESS:
	पक्ष GDK_SCROLL:
	    cancelBalloon(beval);
	    भङ्ग;
	पक्ष GDK_किल्ली_PRESS:
	    किल्ली_event(beval, event->किल्ली.keyval, TRUE);
	    भङ्ग;
	पक्ष GDK_किल्ली_RELEASE:
	    किल्ली_event(beval, event->किल्ली.keyval, FALSE);
	    भङ्ग;
	स्वभाव:
	    भङ्ग;
    }

    प्रत्यागम FALSE; // continue emission
}

    स्थिर gint
mainwin_event_cb(GtkWidget *widget UNUSED, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    विकल्प (event->type)
    {
	पक्ष GDK_किल्ली_PRESS:
	    किल्ली_event(beval, event->किल्ली.keyval, TRUE);
	    भङ्ग;
	पक्ष GDK_किल्ली_RELEASE:
	    किल्ली_event(beval, event->किल्ली.keyval, FALSE);
	    भङ्ग;
	पक्ष GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    यदि (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    भङ्ग;
	स्वभाव:
	    भङ्ग;
    }

    प्रत्यागम FALSE; // continue emission
}
#  endif

    स्थिर शून्य
pointer_event(BalloonEval *beval, पूर्णाङ्क x, पूर्णाङ्क y, unsigned state)
{
    पूर्णाङ्क distance;

    distance = ABS(x - beval->x) + ABS(y - beval->y);

    यदि (distance <= 4)
	प्रत्यागम;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    यदि (!(state & ((पूर्णाङ्क)GDK_BUTTON1_MASK | (पूर्णाङ्क)GDK_BUTTON2_MASK
		    | (पूर्णाङ्क)GDK_BUTTON3_MASK)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	यदि (state & (पूर्णाङ्क)GDK_ALT_MASK)
#  अन्यथा
	यदि (state & (पूर्णाङ्क)GDK_MOD1_MASK)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    यदि (beval->संदेश_CB != रिक्त)
	    {
		beval->showState = ShS_PENDING;
		(*beval->संदेश_CB)(beval, state);
	    }
	}
	अन्यथा
	{
	    beval->timerID = g_timeout_add((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    स्थिर शून्य
किल्ली_event(BalloonEval *beval, unsigned keyval, पूर्णाङ्क is_keypress)
{
    यदि (beval->showState == ShS_SHOWING && beval->संदेश_CB != रिक्त)
    {
	विकल्प (keyval)
	{
#  ifdef USE_GTK4
	    पक्ष GDK_किल्ली_Shift_L:
	    पक्ष GDK_किल्ली_Shift_R:
#  अन्यथा
	    पक्ष GDK_Shift_L:
	    पक्ष GDK_Shift_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->संदेश_CB)(beval, (is_keypress)
						   ? (पूर्णाङ्क)GDK_SHIFT_MASK : 0);
		भङ्ग;
#  ifdef USE_GTK4
	    पक्ष GDK_किल्ली_Control_L:
	    पक्ष GDK_किल्ली_Control_R:
#  अन्यथा
	    पक्ष GDK_Control_L:
	    पक्ष GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->संदेश_CB)(beval, (is_keypress)
						 ? (पूर्णाङ्क)GDK_CONTROL_MASK : 0);
		भङ्ग;
	    स्वभाव:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		यदि (is_keypress)
		    cancelBalloon(beval);
		भङ्ग;
	}
    }
    अन्यथा
	cancelBalloon(beval);
}

    स्थिर gboolean
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

    प्रत्यागम FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   यदि GTK_CHECK_VERSION(3,0,0)
    स्थिर gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 data UNUSED)
{
    GtkStyleContext *context = रिक्त;
    gint width = -1, height = -1;

    यदि (widget == रिक्त)
	प्रत्यागम TRUE;

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

    प्रत्यागम FALSE;
}
#   अन्यथा
    स्थिर gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer data UNUSED)
{
    gtk_paint_flat_box(widget->style, widget->window,
		       GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    प्रत्यागम FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# अन्यथा // !FEAT_GUI_GTK

    स्थिर शून्य
addEventHandler(Widget target, BalloonEval *beval)
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | किल्ली_Press_Mask |
			किल्ली_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}

    स्थिर शून्य
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | किल्ली_Press_Mask |
			किल्ली_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    स्थिर शून्य
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

    स्थिर शून्य
pointerEvent(BalloonEval *beval, XEvent *event)
{
    Position	distance;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    विकल्प (event->type)
    {
	पक्ष EnterNotify:
	पक्ष MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    यदि (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    यदि (delta < 0)
		delta = -delta;
	    distance += delta;
	    यदि (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		यदि (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		अन्यथा यदि (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_मूळ = event->xmotion.x_मूळ;
		    beval->y_मूळ = event->xmotion.y_मूळ;
		    cancelBalloon(beval);
		    यदि (beval->संदेश_CB != रिक्त)
		    {
			beval->showState = ShS_PENDING;
			(*beval->संदेश_CB)(beval, beval->state);
		    }
		}
		अन्यथा
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_मूळ = event->xmotion.x_मूळ;
		    beval->y_मूळ = event->xmotion.y_मूळ;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    भङ्ग;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	पक्ष किल्ली_Press:
	    यदि (beval->showState == ShS_SHOWING && beval->संदेश_CB != रिक्त)
	    {
		Modifiers   modifier;
		किल्ली_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		यदि (keysym == XK_Shift_L || keysym == XK_Shift_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->संदेश_CB)(beval, ShiftMask);
		}
		अन्यथा यदि (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->संदेश_CB)(beval, ControlMask);
		}
		अन्यथा
		    cancelBalloon(beval);
	    }
	    अन्यथा
		cancelBalloon(beval);
	    भङ्ग;

	पक्ष किल्ली_Release:
	    यदि (beval->showState == ShS_SHOWING && beval->संदेश_CB != रिक्त)
	    {
		Modifiers modifier;
		किल्ली_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		यदि ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->संदेश_CB)(beval, 0);
		}
		अन्यथा यदि ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->संदेश_CB)(beval, 0);
		}
		अन्यथा
		    cancelBalloon(beval);
	    }
	    अन्यथा
		cancelBalloon(beval);
	    भङ्ग;

	पक्ष LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		यदि (event->xcrossing.mode == NotifyNormal)
		    cancelBalloon(beval);
		भङ्ग;

	पक्ष ButtonPress:
		cancelBalloon(beval);
		भङ्ग;

	स्वभाव:
	    भङ्ग;
    }
}

    स्थिर शून्य
timerRoutine(XtPointer dx, XtIntervalId *id UNUSED)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)रिक्त;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    स्थिर शून्य
requestBalloon(BalloonEval *beval)
{
    यदि (beval->showState == ShS_PENDING)
	प्रत्यागम;

    // Determine the beval to display
    यदि (beval->संदेश_CB != रिक्त)
    {
	beval->showState = ShS_PENDING;
	(*beval->संदेश_CB)(beval, beval->state);
    }
    अन्यथा यदि (beval->संदेश != रिक्त)
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
    स्थिर शून्य
set_printable_label_text(GtkLabel *label, char_u *text)
{
    char_u	    *convbuf = रिक्त;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    unsigned पूर्णाङ्क    len;
    पूर्णाङ्क		    charlen;
    पूर्णाङ्क		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    यदि (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, text, रिक्त);
	यदि (convbuf != रिक्त)
	    text = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    प्रति (p = text; *p != NUL; p += charlen)
    {
	यदि ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	अन्यथा
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    यदि (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    यदि (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    अन्यथा यदि (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    अन्यथा
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    यदि (buf != रिक्त)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  यदि GTK_CHECK_VERSION(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   यदि PANGO_VERSION_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  अन्यथा
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != रिक्त) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	यदि (pixel != INVALCOLOR)
#  यदि GTK_CHECK_VERSION(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  अन्यथा
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (unsigned long)pixel, &color);
#  endif

	pdest = buf;
	p = text;
	यावत् (*p != NUL)
	{
	    // Be quick for ASCII
	    यदि ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    अन्यथा
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		यदि (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		यदि (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    पूर्णाङ्क	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    यदि (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    अन्यथा यदि (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    अन्यथा
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    यदि (pixel != INVALCOLOR)
		    {
#  यदि GTK_CHECK_VERSION(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   यदि PANGO_VERSION_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  अन्यथा
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->start_index = pdest - buf;
			attr->end_index   = pdest - buf + outlen;
			pango_attr_list_घाला(attr_list, attr);
#  यदि GTK_CHECK_VERSION(3,0,0)
#   यदि PANGO_VERSION_CHECK(1,38,0)
			attr_alpha->start_index = pdest - buf;
			attr_alpha->end_index   = pdest - buf + outlen;
			pango_attr_list_घाला(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		अन्यथा
		{
		    कुरु
			*pdest++ = *p++;
		    यावत् (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_set_text(label, (ध्रुव वर्ण *)buf);
    vim_free(buf);

    gtk_label_set_attributes(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    स्थिर शून्य
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    पूर्णाङ्क		    natural_width = 0;

    यदि (beval->संदेश == रिक्त)
	प्रत्यागम;

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->संदेश);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    रिक्त, &natural_width, रिक्त, रिक्त);

    rect.width = (natural_width > 0 ? natural_width : 1) + gui.char_width * 3;
    rect.height = gui.char_height;

    gtk_popover_set_pointing_to(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  अन्यथा
    स्थिर शून्य
drawBalloon(BalloonEval *beval)
{
    यदि (beval->संदेश == रिक्त)
	प्रत्यागम;

    GtkRequisition	requisition;
    पूर्णाङ्क		screen_w;
    पूर्णाङ्क		screen_h;
    पूर्णाङ्क		screen_x;
    पूर्णाङ्क		screen_y;
    पूर्णाङ्क		x;
    पूर्णाङ्क		y;
    पूर्णाङ्क		x_offset = EVAL_OFFSET_X;
    पूर्णाङ्क		y_offset = EVAL_OFFSET_Y;
    PangoLayout	*layout;

#   यदि !GTK_CHECK_VERSION(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->target);
    gtk_window_set_screen(GTK_WINDOW(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_of_win(beval->target, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   यदि !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->संदेश);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_WORD_CHAR
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
#   अन्यथा
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
#   endif
    pango_layout_set_width(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.अंक_cols * gui.char_width,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   यदि GTK_CHECK_VERSION(3,0,0)
    gtk_widget_get_preferred_size(beval->balloonShell, &requisition, रिक्त);
#   अन्यथा
    gtk_widget_size_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_window_get_origin(gtk_widget_get_window(beval->target), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    यदि (x + x_offset + requisition.width > screen_x + screen_w)
	y_offset += 15;
    यदि (y + y_offset + requisition.height > screen_y + screen_h)
	y_offset = -requisition.height - EVAL_OFFSET_Y;

    // Sanitize values
    x = CLAMP(x + x_offset, 0,
	    MAX(0, screen_x + screen_w - requisition.width));
    y = CLAMP(y + y_offset, 0,
	    MAX(0, screen_y + screen_h - requisition.height));

    // Show the balloon
#   यदि GTK_CHECK_VERSION(3,0,0)
    gtk_window_move(GTK_WINDOW(beval->balloonShell), x, y);
#   अन्यथा
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
    स्थिर शून्य
undrawBalloon(BalloonEval *beval)
{
    यदि (beval->balloonShell != रिक्त)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  अन्यथा
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    स्थिर शून्य
cancelBalloon(BalloonEval *beval)
{
    यदि (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    यदि (beval->timerID != 0)
    {
	g_source_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    स्थिर शून्य
createBalloonEvalWindow(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_set_parent(beval->balloonShell, beval->target);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_set_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(रिक्त);

    gtk_label_set_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_डावा);
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_set_child(GTK_POPOVER(popover), beval->balloonLabel);
}
#  अन्यथा
    स्थिर शून्य
createBalloonEvalWindow(BalloonEval *beval)
{
    beval->balloonShell = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_transient_for(GTK_WINDOW(beval->balloonShell), GTK_WINDOW(gui.mainwin));

    gtk_widget_set_app_paintable(beval->balloonShell, TRUE);
    gtk_window_set_resizable(GTK_WINDOW(beval->balloonShell), FALSE);
    gtk_widget_set_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_set_border_width(GTK_CONTAINER(beval->balloonShell), 4);

#   यदि GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), रिक्त);
#   अन्यथा
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), रिक्त);
#   endif
    beval->balloonLabel = gtk_label_new(रिक्त);

    gtk_label_set_ओळ_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_डावा);
#   यदि GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_VERSION(3,14,0)
    GValue align_val = G_मूल्य_INIT;
    g_मूल्य_init(&align_val, G_TYPE_FLOAT);
    g_मूल्य_set_float(&align_val, 0.5);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_मूल्य_unset(&align_val);
#   अन्यथा
    gtk_misc_set_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_set_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_add(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# अन्यथा // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    स्थिर शून्य
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    यदि (beval->संदेश == रिक्त)
	प्रत्यागम;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    यदि (beval->संदेश_CB != रिक्त)
	s = XmStringCreateLtoR((वर्ण *)beval->संदेश, XmFONTLIST_DEFAULT_TAG);
    अन्यथा
	s = XmStringCreateLocalized((वर्ण *)beval->संदेश);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	यदि (fl == रिक्त)
	{
	    XmStringFree(s);
	    प्रत्यागम;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_offset << 1;
    h += gui.border_offset << 1;
    Xt_Va_Set_मूल्ये(beval->balloonLabel, XmNlabelString, s, रिक्त);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_मूळ + EVAL_OFFSET_X;
    ty = beval->y_मूळ + EVAL_OFFSET_Y;
    यदि ((tx + w) > beval->screen_width)
	tx = beval->screen_width - w;
    यदि ((ty + h) > beval->screen_height)
	ty = beval->screen_height - h;
    Xt_Va_Set_मूल्ये(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    रिक्त);
    // Set tooltip colors
    {
	Arg args[2];

	args[0].name = XmNbackground;
	args[0].मूल्य = gui.tooltip_bg_pixel;
	args[1].name = XmNforeground;
	args[1].मूल्य = gui.tooltip_fg_pixel;
	Xt_Set_मूल्ये(beval->balloonLabel, &args[0], Xt_संख्या(args));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    स्थिर शून्य
undrawBalloon(BalloonEval *beval)
{
    यदि (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = रिक्त;
}

    स्थिर शून्य
cancelBalloon(BalloonEval *beval)
{
    यदि (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    यदि (beval->timerID != (XtIntervalId)रिक्त)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = (XtIntervalId)रिक्त;
    }
    beval->showState = ShS_NEUTRAL;
}


    स्थिर शून्य
createBalloonEvalWindow(BalloonEval *beval)
{
    Arg		args[12];
    पूर्णाङ्क		n;

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
