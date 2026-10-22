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

#অন্তর্ভাৱ "vim.h"

#ifdef FEAT_GUI_GTK

#অন্তর্ভাৱ <gdk/gdkkeysyms.h>
#অন্তর্ভাৱ <gdk/gdk.h>
#অন্তর্ভাৱ <gtk/gtk.h>
#অন্তর্ভাৱ "gui_gtk4_f.h"
#অন্তর্ভাৱ "gui_gtk4_cb.h"
#অন্তর্ভাৱ "gui_gtk4_da.h"
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
#define No_মান		0x0000
#define Width_মান	0x0004
#define Height_মান	0x0008

    স্থির পূর্ণাঙ্ক
vim_parse_geometry(ধ্রুৱ ৱর্ণ *str, unsigned পূর্ণাঙ্ক *width, unsigned পূর্ণাঙ্ক *height)
{
    পূর্ণাঙ্ক mask = No_মান;
    ৱর্ণ *end;
    long val;

    যদি (str == রিক্ত || *str == NUL)
	ঘূৰাই mask;

    // Parse width
    যদি (*str != '+' && *str != '-')
    {
	val = strtol(str, &end, 10);
	যদি (end != str)
	{
	    *width = (unsigned পূর্ণাঙ্ক)val;
	    mask |= Width_মান;
	    str = end;
	}
    }

    // Parse 'x' or 'X' separator and height
    যদি (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &end, 10);
	যদি (end != str)
	{
	    *height = (unsigned পূর্ণাঙ্ক)val;
	    mask |= Height_মান;
	    str = end;
	}
    }

    ঘূৰাই mask;
}

#যদি defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
স্থির পূর্ণাঙ্ক last_shape = 0;
#endif

#define DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
স্থির গণনা {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
স্থির পূর্ণাঙ্ক gtk4_main_loop_level = 0;
স্থির পূর্ণাঙ্ক gtk4_main_loop_quit = FALSE;

#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

প্রকারনির্দেশ gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
স্থির সংরচনা special_চাবি
{
    guint চাবি_sym;
    char_u code0;
    char_u code1;
}
ধ্রুৱ special_চাবিগুলি[] =
{
    {GDK_চাবি_Up,	'k', 'u'},
    {GDK_চাবি_Down,	'k', 'd'},
    {GDK_চাবি_বাম,	'k', 'l'},
    {GDK_চাবি_ডান,	'k', 'r'},
    {GDK_চাবি_F1,	'k', '1'},
    {GDK_চাবি_F2,	'k', '2'},
    {GDK_চাবি_F3,	'k', '3'},
    {GDK_চাবি_F4,	'k', '4'},
    {GDK_চাবি_F5,	'k', '5'},
    {GDK_চাবি_F6,	'k', '6'},
    {GDK_চাবি_F7,	'k', '7'},
    {GDK_চাবি_F8,	'k', '8'},
    {GDK_চাবি_F9,	'k', '9'},
    {GDK_চাবি_F10,	'k', ';'},
    {GDK_চাবি_F11,	'F', '1'},
    {GDK_চাবি_F12,	'F', '2'},
    {GDK_চাবি_Help,	'%', '1'},
    {GDK_চাবি_Undo,	'&', '8'},
    {GDK_চাবি_Back_Space,	'k', 'b'},
    {GDK_চাবি_ঢোকাও,	'k', 'I'},
    {GDK_চাবি_Delete,	'k', 'D'},
    {GDK_চাবি_Home,	'k', 'h'},
    {GDK_চাবি_End,	'@', '7'},
    {GDK_চাবি_Prior,	'k', 'P'},
    {GDK_চাবি_Next,	'k', 'N'},
    {GDK_চাবি_Print,	'%', '9'},
    {GDK_চাবি_KP_বাম,	'k', 'l'},
    {GDK_চাবি_KP_ডান,	'k', 'r'},
    {GDK_চাবি_KP_Up,	'k', 'u'},
    {GDK_চাবি_KP_Down,	'k', 'd'},
    {GDK_চাবি_KP_ঢোকাও,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_চাবি_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_চাবি_KP_Home,	'K', '1'},
    {GDK_চাবি_KP_End,	'K', '4'},
    {GDK_চাবি_KP_Prior,	'K', '3'},
    {GDK_চাবি_KP_Next,	'K', '5'},
    {GDK_চাবি_KP_Add,	'K', '6'},
    {GDK_চাবি_KP_Subtract, 'K', '7'},
    {GDK_চাবি_KP_Divide,	'K', '8'},
    {GDK_চাবি_KP_Multiply, 'K', '9'},
    {GDK_চাবি_KP_Enter,	'K', 'A'},
    {GDK_চাবি_KP_Decimal,	'K', 'B'},
    {GDK_চাবি_KP_0,	'K', 'C'},
    {GDK_চাবি_KP_1,	'K', 'D'},
    {GDK_চাবি_KP_2,	'K', 'E'},
    {GDK_চাবি_KP_3,	'K', 'F'},
    {GDK_চাবি_KP_4,	'K', 'G'},
    {GDK_চাবি_KP_5,	'K', 'H'},
    {GDK_চাবি_KP_6,	'K', 'I'},
    {GDK_চাবি_KP_7,	'K', 'J'},
    {GDK_চাবি_KP_8,	'K', 'K'},
    {GDK_চাবি_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    স্থির পূর্ণাঙ্ক
keyval_to_string(unsigned পূর্ণাঙ্ক keyval, char_u *string)
{
    পূর্ণাঙ্ক		len;
    guint32	uc;

    uc = gdk_keyval_to_unicode(keyval);
    যদি (uc != 0)
    {
	len = utf_char2bytes((পূর্ণাঙ্ক)uc, string);
    }
    অন্যথা
    {
	len = 1;
	ৱিকল্প (keyval)
	{
	    পক্ষ GDK_চাবি_Tab: পক্ষ GDK_চাবি_KP_Tab: পক্ষ GDK_চাবি_ISO_বাম_Tab:
		string[0] = TAB;
		ভঙ্গ;
	    পক্ষ GDK_চাবি_Linefeed:
		string[0] = NL;
		ভঙ্গ;
	    পক্ষ GDK_চাবি_Return: পক্ষ GDK_চাবি_ISO_Enter: পক্ষ GDK_চাবি_3270_Enter:
		string[0] = CAR;
		ভঙ্গ;
	    পক্ষ GDK_চাবি_Escape:
		string[0] = ESC;
		ভঙ্গ;
	    স্ৱভাৱ:
		len = 0;
		ভঙ্গ;
	}
    }
    string[len] = NUL;
    ঘূৰাই len;
}

    স্থির পূর্ণাঙ্ক
modifiers_gdk2vim(guint state)
{
    পূর্ণাঙ্ক modifiers = 0;

    যদি (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    যদি (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    যদি (state & GDK_ALT_MASK)
	modifiers |= MOD_MASK_ALT;
    যদি (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    যদি (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;

    ঘূৰাই modifiers;
}

স্থির GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
স্থির gboolean চাবি_press_event(Gtk_Event_Controller_চাবি *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
স্থির শূন্য চাবি_release_event(Gtk_Event_Controller_চাবি *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
স্থির শূন্য button_press_event(GtkGestureClick *gesture, পূর্ণাঙ্ক n_press, দ্ৱিগুণ x, দ্ৱিগুণ y, gpointer data);
স্থির শূন্য button_release_event(GtkGestureClick *gesture, পূর্ণাঙ্ক n_press, দ্ৱিগুণ x, দ্ৱিগুণ y, gpointer data);
স্থির শূন্য motion_notify_event(GtkEventControllerMotion *controller, দ্ৱিগুণ x, দ্ৱিগুণ y, gpointer data);
স্থির শূন্য enter_notify_event(GtkEventControllerMotion *controller, দ্ৱিগুণ x, দ্ৱিগুণ y, gpointer data);
স্থির gboolean scroll_event(GtkEventControllerScroll *controller, দ্ৱিগুণ dx, দ্ৱিগুণ dy, gpointer data);
স্থির শূন্য focus_in_event(GtkEventControllerFocus *controller, gpointer data);
স্থির শূন্য focus_out_event(GtkEventControllerFocus *controller, gpointer data);
#ifdef FEAT_DND
স্থির gboolean drop_cb(GtkDropTargetAsync *target, GdkDrop *drop, দ্ৱিগুণ x, দ্ৱিগুণ y, শূন্য *data);
#endif
#ifdef FEAT_GUI_TABLINE
স্থির শূন্য tabline_enter_cb(GtkEventController *controller, দ্ৱিগুণ x, দ্ৱিগুণ y, শূন্য *udata);
স্থির শূন্য on_select_tab(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
স্থির শূন্য on_tab_reordered(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
স্থির VimMenu *create_tabline_popup_menu(শূন্য);
স্থির শূন্য tabline_add_css(শূন্য);
স্থির শূন্য tabline_menu_press_event(GtkGestureClick *gesture, পূর্ণাঙ্ক n_press, দ্ৱিগুণ x, দ্ৱিগুণ y, GtkWidget *popover);
#endif
স্থির শূন্য mainwin_destroy_cb(GObject *object, gpointer data);
স্থির gboolean delete_event_cb(GtkWindow *window, gpointer data);
স্থির পূর্ণাঙ্ক query_pointer_pos(পূর্ণাঙ্ক *x, পূর্ণাঙ্ক *y, GdkModifierType *state);
স্থির শূন্য mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_data);
স্থির শূন্য set_form_size(পূর্ণাঙ্ক width, পূর্ণাঙ্ক height);
স্থির শূন্য drawarea_realize_cb(GtkWidget *widget, gpointer data);
স্থির শূন্য drawarea_unrealize_cb(GtkWidget *widget, gpointer data);
স্থির শূন্য clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data);
#ifdef FEAT_MENU
স্থির শূন্য show_menubar_popover(শূন্য);
#endif

স্থির ধ্রুৱ ৱর্ণ *prgname = রিক্ত;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    স্থির gboolean
arg_match(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *name, ৱর্ণ **মান)
{
    size_t  len = strlen(name);

    যদি (strncmp(s, name, len) != 0)
	ঘূৰাই FALSE;
    যদি (s[len] == '=')
	*মান = (ৱর্ণ *)s + len + 1;
    অন্যথা যদি (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	ঘূৰাই FALSE;
    ঘূৰাই TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    শূন্য
gui_mch_prepare(পূর্ণাঙ্ক *argc, ৱর্ণ **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    পূর্ণাঙ্ক	i = 0;

    যেতিয়ালৈ (i < *argc)
    {
	ৱর্ণ	*s = argv[i];
	ৱর্ণ	*মান = রিক্ত;
	পূর্ণাঙ্ক	has_inline_মান = FALSE;
	পূর্ণাঙ্ক	n_strip;

	যদি (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    অনুৱর্ত;
	}

	যদি (strchr(s, '=') != রিক্ত)
	    has_inline_মান = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	যদি (!has_inline_মান)
	{
	    যদি (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		মান = argv[i + 1];
	    অন্যথা
		মান = রিক্ত;
	}

	যদি (arg_match(s, "-fn", &মান) || arg_match(s, "-font", &মান))
	    font_argument = মান;
	অন্যথা যদি (arg_match(s, "-geom", &মান)
		|| arg_match(s, "-geometry", &মান))
	{
	    যদি (মান != রিক্ত)
		gui.geom = vim_strsave((char_u *)মান);
	}
	অন্যথা যদি (arg_match(s, "-bg", &মান)
		|| arg_match(s, "-background", &মান))
	    background_argument = মান;
	অন্যথা যদি (arg_match(s, "-fg", &মান)
		|| arg_match(s, "-foreground", &মান))
	    foreground_argument = মান;
#ifdef FEAT_NETBEANS_INTG
	অন্যথা যদি (strncmp(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_মান = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#endif
	অন্যথা যদি (arg_match(s, "--prg-name", &মান))
	    // GTK4 specific
	    prgname = মান;
	অন্যথা
	{
	    i++;
	    অনুৱর্ত;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	যদি (মান != রিক্ত && !has_inline_মান)
	    n_strip = 2;

	যদি (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    যদি (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * পরিমাণ(ৱর্ণ *));
	    argv[*argc] = রিক্ত;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    শূন্য
gui_mch_free_all(শূন্য)
{
}

    স্থির guint
timeout_add(পূর্ণাঙ্ক time, timeout_cb_type (*callback)(gpointer), পূর্ণাঙ্ক *flagp)
{
    ঘূৰাই g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    স্থির শূন্য
timeout_remove(guint timer)
{
    g_source_remove(timer);
}

স্থির long_u blink_waittime = 700;
স্থির long_u blink_ontime = 400;
স্থির long_u blink_offtime = 250;
স্থির guint blink_timer = 0;

    স্থির timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    যদি (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, রিক্ত);
    }
    অন্যথা
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, রিক্ত);
    }
    ঘূৰাই FALSE;
}

    পূর্ণাঙ্ক
gui_mch_is_blinking(শূন্য)
{
    ঘূৰাই blink_state != BLINK_NONE;
}

    পূর্ণাঙ্ক
gui_mch_is_blink_off(শূন্য)
{
    ঘূৰাই blink_state == BLINK_OFF;
}

    শূন্য
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    শূন্য
gui_mch_stop_blink(পূর্ণাঙ্ক may_call_gui_update_cursor)
{
    যদি (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    যদি (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    শূন্য
gui_mch_start_blink(শূন্য)
{
    যদি (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    যদি (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, রিক্ত);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    পূর্ণাঙ্ক
gui_mch_early_init_check(পূর্ণাঙ্ক give_message UNUSED)
{
    ঘূৰাই OK;
}

    পূর্ণাঙ্ক
gui_mch_init_check(শূন্য)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_set_prgname(prgname == রিক্ত ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    যদি (g_getenv("EGL_LOG_LEVEL") == রিক্ত)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    ঘূৰাই OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    পূর্ণাঙ্ক
gui_mch_init(শূন্য)
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
		     G_CALLBACK(delete_event_cb), রিক্ত);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), রিক্ত);

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
		     G_CALLBACK(on_select_tab), রিক্ত);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), রিক্ত);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), রিক্ত);

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
		     G_CALLBACK(drawarea_realize_cb), রিক্ত);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), রিক্ত);

    // Set up event controllers.
    {
	GtkEventController *চাবি_ctrl = gtk_event_controller_চাবি_new();
	g_signal_connect(চাবি_ctrl, "key-pressed",
			 G_CALLBACK(চাবি_press_event), রিক্ত);
	g_signal_connect(চাবি_ctrl, "key-released",
			 G_CALLBACK(চাবি_release_event), রিক্ত);
	gtk_widget_add_controller(gui.drawarea, চাবি_ctrl);
#ifdef FEAT_XIM
	xim_init();
#endif
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), রিক্ত);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), রিক্ত);
	gtk_widget_add_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), রিক্ত);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), রিক্ত);
	gtk_widget_add_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), রিক্ত);
	gtk_widget_add_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_in_event), রিক্ত);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_out_event), রিক্ত);
	gtk_widget_add_controller(gui.drawarea, focus);
    }

