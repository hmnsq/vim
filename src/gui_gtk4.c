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

#جے_تعریف_شدہ FEAT_GUI_GTK

#ضمن <gdk/gdkkeysyms.h>
#ضمن <gdk/gdk.h>
#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_f.h"
#ضمن "gui_gtk4_cb.h"
#ضمن "gui_gtk4_da.h"
#جے_تعریف_شدہ FEAT_TOOLBAR
# include "gui_gtk4_tb.h"
#جے_انت
#جے_تعریف_شدہ FEAT_MENU
# include "gui_gtk4_menu.h"
#جے_انت

/*
 * Geometry string parser, replacing XParseGeometry to remove X11 dependency.
 * Format: [WIDTHxHEIGHT][{+-}XOFF{+-}YOFF]
 */
#تعریف_کرو No_قدر		0x0000
#تعریف_کرو چوڑائی_قدر	0x0004
#تعریف_کرو اچائی_قدر	0x0008

    ساكن صحيح
vim_parse_geometry(ثابت حرف *str, بنانشانی صحيح *چوڑائی, بنانشانی صحيح *اچائی)
{
    صحيح نقاب = No_قدر;
    حرف *سرا;
    لما val;

    إذا (str == عدم || *str == NUL)
	أرجع نقاب;

    // Parse width
    إذا (*str != '+' && *str != '-')
    {
	val = strtol(str, &سرا, 10);
	إذا (سرا != str)
	{
	    *چوڑائی = (بنانشانی صحيح)val;
	    نقاب |= چوڑائی_قدر;
	    str = سرا;
	}
    }

    // Parse 'x' or 'X' separator and height
    إذا (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &سرا, 10);
	إذا (سرا != str)
	{
	    *اچائی = (بنانشانی صحيح)val;
	    نقاب |= اچائی_قدر;
	    str = سرا;
	}
    }

    أرجع نقاب;
}

#إذا defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
ساكن صحيح آخری_shape = 0;
#جے_انت

#تعریف_کرو DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
ساكن تعداد {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
ساكن صحيح gtk4_main_loop_level = 0;
ساكن صحيح gtk4_main_loop_quit = FALSE;

#جے_تعریف_شدہ USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#جے_انت

تعريفنوع gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
ساكن بنية special_کلید
{
    guint کلید_sym;
    char_u code0;
    char_u code1;
}
ثابت special_کلیدیں[] =
{
    {GDK_کلید_اتے,	'k', 'u'},
    {GDK_کلید_تھلے,	'k', 'd'},
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
    {GDK_کلید_پٹھ_Space,	'k', 'b'},
    {GDK_کلید_داخل_کرو,	'k', 'I'},
    {GDK_کلید_Delete,	'k', 'D'},
    {GDK_کلید_Home,	'k', 'h'},
    {GDK_کلید_سرا,	'@', '7'},
    {GDK_کلید_پہلوں,	'k', 'P'},
    {GDK_کلید_اگلا,	'k', 'N'},
    {GDK_کلید_Print,	'%', '9'},
    {GDK_کلید_KP_بایاں,	'k', 'l'},
    {GDK_کلید_KP_دایاں,	'k', 'r'},
    {GDK_کلید_KP_اتے,	'k', 'u'},
    {GDK_کلید_KP_تھلے,	'k', 'd'},
    {GDK_کلید_KP_داخل_کرو,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_کلید_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_کلید_KP_Home,	'K', '1'},
    {GDK_کلید_KP_سرا,	'K', '4'},
    {GDK_کلید_KP_پہلوں,	'K', '3'},
    {GDK_کلید_KP_اگلا,	'K', '5'},
    {GDK_کلید_KP_جوڑو,	'K', '6'},
    {GDK_کلید_KP_گھٹاؤ, 'K', '7'},
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

    ساكن صحيح
keyval_منزل_string(بنانشانی صحيح keyval, char_u *string)
{
    صحيح		len;
    guint32	uc;

    uc = gdk_keyval_منزل_unicode(keyval);
    إذا (uc != 0)
    {
	len = utf_char2bytes((صحيح)uc, string);
    }
    وإلا
    {
	len = 1;
	بدل (keyval)
	{
	    حالة GDK_کلید_Tab: حالة GDK_کلید_KP_Tab: حالة GDK_کلید_ISO_بایاں_Tab:
		string[0] = TAB;
		اكسر;
	    حالة GDK_کلید_Linefeed:
		string[0] = NL;
		اكسر;
	    حالة GDK_کلید_Return: حالة GDK_کلید_ISO_Enter: حالة GDK_کلید_3270_Enter:
		string[0] = CAR;
		اكسر;
	    حالة GDK_کلید_Escape:
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

    إذا (state & GDK_کھسکاؤ_نقاب)
	modifiers |= MOD_نقاب_کھسکاؤ;
    إذا (state & GDK_CONTROL_نقاب)
	modifiers |= MOD_نقاب_CTRL;
    إذا (state & GDK_ALT_نقاب)
	modifiers |= MOD_نقاب_ALT;
    إذا (state & GDK_META_نقاب)
	modifiers |= MOD_نقاب_META;
    إذا (state & GDK_SUPER_نقاب)
	modifiers |= MOD_نقاب_CMD;

    أرجع modifiers;
}

ساكن GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
ساكن gboolean کلید_press_event(Gtk_Event_Controller_کلید *controller, guint keyval, guint keycode, GdkModifierType state, gpointer انکڑے);
ساكن فراغ کلید_release_event(Gtk_Event_Controller_کلید *controller, guint keyval, guint keycode, GdkModifierType state, gpointer انکڑے);
ساكن فراغ button_press_event(GtkGestureClick *gesture, صحيح n_press, مزدوج x, مزدوج y, gpointer انکڑے);
ساكن فراغ button_release_event(GtkGestureClick *gesture, صحيح n_press, مزدوج x, مزدوج y, gpointer انکڑے);
ساكن فراغ motion_notify_event(GtkEventControllerMotion *controller, مزدوج x, مزدوج y, gpointer انکڑے);
ساكن فراغ enter_notify_event(GtkEventControllerMotion *controller, مزدوج x, مزدوج y, gpointer انکڑے);
ساكن gboolean scroll_event(GtkEventControllerScroll *controller, مزدوج dx, مزدوج dy, gpointer انکڑے);
ساكن فراغ focus_وچ_event(GtkEventControllerFocus *controller, gpointer انکڑے);
ساكن فراغ focus_باہرلا_event(GtkEventControllerFocus *controller, gpointer انکڑے);
#جے_تعریف_شدہ FEAT_DND
ساكن gboolean drop_cb(Gtk_Drop_نشانہ_Async *نشانہ, GdkDrop *drop, مزدوج x, مزدوج y, فراغ *انکڑے);
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_TABLINE
ساكن فراغ tabline_enter_cb(GtkEventController *controller, مزدوج x, مزدوج y, فراغ *udata);
ساكن فراغ on_select_tab(GtkNotebook *notebook, gpointer *ورقہ, gint idx, gpointer انکڑے);
ساكن فراغ on_tab_reordered(GtkNotebook *notebook, gpointer *ورقہ, gint idx, gpointer انکڑے);
ساكن VimMenu *create_tabline_popup_menu(فراغ);
ساكن فراغ tabline_جوڑو_css(فراغ);
ساكن فراغ tabline_menu_press_event(GtkGestureClick *gesture, صحيح n_press, مزدوج x, مزدوج y, GtkWidget *popover);
#جے_انت
ساكن فراغ mainwin_destroy_cb(GObject *object, gpointer انکڑے);
ساكن gboolean delete_event_cb(Gtk_کھڑکی *کھڑکی_2, gpointer انکڑے);
ساكن صحيح query_اشارہ_گر_pos(صحيح *x, صحيح *y, GdkModifierType *state);
ساكن فراغ mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_انکڑے);
ساكن فراغ سیٹ_form_ناپ(صحيح چوڑائی, صحيح اچائی);
ساكن فراغ drawarea_realize_cb(GtkWidget *widget, gpointer انکڑے);
ساكن فراغ drawarea_unrealize_cb(GtkWidget *widget, gpointer انکڑے);
ساكن فراغ clipboard_بدلیا_cb(GdkClipboard *clipboard, gpointer user_انکڑے);
#جے_تعریف_شدہ FEAT_MENU
ساكن فراغ show_menubar_popover(فراغ);
#جے_انت

ساكن ثابت حرف *prgname = عدم;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    ساكن gboolean
arg_میل(ثابت حرف *s, ثابت حرف *name, حرف **قدر)
{
    size_t  len = لڑی_لمبائی(name);

    إذا (لڑی_مقابلہ(s, name, len) != 0)
	أرجع FALSE;
    إذا (s[len] == '=')
	*قدر = (حرف *)s + len + 1;
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
	حرف	*قدر = عدم;
	صحيح	has_inline_قدر = FALSE;
	صحيح	n_strip;

	إذا (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    استمر;
	}

	إذا (strchr(s, '=') != عدم)
	    has_inline_قدر = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	إذا (!has_inline_قدر)
	{
	    إذا (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		قدر = argv[i + 1];
	    وإلا
		قدر = عدم;
	}

	إذا (arg_میل(s, "-fn", &قدر) || arg_میل(s, "-font", &قدر))
	    font_دلیل = قدر;
	وإلا إذا (arg_میل(s, "-geom", &قدر)
		|| arg_میل(s, "-geometry", &قدر))
	{
	    إذا (قدر != عدم)
		gui.geom = vim_strsave((char_u *)قدر);
	}
	وإلا إذا (arg_میل(s, "-bg", &قدر)
		|| arg_میل(s, "-background", &قدر))
	    background_دلیل = قدر;
	وإلا إذا (arg_میل(s, "-fg", &قدر)
		|| arg_میل(s, "-foreground", &قدر))
	    foreground_دلیل = قدر;
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
	وإلا إذا (لڑی_مقابلہ(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_قدر = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#جے_انت
	وإلا إذا (arg_میل(s, "--prg-name", &قدر))
	    // GTK4 specific
	    prgname = قدر;
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
	إذا (قدر != عدم && !has_inline_قدر)
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
timeout_جوڑو(صحيح ویلا, timeout_cb_type (*callback)(gpointer), صحيح *flagp)
{
    أرجع g_timeout_جوڑو((guint)ویلا, (GSourceFunc)callback, flagp);
}

    ساكن فراغ
timeout_remove(guint timer)
{
    g_سروت_remove(timer);
}

ساكن long_u blink_waittime = 700;
ساكن long_u blink_ontime = 400;
ساكن long_u blink_offtime = 250;
ساكن guint blink_timer = 0;

    ساكن timeout_cb_type
blink_cb(gpointer انکڑے بن_ورتیا)
{
    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_جوڑو(blink_offtime, blink_cb, عدم);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_جوڑو(blink_ontime, blink_cb, عدم);
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
gui_mch_سیٹ_blinking(لما waittime, لما on, لما off)
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
gui_mch_شروع_blink(فراغ)
{
    إذا (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.وچ_focus)
    {
	blink_timer = timeout_جوڑو(blink_waittime, blink_cb, عدم);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    صحيح
gui_mch_early_init_check(صحيح give_سنیہا بن_ورتیا)
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
    g_سیٹ_prgname(prgname == عدم ? "gvim" : prgname);

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
    gui.def_پٹھ_pixel = 0x00ffffff;	// white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.پٹھ_pixel = gui.def_پٹھ_pixel;

    gui.scrollbar_چوڑائی = SB_DEFAULT_چوڑائی;
    gui.scrollbar_اچائی = SB_DEFAULT_چوڑائی;

    // Create the main window.
    gui.mainwin = gtk_کھڑکی_new();
    gtk_widget_سیٹ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سیٹ_ادھار_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    g_signal_connect(G_OBJECT(gui.mainwin), "close-request",
		     G_CALLBACK(delete_event_cb), عدم);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), عدم);

    // A vertical box holds the menubar, toolbar and main text window.
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_سیٹ_homogeneous(GTK_BOX(vbox), FALSE);
    gtk_کھڑکی_سیٹ_بچہ(GTK_کھڑکی(gui.mainwin), vbox);

#جے_تعریف_شدہ FEAT_MENU
    gui.menubar = vim_menu_bar_new();
    gtk_widget_سیٹ_name(gui.menubar, "vim-menubar");
    gtk_widget_سیٹ_visible(gui.menubar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.menubar);
#جے_انت

#جے_تعریف_شدہ FEAT_TOOLBAR
    gui.toolbar = vim_toolbar_new();
    gtk_widget_سیٹ_name(gui.toolbar, "vim-toolbar");
    gtk_widget_سیٹ_visible(gui.toolbar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.toolbar);
    vim_toolbar_سیٹ_style(VIM_TOOLBAR(gui.toolbar), toolbar_flags, tbis_flags);
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    gui.tabline = gtk_notebook_new();
    gtk_notebook_سیٹ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سیٹ_show_tabs(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_notebook_سیٹ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_widget_جوڑو_css_class(gui.tabline, "vim-tabline");
    tabline_جوڑو_css();
    gtk_widget_سیٹ_visible(gui.tabline, FALSE);
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
	gtk_event_controller_سیٹ_پسار_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	gtk_widget_جوڑو_controller(gui.tabline, mcontroller);
    }

    // Create right click popup menu for tabline
    {
	GtkGesture	*click;
	VimMenu		*menu;

	click = gtk_gesture_click_new();
	menu = create_tabline_popup_menu();

	gtk_widget_سیٹ_ماپا(GTK_WIDGET(menu), gui.tabline);
	g_object_سیٹ_انکڑے(G_OBJECT(gui.tabline), "menu", menu);

	gtk_popover_سیٹ_has_arrow(GTK_POPOVER(menu), FALSE);
	gtk_popover_سیٹ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
	// Make popover start at top left corner
	gtk_widget_سیٹ_halign(GTK_WIDGET(menu), GTK_ALIGN_شروع);

	// Listen for anny mouse button
	gtk_gesture_single_سیٹ_button(GTK_GESTURE_SINGLE(click), 0);

	g_signal_connect_object(click, "pressed",
		G_CALLBACK(tabline_menu_press_event),
		menu, G_CONNECT_DEFAULT);
	gtk_widget_جوڑو_controller(gui.tabline, GTK_EVENT_CONTROLLER(click));
    }
#جے_انت

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
		     G_CALLBACK(drawarea_realize_cb), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), عدم);

    // Set up event controllers.
    {
	GtkEventController *کلید_ctrl = gtk_event_controller_کلید_new();
	g_signal_connect(کلید_ctrl, "key-pressed",
			 G_CALLBACK(کلید_press_event), عدم);
	g_signal_connect(کلید_ctrl, "key-released",
			 G_CALLBACK(کلید_release_event), عدم);
	gtk_widget_جوڑو_controller(gui.drawarea, کلید_ctrl);
#جے_تعریف_شدہ FEAT_XIM
	xim_init();
#جے_انت
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_سیٹ_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), عدم);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), عدم);
	gtk_widget_جوڑو_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), عدم);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), عدم);
	gtk_widget_جوڑو_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), عدم);
	gtk_widget_جوڑو_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_وچ_event), عدم);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_باہرلا_event), عدم);
	gtk_widget_جوڑو_controller(gui.drawarea, focus);
    }

