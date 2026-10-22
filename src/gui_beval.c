/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#إذا defined(FEAT_BEVAL_GUI)

// on Win32 only get_beval_info() is required
#إذا !defined(FEAT_GUI_MSWIN)

# ifdef FEAT_GUI_GTK
#  ifdef USE_GTK4
#   include <gdk/gdkkeysyms.h>
#  elif GTK_CHECK_نسخو(3,0,0)
#   include <gdk/gdkkeysyms-compat.h>
#  وإلا
#   include <gdk/gdkkeysyms.h>
#  endif
#  include <gtk/gtk.h>
# وإلا
#  include <X11/keysym.h>
#  include <Xm/PushB.h>
#  include <Xm/Separator.h>
#  include <Xm/List.h>
#  include <Xm/Label.h>
#  include <Xm/AtomMgr.h>
#  include <Xm/Protocols.h>
# endif

# ifndef FEAT_GUI_GTK
خارجي Widget vimShell;

/*
 * Currently, we assume that there can be only one BalloonEval showing
 * on-screen at any given moment.  This variable will hold the currently
 * showing BalloonEval or NULL if none is showing.
 */
ساكن BalloonEval *current_beval = عدم;
# endif

# ifdef FEAT_GUI_GTK
ساكن فراغ جوڙيو_Event_Handler(GtkWidget *, BalloonEval *);
ساكن فراغ removeEventHandler(BalloonEval *);
#  ifdef USE_GTK4
ساكن gboolean event_cb(GtkEventController *, GdkEvent *, BalloonEval *);
#  وإلا
ساكن gint هدف_event_cb(GtkWidget *, GdkEvent *, gpointer);
ساكن gint mainwin_event_cb(GtkWidget *, GdkEvent *, gpointer);
#  endif
ساكن فراغ اشارو_event(BalloonEval *, صحيح, صحيح, بنانشان);
ساكن فراغ ڪنجي_event(BalloonEval *, بنانشان, صحيح);
ساكن gboolean timeout_cb(gpointer);
#  ifndef USE_GTK4
#   إذا GTK_CHECK_نسخو(3,0,0)
ساكن gboolean balloon_draw_event_cb (GtkWidget *, cairo_t *, gpointer);
#   وإلا
ساكن gint balloon_expose_event_cb (GtkWidget *, GdkEventExpose *, gpointer);
#   endif
#  endif
# وإلا
ساكن فراغ جوڙيو_Event_Handler(Widget, BalloonEval *);
ساكن فراغ removeEventHandler(BalloonEval *);
ساكن فراغ اشارو_Event_EH(Widget, Xt_اشارو, XEvent *, Boolean *);
ساكن فراغ اشارو_Event(BalloonEval *, XEvent *);
ساكن فراغ timerRoutine(Xt_اشارو, XtIntervalId *);
# endif
ساكن فراغ cancelBalloon(BalloonEval *);
ساكن فراغ requestBalloon(BalloonEval *);
ساكن فراغ drawBalloon(BalloonEval *);
ساكن فراغ undrawBalloon(BalloonEval *beval);
ساكن فراغ create_Balloon_Eval_دري(BalloonEval *);

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
    فراغ	*هدف,
    char_u	*mesg,
    فراغ	(*mesgCB)(BalloonEval *, صحيح),
    فراغ	*client_معلومات)
{
# ifndef FEAT_GUI_GTK
    حرف	*display_name;	    // get from gui.dpy
    صحيح		screen_انگ;
    حرف	*p;
# endif
    BalloonEval	*beval;

    إذا (mesg != عدم && mesgCB != عدم)
    {
	iemsg(e_cannot_create_ballooneval_with_both_پيغام_and_callback);
	أرجع عدم;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    إذا (beval != عدم)
    {
# ifdef FEAT_GUI_GTK
	beval->هدف = GTK_WIDGET(هدف);
# وإلا
	beval->هدف = (Widget)هدف;
	beval->appContext = Xt_Widget_منزل_Application_Context((Widget)هدف);
# endif
	beval->showState = ShS_NEUTRAL;
	vim_free(beval->نياپو);
	beval->نياپو = mesg == عدم ? عدم : vim_strsave(mesg);
	beval->نياپو_CB = mesgCB;
	beval->client_معلومات = client_معلومات;

	/*
	 * Set up event handler which will keep its eyes on the pointer,
	 * and when the pointer rests in a certain spot for a given time
	 * interval, show the beval.
	 */
	جوڙيو_Event_Handler(beval->هدف, beval);
	create_Balloon_Eval_دري(beval);

# ifndef FEAT_GUI_GTK
	/*
	 * Now create and save the screen width and height. Used in drawing.
	 */
	display_name = DisplayString(gui.dpy);
	p = strrchr(display_name, '.');
	إذا (p != عدم)
	    screen_انگ = atoi(++p);
	وإلا
	    screen_انگ = 0;
	beval->screen_ويڪر = Display_ويڪر(gui.dpy, screen_انگ);
	beval->screen_اوچائي = Display_اوچائي(gui.dpy, screen_انگ);
# endif
    }

    أرجع beval;
}

# إذا defined(FEAT_BEVAL_TIP) || defined(USE_GTK4)
/*
 * Destroy a balloon-eval and free its associated memory.
 */
    فراغ
gui_mch_destroy_beval_area(BalloonEval *beval)
{
    cancelBalloon(beval);
    removeEventHandler(beval);
    // Children will automatically be destroyed
#  ifdef FEAT_GUI_GTK
#   ifdef USE_GTK4
    gtk_widget_unparent(beval->balloonShell);
#   وإلا
    gtk_widget_destroy(beval->balloonShell);
#   endif
#  وإلا
    XtDestroyWidget(beval->balloonShell);
#  endif
#  ifdef FEAT_VARTABS
    إذا (beval->vts)
	vim_free(beval->vts);
#  endif
    vim_free(beval->نياپو);
    vim_free(beval);
}
# endif

    فراغ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    إذا (beval != عدم)
	جوڙيو_Event_Handler(beval->هدف, beval);
}

    فراغ
