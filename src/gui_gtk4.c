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

#شامل "vim.h"

#ifdef FEAT_GUI_GTK

#شامل <gdk/gdkkeysyms.h>
#شامل <gdk/gdk.h>
#شامل <gtk/gtk.h>
#شامل "gui_gtk4_f.h"
#شامل "gui_gtk4_cb.h"
#شامل "gui_gtk4_da.h"
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
#define No_مقدار		0x0000
#define Width_مقدار	0x0004
#define Height_مقدار	0x0008

    ایستا صحیح
vim_parse_geometry(ثابت نویسه *str, unsigned صحیح *width, unsigned صحیح *height)
{
    صحیح mask = No_مقدار;
    نویسه *end;
    long val;

    اگر (str == پوچ || *str == NUL)
	بازگشت mask;

    // Parse width
    اگر (*str != '+' && *str != '-')
    {
	val = strtol(str, &end, 10);
	اگر (end != str)
	{
	    *width = (unsigned صحیح)val;
	    mask |= Width_مقدار;
	    str = end;
	}
    }

    // Parse 'x' or 'X' separator and height
    اگر (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &end, 10);
	اگر (end != str)
	{
	    *height = (unsigned صحیح)val;
	    mask |= Height_مقدار;
	    str = end;
	}
    }

    بازگشت mask;
}

#اگر defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
ایستا صحیح last_shape = 0;
#endif

#define DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
ایستا شمارش {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
ایستا صحیح gtk4_main_loop_level = 0;
ایستا صحیح gtk4_main_loop_quit = FALSE;

#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

تعریفنوع gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
ایستا ساختار special_کلید
{
    guint کلید_sym;
    char_u code0;
    char_u code1;
}
ثابت special_کلیدها[] =
{
    {GDK_کلید_Up,	'k', 'u'},
    {GDK_کلید_Down,	'k', 'd'},
    {GDK_کلید_چپ,	'k', 'l'},
    {GDK_کلید_راست,	'k', 'r'},
    {GDK_کلید_F1,	'k', '1'},
    {GDK_کلید_F2,	'k', '2'},
    {GDK_کلید_F3,	'k', '3'},
    {GDK_کلید_F4,	'k', '4'},
    {GDK_کلید_F5,	'k', '5'},
    {GDK_کلید_F6,	'k', '6'},
    {GDK_کلید_F7,	'k', '7'},
    {GDK_کلید_F8,	'k', '8'},
    {GDK_کلید_F9,	'k', '9'},
    {GDK_کلید_F10,	'k', ';'},
    {GDK_کلید_F11,	'F', '1'},
    {GDK_کلید_F12,	'F', '2'},
    {GDK_کلید_Help,	'%', '1'},
    {GDK_کلید_Undo,	'&', '8'},
    {GDK_کلید_Back_Space,	'k', 'b'},
    {GDK_کلید_درج,	'k', 'I'},
    {GDK_کلید_Delete,	'k', 'D'},
    {GDK_کلید_Home,	'k', 'h'},
    {GDK_کلید_End,	'@', '7'},
    {GDK_کلید_Prior,	'k', 'P'},
    {GDK_کلید_Next,	'k', 'N'},
    {GDK_کلید_Print,	'%', '9'},
    {GDK_کلید_KP_چپ,	'k', 'l'},
    {GDK_کلید_KP_راست,	'k', 'r'},
    {GDK_کلید_KP_Up,	'k', 'u'},
    {GDK_کلید_KP_Down,	'k', 'd'},
    {GDK_کلید_KP_درج,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_کلید_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_کلید_KP_Home,	'K', '1'},
    {GDK_کلید_KP_End,	'K', '4'},
    {GDK_کلید_KP_Prior,	'K', '3'},
    {GDK_کلید_KP_Next,	'K', '5'},
    {GDK_کلید_KP_Add,	'K', '6'},
    {GDK_کلید_KP_Subtract, 'K', '7'},
    {GDK_کلید_KP_Divide,	'K', '8'},
    {GDK_کلید_KP_Multiply, 'K', '9'},
    {GDK_کلید_KP_Enter,	'K', 'A'},
    {GDK_کلید_KP_Decimal,	'K', 'B'},
    {GDK_کلید_KP_0,	'K', 'C'},
    {GDK_کلید_KP_1,	'K', 'D'},
    {GDK_کلید_KP_2,	'K', 'E'},
    {GDK_کلید_KP_3,	'K', 'F'},
    {GDK_کلید_KP_4,	'K', 'G'},
    {GDK_کلید_KP_5,	'K', 'H'},
    {GDK_کلید_KP_6,	'K', 'I'},
    {GDK_کلید_KP_7,	'K', 'J'},
    {GDK_کلید_KP_8,	'K', 'K'},
    {GDK_کلید_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    ایستا صحیح
keyval_to_string(unsigned صحیح keyval, char_u *string)
{
    صحیح		len;
    guint32	uc;

    uc = gdk_keyval_to_unicode(keyval);
    اگر (uc != 0)
    {
	len = utf_char2bytes((صحیح)uc, string);
    }
    وگرنه
    {
	len = 1;
	انتخاب (keyval)
	{
	    حالت GDK_کلید_Tab: حالت GDK_کلید_KP_Tab: حالت GDK_کلید_ISO_چپ_Tab:
		string[0] = TAB;
		بشکن;
	    حالت GDK_کلید_Linefeed:
		string[0] = NL;
		بشکن;
	    حالت GDK_کلید_Return: حالت GDK_کلید_ISO_Enter: حالت GDK_کلید_3270_Enter:
		string[0] = CAR;
		بشکن;
	    حالت GDK_کلید_Escape:
		string[0] = ESC;
		بشکن;
	    پیشفرض:
		len = 0;
		بشکن;
	}
    }
    string[len] = NUL;
    بازگشت len;
}

    ایستا صحیح
modifiers_gdk2vim(guint state)
{
    صحیح modifiers = 0;

    اگر (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    اگر (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    اگر (state & GDK_ALT_MASK)
	modifiers |= MOD_MASK_ALT;
    اگر (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    اگر (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;

    بازگشت modifiers;
}

ایستا GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
ایستا gboolean کلید_press_event(Gtk_Event_Controller_کلید *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
ایستا تهی کلید_release_event(Gtk_Event_Controller_کلید *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
ایستا تهی button_press_event(GtkGestureClick *gesture, صحیح n_press, دوگانه x, دوگانه y, gpointer data);
ایستا تهی button_release_event(GtkGestureClick *gesture, صحیح n_press, دوگانه x, دوگانه y, gpointer data);
ایستا تهی motion_notify_event(GtkEventControllerMotion *controller, دوگانه x, دوگانه y, gpointer data);
ایستا تهی enter_notify_event(GtkEventControllerMotion *controller, دوگانه x, دوگانه y, gpointer data);
ایستا gboolean scroll_event(GtkEventControllerScroll *controller, دوگانه dx, دوگانه dy, gpointer data);
ایستا تهی focus_in_event(GtkEventControllerFocus *controller, gpointer data);
ایستا تهی focus_out_event(GtkEventControllerFocus *controller, gpointer data);
#ifdef FEAT_DND
ایستا gboolean drop_cb(GtkDropTargetAsync *target, GdkDrop *drop, دوگانه x, دوگانه y, تهی *data);
#endif
#ifdef FEAT_GUI_TABLINE
ایستا تهی tabline_enter_cb(GtkEventController *controller, دوگانه x, دوگانه y, تهی *udata);
ایستا تهی on_select_tab(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
ایستا تهی on_tab_reordered(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
ایستا VimMenu *create_tabline_popup_menu(تهی);
ایستا تهی tabline_add_css(تهی);
ایستا تهی tabline_menu_press_event(GtkGestureClick *gesture, صحیح n_press, دوگانه x, دوگانه y, GtkWidget *popover);
#endif
ایستا تهی mainwin_destroy_cb(GObject *object, gpointer data);
ایستا gboolean delete_event_cb(GtkWindow *window, gpointer data);
ایستا صحیح query_pointer_pos(صحیح *x, صحیح *y, GdkModifierType *state);
ایستا تهی mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_data);
ایستا تهی set_form_size(صحیح width, صحیح height);
ایستا تهی drawarea_realize_cb(GtkWidget *widget, gpointer data);
ایستا تهی drawarea_unrealize_cb(GtkWidget *widget, gpointer data);
ایستا تهی clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data);
#ifdef FEAT_MENU
ایستا تهی show_menubar_popover(تهی);
#endif

ایستا ثابت نویسه *prgname = پوچ;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    ایستا gboolean
arg_match(ثابت نویسه *s, ثابت نویسه *name, نویسه **مقدار)
{
    size_t  len = strlen(name);

    اگر (strncmp(s, name, len) != 0)
	بازگشت FALSE;
    اگر (s[len] == '=')
	*مقدار = (نویسه *)s + len + 1;
    وگرنه اگر (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    تهی
gui_mch_prepare(صحیح *argc, نویسه **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    صحیح	i = 0;

    وقتی (i < *argc)
    {
	نویسه	*s = argv[i];
	نویسه	*مقدار = پوچ;
	صحیح	has_inline_مقدار = FALSE;
	صحیح	n_strip;

	اگر (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    ادامه;
	}

	اگر (strchr(s, '=') != پوچ)
	    has_inline_مقدار = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	اگر (!has_inline_مقدار)
	{
	    اگر (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		مقدار = argv[i + 1];
	    وگرنه
		مقدار = پوچ;
	}

	اگر (arg_match(s, "-fn", &مقدار) || arg_match(s, "-font", &مقدار))
	    font_argument = مقدار;
	وگرنه اگر (arg_match(s, "-geom", &مقدار)
		|| arg_match(s, "-geometry", &مقدار))
	{
	    اگر (مقدار != پوچ)
		gui.geom = vim_strsave((char_u *)مقدار);
	}
	وگرنه اگر (arg_match(s, "-bg", &مقدار)
		|| arg_match(s, "-background", &مقدار))
	    background_argument = مقدار;
	وگرنه اگر (arg_match(s, "-fg", &مقدار)
		|| arg_match(s, "-foreground", &مقدار))
	    foreground_argument = مقدار;
#ifdef FEAT_NETBEANS_INTG
	وگرنه اگر (strncmp(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_مقدار = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#endif
	وگرنه اگر (arg_match(s, "--prg-name", &مقدار))
	    // GTK4 specific
	    prgname = مقدار;
	وگرنه
	{
	    i++;
	    ادامه;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	اگر (مقدار != پوچ && !has_inline_مقدار)
	    n_strip = 2;

	اگر (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    اگر (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * اندازه(نویسه *));
	    argv[*argc] = پوچ;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    تهی
gui_mch_free_all(تهی)
{
}

    ایستا guint
timeout_add(صحیح time, timeout_cb_type (*callback)(gpointer), صحیح *flagp)
{
    بازگشت g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    ایستا تهی
timeout_remove(guint timer)
{
    g_source_remove(timer);
}

ایستا long_u blink_waittime = 700;
ایستا long_u blink_ontime = 400;
ایستا long_u blink_offtime = 250;
ایستا guint blink_timer = 0;

    ایستا timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    اگر (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, پوچ);
    }
    وگرنه
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, پوچ);
    }
    بازگشت FALSE;
}

    صحیح
gui_mch_is_blinking(تهی)
{
    بازگشت blink_state != BLINK_NONE;
}

    صحیح
gui_mch_is_blink_off(تهی)
{
    بازگشت blink_state == BLINK_OFF;
}

    تهی
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    تهی
gui_mch_stop_blink(صحیح may_call_gui_update_cursor)
{
    اگر (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    اگر (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    تهی
gui_mch_start_blink(تهی)
{
    اگر (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, پوچ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    صحیح
gui_mch_early_init_check(صحیح give_message UNUSED)
{
    بازگشت OK;
}

    صحیح
gui_mch_init_check(تهی)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_set_prgname(prgname == پوچ ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    اگر (g_getenv("EGL_LOG_LEVEL") == پوچ)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    بازگشت OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    صحیح
gui_mch_init(تهی)
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
		     G_CALLBACK(delete_event_cb), پوچ);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), پوچ);

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
		     G_CALLBACK(on_select_tab), پوچ);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), پوچ);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), پوچ);

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
		     G_CALLBACK(drawarea_realize_cb), پوچ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), پوچ);

    // Set up event controllers.
    {
	GtkEventController *کلید_ctrl = gtk_event_controller_کلید_new();
	g_signal_connect(کلید_ctrl, "key-pressed",
			 G_CALLBACK(کلید_press_event), پوچ);
	g_signal_connect(کلید_ctrl, "key-released",
			 G_CALLBACK(کلید_release_event), پوچ);
	gtk_widget_add_controller(gui.drawarea, کلید_ctrl);
#ifdef FEAT_XIM
	xim_init();
#endif
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), پوچ);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), پوچ);
	gtk_widget_add_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), پوچ);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), پوچ);
	gtk_widget_add_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), پوچ);
	gtk_widget_add_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_in_event), پوچ);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_out_event), پوچ);
	gtk_widget_add_controller(gui.drawarea, focus);
    }

