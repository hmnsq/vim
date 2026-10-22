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

#اگر_تعریف_شدہ FEAT_GUI_GTK

#شامل <gdk/gdkkeysyms.h>
#شامل <gdk/gdk.h>
#شامل <gtk/gtk.h>
#شامل "gui_gtk4_f.h"
#شامل "gui_gtk4_cb.h"
#شامل "gui_gtk4_da.h"
#اگر_تعریف_شدہ FEAT_TOOLBAR
# include "gui_gtk4_tb.h"
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
# include "gui_gtk4_menu.h"
#اگر_ختم

/*
 * Geometry string parser, replacing XParseGeometry to remove X11 dependency.
 * Format: [WIDTHxHEIGHT][{+-}XOFF{+-}YOFF]
 */
#تعریف_کرو No_قدر		0x0000
#تعریف_کرو چوڑائی_قدر	0x0004
#تعریف_کرو اونچائی_قدر	0x0008

    ساکن صحیح
vim_parse_geometry(ثابت حرف *str, غیرعلامتی صحیح *چوڑائی, غیرعلامتی صحیح *اونچائی)
{
    صحیح نقاب = No_قدر;
    حرف *انتہا;
    طویل val;

    اگر (str == تہی || *str == NUL)
	واپس نقاب;

    // Parse width
    اگر (*str != '+' && *str != '-')
    {
	val = strtol(str, &انتہا, 10);
	اگر (انتہا != str)
	{
	    *چوڑائی = (غیرعلامتی صحیح)val;
	    نقاب |= چوڑائی_قدر;
	    str = انتہا;
	}
    }

    // Parse 'x' or 'X' separator and height
    اگر (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &انتہا, 10);
	اگر (انتہا != str)
	{
	    *اونچائی = (غیرعلامتی صحیح)val;
	    نقاب |= اونچائی_قدر;
	    str = انتہا;
	}
    }

    واپس نقاب;
}

#اگر defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
ساکن صحیح آخری_shape = 0;
#اگر_ختم

#تعریف_کرو DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
ساکن شمار {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
ساکن صحیح gtk4_main_loop_level = 0;
ساکن صحیح gtk4_main_loop_quit = FALSE;

#اگر_تعریف_شدہ USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#اگر_ختم

قسمتعریف gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
ساکن ساخت special_کلید
{
    guint کلید_sym;
    char_u code0;
    char_u code1;
}
ثابت special_کلیدیں[] =
{
    {GDK_کلید_اوپر,	'k', 'u'},
    {GDK_کلید_نیچے_کو,	'k', 'd'},
    {GDK_کلید_بایاں,	'k', 'l'},
    {GDK_کلید_دایاں,	'k', 'r'},
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
    {GDK_کلید_عقب_Space,	'k', 'b'},
    {GDK_کلید_داخل_کرو,	'k', 'I'},
    {GDK_کلید_Delete,	'k', 'D'},
    {GDK_کلید_Home,	'k', 'h'},
    {GDK_کلید_انتہا,	'@', '7'},
    {GDK_کلید_پیشگی,	'k', 'P'},
    {GDK_کلید_اگلا,	'k', 'N'},
    {GDK_کلید_Print,	'%', '9'},
    {GDK_کلید_KP_بایاں,	'k', 'l'},
    {GDK_کلید_KP_دایاں,	'k', 'r'},
    {GDK_کلید_KP_اوپر,	'k', 'u'},
    {GDK_کلید_KP_نیچے_کو,	'k', 'd'},
    {GDK_کلید_KP_داخل_کرو,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_کلید_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_کلید_KP_Home,	'K', '1'},
    {GDK_کلید_KP_انتہا,	'K', '4'},
    {GDK_کلید_KP_پیشگی,	'K', '3'},
    {GDK_کلید_KP_اگلا,	'K', '5'},
    {GDK_کلید_KP_جوڑو,	'K', '6'},
    {GDK_کلید_KP_تفریق, 'K', '7'},
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

    ساکن صحیح
keyval_مقصد_string(غیرعلامتی صحیح keyval, char_u *string)
{
    صحیح		len;
    guint32	uc;

    uc = gdk_keyval_مقصد_unicode(keyval);
    اگر (uc != 0)
    {
	len = utf_char2bytes((صحیح)uc, string);
    }
    ورنہ
    {
	len = 1;
	انتخاب (keyval)
	{
	    حالت GDK_کلید_Tab: حالت GDK_کلید_KP_Tab: حالت GDK_کلید_ISO_بایاں_Tab:
		string[0] = TAB;
		توڑو;
	    حالت GDK_کلید_Linefeed:
		string[0] = NL;
		توڑو;
	    حالت GDK_کلید_Return: حالت GDK_کلید_ISO_Enter: حالت GDK_کلید_3270_Enter:
		string[0] = CAR;
		توڑو;
	    حالت GDK_کلید_Escape:
		string[0] = ESC;
		توڑو;
	    طےشدہ:
		len = 0;
		توڑو;
	}
    }
    string[len] = NUL;
    واپس len;
}

    ساکن صحیح
modifiers_gdk2vim(guint state)
{
    صحیح modifiers = 0;

    اگر (state & GDK_سرکاؤ_نقاب)
	modifiers |= MOD_نقاب_سرکاؤ;
    اگر (state & GDK_CONTROL_نقاب)
	modifiers |= MOD_نقاب_CTRL;
    اگر (state & GDK_ALT_نقاب)
	modifiers |= MOD_نقاب_ALT;
    اگر (state & GDK_META_نقاب)
	modifiers |= MOD_نقاب_META;
    اگر (state & GDK_SUPER_نقاب)
	modifiers |= MOD_نقاب_CMD;

    واپس modifiers;
}

ساکن GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
ساکن gboolean کلید_press_event(Gtk_Event_Controller_کلید *controller, guint keyval, guint keycode, GdkModifierType state, gpointer ڈیٹا);
ساکن خالی کلید_release_event(Gtk_Event_Controller_کلید *controller, guint keyval, guint keycode, GdkModifierType state, gpointer ڈیٹا);
ساکن خالی button_press_event(GtkGestureClick *gesture, صحیح n_press, دوہرا x, دوہرا y, gpointer ڈیٹا);
ساکن خالی button_release_event(GtkGestureClick *gesture, صحیح n_press, دوہرا x, دوہرا y, gpointer ڈیٹا);
ساکن خالی motion_notify_event(GtkEventControllerMotion *controller, دوہرا x, دوہرا y, gpointer ڈیٹا);
ساکن خالی enter_notify_event(GtkEventControllerMotion *controller, دوہرا x, دوہرا y, gpointer ڈیٹا);
ساکن gboolean scroll_event(GtkEventControllerScroll *controller, دوہرا dx, دوہرا dy, gpointer ڈیٹا);
ساکن خالی focus_اندر_event(GtkEventControllerFocus *controller, gpointer ڈیٹا);
ساکن خالی focus_برآمد_event(GtkEventControllerFocus *controller, gpointer ڈیٹا);
#اگر_تعریف_شدہ FEAT_DND
ساکن gboolean drop_cb(Gtk_Drop_ہدف_Async *ہدف, GdkDrop *drop, دوہرا x, دوہرا y, خالی *ڈیٹا);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
ساکن خالی tabline_enter_cb(GtkEventController *controller, دوہرا x, دوہرا y, خالی *udata);
ساکن خالی on_select_tab(GtkNotebook *notebook, gpointer *صفحہ, gint idx, gpointer ڈیٹا);
ساکن خالی on_tab_reordered(GtkNotebook *notebook, gpointer *صفحہ, gint idx, gpointer ڈیٹا);
ساکن VimMenu *create_tabline_popup_menu(خالی);
ساکن خالی tabline_جوڑو_css(خالی);
ساکن خالی tabline_menu_press_event(GtkGestureClick *gesture, صحیح n_press, دوہرا x, دوہرا y, GtkWidget *popover);
#اگر_ختم
ساکن خالی mainwin_destroy_cb(GObject *object, gpointer ڈیٹا);
ساکن gboolean delete_event_cb(Gtk_کھڑکی *کھڑکی_2, gpointer ڈیٹا);
ساکن صحیح query_اشارہ_گر_pos(صحیح *x, صحیح *y, GdkModifierType *state);
ساکن خالی mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_ڈیٹا);
ساکن خالی سیٹ_form_سائز(صحیح چوڑائی, صحیح اونچائی);
ساکن خالی drawarea_realize_cb(GtkWidget *widget, gpointer ڈیٹا);
ساکن خالی drawarea_unrealize_cb(GtkWidget *widget, gpointer ڈیٹا);
ساکن خالی clipboard_بدلا_cb(GdkClipboard *clipboard, gpointer user_ڈیٹا);
#اگر_تعریف_شدہ FEAT_MENU
ساکن خالی show_menubar_popover(خالی);
#اگر_ختم

ساکن ثابت حرف *prgname = تہی;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    ساکن gboolean
arg_مطابقت(ثابت حرف *s, ثابت حرف *name, حرف **قدر)
{
    size_t  len = لڑی_لمبائی(name);

    اگر (لڑی_موازنہ(s, name, len) != 0)
	واپس FALSE;
    اگر (s[len] == '=')
	*قدر = (حرف *)s + len + 1;
    ورنہ اگر (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	واپس FALSE;
    واپس TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    خالی
gui_mch_prepare(صحیح *argc, حرف **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    صحیح	i = 0;

    جب (i < *argc)
    {
	حرف	*s = argv[i];
	حرف	*قدر = تہی;
	صحیح	has_inline_قدر = FALSE;
	صحیح	n_strip;

	اگر (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    جاری;
	}

	اگر (strchr(s, '=') != تہی)
	    has_inline_قدر = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	اگر (!has_inline_قدر)
	{
	    اگر (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		قدر = argv[i + 1];
	    ورنہ
		قدر = تہی;
	}

	اگر (arg_مطابقت(s, "-fn", &قدر) || arg_مطابقت(s, "-font", &قدر))
	    font_دلیل = قدر;
	ورنہ اگر (arg_مطابقت(s, "-geom", &قدر)
		|| arg_مطابقت(s, "-geometry", &قدر))
	{
	    اگر (قدر != تہی)
		gui.geom = vim_strsave((char_u *)قدر);
	}
	ورنہ اگر (arg_مطابقت(s, "-bg", &قدر)
		|| arg_مطابقت(s, "-background", &قدر))
	    background_دلیل = قدر;
	ورنہ اگر (arg_مطابقت(s, "-fg", &قدر)
		|| arg_مطابقت(s, "-foreground", &قدر))
	    foreground_دلیل = قدر;
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	ورنہ اگر (لڑی_موازنہ(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_قدر = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#اگر_ختم
	ورنہ اگر (arg_مطابقت(s, "--prg-name", &قدر))
	    // GTK4 specific
	    prgname = قدر;
	ورنہ
	{
	    i++;
	    جاری;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	اگر (قدر != تہی && !has_inline_قدر)
	    n_strip = 2;

	اگر (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    اگر (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * حجم(حرف *));
	    argv[*argc] = تہی;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    خالی
gui_mch_free_all(خالی)
{
}

    ساکن guint
timeout_جوڑو(صحیح وقت, timeout_cb_type (*callback)(gpointer), صحیح *flagp)
{
    واپس g_timeout_جوڑو((guint)وقت, (GSourceFunc)callback, flagp);
}

    ساکن خالی
timeout_remove(guint timer)
{
    g_ماخذ_remove(timer);
}

ساکن long_u blink_waittime = 700;
ساکن long_u blink_ontime = 400;
ساکن long_u blink_offtime = 250;
ساکن guint blink_timer = 0;

    ساکن timeout_cb_type
blink_cb(gpointer ڈیٹا غیر_مستعمل)
{
    اگر (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_جوڑو(blink_offtime, blink_cb, تہی);
    }
    ورنہ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_جوڑو(blink_ontime, blink_cb, تہی);
    }
    واپس FALSE;
}

    صحیح
gui_mch_is_blinking(خالی)
{
    واپس blink_state != BLINK_NONE;
}

    صحیح
gui_mch_is_blink_off(خالی)
{
    واپس blink_state == BLINK_OFF;
}

    خالی
gui_mch_سیٹ_blinking(طویل waittime, طویل on, طویل off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    خالی
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

    خالی
gui_mch_آغاز_blink(خالی)
{
    اگر (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.اندر_focus)
    {
	blink_timer = timeout_جوڑو(blink_waittime, blink_cb, تہی);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    صحیح
gui_mch_early_init_check(صحیح give_پیغام غیر_مستعمل)
{
    واپس OK;
}

    صحیح
gui_mch_init_check(خالی)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_سیٹ_prgname(prgname == تہی ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    اگر (g_getenv("EGL_LOG_LEVEL") == تہی)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    واپس OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    صحیح
gui_mch_init(خالی)
{
    // Allocate GdkRGBA color structs.
    gui.fgcolor = g_new0(GdkRGBA, 1);
    gui.bgcolor = g_new0(GdkRGBA, 1);
    gui.spcolor = g_new0(GdkRGBA, 1);

    gui.def_norm_pixel = 0x00000000;	// black
    gui.def_عقب_pixel = 0x00ffffff;	// white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.عقب_pixel = gui.def_عقب_pixel;

    gui.scrollbar_چوڑائی = SB_DEFAULT_چوڑائی;
    gui.scrollbar_اونچائی = SB_DEFAULT_چوڑائی;

    // Create the main window.
    gui.mainwin = gtk_کھڑکی_new();
    gtk_widget_سیٹ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سیٹ_بنیاد_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    g_signal_connect(G_OBJECT(gui.mainwin), "close-request",
		     G_CALLBACK(delete_event_cb), تہی);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), تہی);

    // A vertical box holds the menubar, toolbar and main text window.
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_سیٹ_homogeneous(GTK_BOX(vbox), FALSE);
    gtk_کھڑکی_سیٹ_بچہ(GTK_کھڑکی(gui.mainwin), vbox);

#اگر_تعریف_شدہ FEAT_MENU
    gui.menubar = vim_menu_bar_new();
    gtk_widget_سیٹ_name(gui.menubar, "vim-menubar");
    gtk_widget_سیٹ_visible(gui.menubar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.menubar);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TOOLBAR
    gui.toolbar = vim_toolbar_new();
    gtk_widget_سیٹ_name(gui.toolbar, "vim-toolbar");
    gtk_widget_سیٹ_visible(gui.toolbar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.toolbar);
    vim_toolbar_سیٹ_style(VIM_TOOLBAR(gui.toolbar), toolbar_flags, tbis_flags);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    gui.tabline = gtk_notebook_new();
    gtk_notebook_سیٹ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سیٹ_show_tabs(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_notebook_سیٹ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_widget_جوڑو_css_class(gui.tabline, "vim-tabline");
    tabline_جوڑو_css();
    gtk_widget_سیٹ_visible(gui.tabline, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.tabline);

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), تہی);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), تہی);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), تہی);

	// Make sure that tabline_enter_cb() is always called before
	// tabpage_enter_cb().
	gtk_event_controller_سیٹ_اشاعت_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	gtk_widget_جوڑو_controller(gui.tabline, mcontroller);
    }

    // Create right click popup menu for tabline
    {
	GtkGesture	*click;
	VimMenu		*menu;

	click = gtk_gesture_click_new();
	menu = create_tabline_popup_menu();

	gtk_widget_سیٹ_والد(GTK_WIDGET(menu), gui.tabline);
	g_object_سیٹ_ڈیٹا(G_OBJECT(gui.tabline), "menu", menu);

	gtk_popover_سیٹ_has_arrow(GTK_POPOVER(menu), FALSE);
	gtk_popover_سیٹ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
	// Make popover start at top left corner
	gtk_widget_سیٹ_halign(GTK_WIDGET(menu), GTK_ALIGN_آغاز);

	// Listen for anny mouse button
	gtk_gesture_single_سیٹ_button(GTK_GESTURE_SINGLE(click), 0);

	g_signal_connect_object(click, "pressed",
		G_CALLBACK(tabline_menu_press_event),
		menu, G_CONNECT_DEFAULT);
	gtk_widget_جوڑو_controller(gui.tabline, GTK_EVENT_CONTROLLER(click));
    }
#اگر_ختم

    // The form widget manages absolute positioning of scrollbars and the draw
    // area.
    gui.formwin = vim_form_new();
    gtk_widget_سیٹ_name(gui.formwin, "vim-gtk-form");
    gtk_widget_سیٹ_vexpand(gui.formwin, TRUE);
    gtk_widget_سیٹ_hexpand(gui.formwin, TRUE);
    gtk_box_append(GTK_BOX(vbox), gui.formwin);

    // The drawing area for the editor content.
    gui.drawarea = vim_draw_area_new();
    gtk_widget_سیٹ_focusable(gui.drawarea, TRUE);
    gtk_widget_سیٹ_vexpand(gui.drawarea, TRUE);
    gtk_widget_سیٹ_hexpand(gui.drawarea, TRUE);
    vim_form_put(VIM_FORM(gui.formwin), gui.drawarea, 0, 0);

    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), تہی);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), تہی);

    // Set up event controllers.
    {
	GtkEventController *کلید_ctrl = gtk_event_controller_کلید_new();
	g_signal_connect(کلید_ctrl, "key-pressed",
			 G_CALLBACK(کلید_press_event), تہی);
	g_signal_connect(کلید_ctrl, "key-released",
			 G_CALLBACK(کلید_release_event), تہی);
	gtk_widget_جوڑو_controller(gui.drawarea, کلید_ctrl);
#اگر_تعریف_شدہ FEAT_XIM
	xim_init();
#اگر_ختم
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_سیٹ_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), تہی);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), تہی);
	gtk_widget_جوڑو_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), تہی);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), تہی);
	gtk_widget_جوڑو_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), تہی);
	gtk_widget_جوڑو_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_اندر_event), تہی);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_برآمد_event), تہی);
	gtk_widget_جوڑو_controller(gui.drawarea, focus);
    }