gui_mch_disable_beval_area(BalloonEval *beval)
{
    إذا (beval != عدم)
	removeEventHandler(beval);
}

# إذا defined(FEAT_BEVAL_TIP)
/*
 * This function returns the BalloonEval * associated with the currently
 * displayed tooltip.  Returns NULL if there is no tooltip currently showing.
 *
 * Assumption: Only one tooltip can be shown at a time.
 */
    BalloonEval *
gui_mch_currently_showing_beval(فراغ)
{
    أرجع current_beval;
}
# endif
#جيڪڏهن_پورو // !FEAT_GUI_MSWIN

#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL)
# إذا !defined(FEAT_GUI_MSWIN)

/*
 * Show a balloon with "mesg".
 */
    فراغ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    vim_free(beval->نياپو);
    beval->نياپو = mesg == عدم ? عدم : vim_strsave(mesg);
    إذا (beval->نياپو != عدم)
	drawBalloon(beval);
    وإلا
	undrawBalloon(beval);
}
# endif // !FEAT_GUI_MSWIN
#جيڪڏهن_پورو // FEAT_NETBEANS_INTG || FEAT_EVAL

#إذا !defined(FEAT_GUI_MSWIN)
# إذا defined(FEAT_BEVAL_TIP)
/*
 * Hide the given balloon.
 */
    فراغ
gui_mch_unpost_balloon(BalloonEval *beval)
{
    VIM_CLEAR(beval->نياپو);
    undrawBalloon(beval);
}
# endif

# ifdef FEAT_GUI_GTK
    ساكن فراغ
جوڙيو_Event_Handler(GtkWidget *هدف, BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkEventController *controller = gtk_event_controller_legacy_new();

    g_signal_connect(controller, "event", G_CALLBACK(event_cb), beval);

    g_object_سيٽ_معلومات(G_OBJECT(هدف), "beval", controller);
    gtk_widget_جوڙيو_controller(هدف, controller);
#  وإلا
    /*
     * Connect to the generic "event" signal instead of the individual
     * signals for each event type, because the former is emitted earlier.
     * This allows us to catch events independently of the signal handlers
     * in gui_gtk_x11.c.
     */
    g_signal_connect(G_OBJECT(هدف), "event",
		     G_CALLBACK(هدف_event_cb),
		     beval);
    /*
     * Nasty:  Key press events go to the main window thus the drawing area
     * will never see them.  This means we have to connect to the main window
     * as well in order to catch those events.
     */
    إذا (gtk_socket_id == 0 && gui.mainwin != عدم
	    && gtk_widget_is_ancestor(هدف, gui.mainwin))
    {
	gtk_widget_جوڙيو_events(gui.mainwin,
			      GDK_LEAVE_NOTIFY_نقاب);

	g_signal_connect(G_OBJECT(gui.mainwin), "event",
			 G_CALLBACK(mainwin_event_cb),
			 beval);
    }
#  endif
}

    ساكن فراغ
removeEventHandler(BalloonEval *beval)
{
#  ifdef USE_GTK4
    GtkWidget		*هدف = beval->هدف;
    GtkEventController	*controller;

    controller = g_object_get_معلومات(G_OBJECT(هدف), "beval");
    إذا (controller != عدم)
    {
	gtk_widget_remove_controller(هدف, controller);
	g_object_سيٽ_معلومات(G_OBJECT(هدف), "beval", عدم);
    }
#  وإلا
    g_signal_handlers_disconnect_by_func(G_OBJECT(beval->هدف),
					 FUNC2GENERIC(هدف_event_cb),
					 beval);

    إذا (gtk_socket_id == 0 && gui.mainwin != عدم
	    && gtk_widget_is_ancestor(beval->هدف, gui.mainwin))
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(gui.mainwin),
					     FUNC2GENERIC(mainwin_event_cb),
					     beval);
    }
#  endif
}

#  ifdef USE_GTK4
    ساكن gboolean
event_cb(GtkEventController *controller اڻ_استعمال_ٿيل, GdkEvent *event, BalloonEval *beval)
{
    صحيح type = gdk_event_get_event_type(event);

    بدل (type)
    {
	حالة GDK_ENTER_NOTIFY:
	حالة GDK_MOTION_NOTIFY:
	{
	    GtkWidget		*پاڙ;
	    graphene_نقطو_t	p;
	    graphene_نقطو_t	نتيجو;
	    مزدوج		x = 0, y = 0;

	    // x and y coords returned by gdk_event_get_position are relative to
	    // the surface, not the widget, must convert them into widget local
	    // coordinates.
	    پاڙ = GTK_WIDGET(gtk_widget_get_پاڙ(beval->هدف));
	    gdk_event_get_position(event, &x, &y);
	    p.x = x;
	    p.y = y;

	    إذا (gtk_widget_compute_نقطو(پاڙ, beval->هدف, &p, &نتيجو))
	    {
		x = نتيجو.x;
		y = نتيجو.y;
	    }

	    اشارو_event(beval, (صحيح)x, (صحيح)y,
		    gdk_event_get_modifier_state(event));
	    اكسر;
	}
	حالة GDK_LEAVE_NOTIFY:
	    إذا (gdk_پار_event_get_گهڻائي(event) == GDK_پار_عام)
		cancelBalloon(beval);
	    اكسر;
	حالة GDK_BUTTON_PRESS:
	حالة GDK_SCROLL:
	    cancelBalloon(beval);
	    اكسر;
	حالة GDK_ڪنجي_PRESS:
	حالة GDK_ڪنجي_RELEASE:
	    ڪنجي_event(beval, gdk_ڪنجي_event_get_keyval(event),
		    type == GDK_ڪنجي_PRESS);
	    اكسر;
	افتراضي:
	    اكسر;
    }

    // Always pass through events?
    أرجع FALSE;
}
#  وإلا
    ساكن gint
