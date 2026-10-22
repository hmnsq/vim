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

#અન્તર્ભાવ "vim.h"

#ifdef FEAT_GUI_GTK

#અન્તર્ભાવ <gdk/gdkkeysyms.h>
#અન્તર્ભાવ <gdk/gdk.h>
#અન્તર્ભાવ <gtk/gtk.h>
#અન્તર્ભાવ "gui_gtk4_f.h"
#અન્તર્ભાવ "gui_gtk4_cb.h"
#અન્તર્ભાવ "gui_gtk4_da.h"
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
#define No_કિંમત		0x0000
#define Width_કિંમત	0x0004
#define Height_કિંમત	0x0008

    સ્થિર પૂર્ણાંક
vim_parse_geometry(અચલ અક્ષર *str, unsigned પૂર્ણાંક *width, unsigned પૂર્ણાંક *height)
{
    પૂર્ણાંક mask = No_કિંમત;
    અક્ષર *end;
    long val;

    જો (str == ખાલી || *str == NUL)
	પાછા mask;

    // Parse width
    જો (*str != '+' && *str != '-')
    {
	val = strtol(str, &end, 10);
	જો (end != str)
	{
	    *width = (unsigned પૂર્ણાંક)val;
	    mask |= Width_કિંમત;
	    str = end;
	}
    }

    // Parse 'x' or 'X' separator and height
    જો (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &end, 10);
	જો (end != str)
	{
	    *height = (unsigned પૂર્ણાંક)val;
	    mask |= Height_કિંમત;
	    str = end;
	}
    }

    પાછા mask;
}

#જો defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
સ્થિર પૂર્ણાંક last_shape = 0;
#endif

#define DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
સ્થિર ગણના {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
સ્થિર પૂર્ણાંક gtk4_main_loop_level = 0;
સ્થિર પૂર્ણાંક gtk4_main_loop_quit = FALSE;

#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

પ્રકારવ્યાખ્યા gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
સ્થિર સંરચના special_ચાવી
{
    guint ચાવી_sym;
    char_u code0;
    char_u code1;
}
અચલ special_ચાવીઓ[] =
{
    {GDK_ચાવી_Up,	'k', 'u'},
    {GDK_ચાવી_Down,	'k', 'd'},
    {GDK_ચાવી_ડાબું,	'k', 'l'},
    {GDK_ચાવી_જમણું,	'k', 'r'},
    {GDK_ચાવી_F1,	'k', '1'},
    {GDK_ચાવી_F2,	'k', '2'},
    {GDK_ચાવી_F3,	'k', '3'},
    {GDK_ચાવી_F4,	'k', '4'},
    {GDK_ચાવી_F5,	'k', '5'},
    {GDK_ચાવી_F6,	'k', '6'},
    {GDK_ચાવી_F7,	'k', '7'},
    {GDK_ચાવી_F8,	'k', '8'},
    {GDK_ચાવી_F9,	'k', '9'},
    {GDK_ચાવી_F10,	'k', ';'},
    {GDK_ચાવી_F11,	'F', '1'},
    {GDK_ચાવી_F12,	'F', '2'},
    {GDK_ચાવી_Help,	'%', '1'},
    {GDK_ચાવી_Undo,	'&', '8'},
    {GDK_ચાવી_Back_Space,	'k', 'b'},
    {GDK_ચાવી_ઉમેરો,	'k', 'I'},
    {GDK_ચાવી_Delete,	'k', 'D'},
    {GDK_ચાવી_Home,	'k', 'h'},
    {GDK_ચાવી_End,	'@', '7'},
    {GDK_ચાવી_Prior,	'k', 'P'},
    {GDK_ચાવી_Next,	'k', 'N'},
    {GDK_ચાવી_Print,	'%', '9'},
    {GDK_ચાવી_KP_ડાબું,	'k', 'l'},
    {GDK_ચાવી_KP_જમણું,	'k', 'r'},
    {GDK_ચાવી_KP_Up,	'k', 'u'},
    {GDK_ચાવી_KP_Down,	'k', 'd'},
    {GDK_ચાવી_KP_ઉમેરો,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_ચાવી_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_ચાવી_KP_Home,	'K', '1'},
    {GDK_ચાવી_KP_End,	'K', '4'},
    {GDK_ચાવી_KP_Prior,	'K', '3'},
    {GDK_ચાવી_KP_Next,	'K', '5'},
    {GDK_ચાવી_KP_Add,	'K', '6'},
    {GDK_ચાવી_KP_Subtract, 'K', '7'},
    {GDK_ચાવી_KP_Divide,	'K', '8'},
    {GDK_ચાવી_KP_Multiply, 'K', '9'},
    {GDK_ચાવી_KP_Enter,	'K', 'A'},
    {GDK_ચાવી_KP_Decimal,	'K', 'B'},
    {GDK_ચાવી_KP_0,	'K', 'C'},
    {GDK_ચાવી_KP_1,	'K', 'D'},
    {GDK_ચાવી_KP_2,	'K', 'E'},
    {GDK_ચાવી_KP_3,	'K', 'F'},
    {GDK_ચાવી_KP_4,	'K', 'G'},
    {GDK_ચાવી_KP_5,	'K', 'H'},
    {GDK_ચાવી_KP_6,	'K', 'I'},
    {GDK_ચાવી_KP_7,	'K', 'J'},
    {GDK_ચાવી_KP_8,	'K', 'K'},
    {GDK_ચાવી_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    સ્થિર પૂર્ણાંક
keyval_to_string(unsigned પૂર્ણાંક keyval, char_u *string)
{
    પૂર્ણાંક		len;
    guint32	uc;

    uc = gdk_keyval_to_unicode(keyval);
    જો (uc != 0)
    {
	len = utf_char2bytes((પૂર્ણાંક)uc, string);
    }
    નહીંતર
    {
	len = 1;
	પસંદગી (keyval)
	{
	    સ્થિતિ GDK_ચાવી_Tab: સ્થિતિ GDK_ચાવી_KP_Tab: સ્થિતિ GDK_ચાવી_ISO_ડાબું_Tab:
		string[0] = TAB;
		તોડો;
	    સ્થિતિ GDK_ચાવી_Linefeed:
		string[0] = NL;
		તોડો;
	    સ્થિતિ GDK_ચાવી_Return: સ્થિતિ GDK_ચાવી_ISO_Enter: સ્થિતિ GDK_ચાવી_3270_Enter:
		string[0] = CAR;
		તોડો;
	    સ્થિતિ GDK_ચાવી_Escape:
		string[0] = ESC;
		તોડો;
	    મૂળભૂત:
		len = 0;
		તોડો;
	}
    }
    string[len] = NUL;
    પાછા len;
}

    સ્થિર પૂર્ણાંક
modifiers_gdk2vim(guint state)
{
    પૂર્ણાંક modifiers = 0;

    જો (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    જો (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    જો (state & GDK_ALT_MASK)
	modifiers |= MOD_MASK_ALT;
    જો (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    જો (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;

    પાછા modifiers;
}

સ્થિર GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
સ્થિર gboolean ચાવી_press_event(Gtk_Event_Controller_ચાવી *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
સ્થિર શૂન્ય ચાવી_release_event(Gtk_Event_Controller_ચાવી *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
સ્થિર શૂન્ય button_press_event(GtkGestureClick *gesture, પૂર્ણાંક n_press, દ્વિગુણ x, દ્વિગુણ y, gpointer data);
સ્થિર શૂન્ય button_release_event(GtkGestureClick *gesture, પૂર્ણાંક n_press, દ્વિગુણ x, દ્વિગુણ y, gpointer data);
સ્થિર શૂન્ય motion_notify_event(GtkEventControllerMotion *controller, દ્વિગુણ x, દ્વિગુણ y, gpointer data);
સ્થિર શૂન્ય enter_notify_event(GtkEventControllerMotion *controller, દ્વિગુણ x, દ્વિગુણ y, gpointer data);
સ્થિર gboolean scroll_event(GtkEventControllerScroll *controller, દ્વિગુણ dx, દ્વિગુણ dy, gpointer data);
સ્થિર શૂન્ય focus_in_event(GtkEventControllerFocus *controller, gpointer data);
સ્થિર શૂન્ય focus_out_event(GtkEventControllerFocus *controller, gpointer data);
#ifdef FEAT_DND
સ્થિર gboolean drop_cb(GtkDropTargetAsync *target, GdkDrop *drop, દ્વિગુણ x, દ્વિગુણ y, શૂન્ય *data);
#endif
#ifdef FEAT_GUI_TABLINE
સ્થિર શૂન્ય tabline_enter_cb(GtkEventController *controller, દ્વિગુણ x, દ્વિગુણ y, શૂન્ય *udata);
સ્થિર શૂન્ય on_select_tab(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
સ્થિર શૂન્ય on_tab_reordered(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
સ્થિર VimMenu *create_tabline_popup_menu(શૂન્ય);
સ્થિર શૂન્ય tabline_add_css(શૂન્ય);
સ્થિર શૂન્ય tabline_menu_press_event(GtkGestureClick *gesture, પૂર્ણાંક n_press, દ્વિગુણ x, દ્વિગુણ y, GtkWidget *popover);
#endif
સ્થિર શૂન્ય mainwin_destroy_cb(GObject *object, gpointer data);
સ્થિર gboolean delete_event_cb(GtkWindow *window, gpointer data);
સ્થિર પૂર્ણાંક query_pointer_pos(પૂર્ણાંક *x, પૂર્ણાંક *y, GdkModifierType *state);
સ્થિર શૂન્ય mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_data);
સ્થિર શૂન્ય set_form_size(પૂર્ણાંક width, પૂર્ણાંક height);
સ્થિર શૂન્ય drawarea_realize_cb(GtkWidget *widget, gpointer data);
સ્થિર શૂન્ય drawarea_unrealize_cb(GtkWidget *widget, gpointer data);
સ્થિર શૂન્ય clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data);
#ifdef FEAT_MENU
સ્થિર શૂન્ય show_menubar_popover(શૂન્ય);
#endif

સ્થિર અચલ અક્ષર *prgname = ખાલી;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    સ્થિર gboolean
arg_match(અચલ અક્ષર *s, અચલ અક્ષર *name, અક્ષર **કિંમત)
{
    size_t  len = strlen(name);

    જો (strncmp(s, name, len) != 0)
	પાછા FALSE;
    જો (s[len] == '=')
	*કિંમત = (અક્ષર *)s + len + 1;
    નહીંતર જો (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	પાછા FALSE;
    પાછા TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    શૂન્ય
gui_mch_prepare(પૂર્ણાંક *argc, અક્ષર **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    પૂર્ણાંક	i = 0;

    જ્યારે (i < *argc)
    {
	અક્ષર	*s = argv[i];
	અક્ષર	*કિંમત = ખાલી;
	પૂર્ણાંક	has_inline_કિંમત = FALSE;
	પૂર્ણાંક	n_strip;

	જો (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    ચાલુ;
	}

	જો (strchr(s, '=') != ખાલી)
	    has_inline_કિંમત = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	જો (!has_inline_કિંમત)
	{
	    જો (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		કિંમત = argv[i + 1];
	    નહીંતર
		કિંમત = ખાલી;
	}

	જો (arg_match(s, "-fn", &કિંમત) || arg_match(s, "-font", &કિંમત))
	    font_argument = કિંમત;
	નહીંતર જો (arg_match(s, "-geom", &કિંમત)
		|| arg_match(s, "-geometry", &કિંમત))
	{
	    જો (કિંમત != ખાલી)
		gui.geom = vim_strsave((char_u *)કિંમત);
	}
	નહીંતર જો (arg_match(s, "-bg", &કિંમત)
		|| arg_match(s, "-background", &કિંમત))
	    background_argument = કિંમત;
	નહીંતર જો (arg_match(s, "-fg", &કિંમત)
		|| arg_match(s, "-foreground", &કિંમત))
	    foreground_argument = કિંમત;
#ifdef FEAT_NETBEANS_INTG
	નહીંતર જો (strncmp(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_કિંમત = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#endif
	નહીંતર જો (arg_match(s, "--prg-name", &કિંમત))
	    // GTK4 specific
	    prgname = કિંમત;
	નહીંતર
	{
	    i++;
	    ચાલુ;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	જો (કિંમત != ખાલી && !has_inline_કિંમત)
	    n_strip = 2;

	જો (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    જો (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * કદ(અક્ષર *));
	    argv[*argc] = ખાલી;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    શૂન્ય
gui_mch_free_all(શૂન્ય)
{
}

    સ્થિર guint
timeout_add(પૂર્ણાંક time, timeout_cb_type (*callback)(gpointer), પૂર્ણાંક *flagp)
{
    પાછા g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    સ્થિર શૂન્ય
timeout_remove(guint timer)
{
    g_source_remove(timer);
}

સ્થિર long_u blink_waittime = 700;
સ્થિર long_u blink_ontime = 400;
સ્થિર long_u blink_offtime = 250;
સ્થિર guint blink_timer = 0;

    સ્થિર timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    જો (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, ખાલી);
    }
    નહીંતર
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, ખાલી);
    }
    પાછા FALSE;
}

    પૂર્ણાંક
gui_mch_is_blinking(શૂન્ય)
{
    પાછા blink_state != BLINK_NONE;
}

    પૂર્ણાંક
gui_mch_is_blink_off(શૂન્ય)
{
    પાછા blink_state == BLINK_OFF;
}

    શૂન્ય
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    શૂન્ય
gui_mch_stop_blink(પૂર્ણાંક may_call_gui_update_cursor)
{
    જો (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    જો (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    શૂન્ય
gui_mch_start_blink(શૂન્ય)
{
    જો (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    જો (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, ખાલી);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    પૂર્ણાંક
gui_mch_early_init_check(પૂર્ણાંક give_message UNUSED)
{
    પાછા OK;
}

    પૂર્ણાંક
gui_mch_init_check(શૂન્ય)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_set_prgname(prgname == ખાલી ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    જો (g_getenv("EGL_LOG_LEVEL") == ખાલી)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    પાછા OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    પૂર્ણાંક
gui_mch_init(શૂન્ય)
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
		     G_CALLBACK(delete_event_cb), ખાલી);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), ખાલી);

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
		     G_CALLBACK(on_select_tab), ખાલી);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ખાલી);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), ખાલી);

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
		     G_CALLBACK(drawarea_realize_cb), ખાલી);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ખાલી);

    // Set up event controllers.
    {
	GtkEventController *ચાવી_ctrl = gtk_event_controller_ચાવી_new();
	g_signal_connect(ચાવી_ctrl, "key-pressed",
			 G_CALLBACK(ચાવી_press_event), ખાલી);
	g_signal_connect(ચાવી_ctrl, "key-released",
			 G_CALLBACK(ચાવી_release_event), ખાલી);
	gtk_widget_add_controller(gui.drawarea, ચાવી_ctrl);
#ifdef FEAT_XIM
	xim_init();
#endif
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), ખાલી);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), ખાલી);
	gtk_widget_add_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), ખાલી);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), ખાલી);
	gtk_widget_add_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), ખાલી);
	gtk_widget_add_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_in_event), ખાલી);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_out_event), ખાલી);
	gtk_widget_add_controller(gui.drawarea, focus);
    }