#اگر_تعریف_شدہ FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_ہدف = gtk_drop_ہدف_async_new(تہی,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_سیٹ_dnd_targets();
	g_signal_connect(gui.drop_ہدف, "drop",
			 G_CALLBACK(drop_cb), تہی);
	gtk_widget_جوڑو_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_ہدف));
    }
#اگر_ختم

    gui.border_انحراف = gui.border_چوڑائی;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_اشارہ_گر = gdk_cursor_new_from_name("none", تہی);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	اگر (primary != تہی)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_بدلا_cb), &clip_ستارہ);
	اگر (regular != تہی)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_بدلا_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_ستارہ);

    واپس OK;
}

    خالی
gui_mch_new_colors(خالی)
{
    اگر (gui.drawarea != تہی && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_قطار_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحیح
gui_mch_open(خالی)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_چوڑائی;
    guint pixel_اونچائی;

    اگر (gui.geom != تہی)
    {
	صحیح		نقاب;
	غیرعلامتی صحیح	w, h;

	نقاب = vim_parse_geometry((حرف *)gui.geom, &w, &h);

	اگر (نقاب & چوڑائی_قدر)
	    Columns = w;
	اگر (نقاب & اونچائی_قدر)
	{
	    اگر (p_کھڑکی > (طویل)h - 1 || !option_was_سیٹ((char_u *)"window"))
		p_کھڑکی = h - 1;
	    صفیں = h;
	}
	حد_بندی_screen_سائز();

	VIM_CLEAR(gui.geom);
    }
    ورنہ
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	اگر (Columns > 80)
	    Columns = 80;
	اگر (صفیں > 24)
	    صفیں = 24;
    }

    pixel_چوڑائی = (guint)(gui_get_بنیاد_چوڑائی() + Columns * gui.char_چوڑائی);
    pixel_اونچائی = (guint)(gui_get_بنیاد_اونچائی() + صفیں * gui.char_اونچائی);

    pixel_چوڑائی  += get_menu_tool_چوڑائی();
    pixel_اونچائی += get_menu_tool_اونچائی();

    اگر (foreground_دلیل != تہی)
	fg_pixel = gui_get_color((char_u *)foreground_دلیل);
    اگر (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    اگر (background_دلیل != تہی)
	bg_pixel = gui_get_color((char_u *)background_دلیل);
    اگر (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    اگر (ملا_الٹ_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_عقب_pixel = fg_pixel;
    }
    ورنہ
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_عقب_pixel = bg_pixel;
    }

    سیٹ_معمول_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), تہی);
    // Resize is handled by GtkForm's size_allocate callback.

    سیٹ_form_سائز((صحیح)pixel_چوڑائی - get_menu_tool_چوڑائی(),
	    (صحیح)pixel_اونچائی - get_menu_tool_اونچائی());

    gtk_کھڑکی_present(GTK_کھڑکی(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    واپس OK;
}

    خالی
gui_mch_exit(صحیح rc غیر_مستعمل)
{
    اگر (gui.mainwin != تہی)
    {
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	// Must unparent popover menu for tabline or we will get warning
	gtk_widget_unparent(g_object_get_ڈیٹا(G_OBJECT(gui.tabline), "menu"));
#اگر_ختم
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
	// Make sure to destroy popover used for balloon eval, or we will get a
	// warning from GTK that the draw area still has children left.
	gui_mch_destroy_beval_area(balloonEval);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
	// Make sure to unparent any popover menus
	{
	    vimmenu_T *menu;

	    FOR_ALL_MENUS(menu)
	    {
		اگر ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != تہی)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#اگر_ختم
	gtk_کھڑکی_destroy(GTK_کھڑکی(gui.mainwin));
    }
}

    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    واپس FAIL;
}

    خالی
gui_mch_سیٹ_winpos(صحیح x غیر_مستعمل, صحیح y غیر_مستعمل)
{
    // GTK4/Wayland: window positioning not available
}

    صحیح
gui_mch_maximized(خالی)
{
    واپس gtk_کھڑکی_is_maximized(GTK_کھڑکی(gui.mainwin));
}

    خالی
gui_mch_unmaximize(خالی)
{
    اگر (gui.mainwin != تہی)
	gtk_کھڑکی_unmaximize(GTK_کھڑکی(gui.mainwin));
}

    خالی
gui_mch_سیٹ_fullscreen(صحیح flag)
{
    اگر (gui.mainwin == تہی)
	واپس;
    اگر (flag)
	gtk_کھڑکی_fullscreen(GTK_کھڑکی(gui.mainwin));
    ورنہ
	gtk_کھڑکی_unfullscreen(GTK_کھڑکی(gui.mainwin));
}

    ساکن خالی
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec غیر_مستعمل, gpointer user_ڈیٹا غیر_مستعمل)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    اگر (gtk_کھڑکی_is_fullscreen(GTK_کھڑکی(obj)))
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
    خالی
gui_mch_newfont(خالی)
{
    صحیح w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_چوڑائی(gui.formwin);
    h = gtk_widget_get_اونچائی(gui.formwin);
    gui_resize_shell(w, h);
}

    خالی
gui_mch_settitle(char_u *title, char_u *icon غیر_مستعمل)
{
    اگر (title != تہی && gui.mainwin != تہی)
	gtk_کھڑکی_سیٹ_title(GTK_کھڑکی(gui.mainwin), (ثابت حرف *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    خالی
gui_gtk_init_decor_اونچائی(خالی)
{
    صحیح h = gtk_widget_get_اونچائی(gui.mainwin);

    اگر (h == 0)
	واپس;

    h -= get_menu_tool_اونچائی();
    h -= gtk_widget_get_اونچائی(gui.formwin);

    gui.decor_اونچائی = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    ساکن خالی
سیٹ_form_سائز(صحیح چوڑائی, صحیح اونچائی)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    اگر (gtk_widget_get_چوڑائی(gui.formwin) == چوڑائی
	    && gtk_widget_get_اونچائی(gui.formwin) == اونچائی)
	واپس;

    // Remember the size the form widget is supposed to get. An allocation
    // that arrives before the compositor has answered this request still has
    // the previous size and must not be used.
    gui.pending_form_w = چوڑائی;
    gui.pending_form_h = اونچائی;
    gui.pending_form_skip = 1;

    // GtkWindow default size also includes client side decorations, so must
    // include it also.  It also keeps the natural width of the toolbar from
    // deciding the width.
    gtk_کھڑکی_سیٹ_default_سائز(GTK_کھڑکی(gui.mainwin),
	    چوڑائی + get_menu_tool_چوڑائی(),
	    اونچائی + get_menu_tool_اونچائی() + gui.decor_اونچائی);

    // The window drops a request made while it is being presented, and
    // "gui.decor_height" is not known before that.  A size request on the form
    // widget is honoured then; it is dropped again once the size was given.
    gtk_widget_سیٹ_سائز_request(gui.formwin, چوڑائی, اونچائی);
}

    خالی
gui_mch_سیٹ_shellsize(صحیح چوڑائی, صحیح اونچائی,
	صحیح min_چوڑائی غیر_مستعمل, صحیح min_اونچائی غیر_مستعمل,
	صحیح بنیاد_چوڑائی غیر_مستعمل, صحیح بنیاد_اونچائی غیر_مستعمل,
	صحیح direction غیر_مستعمل)
{
    سیٹ_form_سائز(چوڑائی, اونچائی);

    gui_mch_update();
}

    خالی
gui_mch_get_screen_dimensions(صحیح *screen_w, صحیح *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    اگر (surface != تہی)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	اگر (monitor != تہی)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.چوڑائی;
	    *screen_h = geom.اونچائی;
	    واپس;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#اگر_تعریف_شدہ FEAT_MENU
    خالی
gui_mch_enable_menu(صحیح showit)
{
    اگر (gui.menubar != تہی)
    {
	gtk_widget_سیٹ_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	اگر (gui.drawarea != تہی)
	    gtk_widget_قطار_draw(gui.drawarea);
    }
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TOOLBAR
    خالی
gui_mch_show_toolbar(صحیح showit)
{
    اگر (gui.toolbar != تہی)
    {
	gtk_widget_سیٹ_visible(gui.toolbar, showit);
	اگر (showit)
	    vim_toolbar_سیٹ_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	اگر (gui.drawarea != تہی)
	    gtk_widget_قطار_draw(gui.drawarea);
    }
}
#اگر_ختم

    خالی
gui_mch_سیٹ_dark_theme(صحیح dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    اگر (settings != تہی)
	g_object_سیٹ(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, تہی);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    صحیح
gui_mch_adjust_charheight(خالی)
{
    PangoFontMetrics *metrics;
    صحیح ascent;
    صحیح descent;

    metrics = pango_context_get_metrics(gui.متن_context, gui.norm_font,
			    pango_context_get_language(gui.متن_context));
    ascent = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);
    pango_font_metrics_unref(metrics);

    gui.char_اونچائی = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_اونچائی = MAX(gui.char_اونچائی, gui.char_ascent + 1);

    واپس OK;
}

قسمتعریف ساخت {
    PangoFontDescription    *نتیجہ;
    gboolean		    مکمل;
} Font_Dialog_ڈیٹا;

    ساکن خالی
font_dialog_اختتام_cb(GObject *ماخذ, GAsync_نتیجہ *res, gpointer ڈیٹا)
{
    Font_Dialog_ڈیٹا *fdd = (Font_Dialog_ڈیٹا *)ڈیٹا;
    fdd->نتیجہ = gtk_font_dialog_انتخاب_font_اختتام(
		    GTK_FONT_DIALOG(ماخذ), res, تہی);
    fdd->مکمل = TRUE;
}

    ساکن gboolean
font_filter(gpointer شے, gpointer ڈیٹا غیر_مستعمل)
{
    اگر (PANGO_IS_FONT_FAMILY(شے))
	واپس pango_font_family_is_monospace(PANGO_FONT_FAMILY(شے));
    اگر (PANGO_IS_FONT_FACE(شے))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(شے));
	اگر (family != تہی)
	    واپس pango_font_family_is_monospace(family);
    }
    واپس TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = تہی;
    char_u		*fontname = تہی;
    Font_Dialog_ڈیٹا	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_سیٹ_modal(dlg, TRUE);
    gtk_font_dialog_سیٹ_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, تہی, تہی)));

    اگر (oldval != تہی && oldval[0] != NUL)
    {
	char_u *oldname;

	اگر (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, تہی);
	ورنہ
	    oldname = oldval;

	اگر (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    اگر (p != تہی)
	    {
		STRCPY(p + STRLEN(p), " 10");
		اگر (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((ثابت حرف *)oldname);
	اگر (oldname != oldval)
	    vim_free(oldname);
    }
    ورنہ
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.نتیجہ = تہی;
    fdd.مکمل = FALSE;

    gtk_font_dialog_انتخاب_font(dlg, GTK_کھڑکی(gui.mainwin),
	    initial, تہی, font_dialog_اختتام_cb, &fdd);

    جب (!fdd.مکمل)
	g_main_context_iteration(تہی, TRUE);

    اگر (fdd.نتیجہ != تہی)
    {
	حرف *name = pango_font_description_مقصد_string(fdd.نتیجہ);
	اگر (name != تہی)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    اگر (p != تہی && داخلہ_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&داخلہ_conv, p, تہی);
		vim_free(p);
	    }
	    ورنہ
		fontname = p;
	}
	pango_font_description_free(fdd.نتیجہ);
    }

    اگر (initial != تہی)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    واپس fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    ساکن خالی
ascii_glyph_جدول_init(خالی)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *شے_list;
    صحیح		    i;

    اگر (gui.ascii_glyphs != تہی)
	pango_glyph_string_free(gui.ascii_glyphs);
    اگر (gui.ascii_font != تہی)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = تہی;
    gui.ascii_font   = تہی;

    برائے (i = 0; i < 128; ++i)
    {
	اگر (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	ورنہ
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    شے_list = pango_itemize(gui.متن_context, (ثابت حرف *)ascii_chars,
			      0, حجم(ascii_chars), attr_list, تہی);

    اگر (شے_list != تہی && شے_list->اگلا == تہی)
    {
	Pango_شے   *شے;
	صحیح	    چوڑائی;

	شے  = (Pango_شے *)شے_list->ڈیٹا;
	چوڑائی = gui.char_چوڑائی * PANGO_SCALE;

	gui.ascii_font = شے->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت حرف *)ascii_chars, حجم(ascii_chars),
		    &شے->analysis, gui.ascii_glyphs);

	اگر (gui.ascii_glyphs->عدد_glyphs == (صحیح)حجم(ascii_chars))
	{
	    برائے (i = 0; i < gui.ascii_glyphs->عدد_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_انحراف += MAX(0, چوڑائی - geom->چوڑائی) / 2;
		geom->چوڑائی = چوڑائی;
	    }
	}
	ورنہ
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = تہی;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = تہی;
	}
    }

    g_list_foreach(شے_list, (GFunc)(خالی *)&pango_شے_free, تہی);
    g_list_free(شے_list);
    pango_attr_list_unref(attr_list);
}

    ساکن خالی
get_styled_font_variants(خالی)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_بھرو_font(gui.متن_context, gui.norm_font);
    اگر (plain_font == تہی)
	واپس;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_سیٹ_وزن(bold_font_desc, PANGO_وزن_BOLD);

    bold_font = pango_context_بھرو_font(gui.متن_context, bold_font_desc);
    اگر (bold_font != تہی)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    صحیح
gui_mch_init_font(char_u *font_name, صحیح fontset غیر_مستعمل)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحیح			    چوڑائی;

    اگر (font_name == تہی)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    اگر (font_desc == تہی)
	واپس FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_سیٹ_font_description(gui.متن_context, font_desc);

    layout = pango_layout_new(gui.متن_context);
    pango_layout_سیٹ_متن(layout, "MW", 2);
    pango_layout_get_سائز(layout, &چوڑائی, تہی);
    g_object_unref(layout);

    gui.char_چوڑائی = (چوڑائی / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    اگر (gui.char_چوڑائی <= 0)
	gui.char_چوڑائی = 8;

    gui_mch_adjust_charheight();

    hl_سیٹ_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_جدول_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	صحیح im_صف = gui.صف < screen_صفیں ? gui.صف : screen_صفیں - 1;
	صحیح im_ستون = gui.ستون < screen_Columns ? gui.ستون : screen_Columns - 1;
	im_سیٹ_position(im_صف, im_ستون);
    }
    واپس OK;
}

    GuiFont
