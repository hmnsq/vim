/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 *
 * GTK4 GUI implementation: main window, events, drawing, menus,
 * scrollbars, dialogs, and toolbar.  This is a clean implementation for
 * GTK4, separate from gui_gtk_x11.c which handles GTK2/GTK3.
 *
 * GTK4 differences from GTK3:
 * - No GdkWindow (use GdkSurface for top-level only)
 * - No GtkContainer (use gtk_widget_set_parent/gtk_box_append)
 * - Events via GtkEventController, not signal+mask
 * - Drawing via GtkSnapshot or gtk_drawing_area_set_draw_func
 * - No gtk_dialog_run (async dialogs)
 * - No GdkAtom (string-based content types)
 * - No GtkSocket/GtkPlug
 * - gtk_window_new() takes no arguments
 */

#ضمن "vim.h"

#ifdef FEAT_GUI_GTK

#ضمن <gdk/gdkkeysyms.h>
#ضمن <gdk/gdk.h>
#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_f.h"
#ضمن "gui_gtk4_cb.h"
#ضمن "gui_gtk4_da.h"
#ifdef FEAT_TOOLBAR
# include "gui_gtk4_tb.h"
#endif
#ifdef FEAT_MENU
# include "gui_gtk4_menu.h"
#endif

/*
 * Geometry string parser, replacing XParseGeometry to remove X11 dependency.
 * Format: [WIDTHxHEIGHT][{+-}XOFF{+-}YOFF]
 */
#define No_بەها		0x0000
#define Width_بەها	0x0004
#define Height_بەها	0x0008

    ساكن صحيح
vim_parse_geometry(ثابت حرف *str, unsigned صحيح *width, unsigned صحيح *height)
{
    صحيح mask = No_بەها;
    حرف *end;
    long val;

    إذا (str == عدم || *str == NUL)
	أرجع mask;

    // Parse width
    إذا (*str != '+' && *str != '-')
    {
	val = strtol(str, &end, 10);
	إذا (end != str)
	{
	    *width = (unsigned صحيح)val;
	    mask |= Width_بەها;
	    str = end;
	}
    }

    // Parse 'x' or 'X' separator and height
    إذا (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &end, 10);
	إذا (end != str)
	{
	    *height = (unsigned صحيح)val;
	    mask |= Height_بەها;
	    str = end;
	}
    }

    أرجع mask;
}

#إذا defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
ساكن صحيح last_shape = 0;
#endif

#define DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
ساكن تعداد {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
ساكن صحيح gtk4_main_loop_level = 0;
ساكن صحيح gtk4_main_loop_quit = FALSE;

#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

تعريفنوع gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
ساكن بنية special_کلیل
{
    guint کلیل_sym;
    char_u code0;
    char_u code1;
}
ثابت special_کلیلەکان[] =
{
    {GDK_کلیل_Up,	'k', 'u'},
    {GDK_کلیل_Down,	'k', 'd'},
    {GDK_کلیل_چەپ,	'k', 'l'},
    {GDK_کلیل_ڕاست,	'k', 'r'},
    {GDK_کلیل_F1,	'k', '1'},
    {GDK_کلیل_F2,	'k', '2'},
    {GDK_کلیل_F3,	'k', '3'},
    {GDK_کلیل_F4,	'k', '4'},
    {GDK_کلیل_F5,	'k', '5'},
    {GDK_کلیل_F6,	'k', '6'},
    {GDK_کلیل_F7,	'k', '7'},
    {GDK_کلیل_F8,	'k', '8'},
    {GDK_کلیل_F9,	'k', '9'},
    {GDK_کلیل_F10,	'k', ';'},
    {GDK_کلیل_F11,	'F', '1'},
    {GDK_کلیل_F12,	'F', '2'},
    {GDK_کلیل_Help,	'%', '1'},
    {GDK_کلیل_Undo,	'&', '8'},
    {GDK_کلیل_Back_Space,	'k', 'b'},
    {GDK_کلیل_زیادکردن,	'k', 'I'},
    {GDK_کلیل_Delete,	'k', 'D'},
    {GDK_کلیل_Home,	'k', 'h'},
    {GDK_کلیل_End,	'@', '7'},
    {GDK_کلیل_Prior,	'k', 'P'},
    {GDK_کلیل_Next,	'k', 'N'},
    {GDK_کلیل_Print,	'%', '9'},
    {GDK_کلیل_KP_چەپ,	'k', 'l'},
    {GDK_کلیل_KP_ڕاست,	'k', 'r'},
    {GDK_کلیل_KP_Up,	'k', 'u'},
    {GDK_کلیل_KP_Down,	'k', 'd'},
    {GDK_کلیل_KP_زیادکردن,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_کلیل_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_کلیل_KP_Home,	'K', '1'},
    {GDK_کلیل_KP_End,	'K', '4'},
    {GDK_کلیل_KP_Prior,	'K', '3'},
    {GDK_کلیل_KP_Next,	'K', '5'},
    {GDK_کلیل_KP_Add,	'K', '6'},
    {GDK_کلیل_KP_Subtract, 'K', '7'},
    {GDK_کلیل_KP_Divide,	'K', '8'},
    {GDK_کلیل_KP_Multiply, 'K', '9'},
    {GDK_کلیل_KP_Enter,	'K', 'A'},
    {GDK_کلیل_KP_Decimal,	'K', 'B'},
    {GDK_کلیل_KP_0,	'K', 'C'},
    {GDK_کلیل_KP_1,	'K', 'D'},
    {GDK_کلیل_KP_2,	'K', 'E'},
    {GDK_کلیل_KP_3,	'K', 'F'},
    {GDK_کلیل_KP_4,	'K', 'G'},
    {GDK_کلیل_KP_5,	'K', 'H'},
    {GDK_کلیل_KP_6,	'K', 'I'},
    {GDK_کلیل_KP_7,	'K', 'J'},
    {GDK_کلیل_KP_8,	'K', 'K'},
    {GDK_کلیل_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    ساكن صحيح
keyval_to_string(unsigned صحيح keyval, char_u *string)
{
    صحيح		len;
    guint32	uc;

    uc = gdk_keyval_to_unicode(keyval);
    إذا (uc != 0)
    {
	len = utf_char2bytes((صحيح)uc, string);
    }
    وإلا
    {
	len = 1;
	بدل (keyval)
	{
	    حالة GDK_کلیل_Tab: حالة GDK_کلیل_KP_Tab: حالة GDK_کلیل_ISO_چەپ_Tab:
		string[0] = TAB;
		اكسر;
	    حالة GDK_کلیل_Linefeed:
		string[0] = NL;
		اكسر;
	    حالة GDK_کلیل_Return: حالة GDK_کلیل_ISO_Enter: حالة GDK_کلیل_3270_Enter:
		string[0] = CAR;
		اكسر;
	    حالة GDK_کلیل_Escape:
		string[0] = ESC;
		اكسر;
	    افتراضي:
		len = 0;
		اكسر;
	}
    }
    string[len] = NUL;
    أرجع len;
}

    ساكن صحيح
modifiers_gdk2vim(guint state)
{
    صحيح modifiers = 0;

    إذا (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    إذا (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    إذا (state & GDK_ALT_MASK)
	modifiers |= MOD_MASK_ALT;
    إذا (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    إذا (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;

    أرجع modifiers;
}

ساكن GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
ساكن gboolean کلیل_press_event(Gtk_Event_Controller_کلیل *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
ساكن فراغ کلیل_release_event(Gtk_Event_Controller_کلیل *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
ساكن فراغ button_press_event(GtkGestureClick *gesture, صحيح n_press, مزدوج x, مزدوج y, gpointer data);
ساكن فراغ button_release_event(GtkGestureClick *gesture, صحيح n_press, مزدوج x, مزدوج y, gpointer data);
ساكن فراغ motion_notify_event(GtkEventControllerMotion *controller, مزدوج x, مزدوج y, gpointer data);
ساكن فراغ enter_notify_event(GtkEventControllerMotion *controller, مزدوج x, مزدوج y, gpointer data);
ساكن gboolean scroll_event(GtkEventControllerScroll *controller, مزدوج dx, مزدوج dy, gpointer data);
ساكن فراغ focus_in_event(GtkEventControllerFocus *controller, gpointer data);
ساكن فراغ focus_out_event(GtkEventControllerFocus *controller, gpointer data);
#ifdef FEAT_DND
ساكن gboolean drop_cb(GtkDropTargetAsync *target, GdkDrop *drop, مزدوج x, مزدوج y, فراغ *data);
#endif
#ifdef FEAT_GUI_TABLINE
ساكن فراغ tabline_enter_cb(GtkEventController *controller, مزدوج x, مزدوج y, فراغ *udata);
ساكن فراغ on_select_tab(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
ساكن فراغ on_tab_reordered(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
ساكن VimMenu *create_tabline_popup_menu(فراغ);
ساكن فراغ tabline_add_css(فراغ);
ساكن فراغ tabline_menu_press_event(GtkGestureClick *gesture, صحيح n_press, مزدوج x, مزدوج y, GtkWidget *popover);
#endif
ساكن فراغ mainwin_destroy_cb(GObject *object, gpointer data);
ساكن gboolean delete_event_cb(GtkWindow *window, gpointer data);
ساكن صحيح query_pointer_pos(صحيح *x, صحيح *y, GdkModifierType *state);
ساكن فراغ mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_data);
ساكن فراغ set_form_size(صحيح width, صحيح height);
ساكن فراغ drawarea_realize_cb(GtkWidget *widget, gpointer data);
ساكن فراغ drawarea_unrealize_cb(GtkWidget *widget, gpointer data);
ساكن فراغ clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data);
#ifdef FEAT_MENU
ساكن فراغ show_menubar_popover(فراغ);
#endif

ساكن ثابت حرف *prgname = عدم;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    ساكن gboolean
arg_match(ثابت حرف *s, ثابت حرف *name, حرف **بەها)
{
    size_t  len = strlen(name);

    إذا (strncmp(s, name, len) != 0)
	أرجع FALSE;
    إذا (s[len] == '=')
	*بەها = (حرف *)s + len + 1;
    وإلا إذا (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	أرجع FALSE;
    أرجع TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    فراغ
gui_mch_prepare(صحيح *argc, حرف **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    صحيح	i = 0;

    طالما (i < *argc)
    {
	حرف	*s = argv[i];
	حرف	*بەها = عدم;
	صحيح	has_inline_بەها = FALSE;
	صحيح	n_strip;

	إذا (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    استمر;
	}

	إذا (strchr(s, '=') != عدم)
	    has_inline_بەها = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	إذا (!has_inline_بەها)
	{
	    إذا (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		بەها = argv[i + 1];
	    وإلا
		بەها = عدم;
	}

	إذا (arg_match(s, "-fn", &بەها) || arg_match(s, "-font", &بەها))
	    font_argument = بەها;
	وإلا إذا (arg_match(s, "-geom", &بەها)
		|| arg_match(s, "-geometry", &بەها))
	{
	    إذا (بەها != عدم)
		gui.geom = vim_strsave((char_u *)بەها);
	}
	وإلا إذا (arg_match(s, "-bg", &بەها)
		|| arg_match(s, "-background", &بەها))
	    background_argument = بەها;
	وإلا إذا (arg_match(s, "-fg", &بەها)
		|| arg_match(s, "-foreground", &بەها))
	    foreground_argument = بەها;
#ifdef FEAT_NETBEANS_INTG
	وإلا إذا (strncmp(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_بەها = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#endif
	وإلا إذا (arg_match(s, "--prg-name", &بەها))
	    // GTK4 specific
	    prgname = بەها;
	وإلا
	{
	    i++;
	    استمر;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	إذا (بەها != عدم && !has_inline_بەها)
	    n_strip = 2;

	إذا (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    إذا (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * حجم(حرف *));
	    argv[*argc] = عدم;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    فراغ
gui_mch_free_all(فراغ)
{
}

    ساكن guint
timeout_add(صحيح time, timeout_cb_type (*callback)(gpointer), صحيح *flagp)
{
    أرجع g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    ساكن فراغ
timeout_remove(guint timer)
{
    g_source_remove(timer);
}

ساكن long_u blink_waittime = 700;
ساكن long_u blink_ontime = 400;
ساكن long_u blink_offtime = 250;
ساكن guint blink_timer = 0;

    ساكن timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, عدم);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, عدم);
    }
    أرجع FALSE;
}

    صحيح
gui_mch_is_blinking(فراغ)
{
    أرجع blink_state != BLINK_NONE;
}

    صحيح
gui_mch_is_blink_off(فراغ)
{
    أرجع blink_state == BLINK_OFF;
}

    فراغ
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    فراغ
gui_mch_stop_blink(صحيح may_call_gui_update_cursor)
{
    إذا (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    إذا (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    فراغ
gui_mch_start_blink(فراغ)
{
    إذا (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, عدم);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    صحيح
gui_mch_early_init_check(صحيح give_message UNUSED)
{
    أرجع OK;
}

    صحيح
gui_mch_init_check(فراغ)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_set_prgname(prgname == عدم ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    إذا (g_getenv("EGL_LOG_LEVEL") == عدم)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    أرجع OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    صحيح
gui_mch_init(فراغ)
{
    // Allocate GdkRGBA color structs.
    gui.fgcolor = g_new0(GdkRGBA, 1);
    gui.bgcolor = g_new0(GdkRGBA, 1);
    gui.spcolor = g_new0(GdkRGBA, 1);

    gui.def_norm_pixel = 0x00000000;	// black
    gui.def_back_pixel = 0x00ffffff;	// white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = SB_DEFAULT_WIDTH;
    gui.scrollbar_height = SB_DEFAULT_WIDTH;

    // Create the main window.
    gui.mainwin = gtk_window_new();
    gtk_widget_set_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.text_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_set_base_dir(gui.text_context, PANGO_DIRECTION_LTR);

    g_signal_connect(G_OBJECT(gui.mainwin), "close-request",
		     G_CALLBACK(delete_event_cb), عدم);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), عدم);

    // A vertical box holds the menubar, toolbar and main text window.
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
    gtk_window_set_child(GTK_WINDOW(gui.mainwin), vbox);

#ifdef FEAT_MENU
    gui.menubar = vim_menu_bar_new();
    gtk_widget_set_name(gui.menubar, "vim-menubar");
    gtk_widget_set_visible(gui.menubar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.menubar);
#endif

#ifdef FEAT_TOOLBAR
    gui.toolbar = vim_toolbar_new();
    gtk_widget_set_name(gui.toolbar, "vim-toolbar");
    gtk_widget_set_visible(gui.toolbar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.toolbar);
    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar), toolbar_flags, tbis_flags);
#endif

#ifdef FEAT_GUI_TABLINE
    gui.tabline = gtk_notebook_new();
    gtk_notebook_set_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_widget_add_css_class(gui.tabline, "vim-tabline");
    tabline_add_css();
    gtk_widget_set_visible(gui.tabline, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.tabline);

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), عدم);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), عدم);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), عدم);

	// Make sure that tabline_enter_cb() is always called before
	// tabpage_enter_cb().
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	gtk_widget_add_controller(gui.tabline, mcontroller);
    }

    // Create right click popup menu for tabline
    {
	GtkGesture	*click;
	VimMenu		*menu;

	click = gtk_gesture_click_new();
	menu = create_tabline_popup_menu();

	gtk_widget_set_parent(GTK_WIDGET(menu), gui.tabline);
	g_object_set_data(G_OBJECT(gui.tabline), "menu", menu);

	gtk_popover_set_has_arrow(GTK_POPOVER(menu), FALSE);
	gtk_popover_set_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
	// Make popover start at top left corner
	gtk_widget_set_halign(GTK_WIDGET(menu), GTK_ALIGN_START);

	// Listen for anny mouse button
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);

	g_signal_connect_object(click, "pressed",
		G_CALLBACK(tabline_menu_press_event),
		menu, G_CONNECT_DEFAULT);
	gtk_widget_add_controller(gui.tabline, GTK_EVENT_CONTROLLER(click));
    }
#endif

    // The form widget manages absolute positioning of scrollbars and the draw
    // area.
    gui.formwin = vim_form_new();
    gtk_widget_set_name(gui.formwin, "vim-gtk-form");
    gtk_widget_set_vexpand(gui.formwin, TRUE);
    gtk_widget_set_hexpand(gui.formwin, TRUE);
    gtk_box_append(GTK_BOX(vbox), gui.formwin);

    // The drawing area for the editor content.
    gui.drawarea = vim_draw_area_new();
    gtk_widget_set_focusable(gui.drawarea, TRUE);
    gtk_widget_set_vexpand(gui.drawarea, TRUE);
    gtk_widget_set_hexpand(gui.drawarea, TRUE);
    vim_form_put(VIM_FORM(gui.formwin), gui.drawarea, 0, 0);

    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), عدم);

    // Set up event controllers.
    {
	GtkEventController *کلیل_ctrl = gtk_event_controller_کلیل_new();
	g_signal_connect(کلیل_ctrl, "key-pressed",
			 G_CALLBACK(کلیل_press_event), عدم);
	g_signal_connect(کلیل_ctrl, "key-released",
			 G_CALLBACK(کلیل_release_event), عدم);
	gtk_widget_add_controller(gui.drawarea, کلیل_ctrl);
#ifdef FEAT_XIM
	xim_init();
#endif
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), عدم);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), عدم);
	gtk_widget_add_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), عدم);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), عدم);
	gtk_widget_add_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), عدم);
	gtk_widget_add_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_in_event), عدم);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_out_event), عدم);
	gtk_widget_add_controller(gui.drawarea, focus);
    }