#ifdef FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_target = gtk_drop_target_async_new(রিক্ত,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_set_dnd_targets();
	g_signal_connect(gui.drop_target, "drop",
			 G_CALLBACK(drop_cb), রিক্ত);
	gtk_widget_add_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_target));
    }
#endif

    gui.border_offset = gui.border_width;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_pointer = gdk_cursor_new_from_name("none", রিক্ত);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	যদি (primary != রিক্ত)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_star);
	যদি (regular != রিক্ত)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_star);

    ঘূৰাই OK;
}

    শূন্য
gui_mch_new_colors(শূন্য)
{
    যদি (gui.drawarea != রিক্ত && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    পূর্ণাঙ্ক
gui_mch_open(শূন্য)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_width;
    guint pixel_height;

    যদি (gui.geom != রিক্ত)
    {
	পূর্ণাঙ্ক		mask;
	unsigned পূর্ণাঙ্ক	w, h;

	mask = vim_parse_geometry((ৱর্ণ *)gui.geom, &w, &h);

	যদি (mask & Width_মান)
	    Columns = w;
	যদি (mask & Height_মান)
	{
	    যদি (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	VIM_CLEAR(gui.geom);
    }
    অন্যথা
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	যদি (Columns > 80)
	    Columns = 80;
	যদি (Rows > 24)
	    Rows = 24;
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    pixel_width  += get_menu_tool_width();
    pixel_height += get_menu_tool_height();

    যদি (foreground_argument != রিক্ত)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    যদি (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    যদি (background_argument != রিক্ত)
	bg_pixel = gui_get_color((char_u *)background_argument);
    যদি (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    যদি (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    অন্যথা
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    set_normal_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), রিক্ত);
    // Resize is handled by GtkForm's size_allocate callback.

    set_form_size((পূর্ণাঙ্ক)pixel_width - get_menu_tool_width(),
	    (পূর্ণাঙ্ক)pixel_height - get_menu_tool_height());

    gtk_window_present(GTK_WINDOW(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    ঘূৰাই OK;
}

    শূন্য
gui_mch_exit(পূর্ণাঙ্ক rc UNUSED)
{
    যদি (gui.mainwin != রিক্ত)
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
		যদি ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != রিক্ত)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#endif
	gtk_window_destroy(GTK_WINDOW(gui.mainwin));
    }
}

    পূর্ণাঙ্ক
gui_mch_get_winpos(পূর্ণাঙ্ক *x, পূর্ণাঙ্ক *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    ঘূৰাই FAIL;
}

    শূন্য
gui_mch_set_winpos(পূর্ণাঙ্ক x UNUSED, পূর্ণাঙ্ক y UNUSED)
{
    // GTK4/Wayland: window positioning not available
}

    পূর্ণাঙ্ক
gui_mch_maximized(শূন্য)
{
    ঘূৰাই gtk_window_is_maximized(GTK_WINDOW(gui.mainwin));
}

    শূন্য
gui_mch_unmaximize(শূন্য)
{
    যদি (gui.mainwin != রিক্ত)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

    শূন্য
gui_mch_set_fullscreen(পূর্ণাঙ্ক flag)
{
    যদি (gui.mainwin == রিক্ত)
	ঘূৰাই;
    যদি (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    অন্যথা
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}

    স্থির শূন্য
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec UNUSED, gpointer user_data UNUSED)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    যদি (gtk_window_is_fullscreen(GTK_WINDOW(obj)))
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
    শূন্য
gui_mch_newfont(শূন্য)
{
    পূর্ণাঙ্ক w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_width(gui.formwin);
    h = gtk_widget_get_height(gui.formwin);
    gui_resize_shell(w, h);
}

    শূন্য
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    যদি (title != রিক্ত && gui.mainwin != রিক্ত)
	gtk_window_set_title(GTK_WINDOW(gui.mainwin), (ধ্রুৱ ৱর্ণ *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    শূন্য
gui_gtk_init_decor_height(শূন্য)
{
    পূর্ণাঙ্ক h = gtk_widget_get_height(gui.mainwin);

    যদি (h == 0)
	ঘূৰাই;

    h -= get_menu_tool_height();
    h -= gtk_widget_get_height(gui.formwin);

    gui.decor_height = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    স্থির শূন্য
set_form_size(পূর্ণাঙ্ক width, পূর্ণাঙ্ক height)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    যদি (gtk_widget_get_width(gui.formwin) == width
	    && gtk_widget_get_height(gui.formwin) == height)
	ঘূৰাই;

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

    শূন্য
gui_mch_set_shellsize(পূর্ণাঙ্ক width, পূর্ণাঙ্ক height,
	পূর্ণাঙ্ক min_width UNUSED, পূর্ণাঙ্ক min_height UNUSED,
	পূর্ণাঙ্ক base_width UNUSED, পূর্ণাঙ্ক base_height UNUSED,
	পূর্ণাঙ্ক direction UNUSED)
{
    set_form_size(width, height);

    gui_mch_update();
}

    শূন্য
gui_mch_get_screen_dimensions(পূর্ণাঙ্ক *screen_w, পূর্ণাঙ্ক *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    যদি (surface != রিক্ত)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	যদি (monitor != রিক্ত)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.width;
	    *screen_h = geom.height;
	    ঘূৰাই;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ifdef FEAT_MENU
    শূন্য
gui_mch_enable_menu(পূর্ণাঙ্ক showit)
{
    যদি (gui.menubar != রিক্ত)
    {
	gtk_widget_set_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	যদি (gui.drawarea != রিক্ত)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

#ifdef FEAT_TOOLBAR
    শূন্য
gui_mch_show_toolbar(পূর্ণাঙ্ক showit)
{
    যদি (gui.toolbar != রিক্ত)
    {
	gtk_widget_set_visible(gui.toolbar, showit);
	যদি (showit)
	    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	যদি (gui.drawarea != রিক্ত)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

    শূন্য
gui_mch_set_dark_theme(পূর্ণাঙ্ক dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    যদি (settings != রিক্ত)
	g_object_set(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, রিক্ত);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    পূর্ণাঙ্ক
gui_mch_adjust_charheight(শূন্য)
{
    PangoFontMetrics *metrics;
    পূর্ণাঙ্ক ascent;
    পূর্ণাঙ্ক descent;

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

    ঘূৰাই OK;
}

প্রকারনির্দেশ সংরচনা {
    PangoFontDescription    *result;
    gboolean		    done;
} FontDialogData;

    স্থির শূন্য
font_dialog_finish_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FontDialogData *fdd = (FontDialogData *)data;
    fdd->result = gtk_font_dialog_choose_font_finish(
		    GTK_FONT_DIALOG(source), res, রিক্ত);
    fdd->done = TRUE;
}

    স্থির gboolean
font_filter(gpointer item, gpointer data UNUSED)
{
    যদি (PANGO_IS_FONT_FAMILY(item))
	ঘূৰাই pango_font_family_is_monospace(PANGO_FONT_FAMILY(item));
    যদি (PANGO_IS_FONT_FACE(item))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(item));
	যদি (family != রিক্ত)
	    ঘূৰাই pango_font_family_is_monospace(family);
    }
    ঘূৰাই TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = রিক্ত;
    char_u		*fontname = রিক্ত;
    FontDialogData	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_set_modal(dlg, TRUE);
    gtk_font_dialog_set_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, রিক্ত, রিক্ত)));

    যদি (oldval != রিক্ত && oldval[0] != NUL)
    {
	char_u *oldname;

	যদি (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, রিক্ত);
	অন্যথা
	    oldname = oldval;

	যদি (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    যদি (p != রিক্ত)
	    {
		STRCPY(p + STRLEN(p), " 10");
		যদি (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((ধ্রুৱ ৱর্ণ *)oldname);
	যদি (oldname != oldval)
	    vim_free(oldname);
    }
    অন্যথা
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.result = রিক্ত;
    fdd.done = FALSE;

    gtk_font_dialog_choose_font(dlg, GTK_WINDOW(gui.mainwin),
	    initial, রিক্ত, font_dialog_finish_cb, &fdd);

    যেতিয়ালৈ (!fdd.done)
	g_main_context_iteration(রিক্ত, TRUE);

    যদি (fdd.result != রিক্ত)
    {
	ৱর্ণ *name = pango_font_description_to_string(fdd.result);
	যদি (name != রিক্ত)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    যদি (p != রিক্ত && প্ৰবেশ_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&প্ৰবেশ_conv, p, রিক্ত);
		vim_free(p);
	    }
	    অন্যথা
		fontname = p;
	}
	pango_font_description_free(fdd.result);
    }

    যদি (initial != রিক্ত)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    ঘূৰাই fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    স্থির শূন্য
ascii_glyph_table_init(শূন্য)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    পূর্ণাঙ্ক		    i;

    যদি (gui.ascii_glyphs != রিক্ত)
	pango_glyph_string_free(gui.ascii_glyphs);
    যদি (gui.ascii_font != রিক্ত)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = রিক্ত;
    gui.ascii_font   = রিক্ত;

    বাবে (i = 0; i < 128; ++i)
    {
	যদি (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	অন্যথা
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (ধ্রুৱ ৱর্ণ *)ascii_chars,
			      0, পরিমাণ(ascii_chars), attr_list, রিক্ত);

    যদি (item_list != রিক্ত && item_list->next == রিক্ত)
    {
	PangoItem   *item;
	পূর্ণাঙ্ক	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ধ্রুৱ ৱর্ণ *)ascii_chars, পরিমাণ(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	যদি (gui.ascii_glyphs->অঙ্ক_glyphs == (পূর্ণাঙ্ক)পরিমাণ(ascii_chars))
	{
	    বাবে (i = 0; i < gui.ascii_glyphs->অঙ্ক_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_offset += MAX(0, width - geom->width) / 2;
		geom->width = width;
	    }
	}
	অন্যথা
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = রিক্ত;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = রিক্ত;
	}
    }

    g_list_foreach(item_list, (GFunc)(শূন্য *)&pango_item_free, রিক্ত);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    স্থির শূন্য
get_styled_font_variants(শূন্য)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);
    যদি (plain_font == রিক্ত)
	ঘূৰাই;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    যদি (bold_font != রিক্ত)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    পূর্ণাঙ্ক
gui_mch_init_font(char_u *font_name, পূর্ণাঙ্ক fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    পূর্ণাঙ্ক			    width;

    যদি (font_name == রিক্ত)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    যদি (font_desc == রিক্ত)
	ঘূৰাই FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, রিক্ত);
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    যদি (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	পূর্ণাঙ্ক im_row = gui.row < screen_Rows ? gui.row : screen_Rows - 1;
	পূর্ণাঙ্ক im_col = gui.col < screen_Columns ? gui.col : screen_Columns - 1;
	im_set_position(im_row, im_col);
    }
    ঘূৰাই OK;
}

    GuiFont
gui_mch_get_font(char_u *name, পূর্ণাঙ্ক report_error)
{
    PangoFontDescription *font;

    যদি (name == রিক্ত)
	ঘূৰাই রিক্ত;

    font = pango_font_description_from_string((ধ্রুৱ ৱর্ণ *)name);
    যদি (font == রিক্ত)
    {
	যদি (report_error)
	    semsg(_(e_unknown_font_str), name);
	ঘূৰাই রিক্ত;
    }

    // Ensure a size is set
    যদি (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    ঘূৰাই font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    যদি (font != NOFONT)
    {
	ৱর্ণ *desc = pango_font_description_to_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	ঘূৰাই ret;
    }
    ঘূৰাই রিক্ত;
}

    শূন্য
gui_mch_free_font(GuiFont font)
{
    যদি (font != NOFONT)
	pango_font_description_free(font);
}

    শূন্য
gui_mch_expand_font(
    optexpand_T	*args,
    শূন্য	*param,
    পূর্ণাঙ্ক		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = রিক্ত;
    পূর্ণাঙ্ক			n_families = 0;
    পূর্ণাঙ্ক			wide = *(পূর্ণাঙ্ক *)param;

    যদি (args->oe_include_orig_val && *args->oe_opt_মান == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	যদি (add_match((char_u *)DEFAULT_FONT) != OK)
	    ঘূৰাই;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    বাবে (পূর্ণাঙ্ক i = 0; i < n_families; i++)
    {
	যদি (!wide && !pango_font_family_is_monospace(font_families[i]))
	    অনুৱর্ত;

	ধ্রুৱ ৱর্ণ *fam_name = pango_font_family_get_name(font_families[i]);
	যদি (প্ৰবেশ_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&প্ৰবেশ_conv,
		    (char_u *)fam_name, রিক্ত);
	    যদি (buf != রিক্ত)
	    {
		যদি (add_match(buf) != OK)
		{
		    vim_free(buf);
		    ভঙ্গ;
		}
		vim_free(buf);
	    }
	    অন্যথা
		ভঙ্গ;
	}
	অন্যথা
	{
	    যদি (add_match((char_u *)fam_name) != OK)
		ভঙ্গ;
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
    যদি (!gui.in_use)
	ঘূৰাই INVALCOLOR;

    যদি (name != রিক্ত)
	ঘূৰাই gui_get_color_cmn(name);

    ঘূৰাই INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(পূর্ণাঙ্ক r, পূর্ণাঙ্ক g, পূর্ণাঙ্ক b)
{
    ঘূৰাই gui_get_rgb_color_cmn(r, g, b);
}

    স্থির GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    ঘূৰাই rgba;
}

    শূন্য
gui_mch_set_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_to_rgba(color);
}

    শূন্য
gui_mch_set_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_to_rgba(color);
}

    শূন্য
gui_mch_set_sp_color(guicolor_T color)
{
    *gui.spcolor = color_to_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ঘূৰাই pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    শূন্য
gui_gtk4_update_size(শূন্য)
{
    vim_draw_area_set_size(VIM_DRAW_AREA(gui.drawarea),
	    gui.অঙ্ক_rows, gui.অঙ্ক_cols);
}

#ifdef FEAT_NETBEANS_INTG
    শূন্য
gui_gtk4_add_multisign(
	cairo_surface_t *surf,
	পূর্ণাঙ্ক		row,
	পূর্ণাঙ্ক		col,
	পূর্ণাঙ্ক		width,
	পূর্ণাঙ্ক		height)
{
    vim_draw_area_add_multisign(VIM_DRAW_AREA(gui.drawarea), surf, row, col,
	    width, height);
}
#endif

    শূন্য
gui_mch_clear_block(পূর্ণাঙ্ক row1, পূর্ণাঙ্ক col1, পূর্ণাঙ্ক row2, পূর্ণাঙ্ক col2)
{
    vim_draw_area_clear_block(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    যদি (gui.drawarea != রিক্ত)
	gtk_widget_queue_draw(gui.drawarea);
}

    শূন্য
gui_mch_clear_all(শূন্য)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    যদি (gui.drawarea != রিক্ত)
	gtk_widget_queue_draw(gui.drawarea);
}

    শূন্য
gui_mch_delete_সাৰিগুলি(পূর্ণাঙ্ক row, পূর্ণাঙ্ক অঙ্ক_সাৰিগুলি)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row, row + অঙ্ক_সাৰিগুলি, gui.scroll_region_bot,
	    gui.scroll_region_বাম, gui.scroll_region_ডান);
    gui_clear_block(
	    gui.scroll_region_bot - অঙ্ক_সাৰিগুলি + 1, gui.scroll_region_বাম,
	    gui.scroll_region_bot, gui.scroll_region_ডান);

    gtk_widget_queue_draw(gui.drawarea);
}

    শূন্য
gui_mch_ঢোকাও_সাৰিগুলি(পূর্ণাঙ্ক row, পূর্ণাঙ্ক অঙ্ক_সাৰিগুলি)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row + অঙ্ক_সাৰিগুলি, row, gui.scroll_region_bot - অঙ্ক_সাৰিগুলি,
	    gui.scroll_region_বাম, gui.scroll_region_ডান);
    gui_clear_block(
	    row, gui.scroll_region_বাম,
	    row + অঙ্ক_সাৰিগুলি - 1, gui.scroll_region_ডান);

    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    শূন্য
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h)
{
    gui_mch_set_bg_color(bg);
    gui_mch_set_fg_color(fg);
    vim_draw_area_set_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_queue_draw(gui.drawarea);
}

    শূন্য
gui_mch_flash(পূর্ণাঙ্ক msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (পূর্ণাঙ্ক)Rows - 1, (পূর্ণাঙ্ক)Columns - 1);
    gui_mch_flush();
    ui_delay((long)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (পূর্ণাঙ্ক)Rows - 1, (পূর্ণাঙ্ক)Columns - 1);
}

    শূন্য
gui_mch_invert_rectangle(পূর্ণাঙ্ক r, পূর্ণাঙ্ক c, পূর্ণাঙ্ক nr, পূর্ণাঙ্ক nc)
{
    vim_draw_area_invert_block(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    স্থির gboolean
চাবি_press_event(Gtk_Event_Controller_চাবি *controller UNUSED,
	guint চাবি_sym, guint keycode UNUSED,
	GdkModifierType state, gpointer data UNUSED)
{
    char_u	string[32], string2[32];
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		modifiers;
    পূর্ণাঙ্ক		চাবি;
    char_u	*s, *d;

#ifdef FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    যদি (xic != রিক্ত)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	যদি (event != রিক্ত && gtk_im_context_filter_keypress(xic, event))
	    ঘূৰাই TRUE;
    }
#endif

#ifdef FEAT_MENU
    যদি (চাবি_sym == GDK_চাবি_F10 && gui.menubar != রিক্ত)
    {
	স্থির char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	যদি (check_map(k10, State, FALSE, TRUE, FALSE, রিক্ত, রিক্ত) == রিক্ত)
	{
	    show_menubar_popover();
	    ঘূৰাই TRUE;
	}
    }
#endif

    len = keyval_to_string(চাবি_sym, string2);

    যদি (len > 1 && প্ৰবেশ_conv.vc_type != CONV_NONE)
	len = convert_প্ৰবেশ(string2, len, পরিমাণ(string2));

    s = string2;
    d = string;
    বাবে (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	যদি (d[-1] == CSI && d + 2 < string + পরিমাণ(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (পূর্ণাঙ্ক)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    যদি (চাবি_sym == GDK_চাবি_ISO_বাম_Tab)
    {
	চাবি_sym = GDK_চাবি_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    যদি ((state & GDK_ALT_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	ঘূৰাই FALSE;
#endif

    // Check for special keys
    যদি (len == 0 || len == 1)
    {
	বাবে (i = 0; special_চাবিগুলি[i].চাবি_sym != 0; i++)
	{
	    যদি (special_চাবিগুলি[i].চাবি_sym == চাবি_sym)
	    {
		string[0] = CSI;
		string[1] = special_চাবিগুলি[i].code0;
		string[2] = special_চাবিগুলি[i].code1;
		len = -3;
		ভঙ্গ;
	    }
	}
    }

    যদি (len == 0)
	ঘূৰাই TRUE;

    যদি (len == -3)
	চাবি = TO_SPECIAL(string[1], string[2]);
    অন্যথা
    {
	string[len] = NUL;
	চাবি = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    চাবি = simplify_চাবি(চাবি, &modifiers);
    যদি (চাবি == CSI)
	চাবি = K_CSI;
    যদি (IS_SPECIAL(চাবি))
    {
	string[0] = CSI;
	string[1] = K_SECOND(চাবি);
	string[2] = K_THIRD(চাবি);
	len = 3;
    }
    অন্যথা
    {
	চাবি = may_adjust_চাবি_for_ctrl(modifiers, চাবি);
	modifiers = may_remove_shift_modifier(modifiers, চাবি);
	len = mb_char2bytes(চাবি, string);
    }

    যদি (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_প্ৰবেশ_buf(string2, 3);
    }

    {
	পূর্ণাঙ্ক int_ch = check_for_interrupt(চাবি, modifiers);
	যদি (int_ch != NUL)
	{
	    trash_প্ৰবেশ_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_প্ৰবেশ_buf(string, len);

    যদি (p_mh)
	gui_mch_mousehide(TRUE);

    ঘূৰাই TRUE;
}

    স্থির শূন্য
চাবি_release_event(Gtk_Event_Controller_চাবি *controller UNUSED,
	guint keyval UNUSED, guint keycode UNUSED,
	GdkModifierType state UNUSED, gpointer data UNUSED)
{
}

স্থির পূর্ণাঙ্ক mouse_timed_out = TRUE;
স্থির guint mouse_click_timer = 0;

    স্থির timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    *(পূর্ণাঙ্ক *)data = TRUE;
    ঘূৰাই FALSE;
}

    স্থির পূর্ণাঙ্ক
modifiers_gdk2mouse(guint state)
{
    পূর্ণাঙ্ক modifiers = 0;

    যদি (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    যদি (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    যদি (state & GDK_ALT_MASK)
	modifiers |= MOUSE_ALT;

    ঘূৰাই modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#define ANY_BUTTON_MASK (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK \
	| GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
স্থির পূর্ণাঙ্ক mouse_pressed_button = -1;

স্থির guint motion_repeat_timer = 0;
স্থির gboolean motion_repeat_offset = FALSE;
স্থির পূর্ণাঙ্ক mouse_press_x = -1, mouse_press_y = -1;

    স্থির শূন্য
button_press_event(GtkGestureClick *gesture, পূর্ণাঙ্ক n_press UNUSED,
	দ্ৱিগুণ x, দ্ৱিগুণ y, gpointer data UNUSED)
{
    পূর্ণাঙ্ক button;
    পূর্ণাঙ্ক repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    যদি (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_out);

    ৱিকল্প (btn)
    {
	পক্ষ 1: button = MOUSE_বাম; ভঙ্গ;
	পক্ষ 2: button = MOUSE_মধ্য; ভঙ্গ;
	পক্ষ 3: button = MOUSE_ডান; ভঙ্গ;
	পক্ষ 8: button = MOUSE_X1; ভঙ্গ;
	পক্ষ 9: button = MOUSE_X2; ভঙ্গ;
	স্ৱভাৱ: ঘূৰাই;
    }

    mouse_press_x = (পূর্ণাঙ্ক)x;
    mouse_press_y = (পূর্ণাঙ্ক)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (পূর্ণাঙ্ক)x, (পূর্ণাঙ্ক)y, repeated_click, vim_modifiers);
}

    স্থির শূন্য
button_release_event(GtkGestureClick *gesture, পূর্ণাঙ্ক n_press UNUSED,
	দ্ৱিগুণ x, দ্ৱিগুণ y, gpointer data UNUSED)
{
    পূর্ণাঙ্ক vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    যদি (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (পূর্ণাঙ্ক)x, (পূর্ণাঙ্ক)y, FALSE, vim_modifiers);
}

স্থির দ্ৱিগুণ prev_mouse_x = -1.0;
স্থির দ্ৱিগুণ prev_mouse_y = -1.0;

স্থির GdkModifierType cur_state = 0;

    স্থির timeout_cb_type
mouse_repeat_timer_cb(gpointer data UNUSED)
{
    পূর্ণাঙ্ক x, y;

    যদি (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	ঘূৰাই G_SOURCE_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    যদি (vim_used_in_প্ৰবেশ_buf() > 10)
	ঘূৰাই G_SOURCE_CONTINUE;

    x = (পূর্ণাঙ্ক)prev_mouse_x;
    y = (পূর্ণাঙ্ক)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    যদি (motion_repeat_offset)
	x += gui.char_width;
    motion_repeat_offset = !motion_repeat_offset;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    ঘূৰাই G_SOURCE_CONTINUE;
}

    স্থির শূন্য
motion_notify_event(GtkEventControllerMotion *controller UNUSED,
	দ্ৱিগুণ x, দ্ৱিগুণ y, gpointer data UNUSED)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    যদি (mouse_pressed_button >= 0 && event != রিক্ত
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_MASK))
    {
	যদি (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    যদি (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	পূর্ণাঙ্ক		w, h;

	যদি (event != রিক্ত)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (পূর্ণাঙ্ক)x, (পূর্ণাঙ্ক)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_width(gui.drawarea);
	h = gtk_widget_get_height(gui.drawarea);

	যদি (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	যদি (x < 0 || y < 0 || x >= w || y >= h)
	{
	    পূর্ণাঙ্ক	dx, dy;
	    পূর্ণাঙ্ক offshoot;
	    পূর্ণাঙ্ক	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    যদি (offshoot > 127)
		delay = 5;
	    অন্যথা
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_add(delay,
		    mouse_repeat_timer_cb, রিক্ত);
	}
	অন্যথা
	    motion_repeat_timer = 0;
    }
    অন্যথা
	gui_mouse_moved((পূর্ণাঙ্ক)x, (পূর্ণাঙ্ক)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    যদি (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    স্থির শূন্য
enter_notify_event(GtkEventControllerMotion *controller UNUSED,
	দ্ৱিগুণ x, দ্ৱিগুণ y, gpointer data UNUSED)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    স্থির gboolean
scroll_event(GtkEventControllerScroll *controller UNUSED,
	দ্ৱিগুণ dx UNUSED, দ্ৱিগুণ dy, gpointer data UNUSED)
{
    পূর্ণাঙ্ক button;
    int_u vim_modifiers;
    পূর্ণাঙ্ক x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    যদি (event == রিক্ত)
	ঘূৰাই FALSE;
    state = gdk_event_get_modifier_state(event);

    যদি (dy < 0)
	button = MOUSE_4;	// scroll up
    অন্যথা যদি (dy > 0)
	button = MOUSE_5;	// scroll down
    অন্যথা যদি (dx < 0)
	button = MOUSE_7;	// scroll left
    অন্যথা যদি (dx > 0)
	button = MOUSE_6;	// scroll right
    অন্যথা
	ঘূৰাই FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    ঘূৰাই TRUE;
}

    স্থির শূন্য
focus_in_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    যদি (gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	যদি (blink_state == BLINK_NONE)
	    gui_mch_start_blink();
    }
}

    স্থির শূন্য
focus_out_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    যদি (!gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	যদি (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#যদি defined(FEAT_IMAGE)
    স্থির শূন্য
scale_factor_cb(GdkSurface  *surface,
	GParamSpec	    *pspec UNUSED,
	শূন্য		    *udata UNUSED)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#endif

    স্থির শূন্য
drawarea_realize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#যদি defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_factor_cb), রিক্ত);
    redraw_all_later(UPD_VALID);
#endif
    gui_mch_new_colors();
}

    স্থির শূন্য
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ifdef FEAT_XIM
    im_shutdown();
#endif
}

প্রকারনির্দেশ গণনা
{
    READ_DATA_CLIP,	    // Clipboard data
    READ_DATA_DROP,	    // URI or text DND data
    READ_DATA_DROP_DATA	    // HTML DND data
} ReadDataType;

#define READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
প্রকারনির্দেশ সংরচনা
{
    ReadDataType    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    ৱর্ণ	    *buf; // NULL when not needed
} ReadData;

#ifdef FEAT_DND
প্রকারনির্দেশ সংরচনা
{
    ReadData	rd;
    GdkDrop	*drop;
    পূর্ণাঙ্ক		x;
    পূর্ণাঙ্ক		y;
} DropReadData;
#endif

প্রকারনির্দেশ সংরচনা
{
    ReadData	rd;
    Clipboard_T *cbd;
    ৱর্ণ	*mime_type;
} ClipReadData;

    স্থির ReadData *
read_data_new(ReadDataType type, size_t sz)
{
    ReadData *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    যদি (type == READ_DATA_CLIP || type == READ_DATA_DROP_DATA)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    ঘূৰাই rd;
}

    স্থির শূন্য
read_data_free(ReadData *rd)
{
    g_cancellable_cancel(rd->cancel);

    যদি (rd->type == READ_DATA_CLIP)
	g_free(((ClipReadData *)rd)->mime_type);
    অন্যথা যদি (rd->type == READ_DATA_DROP || rd->type == READ_DATA_DROP_DATA)
	g_object_unref(((DropReadData *)rd)->drop);
    g_object_unref(rd->cancel);
    যদি (rd->arr != রিক্ত)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ifdef FEAT_DND
স্থির শূন্য drop_read_text(GdkDrop *drop, char_u *text);
#endif

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    স্থির শূন্য
read_data_প্ৰবেশ_cb(
	GInputStream	*stream,
	GAsyncResult	*result,
	ReadData	*rd)
{
    ssize_t r = g_প্ৰবেশ_stream_read_finish(stream, result, রিক্ত);

    যদি (r == -1)
    {
	// Error occurred
	DropReadData *drd = (DropReadData *)rd;

	যদি (rd->type == READ_DATA_DROP_DATA)
	    gdk_drop_finish(drd->drop, 0);
    }
    অন্যথা যদি (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	যদি (rd->type == READ_DATA_CLIP)
	{
	    ClipReadData    *crd = (ClipReadData *)rd;
	    long	    len;
	    char_u	    *actual, *final;
	    পূর্ণাঙ্ক		    motion_type = MAUTO;
	    char_u	    *tofree = রিক্ত;

	    len = (long)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = রিক্ত;

	    যদি (clip_convert_data(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ifdef FEAT_DND
	অন্যথা যদি (rd->type == READ_DATA_DROP_DATA)
	{
	    DropReadData    *drd = (DropReadData *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ধ্রুৱ uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = রিক্ত;
	    drop_read_text(drd->drop, str);
	    g_free(str);
	    gdk_drop_finish(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#endif
    }
    অন্যথা
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ধ্রুৱ uint8_t *)rd->buf, r);
	g_প্ৰবেশ_stream_read_async(stream, rd->buf, READDATA_BUFSIZE,
		G_PRIORITY_উপৰেৰ,
		rd->cancel, (GAsyncReadyCallback)read_data_প্ৰবেশ_cb, rd);
	ঘূৰাই;
    }

    read_data_free(rd);
    g_object_unref(stream);
}

#ifdef FEAT_DND
/*
 * Set up for receiving DND items.
 */
    শূন্য
gui_gtk_set_dnd_targets(শূন্য)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_add_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_add_gtype(builder, G_TYPE_STRING);
    যদি (clip_html)
	gdk_content_formats_builder_add_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_to_formats(builder);
    gtk_drop_target_async_set_formats(gui.drop_target, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    স্থির শূন্য
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

    যদি (text == রিক্ত || *text == NUL || !query_pointer_pos(রিক্ত, রিক্ত, &state))
	ঘূৰাই;

    dnd_yank_drag_data((char_u *)text, (long)STRLEN(text));

    dropkey[2] = modifiers_gdk2vim(state);
    যদি (dropkey[2] != 0)
	add_to_প্ৰবেশ_buf(dropkey, (পূর্ণাঙ্ক)পরিমাণ(dropkey));
    অন্যথা
	add_to_প্ৰবেশ_buf(dropkey + 3, (পূর্ণাঙ্ক)(পরিমাণ(dropkey) - 3));
}

    স্থির শূন্য
drop_read_মান_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    ধ্রুৱ GValue    *মান = gdk_drop_read_মান_finish(drop, result, রিক্ত);
    gboolean	    success = FALSE;

    যদি (মান == রিক্ত)
	গচ্ছ exit;

    যদি (G_মান_HOLDS(মান, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_মান_get_boxed(মান);
	পূর্ণাঙ্ক	nfiles = g_slist_length(files);
	char_u	**fnames;
	পূর্ণাঙ্ক	i;

	যদি (nfiles <= 0)
	    গচ্ছ exit;

	fnames = ALLOC_MULT(char_u *, nfiles);
	যদি (fnames == রিক্ত)
	    গচ্ছ exit;

	i = 0;
	বাবে (GSList *l = files; l != রিক্ত; l = l->next)
	{
	    GFile *file = l->data;
	    ৱর্ণ *path = g_file_get_path(file);
	    যদি (path != রিক্ত)
		fnames[i++] = vim_strsave((char_u *)path);
	    g_free(path);
	}
	nfiles = i;

	যদি (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	অন্যথা
	    vim_free(fnames);
	success = TRUE;
    }
    অন্যথা যদি (G_মান_HOLDS(মান, G_TYPE_STRING))
    {
	ধ্রুৱ ৱর্ণ *text = g_মান_get_string(মান);

	যদি (text == রিক্ত || *text == NUL)
	    গচ্ছ exit;

	drop_read_text(drop, (char_u *)text);
	success = TRUE;
    }

exit:
    gdk_drop_finish(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_data_free(&drd->rd);
}

    স্থির শূন্য
drop_read_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    GInputStream    *in_stream;
    ধ্রুৱ ৱর্ণ	    *m;

    in_stream = gdk_drop_read_finish(drop, result, &m, রিক্ত);
    যদি (in_stream == রিক্ত || STRCMP(m, "text/html") != 0)
    {
	read_data_free(&drd->rd);
	gdk_drop_finish(drop, 0);
	ঘূৰাই;
    }

    প্রতিজ্ঞা(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_প্ৰবেশ_stream_read_async(in_stream, drd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_উপৰেৰ,
	    drd->rd.cancel, (GAsyncReadyCallback)read_data_প্ৰবেশ_cb, drd);
}

// GCancellable of current DND operation, else NULL.
স্থির GCancellable *dnd_cancel = রিক্ত;
স্থির guint dnd_timeout_id = 0;

    স্থির gboolean
drop_timeout_cb(শূন্য *udata UNUSED)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    ঘূৰাই G_SOURCE_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    স্থির gboolean
drop_cb(
	GtkDropTargetAsync  *target UNUSED,
	GdkDrop		    *drop,
	দ্ৱিগুণ		    x,
	দ্ৱিগুণ		    y,
	শূন্য		    *udata UNUSED)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    DropReadData	*drd = রিক্ত;

    যদি (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, পরিমাণ(*drd));
	gdk_drop_read_মান_async(drop, GDK_TYPE_FILE_LIST, G_PRIORITY_উপৰেৰ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_মান_cb, drd);
    }
    অন্যথা যদি (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	স্থির ধ্রুৱ ৱর্ণ *m[] = {"text/html", রিক্ত};

	drd = (DropReadData *)read_data_new(READ_DATA_DROP_DATA, পরিমাণ(*drd));
	gdk_drop_read_async(drop, m, G_PRIORITY_উপৰেৰ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    অন্যথা যদি (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, পরিমাণ(*drd));
	gdk_drop_read_মান_async(drop, G_TYPE_STRING, G_PRIORITY_উপৰেৰ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_মান_cb, drd);
    }

    যদি (dnd_cancel != রিক্ত)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    যদি (drd != রিক্ত)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (পূর্ণাঙ্ক)x;
	drd->y = (পূর্ণাঙ্ক)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_add(3000, drop_timeout_cb, রিক্ত);

	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}
#endif

    স্থির শূন্য
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    gui.mainwin = রিক্ত;
    gui.drawarea = রিক্ত;
    যদি (!exiting)
	gui_shell_closed();
}

    স্থির gboolean
delete_event_cb(GtkWindow *window UNUSED, gpointer data UNUSED)
{
    gui_shell_closed();
    ঘূৰাই TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    স্থির timeout_cb_type
প্ৰবেশ_timer_cb(gpointer data)
{
    পূর্ণাঙ্ক *timed_out = (পূর্ণাঙ্ক *)data;

    *timed_out = TRUE;
    ঘূৰাই FALSE;	// don't call me again
}

    শূন্য
gui_mch_update(শূন্য)
{
    পূর্ণাঙ্ক cnt = 0;

    যেতিয়ালৈ (g_main_context_pending(রিক্ত) && !vim_is_প্ৰবেশ_buf_full()
								&& ++cnt < 100)
	g_main_context_iteration(রিক্ত, TRUE);
}

#ifdef FEAT_JOB_CHANNEL
    স্থির timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    ঘূৰাই TRUE; // Keep repeating
}
#endif

    পূর্ণাঙ্ক
gui_mch_wait_for_chars(long wtime)
{
    পূর্ণাঙ্ক		focus;
    guint	timer;
    স্থির পূর্ণাঙ্ক	timed_out;
    পূর্ণাঙ্ক		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    যদি (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   প্ৰবেশ_timer_cb, &timed_out);
    অন্যথা
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    যদি (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, রিক্ত);
#endif

    focus = gui.in_focus;

    কৰা
    {
	// Stop or start blinking when focus changes
	যদি (gui.in_focus != focus)
	{
	    যদি (gui.in_focus)
		gui_mch_start_blink();
	    অন্যথা
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	যদি (did_add_timer)
	    গচ্ছ theend;
# endif
#endif

	যদি (gui.mainwin == রিক্ত)
	    গচ্ছ theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	যদি (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    গচ্ছ theend;
	}

	যদি (!প্ৰবেশ_available())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(রিক্ত, TRUE);
	    --gtk4_main_loop_level;
	}

	যদি (প্ৰবেশ_available())
	{
	    retval = OK;
	    গচ্ছ theend;
	}
    } যেতিয়ালৈ (wtime < 0 || !timed_out);

    gui_mch_update();

theend:
    যদি (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    যদি (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    ঘূৰাই retval;
}

    শূন্য
gui_mch_flush(শূন্য)
{
    // Ensure the offscreen surface content gets painted to the widget.
    যদি (gui.drawarea != রিক্ত)
	gtk_widget_queue_draw(gui.drawarea);
    যদি (gui.mainwin != রিক্ত && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    শূন্য
gui_mch_beep(শূন্য)
{
    GdkDisplay *display;

    যদি (gui.mainwin != রিক্ত && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	যদি (display != রিক্ত)
	    gdk_display_beep(display);
    }
}

    শূন্য *
gui_mch_get_display(শূন্য)
{
    যদি (gui.mainwin != রিক্ত && gtk_widget_get_display(gui.mainwin))
	ঘূৰাই gtk_widget_get_display(gui.mainwin);
    ঘূৰাই রিক্ত;
}

    শূন্য
gui_mch_iconify(শূন্য)
{
    gtk_window_minimize(GTK_WINDOW(gui.mainwin));
}

    শূন্য
gui_mch_set_foreground(শূন্য)
{
    gtk_window_present(GTK_WINDOW(gui.mainwin));
}

    স্থির পূর্ণাঙ্ক
query_pointer_pos(পূর্ণাঙ্ক *x, পূর্ণাঙ্ক *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *pointer;
    দ্ৱিগুণ	     sx, sy, nx, ny;
    graphene_point_t src, dst;

    যদি (gui.drawarea == রিক্ত)
	ঘূৰাই FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    যদি (native == রিক্ত)
	ঘূৰাই FALSE;
    surface = gtk_native_get_surface(native);
    যদি (surface == রিক্ত)
	ঘূৰাই FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    যদি (display == রিক্ত)
	ঘূৰাই FALSE;
    seat = gdk_display_get_default_seat(display);
    যদি (seat == রিক্ত)
	ঘূৰাই FALSE;
    pointer = gdk_seat_get_pointer(seat);
    যদি (pointer == রিক্ত)
	ঘূৰাই FALSE;

    যদি (!gdk_surface_get_device_position(surface, pointer, &sx, &sy, state))
	ঘূৰাই FALSE;

    যদি (x != রিক্ত && y != রিক্ত)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (ভিন্ন)(sx - nx);
	src.y = (ভিন্ন)(sy - ny);
	যদি (!gtk_widget_compute_point(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    ঘূৰাই FALSE;

	*x = (পূর্ণাঙ্ক)dst.x;
	*y = (পূর্ণাঙ্ক)dst.y;
    }
    ঘূৰাই TRUE;
}

    শূন্য
gui_mch_getmouse(পূর্ণাঙ্ক *x, পূর্ণাঙ্ক *y)
{
    যদি (!query_pointer_pos(x, y, রিক্ত))
    {
	*x = 0;
	*y = 0;
    }
}

    শূন্য
gui_mch_setmouse(পূর্ণাঙ্ক x UNUSED, পূর্ণাঙ্ক y UNUSED)
{
    // GTK4/Wayland: cannot warp pointer
}

    শূন্য
gui_mch_mousehide(পূর্ণাঙ্ক hide)
{
    যদি (gui.pointer_hidden == hide)
	ঘূৰাই;

    gui.pointer_hidden = hide;
    যদি (gui.blank_pointer != রিক্ত)
    {
	যদি (hide)
	    gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
	অন্যথা
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#অন্যথা
	    gtk_widget_set_cursor(gui.drawarea, রিক্ত);
#endif
    }
}

    পূর্ণাঙ্ক
gui_mch_haskey(char_u *name)
{
    পূর্ণাঙ্ক i;

    বাবে (i = 0; special_চাবিগুলি[i].চাবি_sym != 0; i++)
	যদি (name[0] == special_চাবিগুলি[i].code0
		&& name[1] == special_চাবিগুলি[i].code1)
	    ঘূৰাই OK;
    ঘূৰাই FAIL;
}

    শূন্য
gui_mch_forked(শূন্য)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    স্থির শূন্য
gui_gtk4_update_scrollbar_size(শূন্য)
{
    win_T	*wp;
    পূর্ণাঙ্ক		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_বাম].id;

	যদি (sbar == রিক্ত || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_বাম]
		    && wp->w_scrollbars[SBAR_ডান].id != রিক্ত))
	    sbar = wp->w_scrollbars[SBAR_ডান].id;

	যদি (sbar != রিক্ত && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    পূর্ণাঙ্ক		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_size(sbar, &min, &nat);
	    sw = MAX(min.width, nat.width);
	    যদি (sw > 0)
	    {
		w = sw;
		ভঙ্গ;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    যদি (sbar != রিক্ত && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	পূর্ণাঙ্ক		    sh;

	gtk_widget_get_preferred_size(sbar, &min, &nat);
	sh = MAX(min.height, nat.height);

	যদি (sh > 0)
	    h = sh;
    }

    যদি (w != -1)
	gui.scrollbar_width = w;
    যদি (h != -1)
	gui.scrollbar_height = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    শূন্য
gui_mch_enable_scrollbar(scrollbar_T *sb, পূর্ণাঙ্ক flag)
{
    যদি (sb->id != রিক্ত)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_set_visible(sb->id, flag);

	যদি (!was_visible && flag)
	    gui_gtk4_update_scrollbar_size();
    }
}

#যদি defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    শূন্য
gui_mch_menu_grey(vimmenu_T *menu, পূর্ণাঙ্ক grey)
{
    যদি (menu->id == রিক্ত)
	ঘূৰাই;
    gtk_widget_set_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    শূন্য
gui_mch_menu_hidden(vimmenu_T *menu, পূর্ণাঙ্ক hidden)
{
    যদি (menu->id == রিক্ত)
	ঘূৰাই;
    gtk_widget_set_visible(menu->id, !hidden);
    gui_mch_update();
}

    শূন্য
gui_mch_draw_menubar(শূন্য)
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
    শূন্য
gui_mch_show_tabline(পূর্ণাঙ্ক showit)
{
    যদি (gui.tabline == রিক্ত)
	ঘূৰাই;
    gtk_widget_set_visible(gui.tabline, showit);
    gui_mch_update();
}

    পূর্ণাঙ্ক
gui_mch_showing_tabline(শূন্য)
{
    ঘূৰাই gui.tabline != রিক্ত && gtk_widget_get_visible(gui.tabline);
}

স্থির পূর্ণাঙ্ক ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
স্থির পূর্ণাঙ্ক tabpage_hover = 0;

    স্থির শূন্য
tabpage_enter_cb(
	GtkEventController  *controller UNUSED,
	দ্ৱিগুণ		    x UNUSED,
	দ্ৱিগুণ		    y UNUSED,
	শূন্য		    *udata)
{
    tabpage_hover = GPOINTER_TO_INT(udata);
}

    স্থির শূন্য
tabline_enter_cb(
	GtkEventController  *controller UNUSED,
	দ্ৱিগুণ		    x UNUSED,
	দ্ৱিগুণ		    y UNUSED,
	শূন্য		    *udata UNUSED)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    স্থির শূন্য
tabline_add_css(শূন্য)
{
    GtkCssProvider  *provider;
    স্থির ধ্রুৱ ৱর্ণ *css =
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
# যদি GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
# অন্যথা
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
    স্থির শূন্য
tab_close_clicked_cb(GtkButton *button UNUSED, gpointer data)
{
    পূর্ণাঙ্ক idx = gtk_notebook_page_অঙ্ক(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(data));

    যদি (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    শূন্য
gui_mch_update_tabline(শূন্য)
{
    GtkWidget	*page;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    পূর্ণাঙ্ক		nr = 0;
    পূর্ণাঙ্ক		tab_অঙ্ক;
    পূর্ণাঙ্ক		curtabidx = 0;
    char_u	*labeltext;

    যদি (gui.tabline == রিক্ত)
	ঘূৰাই;

    ignore_tabline_evt = TRUE;

    বাবে (tp = first_tabpage; tp != রিক্ত; tp = tp->tp_next, ++nr)
    {
	GtkNotebookPage	    *page_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	যদি (tp == curtab)
	    curtabidx = nr;

	tab_অঙ্ক = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	যদি (page == রিক্ত)
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

	    gtk_notebook_ঢোকাও_page(GTK_NOTEBOOK(gui.tabline),
		    page, event_box, nr);
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page, TRUE);
	    g_object_set(gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page),
		    "tab-expand", TRUE, রিক্ত);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	page_widget = gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_TO_POINTER(tab_অঙ্ক));
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(page_widget, "tab", &tab_widget, রিক্ত);
	gtk_widget_add_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	label = g_object_get_data(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	যদি (label != রিক্ত && GTK_IS_LABEL(label))
	    gtk_label_set_text(GTK_LABEL(label), (ধ্রুৱ ৱর্ণ *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_widget_set_tooltip_text(event_box, (ধ্রুৱ gchar *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    যেতিয়ালৈ (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != রিক্ত)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    যদি (nr == 1)
	gtk_widget_add_css_class(gui.tabline, "single-tab");
    অন্যথা
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    যদি (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    শূন্য
gui_mch_set_curtab(পূর্ণাঙ্ক nr)
{
    যদি (gui.tabline != রিক্ত)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    স্থির শূন্য
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    যদি (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    স্থির শূন্য
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    যদি (ignore_tabline_evt)
	ঘূৰাই;

    যদি ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    অন্যথা
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    স্থির শূন্য
tabline_menu_event_cb(VimMenuItem *item UNUSED, VimMenuItemEvent event, শূন্য *udata)
{
    যদি (event == VIM_MENU_ITEM_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_TO_INT(udata));
}

    স্থির শূন্য
add_tabline_menu_item(VimMenu *menu, ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক resp)
{
    VimMenuItem *item = VIM_MENU_ITEM(vim_menu_item_new(name,
		tabline_menu_event_cb, GINT_TO_POINTER(resp)));

    vim_menu_ঢোকাও_item(menu, item, -1);
}

/*
 * Create a menu for the tab line.
 */
    স্থির VimMenu *
create_tabline_popup_menu(শূন্য)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    add_tabline_menu_item(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, _("New Tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    ঘূৰাই menu;
}

    স্থির শূন্য
tabline_menu_press_event(
	GtkGestureClick *gesture,
	পূর্ণাঙ্ক		n_press UNUSED,
	দ্ৱিগুণ		x,
	দ্ৱিগুণ		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    যদি (but == GDK_BUTTON_SECONDARY)
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
    অন্যথা যদি (but == GDK_BUTTON_PRIMARY)
    {
	যদি (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    অন্যথা যদি (but == GDK_BUTTON_মধ্য)
    {
	যদি (tabpage_hover != 0)
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

#যদি defined(FEAT_SIGN_ICONS)
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)

    শূন্য
gui_mch_drawsign(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, পূর্ণাঙ্ক typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    যদি (sign == রিক্ত)
	ঘূৰাই;

    vim_draw_area_add_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    row, col, SIGN_WIDTH, SIGN_HEIGHT);

    gtk_widget_queue_draw(gui.drawarea);
}

    শূন্য *
gui_mch_register_sign(char_u *signfile)
{
    যদি (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GError	    *error = রিক্ত;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ধ্রুৱ ৱর্ণ *)signfile,
							    &error);
	যদি (sign != রিক্ত)
	    ঘূৰাই sign;

	semsg("E255: %s", error->message);
	g_error_free(error);
    }
    ঘূৰাই রিক্ত;
}

    শূন্য
gui_mch_destroy_sign(শূন্য *sign)
{
    যদি (sign != রিক্ত)
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
    পূর্ণাঙ্ক
gui_gtk_draw_string(পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, char_u *s, পূর্ণাঙ্ক len, পূর্ণাঙ্ক flags)
{
    পূর্ণাঙ্ক cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    row, col, s, len, flags);

    যদি (gui.drawarea != রিক্ত)
	gtk_widget_queue_draw(gui.drawarea);

    ঘূৰাই cells;
}

    পূর্ণাঙ্ক
gui_get_x11_windis(Window *win UNUSED, Display **dis UNUSED)
{
    // GTK4: not applicable
    ঘূৰাই FAIL;
}

#যদি defined(FEAT_MENU)
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
    স্থির char_u *
translate_mnemonic_tag(char_u *name, পূর্ণাঙ্ক use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    পূর্ণাঙ্ক	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    যদি (name == রিক্ত)
	ঘূৰাই রিক্ত;

    বাবে (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	যদি (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    যদি (buf != রিক্ত)
    {
	pdest = buf;
	বাবে (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    যদি (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    অন্যথা যদি (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    অন্যথা যদি (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    অন্যথা যদি (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    ঘূৰাই buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    শূন্য
gui_gtk_set_mnemonics(পূর্ণাঙ্ক enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	যদি (menu->id == রিক্ত)
	    অনুৱর্ত;

	name = translate_mnemonic_tag(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	যদি (VIM_IS_MENU_BAR_ITEM(menu->id))
	    vim_menu_bar_item_set_text(VIM_MENU_BAR_ITEM(menu->id),
		    (ধ্রুৱ ৱর্ণ *)name);
	vim_free(name);
    }
}

    স্থির শূন্য
popupmenu_closed_cb(GtkWidget *popover, শূন্য *udata UNUSED)
{
    gtk_widget_unparent(popover);
    যদি (gui.drawarea != রিক্ত)
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    স্থির শূন্য
gui_gtk_popup_at(vimmenu_T *menu, পূর্ণাঙ্ক x, পূর্ণাঙ্ক y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    যদি (menu == রিক্ত || menu->submenu_id == রিক্ত)
	ঘূৰাই;

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
	    G_CALLBACK(popupmenu_closed_cb), রিক্ত);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    শূন্য
gui_make_popup(char_u *path_name, পূর্ণাঙ্ক mouse_pos)
{
    vimmenu_T	*menu;
    পূর্ণাঙ্ক		x, y;

    menu = gui_find_menu(path_name);
    যদি (menu == রিক্ত || menu->submenu_id == রিক্ত)
	ঘূৰাই;

    যদি (mouse_pos)
	gui_mch_getmouse(&x, &y);
    অন্যথা
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#endif // FEAT_MENU

    পূর্ণাঙ্ক
get_menu_tool_width(শূন্য)
{
    ঘূৰাই 0;
}

    পূর্ণাঙ্ক
get_menu_tool_height(শূন্য)
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

    পূর্ণাঙ্ক height = 0;

    বাবে (পূর্ণাঙ্ক i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	পূর্ণাঙ্ক		h;

	যদি (widgets[i] == রিক্ত || !gtk_widget_get_visible(widgets[i]))
	    অনুৱর্ত;

	h = gtk_widget_get_height(widgets[i]);

	যদি (h == 0)
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
	অন্যথা
	    height += h;
    }
    ঘূৰাই height;
}

/*
 * Update selection targets for regular and primary selections.
 */
    শূন্য
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    যদি (cbd == &clip_plus)
	gdk_content_provider_content_changed(gui.regular_provider);
    অন্যথা
	gdk_content_provider_content_changed(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    স্থির GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    যদি (gui.mainwin == রিক্ত)
	ঘূৰাই রিক্ত;

    display = gtk_widget_get_display(gui.mainwin);
    যদি (display == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (cbd == &clip_plus)
    {
	যদি (provider != রিক্ত)
	    *provider = gui.regular_provider;
	ঘূৰাই gdk_display_get_clipboard(display);
    }
    অন্যথা
    {
	যদি (provider != রিক্ত)
	    *provider = gui.primary_provider;
	ঘূৰাই gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    স্থির শূন্য
clip_read_cb(GdkClipboard *cb, GAsyncResult *result, ClipReadData *crd)
{
    GInputStream    *in_stream;
    ধ্রুৱ ৱর্ণ	    *mime_type;

    in_stream = gdk_clipboard_read_finish(cb, result, &mime_type, রিক্ত);
    যদি (in_stream == রিক্ত)
    {
	read_data_free(&crd->rd);
	ঘূৰাই;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_প্ৰবেশ_stream_read_async(in_stream, crd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_উপৰেৰ,
	    crd->rd.cancel, (GAsyncReadyCallback)read_data_প্ৰবেশ_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    শূন্য
clip_mch_request_selection(Clipboard_T *cbd)
{
    স্থির ধ্রুৱ ৱর্ণ *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	রিক্ত
    };

    GdkClipboard *clipboard;
    ClipReadData *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, রিক্ত);
    যদি (clipboard == রিক্ত)
	ঘূৰাই;

    crd = (ClipReadData *)read_data_new(READ_DATA_CLIP, পরিমাণ(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_PRIORITY_উপৰেৰ, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t start;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	start = time(রিক্ত);
	যেতিয়ালৈ (!g_cancellable_is_cancelled(cancel)
		&& time(রিক্ত) < start + 3)
	    g_main_context_iteration(রিক্ত, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

স্থির পূর্ণাঙ্ক in_clipboard_set = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    শূন্য
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

    স্থির শূন্য
clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data)
{
    Clipboard_T *cbd = (Clipboard_T *)user_data;

    যদি (in_clipboard_set)
	ঘূৰাই;
    যদি (gdk_clipboard_is_local(clipboard))
	ঘূৰাই;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    পূর্ণাঙ্ক
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    পূর্ণাঙ্ক			ret;

    যদি (cb == রিক্ত)
	ঘূৰাই FAIL;

    in_clipboard_set = TRUE;
    ret = gdk_clipboard_set_content(cb, cp);
    in_clipboard_set = FALSE;

    ঘূৰাই ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    শূন্য
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, রিক্ত);
    যদি (clipboard == রিক্ত)
	ঘূৰাই;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    যদি (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_set_content(clipboard, রিক্ত);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(শূন্য)
{
    ঘূৰাই gtk4_main_loop_level;
}

    শূন্য
gtk_main_quit(শূন্য)
{
    gtk4_main_loop_quit = TRUE;
}

#যদি defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
স্থির ধ্রুৱ ৱর্ণ *mshape_css_names[] =
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

    শূন্য
mch_set_mouse_shape(পূর্ণাঙ্ক shape)
{
    GdkCursor	*c;
    ধ্রুৱ ৱর্ণ	*css_name = "default";

    যদি (gui.drawarea == রিক্ত)
	ঘূৰাই;

    যদি (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
    অন্যথা
    {
	যদি (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	অন্যথা যদি (shape < (পূর্ণাঙ্ক)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
	অন্যথা
	    ঘূৰাই;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, রিক্ত);
	gtk_widget_set_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    যদি (shape != MSHAPE_HIDE)
	last_shape = shape;
}

#endif // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



স্থির পূর্ণাঙ্ক last_text_area_w = 0;
স্থির পূর্ণাঙ্ক last_text_area_h = 0;

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
স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ toolbar_icon_names[] =
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

    স্থির শূন্য
toolbar_button_clicked_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    স্থির GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = রিক্ত;

    // Try specified icon file first
    যদি (menu->iconfile != রিক্ত)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	যদি (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (ধ্রুৱ ৱর্ণ *)buf, রিক্ত);

	    যদি (texture != রিক্ত)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_set_size_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    যদি (image == রিক্ত)
    {
	ধ্রুৱ ৱর্ণ *icon_name = "image-missing";
	পূর্ণাঙ্ক n = (পূর্ণাঙ্ক)ARRAY_LENGTH(toolbar_icon_names);

	যদি (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_names[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    ঘূৰাই image;
}
# endif

    স্থির শূন্য
menu_button_clicked_cb(
	VimMenuItem	    *item UNUSED,
	VimMenuItemEvent    event,
	vimmenu_T	    *menu)
{
    যদি (event == VIM_MENU_ITEM_CLICKED)
	gui_menu_cb(menu);
    অন্যথা যদি (event == VIM_MENU_ITEM_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	স্থির gboolean did_বাৰ্তা = FALSE;

	যদি (State & MODE_CMDLINE)
	    ঘূৰাই;

	tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	যদি (tooltip != রিক্ত && utf_valid_string(tooltip, রিক্ত))
	{
	    বাৰ্তা((ৱর্ণ *)tooltip);
	    did_বাৰ্তা = TRUE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	অন্যথা যদি (did_বাৰ্তা)
	{
	    বাৰ্তা("");
	    did_বাৰ্তা = FALSE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	CONVERT_TO_UTF8_FREE(tooltip);
    }
}

    শূন্য
gui_mch_add_menu(vimmenu_T *menu, পূর্ণাঙ্ক idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;
    gboolean	use_mnemonic;
    char_u	*text;

    যদি (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_set_parent(menu->submenu_id, gui.drawarea);
	ঘূৰাই;
    }

    parent = menu->parent;

    যদি ((parent != রিক্ত && parent->submenu_id == রিক্ত)
	    || !menu_is_menubar(menu->name))
	ঘূৰাই;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = parent != রিক্ত || p_wak[0] != 'n';
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    যদি (parent != রিক্ত)
    {
	parent_widget = parent->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_item_new(
		    (ধ্রুৱ ৱর্ণ *)text, রিক্ত, রিক্ত));

	vim_menu_item_set_submenu(VIM_MENU_ITEM(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_ঢোকাও_item(VIM_MENU(parent_widget),
		VIM_MENU_ITEM(menu->id), idx);
    }
    অন্যথা
    {
	parent_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_item_new(
		    (ধ্রুৱ ৱর্ণ *)text, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_ঢোকাও_item(VIM_MENU_BAR(parent_widget),
		VIM_MENU_BAR_ITEM(menu->id), idx);
    }

    vim_free(text);
}

    শূন্য
gui_mch_add_menu_item(vimmenu_T *menu, পূর্ণাঙ্ক idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    যদি (parent != রিক্ত && menu_is_toolbar(parent->name))
    {
	যদি (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_ঢোকাও_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	অন্যথা
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*text;
	    char_u	*tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    যদি (tooltip != রিক্ত && !utf_valid_string(tooltip, রিক্ত))
		CONVERT_TO_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_ঢোকাও_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ধ্রুৱ ৱর্ণ *)text, idx);
	    gtk_widget_set_tooltip_text(btn, (ধ্রুৱ gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
	ঘূৰাই;
    }
# endif

    // Menu items (non-toolbar)
    যদি (parent == রিক্ত || parent->submenu_id == রিক্ত)
	ঘূৰাই;

    যদি (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_ঢোকাও_separator(
		VIM_MENU(parent->submenu_id), idx));
    }
    অন্যথা
    {
	char_u	    *text;
	char_u	    *accel_text = রিক্ত;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	text = translate_mnemonic_tag(menu->name, use_mnemonic);

	যদি (menu->actext != রিক্ত && menu->actext[0] != NUL)
	    accel_text = CONVERT_TO_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_item_new((ধ্রুৱ ৱর্ণ *)text,
		    (VimMenuItemFunc)menu_button_clicked_cb, menu));

	যদি (accel_text != রিক্ত)
	    vim_menu_item_set_accel(VIM_MENU_ITEM(menu->id),
		    (ধ্রুৱ ৱর্ণ *)accel_text);

	vim_menu_ঢোকাও_item(VIM_MENU(parent->submenu_id),
		VIM_MENU_ITEM(menu->id), idx);

	vim_free(text);
	CONVERT_TO_UTF8_FREE(accel_text);
    }
}

    শূন্য
gui_mch_toggle_tearoffs(পূর্ণাঙ্ক enable UNUSED)
{
    // GTK4: tearoff menus don't exist.
}

    শূন্য
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    যদি (menu->id == রিক্ত)
	ঘূৰাই;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    যদি (tooltip != রিক্ত && utf_valid_string(tooltip, রিক্ত))
	gtk_widget_set_tooltip_text(menu->id, (ধ্রুৱ ৱর্ণ *)tooltip);
    CONVERT_TO_UTF8_FREE(tooltip);
}

    শূন্য
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    যদি (menu->parent != রিক্ত && menu_is_toolbar(menu->parent->name))
    {
	যদি (menu->id != রিক্ত)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = রিক্ত;
	}
	ঘূৰাই;
    }

    // For popup menus, unparent the menu as well
    যদি (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	যদি (menu->submenu_id != রিক্ত)
	    gtk_widget_unparent(menu->submenu_id);
    }
    অন্যথা যদি (menu->parent == রিক্ত && menu->id != রিক্ত)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    অন্যথা যদি (menu->id != রিক্ত)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->parent->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    শূন্য
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    পূর্ণাঙ্ক x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    স্থির শূন্য
show_menubar_popover(শূন্য)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    যদি (gui.menubar == রিক্ত || gui.drawarea == রিক্ত)
	ঘূৰাই;

    যদি (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), রিক্ত);
	ঘূৰাই;
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
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), রিক্ত);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#endif // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    শূন্য
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    GtkAdjustment *adj;

    যদি (sb->id == রিক্ত)
	ঘূৰাই;
    যদি (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	ঘূৰাই;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_set_lower(adj, 0.0);
    gtk_adjustment_set_upper(adj, (gdouble)max + 1);
    gtk_adjustment_set_মান(adj, (gdouble)val);
    gtk_adjustment_set_step_increment(adj, 1.0);
    gtk_adjustment_set_page_increment(adj, (gdouble)(size > 2 ? size - 2 : 1));
    gtk_adjustment_set_page_size(adj, (gdouble)size);
}

    শূন্য
gui_mch_set_scrollbar_pos(scrollbar_T *sb, পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h)
{
    যদি (sb->id != রিক্ত)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    পূর্ণাঙ্ক
gui_mch_get_scrollbar_xpadding(শূন্য)
{
    পূর্ণাঙ্ক formwin_w = gtk_widget_get_width(gui.formwin);
    পূর্ণাঙ্ক sbar_w = 0;
    পূর্ণাঙ্ক xpad;

    যদি (gui.which_scrollbars[SBAR_বাম])
	sbar_w += gui.scrollbar_width;
    যদি (gui.which_scrollbars[SBAR_ডান])
	sbar_w += gui.scrollbar_width;

    xpad = formwin_w - last_text_area_w - sbar_w;
    ঘূৰাই (xpad < 0) ? 0 : xpad;
}

    পূর্ণাঙ্ক
gui_mch_get_scrollbar_ypadding(শূন্য)
{
    পূর্ণাঙ্ক formwin_h = gtk_widget_get_height(gui.formwin);
    পূর্ণাঙ্ক ypad;

    ypad = formwin_h - last_text_area_h;
    যদি (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_height;

    ঘূৰাই (ypad < 0) ? 0 : ypad;
}

    স্থির শূন্য
adjustment_মান_changed(GtkAdjustment *adj, gpointer data UNUSED)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_data(G_OBJECT(adj), "vim-sb");
    long মান = (long)gtk_adjustment_get_মান(adj);

    যদি (sb != রিক্ত)
	gui_drag_scrollbar(sb, মান, FALSE);
}

    শূন্য
gui_mch_create_scrollbar(scrollbar_T *sb, পূর্ণাঙ্ক orient)
{
    GtkAdjustment *adj;
    যদি (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, রিক্ত);
    অন্যথা
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, রিক্ত);

    gtk_widget_add_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_set_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_set_data(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_মান_changed), রিক্ত);
}

    শূন্য
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    যদি (sb->id != রিক্ত)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = রিক্ত;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    শূন্য
gui_mch_set_text_area_pos(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y, পূর্ণাঙ্ক w, পূর্ণাঙ্ক h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    যদি (w <= 0 || h <= 0)
	ঘূৰাই;
    last_text_area_w = w;
    last_text_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    শূন্য
gui_gtk4_calculate_bleed(পূর্ণাঙ্ক width, পূর্ণাঙ্ক height)
{
    পূর্ণাঙ্ক old_ডান = gui.bleed_ডান;
    পূর্ণাঙ্ক old_bot = gui.bleed_bot;

    gui.bleed_ডান = width - last_text_area_w;
    gui.bleed_bot = height - last_text_area_h;

    যদি (gui.which_scrollbars[SBAR_বাম])
	gui.bleed_ডান -= gui.scrollbar_width;
    যদি (gui.which_scrollbars[SBAR_ডান])
	gui.bleed_ডান -= gui.scrollbar_width;
    যদি (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_height;

    // Not sure if this can happen, but be safe...
    যদি (gui.bleed_ডান < 0)
	gui.bleed_ডান = 0;
    যদি (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    যদি (old_ডান != gui.bleed_ডান || old_bot != gui.bleed_bot)
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
প্রকারনির্দেশ সংরচনা {
    GFile	*result;
    gboolean	done;
} FileDialogData;

    স্থির শূন্য
file_dialog_open_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_open_finish(
		    GTK_FILE_DIALOG(source), res, রিক্ত);
    fdd->done = TRUE;
}

    স্থির শূন্য
file_dialog_save_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_save_finish(
		    GTK_FILE_DIALOG(source), res, রিক্ত);
    fdd->done = TRUE;
}

    স্থির শূন্য
file_dialog_folder_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_select_folder_finish(
		    GTK_FILE_DIALOG(source), res, রিক্ত);
    fdd->done = TRUE;
}

    char_u *
gui_mch_browse(পূর্ণাঙ্ক saving,
	char_u *title,
	char_u *dflt,
	char_u *ext UNUSED,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    FileDialogData  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *result = রিক্ত;
    GListStore	    *filters;

    title = CONVERT_TO_UTF8(title);

    যদি (initdir == রিক্ত || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    অন্যথা যদি (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    add_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    যদি (title != রিক্ত)
	gtk_file_dialog_set_title(dlg, (ধ্রুৱ ৱর্ণ *)title);

    {
	GFile *dir = g_file_new_for_path((ধ্রুৱ ৱর্ণ *)dirbuf);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    যদি (filter != রিক্ত && *filter != NUL)
    {
	পূর্ণাঙ্ক		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	যদি (patt != রিক্ত)
	{
	    যেতিয়ালৈ (p != রিক্ত && *p != NUL)
	    {
		যদি (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    যদি (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    অন্যথা
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			যদি (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    যদি (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		অন্যথা
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

    যদি (saving && dflt != রিক্ত && *dflt != NUL)
	gtk_file_dialog_set_initial_name(dlg, (ধ্রুৱ ৱর্ণ *)dflt);

    fdd.result = রিক্ত;
    fdd.done = FALSE;

    যদি (saving)
	gtk_file_dialog_save(dlg, GTK_WINDOW(gui.mainwin), রিক্ত,
		file_dialog_save_cb, &fdd);
    অন্যথা
	gtk_file_dialog_open(dlg, GTK_WINDOW(gui.mainwin), রিক্ত,
		file_dialog_open_cb, &fdd);

    যেতিয়ালৈ (!fdd.done)
	g_main_context_iteration(রিক্ত, TRUE);

    যদি (fdd.result != রিক্ত)
    {
	ৱর্ণ *path = g_file_get_path(fdd.result);
	যদি (path != রিক্ত)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    ঘূৰাই result;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    FileDialogData	fdd;
    char_u		*result = রিক্ত;

    title = CONVERT_TO_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    যদি (title != রিক্ত)
	gtk_file_dialog_set_title(dlg, (ধ্রুৱ ৱর্ণ *)title);

    যদি (initdir != রিক্ত && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_path((ধ্রুৱ ৱর্ণ *)initdir);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.result = রিক্ত;
    fdd.done = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_WINDOW(gui.mainwin), রিক্ত,
	    file_dialog_folder_cb, &fdd);

    যেতিয়ালৈ (!fdd.done)
	g_main_context_iteration(রিক্ত, TRUE);

    যদি (fdd.result != রিক্ত)
    {
	ৱর্ণ *path = g_file_get_path(fdd.result);
	যদি (path != রিক্ত)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    ঘূৰাই result;
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
    স্থির ৱর্ণ **
split_button_string(char_u *button_string, পূর্ণাঙ্ক *n_buttons, ৱর্ণ **tofree)
{
    ৱর্ণ	    **array;
    char_u	    *p;
    unsigned পূর্ণাঙ্ক    গুনতি = 1;

    button_string = (char_u *)g_strdup((ধ্রুৱ ৱর্ণ *)button_string);

    বাবে (p = button_string; *p != NUL; ++p)
	যদি (*p == DLG_BUTTON_SEP)
	    ++গুনতি;

    array = g_malloc_n(গুনতি, পরিমাণ(ৱর্ণ *));
    গুনতি = 0;

    যদি (array != রিক্ত)
    {
	array[গুনতি++] = (ৱর্ণ *)button_string;
	বাবে (p = button_string; *p != NUL; )
	{
	    যদি (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[গুনতি++] = (ৱর্ণ *)p;
	    }
	    অন্যথা যদি (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    অন্যথা
		MB_PTR_ADV(p);
	}
    }

    *tofree = (ৱর্ণ *)button_string;
    *n_buttons = গুনতি;
    ঘূৰাই array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    স্থির ধ্রুৱ ৱর্ণ *
dialog_type_to_icon(পূর্ণাঙ্ক type)
{
    ৱিকল্প (type)
    {
	পক্ষ VIM_ERROR:
	    ঘূৰাই "dialog-error-symbolic";
	পক্ষ VIM_WARNING:
	    ঘূৰাই "dialog-warning-symbolic";
	পক্ষ VIM_INFO:
	    ঘূৰাই "dialog-information-symbolic";
	পক্ষ VIM_QUESTION:
	    ঘূৰাই "dialog-question-symbolic";
	স্ৱভাৱ:
	    ভঙ্গ;
    }
    ঘূৰাই রিক্ত;
}

// Data associated with each button in the dialog
প্রকারনির্দেশ সংরচনা
{
    পূর্ণাঙ্ক		but_idx;
    পূর্ণাঙ্ক		*response;
    gboolean	*done;
} DialogButtonState;

প্রকারনির্দেশ সংরচনা
{
    GtkWidget	*win;
    gboolean	*done;
    gboolean	no_alt;
} DialogState;

    স্থির শূন্য
dialog_button_clicked_cb(GtkButton *button UNUSED, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->done = TRUE;
}

    স্থির gboolean
dialog_চাবি_pressed_cb(
	Gtk_Event_Controller_চাবি	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	DialogState		*dstate)
{
    যদি (keyval == GDK_চাবি_Escape)
    {
	*dstate->done = TRUE;
	ঘূৰাই TRUE;
    }

    যদি (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_mask()))
	ঘূৰাই gtk_widget_mnemonic_activate(dstate->win, FALSE);
    ঘূৰাই FALSE;
}

    স্থির gboolean
dialog_close_request_cb(GtkWindow *win UNUSED, gboolean *win_closed)
{
    *win_closed = TRUE;
    ঘূৰাই FALSE;
}

    পূর্ণাঙ্ক
gui_mch_dialog(
	পূর্ণাঙ্ক	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	পূর্ণাঙ্ক	def_but,
	char_u	*textfield,
	পূর্ণাঙ্ক	ex_cmd UNUSED)
{
    GtkWindow		*win = GTK_WINDOW(gtk_window_new());
    GtkWidget		*vertbox;
    GtkWidget		*message_box;
    ধ্রুৱ ৱর্ণ		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*entry = রিক্ত;
    char_u		*utf8_title;
    char_u		*utf8_message;
    GtkEventController *চাবি_controller;
    DialogButtonState	*but_states = রিক্ত;
    ৱর্ণ		*tofree = রিক্ত;
    পূর্ণাঙ্ক			response = -1;
    gboolean		done = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_TO_UTF8(title);
    যদি (utf8_title != রিক্ত)
	gtk_window_set_title(win, (ধ্রুৱ ৱর্ণ *)utf8_title);
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
    gtk_widget_set_margin_শীৰ্ষ(vertbox, 24);
    gtk_widget_set_margin_bottom(vertbox, 24);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);

    // Add the message label
    message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(message_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), message_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_to_icon(type);
    যদি (icon_name != রিক্ত)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(message_box), icon);
    }

    utf8_message = CONVERT_TO_UTF8(message);
    label = gtk_label_new((ধ্রুৱ ৱর্ণ *)utf8_message);
    CONVERT_TO_UTF8_FREE(utf8_message);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(message_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    চাবি_controller = gtk_event_controller_চাবি_new();
    g_signal_connect(চাবি_controller, "key-pressed",
	    G_CALLBACK(dialog_চাবি_pressed_cb), &state);
    gtk_widget_add_controller(GTK_WIDGET(win), চাবি_controller);
    state.done = &done;
    state.win = GTK_WIDGET(win);

    যদি (textfield != রিক্ত)
    {
	// Add text entry so user can enter text
	char_u *utf8_text = CONVERT_TO_UTF8(textfield);

	entry = gtk_entry_new();

	যদি (utf8_text != রিক্ত)
	    gtk_editable_set_text(GTK_EDITABLE(entry), (ধ্রুৱ ৱর্ণ *)utf8_text);
	অন্যথা
	    gtk_editable_set_text(GTK_EDITABLE(entry), "");
	CONVERT_TO_UTF8_FREE(utf8_text);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_box_append(GTK_BOX(vertbox), entry);
	state.no_alt = FALSE;
    }
    অন্যথা
    {
	GListModel  *controllers;
	পূর্ণাঙ্ক	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_বস্তুসমূহ(controllers);
	বাবে (পূর্ণাঙ্ক i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_item(controllers, i);
	    যদি (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_set_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_window_set_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    যদি (buttons != রিক্ত)
    {
	GtkWidget   *but_box;
	ৱর্ণ	    **buttons_arr; // Note that array is allocated, not strings
	পূর্ণাঙ্ক	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	যদি (vim_strchr(p_go, GO_VERTICAL) != রিক্ত)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	অন্যথা
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_set_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = split_button_string(buttons, &n_buttons, &tofree);

	but_states = g_malloc_n(n_buttons, পরিমাণ(DialogButtonState));

	বাবে (পূর্ণাঙ্ক i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_states + i;

	    but_label = CONVERT_TO_UTF8((char_u *)buttons_arr[i]);
	    যদি (but_label == রিক্ত)
		অনুৱর্ত;

	    but = gtk_button_new_with_mnemonic((ৱর্ণ *)but_label);
	    যদি (i == def_but - 1)
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

    যেতিয়ালৈ (!done && !win_closed)
	g_main_context_iteration(রিক্ত, TRUE);

    যদি (done)
    {
	যদি (textfield != রিক্ত)
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
    ঘূৰাই response == -1 ? 0 : response + 1;
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

প্রকারনির্দেশ সংরচনা
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

স্থির SharedFindReplace find_widgets = {0};
স্থির SharedFindReplace repl_widgets = {0};

    স্থির শূন্য
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    পূর্ণাঙ্ক			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = GPOINTER_TO_INT(data);

    যদি (flags == FRD_FINDNEXT)
    {
	repl_text = রিক্ত;
	sfr = &find_widgets;
    }
    অন্যথা
    {
	repl_text = (char_u *)gtk_editable_get_text(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(sfr->what));
    direction_down = gtk_check_button_get_active(
	    GTK_CHECK_BUTTON(sfr->down));

    যদি (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    যদি (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

    স্থির শূন্য
dialog_destroyed_cb(GtkWidget *widget UNUSED, gpointer data)
{
    *(GtkWidget **)data = রিক্ত;
}

    স্থির শূন্য
entry_changed_cb(GtkWidget *entry, GtkWidget *dialog)
{
    ধ্রুৱ gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    যদি (!entry_text)
	ঘূৰাই; // Shouldn't happen

    nonempty = (entry_text[0] != '\0');

    যদি (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    যদি (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

    স্থির gboolean
find_চাবি_pressed_cb(
	Gtk_Event_Controller_চাবি	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	SharedFindReplace	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    যদি (state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	ঘূৰাই FALSE;

    // the Escape key synthesizes a cancellation action
    যদি (keyval == GDK_চাবি_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_window_destroy(GTK_WINDOW(frdp->dialog));
	ঘূৰাই TRUE;
    }

    ঘূৰাই FALSE;
}

    স্থির শূন্য
entry_activate_cb(GtkWidget *widget UNUSED, শূন্য *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    স্থির শূন্য
find_replace_dialog_create(char_u *arg, পূর্ণাঙ্ক do_replace)
{
    SharedFindReplace	*frdp;
    char_u		*entry_text;
    পূর্ণাঙ্ক			wword = FALSE;
    পূর্ণাঙ্ক			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*চাবি_controller;

    frdp = do_replace ? &repl_widgets : &find_widgets;
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    যদি (entry_text != রিক্ত && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = entry_text;
	entry_text = string_convert(&output_conv, entry_text, রিক্ত);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    যদি (frdp->dialog != রিক্ত)
    {
	যদি (entry_text != রিক্ত)
	{
	    gtk_editable_set_text(GTK_EDITABLE(frdp->what),
		    (ৱর্ণ *)entry_text);
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
	যদি (do_replace && g_utf8_strlen(
		    gtk_editable_get_text(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	ঘূৰাই;
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
    gtk_widget_set_margin_শীৰ্ষ(vertbox, 12);
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
    sensitive = (entry_text != রিক্ত && entry_text[0] != NUL);
    যদি (entry_text != রিক্ত)
	gtk_editable_set_text(GTK_EDITABLE(frdp->what), (ৱর্ণ *)entry_text);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    যদি (do_replace)
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
    অন্যথা
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

    চাবি_controller = gtk_event_controller_চাবি_new();
    g_signal_connect(চাবি_controller, "key-pressed",
	    G_CALLBACK(find_চাবি_pressed_cb), frdp);
    gtk_widget_add_controller(GTK_WIDGET(frdp->dialog), চাবি_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(entry_changed_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_window_set_default_widget(GTK_WINDOW(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
	    GINT_TO_POINTER(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    যদি (do_replace)
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
    যদি (do_replace && entry_text != রিক্ত && entry_text[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(entry_text);
}

    শূন্য
gui_mch_find_dialog(exarg_T *eap)
{
    যদি (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    শূন্য
gui_mch_replace_dialog(exarg_T *eap)
{
    যদি (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    শূন্য
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
স্থির GtkPrintDialog *print_dialog = রিক্ত;
স্থির GtkPrintSettings *print_settings = রিক্ত;
স্থির GtkPageSetup *page_setup = রিক্ত;
স্থির GtkPrintSetup *print_setup = রিক্ত;
স্থির GFile *print_file;

    স্থির শূন্য
print_dialog_cb(GtkPrintDialog *dialog, GAsyncResult *result, gboolean *done)
{
    GError *error = রিক্ত;
    print_setup = gtk_print_dialog_setup_finish(dialog, result, &error);

    যদি (print_setup == রিক্ত)
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
	দ্ৱিগুণ		*page_width,
	দ্ৱিগুণ		*page_height)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    done = FALSE;
    char_u	    *temp;

    gtk_print_dialog_set_title(dialog, (ধ্রুৱ ৱর্ণ *)jobname);
    gtk_print_dialog_set_modal(dialog, TRUE);

    print_dialog = রিক্ত;
    print_setup = রিক্ত;

    যদি (print_settings == রিক্ত)
    {
	option_table_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	যদি (opt->present)
	{
	    যদি (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    অন্যথা যদি (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    অন্যথা
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	যদি (opt->present)
	{
	    যদি (*opt->string == 'y')
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_PORTRAIT);
	    অন্যথা
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	যদি (opt->present)
	{
	    gtk_print_settings_set_paper_size(print_settings,
		    gtk_paper_size_new((ধ্রুৱ ৱর্ণ *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	যদি (opt->present)
	{
	    যদি (*opt->string == 'y')
		gtk_print_settings_set_collate(print_settings, TRUE);
	    অন্যথা
		gtk_print_settings_set_collate(print_settings, FALSE);
	}
    }

    যদি (page_setup == রিক্ত)
	page_setup = gtk_page_setup_new();

    gtk_print_dialog_set_print_settings(dialog, print_settings);
    gtk_print_dialog_set_page_setup(dialog, page_setup);

    gtk_print_dialog_setup(dialog, GTK_WINDOW(gui.mainwin), রিক্ত,
	    (GAsyncReadyCallback)print_dialog_cb, &done);

    যেতিয়ালৈ (!done)
	g_main_context_iteration(রিক্ত, TRUE);

    যদি (print_setup == রিক্ত)
    {
	g_object_unref(dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_বাৰ্তা = FALSE;
	ঘূৰাই রিক্ত;
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
    যদি (temp == রিক্ত)
    {
	emsg(_(e_cant_find_temp_file_for_writing));
	g_clear_object(&dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_বাৰ্তা = FALSE;
	ঘূৰাই রিক্ত;
    }
    print_file = g_file_new_for_path((ৱর্ণ *)temp);

    print_dialog = dialog;

    ঘূৰাই temp;
}

    স্থির শূন্য
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsyncResult	*result,
	GFile		*file)
{
    ৱর্ণ *path = g_file_get_path(file);

    (শূন্য)gtk_print_dialog_print_file_finish(dialog, result, রিক্ত);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(path);
    g_free(path);
}


    শূন্য
gui_gtk4_print_finish(শূন্য)
{
    যদি (print_dialog != রিক্ত)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_WINDOW(gui.mainwin),
		print_setup, print_file, রিক্ত,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = রিক্ত;
	g_clear_pointer(&print_setup, gtk_print_setup_unref);
    }
}
#endif // USE_GTK4_PRINT_DIALOG

#ifdef FEAT_IMAGE_GUI

প্রকারনির্দেশ সংরচনা
{
    GdkTexture *texture;
} image_gdk_T;

প্রকারনির্দেশ সংরচনা
{
    Gsk_Render_গ্ৰন্থি   **nodes; // Note that elements may be NULL
    পূর্ণাঙ্ক		    n_nodes;
} image_placement_gdk_T;

    পূর্ণাঙ্ক
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    GdkMemoryFormat fmt;
    size_t	    stride;
    GBytes	    *bytes;
    size_t	    size;
    পূর্ণাঙ্ক		    w, h;

    যদি (ctx == রিক্ত)
	ঘূৰাই FAIL;

    যদি (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_MEMORY_R8G8B8A8;
    অন্যথা
	fmt = GDK_MEMORY_R8G8B8;

    image_get_dimensions(img, &w, &h);

    size = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    bytes = g_bytes_new(img->data, size);
    ctx->texture = gdk_memory_texture_new(w, h, fmt, bytes, stride);
    g_bytes_unref(bytes);

    img->backend_data = ctx;
    ঘূৰাই OK;
}

    শূন্য
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_data;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    স্থির শূন্য
clear_nodes(image_placement_gdk_T *ctx)
{
    যদি (ctx->nodes == রিক্ত || gui.drawarea == রিক্ত)
	ঘূৰাই;

    বাবে (পূর্ণাঙ্ক i = 0; i < ctx->n_nodes; i++)
    {
	যদি (ctx->nodes[i] == রিক্ত)
	    অনুৱর্ত;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_গ্ৰন্থি_unref(ctx->nodes[i]);
    }
    g_clear_pointer(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    পূর্ণাঙ্ক
image_placement_gui_init(image_placement_T *place)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    যদি (ctx == রিক্ত)
	ঘূৰাই FAIL;

    place->backend_data = ctx;
    ঘূৰাই OK;
}

    শূন্য
image_placement_gui_uninit(image_placement_T *place)
{
    image_placement_gdk_T *ctx = place->backend_data;

    clear_nodes(ctx);
    vim_free(ctx);
}

    শূন্য
image_placement_gui_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_gdk_T   *ctx = place->backend_data;
    image_gdk_T		    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    পূর্ণাঙ্ক			    n_rects;
    পূর্ণাঙ্ক			    iw, ih;
    Gsk_Render_গ্ৰন্থি	    *texture_গ্ৰন্থি;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    যদি (rects == রিক্ত)
	ঘূৰাই;

    clear_nodes(ctx);
    যদি (n_rects == 0)
	ঘূৰাই;

    image_get_dimensions(img, &iw, &ih);
    texture_গ্ৰন্থি = gsk_texture_গ্ৰন্থি_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(place->col) - FILL_X(place->crop_box.x1),
		FILL_Y(place->row) - FILL_Y(place->crop_box.y1)
		+ FILL_Y(place->row_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    যদি (texture_গ্ৰন্থি == রিক্ত)
	// Not sure if this can happen...
	ঘূৰাই;

    ctx->nodes = g_malloc_n(n_rects, পরিমাণ(Gsk_Render_গ্ৰন্থি *));
    ctx->n_nodes = n_rects;

    বাবে (পূর্ণাঙ্ক i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	পূর্ণাঙ্ক		row, col;
	পূর্ণাঙ্ক		posx, posy;
	পূর্ণাঙ্ক		x, y, w, h;
	Gsk_Render_গ্ৰন্থি	*গ্ৰন্থি;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	গ্ৰন্থি = gsk_clip_গ্ৰন্থি_new(texture_গ্ৰন্থি,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	যদি (গ্ৰন্থি != রিক্ত)
	    vim_draw_area_add_external(VIM_DRAW_AREA(gui.drawarea), গ্ৰন্থি);

	ctx->nodes[i] = গ্ৰন্থি;
    }
    gsk_render_গ্ৰন্থি_unref(texture_গ্ৰন্থি);
}

    শূন্য
image_placement_gui_clear(image_placement_T *place)
{
    image_placement_gdk_T   *ctx = place->backend_data;

    clear_nodes(ctx);
}

#endif

#endif // FEAT_GUI_GTK
