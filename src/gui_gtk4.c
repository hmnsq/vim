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

#அந்தர்பாவ "vim.h"

#ifdef FEAT_GUI_GTK

#அந்தர்பாவ <gdk/gdkkeysyms.h>
#அந்தர்பாவ <gdk/gdk.h>
#அந்தர்பாவ <gtk/gtk.h>
#அந்தர்பாவ "gui_gtk4_f.h"
#அந்தர்பாவ "gui_gtk4_cb.h"
#அந்தர்பாவ "gui_gtk4_da.h"
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
#define No_மதிப்பு		0x0000
#define Width_மதிப்பு	0x0004
#define Height_மதிப்பு	0x0008

    நிலையான முழு
vim_parse_geometry(மாறிலி எழுத்து *str, unsigned முழு *width, unsigned முழு *height)
{
    முழு mask = No_மதிப்பு;
    எழுத்து *end;
    long val;

    எனில் (str == வெற்று || *str == NUL)
	திரும்பு mask;

    // Parse width
    எனில் (*str != '+' && *str != '-')
    {
	val = strtol(str, &end, 10);
	எனில் (end != str)
	{
	    *width = (unsigned முழு)val;
	    mask |= Width_மதிப்பு;
	    str = end;
	}
    }

    // Parse 'x' or 'X' separator and height
    எனில் (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &end, 10);
	எனில் (end != str)
	{
	    *height = (unsigned முழு)val;
	    mask |= Height_மதிப்பு;
	    str = end;
	}
    }

    திரும்பு mask;
}

#எனில் defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
நிலையான முழு last_shape = 0;
#endif

#define DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
நிலையான எண்ணிக்கை {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
நிலையான முழு gtk4_main_loop_level = 0;
நிலையான முழு gtk4_main_loop_quit = FALSE;

#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

வகைவரை gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
நிலையான கட்டமைப்பு special_சாவி
{
    guint சாவி_sym;
    char_u code0;
    char_u code1;
}
மாறிலி special_சாவிகள்[] =
{
    {GDK_சாவி_Up,	'k', 'u'},
    {GDK_சாவி_Down,	'k', 'd'},
    {GDK_சாவி_இடது,	'k', 'l'},
    {GDK_சாவி_வலது,	'k', 'r'},
    {GDK_சாவி_F1,	'k', '1'},
    {GDK_சாவி_F2,	'k', '2'},
    {GDK_சாவி_F3,	'k', '3'},
    {GDK_சாவி_F4,	'k', '4'},
    {GDK_சாவி_F5,	'k', '5'},
    {GDK_சாவி_F6,	'k', '6'},
    {GDK_சாவி_F7,	'k', '7'},
    {GDK_சாவி_F8,	'k', '8'},
    {GDK_சாவி_F9,	'k', '9'},
    {GDK_சாவி_F10,	'k', ';'},
    {GDK_சாவி_F11,	'F', '1'},
    {GDK_சாவி_F12,	'F', '2'},
    {GDK_சாவி_Help,	'%', '1'},
    {GDK_சாவி_Undo,	'&', '8'},
    {GDK_சாவி_Back_Space,	'k', 'b'},
    {GDK_சாவி_செருகு,	'k', 'I'},
    {GDK_சாவி_Delete,	'k', 'D'},
    {GDK_சாவி_Home,	'k', 'h'},
    {GDK_சாவி_End,	'@', '7'},
    {GDK_சாவி_Prior,	'k', 'P'},
    {GDK_சாவி_Next,	'k', 'N'},
    {GDK_சாவி_Print,	'%', '9'},
    {GDK_சாவி_KP_இடது,	'k', 'l'},
    {GDK_சாவி_KP_வலது,	'k', 'r'},
    {GDK_சாவி_KP_Up,	'k', 'u'},
    {GDK_சாவி_KP_Down,	'k', 'd'},
    {GDK_சாவி_KP_செருகு,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_சாவி_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_சாவி_KP_Home,	'K', '1'},
    {GDK_சாவி_KP_End,	'K', '4'},
    {GDK_சாவி_KP_Prior,	'K', '3'},
    {GDK_சாவி_KP_Next,	'K', '5'},
    {GDK_சாவி_KP_Add,	'K', '6'},
    {GDK_சாவி_KP_Subtract, 'K', '7'},
    {GDK_சாவி_KP_Divide,	'K', '8'},
    {GDK_சாவி_KP_Multiply, 'K', '9'},
    {GDK_சாவி_KP_Enter,	'K', 'A'},
    {GDK_சாவி_KP_Decimal,	'K', 'B'},
    {GDK_சாவி_KP_0,	'K', 'C'},
    {GDK_சாவி_KP_1,	'K', 'D'},
    {GDK_சாவி_KP_2,	'K', 'E'},
    {GDK_சாவி_KP_3,	'K', 'F'},
    {GDK_சாவி_KP_4,	'K', 'G'},
    {GDK_சாவி_KP_5,	'K', 'H'},
    {GDK_சாவி_KP_6,	'K', 'I'},
    {GDK_சாவி_KP_7,	'K', 'J'},
    {GDK_சாவி_KP_8,	'K', 'K'},
    {GDK_சாவி_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    நிலையான முழு
keyval_to_string(unsigned முழு keyval, char_u *string)
{
    முழு		len;
    guint32	uc;

    uc = gdk_keyval_to_unicode(keyval);
    எனில் (uc != 0)
    {
	len = utf_char2bytes((முழு)uc, string);
    }
    இல்லையெனில்
    {
	len = 1;
	தேர்வு (keyval)
	{
	    நிலை GDK_சாவி_Tab: நிலை GDK_சாவி_KP_Tab: நிலை GDK_சாவி_ISO_இடது_Tab:
		string[0] = TAB;
		நிறுத்து;
	    நிலை GDK_சாவி_Linefeed:
		string[0] = NL;
		நிறுத்து;
	    நிலை GDK_சாவி_Return: நிலை GDK_சாவி_ISO_Enter: நிலை GDK_சாவி_3270_Enter:
		string[0] = CAR;
		நிறுத்து;
	    நிலை GDK_சாவி_Escape:
		string[0] = ESC;
		நிறுத்து;
	    இயல்பு:
		len = 0;
		நிறுத்து;
	}
    }
    string[len] = NUL;
    திரும்பு len;
}

    நிலையான முழு
modifiers_gdk2vim(guint state)
{
    முழு modifiers = 0;

    எனில் (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    எனில் (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    எனில் (state & GDK_ALT_MASK)
	modifiers |= MOD_MASK_ALT;
    எனில் (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    எனில் (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;

    திரும்பு modifiers;
}

நிலையான GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
நிலையான gboolean சாவி_press_event(Gtk_Event_Controller_சாவி *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
நிலையான வெறுமை சாவி_release_event(Gtk_Event_Controller_சாவி *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
நிலையான வெறுமை button_press_event(GtkGestureClick *gesture, முழு n_press, இரட்டை x, இரட்டை y, gpointer data);
நிலையான வெறுமை button_release_event(GtkGestureClick *gesture, முழு n_press, இரட்டை x, இரட்டை y, gpointer data);
நிலையான வெறுமை motion_notify_event(GtkEventControllerMotion *controller, இரட்டை x, இரட்டை y, gpointer data);
நிலையான வெறுமை enter_notify_event(GtkEventControllerMotion *controller, இரட்டை x, இரட்டை y, gpointer data);
நிலையான gboolean scroll_event(GtkEventControllerScroll *controller, இரட்டை dx, இரட்டை dy, gpointer data);
நிலையான வெறுமை focus_in_event(GtkEventControllerFocus *controller, gpointer data);
நிலையான வெறுமை focus_out_event(GtkEventControllerFocus *controller, gpointer data);
#ifdef FEAT_DND
நிலையான gboolean drop_cb(GtkDropTargetAsync *target, GdkDrop *drop, இரட்டை x, இரட்டை y, வெறுமை *data);
#endif
#ifdef FEAT_GUI_TABLINE
நிலையான வெறுமை tabline_enter_cb(GtkEventController *controller, இரட்டை x, இரட்டை y, வெறுமை *udata);
நிலையான வெறுமை on_select_tab(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
நிலையான வெறுமை on_tab_reordered(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
நிலையான VimMenu *create_tabline_popup_menu(வெறுமை);
நிலையான வெறுமை tabline_add_css(வெறுமை);
நிலையான வெறுமை tabline_menu_press_event(GtkGestureClick *gesture, முழு n_press, இரட்டை x, இரட்டை y, GtkWidget *popover);
#endif
நிலையான வெறுமை mainwin_destroy_cb(GObject *object, gpointer data);
நிலையான gboolean delete_event_cb(GtkWindow *window, gpointer data);
நிலையான முழு query_pointer_pos(முழு *x, முழு *y, GdkModifierType *state);
நிலையான வெறுமை mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_data);
நிலையான வெறுமை set_form_size(முழு width, முழு height);
நிலையான வெறுமை drawarea_realize_cb(GtkWidget *widget, gpointer data);
நிலையான வெறுமை drawarea_unrealize_cb(GtkWidget *widget, gpointer data);
நிலையான வெறுமை clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data);
#ifdef FEAT_MENU
நிலையான வெறுமை show_menubar_popover(வெறுமை);
#endif

நிலையான மாறிலி எழுத்து *prgname = வெற்று;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    நிலையான gboolean
arg_match(மாறிலி எழுத்து *s, மாறிலி எழுத்து *name, எழுத்து **மதிப்பு)
{
    size_t  len = strlen(name);

    எனில் (strncmp(s, name, len) != 0)
	திரும்பு FALSE;
    எனில் (s[len] == '=')
	*மதிப்பு = (எழுத்து *)s + len + 1;
    இல்லையெனில் எனில் (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    வெறுமை
gui_mch_prepare(முழு *argc, எழுத்து **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    முழு	i = 0;

    போது (i < *argc)
    {
	எழுத்து	*s = argv[i];
	எழுத்து	*மதிப்பு = வெற்று;
	முழு	has_inline_மதிப்பு = FALSE;
	முழு	n_strip;

	எனில் (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    தொடர்;
	}

	எனில் (strchr(s, '=') != வெற்று)
	    has_inline_மதிப்பு = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	எனில் (!has_inline_மதிப்பு)
	{
	    எனில் (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		மதிப்பு = argv[i + 1];
	    இல்லையெனில்
		மதிப்பு = வெற்று;
	}

	எனில் (arg_match(s, "-fn", &மதிப்பு) || arg_match(s, "-font", &மதிப்பு))
	    font_argument = மதிப்பு;
	இல்லையெனில் எனில் (arg_match(s, "-geom", &மதிப்பு)
		|| arg_match(s, "-geometry", &மதிப்பு))
	{
	    எனில் (மதிப்பு != வெற்று)
		gui.geom = vim_strsave((char_u *)மதிப்பு);
	}
	இல்லையெனில் எனில் (arg_match(s, "-bg", &மதிப்பு)
		|| arg_match(s, "-background", &மதிப்பு))
	    background_argument = மதிப்பு;
	இல்லையெனில் எனில் (arg_match(s, "-fg", &மதிப்பு)
		|| arg_match(s, "-foreground", &மதிப்பு))
	    foreground_argument = மதிப்பு;
#ifdef FEAT_NETBEANS_INTG
	இல்லையெனில் எனில் (strncmp(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_மதிப்பு = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#endif
	இல்லையெனில் எனில் (arg_match(s, "--prg-name", &மதிப்பு))
	    // GTK4 specific
	    prgname = மதிப்பு;
	இல்லையெனில்
	{
	    i++;
	    தொடர்;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	எனில் (மதிப்பு != வெற்று && !has_inline_மதிப்பு)
	    n_strip = 2;

	எனில் (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    எனில் (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * அளவு(எழுத்து *));
	    argv[*argc] = வெற்று;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    வெறுமை
gui_mch_free_all(வெறுமை)
{
}

    நிலையான guint
timeout_add(முழு time, timeout_cb_type (*callback)(gpointer), முழு *flagp)
{
    திரும்பு g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    நிலையான வெறுமை
timeout_remove(guint timer)
{
    g_source_remove(timer);
}

நிலையான long_u blink_waittime = 700;
நிலையான long_u blink_ontime = 400;
நிலையான long_u blink_offtime = 250;
நிலையான guint blink_timer = 0;

    நிலையான timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    எனில் (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, வெற்று);
    }
    இல்லையெனில்
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, வெற்று);
    }
    திரும்பு FALSE;
}

    முழு
gui_mch_is_blinking(வெறுமை)
{
    திரும்பு blink_state != BLINK_NONE;
}

    முழு
gui_mch_is_blink_off(வெறுமை)
{
    திரும்பு blink_state == BLINK_OFF;
}

    வெறுமை
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    வெறுமை
gui_mch_stop_blink(முழு may_call_gui_update_cursor)
{
    எனில் (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    எனில் (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    வெறுமை
gui_mch_start_blink(வெறுமை)
{
    எனில் (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    எனில் (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, வெற்று);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    முழு
gui_mch_early_init_check(முழு give_message UNUSED)
{
    திரும்பு OK;
}

    முழு
gui_mch_init_check(வெறுமை)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_set_prgname(prgname == வெற்று ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    எனில் (g_getenv("EGL_LOG_LEVEL") == வெற்று)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    திரும்பு OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    முழு
gui_mch_init(வெறுமை)
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
		     G_CALLBACK(delete_event_cb), வெற்று);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), வெற்று);

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
		     G_CALLBACK(on_select_tab), வெற்று);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), வெற்று);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), வெற்று);

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
		     G_CALLBACK(drawarea_realize_cb), வெற்று);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), வெற்று);

    // Set up event controllers.
    {
	GtkEventController *சாவி_ctrl = gtk_event_controller_சாவி_new();
	g_signal_connect(சாவி_ctrl, "key-pressed",
			 G_CALLBACK(சாவி_press_event), வெற்று);
	g_signal_connect(சாவி_ctrl, "key-released",
			 G_CALLBACK(சாவி_release_event), வெற்று);
	gtk_widget_add_controller(gui.drawarea, சாவி_ctrl);
#ifdef FEAT_XIM
	xim_init();
#endif
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), வெற்று);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), வெற்று);
	gtk_widget_add_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), வெற்று);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), வெற்று);
	gtk_widget_add_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), வெற்று);
	gtk_widget_add_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_in_event), வெற்று);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_out_event), வெற்று);
	gtk_widget_add_controller(gui.drawarea, focus);
    }