#ifdef FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_target = gtk_drop_target_async_new(عدم,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_set_dnd_targets();
	g_signal_connect(gui.drop_target, "drop",
			 G_CALLBACK(drop_cb), عدم);
	gtk_widget_add_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_target));
    }
#endif

    gui.border_offset = gui.border_width;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_pointer = gdk_cursor_new_from_name("none", عدم);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	إذا (primary != عدم)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_star);
	إذا (regular != عدم)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_star);

    أرجع OK;
}

    فراغ
gui_mch_new_colors(فراغ)
{
    إذا (gui.drawarea != عدم && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open(فراغ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_width;
    guint pixel_height;

    إذا (gui.geom != عدم)
    {
	صحيح		mask;
	unsigned صحيح	w, h;

	mask = vim_parse_geometry((حرف *)gui.geom, &w, &h);

	إذا (mask & Width_بەها)
	    Columns = w;
	إذا (mask & Height_بەها)
	{
	    إذا (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	VIM_CLEAR(gui.geom);
    }
    وإلا
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	إذا (Columns > 80)
	    Columns = 80;
	إذا (Rows > 24)
	    Rows = 24;
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    pixel_width  += get_menu_tool_width();
    pixel_height += get_menu_tool_height();

    إذا (foreground_argument != عدم)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    إذا (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    إذا (background_argument != عدم)
	bg_pixel = gui_get_color((char_u *)background_argument);
    إذا (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    إذا (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    وإلا
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    set_normal_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), عدم);
    // Resize is handled by GtkForm's size_allocate callback.

    set_form_size((صحيح)pixel_width - get_menu_tool_width(),
	    (صحيح)pixel_height - get_menu_tool_height());

    gtk_window_present(GTK_WINDOW(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    أرجع OK;
}

    فراغ
gui_mch_exit(صحيح rc UNUSED)
{
    إذا (gui.mainwin != عدم)
    {
#ifdef FEAT_GUI_TABLINE
	// Must unparent popover menu for tabline or we will get warning
	gtk_widget_unparent(g_object_get_data(G_OBJECT(gui.tabline), "menu"));
#endif
#ifdef FEAT_BEVAL_GUI
	// Make sure to destroy popover used for balloon eval, or we will get a
	// warning from GTK that the draw area still has children left.
	gui_mch_destroy_beval_area(balloonEval);
#endif
#ifdef FEAT_MENU
	// Make sure to unparent any popover menus
	{
	    vimmenu_T *menu;

	    FOR_ALL_MENUS(menu)
	    {
		إذا ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != عدم)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#endif
	gtk_window_destroy(GTK_WINDOW(gui.mainwin));
    }
}

    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    أرجع FAIL;
}

    فراغ
gui_mch_set_winpos(صحيح x UNUSED, صحيح y UNUSED)
{
    // GTK4/Wayland: window positioning not available
}

    صحيح
gui_mch_maximized(فراغ)
{
    أرجع gtk_window_is_maximized(GTK_WINDOW(gui.mainwin));
}

    فراغ
gui_mch_unmaximize(فراغ)
{
    إذا (gui.mainwin != عدم)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

    فراغ
gui_mch_set_fullscreen(صحيح flag)
{
    إذا (gui.mainwin == عدم)
	أرجع;
    إذا (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    وإلا
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}

    ساكن فراغ
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec UNUSED, gpointer user_data UNUSED)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    إذا (gtk_window_is_fullscreen(GTK_WINDOW(obj)))
	gui_focus_change(TRUE);
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Recalculate Rows and Columns based on the current window size.
 *
 * NOTE: gui_set_shellsize() calls this when GO_KEEPWINSIZE ('k') is in
 * 'guioptions', even when the font hasn't actually changed (e.g. just setting
 * "guioptions=k" triggers it via gui_init_which_components()).  This is
 * arguably a design problem in the common code, but we must not call
 * gui_set_shellsize() back from here or it will cause infinite recursion and
 * crash.  Use gui_resize_shell() to recalculate Rows/Columns from the current
 * window size instead.
 */
    فراغ
gui_mch_newfont(فراغ)
{
    صحيح w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_width(gui.formwin);
    h = gtk_widget_get_height(gui.formwin);
    gui_resize_shell(w, h);
}

    فراغ
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    إذا (title != عدم && gui.mainwin != عدم)
	gtk_window_set_title(GTK_WINDOW(gui.mainwin), (ثابت حرف *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    فراغ
gui_gtk_init_decor_height(فراغ)
{
    صحيح h = gtk_widget_get_height(gui.mainwin);

    إذا (h == 0)
	أرجع;

    h -= get_menu_tool_height();
    h -= gtk_widget_get_height(gui.formwin);

    gui.decor_height = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    ساكن فراغ
set_form_size(صحيح width, صحيح height)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    إذا (gtk_widget_get_width(gui.formwin) == width
	    && gtk_widget_get_height(gui.formwin) == height)
	أرجع;

    // Remember the size the form widget is supposed to get. An allocation
    // that arrives before the compositor has answered this request still has
    // the previous size and must not be used.
    gui.pending_form_w = width;
    gui.pending_form_h = height;
    gui.pending_form_skip = 1;

    // GtkWindow default size also includes client side decorations, so must
    // include it also.  It also keeps the natural width of the toolbar from
    // deciding the width.
    gtk_window_set_default_size(GTK_WINDOW(gui.mainwin),
	    width + get_menu_tool_width(),
	    height + get_menu_tool_height() + gui.decor_height);

    // The window drops a request made while it is being presented, and
    // "gui.decor_height" is not known before that.  A size request on the form
    // widget is honoured then; it is dropped again once the size was given.
    gtk_widget_set_size_request(gui.formwin, width, height);
}

    فراغ
gui_mch_set_shellsize(صحيح width, صحيح height,
	صحيح min_width UNUSED, صحيح min_height UNUSED,
	صحيح base_width UNUSED, صحيح base_height UNUSED,
	صحيح direction UNUSED)
{
    set_form_size(width, height);

    gui_mch_update();
}

    فراغ
gui_mch_get_screen_dimensions(صحيح *screen_w, صحيح *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    إذا (surface != عدم)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	إذا (monitor != عدم)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.width;
	    *screen_h = geom.height;
	    أرجع;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ifdef FEAT_MENU
    فراغ
gui_mch_enable_menu(صحيح showit)
{
    إذا (gui.menubar != عدم)
    {
	gtk_widget_set_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	إذا (gui.drawarea != عدم)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

#ifdef FEAT_TOOLBAR
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    إذا (gui.toolbar != عدم)
    {
	gtk_widget_set_visible(gui.toolbar, showit);
	إذا (showit)
	    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	إذا (gui.drawarea != عدم)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

    فراغ
gui_mch_set_dark_theme(صحيح dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    إذا (settings != عدم)
	g_object_set(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, عدم);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    صحيح
gui_mch_adjust_charheight(فراغ)
{
    PangoFontMetrics *metrics;
    صحيح ascent;
    صحيح descent;

    metrics = pango_context_get_metrics(gui.text_context, gui.norm_font,
			    pango_context_get_language(gui.text_context));
    ascent = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);
    pango_font_metrics_unref(metrics);

    gui.char_height = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_height = MAX(gui.char_height, gui.char_ascent + 1);

    أرجع OK;
}

تعريفنوع بنية {
    PangoFontDescription    *result;
    gboolean		    done;
} FontDialogData;

    ساكن فراغ
font_dialog_finish_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FontDialogData *fdd = (FontDialogData *)data;
    fdd->result = gtk_font_dialog_choose_font_finish(
		    GTK_FONT_DIALOG(source), res, عدم);
    fdd->done = TRUE;
}

    ساكن gboolean
font_filter(gpointer item, gpointer data UNUSED)
{
    إذا (PANGO_IS_FONT_FAMILY(item))
	أرجع pango_font_family_is_monospace(PANGO_FONT_FAMILY(item));
    إذا (PANGO_IS_FONT_FACE(item))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(item));
	إذا (family != عدم)
	    أرجع pango_font_family_is_monospace(family);
    }
    أرجع TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = عدم;
    char_u		*fontname = عدم;
    FontDialogData	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_set_modal(dlg, TRUE);
    gtk_font_dialog_set_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, عدم, عدم)));

    إذا (oldval != عدم && oldval[0] != NUL)
    {
	char_u *oldname;

	إذا (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, عدم);
	وإلا
	    oldname = oldval;

	إذا (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    إذا (p != عدم)
	    {
		STRCPY(p + STRLEN(p), " 10");
		إذا (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((ثابت حرف *)oldname);
	إذا (oldname != oldval)
	    vim_free(oldname);
    }
    وإلا
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.result = عدم;
    fdd.done = FALSE;

    gtk_font_dialog_choose_font(dlg, GTK_WINDOW(gui.mainwin),
	    initial, عدم, font_dialog_finish_cb, &fdd);

    طالما (!fdd.done)
	g_main_context_iteration(عدم, TRUE);

    إذا (fdd.result != عدم)
    {
	حرف *name = pango_font_description_to_string(fdd.result);
	إذا (name != عدم)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    إذا (p != عدم && تێخستن_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&تێخستن_conv, p, عدم);
		vim_free(p);
	    }
	    وإلا
		fontname = p;
	}
	pango_font_description_free(fdd.result);
    }

    إذا (initial != عدم)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    أرجع fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    ساكن فراغ
ascii_glyph_table_init(فراغ)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    صحيح		    i;

    إذا (gui.ascii_glyphs != عدم)
	pango_glyph_string_free(gui.ascii_glyphs);
    إذا (gui.ascii_font != عدم)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = عدم;
    gui.ascii_font   = عدم;

    لكل (i = 0; i < 128; ++i)
    {
	إذا (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	وإلا
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (ثابت حرف *)ascii_chars,
			      0, حجم(ascii_chars), attr_list, عدم);

    إذا (item_list != عدم && item_list->next == عدم)
    {
	PangoItem   *item;
	صحيح	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت حرف *)ascii_chars, حجم(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	إذا (gui.ascii_glyphs->ژمارە_glyphs == (صحيح)حجم(ascii_chars))
	{
	    لكل (i = 0; i < gui.ascii_glyphs->ژمارە_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_offset += MAX(0, width - geom->width) / 2;
		geom->width = width;
	    }
	}
	وإلا
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = عدم;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = عدم;
	}
    }

    g_list_foreach(item_list, (GFunc)(فراغ *)&pango_item_free, عدم);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    ساكن فراغ
get_styled_font_variants(فراغ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);
    إذا (plain_font == عدم)
	أرجع;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    إذا (bold_font != عدم)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    صحيح
gui_mch_init_font(char_u *font_name, صحيح fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحيح			    width;

    إذا (font_name == عدم)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    إذا (font_desc == عدم)
	أرجع FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, عدم);
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    إذا (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	صحيح im_row = gui.row < screen_Rows ? gui.row : screen_Rows - 1;
	صحيح im_col = gui.col < screen_Columns ? gui.col : screen_Columns - 1;
	im_set_position(im_row, im_col);
    }
    أرجع OK;
}

    GuiFont
gui_mch_get_font(char_u *name, صحيح report_error)
{
    PangoFontDescription *font;

    إذا (name == عدم)
	أرجع عدم;

    font = pango_font_description_from_string((ثابت حرف *)name);
    إذا (font == عدم)
    {
	إذا (report_error)
	    semsg(_(e_unknown_font_str), name);
	أرجع عدم;
    }

    // Ensure a size is set
    إذا (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    أرجع font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    إذا (font != NOFONT)
    {
	حرف *desc = pango_font_description_to_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	أرجع ret;
    }
    أرجع عدم;
}

    فراغ
gui_mch_free_font(GuiFont font)
{
    إذا (font != NOFONT)
	pango_font_description_free(font);
}

    فراغ
gui_mch_expand_font(
    optexpand_T	*args,
    فراغ	*param,
    صحيح		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = عدم;
    صحيح			n_families = 0;
    صحيح			wide = *(صحيح *)param;

    إذا (args->oe_include_orig_val && *args->oe_opt_بەها == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	إذا (add_match((char_u *)DEFAULT_FONT) != OK)
	    أرجع;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    لكل (صحيح i = 0; i < n_families; i++)
    {
	إذا (!wide && !pango_font_family_is_monospace(font_families[i]))
	    استمر;

	ثابت حرف *fam_name = pango_font_family_get_name(font_families[i]);
	إذا (تێخستن_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&تێخستن_conv,
		    (char_u *)fam_name, عدم);
	    إذا (buf != عدم)
	    {
		إذا (add_match(buf) != OK)
		{
		    vim_free(buf);
		    اكسر;
		}
		vim_free(buf);
	    }
	    وإلا
		اكسر;
	}
	وإلا
	{
	    إذا (add_match((char_u *)fam_name) != OK)
		اكسر;
	}
    }

    g_free(font_families);
}

/*
 * ============================================================
 * Color handling
 * ============================================================
 */

    guicolor_T
gui_mch_get_color(char_u *name)
{
    إذا (!gui.in_use)
	أرجع INVALCOLOR;

    إذا (name != عدم)
	أرجع gui_get_color_cmn(name);

    أرجع INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(صحيح r, صحيح g, صحيح b)
{
    أرجع gui_get_rgb_color_cmn(r, g, b);
}

    ساكن GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    أرجع rgba;
}

    فراغ
gui_mch_set_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_to_rgba(color);
}

    فراغ
gui_mch_set_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_to_rgba(color);
}

    فراغ
gui_mch_set_sp_color(guicolor_T color)
{
    *gui.spcolor = color_to_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    أرجع pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    فراغ
gui_gtk4_update_size(فراغ)
{
    vim_draw_area_set_size(VIM_DRAW_AREA(gui.drawarea),
	    gui.ژمارە_rows, gui.ژمارە_cols);
}

#ifdef FEAT_NETBEANS_INTG
    فراغ
gui_gtk4_add_multisign(
	cairo_surface_t *surf,
	صحيح		row,
	صحيح		col,
	صحيح		width,
	صحيح		height)
{
    vim_draw_area_add_multisign(VIM_DRAW_AREA(gui.drawarea), surf, row, col,
	    width, height);
}
#endif

    فراغ
gui_mch_clear_block(صحيح row1, صحيح col1, صحيح row2, صحيح col2)
{
    vim_draw_area_clear_block(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    إذا (gui.drawarea != عدم)
	gtk_widget_queue_draw(gui.drawarea);
}

    فراغ
gui_mch_clear_all(فراغ)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    إذا (gui.drawarea != عدم)
	gtk_widget_queue_draw(gui.drawarea);
}

    فراغ
gui_mch_delete_هێڵەکان(صحيح row, صحيح ژمارە_هێڵەکان)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row, row + ژمارە_هێڵەکان, gui.scroll_region_bot,
	    gui.scroll_region_چەپ, gui.scroll_region_ڕاست);
    gui_clear_block(
	    gui.scroll_region_bot - ژمارە_هێڵەکان + 1, gui.scroll_region_چەپ,
	    gui.scroll_region_bot, gui.scroll_region_ڕاست);

    gtk_widget_queue_draw(gui.drawarea);
}

    فراغ
gui_mch_زیادکردن_هێڵەکان(صحيح row, صحيح ژمارە_هێڵەکان)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row + ژمارە_هێڵەکان, row, gui.scroll_region_bot - ژمارە_هێڵەکان,
	    gui.scroll_region_چەپ, gui.scroll_region_ڕاست);
    gui_clear_block(
	    row, gui.scroll_region_چەپ,
	    row + ژمارە_هێڵەکان - 1, gui.scroll_region_ڕاست);

    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    فراغ
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, صحيح w, صحيح h)
{
    gui_mch_set_bg_color(bg);
    gui_mch_set_fg_color(fg);
    vim_draw_area_set_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_queue_draw(gui.drawarea);
}

    فراغ
gui_mch_flash(صحيح msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (صحيح)Rows - 1, (صحيح)Columns - 1);
    gui_mch_flush();
    ui_delay((long)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (صحيح)Rows - 1, (صحيح)Columns - 1);
}

    فراغ
gui_mch_invert_rectangle(صحيح r, صحيح c, صحيح nr, صحيح nc)
{
    vim_draw_area_invert_block(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    ساكن gboolean
کلیل_press_event(Gtk_Event_Controller_کلیل *controller UNUSED,
	guint کلیل_sym, guint keycode UNUSED,
	GdkModifierType state, gpointer data UNUSED)
{
    char_u	string[32], string2[32];
    صحيح		len;
    صحيح		i;
    صحيح		modifiers;
    صحيح		کلیل;
    char_u	*s, *d;

#ifdef FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    إذا (xic != عدم)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	إذا (event != عدم && gtk_im_context_filter_keypress(xic, event))
	    أرجع TRUE;
    }
#endif

#ifdef FEAT_MENU
    إذا (کلیل_sym == GDK_کلیل_F10 && gui.menubar != عدم)
    {
	ساكن char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	إذا (check_map(k10, State, FALSE, TRUE, FALSE, عدم, عدم) == عدم)
	{
	    show_menubar_popover();
	    أرجع TRUE;
	}
    }
#endif

    len = keyval_to_string(کلیل_sym, string2);

    إذا (len > 1 && تێخستن_conv.vc_type != CONV_NONE)
	len = convert_تێخستن(string2, len, حجم(string2));

    s = string2;
    d = string;
    لكل (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	إذا (d[-1] == CSI && d + 2 < string + حجم(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (صحيح)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    إذا (کلیل_sym == GDK_کلیل_ISO_چەپ_Tab)
    {
	کلیل_sym = GDK_کلیل_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    إذا ((state & GDK_ALT_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	أرجع FALSE;
#endif

    // Check for special keys
    إذا (len == 0 || len == 1)
    {
	لكل (i = 0; special_کلیلەکان[i].کلیل_sym != 0; i++)
	{
	    إذا (special_کلیلەکان[i].کلیل_sym == کلیل_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیلەکان[i].code0;
		string[2] = special_کلیلەکان[i].code1;
		len = -3;
		اكسر;
	    }
	}
    }

    إذا (len == 0)
	أرجع TRUE;

    إذا (len == -3)
	کلیل = TO_SPECIAL(string[1], string[2]);
    وإلا
    {
	string[len] = NUL;
	کلیل = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    کلیل = simplify_کلیل(کلیل, &modifiers);
    إذا (کلیل == CSI)
	کلیل = K_CSI;
    إذا (IS_SPECIAL(کلیل))
    {
	string[0] = CSI;
	string[1] = K_SECOND(کلیل);
	string[2] = K_THIRD(کلیل);
	len = 3;
    }
    وإلا
    {
	کلیل = may_adjust_کلیل_for_ctrl(modifiers, کلیل);
	modifiers = may_remove_shift_modifier(modifiers, کلیل);
	len = mb_char2bytes(کلیل, string);
    }

    إذا (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_تێخستن_buf(string2, 3);
    }

    {
	صحيح int_ch = check_for_interrupt(کلیل, modifiers);
	إذا (int_ch != NUL)
	{
	    trash_تێخستن_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_تێخستن_buf(string, len);

    إذا (p_mh)
	gui_mch_mousehide(TRUE);

    أرجع TRUE;
}

    ساكن فراغ
کلیل_release_event(Gtk_Event_Controller_کلیل *controller UNUSED,
	guint keyval UNUSED, guint keycode UNUSED,
	GdkModifierType state UNUSED, gpointer data UNUSED)
{
}

ساكن صحيح mouse_timed_out = TRUE;
ساكن guint mouse_click_timer = 0;

    ساكن timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    *(صحيح *)data = TRUE;
    أرجع FALSE;
}

    ساكن صحيح
modifiers_gdk2mouse(guint state)
{
    صحيح modifiers = 0;

    إذا (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    إذا (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    إذا (state & GDK_ALT_MASK)
	modifiers |= MOUSE_ALT;

    أرجع modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#define ANY_BUTTON_MASK (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK \
	| GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
ساكن صحيح mouse_pressed_button = -1;

ساكن guint motion_repeat_timer = 0;
ساكن gboolean motion_repeat_offset = FALSE;
ساكن صحيح mouse_press_x = -1, mouse_press_y = -1;

    ساكن فراغ
button_press_event(GtkGestureClick *gesture, صحيح n_press UNUSED,
	مزدوج x, مزدوج y, gpointer data UNUSED)
{
    صحيح button;
    صحيح repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    إذا (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_out);

    بدل (btn)
    {
	حالة 1: button = MOUSE_چەپ; اكسر;
	حالة 2: button = MOUSE_ناوەڕاست; اكسر;
	حالة 3: button = MOUSE_ڕاست; اكسر;
	حالة 8: button = MOUSE_X1; اكسر;
	حالة 9: button = MOUSE_X2; اكسر;
	افتراضي: أرجع;
    }

    mouse_press_x = (صحيح)x;
    mouse_press_y = (صحيح)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (صحيح)x, (صحيح)y, repeated_click, vim_modifiers);
}

    ساكن فراغ
button_release_event(GtkGestureClick *gesture, صحيح n_press UNUSED,
	مزدوج x, مزدوج y, gpointer data UNUSED)
{
    صحيح vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    إذا (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (صحيح)x, (صحيح)y, FALSE, vim_modifiers);
}

ساكن مزدوج prev_mouse_x = -1.0;
ساكن مزدوج prev_mouse_y = -1.0;

ساكن GdkModifierType cur_state = 0;

    ساكن timeout_cb_type
mouse_repeat_timer_cb(gpointer data UNUSED)
{
    صحيح x, y;

    إذا (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	أرجع G_SOURCE_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    إذا (vim_used_in_تێخستن_buf() > 10)
	أرجع G_SOURCE_CONTINUE;

    x = (صحيح)prev_mouse_x;
    y = (صحيح)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    إذا (motion_repeat_offset)
	x += gui.char_width;
    motion_repeat_offset = !motion_repeat_offset;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    أرجع G_SOURCE_CONTINUE;
}

    ساكن فراغ
motion_notify_event(GtkEventControllerMotion *controller UNUSED,
	مزدوج x, مزدوج y, gpointer data UNUSED)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    إذا (mouse_pressed_button >= 0 && event != عدم
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_MASK))
    {
	إذا (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    إذا (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	صحيح		w, h;

	إذا (event != عدم)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (صحيح)x, (صحيح)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_width(gui.drawarea);
	h = gtk_widget_get_height(gui.drawarea);

	إذا (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	إذا (x < 0 || y < 0 || x >= w || y >= h)
	{
	    صحيح	dx, dy;
	    صحيح offshoot;
	    صحيح	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    إذا (offshoot > 127)
		delay = 5;
	    وإلا
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_add(delay,
		    mouse_repeat_timer_cb, عدم);
	}
	وإلا
	    motion_repeat_timer = 0;
    }
    وإلا
	gui_mouse_moved((صحيح)x, (صحيح)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    إذا (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    ساكن فراغ
enter_notify_event(GtkEventControllerMotion *controller UNUSED,
	مزدوج x, مزدوج y, gpointer data UNUSED)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    ساكن gboolean
scroll_event(GtkEventControllerScroll *controller UNUSED,
	مزدوج dx UNUSED, مزدوج dy, gpointer data UNUSED)
{
    صحيح button;
    int_u vim_modifiers;
    صحيح x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    إذا (event == عدم)
	أرجع FALSE;
    state = gdk_event_get_modifier_state(event);

    إذا (dy < 0)
	button = MOUSE_4;	// scroll up
    وإلا إذا (dy > 0)
	button = MOUSE_5;	// scroll down
    وإلا إذا (dx < 0)
	button = MOUSE_7;	// scroll left
    وإلا إذا (dx > 0)
	button = MOUSE_6;	// scroll right
    وإلا
	أرجع FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    أرجع TRUE;
}

    ساكن فراغ
focus_in_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    إذا (gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	إذا (blink_state == BLINK_NONE)
	    gui_mch_start_blink();
    }
}

    ساكن فراغ
focus_out_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    إذا (!gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	إذا (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#إذا defined(FEAT_IMAGE)
    ساكن فراغ
scale_factor_cb(GdkSurface  *surface,
	GParamSpec	    *pspec UNUSED,
	فراغ		    *udata UNUSED)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#endif

    ساكن فراغ
drawarea_realize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#إذا defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_factor_cb), عدم);
    redraw_all_later(UPD_VALID);
#endif
    gui_mch_new_colors();
}

    ساكن فراغ
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ifdef FEAT_XIM
    im_shutdown();
#endif
}

تعريفنوع تعداد
{
    READ_DATA_CLIP,	    // Clipboard data
    READ_DATA_DROP,	    // URI or text DND data
    READ_DATA_DROP_DATA	    // HTML DND data
} ReadDataType;

#define READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
تعريفنوع بنية
{
    ReadDataType    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    حرف	    *buf; // NULL when not needed
} ReadData;

#ifdef FEAT_DND
تعريفنوع بنية
{
    ReadData	rd;
    GdkDrop	*drop;
    صحيح		x;
    صحيح		y;
} DropReadData;
#endif

تعريفنوع بنية
{
    ReadData	rd;
    Clipboard_T *cbd;
    حرف	*mime_type;
} ClipReadData;

    ساكن ReadData *
read_data_new(ReadDataType type, size_t sz)
{
    ReadData *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    إذا (type == READ_DATA_CLIP || type == READ_DATA_DROP_DATA)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    أرجع rd;
}

    ساكن فراغ
read_data_free(ReadData *rd)
{
    g_cancellable_cancel(rd->cancel);

    إذا (rd->type == READ_DATA_CLIP)
	g_free(((ClipReadData *)rd)->mime_type);
    وإلا إذا (rd->type == READ_DATA_DROP || rd->type == READ_DATA_DROP_DATA)
	g_object_unref(((DropReadData *)rd)->drop);
    g_object_unref(rd->cancel);
    إذا (rd->arr != عدم)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ifdef FEAT_DND
ساكن فراغ drop_read_text(GdkDrop *drop, char_u *text);
#endif

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    ساكن فراغ
read_data_تێخستن_cb(
	GInputStream	*stream,
	GAsyncResult	*result,
	ReadData	*rd)
{
    ssize_t r = g_تێخستن_stream_read_finish(stream, result, عدم);

    إذا (r == -1)
    {
	// Error occurred
	DropReadData *drd = (DropReadData *)rd;

	إذا (rd->type == READ_DATA_DROP_DATA)
	    gdk_drop_finish(drd->drop, 0);
    }
    وإلا إذا (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	إذا (rd->type == READ_DATA_CLIP)
	{
	    ClipReadData    *crd = (ClipReadData *)rd;
	    long	    len;
	    char_u	    *actual, *final;
	    صحيح		    motion_type = MAUTO;
	    char_u	    *tofree = عدم;

	    len = (long)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = عدم;

	    إذا (clip_convert_data(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ifdef FEAT_DND
	وإلا إذا (rd->type == READ_DATA_DROP_DATA)
	{
	    DropReadData    *drd = (DropReadData *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ثابت uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = عدم;
	    drop_read_text(drd->drop, str);
	    g_free(str);
	    gdk_drop_finish(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#endif
    }
    وإلا
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ثابت uint8_t *)rd->buf, r);
	g_تێخستن_stream_read_async(stream, rd->buf, READDATA_BUFSIZE,
		G_PRIORITY_بەرز,
		rd->cancel, (GAsyncReadyCallback)read_data_تێخستن_cb, rd);
	أرجع;
    }

    read_data_free(rd);
    g_object_unref(stream);
}

#ifdef FEAT_DND
/*
 * Set up for receiving DND items.
 */
    فراغ
gui_gtk_set_dnd_targets(فراغ)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_add_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_add_gtype(builder, G_TYPE_STRING);
    إذا (clip_html)
	gdk_content_formats_builder_add_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_to_formats(builder);
    gtk_drop_target_async_set_formats(gui.drop_target, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    ساكن فراغ
drop_read_text(GdkDrop *drop UNUSED, char_u *text)
{
    GdkModifierType state;
    char_u	    dropkey[6] = {
	CSI,
	KS_MODIFIER,
	0,
	CSI,
	KS_EXTRA,
	(char_u)KE_DROP
    };

    إذا (text == عدم || *text == NUL || !query_pointer_pos(عدم, عدم, &state))
	أرجع;

    dnd_yank_drag_data((char_u *)text, (long)STRLEN(text));

    dropkey[2] = modifiers_gdk2vim(state);
    إذا (dropkey[2] != 0)
	add_to_تێخستن_buf(dropkey, (صحيح)حجم(dropkey));
    وإلا
	add_to_تێخستن_buf(dropkey + 3, (صحيح)(حجم(dropkey) - 3));
}

    ساكن فراغ
drop_read_بەها_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    ثابت GValue    *بەها = gdk_drop_read_بەها_finish(drop, result, عدم);
    gboolean	    success = FALSE;

    إذا (بەها == عدم)
	اذهب exit;

    إذا (G_بەها_HOLDS(بەها, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_بەها_get_boxed(بەها);
	صحيح	nfiles = g_slist_length(files);
	char_u	**fnames;
	صحيح	i;

	إذا (nfiles <= 0)
	    اذهب exit;

	fnames = ALLOC_MULT(char_u *, nfiles);
	إذا (fnames == عدم)
	    اذهب exit;

	i = 0;
	لكل (GSList *l = files; l != عدم; l = l->next)
	{
	    GFile *file = l->data;
	    حرف *path = g_file_get_path(file);
	    إذا (path != عدم)
		fnames[i++] = vim_strsave((char_u *)path);
	    g_free(path);
	}
	nfiles = i;

	إذا (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	وإلا
	    vim_free(fnames);
	success = TRUE;
    }
    وإلا إذا (G_بەها_HOLDS(بەها, G_TYPE_STRING))
    {
	ثابت حرف *text = g_بەها_get_string(بەها);

	إذا (text == عدم || *text == NUL)
	    اذهب exit;

	drop_read_text(drop, (char_u *)text);
	success = TRUE;
    }

exit:
    gdk_drop_finish(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_data_free(&drd->rd);
}

    ساكن فراغ
drop_read_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    GInputStream    *in_stream;
    ثابت حرف	    *m;

    in_stream = gdk_drop_read_finish(drop, result, &m, عدم);
    إذا (in_stream == عدم || STRCMP(m, "text/html") != 0)
    {
	read_data_free(&drd->rd);
	gdk_drop_finish(drop, 0);
	أرجع;
    }

    أكد(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_تێخستن_stream_read_async(in_stream, drd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_بەرز,
	    drd->rd.cancel, (GAsyncReadyCallback)read_data_تێخستن_cb, drd);
}

// GCancellable of current DND operation, else NULL.
ساكن GCancellable *dnd_cancel = عدم;
ساكن guint dnd_timeout_id = 0;

    ساكن gboolean
drop_timeout_cb(فراغ *udata UNUSED)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    أرجع G_SOURCE_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    ساكن gboolean
drop_cb(
	GtkDropTargetAsync  *target UNUSED,
	GdkDrop		    *drop,
	مزدوج		    x,
	مزدوج		    y,
	فراغ		    *udata UNUSED)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    DropReadData	*drd = عدم;

    إذا (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, حجم(*drd));
	gdk_drop_read_بەها_async(drop, GDK_TYPE_FILE_LIST, G_PRIORITY_بەرز,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_بەها_cb, drd);
    }
    وإلا إذا (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	ساكن ثابت حرف *m[] = {"text/html", عدم};

	drd = (DropReadData *)read_data_new(READ_DATA_DROP_DATA, حجم(*drd));
	gdk_drop_read_async(drop, m, G_PRIORITY_بەرز,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    وإلا إذا (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, حجم(*drd));
	gdk_drop_read_بەها_async(drop, G_TYPE_STRING, G_PRIORITY_بەرز,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_بەها_cb, drd);
    }

    إذا (dnd_cancel != عدم)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    إذا (drd != عدم)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (صحيح)x;
	drd->y = (صحيح)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_add(3000, drop_timeout_cb, عدم);

	أرجع TRUE;
    }
    أرجع FALSE;
}
#endif

    ساكن فراغ
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    gui.mainwin = عدم;
    gui.drawarea = عدم;
    إذا (!exiting)
	gui_shell_closed();
}

    ساكن gboolean
delete_event_cb(GtkWindow *window UNUSED, gpointer data UNUSED)
{
    gui_shell_closed();
    أرجع TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    ساكن timeout_cb_type
تێخستن_timer_cb(gpointer data)
{
    صحيح *timed_out = (صحيح *)data;

    *timed_out = TRUE;
    أرجع FALSE;	// don't call me again
}

    فراغ
gui_mch_update(فراغ)
{
    صحيح cnt = 0;

    طالما (g_main_context_pending(عدم) && !vim_is_تێخستن_buf_full()
								&& ++cnt < 100)
	g_main_context_iteration(عدم, TRUE);
}

#ifdef FEAT_JOB_CHANNEL
    ساكن timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    أرجع TRUE; // Keep repeating
}
#endif

    صحيح
gui_mch_wait_for_chars(long wtime)
{
    صحيح		focus;
    guint	timer;
    ساكن صحيح	timed_out;
    صحيح		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    إذا (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   تێخستن_timer_cb, &timed_out);
    وإلا
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    إذا (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, عدم);
#endif

    focus = gui.in_focus;

    افعل
    {
	// Stop or start blinking when focus changes
	إذا (gui.in_focus != focus)
	{
	    إذا (gui.in_focus)
		gui_mch_start_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_add_timer)
	    اذهب theend;
# endif
#endif

	إذا (gui.mainwin == عدم)
	    اذهب theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	إذا (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    اذهب theend;
	}

	إذا (!تێخستن_available())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(عدم, TRUE);
	    --gtk4_main_loop_level;
	}

	إذا (تێخستن_available())
	{
	    retval = OK;
	    اذهب theend;
	}
    } طالما (wtime < 0 || !timed_out);

    gui_mch_update();

theend:
    إذا (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    إذا (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    أرجع retval;
}

    فراغ
gui_mch_flush(فراغ)
{
    // Ensure the offscreen surface content gets painted to the widget.
    إذا (gui.drawarea != عدم)
	gtk_widget_queue_draw(gui.drawarea);
    إذا (gui.mainwin != عدم && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    فراغ
gui_mch_beep(فراغ)
{
    GdkDisplay *display;

    إذا (gui.mainwin != عدم && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	إذا (display != عدم)
	    gdk_display_beep(display);
    }
}

    فراغ *
gui_mch_get_display(فراغ)
{
    إذا (gui.mainwin != عدم && gtk_widget_get_display(gui.mainwin))
	أرجع gtk_widget_get_display(gui.mainwin);
    أرجع عدم;
}

    فراغ
gui_mch_iconify(فراغ)
{
    gtk_window_minimize(GTK_WINDOW(gui.mainwin));
}

    فراغ
gui_mch_set_foreground(فراغ)
{
    gtk_window_present(GTK_WINDOW(gui.mainwin));
}

    ساكن صحيح
query_pointer_pos(صحيح *x, صحيح *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *pointer;
    مزدوج	     sx, sy, nx, ny;
    graphene_point_t src, dst;

    إذا (gui.drawarea == عدم)
	أرجع FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    إذا (native == عدم)
	أرجع FALSE;
    surface = gtk_native_get_surface(native);
    إذا (surface == عدم)
	أرجع FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    إذا (display == عدم)
	أرجع FALSE;
    seat = gdk_display_get_default_seat(display);
    إذا (seat == عدم)
	أرجع FALSE;
    pointer = gdk_seat_get_pointer(seat);
    إذا (pointer == عدم)
	أرجع FALSE;

    إذا (!gdk_surface_get_device_position(surface, pointer, &sx, &sy, state))
	أرجع FALSE;

    إذا (x != عدم && y != عدم)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (عائم)(sx - nx);
	src.y = (عائم)(sy - ny);
	إذا (!gtk_widget_compute_point(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    أرجع FALSE;

	*x = (صحيح)dst.x;
	*y = (صحيح)dst.y;
    }
    أرجع TRUE;
}

    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
{
    إذا (!query_pointer_pos(x, y, عدم))
    {
	*x = 0;
	*y = 0;
    }
}

    فراغ
gui_mch_setmouse(صحيح x UNUSED, صحيح y UNUSED)
{
    // GTK4/Wayland: cannot warp pointer
}

    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (gui.pointer_hidden == hide)
	أرجع;

    gui.pointer_hidden = hide;
    إذا (gui.blank_pointer != عدم)
    {
	إذا (hide)
	    gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
	وإلا
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#وإلا
	    gtk_widget_set_cursor(gui.drawarea, عدم);
#endif
    }
}

    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_کلیلەکان[i].کلیل_sym != 0; i++)
	إذا (name[0] == special_کلیلەکان[i].code0
		&& name[1] == special_کلیلەکان[i].code1)
	    أرجع OK;
    أرجع FAIL;
}

    فراغ
gui_mch_forked(فراغ)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    ساكن فراغ
gui_gtk4_update_scrollbar_size(فراغ)
{
    win_T	*wp;
    صحيح		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_چەپ].id;

	إذا (sbar == عدم || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_چەپ]
		    && wp->w_scrollbars[SBAR_ڕاست].id != عدم))
	    sbar = wp->w_scrollbars[SBAR_ڕاست].id;

	إذا (sbar != عدم && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    صحيح		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_size(sbar, &min, &nat);
	    sw = MAX(min.width, nat.width);
	    إذا (sw > 0)
	    {
		w = sw;
		اكسر;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    إذا (sbar != عدم && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	صحيح		    sh;

	gtk_widget_get_preferred_size(sbar, &min, &nat);
	sh = MAX(min.height, nat.height);

	إذا (sh > 0)
	    h = sh;
    }

    إذا (w != -1)
	gui.scrollbar_width = w;
    إذا (h != -1)
	gui.scrollbar_height = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    إذا (sb->id != عدم)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_set_visible(sb->id, flag);

	إذا (!was_visible && flag)
	    gui_gtk4_update_scrollbar_size();
    }
}

#إذا defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    فراغ
gui_mch_menu_grey(vimmenu_T *menu, صحيح grey)
{
    إذا (menu->id == عدم)
	أرجع;
    gtk_widget_set_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    فراغ
gui_mch_menu_hidden(vimmenu_T *menu, صحيح hidden)
{
    إذا (menu->id == عدم)
	أرجع;
    gtk_widget_set_visible(menu->id, !hidden);
    gui_mch_update();
}

    فراغ
gui_mch_draw_menubar(فراغ)
{
    // Just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#endif

/*
 * ============================================================
 * Tabline
 * ============================================================
 */

#ifdef FEAT_GUI_TABLINE
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    إذا (gui.tabline == عدم)
	أرجع;
    gtk_widget_set_visible(gui.tabline, showit);
    gui_mch_update();
}

    صحيح
gui_mch_showing_tabline(فراغ)
{
    أرجع gui.tabline != عدم && gtk_widget_get_visible(gui.tabline);
}

ساكن صحيح ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
ساكن صحيح tabpage_hover = 0;

    ساكن فراغ
tabpage_enter_cb(
	GtkEventController  *controller UNUSED,
	مزدوج		    x UNUSED,
	مزدوج		    y UNUSED,
	فراغ		    *udata)
{
    tabpage_hover = GPOINTER_TO_INT(udata);
}

    ساكن فراغ
tabline_enter_cb(
	GtkEventController  *controller UNUSED,
	مزدوج		    x UNUSED,
	مزدوج		    y UNUSED,
	فراغ		    *udata UNUSED)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    ساكن فراغ
tabline_add_css(فراغ)
{
    GtkCssProvider  *provider;
    ساكن ثابت حرف *css =
	"notebook.vim-tabline > header {"
	"  background: none; border: none; padding: 6px 1px; }"
	"notebook.vim-tabline > header > tabs { margin: 0; }"
	"notebook.vim-tabline > header > tabs > tab {"
	"  min-height: 26px; padding: 4px; margin: 0 2px;"
	"  border: none; border-radius: 9px; outline: none;"
	"  box-shadow: none; background: none;"
	"  transition: background 150ms ease-in-out; }"
	"notebook.vim-tabline > header > tabs > tab:hover {"
	"  background-color: alpha(currentColor, 0.07); }"
	"notebook.vim-tabline > header > tabs > tab:checked {"
	"  background-color: alpha(currentColor, 0.10); }"
	"notebook.vim-tabline > header > tabs > tab:checked:hover {"
	"  background-color: alpha(currentColor, 0.13); }"
	"notebook.vim-tabline > header > tabs > tab:active {"
	"  background-color: alpha(currentColor, 0.16); }"
	// Separator between tabs, not before the first tab and not next to the
	// current or hovered tab.
	"notebook.vim-tabline > header > tabs > tab {"
	"  background-image: linear-gradient(alpha(currentColor, 0.15),"
	"    alpha(currentColor, 0.15));"
	"  background-size: 1px 50%; background-position: left center;"
	"  background-repeat: no-repeat; }"
	"notebook.vim-tabline > header > tabs > arrow + tab,"
	"notebook.vim-tabline > header > tabs > tab:first-child,"
	"notebook.vim-tabline > header > tabs > tab:checked,"
	"notebook.vim-tabline > header > tabs > tab:hover,"
	"notebook.vim-tabline > header > tabs > tab:checked + tab,"
	"notebook.vim-tabline > header > tabs > tab:hover + tab {"
	"  background-image: none; }"
	"notebook.vim-tabline.single-tab > header > tabs > tab {"
	"  background: none; }"
	"notebook.vim-tabline > header > tabs > tab button {"
	"  padding: 0; margin: 0; min-width: 24px; min-height: 24px;"
	"  border-radius: 99px; transition: opacity 150ms ease-in-out; }"
	"notebook.vim-tabline > header > tabs > tab:not(:checked):not(:hover)"
	"  button { opacity: 0; }"
	// The last tab page cannot be closed.
	"notebook.vim-tabline.single-tab > header > tabs > tab button {"
	"  opacity: 0; }"
	"notebook.vim-tabline > header > tabs > tab .vim-tab-spacer {"
	"  min-width: 24px; }";

    provider = gtk_css_provider_new();
# إذا GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
# وإلا
    gtk_css_provider_load_from_data(provider, css, -1);
# endif
    gtk_style_context_add_provider_for_display(
	    gtk_widget_get_display(gui.tabline), GTK_STYLE_PROVIDER(provider),
	    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

/*
 * Handle clicking the close button of a tab.
 */
    ساكن فراغ
tab_close_clicked_cb(GtkButton *button UNUSED, gpointer data)
{
    صحيح idx = gtk_notebook_page_ژمارە(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(data));

    إذا (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    فراغ
gui_mch_update_tabline(فراغ)
{
    GtkWidget	*page;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    صحيح		nr = 0;
    صحيح		tab_ژمارە;
    صحيح		curtabidx = 0;
    char_u	*labeltext;

    إذا (gui.tabline == عدم)
	أرجع;

    ignore_tabline_evt = TRUE;

    لكل (tp = first_tabpage; tp != عدم; tp = tp->tp_next, ++nr)
    {
	GtkNotebookPage	    *page_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	إذا (tp == curtab)
	    curtabidx = nr;

	tab_ژمارە = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	إذا (page == عدم)
	{
	    GtkWidget	*spacer;
	    GtkWidget	*close_button;

	    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_widget_set_visible(page, TRUE);
	    event_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_set_visible(event_box, TRUE);

	    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_add_css_class(spacer, "vim-tab-spacer");
	    gtk_box_append(GTK_BOX(event_box), spacer);

	    label = gtk_label_new("-Empty-");
	    gtk_widget_set_hexpand(label, TRUE);
	    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
	    gtk_box_append(GTK_BOX(event_box), label);
	    gtk_widget_set_visible(label, TRUE);
	    g_object_set_data(G_OBJECT(event_box), "label", label);

	    close_button = gtk_button_new_from_icon_name("window-close-symbolic");
	    gtk_widget_add_css_class(close_button, "flat");
	    gtk_widget_add_css_class(close_button, "circular");
	    gtk_widget_set_valign(close_button, GTK_ALIGN_CENTER);
	    gtk_widget_set_focus_on_click(close_button, FALSE);
	    gtk_widget_set_tooltip_text(close_button, _("Close Tab"));
	    g_signal_connect(close_button, "clicked",
		    G_CALLBACK(tab_close_clicked_cb), page);
	    gtk_box_append(GTK_BOX(event_box), close_button);

	    gtk_notebook_زیادکردن_page(GTK_NOTEBOOK(gui.tabline),
		    page, event_box, nr);
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page, TRUE);
	    g_object_set(gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page),
		    "tab-expand", TRUE, عدم);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	page_widget = gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_TO_POINTER(tab_ژمارە));
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(page_widget, "tab", &tab_widget, عدم);
	gtk_widget_add_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	label = g_object_get_data(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	إذا (label != عدم && GTK_IS_LABEL(label))
	    gtk_label_set_text(GTK_LABEL(label), (ثابت حرف *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_widget_set_tooltip_text(event_box, (ثابت gchar *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    طالما (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != عدم)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    إذا (nr == 1)
	gtk_widget_add_css_class(gui.tabline, "single-tab");
    وإلا
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    إذا (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    فراغ
gui_mch_set_curtab(صحيح nr)
{
    إذا (gui.tabline != عدم)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    ساكن فراغ
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    إذا (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    ساكن فراغ
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    إذا (ignore_tabline_evt)
	أرجع;

    إذا ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    وإلا
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ساكن فراغ
tabline_menu_event_cb(VimMenuItem *item UNUSED, VimMenuItemEvent event, فراغ *udata)
{
    إذا (event == VIM_MENU_ITEM_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_TO_INT(udata));
}

    ساكن فراغ
add_tabline_menu_item(VimMenu *menu, ثابت حرف *name, صحيح resp)
{
    VimMenuItem *item = VIM_MENU_ITEM(vim_menu_item_new(name,
		tabline_menu_event_cb, GINT_TO_POINTER(resp)));

    vim_menu_زیادکردن_item(menu, item, -1);
}

/*
 * Create a menu for the tab line.
 */
    ساكن VimMenu *
create_tabline_popup_menu(فراغ)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    add_tabline_menu_item(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, _("New Tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    أرجع menu;
}

    ساكن فراغ
tabline_menu_press_event(
	GtkGestureClick *gesture,
	صحيح		n_press UNUSED,
	مزدوج		x,
	مزدوج		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    إذا (but == GDK_BUTTON_SECONDARY)
    {
	// Right mouse button pressed, popup menu
	GdkRectangle rect;
	rect.x = x;
	rect.y = y;
	rect.width = 1;
	rect.height = 1;

	gtk_popover_set_pointing_to(GTK_POPOVER(popover), &rect);
	gtk_popover_popup(GTK_POPOVER(popover));
    }
    وإلا إذا (but == GDK_BUTTON_PRIMARY)
    {
	إذا (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    وإلا إذا (but == GDK_BUTTON_ناوەڕاست)
    {
	إذا (tabpage_hover != 0)
	    // Middle mouse click on tabpage label closes that tab.
	    send_tabline_menu_event(tabpage_hover, TABLINE_MENU_CLOSE);
    }
}
#endif

/*
 * ============================================================
 * Sign support
 * ============================================================
 */

#إذا defined(FEAT_SIGN_ICONS)
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)

    فراغ
gui_mch_drawsign(صحيح row, صحيح col, صحيح typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    إذا (sign == عدم)
	أرجع;

    vim_draw_area_add_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    row, col, SIGN_WIDTH, SIGN_HEIGHT);

    gtk_widget_queue_draw(gui.drawarea);
}

    فراغ *
gui_mch_register_sign(char_u *signfile)
{
    إذا (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GError	    *error = عدم;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ثابت حرف *)signfile,
							    &error);
	إذا (sign != عدم)
	    أرجع sign;

	semsg("E255: %s", error->message);
	g_error_free(error);
    }
    أرجع عدم;
}

    فراغ
gui_mch_destroy_sign(فراغ *sign)
{
    إذا (sign != عدم)
	g_object_unref(sign);
}
#endif

/*
 * ============================================================
 * Stubs for functions not yet implemented or not applicable in GTK4
 * ============================================================
 */

/*
 * Draw a string of characters on the screen using the current font and colors.
 * Splits the string into ASCII and ligature/UTF-8 segments so that ligature
 * characters are sent through Pango for proper shaping, while plain ASCII
 * uses the fast cached glyph path.
 * Returns the number of display cells used.
 */
    صحيح
gui_gtk_draw_string(صحيح row, صحيح col, char_u *s, صحيح len, صحيح flags)
{
    صحيح cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    row, col, s, len, flags);

    إذا (gui.drawarea != عدم)
	gtk_widget_queue_draw(gui.drawarea);

    أرجع cells;
}

    صحيح
gui_get_x11_windis(Window *win UNUSED, Display **dis UNUSED)
{
    // GTK4: not applicable
    أرجع FAIL;
}

#إذا defined(FEAT_MENU)
/*
 * Translate Vim's mnemonic tagging to GTK+ style and convert to UTF-8
 * if necessary.  The caller must vim_free() the returned string.
 *
 *	Input	Output
 *	_	__
 *	&&	&
 *	&	_	stripped if use_mnemonic == FALSE
 *	<Tab>		end of menu label text
 */
    ساكن char_u *
translate_mnemonic_tag(char_u *name, صحيح use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    صحيح	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    إذا (name == عدم)
	أرجع عدم;

    لكل (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	إذا (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    إذا (buf != عدم)
    {
	pdest = buf;
	لكل (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    إذا (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    وإلا إذا (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    وإلا إذا (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    وإلا إذا (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    أرجع buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    فراغ
gui_gtk_set_mnemonics(صحيح enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	إذا (menu->id == عدم)
	    استمر;

	name = translate_mnemonic_tag(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	إذا (VIM_IS_MENU_BAR_ITEM(menu->id))
	    vim_menu_bar_item_set_text(VIM_MENU_BAR_ITEM(menu->id),
		    (ثابت حرف *)name);
	vim_free(name);
    }
}

    ساكن فراغ
popupmenu_closed_cb(GtkWidget *popover, فراغ *udata UNUSED)
{
    gtk_widget_unparent(popover);
    إذا (gui.drawarea != عدم)
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    ساكن فراغ
gui_gtk_popup_at(vimmenu_T *menu, صحيح x, صحيح y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    إذا (menu == عدم || menu->submenu_id == عدم)
	أرجع;

    popover = vim_menu_copy(VIM_MENU(menu->submenu_id));
    gtk_widget_set_parent(popover, gui.drawarea);

    rect.x = x;
    rect.y = y;
    rect.width = rect.height = 1;

    // Make sure popover aligns down-and-to-the-right of the pointer.
    gtk_popover_set_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_widget_set_halign(popover, GTK_ALIGN_START);
    gtk_popover_set_pointing_to(GTK_POPOVER(popover), &rect);

    g_signal_connect(GTK_POPOVER(popover), "closed",
	    G_CALLBACK(popupmenu_closed_cb), عدم);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    فراغ
gui_make_popup(char_u *path_name, صحيح mouse_pos)
{
    vimmenu_T	*menu;
    صحيح		x, y;

    menu = gui_find_menu(path_name);
    إذا (menu == عدم || menu->submenu_id == عدم)
	أرجع;

    إذا (mouse_pos)
	gui_mch_getmouse(&x, &y);
    وإلا
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#endif // FEAT_MENU

    صحيح
get_menu_tool_width(فراغ)
{
    أرجع 0;
}

    صحيح
get_menu_tool_height(فراغ)
{
    GtkWidget *widgets[] = {
#ifdef FEAT_MENU
	gui.menubar,
#endif
#ifdef FEAT_TOOLBAR
	gui.toolbar,
#endif
#ifdef FEAT_GUI_TABLINE
	gui.tabline
#endif
    };

    صحيح height = 0;

    لكل (صحيح i = 0; i < (صحيح)ARRAY_LENGTH(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	صحيح		h;

	إذا (widgets[i] == عدم || !gtk_widget_get_visible(widgets[i]))
	    استمر;

	h = gtk_widget_get_height(widgets[i]);

	إذا (h == 0)
	{
	    // Allocation hasn't been updated yet (widget just became visible).
	    // Query the preferred height so the caller gets a valid value
	    // before the layout pass runs.  Use the maximum of minimum and
	    // natural height: GTK may allocate min_h even when natural_h is
	    // smaller (e.g. GtkNotebook tab bar has min_h > natural_h due to
	    // CSS).
	    gtk_widget_get_preferred_size(widgets[i], &min, &nat);
	    height += MAX(min.height, nat.height);
	}
	وإلا
	    height += h;
    }
    أرجع height;
}

/*
 * Update selection targets for regular and primary selections.
 */
    فراغ
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    إذا (cbd == &clip_plus)
	gdk_content_provider_content_changed(gui.regular_provider);
    وإلا
	gdk_content_provider_content_changed(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    ساكن GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    إذا (gui.mainwin == عدم)
	أرجع عدم;

    display = gtk_widget_get_display(gui.mainwin);
    إذا (display == عدم)
	أرجع عدم;

    إذا (cbd == &clip_plus)
    {
	إذا (provider != عدم)
	    *provider = gui.regular_provider;
	أرجع gdk_display_get_clipboard(display);
    }
    وإلا
    {
	إذا (provider != عدم)
	    *provider = gui.primary_provider;
	أرجع gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    ساكن فراغ
clip_read_cb(GdkClipboard *cb, GAsyncResult *result, ClipReadData *crd)
{
    GInputStream    *in_stream;
    ثابت حرف	    *mime_type;

    in_stream = gdk_clipboard_read_finish(cb, result, &mime_type, عدم);
    إذا (in_stream == عدم)
    {
	read_data_free(&crd->rd);
	أرجع;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_تێخستن_stream_read_async(in_stream, crd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_بەرز,
	    crd->rd.cancel, (GAsyncReadyCallback)read_data_تێخستن_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ساكن ثابت حرف *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	عدم
    };

    GdkClipboard *clipboard;
    ClipReadData *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, عدم);
    إذا (clipboard == عدم)
	أرجع;

    crd = (ClipReadData *)read_data_new(READ_DATA_CLIP, حجم(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_PRIORITY_بەرز, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t start;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	start = time(عدم);
	طالما (!g_cancellable_is_cancelled(cancel)
		&& time(عدم) < start + 3)
	    g_main_context_iteration(عدم, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

ساكن صحيح in_clipboard_set = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    فراغ
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

    ساكن فراغ
clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data)
{
    Clipboard_T *cbd = (Clipboard_T *)user_data;

    إذا (in_clipboard_set)
	أرجع;
    إذا (gdk_clipboard_is_local(clipboard))
	أرجع;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    صحيح
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    صحيح			ret;

    إذا (cb == عدم)
	أرجع FAIL;

    in_clipboard_set = TRUE;
    ret = gdk_clipboard_set_content(cb, cp);
    in_clipboard_set = FALSE;

    أرجع ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    فراغ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, عدم);
    إذا (clipboard == عدم)
	أرجع;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    إذا (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_set_content(clipboard, عدم);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(فراغ)
{
    أرجع gtk4_main_loop_level;
}

    فراغ
gtk_main_quit(فراغ)
{
    gtk4_main_loop_quit = TRUE;
}

#إذا defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
ساكن ثابت حرف *mshape_css_names[] =
{
    "default",			// arrow
    "none",			// blank
    "text",			// beam
    "ns-resize",		// updown
    "nwse-resize",		// udsizing
    "ew-resize",		// leftright
    "ew-resize",		// lrsizing
    "progress",			// busy
    "not-allowed",		// no
    "crosshair",		// crosshair
    "pointer",			// hand1
    "pointer",			// hand2
    "default",			// pencil (no CSS analogue)
    "help",			// question
    "default",			// right-arrow (no CSS analogue)
    "default",			// up-arrow (no CSS analogue)
    "default"			// last entry
};

    فراغ
mch_set_mouse_shape(صحيح shape)
{
    GdkCursor	*c;
    ثابت حرف	*css_name = "default";

    إذا (gui.drawarea == عدم)
	أرجع;

    إذا (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	وإلا إذا (shape < (صحيح)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
	وإلا
	    أرجع;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, عدم);
	gtk_widget_set_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    إذا (shape != MSHAPE_HIDE)
	last_shape = shape;
}

#endif // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



ساكن صحيح last_text_area_w = 0;
ساكن صحيح last_text_area_h = 0;

/*
 * ============================================================
 * Menu functions
 * ============================================================
 */

#ifdef FEAT_MENU

# ifdef FEAT_TOOLBAR
/*
 * Icon name table for toolbar buttons.
 * Must match toolbar_names[] in menu.c.
 */
ساكن ثابت حرف * ثابت toolbar_icon_names[] =
{
    /* 00 */ "document-new",
    /* 01 */ "document-open",
    /* 02 */ "document-save",
    /* 03 */ "edit-undo",
    /* 04 */ "edit-redo",
    /* 05 */ "edit-cut",
    /* 06 */ "edit-copy",
    /* 07 */ "edit-paste",
    /* 08 */ "document-print",
    /* 09 */ "help-browser",
    /* 10 */ "edit-find",
    /* 11 */ "document-save",		// save all (no standard icon)
    /* 12 */ "document-save",		// session save
    /* 13 */ "document-new",		// session new
    /* 14 */ "document-open",		// session load
    /* 15 */ "system-run",
    /* 16 */ "edit-find-replace",
    /* 17 */ "window-close",
    /* 18 */ "window-maximize-symbolic",	// maximize
    /* 19 */ "window-minimize-symbolic",	// minimize
    /* 20 */ "window-maximize-symbolic",	// split (no standard icon)
    /* 21 */ "utilities-terminal",	// shell
    /* 22 */ "go-previous",
    /* 23 */ "go-next",
    /* 24 */ "help-browser",		// find help
    /* 25 */ "edit-find",		// convert (no standard icon)
    /* 26 */ "go-jump",
    /* 27 */ "go-previous",		// back (reuse)
    /* 28 */ "go-next",			// forward (reuse)
    /* 29 */ "image-missing",
    /* 30 */ "image-missing",
};

    ساكن فراغ
toolbar_button_clicked_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    ساكن GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = عدم;

    // Try specified icon file first
    إذا (menu->iconfile != عدم)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	إذا (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (ثابت حرف *)buf, عدم);

	    إذا (texture != عدم)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_set_size_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    إذا (image == عدم)
    {
	ثابت حرف *icon_name = "image-missing";
	صحيح n = (صحيح)ARRAY_LENGTH(toolbar_icon_names);

	إذا (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_names[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    أرجع image;
}
# endif

    ساكن فراغ
menu_button_clicked_cb(
	VimMenuItem	    *item UNUSED,
	VimMenuItemEvent    event,
	vimmenu_T	    *menu)
{
    إذا (event == VIM_MENU_ITEM_CLICKED)
	gui_menu_cb(menu);
    وإلا إذا (event == VIM_MENU_ITEM_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	ساكن gboolean did_پەیام = FALSE;

	إذا (State & MODE_CMDLINE)
	    أرجع;

	tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	إذا (tooltip != عدم && utf_valid_string(tooltip, عدم))
	{
	    پەیام((حرف *)tooltip);
	    did_پەیام = TRUE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	وإلا إذا (did_پەیام)
	{
	    پەیام("");
	    did_پەیام = FALSE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	CONVERT_TO_UTF8_FREE(tooltip);
    }
}

    فراغ
gui_mch_add_menu(vimmenu_T *menu, صحيح idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;
    gboolean	use_mnemonic;
    char_u	*text;

    إذا (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_set_parent(menu->submenu_id, gui.drawarea);
	أرجع;
    }

    parent = menu->parent;

    إذا ((parent != عدم && parent->submenu_id == عدم)
	    || !menu_is_menubar(menu->name))
	أرجع;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = parent != عدم || p_wak[0] != 'n';
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    إذا (parent != عدم)
    {
	parent_widget = parent->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_item_new(
		    (ثابت حرف *)text, عدم, عدم));

	vim_menu_item_set_submenu(VIM_MENU_ITEM(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_زیادکردن_item(VIM_MENU(parent_widget),
		VIM_MENU_ITEM(menu->id), idx);
    }
    وإلا
    {
	parent_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_item_new(
		    (ثابت حرف *)text, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_زیادکردن_item(VIM_MENU_BAR(parent_widget),
		VIM_MENU_BAR_ITEM(menu->id), idx);
    }

    vim_free(text);
}

    فراغ
gui_mch_add_menu_item(vimmenu_T *menu, صحيح idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    إذا (parent != عدم && menu_is_toolbar(parent->name))
    {
	إذا (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_زیادکردن_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	وإلا
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*text;
	    char_u	*tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    إذا (tooltip != عدم && !utf_valid_string(tooltip, عدم))
		CONVERT_TO_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_زیادکردن_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ثابت حرف *)text, idx);
	    gtk_widget_set_tooltip_text(btn, (ثابت gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
	أرجع;
    }
# endif

    // Menu items (non-toolbar)
    إذا (parent == عدم || parent->submenu_id == عدم)
	أرجع;

    إذا (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_زیادکردن_separator(
		VIM_MENU(parent->submenu_id), idx));
    }
    وإلا
    {
	char_u	    *text;
	char_u	    *accel_text = عدم;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	text = translate_mnemonic_tag(menu->name, use_mnemonic);

	إذا (menu->actext != عدم && menu->actext[0] != NUL)
	    accel_text = CONVERT_TO_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_item_new((ثابت حرف *)text,
		    (VimMenuItemFunc)menu_button_clicked_cb, menu));

	إذا (accel_text != عدم)
	    vim_menu_item_set_accel(VIM_MENU_ITEM(menu->id),
		    (ثابت حرف *)accel_text);

	vim_menu_زیادکردن_item(VIM_MENU(parent->submenu_id),
		VIM_MENU_ITEM(menu->id), idx);

	vim_free(text);
	CONVERT_TO_UTF8_FREE(accel_text);
    }
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable UNUSED)
{
    // GTK4: tearoff menus don't exist.
}

    فراغ
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    إذا (menu->id == عدم)
	أرجع;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    إذا (tooltip != عدم && utf_valid_string(tooltip, عدم))
	gtk_widget_set_tooltip_text(menu->id, (ثابت حرف *)tooltip);
    CONVERT_TO_UTF8_FREE(tooltip);
}

    فراغ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    إذا (menu->parent != عدم && menu_is_toolbar(menu->parent->name))
    {
	إذا (menu->id != عدم)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = عدم;
	}
	أرجع;
    }

    // For popup menus, unparent the menu as well
    إذا (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	إذا (menu->submenu_id != عدم)
	    gtk_widget_unparent(menu->submenu_id);
    }
    وإلا إذا (menu->parent == عدم && menu->id != عدم)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    وإلا إذا (menu->id != عدم)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->parent->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    صحيح x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    ساكن فراغ
show_menubar_popover(فراغ)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    إذا (gui.menubar == عدم || gui.drawarea == عدم)
	أرجع;

    إذا (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), عدم);
	أرجع;
    }

    // Copy and convert the menubar into a menu popover
    menu = vim_menu_bar_to_menu(VIM_MENU_BAR(gui.menubar));

    gtk_widget_set_parent(menu, gui.drawarea);
    gtk_popover_set_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    rect.x = 0;
    rect.y = 0;
    rect.width = 1;
    rect.height = 1;
    gtk_popover_set_pointing_to(GTK_POPOVER(menu), &rect);
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), عدم);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#endif // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    فراغ
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    GtkAdjustment *adj;

    إذا (sb->id == عدم)
	أرجع;
    إذا (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	أرجع;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_set_lower(adj, 0.0);
    gtk_adjustment_set_upper(adj, (gdouble)max + 1);
    gtk_adjustment_set_بەها(adj, (gdouble)val);
    gtk_adjustment_set_step_increment(adj, 1.0);
    gtk_adjustment_set_page_increment(adj, (gdouble)(size > 2 ? size - 2 : 1));
    gtk_adjustment_set_page_size(adj, (gdouble)size);
}

    فراغ
gui_mch_set_scrollbar_pos(scrollbar_T *sb, صحيح x, صحيح y, صحيح w, صحيح h)
{
    إذا (sb->id != عدم)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    صحيح formwin_w = gtk_widget_get_width(gui.formwin);
    صحيح sbar_w = 0;
    صحيح xpad;

    إذا (gui.which_scrollbars[SBAR_چەپ])
	sbar_w += gui.scrollbar_width;
    إذا (gui.which_scrollbars[SBAR_ڕاست])
	sbar_w += gui.scrollbar_width;

    xpad = formwin_w - last_text_area_w - sbar_w;
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    صحيح formwin_h = gtk_widget_get_height(gui.formwin);
    صحيح ypad;

    ypad = formwin_h - last_text_area_h;
    إذا (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_height;

    أرجع (ypad < 0) ? 0 : ypad;
}

    ساكن فراغ
adjustment_بەها_changed(GtkAdjustment *adj, gpointer data UNUSED)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_data(G_OBJECT(adj), "vim-sb");
    long بەها = (long)gtk_adjustment_get_بەها(adj);

    إذا (sb != عدم)
	gui_drag_scrollbar(sb, بەها, FALSE);
}

    فراغ
gui_mch_create_scrollbar(scrollbar_T *sb, صحيح orient)
{
    GtkAdjustment *adj;
    إذا (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, عدم);
    وإلا
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, عدم);

    gtk_widget_add_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_set_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_set_data(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_بەها_changed), عدم);
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    إذا (sb->id != عدم)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = عدم;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    فراغ
gui_mch_set_text_area_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    إذا (w <= 0 || h <= 0)
	أرجع;
    last_text_area_w = w;
    last_text_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    فراغ
gui_gtk4_calculate_bleed(صحيح width, صحيح height)
{
    صحيح old_ڕاست = gui.bleed_ڕاست;
    صحيح old_bot = gui.bleed_bot;

    gui.bleed_ڕاست = width - last_text_area_w;
    gui.bleed_bot = height - last_text_area_h;

    إذا (gui.which_scrollbars[SBAR_چەپ])
	gui.bleed_ڕاست -= gui.scrollbar_width;
    إذا (gui.which_scrollbars[SBAR_ڕاست])
	gui.bleed_ڕاست -= gui.scrollbar_width;
    إذا (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_height;

    // Not sure if this can happen, but be safe...
    إذا (gui.bleed_ڕاست < 0)
	gui.bleed_ڕاست = 0;
    إذا (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    إذا (old_ڕاست != gui.bleed_ڕاست || old_bot != gui.bleed_bot)
	gtk_widget_queue_draw(GTK_WIDGET(gui.drawarea));
}

/*
 * ============================================================
 * Browse dialogs
 * ============================================================
 */

/*
 * Blocking helper: run a GtkFileDialog and wait for result.
 */
تعريفنوع بنية {
    GFile	*result;
    gboolean	done;
} FileDialogData;

    ساكن فراغ
file_dialog_open_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_open_finish(
		    GTK_FILE_DIALOG(source), res, عدم);
    fdd->done = TRUE;
}

    ساكن فراغ
file_dialog_save_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_save_finish(
		    GTK_FILE_DIALOG(source), res, عدم);
    fdd->done = TRUE;
}

    ساكن فراغ
file_dialog_folder_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_select_folder_finish(
		    GTK_FILE_DIALOG(source), res, عدم);
    fdd->done = TRUE;
}

    char_u *
gui_mch_browse(صحيح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext UNUSED,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    FileDialogData  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *result = عدم;
    GListStore	    *filters;

    title = CONVERT_TO_UTF8(title);

    إذا (initdir == عدم || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    وإلا إذا (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    add_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    إذا (title != عدم)
	gtk_file_dialog_set_title(dlg, (ثابت حرف *)title);

    {
	GFile *dir = g_file_new_for_path((ثابت حرف *)dirbuf);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    إذا (filter != عدم && *filter != NUL)
    {
	صحيح		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	إذا (patt != عدم)
	{
	    طالما (p != عدم && *p != NUL)
	    {
		إذا (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    إذا (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    وإلا
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			إذا (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    إذا (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		وإلا
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    gtk_file_dialog_set_filters(dlg, G_LIST_MODEL(filters));
    g_object_unref(filters);

    إذا (saving && dflt != عدم && *dflt != NUL)
	gtk_file_dialog_set_initial_name(dlg, (ثابت حرف *)dflt);

    fdd.result = عدم;
    fdd.done = FALSE;

    إذا (saving)
	gtk_file_dialog_save(dlg, GTK_WINDOW(gui.mainwin), عدم,
		file_dialog_save_cb, &fdd);
    وإلا
	gtk_file_dialog_open(dlg, GTK_WINDOW(gui.mainwin), عدم,
		file_dialog_open_cb, &fdd);

    طالما (!fdd.done)
	g_main_context_iteration(عدم, TRUE);

    إذا (fdd.result != عدم)
    {
	حرف *path = g_file_get_path(fdd.result);
	إذا (path != عدم)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    أرجع result;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    FileDialogData	fdd;
    char_u		*result = عدم;

    title = CONVERT_TO_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    إذا (title != عدم)
	gtk_file_dialog_set_title(dlg, (ثابت حرف *)title);

    إذا (initdir != عدم && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_path((ثابت حرف *)initdir);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.result = عدم;
    fdd.done = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_WINDOW(gui.mainwin), عدم,
	    file_dialog_folder_cb, &fdd);

    طالما (!fdd.done)
	g_main_context_iteration(عدم, TRUE);

    إذا (fdd.result != عدم)
    {
	حرف *path = g_file_get_path(fdd.result);
	إذا (path != عدم)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    أرجع result;
}

/*
 * ============================================================
 * Message dialog
 * ============================================================
 */

/*
 * Split up button_string into individual button labels by inserting NUL bytes.
 * Also replace the Vim-style mnemonic accelerator prefix '&' with '_'.
 * "button_string" is duplicated; caller must free the duplicated string via
 * *tofree.
 */
    ساكن حرف **
split_button_string(char_u *button_string, صحيح *n_buttons, حرف **tofree)
{
    حرف	    **array;
    char_u	    *p;
    unsigned صحيح    ژماردن = 1;

    button_string = (char_u *)g_strdup((ثابت حرف *)button_string);

    لكل (p = button_string; *p != NUL; ++p)
	إذا (*p == DLG_BUTTON_SEP)
	    ++ژماردن;

    array = g_malloc_n(ژماردن, حجم(حرف *));
    ژماردن = 0;

    إذا (array != عدم)
    {
	array[ژماردن++] = (حرف *)button_string;
	لكل (p = button_string; *p != NUL; )
	{
	    إذا (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[ژماردن++] = (حرف *)p;
	    }
	    وإلا إذا (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    وإلا
		MB_PTR_ADV(p);
	}
    }

    *tofree = (حرف *)button_string;
    *n_buttons = ژماردن;
    أرجع array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    ساكن ثابت حرف *
dialog_type_to_icon(صحيح type)
{
    بدل (type)
    {
	حالة VIM_ERROR:
	    أرجع "dialog-error-symbolic";
	حالة VIM_WARNING:
	    أرجع "dialog-warning-symbolic";
	حالة VIM_INFO:
	    أرجع "dialog-information-symbolic";
	حالة VIM_QUESTION:
	    أرجع "dialog-question-symbolic";
	افتراضي:
	    اكسر;
    }
    أرجع عدم;
}

// Data associated with each button in the dialog
تعريفنوع بنية
{
    صحيح		but_idx;
    صحيح		*response;
    gboolean	*done;
} DialogButtonState;

تعريفنوع بنية
{
    GtkWidget	*win;
    gboolean	*done;
    gboolean	no_alt;
} DialogState;

    ساكن فراغ
dialog_button_clicked_cb(GtkButton *button UNUSED, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->done = TRUE;
}

    ساكن gboolean
dialog_کلیل_pressed_cb(
	Gtk_Event_Controller_کلیل	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	DialogState		*dstate)
{
    إذا (keyval == GDK_کلیل_Escape)
    {
	*dstate->done = TRUE;
	أرجع TRUE;
    }

    إذا (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_mask()))
	أرجع gtk_widget_mnemonic_activate(dstate->win, FALSE);
    أرجع FALSE;
}

    ساكن gboolean
dialog_close_request_cb(GtkWindow *win UNUSED, gboolean *win_closed)
{
    *win_closed = TRUE;
    أرجع FALSE;
}

    صحيح
gui_mch_dialog(
	صحيح	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	صحيح	def_but,
	char_u	*textfield,
	صحيح	ex_cmd UNUSED)
{
    GtkWindow		*win = GTK_WINDOW(gtk_window_new());
    GtkWidget		*vertbox;
    GtkWidget		*message_box;
    ثابت حرف		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*entry = عدم;
    char_u		*utf8_title;
    char_u		*utf8_message;
    GtkEventController *کلیل_controller;
    DialogButtonState	*but_states = عدم;
    حرف		*tofree = عدم;
    صحيح			response = -1;
    gboolean		done = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_TO_UTF8(title);
    إذا (utf8_title != عدم)
	gtk_window_set_title(win, (ثابت حرف *)utf8_title);
    CONVERT_TO_UTF8_FREE(utf8_title);

    gtk_window_set_transient_for(win, GTK_WINDOW(gui.mainwin));
    gtk_window_set_modal(win, TRUE);
    gtk_window_set_default_size(win, 300, -1);
    gtk_window_set_destroy_with_parent(win, TRUE);
    g_signal_connect(win, "close-request",
	    G_CALLBACK(dialog_close_request_cb), &win_closed);

    // Create main vertical layout container
    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_window_set_child(win, vertbox);
    gtk_widget_set_margin_لووتکە(vertbox, 24);
    gtk_widget_set_margin_bottom(vertbox, 24);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);

    // Add the message label
    message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(message_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), message_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_to_icon(type);
    إذا (icon_name != عدم)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(message_box), icon);
    }

    utf8_message = CONVERT_TO_UTF8(message);
    label = gtk_label_new((ثابت حرف *)utf8_message);
    CONVERT_TO_UTF8_FREE(utf8_message);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(message_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    کلیل_controller = gtk_event_controller_کلیل_new();
    g_signal_connect(کلیل_controller, "key-pressed",
	    G_CALLBACK(dialog_کلیل_pressed_cb), &state);
    gtk_widget_add_controller(GTK_WIDGET(win), کلیل_controller);
    state.done = &done;
    state.win = GTK_WIDGET(win);

    إذا (textfield != عدم)
    {
	// Add text entry so user can enter text
	char_u *utf8_text = CONVERT_TO_UTF8(textfield);

	entry = gtk_entry_new();

	إذا (utf8_text != عدم)
	    gtk_editable_set_text(GTK_EDITABLE(entry), (ثابت حرف *)utf8_text);
	وإلا
	    gtk_editable_set_text(GTK_EDITABLE(entry), "");
	CONVERT_TO_UTF8_FREE(utf8_text);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_box_append(GTK_BOX(vertbox), entry);
	state.no_alt = FALSE;
    }
    وإلا
    {
	GListModel  *controllers;
	صحيح	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_کاڵاکان(controllers);
	لكل (صحيح i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_item(controllers, i);
	    إذا (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_set_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_window_set_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    إذا (buttons != عدم)
    {
	GtkWidget   *but_box;
	حرف	    **buttons_arr; // Note that array is allocated, not strings
	صحيح	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	إذا (vim_strchr(p_go, GO_VERTICAL) != عدم)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	وإلا
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_set_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = split_button_string(buttons, &n_buttons, &tofree);

	but_states = g_malloc_n(n_buttons, حجم(DialogButtonState));

	لكل (صحيح i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_states + i;

	    but_label = CONVERT_TO_UTF8((char_u *)buttons_arr[i]);
	    إذا (but_label == عدم)
		استمر;

	    but = gtk_button_new_with_mnemonic((حرف *)but_label);
	    إذا (i == def_but - 1)
		gtk_window_set_default_widget(win, but);
	    gtk_box_append(GTK_BOX(but_box), but);
	    CONVERT_TO_UTF8_FREE(but_label);

	    but_state->but_idx = i;
	    but_state->response = &response;
	    but_state->done = &done;

	    g_signal_connect(but, "clicked",
		    G_CALLBACK(dialog_button_clicked_cb), but_state);
	}
	g_free(buttons_arr);
    }

    gtk_window_present(win);

    طالما (!done && !win_closed)
	g_main_context_iteration(عدم, TRUE);

    إذا (done)
    {
	إذا (textfield != عدم)
	{
	    // Get the text the user entered
	    char_u *text;

	    text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(entry));
	    text = CONVERT_FROM_UTF8(text);
	    vim_strncpy(textfield, text, IOSIZE - 1);
	    CONVERT_FROM_UTF8_FREE(text);
	}

	gtk_window_destroy(win);
    }
    g_free(but_states);
    g_free(tofree);

    // Vim buttons are indexed starting from one.
    أرجع response == -1 ? 0 : response + 1;
}

/*
 * ============================================================
 * Find/Replace dialogs
 * ============================================================
 */

/*
 * ============================================================
 * Find/Replace dialog
 * ============================================================
 */

تعريفنوع بنية
{
    GtkWidget *dialog;
    GtkWidget *what;	    // Find what entry
    GtkWidget *with;	    // Replace with entry
    GtkWidget *wword;	    // Whole word check
    GtkWidget *mcase;	    // Match case check
    GtkWidget *up;	    // Direction up radio
    GtkWidget *down;	    // Direction down radio
    GtkWidget *find;	    // 'Find Next' action button
    GtkWidget *replace;	    // 'Replace With' action button
    GtkWidget *all;	    // 'Replace All' action button
} SharedFindReplace;

ساكن SharedFindReplace find_widgets = {0};
ساكن SharedFindReplace repl_widgets = {0};

    ساكن فراغ
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    صحيح			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = GPOINTER_TO_INT(data);

    إذا (flags == FRD_FINDNEXT)
    {
	repl_text = عدم;
	sfr = &find_widgets;
    }
    وإلا
    {
	repl_text = (char_u *)gtk_editable_get_text(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(sfr->what));
    direction_down = gtk_check_button_get_active(
	    GTK_CHECK_BUTTON(sfr->down));

    إذا (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    إذا (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

    ساكن فراغ
dialog_destroyed_cb(GtkWidget *widget UNUSED, gpointer data)
{
    *(GtkWidget **)data = عدم;
}

    ساكن فراغ
entry_changed_cb(GtkWidget *entry, GtkWidget *dialog)
{
    ثابت gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    إذا (!entry_text)
	أرجع; // Shouldn't happen

    nonempty = (entry_text[0] != '\0');

    إذا (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    إذا (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

    ساكن gboolean
find_کلیل_pressed_cb(
	Gtk_Event_Controller_کلیل	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	SharedFindReplace	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    إذا (state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	أرجع FALSE;

    // the Escape key synthesizes a cancellation action
    إذا (keyval == GDK_کلیل_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_window_destroy(GTK_WINDOW(frdp->dialog));
	أرجع TRUE;
    }

    أرجع FALSE;
}

    ساكن فراغ
entry_activate_cb(GtkWidget *widget UNUSED, فراغ *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    ساكن فراغ
find_replace_dialog_create(char_u *arg, صحيح do_replace)
{
    SharedFindReplace	*frdp;
    char_u		*entry_text;
    صحيح			wword = FALSE;
    صحيح			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*کلیل_controller;

    frdp = do_replace ? &repl_widgets : &find_widgets;
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    إذا (entry_text != عدم && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = entry_text;
	entry_text = string_convert(&output_conv, entry_text, عدم);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    إذا (frdp->dialog != عدم)
    {
	إذا (entry_text != عدم)
	{
	    gtk_editable_set_text(GTK_EDITABLE(frdp->what),
		    (حرف *)entry_text);
	    gtk_check_button_set_active(GTK_CHECK_BUTTON(frdp->wword),
		    (gboolean)wword);
	    gtk_check_button_set_active(GTK_CHECK_BUTTON(frdp->mcase),
		    (gboolean)mcase);
	}
	gtk_window_present(GTK_WINDOW(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	إذا (do_replace && g_utf8_strlen(
		    gtk_editable_get_text(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	أرجع;
    }

    // Create a new dialog window.
    frdp->dialog = gtk_window_new();
    gtk_window_set_transient_for(GTK_WINDOW(frdp->dialog),
	    GTK_WINDOW(gui.mainwin));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(frdp->dialog), TRUE);
    gtk_window_set_title(GTK_WINDOW(frdp->dialog),
	    do_replace ? _("VIM - Search and Replace...")
	    : _("VIM - Search..."));
    gtk_window_set_resizable(GTK_WINDOW(frdp->dialog), FALSE);

    g_signal_connect(frdp->dialog, "destroy",
	    G_CALLBACK(dialog_destroyed_cb), &frdp->dialog);

    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);
    gtk_widget_set_margin_لووتکە(vertbox, 12);
    gtk_widget_set_margin_bottom(vertbox, 12);
    gtk_window_set_child(GTK_WINDOW(frdp->dialog), vertbox);

    // Grid for labels + entries
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_box_append(GTK_BOX(vertbox), grid);

    // "Find what:" label + entry
    tmp = gtk_label_new(_("Find what:"));
    gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
    gtk_grid_attach(GTK_GRID(grid), tmp, 0, 0, 1, 1);

    frdp->what = gtk_entry_new();
    gtk_widget_set_hexpand(frdp->what, TRUE);
    sensitive = (entry_text != عدم && entry_text[0] != NUL);
    إذا (entry_text != عدم)
	gtk_editable_set_text(GTK_EDITABLE(frdp->what), (حرف *)entry_text);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    إذا (do_replace)
    {
	// "Replace with:" label + entry
	tmp = gtk_label_new(_("Replace with:"));
	gtk_label_set_xalign(GTK_LABEL(tmp), 0.0);
	gtk_grid_attach(GTK_GRID(grid), tmp, 0, 1, 1, 1);

	frdp->with = gtk_entry_new();
	gtk_widget_set_hexpand(frdp->with, TRUE);
	gtk_grid_attach(GTK_GRID(grid), frdp->with, 1, 1, 1, 1);
	gtk_entry_set_activates_default(GTK_ENTRY(frdp->with), TRUE);

	// Make the entry activation only change the input focus onto the
	// with item.
	gtk_entry_set_activates_default(GTK_ENTRY(frdp->what), FALSE);
	g_signal_connect(G_OBJECT(frdp->what), "activate",
		G_CALLBACK(entry_activate_cb), frdp->with);
    }
    وإلا
	// Make the entry activation do the search.
	gtk_entry_set_activates_default(GTK_ENTRY(frdp->what), TRUE);

    // Checkboxes
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    frdp->wword = gtk_check_button_new_with_label(_("Match whole word only"));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(frdp->wword),
	    (gboolean)wword);
    gtk_box_append(GTK_BOX(hbox), frdp->wword);

    frdp->mcase = gtk_check_button_new_with_label(_("Match case"));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(frdp->mcase),
	    (gboolean)mcase);
    gtk_box_append(GTK_BOX(hbox), frdp->mcase);

    // Direction radio buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    tmp = gtk_label_new(_("Direction:"));
    gtk_box_append(GTK_BOX(hbox), tmp);

    frdp->up = gtk_check_button_new_with_label(_("Up"));
    gtk_box_append(GTK_BOX(hbox), frdp->up);

    frdp->down = gtk_check_button_new_with_label(_("Down"));
    gtk_check_button_set_group(GTK_CHECK_BUTTON(frdp->down),
	    GTK_CHECK_BUTTON(frdp->up));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(frdp->down), TRUE);
    gtk_box_append(GTK_BOX(hbox), frdp->down);

    // Action buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_halign(hbox, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    btn = gtk_button_new_with_label(_("Find Next"));
    gtk_widget_set_sensitive(btn, sensitive);
    frdp->find = btn;

    کلیل_controller = gtk_event_controller_کلیل_new();
    g_signal_connect(کلیل_controller, "key-pressed",
	    G_CALLBACK(find_کلیل_pressed_cb), frdp);
    gtk_widget_add_controller(GTK_WIDGET(frdp->dialog), کلیل_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(entry_changed_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_window_set_default_widget(GTK_WINDOW(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
	    GINT_TO_POINTER(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    إذا (do_replace)
    {
	btn = gtk_button_new_with_label(_("Replace"));
	gtk_widget_set_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
		GINT_TO_POINTER(FRD_REPLACE));
	frdp->replace = btn;
	gtk_box_append(GTK_BOX(hbox), btn);

	btn = gtk_button_new_with_label(_("Replace All"));
	gtk_widget_set_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
		GINT_TO_POINTER(FRD_REPLACEALL));
	frdp->all = btn;
	gtk_box_append(GTK_BOX(hbox), btn);
    }

    btn = gtk_button_new_with_label(_("Close"));
    g_signal_connect_swapped(btn, "clicked",
	    G_CALLBACK(gtk_window_destroy), frdp->dialog);
    gtk_box_append(GTK_BOX(hbox), btn);

    gtk_window_present(GTK_WINDOW(frdp->dialog));
    gtk_widget_grab_focus(frdp->what);
    إذا (do_replace && entry_text != عدم && entry_text[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(entry_text);
}

    فراغ
gui_mch_find_dialog(exarg_T *eap)
{
    إذا (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
    إذا (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    فراغ
ex_helpfind(exarg_T *eap UNUSED)
{
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

/*
 * ============================================================
 * Native print dialog
 * ============================================================
 */

#ifdef USE_GTK4_PRINT_DIALOG
ساكن GtkPrintDialog *print_dialog = عدم;
ساكن GtkPrintSettings *print_settings = عدم;
ساكن GtkPageSetup *page_setup = عدم;
ساكن GtkPrintSetup *print_setup = عدم;
ساكن GFile *print_file;

    ساكن فراغ
print_dialog_cb(GtkPrintDialog *dialog, GAsyncResult *result, gboolean *done)
{
    GError *error = عدم;
    print_setup = gtk_print_dialog_setup_finish(dialog, result, &error);

    إذا (print_setup == عدم)
    {
	semsg(_(e_print_error_str), error->message);
	g_error_free(error);
    }
    *done = TRUE;
}

/*
 * Return file to write to on success (allocated string), or NULL on failure
 * (e.g. user cancelled)
 */
    char_u *
gui_gtk4_print_dialog(
	prt_settings_T	*psettings,
	char_u		*jobname,
	مزدوج		*page_width,
	مزدوج		*page_height)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    done = FALSE;
    char_u	    *temp;

    gtk_print_dialog_set_title(dialog, (ثابت حرف *)jobname);
    gtk_print_dialog_set_modal(dialog, TRUE);

    print_dialog = عدم;
    print_setup = عدم;

    إذا (print_settings == عدم)
    {
	option_table_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	إذا (opt->present)
	{
	    إذا (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    وإلا إذا (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    وإلا
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	إذا (opt->present)
	{
	    إذا (*opt->string == 'y')
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_PORTRAIT);
	    وإلا
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	إذا (opt->present)
	{
	    gtk_print_settings_set_paper_size(print_settings,
		    gtk_paper_size_new((ثابت حرف *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	إذا (opt->present)
	{
	    إذا (*opt->string == 'y')
		gtk_print_settings_set_collate(print_settings, TRUE);
	    وإلا
		gtk_print_settings_set_collate(print_settings, FALSE);
	}
    }

    إذا (page_setup == عدم)
	page_setup = gtk_page_setup_new();

    gtk_print_dialog_set_print_settings(dialog, print_settings);
    gtk_print_dialog_set_page_setup(dialog, page_setup);

    gtk_print_dialog_setup(dialog, GTK_WINDOW(gui.mainwin), عدم,
	    (GAsyncReadyCallback)print_dialog_cb, &done);

    طالما (!done)
	g_main_context_iteration(عدم, TRUE);

    إذا (print_setup == عدم)
    {
	g_object_unref(dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_پەیام = FALSE;
	أرجع عدم;
    }

    g_clear_object(&print_settings);
    g_clear_object(&page_setup);
    print_settings = g_object_ref(
	    gtk_print_setup_get_print_settings(print_setup));
    page_setup = g_object_ref(gtk_print_setup_get_page_setup(print_setup));

    {
	GtkPrintDuplex duplex = gtk_print_settings_get_duplex(print_settings);
	psettings->duplex = duplex != GTK_PRINT_DUPLEX_SIMPLEX;
    }

    psettings->has_color = gtk_print_settings_get_use_color(print_settings);

    *page_width = gtk_page_setup_get_paper_width(page_setup,
	    GTK_UNIT_POINTS);
    *page_height = gtk_page_setup_get_paper_height(page_setup,
	    GTK_UNIT_POINTS);

    temp  = vim_tempname('p', FALSE);
    إذا (temp == عدم)
    {
	emsg(_(e_cant_find_temp_file_for_writing));
	g_clear_object(&dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_پەیام = FALSE;
	أرجع عدم;
    }
    print_file = g_file_new_for_path((حرف *)temp);

    print_dialog = dialog;

    أرجع temp;
}

    ساكن فراغ
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsyncResult	*result,
	GFile		*file)
{
    حرف *path = g_file_get_path(file);

    (فراغ)gtk_print_dialog_print_file_finish(dialog, result, عدم);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(path);
    g_free(path);
}


    فراغ
gui_gtk4_print_finish(فراغ)
{
    إذا (print_dialog != عدم)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_WINDOW(gui.mainwin),
		print_setup, print_file, عدم,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = عدم;
	g_clear_pointer(&print_setup, gtk_print_setup_unref);
    }
}
#endif // USE_GTK4_PRINT_DIALOG

#ifdef FEAT_IMAGE_GUI

تعريفنوع بنية
{
    GdkTexture *texture;
} image_gdk_T;

تعريفنوع بنية
{
    Gsk_Render_گرێ   **nodes; // Note that elements may be NULL
    صحيح		    n_nodes;
} image_placement_gdk_T;

    صحيح
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    GdkMemoryFormat fmt;
    size_t	    stride;
    GBytes	    *bytes;
    size_t	    size;
    صحيح		    w, h;

    إذا (ctx == عدم)
	أرجع FAIL;

    إذا (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_MEMORY_R8G8B8A8;
    وإلا
	fmt = GDK_MEMORY_R8G8B8;

    image_get_dimensions(img, &w, &h);

    size = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    bytes = g_bytes_new(img->data, size);
    ctx->texture = gdk_memory_texture_new(w, h, fmt, bytes, stride);
    g_bytes_unref(bytes);

    img->backend_data = ctx;
    أرجع OK;
}

    فراغ
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_data;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    ساكن فراغ
clear_nodes(image_placement_gdk_T *ctx)
{
    إذا (ctx->nodes == عدم || gui.drawarea == عدم)
	أرجع;

    لكل (صحيح i = 0; i < ctx->n_nodes; i++)
    {
	إذا (ctx->nodes[i] == عدم)
	    استمر;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_گرێ_unref(ctx->nodes[i]);
    }
    g_clear_pointer(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    صحيح
image_placement_gui_init(image_placement_T *place)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    إذا (ctx == عدم)
	أرجع FAIL;

    place->backend_data = ctx;
    أرجع OK;
}

    فراغ
image_placement_gui_uninit(image_placement_T *place)
{
    image_placement_gdk_T *ctx = place->backend_data;

    clear_nodes(ctx);
    vim_free(ctx);
}

    فراغ
image_placement_gui_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_gdk_T   *ctx = place->backend_data;
    image_gdk_T		    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    صحيح			    n_rects;
    صحيح			    iw, ih;
    Gsk_Render_گرێ	    *texture_گرێ;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    إذا (rects == عدم)
	أرجع;

    clear_nodes(ctx);
    إذا (n_rects == 0)
	أرجع;

    image_get_dimensions(img, &iw, &ih);
    texture_گرێ = gsk_texture_گرێ_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(place->col) - FILL_X(place->crop_box.x1),
		FILL_Y(place->row) - FILL_Y(place->crop_box.y1)
		+ FILL_Y(place->row_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    إذا (texture_گرێ == عدم)
	// Not sure if this can happen...
	أرجع;

    ctx->nodes = g_malloc_n(n_rects, حجم(Gsk_Render_گرێ *));
    ctx->n_nodes = n_rects;

    لكل (صحيح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحيح		row, col;
	صحيح		posx, posy;
	صحيح		x, y, w, h;
	Gsk_Render_گرێ	*گرێ;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	گرێ = gsk_clip_گرێ_new(texture_گرێ,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	إذا (گرێ != عدم)
	    vim_draw_area_add_external(VIM_DRAW_AREA(gui.drawarea), گرێ);

	ctx->nodes[i] = گرێ;
    }
    gsk_render_گرێ_unref(texture_گرێ);
}

    فراغ
image_placement_gui_clear(image_placement_T *place)
{
    image_placement_gdk_T   *ctx = place->backend_data;

    clear_nodes(ctx);
}

#endif

#endif // FEAT_GUI_GTK