هدف_event_cb(GtkWidget *widget, GdkEvent *event, gpointer معلومات)
{
    BalloonEval *beval = (BalloonEval *)معلومات;

    بدل (event->type)
    {
	حالة GDK_ENTER_NOTIFY:
	    اشارو_event(beval, (صحيح)event->پار.x,
				 (صحيح)event->پار.y,
				 event->پار.state);
	    اكسر;
	حالة GDK_MOTION_NOTIFY:
	    إذا (event->motion.is_hint)
	    {
		صحيح		x;
		صحيح		y;
		GdkModifierType	state;
		/*
		 * GDK_POINTER_MOTION_HINT_MASK is set, thus we cannot obtain
		 * the coordinates from the GdkEventMotion struct directly.
		 */
#   إذا GTK_CHECK_نسخو(3,0,0)
		{
		    Gdk_دري * ثابت win = gtk_widget_get_دري(widget);
		    GdkDisplay * ثابت dpy = gdk_دري_get_display(win);
#    إذا GTK_CHECK_نسخو(3,20,0)
		    GdkSeat * ثابت seat = gdk_display_get_default_seat(dpy);
		    GdkDevice * ثابت dev = gdk_seat_get_اشارو(seat);
#    وإلا
		    GdkDeviceManager * ثابت mngr = gdk_display_get_device_manager(dpy);
		    GdkDevice * ثابت dev = gdk_device_manager_get_client_اشارو(mngr);
#    endif
		    gdk_دري_get_device_position(win, dev , &x, &y, &state);
		}
#   وإلا
		gdk_دري_get_اشارو(widget->دري_2, &x, &y, &state);
#   endif
		اشارو_event(beval, x, y, (بنانشان صحيح)state);
	    }
	    وإلا
	    {
		اشارو_event(beval, (صحيح)event->motion.x,
				     (صحيح)event->motion.y,
				     event->motion.state);
	    }
	    اكسر;
	حالة GDK_LEAVE_NOTIFY:
	    /*
	     * Ignore LeaveNotify events that are not "normal".
	     * Apparently we also get it when somebody else grabs focus.
	     */
	    إذا (event->پار.گهڻائي == GDK_پار_عام)
		cancelBalloon(beval);
	    اكسر;
	حالة GDK_BUTTON_PRESS:
	حالة GDK_SCROLL:
	    cancelBalloon(beval);
	    اكسر;
	حالة GDK_ڪنجي_PRESS:
	    ڪنجي_event(beval, event->ڪنجي.keyval, TRUE);
	    اكسر;
	حالة GDK_ڪنجي_RELEASE:
	    ڪنجي_event(beval, event->ڪنجي.keyval, FALSE);
	    اكسر;
	افتراضي:
	    اكسر;
    }

    أرجع FALSE; // continue emission
}

    ساكن gint
mainwin_event_cb(GtkWidget *widget اڻ_استعمال_ٿيل, GdkEvent *event, gpointer معلومات)
{
    BalloonEval *beval = (BalloonEval *)معلومات;

    بدل (event->type)
    {
	حالة GDK_ڪنجي_PRESS:
	    ڪنجي_event(beval, event->ڪنجي.keyval, TRUE);
	    اكسر;
	حالة GDK_ڪنجي_RELEASE:
	    ڪنجي_event(beval, event->ڪنجي.keyval, FALSE);
	    اكسر;
	حالة GDK_LEAVE_NOTIFY:
	    // Ignore LeaveNotify events that are not "normal".
	    // Apparently we also get it when somebody else grabs focus.
	    إذا (event->پار.گهڻائي == GDK_پار_عام)
		cancelBalloon(beval);
	    اكسر;
	افتراضي:
	    اكسر;
    }

    أرجع FALSE; // continue emission
}
#  endif

    ساكن فراغ
اشارو_event(BalloonEval *beval, صحيح x, صحيح y, بنانشان state)
{
    صحيح مفاصلو;

    مفاصلو = ABS(x - beval->x) + ABS(y - beval->y);

    إذا (مفاصلو <= 4)
	أرجع;

    /*
     * Moved out of the balloon location: cancel it.
     * Remember button state
     */
    beval->state = state;
    cancelBalloon(beval);

    // Mouse buttons are pressed - no balloon now
    إذا (!(state & ((صحيح)GDK_BUTTON1_نقاب | (صحيح)GDK_BUTTON2_نقاب
		    | (صحيح)GDK_BUTTON3_نقاب)))
    {
	beval->x = x;
	beval->y = y;

#  ifdef USE_GTK4
	إذا (state & (صحيح)GDK_ALT_نقاب)
#  وإلا
	إذا (state & (صحيح)GDK_MOD1_نقاب)
#  endif
	{
	    /*
	     * Alt is pressed -- enter super-evaluate-mode,
	     * where there is no time delay
	     */
	    إذا (beval->نياپو_CB != عدم)
	    {
		beval->showState = ShS_PENDING;
		(*beval->نياپو_CB)(beval, state);
	    }
	}
	وإلا
	{
	    beval->timerID = g_timeout_جوڙيو((guint)p_bdlay, &timeout_cb, beval);
	}
    }
}

    ساكن فراغ
