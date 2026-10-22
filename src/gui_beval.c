/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#എങ്കിൽ !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_VERSION(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  അല്ലെങ്കിൽ
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# അല്ലെങ്കിൽ
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
ബാഹ്യ Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
സ്ഥിതം BalloonEval *current_beval = ശൂന്യ;
# endif

# ifdef FEAT_GUI_GTK
സ്ഥിതം ശൂന്യം addEventHandler(GtkWidget *, BalloonEval *);
സ്ഥിതം ശൂന്യം removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
സ്ഥിതം gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  അല്ലെങ്കിൽ
സ്ഥിതം gint target_event_cb(GtkWidget *, GdkEvent *, gpointer);
സ്ഥിതം gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
സ്ഥിതം ശൂന്യം pointer_event(BalloonEval *, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, unsigned);
സ്ഥിതം ശൂന്യം താക്കോൽ_event(BalloonEval *, unsigned, പൂർണ്ണസംഖ്യ);
സ്ഥിതം gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
സ്ഥിതം gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   അല്ലെങ്കിൽ
സ്ഥിതം gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം addEventHandler(Widget, BalloonEval *);
സ്ഥിതം ശൂന്യം removeEventHandler(BalloonEval *);
സ്ഥിതം ശൂന്യം pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
സ്ഥിതം ശൂന്യം pointerEvent(BalloonEval *, XEvent *);
സ്ഥിതം ശൂന്യം timerRoutine(XtPointer, XtIntervalId *);
# endif
സ്ഥിതം ശൂന്യം cancelBalloon(BalloonEval *);
സ്ഥിതം ശൂന്യം requestBalloon(BalloonEval *);
സ്ഥിതം ശൂന്യം drawBalloon(BalloonEval *);
സ്ഥിതം ശൂന്യം undrawBalloon(BalloonEval *beval);
സ്ഥിതം ശൂന്യം createBalloonEvalWindow(BalloonEval *);

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
    ശൂന്യം	*target,
    char_u	*mesg,
    ശൂന്യം	(*mesgCB)(BalloonEval *, പൂർണ്ണസംഖ്യ),
    ശൂന്യം	*clientData)
{
# ifndef FEAT_GUI_GTK
    അക്ഷരം	*display_name;	    // get from gui.dpy
    പൂർണ്ണസംഖ്യ		screen_അക്കം;
    അക്ഷരം	*p;
# endif
    BalloonEval	*beval;

    എങ്കിൽ (mesg != ശൂന്യ && mesgCB != ശൂന്യ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	തിരികെ ശൂന്യ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    എങ്കിൽ (beval != ശൂന്യ)
    {
# ifdef FEAT_GUI_GTK
	beval->target = GTK_WIDGET(target);
# അല്ലെങ്കിൽ
	beval->target = (Widget)target;
	beval->appContext = XtWidgetToApplicationContext((Widget)target);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->സന്ദേശം);
	beval->സന്ദേശം = mesg == ശൂന്യ ? ശൂന്യ : vim_strsave(mesg);
	beval->സന്ദേശം_CB = mesgCB;
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
	എങ്കിൽ (p != ശൂന്യ)
	    screen_അക്കം = atoi(++p);
	അല്ലെങ്കിൽ
	    screen_അക്കം = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_അക്കം);
	beval->screen_height = DisplayHeight(gui.dpy, screen_അക്കം);
# endif
    }

    തിരികെ beval;
}

# എങ്കിൽ defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    ശൂന്യം
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   അല്ലെങ്കിൽ
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  അല്ലെങ്കിൽ
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    എങ്കിൽ (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->സന്ദേശം);
    vim_free(beval);
}
# endif

    ശൂന്യം
gui_mch_enable_beval_area(BalloonEval *beval)
{
    എങ്കിൽ (beval != ശൂന്യ)
	addEventHandler(beval->target, beval);
}

    ശൂന്യം
