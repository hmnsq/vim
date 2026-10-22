/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#ఒకవేళ !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_సంస్కరణ(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  లేకపోతే
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# లేకపోతే
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
బాహ్య Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
స్థిర BalloonEval *current_beval = ఖాళీ;
# endif

# ifdef FEAT_GUI_GTK
స్థిర శూన్య కూడు_Event_Handler(GtkWidget *, BalloonEval *);
స్థిర శూన్య removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
స్థిర gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  లేకపోతే
స్థిర gint లక్ష్యం_event_cb(GtkWidget *, GdkEvent *, gpointer);
స్థిర gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
స్థిర శూన్య సూచిక_event(BalloonEval *, పూర్ణాంకం, పూర్ణాంకం, గుర్తులేని);
స్థిర శూన్య కీలకం_event(BalloonEval *, గుర్తులేని, పూర్ణాంకం);
స్థిర gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
స్థిర gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   లేకపోతే
స్థిర gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# లేకపోతే
స్థిర శూన్య కూడు_Event_Handler(Widget, BalloonEval *);
స్థిర శూన్య removeEventHandler(BalloonEval *);
స్థిర శూన్య సూచిక_Event_EH(Widget, Xt_సూచిక, XEvent *, Boolean *);
స్థిర శూన్య సూచిక_Event(BalloonEval *, XEvent *);
స్థిర శూన్య timerRoutine(Xt_సూచిక, XtIntervalId *);
# endif
స్థిర శూన్య cancelBalloon(BalloonEval *);
స్థిర శూన్య requestBalloon(BalloonEval *);
స్థిర శూన్య drawBalloon(BalloonEval *);
స్థిర శూన్య undrawBalloon(BalloonEval *beval);
స్థిర శూన్య create_Balloon_Eval_కిటికీ(BalloonEval *);

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
    శూన్య	*లక్ష్యం,
    char_u	*mesg,
    శూన్య	(*mesgCB)(BalloonEval *, పూర్ణాంకం),
    శూన్య	*client_దత్తాంశం)
{
# ifndef FEAT_GUI_GTK
    అక్షరం	*display_name;	    // get from gui.dpy
    పూర్ణాంకం		screen_అంకె;
    అక్షరం	*p;
# endif
    BalloonEval	*beval;

    ఒకవేళ (mesg != ఖాళీ && mesgCB != ఖాళీ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_సందేశం_and_callback);
	తిరిగి ఖాళీ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ఒకవేళ (beval != ఖాళీ)
    {
# ifdef FEAT_GUI_GTK
	beval->లక్ష్యం = GTK_WIDGET(లక్ష్యం);
# లేకపోతే
	beval->లక్ష్యం = (Widget)లక్ష్యం;
	beval->appContext = Xt_Widget_గమ్యం_Application_Context((Widget)లక్ష్యం);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->సందేశం);
	beval->సందేశం = mesg == ఖాళీ ? ఖాళీ : vim_strsave(mesg);
	beval->సందేశం_CB = mesgCB;
	beval->client_దత్తాంశం = client_దత్తాంశం;

	/*
	 * Set up event handler which will keep its eyes on the pointer,
	 * and when the pointer rests in a certain spot for a given time
	 * interval, show the beval.
	 */
	కూడు_Event_Handler(beval->లక్ష్యం, beval);
	create_Balloon_Eval_కిటికీ(beval);

# ifndef FEAT_GUI_GTK
	/*
	 * Now create and save the screen width and height. Used in drawing.
	 */
	display_name = DisplayString(gui.dpy);
	p = strrchr(display_name, '.');
	ఒకవేళ (p != ఖాళీ)
	    screen_అంకె = atoi(++p);
	లేకపోతే
	    screen_అంకె = 0;
	beval->screen_వెడల్పు = Display_వెడల్పు(gui.dpy, screen_అంకె);
	beval->screen_ఎత్తు = Display_ఎత్తు(gui.dpy, screen_అంకె);
# endif
    }

    తిరిగి beval;
}