#ifdef FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_target = gtk_drop_target_async_new(ખાલી,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_set_dnd_targets();
	g_signal_connect(gui.drop_target, "drop",
			 G_CALLBACK(drop_cb), ખાલી);
	gtk_widget_add_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_target));
    }
#endif

    gui.border_offset = gui.border_width;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_pointer = gdk_cursor_new_from_name("none", ખાલી);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	જો (primary != ખાલી)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_star);
	જો (regular != ખાલી)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_star);

    પાછા OK;
}

    શૂન્ય
gui_mch_new_colors(શૂન્ય)
{
    જો (gui.drawarea != ખાલી && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    પૂર્ણાંક
gui_mch_open(શૂન્ય)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_width;
    guint pixel_height;

    જો (gui.geom != ખાલી)
    {
	પૂર્ણાંક		mask;
	unsigned પૂર્ણાંક	w, h;

	mask = vim_parse_geometry((અક્ષર *)gui.geom, &w, &h);

	જો (mask & Width_કિંમત)
	    Columns = w;
	જો (mask & Height_કિંમત)
	{
	    જો (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	VIM_CLEAR(gui.geom);
    }
    નહીંતર
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	જો (Columns > 80)
	    Columns = 80;
	જો (Rows > 24)
	    Rows = 24;
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    pixel_width  += get_menu_tool_width();
    pixel_height += get_menu_tool_height();

    જો (foreground_argument != ખાલી)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    જો (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    જો (background_argument != ખાલી)
	bg_pixel = gui_get_color((char_u *)background_argument);
    જો (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    જો (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    નહીંતર
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    set_normal_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ખાલી);
    // Resize is handled by GtkForm's size_allocate callback.

    set_form_size((પૂર્ણાંક)pixel_width - get_menu_tool_width(),
	    (પૂર્ણાંક)pixel_height - get_menu_tool_height());

    gtk_window_present(GTK_WINDOW(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    પાછા OK;
}

    શૂન્ય
gui_mch_exit(પૂર્ણાંક rc UNUSED)
{
    જો (gui.mainwin != ખાલી)
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
		જો ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != ખાલી)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#endif
	gtk_window_destroy(GTK_WINDOW(gui.mainwin));
    }
}

    પૂર્ણાંક
gui_mch_get_winpos(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    પાછા FAIL;
}

    શૂન્ય
gui_mch_set_winpos(પૂર્ણાંક x UNUSED, પૂર્ણાંક y UNUSED)
{
    // GTK4/Wayland: window positioning not available
}

    પૂર્ણાંક
gui_mch_maximized(શૂન્ય)
{
    પાછા gtk_window_is_maximized(GTK_WINDOW(gui.mainwin));
}

    શૂન્ય
gui_mch_unmaximize(શૂન્ય)
{
    જો (gui.mainwin != ખાલી)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

    શૂન્ય
gui_mch_set_fullscreen(પૂર્ણાંક flag)
{
    જો (gui.mainwin == ખાલી)
	પાછા;
    જો (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    નહીંતર
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}

    સ્થિર શૂન્ય
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec UNUSED, gpointer user_data UNUSED)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    જો (gtk_window_is_fullscreen(GTK_WINDOW(obj)))
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
    શૂન્ય
gui_mch_newfont(શૂન્ય)
{
    પૂર્ણાંક w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_width(gui.formwin);
    h = gtk_widget_get_height(gui.formwin);
    gui_resize_shell(w, h);
}

    શૂન્ય
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    જો (title != ખાલી && gui.mainwin != ખાલી)
	gtk_window_set_title(GTK_WINDOW(gui.mainwin), (અચલ અક્ષર *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    શૂન્ય
gui_gtk_init_decor_height(શૂન્ય)
{
    પૂર્ણાંક h = gtk_widget_get_height(gui.mainwin);

    જો (h == 0)
	પાછા;

    h -= get_menu_tool_height();
    h -= gtk_widget_get_height(gui.formwin);

    gui.decor_height = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    સ્થિર શૂન્ય
set_form_size(પૂર્ણાંક width, પૂર્ણાંક height)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    જો (gtk_widget_get_width(gui.formwin) == width
	    && gtk_widget_get_height(gui.formwin) == height)
	પાછા;

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

    શૂન્ય
gui_mch_set_shellsize(પૂર્ણાંક width, પૂર્ણાંક height,
	પૂર્ણાંક min_width UNUSED, પૂર્ણાંક min_height UNUSED,
	પૂર્ણાંક base_width UNUSED, પૂર્ણાંક base_height UNUSED,
	પૂર્ણાંક direction UNUSED)
{
    set_form_size(width, height);

    gui_mch_update();
}

    શૂન્ય
gui_mch_get_screen_dimensions(પૂર્ણાંક *screen_w, પૂર્ણાંક *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    જો (surface != ખાલી)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	જો (monitor != ખાલી)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.width;
	    *screen_h = geom.height;
	    પાછા;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ifdef FEAT_MENU
    શૂન્ય
gui_mch_enable_menu(પૂર્ણાંક showit)
{
    જો (gui.menubar != ખાલી)
    {
	gtk_widget_set_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	જો (gui.drawarea != ખાલી)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

#ifdef FEAT_TOOLBAR
    શૂન્ય
gui_mch_show_toolbar(પૂર્ણાંક showit)
{
    જો (gui.toolbar != ખાલી)
    {
	gtk_widget_set_visible(gui.toolbar, showit);
	જો (showit)
	    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	જો (gui.drawarea != ખાલી)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

    શૂન્ય
gui_mch_set_dark_theme(પૂર્ણાંક dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    જો (settings != ખાલી)
	g_object_set(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, ખાલી);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    પૂર્ણાંક
gui_mch_adjust_charheight(શૂન્ય)
{
    PangoFontMetrics *metrics;
    પૂર્ણાંક ascent;
    પૂર્ણાંક descent;

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

    પાછા OK;
}

પ્રકારવ્યાખ્યા સંરચના {
    PangoFontDescription    *result;
    gboolean		    done;
} FontDialogData;

    સ્થિર શૂન્ય
font_dialog_finish_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FontDialogData *fdd = (FontDialogData *)data;
    fdd->result = gtk_font_dialog_choose_font_finish(
		    GTK_FONT_DIALOG(source), res, ખાલી);
    fdd->done = TRUE;
}

    સ્થિર gboolean
font_filter(gpointer item, gpointer data UNUSED)
{
    જો (PANGO_IS_FONT_FAMILY(item))
	પાછા pango_font_family_is_monospace(PANGO_FONT_FAMILY(item));
    જો (PANGO_IS_FONT_FACE(item))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(item));
	જો (family != ખાલી)
	    પાછા pango_font_family_is_monospace(family);
    }
    પાછા TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = ખાલી;
    char_u		*fontname = ખાલી;
    FontDialogData	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_set_modal(dlg, TRUE);
    gtk_font_dialog_set_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, ખાલી, ખાલી)));

    જો (oldval != ખાલી && oldval[0] != NUL)
    {
	char_u *oldname;

	જો (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ખાલી);
	નહીંતર
	    oldname = oldval;

	જો (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    જો (p != ખાલી)
	    {
		STRCPY(p + STRLEN(p), " 10");
		જો (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((અચલ અક્ષર *)oldname);
	જો (oldname != oldval)
	    vim_free(oldname);
    }
    નહીંતર
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.result = ખાલી;
    fdd.done = FALSE;

    gtk_font_dialog_choose_font(dlg, GTK_WINDOW(gui.mainwin),
	    initial, ખાલી, font_dialog_finish_cb, &fdd);

    જ્યારે (!fdd.done)
	g_main_context_iteration(ખાલી, TRUE);

    જો (fdd.result != ખાલી)
    {
	અક્ષર *name = pango_font_description_to_string(fdd.result);
	જો (name != ખાલી)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    જો (p != ખાલી && આગત_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&આગત_conv, p, ખાલી);
		vim_free(p);
	    }
	    નહીંતર
		fontname = p;
	}
	pango_font_description_free(fdd.result);
    }

    જો (initial != ખાલી)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    પાછા fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    સ્થિર શૂન્ય
ascii_glyph_table_init(શૂન્ય)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    પૂર્ણાંક		    i;

    જો (gui.ascii_glyphs != ખાલી)
	pango_glyph_string_free(gui.ascii_glyphs);
    જો (gui.ascii_font != ખાલી)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ખાલી;
    gui.ascii_font   = ખાલી;

    માટે (i = 0; i < 128; ++i)
    {
	જો (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	નહીંતર
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (અચલ અક્ષર *)ascii_chars,
			      0, કદ(ascii_chars), attr_list, ખાલી);

    જો (item_list != ખાલી && item_list->next == ખાલી)
    {
	PangoItem   *item;
	પૂર્ણાંક	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((અચલ અક્ષર *)ascii_chars, કદ(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	જો (gui.ascii_glyphs->અંક_glyphs == (પૂર્ણાંક)કદ(ascii_chars))
	{
	    માટે (i = 0; i < gui.ascii_glyphs->અંક_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_offset += MAX(0, width - geom->width) / 2;
		geom->width = width;
	    }
	}
	નહીંતર
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = ખાલી;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = ખાલી;
	}
    }

    g_list_foreach(item_list, (GFunc)(શૂન્ય *)&pango_item_free, ખાલી);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    સ્થિર શૂન્ય
get_styled_font_variants(શૂન્ય)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);
    જો (plain_font == ખાલી)
	પાછા;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    જો (bold_font != ખાલી)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    પૂર્ણાંક
gui_mch_init_font(char_u *font_name, પૂર્ણાંક fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    પૂર્ણાંક			    width;

    જો (font_name == ખાલી)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    જો (font_desc == ખાલી)
	પાછા FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, ખાલી);
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    જો (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	પૂર્ણાંક im_row = gui.row < screen_Rows ? gui.row : screen_Rows - 1;
	પૂર્ણાંક im_col = gui.col < screen_Columns ? gui.col : screen_Columns - 1;
	im_set_position(im_row, im_col);
    }
    પાછા OK;
}

    GuiFont
gui_mch_get_font(char_u *name, પૂર્ણાંક report_error)
{
    PangoFontDescription *font;

    જો (name == ખાલી)
	પાછા ખાલી;

    font = pango_font_description_from_string((અચલ અક્ષર *)name);
    જો (font == ખાલી)
    {
	જો (report_error)
	    semsg(_(e_unknown_font_str), name);
	પાછા ખાલી;
    }

    // Ensure a size is set
    જો (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    પાછા font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    જો (font != NOFONT)
    {
	અક્ષર *desc = pango_font_description_to_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	પાછા ret;
    }
    પાછા ખાલી;
}

    શૂન્ય
gui_mch_free_font(GuiFont font)
{
    જો (font != NOFONT)
	pango_font_description_free(font);
}

    શૂન્ય
gui_mch_expand_font(
    optexpand_T	*args,
    શૂન્ય	*param,
    પૂર્ણાંક		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = ખાલી;
    પૂર્ણાંક			n_families = 0;
    પૂર્ણાંક			wide = *(પૂર્ણાંક *)param;

    જો (args->oe_include_orig_val && *args->oe_opt_કિંમત == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	જો (add_match((char_u *)DEFAULT_FONT) != OK)
	    પાછા;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    માટે (પૂર્ણાંક i = 0; i < n_families; i++)
    {
	જો (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ચાલુ;

	અચલ અક્ષર *fam_name = pango_font_family_get_name(font_families[i]);
	જો (આગત_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&આગત_conv,
		    (char_u *)fam_name, ખાલી);
	    જો (buf != ખાલી)
	    {
		જો (add_match(buf) != OK)
		{
		    vim_free(buf);
		    તોડો;
		}
		vim_free(buf);
	    }
	    નહીંતર
		તોડો;
	}
	નહીંતર
	{
	    જો (add_match((char_u *)fam_name) != OK)
		તોડો;
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
    જો (!gui.in_use)
	પાછા INVALCOLOR;

    જો (name != ખાલી)
	પાછા gui_get_color_cmn(name);

    પાછા INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(પૂર્ણાંક r, પૂર્ણાંક g, પૂર્ણાંક b)
{
    પાછા gui_get_rgb_color_cmn(r, g, b);
}

    સ્થિર GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    પાછા rgba;
}

    શૂન્ય
gui_mch_set_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_to_rgba(color);
}

    શૂન્ય
gui_mch_set_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_to_rgba(color);
}

    શૂન્ય
gui_mch_set_sp_color(guicolor_T color)
{
    *gui.spcolor = color_to_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    પાછા pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    શૂન્ય
gui_gtk4_update_size(શૂન્ય)
{
    vim_draw_area_set_size(VIM_DRAW_AREA(gui.drawarea),
	    gui.અંક_rows, gui.અંક_cols);
}

#ifdef FEAT_NETBEANS_INTG
    શૂન્ય
gui_gtk4_add_multisign(
	cairo_surface_t *surf,
	પૂર્ણાંક		row,
	પૂર્ણાંક		col,
	પૂર્ણાંક		width,
	પૂર્ણાંક		height)
{
    vim_draw_area_add_multisign(VIM_DRAW_AREA(gui.drawarea), surf, row, col,
	    width, height);
}
#endif

    શૂન્ય
gui_mch_clear_block(પૂર્ણાંક row1, પૂર્ણાંક col1, પૂર્ણાંક row2, પૂર્ણાંક col2)
{
    vim_draw_area_clear_block(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    જો (gui.drawarea != ખાલી)
	gtk_widget_queue_draw(gui.drawarea);
}

    શૂન્ય
gui_mch_clear_all(શૂન્ય)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    જો (gui.drawarea != ખાલી)
	gtk_widget_queue_draw(gui.drawarea);
}

    શૂન્ય
gui_mch_delete_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row, row + અંક_લીટીઓ, gui.scroll_region_bot,
	    gui.scroll_region_ડાબું, gui.scroll_region_જમણું);
    gui_clear_block(
	    gui.scroll_region_bot - અંક_લીટીઓ + 1, gui.scroll_region_ડાબું,
	    gui.scroll_region_bot, gui.scroll_region_જમણું);

    gtk_widget_queue_draw(gui.drawarea);
}

    શૂન્ય
gui_mch_ઉમેરો_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row + અંક_લીટીઓ, row, gui.scroll_region_bot - અંક_લીટીઓ,
	    gui.scroll_region_ડાબું, gui.scroll_region_જમણું);
    gui_clear_block(
	    row, gui.scroll_region_ડાબું,
	    row + અંક_લીટીઓ - 1, gui.scroll_region_જમણું);

    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    શૂન્ય
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, પૂર્ણાંક w, પૂર્ણાંક h)
{
    gui_mch_set_bg_color(bg);
    gui_mch_set_fg_color(fg);
    vim_draw_area_set_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_queue_draw(gui.drawarea);
}

    શૂન્ય
gui_mch_flash(પૂર્ણાંક msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (પૂર્ણાંક)Rows - 1, (પૂર્ણાંક)Columns - 1);
    gui_mch_flush();
    ui_delay((long)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (પૂર્ણાંક)Rows - 1, (પૂર્ણાંક)Columns - 1);
}

    શૂન્ય
gui_mch_invert_rectangle(પૂર્ણાંક r, પૂર્ણાંક c, પૂર્ણાંક nr, પૂર્ણાંક nc)
{
    vim_draw_area_invert_block(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    સ્થિર gboolean
ચાવી_press_event(Gtk_Event_Controller_ચાવી *controller UNUSED,
	guint ચાવી_sym, guint keycode UNUSED,
	GdkModifierType state, gpointer data UNUSED)
{
    char_u	string[32], string2[32];
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;
    પૂર્ણાંક		modifiers;
    પૂર્ણાંક		ચાવી;
    char_u	*s, *d;

#ifdef FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    જો (xic != ખાલી)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	જો (event != ખાલી && gtk_im_context_filter_keypress(xic, event))
	    પાછા TRUE;
    }
#endif

#ifdef FEAT_MENU
    જો (ચાવી_sym == GDK_ચાવી_F10 && gui.menubar != ખાલી)
    {
	સ્થિર char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	જો (check_map(k10, State, FALSE, TRUE, FALSE, ખાલી, ખાલી) == ખાલી)
	{
	    show_menubar_popover();
	    પાછા TRUE;
	}
    }
#endif

    len = keyval_to_string(ચાવી_sym, string2);

    જો (len > 1 && આગત_conv.vc_type != CONV_NONE)
	len = convert_આગત(string2, len, કદ(string2));

    s = string2;
    d = string;
    માટે (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	જો (d[-1] == CSI && d + 2 < string + કદ(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (પૂર્ણાંક)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    જો (ચાવી_sym == GDK_ચાવી_ISO_ડાબું_Tab)
    {
	ચાવી_sym = GDK_ચાવી_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    જો ((state & GDK_ALT_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	પાછા FALSE;
#endif

    // Check for special keys
    જો (len == 0 || len == 1)
    {
	માટે (i = 0; special_ચાવીઓ[i].ચાવી_sym != 0; i++)
	{
	    જો (special_ચાવીઓ[i].ચાવી_sym == ચાવી_sym)
	    {
		string[0] = CSI;
		string[1] = special_ચાવીઓ[i].code0;
		string[2] = special_ચાવીઓ[i].code1;
		len = -3;
		તોડો;
	    }
	}
    }

    જો (len == 0)
	પાછા TRUE;

    જો (len == -3)
	ચાવી = TO_SPECIAL(string[1], string[2]);
    નહીંતર
    {
	string[len] = NUL;
	ચાવી = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    ચાવી = simplify_ચાવી(ચાવી, &modifiers);
    જો (ચાવી == CSI)
	ચાવી = K_CSI;
    જો (IS_SPECIAL(ચાવી))
    {
	string[0] = CSI;
	string[1] = K_SECOND(ચાવી);
	string[2] = K_THIRD(ચાવી);
	len = 3;
    }
    નહીંતર
    {
	ચાવી = may_adjust_ચાવી_for_ctrl(modifiers, ચાવી);
	modifiers = may_remove_shift_modifier(modifiers, ચાવી);
	len = mb_char2bytes(ચાવી, string);
    }

    જો (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_આગત_buf(string2, 3);
    }

    {
	પૂર્ણાંક int_ch = check_for_interrupt(ચાવી, modifiers);
	જો (int_ch != NUL)
	{
	    trash_આગત_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_આગત_buf(string, len);

    જો (p_mh)
	gui_mch_mousehide(TRUE);

    પાછા TRUE;
}

    સ્થિર શૂન્ય
ચાવી_release_event(Gtk_Event_Controller_ચાવી *controller UNUSED,
	guint keyval UNUSED, guint keycode UNUSED,
	GdkModifierType state UNUSED, gpointer data UNUSED)
{
}

સ્થિર પૂર્ણાંક mouse_timed_out = TRUE;
સ્થિર guint mouse_click_timer = 0;

    સ્થિર timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    *(પૂર્ણાંક *)data = TRUE;
    પાછા FALSE;
}

    સ્થિર પૂર્ણાંક
modifiers_gdk2mouse(guint state)
{
    પૂર્ણાંક modifiers = 0;

    જો (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    જો (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    જો (state & GDK_ALT_MASK)
	modifiers |= MOUSE_ALT;

    પાછા modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#define ANY_BUTTON_MASK (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK \
	| GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
સ્થિર પૂર્ણાંક mouse_pressed_button = -1;

સ્થિર guint motion_repeat_timer = 0;
સ્થિર gboolean motion_repeat_offset = FALSE;
સ્થિર પૂર્ણાંક mouse_press_x = -1, mouse_press_y = -1;

    સ્થિર શૂન્ય
button_press_event(GtkGestureClick *gesture, પૂર્ણાંક n_press UNUSED,
	દ્વિગુણ x, દ્વિગુણ y, gpointer data UNUSED)
{
    પૂર્ણાંક button;
    પૂર્ણાંક repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    જો (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_out);

    પસંદગી (btn)
    {
	સ્થિતિ 1: button = MOUSE_ડાબું; તોડો;
	સ્થિતિ 2: button = MOUSE_મધ્ય; તોડો;
	સ્થિતિ 3: button = MOUSE_જમણું; તોડો;
	સ્થિતિ 8: button = MOUSE_X1; તોડો;
	સ્થિતિ 9: button = MOUSE_X2; તોડો;
	મૂળભૂત: પાછા;
    }

    mouse_press_x = (પૂર્ણાંક)x;
    mouse_press_y = (પૂર્ણાંક)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (પૂર્ણાંક)x, (પૂર્ણાંક)y, repeated_click, vim_modifiers);
}

    સ્થિર શૂન્ય
button_release_event(GtkGestureClick *gesture, પૂર્ણાંક n_press UNUSED,
	દ્વિગુણ x, દ્વિગુણ y, gpointer data UNUSED)
{
    પૂર્ણાંક vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    જો (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (પૂર્ણાંક)x, (પૂર્ણાંક)y, FALSE, vim_modifiers);
}

સ્થિર દ્વિગુણ prev_mouse_x = -1.0;
સ્થિર દ્વિગુણ prev_mouse_y = -1.0;

સ્થિર GdkModifierType cur_state = 0;

    સ્થિર timeout_cb_type
mouse_repeat_timer_cb(gpointer data UNUSED)
{
    પૂર્ણાંક x, y;

    જો (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	પાછા G_SOURCE_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    જો (vim_used_in_આગત_buf() > 10)
	પાછા G_SOURCE_CONTINUE;

    x = (પૂર્ણાંક)prev_mouse_x;
    y = (પૂર્ણાંક)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    જો (motion_repeat_offset)
	x += gui.char_width;
    motion_repeat_offset = !motion_repeat_offset;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    પાછા G_SOURCE_CONTINUE;
}

    સ્થિર શૂન્ય
motion_notify_event(GtkEventControllerMotion *controller UNUSED,
	દ્વિગુણ x, દ્વિગુણ y, gpointer data UNUSED)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    જો (mouse_pressed_button >= 0 && event != ખાલી
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_MASK))
    {
	જો (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    જો (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	પૂર્ણાંક		w, h;

	જો (event != ખાલી)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (પૂર્ણાંક)x, (પૂર્ણાંક)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_width(gui.drawarea);
	h = gtk_widget_get_height(gui.drawarea);

	જો (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	જો (x < 0 || y < 0 || x >= w || y >= h)
	{
	    પૂર્ણાંક	dx, dy;
	    પૂર્ણાંક offshoot;
	    પૂર્ણાંક	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    જો (offshoot > 127)
		delay = 5;
	    નહીંતર
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_add(delay,
		    mouse_repeat_timer_cb, ખાલી);
	}
	નહીંતર
	    motion_repeat_timer = 0;
    }
    નહીંતર
	gui_mouse_moved((પૂર્ણાંક)x, (પૂર્ણાંક)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    જો (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    સ્થિર શૂન્ય
enter_notify_event(GtkEventControllerMotion *controller UNUSED,
	દ્વિગુણ x, દ્વિગુણ y, gpointer data UNUSED)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    સ્થિર gboolean
scroll_event(GtkEventControllerScroll *controller UNUSED,
	દ્વિગુણ dx UNUSED, દ્વિગુણ dy, gpointer data UNUSED)
{
    પૂર્ણાંક button;
    int_u vim_modifiers;
    પૂર્ણાંક x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    જો (event == ખાલી)
	પાછા FALSE;
    state = gdk_event_get_modifier_state(event);

    જો (dy < 0)
	button = MOUSE_4;	// scroll up
    નહીંતર જો (dy > 0)
	button = MOUSE_5;	// scroll down
    નહીંતર જો (dx < 0)
	button = MOUSE_7;	// scroll left
    નહીંતર જો (dx > 0)
	button = MOUSE_6;	// scroll right
    નહીંતર
	પાછા FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    પાછા TRUE;
}

    સ્થિર શૂન્ય
focus_in_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    જો (gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	જો (blink_state == BLINK_NONE)
	    gui_mch_start_blink();
    }
}

    સ્થિર શૂન્ય
focus_out_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    જો (!gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	જો (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#જો defined(FEAT_IMAGE)
    સ્થિર શૂન્ય
scale_factor_cb(GdkSurface  *surface,
	GParamSpec	    *pspec UNUSED,
	શૂન્ય		    *udata UNUSED)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#endif

    સ્થિર શૂન્ય
drawarea_realize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#જો defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_factor_cb), ખાલી);
    redraw_all_later(UPD_VALID);
#endif
    gui_mch_new_colors();
}

    સ્થિર શૂન્ય
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ifdef FEAT_XIM
    im_shutdown();
#endif
}

પ્રકારવ્યાખ્યા ગણના
{
    READ_DATA_CLIP,	    // Clipboard data
    READ_DATA_DROP,	    // URI or text DND data
    READ_DATA_DROP_DATA	    // HTML DND data
} ReadDataType;

#define READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
પ્રકારવ્યાખ્યા સંરચના
{
    ReadDataType    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    અક્ષર	    *buf; // NULL when not needed
} ReadData;

#ifdef FEAT_DND
પ્રકારવ્યાખ્યા સંરચના
{
    ReadData	rd;
    GdkDrop	*drop;
    પૂર્ણાંક		x;
    પૂર્ણાંક		y;
} DropReadData;
#endif

પ્રકારવ્યાખ્યા સંરચના
{
    ReadData	rd;
    Clipboard_T *cbd;
    અક્ષર	*mime_type;
} ClipReadData;

    સ્થિર ReadData *
read_data_new(ReadDataType type, size_t sz)
{
    ReadData *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    જો (type == READ_DATA_CLIP || type == READ_DATA_DROP_DATA)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    પાછા rd;
}

    સ્થિર શૂન્ય
read_data_free(ReadData *rd)
{
    g_cancellable_cancel(rd->cancel);

    જો (rd->type == READ_DATA_CLIP)
	g_free(((ClipReadData *)rd)->mime_type);
    નહીંતર જો (rd->type == READ_DATA_DROP || rd->type == READ_DATA_DROP_DATA)
	g_object_unref(((DropReadData *)rd)->drop);
    g_object_unref(rd->cancel);
    જો (rd->arr != ખાલી)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ifdef FEAT_DND
સ્થિર શૂન્ય drop_read_text(GdkDrop *drop, char_u *text);
#endif

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    સ્થિર શૂન્ય
read_data_આગત_cb(
	GInputStream	*stream,
	GAsyncResult	*result,
	ReadData	*rd)
{
    ssize_t r = g_આગત_stream_read_finish(stream, result, ખાલી);

    જો (r == -1)
    {
	// Error occurred
	DropReadData *drd = (DropReadData *)rd;

	જો (rd->type == READ_DATA_DROP_DATA)
	    gdk_drop_finish(drd->drop, 0);
    }
    નહીંતર જો (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	જો (rd->type == READ_DATA_CLIP)
	{
	    ClipReadData    *crd = (ClipReadData *)rd;
	    long	    len;
	    char_u	    *actual, *final;
	    પૂર્ણાંક		    motion_type = MAUTO;
	    char_u	    *tofree = ખાલી;

	    len = (long)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ખાલી;

	    જો (clip_convert_data(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ifdef FEAT_DND
	નહીંતર જો (rd->type == READ_DATA_DROP_DATA)
	{
	    DropReadData    *drd = (DropReadData *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (અચલ uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ખાલી;
	    drop_read_text(drd->drop, str);
	    g_free(str);
	    gdk_drop_finish(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#endif
    }
    નહીંતર
    {
	// Continue reading
	g_byte_array_append(rd->arr, (અચલ uint8_t *)rd->buf, r);
	g_આગત_stream_read_async(stream, rd->buf, READDATA_BUFSIZE,
		G_PRIORITY_ઉપલું,
		rd->cancel, (GAsyncReadyCallback)read_data_આગત_cb, rd);
	પાછા;
    }

    read_data_free(rd);
    g_object_unref(stream);
}

#ifdef FEAT_DND
/*
 * Set up for receiving DND items.
 */
    શૂન્ય
gui_gtk_set_dnd_targets(શૂન્ય)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_add_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_add_gtype(builder, G_TYPE_STRING);
    જો (clip_html)
	gdk_content_formats_builder_add_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_to_formats(builder);
    gtk_drop_target_async_set_formats(gui.drop_target, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    સ્થિર શૂન્ય
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

    જો (text == ખાલી || *text == NUL || !query_pointer_pos(ખાલી, ખાલી, &state))
	પાછા;

    dnd_yank_drag_data((char_u *)text, (long)STRLEN(text));

    dropkey[2] = modifiers_gdk2vim(state);
    જો (dropkey[2] != 0)
	add_to_આગત_buf(dropkey, (પૂર્ણાંક)કદ(dropkey));
    નહીંતર
	add_to_આગત_buf(dropkey + 3, (પૂર્ણાંક)(કદ(dropkey) - 3));
}

    સ્થિર શૂન્ય
drop_read_કિંમત_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    અચલ GValue    *કિંમત = gdk_drop_read_કિંમત_finish(drop, result, ખાલી);
    gboolean	    success = FALSE;

    જો (કિંમત == ખાલી)
	જાઓ exit;

    જો (G_કિંમત_HOLDS(કિંમત, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_કિંમત_get_boxed(કિંમત);
	પૂર્ણાંક	nfiles = g_slist_length(files);
	char_u	**fnames;
	પૂર્ણાંક	i;

	જો (nfiles <= 0)
	    જાઓ exit;

	fnames = ALLOC_MULT(char_u *, nfiles);
	જો (fnames == ખાલી)
	    જાઓ exit;

	i = 0;
	માટે (GSList *l = files; l != ખાલી; l = l->next)
	{
	    GFile *file = l->data;
	    અક્ષર *path = g_file_get_path(file);
	    જો (path != ખાલી)
		fnames[i++] = vim_strsave((char_u *)path);
	    g_free(path);
	}
	nfiles = i;

	જો (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	નહીંતર
	    vim_free(fnames);
	success = TRUE;
    }
    નહીંતર જો (G_કિંમત_HOLDS(કિંમત, G_TYPE_STRING))
    {
	અચલ અક્ષર *text = g_કિંમત_get_string(કિંમત);

	જો (text == ખાલી || *text == NUL)
	    જાઓ exit;

	drop_read_text(drop, (char_u *)text);
	success = TRUE;
    }

exit:
    gdk_drop_finish(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_data_free(&drd->rd);
}

    સ્થિર શૂન્ય
drop_read_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    GInputStream    *in_stream;
    અચલ અક્ષર	    *m;

    in_stream = gdk_drop_read_finish(drop, result, &m, ખાલી);
    જો (in_stream == ખાલી || STRCMP(m, "text/html") != 0)
    {
	read_data_free(&drd->rd);
	gdk_drop_finish(drop, 0);
	પાછા;
    }

    પ્રતિજ્ઞા(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_આગત_stream_read_async(in_stream, drd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_ઉપલું,
	    drd->rd.cancel, (GAsyncReadyCallback)read_data_આગત_cb, drd);
}

// GCancellable of current DND operation, else NULL.
સ્થિર GCancellable *dnd_cancel = ખાલી;
સ્થિર guint dnd_timeout_id = 0;

    સ્થિર gboolean
drop_timeout_cb(શૂન્ય *udata UNUSED)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    પાછા G_SOURCE_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    સ્થિર gboolean
drop_cb(
	GtkDropTargetAsync  *target UNUSED,
	GdkDrop		    *drop,
	દ્વિગુણ		    x,
	દ્વિગુણ		    y,
	શૂન્ય		    *udata UNUSED)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    DropReadData	*drd = ખાલી;

    જો (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, કદ(*drd));
	gdk_drop_read_કિંમત_async(drop, GDK_TYPE_FILE_LIST, G_PRIORITY_ઉપલું,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_કિંમત_cb, drd);
    }
    નહીંતર જો (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	સ્થિર અચલ અક્ષર *m[] = {"text/html", ખાલી};

	drd = (DropReadData *)read_data_new(READ_DATA_DROP_DATA, કદ(*drd));
	gdk_drop_read_async(drop, m, G_PRIORITY_ઉપલું,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    નહીંતર જો (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, કદ(*drd));
	gdk_drop_read_કિંમત_async(drop, G_TYPE_STRING, G_PRIORITY_ઉપલું,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_કિંમત_cb, drd);
    }

    જો (dnd_cancel != ખાલી)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    જો (drd != ખાલી)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (પૂર્ણાંક)x;
	drd->y = (પૂર્ણાંક)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_add(3000, drop_timeout_cb, ખાલી);

	પાછા TRUE;
    }
    પાછા FALSE;
}
#endif

    સ્થિર શૂન્ય
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    gui.mainwin = ખાલી;
    gui.drawarea = ખાલી;
    જો (!exiting)
	gui_shell_closed();
}

    સ્થિર gboolean
delete_event_cb(GtkWindow *window UNUSED, gpointer data UNUSED)
{
    gui_shell_closed();
    પાછા TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    સ્થિર timeout_cb_type
આગત_timer_cb(gpointer data)
{
    પૂર્ણાંક *timed_out = (પૂર્ણાંક *)data;

    *timed_out = TRUE;
    પાછા FALSE;	// don't call me again
}

    શૂન્ય
gui_mch_update(શૂન્ય)
{
    પૂર્ણાંક cnt = 0;

    જ્યારે (g_main_context_pending(ખાલી) && !vim_is_આગત_buf_full()
								&& ++cnt < 100)
	g_main_context_iteration(ખાલી, TRUE);
}

#ifdef FEAT_JOB_CHANNEL
    સ્થિર timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    પાછા TRUE; // Keep repeating
}
#endif

    પૂર્ણાંક
gui_mch_wait_for_chars(long wtime)
{
    પૂર્ણાંક		focus;
    guint	timer;
    સ્થિર પૂર્ણાંક	timed_out;
    પૂર્ણાંક		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    જો (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   આગત_timer_cb, &timed_out);
    નહીંતર
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    જો (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, ખાલી);
#endif

    focus = gui.in_focus;

    કરો
    {
	// Stop or start blinking when focus changes
	જો (gui.in_focus != focus)
	{
	    જો (gui.in_focus)
		gui_mch_start_blink();
	    નહીંતર
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	જો (did_add_timer)
	    જાઓ theend;
# endif
#endif

	જો (gui.mainwin == ખાલી)
	    જાઓ theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	જો (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    જાઓ theend;
	}

	જો (!આગત_available())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(ખાલી, TRUE);
	    --gtk4_main_loop_level;
	}

	જો (આગત_available())
	{
	    retval = OK;
	    જાઓ theend;
	}
    } જ્યારે (wtime < 0 || !timed_out);

    gui_mch_update();

theend:
    જો (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    જો (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    પાછા retval;
}

    શૂન્ય
gui_mch_flush(શૂન્ય)
{
    // Ensure the offscreen surface content gets painted to the widget.
    જો (gui.drawarea != ખાલી)
	gtk_widget_queue_draw(gui.drawarea);
    જો (gui.mainwin != ખાલી && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    શૂન્ય
gui_mch_beep(શૂન્ય)
{
    GdkDisplay *display;

    જો (gui.mainwin != ખાલી && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	જો (display != ખાલી)
	    gdk_display_beep(display);
    }
}

    શૂન્ય *
gui_mch_get_display(શૂન્ય)
{
    જો (gui.mainwin != ખાલી && gtk_widget_get_display(gui.mainwin))
	પાછા gtk_widget_get_display(gui.mainwin);
    પાછા ખાલી;
}

    શૂન્ય
gui_mch_iconify(શૂન્ય)
{
    gtk_window_minimize(GTK_WINDOW(gui.mainwin));
}

    શૂન્ય
gui_mch_set_foreground(શૂન્ય)
{
    gtk_window_present(GTK_WINDOW(gui.mainwin));
}

    સ્થિર પૂર્ણાંક
query_pointer_pos(પૂર્ણાંક *x, પૂર્ણાંક *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *pointer;
    દ્વિગુણ	     sx, sy, nx, ny;
    graphene_point_t src, dst;

    જો (gui.drawarea == ખાલી)
	પાછા FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    જો (native == ખાલી)
	પાછા FALSE;
    surface = gtk_native_get_surface(native);
    જો (surface == ખાલી)
	પાછા FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    જો (display == ખાલી)
	પાછા FALSE;
    seat = gdk_display_get_default_seat(display);
    જો (seat == ખાલી)
	પાછા FALSE;
    pointer = gdk_seat_get_pointer(seat);
    જો (pointer == ખાલી)
	પાછા FALSE;

    જો (!gdk_surface_get_device_position(surface, pointer, &sx, &sy, state))
	પાછા FALSE;

    જો (x != ખાલી && y != ખાલી)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (દશાંશ)(sx - nx);
	src.y = (દશાંશ)(sy - ny);
	જો (!gtk_widget_compute_point(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    પાછા FALSE;

	*x = (પૂર્ણાંક)dst.x;
	*y = (પૂર્ણાંક)dst.y;
    }
    પાછા TRUE;
}

    શૂન્ય
gui_mch_getmouse(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    જો (!query_pointer_pos(x, y, ખાલી))
    {
	*x = 0;
	*y = 0;
    }
}

    શૂન્ય
gui_mch_setmouse(પૂર્ણાંક x UNUSED, પૂર્ણાંક y UNUSED)
{
    // GTK4/Wayland: cannot warp pointer
}

    શૂન્ય
gui_mch_mousehide(પૂર્ણાંક hide)
{
    જો (gui.pointer_hidden == hide)
	પાછા;

    gui.pointer_hidden = hide;
    જો (gui.blank_pointer != ખાલી)
    {
	જો (hide)
	    gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
	નહીંતર
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#નહીંતર
	    gtk_widget_set_cursor(gui.drawarea, ખાલી);
#endif
    }
}

    પૂર્ણાંક
gui_mch_haskey(char_u *name)
{
    પૂર્ણાંક i;

    માટે (i = 0; special_ચાવીઓ[i].ચાવી_sym != 0; i++)
	જો (name[0] == special_ચાવીઓ[i].code0
		&& name[1] == special_ચાવીઓ[i].code1)
	    પાછા OK;
    પાછા FAIL;
}

    શૂન્ય
gui_mch_forked(શૂન્ય)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    સ્થિર શૂન્ય
gui_gtk4_update_scrollbar_size(શૂન્ય)
{
    win_T	*wp;
    પૂર્ણાંક		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_ડાબું].id;

	જો (sbar == ખાલી || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_ડાબું]
		    && wp->w_scrollbars[SBAR_જમણું].id != ખાલી))
	    sbar = wp->w_scrollbars[SBAR_જમણું].id;

	જો (sbar != ખાલી && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    પૂર્ણાંક		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_size(sbar, &min, &nat);
	    sw = MAX(min.width, nat.width);
	    જો (sw > 0)
	    {
		w = sw;
		તોડો;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    જો (sbar != ખાલી && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	પૂર્ણાંક		    sh;

	gtk_widget_get_preferred_size(sbar, &min, &nat);
	sh = MAX(min.height, nat.height);

	જો (sh > 0)
	    h = sh;
    }

    જો (w != -1)
	gui.scrollbar_width = w;
    જો (h != -1)
	gui.scrollbar_height = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    શૂન્ય
gui_mch_enable_scrollbar(scrollbar_T *sb, પૂર્ણાંક flag)
{
    જો (sb->id != ખાલી)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_set_visible(sb->id, flag);

	જો (!was_visible && flag)
	    gui_gtk4_update_scrollbar_size();
    }
}

#જો defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    શૂન્ય
gui_mch_menu_grey(vimmenu_T *menu, પૂર્ણાંક grey)
{
    જો (menu->id == ખાલી)
	પાછા;
    gtk_widget_set_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    શૂન્ય
gui_mch_menu_hidden(vimmenu_T *menu, પૂર્ણાંક hidden)
{
    જો (menu->id == ખાલી)
	પાછા;
    gtk_widget_set_visible(menu->id, !hidden);
    gui_mch_update();
}

    શૂન્ય
gui_mch_draw_menubar(શૂન્ય)
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
    શૂન્ય
gui_mch_show_tabline(પૂર્ણાંક showit)
{
    જો (gui.tabline == ખાલી)
	પાછા;
    gtk_widget_set_visible(gui.tabline, showit);
    gui_mch_update();
}

    પૂર્ણાંક
gui_mch_showing_tabline(શૂન્ય)
{
    પાછા gui.tabline != ખાલી && gtk_widget_get_visible(gui.tabline);
}

સ્થિર પૂર્ણાંક ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
સ્થિર પૂર્ણાંક tabpage_hover = 0;

    સ્થિર શૂન્ય
tabpage_enter_cb(
	GtkEventController  *controller UNUSED,
	દ્વિગુણ		    x UNUSED,
	દ્વિગુણ		    y UNUSED,
	શૂન્ય		    *udata)
{
    tabpage_hover = GPOINTER_TO_INT(udata);
}

    સ્થિર શૂન્ય
tabline_enter_cb(
	GtkEventController  *controller UNUSED,
	દ્વિગુણ		    x UNUSED,
	દ્વિગુણ		    y UNUSED,
	શૂન્ય		    *udata UNUSED)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    સ્થિર શૂન્ય
tabline_add_css(શૂન્ય)
{
    GtkCssProvider  *provider;
    સ્થિર અચલ અક્ષર *css =
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
# જો GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
# નહીંતર
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
    સ્થિર શૂન્ય
tab_close_clicked_cb(GtkButton *button UNUSED, gpointer data)
{
    પૂર્ણાંક idx = gtk_notebook_page_અંક(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(data));

    જો (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    શૂન્ય
gui_mch_update_tabline(શૂન્ય)
{
    GtkWidget	*page;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    પૂર્ણાંક		nr = 0;
    પૂર્ણાંક		tab_અંક;
    પૂર્ણાંક		curtabidx = 0;
    char_u	*labeltext;

    જો (gui.tabline == ખાલી)
	પાછા;

    ignore_tabline_evt = TRUE;

    માટે (tp = first_tabpage; tp != ખાલી; tp = tp->tp_next, ++nr)
    {
	GtkNotebookPage	    *page_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	જો (tp == curtab)
	    curtabidx = nr;

	tab_અંક = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	જો (page == ખાલી)
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

	    gtk_notebook_ઉમેરો_page(GTK_NOTEBOOK(gui.tabline),
		    page, event_box, nr);
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page, TRUE);
	    g_object_set(gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page),
		    "tab-expand", TRUE, ખાલી);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	page_widget = gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_TO_POINTER(tab_અંક));
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(page_widget, "tab", &tab_widget, ખાલી);
	gtk_widget_add_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	label = g_object_get_data(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	જો (label != ખાલી && GTK_IS_LABEL(label))
	    gtk_label_set_text(GTK_LABEL(label), (અચલ અક્ષર *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_widget_set_tooltip_text(event_box, (અચલ gchar *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    જ્યારે (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != ખાલી)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    જો (nr == 1)
	gtk_widget_add_css_class(gui.tabline, "single-tab");
    નહીંતર
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    જો (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    શૂન્ય
gui_mch_set_curtab(પૂર્ણાંક nr)
{
    જો (gui.tabline != ખાલી)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    સ્થિર શૂન્ય
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    જો (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    સ્થિર શૂન્ય
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    જો (ignore_tabline_evt)
	પાછા;

    જો ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    નહીંતર
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    સ્થિર શૂન્ય
tabline_menu_event_cb(VimMenuItem *item UNUSED, VimMenuItemEvent event, શૂન્ય *udata)
{
    જો (event == VIM_MENU_ITEM_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_TO_INT(udata));
}

    સ્થિર શૂન્ય
add_tabline_menu_item(VimMenu *menu, અચલ અક્ષર *name, પૂર્ણાંક resp)
{
    VimMenuItem *item = VIM_MENU_ITEM(vim_menu_item_new(name,
		tabline_menu_event_cb, GINT_TO_POINTER(resp)));

    vim_menu_ઉમેરો_item(menu, item, -1);
}

/*
 * Create a menu for the tab line.
 */
    સ્થિર VimMenu *
create_tabline_popup_menu(શૂન્ય)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    add_tabline_menu_item(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, _("New Tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    પાછા menu;
}

    સ્થિર શૂન્ય
tabline_menu_press_event(
	GtkGestureClick *gesture,
	પૂર્ણાંક		n_press UNUSED,
	દ્વિગુણ		x,
	દ્વિગુણ		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    જો (but == GDK_BUTTON_SECONDARY)
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
    નહીંતર જો (but == GDK_BUTTON_PRIMARY)
    {
	જો (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    નહીંતર જો (but == GDK_BUTTON_મધ્ય)
    {
	જો (tabpage_hover != 0)
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

#જો defined(FEAT_SIGN_ICONS)
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)

    શૂન્ય
gui_mch_drawsign(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    જો (sign == ખાલી)
	પાછા;

    vim_draw_area_add_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    row, col, SIGN_WIDTH, SIGN_HEIGHT);

    gtk_widget_queue_draw(gui.drawarea);
}

    શૂન્ય *
gui_mch_register_sign(char_u *signfile)
{
    જો (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GError	    *error = ખાલી;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((અચલ અક્ષર *)signfile,
							    &error);
	જો (sign != ખાલી)
	    પાછા sign;

	semsg("E255: %s", error->message);
	g_error_free(error);
    }
    પાછા ખાલી;
}

    શૂન્ય
gui_mch_destroy_sign(શૂન્ય *sign)
{
    જો (sign != ખાલી)
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
    પૂર્ણાંક
gui_gtk_draw_string(પૂર્ણાંક row, પૂર્ણાંક col, char_u *s, પૂર્ણાંક len, પૂર્ણાંક flags)
{
    પૂર્ણાંક cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    row, col, s, len, flags);

    જો (gui.drawarea != ખાલી)
	gtk_widget_queue_draw(gui.drawarea);

    પાછા cells;
}

    પૂર્ણાંક
gui_get_x11_windis(Window *win UNUSED, Display **dis UNUSED)
{
    // GTK4: not applicable
    પાછા FAIL;
}

#જો defined(FEAT_MENU)
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
    સ્થિર char_u *
translate_mnemonic_tag(char_u *name, પૂર્ણાંક use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    પૂર્ણાંક	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    જો (name == ખાલી)
	પાછા ખાલી;

    માટે (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	જો (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    જો (buf != ખાલી)
    {
	pdest = buf;
	માટે (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    જો (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    નહીંતર જો (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    નહીંતર જો (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    નહીંતર જો (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    પાછા buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    શૂન્ય
gui_gtk_set_mnemonics(પૂર્ણાંક enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	જો (menu->id == ખાલી)
	    ચાલુ;

	name = translate_mnemonic_tag(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	જો (VIM_IS_MENU_BAR_ITEM(menu->id))
	    vim_menu_bar_item_set_text(VIM_MENU_BAR_ITEM(menu->id),
		    (અચલ અક્ષર *)name);
	vim_free(name);
    }
}

    સ્થિર શૂન્ય
popupmenu_closed_cb(GtkWidget *popover, શૂન્ય *udata UNUSED)
{
    gtk_widget_unparent(popover);
    જો (gui.drawarea != ખાલી)
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    સ્થિર શૂન્ય
gui_gtk_popup_at(vimmenu_T *menu, પૂર્ણાંક x, પૂર્ણાંક y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    જો (menu == ખાલી || menu->submenu_id == ખાલી)
	પાછા;

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
	    G_CALLBACK(popupmenu_closed_cb), ખાલી);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    શૂન્ય
gui_make_popup(char_u *path_name, પૂર્ણાંક mouse_pos)
{
    vimmenu_T	*menu;
    પૂર્ણાંક		x, y;

    menu = gui_find_menu(path_name);
    જો (menu == ખાલી || menu->submenu_id == ખાલી)
	પાછા;

    જો (mouse_pos)
	gui_mch_getmouse(&x, &y);
    નહીંતર
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#endif // FEAT_MENU

    પૂર્ણાંક
get_menu_tool_width(શૂન્ય)
{
    પાછા 0;
}

    પૂર્ણાંક
get_menu_tool_height(શૂન્ય)
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

    પૂર્ણાંક height = 0;

    માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	પૂર્ણાંક		h;

	જો (widgets[i] == ખાલી || !gtk_widget_get_visible(widgets[i]))
	    ચાલુ;

	h = gtk_widget_get_height(widgets[i]);

	જો (h == 0)
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
	નહીંતર
	    height += h;
    }
    પાછા height;
}

/*
 * Update selection targets for regular and primary selections.
 */
    શૂન્ય
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    જો (cbd == &clip_plus)
	gdk_content_provider_content_changed(gui.regular_provider);
    નહીંતર
	gdk_content_provider_content_changed(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    સ્થિર GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    જો (gui.mainwin == ખાલી)
	પાછા ખાલી;

    display = gtk_widget_get_display(gui.mainwin);
    જો (display == ખાલી)
	પાછા ખાલી;

    જો (cbd == &clip_plus)
    {
	જો (provider != ખાલી)
	    *provider = gui.regular_provider;
	પાછા gdk_display_get_clipboard(display);
    }
    નહીંતર
    {
	જો (provider != ખાલી)
	    *provider = gui.primary_provider;
	પાછા gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    સ્થિર શૂન્ય
clip_read_cb(GdkClipboard *cb, GAsyncResult *result, ClipReadData *crd)
{
    GInputStream    *in_stream;
    અચલ અક્ષર	    *mime_type;

    in_stream = gdk_clipboard_read_finish(cb, result, &mime_type, ખાલી);
    જો (in_stream == ખાલી)
    {
	read_data_free(&crd->rd);
	પાછા;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_આગત_stream_read_async(in_stream, crd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_ઉપલું,
	    crd->rd.cancel, (GAsyncReadyCallback)read_data_આગત_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    શૂન્ય
clip_mch_request_selection(Clipboard_T *cbd)
{
    સ્થિર અચલ અક્ષર *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	ખાલી
    };

    GdkClipboard *clipboard;
    ClipReadData *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, ખાલી);
    જો (clipboard == ખાલી)
	પાછા;

    crd = (ClipReadData *)read_data_new(READ_DATA_CLIP, કદ(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_PRIORITY_ઉપલું, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t start;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	start = time(ખાલી);
	જ્યારે (!g_cancellable_is_cancelled(cancel)
		&& time(ખાલી) < start + 3)
	    g_main_context_iteration(ખાલી, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

સ્થિર પૂર્ણાંક in_clipboard_set = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    શૂન્ય
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

    સ્થિર શૂન્ય
clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data)
{
    Clipboard_T *cbd = (Clipboard_T *)user_data;

    જો (in_clipboard_set)
	પાછા;
    જો (gdk_clipboard_is_local(clipboard))
	પાછા;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    પૂર્ણાંક
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    પૂર્ણાંક			ret;

    જો (cb == ખાલી)
	પાછા FAIL;

    in_clipboard_set = TRUE;
    ret = gdk_clipboard_set_content(cb, cp);
    in_clipboard_set = FALSE;

    પાછા ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    શૂન્ય
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, ખાલી);
    જો (clipboard == ખાલી)
	પાછા;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    જો (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_set_content(clipboard, ખાલી);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(શૂન્ય)
{
    પાછા gtk4_main_loop_level;
}

    શૂન્ય
gtk_main_quit(શૂન્ય)
{
    gtk4_main_loop_quit = TRUE;
}

#જો defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
સ્થિર અચલ અક્ષર *mshape_css_names[] =
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

    શૂન્ય
mch_set_mouse_shape(પૂર્ણાંક shape)
{
    GdkCursor	*c;
    અચલ અક્ષર	*css_name = "default";

    જો (gui.drawarea == ખાલી)
	પાછા;

    જો (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
    નહીંતર
    {
	જો (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	નહીંતર જો (shape < (પૂર્ણાંક)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
	નહીંતર
	    પાછા;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, ખાલી);
	gtk_widget_set_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    જો (shape != MSHAPE_HIDE)
	last_shape = shape;
}

#endif // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



સ્થિર પૂર્ણાંક last_text_area_w = 0;
સ્થિર પૂર્ણાંક last_text_area_h = 0;

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
સ્થિર અચલ અક્ષર * અચલ toolbar_icon_names[] =
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

    સ્થિર શૂન્ય
toolbar_button_clicked_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    સ્થિર GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = ખાલી;

    // Try specified icon file first
    જો (menu->iconfile != ખાલી)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	જો (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (અચલ અક્ષર *)buf, ખાલી);

	    જો (texture != ખાલી)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_set_size_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    જો (image == ખાલી)
    {
	અચલ અક્ષર *icon_name = "image-missing";
	પૂર્ણાંક n = (પૂર્ણાંક)ARRAY_LENGTH(toolbar_icon_names);

	જો (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_names[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    પાછા image;
}
# endif

    સ્થિર શૂન્ય
menu_button_clicked_cb(
	VimMenuItem	    *item UNUSED,
	VimMenuItemEvent    event,
	vimmenu_T	    *menu)
{
    જો (event == VIM_MENU_ITEM_CLICKED)
	gui_menu_cb(menu);
    નહીંતર જો (event == VIM_MENU_ITEM_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	સ્થિર gboolean did_સંદેશ = FALSE;

	જો (State & MODE_CMDLINE)
	    પાછા;

	tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	જો (tooltip != ખાલી && utf_valid_string(tooltip, ખાલી))
	{
	    સંદેશ((અક્ષર *)tooltip);
	    did_સંદેશ = TRUE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	નહીંતર જો (did_સંદેશ)
	{
	    સંદેશ("");
	    did_સંદેશ = FALSE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	CONVERT_TO_UTF8_FREE(tooltip);
    }
}

    શૂન્ય
gui_mch_add_menu(vimmenu_T *menu, પૂર્ણાંક idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;
    gboolean	use_mnemonic;
    char_u	*text;

    જો (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_set_parent(menu->submenu_id, gui.drawarea);
	પાછા;
    }

    parent = menu->parent;

    જો ((parent != ખાલી && parent->submenu_id == ખાલી)
	    || !menu_is_menubar(menu->name))
	પાછા;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = parent != ખાલી || p_wak[0] != 'n';
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    જો (parent != ખાલી)
    {
	parent_widget = parent->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_item_new(
		    (અચલ અક્ષર *)text, ખાલી, ખાલી));

	vim_menu_item_set_submenu(VIM_MENU_ITEM(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_ઉમેરો_item(VIM_MENU(parent_widget),
		VIM_MENU_ITEM(menu->id), idx);
    }
    નહીંતર
    {
	parent_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_item_new(
		    (અચલ અક્ષર *)text, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_ઉમેરો_item(VIM_MENU_BAR(parent_widget),
		VIM_MENU_BAR_ITEM(menu->id), idx);
    }

    vim_free(text);
}

    શૂન્ય
gui_mch_add_menu_item(vimmenu_T *menu, પૂર્ણાંક idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    જો (parent != ખાલી && menu_is_toolbar(parent->name))
    {
	જો (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_ઉમેરો_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	નહીંતર
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*text;
	    char_u	*tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    જો (tooltip != ખાલી && !utf_valid_string(tooltip, ખાલી))
		CONVERT_TO_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_ઉમેરો_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (અચલ અક્ષર *)text, idx);
	    gtk_widget_set_tooltip_text(btn, (અચલ gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
	પાછા;
    }
# endif

    // Menu items (non-toolbar)
    જો (parent == ખાલી || parent->submenu_id == ખાલી)
	પાછા;

    જો (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_ઉમેરો_separator(
		VIM_MENU(parent->submenu_id), idx));
    }
    નહીંતર
    {
	char_u	    *text;
	char_u	    *accel_text = ખાલી;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	text = translate_mnemonic_tag(menu->name, use_mnemonic);

	જો (menu->actext != ખાલી && menu->actext[0] != NUL)
	    accel_text = CONVERT_TO_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_item_new((અચલ અક્ષર *)text,
		    (VimMenuItemFunc)menu_button_clicked_cb, menu));

	જો (accel_text != ખાલી)
	    vim_menu_item_set_accel(VIM_MENU_ITEM(menu->id),
		    (અચલ અક્ષર *)accel_text);

	vim_menu_ઉમેરો_item(VIM_MENU(parent->submenu_id),
		VIM_MENU_ITEM(menu->id), idx);

	vim_free(text);
	CONVERT_TO_UTF8_FREE(accel_text);
    }
}

    શૂન્ય
gui_mch_toggle_tearoffs(પૂર્ણાંક enable UNUSED)
{
    // GTK4: tearoff menus don't exist.
}

    શૂન્ય
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    જો (menu->id == ખાલી)
	પાછા;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    જો (tooltip != ખાલી && utf_valid_string(tooltip, ખાલી))
	gtk_widget_set_tooltip_text(menu->id, (અચલ અક્ષર *)tooltip);
    CONVERT_TO_UTF8_FREE(tooltip);
}

    શૂન્ય
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    જો (menu->parent != ખાલી && menu_is_toolbar(menu->parent->name))
    {
	જો (menu->id != ખાલી)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = ખાલી;
	}
	પાછા;
    }

    // For popup menus, unparent the menu as well
    જો (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	જો (menu->submenu_id != ખાલી)
	    gtk_widget_unparent(menu->submenu_id);
    }
    નહીંતર જો (menu->parent == ખાલી && menu->id != ખાલી)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    નહીંતર જો (menu->id != ખાલી)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->parent->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    શૂન્ય
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    પૂર્ણાંક x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    સ્થિર શૂન્ય
show_menubar_popover(શૂન્ય)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    જો (gui.menubar == ખાલી || gui.drawarea == ખાલી)
	પાછા;

    જો (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), ખાલી);
	પાછા;
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
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), ખાલી);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#endif // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    શૂન્ય
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    GtkAdjustment *adj;

    જો (sb->id == ખાલી)
	પાછા;
    જો (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	પાછા;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_set_lower(adj, 0.0);
    gtk_adjustment_set_upper(adj, (gdouble)max + 1);
    gtk_adjustment_set_કિંમત(adj, (gdouble)val);
    gtk_adjustment_set_step_increment(adj, 1.0);
    gtk_adjustment_set_page_increment(adj, (gdouble)(size > 2 ? size - 2 : 1));
    gtk_adjustment_set_page_size(adj, (gdouble)size);
}

    શૂન્ય
gui_mch_set_scrollbar_pos(scrollbar_T *sb, પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h)
{
    જો (sb->id != ખાલી)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    પૂર્ણાંક
gui_mch_get_scrollbar_xpadding(શૂન્ય)
{
    પૂર્ણાંક formwin_w = gtk_widget_get_width(gui.formwin);
    પૂર્ણાંક sbar_w = 0;
    પૂર્ણાંક xpad;

    જો (gui.which_scrollbars[SBAR_ડાબું])
	sbar_w += gui.scrollbar_width;
    જો (gui.which_scrollbars[SBAR_જમણું])
	sbar_w += gui.scrollbar_width;

    xpad = formwin_w - last_text_area_w - sbar_w;
    પાછા (xpad < 0) ? 0 : xpad;
}

    પૂર્ણાંક
gui_mch_get_scrollbar_ypadding(શૂન્ય)
{
    પૂર્ણાંક formwin_h = gtk_widget_get_height(gui.formwin);
    પૂર્ણાંક ypad;

    ypad = formwin_h - last_text_area_h;
    જો (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_height;

    પાછા (ypad < 0) ? 0 : ypad;
}

    સ્થિર શૂન્ય
adjustment_કિંમત_changed(GtkAdjustment *adj, gpointer data UNUSED)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_data(G_OBJECT(adj), "vim-sb");
    long કિંમત = (long)gtk_adjustment_get_કિંમત(adj);

    જો (sb != ખાલી)
	gui_drag_scrollbar(sb, કિંમત, FALSE);
}

    શૂન્ય
gui_mch_create_scrollbar(scrollbar_T *sb, પૂર્ણાંક orient)
{
    GtkAdjustment *adj;
    જો (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ખાલી);
    નહીંતર
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ખાલી);

    gtk_widget_add_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_set_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_set_data(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_કિંમત_changed), ખાલી);
}

    શૂન્ય
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    જો (sb->id != ખાલી)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = ખાલી;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    શૂન્ય
gui_mch_set_text_area_pos(પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    જો (w <= 0 || h <= 0)
	પાછા;
    last_text_area_w = w;
    last_text_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    શૂન્ય
gui_gtk4_calculate_bleed(પૂર્ણાંક width, પૂર્ણાંક height)
{
    પૂર્ણાંક old_જમણું = gui.bleed_જમણું;
    પૂર્ણાંક old_bot = gui.bleed_bot;

    gui.bleed_જમણું = width - last_text_area_w;
    gui.bleed_bot = height - last_text_area_h;

    જો (gui.which_scrollbars[SBAR_ડાબું])
	gui.bleed_જમણું -= gui.scrollbar_width;
    જો (gui.which_scrollbars[SBAR_જમણું])
	gui.bleed_જમણું -= gui.scrollbar_width;
    જો (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_height;

    // Not sure if this can happen, but be safe...
    જો (gui.bleed_જમણું < 0)
	gui.bleed_જમણું = 0;
    જો (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    જો (old_જમણું != gui.bleed_જમણું || old_bot != gui.bleed_bot)
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
પ્રકારવ્યાખ્યા સંરચના {
    GFile	*result;
    gboolean	done;
} FileDialogData;

    સ્થિર શૂન્ય
file_dialog_open_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_open_finish(
		    GTK_FILE_DIALOG(source), res, ખાલી);
    fdd->done = TRUE;
}

    સ્થિર શૂન્ય
file_dialog_save_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_save_finish(
		    GTK_FILE_DIALOG(source), res, ખાલી);
    fdd->done = TRUE;
}

    સ્થિર શૂન્ય
file_dialog_folder_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_select_folder_finish(
		    GTK_FILE_DIALOG(source), res, ખાલી);
    fdd->done = TRUE;
}

    char_u *
gui_mch_browse(પૂર્ણાંક saving,
	char_u *title,
	char_u *dflt,
	char_u *ext UNUSED,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    FileDialogData  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *result = ખાલી;
    GListStore	    *filters;

    title = CONVERT_TO_UTF8(title);

    જો (initdir == ખાલી || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    નહીંતર જો (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    add_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    જો (title != ખાલી)
	gtk_file_dialog_set_title(dlg, (અચલ અક્ષર *)title);

    {
	GFile *dir = g_file_new_for_path((અચલ અક્ષર *)dirbuf);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    જો (filter != ખાલી && *filter != NUL)
    {
	પૂર્ણાંક		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	જો (patt != ખાલી)
	{
	    જ્યારે (p != ખાલી && *p != NUL)
	    {
		જો (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    જો (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    નહીંતર
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			જો (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    જો (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		નહીંતર
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

    જો (saving && dflt != ખાલી && *dflt != NUL)
	gtk_file_dialog_set_initial_name(dlg, (અચલ અક્ષર *)dflt);

    fdd.result = ખાલી;
    fdd.done = FALSE;

    જો (saving)
	gtk_file_dialog_save(dlg, GTK_WINDOW(gui.mainwin), ખાલી,
		file_dialog_save_cb, &fdd);
    નહીંતર
	gtk_file_dialog_open(dlg, GTK_WINDOW(gui.mainwin), ખાલી,
		file_dialog_open_cb, &fdd);

    જ્યારે (!fdd.done)
	g_main_context_iteration(ખાલી, TRUE);

    જો (fdd.result != ખાલી)
    {
	અક્ષર *path = g_file_get_path(fdd.result);
	જો (path != ખાલી)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    પાછા result;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    FileDialogData	fdd;
    char_u		*result = ખાલી;

    title = CONVERT_TO_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    જો (title != ખાલી)
	gtk_file_dialog_set_title(dlg, (અચલ અક્ષર *)title);

    જો (initdir != ખાલી && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_path((અચલ અક્ષર *)initdir);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.result = ખાલી;
    fdd.done = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_WINDOW(gui.mainwin), ખાલી,
	    file_dialog_folder_cb, &fdd);

    જ્યારે (!fdd.done)
	g_main_context_iteration(ખાલી, TRUE);

    જો (fdd.result != ખાલી)
    {
	અક્ષર *path = g_file_get_path(fdd.result);
	જો (path != ખાલી)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    પાછા result;
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
    સ્થિર અક્ષર **
split_button_string(char_u *button_string, પૂર્ણાંક *n_buttons, અક્ષર **tofree)
{
    અક્ષર	    **array;
    char_u	    *p;
    unsigned પૂર્ણાંક    ગણતરી = 1;

    button_string = (char_u *)g_strdup((અચલ અક્ષર *)button_string);

    માટે (p = button_string; *p != NUL; ++p)
	જો (*p == DLG_BUTTON_SEP)
	    ++ગણતરી;

    array = g_malloc_n(ગણતરી, કદ(અક્ષર *));
    ગણતરી = 0;

    જો (array != ખાલી)
    {
	array[ગણતરી++] = (અક્ષર *)button_string;
	માટે (p = button_string; *p != NUL; )
	{
	    જો (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[ગણતરી++] = (અક્ષર *)p;
	    }
	    નહીંતર જો (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    નહીંતર
		MB_PTR_ADV(p);
	}
    }

    *tofree = (અક્ષર *)button_string;
    *n_buttons = ગણતરી;
    પાછા array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    સ્થિર અચલ અક્ષર *
dialog_type_to_icon(પૂર્ણાંક type)
{
    પસંદગી (type)
    {
	સ્થિતિ VIM_ERROR:
	    પાછા "dialog-error-symbolic";
	સ્થિતિ VIM_WARNING:
	    પાછા "dialog-warning-symbolic";
	સ્થિતિ VIM_INFO:
	    પાછા "dialog-information-symbolic";
	સ્થિતિ VIM_QUESTION:
	    પાછા "dialog-question-symbolic";
	મૂળભૂત:
	    તોડો;
    }
    પાછા ખાલી;
}

// Data associated with each button in the dialog
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક		but_idx;
    પૂર્ણાંક		*response;
    gboolean	*done;
} DialogButtonState;

પ્રકારવ્યાખ્યા સંરચના
{
    GtkWidget	*win;
    gboolean	*done;
    gboolean	no_alt;
} DialogState;

    સ્થિર શૂન્ય
dialog_button_clicked_cb(GtkButton *button UNUSED, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->done = TRUE;
}

    સ્થિર gboolean
dialog_ચાવી_pressed_cb(
	Gtk_Event_Controller_ચાવી	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	DialogState		*dstate)
{
    જો (keyval == GDK_ચાવી_Escape)
    {
	*dstate->done = TRUE;
	પાછા TRUE;
    }

    જો (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_mask()))
	પાછા gtk_widget_mnemonic_activate(dstate->win, FALSE);
    પાછા FALSE;
}

    સ્થિર gboolean
dialog_close_request_cb(GtkWindow *win UNUSED, gboolean *win_closed)
{
    *win_closed = TRUE;
    પાછા FALSE;
}

    પૂર્ણાંક
gui_mch_dialog(
	પૂર્ણાંક	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	પૂર્ણાંક	def_but,
	char_u	*textfield,
	પૂર્ણાંક	ex_cmd UNUSED)
{
    GtkWindow		*win = GTK_WINDOW(gtk_window_new());
    GtkWidget		*vertbox;
    GtkWidget		*message_box;
    અચલ અક્ષર		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*entry = ખાલી;
    char_u		*utf8_title;
    char_u		*utf8_message;
    GtkEventController *ચાવી_controller;
    DialogButtonState	*but_states = ખાલી;
    અક્ષર		*tofree = ખાલી;
    પૂર્ણાંક			response = -1;
    gboolean		done = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_TO_UTF8(title);
    જો (utf8_title != ખાલી)
	gtk_window_set_title(win, (અચલ અક્ષર *)utf8_title);
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
    gtk_widget_set_margin_ટોચ(vertbox, 24);
    gtk_widget_set_margin_bottom(vertbox, 24);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);

    // Add the message label
    message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(message_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), message_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_to_icon(type);
    જો (icon_name != ખાલી)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(message_box), icon);
    }

    utf8_message = CONVERT_TO_UTF8(message);
    label = gtk_label_new((અચલ અક્ષર *)utf8_message);
    CONVERT_TO_UTF8_FREE(utf8_message);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(message_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    ચાવી_controller = gtk_event_controller_ચાવી_new();
    g_signal_connect(ચાવી_controller, "key-pressed",
	    G_CALLBACK(dialog_ચાવી_pressed_cb), &state);
    gtk_widget_add_controller(GTK_WIDGET(win), ચાવી_controller);
    state.done = &done;
    state.win = GTK_WIDGET(win);

    જો (textfield != ખાલી)
    {
	// Add text entry so user can enter text
	char_u *utf8_text = CONVERT_TO_UTF8(textfield);

	entry = gtk_entry_new();

	જો (utf8_text != ખાલી)
	    gtk_editable_set_text(GTK_EDITABLE(entry), (અચલ અક્ષર *)utf8_text);
	નહીંતર
	    gtk_editable_set_text(GTK_EDITABLE(entry), "");
	CONVERT_TO_UTF8_FREE(utf8_text);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_box_append(GTK_BOX(vertbox), entry);
	state.no_alt = FALSE;
    }
    નહીંતર
    {
	GListModel  *controllers;
	પૂર્ણાંક	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_વસ્તુઓ(controllers);
	માટે (પૂર્ણાંક i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_item(controllers, i);
	    જો (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_set_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_window_set_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    જો (buttons != ખાલી)
    {
	GtkWidget   *but_box;
	અક્ષર	    **buttons_arr; // Note that array is allocated, not strings
	પૂર્ણાંક	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	જો (vim_strchr(p_go, GO_VERTICAL) != ખાલી)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	નહીંતર
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_set_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = split_button_string(buttons, &n_buttons, &tofree);

	but_states = g_malloc_n(n_buttons, કદ(DialogButtonState));

	માટે (પૂર્ણાંક i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_states + i;

	    but_label = CONVERT_TO_UTF8((char_u *)buttons_arr[i]);
	    જો (but_label == ખાલી)
		ચાલુ;

	    but = gtk_button_new_with_mnemonic((અક્ષર *)but_label);
	    જો (i == def_but - 1)
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

    જ્યારે (!done && !win_closed)
	g_main_context_iteration(ખાલી, TRUE);

    જો (done)
    {
	જો (textfield != ખાલી)
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
    પાછા response == -1 ? 0 : response + 1;
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

પ્રકારવ્યાખ્યા સંરચના
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

સ્થિર SharedFindReplace find_widgets = {0};
સ્થિર SharedFindReplace repl_widgets = {0};

    સ્થિર શૂન્ય
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    પૂર્ણાંક			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = GPOINTER_TO_INT(data);

    જો (flags == FRD_FINDNEXT)
    {
	repl_text = ખાલી;
	sfr = &find_widgets;
    }
    નહીંતર
    {
	repl_text = (char_u *)gtk_editable_get_text(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(sfr->what));
    direction_down = gtk_check_button_get_active(
	    GTK_CHECK_BUTTON(sfr->down));

    જો (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    જો (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

    સ્થિર શૂન્ય
dialog_destroyed_cb(GtkWidget *widget UNUSED, gpointer data)
{
    *(GtkWidget **)data = ખાલી;
}

    સ્થિર શૂન્ય
entry_changed_cb(GtkWidget *entry, GtkWidget *dialog)
{
    અચલ gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    જો (!entry_text)
	પાછા; // Shouldn't happen

    nonempty = (entry_text[0] != '\0');

    જો (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    જો (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

    સ્થિર gboolean
find_ચાવી_pressed_cb(
	Gtk_Event_Controller_ચાવી	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	SharedFindReplace	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    જો (state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	પાછા FALSE;

    // the Escape key synthesizes a cancellation action
    જો (keyval == GDK_ચાવી_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_window_destroy(GTK_WINDOW(frdp->dialog));
	પાછા TRUE;
    }

    પાછા FALSE;
}

    સ્થિર શૂન્ય
entry_activate_cb(GtkWidget *widget UNUSED, શૂન્ય *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    સ્થિર શૂન્ય
find_replace_dialog_create(char_u *arg, પૂર્ણાંક do_replace)
{
    SharedFindReplace	*frdp;
    char_u		*entry_text;
    પૂર્ણાંક			wword = FALSE;
    પૂર્ણાંક			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*ચાવી_controller;

    frdp = do_replace ? &repl_widgets : &find_widgets;
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    જો (entry_text != ખાલી && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = entry_text;
	entry_text = string_convert(&output_conv, entry_text, ખાલી);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    જો (frdp->dialog != ખાલી)
    {
	જો (entry_text != ખાલી)
	{
	    gtk_editable_set_text(GTK_EDITABLE(frdp->what),
		    (અક્ષર *)entry_text);
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
	જો (do_replace && g_utf8_strlen(
		    gtk_editable_get_text(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	પાછા;
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
    gtk_widget_set_margin_ટોચ(vertbox, 12);
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
    sensitive = (entry_text != ખાલી && entry_text[0] != NUL);
    જો (entry_text != ખાલી)
	gtk_editable_set_text(GTK_EDITABLE(frdp->what), (અક્ષર *)entry_text);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    જો (do_replace)
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
    નહીંતર
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

    ચાવી_controller = gtk_event_controller_ચાવી_new();
    g_signal_connect(ચાવી_controller, "key-pressed",
	    G_CALLBACK(find_ચાવી_pressed_cb), frdp);
    gtk_widget_add_controller(GTK_WIDGET(frdp->dialog), ચાવી_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(entry_changed_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_window_set_default_widget(GTK_WINDOW(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
	    GINT_TO_POINTER(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    જો (do_replace)
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
    જો (do_replace && entry_text != ખાલી && entry_text[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(entry_text);
}

    શૂન્ય
gui_mch_find_dialog(exarg_T *eap)
{
    જો (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    શૂન્ય
gui_mch_replace_dialog(exarg_T *eap)
{
    જો (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    શૂન્ય
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
સ્થિર GtkPrintDialog *print_dialog = ખાલી;
સ્થિર GtkPrintSettings *print_settings = ખાલી;
સ્થિર GtkPageSetup *page_setup = ખાલી;
સ્થિર GtkPrintSetup *print_setup = ખાલી;
સ્થિર GFile *print_file;

    સ્થિર શૂન્ય
print_dialog_cb(GtkPrintDialog *dialog, GAsyncResult *result, gboolean *done)
{
    GError *error = ખાલી;
    print_setup = gtk_print_dialog_setup_finish(dialog, result, &error);

    જો (print_setup == ખાલી)
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
	દ્વિગુણ		*page_width,
	દ્વિગુણ		*page_height)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    done = FALSE;
    char_u	    *temp;

    gtk_print_dialog_set_title(dialog, (અચલ અક્ષર *)jobname);
    gtk_print_dialog_set_modal(dialog, TRUE);

    print_dialog = ખાલી;
    print_setup = ખાલી;

    જો (print_settings == ખાલી)
    {
	option_table_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	જો (opt->present)
	{
	    જો (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    નહીંતર જો (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    નહીંતર
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	જો (opt->present)
	{
	    જો (*opt->string == 'y')
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_PORTRAIT);
	    નહીંતર
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	જો (opt->present)
	{
	    gtk_print_settings_set_paper_size(print_settings,
		    gtk_paper_size_new((અચલ અક્ષર *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	જો (opt->present)
	{
	    જો (*opt->string == 'y')
		gtk_print_settings_set_collate(print_settings, TRUE);
	    નહીંતર
		gtk_print_settings_set_collate(print_settings, FALSE);
	}
    }

    જો (page_setup == ખાલી)
	page_setup = gtk_page_setup_new();

    gtk_print_dialog_set_print_settings(dialog, print_settings);
    gtk_print_dialog_set_page_setup(dialog, page_setup);

    gtk_print_dialog_setup(dialog, GTK_WINDOW(gui.mainwin), ખાલી,
	    (GAsyncReadyCallback)print_dialog_cb, &done);

    જ્યારે (!done)
	g_main_context_iteration(ખાલી, TRUE);

    જો (print_setup == ખાલી)
    {
	g_object_unref(dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_સંદેશ = FALSE;
	પાછા ખાલી;
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
    જો (temp == ખાલી)
    {
	emsg(_(e_cant_find_temp_file_for_writing));
	g_clear_object(&dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_સંદેશ = FALSE;
	પાછા ખાલી;
    }
    print_file = g_file_new_for_path((અક્ષર *)temp);

    print_dialog = dialog;

    પાછા temp;
}

    સ્થિર શૂન્ય
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsyncResult	*result,
	GFile		*file)
{
    અક્ષર *path = g_file_get_path(file);

    (શૂન્ય)gtk_print_dialog_print_file_finish(dialog, result, ખાલી);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(path);
    g_free(path);
}


    શૂન્ય
gui_gtk4_print_finish(શૂન્ય)
{
    જો (print_dialog != ખાલી)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_WINDOW(gui.mainwin),
		print_setup, print_file, ખાલી,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = ખાલી;
	g_clear_pointer(&print_setup, gtk_print_setup_unref);
    }
}
#endif // USE_GTK4_PRINT_DIALOG

#ifdef FEAT_IMAGE_GUI

પ્રકારવ્યાખ્યા સંરચના
{
    GdkTexture *texture;
} image_gdk_T;

પ્રકારવ્યાખ્યા સંરચના
{
    Gsk_Render_ગાંઠ   **nodes; // Note that elements may be NULL
    પૂર્ણાંક		    n_nodes;
} image_placement_gdk_T;

    પૂર્ણાંક
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    GdkMemoryFormat fmt;
    size_t	    stride;
    GBytes	    *bytes;
    size_t	    size;
    પૂર્ણાંક		    w, h;

    જો (ctx == ખાલી)
	પાછા FAIL;

    જો (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_MEMORY_R8G8B8A8;
    નહીંતર
	fmt = GDK_MEMORY_R8G8B8;

    image_get_dimensions(img, &w, &h);

    size = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    bytes = g_bytes_new(img->data, size);
    ctx->texture = gdk_memory_texture_new(w, h, fmt, bytes, stride);
    g_bytes_unref(bytes);

    img->backend_data = ctx;
    પાછા OK;
}

    શૂન્ય
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_data;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    સ્થિર શૂન્ય
clear_nodes(image_placement_gdk_T *ctx)
{
    જો (ctx->nodes == ખાલી || gui.drawarea == ખાલી)
	પાછા;

    માટે (પૂર્ણાંક i = 0; i < ctx->n_nodes; i++)
    {
	જો (ctx->nodes[i] == ખાલી)
	    ચાલુ;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_ગાંઠ_unref(ctx->nodes[i]);
    }
    g_clear_pointer(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    પૂર્ણાંક
image_placement_gui_init(image_placement_T *place)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    જો (ctx == ખાલી)
	પાછા FAIL;

    place->backend_data = ctx;
    પાછા OK;
}

    શૂન્ય
image_placement_gui_uninit(image_placement_T *place)
{
    image_placement_gdk_T *ctx = place->backend_data;

    clear_nodes(ctx);
    vim_free(ctx);
}

    શૂન્ય
image_placement_gui_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_gdk_T   *ctx = place->backend_data;
    image_gdk_T		    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    પૂર્ણાંક			    n_rects;
    પૂર્ણાંક			    iw, ih;
    Gsk_Render_ગાંઠ	    *texture_ગાંઠ;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    જો (rects == ખાલી)
	પાછા;

    clear_nodes(ctx);
    જો (n_rects == 0)
	પાછા;

    image_get_dimensions(img, &iw, &ih);
    texture_ગાંઠ = gsk_texture_ગાંઠ_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(place->col) - FILL_X(place->crop_box.x1),
		FILL_Y(place->row) - FILL_Y(place->crop_box.y1)
		+ FILL_Y(place->row_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    જો (texture_ગાંઠ == ખાલી)
	// Not sure if this can happen...
	પાછા;

    ctx->nodes = g_malloc_n(n_rects, કદ(Gsk_Render_ગાંઠ *));
    ctx->n_nodes = n_rects;

    માટે (પૂર્ણાંક i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	પૂર્ણાંક		row, col;
	પૂર્ણાંક		posx, posy;
	પૂર્ણાંક		x, y, w, h;
	Gsk_Render_ગાંઠ	*ગાંઠ;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	ગાંઠ = gsk_clip_ગાંઠ_new(texture_ગાંઠ,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	જો (ગાંઠ != ખાલી)
	    vim_draw_area_add_external(VIM_DRAW_AREA(gui.drawarea), ગાંઠ);

	ctx->nodes[i] = ગાંઠ;
    }
    gsk_render_ગાંઠ_unref(texture_ગાંઠ);
}

    શૂન્ય
image_placement_gui_clear(image_placement_T *place)
{
    image_placement_gdk_T   *ctx = place->backend_data;

    clear_nodes(ctx);
}

#endif

#endif // FEAT_GUI_GTK