#ifdef FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_target = gtk_drop_target_async_new(வெற்று,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_set_dnd_targets();
	g_signal_connect(gui.drop_target, "drop",
			 G_CALLBACK(drop_cb), வெற்று);
	gtk_widget_add_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_target));
    }
#endif

    gui.border_offset = gui.border_width;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_pointer = gdk_cursor_new_from_name("none", வெற்று);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	எனில் (primary != வெற்று)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_star);
	எனில் (regular != வெற்று)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_star);

    திரும்பு OK;
}

    வெறுமை
gui_mch_new_colors(வெறுமை)
{
    எனில் (gui.drawarea != வெற்று && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    முழு
gui_mch_open(வெறுமை)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_width;
    guint pixel_height;

    எனில் (gui.geom != வெற்று)
    {
	முழு		mask;
	unsigned முழு	w, h;

	mask = vim_parse_geometry((எழுத்து *)gui.geom, &w, &h);

	எனில் (mask & Width_மதிப்பு)
	    Columns = w;
	எனில் (mask & Height_மதிப்பு)
	{
	    எனில் (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	VIM_CLEAR(gui.geom);
    }
    இல்லையெனில்
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	எனில் (Columns > 80)
	    Columns = 80;
	எனில் (Rows > 24)
	    Rows = 24;
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    pixel_width  += get_menu_tool_width();
    pixel_height += get_menu_tool_height();

    எனில் (foreground_argument != வெற்று)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    எனில் (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    எனில் (background_argument != வெற்று)
	bg_pixel = gui_get_color((char_u *)background_argument);
    எனில் (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    எனில் (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    இல்லையெனில்
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    set_normal_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), வெற்று);
    // Resize is handled by GtkForm's size_allocate callback.

    set_form_size((முழு)pixel_width - get_menu_tool_width(),
	    (முழு)pixel_height - get_menu_tool_height());

    gtk_window_present(GTK_WINDOW(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    திரும்பு OK;
}

    வெறுமை
gui_mch_exit(முழு rc UNUSED)
{
    எனில் (gui.mainwin != வெற்று)
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
		எனில் ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != வெற்று)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#endif
	gtk_window_destroy(GTK_WINDOW(gui.mainwin));
    }
}

    முழு
gui_mch_get_winpos(முழு *x, முழு *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    திரும்பு FAIL;
}

    வெறுமை
gui_mch_set_winpos(முழு x UNUSED, முழு y UNUSED)
{
    // GTK4/Wayland: window positioning not available
}

    முழு
gui_mch_maximized(வெறுமை)
{
    திரும்பு gtk_window_is_maximized(GTK_WINDOW(gui.mainwin));
}

    வெறுமை
gui_mch_unmaximize(வெறுமை)
{
    எனில் (gui.mainwin != வெற்று)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

    வெறுமை
gui_mch_set_fullscreen(முழு flag)
{
    எனில் (gui.mainwin == வெற்று)
	திரும்பு;
    எனில் (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    இல்லையெனில்
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}

    நிலையான வெறுமை
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec UNUSED, gpointer user_data UNUSED)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    எனில் (gtk_window_is_fullscreen(GTK_WINDOW(obj)))
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
    வெறுமை
gui_mch_newfont(வெறுமை)
{
    முழு w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_width(gui.formwin);
    h = gtk_widget_get_height(gui.formwin);
    gui_resize_shell(w, h);
}

    வெறுமை
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    எனில் (title != வெற்று && gui.mainwin != வெற்று)
	gtk_window_set_title(GTK_WINDOW(gui.mainwin), (மாறிலி எழுத்து *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    வெறுமை
gui_gtk_init_decor_height(வெறுமை)
{
    முழு h = gtk_widget_get_height(gui.mainwin);

    எனில் (h == 0)
	திரும்பு;

    h -= get_menu_tool_height();
    h -= gtk_widget_get_height(gui.formwin);

    gui.decor_height = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    நிலையான வெறுமை
set_form_size(முழு width, முழு height)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    எனில் (gtk_widget_get_width(gui.formwin) == width
	    && gtk_widget_get_height(gui.formwin) == height)
	திரும்பு;

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

    வெறுமை
gui_mch_set_shellsize(முழு width, முழு height,
	முழு min_width UNUSED, முழு min_height UNUSED,
	முழு base_width UNUSED, முழு base_height UNUSED,
	முழு direction UNUSED)
{
    set_form_size(width, height);

    gui_mch_update();
}

    வெறுமை
gui_mch_get_screen_dimensions(முழு *screen_w, முழு *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    எனில் (surface != வெற்று)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	எனில் (monitor != வெற்று)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.width;
	    *screen_h = geom.height;
	    திரும்பு;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ifdef FEAT_MENU
    வெறுமை
gui_mch_enable_menu(முழு showit)
{
    எனில் (gui.menubar != வெற்று)
    {
	gtk_widget_set_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	எனில் (gui.drawarea != வெற்று)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

#ifdef FEAT_TOOLBAR
    வெறுமை
gui_mch_show_toolbar(முழு showit)
{
    எனில் (gui.toolbar != வெற்று)
    {
	gtk_widget_set_visible(gui.toolbar, showit);
	எனில் (showit)
	    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	எனில் (gui.drawarea != வெற்று)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

    வெறுமை
gui_mch_set_dark_theme(முழு dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    எனில் (settings != வெற்று)
	g_object_set(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, வெற்று);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    முழு
gui_mch_adjust_charheight(வெறுமை)
{
    PangoFontMetrics *metrics;
    முழு ascent;
    முழு descent;

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

    திரும்பு OK;
}

வகைவரை கட்டமைப்பு {
    PangoFontDescription    *result;
    gboolean		    done;
} FontDialogData;

    நிலையான வெறுமை
font_dialog_finish_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FontDialogData *fdd = (FontDialogData *)data;
    fdd->result = gtk_font_dialog_choose_font_finish(
		    GTK_FONT_DIALOG(source), res, வெற்று);
    fdd->done = TRUE;
}

    நிலையான gboolean
font_filter(gpointer item, gpointer data UNUSED)
{
    எனில் (PANGO_IS_FONT_FAMILY(item))
	திரும்பு pango_font_family_is_monospace(PANGO_FONT_FAMILY(item));
    எனில் (PANGO_IS_FONT_FACE(item))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(item));
	எனில் (family != வெற்று)
	    திரும்பு pango_font_family_is_monospace(family);
    }
    திரும்பு TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = வெற்று;
    char_u		*fontname = வெற்று;
    FontDialogData	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_set_modal(dlg, TRUE);
    gtk_font_dialog_set_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, வெற்று, வெற்று)));

    எனில் (oldval != வெற்று && oldval[0] != NUL)
    {
	char_u *oldname;

	எனில் (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, வெற்று);
	இல்லையெனில்
	    oldname = oldval;

	எனில் (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    எனில் (p != வெற்று)
	    {
		STRCPY(p + STRLEN(p), " 10");
		எனில் (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((மாறிலி எழுத்து *)oldname);
	எனில் (oldname != oldval)
	    vim_free(oldname);
    }
    இல்லையெனில்
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.result = வெற்று;
    fdd.done = FALSE;

    gtk_font_dialog_choose_font(dlg, GTK_WINDOW(gui.mainwin),
	    initial, வெற்று, font_dialog_finish_cb, &fdd);

    போது (!fdd.done)
	g_main_context_iteration(வெற்று, TRUE);

    எனில் (fdd.result != வெற்று)
    {
	எழுத்து *name = pango_font_description_to_string(fdd.result);
	எனில் (name != வெற்று)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    எனில் (p != வெற்று && உள்ளீடு_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&உள்ளீடு_conv, p, வெற்று);
		vim_free(p);
	    }
	    இல்லையெனில்
		fontname = p;
	}
	pango_font_description_free(fdd.result);
    }

    எனில் (initial != வெற்று)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    திரும்பு fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    நிலையான வெறுமை
ascii_glyph_table_init(வெறுமை)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    முழு		    i;

    எனில் (gui.ascii_glyphs != வெற்று)
	pango_glyph_string_free(gui.ascii_glyphs);
    எனில் (gui.ascii_font != வெற்று)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = வெற்று;
    gui.ascii_font   = வெற்று;

    க்கு (i = 0; i < 128; ++i)
    {
	எனில் (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	இல்லையெனில்
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (மாறிலி எழுத்து *)ascii_chars,
			      0, அளவு(ascii_chars), attr_list, வெற்று);

    எனில் (item_list != வெற்று && item_list->next == வெற்று)
    {
	PangoItem   *item;
	முழு	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((மாறிலி எழுத்து *)ascii_chars, அளவு(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	எனில் (gui.ascii_glyphs->இலக்கம்_glyphs == (முழு)அளவு(ascii_chars))
	{
	    க்கு (i = 0; i < gui.ascii_glyphs->இலக்கம்_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_offset += MAX(0, width - geom->width) / 2;
		geom->width = width;
	    }
	}
	இல்லையெனில்
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = வெற்று;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = வெற்று;
	}
    }

    g_list_foreach(item_list, (GFunc)(வெறுமை *)&pango_item_free, வெற்று);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    நிலையான வெறுமை
get_styled_font_variants(வெறுமை)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);
    எனில் (plain_font == வெற்று)
	திரும்பு;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    எனில் (bold_font != வெற்று)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    முழு
gui_mch_init_font(char_u *font_name, முழு fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    முழு			    width;

    எனில் (font_name == வெற்று)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    எனில் (font_desc == வெற்று)
	திரும்பு FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, வெற்று);
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    எனில் (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	முழு im_row = gui.row < screen_Rows ? gui.row : screen_Rows - 1;
	முழு im_col = gui.col < screen_Columns ? gui.col : screen_Columns - 1;
	im_set_position(im_row, im_col);
    }
    திரும்பு OK;
}

    GuiFont
gui_mch_get_font(char_u *name, முழு report_error)
{
    PangoFontDescription *font;

    எனில் (name == வெற்று)
	திரும்பு வெற்று;

    font = pango_font_description_from_string((மாறிலி எழுத்து *)name);
    எனில் (font == வெற்று)
    {
	எனில் (report_error)
	    semsg(_(e_unknown_font_str), name);
	திரும்பு வெற்று;
    }

    // Ensure a size is set
    எனில் (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    திரும்பு font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    எனில் (font != NOFONT)
    {
	எழுத்து *desc = pango_font_description_to_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	திரும்பு ret;
    }
    திரும்பு வெற்று;
}

    வெறுமை
gui_mch_free_font(GuiFont font)
{
    எனில் (font != NOFONT)
	pango_font_description_free(font);
}

    வெறுமை
gui_mch_expand_font(
    optexpand_T	*args,
    வெறுமை	*param,
    முழு		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = வெற்று;
    முழு			n_families = 0;
    முழு			wide = *(முழு *)param;

    எனில் (args->oe_include_orig_val && *args->oe_opt_மதிப்பு == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	எனில் (add_match((char_u *)DEFAULT_FONT) != OK)
	    திரும்பு;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    க்கு (முழு i = 0; i < n_families; i++)
    {
	எனில் (!wide && !pango_font_family_is_monospace(font_families[i]))
	    தொடர்;

	மாறிலி எழுத்து *fam_name = pango_font_family_get_name(font_families[i]);
	எனில் (உள்ளீடு_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&உள்ளீடு_conv,
		    (char_u *)fam_name, வெற்று);
	    எனில் (buf != வெற்று)
	    {
		எனில் (add_match(buf) != OK)
		{
		    vim_free(buf);
		    நிறுத்து;
		}
		vim_free(buf);
	    }
	    இல்லையெனில்
		நிறுத்து;
	}
	இல்லையெனில்
	{
	    எனில் (add_match((char_u *)fam_name) != OK)
		நிறுத்து;
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
    எனில் (!gui.in_use)
	திரும்பு INVALCOLOR;

    எனில் (name != வெற்று)
	திரும்பு gui_get_color_cmn(name);

    திரும்பு INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(முழு r, முழு g, முழு b)
{
    திரும்பு gui_get_rgb_color_cmn(r, g, b);
}

    நிலையான GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    திரும்பு rgba;
}

    வெறுமை
gui_mch_set_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_to_rgba(color);
}

    வெறுமை
gui_mch_set_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_to_rgba(color);
}

    வெறுமை
gui_mch_set_sp_color(guicolor_T color)
{
    *gui.spcolor = color_to_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    திரும்பு pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    வெறுமை
gui_gtk4_update_size(வெறுமை)
{
    vim_draw_area_set_size(VIM_DRAW_AREA(gui.drawarea),
	    gui.இலக்கம்_rows, gui.இலக்கம்_cols);
}

#ifdef FEAT_NETBEANS_INTG
    வெறுமை
gui_gtk4_add_multisign(
	cairo_surface_t *surf,
	முழு		row,
	முழு		col,
	முழு		width,
	முழு		height)
{
    vim_draw_area_add_multisign(VIM_DRAW_AREA(gui.drawarea), surf, row, col,
	    width, height);
}
#endif

    வெறுமை
gui_mch_clear_block(முழு row1, முழு col1, முழு row2, முழு col2)
{
    vim_draw_area_clear_block(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    எனில் (gui.drawarea != வெற்று)
	gtk_widget_queue_draw(gui.drawarea);
}

    வெறுமை
gui_mch_clear_all(வெறுமை)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    எனில் (gui.drawarea != வெற்று)
	gtk_widget_queue_draw(gui.drawarea);
}

    வெறுமை
gui_mch_delete_வரிகள்(முழு row, முழு இலக்கம்_வரிகள்)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row, row + இலக்கம்_வரிகள், gui.scroll_region_bot,
	    gui.scroll_region_இடது, gui.scroll_region_வலது);
    gui_clear_block(
	    gui.scroll_region_bot - இலக்கம்_வரிகள் + 1, gui.scroll_region_இடது,
	    gui.scroll_region_bot, gui.scroll_region_வலது);

    gtk_widget_queue_draw(gui.drawarea);
}

    வெறுமை
gui_mch_செருகு_வரிகள்(முழு row, முழு இலக்கம்_வரிகள்)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row + இலக்கம்_வரிகள், row, gui.scroll_region_bot - இலக்கம்_வரிகள்,
	    gui.scroll_region_இடது, gui.scroll_region_வலது);
    gui_clear_block(
	    row, gui.scroll_region_இடது,
	    row + இலக்கம்_வரிகள் - 1, gui.scroll_region_வலது);

    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    வெறுமை
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, முழு w, முழு h)
{
    gui_mch_set_bg_color(bg);
    gui_mch_set_fg_color(fg);
    vim_draw_area_set_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_queue_draw(gui.drawarea);
}

    வெறுமை
gui_mch_flash(முழு msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (முழு)Rows - 1, (முழு)Columns - 1);
    gui_mch_flush();
    ui_delay((long)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (முழு)Rows - 1, (முழு)Columns - 1);
}

    வெறுமை
gui_mch_invert_rectangle(முழு r, முழு c, முழு nr, முழு nc)
{
    vim_draw_area_invert_block(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    நிலையான gboolean
சாவி_press_event(Gtk_Event_Controller_சாவி *controller UNUSED,
	guint சாவி_sym, guint keycode UNUSED,
	GdkModifierType state, gpointer data UNUSED)
{
    char_u	string[32], string2[32];
    முழு		len;
    முழு		i;
    முழு		modifiers;
    முழு		சாவி;
    char_u	*s, *d;

#ifdef FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    எனில் (xic != வெற்று)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	எனில் (event != வெற்று && gtk_im_context_filter_keypress(xic, event))
	    திரும்பு TRUE;
    }
#endif

#ifdef FEAT_MENU
    எனில் (சாவி_sym == GDK_சாவி_F10 && gui.menubar != வெற்று)
    {
	நிலையான char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	எனில் (check_map(k10, State, FALSE, TRUE, FALSE, வெற்று, வெற்று) == வெற்று)
	{
	    show_menubar_popover();
	    திரும்பு TRUE;
	}
    }
#endif

    len = keyval_to_string(சாவி_sym, string2);

    எனில் (len > 1 && உள்ளீடு_conv.vc_type != CONV_NONE)
	len = convert_உள்ளீடு(string2, len, அளவு(string2));

    s = string2;
    d = string;
    க்கு (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	எனில் (d[-1] == CSI && d + 2 < string + அளவு(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (முழு)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    எனில் (சாவி_sym == GDK_சாவி_ISO_இடது_Tab)
    {
	சாவி_sym = GDK_சாவி_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    எனில் ((state & GDK_ALT_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	திரும்பு FALSE;
#endif

    // Check for special keys
    எனில் (len == 0 || len == 1)
    {
	க்கு (i = 0; special_சாவிகள்[i].சாவி_sym != 0; i++)
	{
	    எனில் (special_சாவிகள்[i].சாவி_sym == சாவி_sym)
	    {
		string[0] = CSI;
		string[1] = special_சாவிகள்[i].code0;
		string[2] = special_சாவிகள்[i].code1;
		len = -3;
		நிறுத்து;
	    }
	}
    }

    எனில் (len == 0)
	திரும்பு TRUE;

    எனில் (len == -3)
	சாவி = TO_SPECIAL(string[1], string[2]);
    இல்லையெனில்
    {
	string[len] = NUL;
	சாவி = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    சாவி = simplify_சாவி(சாவி, &modifiers);
    எனில் (சாவி == CSI)
	சாவி = K_CSI;
    எனில் (IS_SPECIAL(சாவி))
    {
	string[0] = CSI;
	string[1] = K_SECOND(சாவி);
	string[2] = K_THIRD(சாவி);
	len = 3;
    }
    இல்லையெனில்
    {
	சாவி = may_adjust_சாவி_for_ctrl(modifiers, சாவி);
	modifiers = may_remove_shift_modifier(modifiers, சாவி);
	len = mb_char2bytes(சாவி, string);
    }

    எனில் (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_உள்ளீடு_buf(string2, 3);
    }

    {
	முழு int_ch = check_for_interrupt(சாவி, modifiers);
	எனில் (int_ch != NUL)
	{
	    trash_உள்ளீடு_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_உள்ளீடு_buf(string, len);

    எனில் (p_mh)
	gui_mch_mousehide(TRUE);

    திரும்பு TRUE;
}

    நிலையான வெறுமை
சாவி_release_event(Gtk_Event_Controller_சாவி *controller UNUSED,
	guint keyval UNUSED, guint keycode UNUSED,
	GdkModifierType state UNUSED, gpointer data UNUSED)
{
}

நிலையான முழு mouse_timed_out = TRUE;
நிலையான guint mouse_click_timer = 0;

    நிலையான timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    *(முழு *)data = TRUE;
    திரும்பு FALSE;
}

    நிலையான முழு
modifiers_gdk2mouse(guint state)
{
    முழு modifiers = 0;

    எனில் (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    எனில் (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    எனில் (state & GDK_ALT_MASK)
	modifiers |= MOUSE_ALT;

    திரும்பு modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#define ANY_BUTTON_MASK (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK \
	| GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
நிலையான முழு mouse_pressed_button = -1;

நிலையான guint motion_repeat_timer = 0;
நிலையான gboolean motion_repeat_offset = FALSE;
நிலையான முழு mouse_press_x = -1, mouse_press_y = -1;

    நிலையான வெறுமை
button_press_event(GtkGestureClick *gesture, முழு n_press UNUSED,
	இரட்டை x, இரட்டை y, gpointer data UNUSED)
{
    முழு button;
    முழு repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    எனில் (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_out);

    தேர்வு (btn)
    {
	நிலை 1: button = MOUSE_இடது; நிறுத்து;
	நிலை 2: button = MOUSE_நடு; நிறுத்து;
	நிலை 3: button = MOUSE_வலது; நிறுத்து;
	நிலை 8: button = MOUSE_X1; நிறுத்து;
	நிலை 9: button = MOUSE_X2; நிறுத்து;
	இயல்பு: திரும்பு;
    }

    mouse_press_x = (முழு)x;
    mouse_press_y = (முழு)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (முழு)x, (முழு)y, repeated_click, vim_modifiers);
}

    நிலையான வெறுமை
button_release_event(GtkGestureClick *gesture, முழு n_press UNUSED,
	இரட்டை x, இரட்டை y, gpointer data UNUSED)
{
    முழு vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    எனில் (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (முழு)x, (முழு)y, FALSE, vim_modifiers);
}

நிலையான இரட்டை prev_mouse_x = -1.0;
நிலையான இரட்டை prev_mouse_y = -1.0;

நிலையான GdkModifierType cur_state = 0;

    நிலையான timeout_cb_type
mouse_repeat_timer_cb(gpointer data UNUSED)
{
    முழு x, y;

    எனில் (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	திரும்பு G_SOURCE_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    எனில் (vim_used_in_உள்ளீடு_buf() > 10)
	திரும்பு G_SOURCE_CONTINUE;

    x = (முழு)prev_mouse_x;
    y = (முழு)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    எனில் (motion_repeat_offset)
	x += gui.char_width;
    motion_repeat_offset = !motion_repeat_offset;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    திரும்பு G_SOURCE_CONTINUE;
}

    நிலையான வெறுமை
motion_notify_event(GtkEventControllerMotion *controller UNUSED,
	இரட்டை x, இரட்டை y, gpointer data UNUSED)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    எனில் (mouse_pressed_button >= 0 && event != வெற்று
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_MASK))
    {
	எனில் (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    எனில் (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	முழு		w, h;

	எனில் (event != வெற்று)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (முழு)x, (முழு)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_width(gui.drawarea);
	h = gtk_widget_get_height(gui.drawarea);

	எனில் (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	எனில் (x < 0 || y < 0 || x >= w || y >= h)
	{
	    முழு	dx, dy;
	    முழு offshoot;
	    முழு	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    எனில் (offshoot > 127)
		delay = 5;
	    இல்லையெனில்
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_add(delay,
		    mouse_repeat_timer_cb, வெற்று);
	}
	இல்லையெனில்
	    motion_repeat_timer = 0;
    }
    இல்லையெனில்
	gui_mouse_moved((முழு)x, (முழு)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    எனில் (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    நிலையான வெறுமை
enter_notify_event(GtkEventControllerMotion *controller UNUSED,
	இரட்டை x, இரட்டை y, gpointer data UNUSED)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    நிலையான gboolean
scroll_event(GtkEventControllerScroll *controller UNUSED,
	இரட்டை dx UNUSED, இரட்டை dy, gpointer data UNUSED)
{
    முழு button;
    int_u vim_modifiers;
    முழு x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    எனில் (event == வெற்று)
	திரும்பு FALSE;
    state = gdk_event_get_modifier_state(event);

    எனில் (dy < 0)
	button = MOUSE_4;	// scroll up
    இல்லையெனில் எனில் (dy > 0)
	button = MOUSE_5;	// scroll down
    இல்லையெனில் எனில் (dx < 0)
	button = MOUSE_7;	// scroll left
    இல்லையெனில் எனில் (dx > 0)
	button = MOUSE_6;	// scroll right
    இல்லையெனில்
	திரும்பு FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    திரும்பு TRUE;
}

    நிலையான வெறுமை
focus_in_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    எனில் (gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	எனில் (blink_state == BLINK_NONE)
	    gui_mch_start_blink();
    }
}

    நிலையான வெறுமை
focus_out_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    எனில் (!gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	எனில் (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#எனில் defined(FEAT_IMAGE)
    நிலையான வெறுமை
scale_factor_cb(GdkSurface  *surface,
	GParamSpec	    *pspec UNUSED,
	வெறுமை		    *udata UNUSED)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#endif

    நிலையான வெறுமை
drawarea_realize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#எனில் defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_factor_cb), வெற்று);
    redraw_all_later(UPD_VALID);
#endif
    gui_mch_new_colors();
}

    நிலையான வெறுமை
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ifdef FEAT_XIM
    im_shutdown();
#endif
}

வகைவரை எண்ணிக்கை
{
    READ_DATA_CLIP,	    // Clipboard data
    READ_DATA_DROP,	    // URI or text DND data
    READ_DATA_DROP_DATA	    // HTML DND data
} ReadDataType;

#define READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
வகைவரை கட்டமைப்பு
{
    ReadDataType    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    எழுத்து	    *buf; // NULL when not needed
} ReadData;

#ifdef FEAT_DND
வகைவரை கட்டமைப்பு
{
    ReadData	rd;
    GdkDrop	*drop;
    முழு		x;
    முழு		y;
} DropReadData;
#endif

வகைவரை கட்டமைப்பு
{
    ReadData	rd;
    Clipboard_T *cbd;
    எழுத்து	*mime_type;
} ClipReadData;

    நிலையான ReadData *
read_data_new(ReadDataType type, size_t sz)
{
    ReadData *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    எனில் (type == READ_DATA_CLIP || type == READ_DATA_DROP_DATA)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    திரும்பு rd;
}

    நிலையான வெறுமை
read_data_free(ReadData *rd)
{
    g_cancellable_cancel(rd->cancel);

    எனில் (rd->type == READ_DATA_CLIP)
	g_free(((ClipReadData *)rd)->mime_type);
    இல்லையெனில் எனில் (rd->type == READ_DATA_DROP || rd->type == READ_DATA_DROP_DATA)
	g_object_unref(((DropReadData *)rd)->drop);
    g_object_unref(rd->cancel);
    எனில் (rd->arr != வெற்று)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ifdef FEAT_DND
நிலையான வெறுமை drop_read_text(GdkDrop *drop, char_u *text);
#endif

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    நிலையான வெறுமை
read_data_உள்ளீடு_cb(
	GInputStream	*stream,
	GAsyncResult	*result,
	ReadData	*rd)
{
    ssize_t r = g_உள்ளீடு_stream_read_finish(stream, result, வெற்று);

    எனில் (r == -1)
    {
	// Error occurred
	DropReadData *drd = (DropReadData *)rd;

	எனில் (rd->type == READ_DATA_DROP_DATA)
	    gdk_drop_finish(drd->drop, 0);
    }
    இல்லையெனில் எனில் (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	எனில் (rd->type == READ_DATA_CLIP)
	{
	    ClipReadData    *crd = (ClipReadData *)rd;
	    long	    len;
	    char_u	    *actual, *final;
	    முழு		    motion_type = MAUTO;
	    char_u	    *tofree = வெற்று;

	    len = (long)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = வெற்று;

	    எனில் (clip_convert_data(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ifdef FEAT_DND
	இல்லையெனில் எனில் (rd->type == READ_DATA_DROP_DATA)
	{
	    DropReadData    *drd = (DropReadData *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (மாறிலி uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = வெற்று;
	    drop_read_text(drd->drop, str);
	    g_free(str);
	    gdk_drop_finish(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#endif
    }
    இல்லையெனில்
    {
	// Continue reading
	g_byte_array_append(rd->arr, (மாறிலி uint8_t *)rd->buf, r);
	g_உள்ளீடு_stream_read_async(stream, rd->buf, READDATA_BUFSIZE,
		G_PRIORITY_உயர்வு,
		rd->cancel, (GAsyncReadyCallback)read_data_உள்ளீடு_cb, rd);
	திரும்பு;
    }

    read_data_free(rd);
    g_object_unref(stream);
}

#ifdef FEAT_DND
/*
 * Set up for receiving DND items.
 */
    வெறுமை
gui_gtk_set_dnd_targets(வெறுமை)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_add_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_add_gtype(builder, G_TYPE_STRING);
    எனில் (clip_html)
	gdk_content_formats_builder_add_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_to_formats(builder);
    gtk_drop_target_async_set_formats(gui.drop_target, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    நிலையான வெறுமை
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

    எனில் (text == வெற்று || *text == NUL || !query_pointer_pos(வெற்று, வெற்று, &state))
	திரும்பு;

    dnd_yank_drag_data((char_u *)text, (long)STRLEN(text));

    dropkey[2] = modifiers_gdk2vim(state);
    எனில் (dropkey[2] != 0)
	add_to_உள்ளீடு_buf(dropkey, (முழு)அளவு(dropkey));
    இல்லையெனில்
	add_to_உள்ளீடு_buf(dropkey + 3, (முழு)(அளவு(dropkey) - 3));
}

    நிலையான வெறுமை
drop_read_மதிப்பு_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    மாறிலி GValue    *மதிப்பு = gdk_drop_read_மதிப்பு_finish(drop, result, வெற்று);
    gboolean	    success = FALSE;

    எனில் (மதிப்பு == வெற்று)
	செல் exit;

    எனில் (G_மதிப்பு_HOLDS(மதிப்பு, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_மதிப்பு_get_boxed(மதிப்பு);
	முழு	nfiles = g_slist_length(files);
	char_u	**fnames;
	முழு	i;

	எனில் (nfiles <= 0)
	    செல் exit;

	fnames = ALLOC_MULT(char_u *, nfiles);
	எனில் (fnames == வெற்று)
	    செல் exit;

	i = 0;
	க்கு (GSList *l = files; l != வெற்று; l = l->next)
	{
	    GFile *file = l->data;
	    எழுத்து *path = g_file_get_path(file);
	    எனில் (path != வெற்று)
		fnames[i++] = vim_strsave((char_u *)path);
	    g_free(path);
	}
	nfiles = i;

	எனில் (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	இல்லையெனில்
	    vim_free(fnames);
	success = TRUE;
    }
    இல்லையெனில் எனில் (G_மதிப்பு_HOLDS(மதிப்பு, G_TYPE_STRING))
    {
	மாறிலி எழுத்து *text = g_மதிப்பு_get_string(மதிப்பு);

	எனில் (text == வெற்று || *text == NUL)
	    செல் exit;

	drop_read_text(drop, (char_u *)text);
	success = TRUE;
    }

exit:
    gdk_drop_finish(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_data_free(&drd->rd);
}

    நிலையான வெறுமை
drop_read_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    GInputStream    *in_stream;
    மாறிலி எழுத்து	    *m;

    in_stream = gdk_drop_read_finish(drop, result, &m, வெற்று);
    எனில் (in_stream == வெற்று || STRCMP(m, "text/html") != 0)
    {
	read_data_free(&drd->rd);
	gdk_drop_finish(drop, 0);
	திரும்பு;
    }

    ப்ரதிஜ்ஞா(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_உள்ளீடு_stream_read_async(in_stream, drd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_உயர்வு,
	    drd->rd.cancel, (GAsyncReadyCallback)read_data_உள்ளீடு_cb, drd);
}

// GCancellable of current DND operation, else NULL.
நிலையான GCancellable *dnd_cancel = வெற்று;
நிலையான guint dnd_timeout_id = 0;

    நிலையான gboolean
drop_timeout_cb(வெறுமை *udata UNUSED)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    திரும்பு G_SOURCE_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    நிலையான gboolean
drop_cb(
	GtkDropTargetAsync  *target UNUSED,
	GdkDrop		    *drop,
	இரட்டை		    x,
	இரட்டை		    y,
	வெறுமை		    *udata UNUSED)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    DropReadData	*drd = வெற்று;

    எனில் (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, அளவு(*drd));
	gdk_drop_read_மதிப்பு_async(drop, GDK_TYPE_FILE_LIST, G_PRIORITY_உயர்வு,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_மதிப்பு_cb, drd);
    }
    இல்லையெனில் எனில் (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	நிலையான மாறிலி எழுத்து *m[] = {"text/html", வெற்று};

	drd = (DropReadData *)read_data_new(READ_DATA_DROP_DATA, அளவு(*drd));
	gdk_drop_read_async(drop, m, G_PRIORITY_உயர்வு,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    இல்லையெனில் எனில் (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, அளவு(*drd));
	gdk_drop_read_மதிப்பு_async(drop, G_TYPE_STRING, G_PRIORITY_உயர்வு,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_மதிப்பு_cb, drd);
    }

    எனில் (dnd_cancel != வெற்று)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    எனில் (drd != வெற்று)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (முழு)x;
	drd->y = (முழு)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_add(3000, drop_timeout_cb, வெற்று);

	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}
#endif

    நிலையான வெறுமை
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    gui.mainwin = வெற்று;
    gui.drawarea = வெற்று;
    எனில் (!exiting)
	gui_shell_closed();
}

    நிலையான gboolean
delete_event_cb(GtkWindow *window UNUSED, gpointer data UNUSED)
{
    gui_shell_closed();
    திரும்பு TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    நிலையான timeout_cb_type
உள்ளீடு_timer_cb(gpointer data)
{
    முழு *timed_out = (முழு *)data;

    *timed_out = TRUE;
    திரும்பு FALSE;	// don't call me again
}

    வெறுமை
gui_mch_update(வெறுமை)
{
    முழு cnt = 0;

    போது (g_main_context_pending(வெற்று) && !vim_is_உள்ளீடு_buf_full()
								&& ++cnt < 100)
	g_main_context_iteration(வெற்று, TRUE);
}

#ifdef FEAT_JOB_CHANNEL
    நிலையான timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    திரும்பு TRUE; // Keep repeating
}
#endif

    முழு
gui_mch_wait_for_chars(long wtime)
{
    முழு		focus;
    guint	timer;
    நிலையான முழு	timed_out;
    முழு		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    எனில் (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   உள்ளீடு_timer_cb, &timed_out);
    இல்லையெனில்
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    எனில் (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, வெற்று);
#endif

    focus = gui.in_focus;

    செய்
    {
	// Stop or start blinking when focus changes
	எனில் (gui.in_focus != focus)
	{
	    எனில் (gui.in_focus)
		gui_mch_start_blink();
	    இல்லையெனில்
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	எனில் (did_add_timer)
	    செல் theend;
# endif
#endif

	எனில் (gui.mainwin == வெற்று)
	    செல் theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	எனில் (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    செல் theend;
	}

	எனில் (!உள்ளீடு_available())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(வெற்று, TRUE);
	    --gtk4_main_loop_level;
	}

	எனில் (உள்ளீடு_available())
	{
	    retval = OK;
	    செல் theend;
	}
    } போது (wtime < 0 || !timed_out);

    gui_mch_update();

theend:
    எனில் (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    எனில் (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    திரும்பு retval;
}

    வெறுமை
gui_mch_flush(வெறுமை)
{
    // Ensure the offscreen surface content gets painted to the widget.
    எனில் (gui.drawarea != வெற்று)
	gtk_widget_queue_draw(gui.drawarea);
    எனில் (gui.mainwin != வெற்று && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    வெறுமை
gui_mch_beep(வெறுமை)
{
    GdkDisplay *display;

    எனில் (gui.mainwin != வெற்று && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	எனில் (display != வெற்று)
	    gdk_display_beep(display);
    }
}

    வெறுமை *
gui_mch_get_display(வெறுமை)
{
    எனில் (gui.mainwin != வெற்று && gtk_widget_get_display(gui.mainwin))
	திரும்பு gtk_widget_get_display(gui.mainwin);
    திரும்பு வெற்று;
}

    வெறுமை
gui_mch_iconify(வெறுமை)
{
    gtk_window_minimize(GTK_WINDOW(gui.mainwin));
}

    வெறுமை
gui_mch_set_foreground(வெறுமை)
{
    gtk_window_present(GTK_WINDOW(gui.mainwin));
}

    நிலையான முழு
query_pointer_pos(முழு *x, முழு *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *pointer;
    இரட்டை	     sx, sy, nx, ny;
    graphene_point_t src, dst;

    எனில் (gui.drawarea == வெற்று)
	திரும்பு FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    எனில் (native == வெற்று)
	திரும்பு FALSE;
    surface = gtk_native_get_surface(native);
    எனில் (surface == வெற்று)
	திரும்பு FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    எனில் (display == வெற்று)
	திரும்பு FALSE;
    seat = gdk_display_get_default_seat(display);
    எனில் (seat == வெற்று)
	திரும்பு FALSE;
    pointer = gdk_seat_get_pointer(seat);
    எனில் (pointer == வெற்று)
	திரும்பு FALSE;

    எனில் (!gdk_surface_get_device_position(surface, pointer, &sx, &sy, state))
	திரும்பு FALSE;

    எனில் (x != வெற்று && y != வெற்று)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (தசம)(sx - nx);
	src.y = (தசம)(sy - ny);
	எனில் (!gtk_widget_compute_point(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    திரும்பு FALSE;

	*x = (முழு)dst.x;
	*y = (முழு)dst.y;
    }
    திரும்பு TRUE;
}

    வெறுமை
gui_mch_getmouse(முழு *x, முழு *y)
{
    எனில் (!query_pointer_pos(x, y, வெற்று))
    {
	*x = 0;
	*y = 0;
    }
}

    வெறுமை
gui_mch_setmouse(முழு x UNUSED, முழு y UNUSED)
{
    // GTK4/Wayland: cannot warp pointer
}

    வெறுமை
gui_mch_mousehide(முழு hide)
{
    எனில் (gui.pointer_hidden == hide)
	திரும்பு;

    gui.pointer_hidden = hide;
    எனில் (gui.blank_pointer != வெற்று)
    {
	எனில் (hide)
	    gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
	இல்லையெனில்
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#இல்லையெனில்
	    gtk_widget_set_cursor(gui.drawarea, வெற்று);
#endif
    }
}

    முழு
gui_mch_haskey(char_u *name)
{
    முழு i;

    க்கு (i = 0; special_சாவிகள்[i].சாவி_sym != 0; i++)
	எனில் (name[0] == special_சாவிகள்[i].code0
		&& name[1] == special_சாவிகள்[i].code1)
	    திரும்பு OK;
    திரும்பு FAIL;
}

    வெறுமை
gui_mch_forked(வெறுமை)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    நிலையான வெறுமை
gui_gtk4_update_scrollbar_size(வெறுமை)
{
    win_T	*wp;
    முழு		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_இடது].id;

	எனில் (sbar == வெற்று || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_இடது]
		    && wp->w_scrollbars[SBAR_வலது].id != வெற்று))
	    sbar = wp->w_scrollbars[SBAR_வலது].id;

	எனில் (sbar != வெற்று && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    முழு		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_size(sbar, &min, &nat);
	    sw = MAX(min.width, nat.width);
	    எனில் (sw > 0)
	    {
		w = sw;
		நிறுத்து;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    எனில் (sbar != வெற்று && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	முழு		    sh;

	gtk_widget_get_preferred_size(sbar, &min, &nat);
	sh = MAX(min.height, nat.height);

	எனில் (sh > 0)
	    h = sh;
    }

    எனில் (w != -1)
	gui.scrollbar_width = w;
    எனில் (h != -1)
	gui.scrollbar_height = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    வெறுமை
gui_mch_enable_scrollbar(scrollbar_T *sb, முழு flag)
{
    எனில் (sb->id != வெற்று)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_set_visible(sb->id, flag);

	எனில் (!was_visible && flag)
	    gui_gtk4_update_scrollbar_size();
    }
}

#எனில் defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    வெறுமை
gui_mch_menu_grey(vimmenu_T *menu, முழு grey)
{
    எனில் (menu->id == வெற்று)
	திரும்பு;
    gtk_widget_set_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    வெறுமை
gui_mch_menu_hidden(vimmenu_T *menu, முழு hidden)
{
    எனில் (menu->id == வெற்று)
	திரும்பு;
    gtk_widget_set_visible(menu->id, !hidden);
    gui_mch_update();
}

    வெறுமை
gui_mch_draw_menubar(வெறுமை)
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
    வெறுமை
gui_mch_show_tabline(முழு showit)
{
    எனில் (gui.tabline == வெற்று)
	திரும்பு;
    gtk_widget_set_visible(gui.tabline, showit);
    gui_mch_update();
}

    முழு
gui_mch_showing_tabline(வெறுமை)
{
    திரும்பு gui.tabline != வெற்று && gtk_widget_get_visible(gui.tabline);
}

நிலையான முழு ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
நிலையான முழு tabpage_hover = 0;

    நிலையான வெறுமை
tabpage_enter_cb(
	GtkEventController  *controller UNUSED,
	இரட்டை		    x UNUSED,
	இரட்டை		    y UNUSED,
	வெறுமை		    *udata)
{
    tabpage_hover = GPOINTER_TO_INT(udata);
}

    நிலையான வெறுமை
tabline_enter_cb(
	GtkEventController  *controller UNUSED,
	இரட்டை		    x UNUSED,
	இரட்டை		    y UNUSED,
	வெறுமை		    *udata UNUSED)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    நிலையான வெறுமை
tabline_add_css(வெறுமை)
{
    GtkCssProvider  *provider;
    நிலையான மாறிலி எழுத்து *css =
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
# எனில் GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
# இல்லையெனில்
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
    நிலையான வெறுமை
tab_close_clicked_cb(GtkButton *button UNUSED, gpointer data)
{
    முழு idx = gtk_notebook_page_இலக்கம்(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(data));

    எனில் (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    வெறுமை
gui_mch_update_tabline(வெறுமை)
{
    GtkWidget	*page;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    முழு		nr = 0;
    முழு		tab_இலக்கம்;
    முழு		curtabidx = 0;
    char_u	*labeltext;

    எனில் (gui.tabline == வெற்று)
	திரும்பு;

    ignore_tabline_evt = TRUE;

    க்கு (tp = first_tabpage; tp != வெற்று; tp = tp->tp_next, ++nr)
    {
	GtkNotebookPage	    *page_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	எனில் (tp == curtab)
	    curtabidx = nr;

	tab_இலக்கம் = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	எனில் (page == வெற்று)
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

	    gtk_notebook_செருகு_page(GTK_NOTEBOOK(gui.tabline),
		    page, event_box, nr);
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page, TRUE);
	    g_object_set(gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page),
		    "tab-expand", TRUE, வெற்று);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	page_widget = gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_TO_POINTER(tab_இலக்கம்));
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(page_widget, "tab", &tab_widget, வெற்று);
	gtk_widget_add_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	label = g_object_get_data(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	எனில் (label != வெற்று && GTK_IS_LABEL(label))
	    gtk_label_set_text(GTK_LABEL(label), (மாறிலி எழுத்து *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_widget_set_tooltip_text(event_box, (மாறிலி gchar *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    போது (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != வெற்று)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    எனில் (nr == 1)
	gtk_widget_add_css_class(gui.tabline, "single-tab");
    இல்லையெனில்
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    எனில் (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    வெறுமை
gui_mch_set_curtab(முழு nr)
{
    எனில் (gui.tabline != வெற்று)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    நிலையான வெறுமை
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    எனில் (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    நிலையான வெறுமை
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    எனில் (ignore_tabline_evt)
	திரும்பு;

    எனில் ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    இல்லையெனில்
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    நிலையான வெறுமை
tabline_menu_event_cb(VimMenuItem *item UNUSED, VimMenuItemEvent event, வெறுமை *udata)
{
    எனில் (event == VIM_MENU_ITEM_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_TO_INT(udata));
}

    நிலையான வெறுமை
add_tabline_menu_item(VimMenu *menu, மாறிலி எழுத்து *name, முழு resp)
{
    VimMenuItem *item = VIM_MENU_ITEM(vim_menu_item_new(name,
		tabline_menu_event_cb, GINT_TO_POINTER(resp)));

    vim_menu_செருகு_item(menu, item, -1);
}

/*
 * Create a menu for the tab line.
 */
    நிலையான VimMenu *
create_tabline_popup_menu(வெறுமை)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    add_tabline_menu_item(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, _("New Tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    திரும்பு menu;
}

    நிலையான வெறுமை
tabline_menu_press_event(
	GtkGestureClick *gesture,
	முழு		n_press UNUSED,
	இரட்டை		x,
	இரட்டை		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    எனில் (but == GDK_BUTTON_SECONDARY)
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
    இல்லையெனில் எனில் (but == GDK_BUTTON_PRIMARY)
    {
	எனில் (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    இல்லையெனில் எனில் (but == GDK_BUTTON_நடு)
    {
	எனில் (tabpage_hover != 0)
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

#எனில் defined(FEAT_SIGN_ICONS)
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)

    வெறுமை
gui_mch_drawsign(முழு row, முழு col, முழு typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    எனில் (sign == வெற்று)
	திரும்பு;

    vim_draw_area_add_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    row, col, SIGN_WIDTH, SIGN_HEIGHT);

    gtk_widget_queue_draw(gui.drawarea);
}

    வெறுமை *
gui_mch_register_sign(char_u *signfile)
{
    எனில் (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GError	    *error = வெற்று;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((மாறிலி எழுத்து *)signfile,
							    &error);
	எனில் (sign != வெற்று)
	    திரும்பு sign;

	semsg("E255: %s", error->message);
	g_error_free(error);
    }
    திரும்பு வெற்று;
}

    வெறுமை
gui_mch_destroy_sign(வெறுமை *sign)
{
    எனில் (sign != வெற்று)
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
    முழு
gui_gtk_draw_string(முழு row, முழு col, char_u *s, முழு len, முழு flags)
{
    முழு cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    row, col, s, len, flags);

    எனில் (gui.drawarea != வெற்று)
	gtk_widget_queue_draw(gui.drawarea);

    திரும்பு cells;
}

    முழு
gui_get_x11_windis(Window *win UNUSED, Display **dis UNUSED)
{
    // GTK4: not applicable
    திரும்பு FAIL;
}

#எனில் defined(FEAT_MENU)
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
    நிலையான char_u *
translate_mnemonic_tag(char_u *name, முழு use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    முழு	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    எனில் (name == வெற்று)
	திரும்பு வெற்று;

    க்கு (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	எனில் (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    எனில் (buf != வெற்று)
    {
	pdest = buf;
	க்கு (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    எனில் (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    இல்லையெனில் எனில் (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    இல்லையெனில் எனில் (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    இல்லையெனில் எனில் (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    திரும்பு buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    வெறுமை
gui_gtk_set_mnemonics(முழு enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	எனில் (menu->id == வெற்று)
	    தொடர்;

	name = translate_mnemonic_tag(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	எனில் (VIM_IS_MENU_BAR_ITEM(menu->id))
	    vim_menu_bar_item_set_text(VIM_MENU_BAR_ITEM(menu->id),
		    (மாறிலி எழுத்து *)name);
	vim_free(name);
    }
}

    நிலையான வெறுமை
popupmenu_closed_cb(GtkWidget *popover, வெறுமை *udata UNUSED)
{
    gtk_widget_unparent(popover);
    எனில் (gui.drawarea != வெற்று)
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    நிலையான வெறுமை
gui_gtk_popup_at(vimmenu_T *menu, முழு x, முழு y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    எனில் (menu == வெற்று || menu->submenu_id == வெற்று)
	திரும்பு;

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
	    G_CALLBACK(popupmenu_closed_cb), வெற்று);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    வெறுமை
gui_make_popup(char_u *path_name, முழு mouse_pos)
{
    vimmenu_T	*menu;
    முழு		x, y;

    menu = gui_find_menu(path_name);
    எனில் (menu == வெற்று || menu->submenu_id == வெற்று)
	திரும்பு;

    எனில் (mouse_pos)
	gui_mch_getmouse(&x, &y);
    இல்லையெனில்
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#endif // FEAT_MENU

    முழு
get_menu_tool_width(வெறுமை)
{
    திரும்பு 0;
}

    முழு
get_menu_tool_height(வெறுமை)
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

    முழு height = 0;

    க்கு (முழு i = 0; i < (முழு)ARRAY_LENGTH(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	முழு		h;

	எனில் (widgets[i] == வெற்று || !gtk_widget_get_visible(widgets[i]))
	    தொடர்;

	h = gtk_widget_get_height(widgets[i]);

	எனில் (h == 0)
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
	இல்லையெனில்
	    height += h;
    }
    திரும்பு height;
}

/*
 * Update selection targets for regular and primary selections.
 */
    வெறுமை
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    எனில் (cbd == &clip_plus)
	gdk_content_provider_content_changed(gui.regular_provider);
    இல்லையெனில்
	gdk_content_provider_content_changed(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    நிலையான GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    எனில் (gui.mainwin == வெற்று)
	திரும்பு வெற்று;

    display = gtk_widget_get_display(gui.mainwin);
    எனில் (display == வெற்று)
	திரும்பு வெற்று;

    எனில் (cbd == &clip_plus)
    {
	எனில் (provider != வெற்று)
	    *provider = gui.regular_provider;
	திரும்பு gdk_display_get_clipboard(display);
    }
    இல்லையெனில்
    {
	எனில் (provider != வெற்று)
	    *provider = gui.primary_provider;
	திரும்பு gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    நிலையான வெறுமை
clip_read_cb(GdkClipboard *cb, GAsyncResult *result, ClipReadData *crd)
{
    GInputStream    *in_stream;
    மாறிலி எழுத்து	    *mime_type;

    in_stream = gdk_clipboard_read_finish(cb, result, &mime_type, வெற்று);
    எனில் (in_stream == வெற்று)
    {
	read_data_free(&crd->rd);
	திரும்பு;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_உள்ளீடு_stream_read_async(in_stream, crd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_உயர்வு,
	    crd->rd.cancel, (GAsyncReadyCallback)read_data_உள்ளீடு_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    வெறுமை
clip_mch_request_selection(Clipboard_T *cbd)
{
    நிலையான மாறிலி எழுத்து *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	வெற்று
    };

    GdkClipboard *clipboard;
    ClipReadData *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, வெற்று);
    எனில் (clipboard == வெற்று)
	திரும்பு;

    crd = (ClipReadData *)read_data_new(READ_DATA_CLIP, அளவு(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_PRIORITY_உயர்வு, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t start;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	start = time(வெற்று);
	போது (!g_cancellable_is_cancelled(cancel)
		&& time(வெற்று) < start + 3)
	    g_main_context_iteration(வெற்று, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

நிலையான முழு in_clipboard_set = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    வெறுமை
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

    நிலையான வெறுமை
clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data)
{
    Clipboard_T *cbd = (Clipboard_T *)user_data;

    எனில் (in_clipboard_set)
	திரும்பு;
    எனில் (gdk_clipboard_is_local(clipboard))
	திரும்பு;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    முழு
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    முழு			ret;

    எனில் (cb == வெற்று)
	திரும்பு FAIL;

    in_clipboard_set = TRUE;
    ret = gdk_clipboard_set_content(cb, cp);
    in_clipboard_set = FALSE;

    திரும்பு ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    வெறுமை
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, வெற்று);
    எனில் (clipboard == வெற்று)
	திரும்பு;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    எனில் (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_set_content(clipboard, வெற்று);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(வெறுமை)
{
    திரும்பு gtk4_main_loop_level;
}

    வெறுமை
gtk_main_quit(வெறுமை)
{
    gtk4_main_loop_quit = TRUE;
}

#எனில் defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
நிலையான மாறிலி எழுத்து *mshape_css_names[] =
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

    வெறுமை
mch_set_mouse_shape(முழு shape)
{
    GdkCursor	*c;
    மாறிலி எழுத்து	*css_name = "default";

    எனில் (gui.drawarea == வெற்று)
	திரும்பு;

    எனில் (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
    இல்லையெனில்
    {
	எனில் (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	இல்லையெனில் எனில் (shape < (முழு)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
	இல்லையெனில்
	    திரும்பு;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, வெற்று);
	gtk_widget_set_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    எனில் (shape != MSHAPE_HIDE)
	last_shape = shape;
}

#endif // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



நிலையான முழு last_text_area_w = 0;
நிலையான முழு last_text_area_h = 0;

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
நிலையான மாறிலி எழுத்து * மாறிலி toolbar_icon_names[] =
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

    நிலையான வெறுமை
toolbar_button_clicked_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    நிலையான GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = வெற்று;

    // Try specified icon file first
    எனில் (menu->iconfile != வெற்று)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	எனில் (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (மாறிலி எழுத்து *)buf, வெற்று);

	    எனில் (texture != வெற்று)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_set_size_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    எனில் (image == வெற்று)
    {
	மாறிலி எழுத்து *icon_name = "image-missing";
	முழு n = (முழு)ARRAY_LENGTH(toolbar_icon_names);

	எனில் (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_names[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    திரும்பு image;
}
# endif

    நிலையான வெறுமை
menu_button_clicked_cb(
	VimMenuItem	    *item UNUSED,
	VimMenuItemEvent    event,
	vimmenu_T	    *menu)
{
    எனில் (event == VIM_MENU_ITEM_CLICKED)
	gui_menu_cb(menu);
    இல்லையெனில் எனில் (event == VIM_MENU_ITEM_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	நிலையான gboolean did_செய்தி = FALSE;

	எனில் (State & MODE_CMDLINE)
	    திரும்பு;

	tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	எனில் (tooltip != வெற்று && utf_valid_string(tooltip, வெற்று))
	{
	    செய்தி((எழுத்து *)tooltip);
	    did_செய்தி = TRUE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	இல்லையெனில் எனில் (did_செய்தி)
	{
	    செய்தி("");
	    did_செய்தி = FALSE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	CONVERT_TO_UTF8_FREE(tooltip);
    }
}

    வெறுமை
gui_mch_add_menu(vimmenu_T *menu, முழு idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;
    gboolean	use_mnemonic;
    char_u	*text;

    எனில் (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_set_parent(menu->submenu_id, gui.drawarea);
	திரும்பு;
    }

    parent = menu->parent;

    எனில் ((parent != வெற்று && parent->submenu_id == வெற்று)
	    || !menu_is_menubar(menu->name))
	திரும்பு;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = parent != வெற்று || p_wak[0] != 'n';
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    எனில் (parent != வெற்று)
    {
	parent_widget = parent->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_item_new(
		    (மாறிலி எழுத்து *)text, வெற்று, வெற்று));

	vim_menu_item_set_submenu(VIM_MENU_ITEM(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_செருகு_item(VIM_MENU(parent_widget),
		VIM_MENU_ITEM(menu->id), idx);
    }
    இல்லையெனில்
    {
	parent_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_item_new(
		    (மாறிலி எழுத்து *)text, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_செருகு_item(VIM_MENU_BAR(parent_widget),
		VIM_MENU_BAR_ITEM(menu->id), idx);
    }

    vim_free(text);
}

    வெறுமை
gui_mch_add_menu_item(vimmenu_T *menu, முழு idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    எனில் (parent != வெற்று && menu_is_toolbar(parent->name))
    {
	எனில் (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_செருகு_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	இல்லையெனில்
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*text;
	    char_u	*tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    எனில் (tooltip != வெற்று && !utf_valid_string(tooltip, வெற்று))
		CONVERT_TO_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_செருகு_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (மாறிலி எழுத்து *)text, idx);
	    gtk_widget_set_tooltip_text(btn, (மாறிலி gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
	திரும்பு;
    }
# endif

    // Menu items (non-toolbar)
    எனில் (parent == வெற்று || parent->submenu_id == வெற்று)
	திரும்பு;

    எனில் (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_செருகு_separator(
		VIM_MENU(parent->submenu_id), idx));
    }
    இல்லையெனில்
    {
	char_u	    *text;
	char_u	    *accel_text = வெற்று;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	text = translate_mnemonic_tag(menu->name, use_mnemonic);

	எனில் (menu->actext != வெற்று && menu->actext[0] != NUL)
	    accel_text = CONVERT_TO_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_item_new((மாறிலி எழுத்து *)text,
		    (VimMenuItemFunc)menu_button_clicked_cb, menu));

	எனில் (accel_text != வெற்று)
	    vim_menu_item_set_accel(VIM_MENU_ITEM(menu->id),
		    (மாறிலி எழுத்து *)accel_text);

	vim_menu_செருகு_item(VIM_MENU(parent->submenu_id),
		VIM_MENU_ITEM(menu->id), idx);

	vim_free(text);
	CONVERT_TO_UTF8_FREE(accel_text);
    }
}

    வெறுமை
gui_mch_toggle_tearoffs(முழு enable UNUSED)
{
    // GTK4: tearoff menus don't exist.
}

    வெறுமை
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    எனில் (menu->id == வெற்று)
	திரும்பு;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    எனில் (tooltip != வெற்று && utf_valid_string(tooltip, வெற்று))
	gtk_widget_set_tooltip_text(menu->id, (மாறிலி எழுத்து *)tooltip);
    CONVERT_TO_UTF8_FREE(tooltip);
}

    வெறுமை
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    எனில் (menu->parent != வெற்று && menu_is_toolbar(menu->parent->name))
    {
	எனில் (menu->id != வெற்று)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = வெற்று;
	}
	திரும்பு;
    }

    // For popup menus, unparent the menu as well
    எனில் (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	எனில் (menu->submenu_id != வெற்று)
	    gtk_widget_unparent(menu->submenu_id);
    }
    இல்லையெனில் எனில் (menu->parent == வெற்று && menu->id != வெற்று)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    இல்லையெனில் எனில் (menu->id != வெற்று)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->parent->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    வெறுமை
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    முழு x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    நிலையான வெறுமை
show_menubar_popover(வெறுமை)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    எனில் (gui.menubar == வெற்று || gui.drawarea == வெற்று)
	திரும்பு;

    எனில் (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), வெற்று);
	திரும்பு;
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
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), வெற்று);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#endif // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    வெறுமை
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    GtkAdjustment *adj;

    எனில் (sb->id == வெற்று)
	திரும்பு;
    எனில் (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	திரும்பு;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_set_lower(adj, 0.0);
    gtk_adjustment_set_upper(adj, (gdouble)max + 1);
    gtk_adjustment_set_மதிப்பு(adj, (gdouble)val);
    gtk_adjustment_set_step_increment(adj, 1.0);
    gtk_adjustment_set_page_increment(adj, (gdouble)(size > 2 ? size - 2 : 1));
    gtk_adjustment_set_page_size(adj, (gdouble)size);
}

    வெறுமை
gui_mch_set_scrollbar_pos(scrollbar_T *sb, முழு x, முழு y, முழு w, முழு h)
{
    எனில் (sb->id != வெற்று)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    முழு
gui_mch_get_scrollbar_xpadding(வெறுமை)
{
    முழு formwin_w = gtk_widget_get_width(gui.formwin);
    முழு sbar_w = 0;
    முழு xpad;

    எனில் (gui.which_scrollbars[SBAR_இடது])
	sbar_w += gui.scrollbar_width;
    எனில் (gui.which_scrollbars[SBAR_வலது])
	sbar_w += gui.scrollbar_width;

    xpad = formwin_w - last_text_area_w - sbar_w;
    திரும்பு (xpad < 0) ? 0 : xpad;
}

    முழு
gui_mch_get_scrollbar_ypadding(வெறுமை)
{
    முழு formwin_h = gtk_widget_get_height(gui.formwin);
    முழு ypad;

    ypad = formwin_h - last_text_area_h;
    எனில் (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_height;

    திரும்பு (ypad < 0) ? 0 : ypad;
}

    நிலையான வெறுமை
adjustment_மதிப்பு_changed(GtkAdjustment *adj, gpointer data UNUSED)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_data(G_OBJECT(adj), "vim-sb");
    long மதிப்பு = (long)gtk_adjustment_get_மதிப்பு(adj);

    எனில் (sb != வெற்று)
	gui_drag_scrollbar(sb, மதிப்பு, FALSE);
}

    வெறுமை
gui_mch_create_scrollbar(scrollbar_T *sb, முழு orient)
{
    GtkAdjustment *adj;
    எனில் (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, வெற்று);
    இல்லையெனில்
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, வெற்று);

    gtk_widget_add_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_set_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_set_data(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_மதிப்பு_changed), வெற்று);
}

    வெறுமை
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    எனில் (sb->id != வெற்று)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = வெற்று;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    வெறுமை
gui_mch_set_text_area_pos(முழு x, முழு y, முழு w, முழு h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    எனில் (w <= 0 || h <= 0)
	திரும்பு;
    last_text_area_w = w;
    last_text_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    வெறுமை
gui_gtk4_calculate_bleed(முழு width, முழு height)
{
    முழு old_வலது = gui.bleed_வலது;
    முழு old_bot = gui.bleed_bot;

    gui.bleed_வலது = width - last_text_area_w;
    gui.bleed_bot = height - last_text_area_h;

    எனில் (gui.which_scrollbars[SBAR_இடது])
	gui.bleed_வலது -= gui.scrollbar_width;
    எனில் (gui.which_scrollbars[SBAR_வலது])
	gui.bleed_வலது -= gui.scrollbar_width;
    எனில் (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_height;

    // Not sure if this can happen, but be safe...
    எனில் (gui.bleed_வலது < 0)
	gui.bleed_வலது = 0;
    எனில் (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    எனில் (old_வலது != gui.bleed_வலது || old_bot != gui.bleed_bot)
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
வகைவரை கட்டமைப்பு {
    GFile	*result;
    gboolean	done;
} FileDialogData;

    நிலையான வெறுமை
file_dialog_open_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_open_finish(
		    GTK_FILE_DIALOG(source), res, வெற்று);
    fdd->done = TRUE;
}

    நிலையான வெறுமை
file_dialog_save_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_save_finish(
		    GTK_FILE_DIALOG(source), res, வெற்று);
    fdd->done = TRUE;
}

    நிலையான வெறுமை
file_dialog_folder_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_select_folder_finish(
		    GTK_FILE_DIALOG(source), res, வெற்று);
    fdd->done = TRUE;
}

    char_u *
gui_mch_browse(முழு saving,
	char_u *title,
	char_u *dflt,
	char_u *ext UNUSED,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    FileDialogData  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *result = வெற்று;
    GListStore	    *filters;

    title = CONVERT_TO_UTF8(title);

    எனில் (initdir == வெற்று || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    இல்லையெனில் எனில் (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    add_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    எனில் (title != வெற்று)
	gtk_file_dialog_set_title(dlg, (மாறிலி எழுத்து *)title);

    {
	GFile *dir = g_file_new_for_path((மாறிலி எழுத்து *)dirbuf);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    எனில் (filter != வெற்று && *filter != NUL)
    {
	முழு		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	எனில் (patt != வெற்று)
	{
	    போது (p != வெற்று && *p != NUL)
	    {
		எனில் (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    எனில் (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    இல்லையெனில்
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			எனில் (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    எனில் (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		இல்லையெனில்
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

    எனில் (saving && dflt != வெற்று && *dflt != NUL)
	gtk_file_dialog_set_initial_name(dlg, (மாறிலி எழுத்து *)dflt);

    fdd.result = வெற்று;
    fdd.done = FALSE;

    எனில் (saving)
	gtk_file_dialog_save(dlg, GTK_WINDOW(gui.mainwin), வெற்று,
		file_dialog_save_cb, &fdd);
    இல்லையெனில்
	gtk_file_dialog_open(dlg, GTK_WINDOW(gui.mainwin), வெற்று,
		file_dialog_open_cb, &fdd);

    போது (!fdd.done)
	g_main_context_iteration(வெற்று, TRUE);

    எனில் (fdd.result != வெற்று)
    {
	எழுத்து *path = g_file_get_path(fdd.result);
	எனில் (path != வெற்று)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    திரும்பு result;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    FileDialogData	fdd;
    char_u		*result = வெற்று;

    title = CONVERT_TO_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    எனில் (title != வெற்று)
	gtk_file_dialog_set_title(dlg, (மாறிலி எழுத்து *)title);

    எனில் (initdir != வெற்று && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_path((மாறிலி எழுத்து *)initdir);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.result = வெற்று;
    fdd.done = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_WINDOW(gui.mainwin), வெற்று,
	    file_dialog_folder_cb, &fdd);

    போது (!fdd.done)
	g_main_context_iteration(வெற்று, TRUE);

    எனில் (fdd.result != வெற்று)
    {
	எழுத்து *path = g_file_get_path(fdd.result);
	எனில் (path != வெற்று)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    திரும்பு result;
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
    நிலையான எழுத்து **
split_button_string(char_u *button_string, முழு *n_buttons, எழுத்து **tofree)
{
    எழுத்து	    **array;
    char_u	    *p;
    unsigned முழு    மொத்தம் = 1;

    button_string = (char_u *)g_strdup((மாறிலி எழுத்து *)button_string);

    க்கு (p = button_string; *p != NUL; ++p)
	எனில் (*p == DLG_BUTTON_SEP)
	    ++மொத்தம்;

    array = g_malloc_n(மொத்தம், அளவு(எழுத்து *));
    மொத்தம் = 0;

    எனில் (array != வெற்று)
    {
	array[மொத்தம்++] = (எழுத்து *)button_string;
	க்கு (p = button_string; *p != NUL; )
	{
	    எனில் (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[மொத்தம்++] = (எழுத்து *)p;
	    }
	    இல்லையெனில் எனில் (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    இல்லையெனில்
		MB_PTR_ADV(p);
	}
    }

    *tofree = (எழுத்து *)button_string;
    *n_buttons = மொத்தம்;
    திரும்பு array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    நிலையான மாறிலி எழுத்து *
dialog_type_to_icon(முழு type)
{
    தேர்வு (type)
    {
	நிலை VIM_ERROR:
	    திரும்பு "dialog-error-symbolic";
	நிலை VIM_WARNING:
	    திரும்பு "dialog-warning-symbolic";
	நிலை VIM_INFO:
	    திரும்பு "dialog-information-symbolic";
	நிலை VIM_QUESTION:
	    திரும்பு "dialog-question-symbolic";
	இயல்பு:
	    நிறுத்து;
    }
    திரும்பு வெற்று;
}

// Data associated with each button in the dialog
வகைவரை கட்டமைப்பு
{
    முழு		but_idx;
    முழு		*response;
    gboolean	*done;
} DialogButtonState;

வகைவரை கட்டமைப்பு
{
    GtkWidget	*win;
    gboolean	*done;
    gboolean	no_alt;
} DialogState;

    நிலையான வெறுமை
dialog_button_clicked_cb(GtkButton *button UNUSED, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->done = TRUE;
}

    நிலையான gboolean
dialog_சாவி_pressed_cb(
	Gtk_Event_Controller_சாவி	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	DialogState		*dstate)
{
    எனில் (keyval == GDK_சாவி_Escape)
    {
	*dstate->done = TRUE;
	திரும்பு TRUE;
    }

    எனில் (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_mask()))
	திரும்பு gtk_widget_mnemonic_activate(dstate->win, FALSE);
    திரும்பு FALSE;
}

    நிலையான gboolean
dialog_close_request_cb(GtkWindow *win UNUSED, gboolean *win_closed)
{
    *win_closed = TRUE;
    திரும்பு FALSE;
}

    முழு
gui_mch_dialog(
	முழு	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	முழு	def_but,
	char_u	*textfield,
	முழு	ex_cmd UNUSED)
{
    GtkWindow		*win = GTK_WINDOW(gtk_window_new());
    GtkWidget		*vertbox;
    GtkWidget		*message_box;
    மாறிலி எழுத்து		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*entry = வெற்று;
    char_u		*utf8_title;
    char_u		*utf8_message;
    GtkEventController *சாவி_controller;
    DialogButtonState	*but_states = வெற்று;
    எழுத்து		*tofree = வெற்று;
    முழு			response = -1;
    gboolean		done = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_TO_UTF8(title);
    எனில் (utf8_title != வெற்று)
	gtk_window_set_title(win, (மாறிலி எழுத்து *)utf8_title);
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
    gtk_widget_set_margin_உச்சி(vertbox, 24);
    gtk_widget_set_margin_bottom(vertbox, 24);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);

    // Add the message label
    message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(message_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), message_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_to_icon(type);
    எனில் (icon_name != வெற்று)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(message_box), icon);
    }

    utf8_message = CONVERT_TO_UTF8(message);
    label = gtk_label_new((மாறிலி எழுத்து *)utf8_message);
    CONVERT_TO_UTF8_FREE(utf8_message);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(message_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    சாவி_controller = gtk_event_controller_சாவி_new();
    g_signal_connect(சாவி_controller, "key-pressed",
	    G_CALLBACK(dialog_சாவி_pressed_cb), &state);
    gtk_widget_add_controller(GTK_WIDGET(win), சாவி_controller);
    state.done = &done;
    state.win = GTK_WIDGET(win);

    எனில் (textfield != வெற்று)
    {
	// Add text entry so user can enter text
	char_u *utf8_text = CONVERT_TO_UTF8(textfield);

	entry = gtk_entry_new();

	எனில் (utf8_text != வெற்று)
	    gtk_editable_set_text(GTK_EDITABLE(entry), (மாறிலி எழுத்து *)utf8_text);
	இல்லையெனில்
	    gtk_editable_set_text(GTK_EDITABLE(entry), "");
	CONVERT_TO_UTF8_FREE(utf8_text);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_box_append(GTK_BOX(vertbox), entry);
	state.no_alt = FALSE;
    }
    இல்லையெனில்
    {
	GListModel  *controllers;
	முழு	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_பொருட்கள்(controllers);
	க்கு (முழு i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_item(controllers, i);
	    எனில் (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_set_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_window_set_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    எனில் (buttons != வெற்று)
    {
	GtkWidget   *but_box;
	எழுத்து	    **buttons_arr; // Note that array is allocated, not strings
	முழு	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	எனில் (vim_strchr(p_go, GO_VERTICAL) != வெற்று)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	இல்லையெனில்
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_set_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = split_button_string(buttons, &n_buttons, &tofree);

	but_states = g_malloc_n(n_buttons, அளவு(DialogButtonState));

	க்கு (முழு i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_states + i;

	    but_label = CONVERT_TO_UTF8((char_u *)buttons_arr[i]);
	    எனில் (but_label == வெற்று)
		தொடர்;

	    but = gtk_button_new_with_mnemonic((எழுத்து *)but_label);
	    எனில் (i == def_but - 1)
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

    போது (!done && !win_closed)
	g_main_context_iteration(வெற்று, TRUE);

    எனில் (done)
    {
	எனில் (textfield != வெற்று)
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
    திரும்பு response == -1 ? 0 : response + 1;
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

வகைவரை கட்டமைப்பு
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

நிலையான SharedFindReplace find_widgets = {0};
நிலையான SharedFindReplace repl_widgets = {0};

    நிலையான வெறுமை
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    முழு			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = GPOINTER_TO_INT(data);

    எனில் (flags == FRD_FINDNEXT)
    {
	repl_text = வெற்று;
	sfr = &find_widgets;
    }
    இல்லையெனில்
    {
	repl_text = (char_u *)gtk_editable_get_text(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(sfr->what));
    direction_down = gtk_check_button_get_active(
	    GTK_CHECK_BUTTON(sfr->down));

    எனில் (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    எனில் (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

    நிலையான வெறுமை
dialog_destroyed_cb(GtkWidget *widget UNUSED, gpointer data)
{
    *(GtkWidget **)data = வெற்று;
}

    நிலையான வெறுமை
entry_changed_cb(GtkWidget *entry, GtkWidget *dialog)
{
    மாறிலி gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    எனில் (!entry_text)
	திரும்பு; // Shouldn't happen

    nonempty = (entry_text[0] != '\0');

    எனில் (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    எனில் (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

    நிலையான gboolean
find_சாவி_pressed_cb(
	Gtk_Event_Controller_சாவி	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	SharedFindReplace	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    எனில் (state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	திரும்பு FALSE;

    // the Escape key synthesizes a cancellation action
    எனில் (keyval == GDK_சாவி_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_window_destroy(GTK_WINDOW(frdp->dialog));
	திரும்பு TRUE;
    }

    திரும்பு FALSE;
}

    நிலையான வெறுமை
entry_activate_cb(GtkWidget *widget UNUSED, வெறுமை *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    நிலையான வெறுமை
find_replace_dialog_create(char_u *arg, முழு do_replace)
{
    SharedFindReplace	*frdp;
    char_u		*entry_text;
    முழு			wword = FALSE;
    முழு			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*சாவி_controller;

    frdp = do_replace ? &repl_widgets : &find_widgets;
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    எனில் (entry_text != வெற்று && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = entry_text;
	entry_text = string_convert(&output_conv, entry_text, வெற்று);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    எனில் (frdp->dialog != வெற்று)
    {
	எனில் (entry_text != வெற்று)
	{
	    gtk_editable_set_text(GTK_EDITABLE(frdp->what),
		    (எழுத்து *)entry_text);
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
	எனில் (do_replace && g_utf8_strlen(
		    gtk_editable_get_text(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	திரும்பு;
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
    gtk_widget_set_margin_உச்சி(vertbox, 12);
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
    sensitive = (entry_text != வெற்று && entry_text[0] != NUL);
    எனில் (entry_text != வெற்று)
	gtk_editable_set_text(GTK_EDITABLE(frdp->what), (எழுத்து *)entry_text);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    எனில் (do_replace)
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
    இல்லையெனில்
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

    சாவி_controller = gtk_event_controller_சாவி_new();
    g_signal_connect(சாவி_controller, "key-pressed",
	    G_CALLBACK(find_சாவி_pressed_cb), frdp);
    gtk_widget_add_controller(GTK_WIDGET(frdp->dialog), சாவி_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(entry_changed_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_window_set_default_widget(GTK_WINDOW(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
	    GINT_TO_POINTER(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    எனில் (do_replace)
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
    எனில் (do_replace && entry_text != வெற்று && entry_text[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(entry_text);
}

    வெறுமை
gui_mch_find_dialog(exarg_T *eap)
{
    எனில் (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    வெறுமை
gui_mch_replace_dialog(exarg_T *eap)
{
    எனில் (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    வெறுமை
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
நிலையான GtkPrintDialog *print_dialog = வெற்று;
நிலையான GtkPrintSettings *print_settings = வெற்று;
நிலையான GtkPageSetup *page_setup = வெற்று;
நிலையான GtkPrintSetup *print_setup = வெற்று;
நிலையான GFile *print_file;

    நிலையான வெறுமை
print_dialog_cb(GtkPrintDialog *dialog, GAsyncResult *result, gboolean *done)
{
    GError *error = வெற்று;
    print_setup = gtk_print_dialog_setup_finish(dialog, result, &error);

    எனில் (print_setup == வெற்று)
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
	இரட்டை		*page_width,
	இரட்டை		*page_height)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    done = FALSE;
    char_u	    *temp;

    gtk_print_dialog_set_title(dialog, (மாறிலி எழுத்து *)jobname);
    gtk_print_dialog_set_modal(dialog, TRUE);

    print_dialog = வெற்று;
    print_setup = வெற்று;

    எனில் (print_settings == வெற்று)
    {
	option_table_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	எனில் (opt->present)
	{
	    எனில் (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    இல்லையெனில் எனில் (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    இல்லையெனில்
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	எனில் (opt->present)
	{
	    எனில் (*opt->string == 'y')
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_PORTRAIT);
	    இல்லையெனில்
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	எனில் (opt->present)
	{
	    gtk_print_settings_set_paper_size(print_settings,
		    gtk_paper_size_new((மாறிலி எழுத்து *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	எனில் (opt->present)
	{
	    எனில் (*opt->string == 'y')
		gtk_print_settings_set_collate(print_settings, TRUE);
	    இல்லையெனில்
		gtk_print_settings_set_collate(print_settings, FALSE);
	}
    }

    எனில் (page_setup == வெற்று)
	page_setup = gtk_page_setup_new();

    gtk_print_dialog_set_print_settings(dialog, print_settings);
    gtk_print_dialog_set_page_setup(dialog, page_setup);

    gtk_print_dialog_setup(dialog, GTK_WINDOW(gui.mainwin), வெற்று,
	    (GAsyncReadyCallback)print_dialog_cb, &done);

    போது (!done)
	g_main_context_iteration(வெற்று, TRUE);

    எனில் (print_setup == வெற்று)
    {
	g_object_unref(dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_செய்தி = FALSE;
	திரும்பு வெற்று;
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
    எனில் (temp == வெற்று)
    {
	emsg(_(e_cant_find_temp_file_for_writing));
	g_clear_object(&dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_செய்தி = FALSE;
	திரும்பு வெற்று;
    }
    print_file = g_file_new_for_path((எழுத்து *)temp);

    print_dialog = dialog;

    திரும்பு temp;
}

    நிலையான வெறுமை
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsyncResult	*result,
	GFile		*file)
{
    எழுத்து *path = g_file_get_path(file);

    (வெறுமை)gtk_print_dialog_print_file_finish(dialog, result, வெற்று);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(path);
    g_free(path);
}


    வெறுமை
gui_gtk4_print_finish(வெறுமை)
{
    எனில் (print_dialog != வெற்று)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_WINDOW(gui.mainwin),
		print_setup, print_file, வெற்று,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = வெற்று;
	g_clear_pointer(&print_setup, gtk_print_setup_unref);
    }
}
#endif // USE_GTK4_PRINT_DIALOG

#ifdef FEAT_IMAGE_GUI

வகைவரை கட்டமைப்பு
{
    GdkTexture *texture;
} image_gdk_T;

வகைவரை கட்டமைப்பு
{
    Gsk_Render_முனை   **nodes; // Note that elements may be NULL
    முழு		    n_nodes;
} image_placement_gdk_T;

    முழு
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    GdkMemoryFormat fmt;
    size_t	    stride;
    GBytes	    *bytes;
    size_t	    size;
    முழு		    w, h;

    எனில் (ctx == வெற்று)
	திரும்பு FAIL;

    எனில் (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_MEMORY_R8G8B8A8;
    இல்லையெனில்
	fmt = GDK_MEMORY_R8G8B8;

    image_get_dimensions(img, &w, &h);

    size = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    bytes = g_bytes_new(img->data, size);
    ctx->texture = gdk_memory_texture_new(w, h, fmt, bytes, stride);
    g_bytes_unref(bytes);

    img->backend_data = ctx;
    திரும்பு OK;
}

    வெறுமை
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_data;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    நிலையான வெறுமை
clear_nodes(image_placement_gdk_T *ctx)
{
    எனில் (ctx->nodes == வெற்று || gui.drawarea == வெற்று)
	திரும்பு;

    க்கு (முழு i = 0; i < ctx->n_nodes; i++)
    {
	எனில் (ctx->nodes[i] == வெற்று)
	    தொடர்;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_முனை_unref(ctx->nodes[i]);
    }
    g_clear_pointer(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    முழு
image_placement_gui_init(image_placement_T *place)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    எனில் (ctx == வெற்று)
	திரும்பு FAIL;

    place->backend_data = ctx;
    திரும்பு OK;
}

    வெறுமை
image_placement_gui_uninit(image_placement_T *place)
{
    image_placement_gdk_T *ctx = place->backend_data;

    clear_nodes(ctx);
    vim_free(ctx);
}

    வெறுமை
image_placement_gui_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_gdk_T   *ctx = place->backend_data;
    image_gdk_T		    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    முழு			    n_rects;
    முழு			    iw, ih;
    Gsk_Render_முனை	    *texture_முனை;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    எனில் (rects == வெற்று)
	திரும்பு;

    clear_nodes(ctx);
    எனில் (n_rects == 0)
	திரும்பு;

    image_get_dimensions(img, &iw, &ih);
    texture_முனை = gsk_texture_முனை_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(place->col) - FILL_X(place->crop_box.x1),
		FILL_Y(place->row) - FILL_Y(place->crop_box.y1)
		+ FILL_Y(place->row_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    எனில் (texture_முனை == வெற்று)
	// Not sure if this can happen...
	திரும்பு;

    ctx->nodes = g_malloc_n(n_rects, அளவு(Gsk_Render_முனை *));
    ctx->n_nodes = n_rects;

    க்கு (முழு i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	முழு		row, col;
	முழு		posx, posy;
	முழு		x, y, w, h;
	Gsk_Render_முனை	*முனை;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	முனை = gsk_clip_முனை_new(texture_முனை,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	எனில் (முனை != வெற்று)
	    vim_draw_area_add_external(VIM_DRAW_AREA(gui.drawarea), முனை);

	ctx->nodes[i] = முனை;
    }
    gsk_render_முனை_unref(texture_முனை);
}

    வெறுமை
image_placement_gui_clear(image_placement_T *place)
{
    image_placement_gdk_T   *ctx = place->backend_data;

    clear_nodes(ctx);
}

#endif

#endif // FEAT_GUI_GTK
