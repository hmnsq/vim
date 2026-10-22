/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

#ᮚᮦᮔ᮪ defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#ᮚᮦᮔ᮪ !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_VERSION(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  ᮜᮤᮚ
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# ᮜᮤᮚ
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
extern Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
static BalloonEval *current_beval = NULL;
# endif

# ifdef FEAT_GUI_GTK
static void addEventHandler(GtkWidget *, BalloonEval *);
static void removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
static gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  ᮜᮤᮚ
static gint target_event_cb(GtkWidget *, GdkEvent *, gpointer);
static gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
static void pointer_event(BalloonEval *, puur_naa_nka, puur_naa_nka, unsigned);
static void key_event(BalloonEval *, unsigned, puur_naa_nka);
static gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
static gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   ᮜᮤᮚ
static gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# ᮜᮤᮚ
static void addEventHandler(Widget, BalloonEval *);
static void removeEventHandler(BalloonEval *);
static void pointerEventEH(Widget, XtPointer, XEvent *, Boolean *);
static void pointerEvent(BalloonEval *, XEvent *);
static void timerRoutine(XtPointer, XtIntervalId *);
# endif
static void cancelBalloon(BalloonEval *);
static void requestBalloon(BalloonEval *);
static void drawBalloon(BalloonEval *);
static void undrawBalloon(BalloonEval *beval);
static void createBalloonEvalWindow(BalloonEval *);

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
    void	*target,
    char_u	*mesg,
    void	(*mesgCB)(BalloonEval *, puur_naa_nka),
    void	*clientData)
{
# ifndef FEAT_GUI_GTK
    var_na	*display_name;	    // get from gui.dpy
    puur_naa_nka		screen_num;
    var_na	*p;
# endif
    BalloonEval	*beval;

    ᮚᮦᮔ᮪ (mesg != NULL && mesgCB != NULL)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	ᮝᮜᮤ NULL;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ᮚᮦᮔ᮪ (beval != NULL)
    {
# ifdef FEAT_GUI_GTK
	beval->target = GTK_WIDGET(target);
# ᮜᮤᮚ
	beval->target = (Widget)target;
	beval->appContext = XtWidgetToApplicationContext((Widget)target);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->msg);
	beval->msg = mesg == NULL ? NULL : vim_strsave(mesg);
	beval->msgCB = mesgCB;
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
	ᮚᮦᮔ᮪ (p != NULL)
	    screen_num = atoi(++p);
	ᮜᮤᮚ
	    screen_num = 0;
	beval->screen_width = DisplayWidth(gui.dpy, screen_num);
	beval->screen_height = DisplayHeight(gui.dpy, screen_num);
# endif
    }

    ᮝᮜᮤ beval;
}

# ᮚᮦᮔ᮪ defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    void
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   ᮜᮤᮚ
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  ᮜᮤᮚ
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    ᮚᮦᮔ᮪ (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->msg);
    vim_free(beval);
}
# endif

    void
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ᮚᮦᮔ᮪ (beval != NULL)
	addEventHandler(beval->target, beval);
}

    void
gui_mch_disable_beval_area(BalloonEval *beval)
{
    ᮚᮦᮔ᮪ (beval != NULL)
	removeEventHandler(beval);
}

# ᮚᮦᮔ᮪ defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(void)
{
    ᮝᮜᮤ current_beval;
}
# endif
#endif // !FEAT_GUI_MSWIN

#ᮚᮦᮔ᮪ defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# ᮚᮦᮔ᮪ !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    void
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->msg);
    beval->msg = mesg == NULL ? NULL : vim_strsave(mesg);
    ᮚᮦᮔ᮪ (beval->msg != NULL)
	drawBalloon(beval);
    ᮜᮤᮚ
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#endif // FEAT_NETBEANS_INTG || FEAT_EVAL