gui_mch_get_font(char_u *name, صحیح report_error)
{
    PangoFontDescription *font;

    اگر (name == تہی)
	واپس تہی;

    font = pango_font_description_from_string((ثابت حرف *)name);
    اگر (font == تہی)
    {
	اگر (report_error)
	    semsg(_(e_unknown_font_str), name);
	واپس تہی;
    }

    // Ensure a size is set
    اگر (pango_font_description_get_سائز(font) <= 0)
	pango_font_description_سیٹ_سائز(font, 10 * PANGO_SCALE);

    واپس font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name غیر_مستعمل)
{
    اگر (font != NOFONT)
    {
	حرف *desc = pango_font_description_مقصد_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	واپس ret;
    }
    واپس تہی;
}

    خالی
gui_mch_free_font(GuiFont font)
{
    اگر (font != NOFONT)
	pango_font_description_free(font);
}

    خالی
gui_mch_expand_font(
    optexpand_T	*دلائل,
    خالی	*param,
    صحیح		(*جوڑو_مطابقت)(char_u *val))
{
    PangoFontFamily	**font_families = تہی;
    صحیح			n_families = 0;
    صحیح			wide = *(صحیح *)param;

    اگر (دلائل->oe_include_orig_val && *دلائل->oe_opt_قدر == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	اگر (جوڑو_مطابقت((char_u *)DEFAULT_FONT) != OK)
	    واپس;
    }

    pango_context_list_families(
	    gui.متن_context,
	    &font_families,
	    &n_families);

    برائے (صحیح i = 0; i < n_families; i++)
    {
	اگر (!wide && !pango_font_family_is_monospace(font_families[i]))
	    جاری;

	ثابت حرف *fam_name = pango_font_family_get_name(font_families[i]);
	اگر (داخلہ_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&داخلہ_conv,
		    (char_u *)fam_name, تہی);
	    اگر (buf != تہی)
	    {
		اگر (جوڑو_مطابقت(buf) != OK)
		{
		    vim_free(buf);
		    توڑو;
		}
		vim_free(buf);
	    }
	    ورنہ
		توڑو;
	}
	ورنہ
	{
	    اگر (جوڑو_مطابقت((char_u *)fam_name) != OK)
		توڑو;
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
    اگر (!gui.اندر_use)
	واپس INVALCOLOR;

    اگر (name != تہی)
	واپس gui_get_color_cmn(name);

    واپس INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
    واپس gui_get_rgb_color_cmn(r, g, b);
}

    ساکن GdkRGBA
color_مقصد_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    واپس rgba;
}

    خالی
gui_mch_سیٹ_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_مقصد_rgba(color);
}

    خالی
gui_mch_سیٹ_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_مقصد_rgba(color);
}

    خالی
gui_mch_سیٹ_sp_color(guicolor_T color)
{
    *gui.spcolor = color_مقصد_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    واپس pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    خالی
gui_gtk4_update_سائز(خالی)
{
    vim_draw_area_سیٹ_سائز(VIM_DRAW_AREA(gui.drawarea),
	    gui.عدد_صفیں, gui.عدد_ستون_تعداد);
}

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    خالی
gui_gtk4_جوڑو_multisign(
	cairo_surface_t *surf,
	صحیح		صف,
	صحیح		ستون,
	صحیح		چوڑائی,
	صحیح		اونچائی)
{
    vim_draw_area_جوڑو_multisign(VIM_DRAW_AREA(gui.drawarea), surf, صف, ستون,
	    چوڑائی, اونچائی);
}
#اگر_ختم

    خالی
gui_mch_clear_بلاک(صحیح row1, صحیح col1, صحیح row2, صحیح col2)
{
    vim_draw_area_clear_بلاک(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    اگر (gui.drawarea != تہی)
	gtk_widget_قطار_draw(gui.drawarea);
}

    خالی
gui_mch_clear_all(خالی)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    اگر (gui.drawarea != تہی)
	gtk_widget_قطار_draw(gui.drawarea);
}

    خالی
gui_mch_delete_سطریں(صحیح صف, صحیح عدد_سطریں)
{
    vim_draw_area_move_بلاک(VIM_DRAW_AREA(gui.drawarea),
	    صف, صف + عدد_سطریں, gui.scroll_region_bot,
	    gui.scroll_region_بایاں, gui.scroll_region_دایاں);
    gui_clear_بلاک(
	    gui.scroll_region_bot - عدد_سطریں + 1, gui.scroll_region_بایاں,
	    gui.scroll_region_bot, gui.scroll_region_دایاں);

    gtk_widget_قطار_draw(gui.drawarea);
}

    خالی
gui_mch_داخل_کرو_سطریں(صحیح صف, صحیح عدد_سطریں)
{
    vim_draw_area_move_بلاک(VIM_DRAW_AREA(gui.drawarea),
	    صف + عدد_سطریں, صف, gui.scroll_region_bot - عدد_سطریں,
	    gui.scroll_region_بایاں, gui.scroll_region_دایاں);
    gui_clear_بلاک(
	    صف, gui.scroll_region_بایاں,
	    صف + عدد_سطریں - 1, gui.scroll_region_دایاں);

    gtk_widget_قطار_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    خالی
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, صحیح w, صحیح h)
{
    gui_mch_سیٹ_bg_color(bg);
    gui_mch_سیٹ_fg_color(fg);
    vim_draw_area_سیٹ_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_قطار_draw(gui.drawarea);
}

    خالی
gui_mch_flash(صحیح msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (صحیح)صفیں - 1, (صحیح)Columns - 1);
    gui_mch_flush();
    ui_delay((طویل)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (صحیح)صفیں - 1, (صحیح)Columns - 1);
}

    خالی