#ifdef FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_target = gtk_drop_target_async_new(پوچ,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_set_dnd_targets();
	g_signal_connect(gui.drop_target, "drop",
			 G_CALLBACK(drop_cb), پوچ);
	gtk_widget_add_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_target));
    }
#endif

    gui.border_offset = gui.border_width;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_pointer = gdk_cursor_new_from_name("none", پوچ);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	اگر (primary != پوچ)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_star);
	اگر (regular != پوچ)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_star);

    بازگشت OK;
}

    تهی
gui_mch_new_colors(تهی)
{
    اگر (gui.drawarea != پوچ && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحیح
gui_mch_open(تهی)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_width;
    guint pixel_height;

    اگر (gui.geom != پوچ)
    {
	صحیح		mask;
	unsigned صحیح	w, h;

	mask = vim_parse_geometry((نویسه *)gui.geom, &w, &h);

	اگر (mask & Width_مقدار)
	    Columns = w;
	اگر (mask & Height_مقدار)
	{
	    اگر (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	VIM_CLEAR(gui.geom);
    }
    وگرنه
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	اگر (Columns > 80)
	    Columns = 80;
	اگر (Rows > 24)
	    Rows = 24;
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    pixel_width  += get_menu_tool_width();
    pixel_height += get_menu_tool_height();

    اگر (foreground_argument != پوچ)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    اگر (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    اگر (background_argument != پوچ)
	bg_pixel = gui_get_color((char_u *)background_argument);
    اگر (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    اگر (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    وگرنه
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    set_normal_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), پوچ);
    // Resize is handled by GtkForm's size_allocate callback.

    set_form_size((صحیح)pixel_width - get_menu_tool_width(),
	    (صحیح)pixel_height - get_menu_tool_height());

    gtk_window_present(GTK_WINDOW(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    بازگشت OK;
}

    تهی
gui_mch_exit(صحیح rc UNUSED)
{
    اگر (gui.mainwin != پوچ)
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
		اگر ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != پوچ)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#endif
	gtk_window_destroy(GTK_WINDOW(gui.mainwin));
    }
}

    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    بازگشت FAIL;
}

    تهی
gui_mch_set_winpos(صحیح x UNUSED, صحیح y UNUSED)
{
    // GTK4/Wayland: window positioning not available
}

    صحیح
gui_mch_maximized(تهی)
{
    بازگشت gtk_window_is_maximized(GTK_WINDOW(gui.mainwin));
}

    تهی
gui_mch_unmaximize(تهی)
{
    اگر (gui.mainwin != پوچ)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

    تهی
gui_mch_set_fullscreen(صحیح flag)
{
    اگر (gui.mainwin == پوچ)
	بازگشت;
    اگر (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    وگرنه
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}

    ایستا تهی
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec UNUSED, gpointer user_data UNUSED)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    اگر (gtk_window_is_fullscreen(GTK_WINDOW(obj)))
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
    تهی
gui_mch_newfont(تهی)
{
    صحیح w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_width(gui.formwin);
    h = gtk_widget_get_height(gui.formwin);
    gui_resize_shell(w, h);
}

    تهی
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    اگر (title != پوچ && gui.mainwin != پوچ)
	gtk_window_set_title(GTK_WINDOW(gui.mainwin), (ثابت نویسه *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    تهی
gui_gtk_init_decor_height(تهی)
{
    صحیح h = gtk_widget_get_height(gui.mainwin);

    اگر (h == 0)
	بازگشت;

    h -= get_menu_tool_height();
    h -= gtk_widget_get_height(gui.formwin);

    gui.decor_height = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    ایستا تهی
set_form_size(صحیح width, صحیح height)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    اگر (gtk_widget_get_width(gui.formwin) == width
	    && gtk_widget_get_height(gui.formwin) == height)
	بازگشت;

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

    تهی
gui_mch_set_shellsize(صحیح width, صحیح height,
	صحیح min_width UNUSED, صحیح min_height UNUSED,
	صحیح base_width UNUSED, صحیح base_height UNUSED,
	صحیح direction UNUSED)
{
    set_form_size(width, height);

    gui_mch_update();
}

    تهی
gui_mch_get_screen_dimensions(صحیح *screen_w, صحیح *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    اگر (surface != پوچ)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	اگر (monitor != پوچ)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.width;
	    *screen_h = geom.height;
	    بازگشت;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ifdef FEAT_MENU
    تهی
gui_mch_enable_menu(صحیح showit)
{
    اگر (gui.menubar != پوچ)
    {
	gtk_widget_set_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	اگر (gui.drawarea != پوچ)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

#ifdef FEAT_TOOLBAR
    تهی
gui_mch_show_toolbar(صحیح showit)
{
    اگر (gui.toolbar != پوچ)
    {
	gtk_widget_set_visible(gui.toolbar, showit);
	اگر (showit)
	    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	اگر (gui.drawarea != پوچ)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

    تهی
gui_mch_set_dark_theme(صحیح dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    اگر (settings != پوچ)
	g_object_set(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, پوچ);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    صحیح
gui_mch_adjust_charheight(تهی)
{
    PangoFontMetrics *metrics;
    صحیح ascent;
    صحیح descent;

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

    بازگشت OK;
}

تعریفنوع ساختار {
    PangoFontDescription    *result;
    gboolean		    done;
} FontDialogData;

    ایستا تهی
font_dialog_finish_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FontDialogData *fdd = (FontDialogData *)data;
    fdd->result = gtk_font_dialog_choose_font_finish(
		    GTK_FONT_DIALOG(source), res, پوچ);
    fdd->done = TRUE;
}

    ایستا gboolean
font_filter(gpointer item, gpointer data UNUSED)
{
    اگر (PANGO_IS_FONT_FAMILY(item))
	بازگشت pango_font_family_is_monospace(PANGO_FONT_FAMILY(item));
    اگر (PANGO_IS_FONT_FACE(item))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(item));
	اگر (family != پوچ)
	    بازگشت pango_font_family_is_monospace(family);
    }
    بازگشت TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = پوچ;
    char_u		*fontname = پوچ;
    FontDialogData	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_set_modal(dlg, TRUE);
    gtk_font_dialog_set_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, پوچ, پوچ)));

    اگر (oldval != پوچ && oldval[0] != NUL)
    {
	char_u *oldname;

	اگر (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, پوچ);
	وگرنه
	    oldname = oldval;

	اگر (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    اگر (p != پوچ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		اگر (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((ثابت نویسه *)oldname);
	اگر (oldname != oldval)
	    vim_free(oldname);
    }
    وگرنه
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.result = پوچ;
    fdd.done = FALSE;

    gtk_font_dialog_choose_font(dlg, GTK_WINDOW(gui.mainwin),
	    initial, پوچ, font_dialog_finish_cb, &fdd);

    وقتی (!fdd.done)
	g_main_context_iteration(پوچ, TRUE);

    اگر (fdd.result != پوچ)
    {
	نویسه *name = pango_font_description_to_string(fdd.result);
	اگر (name != پوچ)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    اگر (p != پوچ && درون_داد_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&درون_داد_conv, p, پوچ);
		vim_free(p);
	    }
	    وگرنه
		fontname = p;
	}
	pango_font_description_free(fdd.result);
    }

    اگر (initial != پوچ)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    بازگشت fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    ایستا تهی
ascii_glyph_table_init(تهی)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    صحیح		    i;

    اگر (gui.ascii_glyphs != پوچ)
	pango_glyph_string_free(gui.ascii_glyphs);
    اگر (gui.ascii_font != پوچ)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = پوچ;
    gui.ascii_font   = پوچ;

    برای (i = 0; i < 128; ++i)
    {
	اگر (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	وگرنه
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (ثابت نویسه *)ascii_chars,
			      0, اندازه(ascii_chars), attr_list, پوچ);

    اگر (item_list != پوچ && item_list->next == پوچ)
    {
	PangoItem   *item;
	صحیح	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت نویسه *)ascii_chars, اندازه(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	اگر (gui.ascii_glyphs->رقم_glyphs == (صحیح)اندازه(ascii_chars))
	{
	    برای (i = 0; i < gui.ascii_glyphs->رقم_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_offset += MAX(0, width - geom->width) / 2;
		geom->width = width;
	    }
	}
	وگرنه
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = پوچ;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = پوچ;
	}
    }

    g_list_foreach(item_list, (GFunc)(تهی *)&pango_item_free, پوچ);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    ایستا تهی
get_styled_font_variants(تهی)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);
    اگر (plain_font == پوچ)
	بازگشت;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    اگر (bold_font != پوچ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    صحیح
gui_mch_init_font(char_u *font_name, صحیح fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحیح			    width;

    اگر (font_name == پوچ)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    اگر (font_desc == پوچ)
	بازگشت FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, پوچ);
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    اگر (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	صحیح im_row = gui.row < screen_Rows ? gui.row : screen_Rows - 1;
	صحیح im_col = gui.col < screen_Columns ? gui.col : screen_Columns - 1;
	im_set_position(im_row, im_col);
    }
    بازگشت OK;
}

    GuiFont
gui_mch_get_font(char_u *name, صحیح report_error)
{
    PangoFontDescription *font;

    اگر (name == پوچ)
	بازگشت پوچ;

    font = pango_font_description_from_string((ثابت نویسه *)name);
    اگر (font == پوچ)
    {
	اگر (report_error)
	    semsg(_(e_unknown_font_str), name);
	بازگشت پوچ;
    }

    // Ensure a size is set
    اگر (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    بازگشت font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    اگر (font != NOFONT)
    {
	نویسه *desc = pango_font_description_to_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	بازگشت ret;
    }
    بازگشت پوچ;
}

    تهی
gui_mch_free_font(GuiFont font)
{
    اگر (font != NOFONT)
	pango_font_description_free(font);
}

    تهی
gui_mch_expand_font(
    optexpand_T	*args,
    تهی	*param,
    صحیح		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = پوچ;
    صحیح			n_families = 0;
    صحیح			wide = *(صحیح *)param;

    اگر (args->oe_include_orig_val && *args->oe_opt_مقدار == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	اگر (add_match((char_u *)DEFAULT_FONT) != OK)
	    بازگشت;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    برای (صحیح i = 0; i < n_families; i++)
    {
	اگر (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ادامه;

	ثابت نویسه *fam_name = pango_font_family_get_name(font_families[i]);
	اگر (درون_داد_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&درون_داد_conv,
		    (char_u *)fam_name, پوچ);
	    اگر (buf != پوچ)
	    {
		اگر (add_match(buf) != OK)
		{
		    vim_free(buf);
		    بشکن;
		}
		vim_free(buf);
	    }
	    وگرنه
		بشکن;
	}
	وگرنه
	{
	    اگر (add_match((char_u *)fam_name) != OK)
		بشکن;
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
    اگر (!gui.in_use)
	بازگشت INVALCOLOR;

    اگر (name != پوچ)
	بازگشت gui_get_color_cmn(name);

    بازگشت INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
    بازگشت gui_get_rgb_color_cmn(r, g, b);
}

    ایستا GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    بازگشت rgba;
}

    تهی
gui_mch_set_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_to_rgba(color);
}

    تهی
gui_mch_set_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_to_rgba(color);
}

    تهی
gui_mch_set_sp_color(guicolor_T color)
{
    *gui.spcolor = color_to_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    بازگشت pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    تهی
gui_gtk4_update_size(تهی)
{
    vim_draw_area_set_size(VIM_DRAW_AREA(gui.drawarea),
	    gui.رقم_rows, gui.رقم_cols);
}

#ifdef FEAT_NETBEANS_INTG
    تهی
gui_gtk4_add_multisign(
	cairo_surface_t *surf,
	صحیح		row,
	صحیح		col,
	صحیح		width,
	صحیح		height)
{
    vim_draw_area_add_multisign(VIM_DRAW_AREA(gui.drawarea), surf, row, col,
	    width, height);
}
#endif

    تهی
gui_mch_clear_block(صحیح row1, صحیح col1, صحیح row2, صحیح col2)
{
    vim_draw_area_clear_block(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    اگر (gui.drawarea != پوچ)
	gtk_widget_queue_draw(gui.drawarea);
}

    تهی
gui_mch_clear_all(تهی)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    اگر (gui.drawarea != پوچ)
	gtk_widget_queue_draw(gui.drawarea);
}

    تهی
gui_mch_delete_سطرها(صحیح row, صحیح رقم_سطرها)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row, row + رقم_سطرها, gui.scroll_region_bot,
	    gui.scroll_region_چپ, gui.scroll_region_راست);
    gui_clear_block(
	    gui.scroll_region_bot - رقم_سطرها + 1, gui.scroll_region_چپ,
	    gui.scroll_region_bot, gui.scroll_region_راست);

    gtk_widget_queue_draw(gui.drawarea);
}

    تهی
gui_mch_درج_سطرها(صحیح row, صحیح رقم_سطرها)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row + رقم_سطرها, row, gui.scroll_region_bot - رقم_سطرها,
	    gui.scroll_region_چپ, gui.scroll_region_راست);
    gui_clear_block(
	    row, gui.scroll_region_چپ,
	    row + رقم_سطرها - 1, gui.scroll_region_راست);

    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    تهی
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, صحیح w, صحیح h)
{
    gui_mch_set_bg_color(bg);
    gui_mch_set_fg_color(fg);
    vim_draw_area_set_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_queue_draw(gui.drawarea);
}

    تهی
gui_mch_flash(صحیح msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (صحیح)Rows - 1, (صحیح)Columns - 1);
    gui_mch_flush();
    ui_delay((long)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (صحیح)Rows - 1, (صحیح)Columns - 1);
}

    تهی
gui_mch_invert_rectangle(صحیح r, صحیح c, صحیح nr, صحیح nc)
{
    vim_draw_area_invert_block(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    ایستا gboolean
کلید_press_event(Gtk_Event_Controller_کلید *controller UNUSED,
	guint کلید_sym, guint keycode UNUSED,
	GdkModifierType state, gpointer data UNUSED)
{
    char_u	string[32], string2[32];
    صحیح		len;
    صحیح		i;
    صحیح		modifiers;
    صحیح		کلید;
    char_u	*s, *d;

#ifdef FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    اگر (xic != پوچ)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	اگر (event != پوچ && gtk_im_context_filter_keypress(xic, event))
	    بازگشت TRUE;
    }
#endif

#ifdef FEAT_MENU
    اگر (کلید_sym == GDK_کلید_F10 && gui.menubar != پوچ)
    {
	ایستا char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	اگر (check_map(k10, State, FALSE, TRUE, FALSE, پوچ, پوچ) == پوچ)
	{
	    show_menubar_popover();
	    بازگشت TRUE;
	}
    }
#endif

    len = keyval_to_string(کلید_sym, string2);

    اگر (len > 1 && درون_داد_conv.vc_type != CONV_NONE)
	len = convert_درون_داد(string2, len, اندازه(string2));

    s = string2;
    d = string;
    برای (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	اگر (d[-1] == CSI && d + 2 < string + اندازه(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (صحیح)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    اگر (کلید_sym == GDK_کلید_ISO_چپ_Tab)
    {
	کلید_sym = GDK_کلید_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    اگر ((state & GDK_ALT_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	بازگشت FALSE;
#endif

    // Check for special keys
    اگر (len == 0 || len == 1)
    {
	برای (i = 0; special_کلیدها[i].کلید_sym != 0; i++)
	{
	    اگر (special_کلیدها[i].کلید_sym == کلید_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیدها[i].code0;
		string[2] = special_کلیدها[i].code1;
		len = -3;
		بشکن;
	    }
	}
    }

    اگر (len == 0)
	بازگشت TRUE;

    اگر (len == -3)
	کلید = TO_SPECIAL(string[1], string[2]);
    وگرنه
    {
	string[len] = NUL;
	کلید = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    کلید = simplify_کلید(کلید, &modifiers);
    اگر (کلید == CSI)
	کلید = K_CSI;
    اگر (IS_SPECIAL(کلید))
    {
	string[0] = CSI;
	string[1] = K_SECOND(کلید);
	string[2] = K_THIRD(کلید);
	len = 3;
    }
    وگرنه
    {
	کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);
	modifiers = may_remove_shift_modifier(modifiers, کلید);
	len = mb_char2bytes(کلید, string);
    }

    اگر (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_درون_داد_buf(string2, 3);
    }

    {
	صحیح int_ch = check_for_interrupt(کلید, modifiers);
	اگر (int_ch != NUL)
	{
	    trash_درون_داد_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_درون_داد_buf(string, len);

    اگر (p_mh)
	gui_mch_mousehide(TRUE);

    بازگشت TRUE;
}

    ایستا تهی
کلید_release_event(Gtk_Event_Controller_کلید *controller UNUSED,
	guint keyval UNUSED, guint keycode UNUSED,
	GdkModifierType state UNUSED, gpointer data UNUSED)
{
}

ایستا صحیح mouse_timed_out = TRUE;
ایستا guint mouse_click_timer = 0;

    ایستا timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    *(صحیح *)data = TRUE;
    بازگشت FALSE;
}

    ایستا صحیح
modifiers_gdk2mouse(guint state)
{
    صحیح modifiers = 0;

    اگر (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    اگر (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    اگر (state & GDK_ALT_MASK)
	modifiers |= MOUSE_ALT;

    بازگشت modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#define ANY_BUTTON_MASK (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK \
	| GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
ایستا صحیح mouse_pressed_button = -1;

ایستا guint motion_repeat_timer = 0;
ایستا gboolean motion_repeat_offset = FALSE;
ایستا صحیح mouse_press_x = -1, mouse_press_y = -1;

    ایستا تهی
button_press_event(GtkGestureClick *gesture, صحیح n_press UNUSED,
	دوگانه x, دوگانه y, gpointer data UNUSED)
{
    صحیح button;
    صحیح repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    اگر (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_out);

    انتخاب (btn)
    {
	حالت 1: button = MOUSE_چپ; بشکن;
	حالت 2: button = MOUSE_میانه; بشکن;
	حالت 3: button = MOUSE_راست; بشکن;
	حالت 8: button = MOUSE_X1; بشکن;
	حالت 9: button = MOUSE_X2; بشکن;
	پیشفرض: بازگشت;
    }

    mouse_press_x = (صحیح)x;
    mouse_press_y = (صحیح)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (صحیح)x, (صحیح)y, repeated_click, vim_modifiers);
}

    ایستا تهی
button_release_event(GtkGestureClick *gesture, صحیح n_press UNUSED,
	دوگانه x, دوگانه y, gpointer data UNUSED)
{
    صحیح vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    اگر (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (صحیح)x, (صحیح)y, FALSE, vim_modifiers);
}

ایستا دوگانه prev_mouse_x = -1.0;
ایستا دوگانه prev_mouse_y = -1.0;

ایستا GdkModifierType cur_state = 0;

    ایستا timeout_cb_type
mouse_repeat_timer_cb(gpointer data UNUSED)
{
    صحیح x, y;

    اگر (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	بازگشت G_SOURCE_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    اگر (vim_used_in_درون_داد_buf() > 10)
	بازگشت G_SOURCE_CONTINUE;

    x = (صحیح)prev_mouse_x;
    y = (صحیح)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    اگر (motion_repeat_offset)
	x += gui.char_width;
    motion_repeat_offset = !motion_repeat_offset;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    بازگشت G_SOURCE_CONTINUE;
}

    ایستا تهی
motion_notify_event(GtkEventControllerMotion *controller UNUSED,
	دوگانه x, دوگانه y, gpointer data UNUSED)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    اگر (mouse_pressed_button >= 0 && event != پوچ
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_MASK))
    {
	اگر (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    اگر (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	صحیح		w, h;

	اگر (event != پوچ)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (صحیح)x, (صحیح)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_width(gui.drawarea);
	h = gtk_widget_get_height(gui.drawarea);

	اگر (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	اگر (x < 0 || y < 0 || x >= w || y >= h)
	{
	    صحیح	dx, dy;
	    صحیح offshoot;
	    صحیح	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    اگر (offshoot > 127)
		delay = 5;
	    وگرنه
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_add(delay,
		    mouse_repeat_timer_cb, پوچ);
	}
	وگرنه
	    motion_repeat_timer = 0;
    }
    وگرنه
	gui_mouse_moved((صحیح)x, (صحیح)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    اگر (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    ایستا تهی
enter_notify_event(GtkEventControllerMotion *controller UNUSED,
	دوگانه x, دوگانه y, gpointer data UNUSED)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    ایستا gboolean
scroll_event(GtkEventControllerScroll *controller UNUSED,
	دوگانه dx UNUSED, دوگانه dy, gpointer data UNUSED)
{
    صحیح button;
    int_u vim_modifiers;
    صحیح x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    اگر (event == پوچ)
	بازگشت FALSE;
    state = gdk_event_get_modifier_state(event);

    اگر (dy < 0)
	button = MOUSE_4;	// scroll up
    وگرنه اگر (dy > 0)
	button = MOUSE_5;	// scroll down
    وگرنه اگر (dx < 0)
	button = MOUSE_7;	// scroll left
    وگرنه اگر (dx > 0)
	button = MOUSE_6;	// scroll right
    وگرنه
	بازگشت FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    بازگشت TRUE;
}

    ایستا تهی
focus_in_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    اگر (gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	اگر (blink_state == BLINK_NONE)
	    gui_mch_start_blink();
    }
}

    ایستا تهی
focus_out_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    اگر (!gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	اگر (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#اگر defined(FEAT_IMAGE)
    ایستا تهی
scale_factor_cb(GdkSurface  *surface,
	GParamSpec	    *pspec UNUSED,
	تهی		    *udata UNUSED)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#endif

    ایستا تهی
drawarea_realize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#اگر defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_factor_cb), پوچ);
    redraw_all_later(UPD_VALID);
#endif
    gui_mch_new_colors();
}

    ایستا تهی
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ifdef FEAT_XIM
    im_shutdown();
#endif
}

تعریفنوع شمارش
{
    READ_DATA_CLIP,	    // Clipboard data
    READ_DATA_DROP,	    // URI or text DND data
    READ_DATA_DROP_DATA	    // HTML DND data
} ReadDataType;

#define READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
تعریفنوع ساختار
{
    ReadDataType    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    نویسه	    *buf; // NULL when not needed
} ReadData;

#ifdef FEAT_DND
تعریفنوع ساختار
{
    ReadData	rd;
    GdkDrop	*drop;
    صحیح		x;
    صحیح		y;
} DropReadData;
#endif

تعریفنوع ساختار
{
    ReadData	rd;
    Clipboard_T *cbd;
    نویسه	*mime_type;
} ClipReadData;

    ایستا ReadData *
read_data_new(ReadDataType type, size_t sz)
{
    ReadData *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    اگر (type == READ_DATA_CLIP || type == READ_DATA_DROP_DATA)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    بازگشت rd;
}

    ایستا تهی
read_data_free(ReadData *rd)
{
    g_cancellable_cancel(rd->cancel);

    اگر (rd->type == READ_DATA_CLIP)
	g_free(((ClipReadData *)rd)->mime_type);
    وگرنه اگر (rd->type == READ_DATA_DROP || rd->type == READ_DATA_DROP_DATA)
	g_object_unref(((DropReadData *)rd)->drop);
    g_object_unref(rd->cancel);
    اگر (rd->arr != پوچ)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ifdef FEAT_DND
ایستا تهی drop_read_text(GdkDrop *drop, char_u *text);
#endif

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    ایستا تهی
read_data_درون_داد_cb(
	GInputStream	*stream,
	GAsyncResult	*result,
	ReadData	*rd)
{
    ssize_t r = g_درون_داد_stream_read_finish(stream, result, پوچ);

    اگر (r == -1)
    {
	// Error occurred
	DropReadData *drd = (DropReadData *)rd;

	اگر (rd->type == READ_DATA_DROP_DATA)
	    gdk_drop_finish(drd->drop, 0);
    }
    وگرنه اگر (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	اگر (rd->type == READ_DATA_CLIP)
	{
	    ClipReadData    *crd = (ClipReadData *)rd;
	    long	    len;
	    char_u	    *actual, *final;
	    صحیح		    motion_type = MAUTO;
	    char_u	    *tofree = پوچ;

	    len = (long)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = پوچ;

	    اگر (clip_convert_data(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ifdef FEAT_DND
	وگرنه اگر (rd->type == READ_DATA_DROP_DATA)
	{
	    DropReadData    *drd = (DropReadData *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ثابت uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = پوچ;
	    drop_read_text(drd->drop, str);
	    g_free(str);
	    gdk_drop_finish(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#endif
    }
    وگرنه
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ثابت uint8_t *)rd->buf, r);
	g_درون_داد_stream_read_async(stream, rd->buf, READDATA_BUFSIZE,
		G_PRIORITY_بالا,
		rd->cancel, (GAsyncReadyCallback)read_data_درون_داد_cb, rd);
	بازگشت;
    }

    read_data_free(rd);
    g_object_unref(stream);
}

#ifdef FEAT_DND
/*
 * Set up for receiving DND items.
 */
    تهی
gui_gtk_set_dnd_targets(تهی)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_add_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_add_gtype(builder, G_TYPE_STRING);
    اگر (clip_html)
	gdk_content_formats_builder_add_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_to_formats(builder);
    gtk_drop_target_async_set_formats(gui.drop_target, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    ایستا تهی
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

    اگر (text == پوچ || *text == NUL || !query_pointer_pos(پوچ, پوچ, &state))
	بازگشت;

    dnd_yank_drag_data((char_u *)text, (long)STRLEN(text));

    dropkey[2] = modifiers_gdk2vim(state);
    اگر (dropkey[2] != 0)
	add_to_درون_داد_buf(dropkey, (صحیح)اندازه(dropkey));
    وگرنه
	add_to_درون_داد_buf(dropkey + 3, (صحیح)(اندازه(dropkey) - 3));
}

    ایستا تهی
drop_read_مقدار_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    ثابت GValue    *مقدار = gdk_drop_read_مقدار_finish(drop, result, پوچ);
    gboolean	    success = FALSE;

    اگر (مقدار == پوچ)
	برو exit;

    اگر (G_مقدار_HOLDS(مقدار, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_مقدار_get_boxed(مقدار);
	صحیح	nfiles = g_slist_length(files);
	char_u	**fnames;
	صحیح	i;

	اگر (nfiles <= 0)
	    برو exit;

	fnames = ALLOC_MULT(char_u *, nfiles);
	اگر (fnames == پوچ)
	    برو exit;

	i = 0;
	برای (GSList *l = files; l != پوچ; l = l->next)
	{
	    GFile *file = l->data;
	    نویسه *path = g_file_get_path(file);
	    اگر (path != پوچ)
		fnames[i++] = vim_strsave((char_u *)path);
	    g_free(path);
	}
	nfiles = i;

	اگر (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	وگرنه
	    vim_free(fnames);
	success = TRUE;
    }
    وگرنه اگر (G_مقدار_HOLDS(مقدار, G_TYPE_STRING))
    {
	ثابت نویسه *text = g_مقدار_get_string(مقدار);

	اگر (text == پوچ || *text == NUL)
	    برو exit;

	drop_read_text(drop, (char_u *)text);
	success = TRUE;
    }

exit:
    gdk_drop_finish(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_data_free(&drd->rd);
}

    ایستا تهی
drop_read_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    GInputStream    *in_stream;
    ثابت نویسه	    *m;

    in_stream = gdk_drop_read_finish(drop, result, &m, پوچ);
    اگر (in_stream == پوچ || STRCMP(m, "text/html") != 0)
    {
	read_data_free(&drd->rd);
	gdk_drop_finish(drop, 0);
	بازگشت;
    }

    أكد(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_درون_داد_stream_read_async(in_stream, drd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_بالا,
	    drd->rd.cancel, (GAsyncReadyCallback)read_data_درون_داد_cb, drd);
}

// GCancellable of current DND operation, else NULL.
ایستا GCancellable *dnd_cancel = پوچ;
ایستا guint dnd_timeout_id = 0;

    ایستا gboolean
drop_timeout_cb(تهی *udata UNUSED)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    بازگشت G_SOURCE_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    ایستا gboolean
drop_cb(
	GtkDropTargetAsync  *target UNUSED,
	GdkDrop		    *drop,
	دوگانه		    x,
	دوگانه		    y,
	تهی		    *udata UNUSED)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    DropReadData	*drd = پوچ;

    اگر (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, اندازه(*drd));
	gdk_drop_read_مقدار_async(drop, GDK_TYPE_FILE_LIST, G_PRIORITY_بالا,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_مقدار_cb, drd);
    }
    وگرنه اگر (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	ایستا ثابت نویسه *m[] = {"text/html", پوچ};

	drd = (DropReadData *)read_data_new(READ_DATA_DROP_DATA, اندازه(*drd));
	gdk_drop_read_async(drop, m, G_PRIORITY_بالا,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    وگرنه اگر (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, اندازه(*drd));
	gdk_drop_read_مقدار_async(drop, G_TYPE_STRING, G_PRIORITY_بالا,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_مقدار_cb, drd);
    }

    اگر (dnd_cancel != پوچ)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    اگر (drd != پوچ)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (صحیح)x;
	drd->y = (صحیح)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_add(3000, drop_timeout_cb, پوچ);

	بازگشت TRUE;
    }
    بازگشت FALSE;
}
#endif

    ایستا تهی
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    gui.mainwin = پوچ;
    gui.drawarea = پوچ;
    اگر (!exiting)
	gui_shell_closed();
}

    ایستا gboolean
delete_event_cb(GtkWindow *window UNUSED, gpointer data UNUSED)
{
    gui_shell_closed();
    بازگشت TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    ایستا timeout_cb_type
درون_داد_timer_cb(gpointer data)
{
    صحیح *timed_out = (صحیح *)data;

    *timed_out = TRUE;
    بازگشت FALSE;	// don't call me again
}

    تهی
gui_mch_update(تهی)
{
    صحیح cnt = 0;

    وقتی (g_main_context_pending(پوچ) && !vim_is_درون_داد_buf_full()
								&& ++cnt < 100)
	g_main_context_iteration(پوچ, TRUE);
}

#ifdef FEAT_JOB_CHANNEL
    ایستا timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    بازگشت TRUE; // Keep repeating
}
#endif

    صحیح
gui_mch_wait_for_chars(long wtime)
{
    صحیح		focus;
    guint	timer;
    ایستا صحیح	timed_out;
    صحیح		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    اگر (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   درون_داد_timer_cb, &timed_out);
    وگرنه
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    اگر (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, پوچ);
#endif

    focus = gui.in_focus;

    انجامبده
    {
	// Stop or start blinking when focus changes
	اگر (gui.in_focus != focus)
	{
	    اگر (gui.in_focus)
		gui_mch_start_blink();
	    وگرنه
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	اگر (did_add_timer)
	    برو theend;
# endif
#endif

	اگر (gui.mainwin == پوچ)
	    برو theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	اگر (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    برو theend;
	}

	اگر (!درون_داد_available())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(پوچ, TRUE);
	    --gtk4_main_loop_level;
	}

	اگر (درون_داد_available())
	{
	    retval = OK;
	    برو theend;
	}
    } وقتی (wtime < 0 || !timed_out);

    gui_mch_update();

theend:
    اگر (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    اگر (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    بازگشت retval;
}

    تهی
gui_mch_flush(تهی)
{
    // Ensure the offscreen surface content gets painted to the widget.
    اگر (gui.drawarea != پوچ)
	gtk_widget_queue_draw(gui.drawarea);
    اگر (gui.mainwin != پوچ && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    تهی
gui_mch_beep(تهی)
{
    GdkDisplay *display;

    اگر (gui.mainwin != پوچ && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	اگر (display != پوچ)
	    gdk_display_beep(display);
    }
}

    تهی *
gui_mch_get_display(تهی)
{
    اگر (gui.mainwin != پوچ && gtk_widget_get_display(gui.mainwin))
	بازگشت gtk_widget_get_display(gui.mainwin);
    بازگشت پوچ;
}

    تهی
gui_mch_iconify(تهی)
{
    gtk_window_minimize(GTK_WINDOW(gui.mainwin));
}

    تهی
gui_mch_set_foreground(تهی)
{
    gtk_window_present(GTK_WINDOW(gui.mainwin));
}

    ایستا صحیح
query_pointer_pos(صحیح *x, صحیح *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *pointer;
    دوگانه	     sx, sy, nx, ny;
    graphene_point_t src, dst;

    اگر (gui.drawarea == پوچ)
	بازگشت FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    اگر (native == پوچ)
	بازگشت FALSE;
    surface = gtk_native_get_surface(native);
    اگر (surface == پوچ)
	بازگشت FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    اگر (display == پوچ)
	بازگشت FALSE;
    seat = gdk_display_get_default_seat(display);
    اگر (seat == پوچ)
	بازگشت FALSE;
    pointer = gdk_seat_get_pointer(seat);
    اگر (pointer == پوچ)
	بازگشت FALSE;

    اگر (!gdk_surface_get_device_position(surface, pointer, &sx, &sy, state))
	بازگشت FALSE;

    اگر (x != پوچ && y != پوچ)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (اعشاری)(sx - nx);
	src.y = (اعشاری)(sy - ny);
	اگر (!gtk_widget_compute_point(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    بازگشت FALSE;

	*x = (صحیح)dst.x;
	*y = (صحیح)dst.y;
    }
    بازگشت TRUE;
}

    تهی
gui_mch_getmouse(صحیح *x, صحیح *y)
{
    اگر (!query_pointer_pos(x, y, پوچ))
    {
	*x = 0;
	*y = 0;
    }
}

    تهی
gui_mch_setmouse(صحیح x UNUSED, صحیح y UNUSED)
{
    // GTK4/Wayland: cannot warp pointer
}

    تهی
gui_mch_mousehide(صحیح hide)
{
    اگر (gui.pointer_hidden == hide)
	بازگشت;

    gui.pointer_hidden = hide;
    اگر (gui.blank_pointer != پوچ)
    {
	اگر (hide)
	    gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
	وگرنه
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#وگرنه
	    gtk_widget_set_cursor(gui.drawarea, پوچ);
#endif
    }
}

    صحیح
gui_mch_haskey(char_u *name)
{
    صحیح i;

    برای (i = 0; special_کلیدها[i].کلید_sym != 0; i++)
	اگر (name[0] == special_کلیدها[i].code0
		&& name[1] == special_کلیدها[i].code1)
	    بازگشت OK;
    بازگشت FAIL;
}

    تهی
gui_mch_forked(تهی)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    ایستا تهی
gui_gtk4_update_scrollbar_size(تهی)
{
    win_T	*wp;
    صحیح		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_چپ].id;

	اگر (sbar == پوچ || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_چپ]
		    && wp->w_scrollbars[SBAR_راست].id != پوچ))
	    sbar = wp->w_scrollbars[SBAR_راست].id;

	اگر (sbar != پوچ && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    صحیح		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_size(sbar, &min, &nat);
	    sw = MAX(min.width, nat.width);
	    اگر (sw > 0)
	    {
		w = sw;
		بشکن;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    اگر (sbar != پوچ && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	صحیح		    sh;

	gtk_widget_get_preferred_size(sbar, &min, &nat);
	sh = MAX(min.height, nat.height);

	اگر (sh > 0)
	    h = sh;
    }

    اگر (w != -1)
	gui.scrollbar_width = w;
    اگر (h != -1)
	gui.scrollbar_height = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    تهی
gui_mch_enable_scrollbar(scrollbar_T *sb, صحیح flag)
{
    اگر (sb->id != پوچ)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_set_visible(sb->id, flag);

	اگر (!was_visible && flag)
	    gui_gtk4_update_scrollbar_size();
    }
}

#اگر defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    تهی
gui_mch_menu_grey(vimmenu_T *menu, صحیح grey)
{
    اگر (menu->id == پوچ)
	بازگشت;
    gtk_widget_set_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    تهی
gui_mch_menu_hidden(vimmenu_T *menu, صحیح hidden)
{
    اگر (menu->id == پوچ)
	بازگشت;
    gtk_widget_set_visible(menu->id, !hidden);
    gui_mch_update();
}

    تهی
gui_mch_draw_menubar(تهی)
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
    تهی
gui_mch_show_tabline(صحیح showit)
{
    اگر (gui.tabline == پوچ)
	بازگشت;
    gtk_widget_set_visible(gui.tabline, showit);
    gui_mch_update();
}

    صحیح
gui_mch_showing_tabline(تهی)
{
    بازگشت gui.tabline != پوچ && gtk_widget_get_visible(gui.tabline);
}

ایستا صحیح ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
ایستا صحیح tabpage_hover = 0;

    ایستا تهی
tabpage_enter_cb(
	GtkEventController  *controller UNUSED,
	دوگانه		    x UNUSED,
	دوگانه		    y UNUSED,
	تهی		    *udata)
{
    tabpage_hover = GPOINTER_TO_INT(udata);
}

    ایستا تهی
tabline_enter_cb(
	GtkEventController  *controller UNUSED,
	دوگانه		    x UNUSED,
	دوگانه		    y UNUSED,
	تهی		    *udata UNUSED)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    ایستا تهی
tabline_add_css(تهی)
{
    GtkCssProvider  *provider;
    ایستا ثابت نویسه *css =
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
# اگر GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
# وگرنه
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
    ایستا تهی
tab_close_clicked_cb(GtkButton *button UNUSED, gpointer data)
{
    صحیح idx = gtk_notebook_page_رقم(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(data));

    اگر (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    تهی
gui_mch_update_tabline(تهی)
{
    GtkWidget	*page;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    صحیح		nr = 0;
    صحیح		tab_رقم;
    صحیح		curtabidx = 0;
    char_u	*labeltext;

    اگر (gui.tabline == پوچ)
	بازگشت;

    ignore_tabline_evt = TRUE;

    برای (tp = first_tabpage; tp != پوچ; tp = tp->tp_next, ++nr)
    {
	GtkNotebookPage	    *page_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	اگر (tp == curtab)
	    curtabidx = nr;

	tab_رقم = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	اگر (page == پوچ)
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

	    gtk_notebook_درج_page(GTK_NOTEBOOK(gui.tabline),
		    page, event_box, nr);
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page, TRUE);
	    g_object_set(gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page),
		    "tab-expand", TRUE, پوچ);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	page_widget = gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_TO_POINTER(tab_رقم));
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(page_widget, "tab", &tab_widget, پوچ);
	gtk_widget_add_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	label = g_object_get_data(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	اگر (label != پوچ && GTK_IS_LABEL(label))
	    gtk_label_set_text(GTK_LABEL(label), (ثابت نویسه *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_widget_set_tooltip_text(event_box, (ثابت gchar *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    وقتی (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != پوچ)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    اگر (nr == 1)
	gtk_widget_add_css_class(gui.tabline, "single-tab");
    وگرنه
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    اگر (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    تهی
gui_mch_set_curtab(صحیح nr)
{
    اگر (gui.tabline != پوچ)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    ایستا تهی
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    اگر (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    ایستا تهی
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    اگر (ignore_tabline_evt)
	بازگشت;

    اگر ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    وگرنه
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ایستا تهی
tabline_menu_event_cb(VimMenuItem *item UNUSED, VimMenuItemEvent event, تهی *udata)
{
    اگر (event == VIM_MENU_ITEM_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_TO_INT(udata));
}

    ایستا تهی
add_tabline_menu_item(VimMenu *menu, ثابت نویسه *name, صحیح resp)
{
    VimMenuItem *item = VIM_MENU_ITEM(vim_menu_item_new(name,
		tabline_menu_event_cb, GINT_TO_POINTER(resp)));

    vim_menu_درج_item(menu, item, -1);
}

/*
 * Create a menu for the tab line.
 */
    ایستا VimMenu *
create_tabline_popup_menu(تهی)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    add_tabline_menu_item(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, _("New Tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    بازگشت menu;
}

    ایستا تهی
tabline_menu_press_event(
	GtkGestureClick *gesture,
	صحیح		n_press UNUSED,
	دوگانه		x,
	دوگانه		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    اگر (but == GDK_BUTTON_SECONDARY)
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
    وگرنه اگر (but == GDK_BUTTON_PRIMARY)
    {
	اگر (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    وگرنه اگر (but == GDK_BUTTON_میانه)
    {
	اگر (tabpage_hover != 0)
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

#اگر defined(FEAT_SIGN_ICONS)
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)

    تهی
gui_mch_drawsign(صحیح row, صحیح col, صحیح typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    اگر (sign == پوچ)
	بازگشت;

    vim_draw_area_add_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    row, col, SIGN_WIDTH, SIGN_HEIGHT);

    gtk_widget_queue_draw(gui.drawarea);
}

    تهی *
gui_mch_register_sign(char_u *signfile)
{
    اگر (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GError	    *error = پوچ;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ثابت نویسه *)signfile,
							    &error);
	اگر (sign != پوچ)
	    بازگشت sign;

	semsg("E255: %s", error->message);
	g_error_free(error);
    }
    بازگشت پوچ;
}

    تهی
gui_mch_destroy_sign(تهی *sign)
{
    اگر (sign != پوچ)
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
    صحیح
gui_gtk_draw_string(صحیح row, صحیح col, char_u *s, صحیح len, صحیح flags)
{
    صحیح cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    row, col, s, len, flags);

    اگر (gui.drawarea != پوچ)
	gtk_widget_queue_draw(gui.drawarea);

    بازگشت cells;
}

    صحیح
gui_get_x11_windis(Window *win UNUSED, Display **dis UNUSED)
{
    // GTK4: not applicable
    بازگشت FAIL;
}

#اگر defined(FEAT_MENU)
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
    ایستا char_u *
translate_mnemonic_tag(char_u *name, صحیح use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    صحیح	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    اگر (name == پوچ)
	بازگشت پوچ;

    برای (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	اگر (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    اگر (buf != پوچ)
    {
	pdest = buf;
	برای (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    اگر (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    وگرنه اگر (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    وگرنه اگر (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    وگرنه اگر (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    بازگشت buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    تهی
gui_gtk_set_mnemonics(صحیح enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	اگر (menu->id == پوچ)
	    ادامه;

	name = translate_mnemonic_tag(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	اگر (VIM_IS_MENU_BAR_ITEM(menu->id))
	    vim_menu_bar_item_set_text(VIM_MENU_BAR_ITEM(menu->id),
		    (ثابت نویسه *)name);
	vim_free(name);
    }
}

    ایستا تهی
popupmenu_closed_cb(GtkWidget *popover, تهی *udata UNUSED)
{
    gtk_widget_unparent(popover);
    اگر (gui.drawarea != پوچ)
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    ایستا تهی
gui_gtk_popup_at(vimmenu_T *menu, صحیح x, صحیح y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    اگر (menu == پوچ || menu->submenu_id == پوچ)
	بازگشت;

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
	    G_CALLBACK(popupmenu_closed_cb), پوچ);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    تهی
gui_make_popup(char_u *path_name, صحیح mouse_pos)
{
    vimmenu_T	*menu;
    صحیح		x, y;

    menu = gui_find_menu(path_name);
    اگر (menu == پوچ || menu->submenu_id == پوچ)
	بازگشت;

    اگر (mouse_pos)
	gui_mch_getmouse(&x, &y);
    وگرنه
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#endif // FEAT_MENU

    صحیح
get_menu_tool_width(تهی)
{
    بازگشت 0;
}

    صحیح
get_menu_tool_height(تهی)
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

    صحیح height = 0;

    برای (صحیح i = 0; i < (صحیح)ARRAY_LENGTH(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	صحیح		h;

	اگر (widgets[i] == پوچ || !gtk_widget_get_visible(widgets[i]))
	    ادامه;

	h = gtk_widget_get_height(widgets[i]);

	اگر (h == 0)
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
	وگرنه
	    height += h;
    }
    بازگشت height;
}

/*
 * Update selection targets for regular and primary selections.
 */
    تهی
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    اگر (cbd == &clip_plus)
	gdk_content_provider_content_changed(gui.regular_provider);
    وگرنه
	gdk_content_provider_content_changed(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    ایستا GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    اگر (gui.mainwin == پوچ)
	بازگشت پوچ;

    display = gtk_widget_get_display(gui.mainwin);
    اگر (display == پوچ)
	بازگشت پوچ;

    اگر (cbd == &clip_plus)
    {
	اگر (provider != پوچ)
	    *provider = gui.regular_provider;
	بازگشت gdk_display_get_clipboard(display);
    }
    وگرنه
    {
	اگر (provider != پوچ)
	    *provider = gui.primary_provider;
	بازگشت gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    ایستا تهی
clip_read_cb(GdkClipboard *cb, GAsyncResult *result, ClipReadData *crd)
{
    GInputStream    *in_stream;
    ثابت نویسه	    *mime_type;

    in_stream = gdk_clipboard_read_finish(cb, result, &mime_type, پوچ);
    اگر (in_stream == پوچ)
    {
	read_data_free(&crd->rd);
	بازگشت;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_درون_داد_stream_read_async(in_stream, crd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_بالا,
	    crd->rd.cancel, (GAsyncReadyCallback)read_data_درون_داد_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    تهی
clip_mch_request_selection(Clipboard_T *cbd)
{
    ایستا ثابت نویسه *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	پوچ
    };

    GdkClipboard *clipboard;
    ClipReadData *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, پوچ);
    اگر (clipboard == پوچ)
	بازگشت;

    crd = (ClipReadData *)read_data_new(READ_DATA_CLIP, اندازه(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_PRIORITY_بالا, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t start;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	start = time(پوچ);
	وقتی (!g_cancellable_is_cancelled(cancel)
		&& time(پوچ) < start + 3)
	    g_main_context_iteration(پوچ, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

ایستا صحیح in_clipboard_set = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    تهی
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

    ایستا تهی
clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data)
{
    Clipboard_T *cbd = (Clipboard_T *)user_data;

    اگر (in_clipboard_set)
	بازگشت;
    اگر (gdk_clipboard_is_local(clipboard))
	بازگشت;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    صحیح
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    صحیح			ret;

    اگر (cb == پوچ)
	بازگشت FAIL;

    in_clipboard_set = TRUE;
    ret = gdk_clipboard_set_content(cb, cp);
    in_clipboard_set = FALSE;

    بازگشت ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    تهی
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, پوچ);
    اگر (clipboard == پوچ)
	بازگشت;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    اگر (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_set_content(clipboard, پوچ);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(تهی)
{
    بازگشت gtk4_main_loop_level;
}

    تهی
gtk_main_quit(تهی)
{
    gtk4_main_loop_quit = TRUE;
}

#اگر defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
ایستا ثابت نویسه *mshape_css_names[] =
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

    تهی
mch_set_mouse_shape(صحیح shape)
{
    GdkCursor	*c;
    ثابت نویسه	*css_name = "default";

    اگر (gui.drawarea == پوچ)
	بازگشت;

    اگر (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
    وگرنه
    {
	اگر (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	وگرنه اگر (shape < (صحیح)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
	وگرنه
	    بازگشت;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, پوچ);
	gtk_widget_set_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    اگر (shape != MSHAPE_HIDE)
	last_shape = shape;
}

#endif // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



ایستا صحیح last_text_area_w = 0;
ایستا صحیح last_text_area_h = 0;

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
ایستا ثابت نویسه * ثابت toolbar_icon_names[] =
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

    ایستا تهی
toolbar_button_clicked_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    ایستا GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = پوچ;

    // Try specified icon file first
    اگر (menu->iconfile != پوچ)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	اگر (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (ثابت نویسه *)buf, پوچ);

	    اگر (texture != پوچ)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_set_size_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    اگر (image == پوچ)
    {
	ثابت نویسه *icon_name = "image-missing";
	صحیح n = (صحیح)ARRAY_LENGTH(toolbar_icon_names);

	اگر (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_names[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    بازگشت image;
}
# endif

    ایستا تهی
menu_button_clicked_cb(
	VimMenuItem	    *item UNUSED,
	VimMenuItemEvent    event,
	vimmenu_T	    *menu)
{
    اگر (event == VIM_MENU_ITEM_CLICKED)
	gui_menu_cb(menu);
    وگرنه اگر (event == VIM_MENU_ITEM_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	ایستا gboolean did_پیام = FALSE;

	اگر (State & MODE_CMDLINE)
	    بازگشت;

	tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	اگر (tooltip != پوچ && utf_valid_string(tooltip, پوچ))
	{
	    پیام((نویسه *)tooltip);
	    did_پیام = TRUE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	وگرنه اگر (did_پیام)
	{
	    پیام("");
	    did_پیام = FALSE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	CONVERT_TO_UTF8_FREE(tooltip);
    }
}

    تهی
gui_mch_add_menu(vimmenu_T *menu, صحیح idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;
    gboolean	use_mnemonic;
    char_u	*text;

    اگر (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_set_parent(menu->submenu_id, gui.drawarea);
	بازگشت;
    }

    parent = menu->parent;

    اگر ((parent != پوچ && parent->submenu_id == پوچ)
	    || !menu_is_menubar(menu->name))
	بازگشت;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = parent != پوچ || p_wak[0] != 'n';
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    اگر (parent != پوچ)
    {
	parent_widget = parent->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_item_new(
		    (ثابت نویسه *)text, پوچ, پوچ));

	vim_menu_item_set_submenu(VIM_MENU_ITEM(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_درج_item(VIM_MENU(parent_widget),
		VIM_MENU_ITEM(menu->id), idx);
    }
    وگرنه
    {
	parent_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_item_new(
		    (ثابت نویسه *)text, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_درج_item(VIM_MENU_BAR(parent_widget),
		VIM_MENU_BAR_ITEM(menu->id), idx);
    }

    vim_free(text);
}

    تهی
gui_mch_add_menu_item(vimmenu_T *menu, صحیح idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    اگر (parent != پوچ && menu_is_toolbar(parent->name))
    {
	اگر (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_درج_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	وگرنه
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*text;
	    char_u	*tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    اگر (tooltip != پوچ && !utf_valid_string(tooltip, پوچ))
		CONVERT_TO_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_درج_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ثابت نویسه *)text, idx);
	    gtk_widget_set_tooltip_text(btn, (ثابت gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
	بازگشت;
    }
# endif

    // Menu items (non-toolbar)
    اگر (parent == پوچ || parent->submenu_id == پوچ)
	بازگشت;

    اگر (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_درج_separator(
		VIM_MENU(parent->submenu_id), idx));
    }
    وگرنه
    {
	char_u	    *text;
	char_u	    *accel_text = پوچ;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	text = translate_mnemonic_tag(menu->name, use_mnemonic);

	اگر (menu->actext != پوچ && menu->actext[0] != NUL)
	    accel_text = CONVERT_TO_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_item_new((ثابت نویسه *)text,
		    (VimMenuItemFunc)menu_button_clicked_cb, menu));

	اگر (accel_text != پوچ)
	    vim_menu_item_set_accel(VIM_MENU_ITEM(menu->id),
		    (ثابت نویسه *)accel_text);

	vim_menu_درج_item(VIM_MENU(parent->submenu_id),
		VIM_MENU_ITEM(menu->id), idx);

	vim_free(text);
	CONVERT_TO_UTF8_FREE(accel_text);
    }
}

    تهی
gui_mch_toggle_tearoffs(صحیح enable UNUSED)
{
    // GTK4: tearoff menus don't exist.
}

    تهی
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    اگر (menu->id == پوچ)
	بازگشت;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    اگر (tooltip != پوچ && utf_valid_string(tooltip, پوچ))
	gtk_widget_set_tooltip_text(menu->id, (ثابت نویسه *)tooltip);
    CONVERT_TO_UTF8_FREE(tooltip);
}

    تهی
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    اگر (menu->parent != پوچ && menu_is_toolbar(menu->parent->name))
    {
	اگر (menu->id != پوچ)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = پوچ;
	}
	بازگشت;
    }

    // For popup menus, unparent the menu as well
    اگر (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	اگر (menu->submenu_id != پوچ)
	    gtk_widget_unparent(menu->submenu_id);
    }
    وگرنه اگر (menu->parent == پوچ && menu->id != پوچ)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    وگرنه اگر (menu->id != پوچ)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->parent->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    تهی
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    صحیح x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    ایستا تهی
show_menubar_popover(تهی)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    اگر (gui.menubar == پوچ || gui.drawarea == پوچ)
	بازگشت;

    اگر (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), پوچ);
	بازگشت;
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
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), پوچ);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#endif // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    تهی
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    GtkAdjustment *adj;

    اگر (sb->id == پوچ)
	بازگشت;
    اگر (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	بازگشت;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_set_lower(adj, 0.0);
    gtk_adjustment_set_upper(adj, (gdouble)max + 1);
    gtk_adjustment_set_مقدار(adj, (gdouble)val);
    gtk_adjustment_set_step_increment(adj, 1.0);
    gtk_adjustment_set_page_increment(adj, (gdouble)(size > 2 ? size - 2 : 1));
    gtk_adjustment_set_page_size(adj, (gdouble)size);
}

    تهی
gui_mch_set_scrollbar_pos(scrollbar_T *sb, صحیح x, صحیح y, صحیح w, صحیح h)
{
    اگر (sb->id != پوچ)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    صحیح
gui_mch_get_scrollbar_xpadding(تهی)
{
    صحیح formwin_w = gtk_widget_get_width(gui.formwin);
    صحیح sbar_w = 0;
    صحیح xpad;

    اگر (gui.which_scrollbars[SBAR_چپ])
	sbar_w += gui.scrollbar_width;
    اگر (gui.which_scrollbars[SBAR_راست])
	sbar_w += gui.scrollbar_width;

    xpad = formwin_w - last_text_area_w - sbar_w;
    بازگشت (xpad < 0) ? 0 : xpad;
}

    صحیح
gui_mch_get_scrollbar_ypadding(تهی)
{
    صحیح formwin_h = gtk_widget_get_height(gui.formwin);
    صحیح ypad;

    ypad = formwin_h - last_text_area_h;
    اگر (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_height;

    بازگشت (ypad < 0) ? 0 : ypad;
}

    ایستا تهی
adjustment_مقدار_changed(GtkAdjustment *adj, gpointer data UNUSED)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_data(G_OBJECT(adj), "vim-sb");
    long مقدار = (long)gtk_adjustment_get_مقدار(adj);

    اگر (sb != پوچ)
	gui_drag_scrollbar(sb, مقدار, FALSE);
}

    تهی
gui_mch_create_scrollbar(scrollbar_T *sb, صحیح orient)
{
    GtkAdjustment *adj;
    اگر (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, پوچ);
    وگرنه
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, پوچ);

    gtk_widget_add_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_set_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_set_data(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_مقدار_changed), پوچ);
}

    تهی
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    اگر (sb->id != پوچ)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = پوچ;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    تهی
gui_mch_set_text_area_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    اگر (w <= 0 || h <= 0)
	بازگشت;
    last_text_area_w = w;
    last_text_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    تهی
gui_gtk4_calculate_bleed(صحیح width, صحیح height)
{
    صحیح old_راست = gui.bleed_راست;
    صحیح old_bot = gui.bleed_bot;

    gui.bleed_راست = width - last_text_area_w;
    gui.bleed_bot = height - last_text_area_h;

    اگر (gui.which_scrollbars[SBAR_چپ])
	gui.bleed_راست -= gui.scrollbar_width;
    اگر (gui.which_scrollbars[SBAR_راست])
	gui.bleed_راست -= gui.scrollbar_width;
    اگر (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_height;

    // Not sure if this can happen, but be safe...
    اگر (gui.bleed_راست < 0)
	gui.bleed_راست = 0;
    اگر (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    اگر (old_راست != gui.bleed_راست || old_bot != gui.bleed_bot)
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
تعریفنوع ساختار {
    GFile	*result;
    gboolean	done;
} FileDialogData;

    ایستا تهی
file_dialog_open_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_open_finish(
		    GTK_FILE_DIALOG(source), res, پوچ);
    fdd->done = TRUE;
}

    ایستا تهی
file_dialog_save_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_save_finish(
		    GTK_FILE_DIALOG(source), res, پوچ);
    fdd->done = TRUE;
}

    ایستا تهی
file_dialog_folder_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_select_folder_finish(
		    GTK_FILE_DIALOG(source), res, پوچ);
    fdd->done = TRUE;
}

    char_u *
gui_mch_browse(صحیح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext UNUSED,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    FileDialogData  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *result = پوچ;
    GListStore	    *filters;

    title = CONVERT_TO_UTF8(title);

    اگر (initdir == پوچ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    وگرنه اگر (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    add_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    اگر (title != پوچ)
	gtk_file_dialog_set_title(dlg, (ثابت نویسه *)title);

    {
	GFile *dir = g_file_new_for_path((ثابت نویسه *)dirbuf);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    اگر (filter != پوچ && *filter != NUL)
    {
	صحیح		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	اگر (patt != پوچ)
	{
	    وقتی (p != پوچ && *p != NUL)
	    {
		اگر (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    اگر (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    وگرنه
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			اگر (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    اگر (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		وگرنه
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

    اگر (saving && dflt != پوچ && *dflt != NUL)
	gtk_file_dialog_set_initial_name(dlg, (ثابت نویسه *)dflt);

    fdd.result = پوچ;
    fdd.done = FALSE;

    اگر (saving)
	gtk_file_dialog_save(dlg, GTK_WINDOW(gui.mainwin), پوچ,
		file_dialog_save_cb, &fdd);
    وگرنه
	gtk_file_dialog_open(dlg, GTK_WINDOW(gui.mainwin), پوچ,
		file_dialog_open_cb, &fdd);

    وقتی (!fdd.done)
	g_main_context_iteration(پوچ, TRUE);

    اگر (fdd.result != پوچ)
    {
	نویسه *path = g_file_get_path(fdd.result);
	اگر (path != پوچ)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    بازگشت result;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    FileDialogData	fdd;
    char_u		*result = پوچ;

    title = CONVERT_TO_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    اگر (title != پوچ)
	gtk_file_dialog_set_title(dlg, (ثابت نویسه *)title);

    اگر (initdir != پوچ && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_path((ثابت نویسه *)initdir);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.result = پوچ;
    fdd.done = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_WINDOW(gui.mainwin), پوچ,
	    file_dialog_folder_cb, &fdd);

    وقتی (!fdd.done)
	g_main_context_iteration(پوچ, TRUE);

    اگر (fdd.result != پوچ)
    {
	نویسه *path = g_file_get_path(fdd.result);
	اگر (path != پوچ)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    بازگشت result;
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
    ایستا نویسه **
split_button_string(char_u *button_string, صحیح *n_buttons, نویسه **tofree)
{
    نویسه	    **array;
    char_u	    *p;
    unsigned صحیح    شمار = 1;

    button_string = (char_u *)g_strdup((ثابت نویسه *)button_string);

    برای (p = button_string; *p != NUL; ++p)
	اگر (*p == DLG_BUTTON_SEP)
	    ++شمار;

    array = g_malloc_n(شمار, اندازه(نویسه *));
    شمار = 0;

    اگر (array != پوچ)
    {
	array[شمار++] = (نویسه *)button_string;
	برای (p = button_string; *p != NUL; )
	{
	    اگر (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[شمار++] = (نویسه *)p;
	    }
	    وگرنه اگر (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    وگرنه
		MB_PTR_ADV(p);
	}
    }

    *tofree = (نویسه *)button_string;
    *n_buttons = شمار;
    بازگشت array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    ایستا ثابت نویسه *
dialog_type_to_icon(صحیح type)
{
    انتخاب (type)
    {
	حالت VIM_ERROR:
	    بازگشت "dialog-error-symbolic";
	حالت VIM_WARNING:
	    بازگشت "dialog-warning-symbolic";
	حالت VIM_INFO:
	    بازگشت "dialog-information-symbolic";
	حالت VIM_QUESTION:
	    بازگشت "dialog-question-symbolic";
	پیشفرض:
	    بشکن;
    }
    بازگشت پوچ;
}

// Data associated with each button in the dialog
تعریفنوع ساختار
{
    صحیح		but_idx;
    صحیح		*response;
    gboolean	*done;
} DialogButtonState;

تعریفنوع ساختار
{
    GtkWidget	*win;
    gboolean	*done;
    gboolean	no_alt;
} DialogState;

    ایستا تهی
dialog_button_clicked_cb(GtkButton *button UNUSED, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->done = TRUE;
}

    ایستا gboolean
dialog_کلید_pressed_cb(
	Gtk_Event_Controller_کلید	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	DialogState		*dstate)
{
    اگر (keyval == GDK_کلید_Escape)
    {
	*dstate->done = TRUE;
	بازگشت TRUE;
    }

    اگر (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_mask()))
	بازگشت gtk_widget_mnemonic_activate(dstate->win, FALSE);
    بازگشت FALSE;
}

    ایستا gboolean
dialog_close_request_cb(GtkWindow *win UNUSED, gboolean *win_closed)
{
    *win_closed = TRUE;
    بازگشت FALSE;
}

    صحیح
gui_mch_dialog(
	صحیح	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	صحیح	def_but,
	char_u	*textfield,
	صحیح	ex_cmd UNUSED)
{
    GtkWindow		*win = GTK_WINDOW(gtk_window_new());
    GtkWidget		*vertbox;
    GtkWidget		*message_box;
    ثابت نویسه		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*entry = پوچ;
    char_u		*utf8_title;
    char_u		*utf8_message;
    GtkEventController *کلید_controller;
    DialogButtonState	*but_states = پوچ;
    نویسه		*tofree = پوچ;
    صحیح			response = -1;
    gboolean		done = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_TO_UTF8(title);
    اگر (utf8_title != پوچ)
	gtk_window_set_title(win, (ثابت نویسه *)utf8_title);
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
    gtk_widget_set_margin_بالاترین(vertbox, 24);
    gtk_widget_set_margin_bottom(vertbox, 24);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);

    // Add the message label
    message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(message_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), message_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_to_icon(type);
    اگر (icon_name != پوچ)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(message_box), icon);
    }

    utf8_message = CONVERT_TO_UTF8(message);
    label = gtk_label_new((ثابت نویسه *)utf8_message);
    CONVERT_TO_UTF8_FREE(utf8_message);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(message_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    کلید_controller = gtk_event_controller_کلید_new();
    g_signal_connect(کلید_controller, "key-pressed",
	    G_CALLBACK(dialog_کلید_pressed_cb), &state);
    gtk_widget_add_controller(GTK_WIDGET(win), کلید_controller);
    state.done = &done;
    state.win = GTK_WIDGET(win);

    اگر (textfield != پوچ)
    {
	// Add text entry so user can enter text
	char_u *utf8_text = CONVERT_TO_UTF8(textfield);

	entry = gtk_entry_new();

	اگر (utf8_text != پوچ)
	    gtk_editable_set_text(GTK_EDITABLE(entry), (ثابت نویسه *)utf8_text);
	وگرنه
	    gtk_editable_set_text(GTK_EDITABLE(entry), "");
	CONVERT_TO_UTF8_FREE(utf8_text);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_box_append(GTK_BOX(vertbox), entry);
	state.no_alt = FALSE;
    }
    وگرنه
    {
	GListModel  *controllers;
	صحیح	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_اقلام(controllers);
	برای (صحیح i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_item(controllers, i);
	    اگر (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_set_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_window_set_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    اگر (buttons != پوچ)
    {
	GtkWidget   *but_box;
	نویسه	    **buttons_arr; // Note that array is allocated, not strings
	صحیح	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	اگر (vim_strchr(p_go, GO_VERTICAL) != پوچ)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	وگرنه
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_set_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = split_button_string(buttons, &n_buttons, &tofree);

	but_states = g_malloc_n(n_buttons, اندازه(DialogButtonState));

	برای (صحیح i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_states + i;

	    but_label = CONVERT_TO_UTF8((char_u *)buttons_arr[i]);
	    اگر (but_label == پوچ)
		ادامه;

	    but = gtk_button_new_with_mnemonic((نویسه *)but_label);
	    اگر (i == def_but - 1)
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

    وقتی (!done && !win_closed)
	g_main_context_iteration(پوچ, TRUE);

    اگر (done)
    {
	اگر (textfield != پوچ)
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
    بازگشت response == -1 ? 0 : response + 1;
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

تعریفنوع ساختار
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

ایستا SharedFindReplace find_widgets = {0};
ایستا SharedFindReplace repl_widgets = {0};

    ایستا تهی
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    صحیح			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = GPOINTER_TO_INT(data);

    اگر (flags == FRD_FINDNEXT)
    {
	repl_text = پوچ;
	sfr = &find_widgets;
    }
    وگرنه
    {
	repl_text = (char_u *)gtk_editable_get_text(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(sfr->what));
    direction_down = gtk_check_button_get_active(
	    GTK_CHECK_BUTTON(sfr->down));

    اگر (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    اگر (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

    ایستا تهی
dialog_destroyed_cb(GtkWidget *widget UNUSED, gpointer data)
{
    *(GtkWidget **)data = پوچ;
}

    ایستا تهی
entry_changed_cb(GtkWidget *entry, GtkWidget *dialog)
{
    ثابت gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    اگر (!entry_text)
	بازگشت; // Shouldn't happen

    nonempty = (entry_text[0] != '\0');

    اگر (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    اگر (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

    ایستا gboolean
find_کلید_pressed_cb(
	Gtk_Event_Controller_کلید	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	SharedFindReplace	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    اگر (state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	بازگشت FALSE;

    // the Escape key synthesizes a cancellation action
    اگر (keyval == GDK_کلید_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_window_destroy(GTK_WINDOW(frdp->dialog));
	بازگشت TRUE;
    }

    بازگشت FALSE;
}

    ایستا تهی
entry_activate_cb(GtkWidget *widget UNUSED, تهی *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    ایستا تهی
find_replace_dialog_create(char_u *arg, صحیح do_replace)
{
    SharedFindReplace	*frdp;
    char_u		*entry_text;
    صحیح			wword = FALSE;
    صحیح			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*کلید_controller;

    frdp = do_replace ? &repl_widgets : &find_widgets;
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    اگر (entry_text != پوچ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = entry_text;
	entry_text = string_convert(&output_conv, entry_text, پوچ);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    اگر (frdp->dialog != پوچ)
    {
	اگر (entry_text != پوچ)
	{
	    gtk_editable_set_text(GTK_EDITABLE(frdp->what),
		    (نویسه *)entry_text);
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
	اگر (do_replace && g_utf8_strlen(
		    gtk_editable_get_text(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	بازگشت;
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
    gtk_widget_set_margin_بالاترین(vertbox, 12);
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
    sensitive = (entry_text != پوچ && entry_text[0] != NUL);
    اگر (entry_text != پوچ)
	gtk_editable_set_text(GTK_EDITABLE(frdp->what), (نویسه *)entry_text);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    اگر (do_replace)
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
    وگرنه
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

    کلید_controller = gtk_event_controller_کلید_new();
    g_signal_connect(کلید_controller, "key-pressed",
	    G_CALLBACK(find_کلید_pressed_cb), frdp);
    gtk_widget_add_controller(GTK_WIDGET(frdp->dialog), کلید_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(entry_changed_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_window_set_default_widget(GTK_WINDOW(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
	    GINT_TO_POINTER(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    اگر (do_replace)
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
    اگر (do_replace && entry_text != پوچ && entry_text[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(entry_text);
}

    تهی
gui_mch_find_dialog(exarg_T *eap)
{
    اگر (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    تهی
gui_mch_replace_dialog(exarg_T *eap)
{
    اگر (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    تهی
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
ایستا GtkPrintDialog *print_dialog = پوچ;
ایستا GtkPrintSettings *print_settings = پوچ;
ایستا GtkPageSetup *page_setup = پوچ;
ایستا GtkPrintSetup *print_setup = پوچ;
ایستا GFile *print_file;

    ایستا تهی
print_dialog_cb(GtkPrintDialog *dialog, GAsyncResult *result, gboolean *done)
{
    GError *error = پوچ;
    print_setup = gtk_print_dialog_setup_finish(dialog, result, &error);

    اگر (print_setup == پوچ)
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
	دوگانه		*page_width,
	دوگانه		*page_height)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    done = FALSE;
    char_u	    *temp;

    gtk_print_dialog_set_title(dialog, (ثابت نویسه *)jobname);
    gtk_print_dialog_set_modal(dialog, TRUE);

    print_dialog = پوچ;
    print_setup = پوچ;

    اگر (print_settings == پوچ)
    {
	option_table_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	اگر (opt->present)
	{
	    اگر (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    وگرنه اگر (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    وگرنه
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	اگر (opt->present)
	{
	    اگر (*opt->string == 'y')
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_PORTRAIT);
	    وگرنه
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	اگر (opt->present)
	{
	    gtk_print_settings_set_paper_size(print_settings,
		    gtk_paper_size_new((ثابت نویسه *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	اگر (opt->present)
	{
	    اگر (*opt->string == 'y')
		gtk_print_settings_set_collate(print_settings, TRUE);
	    وگرنه
		gtk_print_settings_set_collate(print_settings, FALSE);
	}
    }

    اگر (page_setup == پوچ)
	page_setup = gtk_page_setup_new();

    gtk_print_dialog_set_print_settings(dialog, print_settings);
    gtk_print_dialog_set_page_setup(dialog, page_setup);

    gtk_print_dialog_setup(dialog, GTK_WINDOW(gui.mainwin), پوچ,
	    (GAsyncReadyCallback)print_dialog_cb, &done);

    وقتی (!done)
	g_main_context_iteration(پوچ, TRUE);

    اگر (print_setup == پوچ)
    {
	g_object_unref(dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_پیام = FALSE;
	بازگشت پوچ;
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
    اگر (temp == پوچ)
    {
	emsg(_(e_cant_find_temp_file_for_writing));
	g_clear_object(&dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_پیام = FALSE;
	بازگشت پوچ;
    }
    print_file = g_file_new_for_path((نویسه *)temp);

    print_dialog = dialog;

    بازگشت temp;
}

    ایستا تهی
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsyncResult	*result,
	GFile		*file)
{
    نویسه *path = g_file_get_path(file);

    (تهی)gtk_print_dialog_print_file_finish(dialog, result, پوچ);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(path);
    g_free(path);
}


    تهی
gui_gtk4_print_finish(تهی)
{
    اگر (print_dialog != پوچ)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_WINDOW(gui.mainwin),
		print_setup, print_file, پوچ,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = پوچ;
	g_clear_pointer(&print_setup, gtk_print_setup_unref);
    }
}
#endif // USE_GTK4_PRINT_DIALOG

#ifdef FEAT_IMAGE_GUI

تعریفنوع ساختار
{
    GdkTexture *texture;
} image_gdk_T;

تعریفنوع ساختار
{
    Gsk_Render_گره   **nodes; // Note that elements may be NULL
    صحیح		    n_nodes;
} image_placement_gdk_T;

    صحیح
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    GdkMemoryFormat fmt;
    size_t	    stride;
    GBytes	    *bytes;
    size_t	    size;
    صحیح		    w, h;

    اگر (ctx == پوچ)
	بازگشت FAIL;

    اگر (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_MEMORY_R8G8B8A8;
    وگرنه
	fmt = GDK_MEMORY_R8G8B8;

    image_get_dimensions(img, &w, &h);

    size = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    bytes = g_bytes_new(img->data, size);
    ctx->texture = gdk_memory_texture_new(w, h, fmt, bytes, stride);
    g_bytes_unref(bytes);

    img->backend_data = ctx;
    بازگشت OK;
}

    تهی
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_data;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    ایستا تهی
clear_nodes(image_placement_gdk_T *ctx)
{
    اگر (ctx->nodes == پوچ || gui.drawarea == پوچ)
	بازگشت;

    برای (صحیح i = 0; i < ctx->n_nodes; i++)
    {
	اگر (ctx->nodes[i] == پوچ)
	    ادامه;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_گره_unref(ctx->nodes[i]);
    }
    g_clear_pointer(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    صحیح
image_placement_gui_init(image_placement_T *place)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    اگر (ctx == پوچ)
	بازگشت FAIL;

    place->backend_data = ctx;
    بازگشت OK;
}

    تهی
image_placement_gui_uninit(image_placement_T *place)
{
    image_placement_gdk_T *ctx = place->backend_data;

    clear_nodes(ctx);
    vim_free(ctx);
}

    تهی
image_placement_gui_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_gdk_T   *ctx = place->backend_data;
    image_gdk_T		    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    صحیح			    n_rects;
    صحیح			    iw, ih;
    Gsk_Render_گره	    *texture_گره;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    اگر (rects == پوچ)
	بازگشت;

    clear_nodes(ctx);
    اگر (n_rects == 0)
	بازگشت;

    image_get_dimensions(img, &iw, &ih);
    texture_گره = gsk_texture_گره_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(place->col) - FILL_X(place->crop_box.x1),
		FILL_Y(place->row) - FILL_Y(place->crop_box.y1)
		+ FILL_Y(place->row_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    اگر (texture_گره == پوچ)
	// Not sure if this can happen...
	بازگشت;

    ctx->nodes = g_malloc_n(n_rects, اندازه(Gsk_Render_گره *));
    ctx->n_nodes = n_rects;

    برای (صحیح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحیح		row, col;
	صحیح		posx, posy;
	صحیح		x, y, w, h;
	Gsk_Render_گره	*گره;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	گره = gsk_clip_گره_new(texture_گره,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	اگر (گره != پوچ)
	    vim_draw_area_add_external(VIM_DRAW_AREA(gui.drawarea), گره);

	ctx->nodes[i] = گره;
    }
    gsk_render_گره_unref(texture_گره);
}

    تهی
image_placement_gui_clear(image_placement_T *place)
{
    image_placement_gdk_T   *ctx = place->backend_data;

    clear_nodes(ctx);
}

#endif

#endif // FEAT_GUI_GTK
