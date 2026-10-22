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
#  elif GTK_CHECK_نسخہ(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  ورنہ
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# ورنہ
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
خارجی Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
ساکن BalloonEval *current_beval = تہی;
# endif

# ifdef FEAT_GUI_GTK
ساکن خالی جوڑو_Event_Handler(GtkWidget *, BalloonEval *);
ساکن خالی removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
ساکن gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  ورنہ
ساکن gint ہدف_event_cb(GtkWidget *, GdkEvent *, gpointer);
ساکن gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
ساکن خالی اشارہ_گر_event(BalloonEval *, صحیح, صحیح, غیرعلامتی);
ساکن خالی کلید_event(BalloonEval *, غیرعلامتی, صحیح);
ساکن gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   اگر GTK_CHECK_نسخہ(3,0,0)
ساکن gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   ورنہ
ساکن gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# ورنہ
ساکن خالی جوڑو_Event_Handler(Widget, BalloonEval *);
ساکن خالی removeEventHandler(BalloonEval *);
ساکن خالی اشارہ_گر_Event_EH(Widget, Xt_اشارہ_گر, XEvent *, Boolean *);
ساکن خالی اشارہ_گر_Event(BalloonEval *, XEvent *);
ساکن خالی timerRoutine(Xt_اشارہ_گر, XtIntervalId *);
# endif
ساکن خالی cancelBalloon(BalloonEval *);
ساکن خالی requestBalloon(BalloonEval *);
ساکن خالی drawBalloon(BalloonEval *);
ساکن خالی undrawBalloon(BalloonEval *beval);
ساکن خالی create_Balloon_Eval_کھڑکی(BalloonEval *);

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
    خالی	*ہدف,
    char_u	*mesg,
    خالی	(*mesgCB)(BalloonEval *, صحیح),
    خالی	*client_ڈیٹا)
{
# ifndef FEAT_GUI_GTK
    حرف	*display_name;	    // get from gui.dpy
    صحیح		screen_عدد;
    حرف	*p;
# endif
    BalloonEval	*beval;

    اگر (mesg != تہی && mesgCB != تہی)
    {
	iemsg(e_cannot_create_ballooneval_with_both_پیغام_and_callback);
	واپس تہی;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    اگر (beval != تہی)
    {
# ifdef FEAT_GUI_GTK
	beval->ہدف = GTK_WIDGET(ہدف);
# ورنہ
	beval->ہدف = (Widget)ہدف;
	beval->appContext = Xt_Widget_مقصد_Application_Context((Widget)ہدف);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->پیغام);
	beval->پیغام = mesg == تہی ? تہی : vim_strsave(mesg);
	beval->پیغام_CB = mesgCB;
	beval->client_ڈیٹا = client_ڈیٹا;

	/*
	 * Set up event handler which will keep its eyes on the pointer,
	 * and when the pointer rests in a certain spot for a given time
	 * interval, show the beval.
	 */
	جوڑو_Event_Handler(beval->ہدف, beval);
	create_Balloon_Eval_کھڑکی(beval);

# ifndef FEAT_GUI_GTK
	/*
	 * Now create and save the screen width and height. Used in drawing.
	 */
	display_name = DisplayString(gui.dpy);
	p = strrchr(display_name, '.');
	اگر (p != تہی)
	    screen_عدد = atoi(++p);
	ورنہ
	    screen_عدد = 0;
	beval->screen_چوڑائی = Display_چوڑائی(gui.dpy, screen_عدد);
	beval->screen_اونچائی = Display_اونچائی(gui.dpy, screen_عدد);
# endif
    }

    واپس beval;
}

# اگر defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    خالی
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   ورنہ
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  ورنہ
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    اگر (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->پیغام);
    vim_free(beval);
}
# endif

    خالی
gui_mch_enable_beval_area(BalloonEval *beval)
{
    اگر (beval != تہی)
	جوڑو_Event_Handler(beval->ہدف, beval);
}

    خالی