ڪنجي_event(BalloonEval *beval, بنانشان keyval, صحيح is_keypress)
{
    إذا (beval->showState == ShS_SHOWING && beval->نياپو_CB != عدم)
    {
	بدل (keyval)
	{
#  ifdef USE_GTK4
	    حالة GDK_ڪنجي_سرڪايو_L:
	    حالة GDK_ڪنجي_سرڪايو_R:
#  وإلا
	    حالة GDK_سرڪايو_L:
	    حالة GDK_سرڪايو_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->نياپو_CB)(beval, (is_keypress)
						   ? (صحيح)GDK_سرڪايو_نقاب : 0);
		اكسر;
#  ifdef USE_GTK4
	    حالة GDK_ڪنجي_Control_L:
	    حالة GDK_ڪنجي_Control_R:
#  وإلا
	    حالة GDK_Control_L:
	    حالة GDK_Control_R:
#  endif
		beval->showState = ShS_UPDATE_PENDING;
		(*beval->نياپو_CB)(beval, (is_keypress)
						 ? (صحيح)GDK_CONTROL_نقاب : 0);
		اكسر;
	    افتراضي:
		// Don't do this for key release, we apparently get these with
		// focus changes in some GTK version.
		إذا (is_keypress)
		    cancelBalloon(beval);
		اكسر;
	}
    }
    وإلا
	cancelBalloon(beval);
}

    ساكن gboolean
timeout_cb(gpointer معلومات)
{
    BalloonEval *beval = (BalloonEval *)معلومات;

    beval->timerID = 0;
    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);

    أرجع FALSE; // don't call me again
}

#  ifndef USE_GTK4
#   إذا GTK_CHECK_نسخو(3,0,0)
    ساكن gboolean
balloon_draw_event_cb(GtkWidget *widget,
		      cairo_t	*cr,
		      gpointer	 معلومات اڻ_استعمال_ٿيل)
{
    GtkStyleContext *context = عدم;
    gint ويڪر = -1, اوچائي = -1;

    إذا (widget == عدم)
	أرجع TRUE;

    context = gtk_widget_get_style_context(widget);
    ويڪر = gtk_widget_get_allocated_ويڪر(widget);
    اوچائي = gtk_widget_get_allocated_اوچائي(widget);

    gtk_style_context_save(context);

    gtk_style_context_جوڙيو_class(context, "tooltip");
    gtk_style_context_سيٽ_state(context, GTK_STATE_FLAG_عام);

    cairo_save(cr);
    gtk_render_چوکٺو(context, cr, 0, 0, ويڪر, اوچائي);
    gtk_render_background(context, cr, 0, 0, ويڪر, اوچائي);
    cairo_restore(cr);

    gtk_style_context_restore(context);

    أرجع FALSE;
}
#   وإلا
    ساكن gint
balloon_expose_event_cb(GtkWidget *widget,
			GdkEventExpose *event,
			gpointer معلومات اڻ_استعمال_ٿيل)
{
    gtk_paint_flat_box(widget->style, widget->دري_2,
		       GTK_STATE_عام, GTK_SHADOW_ٻاهريون,
		       &event->area, widget, "tooltip",
		       0, 0, -1, -1);

    أرجع FALSE; // continue emission
}
#   endif // !GTK_CHECK_VERSION(3,0,0)
#  endif

# وإلا // !FEAT_GUI_GTK

    ساكن فراغ
جوڙيو_Event_Handler(Widget هدف, BalloonEval *beval)
{
    Xt_جوڙيو_Event_Handler(هدف,
			اشارو_Motion_نقاب | Enter_دري_نقاب |
			Leave_دري_نقاب | Button_Press_نقاب | ڪنجي_Press_نقاب |
			ڪنجي_Release_نقاب,
			False,
			اشارو_Event_EH, (Xt_اشارو)beval);
}

    ساكن فراغ
removeEventHandler(BalloonEval *beval)
{
    XtRemoveEventHandler(beval->هدف,
			اشارو_Motion_نقاب | Enter_دري_نقاب |
			Leave_دري_نقاب | Button_Press_نقاب | ڪنجي_Press_نقاب |
			ڪنجي_Release_نقاب,
			False,
			اشارو_Event_EH, (Xt_اشارو)beval);
}


/*
 * The X event handler. All it does is call the real event handler.
 */
    ساكن فراغ
اشارو_Event_EH(
    Widget	w اڻ_استعمال_ٿيل,
    Xt_اشارو	client_معلومات_2,
    XEvent	*event,
    Boolean	*اڻ_استعمال_ٿيل_2 اڻ_استعمال_ٿيل)
{
    BalloonEval *beval = (BalloonEval *)client_معلومات_2;
    اشارو_Event(beval, event);
}


/*
 * The real event handler. Called by pointerEventEH() whenever an event we are
 * interested in occurs.
 */

    ساكن فراغ