# ఒకవేళ defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    శూన్య
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   లేకపోతే
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  లేకపోతే
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    ఒకవేళ (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->సందేశం);
    vim_free(beval);
}
# endif

    శూన్య
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ఒకవేళ (beval != ఖాళీ)
	కూడు_Event_Handler(beval->లక్ష్యం, beval);
}

    శూన్య
gui_mch_disable_beval_area(BalloonEval *beval)
{
    ఒకవేళ (beval != ఖాళీ)
	removeEventHandler(beval);
}

# ఒకవేళ defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(శూన్య)
{
    తిరిగి current_beval;
}
# endif
#అయితే_ముగింపు // !FEAT_GUI_MSWIN

#ఒకవేళ defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# ఒకవేళ !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    శూన్య
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->సందేశం);
    beval->సందేశం = mesg == ఖాళీ ? ఖాళీ : vim_strsave(mesg);
    ఒకవేళ (beval->సందేశం != ఖాళీ)
	drawBalloon(beval);
    లేకపోతే
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#అయితే_ముగింపు // FEAT_NETBEANS_INTG || FEAT_EVAL

#ఒకవేళ !defined(FEAT_GUI_MSWIN)
# ఒకవేళ defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    శూన్య
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->సందేశం);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    స్థిర శూన్య
కూడు_Event_Handler(GtkWidget *లక్ష్యం, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_సమితి_దత్తాంశం(G_OBJECT(లక్ష్యం), "beval", controller);
    gtk_widget_కూడు_controller(లక్ష్యం, controller);
#  లేకపోతే
    /*
     * Connect to the generic "event" signal instead of the individual
     * signals for each event type, because the former is emitted earlier.
     * This allows us to catch events independently of the signal handlers
     * in gui_gtk_x11.c.
     */
    g_signal_connect(G_OBJECT(లక్ష్యం), "event",
		     G_CALLBACK(లక్ష్యం_event_cb),
		     beval);
    /*
     * Nasty:  Key press events go to the main window thus the drawing area
     * will never see them.  This means we have to connect to the main window
     * as well in order to catch those events.
     */
    ఒకవేళ (gtk_socket_id == 0 && gui.mainwin != ఖాళీ
	    && gtk_widget_is_ancestor(లక్ష్యం, gui.mainwin))
    {
	gtk_widget_కూడు_events(gui.mainwin,
			      GDK_LEAVE_NOTIFY_ముసుగు);

	g_signal_connect(G_OBJECT(gui.mainwin), "event",
			 G_CALLBACK(mainwin_event_cb),
			 beval);
    }
#  endif
}

    స్థిర శూన్య
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*లక్ష్యం = beval->లక్ష్యం;
    GtkEventController	*controller;

    controller = g_object_get_దత్తాంశం(G_OBJECT(లక్ష్యం), "beval");
    ఒకవేళ (controller != ఖాళీ)
    {
	gtk_widget_remove_controller(లక్ష్యం, controller);
	g_object_సమితి_దత్తాంశం(G_OBJECT(లక్ష్యం), "beval", ఖాళీ);
    }