#جے_تعریف_شدہ FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_نشانہ = gtk_drop_نشانہ_async_new(عدم,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_سیٹ_dnd_targets();
	g_signal_connect(gui.drop_نشانہ, "drop",
			 G_CALLBACK(drop_cb), عدم);
	gtk_widget_جوڑو_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_نشانہ));
    }
#جے_انت

    gui.border_وتھ = gui.border_چوڑائی;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_اشارہ_گر = gdk_cursor_new_from_name("none", عدم);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	إذا (primary != عدم)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_بدلیا_cb), &clip_تارا);
	إذا (regular != عدم)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_بدلیا_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_تارا);

    أرجع OK;
}

    فراغ
gui_mch_new_colors(فراغ)
{
    إذا (gui.drawarea != عدم && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_قطار_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open(فراغ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_چوڑائی;
    guint pixel_اچائی;

    إذا (gui.geom != عدم)
    {
	صحيح		نقاب;
	بنانشانی صحيح	w, h;

	نقاب = vim_parse_geometry((حرف *)gui.geom, &w, &h);

	إذا (نقاب & چوڑائی_قدر)
	    Columns = w;
	إذا (نقاب & اچائی_قدر)
	{
	    إذا (p_کھڑکی > (لما)h - 1 || !option_was_سیٹ((char_u *)"window"))
		p_کھڑکی = h - 1;
	    صفاں = h;
	}
	حد_بندی_screen_ناپ();

	VIM_CLEAR(gui.geom);
    }
    وإلا
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	إذا (Columns > 80)
	    Columns = 80;
	إذا (صفاں > 24)
	    صفاں = 24;
    }

    pixel_چوڑائی = (guint)(gui_get_ادھار_چوڑائی() + Columns * gui.char_چوڑائی);
    pixel_اچائی = (guint)(gui_get_ادھار_اچائی() + صفاں * gui.char_اچائی);

    pixel_چوڑائی  += get_menu_tool_چوڑائی();
    pixel_اچائی += get_menu_tool_اچائی();

    إذا (foreground_دلیل != عدم)
	fg_pixel = gui_get_color((char_u *)foreground_دلیل);
    إذا (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    إذا (background_دلیل != عدم)
	bg_pixel = gui_get_color((char_u *)background_دلیل);
    إذا (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    إذا (لبھیا_الٹ_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_پٹھ_pixel = fg_pixel;
    }
    وإلا
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_پٹھ_pixel = bg_pixel;
    }

    سیٹ_عام_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), عدم);
    // Resize is handled by GtkForm's size_allocate callback.

    سیٹ_form_ناپ((صحيح)pixel_چوڑائی - get_menu_tool_چوڑائی(),
	    (صحيح)pixel_اچائی - get_menu_tool_اچائی());

    gtk_کھڑکی_present(GTK_کھڑکی(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    أرجع OK;
}

    فراغ
gui_mch_exit(صحيح rc بن_ورتیا)
{
    إذا (gui.mainwin != عدم)
    {
#جے_تعریف_شدہ FEAT_GUI_TABLINE
	// Must unparent popover menu for tabline or we will get warning
	gtk_widget_unparent(g_object_get_انکڑے(G_OBJECT(gui.tabline), "menu"));
#جے_انت
#جے_تعریف_شدہ FEAT_BEVAL_GUI
	// Make sure to destroy popover used for balloon eval, or we will get a
	// warning from GTK that the draw area still has children left.
	gui_mch_destroy_beval_area(balloonEval);
#جے_انت
#جے_تعریف_شدہ FEAT_MENU
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
#جے_انت
	gtk_کھڑکی_destroy(GTK_کھڑکی(gui.mainwin));
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
gui_mch_سیٹ_winpos(صحيح x بن_ورتیا, صحيح y بن_ورتیا)
{
    // GTK4/Wayland: window positioning not available
}

    صحيح
gui_mch_maximized(فراغ)
{
    أرجع gtk_کھڑکی_is_maximized(GTK_کھڑکی(gui.mainwin));
}

    فراغ
gui_mch_unmaximize(فراغ)
{
    إذا (gui.mainwin != عدم)
	gtk_کھڑکی_unmaximize(GTK_کھڑکی(gui.mainwin));
}

    فراغ
gui_mch_سیٹ_fullscreen(صحيح flag)
{
    إذا (gui.mainwin == عدم)
	أرجع;
    إذا (flag)
	gtk_کھڑکی_fullscreen(GTK_کھڑکی(gui.mainwin));
    وإلا
	gtk_کھڑکی_unfullscreen(GTK_کھڑکی(gui.mainwin));
}

    ساكن فراغ
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec بن_ورتیا, gpointer user_انکڑے بن_ورتیا)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    إذا (gtk_کھڑکی_is_fullscreen(GTK_کھڑکی(obj)))
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
    w = gtk_widget_get_چوڑائی(gui.formwin);
    h = gtk_widget_get_اچائی(gui.formwin);
    gui_resize_shell(w, h);
}

    فراغ
gui_mch_settitle(char_u *title, char_u *icon بن_ورتیا)
{
    إذا (title != عدم && gui.mainwin != عدم)
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
    فراغ
gui_gtk_init_decor_اچائی(فراغ)
{
    صحيح h = gtk_widget_get_اچائی(gui.mainwin);

    إذا (h == 0)
	أرجع;

    h -= get_menu_tool_اچائی();
    h -= gtk_widget_get_اچائی(gui.formwin);

    gui.decor_اچائی = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    ساكن فراغ
سیٹ_form_ناپ(صحيح چوڑائی, صحيح اچائی)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    إذا (gtk_widget_get_چوڑائی(gui.formwin) == چوڑائی
	    && gtk_widget_get_اچائی(gui.formwin) == اچائی)
	أرجع;

    // Remember the size the form widget is supposed to get. An allocation
    // that arrives before the compositor has answered this request still has
    // the previous size and must not be used.
    gui.pending_form_w = چوڑائی;
    gui.pending_form_h = اچائی;
    gui.pending_form_skip = 1;

    // GtkWindow default size also includes client side decorations, so must
    // include it also.  It also keeps the natural width of the toolbar from
    // deciding the width.
    gtk_کھڑکی_سیٹ_default_ناپ(GTK_کھڑکی(gui.mainwin),
	    چوڑائی + get_menu_tool_چوڑائی(),
	    اچائی + get_menu_tool_اچائی() + gui.decor_اچائی);

    // The window drops a request made while it is being presented, and
    // "gui.decor_height" is not known before that.  A size request on the form
    // widget is honoured then; it is dropped again once the size was given.
    gtk_widget_سیٹ_ناپ_request(gui.formwin, چوڑائی, اچائی);
}

    فراغ
gui_mch_سیٹ_shellsize(صحيح چوڑائی, صحيح اچائی,
	صحيح min_چوڑائی بن_ورتیا, صحيح min_اچائی بن_ورتیا,
	صحيح ادھار_چوڑائی بن_ورتیا, صحيح ادھار_اچائی بن_ورتیا,
	صحيح direction بن_ورتیا)
{
    سیٹ_form_ناپ(چوڑائی, اچائی);

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
	    *screen_w = geom.چوڑائی;
	    *screen_h = geom.اچائی;
	    أرجع;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#جے_تعریف_شدہ FEAT_MENU
    فراغ
gui_mch_enable_menu(صحيح showit)
{
    إذا (gui.menubar != عدم)
    {
	gtk_widget_سیٹ_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	إذا (gui.drawarea != عدم)
	    gtk_widget_قطار_draw(gui.drawarea);
    }
}
#جے_انت

#جے_تعریف_شدہ FEAT_TOOLBAR
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    إذا (gui.toolbar != عدم)
    {
	gtk_widget_سیٹ_visible(gui.toolbar, showit);
	إذا (showit)
	    vim_toolbar_سیٹ_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	إذا (gui.drawarea != عدم)
	    gtk_widget_قطار_draw(gui.drawarea);
    }
}
#جے_انت

    فراغ
gui_mch_سیٹ_dark_theme(صحيح dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    إذا (settings != عدم)
	g_object_سیٹ(settings, "gtk-application-prefer-dark-theme",
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

    metrics = pango_context_get_metrics(gui.متن_context, gui.norm_font,
			    pango_context_get_language(gui.متن_context));
    ascent = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);
    pango_font_metrics_unref(metrics);

    gui.char_اچائی = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_اچائی = MAX(gui.char_اچائی, gui.char_ascent + 1);

    أرجع OK;
}

تعريفنوع بنية {
    PangoFontDescription    *نتیجہ;
    gboolean		    پورا;
} Font_Dialog_انکڑے;

    ساكن فراغ
font_dialog_انت_cb(GObject *سروت, GAsync_نتیجہ *res, gpointer انکڑے)
{
    Font_Dialog_انکڑے *fdd = (Font_Dialog_انکڑے *)انکڑے;
    fdd->نتیجہ = gtk_font_dialog_چنو_font_انت(
		    GTK_FONT_DIALOG(سروت), res, عدم);
    fdd->پورا = TRUE;
}

    ساكن gboolean
font_filter(gpointer شے, gpointer انکڑے بن_ورتیا)
{
    إذا (PANGO_IS_FONT_FAMILY(شے))
	أرجع pango_font_family_is_monospace(PANGO_FONT_FAMILY(شے));
    إذا (PANGO_IS_FONT_FACE(شے))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(شے));
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
    Font_Dialog_انکڑے	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_سیٹ_modal(dlg, TRUE);
    gtk_font_dialog_سیٹ_filter(dlg,
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

    fdd.نتیجہ = عدم;
    fdd.پورا = FALSE;

    gtk_font_dialog_چنو_font(dlg, GTK_کھڑکی(gui.mainwin),
	    initial, عدم, font_dialog_انت_cb, &fdd);

    طالما (!fdd.پورا)
	g_main_context_iteration(عدم, TRUE);

    إذا (fdd.نتیجہ != عدم)
    {
	حرف *name = pango_font_description_منزل_string(fdd.نتیجہ);
	إذا (name != عدم)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    إذا (p != عدم && داخلہ_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&داخلہ_conv, p, عدم);
		vim_free(p);
	    }
	    وإلا
		fontname = p;
	}
	pango_font_description_free(fdd.نتیجہ);
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
ascii_glyph_جدول_init(فراغ)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *شے_list;
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
    شے_list = pango_itemize(gui.متن_context, (ثابت حرف *)ascii_chars,
			      0, حجم(ascii_chars), attr_list, عدم);

    إذا (شے_list != عدم && شے_list->اگلا == عدم)
    {
	Pango_شے   *شے;
	صحيح	    چوڑائی;

	شے  = (Pango_شے *)شے_list->انکڑے;
	چوڑائی = gui.char_چوڑائی * PANGO_SCALE;

	gui.ascii_font = شے->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت حرف *)ascii_chars, حجم(ascii_chars),
		    &شے->analysis, gui.ascii_glyphs);

	إذا (gui.ascii_glyphs->عدد_glyphs == (صحيح)حجم(ascii_chars))
	{
	    لكل (i = 0; i < gui.ascii_glyphs->عدد_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_وتھ += MAX(0, چوڑائی - geom->چوڑائی) / 2;
		geom->چوڑائی = چوڑائی;
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

    g_list_foreach(شے_list, (GFunc)(فراغ *)&pango_شے_free, عدم);
    g_list_free(شے_list);
    pango_attr_list_unref(attr_list);
}

    ساكن فراغ
get_styled_font_variants(فراغ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_لدو_font(gui.متن_context, gui.norm_font);
    إذا (plain_font == عدم)
	أرجع;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_سیٹ_بھار(bold_font_desc, PANGO_بھار_BOLD);

    bold_font = pango_context_لدو_font(gui.متن_context, bold_font_desc);
    إذا (bold_font != عدم)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    صحيح
gui_mch_init_font(char_u *font_name, صحيح fontset بن_ورتیا)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحيح			    چوڑائی;

    إذا (font_name == عدم)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    إذا (font_desc == عدم)
	أرجع FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_سیٹ_font_description(gui.متن_context, font_desc);

    layout = pango_layout_new(gui.متن_context);
    pango_layout_سیٹ_متن(layout, "MW", 2);
    pango_layout_get_ناپ(layout, &چوڑائی, عدم);
    g_object_unref(layout);

    gui.char_چوڑائی = (چوڑائی / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    إذا (gui.char_چوڑائی <= 0)
	gui.char_چوڑائی = 8;

    gui_mch_adjust_charheight();

    hl_سیٹ_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_جدول_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	صحيح im_صف = gui.صف < screen_صفاں ? gui.صف : screen_صفاں - 1;
	صحيح im_خانہ = gui.خانہ < screen_Columns ? gui.خانہ : screen_Columns - 1;
	im_سیٹ_position(im_صف, im_خانہ);
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
    إذا (pango_font_description_get_ناپ(font) <= 0)
	pango_font_description_سیٹ_ناپ(font, 10 * PANGO_SCALE);

    أرجع font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name بن_ورتیا)
{
    إذا (font != NOFONT)
    {
	حرف *desc = pango_font_description_منزل_string(font);
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
    optexpand_T	*دلیلاں,
    فراغ	*param,
    صحيح		(*جوڑو_میل)(char_u *val))
{
    PangoFontFamily	**font_families = عدم;
    صحيح			n_families = 0;
    صحيح			wide = *(صحيح *)param;

    إذا (دلیلاں->oe_include_orig_val && *دلیلاں->oe_opt_قدر == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	إذا (جوڑو_میل((char_u *)DEFAULT_FONT) != OK)
	    أرجع;
    }

    pango_context_list_families(
	    gui.متن_context,
	    &font_families,
	    &n_families);

    لكل (صحيح i = 0; i < n_families; i++)
    {
	إذا (!wide && !pango_font_family_is_monospace(font_families[i]))
	    استمر;

	ثابت حرف *fam_name = pango_font_family_get_name(font_families[i]);
	إذا (داخلہ_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&داخلہ_conv,
		    (char_u *)fam_name, عدم);
	    إذا (buf != عدم)
	    {
		إذا (جوڑو_میل(buf) != OK)
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
	    إذا (جوڑو_میل((char_u *)fam_name) != OK)
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
    إذا (!gui.وچ_use)
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
color_منزل_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    أرجع rgba;
}

    فراغ
gui_mch_سیٹ_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_منزل_rgba(color);
}

    فراغ
gui_mch_سیٹ_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_منزل_rgba(color);
}

    فراغ
gui_mch_سیٹ_sp_color(guicolor_T color)
{
    *gui.spcolor = color_منزل_rgba(color);
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
gui_gtk4_update_ناپ(فراغ)
{
    vim_draw_area_سیٹ_ناپ(VIM_DRAW_AREA(gui.drawarea),
	    gui.عدد_صفاں, gui.عدد_خانے_گنتی);
}

#جے_تعریف_شدہ FEAT_NETBEANS_INTG
    فراغ
gui_gtk4_جوڑو_multisign(
	cairo_surface_t *surf,
	صحيح		صف,
	صحيح		خانہ,
	صحيح		چوڑائی,
	صحيح		اچائی)
{
    vim_draw_area_جوڑو_multisign(VIM_DRAW_AREA(gui.drawarea), surf, صف, خانہ,
	    چوڑائی, اچائی);
}
#جے_انت

    فراغ
gui_mch_clear_بلاک(صحيح row1, صحيح col1, صحيح row2, صحيح col2)
{
    vim_draw_area_clear_بلاک(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    إذا (gui.drawarea != عدم)
	gtk_widget_قطار_draw(gui.drawarea);
}

    فراغ
gui_mch_clear_all(فراغ)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    إذا (gui.drawarea != عدم)
	gtk_widget_قطار_draw(gui.drawarea);
}

    فراغ
gui_mch_delete_سطریں(صحيح صف, صحيح عدد_سطریں)
{
    vim_draw_area_move_بلاک(VIM_DRAW_AREA(gui.drawarea),
	    صف, صف + عدد_سطریں, gui.scroll_region_bot,
	    gui.scroll_region_بایاں, gui.scroll_region_دایاں);
    gui_clear_بلاک(
	    gui.scroll_region_bot - عدد_سطریں + 1, gui.scroll_region_بایاں,
	    gui.scroll_region_bot, gui.scroll_region_دایاں);

    gtk_widget_قطار_draw(gui.drawarea);
}

    فراغ
gui_mch_داخل_کرو_سطریں(صحيح صف, صحيح عدد_سطریں)
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
    فراغ
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, صحيح w, صحيح h)
{
    gui_mch_سیٹ_bg_color(bg);
    gui_mch_سیٹ_fg_color(fg);
    vim_draw_area_سیٹ_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_قطار_draw(gui.drawarea);
}

    فراغ
gui_mch_flash(صحيح msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (صحيح)صفاں - 1, (صحيح)Columns - 1);
    gui_mch_flush();
    ui_delay((لما)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (صحيح)صفاں - 1, (صحيح)Columns - 1);
}

    فراغ
gui_mch_invert_rectangle(صحيح r, صحيح c, صحيح nr, صحيح nc)
{
    vim_draw_area_invert_بلاک(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_قطار_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    ساكن gboolean
کلید_press_event(Gtk_Event_Controller_کلید *controller بن_ورتیا,
	guint کلید_sym, guint keycode بن_ورتیا,
	GdkModifierType state, gpointer انکڑے بن_ورتیا)
{
    char_u	string[32], string2[32];
    صحيح		len;
    صحيح		i;
    صحيح		modifiers;
    صحيح		کلید;
    char_u	*s, *d;

#جے_تعریف_شدہ FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    إذا (xic != عدم)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	إذا (event != عدم && gtk_im_context_filter_keypress(xic, event))
	    أرجع TRUE;
    }
#جے_انت

#جے_تعریف_شدہ FEAT_MENU
    إذا (کلید_sym == GDK_کلید_F10 && gui.menubar != عدم)
    {
	ساكن char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	إذا (check_map(k10, State, FALSE, TRUE, FALSE, عدم, عدم) == عدم)
	{
	    show_menubar_popover();
	    أرجع TRUE;
	}
    }
#جے_انت

    len = keyval_منزل_string(کلید_sym, string2);

    إذا (len > 1 && داخلہ_conv.vc_type != CONV_NONE)
	len = convert_داخلہ(string2, len, حجم(string2));

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
    إذا (کلید_sym == GDK_کلید_ISO_بایاں_Tab)
    {
	کلید_sym = GDK_کلید_Tab;
	state |= GDK_کھسکاؤ_نقاب;
    }

#جے_تعریف_شدہ FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    إذا ((state & GDK_ALT_نقاب)
	    && gui.menu_is_سرگرم
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	أرجع FALSE;
#جے_انت

    // Check for special keys
    إذا (len == 0 || len == 1)
    {
	لكل (i = 0; special_کلیدیں[i].کلید_sym != 0; i++)
	{
	    إذا (special_کلیدیں[i].کلید_sym == کلید_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیدیں[i].code0;
		string[2] = special_کلیدیں[i].code1;
		len = -3;
		اكسر;
	    }
	}
    }

    إذا (len == 0)
	أرجع TRUE;

    إذا (len == -3)
	کلید = منزل_SPECIAL(string[1], string[2]);
    وإلا
    {
	string[len] = NUL;
	کلید = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    کلید = simplify_کلید(کلید, &modifiers);
    إذا (کلید == CSI)
	کلید = K_CSI;
    إذا (IS_SPECIAL(کلید))
    {
	string[0] = CSI;
	string[1] = K_دوجا(کلید);
	string[2] = K_تیجا(کلید);
	len = 3;
    }
    وإلا
    {
	کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);
	modifiers = may_remove_کھسکاؤ_modifier(modifiers, کلید);
	len = mb_char2bytes(کلید, string);
    }

    إذا (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	جوڑو_منزل_داخلہ_buf(string2, 3);
    }

    {
	صحيح int_ch = check_for_رکاوٹ(کلید, modifiers);
	إذا (int_ch != NUL)
	{
	    trash_داخلہ_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    جوڑو_منزل_داخلہ_buf(string, len);

    إذا (p_mh)
	gui_mch_mousehide(TRUE);

    أرجع TRUE;
}

    ساكن فراغ
کلید_release_event(Gtk_Event_Controller_کلید *controller بن_ورتیا,
	guint keyval بن_ورتیا, guint keycode بن_ورتیا,
	GdkModifierType state بن_ورتیا, gpointer انکڑے بن_ورتیا)
{
}

ساكن صحيح mouse_timed_باہرلا = TRUE;
ساكن guint mouse_click_timer = 0;

    ساكن timeout_cb_type
mouse_click_timer_cb(gpointer انکڑے)
{
    *(صحيح *)انکڑے = TRUE;
    أرجع FALSE;
}

    ساكن صحيح
modifiers_gdk2mouse(guint state)
{
    صحيح modifiers = 0;

    إذا (state & GDK_کھسکاؤ_نقاب)
	modifiers |= MOUSE_کھسکاؤ;
    إذا (state & GDK_CONTROL_نقاب)
	modifiers |= MOUSE_CTRL;
    إذا (state & GDK_ALT_نقاب)
	modifiers |= MOUSE_ALT;

    أرجع modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#تعریف_کرو ANY_BUTTON_نقاب (GDK_BUTTON1_نقاب | GDK_BUTTON2_نقاب \
	| GDK_BUTTON3_نقاب | GDK_BUTTON4_نقاب | GDK_BUTTON5_نقاب)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
ساكن صحيح mouse_pressed_button = -1;

ساكن guint motion_repeat_timer = 0;
ساكن gboolean motion_repeat_وتھ = FALSE;
ساكن صحيح mouse_press_x = -1, mouse_press_y = -1;

    ساكن فراغ
button_press_event(GtkGestureClick *gesture, صحيح n_press بن_ورتیا,
	مزدوج x, مزدوج y, gpointer انکڑے بن_ورتیا)
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

    إذا (!mouse_timed_باہرلا && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_باہرلا = FALSE;
    mouse_click_timer = timeout_جوڑو(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_باہرلا);

    بدل (btn)
    {
	حالة 1: button = MOUSE_بایاں; اكسر;
	حالة 2: button = MOUSE_درمیان; اكسر;
	حالة 3: button = MOUSE_دایاں; اكسر;
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
button_release_event(GtkGestureClick *gesture, صحيح n_press بن_ورتیا,
	مزدوج x, مزدوج y, gpointer انکڑے بن_ورتیا)
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
mouse_repeat_timer_cb(gpointer انکڑے بن_ورتیا)
{
    صحيح x, y;

    إذا (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	أرجع G_سروت_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    إذا (vim_ورتیا_وچ_داخلہ_buf() > 10)
	أرجع G_سروت_CONTINUE;

    x = (صحيح)prev_mouse_x;
    y = (صحيح)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    إذا (motion_repeat_وتھ)
	x += gui.char_چوڑائی;
    motion_repeat_وتھ = !motion_repeat_وتھ;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    أرجع G_سروت_CONTINUE;
}

    ساكن فراغ
motion_notify_event(GtkEventControllerMotion *controller بن_ورتیا,
	مزدوج x, مزدوج y, gpointer انکڑے بن_ورتیا)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    إذا (mouse_pressed_button >= 0 && event != عدم
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_نقاب))
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
	w = gtk_widget_get_چوڑائی(gui.drawarea);
	h = gtk_widget_get_اچائی(gui.drawarea);

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

	    motion_repeat_timer = timeout_جوڑو(delay,
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
enter_notify_event(GtkEventControllerMotion *controller بن_ورتیا,
	مزدوج x, مزدوج y, gpointer انکڑے بن_ورتیا)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    ساكن gboolean
scroll_event(GtkEventControllerScroll *controller بن_ورتیا,
	مزدوج dx بن_ورتیا, مزدوج dy, gpointer انکڑے بن_ورتیا)
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
focus_وچ_event(GtkEventControllerFocus *controller بن_ورتیا,
	gpointer انکڑے بن_ورتیا)
{
    إذا (gtk_کھڑکی_is_سرگرم(GTK_کھڑکی(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	إذا (blink_state == BLINK_NONE)
	    gui_mch_شروع_blink();
    }
}

    ساكن فراغ
focus_باہرلا_event(GtkEventControllerFocus *controller بن_ورتیا,
	gpointer انکڑے بن_ورتیا)
{
    إذا (!gtk_کھڑکی_is_سرگرم(GTK_کھڑکی(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	إذا (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#إذا defined(FEAT_IMAGE)
    ساكن فراغ
scale_ضارب_cb(GdkSurface  *surface,
	GParamSpec	    *pspec بن_ورتیا,
	فراغ		    *udata بن_ورتیا)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#جے_انت

    ساكن فراغ
drawarea_realize_cb(GtkWidget *widget بن_ورتیا, gpointer انکڑے بن_ورتیا)
{
#إذا defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_ضارب_cb), عدم);
    redraw_all_later(UPD_VALID);
#جے_انت
    gui_mch_new_colors();
}

    ساكن فراغ
drawarea_unrealize_cb(GtkWidget *widget بن_ورتیا, gpointer انکڑے بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_XIM
    im_shutdown();
#جے_انت
}

تعريفنوع تعداد
{
    READ_انکڑے_CLIP,	    // Clipboard data
    READ_انکڑے_DROP,	    // URI or text DND data
    READ_انکڑے_DROP_انکڑے	    // HTML DND data
} Read_انکڑے_Type;

#تعریف_کرو READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
تعريفنوع بنية
{
    Read_انکڑے_Type    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    حرف	    *buf; // NULL when not needed
} Read_انکڑے;

#جے_تعریف_شدہ FEAT_DND
تعريفنوع بنية
{
    Read_انکڑے	rd;
    GdkDrop	*drop;
    صحيح		x;
    صحيح		y;
} Drop_Read_انکڑے;
#جے_انت

تعريفنوع بنية
{
    Read_انکڑے	rd;
    Clipboard_T *cbd;
    حرف	*mime_type;
} Clip_Read_انکڑے;

    ساكن Read_انکڑے *
read_انکڑے_new(Read_انکڑے_Type type, size_t sz)
{
    Read_انکڑے *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    إذا (type == READ_انکڑے_CLIP || type == READ_انکڑے_DROP_انکڑے)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    أرجع rd;
}

    ساكن فراغ
read_انکڑے_free(Read_انکڑے *rd)
{
    g_cancellable_cancel(rd->cancel);

    إذا (rd->type == READ_انکڑے_CLIP)
	g_free(((Clip_Read_انکڑے *)rd)->mime_type);
    وإلا إذا (rd->type == READ_انکڑے_DROP || rd->type == READ_انکڑے_DROP_انکڑے)
	g_object_unref(((Drop_Read_انکڑے *)rd)->drop);
    g_object_unref(rd->cancel);
    إذا (rd->arr != عدم)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#جے_تعریف_شدہ FEAT_DND
ساكن فراغ drop_read_متن(GdkDrop *drop, char_u *متن);
#جے_انت

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    ساكن فراغ
read_انکڑے_داخلہ_cb(
	GInput_دھارا	*دھارا,
	GAsync_نتیجہ	*نتیجہ,
	Read_انکڑے	*rd)
{
    ssize_t r = g_داخلہ_دھارا_read_انت(دھارا, نتیجہ, عدم);

    إذا (r == -1)
    {
	// Error occurred
	Drop_Read_انکڑے *drd = (Drop_Read_انکڑے *)rd;

	إذا (rd->type == READ_انکڑے_DROP_انکڑے)
	    gdk_drop_انت(drd->drop, 0);
    }
    وإلا إذا (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	إذا (rd->type == READ_انکڑے_CLIP)
	{
	    Clip_Read_انکڑے    *crd = (Clip_Read_انکڑے *)rd;
	    لما	    len;
	    char_u	    *actual, *final;
	    صحيح		    motion_type = MAUTO;
	    char_u	    *tofree = عدم;

	    len = (لما)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = عدم;

	    إذا (clip_convert_انکڑے(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#جے_تعریف_شدہ FEAT_DND
	وإلا إذا (rd->type == READ_انکڑے_DROP_انکڑے)
	{
	    Drop_Read_انکڑے    *drd = (Drop_Read_انکڑے *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ثابت uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = عدم;
	    drop_read_متن(drd->drop, str);
	    g_free(str);
	    gdk_drop_انت(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#جے_انت
    }
    وإلا
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ثابت uint8_t *)rd->buf, r);
	g_داخلہ_دھارا_read_async(دھارا, rd->buf, READDATA_BUFSIZE,
		G_ترجیح_بلند,
		rd->cancel, (GAsyncReadyCallback)read_انکڑے_داخلہ_cb, rd);
	أرجع;
    }

    read_انکڑے_free(rd);
    g_object_unref(دھارا);
}

#جے_تعریف_شدہ FEAT_DND
/*
 * Set up for receiving DND items.
 */
    فراغ
gui_gtk_سیٹ_dnd_targets(فراغ)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_جوڑو_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_جوڑو_gtype(builder, G_TYPE_STRING);
    إذا (clip_html)
	gdk_content_formats_builder_جوڑو_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_منزل_formats(builder);
    gtk_drop_نشانہ_async_سیٹ_formats(gui.drop_نشانہ, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    ساكن فراغ
drop_read_متن(GdkDrop *drop بن_ورتیا, char_u *متن)
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

    إذا (متن == عدم || *متن == NUL || !query_اشارہ_گر_pos(عدم, عدم, &state))
	أرجع;

    dnd_yank_drag_انکڑے((char_u *)متن, (لما)STRLEN(متن));

    dropkey[2] = modifiers_gdk2vim(state);
    إذا (dropkey[2] != 0)
	جوڑو_منزل_داخلہ_buf(dropkey, (صحيح)حجم(dropkey));
    وإلا
	جوڑو_منزل_داخلہ_buf(dropkey + 3, (صحيح)(حجم(dropkey) - 3));
}

    ساكن فراغ
drop_read_قدر_cb(GdkDrop *drop, GAsync_نتیجہ *نتیجہ, Drop_Read_انکڑے *drd)
{
    ثابت GValue    *قدر = gdk_drop_read_قدر_انت(drop, نتیجہ, عدم);
    gboolean	    success = FALSE;

    إذا (قدر == عدم)
	اذهب باہر_نکلو;

    إذا (G_قدر_HOLDS(قدر, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_قدر_get_boxed(قدر);
	صحيح	nfiles = g_slist_لمائی(files);
	char_u	**fnames;
	صحيح	i;

	إذا (nfiles <= 0)
	    اذهب باہر_نکلو;

	fnames = ALLOC_MULT(char_u *, nfiles);
	إذا (fnames == عدم)
	    اذهب باہر_نکلو;

	i = 0;
	لكل (GSList *l = files; l != عدم; l = l->اگلا)
	{
	    GFile *file = l->انکڑے;
	    حرف *رستہ = g_file_get_رستہ(file);
	    إذا (رستہ != عدم)
		fnames[i++] = vim_strsave((char_u *)رستہ);
	    g_free(رستہ);
	}
	nfiles = i;

	إذا (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	وإلا
	    vim_free(fnames);
	success = TRUE;
    }
    وإلا إذا (G_قدر_HOLDS(قدر, G_TYPE_STRING))
    {
	ثابت حرف *متن = g_قدر_get_string(قدر);

	إذا (متن == عدم || *متن == NUL)
	    اذهب باہر_نکلو;

	drop_read_متن(drop, (char_u *)متن);
	success = TRUE;
    }

باہر_نکلو:
    gdk_drop_انت(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_انکڑے_free(&drd->rd);
}

    ساكن فراغ
drop_read_cb(GdkDrop *drop, GAsync_نتیجہ *نتیجہ, Drop_Read_انکڑے *drd)
{
    GInput_دھارا    *وچ_دھارا;
    ثابت حرف	    *m;

    وچ_دھارا = gdk_drop_read_انت(drop, نتیجہ, &m, عدم);
    إذا (وچ_دھارا == عدم || STRCMP(m, "text/html") != 0)
    {
	read_انکڑے_free(&drd->rd);
	gdk_drop_انت(drop, 0);
	أرجع;
    }

    أكد(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_داخلہ_دھارا_read_async(وچ_دھارا, drd->rd.buf, READDATA_BUFSIZE,
	    G_ترجیح_بلند,
	    drd->rd.cancel, (GAsyncReadyCallback)read_انکڑے_داخلہ_cb, drd);
}

// GCancellable of current DND operation, else NULL.
ساكن GCancellable *dnd_cancel = عدم;
ساكن guint dnd_timeout_id = 0;

    ساكن gboolean
drop_timeout_cb(فراغ *udata بن_ورتیا)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    أرجع G_سروت_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    ساكن gboolean
drop_cb(
	Gtk_Drop_نشانہ_Async  *نشانہ بن_ورتیا,
	GdkDrop		    *drop,
	مزدوج		    x,
	مزدوج		    y,
	فراغ		    *udata بن_ورتیا)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    Drop_Read_انکڑے	*drd = عدم;

    إذا (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (Drop_Read_انکڑے *)read_انکڑے_new(READ_انکڑے_DROP, حجم(*drd));
	gdk_drop_read_قدر_async(drop, GDK_TYPE_FILE_LIST, G_ترجیح_بلند,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_قدر_cb, drd);
    }
    وإلا إذا (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	ساكن ثابت حرف *m[] = {"text/html", عدم};

	drd = (Drop_Read_انکڑے *)read_انکڑے_new(READ_انکڑے_DROP_انکڑے, حجم(*drd));
	gdk_drop_read_async(drop, m, G_ترجیح_بلند,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    وإلا إذا (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (Drop_Read_انکڑے *)read_انکڑے_new(READ_انکڑے_DROP, حجم(*drd));
	gdk_drop_read_قدر_async(drop, G_TYPE_STRING, G_ترجیح_بلند,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_قدر_cb, drd);
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
	dnd_timeout_id = timeout_جوڑو(3000, drop_timeout_cb, عدم);

	أرجع TRUE;
    }
    أرجع FALSE;
}
#جے_انت

    ساكن فراغ
mainwin_destroy_cb(GObject *object بن_ورتیا, gpointer انکڑے بن_ورتیا)
{
    gui.mainwin = عدم;
    gui.drawarea = عدم;
    إذا (!exiting)
	gui_shell_closed();
}

    ساكن gboolean
delete_event_cb(Gtk_کھڑکی *کھڑکی_2 بن_ورتیا, gpointer انکڑے بن_ورتیا)
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
داخلہ_timer_cb(gpointer انکڑے)
{
    صحيح *timed_باہرلا = (صحيح *)انکڑے;

    *timed_باہرلا = TRUE;
    أرجع FALSE;	// don't call me again
}

    فراغ
gui_mch_update(فراغ)
{
    صحيح cnt = 0;

    طالما (g_main_context_pending(عدم) && !vim_is_داخلہ_buf_بھریا()
								&& ++cnt < 100)
	g_main_context_iteration(عدم, TRUE);
}

#جے_تعریف_شدہ FEAT_کم_CHANNEL
    ساكن timeout_cb_type
channel_poll_cb(gpointer انکڑے بن_ورتیا)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    أرجع TRUE; // Keep repeating
}
#جے_انت

    صحيح
gui_mch_wait_for_chars(لما wtime)
{
    صحيح		focus;
    guint	timer;
    ساكن صحيح	timed_باہرلا;
    صحيح		retval = FAIL;
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    guint	channel_timer = 0;
#جے_انت

    timed_باہرلا = FALSE;

    إذا (wtime >= 0)
	timer = timeout_جوڑو(wtime == 0 ? 1L : wtime,
						   داخلہ_timer_cb, &timed_باہرلا);
    وإلا
	timer = 0;

#جے_تعریف_شدہ FEAT_کم_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    إذا (channel_any_keep_open())
	channel_timer = timeout_جوڑو(20, channel_poll_cb, عدم);
#جے_انت

    focus = gui.وچ_focus;

    افعل
    {
	// Stop or start blinking when focus changes
	إذا (gui.وچ_focus != focus)
	{
	    إذا (gui.وچ_focus)
		gui_mch_شروع_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.وچ_focus;
	}

#جے_تعریف_شدہ سنیہا_قطار
# ifdef FEAT_TIMERS
	did_جوڑو_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جوڑو_timer)
	    اذهب theend;
# endif
#جے_انت

	إذا (gui.mainwin == عدم)
	    اذهب theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	إذا (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    اذهب theend;
	}

	إذا (!داخلہ_دستیاب())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(عدم, TRUE);
	    --gtk4_main_loop_level;
	}

	إذا (داخلہ_دستیاب())
	{
	    retval = OK;
	    اذهب theend;
	}
    } طالما (wtime < 0 || !timed_باہرلا);

    gui_mch_update();

theend:
    إذا (timer != 0 && !timed_باہرلا)
	timeout_remove(timer);
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    إذا (channel_timer != 0)
	timeout_remove(channel_timer);
#جے_انت

    أرجع retval;
}

    فراغ
gui_mch_flush(فراغ)
{
    // Ensure the offscreen surface content gets painted to the widget.
    إذا (gui.drawarea != عدم)
	gtk_widget_قطار_draw(gui.drawarea);
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
    gtk_کھڑکی_minimize(GTK_کھڑکی(gui.mainwin));
}

    فراغ
gui_mch_سیٹ_foreground(فراغ)
{
    gtk_کھڑکی_present(GTK_کھڑکی(gui.mainwin));
}

    ساكن صحيح
query_اشارہ_گر_pos(صحيح *x, صحيح *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *اشارہ_گر;
    مزدوج	     sx, sy, nx, ny;
    graphene_نقطہ_t src, dst;

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
    اشارہ_گر = gdk_seat_get_اشارہ_گر(seat);
    إذا (اشارہ_گر == عدم)
	أرجع FALSE;

    إذا (!gdk_surface_get_device_position(surface, اشارہ_گر, &sx, &sy, state))
	أرجع FALSE;

    إذا (x != عدم && y != عدم)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (عائم)(sx - nx);
	src.y = (عائم)(sy - ny);
	إذا (!gtk_widget_compute_نقطہ(GTK_WIDGET(native), gui.drawarea,
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
    إذا (!query_اشارہ_گر_pos(x, y, عدم))
    {
	*x = 0;
	*y = 0;
    }
}

    فراغ
gui_mch_setmouse(صحيح x بن_ورتیا, صحيح y بن_ورتیا)
{
    // GTK4/Wayland: cannot warp pointer
}

    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (gui.اشارہ_گر_hidden == hide)
	أرجع;

    gui.اشارہ_گر_hidden = hide;
    إذا (gui.blank_اشارہ_گر != عدم)
    {
	إذا (hide)
	    gtk_widget_سیٹ_cursor(gui.drawarea, gui.blank_اشارہ_گر);
	وإلا
#جے_تعریف_شدہ FEAT_MOUSESHAPE
	    mch_سیٹ_mouse_shape(آخری_shape);
#وإلا
	    gtk_widget_سیٹ_cursor(gui.drawarea, عدم);
#جے_انت
    }
}

    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_کلیدیں[i].کلید_sym != 0; i++)
	إذا (name[0] == special_کلیدیں[i].code0
		&& name[1] == special_کلیدیں[i].code1)
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
gui_gtk4_update_scrollbar_ناپ(فراغ)
{
    win_T	*wp;
    صحيح		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_بایاں].id;

	إذا (sbar == عدم || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_بایاں]
		    && wp->w_scrollbars[SBAR_دایاں].id != عدم))
	    sbar = wp->w_scrollbars[SBAR_دایاں].id;

	إذا (sbar != عدم && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    صحيح		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_ناپ(sbar, &min, &nat);
	    sw = MAX(min.چوڑائی, nat.چوڑائی);
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

	gtk_widget_get_preferred_ناپ(sbar, &min, &nat);
	sh = MAX(min.اچائی, nat.اچائی);

	إذا (sh > 0)
	    h = sh;
    }

    إذا (w != -1)
	gui.scrollbar_چوڑائی = w;
    إذا (h != -1)
	gui.scrollbar_اچائی = h;
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

	gtk_widget_سیٹ_visible(sb->id, flag);

	إذا (!was_visible && flag)
	    gui_gtk4_update_scrollbar_ناپ();
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
    gtk_widget_سیٹ_sensitive(menu->id, !grey);
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
    gtk_widget_سیٹ_visible(menu->id, !hidden);
    gui_mch_update();
}

    فراغ
gui_mch_draw_menubar(فراغ)
{
    // Just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#جے_انت

/*
 * ============================================================
 * Tabline
 * ============================================================
 */

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    إذا (gui.tabline == عدم)
	أرجع;
    gtk_widget_سیٹ_visible(gui.tabline, showit);
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
	GtkEventController  *controller بن_ورتیا,
	مزدوج		    x بن_ورتیا,
	مزدوج		    y بن_ورتیا,
	فراغ		    *udata)
{
    tabpage_hover = GPOINTER_منزل_INT(udata);
}

    ساكن فراغ
tabline_enter_cb(
	GtkEventController  *controller بن_ورتیا,
	مزدوج		    x بن_ورتیا,
	مزدوج		    y بن_ورتیا,
	فراغ		    *udata بن_ورتیا)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    ساكن فراغ
tabline_جوڑو_css(فراغ)
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
# إذا GTK_CHECK_نسخہ(4, 12, 0)
    gtk_css_provider_لدو_from_string(provider, css);
# وإلا
    gtk_css_provider_لدو_from_انکڑے(provider, css, -1);
# endif
    gtk_style_context_جوڑو_provider_for_display(
	    gtk_widget_get_display(gui.tabline), GTK_STYLE_PROVIDER(provider),
	    GTK_STYLE_PROVIDER_ترجیح_APPLICATION);
    g_object_unref(provider);
}

/*
 * Handle clicking the close button of a tab.
 */
    ساكن فراغ
tab_close_clicked_cb(GtkButton *button بن_ورتیا, gpointer انکڑے)
{
    صحيح idx = gtk_notebook_ورقہ_عدد(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(انکڑے));

    إذا (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    فراغ
gui_mch_update_tabline(فراغ)
{
    GtkWidget	*ورقہ;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    صحيح		nr = 0;
    صحيح		tab_عدد;
    صحيح		curtabidx = 0;
    char_u	*labeltext;

    إذا (gui.tabline == عدم)
	أرجع;

    ignore_tabline_evt = TRUE;

    لكل (tp = پہلا_tabpage; tp != عدم; tp = tp->tp_اگلا, ++nr)
    {
	Gtk_Notebook_ورقہ	    *ورقہ_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	إذا (tp == curtab)
	    curtabidx = nr;

	tab_عدد = nr + 1;

	ورقہ = gtk_notebook_get_nth_ورقہ(GTK_NOTEBOOK(gui.tabline), nr);
	إذا (ورقہ == عدم)
	{
	    GtkWidget	*spacer;
	    GtkWidget	*close_button;

	    ورقہ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_widget_سیٹ_visible(ورقہ, TRUE);
	    event_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_سیٹ_visible(event_box, TRUE);

	    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_جوڑو_css_class(spacer, "vim-tab-spacer");
	    gtk_box_append(GTK_BOX(event_box), spacer);

	    label = gtk_label_new("-Empty-");
	    gtk_widget_سیٹ_hexpand(label, TRUE);
	    gtk_label_سیٹ_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_سرا);
	    gtk_box_append(GTK_BOX(event_box), label);
	    gtk_widget_سیٹ_visible(label, TRUE);
	    g_object_سیٹ_انکڑے(G_OBJECT(event_box), "label", label);

	    close_button = gtk_button_new_from_icon_name("window-close-symbolic");
	    gtk_widget_جوڑو_css_class(close_button, "flat");
	    gtk_widget_جوڑو_css_class(close_button, "circular");
	    gtk_widget_سیٹ_valign(close_button, GTK_ALIGN_CENTER);
	    gtk_widget_سیٹ_focus_on_click(close_button, FALSE);
	    gtk_widget_سیٹ_tooltip_متن(close_button, _("Close Tab"));
	    g_signal_connect(close_button, "clicked",
		    G_CALLBACK(tab_close_clicked_cb), ورقہ);
	    gtk_box_append(GTK_BOX(event_box), close_button);

	    gtk_notebook_داخل_کرو_ورقہ(GTK_NOTEBOOK(gui.tabline),
		    ورقہ, event_box, nr);
	    gtk_notebook_سیٹ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    ورقہ, TRUE);
	    g_object_سیٹ(gtk_notebook_get_ورقہ(GTK_NOTEBOOK(gui.tabline), ورقہ),
		    "tab-expand", TRUE, عدم);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	ورقہ_widget = gtk_notebook_get_ورقہ(GTK_NOTEBOOK(gui.tabline), ورقہ);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_منزل_اشارہ_گر(tab_عدد));
	gtk_event_controller_سیٹ_پسار_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(ورقہ_widget, "tab", &tab_widget, عدم);
	gtk_widget_جوڑو_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), ورقہ);
	label = g_object_get_انکڑے(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_منزل_UTF8(NameBuff);
	إذا (label != عدم && GTK_IS_LABEL(label))
	    gtk_label_سیٹ_متن(GTK_LABEL(label), (ثابت حرف *)labeltext);
	CONVERT_منزل_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_منزل_UTF8(NameBuff);
	gtk_widget_سیٹ_tooltip_متن(event_box, (ثابت gchar *)labeltext);
	CONVERT_منزل_UTF8_FREE(labeltext);
    }

    طالما (gtk_notebook_get_nth_ورقہ(GTK_NOTEBOOK(gui.tabline), nr) != عدم)
	gtk_notebook_remove_ورقہ(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    إذا (nr == 1)
	gtk_widget_جوڑو_css_class(gui.tabline, "single-tab");
    وإلا
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    إذا (gtk_notebook_get_current_ورقہ(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_سیٹ_current_ورقہ(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    فراغ
gui_mch_سیٹ_curtab(صحيح nr)
{
    إذا (gui.tabline != عدم)
	gtk_notebook_سیٹ_current_ورقہ(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    ساكن فراغ
on_select_tab(
	GtkNotebook	*notebook بن_ورتیا,
	gpointer	*ورقہ بن_ورتیا,
	gint		idx,
	gpointer	انکڑے بن_ورتیا)
{
    إذا (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    ساكن فراغ
on_tab_reordered(
	GtkNotebook	*notebook بن_ورتیا,
	gpointer	*ورقہ بن_ورتیا,
	gint		idx,
	gpointer	انکڑے بن_ورتیا)
{
    إذا (ignore_tabline_evt)
	أرجع;

    إذا ((tabpage_اشاریہ(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    وإلا
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ساكن فراغ
tabline_menu_event_cb(Vim_Menu_شے *شے بن_ورتیا, Vim_Menu_شے_Event event, فراغ *udata)
{
    إذا (event == VIM_MENU_شے_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_منزل_INT(udata));
}

    ساكن فراغ
جوڑو_tabline_menu_شے(VimMenu *menu, ثابت حرف *name, صحيح resp)
{
    Vim_Menu_شے *شے = VIM_MENU_شے(vim_menu_شے_new(name,
		tabline_menu_event_cb, GINT_منزل_اشارہ_گر(resp)));

    vim_menu_داخل_کرو_شے(menu, شے, -1);
}

/*
 * Create a menu for the tab line.
 */
    ساكن VimMenu *
create_tabline_popup_menu(فراغ)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    جوڑو_tabline_menu_شے(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    جوڑو_tabline_menu_شے(menu, _("New Tab"), TABLINE_MENU_NEW);
    جوڑو_tabline_menu_شے(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    أرجع menu;
}

    ساكن فراغ
tabline_menu_press_event(
	GtkGestureClick *gesture,
	صحيح		n_press بن_ورتیا,
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
	rect.چوڑائی = 1;
	rect.اچائی = 1;

	gtk_popover_سیٹ_pointing_منزل(GTK_POPOVER(popover), &rect);
	gtk_popover_popup(GTK_POPOVER(popover));
    }
    وإلا إذا (but == GDK_BUTTON_PRIMARY)
    {
	إذا (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    وإلا إذا (but == GDK_BUTTON_درمیان)
    {
	إذا (tabpage_hover != 0)
	    // Middle mouse click on tabpage label closes that tab.
	    send_tabline_menu_event(tabpage_hover, TABLINE_MENU_CLOSE);
    }
}
#جے_انت

/*
 * ============================================================
 * Sign support
 * ============================================================
 */

#إذا defined(FEAT_SIGN_ICONS)
# define SIGN_چوڑائی  (2 * gui.char_چوڑائی)
# define SIGN_اچائی (gui.char_اچائی)

    فراغ
gui_mch_drawsign(صحيح صف, صحيح خانہ, صحيح typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    إذا (sign == عدم)
	أرجع;

    vim_draw_area_جوڑو_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    صف, خانہ, SIGN_چوڑائی, SIGN_اچائی);

    gtk_widget_قطار_draw(gui.drawarea);
}

    فراغ *
gui_mch_رجسٹر_sign(char_u *signfile)
{
    إذا (signfile[0] != NUL && signfile[0] != '-' && gui.وچ_use)
    {
	GError	    *error = عدم;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ثابت حرف *)signfile,
							    &error);
	إذا (sign != عدم)
	    أرجع sign;

	semsg("E255: %s", error->سنیہا);
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
#جے_انت

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
gui_gtk_draw_string(صحيح صف, صحيح خانہ, char_u *s, صحيح len, صحيح flags)
{
    صحيح cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    صف, خانہ, s, len, flags);

    إذا (gui.drawarea != عدم)
	gtk_widget_قطار_draw(gui.drawarea);

    أرجع cells;
}

    صحيح
gui_get_x11_windis(کھڑکی *win بن_ورتیا, Display **dis بن_ورتیا)
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
translate_mnemonic_ٹیگ(char_u *name, صحيح use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    صحيح	    n_underscores = 0;

    name = CONVERT_منزل_UTF8(name);
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

    CONVERT_منزل_UTF8_FREE(name);
    أرجع buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    فراغ
gui_gtk_سیٹ_mnemonics(صحيح enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	إذا (menu->id == عدم)
	    استمر;

	name = translate_mnemonic_ٹیگ(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	إذا (VIM_IS_MENU_BAR_شے(menu->id))
	    vim_menu_bar_شے_سیٹ_متن(VIM_MENU_BAR_شے(menu->id),
		    (ثابت حرف *)name);
	vim_free(name);
    }
}

    ساكن فراغ
popupmenu_closed_cb(GtkWidget *popover, فراغ *udata بن_ورتیا)
{
    gtk_widget_unparent(popover);
    إذا (gui.drawarea != عدم)
	gtk_widget_قطار_draw(gui.drawarea);
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
    gtk_widget_سیٹ_ماپا(popover, gui.drawarea);

    rect.x = x;
    rect.y = y;
    rect.چوڑائی = rect.اچائی = 1;

    // Make sure popover aligns down-and-to-the-right of the pointer.
    gtk_popover_سیٹ_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_widget_سیٹ_halign(popover, GTK_ALIGN_شروع);
    gtk_popover_سیٹ_pointing_منزل(GTK_POPOVER(popover), &rect);

    g_signal_connect(GTK_POPOVER(popover), "closed",
	    G_CALLBACK(popupmenu_closed_cb), عدم);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    فراغ
gui_make_popup(char_u *رستہ_name, صحيح mouse_pos)
{
    vimmenu_T	*menu;
    صحيح		x, y;

    menu = gui_لبھو_menu(رستہ_name);
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
#جے_انت // FEAT_MENU

    صحيح
get_menu_tool_چوڑائی(فراغ)
{
    أرجع 0;
}

    صحيح
get_menu_tool_اچائی(فراغ)
{
    GtkWidget *widgets[] = {
#جے_تعریف_شدہ FEAT_MENU
	gui.menubar,
#جے_انت
#جے_تعریف_شدہ FEAT_TOOLBAR
	gui.toolbar,
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_TABLINE
	gui.tabline
#جے_انت
    };

    صحيح اچائی = 0;

    لكل (صحيح i = 0; i < (صحيح)ARRAY_لمائی(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	صحيح		h;

	إذا (widgets[i] == عدم || !gtk_widget_get_visible(widgets[i]))
	    استمر;

	h = gtk_widget_get_اچائی(widgets[i]);

	إذا (h == 0)
	{
	    // Allocation hasn't been updated yet (widget just became visible).
	    // Query the preferred height so the caller gets a valid value
	    // before the layout pass runs.  Use the maximum of minimum and
	    // natural height: GTK may allocate min_h even when natural_h is
	    // smaller (e.g. GtkNotebook tab bar has min_h > natural_h due to
	    // CSS).
	    gtk_widget_get_preferred_ناپ(widgets[i], &min, &nat);
	    اچائی += MAX(min.اچائی, nat.اچائی);
	}
	وإلا
	    اچائی += h;
    }
    أرجع اچائی;
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
	gdk_content_provider_content_بدلیا(gui.regular_provider);
    وإلا
	gdk_content_provider_content_بدلیا(gui.primary_provider);
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
clip_read_cb(GdkClipboard *cb, GAsync_نتیجہ *نتیجہ, Clip_Read_انکڑے *crd)
{
    GInput_دھارا    *وچ_دھارا;
    ثابت حرف	    *mime_type;

    وچ_دھارا = gdk_clipboard_read_انت(cb, نتیجہ, &mime_type, عدم);
    إذا (وچ_دھارا == عدم)
    {
	read_انکڑے_free(&crd->rd);
	أرجع;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_داخلہ_دھارا_read_async(وچ_دھارا, crd->rd.buf, READDATA_BUFSIZE,
	    G_ترجیح_بلند,
	    crd->rd.cancel, (GAsyncReadyCallback)read_انکڑے_داخلہ_cb, crd);
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
    Clip_Read_انکڑے *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, عدم);
    إذا (clipboard == عدم)
	أرجع;

    crd = (Clip_Read_انکڑے *)read_انکڑے_new(READ_انکڑے_CLIP, حجم(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_ترجیح_بلند, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t شروع;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	شروع = ویلا(عدم);
	طالما (!g_cancellable_is_cancelled(cancel)
		&& ویلا(عدم) < شروع + 3)
	    g_main_context_iteration(عدم, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

ساكن صحيح وچ_clipboard_سیٹ = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    فراغ
clip_mch_سیٹ_selection(Clipboard_T *cbd بن_ورتیا)
{
}

    ساكن فراغ
clipboard_بدلیا_cb(GdkClipboard *clipboard, gpointer user_انکڑے)
{
    Clipboard_T *cbd = (Clipboard_T *)user_انکڑے;

    إذا (وچ_clipboard_سیٹ)
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

    وچ_clipboard_سیٹ = TRUE;
    ret = gdk_clipboard_سیٹ_content(cb, cp);
    وچ_clipboard_سیٹ = FALSE;

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
	gdk_clipboard_سیٹ_content(clipboard, عدم);
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
ساكن ثابت حرف *mshape_css_ناں[] =
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
mch_سیٹ_mouse_shape(صحيح shape)
{
    GdkCursor	*c;
    ثابت حرف	*css_name = "default";

    إذا (gui.drawarea == عدم)
	أرجع;

    إذا (shape == MSHAPE_HIDE || gui.اشارہ_گر_hidden)
	gtk_widget_سیٹ_cursor(gui.drawarea, gui.blank_اشارہ_گر);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	وإلا إذا (shape < (صحيح)ARRAY_لمائی(mshape_css_ناں))
	    css_name = mshape_css_ناں[shape];
	وإلا
	    أرجع;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, عدم);
	gtk_widget_سیٹ_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    إذا (shape != MSHAPE_HIDE)
	آخری_shape = shape;
}

#جے_انت // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



ساكن صحيح آخری_متن_area_w = 0;
ساكن صحيح آخری_متن_area_h = 0;

/*
 * ============================================================
 * Menu functions
 * ============================================================
 */

#جے_تعریف_شدہ FEAT_MENU

# ifdef FEAT_TOOLBAR
/*
 * Icon name table for toolbar buttons.
 * Must match toolbar_names[] in menu.c.
 */
ساكن ثابت حرف * ثابت toolbar_icon_ناں[] =
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
toolbar_button_clicked_cb(GtkWidget *widget بن_ورتیا, gpointer انکڑے)
{
    gui_menu_cb((vimmenu_T *)انکڑے);
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
		gtk_widget_سیٹ_ناپ_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    إذا (image == عدم)
    {
	ثابت حرف *icon_name = "image-missing";
	صحيح n = (صحيح)ARRAY_لمائی(toolbar_icon_ناں);

	إذا (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_ناں[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    أرجع image;
}
# endif

    ساكن فراغ
menu_button_clicked_cb(
	Vim_Menu_شے	    *شے بن_ورتیا,
	Vim_Menu_شے_Event    event,
	vimmenu_T	    *menu)
{
    إذا (event == VIM_MENU_شے_CLICKED)
	gui_menu_cb(menu);
    وإلا إذا (event == VIM_MENU_شے_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	ساكن gboolean did_پیغام = FALSE;

	إذا (State & منوال_CMDLINE)
	    أرجع;

	tooltip = CONVERT_منزل_UTF8(menu->لڑیاں[MENU_اشاریہ_TIP]);
	إذا (tooltip != عدم && utf_valid_string(tooltip, عدم))
	{
	    پیغام((حرف *)tooltip);
	    did_پیغام = TRUE;
	    setcursor();
	    باہرلا_flush_cursor(TRUE, FALSE);
	}
	وإلا إذا (did_پیغام)
	{
	    پیغام("");
	    did_پیغام = FALSE;
	    setcursor();
	    باہرلا_flush_cursor(TRUE, FALSE);
	}
	CONVERT_منزل_UTF8_FREE(tooltip);
    }
}

    فراغ
gui_mch_جوڑو_menu(vimmenu_T *menu, صحيح idx)
{
    vimmenu_T	*ماپا;
    GtkWidget	*ماپا_widget;
    gboolean	use_mnemonic;
    char_u	*متن;

    إذا (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_سیٹ_ماپا(menu->submenu_id, gui.drawarea);
	أرجع;
    }

    ماپا = menu->ماپا;

    إذا ((ماپا != عدم && ماپا->submenu_id == عدم)
	    || !menu_is_menubar(menu->name))
	أرجع;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = ماپا != عدم || p_wak[0] != 'n';
    متن = translate_mnemonic_ٹیگ(menu->name, use_mnemonic);

    إذا (ماپا != عدم)
    {
	ماپا_widget = ماپا->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_شے_new(
		    (ثابت حرف *)متن, عدم, عدم));

	vim_menu_شے_سیٹ_submenu(VIM_MENU_شے(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_داخل_کرو_شے(VIM_MENU(ماپا_widget),
		VIM_MENU_شے(menu->id), idx);
    }
    وإلا
    {
	ماپا_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_شے_new(
		    (ثابت حرف *)متن, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_داخل_کرو_شے(VIM_MENU_BAR(ماپا_widget),
		VIM_MENU_BAR_شے(menu->id), idx);
    }

    vim_free(متن);
}

    فراغ
gui_mch_جوڑو_menu_شے(vimmenu_T *menu, صحيح idx)
{
    vimmenu_T	*ماپا = menu->ماپا;

# ifdef FEAT_TOOLBAR
    إذا (ماپا != عدم && menu_is_toolbar(ماپا->name))
    {
	إذا (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_داخل_کرو_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	وإلا
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*متن;
	    char_u	*tooltip;

	    متن    = CONVERT_منزل_UTF8(menu->dname);
	    tooltip = CONVERT_منزل_UTF8(menu->لڑیاں[MENU_اشاریہ_TIP]);
	    إذا (tooltip != عدم && !utf_valid_string(tooltip, عدم))
		CONVERT_منزل_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_داخل_کرو_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ثابت حرف *)متن, idx);
	    gtk_widget_سیٹ_tooltip_متن(btn, (ثابت gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_منزل_UTF8_FREE(متن);
	    CONVERT_منزل_UTF8_FREE(tooltip);
	}
	أرجع;
    }
# endif

    // Menu items (non-toolbar)
    إذا (ماپا == عدم || ماپا->submenu_id == عدم)
	أرجع;

    إذا (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_داخل_کرو_separator(
		VIM_MENU(ماپا->submenu_id), idx));
    }
    وإلا
    {
	char_u	    *متن;
	char_u	    *accel_متن = عدم;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	متن = translate_mnemonic_ٹیگ(menu->name, use_mnemonic);

	إذا (menu->actext != عدم && menu->actext[0] != NUL)
	    accel_متن = CONVERT_منزل_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_شے_new((ثابت حرف *)متن,
		    (Vim_Menu_شے_Func)menu_button_clicked_cb, menu));

	إذا (accel_متن != عدم)
	    vim_menu_شے_سیٹ_accel(VIM_MENU_شے(menu->id),
		    (ثابت حرف *)accel_متن);

	vim_menu_داخل_کرو_شے(VIM_MENU(ماپا->submenu_id),
		VIM_MENU_شے(menu->id), idx);

	vim_free(متن);
	CONVERT_منزل_UTF8_FREE(accel_متن);
    }
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable بن_ورتیا)
{
    // GTK4: tearoff menus don't exist.
}

    فراغ
gui_mch_menu_سیٹ_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    إذا (menu->id == عدم)
	أرجع;

    tooltip = CONVERT_منزل_UTF8(menu->لڑیاں[MENU_اشاریہ_TIP]);
    إذا (tooltip != عدم && utf_valid_string(tooltip, عدم))
	gtk_widget_سیٹ_tooltip_متن(menu->id, (ثابت حرف *)tooltip);
    CONVERT_منزل_UTF8_FREE(tooltip);
}

    فراغ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    إذا (menu->ماپا != عدم && menu_is_toolbar(menu->ماپا->name))
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
    وإلا إذا (menu->ماپا == عدم && menu->id != عدم)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    وإلا إذا (menu->id != عدم)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->ماپا->submenu_id), menu->id);

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
    menu = vim_menu_bar_منزل_menu(VIM_MENU_BAR(gui.menubar));

    gtk_widget_سیٹ_ماپا(menu, gui.drawarea);
    gtk_popover_سیٹ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    rect.x = 0;
    rect.y = 0;
    rect.چوڑائی = 1;
    rect.اچائی = 1;
    gtk_popover_سیٹ_pointing_منزل(GTK_POPOVER(menu), &rect);
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), عدم);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#جے_انت // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    فراغ
gui_mch_سیٹ_scrollbar_thumb(scrollbar_T *sb, لما val, لما ناپ, لما max)
{
    GtkAdjustment *adj;

    إذا (sb->id == عدم)
	أرجع;
    إذا (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	أرجع;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_سیٹ_ہیٹھلا(adj, 0.0);
    gtk_adjustment_سیٹ_اتلا(adj, (gdouble)max + 1);
    gtk_adjustment_سیٹ_قدر(adj, (gdouble)val);
    gtk_adjustment_سیٹ_قدم_increment(adj, 1.0);
    gtk_adjustment_سیٹ_ورقہ_increment(adj, (gdouble)(ناپ > 2 ? ناپ - 2 : 1));
    gtk_adjustment_سیٹ_ورقہ_ناپ(adj, (gdouble)ناپ);
}

    فراغ
gui_mch_سیٹ_scrollbar_pos(scrollbar_T *sb, صحيح x, صحيح y, صحيح w, صحيح h)
{
    إذا (sb->id != عدم)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    صحيح formwin_w = gtk_widget_get_چوڑائی(gui.formwin);
    صحيح sbar_w = 0;
    صحيح xpad;

    إذا (gui.which_scrollbars[SBAR_بایاں])
	sbar_w += gui.scrollbar_چوڑائی;
    إذا (gui.which_scrollbars[SBAR_دایاں])
	sbar_w += gui.scrollbar_چوڑائی;

    xpad = formwin_w - آخری_متن_area_w - sbar_w;
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    صحيح formwin_h = gtk_widget_get_اچائی(gui.formwin);
    صحيح ypad;

    ypad = formwin_h - آخری_متن_area_h;
    إذا (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_اچائی;

    أرجع (ypad < 0) ? 0 : ypad;
}

    ساكن فراغ
adjustment_قدر_بدلیا(GtkAdjustment *adj, gpointer انکڑے بن_ورتیا)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_انکڑے(G_OBJECT(adj), "vim-sb");
    لما قدر = (لما)gtk_adjustment_get_قدر(adj);

    إذا (sb != عدم)
	gui_drag_scrollbar(sb, قدر, FALSE);
}

    فراغ
gui_mch_create_scrollbar(scrollbar_T *sb, صحيح orient)
{
    GtkAdjustment *adj;
    إذا (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, عدم);
    وإلا
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, عدم);

    gtk_widget_جوڑو_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_سیٹ_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_سیٹ_انکڑے(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_قدر_بدلیا), عدم);
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
gui_mch_سیٹ_متن_area_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    إذا (w <= 0 || h <= 0)
	أرجع;
    آخری_متن_area_w = w;
    آخری_متن_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    فراغ
gui_gtk4_calculate_bleed(صحيح چوڑائی, صحيح اچائی)
{
    صحيح old_دایاں = gui.bleed_دایاں;
    صحيح old_bot = gui.bleed_bot;

    gui.bleed_دایاں = چوڑائی - آخری_متن_area_w;
    gui.bleed_bot = اچائی - آخری_متن_area_h;

    إذا (gui.which_scrollbars[SBAR_بایاں])
	gui.bleed_دایاں -= gui.scrollbar_چوڑائی;
    إذا (gui.which_scrollbars[SBAR_دایاں])
	gui.bleed_دایاں -= gui.scrollbar_چوڑائی;
    إذا (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_اچائی;

    // Not sure if this can happen, but be safe...
    إذا (gui.bleed_دایاں < 0)
	gui.bleed_دایاں = 0;
    إذا (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    إذا (old_دایاں != gui.bleed_دایاں || old_bot != gui.bleed_bot)
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
تعريفنوع بنية {
    GFile	*نتیجہ;
    gboolean	پورا;
} File_Dialog_انکڑے;

    ساكن فراغ
file_dialog_open_cb(GObject *سروت, GAsync_نتیجہ *res, gpointer انکڑے)
{
    File_Dialog_انکڑے *fdd = (File_Dialog_انکڑے *)انکڑے;
    fdd->نتیجہ = gtk_file_dialog_open_انت(
		    GTK_FILE_DIALOG(سروت), res, عدم);
    fdd->پورا = TRUE;
}

    ساكن فراغ
file_dialog_save_cb(GObject *سروت, GAsync_نتیجہ *res, gpointer انکڑے)
{
    File_Dialog_انکڑے *fdd = (File_Dialog_انکڑے *)انکڑے;
    fdd->نتیجہ = gtk_file_dialog_save_انت(
		    GTK_FILE_DIALOG(سروت), res, عدم);
    fdd->پورا = TRUE;
}

    ساكن فراغ
file_dialog_folder_cb(GObject *سروت, GAsync_نتیجہ *res, gpointer انکڑے)
{
    File_Dialog_انکڑے *fdd = (File_Dialog_انکڑے *)انکڑے;
    fdd->نتیجہ = gtk_file_dialog_select_folder_انت(
		    GTK_FILE_DIALOG(سروت), res, عدم);
    fdd->پورا = TRUE;
}

    char_u *
gui_mch_browse(صحيح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext بن_ورتیا,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    File_Dialog_انکڑے  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *نتیجہ = عدم;
    GListStore	    *filters;

    title = CONVERT_منزل_UTF8(title);

    إذا (initdir == عدم || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    وإلا إذا (vim_بھریا_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    جوڑو_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_سیٹ_modal(dlg, TRUE);
    إذا (title != عدم)
	gtk_file_dialog_سیٹ_title(dlg, (ثابت حرف *)title);

    {
	GFile *dir = g_file_new_for_رستہ((ثابت حرف *)dirbuf);
	gtk_file_dialog_سیٹ_initial_folder(dlg, dir);
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
			gtk_file_filter_سیٹ_name(gfilter, (gchar *)patt);
		    وإلا
		    {
			gtk_file_filter_جوڑو_نمونہ(gfilter, (gchar *)patt);
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
    gtk_file_dialog_سیٹ_filters(dlg, G_LIST_MODEL(filters));
    g_object_unref(filters);

    إذا (saving && dflt != عدم && *dflt != NUL)
	gtk_file_dialog_سیٹ_initial_name(dlg, (ثابت حرف *)dflt);

    fdd.نتیجہ = عدم;
    fdd.پورا = FALSE;

    إذا (saving)
	gtk_file_dialog_save(dlg, GTK_کھڑکی(gui.mainwin), عدم,
		file_dialog_save_cb, &fdd);
    وإلا
	gtk_file_dialog_open(dlg, GTK_کھڑکی(gui.mainwin), عدم,
		file_dialog_open_cb, &fdd);

    طالما (!fdd.پورا)
	g_main_context_iteration(عدم, TRUE);

    إذا (fdd.نتیجہ != عدم)
    {
	حرف *رستہ = g_file_get_رستہ(fdd.نتیجہ);
	إذا (رستہ != عدم)
	{
	    نتیجہ = vim_strsave((char_u *)رستہ);
	    g_free(رستہ);
	}
	g_object_unref(fdd.نتیجہ);
    }

    g_object_unref(dlg);
    CONVERT_منزل_UTF8_FREE(title);

    أرجع نتیجہ;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    File_Dialog_انکڑے	fdd;
    char_u		*نتیجہ = عدم;

    title = CONVERT_منزل_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_سیٹ_modal(dlg, TRUE);
    إذا (title != عدم)
	gtk_file_dialog_سیٹ_title(dlg, (ثابت حرف *)title);

    إذا (initdir != عدم && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_رستہ((ثابت حرف *)initdir);
	gtk_file_dialog_سیٹ_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.نتیجہ = عدم;
    fdd.پورا = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_کھڑکی(gui.mainwin), عدم,
	    file_dialog_folder_cb, &fdd);

    طالما (!fdd.پورا)
	g_main_context_iteration(عدم, TRUE);

    إذا (fdd.نتیجہ != عدم)
    {
	حرف *رستہ = g_file_get_رستہ(fdd.نتیجہ);
	إذا (رستہ != عدم)
	{
	    نتیجہ = vim_strsave((char_u *)رستہ);
	    g_free(رستہ);
	}
	g_object_unref(fdd.نتیجہ);
    }

    g_object_unref(dlg);
    CONVERT_منزل_UTF8_FREE(title);

    أرجع نتیجہ;
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
ونڈو_button_string(char_u *button_string, صحيح *n_buttons, حرف **tofree)
{
    حرف	    **array;
    char_u	    *p;
    بنانشانی صحيح    گنتی = 1;

    button_string = (char_u *)g_strdup((ثابت حرف *)button_string);

    لكل (p = button_string; *p != NUL; ++p)
	إذا (*p == DLG_BUTTON_SEP)
	    ++گنتی;

    array = g_malloc_n(گنتی, حجم(حرف *));
    گنتی = 0;

    إذا (array != عدم)
    {
	array[گنتی++] = (حرف *)button_string;
	لكل (p = button_string; *p != NUL; )
	{
	    إذا (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[گنتی++] = (حرف *)p;
	    }
	    وإلا إذا (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    وإلا
		MB_PTR_ADV(p);
	}
    }

    *tofree = (حرف *)button_string;
    *n_buttons = گنتی;
    أرجع array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    ساكن ثابت حرف *
dialog_type_منزل_icon(صحيح type)
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
    gboolean	*پورا;
} DialogButtonState;

تعريفنوع بنية
{
    GtkWidget	*win;
    gboolean	*پورا;
    gboolean	no_alt;
} DialogState;

    ساكن فراغ
dialog_button_clicked_cb(GtkButton *button بن_ورتیا, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->پورا = TRUE;
}

    ساكن gboolean
dialog_کلید_pressed_cb(
	Gtk_Event_Controller_کلید	*controller بن_ورتیا,
	guint			keyval,
	guint			keycode بن_ورتیا,
	GdkModifierType		state,
	DialogState		*dstate)
{
    إذا (keyval == GDK_کلید_Escape)
    {
	*dstate->پورا = TRUE;
	أرجع TRUE;
    }

    إذا (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_نقاب()))
	أرجع gtk_widget_mnemonic_activate(dstate->win, FALSE);
    أرجع FALSE;
}

    ساكن gboolean
dialog_close_request_cb(Gtk_کھڑکی *win بن_ورتیا, gboolean *win_closed)
{
    *win_closed = TRUE;
    أرجع FALSE;
}

    صحيح
gui_mch_dialog(
	صحيح	type,
	char_u	*title,
	char_u	*سنیہا,
	char_u	*buttons,
	صحيح	def_but,
	char_u	*textfield,
	صحيح	ex_cmd بن_ورتیا)
{
    Gtk_کھڑکی		*win = GTK_کھڑکی(gtk_کھڑکی_new());
    GtkWidget		*vertbox;
    GtkWidget		*سنیہا_box;
    ثابت حرف		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*اندراج = عدم;
    char_u		*utf8_title;
    char_u		*utf8_سنیہا;
    GtkEventController *کلید_controller;
    DialogButtonState	*but_حالتاں = عدم;
    حرف		*tofree = عدم;
    صحيح			response = -1;
    gboolean		پورا = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_منزل_UTF8(title);
    إذا (utf8_title != عدم)
	gtk_کھڑکی_سیٹ_title(win, (ثابت حرف *)utf8_title);
    CONVERT_منزل_UTF8_FREE(utf8_title);

    gtk_کھڑکی_سیٹ_transient_for(win, GTK_کھڑکی(gui.mainwin));
    gtk_کھڑکی_سیٹ_modal(win, TRUE);
    gtk_کھڑکی_سیٹ_default_ناپ(win, 300, -1);
    gtk_کھڑکی_سیٹ_destroy_with_ماپا(win, TRUE);
    g_signal_connect(win, "close-request",
	    G_CALLBACK(dialog_close_request_cb), &win_closed);

    // Create main vertical layout container
    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_کھڑکی_سیٹ_بچہ(win, vertbox);
    gtk_widget_سیٹ_margin_چوٹی(vertbox, 24);
    gtk_widget_سیٹ_margin_bottom(vertbox, 24);
    gtk_widget_سیٹ_margin_شروع(vertbox, 12);
    gtk_widget_سیٹ_margin_سرا(vertbox, 12);

    // Add the message label
    سنیہا_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_سیٹ_halign(سنیہا_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), سنیہا_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_منزل_icon(type);
    إذا (icon_name != عدم)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_سیٹ_icon_ناپ(GTK_IMAGE(icon), GTK_ICON_ناپ_LARGE);
	gtk_box_append(GTK_BOX(سنیہا_box), icon);
    }

    utf8_سنیہا = CONVERT_منزل_UTF8(سنیہا);
    label = gtk_label_new((ثابت حرف *)utf8_سنیہا);
    CONVERT_منزل_UTF8_FREE(utf8_سنیہا);
    gtk_label_سیٹ_wrap(GTK_LABEL(label), TRUE);
    gtk_label_سیٹ_max_چوڑائی_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(سنیہا_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    کلید_controller = gtk_event_controller_کلید_new();
    g_signal_connect(کلید_controller, "key-pressed",
	    G_CALLBACK(dialog_کلید_pressed_cb), &state);
    gtk_widget_جوڑو_controller(GTK_WIDGET(win), کلید_controller);
    state.پورا = &پورا;
    state.win = GTK_WIDGET(win);

    إذا (textfield != عدم)
    {
	// Add text entry so user can enter text
	char_u *utf8_متن = CONVERT_منزل_UTF8(textfield);

	اندراج = gtk_اندراج_new();

	إذا (utf8_متن != عدم)
	    gtk_editable_سیٹ_متن(GTK_EDITABLE(اندراج), (ثابت حرف *)utf8_متن);
	وإلا
	    gtk_editable_سیٹ_متن(GTK_EDITABLE(اندراج), "");
	CONVERT_منزل_UTF8_FREE(utf8_متن);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_اندراج_سیٹ_activates_default(GTK_اندراج(اندراج), TRUE);
	gtk_box_append(GTK_BOX(vertbox), اندراج);
	state.no_alt = FALSE;
    }
    وإلا
    {
	GListModel  *controllers;
	صحيح	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_اشیاء(controllers);
	لكل (صحيح i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_شے(controllers, i);
	    إذا (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_سیٹ_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_کھڑکی_سیٹ_mnemonics_visible(win, TRUE);
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

	gtk_widget_سیٹ_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_سیٹ_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = ونڈو_button_string(buttons, &n_buttons, &tofree);

	but_حالتاں = g_malloc_n(n_buttons, حجم(DialogButtonState));

	لكل (صحيح i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_حالتاں + i;

	    but_label = CONVERT_منزل_UTF8((char_u *)buttons_arr[i]);
	    إذا (but_label == عدم)
		استمر;

	    but = gtk_button_new_with_mnemonic((حرف *)but_label);
	    إذا (i == def_but - 1)
		gtk_کھڑکی_سیٹ_default_widget(win, but);
	    gtk_box_append(GTK_BOX(but_box), but);
	    CONVERT_منزل_UTF8_FREE(but_label);

	    but_state->but_idx = i;
	    but_state->response = &response;
	    but_state->پورا = &پورا;

	    g_signal_connect(but, "clicked",
		    G_CALLBACK(dialog_button_clicked_cb), but_state);
	}
	g_free(buttons_arr);
    }

    gtk_کھڑکی_present(win);

    طالما (!پورا && !win_closed)
	g_main_context_iteration(عدم, TRUE);

    إذا (پورا)
    {
	إذا (textfield != عدم)
	{
	    // Get the text the user entered
	    char_u *متن;

	    متن = (char_u *)gtk_editable_get_متن(GTK_EDITABLE(اندراج));
	    متن = CONVERT_FROM_UTF8(متن);
	    vim_strncpy(textfield, متن, IOSIZE - 1);
	    CONVERT_FROM_UTF8_FREE(متن);
	}

	gtk_کھڑکی_destroy(win);
    }
    g_free(but_حالتاں);
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
    GtkWidget *اتے;	    // Direction up radio
    GtkWidget *تھلے;	    // Direction down radio
    GtkWidget *لبھو;	    // 'Find Next' action button
    GtkWidget *replace;	    // 'Replace With' action button
    GtkWidget *all;	    // 'Replace All' action button
} Shared_لبھو_Replace_2;

ساكن Shared_لبھو_Replace_2 لبھو_widgets = {0};
ساكن Shared_لبھو_Replace_2 repl_widgets = {0};

    ساكن فراغ
لبھو_replace_cb(GtkWidget *widget بن_ورتیا, gpointer انکڑے)
{
    صحيح			flags;
    char_u		*لبھو_متن;
    char_u		*repl_متن;
    gboolean		direction_تھلے;
    Shared_لبھو_Replace_2	*sfr;

    flags = GPOINTER_منزل_INT(انکڑے);

    إذا (flags == FRD_FINDNEXT)
    {
	repl_متن = عدم;
	sfr = &لبھو_widgets;
    }
    وإلا
    {
	repl_متن = (char_u *)gtk_editable_get_متن(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    لبھو_متن = (char_u *)gtk_editable_get_متن(GTK_EDITABLE(sfr->what));
    direction_تھلے = gtk_check_button_get_سرگرم(
	    GTK_CHECK_BUTTON(sfr->تھلے));

    إذا (gtk_check_button_get_سرگرم(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_سارا_لفظ;
    إذا (gtk_check_button_get_سرگرم(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_میل_CASE;

    repl_متن = CONVERT_FROM_UTF8(repl_متن);
    لبھو_متن = CONVERT_FROM_UTF8(لبھو_متن);
    gui_do_findrepl(flags, لبھو_متن, repl_متن, direction_تھلے);
    CONVERT_FROM_UTF8_FREE(repl_متن);
    CONVERT_FROM_UTF8_FREE(لبھو_متن);
}

    ساكن فراغ
dialog_destroyed_cb(GtkWidget *widget بن_ورتیا, gpointer انکڑے)
{
    *(GtkWidget **)انکڑے = عدم;
}

    ساكن فراغ
اندراج_بدلیا_cb(GtkWidget *اندراج, GtkWidget *dialog)
{
    ثابت gchar	*اندراج_متن;
    gboolean	nonempty;

    اندراج_متن = gtk_editable_get_متن(GTK_EDITABLE(اندراج));

    إذا (!اندراج_متن)
	أرجع; // Shouldn't happen

    nonempty = (اندراج_متن[0] != '\0');

    إذا (dialog == لبھو_widgets.dialog)
	gtk_widget_سیٹ_sensitive(لبھو_widgets.لبھو, nonempty);

    إذا (dialog == repl_widgets.dialog)
    {
	gtk_widget_سیٹ_sensitive(repl_widgets.لبھو, nonempty);
	gtk_widget_سیٹ_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_سیٹ_sensitive(repl_widgets.all, nonempty);
    }
}

    ساكن gboolean
لبھو_کلید_pressed_cb(
	Gtk_Event_Controller_کلید	*controller بن_ورتیا,
	guint			keyval,
	guint			keycode بن_ورتیا,
	GdkModifierType		state,
	Shared_لبھو_Replace_2	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    إذا (state & (GDK_CONTROL_نقاب | GDK_کھسکاؤ_نقاب))
	أرجع FALSE;

    // the Escape key synthesizes a cancellation action
    إذا (keyval == GDK_کلید_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_کھڑکی_destroy(GTK_کھڑکی(frdp->dialog));
	أرجع TRUE;
    }

    أرجع FALSE;
}

    ساكن فراغ
اندراج_activate_cb(GtkWidget *widget بن_ورتیا, فراغ *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    ساكن فراغ
لبھو_replace_dialog_create(char_u *arg, صحيح do_replace)
{
    Shared_لبھو_Replace_2	*frdp;
    char_u		*اندراج_متن;
    صحيح			wword = FALSE;
    صحيح			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*کلید_controller;

    frdp = do_replace ? &repl_widgets : &لبھو_widgets;
    اندراج_متن = get_لبھو_dialog_متن(arg, &wword, &mcase);

    إذا (اندراج_متن != عدم && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = اندراج_متن;
	اندراج_متن = string_convert(&output_conv, اندراج_متن, عدم);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    إذا (frdp->dialog != عدم)
    {
	إذا (اندراج_متن != عدم)
	{
	    gtk_editable_سیٹ_متن(GTK_EDITABLE(frdp->what),
		    (حرف *)اندراج_متن);
	    gtk_check_button_سیٹ_سرگرم(GTK_CHECK_BUTTON(frdp->wword),
		    (gboolean)wword);
	    gtk_check_button_سیٹ_سرگرم(GTK_CHECK_BUTTON(frdp->mcase),
		    (gboolean)mcase);
	}
	gtk_کھڑکی_present(GTK_کھڑکی(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	إذا (do_replace && g_utf8_strlen(
		    gtk_editable_get_متن(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(اندراج_متن);
	أرجع;
    }

    // Create a new dialog window.
    frdp->dialog = gtk_کھڑکی_new();
    gtk_کھڑکی_سیٹ_transient_for(GTK_کھڑکی(frdp->dialog),
	    GTK_کھڑکی(gui.mainwin));
    gtk_کھڑکی_سیٹ_destroy_with_ماپا(GTK_کھڑکی(frdp->dialog), TRUE);
    gtk_کھڑکی_سیٹ_title(GTK_کھڑکی(frdp->dialog),
	    do_replace ? _("VIM - Search and Replace...")
	    : _("VIM - Search..."));
    gtk_کھڑکی_سیٹ_resizable(GTK_کھڑکی(frdp->dialog), FALSE);

    g_signal_connect(frdp->dialog, "destroy",
	    G_CALLBACK(dialog_destroyed_cb), &frdp->dialog);

    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_سیٹ_margin_شروع(vertbox, 12);
    gtk_widget_سیٹ_margin_سرا(vertbox, 12);
    gtk_widget_سیٹ_margin_چوٹی(vertbox, 12);
    gtk_widget_سیٹ_margin_bottom(vertbox, 12);
    gtk_کھڑکی_سیٹ_بچہ(GTK_کھڑکی(frdp->dialog), vertbox);

    // Grid for labels + entries
    grid = gtk_grid_new();
    gtk_grid_سیٹ_صف_spacing(GTK_GRID(grid), 6);
    gtk_grid_سیٹ_کالم_spacing(GTK_GRID(grid), 6);
    gtk_box_append(GTK_BOX(vertbox), grid);

    // "Find what:" label + entry
    tmp = gtk_label_new(_("Find what:"));
    gtk_label_سیٹ_xalign(GTK_LABEL(tmp), 0.0);
    gtk_grid_attach(GTK_GRID(grid), tmp, 0, 0, 1, 1);

    frdp->what = gtk_اندراج_new();
    gtk_widget_سیٹ_hexpand(frdp->what, TRUE);
    sensitive = (اندراج_متن != عدم && اندراج_متن[0] != NUL);
    إذا (اندراج_متن != عدم)
	gtk_editable_سیٹ_متن(GTK_EDITABLE(frdp->what), (حرف *)اندراج_متن);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    إذا (do_replace)
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
    وإلا
	// Make the entry activation do the search.
	gtk_اندراج_سیٹ_activates_default(GTK_اندراج(frdp->what), TRUE);

    // Checkboxes
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    frdp->wword = gtk_check_button_new_with_label(_("Match whole word only"));
    gtk_check_button_سیٹ_سرگرم(GTK_CHECK_BUTTON(frdp->wword),
	    (gboolean)wword);
    gtk_box_append(GTK_BOX(hbox), frdp->wword);

    frdp->mcase = gtk_check_button_new_with_label(_("Match case"));
    gtk_check_button_سیٹ_سرگرم(GTK_CHECK_BUTTON(frdp->mcase),
	    (gboolean)mcase);
    gtk_box_append(GTK_BOX(hbox), frdp->mcase);

    // Direction radio buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    tmp = gtk_label_new(_("Direction:"));
    gtk_box_append(GTK_BOX(hbox), tmp);

    frdp->اتے = gtk_check_button_new_with_label(_("Up"));
    gtk_box_append(GTK_BOX(hbox), frdp->اتے);

    frdp->تھلے = gtk_check_button_new_with_label(_("Down"));
    gtk_check_button_سیٹ_گروہ(GTK_CHECK_BUTTON(frdp->تھلے),
	    GTK_CHECK_BUTTON(frdp->اتے));
    gtk_check_button_سیٹ_سرگرم(GTK_CHECK_BUTTON(frdp->تھلے), TRUE);
    gtk_box_append(GTK_BOX(hbox), frdp->تھلے);

    // Action buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_سیٹ_halign(hbox, GTK_ALIGN_سرا);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    btn = gtk_button_new_with_label(_("Find Next"));
    gtk_widget_سیٹ_sensitive(btn, sensitive);
    frdp->لبھو = btn;

    کلید_controller = gtk_event_controller_کلید_new();
    g_signal_connect(کلید_controller, "key-pressed",
	    G_CALLBACK(لبھو_کلید_pressed_cb), frdp);
    gtk_widget_جوڑو_controller(GTK_WIDGET(frdp->dialog), کلید_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(اندراج_بدلیا_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_کھڑکی_سیٹ_default_widget(GTK_کھڑکی(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(لبھو_replace_cb),
	    GINT_منزل_اشارہ_گر(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    إذا (do_replace)
    {
	btn = gtk_button_new_with_label(_("Replace"));
	gtk_widget_سیٹ_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(لبھو_replace_cb),
		GINT_منزل_اشارہ_گر(FRD_REPLACE));
	frdp->replace = btn;
	gtk_box_append(GTK_BOX(hbox), btn);

	btn = gtk_button_new_with_label(_("Replace All"));
	gtk_widget_سیٹ_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(لبھو_replace_cb),
		GINT_منزل_اشارہ_گر(FRD_REPLACEALL));
	frdp->all = btn;
	gtk_box_append(GTK_BOX(hbox), btn);
    }

    btn = gtk_button_new_with_label(_("Close"));
    g_signal_connect_swapped(btn, "clicked",
	    G_CALLBACK(gtk_کھڑکی_destroy), frdp->dialog);
    gtk_box_append(GTK_BOX(hbox), btn);

    gtk_کھڑکی_present(GTK_کھڑکی(frdp->dialog));
    gtk_widget_grab_focus(frdp->what);
    إذا (do_replace && اندراج_متن != عدم && اندراج_متن[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(اندراج_متن);
}

    فراغ
gui_mch_لبھو_dialog(exarg_T *eap)
{
    إذا (gui.وچ_use)
	لبھو_replace_dialog_create(eap->arg, FALSE);
}

    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
    إذا (gui.وچ_use)
	لبھو_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    فراغ
ex_helpfind(exarg_T *eap بن_ورتیا)
{
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

/*
 * ============================================================
 * Native print dialog
 * ============================================================
 */

#جے_تعریف_شدہ USE_GTK4_PRINT_DIALOG
ساكن GtkPrintDialog *print_dialog = عدم;
ساكن GtkPrintSettings *print_settings = عدم;
ساكن Gtk_ورقہ_تیاری *ورقہ_تیاری = عدم;
ساكن Gtk_Print_تیاری *print_تیاری = عدم;
ساكن GFile *print_file;

    ساكن فراغ
print_dialog_cb(GtkPrintDialog *dialog, GAsync_نتیجہ *نتیجہ, gboolean *پورا)
{
    GError *error = عدم;
    print_تیاری = gtk_print_dialog_تیاری_انت(dialog, نتیجہ, &error);

    إذا (print_تیاری == عدم)
    {
	semsg(_(e_print_error_str), error->سنیہا);
	g_error_free(error);
    }
    *پورا = TRUE;
}

/*
 * Return file to write to on success (allocated string), or NULL on failure
 * (e.g. user cancelled)
 */
    char_u *
gui_gtk4_print_dialog(
	prt_settings_T	*psettings,
	char_u		*jobname,
	مزدوج		*ورقہ_چوڑائی,
	مزدوج		*ورقہ_اچائی)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    پورا = FALSE;
    char_u	    *عارضی;

    gtk_print_dialog_سیٹ_title(dialog, (ثابت حرف *)jobname);
    gtk_print_dialog_سیٹ_modal(dialog, TRUE);

    print_dialog = عدم;
    print_تیاری = عدم;

    إذا (print_settings == عدم)
    {
	option_جدول_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	إذا (opt->present)
	{
	    إذا (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_سیٹ_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    وإلا إذا (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_سیٹ_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    وإلا
		gtk_print_settings_سیٹ_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	إذا (opt->present)
	{
	    إذا (*opt->string == 'y')
		gtk_print_settings_سیٹ_orientation(print_settings,
			GTK_ورقہ_ORIENTATION_PORTRAIT);
	    وإلا
		gtk_print_settings_سیٹ_orientation(print_settings,
			GTK_ورقہ_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	إذا (opt->present)
	{
	    gtk_print_settings_سیٹ_paper_ناپ(print_settings,
		    gtk_paper_ناپ_new((ثابت حرف *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	إذا (opt->present)
	{
	    إذا (*opt->string == 'y')
		gtk_print_settings_سیٹ_collate(print_settings, TRUE);
	    وإلا
		gtk_print_settings_سیٹ_collate(print_settings, FALSE);
	}
    }

    إذا (ورقہ_تیاری == عدم)
	ورقہ_تیاری = gtk_ورقہ_تیاری_new();

    gtk_print_dialog_سیٹ_print_settings(dialog, print_settings);
    gtk_print_dialog_سیٹ_ورقہ_تیاری(dialog, ورقہ_تیاری);

    gtk_print_dialog_تیاری(dialog, GTK_کھڑکی(gui.mainwin), عدم,
	    (GAsyncReadyCallback)print_dialog_cb, &پورا);

    طالما (!پورا)
	g_main_context_iteration(عدم, TRUE);

    إذا (print_تیاری == عدم)
    {
	g_object_unref(dialog);
	g_clear_اشارہ_گر(&print_تیاری, gtk_print_تیاری_unref);

	psettings->user_abort_پیغام = FALSE;
	أرجع عدم;
    }

    g_clear_object(&print_settings);
    g_clear_object(&ورقہ_تیاری);
    print_settings = g_object_ref(
	    gtk_print_تیاری_get_print_settings(print_تیاری));
    ورقہ_تیاری = g_object_ref(gtk_print_تیاری_get_ورقہ_تیاری(print_تیاری));

    {
	GtkPrintDuplex duplex = gtk_print_settings_get_duplex(print_settings);
	psettings->duplex = duplex != GTK_PRINT_DUPLEX_SIMPLEX;
    }

    psettings->has_color = gtk_print_settings_get_use_color(print_settings);

    *ورقہ_چوڑائی = gtk_ورقہ_تیاری_get_paper_چوڑائی(ورقہ_تیاری,
	    GTK_UNIT_POINTS);
    *ورقہ_اچائی = gtk_ورقہ_تیاری_get_paper_اچائی(ورقہ_تیاری,
	    GTK_UNIT_POINTS);

    عارضی  = vim_tempname('p', FALSE);
    إذا (عارضی == عدم)
    {
	emsg(_(e_cant_لبھو_عارضی_file_for_writing));
	g_clear_object(&dialog);
	g_clear_اشارہ_گر(&print_تیاری, gtk_print_تیاری_unref);

	psettings->user_abort_پیغام = FALSE;
	أرجع عدم;
    }
    print_file = g_file_new_for_رستہ((حرف *)عارضی);

    print_dialog = dialog;

    أرجع عارضی;
}

    ساكن فراغ
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsync_نتیجہ	*نتیجہ,
	GFile		*file)
{
    حرف *رستہ = g_file_get_رستہ(file);

    (فراغ)gtk_print_dialog_print_file_انت(dialog, نتیجہ, عدم);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(رستہ);
    g_free(رستہ);
}


    فراغ
gui_gtk4_print_انت(فراغ)
{
    إذا (print_dialog != عدم)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_کھڑکی(gui.mainwin),
		print_تیاری, print_file, عدم,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = عدم;
	g_clear_اشارہ_گر(&print_تیاری, gtk_print_تیاری_unref);
    }
}
#جے_انت // USE_GTK4_PRINT_DIALOG

#جے_تعریف_شدہ FEAT_IMAGE_GUI

تعريفنوع بنية
{
    GdkTexture *texture;
} image_gdk_T;

تعريفنوع بنية
{
    Gsk_Render_گرہ   **nodes; // Note that elements may be NULL
    صحيح		    n_nodes;
} image_placement_gdk_T;

    صحيح
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    Gdk_حافظہ_Format fmt;
    size_t	    stride;
    GBytes	    *بائٹاں;
    size_t	    ناپ;
    صحيح		    w, h;

    إذا (ctx == عدم)
	أرجع FAIL;

    إذا (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_حافظہ_R8_G8_B8_A8;
    وإلا
	fmt = GDK_حافظہ_R8_G8_B8;

    image_get_dimensions(img, &w, &h);

    ناپ = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    بائٹاں = g_بائٹاں_new(img->انکڑے, ناپ);
    ctx->texture = gdk_حافظہ_texture_new(w, h, fmt, بائٹاں, stride);
    g_بائٹاں_unref(بائٹاں);

    img->backend_انکڑے = ctx;
    أرجع OK;
}

    فراغ
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_انکڑے;

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
	gsk_render_گرہ_unref(ctx->nodes[i]);
    }
    g_clear_اشارہ_گر(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    صحيح
image_placement_gui_init(image_placement_T *رکھو)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    إذا (ctx == عدم)
	أرجع FAIL;

    رکھو->backend_انکڑے = ctx;
    أرجع OK;
}

    فراغ
image_placement_gui_uninit(image_placement_T *رکھو)
{
    image_placement_gdk_T *ctx = رکھو->backend_انکڑے;

    clear_nodes(ctx);
    vim_free(ctx);
}

    فراغ
image_placement_gui_draw(image_placement_T *رکھو)
{
    image_T		    *img = رکھو->img;
    image_placement_gdk_T   *ctx = رکھو->backend_انکڑے;
    image_gdk_T		    *ictx = img->backend_انکڑے;
    pixman_box32_t	    *rects;
    صحيح			    n_rects;
    صحيح			    iw, ih;
    Gsk_Render_گرہ	    *texture_گرہ;

    rects = pixman_region32_rectangles(&رکھو->visible, &n_rects);
    إذا (rects == عدم)
	أرجع;

    clear_nodes(ctx);
    إذا (n_rects == 0)
	أرجع;

    image_get_dimensions(img, &iw, &ih);
    texture_گرہ = gsk_texture_گرہ_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(رکھو->خانہ) - FILL_X(رکھو->crop_box.x1),
		FILL_Y(رکھو->صف) - FILL_Y(رکھو->crop_box.y1)
		+ FILL_Y(رکھو->صف_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    إذا (texture_گرہ == عدم)
	// Not sure if this can happen...
	أرجع;

    ctx->nodes = g_malloc_n(n_rects, حجم(Gsk_Render_گرہ *));
    ctx->n_nodes = n_rects;

    لكل (صحيح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحيح		صف, خانہ;
	صحيح		posx, posy;
	صحيح		x, y, w, h;
	Gsk_Render_گرہ	*گرہ;

	image_placement_subrect(رکھو, rect, &صف, &خانہ, &x, &y, &w, &h, false);
	posx = FILL_X(خانہ);
	posy = FILL_Y(صف);

	گرہ = gsk_clip_گرہ_new(texture_گرہ,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	إذا (گرہ != عدم)
	    vim_draw_area_جوڑو_external(VIM_DRAW_AREA(gui.drawarea), گرہ);

	ctx->nodes[i] = گرہ;
    }
    gsk_render_گرہ_unref(texture_گرہ);
}

    فراغ
image_placement_gui_clear(image_placement_T *رکھو)
{
    image_placement_gdk_T   *ctx = رکھو->backend_انکڑے;

    clear_nodes(ctx);
}

#جے_انت

#جے_انت // FEAT_GUI_GTK