اشارو_Event(BalloonEval *beval, XEvent *event)
{
    Position	مفاصلو;	    // a measure of how much the pointer moved
    Position	delta;		    // used to compute distance

    بدل (event->type)
    {
	حالة EnterNotify:
	حالة MotionNotify:
	    delta = event->xmotion.x - beval->x;
	    إذا (delta < 0)
		delta = -delta;
	    مفاصلو = delta;
	    delta = event->xmotion.y - beval->y;
	    إذا (delta < 0)
		delta = -delta;
	    مفاصلو += delta;
	    إذا (مفاصلو > 4)
	    {
		/*
		 * Moved out of the balloon location: cancel it.
		 * Remember button state
		 */
		beval->state = event->xmotion.state;
		إذا (beval->state & (Button1_نقاب|Button2_نقاب|Button3_نقاب))
		{
		    // Mouse buttons are pressed - no balloon now
		    cancelBalloon(beval);
		}
		وإلا إذا (beval->state & (Mod1_نقاب|Mod2_نقاب|Mod3_نقاب))
		{
		    /*
		     * Alt is pressed -- enter super-evaluate-mode,
		     * where there is no time delay
		     */
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_پاڙ = event->xmotion.x_پاڙ;
		    beval->y_پاڙ = event->xmotion.y_پاڙ;
		    cancelBalloon(beval);
		    إذا (beval->نياپو_CB != عدم)
		    {
			beval->showState = ShS_PENDING;
			(*beval->نياپو_CB)(beval, beval->state);
		    }
		}
		وإلا
		{
		    beval->x = event->xmotion.x;
		    beval->y = event->xmotion.y;
		    beval->x_پاڙ = event->xmotion.x_پاڙ;
		    beval->y_پاڙ = event->xmotion.y_پاڙ;
		    cancelBalloon(beval);
		    beval->timerID = Xt_App_جوڙيو_وقت_ٻاهريون( beval->appContext,
					(long_u)p_bdlay, timerRoutine, beval);
		}
	    }
	    اكسر;

	/*
	 * Motif version: Keystrokes will be caught by the
	 * "textArea" widget, and handled in gui_x11_key_hit_cb().
	 */
	حالة ڪنجي_Press:
	    إذا (beval->showState == ShS_SHOWING && beval->نياپو_CB != عدم)
	    {
		Modifiers   modifier;
		ڪنجي_Sym	    keysym;

		XtTranslateKeycode(gui.dpy,
				       event->xkey.keycode, event->xkey.state,
				       &modifier, &keysym);
		إذا (keysym == XK_سرڪايو_L || keysym == XK_سرڪايو_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->نياپو_CB)(beval, سرڪايو_نقاب);
		}
		وإلا إذا (keysym == XK_Control_L || keysym == XK_Control_R)
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->نياپو_CB)(beval, Control_نقاب);
		}
		وإلا
		    cancelBalloon(beval);
	    }
	    وإلا
		cancelBalloon(beval);
	    اكسر;

	حالة ڪنجي_Release:
	    إذا (beval->showState == ShS_SHOWING && beval->نياپو_CB != عدم)
	    {
		Modifiers modifier;
		ڪنجي_Sym keysym;

		XtTranslateKeycode(gui.dpy, event->xkey.keycode,
				event->xkey.state, &modifier, &keysym);
		إذا ((keysym == XK_سرڪايو_L) || (keysym == XK_سرڪايو_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->نياپو_CB)(beval, 0);
		}
		وإلا إذا ((keysym == XK_Control_L) || (keysym == XK_Control_R))
		{
		    beval->showState = ShS_UPDATE_PENDING;
		    (*beval->نياپو_CB)(beval, 0);
		}
		وإلا
		    cancelBalloon(beval);
	    }
	    وإلا
		cancelBalloon(beval);
	    اكسر;

	حالة LeaveNotify:
		// Ignore LeaveNotify events that are not "normal".
		// Apparently we also get it when somebody else grabs focus.
		// Happens for me every two seconds (some clipboard tool?)
		إذا (event->xcrossing.گهڻائي == Notify_عام)
		    cancelBalloon(beval);
		اكسر;

	حالة ButtonPress:
		cancelBalloon(beval);
		اكسر;

	افتراضي:
	    اكسر;
    }
}

    ساكن فراغ
timerRoutine(Xt_اشارو dx, XtIntervalId *id اڻ_استعمال_ٿيل)
{
    BalloonEval *beval = (BalloonEval *)dx;

    beval->timerID = (XtIntervalId)عدم;

    /*
     * If the timer event happens then the mouse has stopped long enough for
     * a request to be started. The request will only send to the debugger if
     * there the mouse is pointing at real data.
     */
    requestBalloon(beval);
}

# endif // !FEAT_GUI_GTK

    ساكن فراغ