#  లేకపోతే
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->లక్ష్యం),
					 FUNC2GENERIC(లక్ష్యం_event_cb),
					 beval);

    ఒకవేళ (gtk_socket_id == 0 && gui.mainwin != ఖాళీ
	    && gtk_widget_is_ancestor(beval->లక్ష్యం, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    స్థిర gboolean
event_cb(GtkEventController *controller వాడని, GdkEvent *event, BalloonEval *beval)
{
    పూర్ణాంకం type = gdk_event_get_event_type(event);

    ఎంపిక (type)
    {
	స్థితి GDK_ENTER_NOTIFY:
	స్థితి GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*మూలం;
	    graphene_బిందువు_t	p;
	    graphene_బిందువు_t	ఫలితం;
	    ద్విగుణ		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    మూలం = GTK_WIDGET(gtk_widget_get_మూలం(beval->లక్ష్యం));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    ఒకవేళ (gtk_widget_compute_బిందువు(మూలం, beval->లక్ష్యం, &p, &ఫలితం))
	    {
		x = ఫలితం.x;
		y = ఫలితం.y;
	    }

	    సూచిక_event(beval, (పూర్ణాంకం)x, (పూర్ణాంకం)y,
		    gdk_event_get_modifier_state(event));
	    ఆపు;
	}
	స్థితి GDK_LEAVE_NOTIFY:
	    ఒకవేళ (gdk_దాటడం_event_get_బహుళకం(event) == GDK_దాటడం_సాధారణ)
		cancelBalloon(beval);
	    ఆపు;
	స్థితి GDK_BUTTON_PRESS:
	స్థితి GDK_SCROLL:
	    cancelBalloon(beval);
	    ఆపు;
	స్థితి GDK_కీలకం_PRESS:
	స్థితి GDK_కీలకం_RELEASE:
	    కీలకం_event(beval, gdk_కీలకం_event_get_keyval(event),
		    type == GDK_కీలకం_PRESS);
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }

    // Always pass through events?
    తిరిగి FALSE;
}
#  లేకపోతే
    స్థిర gint
లక్ష్యం_event_cb(GtkWidget *widget, GdkEvent *event, gpointer దత్తాంశం)
{
    BalloonEval *beval = (BalloonEval *)దత్తాంశం;

    ఎంపిక (event->type)
    {
	స్థితి GDK_ENTER_NOTIFY:
	    సూచిక_event(beval, (పూర్ణాంకం)event->దాటడం.x,
				 (పూర్ణాంకం)event->దాటడం.y,
				 event->దాటడం.state);
	    ఆపు;
	స్థితి GDK_MOTION_NOTIFY:
	    ఒకవేళ (event->motion.is_hint)
	    {
		పూర్ణాంకం		x;
		పూర్ణాంకం		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
		{
		    Gdk_కిటికీ * స్థిరరాశి win = gtk_widget_get_కిటికీ(widget);
		    GdkDisplay * స్థిరరాశి dpy = gdk_కిటికీ_get_display(win);
#    ఒకవేళ GTK_CHECK_సంస్కరణ(3,20,0)
		    GdkSeat * స్థిరరాశి seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * స్థిరరాశి dev = gdk_seat_get_సూచిక(seat);
#    లేకపోతే
		    GdkDeviceManager * స్థిరరాశి mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * స్థిరరాశి dev = gdk_device_manager_get_client_సూచిక(mngr);
#    endif
		    gdk_కిటికీ_get_device_position(win, dev , &x, &y, &state);
		}
#   లేకపోతే
		gdk_కిటికీ_get_సూచిక(widget->కిటికీ_2, &x, &y, &state);
#   endif
		సూచిక_event(beval, x, y, (గుర్తులేని పూర్ణాంకం)state);
	    }
	    లేకపోతే
	    {
		సూచిక_event(beval, (పూర్ణాంకం)event->motion.x,
				     (పూర్ణాంకం)event->motion.y,
				     event->motion.state);
	    }
	    ఆపు;
	స్థితి GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    ఒకవేళ (event->దాటడం.బహుళకం == GDK_దాటడం_సాధారణ)
		cancelBalloon(beval);
	    ఆపు;
	స్థితి GDK_BUTTON_PRESS:
	స్థితి GDK_SCROLL:
	    cancelBalloon(beval);
	    ఆపు;
	స్థితి GDK_కీలకం_PRESS:
	    కీలకం_event(beval, event->కీలకం.keyval, TRUE);
	    ఆపు;
	స్థితి GDK_కీలకం_RELEASE:
	    కీలకం_event(beval, event->కీలకం.keyval, FALSE);
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }

    తిరిగి FALSE; // continue emission
}

    స్థిర gint
mainwin_event_cb(GtkWidget *widget వాడని, GdkEvent *event, gpointer దత్తాంశం)
{
    BalloonEval *beval = (BalloonEval *)దత్తాంశం;

    ఎంపిక (event->type)
    {
	స్థితి GDK_కీలకం_PRESS:
	    కీలకం_event(beval, event->కీలకం.keyval, TRUE);
	    ఆపు;
	స్థితి GDK_కీలకం_RELEASE:
	    కీలకం_event(beval, event->కీలకం.keyval, FALSE);
	    ఆపు;
	స్థితి GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    ఒకవేళ (event->దాటడం.బహుళకం == GDK_దాటడం_సాధారణ)
		cancelBalloon(beval);
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }

    తిరిగి FALSE; // continue emission
}
#  endif

    స్థిర శూన్య
సూచిక_event(BalloonEval *beval, పూర్ణాంకం x, పూర్ణాంకం y, గుర్తులేని state)
{
    పూర్ణాంకం దూరం;

    దూరం = ABS(x - beval->x) + ABS(y - beval->y);

    ఒకవేళ (దూరం <= 4)
	తిరిగి;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    ఒకవేళ (!(state & ((పూర్ణాంకం)GDK_BUTTON1_ముసుగు | (పూర్ణాంకం)GDK_BUTTON2_ముసుగు
		    | (పూర్ణాంకం)GDK_BUTTON3_ముసుగు)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	ఒకవేళ (state & (పూర్ణాంకం)GDK_ALT_ముసుగు)
#  లేకపోతే
	ఒకవేళ (state & (పూర్ణాంకం)GDK_MOD1_ముసుగు)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    ఒకవేళ (beval->సందేశం_CB != ఖాళీ)
	    {
		beval->showState = ShS_PENDING;
		(*beval->సందేశం_CB)(beval, state);
	    }
	}
	లేకపోతే
	{
	    beval->timerID = g_timeout_కూడు((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    స్థిర శూన్య
కీలకం_event(BalloonEval *beval, గుర్తులేని keyval, పూర్ణాంకం is_keypress)
{
    ఒకవేళ (beval->showState == ShS_SHOWING && beval->సందేశం_CB != ఖాళీ)
    {
	ఎంపిక (keyval)
	{
#  ifdef USE_GTK4
	    స్థితి GDK_కీలకం_జరుపు_L:
	    స్థితి GDK_కీలకం_జరుపు_R:
#  లేకపోతే
	    స్థితి GDK_జరుపు_L:
	    స్థితి GDK_జరుపు_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->సందేశం_CB)(beval, (is_keypress)
						   ? (పూర్ణాంకం)GDK_జరుపు_ముసుగు : 0);
		ఆపు;
#  ifdef USE_GTK4
	    స్థితి GDK_కీలకం_Control_L:
	    స్థితి GDK_కీలకం_Control_R:
#  లేకపోతే
	    స్థితి GDK_Control_L:
	    స్థితి GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->సందేశం_CB)(beval, (is_keypress)
						 ? (పూర్ణాంకం)GDK_CONTROL_ముసుగు : 0);
		ఆపు;
	    అప్రమేయ:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		ఒకవేళ (is_keypress)
		    cancelBalloon(beval);
		ఆపు;
	}
    }
    లేకపోతే
	cancelBalloon(beval);
}

    స్థిర gboolean
timeout_cb(gpointer దత్తాంశం)
{
    BalloonEval *beval = (BalloonEval *)దత్తాంశం;

    beval->timerID = 0;
    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);

    తిరిగి FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 దత్తాంశం వాడని)
{
    GtkStyleContext *context = ఖాళీ;
    gint వెడల్పు = -1, ఎత్తు = -1;

    ఒకవేళ (widget == ఖాళీ)
	తిరిగి TRUE;

    context = gtk_widget_get_style_context(widget);
    వెడల్పు = gtk_widget_get_allocated_వెడల్పు(widget);
    ఎత్తు = gtk_widget_get_allocated_ఎత్తు(widget);

    gtk_style_context_save(context);

    gtk_style_context_కూడు_class(context, "tooltip");
    gtk_style_context_సమితి_state(context, GTK_STATE_FLAG_సాధారణ);

    cairo_save(cr);
    gtk_render_చట్రం(context, cr, 0, 0, వెడల్పు, ఎత్తు);
    gtk_render_background(context, cr, 0, 0, వెడల్పు, ఎత్తు);
    cairo_restore(cr);

    gtk_style_context_restore(context);

    తిరిగి FALSE;
}
#   లేకపోతే
    స్థిర gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer దత్తాంశం వాడని)
{
    gtk_paint_flat_box(widget->style, widget->కిటికీ_2,
		       GTK_STATE_సాధారణ, GTK_SHADOW_బహిర్గతం,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    తిరిగి FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# లేకపోతే // !FEAT_GUI_GTK

    స్థిర శూన్య
కూడు_Event_Handler(Widget లక్ష్యం, BalloonEval *beval)
{
    Xt_కూడు_Event_Handler(లక్ష్యం,
			సూచిక_Motion_ముసుగు | Enter_కిటికీ_ముసుగు |
			Leave_కిటికీ_ముసుగు | Button_Press_ముసుగు | కీలకం_Press_ముసుగు |
			కీలకం_Release_ముసుగు,
			False,
			సూచిక_Event_EH, (Xt_సూచిక)beval);
}

    స్థిర శూన్య
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->లక్ష్యం,
			సూచిక_Motion_ముసుగు | Enter_కిటికీ_ముసుగు |
			Leave_కిటికీ_ముసుగు | Button_Press_ముసుగు | కీలకం_Press_ముసుగు |
			కీలకం_Release_ముసుగు,
			False,
			సూచిక_Event_EH, (Xt_సూచిక)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    స్థిర శూన్య
సూచిక_Event_EH(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2,
    XEvent	*event,
    Boolean	*వాడని_2 వాడని)
{
    BalloonEval *beval = (BalloonEval *)client_దత్తాంశం_2;
    సూచిక_Event(beval, event);
}


/*
 * The real event handler. Called by pointerEventEH() whenever an event we are
 * interested in occurs.
 */

    స్థిర శూన్య
సూచిక_Event(BalloonEval *beval, XEvent *event)
{
    Position	దూరం;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    ఎంపిక (event->type)
    {
	స్థితి EnterNotify:
	స్థితి MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    ఒకవేళ (delta < 0)
		delta = -delta;
	    దూరం = delta;
	    delta = event->xmotion.y - beval->y;
	    ఒకవేళ (delta < 0)
		delta = -delta;
	    దూరం += delta;
	    ఒకవేళ (దూరం > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		ఒకవేళ (beval->state & (Button1_ముసుగు|Button2_ముసుగు|Button3_ముసుగు))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		లేకపోతే ఒకవేళ (beval->state & (Mod1_ముసుగు|Mod2_ముసుగు|Mod3_ముసుగు))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_మూలం = event->xmotion.x_మూలం;
		    beval->y_మూలం = event->xmotion.y_మూలం;
		    cancelBalloon(beval);
		    ఒకవేళ (beval->సందేశం_CB != ఖాళీ)
		    {
			beval->showState = ShS_PENDING;
			(*beval->సందేశం_CB)(beval, beval->state);
		    }
		}
		లేకపోతే
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_మూలం = event->xmotion.x_మూలం;
		    beval->y_మూలం = event->xmotion.y_మూలం;
		    cancelBalloon(beval);
		    beval->timerID = Xt_App_కూడు_సమయం_బహిర్గతం( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    ఆపు;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	స్థితి కీలకం_Press:
	    ఒకవేళ (beval->showState == ShS_SHOWING && beval->సందేశం_CB != ఖాళీ)
	    {
		Modifiers   modifier;
		కీలకం_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		ఒకవేళ (keysym == XK_జరుపు_L || keysym == XK_జరుపు_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->సందేశం_CB)(beval, జరుపు_ముసుగు);
		}
		లేకపోతే ఒకవేళ (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->సందేశం_CB)(beval, Control_ముసుగు);
		}
		లేకపోతే
		    cancelBalloon(beval);
	    }
	    లేకపోతే
		cancelBalloon(beval);
	    ఆపు;

	స్థితి కీలకం_Release:
	    ఒకవేళ (beval->showState == ShS_SHOWING && beval->సందేశం_CB != ఖాళీ)
	    {
		Modifiers modifier;
		కీలకం_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		ఒకవేళ ((keysym == XK_జరుపు_L) || (keysym == XK_జరుపు_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->సందేశం_CB)(beval, 0);
		}
		లేకపోతే ఒకవేళ ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->సందేశం_CB)(beval, 0);
		}
		లేకపోతే
		    cancelBalloon(beval);
	    }
	    లేకపోతే
		cancelBalloon(beval);
	    ఆపు;

	స్థితి LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		ఒకవేళ (event->xcrossing.బహుళకం == Notify_సాధారణ)
		    cancelBalloon(beval);
		ఆపు;

	స్థితి ButtonPress:
		cancelBalloon(beval);
		ఆపు;

	అప్రమేయ:
	    ఆపు;
    }
}

    స్థిర శూన్య
timerRoutine(Xt_సూచిక dx, XtIntervalId *id వాడని)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)ఖాళీ;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    స్థిర శూన్య
requestBalloon(BalloonEval *beval)
{
    ఒకవేళ (beval->showState == ShS_PENDING)
	తిరిగి;

    // Determine the beval to display
    ఒకవేళ (beval->సందేశం_CB != ఖాళీ)
    {
	beval->showState = ShS_PENDING;
	(*beval->సందేశం_CB)(beval, beval->state);
    }
    లేకపోతే ఒకవేళ (beval->సందేశం != ఖాళీ)
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
    స్థిర శూన్య
సమితి_printable_label_పాఠ్యం(GtkLabel *label, char_u *పాఠ్యం)
{
    char_u	    *convbuf = ఖాళీ;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    గుర్తులేని పూర్ణాంకం    len;
    పూర్ణాంకం		    charlen;
    పూర్ణాంకం		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    ఒకవేళ (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, పాఠ్యం, ఖాళీ);
	ఒకవేళ (convbuf != ఖాళీ)
	    పాఠ్యం = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    కొరకు (p = పాఠ్యం; *p != NUL; p += charlen)
    {
	ఒకవేళ ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	లేకపోతే
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    ఒకవేళ (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    ఒకవేళ (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    లేకపోతే ఒకవేళ (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    లేకపోతే
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    ఒకవేళ (buf != ఖాళీ)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   ఒకవేళ PANGO_సంస్కరణ_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  లేకపోతే
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != ఖాళీ) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	ఒకవేళ (pixel != INVALCOLOR)
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  లేకపోతే
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (గుర్తులేని దీర్ఘ)pixel, &color);
#  endif

	pdest = buf;
	p = పాఠ్యం;
	అయితే (*p != NUL)
	{
	    // Be quick for ASCII
	    ఒకవేళ ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    లేకపోతే
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		ఒకవేళ (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		ఒకవేళ (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    పూర్ణాంకం	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    ఒకవేళ (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    లేకపోతే ఒకవేళ (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    లేకపోతే
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    ఒకవేళ (pixel != INVALCOLOR)
		    {
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   ఒకవేళ PANGO_సంస్కరణ_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  లేకపోతే
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->ప్రారంభం_సూచీ = pdest - buf;
			attr->చివర_సూచీ   = pdest - buf + outlen;
			pango_attr_list_చొప్పించు(attr_list, attr);
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
#   ఒకవేళ PANGO_సంస్కరణ_CHECK(1,38,0)
			attr_alpha->ప్రారంభం_సూచీ = pdest - buf;
			attr_alpha->చివర_సూచీ   = pdest - buf + outlen;
			pango_attr_list_చొప్పించు(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		లేకపోతే
		{
		    చేయి
			*pdest++ = *p++;
		    అయితే (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_సమితి_పాఠ్యం(label, (స్థిరరాశి అక్షరం *)buf);
    vim_free(buf);

    gtk_label_సమితి_లక్షణాలు(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    స్థిర శూన్య
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    పూర్ణాంకం		    natural_వెడల్పు = 0;

    ఒకవేళ (beval->సందేశం == ఖాళీ)
	తిరిగి;

    సమితి_printable_label_పాఠ్యం(GTK_LABEL(beval->balloonLabel), beval->సందేశం);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    ఖాళీ, &natural_వెడల్పు, ఖాళీ, ఖాళీ);

    rect.వెడల్పు = (natural_వెడల్పు > 0 ? natural_వెడల్పు : 1) + gui.char_వెడల్పు * 3;
    rect.ఎత్తు = gui.char_ఎత్తు;

    gtk_popover_సమితి_pointing_గమ్యం(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  లేకపోతే
    స్థిర శూన్య
drawBalloon(BalloonEval *beval)
{
    ఒకవేళ (beval->సందేశం == ఖాళీ)
	తిరిగి;

    GtkRequisition	requisition;
    పూర్ణాంకం		screen_w;
    పూర్ణాంకం		screen_h;
    పూర్ణాంకం		screen_x;
    పూర్ణాంకం		screen_y;
    పూర్ణాంకం		x;
    పూర్ణాంకం		y;
    పూర్ణాంకం		x_స్థానభ్రంశం = EVAL_స్థానభ్రంశం_X;
    పూర్ణాంకం		y_స్థానభ్రంశం = EVAL_స్థానభ్రంశం_Y;
    PangoLayout	*layout;

#   ఒకవేళ !GTK_CHECK_సంస్కరణ(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->లక్ష్యం);
    gtk_కిటికీ_సమితి_screen(GTK_కిటికీ(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_యొక్క_win(beval->లక్ష్యం, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    సమితి_printable_label_పాఠ్యం(GTK_LABEL(beval->balloonLabel), beval->సందేశం);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_మాట_CHAR
    pango_layout_సమితి_wrap(layout, PANGO_WRAP_మాట_CHAR);
#   లేకపోతే
    pango_layout_సమితి_wrap(layout, PANGO_WRAP_మాట);
#   endif
    pango_layout_సమితి_వెడల్పు(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.అంకె_నిలువుల_సంఖ్య * gui.char_వెడల్పు,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_widget_get_preferred_కొలత(beval->balloonShell, &requisition, ఖాళీ);
#   లేకపోతే
    gtk_widget_కొలత_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_కిటికీ_get_origin(gtk_widget_get_కిటికీ(beval->లక్ష్యం), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    ఒకవేళ (x + x_స్థానభ్రంశం + requisition.వెడల్పు > screen_x + screen_w)
	y_స్థానభ్రంశం += 15;
    ఒకవేళ (y + y_స్థానభ్రంశం + requisition.ఎత్తు > screen_y + screen_h)
	y_స్థానభ్రంశం = -requisition.ఎత్తు - EVAL_స్థానభ్రంశం_Y;

    // Sanitize values
    x = CLAMP(x + x_స్థానభ్రంశం, 0,
	    MAX(0, screen_x + screen_w - requisition.వెడల్పు));
    y = CLAMP(y + y_స్థానభ్రంశం, 0,
	    MAX(0, screen_y + screen_h - requisition.ఎత్తు));

    // Show the balloon
#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_కిటికీ_move(GTK_కిటికీ(beval->balloonShell), x, y);
#   లేకపోతే
    gtk_widget_సమితి_uposition(beval->balloonShell, x, y);
#   endif
    gtk_widget_show(beval->balloonShell);

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  endif

/*
 * Undraw a balloon.
 */
    స్థిర శూన్య
undrawBalloon(BalloonEval *beval)
{
    ఒకవేళ (beval->balloonShell != ఖాళీ)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  లేకపోతే
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    స్థిర శూన్య
cancelBalloon(BalloonEval *beval)
{
    ఒకవేళ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ఒకవేళ (beval->timerID != 0)
    {
	g_ఆధారం_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    స్థిర శూన్య
create_Balloon_Eval_కిటికీ(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_సమితి_మాతృ(beval->balloonShell, beval->లక్ష్యం);
    gtk_popover_సమితి_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_సమితి_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_సమితి_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(ఖాళీ);

    gtk_label_సమితి_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_సమితి_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ఎడమ);
    gtk_label_సమితి_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_సమితి_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_సమితి_బిడ్డ(GTK_POPOVER(popover), beval->balloonLabel);
}
#  లేకపోతే
    స్థిర శూన్య
create_Balloon_Eval_కిటికీ(BalloonEval *beval)
{
    beval->balloonShell = gtk_కిటికీ_new(GTK_కిటికీ_POPUP);
    gtk_కిటికీ_సమితి_transient_for(GTK_కిటికీ(beval->balloonShell), GTK_కిటికీ(gui.mainwin));

    gtk_widget_సమితి_app_paintable(beval->balloonShell, TRUE);
    gtk_కిటికీ_సమితి_resizable(GTK_కిటికీ(beval->balloonShell), FALSE);
    gtk_widget_సమితి_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(beval->balloonShell), 4);

#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), ఖాళీ);
#   లేకపోతే
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), ఖాళీ);
#   endif
    beval->balloonLabel = gtk_label_new(ఖాళీ);

    gtk_label_సమితి_పంక్తి_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_సమితి_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_ఎడమ);
#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,16,0)
    gtk_label_సమితి_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_సమితి_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_సంస్కరణ(3,14,0)
    GValue align_val = G_విలువ_INIT;
    g_విలువ_init(&align_val, G_TYPE_FLOAT);
    g_విలువ_సమితి_float(&align_val, 0.5);
    g_object_సమితి_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_సమితి_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_విలువ_unset(&align_val);
#   లేకపోతే
    gtk_misc_సమితి_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_సమితి_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_కూడు(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# లేకపోతే // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    స్థిర శూన్య
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    ఒకవేళ (beval->సందేశం == ఖాళీ)
	తిరిగి;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    ఒకవేళ (beval->సందేశం_CB != ఖాళీ)
	s = XmStringCreateLtoR((అక్షరం *)beval->సందేశం, Xm_FONTLIST_DEFAULT_ట్యాగ్);
    లేకపోతే
	s = XmStringCreateLocalized((అక్షరం *)beval->సందేశం);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	ఒకవేళ (fl == ఖాళీ)
	{
	    XmStringFree(s);
	    తిరిగి;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_స్థానభ్రంశం << 1;
    h += gui.border_స్థానభ్రంశం << 1;
    Xt_Va_సమితి_విలువలు(beval->balloonLabel, XmNlabelString, s, ఖాళీ);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_మూలం + EVAL_స్థానభ్రంశం_X;
    ty = beval->y_మూలం + EVAL_స్థానభ్రంశం_Y;
    ఒకవేళ ((tx + w) > beval->screen_వెడల్పు)
	tx = beval->screen_వెడల్పు - w;
    ఒకవేళ ((ty + h) > beval->screen_ఎత్తు)
	ty = beval->screen_ఎత్తు - h;
    Xt_Va_సమితి_విలువలు(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    ఖాళీ);
    // Set tooltip colors
    {
	Arg వాదనలు[2];

	వాదనలు[0].name = XmNbackground;
	వాదనలు[0].విలువ = gui.tooltip_bg_pixel;
	వాదనలు[1].name = XmNforeground;
	వాదనలు[1].విలువ = gui.tooltip_fg_pixel;
	Xt_సమితి_విలువలు(beval->balloonLabel, &వాదనలు[0], Xt_సంఖ్య(వాదనలు));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    స్థిర శూన్య
undrawBalloon(BalloonEval *beval)
{
    ఒకవేళ (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = ఖాళీ;
}

    స్థిర శూన్య
cancelBalloon(BalloonEval *beval)
{
    ఒకవేళ (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    ఒకవేళ (beval->timerID != (XtIntervalId)ఖాళీ)
    {
	Xt_Remove_సమయం_బహిర్గతం(beval->timerID);
	beval->timerID = (XtIntervalId)ఖాళీ;
    }
    beval->showState = ShS_NEUTRAL;
}


    స్థిర శూన్య
create_Balloon_Eval_కిటికీ(BalloonEval *beval)
{
    Arg		వాదనలు[12];
    పూర్ణాంకం		n;

    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], XmNallowShellResize, True); n++;
    beval->balloonShell = XtAppCreateShell("balloonEval", "BalloonEval",
		    overrideShellWidgetClass, gui.dpy, వాదనలు, n);

    XmFontList fl;

    n = 0;
    fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
    Xt_సమితి_Arg(వాదనలు[n], XmNforeground, gui.tooltip_fg_pixel); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNbackground, gui.tooltip_bg_pixel); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNfontList, fl); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    beval->balloonLabel = XtCreateManagedWidget("balloonLabel",
	    xmLabelWidgetClass, beval->balloonShell, వాదనలు, n);
}

# endif // !FEAT_GUI_GTK
#అయితే_ముగింపు // !FEAT_GUI_MSWIN

#అయితే_ముగింపు // FEAT_BEVAL_GUI