#ᮚᮦᮔ᮪ !defined(FEAT_GUI_MSWIN)
# ᮚᮦᮔ᮪ defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    void
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->msg);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    static void
addEventHandler(GtkWidget *target, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_set_data(G_OBJECT(target), "beval", controller);
    gtk_widget_add_controller(target, controller);
#  ᮜᮤᮚ
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
    ᮚᮦᮔ᮪ (gtk_socket_id == 0 && gui.mainwin != NULL
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

    static void
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*target = beval->target;
    GtkEventController	*controller;

    controller = g_object_get_data(G_OBJECT(target), "beval");
    ᮚᮦᮔ᮪ (controller != NULL)
    {
	gtk_widget_remove_controller(target, controller);
	g_object_set_data(G_OBJECT(target), "beval", NULL);
    }
#  ᮜᮤᮚ
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->target),
					 FUNC2GENERIC(target_event_cb),
					 beval);

    ᮚᮦᮔ᮪ (gtk_socket_id == 0 && gui.mainwin != NULL
	    && gtk_widget_is_ancestor(beval->target, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    static gboolean
event_cb(GtkEventController *controller UNUSED, GdkEvent *event, BalloonEval *beval)
{
    puur_naa_nka type = gdk_event_get_event_type(event);

    switch (type)
    {
	pak_sa GDK_ENTER_NOTIFY:
	pak_sa GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*root;
	    graphene_point_t	p;
	    graphene_point_t	result;
	    dviguu_na		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    root = GTK_WIDGET(gtk_widget_get_root(beval->target));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    ᮚᮦᮔ᮪ (gtk_widget_compute_point(root, beval->target, &p, &result))
	    {
		x = result.x;
		y = result.y;
	    }

	    pointer_event(beval, (puur_naa_nka)x, (puur_naa_nka)y,
		    gdk_event_get_modifier_state(event));
	    bha_nga;
	}
	pak_sa GDK_LEAVE_NOTIFY:
	    ᮚᮦᮔ᮪ (gdk_crossing_event_get_mode(event) == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    bha_nga;
	pak_sa GDK_BUTTON_PRESS:
	pak_sa GDK_SCROLL:
	    cancelBalloon(beval);
	    bha_nga;
	pak_sa GDK_KEY_PRESS:
	pak_sa GDK_KEY_RELEASE:
	    key_event(beval, gdk_key_event_get_keyval(event),
		    type == GDK_KEY_PRESS);
	    bha_nga;
	default:
	    bha_nga;
    }

    // Always pass through events?
    ᮝᮜᮤ FALSE;
}
#  ᮜᮤᮚ
    static gint
target_event_cb(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    switch (event->type)
    {
	pak_sa GDK_ENTER_NOTIFY:
	    pointer_event(beval, (puur_naa_nka)event->crossing.x,
				 (puur_naa_nka)event->crossing.y,
				 event->crossing.state);
	    bha_nga;
	pak_sa GDK_MOTION_NOTIFY:
	    ᮚᮦᮔ᮪ (event->motion.is_hint)
	    {
		puur_naa_nka		x;
		puur_naa_nka		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
		{
		    GdkWindow * const win = gtk_widget_get_window(widget);
		    GdkDisplay * const dpy = gdk_window_get_display(win);
#    ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,20,0)
		    GdkSeat * const seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * const dev = gdk_seat_get_pointer(seat);
#    ᮜᮤᮚ
		    GdkDeviceManager * const mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * const dev = gdk_device_manager_get_client_pointer(mngr);
#    endif
		    gdk_window_get_device_position(win, dev , &x, &y, &state);
		}
#   ᮜᮤᮚ
		gdk_window_get_pointer(widget->window, &x, &y, &state);
#   endif
		pointer_event(beval, x, y, (unsigned puur_naa_nka)state);
	    }
	    ᮜᮤᮚ
	    {
		pointer_event(beval, (puur_naa_nka)event->motion.x,
				     (puur_naa_nka)event->motion.y,
				     event->motion.state);
	    }
	    bha_nga;
	pak_sa GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    ᮚᮦᮔ᮪ (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    bha_nga;
	pak_sa GDK_BUTTON_PRESS:
	pak_sa GDK_SCROLL:
	    cancelBalloon(beval);
	    bha_nga;
	pak_sa GDK_KEY_PRESS:
	    key_event(beval, event->key.keyval, TRUE);
	    bha_nga;
	pak_sa GDK_KEY_RELEASE:
	    key_event(beval, event->key.keyval, FALSE);
	    bha_nga;
	default:
	    bha_nga;
    }

    ᮝᮜᮤ FALSE; // continue emission
}

    static gint
mainwin_event_cb(GtkWidget *widget UNUSED, GdkEvent *event, gpointer data)
{
    BalloonEval *beval = (BalloonEval *)data;

    switch (event->type)
    {
	pak_sa GDK_KEY_PRESS:
	    key_event(beval, event->key.keyval, TRUE);
	    bha_nga;
	pak_sa GDK_KEY_RELEASE:
	    key_event(beval, event->key.keyval, FALSE);
	    bha_nga;
	pak_sa GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    ᮚᮦᮔ᮪ (event->crossing.mode == GDK_CROSSING_NORMAL)
		cancelBalloon(beval);
	    bha_nga;
	default:
	    bha_nga;
    }

    ᮝᮜᮤ FALSE; // continue emission
}
#  endif

    static void
pointer_event(BalloonEval *beval, puur_naa_nka x, puur_naa_nka y, unsigned state)
{
    puur_naa_nka distance;

    distance = ABS(x - beval->x) + ABS(y - beval->y);

    ᮚᮦᮔ᮪ (distance <= 4)
	ᮝᮜᮤ;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    ᮚᮦᮔ᮪ (!(state & ((puur_naa_nka)GDK_BUTTON1_MASK | (puur_naa_nka)GDK_BUTTON2_MASK
		    | (puur_naa_nka)GDK_BUTTON3_MASK)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	ᮚᮦᮔ᮪ (state & (puur_naa_nka)GDK_ALT_MASK)
#  ᮜᮤᮚ
	ᮚᮦᮔ᮪ (state & (puur_naa_nka)GDK_MOD1_MASK)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    ᮚᮦᮔ᮪ (beval->msgCB != NULL)
	    {
		beval->showState = ShS_PENDING;
		(*beval->msgCB)(beval, state);
	    }
	}
	ᮜᮤᮚ
	{
	    beval->timerID = g_timeout_add((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    static void
key_event(BalloonEval *beval, unsigned keyval, puur_naa_nka is_keypress)
{
    ᮚᮦᮔ᮪ (beval->showState == ShS_SHOWING && beval->msgCB != NULL)
    {
	switch (keyval)
	{
#  ifdef USE_GTK4
	    pak_sa GDK_KEY_Shift_L:
	    pak_sa GDK_KEY_Shift_R:
#  ᮜᮤᮚ
	    pak_sa GDK_Shift_L:
	    pak_sa GDK_Shift_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->msgCB)(beval, (is_keypress)
						   ? (puur_naa_nka)GDK_SHIFT_MASK : 0);
		bha_nga;
#  ifdef USE_GTK4
	    pak_sa GDK_KEY_Control_L:
	    pak_sa GDK_KEY_Control_R:
#  ᮜᮤᮚ
	    pak_sa GDK_Control_L:
	    pak_sa GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->msgCB)(beval, (is_keypress)
						 ? (puur_naa_nka)GDK_CONTROL_MASK : 0);
		bha_nga;
	    default:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		ᮚᮦᮔ᮪ (is_keypress)
		    cancelBalloon(beval);
		bha_nga;
	}
    }
    ᮜᮤᮚ
	cancelBalloon(beval);
}

    static gboolean
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

    ᮝᮜᮤ FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
    static gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 data UNUSED)
{
    GtkStyleContext *context = NULL;
    gint width = -1, height = -1;

    ᮚᮦᮔ᮪ (widget == NULL)
	ᮝᮜᮤ TRUE;

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

    ᮝᮜᮤ FALSE;
}
#   ᮜᮤᮚ
    static gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer data UNUSED)
{
    gtk_paint_flat_box(widget->style, widget->window,
		       GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    ᮝᮜᮤ FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# ᮜᮤᮚ // !FEAT_GUI_GTK

    static void
addEventHandler(Widget target, BalloonEval *beval)
{
    XtAddEventHandler(target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | KeyPressMask |
			KeyReleaseMask,
			False,
			pointerEventEH, (XtPointer)beval);
}

    static void
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->target,
			PointerMotionMask | EnterWindowMask |
			LeaveWindowMask | ButtonPressMask | KeyPressMask |
			KeyReleaseMask,
			False,
			pointerEventEH, (XtPointer)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    static void
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

    static void
pointerEvent(BalloonEval *beval, XEvent *event)
{
    Position	distance;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    switch (event->type)
    {
	pak_sa EnterNotify:
	pak_sa MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    ᮚᮦᮔ᮪ (delta < 0)
		delta = -delta;
	    distance = delta;
	    delta = event->xmotion.y - beval->y;
	    ᮚᮦᮔ᮪ (delta < 0)
		delta = -delta;
	    distance += delta;
	    ᮚᮦᮔ᮪ (distance > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		ᮚᮦᮔ᮪ (beval->state & (Button1Mask|Button2Mask|Button3Mask))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (beval->state & (Mod1Mask|Mod2Mask|Mod3Mask))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_root = event->xmotion.x_root;
		    beval->y_root = event->xmotion.y_root;
		    cancelBalloon(beval);
		    ᮚᮦᮔ᮪ (beval->msgCB != NULL)
		    {
			beval->showState = ShS_PENDING;
			(*beval->msgCB)(beval, beval->state);
		    }
		}
		ᮜᮤᮚ
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_root = event->xmotion.x_root;
		    beval->y_root = event->xmotion.y_root;
		    cancelBalloon(beval);
		    beval->timerID = XtAppAddTimeOut( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    bha_nga;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	pak_sa KeyPress:
	    ᮚᮦᮔ᮪ (beval->showState == ShS_SHOWING && beval->msgCB != NULL)
	    {
		Modifiers   modifier;
		KeySym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		ᮚᮦᮔ᮪ (keysym == XK_Shift_L || keysym == XK_Shift_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->msgCB)(beval, ShiftMask);
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->msgCB)(beval, ControlMask);
		}
		ᮜᮤᮚ
		    cancelBalloon(beval);
	    }
	    ᮜᮤᮚ
		cancelBalloon(beval);
	    bha_nga;

	pak_sa KeyRelease:
	    ᮚᮦᮔ᮪ (beval->showState == ShS_SHOWING && beval->msgCB != NULL)
	    {
		Modifiers modifier;
		KeySym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		ᮚᮦᮔ᮪ ((keysym == XK_Shift_L) || (keysym == XK_Shift_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->msgCB)(beval, 0);
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->msgCB)(beval, 0);
		}
		ᮜᮤᮚ
		    cancelBalloon(beval);
	    }
	    ᮜᮤᮚ
		cancelBalloon(beval);
	    bha_nga;

	pak_sa LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		ᮚᮦᮔ᮪ (event->xcrossing.mode == NotifyNormal)
		    cancelBalloon(beval);
		bha_nga;

	pak_sa ButtonPress:
		cancelBalloon(beval);
		bha_nga;

	default:
	    bha_nga;
    }
}

    static void
timerRoutine(XtPointer dx, XtIntervalId *id UNUSED)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)NULL;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    static void
requestBalloon(BalloonEval *beval)
{
    ᮚᮦᮔ᮪ (beval->showState == ShS_PENDING)
	ᮝᮜᮤ;

    // Determine the beval to display
    ᮚᮦᮔ᮪ (beval->msgCB != NULL)
    {
	beval->showState = ShS_PENDING;
	(*beval->msgCB)(beval, beval->state);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (beval->msg != NULL)
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
    static void
set_printable_label_text(GtkLabel *label, char_u *text)
{
    char_u	    *convbuf = NULL;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    unsigned puur_naa_nka    len;
    puur_naa_nka		    charlen;
    puur_naa_nka		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    ᮚᮦᮔ᮪ (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, text, NULL);
	ᮚᮦᮔ᮪ (convbuf != NULL)
	    text = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    for (p = text; *p != NUL; p += charlen)
    {
	ᮚᮦᮔ᮪ ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	ᮜᮤᮚ
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    ᮚᮦᮔ᮪ (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    ᮚᮦᮔ᮪ (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    ᮜᮤᮚ
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    ᮚᮦᮔ᮪ (buf != NULL)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   ᮚᮦᮔ᮪ PANGO_VERSION_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  ᮜᮤᮚ
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != NULL) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	ᮚᮦᮔ᮪ (pixel != INVALCOLOR)
#  ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  ᮜᮤᮚ
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (unsigned long)pixel, &color);
#  endif

	pdest = buf;
	p = text;
	ᮞᮨᮓᮨᮜ (*p != NUL)
	{
	    // Be quick for ASCII
	    ᮚᮦᮔ᮪ ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    ᮜᮤᮚ
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		ᮚᮦᮔ᮪ (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		ᮚᮦᮔ᮪ (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    puur_naa_nka	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    ᮚᮦᮔ᮪ (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    ᮜᮤᮚ ᮚᮦᮔ᮪ (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    ᮜᮤᮚ
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    ᮚᮦᮔ᮪ (pixel != INVALCOLOR)
		    {
#  ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   ᮚᮦᮔ᮪ PANGO_VERSION_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  ᮜᮤᮚ
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->start_index = pdest - buf;
			attr->end_index   = pdest - buf + outlen;
			pango_attr_list_insert(attr_list, attr);
#  ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
#   ᮚᮦᮔ᮪ PANGO_VERSION_CHECK(1,38,0)
			attr_alpha->start_index = pdest - buf;
			attr_alpha->end_index   = pdest - buf + outlen;
			pango_attr_list_insert(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		ᮜᮤᮚ
		{
		    do
			*pdest++ = *p++;
		    ᮞᮨᮓᮨᮜ (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_set_text(label, (const var_na *)buf);
    vim_free(buf);

    gtk_label_set_attributes(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    static void
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    puur_naa_nka		    natural_width = 0;

    ᮚᮦᮔ᮪ (beval->msg == NULL)
	ᮝᮜᮤ;

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->msg);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    NULL, &natural_width, NULL, NULL);

    rect.width = (natural_width > 0 ? natural_width : 1) + gui.char_width * 3;
    rect.height = gui.char_height;

    gtk_popover_set_pointing_to(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  ᮜᮤᮚ
    static void
drawBalloon(BalloonEval *beval)
{
    ᮚᮦᮔ᮪ (beval->msg == NULL)
	ᮝᮜᮤ;

    GtkRequisition	requisition;
    puur_naa_nka		screen_w;
    puur_naa_nka		screen_h;
    puur_naa_nka		screen_x;
    puur_naa_nka		screen_y;
    puur_naa_nka		x;
    puur_naa_nka		y;
    puur_naa_nka		x_offset = EVAL_OFFSET_X;
    puur_naa_nka		y_offset = EVAL_OFFSET_Y;
    PangoLayout	*layout;

#   ᮚᮦᮔ᮪ !GTK_CHECK_VERSION(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->target);
    gtk_window_set_screen(GTK_WINDOW(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_of_win(beval->target, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   ᮚᮦᮔ᮪ !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    set_printable_label_text(GTK_LABEL(beval->balloonLabel), beval->msg);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_WORD_CHAR
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
#   ᮜᮤᮚ
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
#   endif
    pango_layout_set_width(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.num_cols * gui.char_width,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
    gtk_widget_get_preferred_size(beval->balloonShell, &requisition, NULL);
#   ᮜᮤᮚ
    gtk_widget_size_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_window_get_origin(gtk_widget_get_window(beval->target), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    ᮚᮦᮔ᮪ (x + x_offset + requisition.width > screen_x + screen_w)
	y_offset += 15;
    ᮚᮦᮔ᮪ (y + y_offset + requisition.height > screen_y + screen_h)
	y_offset = -requisition.height - EVAL_OFFSET_Y;

    // Sanitize values
    x = CLAMP(x + x_offset, 0,
	    MAX(0, screen_x + screen_w - requisition.width));
    y = CLAMP(y + y_offset, 0,
	    MAX(0, screen_y + screen_h - requisition.height));

    // Show the balloon
#   ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
    gtk_window_move(GTK_WINDOW(beval->balloonShell), x, y);
#   ᮜᮤᮚ
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
    static void
undrawBalloon(BalloonEval *beval)
{
    ᮚᮦᮔ᮪ (beval->balloonShell != NULL)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  ᮜᮤᮚ
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    static void
cancelBalloon(BalloonEval *beval)
{
    ᮚᮦᮔ᮪ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ᮚᮦᮔ᮪ (beval->timerID != 0)
    {
	g_source_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    static void
createBalloonEvalWindow(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_set_parent(beval->balloonShell, beval->target);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_set_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(NULL);

    gtk_label_set_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_LEFT);
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_set_child(GTK_POPOVER(popover), beval->balloonLabel);
}
#  ᮜᮤᮚ
    static void
createBalloonEvalWindow(BalloonEval *beval)
{
    beval->balloonShell = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_transient_for(GTK_WINDOW(beval->balloonShell), GTK_WINDOW(gui.mainwin));

    gtk_widget_set_app_paintable(beval->balloonShell, TRUE);
    gtk_window_set_resizable(GTK_WINDOW(beval->balloonShell), FALSE);
    gtk_widget_set_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_set_border_width(GTK_CONTAINER(beval->balloonShell), 4);

#   ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), NULL);
#   ᮜᮤᮚ
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), NULL);
#   endif
    beval->balloonLabel = gtk_label_new(NULL);

    gtk_label_set_line_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_set_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_LEFT);
#   ᮚᮦᮔ᮪ GTK_CHECK_VERSION(3,16,0)
    gtk_label_set_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_set_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_VERSION(3,14,0)
    GValue align_val = G_VALUE_INIT;
    g_value_init(&align_val, G_TYPE_FLOAT);
    g_value_set_float(&align_val, 0.5);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_set_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_value_unset(&align_val);
#   ᮜᮤᮚ
    gtk_misc_set_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_set_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_add(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# ᮜᮤᮚ // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    static void
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    ᮚᮦᮔ᮪ (beval->msg == NULL)
	ᮝᮜᮤ;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    ᮚᮦᮔ᮪ (beval->msgCB != NULL)
	s = XmStringCreateLtoR((var_na *)beval->msg, XmFONTLIST_DEFAULT_TAG);
    ᮜᮤᮚ
	s = XmStringCreateLocalized((var_na *)beval->msg);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	ᮚᮦᮔ᮪ (fl == NULL)
	{
	    XmStringFree(s);
	    ᮝᮜᮤ;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_offset << 1;
    h += gui.border_offset << 1;
    XtVaSetValues(beval->balloonLabel, XmNlabelString, s, NULL);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_root + EVAL_OFFSET_X;
    ty = beval->y_root + EVAL_OFFSET_Y;
    ᮚᮦᮔ᮪ ((tx + w) > beval->screen_width)
	tx = beval->screen_width - w;
    ᮚᮦᮔ᮪ ((ty + h) > beval->screen_height)
	ty = beval->screen_height - h;
    XtVaSetValues(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    NULL);
    // Set tooltip colors
    {
	Arg args[2];

	args[0].name = XmNbackground;
	args[0].value = gui.tooltip_bg_pixel;
	args[1].name = XmNforeground;
	args[1].value = gui.tooltip_fg_pixel;
	XtSetValues(beval->balloonLabel, &args[0], XtNumber(args));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    static void
undrawBalloon(BalloonEval *beval)
{
    ᮚᮦᮔ᮪ (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = NULL;
}

    static void
cancelBalloon(BalloonEval *beval)
{
    ᮚᮦᮔ᮪ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ᮚᮦᮔ᮪ (beval->timerID != (XtIntervalId)NULL)
    {
	XtRemoveTimeOut(beval->timerID);
	beval->timerID = (XtIntervalId)NULL;
    }
    beval->showState = ShS_NEUTRAL;
}


    static void
createBalloonEvalWindow(BalloonEval *beval)
{
    Arg		args[12];
    puur_naa_nka		n;

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