requestBalloon(BalloonEval *beval)
{
    إذا (beval->showState == ShS_PENDING)
	أرجع;

    // Determine the beval to display
    إذا (beval->نياپو_CB != عدم)
    {
	beval->showState = ShS_PENDING;
	(*beval->نياپو_CB)(beval, beval->state);
    }
    وإلا إذا (beval->نياپو != عدم)
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
    ساكن فراغ
سيٽ_printable_label_متن(GtkLabel *label, char_u *متن)
{
    char_u	    *convbuf = عدم;
    char_u	    *buf;
    char_u	    *p;
    char_u	    *pdest;
    بنانشان صحيح    len;
    صحيح		    charlen;
    صحيح		    uc;
    PangoAttrList   *attr_list;

    // Convert to UTF-8 if it isn't already
    إذا (output_conv.vc_type != CONV_NONE)
    {
	convbuf = string_convert(&output_conv, متن, عدم);
	إذا (convbuf != عدم)
	    متن = convbuf;
    }

    // First let's see how much we need to allocate
    len = 0;
    لكل (p = متن; *p != NUL; p += charlen)
    {
	إذا ((*p & 0x80) == 0)	// be quick for ASCII
	{
	    charlen = 1;
	    len += IS_NONPRINTABLE(*p) ? 2 : 1;	// nonprintable: ^X
	}
	وإلا
	{
	    charlen = utf_ptr2len(p);
	    uc = utf_ptr2char(p);

	    إذا (charlen != utf_char2len(uc))
		charlen = 1; // reject overlong sequences

	    إذا (charlen == 1 || uc < 0xa0)	// illegal byte or
		len += 4;			// control char: <xx>
	    وإلا إذا (!utf_printable(uc))
		// Note: we assume here that utf_printable() doesn't
		// care about characters outside the BMP.
		len += 6;			// nonprintable: <xxxx>
	    وإلا
		len += charlen;
	}
    }

    attr_list = pango_attr_list_new();
    buf = alloc(len + 1);

    // Now go for the real work
    إذا (buf != عدم)
    {
	attrentry_T	*aep;
	PangoAttribute	*attr;
	guicolor_T	pixel;
#  إذا GTK_CHECK_نسخو(3,0,0)
	GdkRGBA		color = { 0.0, 0.0, 0.0, 1.0 };
#   إذا PANGO_نسخو_CHECK(1,38,0)
	PangoAttribute  *attr_alpha;
#   endif
#  وإلا
	GdkColor	color = { 0, 0, 0, 0 };
#  endif

	// Look up the RGB values of the SpecialKey foreground color.
	aep = syn_gui_attr2entry(HL_ATTR(HLF_8));
	pixel = (aep != عدم) ? aep->ae_u.gui.fg_color : INVALCOLOR;
	إذا (pixel != INVALCOLOR)
#  إذا GTK_CHECK_نسخو(3,0,0)
	{
	    color.red = ((pixel & 0xff0000) >> 16) / 255.0;
	    color.green = ((pixel & 0xff00) >> 8) / 255.0;
	    color.blue = (pixel & 0xff) / 255.0;
	    color.alpha = 1.0;
	}
#  وإلا
	    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
				     (بنانشان ڊگهو)pixel, &color);
#  endif

	pdest = buf;
	p = متن;
	طالما (*p != NUL)
	{
	    // Be quick for ASCII
	    إذا ((*p & 0x80) == 0 && !IS_NONPRINTABLE(*p))
	    {
		*pdest++ = *p++;
	    }
	    وإلا
	    {
		charlen = utf_ptr2len(p);
		uc = utf_ptr2char(p);

		إذا (charlen != utf_char2len(uc))
		    charlen = 1; // reject overlong sequences

		إذا (charlen == 1 || uc < 0xa0 || !utf_printable(uc))
		{
		    صحيح	outlen;

		    // Careful: we can't just use transchar_byte() here,
		    // since 'encoding' is not necessarily set to "utf-8".
		    إذا (*p & 0x80 && charlen == 1)
		    {
			transchar_hex(pdest, *p);	// <xx>
			outlen = 4;
		    }
		    وإلا إذا (uc >= 0x80)
		    {
			// Note: we assume here that utf_printable() doesn't
			// care about characters outside the BMP.
			transchar_hex(pdest, uc);	// <xx> or <xxxx>
			outlen = (uc < 0x100) ? 4 : 6;
		    }
		    وإلا
		    {
			transchar_nonprint(curbuf, pdest, *p);	// ^X
			outlen = 2;
		    }
		    إذا (pixel != INVALCOLOR)
		    {
#  إذا GTK_CHECK_نسخو(3,0,0)
#   define DOUBLE2UINT16(val) ((guint16)((val) * 65535 + 0.5))
			attr = pango_attr_foreground_new(
				DOUBLE2UINT16(color.red),
				DOUBLE2UINT16(color.green),
				DOUBLE2UINT16(color.blue));
#   إذا PANGO_نسخو_CHECK(1,38,0)
			attr_alpha = pango_attr_foreground_alpha_new(
				DOUBLE2UINT16(color.alpha));
#   endif
#   undef DOUBLE2UINT16
#  وإلا
			attr = pango_attr_foreground_new(
				color.red, color.green, color.blue);
#  endif
			attr->شروعات_اشاريو = pdest - buf;
			attr->پڇاڙي_اشاريو   = pdest - buf + outlen;
			pango_attr_list_وجهو(attr_list, attr);
#  إذا GTK_CHECK_نسخو(3,0,0)
#   إذا PANGO_نسخو_CHECK(1,38,0)
			attr_alpha->شروعات_اشاريو = pdest - buf;
			attr_alpha->پڇاڙي_اشاريو   = pdest - buf + outlen;
			pango_attr_list_وجهو(attr_list, attr_alpha);
#   endif
#  endif
		    }
		    pdest += outlen;
		    p += charlen;
		}
		وإلا
		{
		    افعل
			*pdest++ = *p++;
		    طالما (--charlen != 0);
		}
	    }
	}
	*pdest = NUL;
    }

    vim_free(convbuf);

    gtk_label_سيٽ_متن(label, (ثابت حرف *)buf);
    vim_free(buf);

    gtk_label_سيٽ_خاصيتون(label, attr_list);
    pango_attr_list_unref(attr_list);
}
#  undef IS_NONPRINTABLE

/*
 * Draw a balloon.
 */
#  ifdef USE_GTK4
    ساكن فراغ