gui_mch_disable_beval_area(BalloonEval *beval)
{
    اگر (beval != تہی)
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
gui_mch_currently_showing_beval(خالی)
{
    واپس current_beval;
}
# endif
#اگر_ختم // !FEAT_GUI_MSWIN

#اگر defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# اگر !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    خالی
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->پیغام);
    beval->پیغام = mesg == تہی ? تہی : vim_strsave(mesg);
    اگر (beval->پیغام != تہی)
	drawBalloon(beval);
    ورنہ
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#اگر_ختم // FEAT_NETBEANS_INTG || FEAT_EVAL

#اگر !defined(FEAT_GUI_MSWIN)
# اگر defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    خالی
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->پیغام);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    ساکن خالی
جوڑو_Event_Handler(GtkWidget *ہدف, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_سیٹ_ڈیٹا(G_OBJECT(ہدف), "beval", controller);
    gtk_widget_جوڑو_controller(ہدف, controller);
#  ورنہ
    /*
     * Connect to the generic "event" signal instead of the individual
     * signals for each event type, because the former is emitted earlier.
     * This allows us to catch events independently of the signal handlers
     * in gui_gtk_x11.c.
     */
    g_signal_connect(G_OBJECT(ہدف), "event",
		     G_CALLBACK(ہدف_event_cb),
		     beval);
    /*
     * Nasty:  Key press events go to the main window thus the drawing area
     * will never see them.  This means we have to connect to the main window
     * as well in order to catch those events.
     */
    اگر (gtk_socket_id == 0 && gui.mainwin != تہی
	    && gtk_widget_is_ancestor(ہدف, gui.mainwin))
    {
	gtk_widget_جوڑو_events(gui.mainwin,
			      GDK_LEAVE_NOTIFY_نقاب);

	g_signal_connect(G_OBJECT(gui.mainwin), "event",
			 G_CALLBACK(mainwin_event_cb),
			 beval);
    }
#  endif
}

    ساکن خالی
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*ہدف = beval->ہدف;
    GtkEventController	*controller;

    controller = g_object_get_ڈیٹا(G_OBJECT(ہدف), "beval");
    اگر (controller != تہی)
    {
	gtk_widget_remove_controller(ہدف, controller);
	g_object_سیٹ_ڈیٹا(G_OBJECT(ہدف), "beval", تہی);
    }
#  ورنہ
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->ہدف),
					 FUNC2GENERIC(ہدف_event_cb),
					 beval);

    اگر (gtk_socket_id == 0 && gui.mainwin != تہی
	    && gtk_widget_is_ancestor(beval->ہدف, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    ساکن gboolean
event_cb(GtkEventController *controller غیر_مستعمل, GdkEvent *event, BalloonEval *beval)
{
    صحیح type = gdk_event_get_event_type(event);

    انتخاب (type)
    {
	حالت GDK_ENTER_NOTIFY:
	حالت GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*جڑ;
	    graphene_نقطہ_t	p;
	    graphene_نقطہ_t	نتیجہ;
	    دوہرا		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    جڑ = GTK_WIDGET(gtk_widget_get_جڑ(beval->ہدف));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    اگر (gtk_widget_compute_نقطہ(جڑ, beval->ہدف, &p, &نتیجہ))
	    {
		x = نتیجہ.x;
		y = نتیجہ.y;
	    }

	    اشارہ_گر_event(beval, (صحیح)x, (صحیح)y,
		    gdk_event_get_modifier_state(event));
	    توڑو;
	}
	حالت GDK_LEAVE_NOTIFY:
	    اگر (gdk_پار_event_get_منوال(event) == GDK_پار_معمول)
		cancelBalloon(beval);
	    توڑو;
	حالت GDK_BUTTON_PRESS:
	حالت GDK_SCROLL:
	    cancelBalloon(beval);
	    توڑو;
	حالت GDK_کلید_PRESS:
	حالت GDK_کلید_RELEASE:
	    کلید_event(beval, gdk_کلید_event_get_keyval(event),
		    type == GDK_کلید_PRESS);
	    توڑو;
	طےشدہ:
	    توڑو;
    }

    // Always pass through events?
    واپس FALSE;
}
#  ورنہ
    ساکن gint
ہدف_event_cb(GtkWidget *widget, GdkEvent *event, gpointer ڈیٹا)
{
    BalloonEval *beval = (BalloonEval *)ڈیٹا;

    انتخاب (event->type)
    {
	حالت GDK_ENTER_NOTIFY:
	    اشارہ_گر_event(beval, (صحیح)event->پار.x,
				 (صحیح)event->پار.y,
				 event->پار.state);
	    توڑو;
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
#   اگر GTK_CHECK_نسخہ(3,0,0)
		{
		    Gdk_کھڑکی * ثابت win = gtk_widget_get_کھڑکی(widget);
		    GdkDisplay * ثابت dpy = gdk_کھڑکی_get_display(win);
#    اگر GTK_CHECK_نسخہ(3,20,0)
		    GdkSeat * ثابت seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * ثابت dev = gdk_seat_get_اشارہ_گر(seat);
#    ورنہ
		    GdkDeviceManager * ثابت mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * ثابت dev = gdk_device_manager_get_client_اشارہ_گر(mngr);
#    endif
		    gdk_کھڑکی_get_device_position(win, dev , &x, &y, &state);
		}
#   ورنہ
		gdk_کھڑکی_get_اشارہ_گر(widget->کھڑکی_2, &x, &y, &state);
#   endif
		اشارہ_گر_event(beval, x, y, (غیرعلامتی صحیح)state);
	    }
	    ورنہ
	    {
		اشارہ_گر_event(beval, (صحیح)event->motion.x,
				     (صحیح)event->motion.y,
				     event->motion.state);
	    }
	    توڑو;
	حالت GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    اگر (event->پار.منوال == GDK_پار_معمول)
		cancelBalloon(beval);
	    توڑو;
	حالت GDK_BUTTON_PRESS:
	حالت GDK_SCROLL:
	    cancelBalloon(beval);
	    توڑو;
	حالت GDK_کلید_PRESS:
	    کلید_event(beval, event->کلید.keyval, TRUE);
	    توڑو;
	حالت GDK_کلید_RELEASE:
	    کلید_event(beval, event->کلید.keyval, FALSE);
	    توڑو;
	طےشدہ:
	    توڑو;
    }

    واپس FALSE; // continue emission
}

    ساکن gint