gui_mch_invert_rectangle(صحیح r, صحیح c, صحیح nr, صحیح nc)
{
    vim_draw_area_invert_بلاک(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_قطار_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    ساکن gboolean
کلید_press_event(Gtk_Event_Controller_کلید *controller غیر_مستعمل,
	guint کلید_sym, guint keycode غیر_مستعمل,
	GdkModifierType state, gpointer ڈیٹا غیر_مستعمل)
{
    char_u	string[32], string2[32];
    صحیح		len;
    صحیح		i;
    صحیح		modifiers;
    صحیح		کلید;
    char_u	*s, *d;

#اگر_تعریف_شدہ FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    اگر (xic != تہی)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	اگر (event != تہی && gtk_im_context_filter_keypress(xic, event))
	    واپس TRUE;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MENU
    اگر (کلید_sym == GDK_کلید_F10 && gui.menubar != تہی)
    {
	ساکن char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	اگر (check_map(k10, State, FALSE, TRUE, FALSE, تہی, تہی) == تہی)
	{
	    show_menubar_popover();
	    واپس TRUE;
	}
    }
#اگر_ختم

    len = keyval_مقصد_string(کلید_sym, string2);

    اگر (len > 1 && داخلہ_conv.vc_type != CONV_NONE)
	len = convert_داخلہ(string2, len, حجم(string2));

    s = string2;
    d = string;
    برائے (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	اگر (d[-1] == CSI && d + 2 < string + حجم(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (صحیح)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    اگر (کلید_sym == GDK_کلید_ISO_بایاں_Tab)
    {
	کلید_sym = GDK_کلید_Tab;
	state |= GDK_سرکاؤ_نقاب;
    }

#اگر_تعریف_شدہ FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    اگر ((state & GDK_ALT_نقاب)
	    && gui.menu_is_فعال
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	واپس FALSE;
#اگر_ختم

    // Check for special keys
    اگر (len == 0 || len == 1)
    {
	برائے (i = 0; special_کلیدیں[i].کلید_sym != 0; i++)
	{
	    اگر (special_کلیدیں[i].کلید_sym == کلید_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیدیں[i].code0;
		string[2] = special_کلیدیں[i].code1;
		len = -3;
		توڑو;
	    }
	}
    }

    اگر (len == 0)
	واپس TRUE;

    اگر (len == -3)
	کلید = مقصد_SPECIAL(string[1], string[2]);
    ورنہ
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
	string[1] = K_دوسرا(کلید);
	string[2] = K_تیسرا(کلید);
	len = 3;
    }
    ورنہ
    {
	کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);
	modifiers = may_remove_سرکاؤ_modifier(modifiers, کلید);
	len = mb_char2bytes(کلید, string);
    }

    اگر (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	جوڑو_مقصد_داخلہ_buf(string2, 3);
    }

    {
	صحیح int_ch = check_for_مداخلت(کلید, modifiers);
	اگر (int_ch != NUL)
	{
	    trash_داخلہ_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    جوڑو_مقصد_داخلہ_buf(string, len);

    اگر (p_mh)
	gui_mch_mousehide(TRUE);

    واپس TRUE;
}

    ساکن خالی
کلید_release_event(Gtk_Event_Controller_کلید *controller غیر_مستعمل,
	guint keyval غیر_مستعمل, guint keycode غیر_مستعمل,
	GdkModifierType state غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
}

ساکن صحیح mouse_timed_برآمد = TRUE;
ساکن guint mouse_click_timer = 0;

    ساکن timeout_cb_type
mouse_click_timer_cb(gpointer ڈیٹا)
{
    *(صحیح *)ڈیٹا = TRUE;
    واپس FALSE;
}

    ساکن صحیح
modifiers_gdk2mouse(guint state)
{
    صحیح modifiers = 0;

    اگر (state & GDK_سرکاؤ_نقاب)
	modifiers |= MOUSE_سرکاؤ;
    اگر (state & GDK_CONTROL_نقاب)
	modifiers |= MOUSE_CTRL;
    اگر (state & GDK_ALT_نقاب)
	modifiers |= MOUSE_ALT;

    واپس modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#تعریف_کرو ANY_BUTTON_نقاب (GDK_BUTTON1_نقاب | GDK_BUTTON2_نقاب \
	| GDK_BUTTON3_نقاب | GDK_BUTTON4_نقاب | GDK_BUTTON5_نقاب)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
ساکن صحیح mouse_pressed_button = -1;

ساکن guint motion_repeat_timer = 0;
ساکن gboolean motion_repeat_انحراف = FALSE;
ساکن صحیح mouse_press_x = -1, mouse_press_y = -1;

    ساکن خالی
button_press_event(GtkGestureClick *gesture, صحیح n_press غیر_مستعمل,
	دوہرا x, دوہرا y, gpointer ڈیٹا غیر_مستعمل)
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

    اگر (!mouse_timed_برآمد && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_برآمد = FALSE;
    mouse_click_timer = timeout_جوڑو(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_برآمد);

    انتخاب (btn)
    {
	حالت 1: button = MOUSE_بایاں; توڑو;
	حالت 2: button = MOUSE_درمیان; توڑو;
	حالت 3: button = MOUSE_دایاں; توڑو;
	حالت 8: button = MOUSE_X1; توڑو;
	حالت 9: button = MOUSE_X2; توڑو;
	طےشدہ: واپس;
    }

    mouse_press_x = (صحیح)x;
    mouse_press_y = (صحیح)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (صحیح)x, (صحیح)y, repeated_click, vim_modifiers);
}

    ساکن خالی
button_release_event(GtkGestureClick *gesture, صحیح n_press غیر_مستعمل,
	دوہرا x, دوہرا y, gpointer ڈیٹا غیر_مستعمل)
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

ساکن دوہرا پچھلا_mouse_x = -1.0;
ساکن دوہرا پچھلا_mouse_y = -1.0;

ساکن GdkModifierType cur_state = 0;

    ساکن timeout_cb_type
mouse_repeat_timer_cb(gpointer ڈیٹا غیر_مستعمل)
{
    صحیح x, y;

    اگر (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	واپس G_ماخذ_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    اگر (vim_مستعمل_اندر_داخلہ_buf() > 10)
	واپس G_ماخذ_CONTINUE;

    x = (صحیح)پچھلا_mouse_x;
    y = (صحیح)پچھلا_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    اگر (motion_repeat_انحراف)
	x += gui.char_چوڑائی;
    motion_repeat_انحراف = !motion_repeat_انحراف;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    واپس G_ماخذ_CONTINUE;
}

    ساکن خالی
motion_notify_event(GtkEventControllerMotion *controller غیر_مستعمل,
	دوہرا x, دوہرا y, gpointer ڈیٹا غیر_مستعمل)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    اگر (mouse_pressed_button >= 0 && event != تہی
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_نقاب))
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

	اگر (event != تہی)
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
	w = gtk_widget_get_چوڑائی(gui.drawarea);
	h = gtk_widget_get_اونچائی(gui.drawarea);

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
	    ورنہ
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_جوڑو(delay,
		    mouse_repeat_timer_cb, تہی);
	}
	ورنہ
	    motion_repeat_timer = 0;
    }
    ورنہ
	gui_mouse_moved((صحیح)x, (صحیح)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    اگر (p_mh && ((پچھلا_mouse_x == -1 || پچھلا_mouse_y == -1)
		|| (fabs(پچھلا_mouse_x - x) > 0.05
		    && fabs(پچھلا_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    پچھلا_mouse_x = x;
    پچھلا_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    ساکن خالی
enter_notify_event(GtkEventControllerMotion *controller غیر_مستعمل,
	دوہرا x, دوہرا y, gpointer ڈیٹا غیر_مستعمل)
{
    پچھلا_mouse_x = x;
    پچھلا_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    ساکن gboolean
scroll_event(GtkEventControllerScroll *controller غیر_مستعمل,
	دوہرا dx غیر_مستعمل, دوہرا dy, gpointer ڈیٹا غیر_مستعمل)
{
    صحیح button;
    int_u vim_modifiers;
    صحیح x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    اگر (event == تہی)
	واپس FALSE;
    state = gdk_event_get_modifier_state(event);

    اگر (dy < 0)
	button = MOUSE_4;	// scroll up
    ورنہ اگر (dy > 0)
	button = MOUSE_5;	// scroll down
    ورنہ اگر (dx < 0)
	button = MOUSE_7;	// scroll left
    ورنہ اگر (dx > 0)
	button = MOUSE_6;	// scroll right
    ورنہ
	واپس FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    واپس TRUE;
}

    ساکن خالی
focus_اندر_event(GtkEventControllerFocus *controller غیر_مستعمل,
	gpointer ڈیٹا غیر_مستعمل)
{
    اگر (gtk_کھڑکی_is_فعال(GTK_کھڑکی(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	اگر (blink_state == BLINK_NONE)
	    gui_mch_آغاز_blink();
    }
}

    ساکن خالی
focus_برآمد_event(GtkEventControllerFocus *controller غیر_مستعمل,
	gpointer ڈیٹا غیر_مستعمل)
{
    اگر (!gtk_کھڑکی_is_فعال(GTK_کھڑکی(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	اگر (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#اگر defined(FEAT_IMAGE)
    ساکن خالی
scale_ضربی_عدد_cb(GdkSurface  *surface,
	GParamSpec	    *pspec غیر_مستعمل,
	خالی		    *udata غیر_مستعمل)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#اگر_ختم

    ساکن خالی
drawarea_realize_cb(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
#اگر defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_ضربی_عدد_cb), تہی);
    redraw_all_later(UPD_VALID);
#اگر_ختم
    gui_mch_new_colors();
}

    ساکن خالی
drawarea_unrealize_cb(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_XIM
    im_shutdown();
#اگر_ختم
}

قسمتعریف شمار
{
    READ_ڈیٹا_CLIP,	    // Clipboard data
    READ_ڈیٹا_DROP,	    // URI or text DND data
    READ_ڈیٹا_DROP_ڈیٹا	    // HTML DND data
} Read_ڈیٹا_Type;

#تعریف_کرو READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
قسمتعریف ساخت
{
    Read_ڈیٹا_Type    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    حرف	    *buf; // NULL when not needed
} Read_ڈیٹا;

#اگر_تعریف_شدہ FEAT_DND
قسمتعریف ساخت
{
    Read_ڈیٹا	rd;
    GdkDrop	*drop;
    صحیح		x;
    صحیح		y;
} Drop_Read_ڈیٹا;
#اگر_ختم

قسمتعریف ساخت
{
    Read_ڈیٹا	rd;
    Clipboard_T *cbd;
    حرف	*mime_type;
} Clip_Read_ڈیٹا;

    ساکن Read_ڈیٹا *
read_ڈیٹا_new(Read_ڈیٹا_Type type, size_t sz)
{
    Read_ڈیٹا *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    اگر (type == READ_ڈیٹا_CLIP || type == READ_ڈیٹا_DROP_ڈیٹا)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    واپس rd;
}

    ساکن خالی
read_ڈیٹا_free(Read_ڈیٹا *rd)
{
    g_cancellable_cancel(rd->cancel);

    اگر (rd->type == READ_ڈیٹا_CLIP)
	g_free(((Clip_Read_ڈیٹا *)rd)->mime_type);
    ورنہ اگر (rd->type == READ_ڈیٹا_DROP || rd->type == READ_ڈیٹا_DROP_ڈیٹا)
	g_object_unref(((Drop_Read_ڈیٹا *)rd)->drop);
    g_object_unref(rd->cancel);
    اگر (rd->arr != تہی)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#اگر_تعریف_شدہ FEAT_DND
ساکن خالی drop_read_متن(GdkDrop *drop, char_u *متن);
#اگر_ختم

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    ساکن خالی
read_ڈیٹا_داخلہ_cb(
	GInput_دھارا	*دھارا,
	GAsync_نتیجہ	*نتیجہ,
	Read_ڈیٹا	*rd)
{
    ssize_t r = g_داخلہ_دھارا_read_اختتام(دھارا, نتیجہ, تہی);

    اگر (r == -1)
    {
	// Error occurred
	Drop_Read_ڈیٹا *drd = (Drop_Read_ڈیٹا *)rd;

	اگر (rd->type == READ_ڈیٹا_DROP_ڈیٹا)
	    gdk_drop_اختتام(drd->drop, 0);
    }
    ورنہ اگر (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	اگر (rd->type == READ_ڈیٹا_CLIP)
	{
	    Clip_Read_ڈیٹا    *crd = (Clip_Read_ڈیٹا *)rd;
	    طویل	    len;
	    char_u	    *actual, *final;
	    صحیح		    motion_type = MAUTO;
	    char_u	    *tofree = تہی;

	    len = (طویل)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = تہی;

	    اگر (clip_convert_ڈیٹا(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#اگر_تعریف_شدہ FEAT_DND
	ورنہ اگر (rd->type == READ_ڈیٹا_DROP_ڈیٹا)
	{
	    Drop_Read_ڈیٹا    *drd = (Drop_Read_ڈیٹا *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ثابت uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = تہی;
	    drop_read_متن(drd->drop, str);
	    g_free(str);
	    gdk_drop_اختتام(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#اگر_ختم
    }
    ورنہ
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ثابت uint8_t *)rd->buf, r);
	g_داخلہ_دھارا_read_async(دھارا, rd->buf, READDATA_BUFSIZE,
		G_ترجیح_بلند,
		rd->cancel, (GAsyncReadyCallback)read_ڈیٹا_داخلہ_cb, rd);
	واپس;
    }

    read_ڈیٹا_free(rd);
    g_object_unref(دھارا);
}

#اگر_تعریف_شدہ FEAT_DND
/*
 * Set up for receiving DND items.
 */
    خالی
gui_gtk_سیٹ_dnd_targets(خالی)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_جوڑو_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_جوڑو_gtype(builder, G_TYPE_STRING);
    اگر (clip_html)
	gdk_content_formats_builder_جوڑو_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_مقصد_formats(builder);
    gtk_drop_ہدف_async_سیٹ_formats(gui.drop_ہدف, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    ساکن خالی
drop_read_متن(GdkDrop *drop غیر_مستعمل, char_u *متن)
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

    اگر (متن == تہی || *متن == NUL || !query_اشارہ_گر_pos(تہی, تہی, &state))
	واپس;

    dnd_yank_drag_ڈیٹا((char_u *)متن, (طویل)STRLEN(متن));

    dropkey[2] = modifiers_gdk2vim(state);
    اگر (dropkey[2] != 0)
	جوڑو_مقصد_داخلہ_buf(dropkey, (صحیح)حجم(dropkey));
    ورنہ
	جوڑو_مقصد_داخلہ_buf(dropkey + 3, (صحیح)(حجم(dropkey) - 3));
}

    ساکن خالی
drop_read_قدر_cb(GdkDrop *drop, GAsync_نتیجہ *نتیجہ, Drop_Read_ڈیٹا *drd)
{
    ثابت GValue    *قدر = gdk_drop_read_قدر_اختتام(drop, نتیجہ, تہی);
    gboolean	    success = FALSE;

    اگر (قدر == تہی)
	جاؤ باہر_نکلو;

    اگر (G_قدر_HOLDS(قدر, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_قدر_get_boxed(قدر);
	صحیح	nfiles = g_slist_لمبائی(files);
	char_u	**fnames;
	صحیح	i;

	اگر (nfiles <= 0)
	    جاؤ باہر_نکلو;

	fnames = ALLOC_MULT(char_u *, nfiles);
	اگر (fnames == تہی)
	    جاؤ باہر_نکلو;

	i = 0;
	برائے (GSList *l = files; l != تہی; l = l->اگلا)
	{
	    GFile *file = l->ڈیٹا;
	    حرف *راستہ = g_file_get_راستہ(file);
	    اگر (راستہ != تہی)
		fnames[i++] = vim_strsave((char_u *)راستہ);
	    g_free(راستہ);
	}
	nfiles = i;

	اگر (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	ورنہ
	    vim_free(fnames);
	success = TRUE;
    }
    ورنہ اگر (G_قدر_HOLDS(قدر, G_TYPE_STRING))
    {
	ثابت حرف *متن = g_قدر_get_string(قدر);

	اگر (متن == تہی || *متن == NUL)
	    جاؤ باہر_نکلو;

	drop_read_متن(drop, (char_u *)متن);
	success = TRUE;
    }

باہر_نکلو:
    gdk_drop_اختتام(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_ڈیٹا_free(&drd->rd);
}

    ساکن خالی
drop_read_cb(GdkDrop *drop, GAsync_نتیجہ *نتیجہ, Drop_Read_ڈیٹا *drd)
{
    GInput_دھارا    *اندر_دھارا;
    ثابت حرف	    *m;

    اندر_دھارا = gdk_drop_read_اختتام(drop, نتیجہ, &m, تہی);
    اگر (اندر_دھارا == تہی || STRCMP(m, "text/html") != 0)
    {
	read_ڈیٹا_free(&drd->rd);
	gdk_drop_اختتام(drop, 0);
	واپس;
    }

    أكد(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_داخلہ_دھارا_read_async(اندر_دھارا, drd->rd.buf, READDATA_BUFSIZE,
	    G_ترجیح_بلند,
	    drd->rd.cancel, (GAsyncReadyCallback)read_ڈیٹا_داخلہ_cb, drd);
}

// GCancellable of current DND operation, else NULL.
ساکن GCancellable *dnd_cancel = تہی;
ساکن guint dnd_timeout_id = 0;

    ساکن gboolean
drop_timeout_cb(خالی *udata غیر_مستعمل)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    واپس G_ماخذ_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    ساکن gboolean
drop_cb(
	Gtk_Drop_ہدف_Async  *ہدف غیر_مستعمل,
	GdkDrop		    *drop,
	دوہرا		    x,
	دوہرا		    y,
	خالی		    *udata غیر_مستعمل)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    Drop_Read_ڈیٹا	*drd = تہی;

    اگر (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (Drop_Read_ڈیٹا *)read_ڈیٹا_new(READ_ڈیٹا_DROP, حجم(*drd));
	gdk_drop_read_قدر_async(drop, GDK_TYPE_FILE_LIST, G_ترجیح_بلند,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_قدر_cb, drd);
    }
    ورنہ اگر (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	ساکن ثابت حرف *m[] = {"text/html", تہی};

	drd = (Drop_Read_ڈیٹا *)read_ڈیٹا_new(READ_ڈیٹا_DROP_ڈیٹا, حجم(*drd));
	gdk_drop_read_async(drop, m, G_ترجیح_بلند,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    ورنہ اگر (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (Drop_Read_ڈیٹا *)read_ڈیٹا_new(READ_ڈیٹا_DROP, حجم(*drd));
	gdk_drop_read_قدر_async(drop, G_TYPE_STRING, G_ترجیح_بلند,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_قدر_cb, drd);
    }

    اگر (dnd_cancel != تہی)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    اگر (drd != تہی)
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
	dnd_timeout_id = timeout_جوڑو(3000, drop_timeout_cb, تہی);

	واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

    ساکن خالی
mainwin_destroy_cb(GObject *object غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
    gui.mainwin = تہی;
    gui.drawarea = تہی;
    اگر (!exiting)
	gui_shell_closed();
}

    ساکن gboolean
delete_event_cb(Gtk_کھڑکی *کھڑکی_2 غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
    gui_shell_closed();
    واپس TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    ساکن timeout_cb_type
داخلہ_timer_cb(gpointer ڈیٹا)
{
    صحیح *timed_برآمد = (صحیح *)ڈیٹا;

    *timed_برآمد = TRUE;
    واپس FALSE;	// don't call me again
}

    خالی
gui_mch_update(خالی)
{
    صحیح cnt = 0;

    جب (g_main_context_pending(تہی) && !vim_is_داخلہ_buf_بھرا()
								&& ++cnt < 100)
	g_main_context_iteration(تہی, TRUE);
}

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    ساکن timeout_cb_type
channel_poll_cb(gpointer ڈیٹا غیر_مستعمل)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    واپس TRUE; // Keep repeating
}
#اگر_ختم

    صحیح
gui_mch_wait_for_chars(طویل wtime)
{
    صحیح		focus;
    guint	timer;
    ساکن صحیح	timed_برآمد;
    صحیح		retval = FAIL;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    guint	channel_timer = 0;
#اگر_ختم

    timed_برآمد = FALSE;

    اگر (wtime >= 0)
	timer = timeout_جوڑو(wtime == 0 ? 1L : wtime,
						   داخلہ_timer_cb, &timed_برآمد);
    ورنہ
	timer = 0;

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    اگر (channel_any_keep_open())
	channel_timer = timeout_جوڑو(20, channel_poll_cb, تہی);
#اگر_ختم

    focus = gui.اندر_focus;

    کرو
    {
	// Stop or start blinking when focus changes
	اگر (gui.اندر_focus != focus)
	{
	    اگر (gui.اندر_focus)
		gui_mch_آغاز_blink();
	    ورنہ
		gui_mch_stop_blink(TRUE);
	    focus = gui.اندر_focus;
	}

#اگر_تعریف_شدہ پیغام_قطار
# ifdef FEAT_TIMERS
	did_جوڑو_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	اگر (did_جوڑو_timer)
	    جاؤ theend;
# endif
#اگر_ختم

	اگر (gui.mainwin == تہی)
	    جاؤ theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	اگر (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    جاؤ theend;
	}

	اگر (!داخلہ_دستیاب())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(تہی, TRUE);
	    --gtk4_main_loop_level;
	}

	اگر (داخلہ_دستیاب())
	{
	    retval = OK;
	    جاؤ theend;
	}
    } جب (wtime < 0 || !timed_برآمد);

    gui_mch_update();

theend:
    اگر (timer != 0 && !timed_برآمد)
	timeout_remove(timer);
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    اگر (channel_timer != 0)
	timeout_remove(channel_timer);
#اگر_ختم

    واپس retval;
}

    خالی
gui_mch_flush(خالی)
{
    // Ensure the offscreen surface content gets painted to the widget.
    اگر (gui.drawarea != تہی)
	gtk_widget_قطار_draw(gui.drawarea);
    اگر (gui.mainwin != تہی && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    خالی
gui_mch_beep(خالی)
{
    GdkDisplay *display;

    اگر (gui.mainwin != تہی && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	اگر (display != تہی)
	    gdk_display_beep(display);
    }
}

    خالی *
gui_mch_get_display(خالی)
{
    اگر (gui.mainwin != تہی && gtk_widget_get_display(gui.mainwin))
	واپس gtk_widget_get_display(gui.mainwin);
    واپس تہی;
}

    خالی
gui_mch_iconify(خالی)
{
    gtk_کھڑکی_minimize(GTK_کھڑکی(gui.mainwin));
}

    خالی
gui_mch_سیٹ_foreground(خالی)
{
    gtk_کھڑکی_present(GTK_کھڑکی(gui.mainwin));
}

    ساکن صحیح
query_اشارہ_گر_pos(صحیح *x, صحیح *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *اشارہ_گر;
    دوہرا	     sx, sy, nx, ny;
    graphene_نقطہ_t src, dst;

    اگر (gui.drawarea == تہی)
	واپس FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    اگر (native == تہی)
	واپس FALSE;
    surface = gtk_native_get_surface(native);
    اگر (surface == تہی)
	واپس FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    اگر (display == تہی)
	واپس FALSE;
    seat = gdk_display_get_default_seat(display);
    اگر (seat == تہی)
	واپس FALSE;
    اشارہ_گر = gdk_seat_get_اشارہ_گر(seat);
    اگر (اشارہ_گر == تہی)
	واپس FALSE;

    اگر (!gdk_surface_get_device_position(surface, اشارہ_گر, &sx, &sy, state))
	واپس FALSE;

    اگر (x != تہی && y != تہی)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (اعشاری)(sx - nx);
	src.y = (اعشاری)(sy - ny);
	اگر (!gtk_widget_compute_نقطہ(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    واپس FALSE;

	*x = (صحیح)dst.x;
	*y = (صحیح)dst.y;
    }
    واپس TRUE;
}

    خالی
gui_mch_getmouse(صحیح *x, صحیح *y)
{
    اگر (!query_اشارہ_گر_pos(x, y, تہی))
    {
	*x = 0;
	*y = 0;
    }
}

    خالی
gui_mch_setmouse(صحیح x غیر_مستعمل, صحیح y غیر_مستعمل)
{
    // GTK4/Wayland: cannot warp pointer
}

    خالی
gui_mch_mousehide(صحیح hide)
{
    اگر (gui.اشارہ_گر_hidden == hide)
	واپس;

    gui.اشارہ_گر_hidden = hide;
    اگر (gui.blank_اشارہ_گر != تہی)
    {
	اگر (hide)
	    gtk_widget_سیٹ_cursor(gui.drawarea, gui.blank_اشارہ_گر);
	ورنہ
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
	    mch_سیٹ_mouse_shape(آخری_shape);
#ورنہ
	    gtk_widget_سیٹ_cursor(gui.drawarea, تہی);
#اگر_ختم
    }
}

    صحیح
gui_mch_haskey(char_u *name)
{
    صحیح i;

    برائے (i = 0; special_کلیدیں[i].کلید_sym != 0; i++)
	اگر (name[0] == special_کلیدیں[i].code0
		&& name[1] == special_کلیدیں[i].code1)
	    واپس OK;
    واپس FAIL;
}

    خالی
gui_mch_forked(خالی)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    ساکن خالی
gui_gtk4_update_scrollbar_سائز(خالی)
{
    win_T	*wp;
    صحیح		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_بایاں].id;

	اگر (sbar == تہی || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_بایاں]
		    && wp->w_scrollbars[SBAR_دایاں].id != تہی))
	    sbar = wp->w_scrollbars[SBAR_دایاں].id;

	اگر (sbar != تہی && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    صحیح		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_سائز(sbar, &min, &nat);
	    sw = MAX(min.چوڑائی, nat.چوڑائی);
	    اگر (sw > 0)
	    {
		w = sw;
		توڑو;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    اگر (sbar != تہی && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	صحیح		    sh;

	gtk_widget_get_preferred_سائز(sbar, &min, &nat);
	sh = MAX(min.اونچائی, nat.اونچائی);

	اگر (sh > 0)
	    h = sh;
    }

    اگر (w != -1)
	gui.scrollbar_چوڑائی = w;
    اگر (h != -1)
	gui.scrollbar_اونچائی = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    خالی
gui_mch_enable_scrollbar(scrollbar_T *sb, صحیح flag)
{
    اگر (sb->id != تہی)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_سیٹ_visible(sb->id, flag);

	اگر (!was_visible && flag)
	    gui_gtk4_update_scrollbar_سائز();
    }
}

#اگر defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    خالی
gui_mch_menu_grey(vimmenu_T *menu, صحیح grey)
{
    اگر (menu->id == تہی)
	واپس;
    gtk_widget_سیٹ_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    خالی
gui_mch_menu_hidden(vimmenu_T *menu, صحیح hidden)
{
    اگر (menu->id == تہی)
	واپس;
    gtk_widget_سیٹ_visible(menu->id, !hidden);
    gui_mch_update();
}

    خالی
gui_mch_draw_menubar(خالی)
{
    // Just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#اگر_ختم

/*
 * ============================================================
 * Tabline
 * ============================================================
 */

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    خالی
gui_mch_show_tabline(صحیح showit)
{
    اگر (gui.tabline == تہی)
	واپس;
    gtk_widget_سیٹ_visible(gui.tabline, showit);
    gui_mch_update();
}

    صحیح
gui_mch_showing_tabline(خالی)
{
    واپس gui.tabline != تہی && gtk_widget_get_visible(gui.tabline);
}

ساکن صحیح ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
ساکن صحیح tabpage_hover = 0;

    ساکن خالی
tabpage_enter_cb(
	GtkEventController  *controller غیر_مستعمل,
	دوہرا		    x غیر_مستعمل,
	دوہرا		    y غیر_مستعمل,
	خالی		    *udata)
{
    tabpage_hover = GPOINTER_مقصد_INT(udata);
}

    ساکن خالی
tabline_enter_cb(
	GtkEventController  *controller غیر_مستعمل,
	دوہرا		    x غیر_مستعمل,
	دوہرا		    y غیر_مستعمل,
	خالی		    *udata غیر_مستعمل)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    ساکن خالی
tabline_جوڑو_css(خالی)
{
    GtkCssProvider  *provider;
    ساکن ثابت حرف *css =
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
# اگر GTK_CHECK_نسخہ(4, 12, 0)
    gtk_css_provider_بھرو_from_string(provider, css);
# ورنہ
    gtk_css_provider_بھرو_from_ڈیٹا(provider, css, -1);
# endif
    gtk_style_context_جوڑو_provider_for_display(
	    gtk_widget_get_display(gui.tabline), GTK_STYLE_PROVIDER(provider),
	    GTK_STYLE_PROVIDER_ترجیح_APPLICATION);
    g_object_unref(provider);
}

/*
 * Handle clicking the close button of a tab.
 */
    ساکن خالی
tab_close_clicked_cb(GtkButton *button غیر_مستعمل, gpointer ڈیٹا)
{
    صحیح idx = gtk_notebook_صفحہ_عدد(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(ڈیٹا));

    اگر (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    خالی
gui_mch_update_tabline(خالی)
{
    GtkWidget	*صفحہ;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    صحیح		nr = 0;
    صحیح		tab_عدد;
    صحیح		curtabidx = 0;
    char_u	*labeltext;

    اگر (gui.tabline == تہی)
	واپس;

    ignore_tabline_evt = TRUE;

    برائے (tp = پہلا_tabpage; tp != تہی; tp = tp->tp_اگلا, ++nr)
    {
	Gtk_Notebook_صفحہ	    *صفحہ_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	اگر (tp == curtab)
	    curtabidx = nr;

	tab_عدد = nr + 1;

	صفحہ = gtk_notebook_get_nth_صفحہ(GTK_NOTEBOOK(gui.tabline), nr);
	اگر (صفحہ == تہی)
	{
	    GtkWidget	*spacer;
	    GtkWidget	*close_button;

	    صفحہ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_widget_سیٹ_visible(صفحہ, TRUE);
	    event_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_سیٹ_visible(event_box, TRUE);

	    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_جوڑو_css_class(spacer, "vim-tab-spacer");
	    gtk_box_append(GTK_BOX(event_box), spacer);

	    label = gtk_label_new("-Empty-");
	    gtk_widget_سیٹ_hexpand(label, TRUE);
	    gtk_label_سیٹ_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_انتہا);
	    gtk_box_append(GTK_BOX(event_box), label);
	    gtk_widget_سیٹ_visible(label, TRUE);
	    g_object_سیٹ_ڈیٹا(G_OBJECT(event_box), "label", label);

	    close_button = gtk_button_new_from_icon_name("window-close-symbolic");
	    gtk_widget_جوڑو_css_class(close_button, "flat");
	    gtk_widget_جوڑو_css_class(close_button, "circular");
	    gtk_widget_سیٹ_valign(close_button, GTK_ALIGN_CENTER);
	    gtk_widget_سیٹ_focus_on_click(close_button, FALSE);
	    gtk_widget_سیٹ_tooltip_متن(close_button, _("Close Tab"));
	    g_signal_connect(close_button, "clicked",
		    G_CALLBACK(tab_close_clicked_cb), صفحہ);
	    gtk_box_append(GTK_BOX(event_box), close_button);

	    gtk_notebook_داخل_کرو_صفحہ(GTK_NOTEBOOK(gui.tabline),
		    صفحہ, event_box, nr);
	    gtk_notebook_سیٹ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    صفحہ, TRUE);
	    g_object_سیٹ(gtk_notebook_get_صفحہ(GTK_NOTEBOOK(gui.tabline), صفحہ),
		    "tab-expand", TRUE, تہی);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	صفحہ_widget = gtk_notebook_get_صفحہ(GTK_NOTEBOOK(gui.tabline), صفحہ);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_مقصد_اشارہ_گر(tab_عدد));
	gtk_event_controller_سیٹ_اشاعت_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(صفحہ_widget, "tab", &tab_widget, تہی);
	gtk_widget_جوڑو_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), صفحہ);
	label = g_object_get_ڈیٹا(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_مقصد_UTF8(NameBuff);
	اگر (label != تہی && GTK_IS_LABEL(label))
	    gtk_label_سیٹ_متن(GTK_LABEL(label), (ثابت حرف *)labeltext);
	CONVERT_مقصد_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_مقصد_UTF8(NameBuff);
	gtk_widget_سیٹ_tooltip_متن(event_box, (ثابت gchar *)labeltext);
	CONVERT_مقصد_UTF8_FREE(labeltext);
    }

    جب (gtk_notebook_get_nth_صفحہ(GTK_NOTEBOOK(gui.tabline), nr) != تہی)
	gtk_notebook_remove_صفحہ(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    اگر (nr == 1)
	gtk_widget_جوڑو_css_class(gui.tabline, "single-tab");
    ورنہ
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    اگر (gtk_notebook_get_current_صفحہ(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_سیٹ_current_صفحہ(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    خالی
gui_mch_سیٹ_curtab(صحیح nr)
{
    اگر (gui.tabline != تہی)
	gtk_notebook_سیٹ_current_صفحہ(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    ساکن خالی
on_select_tab(
	GtkNotebook	*notebook غیر_مستعمل,
	gpointer	*صفحہ غیر_مستعمل,
	gint		idx,
	gpointer	ڈیٹا غیر_مستعمل)
{
    اگر (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    ساکن خالی
on_tab_reordered(
	GtkNotebook	*notebook غیر_مستعمل,
	gpointer	*صفحہ غیر_مستعمل,
	gint		idx,
	gpointer	ڈیٹا غیر_مستعمل)
{
    اگر (ignore_tabline_evt)
	واپس;

    اگر ((tabpage_اشاریہ(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    ورنہ
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ساکن خالی
tabline_menu_event_cb(Vim_Menu_شے *شے غیر_مستعمل, Vim_Menu_شے_Event event, خالی *udata)
{
    اگر (event == VIM_MENU_شے_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_مقصد_INT(udata));
}

    ساکن خالی
جوڑو_tabline_menu_شے(VimMenu *menu, ثابت حرف *name, صحیح resp)
{
    Vim_Menu_شے *شے = VIM_MENU_شے(vim_menu_شے_new(name,
		tabline_menu_event_cb, GINT_مقصد_اشارہ_گر(resp)));

    vim_menu_داخل_کرو_شے(menu, شے, -1);
}

/*
 * Create a menu for the tab line.
 */
    ساکن VimMenu *
create_tabline_popup_menu(خالی)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    جوڑو_tabline_menu_شے(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    جوڑو_tabline_menu_شے(menu, _("New Tab"), TABLINE_MENU_NEW);
    جوڑو_tabline_menu_شے(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    واپس menu;
}

    ساکن خالی
tabline_menu_press_event(
	GtkGestureClick *gesture,
	صحیح		n_press غیر_مستعمل,
	دوہرا		x,
	دوہرا		y,
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
	rect.چوڑائی = 1;
	rect.اونچائی = 1;

	gtk_popover_سیٹ_pointing_مقصد(GTK_POPOVER(popover), &rect);
	gtk_popover_popup(GTK_POPOVER(popover));
    }
    ورنہ اگر (but == GDK_BUTTON_PRIMARY)
    {
	اگر (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    ورنہ اگر (but == GDK_BUTTON_درمیان)
    {
	اگر (tabpage_hover != 0)
	    // Middle mouse click on tabpage label closes that tab.
	    send_tabline_menu_event(tabpage_hover, TABLINE_MENU_CLOSE);
    }
}
#اگر_ختم

/*
 * ============================================================
 * Sign support
 * ============================================================
 */

#اگر defined(FEAT_SIGN_ICONS)
# define SIGN_چوڑائی  (2 * gui.char_چوڑائی)
# define SIGN_اونچائی (gui.char_اونچائی)

    خالی
gui_mch_drawsign(صحیح صف, صحیح ستون, صحیح typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    اگر (sign == تہی)
	واپس;

    vim_draw_area_جوڑو_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    صف, ستون, SIGN_چوڑائی, SIGN_اونچائی);

    gtk_widget_قطار_draw(gui.drawarea);
}

    خالی *
gui_mch_رجسٹر_sign(char_u *signfile)
{
    اگر (signfile[0] != NUL && signfile[0] != '-' && gui.اندر_use)
    {
	GError	    *error = تہی;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ثابت حرف *)signfile,
							    &error);
	اگر (sign != تہی)
	    واپس sign;

	semsg("E255: %s", error->پیغام_2);
	g_error_free(error);
    }
    واپس تہی;
}

    خالی
gui_mch_destroy_sign(خالی *sign)
{
    اگر (sign != تہی)
	g_object_unref(sign);
}
#اگر_ختم

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
gui_gtk_draw_string(صحیح صف, صحیح ستون, char_u *s, صحیح len, صحیح flags)
{
    صحیح cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    صف, ستون, s, len, flags);

    اگر (gui.drawarea != تہی)
	gtk_widget_قطار_draw(gui.drawarea);

    واپس cells;
}

    صحیح
gui_get_x11_windis(کھڑکی *win غیر_مستعمل, Display **dis غیر_مستعمل)
{
    // GTK4: not applicable
    واپس FAIL;
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
    ساکن char_u *
translate_mnemonic_لیبل(char_u *name, صحیح use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    صحیح	    n_underscores = 0;

    name = CONVERT_مقصد_UTF8(name);
    اگر (name == تہی)
	واپس تہی;

    برائے (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	اگر (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    اگر (buf != تہی)
    {
	pdest = buf;
	برائے (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    اگر (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ورنہ اگر (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ورنہ اگر (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ورنہ اگر (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_مقصد_UTF8_FREE(name);
    واپس buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    خالی
gui_gtk_سیٹ_mnemonics(صحیح enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	اگر (menu->id == تہی)
	    جاری;

	name = translate_mnemonic_لیبل(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	اگر (VIM_IS_MENU_BAR_شے(menu->id))
	    vim_menu_bar_شے_سیٹ_متن(VIM_MENU_BAR_شے(menu->id),
		    (ثابت حرف *)name);
	vim_free(name);
    }
}

    ساکن خالی
popupmenu_closed_cb(GtkWidget *popover, خالی *udata غیر_مستعمل)
{
    gtk_widget_unparent(popover);
    اگر (gui.drawarea != تہی)
	gtk_widget_قطار_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    ساکن خالی
gui_gtk_popup_at(vimmenu_T *menu, صحیح x, صحیح y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    اگر (menu == تہی || menu->submenu_id == تہی)
	واپس;

    popover = vim_menu_copy(VIM_MENU(menu->submenu_id));
    gtk_widget_سیٹ_والد(popover, gui.drawarea);

    rect.x = x;
    rect.y = y;
    rect.چوڑائی = rect.اونچائی = 1;

    // Make sure popover aligns down-and-to-the-right of the pointer.
    gtk_popover_سیٹ_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_widget_سیٹ_halign(popover, GTK_ALIGN_آغاز);
    gtk_popover_سیٹ_pointing_مقصد(GTK_POPOVER(popover), &rect);

    g_signal_connect(GTK_POPOVER(popover), "closed",
	    G_CALLBACK(popupmenu_closed_cb), تہی);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    خالی
gui_make_popup(char_u *راستہ_name, صحیح mouse_pos)
{
    vimmenu_T	*menu;
    صحیح		x, y;

    menu = gui_ڈھونڈو_menu(راستہ_name);
    اگر (menu == تہی || menu->submenu_id == تہی)
	واپس;

    اگر (mouse_pos)
	gui_mch_getmouse(&x, &y);
    ورنہ
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#اگر_ختم // FEAT_MENU

    صحیح
get_menu_tool_چوڑائی(خالی)
{
    واپس 0;
}

    صحیح
get_menu_tool_اونچائی(خالی)
{
    GtkWidget *widgets[] = {
#اگر_تعریف_شدہ FEAT_MENU
	gui.menubar,
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TOOLBAR
	gui.toolbar,
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	gui.tabline
#اگر_ختم
    };

    صحیح اونچائی = 0;

    برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	صحیح		h;

	اگر (widgets[i] == تہی || !gtk_widget_get_visible(widgets[i]))
	    جاری;

	h = gtk_widget_get_اونچائی(widgets[i]);

	اگر (h == 0)
	{
	    // Allocation hasn't been updated yet (widget just became visible).
	    // Query the preferred height so the caller gets a valid value
	    // before the layout pass runs.  Use the maximum of minimum and
	    // natural height: GTK may allocate min_h even when natural_h is
	    // smaller (e.g. GtkNotebook tab bar has min_h > natural_h due to
	    // CSS).
	    gtk_widget_get_preferred_سائز(widgets[i], &min, &nat);
	    اونچائی += MAX(min.اونچائی, nat.اونچائی);
	}
	ورنہ
	    اونچائی += h;
    }
    واپس اونچائی;
}

/*
 * Update selection targets for regular and primary selections.
 */
    خالی
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    اگر (cbd == &clip_plus)
	gdk_content_provider_content_بدلا(gui.regular_provider);
    ورنہ
	gdk_content_provider_content_بدلا(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    ساکن GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    اگر (gui.mainwin == تہی)
	واپس تہی;

    display = gtk_widget_get_display(gui.mainwin);
    اگر (display == تہی)
	واپس تہی;

    اگر (cbd == &clip_plus)
    {
	اگر (provider != تہی)
	    *provider = gui.regular_provider;
	واپس gdk_display_get_clipboard(display);
    }
    ورنہ
    {
	اگر (provider != تہی)
	    *provider = gui.primary_provider;
	واپس gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    ساکن خالی
clip_read_cb(GdkClipboard *cb, GAsync_نتیجہ *نتیجہ, Clip_Read_ڈیٹا *crd)
{
    GInput_دھارا    *اندر_دھارا;
    ثابت حرف	    *mime_type;

    اندر_دھارا = gdk_clipboard_read_اختتام(cb, نتیجہ, &mime_type, تہی);
    اگر (اندر_دھارا == تہی)
    {
	read_ڈیٹا_free(&crd->rd);
	واپس;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_داخلہ_دھارا_read_async(اندر_دھارا, crd->rd.buf, READDATA_BUFSIZE,
	    G_ترجیح_بلند,
	    crd->rd.cancel, (GAsyncReadyCallback)read_ڈیٹا_داخلہ_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    خالی
clip_mch_request_selection(Clipboard_T *cbd)
{
    ساکن ثابت حرف *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	تہی
    };

    GdkClipboard *clipboard;
    Clip_Read_ڈیٹا *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, تہی);
    اگر (clipboard == تہی)
	واپس;

    crd = (Clip_Read_ڈیٹا *)read_ڈیٹا_new(READ_ڈیٹا_CLIP, حجم(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_ترجیح_بلند, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t آغاز;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	آغاز = وقت(تہی);
	جب (!g_cancellable_is_cancelled(cancel)
		&& وقت(تہی) < آغاز + 3)
	    g_main_context_iteration(تہی, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

ساکن صحیح اندر_clipboard_سیٹ = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    خالی
clip_mch_سیٹ_selection(Clipboard_T *cbd غیر_مستعمل)
{
}

    ساکن خالی
clipboard_بدلا_cb(GdkClipboard *clipboard, gpointer user_ڈیٹا)
{
    Clipboard_T *cbd = (Clipboard_T *)user_ڈیٹا;

    اگر (اندر_clipboard_سیٹ)
	واپس;
    اگر (gdk_clipboard_is_local(clipboard))
	واپس;
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

    اگر (cb == تہی)
	واپس FAIL;

    اندر_clipboard_سیٹ = TRUE;
    ret = gdk_clipboard_سیٹ_content(cb, cp);
    اندر_clipboard_سیٹ = FALSE;

    واپس ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    خالی
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, تہی);
    اگر (clipboard == تہی)
	واپس;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    اگر (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_سیٹ_content(clipboard, تہی);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(خالی)
{
    واپس gtk4_main_loop_level;
}

    خالی
gtk_main_quit(خالی)
{
    gtk4_main_loop_quit = TRUE;
}

#اگر defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
ساکن ثابت حرف *mshape_css_نام[] =
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

    خالی
mch_سیٹ_mouse_shape(صحیح shape)
{
    GdkCursor	*c;
    ثابت حرف	*css_name = "default";

    اگر (gui.drawarea == تہی)
	واپس;

    اگر (shape == MSHAPE_HIDE || gui.اشارہ_گر_hidden)
	gtk_widget_سیٹ_cursor(gui.drawarea, gui.blank_اشارہ_گر);
    ورنہ
    {
	اگر (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	ورنہ اگر (shape < (صحیح)ARRAY_لمبائی(mshape_css_نام))
	    css_name = mshape_css_نام[shape];
	ورنہ
	    واپس;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, تہی);
	gtk_widget_سیٹ_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    اگر (shape != MSHAPE_HIDE)
	آخری_shape = shape;
}

#اگر_ختم // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



ساکن صحیح آخری_متن_area_w = 0;
ساکن صحیح آخری_متن_area_h = 0;

/*
 * ============================================================
 * Menu functions
 * ============================================================
 */

#اگر_تعریف_شدہ FEAT_MENU

# ifdef FEAT_TOOLBAR
/*
 * Icon name table for toolbar buttons.
 * Must match toolbar_names[] in menu.c.
 */
ساکن ثابت حرف * ثابت toolbar_icon_نام[] =
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

    ساکن خالی
toolbar_button_clicked_cb(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا)
{
    gui_menu_cb((vimmenu_T *)ڈیٹا);
}

    ساکن GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = تہی;

    // Try specified icon file first
    اگر (menu->iconfile != تہی)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	اگر (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (ثابت حرف *)buf, تہی);

	    اگر (texture != تہی)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_سیٹ_سائز_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    اگر (image == تہی)
    {
	ثابت حرف *icon_name = "image-missing";
	صحیح n = (صحیح)ARRAY_لمبائی(toolbar_icon_نام);

	اگر (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_نام[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    واپس image;
}
# endif

    ساکن خالی
menu_button_clicked_cb(
	Vim_Menu_شے	    *شے غیر_مستعمل,
	Vim_Menu_شے_Event    event,
	vimmenu_T	    *menu)
{
    اگر (event == VIM_MENU_شے_CLICKED)
	gui_menu_cb(menu);
    ورنہ اگر (event == VIM_MENU_شے_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	ساکن gboolean did_پیغام = FALSE;

	اگر (State & منوال_CMDLINE)
	    واپس;

	tooltip = CONVERT_مقصد_UTF8(menu->لڑیاں[MENU_اشاریہ_TIP]);
	اگر (tooltip != تہی && utf_valid_string(tooltip, تہی))
	{
	    پیغام((حرف *)tooltip);
	    did_پیغام = TRUE;
	    setcursor();
	    برآمد_flush_cursor(TRUE, FALSE);
	}
	ورنہ اگر (did_پیغام)
	{
	    پیغام("");
	    did_پیغام = FALSE;
	    setcursor();
	    برآمد_flush_cursor(TRUE, FALSE);
	}
	CONVERT_مقصد_UTF8_FREE(tooltip);
    }
}

    خالی
gui_mch_جوڑو_menu(vimmenu_T *menu, صحیح idx)
{
    vimmenu_T	*والد;
    GtkWidget	*والد_widget;
    gboolean	use_mnemonic;
    char_u	*متن;

    اگر (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_سیٹ_والد(menu->submenu_id, gui.drawarea);
	واپس;
    }

    والد = menu->والد;

    اگر ((والد != تہی && والد->submenu_id == تہی)
	    || !menu_is_menubar(menu->name))
	واپس;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = والد != تہی || p_wak[0] != 'n';
    متن = translate_mnemonic_لیبل(menu->name, use_mnemonic);

    اگر (والد != تہی)
    {
	والد_widget = والد->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_شے_new(
		    (ثابت حرف *)متن, تہی, تہی));

	vim_menu_شے_سیٹ_submenu(VIM_MENU_شے(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_داخل_کرو_شے(VIM_MENU(والد_widget),
		VIM_MENU_شے(menu->id), idx);
    }
    ورنہ
    {
	والد_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_شے_new(
		    (ثابت حرف *)متن, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_داخل_کرو_شے(VIM_MENU_BAR(والد_widget),
		VIM_MENU_BAR_شے(menu->id), idx);
    }

    vim_free(متن);
}

    خالی
gui_mch_جوڑو_menu_شے(vimmenu_T *menu, صحیح idx)
{
    vimmenu_T	*والد = menu->والد;

# ifdef FEAT_TOOLBAR
    اگر (والد != تہی && menu_is_toolbar(والد->name))
    {
	اگر (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_داخل_کرو_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	ورنہ
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*متن;
	    char_u	*tooltip;

	    متن    = CONVERT_مقصد_UTF8(menu->dname);
	    tooltip = CONVERT_مقصد_UTF8(menu->لڑیاں[MENU_اشاریہ_TIP]);
	    اگر (tooltip != تہی && !utf_valid_string(tooltip, تہی))
		CONVERT_مقصد_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_داخل_کرو_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ثابت حرف *)متن, idx);
	    gtk_widget_سیٹ_tooltip_متن(btn, (ثابت gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_مقصد_UTF8_FREE(متن);
	    CONVERT_مقصد_UTF8_FREE(tooltip);
	}
	واپس;
    }
# endif

    // Menu items (non-toolbar)
    اگر (والد == تہی || والد->submenu_id == تہی)
	واپس;

    اگر (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_داخل_کرو_separator(
		VIM_MENU(والد->submenu_id), idx));
    }
    ورنہ
    {
	char_u	    *متن;
	char_u	    *accel_متن = تہی;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	متن = translate_mnemonic_لیبل(menu->name, use_mnemonic);

	اگر (menu->actext != تہی && menu->actext[0] != NUL)
	    accel_متن = CONVERT_مقصد_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_شے_new((ثابت حرف *)متن,
		    (Vim_Menu_شے_Func)menu_button_clicked_cb, menu));

	اگر (accel_متن != تہی)
	    vim_menu_شے_سیٹ_accel(VIM_MENU_شے(menu->id),
		    (ثابت حرف *)accel_متن);

	vim_menu_داخل_کرو_شے(VIM_MENU(والد->submenu_id),
		VIM_MENU_شے(menu->id), idx);

	vim_free(متن);
	CONVERT_مقصد_UTF8_FREE(accel_متن);
    }
}

    خالی
gui_mch_toggle_tearoffs(صحیح enable غیر_مستعمل)
{
    // GTK4: tearoff menus don't exist.
}

    خالی
gui_mch_menu_سیٹ_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    اگر (menu->id == تہی)
	واپس;

    tooltip = CONVERT_مقصد_UTF8(menu->لڑیاں[MENU_اشاریہ_TIP]);
    اگر (tooltip != تہی && utf_valid_string(tooltip, تہی))
	gtk_widget_سیٹ_tooltip_متن(menu->id, (ثابت حرف *)tooltip);
    CONVERT_مقصد_UTF8_FREE(tooltip);
}

    خالی
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    اگر (menu->والد != تہی && menu_is_toolbar(menu->والد->name))
    {
	اگر (menu->id != تہی)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = تہی;
	}
	واپس;
    }

    // For popup menus, unparent the menu as well
    اگر (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	اگر (menu->submenu_id != تہی)
	    gtk_widget_unparent(menu->submenu_id);
    }
    ورنہ اگر (menu->والد == تہی && menu->id != تہی)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    ورنہ اگر (menu->id != تہی)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->والد->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    خالی
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    صحیح x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    ساکن خالی
show_menubar_popover(خالی)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    اگر (gui.menubar == تہی || gui.drawarea == تہی)
	واپس;

    اگر (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), تہی);
	واپس;
    }

    // Copy and convert the menubar into a menu popover
    menu = vim_menu_bar_مقصد_menu(VIM_MENU_BAR(gui.menubar));

    gtk_widget_سیٹ_والد(menu, gui.drawarea);
    gtk_popover_سیٹ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    rect.x = 0;
    rect.y = 0;
    rect.چوڑائی = 1;
    rect.اونچائی = 1;
    gtk_popover_سیٹ_pointing_مقصد(GTK_POPOVER(menu), &rect);
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), تہی);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#اگر_ختم // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    خالی
gui_mch_سیٹ_scrollbar_thumb(scrollbar_T *sb, طویل val, طویل سائز, طویل max)
{
    GtkAdjustment *adj;

    اگر (sb->id == تہی)
	واپس;
    اگر (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	واپس;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_سیٹ_زیریں(adj, 0.0);
    gtk_adjustment_سیٹ_بالائی(adj, (gdouble)max + 1);
    gtk_adjustment_سیٹ_قدر(adj, (gdouble)val);
    gtk_adjustment_سیٹ_قدم_increment(adj, 1.0);
    gtk_adjustment_سیٹ_صفحہ_increment(adj, (gdouble)(سائز > 2 ? سائز - 2 : 1));
    gtk_adjustment_سیٹ_صفحہ_سائز(adj, (gdouble)سائز);
}

    خالی
gui_mch_سیٹ_scrollbar_pos(scrollbar_T *sb, صحیح x, صحیح y, صحیح w, صحیح h)
{
    اگر (sb->id != تہی)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    صحیح
gui_mch_get_scrollbar_xpadding(خالی)
{
    صحیح formwin_w = gtk_widget_get_چوڑائی(gui.formwin);
    صحیح sbar_w = 0;
    صحیح xpad;

    اگر (gui.which_scrollbars[SBAR_بایاں])
	sbar_w += gui.scrollbar_چوڑائی;
    اگر (gui.which_scrollbars[SBAR_دایاں])
	sbar_w += gui.scrollbar_چوڑائی;

    xpad = formwin_w - آخری_متن_area_w - sbar_w;
    واپس (xpad < 0) ? 0 : xpad;
}

    صحیح
gui_mch_get_scrollbar_ypadding(خالی)
{
    صحیح formwin_h = gtk_widget_get_اونچائی(gui.formwin);
    صحیح ypad;

    ypad = formwin_h - آخری_متن_area_h;
    اگر (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_اونچائی;

    واپس (ypad < 0) ? 0 : ypad;
}

    ساکن خالی
adjustment_قدر_بدلا(GtkAdjustment *adj, gpointer ڈیٹا غیر_مستعمل)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_ڈیٹا(G_OBJECT(adj), "vim-sb");
    طویل قدر = (طویل)gtk_adjustment_get_قدر(adj);

    اگر (sb != تہی)
	gui_drag_scrollbar(sb, قدر, FALSE);
}

    خالی
gui_mch_create_scrollbar(scrollbar_T *sb, صحیح orient)
{
    GtkAdjustment *adj;
    اگر (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, تہی);
    ورنہ
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, تہی);

    gtk_widget_جوڑو_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_سیٹ_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_سیٹ_ڈیٹا(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_قدر_بدلا), تہی);
}

    خالی
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    اگر (sb->id != تہی)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = تہی;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    خالی
gui_mch_سیٹ_متن_area_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    اگر (w <= 0 || h <= 0)
	واپس;
    آخری_متن_area_w = w;
    آخری_متن_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    خالی
gui_gtk4_calculate_bleed(صحیح چوڑائی, صحیح اونچائی)
{
    صحیح old_دایاں = gui.bleed_دایاں;
    صحیح old_bot = gui.bleed_bot;

    gui.bleed_دایاں = چوڑائی - آخری_متن_area_w;
    gui.bleed_bot = اونچائی - آخری_متن_area_h;

    اگر (gui.which_scrollbars[SBAR_بایاں])
	gui.bleed_دایاں -= gui.scrollbar_چوڑائی;
    اگر (gui.which_scrollbars[SBAR_دایاں])
	gui.bleed_دایاں -= gui.scrollbar_چوڑائی;
    اگر (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_اونچائی;

    // Not sure if this can happen, but be safe...
    اگر (gui.bleed_دایاں < 0)
	gui.bleed_دایاں = 0;
    اگر (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    اگر (old_دایاں != gui.bleed_دایاں || old_bot != gui.bleed_bot)
	gtk_widget_قطار_draw(GTK_WIDGET(gui.drawarea));
}

/*
 * ============================================================
 * Browse dialogs
 * ============================================================
 */

/*
 * Blocking helper: run a GtkFileDialog and wait for result.
 */
قسمتعریف ساخت {
    GFile	*نتیجہ;
    gboolean	مکمل;
} File_Dialog_ڈیٹا;

    ساکن خالی
file_dialog_open_cb(GObject *ماخذ, GAsync_نتیجہ *res, gpointer ڈیٹا)
{
    File_Dialog_ڈیٹا *fdd = (File_Dialog_ڈیٹا *)ڈیٹا;
    fdd->نتیجہ = gtk_file_dialog_open_اختتام(
		    GTK_FILE_DIALOG(ماخذ), res, تہی);
    fdd->مکمل = TRUE;
}

    ساکن خالی
file_dialog_save_cb(GObject *ماخذ, GAsync_نتیجہ *res, gpointer ڈیٹا)
{
    File_Dialog_ڈیٹا *fdd = (File_Dialog_ڈیٹا *)ڈیٹا;
    fdd->نتیجہ = gtk_file_dialog_save_اختتام(
		    GTK_FILE_DIALOG(ماخذ), res, تہی);
    fdd->مکمل = TRUE;
}

    ساکن خالی
file_dialog_folder_cb(GObject *ماخذ, GAsync_نتیجہ *res, gpointer ڈیٹا)
{
    File_Dialog_ڈیٹا *fdd = (File_Dialog_ڈیٹا *)ڈیٹا;
    fdd->نتیجہ = gtk_file_dialog_select_folder_اختتام(
		    GTK_FILE_DIALOG(ماخذ), res, تہی);
    fdd->مکمل = TRUE;
}

    char_u *
gui_mch_browse(صحیح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext غیر_مستعمل,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    File_Dialog_ڈیٹا  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *نتیجہ = تہی;
    GListStore	    *filters;

    title = CONVERT_مقصد_UTF8(title);

    اگر (initdir == تہی || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ورنہ اگر (vim_بھرا_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    جوڑو_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_سیٹ_modal(dlg, TRUE);
    اگر (title != تہی)
	gtk_file_dialog_سیٹ_title(dlg, (ثابت حرف *)title);

    {
	GFile *dir = g_file_new_for_راستہ((ثابت حرف *)dirbuf);
	gtk_file_dialog_سیٹ_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    اگر (filter != تہی && *filter != NUL)
    {
	صحیح		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	اگر (patt != تہی)
	{
	    جب (p != تہی && *p != NUL)
	    {
		اگر (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    اگر (*p == '\t')
			gtk_file_filter_سیٹ_name(gfilter, (gchar *)patt);
		    ورنہ
		    {
			gtk_file_filter_جوڑو_نمونہ(gfilter, (gchar *)patt);
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
		ورنہ
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    gtk_file_dialog_سیٹ_filters(dlg, G_LIST_MODEL(filters));
    g_object_unref(filters);

    اگر (saving && dflt != تہی && *dflt != NUL)
	gtk_file_dialog_سیٹ_initial_name(dlg, (ثابت حرف *)dflt);

    fdd.نتیجہ = تہی;
    fdd.مکمل = FALSE;

    اگر (saving)
	gtk_file_dialog_save(dlg, GTK_کھڑکی(gui.mainwin), تہی,
		file_dialog_save_cb, &fdd);
    ورنہ
	gtk_file_dialog_open(dlg, GTK_کھڑکی(gui.mainwin), تہی,
		file_dialog_open_cb, &fdd);

    جب (!fdd.مکمل)
	g_main_context_iteration(تہی, TRUE);

    اگر (fdd.نتیجہ != تہی)
    {
	حرف *راستہ = g_file_get_راستہ(fdd.نتیجہ);
	اگر (راستہ != تہی)
	{
	    نتیجہ = vim_strsave((char_u *)راستہ);
	    g_free(راستہ);
	}
	g_object_unref(fdd.نتیجہ);
    }

    g_object_unref(dlg);
    CONVERT_مقصد_UTF8_FREE(title);

    واپس نتیجہ;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    File_Dialog_ڈیٹا	fdd;
    char_u		*نتیجہ = تہی;

    title = CONVERT_مقصد_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_سیٹ_modal(dlg, TRUE);
    اگر (title != تہی)
	gtk_file_dialog_سیٹ_title(dlg, (ثابت حرف *)title);

    اگر (initdir != تہی && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_راستہ((ثابت حرف *)initdir);
	gtk_file_dialog_سیٹ_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.نتیجہ = تہی;
    fdd.مکمل = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_کھڑکی(gui.mainwin), تہی,
	    file_dialog_folder_cb, &fdd);

    جب (!fdd.مکمل)
	g_main_context_iteration(تہی, TRUE);

    اگر (fdd.نتیجہ != تہی)
    {
	حرف *راستہ = g_file_get_راستہ(fdd.نتیجہ);
	اگر (راستہ != تہی)
	{
	    نتیجہ = vim_strsave((char_u *)راستہ);
	    g_free(راستہ);
	}
	g_object_unref(fdd.نتیجہ);
    }

    g_object_unref(dlg);
    CONVERT_مقصد_UTF8_FREE(title);

    واپس نتیجہ;
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
    ساکن حرف **
بانٹو_button_string(char_u *button_string, صحیح *n_buttons, حرف **tofree)
{
    حرف	    **array;
    char_u	    *p;
    غیرعلامتی صحیح    گنتی = 1;

    button_string = (char_u *)g_strdup((ثابت حرف *)button_string);

    برائے (p = button_string; *p != NUL; ++p)
	اگر (*p == DLG_BUTTON_SEP)
	    ++گنتی;

    array = g_malloc_n(گنتی, حجم(حرف *));
    گنتی = 0;

    اگر (array != تہی)
    {
	array[گنتی++] = (حرف *)button_string;
	برائے (p = button_string; *p != NUL; )
	{
	    اگر (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[گنتی++] = (حرف *)p;
	    }
	    ورنہ اگر (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ورنہ
		MB_PTR_ADV(p);
	}
    }

    *tofree = (حرف *)button_string;
    *n_buttons = گنتی;
    واپس array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    ساکن ثابت حرف *
dialog_type_مقصد_icon(صحیح type)
{
    انتخاب (type)
    {
	حالت VIM_ERROR:
	    واپس "dialog-error-symbolic";
	حالت VIM_WARNING:
	    واپس "dialog-warning-symbolic";
	حالت VIM_INFO:
	    واپس "dialog-information-symbolic";
	حالت VIM_QUESTION:
	    واپس "dialog-question-symbolic";
	طےشدہ:
	    توڑو;
    }
    واپس تہی;
}

// Data associated with each button in the dialog
قسمتعریف ساخت
{
    صحیح		but_idx;
    صحیح		*response;
    gboolean	*مکمل;
} DialogButtonState;

قسمتعریف ساخت
{
    GtkWidget	*win;
    gboolean	*مکمل;
    gboolean	no_alt;
} DialogState;

    ساکن خالی
dialog_button_clicked_cb(GtkButton *button غیر_مستعمل, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->مکمل = TRUE;
}

    ساکن gboolean
dialog_کلید_pressed_cb(
	Gtk_Event_Controller_کلید	*controller غیر_مستعمل,
	guint			keyval,
	guint			keycode غیر_مستعمل,
	GdkModifierType		state,
	DialogState		*dstate)
{
    اگر (keyval == GDK_کلید_Escape)
    {
	*dstate->مکمل = TRUE;
	واپس TRUE;
    }

    اگر (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_نقاب()))
	واپس gtk_widget_mnemonic_activate(dstate->win, FALSE);
    واپس FALSE;
}

    ساکن gboolean
dialog_close_request_cb(Gtk_کھڑکی *win غیر_مستعمل, gboolean *win_closed)
{
    *win_closed = TRUE;
    واپس FALSE;
}

    صحیح
gui_mch_dialog(
	صحیح	type,
	char_u	*title,
	char_u	*پیغام_2,
	char_u	*buttons,
	صحیح	def_but,
	char_u	*textfield,
	صحیح	ex_cmd غیر_مستعمل)
{
    Gtk_کھڑکی		*win = GTK_کھڑکی(gtk_کھڑکی_new());
    GtkWidget		*vertbox;
    GtkWidget		*پیغام_box;
    ثابت حرف		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*اندراج_2 = تہی;
    char_u		*utf8_title;
    char_u		*utf8_پیغام;
    GtkEventController *کلید_controller;
    DialogButtonState	*but_حالتیں = تہی;
    حرف		*tofree = تہی;
    صحیح			response = -1;
    gboolean		مکمل = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_مقصد_UTF8(title);
    اگر (utf8_title != تہی)
	gtk_کھڑکی_سیٹ_title(win, (ثابت حرف *)utf8_title);
    CONVERT_مقصد_UTF8_FREE(utf8_title);

    gtk_کھڑکی_سیٹ_transient_for(win, GTK_کھڑکی(gui.mainwin));
    gtk_کھڑکی_سیٹ_modal(win, TRUE);
    gtk_کھڑکی_سیٹ_default_سائز(win, 300, -1);
    gtk_کھڑکی_سیٹ_destroy_with_والد(win, TRUE);
    g_signal_connect(win, "close-request",
	    G_CALLBACK(dialog_close_request_cb), &win_closed);

    // Create main vertical layout container
    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_کھڑکی_سیٹ_بچہ(win, vertbox);
    gtk_widget_سیٹ_margin_چوٹی(vertbox, 24);
    gtk_widget_سیٹ_margin_bottom(vertbox, 24);
    gtk_widget_سیٹ_margin_آغاز(vertbox, 12);
    gtk_widget_سیٹ_margin_انتہا(vertbox, 12);

    // Add the message label
    پیغام_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_سیٹ_halign(پیغام_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), پیغام_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_مقصد_icon(type);
    اگر (icon_name != تہی)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_سیٹ_icon_سائز(GTK_IMAGE(icon), GTK_ICON_سائز_LARGE);
	gtk_box_append(GTK_BOX(پیغام_box), icon);
    }

    utf8_پیغام = CONVERT_مقصد_UTF8(پیغام_2);
    label = gtk_label_new((ثابت حرف *)utf8_پیغام);
    CONVERT_مقصد_UTF8_FREE(utf8_پیغام);
    gtk_label_سیٹ_wrap(GTK_LABEL(label), TRUE);
    gtk_label_سیٹ_max_چوڑائی_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(پیغام_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    کلید_controller = gtk_event_controller_کلید_new();
    g_signal_connect(کلید_controller, "key-pressed",
	    G_CALLBACK(dialog_کلید_pressed_cb), &state);
    gtk_widget_جوڑو_controller(GTK_WIDGET(win), کلید_controller);
    state.مکمل = &مکمل;
    state.win = GTK_WIDGET(win);

    اگر (textfield != تہی)
    {
	// Add text entry so user can enter text
	char_u *utf8_متن = CONVERT_مقصد_UTF8(textfield);

	اندراج_2 = gtk_اندراج_new();

	اگر (utf8_متن != تہی)
	    gtk_editable_سیٹ_متن(GTK_EDITABLE(اندراج_2), (ثابت حرف *)utf8_متن);
	ورنہ
	    gtk_editable_سیٹ_متن(GTK_EDITABLE(اندراج_2), "");
	CONVERT_مقصد_UTF8_FREE(utf8_متن);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_اندراج_سیٹ_activates_default(GTK_اندراج(اندراج_2), TRUE);
	gtk_box_append(GTK_BOX(vertbox), اندراج_2);
	state.no_alt = FALSE;
    }
    ورنہ
    {
	GListModel  *controllers;
	صحیح	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_اشیاء(controllers);
	برائے (صحیح i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_شے(controllers, i);
	    اگر (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_سیٹ_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_کھڑکی_سیٹ_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    اگر (buttons != تہی)
    {
	GtkWidget   *but_box;
	حرف	    **buttons_arr; // Note that array is allocated, not strings
	صحیح	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	اگر (vim_strchr(p_go, GO_VERTICAL) != تہی)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	ورنہ
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_سیٹ_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_سیٹ_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = بانٹو_button_string(buttons, &n_buttons, &tofree);

	but_حالتیں = g_malloc_n(n_buttons, حجم(DialogButtonState));

	برائے (صحیح i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_حالتیں + i;

	    but_label = CONVERT_مقصد_UTF8((char_u *)buttons_arr[i]);
	    اگر (but_label == تہی)
		جاری;

	    but = gtk_button_new_with_mnemonic((حرف *)but_label);
	    اگر (i == def_but - 1)
		gtk_کھڑکی_سیٹ_default_widget(win, but);
	    gtk_box_append(GTK_BOX(but_box), but);
	    CONVERT_مقصد_UTF8_FREE(but_label);

	    but_state->but_idx = i;
	    but_state->response = &response;
	    but_state->مکمل = &مکمل;

	    g_signal_connect(but, "clicked",
		    G_CALLBACK(dialog_button_clicked_cb), but_state);
	}
	g_free(buttons_arr);
    }

    gtk_کھڑکی_present(win);

    جب (!مکمل && !win_closed)
	g_main_context_iteration(تہی, TRUE);

    اگر (مکمل)
    {
	اگر (textfield != تہی)
	{
	    // Get the text the user entered
	    char_u *متن;

	    متن = (char_u *)gtk_editable_get_متن(GTK_EDITABLE(اندراج_2));
	    متن = CONVERT_FROM_UTF8(متن);
	    vim_strncpy(textfield, متن, IOSIZE - 1);
	    CONVERT_FROM_UTF8_FREE(متن);
	}

	gtk_کھڑکی_destroy(win);
    }
    g_free(but_حالتیں);
    g_free(tofree);

    // Vim buttons are indexed starting from one.
    واپس response == -1 ? 0 : response + 1;
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

قسمتعریف ساخت
{
    GtkWidget *dialog;
    GtkWidget *what;	    // Find what entry
    GtkWidget *with;	    // Replace with entry
    GtkWidget *wword;	    // Whole word check
    GtkWidget *mcase;	    // Match case check
    GtkWidget *اوپر;	    // Direction up radio
    GtkWidget *نیچے_کو;	    // Direction down radio
    GtkWidget *ڈھونڈو;	    // 'Find Next' action button
    GtkWidget *replace;	    // 'Replace With' action button
    GtkWidget *all;	    // 'Replace All' action button
} Shared_ڈھونڈو_Replace_2;

ساکن Shared_ڈھونڈو_Replace_2 ڈھونڈو_widgets = {0};
ساکن Shared_ڈھونڈو_Replace_2 repl_widgets = {0};

    ساکن خالی
ڈھونڈو_replace_cb(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا)
{
    صحیح			flags;
    char_u		*ڈھونڈو_متن;
    char_u		*repl_متن;
    gboolean		direction_نیچے_کو;
    Shared_ڈھونڈو_Replace_2	*sfr;

    flags = GPOINTER_مقصد_INT(ڈیٹا);

    اگر (flags == FRD_FINDNEXT)
    {
	repl_متن = تہی;
	sfr = &ڈھونڈو_widgets;
    }
    ورنہ
    {
	repl_متن = (char_u *)gtk_editable_get_متن(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    ڈھونڈو_متن = (char_u *)gtk_editable_get_متن(GTK_EDITABLE(sfr->what));
    direction_نیچے_کو = gtk_check_button_get_فعال(
	    GTK_CHECK_BUTTON(sfr->نیچے_کو));

    اگر (gtk_check_button_get_فعال(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_پورا_لفظ;
    اگر (gtk_check_button_get_فعال(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_مطابقت_CASE;

    repl_متن = CONVERT_FROM_UTF8(repl_متن);
    ڈھونڈو_متن = CONVERT_FROM_UTF8(ڈھونڈو_متن);
    gui_do_findrepl(flags, ڈھونڈو_متن, repl_متن, direction_نیچے_کو);
    CONVERT_FROM_UTF8_FREE(repl_متن);
    CONVERT_FROM_UTF8_FREE(ڈھونڈو_متن);
}

    ساکن خالی
dialog_destroyed_cb(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا)
{
    *(GtkWidget **)ڈیٹا = تہی;
}

    ساکن خالی
اندراج_بدلا_cb(GtkWidget *اندراج_2, GtkWidget *dialog)
{
    ثابت gchar	*اندراج_متن;
    gboolean	nonempty;

    اندراج_متن = gtk_editable_get_متن(GTK_EDITABLE(اندراج_2));

    اگر (!اندراج_متن)
	واپس; // Shouldn't happen

    nonempty = (اندراج_متن[0] != '\0');

    اگر (dialog == ڈھونڈو_widgets.dialog)
	gtk_widget_سیٹ_sensitive(ڈھونڈو_widgets.ڈھونڈو, nonempty);

    اگر (dialog == repl_widgets.dialog)
    {
	gtk_widget_سیٹ_sensitive(repl_widgets.ڈھونڈو, nonempty);
	gtk_widget_سیٹ_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_سیٹ_sensitive(repl_widgets.all, nonempty);
    }
}

    ساکن gboolean
ڈھونڈو_کلید_pressed_cb(
	Gtk_Event_Controller_کلید	*controller غیر_مستعمل,
	guint			keyval,
	guint			keycode غیر_مستعمل,
	GdkModifierType		state,
	Shared_ڈھونڈو_Replace_2	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    اگر (state & (GDK_CONTROL_نقاب | GDK_سرکاؤ_نقاب))
	واپس FALSE;

    // the Escape key synthesizes a cancellation action
    اگر (keyval == GDK_کلید_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_کھڑکی_destroy(GTK_کھڑکی(frdp->dialog));
	واپس TRUE;
    }

    واپس FALSE;
}

    ساکن خالی
اندراج_activate_cb(GtkWidget *widget غیر_مستعمل, خالی *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    ساکن خالی
ڈھونڈو_replace_dialog_create(char_u *arg, صحیح do_replace)
{
    Shared_ڈھونڈو_Replace_2	*frdp;
    char_u		*اندراج_متن;
    صحیح			wword = FALSE;
    صحیح			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*کلید_controller;

    frdp = do_replace ? &repl_widgets : &ڈھونڈو_widgets;
    اندراج_متن = get_ڈھونڈو_dialog_متن(arg, &wword, &mcase);

    اگر (اندراج_متن != تہی && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = اندراج_متن;
	اندراج_متن = string_convert(&output_conv, اندراج_متن, تہی);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    اگر (frdp->dialog != تہی)
    {
	اگر (اندراج_متن != تہی)
	{
	    gtk_editable_سیٹ_متن(GTK_EDITABLE(frdp->what),
		    (حرف *)اندراج_متن);
	    gtk_check_button_سیٹ_فعال(GTK_CHECK_BUTTON(frdp->wword),
		    (gboolean)wword);
	    gtk_check_button_سیٹ_فعال(GTK_CHECK_BUTTON(frdp->mcase),
		    (gboolean)mcase);
	}
	gtk_کھڑکی_present(GTK_کھڑکی(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	اگر (do_replace && g_utf8_strlen(
		    gtk_editable_get_متن(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(اندراج_متن);
	واپس;
    }

    // Create a new dialog window.
    frdp->dialog = gtk_کھڑکی_new();
    gtk_کھڑکی_سیٹ_transient_for(GTK_کھڑکی(frdp->dialog),
	    GTK_کھڑکی(gui.mainwin));
    gtk_کھڑکی_سیٹ_destroy_with_والد(GTK_کھڑکی(frdp->dialog), TRUE);
    gtk_کھڑکی_سیٹ_title(GTK_کھڑکی(frdp->dialog),
	    do_replace ? _("VIM - Search and Replace...")
	    : _("VIM - Search..."));
    gtk_کھڑکی_سیٹ_resizable(GTK_کھڑکی(frdp->dialog), FALSE);

    g_signal_connect(frdp->dialog, "destroy",
	    G_CALLBACK(dialog_destroyed_cb), &frdp->dialog);

    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_سیٹ_margin_آغاز(vertbox, 12);
    gtk_widget_سیٹ_margin_انتہا(vertbox, 12);
    gtk_widget_سیٹ_margin_چوٹی(vertbox, 12);
    gtk_widget_سیٹ_margin_bottom(vertbox, 12);
    gtk_کھڑکی_سیٹ_بچہ(GTK_کھڑکی(frdp->dialog), vertbox);

    // Grid for labels + entries
    grid = gtk_grid_new();
    gtk_grid_سیٹ_صف_spacing(GTK_GRID(grid), 6);
    gtk_grid_سیٹ_عمود_spacing(GTK_GRID(grid), 6);
    gtk_box_append(GTK_BOX(vertbox), grid);

    // "Find what:" label + entry
    tmp = gtk_label_new(_("Find what:"));
    gtk_label_سیٹ_xalign(GTK_LABEL(tmp), 0.0);
    gtk_grid_attach(GTK_GRID(grid), tmp, 0, 0, 1, 1);

    frdp->what = gtk_اندراج_new();
    gtk_widget_سیٹ_hexpand(frdp->what, TRUE);
    sensitive = (اندراج_متن != تہی && اندراج_متن[0] != NUL);
    اگر (اندراج_متن != تہی)
	gtk_editable_سیٹ_متن(GTK_EDITABLE(frdp->what), (حرف *)اندراج_متن);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    اگر (do_replace)
    {
	// "Replace with:" label + entry
	tmp = gtk_label_new(_("Replace with:"));
	gtk_label_سیٹ_xalign(GTK_LABEL(tmp), 0.0);
	gtk_grid_attach(GTK_GRID(grid), tmp, 0, 1, 1, 1);

	frdp->with = gtk_اندراج_new();
	gtk_widget_سیٹ_hexpand(frdp->with, TRUE);
	gtk_grid_attach(GTK_GRID(grid), frdp->with, 1, 1, 1, 1);
	gtk_اندراج_سیٹ_activates_default(GTK_اندراج(frdp->with), TRUE);

	// Make the entry activation only change the input focus onto the
	// with item.
	gtk_اندراج_سیٹ_activates_default(GTK_اندراج(frdp->what), FALSE);
	g_signal_connect(G_OBJECT(frdp->what), "activate",
		G_CALLBACK(اندراج_activate_cb), frdp->with);
    }
    ورنہ
	// Make the entry activation do the search.
	gtk_اندراج_سیٹ_activates_default(GTK_اندراج(frdp->what), TRUE);

    // Checkboxes
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    frdp->wword = gtk_check_button_new_with_label(_("Match whole word only"));
    gtk_check_button_سیٹ_فعال(GTK_CHECK_BUTTON(frdp->wword),
	    (gboolean)wword);
    gtk_box_append(GTK_BOX(hbox), frdp->wword);

    frdp->mcase = gtk_check_button_new_with_label(_("Match case"));
    gtk_check_button_سیٹ_فعال(GTK_CHECK_BUTTON(frdp->mcase),
	    (gboolean)mcase);
    gtk_box_append(GTK_BOX(hbox), frdp->mcase);

    // Direction radio buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    tmp = gtk_label_new(_("Direction:"));
    gtk_box_append(GTK_BOX(hbox), tmp);

    frdp->اوپر = gtk_check_button_new_with_label(_("Up"));
    gtk_box_append(GTK_BOX(hbox), frdp->اوپر);

    frdp->نیچے_کو = gtk_check_button_new_with_label(_("Down"));
    gtk_check_button_سیٹ_گروہ(GTK_CHECK_BUTTON(frdp->نیچے_کو),
	    GTK_CHECK_BUTTON(frdp->اوپر));
    gtk_check_button_سیٹ_فعال(GTK_CHECK_BUTTON(frdp->نیچے_کو), TRUE);
    gtk_box_append(GTK_BOX(hbox), frdp->نیچے_کو);

    // Action buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_سیٹ_halign(hbox, GTK_ALIGN_انتہا);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    btn = gtk_button_new_with_label(_("Find Next"));
    gtk_widget_سیٹ_sensitive(btn, sensitive);
    frdp->ڈھونڈو = btn;

    کلید_controller = gtk_event_controller_کلید_new();
    g_signal_connect(کلید_controller, "key-pressed",
	    G_CALLBACK(ڈھونڈو_کلید_pressed_cb), frdp);
    gtk_widget_جوڑو_controller(GTK_WIDGET(frdp->dialog), کلید_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(اندراج_بدلا_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_کھڑکی_سیٹ_default_widget(GTK_کھڑکی(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(ڈھونڈو_replace_cb),
	    GINT_مقصد_اشارہ_گر(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    اگر (do_replace)
    {
	btn = gtk_button_new_with_label(_("Replace"));
	gtk_widget_سیٹ_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(ڈھونڈو_replace_cb),
		GINT_مقصد_اشارہ_گر(FRD_REPLACE));
	frdp->replace = btn;
	gtk_box_append(GTK_BOX(hbox), btn);

	btn = gtk_button_new_with_label(_("Replace All"));
	gtk_widget_سیٹ_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(ڈھونڈو_replace_cb),
		GINT_مقصد_اشارہ_گر(FRD_REPLACEALL));
	frdp->all = btn;
	gtk_box_append(GTK_BOX(hbox), btn);
    }

    btn = gtk_button_new_with_label(_("Close"));
    g_signal_connect_swapped(btn, "clicked",
	    G_CALLBACK(gtk_کھڑکی_destroy), frdp->dialog);
    gtk_box_append(GTK_BOX(hbox), btn);

    gtk_کھڑکی_present(GTK_کھڑکی(frdp->dialog));
    gtk_widget_grab_focus(frdp->what);
    اگر (do_replace && اندراج_متن != تہی && اندراج_متن[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(اندراج_متن);
}

    خالی
gui_mch_ڈھونڈو_dialog(exarg_T *eap)
{
    اگر (gui.اندر_use)
	ڈھونڈو_replace_dialog_create(eap->arg, FALSE);
}

    خالی
gui_mch_replace_dialog(exarg_T *eap)
{
    اگر (gui.اندر_use)
	ڈھونڈو_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    خالی
ex_helpfind(exarg_T *eap غیر_مستعمل)
{
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

/*
 * ============================================================
 * Native print dialog
 * ============================================================
 */

#اگر_تعریف_شدہ USE_GTK4_PRINT_DIALOG
ساکن GtkPrintDialog *print_dialog = تہی;
ساکن GtkPrintSettings *print_settings = تہی;
ساکن Gtk_صفحہ_تیاری *صفحہ_تیاری = تہی;
ساکن Gtk_Print_تیاری *print_تیاری = تہی;
ساکن GFile *print_file;

    ساکن خالی
print_dialog_cb(GtkPrintDialog *dialog, GAsync_نتیجہ *نتیجہ, gboolean *مکمل)
{
    GError *error = تہی;
    print_تیاری = gtk_print_dialog_تیاری_اختتام(dialog, نتیجہ, &error);

    اگر (print_تیاری == تہی)
    {
	semsg(_(e_print_error_str), error->پیغام_2);
	g_error_free(error);
    }
    *مکمل = TRUE;
}

/*
 * Return file to write to on success (allocated string), or NULL on failure
 * (e.g. user cancelled)
 */
    char_u *
gui_gtk4_print_dialog(
	prt_settings_T	*psettings,
	char_u		*jobname,
	دوہرا		*صفحہ_چوڑائی,
	دوہرا		*صفحہ_اونچائی)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    مکمل = FALSE;
    char_u	    *عارضی;

    gtk_print_dialog_سیٹ_title(dialog, (ثابت حرف *)jobname);
    gtk_print_dialog_سیٹ_modal(dialog, TRUE);

    print_dialog = تہی;
    print_تیاری = تہی;

    اگر (print_settings == تہی)
    {
	option_جدول_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	اگر (opt->present)
	{
	    اگر (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_سیٹ_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    ورنہ اگر (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_سیٹ_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    ورنہ
		gtk_print_settings_سیٹ_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	اگر (opt->present)
	{
	    اگر (*opt->string == 'y')
		gtk_print_settings_سیٹ_orientation(print_settings,
			GTK_صفحہ_ORIENTATION_PORTRAIT);
	    ورنہ
		gtk_print_settings_سیٹ_orientation(print_settings,
			GTK_صفحہ_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	اگر (opt->present)
	{
	    gtk_print_settings_سیٹ_paper_سائز(print_settings,
		    gtk_paper_سائز_new((ثابت حرف *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	اگر (opt->present)
	{
	    اگر (*opt->string == 'y')
		gtk_print_settings_سیٹ_collate(print_settings, TRUE);
	    ورنہ
		gtk_print_settings_سیٹ_collate(print_settings, FALSE);
	}
    }

    اگر (صفحہ_تیاری == تہی)
	صفحہ_تیاری = gtk_صفحہ_تیاری_new();

    gtk_print_dialog_سیٹ_print_settings(dialog, print_settings);
    gtk_print_dialog_سیٹ_صفحہ_تیاری(dialog, صفحہ_تیاری);

    gtk_print_dialog_تیاری(dialog, GTK_کھڑکی(gui.mainwin), تہی,
	    (GAsyncReadyCallback)print_dialog_cb, &مکمل);

    جب (!مکمل)
	g_main_context_iteration(تہی, TRUE);

    اگر (print_تیاری == تہی)
    {
	g_object_unref(dialog);
	g_clear_اشارہ_گر(&print_تیاری, gtk_print_تیاری_unref);

	psettings->user_abort_پیغام = FALSE;
	واپس تہی;
    }

    g_clear_object(&print_settings);
    g_clear_object(&صفحہ_تیاری);
    print_settings = g_object_ref(
	    gtk_print_تیاری_get_print_settings(print_تیاری));
    صفحہ_تیاری = g_object_ref(gtk_print_تیاری_get_صفحہ_تیاری(print_تیاری));

    {
	GtkPrintDuplex duplex = gtk_print_settings_get_duplex(print_settings);
	psettings->duplex = duplex != GTK_PRINT_DUPLEX_SIMPLEX;
    }

    psettings->has_color = gtk_print_settings_get_use_color(print_settings);

    *صفحہ_چوڑائی = gtk_صفحہ_تیاری_get_paper_چوڑائی(صفحہ_تیاری,
	    GTK_UNIT_POINTS);
    *صفحہ_اونچائی = gtk_صفحہ_تیاری_get_paper_اونچائی(صفحہ_تیاری,
	    GTK_UNIT_POINTS);

    عارضی  = vim_tempname('p', FALSE);
    اگر (عارضی == تہی)
    {
	emsg(_(e_cant_ڈھونڈو_عارضی_file_for_writing));
	g_clear_object(&dialog);
	g_clear_اشارہ_گر(&print_تیاری, gtk_print_تیاری_unref);

	psettings->user_abort_پیغام = FALSE;
	واپس تہی;
    }
    print_file = g_file_new_for_راستہ((حرف *)عارضی);

    print_dialog = dialog;

    واپس عارضی;
}

    ساکن خالی
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsync_نتیجہ	*نتیجہ,
	GFile		*file)
{
    حرف *راستہ = g_file_get_راستہ(file);

    (خالی)gtk_print_dialog_print_file_اختتام(dialog, نتیجہ, تہی);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(راستہ);
    g_free(راستہ);
}


    خالی
gui_gtk4_print_اختتام(خالی)
{
    اگر (print_dialog != تہی)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_کھڑکی(gui.mainwin),
		print_تیاری, print_file, تہی,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = تہی;
	g_clear_اشارہ_گر(&print_تیاری, gtk_print_تیاری_unref);
    }
}
#اگر_ختم // USE_GTK4_PRINT_DIALOG

#اگر_تعریف_شدہ FEAT_IMAGE_GUI

قسمتعریف ساخت
{
    GdkTexture *texture;
} image_gdk_T;

قسمتعریف ساخت
{
    Gsk_Render_گرہ   **nodes; // Note that elements may be NULL
    صحیح		    n_nodes;
} image_placement_gdk_T;

    صحیح
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    Gdk_حافظہ_Format fmt;
    size_t	    stride;
    GBytes	    *بائٹیں;
    size_t	    سائز;
    صحیح		    w, h;

    اگر (ctx == تہی)
	واپس FAIL;

    اگر (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_حافظہ_R8_G8_B8_A8;
    ورنہ
	fmt = GDK_حافظہ_R8_G8_B8;

    image_get_dimensions(img, &w, &h);

    سائز = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    بائٹیں = g_بائٹیں_new(img->ڈیٹا, سائز);
    ctx->texture = gdk_حافظہ_texture_new(w, h, fmt, بائٹیں, stride);
    g_بائٹیں_unref(بائٹیں);

    img->backend_ڈیٹا = ctx;
    واپس OK;
}

    خالی
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_ڈیٹا;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    ساکن خالی
clear_nodes(image_placement_gdk_T *ctx)
{
    اگر (ctx->nodes == تہی || gui.drawarea == تہی)
	واپس;

    برائے (صحیح i = 0; i < ctx->n_nodes; i++)
    {
	اگر (ctx->nodes[i] == تہی)
	    جاری;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_گرہ_unref(ctx->nodes[i]);
    }
    g_clear_اشارہ_گر(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    صحیح
image_placement_gui_init(image_placement_T *رکھو)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    اگر (ctx == تہی)
	واپس FAIL;

    رکھو->backend_ڈیٹا = ctx;
    واپس OK;
}

    خالی
image_placement_gui_uninit(image_placement_T *رکھو)
{
    image_placement_gdk_T *ctx = رکھو->backend_ڈیٹا;

    clear_nodes(ctx);
    vim_free(ctx);
}

    خالی
image_placement_gui_draw(image_placement_T *رکھو)
{
    image_T		    *img = رکھو->img;
    image_placement_gdk_T   *ctx = رکھو->backend_ڈیٹا;
    image_gdk_T		    *ictx = img->backend_ڈیٹا;
    pixman_box32_t	    *rects;
    صحیح			    n_rects;
    صحیح			    iw, ih;
    Gsk_Render_گرہ	    *texture_گرہ;

    rects = pixman_region32_rectangles(&رکھو->visible, &n_rects);
    اگر (rects == تہی)
	واپس;

    clear_nodes(ctx);
    اگر (n_rects == 0)
	واپس;

    image_get_dimensions(img, &iw, &ih);
    texture_گرہ = gsk_texture_گرہ_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(رکھو->ستون) - FILL_X(رکھو->crop_box.x1),
		FILL_Y(رکھو->صف) - FILL_Y(رکھو->crop_box.y1)
		+ FILL_Y(رکھو->صف_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    اگر (texture_گرہ == تہی)
	// Not sure if this can happen...
	واپس;

    ctx->nodes = g_malloc_n(n_rects, حجم(Gsk_Render_گرہ *));
    ctx->n_nodes = n_rects;

    برائے (صحیح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحیح		صف, ستون;
	صحیح		posx, posy;
	صحیح		x, y, w, h;
	Gsk_Render_گرہ	*گرہ;

	image_placement_subrect(رکھو, rect, &صف, &ستون, &x, &y, &w, &h, false);
	posx = FILL_X(ستون);
	posy = FILL_Y(صف);

	گرہ = gsk_clip_گرہ_new(texture_گرہ,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	اگر (گرہ != تہی)
	    vim_draw_area_جوڑو_external(VIM_DRAW_AREA(gui.drawarea), گرہ);

	ctx->nodes[i] = گرہ;
    }
    gsk_render_گرہ_unref(texture_گرہ);
}

    خالی
image_placement_gui_clear(image_placement_T *رکھو)
{
    image_placement_gdk_T   *ctx = رکھو->backend_ڈیٹا;

    clear_nodes(ctx);
}

#اگر_ختم

#اگر_ختم // FEAT_GUI_GTK