drawBalloon(BalloonEval *beval)
{
    GdkRectangle    rect;
    صحيح		    natural_ويڪر = 0;

    إذا (beval->نياپو == عدم)
	أرجع;

    سيٽ_printable_label_متن(GTK_LABEL(beval->balloonLabel), beval->نياپو);

    rect.x = beval->x;
    rect.y = beval->y;

    // Make popover appear at the bottom right of the pointer. Also add an
    // offset (gui.char_width * 3, gui.char_height), so that popover doesn't
    // block the text. This also prevents the popover from stealing motion
    // events, making pressing alt (to force show beval) and moving cursor
    // around feel smooth.
    gtk_widget_measure(beval->balloonLabel, GTK_ORIENTATION_HORIZONTAL, -1,
	    عدم, &natural_ويڪر, عدم, عدم);

    rect.ويڪر = (natural_ويڪر > 0 ? natural_ويڪر : 1) + gui.char_ويڪر * 3;
    rect.اوچائي = gui.char_اوچائي;

    gtk_popover_سيٽ_pointing_منزل(GTK_POPOVER(beval->balloonShell), &rect);
    gtk_popover_popup(GTK_POPOVER(beval->balloonShell));

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  وإلا
    ساكن فراغ
drawBalloon(BalloonEval *beval)
{
    إذا (beval->نياپو == عدم)
	أرجع;

    GtkRequisition	requisition;
    صحيح		screen_w;
    صحيح		screen_h;
    صحيح		screen_x;
    صحيح		screen_y;
    صحيح		x;
    صحيح		y;
    صحيح		x_وٿي = EVAL_وٿي_X;
    صحيح		y_وٿي = EVAL_وٿي_Y;
    PangoLayout	*layout;

#   إذا !GTK_CHECK_نسخو(3,22,2)
    GdkScreen	*screen;

    screen = gtk_widget_get_screen(beval->هدف);
    gtk_دري_سيٽ_screen(GTK_دري(beval->balloonShell), screen);
#   endif
    gui_gtk_get_screen_geom_جو_win(beval->هدف, 0, 0,
	    &screen_x, &screen_y, &screen_w, &screen_h);
#   إذا !GTK_CHECK_نسخو(3,0,0)
    gtk_widget_ensure_style(beval->balloonShell);
    gtk_widget_ensure_style(beval->balloonLabel);
#   endif

    سيٽ_printable_label_متن(GTK_LABEL(beval->balloonLabel), beval->نياپو);
    /*
     * Dirty trick:  Enable wrapping mode on the label's layout behind its
     * back.  This way GtkLabel won't try to constrain the wrap width to a
     * builtin maximum value of about 65 Latin characters.
     */
    layout = gtk_label_get_layout(GTK_LABEL(beval->balloonLabel));
#   ifdef PANGO_WRAP_لفظ_CHAR
    pango_layout_سيٽ_wrap(layout, PANGO_WRAP_لفظ_CHAR);
#   وإلا
    pango_layout_سيٽ_wrap(layout, PANGO_WRAP_لفظ);
#   endif
    pango_layout_سيٽ_ويڪر(layout,
	    // try to come up with some reasonable width
	    PANGO_SCALE * CLAMP(gui.انگ_کانن_ڳڻپ * gui.char_ويڪر,
		screen_w / 2,
		MAX(20, screen_w - 20)));

    // Calculate the balloon's width and height.
#   إذا GTK_CHECK_نسخو(3,0,0)
    gtk_widget_get_preferred_سائيز(beval->balloonShell, &requisition, عدم);
#   وإلا
    gtk_widget_سائيز_request(beval->balloonShell, &requisition);
#   endif

    // Compute position of the balloon area
    gdk_دري_get_origin(gtk_widget_get_دري(beval->هدف), &x, &y);
    x += beval->x;
    y += beval->y;

    // Get out of the way of the mouse pointer
    إذا (x + x_وٿي + requisition.ويڪر > screen_x + screen_w)
	y_وٿي += 15;
    إذا (y + y_وٿي + requisition.اوچائي > screen_y + screen_h)
	y_وٿي = -requisition.اوچائي - EVAL_وٿي_Y;

    // Sanitize values
    x = CLAMP(x + x_وٿي, 0,
	    MAX(0, screen_x + screen_w - requisition.ويڪر));
    y = CLAMP(y + y_وٿي, 0,
	    MAX(0, screen_y + screen_h - requisition.اوچائي));

    // Show the balloon
#   إذا GTK_CHECK_نسخو(3,0,0)
    gtk_دري_move(GTK_دري(beval->balloonShell), x, y);
#   وإلا
    gtk_widget_سيٽ_uposition(beval->balloonShell, x, y);
#   endif
    gtk_widget_show(beval->balloonShell);

    beval->showState = ShS_SHOWING;
    gui_mch_update();
}
#  endif

/*
 * Undraw a balloon.
 */
    ساكن فراغ
undrawBalloon(BalloonEval *beval)
{
    إذا (beval->balloonShell != عدم)
#  ifdef USE_GTK4
	gtk_popover_popdown(GTK_POPOVER(beval->balloonShell));
#  وإلا
	gtk_widget_hide(beval->balloonShell);
#  endif
    beval->showState = ShS_NEUTRAL;
}

    ساكن فراغ
cancelBalloon(BalloonEval *beval)
{
    إذا (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    إذا (beval->timerID != 0)
    {
	g_ذريعو_remove(beval->timerID);
	beval->timerID = 0;
    }
    beval->showState = ShS_NEUTRAL;
}

#  ifdef USE_GTK4
    ساكن فراغ
create_Balloon_Eval_دري(BalloonEval *beval)
{
    GtkWidget *popover = gtk_popover_new();
    beval->balloonShell = popover;

    gtk_widget_سيٽ_والد(beval->balloonShell, beval->هدف);
    gtk_popover_سيٽ_has_arrow(GTK_POPOVER(popover), FALSE);
    gtk_popover_سيٽ_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    // Don't make popover grab focus
    gtk_popover_سيٽ_autohide(GTK_POPOVER(popover), FALSE);

    beval->balloonLabel = gtk_label_new(عدم);

    gtk_label_سيٽ_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_سيٽ_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_کاٻو);
    gtk_label_سيٽ_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_سيٽ_yalign(GTK_LABEL(beval->balloonLabel), 0.5);

    gtk_popover_سيٽ_ٻار(GTK_POPOVER(popover), beval->balloonLabel);
}
#  وإلا
    ساكن فراغ
create_Balloon_Eval_دري(BalloonEval *beval)
{
    beval->balloonShell = gtk_دري_new(GTK_دري_POPUP);
    gtk_دري_سيٽ_transient_for(GTK_دري(beval->balloonShell), GTK_دري(gui.mainwin));

    gtk_widget_سيٽ_app_paintable(beval->balloonShell, TRUE);
    gtk_دري_سيٽ_resizable(GTK_دري(beval->balloonShell), FALSE);
    gtk_widget_سيٽ_name(beval->balloonShell, "gtk-tooltips");
    gtk_container_سيٽ_border_ويڪر(GTK_CONTAINER(beval->balloonShell), 4);

#   إذا GTK_CHECK_نسخو(3,0,0)
    g_signal_connect(G_OBJECT(beval->balloonShell), "draw",
	    G_CALLBACK(balloon_draw_event_cb), عدم);
#   وإلا
    gtk_signal_connect((GtkObject*)(beval->balloonShell), "expose_event",
	    GTK_SIGNAL_FUNC(balloon_expose_event_cb), عدم);
#   endif
    beval->balloonLabel = gtk_label_new(عدم);

    gtk_label_سيٽ_سٽ_wrap(GTK_LABEL(beval->balloonLabel), FALSE);
    gtk_label_سيٽ_justify(GTK_LABEL(beval->balloonLabel), GTK_JUSTIFY_کاٻو);
#   إذا GTK_CHECK_نسخو(3,16,0)
    gtk_label_سيٽ_xalign(GTK_LABEL(beval->balloonLabel), 0.5);
    gtk_label_سيٽ_yalign(GTK_LABEL(beval->balloonLabel), 0.5);
#   elif GTK_CHECK_نسخو(3,14,0)
    GValue align_val = G_قدر_INIT;
    g_قدر_init(&align_val, G_TYPE_FLOAT);
    g_قدر_سيٽ_float(&align_val, 0.5);
    g_object_سيٽ_property(G_OBJECT(beval->balloonLabel), "xalign", &align_val);
    g_object_سيٽ_property(G_OBJECT(beval->balloonLabel), "yalign", &align_val);
    g_قدر_unset(&align_val);
#   وإلا
    gtk_misc_سيٽ_alignment(GTK_MISC(beval->balloonLabel), 0.5f, 0.5f);
#   endif
    gtk_widget_سيٽ_name(beval->balloonLabel, "vim-balloon-label");
    gtk_widget_show(beval->balloonLabel);

    gtk_container_جوڙيو(GTK_CONTAINER(beval->balloonShell), beval->balloonLabel);
}
#  endif

# وإلا // !FEAT_GUI_GTK

/*
 * Draw a balloon.
 */
    ساكن فراغ
drawBalloon(BalloonEval *beval)
{
    Dimension	w;
    Dimension	h;
    Position tx;
    Position ty;

    إذا (beval->نياپو == عدم)
	أرجع;

    XmString s;
    // Show the Balloon

    // Calculate the label's width and height

    // For the callback function we parse NL characters to create a
    // multi-line label.  This doesn't work for all languages, but
    // XmStringCreateLocalized() doesn't do multi-line labels...
    إذا (beval->نياپو_CB != عدم)
	s = XmStringCreateLtoR((حرف *)beval->نياپو, Xm_FONTLIST_DEFAULT_ٽيگ);
    وإلا
	s = XmStringCreateLocalized((حرف *)beval->نياپو);
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
	إذا (fl == عدم)
	{
	    XmStringFree(s);
	    أرجع;
	}
	XmStringExtent(fl, s, &w, &h);
	XmFontListFree(fl);
    }
    w += gui.border_وٿي << 1;
    h += gui.border_وٿي << 1;
    Xt_Va_سيٽ_قدرون(beval->balloonLabel, XmNlabelString, s, عدم);
    XmStringFree(s);

    // Compute position of the balloon area
    tx = beval->x_پاڙ + EVAL_وٿي_X;
    ty = beval->y_پاڙ + EVAL_وٿي_Y;
    إذا ((tx + w) > beval->screen_ويڪر)
	tx = beval->screen_ويڪر - w;
    إذا ((ty + h) > beval->screen_اوچائي)
	ty = beval->screen_اوچائي - h;
    Xt_Va_سيٽ_قدرون(beval->balloonShell,
	    XmNx, tx,
	    XmNy, ty,
	    عدم);
    // Set tooltip colors
    {
	Arg دليلون[2];

	دليلون[0].name = XmNbackground;
	دليلون[0].قدر = gui.tooltip_bg_pixel;
	دليلون[1].name = XmNforeground;
	دليلون[1].قدر = gui.tooltip_fg_pixel;
	Xt_سيٽ_قدرون(beval->balloonLabel, &دليلون[0], Xt_عدد(دليلون));
    }

    XtPopup(beval->balloonShell, XtGrabNone);

    beval->showState = ShS_SHOWING;

    current_beval = beval;
}