mainwin_event_cb(GtkWidget *widget غیر_مستعمل, GdkEvent *event, gpointer ڈیٹا)
{
    BalloonEval *beval = (BalloonEval *)ڈیٹا;

    انتخاب (event->type)
    {
	حالت GDK_کلید_PRESS:
	    کلید_event(beval, event->کلید.keyval, TRUE);
	    توڑو;
	حالت GDK_کلید_RELEASE:
	    کلید_event(beval, event->کلید.keyval, FALSE);
	    توڑو;
	حالت GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    اگر (event->پار.منوال == GDK_پار_معمول)
		cancelBalloon(beval);
	    توڑو;
	طےشدہ:
	    توڑو;
    }

    واپس FALSE; // continue emission
}
#  endif

    ساکن خالی
اشارہ_گر_event(BalloonEval *beval, صحیح x, صحیح y, غیرعلامتی state)
{
    صحیح فاصلہ;

    فاصلہ = ABS(x - beval->x) + ABS(y - beval->y);

    اگر (فاصلہ <= 4)
	واپس;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    اگر (!(state & ((صحیح)GDK_BUTTON1_نقاب | (صحیح)GDK_BUTTON2_نقاب
		    | (صحیح)GDK_BUTTON3_نقاب)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	اگر (state & (صحیح)GDK_ALT_نقاب)
#  ورنہ
	اگر (state & (صحیح)GDK_MOD1_نقاب)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    اگر (beval->پیغام_CB != تہی)
	    {
		beval->showState = ShS_PENDING;
		(*beval->پیغام_CB)(beval, state);
	    }
	}
	ورنہ
	{
	    beval->timerID = g_timeout_جوڑو((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    ساکن خالی
کلید_event(BalloonEval *beval, غیرعلامتی keyval, صحیح is_keypress)
{
    اگر (beval->showState == ShS_SHOWING && beval->پیغام_CB != تہی)
    {
	انتخاب (keyval)
	{
#  ifdef USE_GTK4
	    حالت GDK_کلید_سرکاؤ_L:
	    حالت GDK_کلید_سرکاؤ_R:
#  ورنہ
	    حالت GDK_سرکاؤ_L:
	    حالت GDK_سرکاؤ_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->پیغام_CB)(beval, (is_keypress)
						   ? (صحیح)GDK_سرکاؤ_نقاب : 0);
		توڑو;
#  ifdef USE_GTK4
	    حالت GDK_کلید_Control_L:
	    حالت GDK_کلید_Control_R:
#  ورنہ
	    حالت GDK_Control_L:
	    حالت GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->پیغام_CB)(beval, (is_keypress)
						 ? (صحیح)GDK_CONTROL_نقاب : 0);
		توڑو;
	    طےشدہ:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		اگر (is_keypress)
		    cancelBalloon(beval);
		توڑو;
	}
    }
    ورنہ
	cancelBalloon(beval);
}

    ساکن gboolean
timeout_cb(gpointer ڈیٹا)
{
    BalloonEval *beval = (BalloonEval *)ڈیٹا;

    beval->timerID = 0;
    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);

    واپس FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 ڈیٹا غیر_مستعمل)
{
    GtkStyleContext *context = تہی;
    gint چوڑائی = -1, اونچائی = -1;

    اگر (widget == تہی)
	واپس TRUE;

    context = gtk_widget_get_style_context(widget);
    چوڑائی = gtk_widget_get_allocated_چوڑائی(widget);
    اونچائی = gtk_widget_get_allocated_اونچائی(widget);

    gtk_style_context_save(context);

    gtk_style_context_جوڑو_class(context, "tooltip");
    gtk_style_context_سیٹ_state(context, GTK_STATE_FLAG_معمول);

    cairo_save(cr);
    gtk_render_چوکھٹا(context, cr, 0, 0, چوڑائی, اونچائی);
    gtk_render_background(context, cr, 0, 0, چوڑائی, اونچائی);
    cairo_restore(cr);

    gtk_style_context_restore(context);

    واپس FALSE;
}
#   ورنہ
    ساکن gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer ڈیٹا غیر_مستعمل)
{
    gtk_paint_flat_box(widget->style, widget->کھڑکی_2,
		       GTK_STATE_معمول, GTK_SHADOW_برآمد,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    واپس FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# ورنہ // !FEAT_GUI_GTK

    ساکن خالی
جوڑو_Event_Handler(Widget ہدف, BalloonEval *beval)
{
    Xt_جوڑو_Event_Handler(ہدف,
			اشارہ_گر_Motion_نقاب | Enter_کھڑکی_نقاب |
			Leave_کھڑکی_نقاب | Button_Press_نقاب | کلید_Press_نقاب |
			کلید_Release_نقاب,
			False,
			اشارہ_گر_Event_EH, (Xt_اشارہ_گر)beval);
}

    ساکن خالی
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->ہدف,
			اشارہ_گر_Motion_نقاب | Enter_کھڑکی_نقاب |
			Leave_کھڑکی_نقاب | Button_Press_نقاب | کلید_Press_نقاب |
			کلید_Release_نقاب,
			False,
			اشارہ_گر_Event_EH, (Xt_اشارہ_گر)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    ساکن خالی
اشارہ_گر_Event_EH(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2,
    XEvent	*event,
    Boolean	*غیر_مستعمل_2 غیر_مستعمل)
{
    BalloonEval *beval = (BalloonEval *)client_ڈیٹا_2;
    اشارہ_گر_Event(beval, event);
}


/*
 * The real event handler. Called by pointerEventEH() whenever an event we are
 * interested in occurs.
 */

    ساکن خالی
اشارہ_گر_Event(BalloonEval *beval, XEvent *event)
{
    Position	فاصلہ;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    انتخاب (event->type)
    {
	حالت EnterNotify:
	حالت MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    اگر (delta < 0)
		delta = -delta;
	    فاصلہ = delta;
	    delta = event->xmotion.y - beval->y;
	    اگر (delta < 0)
		delta = -delta;
	    فاصلہ += delta;
	    اگر (فاصلہ > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		اگر (beval->state & (Button1_نقاب|Button2_نقاب|Button3_نقاب))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		ورنہ اگر (beval->state & (Mod1_نقاب|Mod2_نقاب|Mod3_نقاب))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_جڑ = event->xmotion.x_جڑ;
		    beval->y_جڑ = event->xmotion.y_جڑ;
		    cancelBalloon(beval);
		    اگر (beval->پیغام_CB != تہی)
		    {
			beval->showState = ShS_PENDING;
			(*beval->پیغام_CB)(beval, beval->state);
		    }
		}
		ورنہ
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_جڑ = event->xmotion.x_جڑ;
		    beval->y_جڑ = event->xmotion.y_جڑ;
		    cancelBalloon(beval);
		    beval->timerID = Xt_App_جوڑو_وقت_برآمد( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    توڑو;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	حالت کلید_Press:
	    اگر (beval->showState == ShS_SHOWING && beval->پیغام_CB != تہی)
	    {
		Modifiers   modifier;
		کلید_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		اگر (keysym == XK_سرکاؤ_L || keysym == XK_سرکاؤ_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->پیغام_CB)(beval, سرکاؤ_نقاب);
		}
		ورنہ اگر (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->پیغام_CB)(beval, Control_نقاب);
		}
		ورنہ
		    cancelBalloon(beval);
	    }
	    ورنہ
		cancelBalloon(beval);
	    توڑو;

	حالت کلید_Release:
	    اگر (beval->showState == ShS_SHOWING && beval->پیغام_CB != تہی)
	    {
		Modifiers modifier;
		کلید_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		اگر ((keysym == XK_سرکاؤ_L) || (keysym == XK_سرکاؤ_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->پیغام_CB)(beval, 0);
		}
		ورنہ اگر ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->پیغام_CB)(beval, 0);
		}
		ورنہ
		    cancelBalloon(beval);
	    }
	    ورنہ
		cancelBalloon(beval);
	    توڑو;

	حالت LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		اگر (event->xcrossing.منوال == Notify_معمول)
		    cancelBalloon(beval);
		توڑو;

	حالت ButtonPress:
		cancelBalloon(beval);
		توڑو;

	طےشدہ:
	    توڑو;
    }
}

    ساکن خالی