gui_mch_disable_beval_area(BalloonEval *beval)
{
    എങ്കിൽ (beval != ശൂന്യ)
	removeEventHandler(beval);
}

# എങ്കിൽ defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(ശൂന്യം)
{
    തിരികെ current_beval;
}
# endif
#endif // !FEAT_GUI_MSWIN

#എങ്കിൽ defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# എങ്കിൽ !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    ശൂന്യം
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->സന്ദേശം);
    beval->സന്ദേശം = mesg == ശൂന്യ ? ശൂന്യ : vim_strsave(mesg);
    എങ്കിൽ (beval->സന്ദേശം != ശൂന്യ)
	drawBalloon(beval);
    അല്ലെങ്കിൽ
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#endif // FEAT_NETBEANS_INTG || FEAT_EVAL

#എങ്കിൽ !defined(FEAT_GUI_MSWIN)
# എങ്കിൽ defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    ശൂന്യം
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->സന്ദേശം);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    സ്ഥിതം ശൂന്യം
addEventHandler(GtkWidget *target, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_set_data(G_OBJECT(target), "beval", controller);
    gtk_widget_add_controller(target, controller);
#  അല്ലെങ്കിൽ
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
    എങ്കിൽ (gtk_socket_id == 0 && gui.mainwin != ശൂന്യ
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

    സ്ഥിതം ശൂന്യം
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*target = beval->target;
    GtkEventController	*controller;

    controller = g_object_get_data(G_OBJECT(target), "beval");
    എങ്കിൽ (controller != ശൂന്യ)
    {
	gtk_widget_remove_controller(target, controller);
	g_object_set_data(G_OBJECT(target), "beval", ശൂന്യ);
    }
#  അല്ലെങ്കിൽ
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->target),
					 FUNC2GENERIC(target_event_cb),
					 beval);

    എങ്കിൽ (gtk_socket_id == 0 && gui.mainwin != ശൂന്യ
	    && gtk_widget_is_ancestor(beval->target, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    സ്ഥിതം gboolean
event_cb(GtkEventController *controller UNUSED, GdkEvent *event, BalloonEval *beval)
{
    പൂർണ്ണസംഖ്യ type = gdk_event_get_event_type(event);

    തിരഞ്ഞെടുപ്പ് (type)
    {
	അവസ്ഥ GDK_ENTER_NOTIFY:
	അവസ്ഥ GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*വേര്;
	    graphene_point_t	p;
	    graphene_point_t	result;
	    ഇരട്ട		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    വേര് = GTK_WIDGET(gtk_widget_get_വേര്(beval->target));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    എങ്കിൽ (gtk_widget_compute_point(വേര്, beval->target, &p, &result))
	    {
		x = result.x;
		y = result.y;
	    }

	    pointer_event(beval, (പൂർണ്ണസംഖ്യ)x, (പൂർണ്ണസംഖ്യ)y,
		    gdk_event_get_modifier_state(event));
	    നിർത്തുക;
	}
	അവസ്ഥ GDK_LEAVE_NOTIFY:
	    എങ്കിൽ (gdk_crossing_event_get_mode(event) == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    നിർത്തുക;
	അവസ്ഥ GDK_BUTTON_PRESS:
	അവസ്ഥ GDK_SCROLL:
	    cancelBalloon(beval);
	    നിർത്തുക;
	അവസ്ഥ GDK_താക്കോൽ_PRESS:
	അവസ്ഥ GDK_താക്കോൽ_RELEASE:
	    താക്കോൽ_event(beval, gdk_താക്കോൽ_event_get_keyval(event),
		    type == GDK_താക്കോൽ_PRESS);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }

    // Always pass through events?
    തിരികെ FALSE;
}
#  അല്ലെങ്കിൽ
    സ്ഥിതം gint
target_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    തിരഞ്ഞെടുപ്പ് (event->type)
    {
	അവസ്ഥ GDK_ENTER_NOTIFY:
	    pointer_event(beval, (പൂർണ്ണസംഖ്യ)event->crossing.x,
				 (പൂർണ്ണസംഖ്യ)event->crossing.y,
				 event->crossing.state);
	    നിർത്തുക;
	അവസ്ഥ GDK_MOTION_NOTIFY:
	    എങ്കിൽ (event->motion.is_hint)
	    {
		പൂർണ്ണസംഖ്യ		x;
		പൂർണ്ണസംഖ്യ		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
		{
		    GdkWindow * സ്ഥിരം win = gtk_widget_get_window(widget);
		    GdkDisplay * സ്ഥിരം dpy = gdk_window_get_display(win);
#    എങ്കിൽ GTK_CHECK_VERSION(3,20,0)
		    GdkSeat * സ്ഥിരം seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * സ്ഥിരം dev = gdk_seat_get_pointer(seat);
#    അല്ലെങ്കിൽ
		    GdkDeviceManager * സ്ഥിരം mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * സ്ഥിരം dev = gdk_device_manager_get_client_pointer(mngr);
#    endif
		    gdk_window_get_device_position(win, dev , &x, &y, &state);
		}
#   അല്ലെങ്കിൽ
		gdk_window_get_pointer(widget->window, &x, &y, &state);
#   endif
		pointer_event(beval, x, y, (unsigned പൂർണ്ണസംഖ്യ)state);
	    }
	    അല്ലെങ്കിൽ
	    {
		pointer_event(beval, (പൂർണ്ണസംഖ്യ)event->motion.x,
				     (പൂർണ്ണസംഖ്യ)event->motion.y,
				     event->motion.state);
	    }
	    നിർത്തുക;
	അവസ്ഥ GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    എങ്കിൽ (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    നിർത്തുക;
	അവസ്ഥ GDK_BUTTON_PRESS:
	അവസ്ഥ GDK_SCROLL:
	    cancelBalloon(beval);
	    നിർത്തുക;
	അവസ്ഥ GDK_താക്കോൽ_PRESS:
	    താക്കോൽ_event(beval, event->താക്കോൽ.keyval, TRUE);
	    നിർത്തുക;
	അവസ്ഥ GDK_താക്കോൽ_RELEASE:
	    താക്കോൽ_event(beval, event->താക്കോൽ.keyval, FALSE);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }

    തിരികെ FALSE; // continue emission
}

    സ്ഥിതം gint
mainwin_event_cb(GtkWidget *widget UNUSED, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    തിരഞ്ഞെടുപ്പ് (event->type)
    {
	അവസ്ഥ GDK_താക്കോൽ_PRESS:
	    താക്കോൽ_event(beval, event->താക്കോൽ.keyval, TRUE);
	    നിർത്തുക;
	അവസ്ഥ GDK_താക്കോൽ_RELEASE:
	    താക്കോൽ_event(beval, event->താക്കോൽ.keyval, FALSE);
	    നിർത്തുക;
	അവസ്ഥ GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    എങ്കിൽ (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }

    തിരികെ FALSE; // continue emission
}
#  endif

    സ്ഥിതം ശൂന്യം
pointer_event(BalloonEval *beval, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, unsigned state)
{
    പൂർണ്ണസംഖ്യ distance;

    distance = ABS(x - beval->x) + ABS(y - beval->y);

    എങ്കിൽ (distance <= 4)
	തിരികെ;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    എങ്കിൽ (!(state & ((പൂർണ്ണസംഖ്യ)GDK_BUTTON1_MASK | (പൂർണ്ണസംഖ്യ)GDK_BUTTON2_MASK
		    | (പൂർണ്ണസംഖ്യ)GDK_BUTTON3_MASK)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	എങ്കിൽ (state & (പൂർണ്ണസംഖ്യ)GDK_ALT_MASK)
#  അല്ലെങ്കിൽ
	എങ്കിൽ (state & (പൂർണ്ണസംഖ്യ)GDK_MOD1_MASK)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    എങ്കിൽ (beval->സന്ദേശം_CB != ശൂന്യ)
	    {
		beval->showState = ShS_PENDING;
		(*beval->സന്ദേശം_CB)(beval, state);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    beval->timerID = g_timeout_add((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    സ്ഥിതം ശൂന്യം
താക്കോൽ_event(BalloonEval *beval, unsigned keyval, പൂർണ്ണസംഖ്യ is_keypress)
{
    എങ്കിൽ (beval->showState == ShS_SHOWING && beval->സന്ദേശം_CB != ശൂന്യ)
    {
	തിരഞ്ഞെടുപ്പ് (keyval)
	{
#  ifdef USE_GTK4
	    അവസ്ഥ GDK_താക്കോൽ_Shift_L:
	    അവസ്ഥ GDK_താക്കോൽ_Shift_R:
#  അല്ലെങ്കിൽ
	    അവസ്ഥ GDK_Shift_L:
	    അവസ്ഥ GDK_Shift_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->സന്ദേശം_CB)(beval, (is_keypress)
						   ? (പൂർണ്ണസംഖ്യ)GDK_SHIFT_MASK : 0);
		നിർത്തുക;
#  ifdef USE_GTK4
	    അവസ്ഥ GDK_താക്കോൽ_Control_L:
	    അവസ്ഥ GDK_താക്കോൽ_Control_R:
#  അല്ലെങ്കിൽ
	    അവസ്ഥ GDK_Control_L:
	    അവസ്ഥ GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->സന്ദേശം_CB)(beval, (is_keypress)
						 ? (പൂർണ്ണസംഖ്യ)GDK_CONTROL_MASK : 0);
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		എങ്കിൽ (is_keypress)
		    cancelBalloon(beval);
		നിർത്തുക;
	}
    }
    അല്ലെങ്കിൽ
	cancelBalloon(beval);
}

    സ്ഥിതം gboolean
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

    തിരികെ FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 data UNUSED)
{
    GtkStyleContext *context = ശൂന്യ;
    gint width = -1, height = -1;

    എങ്കിൽ (widget == ശൂന്യ)
	തിരികെ TRUE;

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

    തിരികെ FALSE;
}
#   അല്ലെങ്കിൽ
    സ്ഥിതം gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer data UNUSED)
{
    gtk_paint_flat_box(widget->style, widget->window,
		       GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    തിരികെ FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# അല്ലെങ്കിൽ // !FEAT_GUI_GTK

    സ്ഥിതം ശൂന്യം
addEventHandler(Widget target, BalloonEval *beval)
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | താക്കോൽ_Press_Mask |
			താക്കോൽ_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}

    സ്ഥിതം ശൂന്യം
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | താക്കോൽ_Press_Mask |
			താക്കോൽ_Release_Mask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    സ്ഥിതം ശൂന്യം
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

    സ്ഥിതം ശൂന്യം
pointerEvent(BalloonEval *beval, XEvent *event)
{
    Position	distance;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    തിരഞ്ഞെടുപ്പ് (event->type)
    {
	അവസ്ഥ EnterNotify:
	അവസ്ഥ MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    എങ്കിൽ (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    എങ്കിൽ (delta < 0)
		delta = -delta;
	    distance += delta;
	    എങ്കിൽ (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		എങ്കിൽ (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_വേര് = event->xmotion.x_വേര്;
		    beval->y_വേര് = event->xmotion.y_വേര്;
		    cancelBalloon(beval);
		    എങ്കിൽ (beval->സന്ദേശം_CB != ശൂന്യ)
		    {
			beval->showState = ShS_PENDING;
			(*beval->സന്ദേശം_CB)(beval, beval->state);
		    }
		}
		അല്ലെങ്കിൽ
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_വേര് = event->xmotion.x_വേര്;
		    beval->y_വേര് = event->xmotion.y_വേര്;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    നിർത്തുക;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	അവസ്ഥ താക്കോൽ_Press:
	    എങ്കിൽ (beval->showState == ShS_SHOWING && beval->സന്ദേശം_CB != ശൂന്യ)
	    {
		Modifiers   modifier;
		താക്കോൽ_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		എങ്കിൽ (keysym == XK_Shift_L || keysym == XK_Shift_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->സന്ദേശം_CB)(beval, ShiftMask);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->സന്ദേശം_CB)(beval, ControlMask);
		}
		അല്ലെങ്കിൽ
		    cancelBalloon(beval);
	    }
	    അല്ലെങ്കിൽ
		cancelBalloon(beval);
	    നിർത്തുക;

	അവസ്ഥ താക്കോൽ_Release:
	    എങ്കിൽ (beval->showState == ShS_SHOWING && beval->സന്ദേശം_CB != ശൂന്യ)
	    {
		Modifiers modifier;
		താക്കോൽ_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		എങ്കിൽ ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->സന്ദേശം_CB)(beval, 0);
		}
		അല്ലെങ്കിൽ എങ്കിൽ ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->സന്ദേശം_CB)(beval, 0);
		}
		അല്ലെങ്കിൽ
		    cancelBalloon(beval);
	    }
	    അല്ലെങ്കിൽ
		cancelBalloon(beval);
	    നിർത്തുക;

	അവസ്ഥ LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		എങ്കിൽ (event->xcrossing.mode == NotifyNormal)
		    cancelBalloon(beval);
		നിർത്തുക;

	അവസ്ഥ ButtonPress:
		cancelBalloon(beval);
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
}

    സ്ഥിതം ശൂന്യം
timerRoutine(XtPointer dx, XtIntervalId *id UNUSED)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)ശൂന്യ;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    സ്ഥിതം ശൂന്യം
requestBalloon(BalloonEval *beval)
{
    എങ്കിൽ (beval->showState == ShS_PENDING)
	തിരികെ;

    // Determine the beval to display
    എങ്കിൽ (beval->സന്ദേശം_CB != ശൂന്യ)
    {
	beval->showState = ShS_PENDING;
	(*beval->സന്ദേശം_CB)(beval, beval->state);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (beval->സന്ദേശം != ശൂന്യ)
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
    സ്ഥിതം ശൂന്യം
set_printable_label_text(GtkLabel *label, char_u *text)
{
    char_u	    *convbuf = ശൂന്യ;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    unsigned പൂർണ്ണസംഖ്യ    len;
    പൂർണ്ണസംഖ്യ		    charlen;
    പൂർണ്ണസംഖ്യ		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    എങ്കിൽ (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, text, ശൂന്യ);
	എങ്കിൽ (convbuf != ശൂന്യ)
	    text = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    വേണ്ടി (p = text; *p != NUL; p += charlen)
    {
	എങ്കിൽ ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	അല്ലെങ്കിൽ
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    എങ്കിൽ (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    എങ്കിൽ (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    അല്ലെങ്കിൽ എങ്കിൽ (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    അല്ലെങ്കിൽ
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    എങ്കിൽ (buf != ശൂന്യ)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   എങ്കിൽ PANGO_VERSION_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  അല്ലെങ്കിൽ
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != ശൂന്യ) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	എങ്കിൽ (pixel != INVALCOLOR)
#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  അല്ലെങ്കിൽ
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (unsigned long)pixel, &color);
#  endif

	pdest = buf;
	p = text;
	ആകുമ്പോൾ (*p != NUL)
	{
	    // Be quick for ASCII
	    എങ്കിൽ ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    അല്ലെങ്കിൽ
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		എങ്കിൽ (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		എങ്കിൽ (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    പൂർണ്ണസംഖ്യ	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    എങ്കിൽ (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    അല്ലെങ്കിൽ
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    എങ്കിൽ (pixel != INVALCOLOR)
		    {
#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   എങ്കിൽ PANGO_VERSION_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  അല്ലെങ്കിൽ
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->start_index = pdest - buf;
			attr->end_index   = pdest - buf + outlen;
			pango_attr_list_തിരുകുക(attr_list, attr);
#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
#   എങ്കിൽ PANGO_VERSION_CHECK(1,38,0)
			attr_alpha->start_index = pdest - buf;
			attr_alpha->end_index   = pdest - buf + outlen;
			pango_attr_list_തിരുകുക(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		അല്ലെങ്കിൽ
		{
		    ചെയ്യുക
			*pdest++ = *p++;
		    ആകുമ്പോൾ (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_set_text(label, (സ്ഥിരം അക്ഷരം *)buf);
    vim_free(buf);

    gtk_label_set_attributes(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    സ്ഥിതം ശൂന്യം
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    പൂർണ്ണസംഖ്യ		    natural_width = 0;

    എങ്കിൽ (beval->സന്ദേശം == ശൂന്യ)
	തിരികെ;

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->സന്ദേശം);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    ശൂന്യ, &natural_width, ശൂന്യ, ശൂന്യ);

    rect.width = (natural_width > 0 ? natural_width : 1) + gui.char_width * 3;
    rect.height = gui.char_height;

    gtk_popover_set_pointing_to(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  അല്ലെങ്കിൽ
    സ്ഥിതം ശൂന്യം
drawBalloon(BalloonEval *beval)
{
    എങ്കിൽ (beval->സന്ദേശം == ശൂന്യ)
	തിരികെ;

    GtkRequisition	requisition;
    പൂർണ്ണസംഖ്യ		screen_w;
    പൂർണ്ണസംഖ്യ		screen_h;
    പൂർണ്ണസംഖ്യ		screen_x;
    പൂർണ്ണസംഖ്യ		screen_y;
    പൂർണ്ണസംഖ്യ		x;
    പൂർണ്ണസംഖ്യ		y;
    പൂർണ്ണസംഖ്യ		x_offset = EVAL_OFFSET_X;
    പൂർണ്ണസംഖ്യ		y_offset = EVAL_OFFSET_Y;
    PangoLayout	*layout;

#   എങ്കിൽ !GTK_CHECK_VERSION(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->target);
    gtk_window_set_screen(GTK_WINDOW(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_of_win(beval->target, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->സന്ദേശം);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_WORD_CHAR
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
#   അല്ലെങ്കിൽ
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
#   endif
    pango_layout_set_width(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.അക്കം_cols * gui.char_width,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    gtk_widget_get_preferred_size(beval->balloonShell, &requisition, ശൂന്യ);
#   അല്ലെങ്കിൽ
    gtk_widget_size_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_window_get_origin(gtk_widget_get_window(beval->target), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    എങ്കിൽ (x + x_offset + requisition.width > screen_x + screen_w)
	y_offset += 15;
    എങ്കിൽ (y + y_offset + requisition.height > screen_y + screen_h)
	y_offset = -requisition.height - EVAL_OFFSET_Y;

    // Sanitize values
    x = CLAMP(x + x_offset, 0,
	    MAX(0, screen_x + screen_w - requisition.width));
    y = CLAMP(y + y_offset, 0,
	    MAX(0, screen_y + screen_h - requisition.height));

    // Show the balloon
#   എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    gtk_window_move(GTK_WINDOW(beval->balloonShell), x, y);
#   അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
undrawBalloon(BalloonEval *beval)
{
    എങ്കിൽ (beval->balloonShell != ശൂന്യ)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  അല്ലെങ്കിൽ
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    സ്ഥിതം ശൂന്യം
cancelBalloon(BalloonEval *beval)
{
    എങ്കിൽ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    എങ്കിൽ (beval->timerID != 0)
    {
	g_source_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    സ്ഥിതം ശൂന്യം
createBalloonEvalWindow(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_set_parent(beval->balloonShell, beval->target);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_set_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(ശൂന്യ);

    gtk_label_set_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ഇടത്);
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_set_child(GTK_POPOVER(popover), beval->balloonLabel);
}
#  അല്ലെങ്കിൽ
    സ്ഥിതം ശൂന്യം
createBalloonEvalWindow(BalloonEval *beval)
{
    beval->balloonShell = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_transient_for(GTK_WINDOW(beval->balloonShell), GTK_WINDOW(gui.mainwin));

    gtk_widget_set_app_paintable(beval->balloonShell, TRUE);
    gtk_window_set_resizable(GTK_WINDOW(beval->balloonShell), FALSE);
    gtk_widget_set_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_set_border_width(GTK_CONTAINER(beval->balloonShell), 4);

#   എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), ശൂന്യ);
#   അല്ലെങ്കിൽ
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), ശൂന്യ);
#   endif
    beval->balloonLabel = gtk_label_new(ശൂന്യ);

    gtk_label_set_വരി_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ഇടത്);
#   എങ്കിൽ GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_VERSION(3,14,0)
    GValue align_val = G_മൂല്യം_INIT;
    g_മൂല്യം_init(&align_val, G_TYPE_FLOAT);
    g_മൂല്യം_set_float(&align_val, 0.5);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_മൂല്യം_unset(&align_val);
#   അല്ലെങ്കിൽ
    gtk_misc_set_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_set_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_add(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# അല്ലെങ്കിൽ // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    സ്ഥിതം ശൂന്യം
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    എങ്കിൽ (beval->സന്ദേശം == ശൂന്യ)
	തിരികെ;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    എങ്കിൽ (beval->സന്ദേശം_CB != ശൂന്യ)
	s = XmStringCreateLtoR((അക്ഷരം *)beval->സന്ദേശം, XmFONTLIST_DEFAULT_TAG);
    അല്ലെങ്കിൽ
	s = XmStringCreateLocalized((അക്ഷരം *)beval->സന്ദേശം);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	എങ്കിൽ (fl == ശൂന്യ)
	{
	    XmStringFree(s);
	    തിരികെ;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_offset << 1;
    h += gui.border_offset << 1;
    Xt_Va_Set_മൂല്യങ്ങൾ(beval->balloonLabel, XmNlabelString, s, ശൂന്യ);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_വേര് + EVAL_OFFSET_X;
    ty = beval->y_വേര് + EVAL_OFFSET_Y;
    എങ്കിൽ ((tx + w) > beval->screen_width)
	tx = beval->screen_width - w;
    എങ്കിൽ ((ty + h) > beval->screen_height)
	ty = beval->screen_height - h;
    Xt_Va_Set_മൂല്യങ്ങൾ(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    ശൂന്യ);
    // Set tooltip colors
    {
	Arg args[2];

	args[0].name = XmNbackground;
	args[0].മൂല്യം = gui.tooltip_bg_pixel;
	args[1].name = XmNforeground;
	args[1].മൂല്യം = gui.tooltip_fg_pixel;
	Xt_Set_മൂല്യങ്ങൾ(beval->balloonLabel, &args[0], Xt_സംഖ്യ(args));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    സ്ഥിതം ശൂന്യം
undrawBalloon(BalloonEval *beval)
{
    എങ്കിൽ (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം
cancelBalloon(BalloonEval *beval)
{
    എങ്കിൽ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    എങ്കിൽ (beval->timerID != (XtIntervalId)ശൂന്യ)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = (XtIntervalId)ശൂന്യ;
    }
    beval->showState = ShS_NEUTRAL;
}


    സ്ഥിതം ശൂന്യം
createBalloonEvalWindow(BalloonEval *beval)
{
    Arg		args[12];
    പൂർണ്ണസംഖ്യ		n;

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