/*
 * Undraw a balloon.
 */
    ساكن فراغ
undrawBalloon(BalloonEval *beval)
{
    إذا (beval->balloonShell != (Widget)0)
	XtPopdown(beval->balloonShell);
    beval->showState = ShS_NEUTRAL;

    current_beval = عدم;
}

    ساكن فراغ
cancelBalloon(BalloonEval *beval)
{
    إذا (beval->showState == ShS_SHOWING
	    || beval->showState == ShS_UPDATE_PENDING)
	undrawBalloon(beval);

    إذا (beval->timerID != (XtIntervalId)عدم)
    {
	Xt_Remove_وقت_ٻاهريون(beval->timerID);
	beval->timerID = (XtIntervalId)عدم;
    }
    beval->showState = ShS_NEUTRAL;
}


    ساكن فراغ
create_Balloon_Eval_دري(BalloonEval *beval)
{
    Arg		دليلون[12];
    صحيح		n;

    n = 0;
    Xt_سيٽ_Arg(دليلون[n], XmNallowShellResize, True); n++;
    beval->balloonShell = XtAppCreateShell("balloonEval", "BalloonEval",
		    overrideShellWidgetClass, gui.dpy, دليلون, n);

    XmFontList fl;

    n = 0;
    fl = gui_motif_fontset2fontlist(&gui.tooltip_fontset);
    Xt_سيٽ_Arg(دليلون[n], XmNforeground, gui.tooltip_fg_pixel); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNbackground, gui.tooltip_bg_pixel); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNfontList, fl); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    beval->balloonLabel = XtCreateManagedWidget("balloonLabel",
	    xmLabelWidgetClass, beval->balloonShell, دليلون, n);
}

# endif // !FEAT_GUI_GTK
#جيڪڏهن_پورو // !FEAT_GUI_MSWIN

#جيڪڏهن_پورو // FEAT_BEVAL_GUI