timerRoutine(Xt_اشارہ_گر dx, XtIntervalId *id غیر_مستعمل)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)تہی;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    ساکن خالی
requestBalloon(BalloonEval *beval)
{
    اگر (beval->showState == ShS_PENDING)
	واپس;

    // Determine the beval to display
    اگر (beval->پیغام_CB != تہی)
    {
	beval->showState = ShS_PENDING;
	(*beval->پیغام_CB)(beval, beval->state);
    }
    ورنہ اگر (beval->پیغام != تہی)
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
    ساکن خالی
سیٹ_printable_label_متن(GtkLabel *label, char_u *متن)
{
    char_u	    *convbuf = تہی;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    غیرعلامتی صحیح    len;
    صحیح		    charlen;
    صحیح		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    اگر (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, متن, تہی);
	اگر (convbuf != تہی)
	    متن = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    برائے (p = متن; *p != NUL; p += charlen)
    {
	اگر ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	ورنہ
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    اگر (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    اگر (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    ورنہ اگر (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    ورنہ
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    اگر (buf != تہی)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  اگر GTK_CHECK_نسخہ(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   اگر PANGO_نسخہ_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  ورنہ
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != تہی) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	اگر (pixel != INVALCOLOR)
#  اگر GTK_CHECK_نسخہ(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  ورنہ
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (غیرعلامتی طویل)pixel, &color);
#  endif

	pdest = buf;
	p = متن;
	جب (*p != NUL)
	{
	    // Be quick for ASCII
	    اگر ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    ورنہ
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
		    ورنہ اگر (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    ورنہ
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    اگر (pixel != INVALCOLOR)
		    {
#  اگر GTK_CHECK_نسخہ(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   اگر PANGO_نسخہ_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  ورنہ
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->آغاز_اشاریہ = pdest - buf;
			attr->انتہا_اشاریہ   = pdest - buf + outlen;
			pango_attr_list_داخل_کرو(attr_list, attr);
#  اگر GTK_CHECK_نسخہ(3,0,0)
#   اگر PANGO_نسخہ_CHECK(1,38,0)
			attr_alpha->آغاز_اشاریہ = pdest - buf;
			attr_alpha->انتہا_اشاریہ   = pdest - buf + outlen;
			pango_attr_list_داخل_کرو(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		ورنہ
		{
		    کرو
			*pdest++ = *p++;
		    جب (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_سیٹ_متن(label, (ثابت حرف *)buf);
    vim_free(buf);

    gtk_label_سیٹ_اوصاف(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    ساکن خالی
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    صحیح		    natural_چوڑائی = 0;

    اگر (beval->پیغام == تہی)
	واپس;

    سیٹ_printable_label_متن(GTK_LABEL(beval->balloonLabel), beval->پیغام);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    تہی, &natural_چوڑائی, تہی, تہی);

    rect.چوڑائی = (natural_چوڑائی > 0 ? natural_چوڑائی : 1) + gui.char_چوڑائی * 3;
    rect.اونچائی = gui.char_اونچائی;

    gtk_popover_سیٹ_pointing_مقصد(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  ورنہ
    ساکن خالی
drawBalloon(BalloonEval *beval)
{
    اگر (beval->پیغام == تہی)
	واپس;

    GtkRequisition	requisition;
    صحیح		screen_w;
    صحیح		screen_h;
    صحیح		screen_x;
    صحیح		screen_y;
    صحیح		x;
    صحیح		y;
    صحیح		x_انحراف = EVAL_انحراف_X;
    صحیح		y_انحراف = EVAL_انحراف_Y;
    PangoLayout	*layout;

#   اگر !GTK_CHECK_نسخہ(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->ہدف);
    gtk_کھڑکی_سیٹ_screen(GTK_کھڑکی(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_کا_win(beval->ہدف, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   اگر !GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    سیٹ_printable_label_متن(GTK_LABEL(beval->balloonLabel), beval->پیغام);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_لفظ_CHAR
    pango_layout_سیٹ_wrap(layout, PANGO_WRAP_لفظ_CHAR);
#   ورنہ
    pango_layout_سیٹ_wrap(layout, PANGO_WRAP_لفظ);
#   endif
    pango_layout_سیٹ_چوڑائی(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.عدد_ستون_تعداد * gui.char_چوڑائی,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   اگر GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_get_preferred_سائز(beval->balloonShell, &requisition, تہی);
#   ورنہ
    gtk_widget_سائز_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_کھڑکی_get_origin(gtk_widget_get_کھڑکی(beval->ہدف), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    اگر (x + x_انحراف + requisition.چوڑائی > screen_x + screen_w)
	y_انحراف += 15;
    اگر (y + y_انحراف + requisition.اونچائی > screen_y + screen_h)
	y_انحراف = -requisition.اونچائی - EVAL_انحراف_Y;

    // Sanitize values
    x = CLAMP(x + x_انحراف, 0,
	    MAX(0, screen_x + screen_w - requisition.چوڑائی));
    y = CLAMP(y + y_انحراف, 0,
	    MAX(0, screen_y + screen_h - requisition.اونچائی));

    // Show the balloon
#   اگر GTK_CHECK_نسخہ(3,0,0)
    gtk_کھڑکی_move(GTK_کھڑکی(beval->balloonShell), x, y);
#   ورنہ
    gtk_widget_سیٹ_uposition(beval->balloonShell, x, y);
#   endif
    gtk_widget_show(beval->balloonShell);

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  endif

/*
 * Undraw a balloon.
 */
    ساکن خالی
undrawBalloon(BalloonEval *beval)
{
    اگر (beval->balloonShell != تہی)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  ورنہ
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    ساکن خالی
cancelBalloon(BalloonEval *beval)
{
    اگر (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    اگر (beval->timerID != 0)
    {
	g_ماخذ_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    ساکن خالی
create_Balloon_Eval_کھڑکی(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_سیٹ_والد(beval->balloonShell, beval->ہدف);
    gtk_popover_سیٹ_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_سیٹ_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_سیٹ_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(تہی);

    gtk_label_سیٹ_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_سیٹ_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_بایاں);
    gtk_label_سیٹ_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_سیٹ_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_سیٹ_بچہ(GTK_POPOVER(popover), beval->balloonLabel);
}
#  ورنہ
    ساکن خالی
create_Balloon_Eval_کھڑکی(BalloonEval *beval)
{
    beval->balloonShell = gtk_کھڑکی_new(GTK_کھڑکی_POPUP);
    gtk_کھڑکی_سیٹ_transient_for(GTK_کھڑکی(beval->balloonShell), GTK_کھڑکی(gui.mainwin));

    gtk_widget_سیٹ_app_paintable(beval->balloonShell, TRUE);
    gtk_کھڑکی_سیٹ_resizable(GTK_کھڑکی(beval->balloonShell), FALSE);
    gtk_widget_سیٹ_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(beval->balloonShell), 4);

#   اگر GTK_CHECK_نسخہ(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), تہی);
#   ورنہ
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), تہی);
#   endif
    beval->balloonLabel = gtk_label_new(تہی);

    gtk_label_سیٹ_سطر_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_سیٹ_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_بایاں);
#   اگر GTK_CHECK_نسخہ(3,16,0)
    gtk_label_سیٹ_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_سیٹ_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_نسخہ(3,14,0)
    GValue align_val = G_قدر_INIT;
    g_قدر_init(&align_val, G_TYPE_FLOAT);
    g_قدر_سیٹ_float(&align_val, 0.5);
    g_object_سیٹ_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_سیٹ_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_قدر_unset(&align_val);
#   ورنہ
    gtk_misc_سیٹ_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_سیٹ_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_جوڑو(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# ورنہ // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    ساکن خالی
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    اگر (beval->پیغام == تہی)
	واپس;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    اگر (beval->پیغام_CB != تہی)
	s = XmStringCreateLtoR((حرف *)beval->پیغام, Xm_FONTLIST_DEFAULT_لیبل);
    ورنہ
	s = XmStringCreateLocalized((حرف *)beval->پیغام);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	اگر (fl == تہی)
	{
	    XmStringFree(s);
	    واپس;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_انحراف << 1;
    h += gui.border_انحراف << 1;
    Xt_Va_سیٹ_اقدار(beval->balloonLabel, XmNlabelString, s, تہی);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_جڑ + EVAL_انحراف_X;
    ty = beval->y_جڑ + EVAL_انحراف_Y;
    اگر ((tx + w) > beval->screen_چوڑائی)
	tx = beval->screen_چوڑائی - w;
    اگر ((ty + h) > beval->screen_اونچائی)
	ty = beval->screen_اونچائی - h;
    Xt_Va_سیٹ_اقدار(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    تہی);
    // Set tooltip colors
    {
	Arg دلائل[2];

	دلائل[0].name = XmNbackground;
	دلائل[0].قدر = gui.tooltip_bg_pixel;
	دلائل[1].name = XmNforeground;
	دلائل[1].قدر = gui.tooltip_fg_pixel;
	Xt_سیٹ_اقدار(beval->balloonLabel, &دلائل[0], Xt_نمبر(دلائل));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    ساکن خالی
undrawBalloon(BalloonEval *beval)
{
    اگر (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = تہی;
}

    ساکن خالی
cancelBalloon(BalloonEval *beval)
{
    اگر (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    اگر (beval->timerID != (XtIntervalId)تہی)
    {
	Xt_Remove_وقت_برآمد(beval->timerID);
	beval->timerID = (XtIntervalId)تہی;
    }
    beval->showState = ShS_NEUTRAL;
}


    ساکن خالی
create_Balloon_Eval_کھڑکی(BalloonEval *beval)
{
    Arg		دلائل[12];
    صحیح		n;

    n = 0;
    Xt_سیٹ_Arg(دلائل[n], XmNallowShellResize, True); n++;
    beval->balloonShell = XtAppCreateShell("balloonEval", "BalloonEval",
		    overrideShellWidgetClass, gui.dpy, دلائل, n);

    XmFontList fl;

    n = 0;
    fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
    Xt_سیٹ_Arg(دلائل[n], XmNforeground, gui.tooltip_fg_pixel); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNbackground, gui.tooltip_bg_pixel); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNfontList, fl); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    beval->balloonLabel = XtCreateManagedWidget("balloonLabel",
	    xmLabelWidgetClass, beval->balloonShell, دلائل, n);
}

# endif // !FEAT_GUI_GTK
#اگر_ختم // !FEAT_GUI_MSWIN

#اگر_ختم // FEAT_BEVAL_GUI
