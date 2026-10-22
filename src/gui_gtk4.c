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

#اگر_تعریف_شده FEAT_GUI_GTK

#ضمن <gdk/gdkkeysyms.h>
#ضمن <gdk/gdk.h>
#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_f.h"
#ضمن "gui_gtk4_cb.h"
#ضمن "gui_gtk4_da.h"
#اگر_تعریف_شده FEAT_TOOLBAR
# include "gui_gtk4_tb.h"
#پایان_اگر
#اگر_تعریف_شده FEAT_MENU
# include "gui_gtk4_menu.h"
#پایان_اگر

/*
 * Geometry string parser, replacing XParseGeometry to remove X11 dependency.
 * Format: [WIDTHxHEIGHT][{+-}XOFF{+-}YOFF]
 */
#تعریف_کن No_مقدار		0x0000
#تعریف_کن عرض_مقدار	0x0004
#تعریف_کن ارتفاع_مقدار	0x0008

    ساكن صحيح
vim_parse_geometry(ثابت حرف *str, بیعلامت صحيح *عرض, بیعلامت صحيح *ارتفاع)
{
    صحيح ماسک = No_مقدار;
    حرف *انتها;
    دراز val;

    إذا (str == عدم || *str == NUL)
	أرجع ماسک;

    // Parse width
    إذا (*str != '+' && *str != '-')
    {
	val = strtol(str, &انتها, 10);
	إذا (انتها != str)
	{
	    *عرض = (بیعلامت صحيح)val;
	    ماسک |= عرض_مقدار;
	    str = انتها;
	}
    }

    // Parse 'x' or 'X' separator and height
    إذا (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &انتها, 10);
	إذا (انتها != str)
	{
	    *ارتفاع = (بیعلامت صحيح)val;
	    ماسک |= ارتفاع_مقدار;
	    str = انتها;
	}
    }

    أرجع ماسک;
}

#إذا defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
ساكن صحيح آخرین_shape = 0;
#پایان_اگر

#تعریف_کن DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
ساكن تعداد {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
ساكن صحيح gtk4_main_loop_level = 0;
ساكن صحيح gtk4_main_loop_quit = FALSE;

#اگر_تعریف_شده USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#پایان_اگر

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
ثابت special_کلیدها[] =
{
    {GDK_کلید_به_بالا,	'k', 'u'},
    {GDK_کلید_پایین,	'k', 'd'},
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
    {GDK_کلید_پشت_Space,	'k', 'b'},
    {GDK_کلید_درج,	'k', 'I'},
    {GDK_کلید_Delete,	'k', 'D'},
    {GDK_کلید_Home,	'k', 'h'},
    {GDK_کلید_انتها,	'@', '7'},
    {GDK_کلید_پیشین,	'k', 'P'},
    {GDK_کلید_بعدی,	'k', 'N'},
    {GDK_کلید_Print,	'%', '9'},
    {GDK_کلید_KP_چپ,	'k', 'l'},
    {GDK_کلید_KP_راست,	'k', 'r'},
    {GDK_کلید_KP_به_بالا,	'k', 'u'},
    {GDK_کلید_KP_پایین,	'k', 'd'},
    {GDK_کلید_KP_درج,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_کلید_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_کلید_KP_Home,	'K', '1'},
    {GDK_کلید_KP_انتها,	'K', '4'},
    {GDK_کلید_KP_پیشین,	'K', '3'},
    {GDK_کلید_KP_بعدی,	'K', '5'},
    {GDK_کلید_KP_جمع_کن,	'K', '6'},
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

    ساكن صحيح
keyval_مقصد_string(بیعلامت صحيح keyval, char_u *string)
{
    صحيح		len;
    guint32	uc;

    uc = gdk_keyval_مقصد_unicode(keyval);
    إذا (uc != 0)
    {
	len = utf_char2bytes((صحيح)uc, string);
    }
    وإلا
    {
	len = 1;
	بدل (keyval)
	{
	    حالة GDK_کلید_Tab: حالة GDK_کلید_KP_Tab: حالة GDK_کلید_ISO_چپ_Tab:
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

    إذا (state & GDK_لغزش_ماسک)
	modifiers |= MOD_ماسک_لغزش;
    إذا (state & GDK_CONTROL_ماسک)
	modifiers |= MOD_ماسک_CTRL;
    إذا (state & GDK_ALT_ماسک)
	modifiers |= MOD_ماسک_ALT;
    إذا (state & GDK_META_ماسک)
	modifiers |= MOD_ماسک_META;
    إذا (state & GDK_SUPER_ماسک)
	modifiers |= MOD_ماسک_CMD;

    أرجع modifiers;
}

ساكن GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
ساكن gboolean کلید_press_event(Gtk_Event_Controller_کلید *controller, guint keyval, guint keycode, GdkModifierType state, gpointer داده);
ساكن فراغ کلید_release_event(Gtk_Event_Controller_کلید *controller, guint keyval, guint keycode, GdkModifierType state, gpointer داده);
ساكن فراغ button_press_event(GtkGestureClick *gesture, صحيح n_press, مزدوج x, مزدوج y, gpointer داده);
ساكن فراغ button_release_event(GtkGestureClick *gesture, صحيح n_press, مزدوج x, مزدوج y, gpointer داده);
ساكن فراغ motion_notify_event(GtkEventControllerMotion *controller, مزدوج x, مزدوج y, gpointer داده);
ساكن فراغ enter_notify_event(GtkEventControllerMotion *controller, مزدوج x, مزدوج y, gpointer داده);
ساكن gboolean scroll_event(GtkEventControllerScroll *controller, مزدوج dx, مزدوج dy, gpointer داده);
ساكن فراغ focus_درون_event(GtkEventControllerFocus *controller, gpointer داده);
ساكن فراغ focus_بیرون_event(GtkEventControllerFocus *controller, gpointer داده);
#اگر_تعریف_شده FEAT_DND
ساكن gboolean drop_cb(Gtk_Drop_هدف_Async *هدف, GdkDrop *drop, مزدوج x, مزدوج y, فراغ *داده);
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_TABLINE
ساكن فراغ tabline_enter_cb(GtkEventController *controller, مزدوج x, مزدوج y, فراغ *udata);
ساكن فراغ on_select_tab(GtkNotebook *notebook, gpointer *صفحه, gint idx, gpointer داده);
ساكن فراغ on_tab_reordered(GtkNotebook *notebook, gpointer *صفحه, gint idx, gpointer داده);
ساكن VimMenu *create_tabline_popup_menu(فراغ);
ساكن فراغ tabline_جمع_کن_css(فراغ);
ساكن فراغ tabline_menu_press_event(GtkGestureClick *gesture, صحيح n_press, مزدوج x, مزدوج y, GtkWidget *popover);
#پایان_اگر
ساكن فراغ mainwin_destroy_cb(GObject *object, gpointer داده);
ساكن gboolean delete_event_cb(Gtk_پنجره *پنجره_2, gpointer داده);
ساكن صحيح query_اشاره_گر_pos(صحيح *x, صحيح *y, GdkModifierType *state);
ساكن فراغ mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_داده);
ساكن فراغ مجموعه_form_اندازه(صحيح عرض, صحيح ارتفاع);
ساكن فراغ drawarea_realize_cb(GtkWidget *widget, gpointer داده);
ساكن فراغ drawarea_unrealize_cb(GtkWidget *widget, gpointer داده);
ساكن فراغ clipboard_تغییریافته_cb(GdkClipboard *clipboard, gpointer user_داده);
#اگر_تعریف_شده FEAT_MENU
ساكن فراغ show_menubar_popover(فراغ);
#پایان_اگر

ساكن ثابت حرف *prgname = عدم;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    ساكن gboolean
arg_تطابق(ثابت حرف *s, ثابت حرف *name, حرف **مقدار)
{
    size_t  len = طول_رشته(name);

    إذا (مقایسه_رشته(s, name, len) != 0)
	أرجع FALSE;
    إذا (s[len] == '=')
	*مقدار = (حرف *)s + len + 1;
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
	حرف	*مقدار = عدم;
	صحيح	has_inline_مقدار = FALSE;
	صحيح	n_strip;

	إذا (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    استمر;
	}

	إذا (strchr(s, '=') != عدم)
	    has_inline_مقدار = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	إذا (!has_inline_مقدار)
	{
	    إذا (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		مقدار = argv[i + 1];
	    وإلا
		مقدار = عدم;
	}

	إذا (arg_تطابق(s, "-fn", &مقدار) || arg_تطابق(s, "-font", &مقدار))
	    font_آرگومان = مقدار;
	وإلا إذا (arg_تطابق(s, "-geom", &مقدار)
		|| arg_تطابق(s, "-geometry", &مقدار))
	{
	    إذا (مقدار != عدم)
		gui.geom = vim_strsave((char_u *)مقدار);
	}
	وإلا إذا (arg_تطابق(s, "-bg", &مقدار)
		|| arg_تطابق(s, "-background", &مقدار))
	    background_آرگومان = مقدار;
	وإلا إذا (arg_تطابق(s, "-fg", &مقدار)
		|| arg_تطابق(s, "-foreground", &مقدار))
	    foreground_آرگومان = مقدار;
#اگر_تعریف_شده FEAT_NETBEANS_INTG
	وإلا إذا (مقایسه_رشته(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_مقدار = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#پایان_اگر
	وإلا إذا (arg_تطابق(s, "--prg-name", &مقدار))
	    // GTK4 specific
	    prgname = مقدار;
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
	إذا (مقدار != عدم && !has_inline_مقدار)
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
timeout_جمع_کن(صحيح زمان, timeout_cb_type (*callback)(gpointer), صحيح *flagp)
{
    أرجع g_timeout_جمع_کن((guint)زمان, (GSourceFunc)callback, flagp);
}

    ساكن فراغ
timeout_remove(guint timer)
{
    g_منبع_remove(timer);
}

ساكن long_u blink_waittime = 700;
ساكن long_u blink_ontime = 400;
ساكن long_u blink_offtime = 250;
ساكن guint blink_timer = 0;

    ساكن timeout_cb_type
blink_cb(gpointer داده استفادهنشده)
{
    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_جمع_کن(blink_offtime, blink_cb, عدم);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_جمع_کن(blink_ontime, blink_cb, عدم);
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
gui_mch_مجموعه_blinking(دراز waittime, دراز on, دراز off)
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
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.درون_focus)
    {
	blink_timer = timeout_جمع_کن(blink_waittime, blink_cb, عدم);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    صحيح
gui_mch_early_init_check(صحيح give_پیام استفادهنشده)
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
    g_مجموعه_prgname(prgname == عدم ? "gvim" : prgname);

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
    gui.def_پشت_pixel = 0x00ffffff;	// white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.پشت_pixel = gui.def_پشت_pixel;

    gui.scrollbar_عرض = SB_DEFAULT_عرض;
    gui.scrollbar_ارتفاع = SB_DEFAULT_عرض;

    // Create the main window.
    gui.mainwin = gtk_پنجره_new();
    gtk_widget_مجموعه_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_مجموعه_پایه_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    g_signal_connect(G_OBJECT(gui.mainwin), "close-request",
		     G_CALLBACK(delete_event_cb), عدم);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), عدم);

    // A vertical box holds the menubar, toolbar and main text window.
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_مجموعه_homogeneous(GTK_BOX(vbox), FALSE);
    gtk_پنجره_مجموعه_فرزند(GTK_پنجره(gui.mainwin), vbox);

#اگر_تعریف_شده FEAT_MENU
    gui.menubar = vim_menu_bar_new();
    gtk_widget_مجموعه_name(gui.menubar, "vim-menubar");
    gtk_widget_مجموعه_visible(gui.menubar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.menubar);
#پایان_اگر

#اگر_تعریف_شده FEAT_TOOLBAR
    gui.toolbar = vim_toolbar_new();
    gtk_widget_مجموعه_name(gui.toolbar, "vim-toolbar");
    gtk_widget_مجموعه_visible(gui.toolbar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.toolbar);
    vim_toolbar_مجموعه_style(VIM_TOOLBAR(gui.toolbar), toolbar_flags, tbis_flags);
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_TABLINE
    gui.tabline = gtk_notebook_new();
    gtk_notebook_مجموعه_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_مجموعه_show_tabs(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_notebook_مجموعه_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_widget_جمع_کن_css_class(gui.tabline, "vim-tabline");
    tabline_جمع_کن_css();
    gtk_widget_مجموعه_visible(gui.tabline, FALSE);
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
	gtk_event_controller_مجموعه_انتشار_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	gtk_widget_جمع_کن_controller(gui.tabline, mcontroller);
    }

    // Create right click popup menu for tabline
    {
	GtkGesture	*click;
	VimMenu		*menu;

	click = gtk_gesture_click_new();
	menu = create_tabline_popup_menu();

	gtk_widget_مجموعه_والد(GTK_WIDGET(menu), gui.tabline);
	g_object_مجموعه_داده(G_OBJECT(gui.tabline), "menu", menu);

	gtk_popover_مجموعه_has_arrow(GTK_POPOVER(menu), FALSE);
	gtk_popover_مجموعه_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
	// Make popover start at top left corner
	gtk_widget_مجموعه_halign(GTK_WIDGET(menu), GTK_ALIGN_شروع);

	// Listen for anny mouse button
	gtk_gesture_single_مجموعه_button(GTK_GESTURE_SINGLE(click), 0);

	g_signal_connect_object(click, "pressed",
		G_CALLBACK(tabline_menu_press_event),
		menu, G_CONNECT_DEFAULT);
	gtk_widget_جمع_کن_controller(gui.tabline, GTK_EVENT_CONTROLLER(click));
    }
#پایان_اگر

    // The form widget manages absolute positioning of scrollbars and the draw
    // area.
    gui.formwin = vim_form_new();
    gtk_widget_مجموعه_name(gui.formwin, "vim-gtk-form");
    gtk_widget_مجموعه_vexpand(gui.formwin, TRUE);
    gtk_widget_مجموعه_hexpand(gui.formwin, TRUE);
    gtk_box_append(GTK_BOX(vbox), gui.formwin);

    // The drawing area for the editor content.
    gui.drawarea = vim_draw_area_new();
    gtk_widget_مجموعه_focusable(gui.drawarea, TRUE);
    gtk_widget_مجموعه_vexpand(gui.drawarea, TRUE);
    gtk_widget_مجموعه_hexpand(gui.drawarea, TRUE);
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
	gtk_widget_جمع_کن_controller(gui.drawarea, کلید_ctrl);
#اگر_تعریف_شده FEAT_XIM
	xim_init();
#پایان_اگر
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_مجموعه_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), عدم);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), عدم);
	gtk_widget_جمع_کن_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), عدم);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), عدم);
	gtk_widget_جمع_کن_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), عدم);
	gtk_widget_جمع_کن_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_درون_event), عدم);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_بیرون_event), عدم);
	gtk_widget_جمع_کن_controller(gui.drawarea, focus);
    }

#اگر_تعریف_شده FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_هدف = gtk_drop_هدف_async_new(عدم,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_مجموعه_dnd_targets();
	g_signal_connect(gui.drop_هدف, "drop",
			 G_CALLBACK(drop_cb), عدم);
	gtk_widget_جمع_کن_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_هدف));
    }
#پایان_اگر

    gui.border_جابه_جایی = gui.border_عرض;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_اشاره_گر = gdk_cursor_new_from_name("none", عدم);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	إذا (primary != عدم)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_تغییریافته_cb), &clip_ستاره);
	إذا (regular != عدم)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_تغییریافته_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_ستاره);

    أرجع OK;
}

    فراغ
gui_mch_new_colors(فراغ)
{
    إذا (gui.drawarea != عدم && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_صف_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open(فراغ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_عرض;
    guint pixel_ارتفاع;

    إذا (gui.geom != عدم)
    {
	صحيح		ماسک;
	بیعلامت صحيح	w, h;

	ماسک = vim_parse_geometry((حرف *)gui.geom, &w, &h);

	إذا (ماسک & عرض_مقدار)
	    Columns = w;
	إذا (ماسک & ارتفاع_مقدار)
	{
	    إذا (p_پنجره > (دراز)h - 1 || !option_was_مجموعه((char_u *)"window"))
		p_پنجره = h - 1;
	    ردیفها = h;
	}
	حد_screen_اندازه();

	VIM_CLEAR(gui.geom);
    }
    وإلا
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	إذا (Columns > 80)
	    Columns = 80;
	إذا (ردیفها > 24)
	    ردیفها = 24;
    }

    pixel_عرض = (guint)(gui_get_پایه_عرض() + Columns * gui.char_عرض);
    pixel_ارتفاع = (guint)(gui_get_پایه_ارتفاع() + ردیفها * gui.char_ارتفاع);

    pixel_عرض  += get_menu_tool_عرض();
    pixel_ارتفاع += get_menu_tool_ارتفاع();

    إذا (foreground_آرگومان != عدم)
	fg_pixel = gui_get_color((char_u *)foreground_آرگومان);
    إذا (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    إذا (background_آرگومان != عدم)
	bg_pixel = gui_get_color((char_u *)background_آرگومان);
    إذا (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    إذا (یافت_معکوس_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_پشت_pixel = fg_pixel;
    }
    وإلا
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_پشت_pixel = bg_pixel;
    }

    مجموعه_نرمال_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), عدم);
    // Resize is handled by GtkForm's size_allocate callback.

    مجموعه_form_اندازه((صحيح)pixel_عرض - get_menu_tool_عرض(),
	    (صحيح)pixel_ارتفاع - get_menu_tool_ارتفاع());

    gtk_پنجره_present(GTK_پنجره(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    أرجع OK;
}

    فراغ
gui_mch_exit(صحيح rc استفادهنشده)
{
    إذا (gui.mainwin != عدم)
    {
#اگر_تعریف_شده FEAT_GUI_TABLINE
	// Must unparent popover menu for tabline or we will get warning
	gtk_widget_unparent(g_object_get_داده(G_OBJECT(gui.tabline), "menu"));
#پایان_اگر
#اگر_تعریف_شده FEAT_BEVAL_GUI
	// Make sure to destroy popover used for balloon eval, or we will get a
	// warning from GTK that the draw area still has children left.
	gui_mch_destroy_beval_area(balloonEval);
#پایان_اگر
#اگر_تعریف_شده FEAT_MENU
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
#پایان_اگر
	gtk_پنجره_destroy(GTK_پنجره(gui.mainwin));
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
gui_mch_مجموعه_winpos(صحيح x استفادهنشده, صحيح y استفادهنشده)
{
    // GTK4/Wayland: window positioning not available
}

    صحيح
gui_mch_maximized(فراغ)
{
    أرجع gtk_پنجره_is_maximized(GTK_پنجره(gui.mainwin));
}

    فراغ
gui_mch_unmaximize(فراغ)
{
    إذا (gui.mainwin != عدم)
	gtk_پنجره_unmaximize(GTK_پنجره(gui.mainwin));
}

    فراغ
gui_mch_مجموعه_fullscreen(صحيح flag)
{
    إذا (gui.mainwin == عدم)
	أرجع;
    إذا (flag)
	gtk_پنجره_fullscreen(GTK_پنجره(gui.mainwin));
    وإلا
	gtk_پنجره_unfullscreen(GTK_پنجره(gui.mainwin));
}

    ساكن فراغ
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec استفادهنشده, gpointer user_داده استفادهنشده)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    إذا (gtk_پنجره_is_fullscreen(GTK_پنجره(obj)))
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
    w = gtk_widget_get_عرض(gui.formwin);
    h = gtk_widget_get_ارتفاع(gui.formwin);
    gui_resize_shell(w, h);
}

    فراغ
gui_mch_settitle(char_u *title, char_u *icon استفادهنشده)
{
    إذا (title != عدم && gui.mainwin != عدم)
	gtk_پنجره_مجموعه_title(GTK_پنجره(gui.mainwin), (ثابت حرف *)title);
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
gui_gtk_init_decor_ارتفاع(فراغ)
{
    صحيح h = gtk_widget_get_ارتفاع(gui.mainwin);

    إذا (h == 0)
	أرجع;

    h -= get_menu_tool_ارتفاع();
    h -= gtk_widget_get_ارتفاع(gui.formwin);

    gui.decor_ارتفاع = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    ساكن فراغ
مجموعه_form_اندازه(صحيح عرض, صحيح ارتفاع)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    إذا (gtk_widget_get_عرض(gui.formwin) == عرض
	    && gtk_widget_get_ارتفاع(gui.formwin) == ارتفاع)
	أرجع;

    // Remember the size the form widget is supposed to get. An allocation
    // that arrives before the compositor has answered this request still has
    // the previous size and must not be used.
    gui.pending_form_w = عرض;
    gui.pending_form_h = ارتفاع;
    gui.pending_form_skip = 1;

    // GtkWindow default size also includes client side decorations, so must
    // include it also.  It also keeps the natural width of the toolbar from
    // deciding the width.
    gtk_پنجره_مجموعه_default_اندازه(GTK_پنجره(gui.mainwin),
	    عرض + get_menu_tool_عرض(),
	    ارتفاع + get_menu_tool_ارتفاع() + gui.decor_ارتفاع);

    // The window drops a request made while it is being presented, and
    // "gui.decor_height" is not known before that.  A size request on the form
    // widget is honoured then; it is dropped again once the size was given.
    gtk_widget_مجموعه_اندازه_request(gui.formwin, عرض, ارتفاع);
}

    فراغ
gui_mch_مجموعه_shellsize(صحيح عرض, صحيح ارتفاع,
	صحيح min_عرض استفادهنشده, صحيح min_ارتفاع استفادهنشده,
	صحيح پایه_عرض استفادهنشده, صحيح پایه_ارتفاع استفادهنشده,
	صحيح direction استفادهنشده)
{
    مجموعه_form_اندازه(عرض, ارتفاع);

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
	    *screen_w = geom.عرض;
	    *screen_h = geom.ارتفاع;
	    أرجع;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#اگر_تعریف_شده FEAT_MENU
    فراغ
gui_mch_enable_menu(صحيح showit)
{
    إذا (gui.menubar != عدم)
    {
	gtk_widget_مجموعه_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	إذا (gui.drawarea != عدم)
	    gtk_widget_صف_draw(gui.drawarea);
    }
}
#پایان_اگر

#اگر_تعریف_شده FEAT_TOOLBAR
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    إذا (gui.toolbar != عدم)
    {
	gtk_widget_مجموعه_visible(gui.toolbar, showit);
	إذا (showit)
	    vim_toolbar_مجموعه_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	إذا (gui.drawarea != عدم)
	    gtk_widget_صف_draw(gui.drawarea);
    }
}
#پایان_اگر

    فراغ
gui_mch_مجموعه_dark_theme(صحيح dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    إذا (settings != عدم)
	g_object_مجموعه(settings, "gtk-application-prefer-dark-theme",
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

    gui.char_ارتفاع = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_ارتفاع = MAX(gui.char_ارتفاع, gui.char_ascent + 1);

    أرجع OK;
}

تعريفنوع بنية {
    PangoFontDescription    *نتیجه;
    gboolean		    انجامشده;
} Font_Dialog_داده;

    ساكن فراغ
font_dialog_پایان_cb(GObject *منبع, GAsync_نتیجه *res, gpointer داده)
{
    Font_Dialog_داده *fdd = (Font_Dialog_داده *)داده;
    fdd->نتیجه = gtk_font_dialog_انتخاب_font_پایان(
		    GTK_FONT_DIALOG(منبع), res, عدم);
    fdd->انجامشده = TRUE;
}

    ساكن gboolean
font_filter(gpointer قلم, gpointer داده استفادهنشده)
{
    إذا (PANGO_IS_FONT_FAMILY(قلم))
	أرجع pango_font_family_is_monospace(PANGO_FONT_FAMILY(قلم));
    إذا (PANGO_IS_FONT_FACE(قلم))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(قلم));
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
    Font_Dialog_داده	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_مجموعه_modal(dlg, TRUE);
    gtk_font_dialog_مجموعه_filter(dlg,
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

    fdd.نتیجه = عدم;
    fdd.انجامشده = FALSE;

    gtk_font_dialog_انتخاب_font(dlg, GTK_پنجره(gui.mainwin),
	    initial, عدم, font_dialog_پایان_cb, &fdd);

    طالما (!fdd.انجامشده)
	g_main_context_iteration(عدم, TRUE);

    إذا (fdd.نتیجه != عدم)
    {
	حرف *name = pango_font_description_مقصد_string(fdd.نتیجه);
	إذا (name != عدم)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    إذا (p != عدم && درون_داد_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&درون_داد_conv, p, عدم);
		vim_free(p);
	    }
	    وإلا
		fontname = p;
	}
	pango_font_description_free(fdd.نتیجه);
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
    GList	    *قلم_list;
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
    قلم_list = pango_itemize(gui.متن_context, (ثابت حرف *)ascii_chars,
			      0, حجم(ascii_chars), attr_list, عدم);

    إذا (قلم_list != عدم && قلم_list->بعدی == عدم)
    {
	Pango_قلم   *قلم;
	صحيح	    عرض;

	قلم  = (Pango_قلم *)قلم_list->داده;
	عرض = gui.char_عرض * PANGO_SCALE;

	gui.ascii_font = قلم->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت حرف *)ascii_chars, حجم(ascii_chars),
		    &قلم->analysis, gui.ascii_glyphs);

	إذا (gui.ascii_glyphs->رقم_glyphs == (صحيح)حجم(ascii_chars))
	{
	    لكل (i = 0; i < gui.ascii_glyphs->رقم_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_جابه_جایی += MAX(0, عرض - geom->عرض) / 2;
		geom->عرض = عرض;
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

    g_list_foreach(قلم_list, (GFunc)(فراغ *)&pango_قلم_free, عدم);
    g_list_free(قلم_list);
    pango_attr_list_unref(attr_list);
}

    ساكن فراغ
get_styled_font_variants(فراغ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_بارگیری_font(gui.متن_context, gui.norm_font);
    إذا (plain_font == عدم)
	أرجع;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_مجموعه_وزن(bold_font_desc, PANGO_وزن_BOLD);

    bold_font = pango_context_بارگیری_font(gui.متن_context, bold_font_desc);
    إذا (bold_font != عدم)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    صحيح
gui_mch_init_font(char_u *font_name, صحيح fontset استفادهنشده)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحيح			    عرض;

    إذا (font_name == عدم)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    إذا (font_desc == عدم)
	أرجع FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_مجموعه_font_description(gui.متن_context, font_desc);

    layout = pango_layout_new(gui.متن_context);
    pango_layout_مجموعه_متن(layout, "MW", 2);
    pango_layout_get_اندازه(layout, &عرض, عدم);
    g_object_unref(layout);

    gui.char_عرض = (عرض / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    إذا (gui.char_عرض <= 0)
	gui.char_عرض = 8;

    gui_mch_adjust_charheight();

    hl_مجموعه_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_جدول_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	صحيح im_ردیف = gui.ردیف < screen_ردیفها ? gui.ردیف : screen_ردیفها - 1;
	صحيح im_ستون = gui.ستون < screen_Columns ? gui.ستون : screen_Columns - 1;
	im_مجموعه_position(im_ردیف, im_ستون);
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
    إذا (pango_font_description_get_اندازه(font) <= 0)
	pango_font_description_مجموعه_اندازه(font, 10 * PANGO_SCALE);

    أرجع font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name استفادهنشده)
{
    إذا (font != NOFONT)
    {
	حرف *desc = pango_font_description_مقصد_string(font);
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
    optexpand_T	*آرگومانها,
    فراغ	*param,
    صحيح		(*جمع_کن_تطابق)(char_u *val))
{
    PangoFontFamily	**font_families = عدم;
    صحيح			n_families = 0;
    صحيح			wide = *(صحيح *)param;

    إذا (آرگومانها->oe_include_orig_val && *آرگومانها->oe_opt_مقدار == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	إذا (جمع_کن_تطابق((char_u *)DEFAULT_FONT) != OK)
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
	إذا (درون_داد_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&درون_داد_conv,
		    (char_u *)fam_name, عدم);
	    إذا (buf != عدم)
	    {
		إذا (جمع_کن_تطابق(buf) != OK)
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
	    إذا (جمع_کن_تطابق((char_u *)fam_name) != OK)
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
    إذا (!gui.درون_use)
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
color_مقصد_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    أرجع rgba;
}

    فراغ
gui_mch_مجموعه_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_مقصد_rgba(color);
}

    فراغ
gui_mch_مجموعه_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_مقصد_rgba(color);
}

    فراغ
gui_mch_مجموعه_sp_color(guicolor_T color)
{
    *gui.spcolor = color_مقصد_rgba(color);
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
gui_gtk4_update_اندازه(فراغ)
{
    vim_draw_area_مجموعه_اندازه(VIM_DRAW_AREA(gui.drawarea),
	    gui.رقم_ردیفها, gui.رقم_تعداد_ستون);
}

#اگر_تعریف_شده FEAT_NETBEANS_INTG
    فراغ
gui_gtk4_جمع_کن_multisign(
	cairo_surface_t *surf,
	صحيح		ردیف,
	صحيح		ستون,
	صحيح		عرض,
	صحيح		ارتفاع)
{
    vim_draw_area_جمع_کن_multisign(VIM_DRAW_AREA(gui.drawarea), surf, ردیف, ستون,
	    عرض, ارتفاع);
}
#پایان_اگر

    فراغ
gui_mch_clear_بلوک(صحيح row1, صحيح col1, صحيح row2, صحيح col2)
{
    vim_draw_area_clear_بلوک(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    إذا (gui.drawarea != عدم)
	gtk_widget_صف_draw(gui.drawarea);
}

    فراغ
gui_mch_clear_all(فراغ)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    إذا (gui.drawarea != عدم)
	gtk_widget_صف_draw(gui.drawarea);
}

    فراغ
gui_mch_delete_سطرها(صحيح ردیف, صحيح رقم_سطرها)
{
    vim_draw_area_move_بلوک(VIM_DRAW_AREA(gui.drawarea),
	    ردیف, ردیف + رقم_سطرها, gui.scroll_region_bot,
	    gui.scroll_region_چپ, gui.scroll_region_راست);
    gui_clear_بلوک(
	    gui.scroll_region_bot - رقم_سطرها + 1, gui.scroll_region_چپ,
	    gui.scroll_region_bot, gui.scroll_region_راست);

    gtk_widget_صف_draw(gui.drawarea);
}

    فراغ
gui_mch_درج_سطرها(صحيح ردیف, صحيح رقم_سطرها)
{
    vim_draw_area_move_بلوک(VIM_DRAW_AREA(gui.drawarea),
	    ردیف + رقم_سطرها, ردیف, gui.scroll_region_bot - رقم_سطرها,
	    gui.scroll_region_چپ, gui.scroll_region_راست);
    gui_clear_بلوک(
	    ردیف, gui.scroll_region_چپ,
	    ردیف + رقم_سطرها - 1, gui.scroll_region_راست);

    gtk_widget_صف_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    فراغ
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, صحيح w, صحيح h)
{
    gui_mch_مجموعه_bg_color(bg);
    gui_mch_مجموعه_fg_color(fg);
    vim_draw_area_مجموعه_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_صف_draw(gui.drawarea);
}

    فراغ
gui_mch_flash(صحيح msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (صحيح)ردیفها - 1, (صحيح)Columns - 1);
    gui_mch_flush();
    ui_delay((دراز)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (صحيح)ردیفها - 1, (صحيح)Columns - 1);
}

    فراغ
gui_mch_invert_rectangle(صحيح r, صحيح c, صحيح nr, صحيح nc)
{
    vim_draw_area_invert_بلوک(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_صف_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    ساكن gboolean
کلید_press_event(Gtk_Event_Controller_کلید *controller استفادهنشده,
	guint کلید_sym, guint keycode استفادهنشده,
	GdkModifierType state, gpointer داده استفادهنشده)
{
    char_u	string[32], string2[32];
    صحيح		len;
    صحيح		i;
    صحيح		modifiers;
    صحيح		کلید;
    char_u	*s, *d;

#اگر_تعریف_شده FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    إذا (xic != عدم)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	إذا (event != عدم && gtk_im_context_filter_keypress(xic, event))
	    أرجع TRUE;
    }
#پایان_اگر

#اگر_تعریف_شده FEAT_MENU
    إذا (کلید_sym == GDK_کلید_F10 && gui.menubar != عدم)
    {
	ساكن char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	إذا (check_map(k10, State, FALSE, TRUE, FALSE, عدم, عدم) == عدم)
	{
	    show_menubar_popover();
	    أرجع TRUE;
	}
    }
#پایان_اگر

    len = keyval_مقصد_string(کلید_sym, string2);

    إذا (len > 1 && درون_داد_conv.vc_type != CONV_NONE)
	len = convert_درون_داد(string2, len, حجم(string2));

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
    إذا (کلید_sym == GDK_کلید_ISO_چپ_Tab)
    {
	کلید_sym = GDK_کلید_Tab;
	state |= GDK_لغزش_ماسک;
    }

#اگر_تعریف_شده FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    إذا ((state & GDK_ALT_ماسک)
	    && gui.menu_is_فعال
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	أرجع FALSE;
#پایان_اگر

    // Check for special keys
    إذا (len == 0 || len == 1)
    {
	لكل (i = 0; special_کلیدها[i].کلید_sym != 0; i++)
	{
	    إذا (special_کلیدها[i].کلید_sym == کلید_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیدها[i].code0;
		string[2] = special_کلیدها[i].code1;
		len = -3;
		اكسر;
	    }
	}
    }

    إذا (len == 0)
	أرجع TRUE;

    إذا (len == -3)
	کلید = مقصد_SPECIAL(string[1], string[2]);
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
	string[1] = K_دومی(کلید);
	string[2] = K_سوم(کلید);
	len = 3;
    }
    وإلا
    {
	کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);
	modifiers = may_remove_لغزش_modifier(modifiers, کلید);
	len = mb_char2bytes(کلید, string);
    }

    إذا (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	جمع_کن_مقصد_درون_داد_buf(string2, 3);
    }

    {
	صحيح int_ch = check_for_وقفه(کلید, modifiers);
	إذا (int_ch != NUL)
	{
	    trash_درون_داد_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    جمع_کن_مقصد_درون_داد_buf(string, len);

    إذا (p_mh)
	gui_mch_mousehide(TRUE);

    أرجع TRUE;
}

    ساكن فراغ
کلید_release_event(Gtk_Event_Controller_کلید *controller استفادهنشده,
	guint keyval استفادهنشده, guint keycode استفادهنشده,
	GdkModifierType state استفادهنشده, gpointer داده استفادهنشده)
{
}

ساكن صحيح mouse_timed_بیرون = TRUE;
ساكن guint mouse_click_timer = 0;

    ساكن timeout_cb_type
mouse_click_timer_cb(gpointer داده)
{
    *(صحيح *)داده = TRUE;
    أرجع FALSE;
}

    ساكن صحيح
modifiers_gdk2mouse(guint state)
{
    صحيح modifiers = 0;

    إذا (state & GDK_لغزش_ماسک)
	modifiers |= MOUSE_لغزش;
    إذا (state & GDK_CONTROL_ماسک)
	modifiers |= MOUSE_CTRL;
    إذا (state & GDK_ALT_ماسک)
	modifiers |= MOUSE_ALT;

    أرجع modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#تعریف_کن ANY_BUTTON_ماسک (GDK_BUTTON1_ماسک | GDK_BUTTON2_ماسک \
	| GDK_BUTTON3_ماسک | GDK_BUTTON4_ماسک | GDK_BUTTON5_ماسک)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
ساكن صحيح mouse_pressed_button = -1;

ساكن guint motion_repeat_timer = 0;
ساكن gboolean motion_repeat_جابه_جایی = FALSE;
ساكن صحيح mouse_press_x = -1, mouse_press_y = -1;

    ساكن فراغ
button_press_event(GtkGestureClick *gesture, صحيح n_press استفادهنشده,
	مزدوج x, مزدوج y, gpointer داده استفادهنشده)
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

    إذا (!mouse_timed_بیرون && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_بیرون = FALSE;
    mouse_click_timer = timeout_جمع_کن(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_بیرون);

    بدل (btn)
    {
	حالة 1: button = MOUSE_چپ; اكسر;
	حالة 2: button = MOUSE_میانه; اكسر;
	حالة 3: button = MOUSE_راست; اكسر;
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
button_release_event(GtkGestureClick *gesture, صحيح n_press استفادهنشده,
	مزدوج x, مزدوج y, gpointer داده استفادهنشده)
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
mouse_repeat_timer_cb(gpointer داده استفادهنشده)
{
    صحيح x, y;

    إذا (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	أرجع G_منبع_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    إذا (vim_استفاده_شده_درون_درون_داد_buf() > 10)
	أرجع G_منبع_CONTINUE;

    x = (صحيح)prev_mouse_x;
    y = (صحيح)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    إذا (motion_repeat_جابه_جایی)
	x += gui.char_عرض;
    motion_repeat_جابه_جایی = !motion_repeat_جابه_جایی;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    أرجع G_منبع_CONTINUE;
}

    ساكن فراغ
motion_notify_event(GtkEventControllerMotion *controller استفادهنشده,
	مزدوج x, مزدوج y, gpointer داده استفادهنشده)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    إذا (mouse_pressed_button >= 0 && event != عدم
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_ماسک))
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
	w = gtk_widget_get_عرض(gui.drawarea);
	h = gtk_widget_get_ارتفاع(gui.drawarea);

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

	    motion_repeat_timer = timeout_جمع_کن(delay,
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
enter_notify_event(GtkEventControllerMotion *controller استفادهنشده,
	مزدوج x, مزدوج y, gpointer داده استفادهنشده)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    ساكن gboolean
scroll_event(GtkEventControllerScroll *controller استفادهنشده,
	مزدوج dx استفادهنشده, مزدوج dy, gpointer داده استفادهنشده)
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
focus_درون_event(GtkEventControllerFocus *controller استفادهنشده,
	gpointer داده استفادهنشده)
{
    إذا (gtk_پنجره_is_فعال(GTK_پنجره(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	إذا (blink_state == BLINK_NONE)
	    gui_mch_شروع_blink();
    }
}

    ساكن فراغ
focus_بیرون_event(GtkEventControllerFocus *controller استفادهنشده,
	gpointer داده استفادهنشده)
{
    إذا (!gtk_پنجره_is_فعال(GTK_پنجره(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	إذا (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#إذا defined(FEAT_IMAGE)
    ساكن فراغ
scale_ضریب_cb(GdkSurface  *surface,
	GParamSpec	    *pspec استفادهنشده,
	فراغ		    *udata استفادهنشده)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#پایان_اگر

    ساكن فراغ
drawarea_realize_cb(GtkWidget *widget استفادهنشده, gpointer داده استفادهنشده)
{
#إذا defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_ضریب_cb), عدم);
    redraw_all_later(UPD_VALID);
#پایان_اگر
    gui_mch_new_colors();
}

    ساكن فراغ
drawarea_unrealize_cb(GtkWidget *widget استفادهنشده, gpointer داده استفادهنشده)
{
#اگر_تعریف_شده FEAT_XIM
    im_shutdown();
#پایان_اگر
}

تعريفنوع تعداد
{
    READ_داده_CLIP,	    // Clipboard data
    READ_داده_DROP,	    // URI or text DND data
    READ_داده_DROP_داده	    // HTML DND data
} Read_داده_Type;

#تعریف_کن READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
تعريفنوع بنية
{
    Read_داده_Type    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    حرف	    *buf; // NULL when not needed
} Read_داده;

#اگر_تعریف_شده FEAT_DND
تعريفنوع بنية
{
    Read_داده	rd;
    GdkDrop	*drop;
    صحيح		x;
    صحيح		y;
} Drop_Read_داده;
#پایان_اگر

تعريفنوع بنية
{
    Read_داده	rd;
    Clipboard_T *cbd;
    حرف	*mime_type;
} Clip_Read_داده;

    ساكن Read_داده *
read_داده_new(Read_داده_Type type, size_t sz)
{
    Read_داده *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    إذا (type == READ_داده_CLIP || type == READ_داده_DROP_داده)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    أرجع rd;
}

    ساكن فراغ
read_داده_free(Read_داده *rd)
{
    g_cancellable_cancel(rd->cancel);

    إذا (rd->type == READ_داده_CLIP)
	g_free(((Clip_Read_داده *)rd)->mime_type);
    وإلا إذا (rd->type == READ_داده_DROP || rd->type == READ_داده_DROP_داده)
	g_object_unref(((Drop_Read_داده *)rd)->drop);
    g_object_unref(rd->cancel);
    إذا (rd->arr != عدم)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#اگر_تعریف_شده FEAT_DND
ساكن فراغ drop_read_متن(GdkDrop *drop, char_u *متن);
#پایان_اگر

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    ساكن فراغ
read_داده_درون_داد_cb(
	GInput_جریان	*جریان,
	GAsync_نتیجه	*نتیجه,
	Read_داده	*rd)
{
    ssize_t r = g_درون_داد_جریان_read_پایان(جریان, نتیجه, عدم);

    إذا (r == -1)
    {
	// Error occurred
	Drop_Read_داده *drd = (Drop_Read_داده *)rd;

	إذا (rd->type == READ_داده_DROP_داده)
	    gdk_drop_پایان(drd->drop, 0);
    }
    وإلا إذا (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	إذا (rd->type == READ_داده_CLIP)
	{
	    Clip_Read_داده    *crd = (Clip_Read_داده *)rd;
	    دراز	    len;
	    char_u	    *actual, *final;
	    صحيح		    motion_type = MAUTO;
	    char_u	    *tofree = عدم;

	    len = (دراز)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = عدم;

	    إذا (clip_convert_داده(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#اگر_تعریف_شده FEAT_DND
	وإلا إذا (rd->type == READ_داده_DROP_داده)
	{
	    Drop_Read_داده    *drd = (Drop_Read_داده *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ثابت uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = عدم;
	    drop_read_متن(drd->drop, str);
	    g_free(str);
	    gdk_drop_پایان(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#پایان_اگر
    }
    وإلا
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ثابت uint8_t *)rd->buf, r);
	g_درون_داد_جریان_read_async(جریان, rd->buf, READDATA_BUFSIZE,
		G_اولویت_بالا,
		rd->cancel, (GAsyncReadyCallback)read_داده_درون_داد_cb, rd);
	أرجع;
    }

    read_داده_free(rd);
    g_object_unref(جریان);
}

#اگر_تعریف_شده FEAT_DND
/*
 * Set up for receiving DND items.
 */
    فراغ
gui_gtk_مجموعه_dnd_targets(فراغ)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_جمع_کن_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_جمع_کن_gtype(builder, G_TYPE_STRING);
    إذا (clip_html)
	gdk_content_formats_builder_جمع_کن_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_مقصد_formats(builder);
    gtk_drop_هدف_async_مجموعه_formats(gui.drop_هدف, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    ساكن فراغ
drop_read_متن(GdkDrop *drop استفادهنشده, char_u *متن)
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

    إذا (متن == عدم || *متن == NUL || !query_اشاره_گر_pos(عدم, عدم, &state))
	أرجع;

    dnd_yank_drag_داده((char_u *)متن, (دراز)STRLEN(متن));

    dropkey[2] = modifiers_gdk2vim(state);
    إذا (dropkey[2] != 0)
	جمع_کن_مقصد_درون_داد_buf(dropkey, (صحيح)حجم(dropkey));
    وإلا
	جمع_کن_مقصد_درون_داد_buf(dropkey + 3, (صحيح)(حجم(dropkey) - 3));
}

    ساكن فراغ
drop_read_مقدار_cb(GdkDrop *drop, GAsync_نتیجه *نتیجه, Drop_Read_داده *drd)
{
    ثابت GValue    *مقدار = gdk_drop_read_مقدار_پایان(drop, نتیجه, عدم);
    gboolean	    success = FALSE;

    إذا (مقدار == عدم)
	اذهب خروج;

    إذا (G_مقدار_HOLDS(مقدار, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_مقدار_get_boxed(مقدار);
	صحيح	nfiles = g_slist_درازی(files);
	char_u	**fnames;
	صحيح	i;

	إذا (nfiles <= 0)
	    اذهب خروج;

	fnames = ALLOC_MULT(char_u *, nfiles);
	إذا (fnames == عدم)
	    اذهب خروج;

	i = 0;
	لكل (GSList *l = files; l != عدم; l = l->بعدی)
	{
	    GFile *file = l->داده;
	    حرف *مسیر = g_file_get_مسیر(file);
	    إذا (مسیر != عدم)
		fnames[i++] = vim_strsave((char_u *)مسیر);
	    g_free(مسیر);
	}
	nfiles = i;

	إذا (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	وإلا
	    vim_free(fnames);
	success = TRUE;
    }
    وإلا إذا (G_مقدار_HOLDS(مقدار, G_TYPE_STRING))
    {
	ثابت حرف *متن = g_مقدار_get_string(مقدار);

	إذا (متن == عدم || *متن == NUL)
	    اذهب خروج;

	drop_read_متن(drop, (char_u *)متن);
	success = TRUE;
    }

خروج:
    gdk_drop_پایان(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_داده_free(&drd->rd);
}

    ساكن فراغ
drop_read_cb(GdkDrop *drop, GAsync_نتیجه *نتیجه, Drop_Read_داده *drd)
{
    GInput_جریان    *درون_جریان;
    ثابت حرف	    *m;

    درون_جریان = gdk_drop_read_پایان(drop, نتیجه, &m, عدم);
    إذا (درون_جریان == عدم || STRCMP(m, "text/html") != 0)
    {
	read_داده_free(&drd->rd);
	gdk_drop_پایان(drop, 0);
	أرجع;
    }

    أكد(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_درون_داد_جریان_read_async(درون_جریان, drd->rd.buf, READDATA_BUFSIZE,
	    G_اولویت_بالا,
	    drd->rd.cancel, (GAsyncReadyCallback)read_داده_درون_داد_cb, drd);
}

// GCancellable of current DND operation, else NULL.
ساكن GCancellable *dnd_cancel = عدم;
ساكن guint dnd_timeout_id = 0;

    ساكن gboolean
drop_timeout_cb(فراغ *udata استفادهنشده)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    أرجع G_منبع_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    ساكن gboolean
drop_cb(
	Gtk_Drop_هدف_Async  *هدف استفادهنشده,
	GdkDrop		    *drop,
	مزدوج		    x,
	مزدوج		    y,
	فراغ		    *udata استفادهنشده)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    Drop_Read_داده	*drd = عدم;

    إذا (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (Drop_Read_داده *)read_داده_new(READ_داده_DROP, حجم(*drd));
	gdk_drop_read_مقدار_async(drop, GDK_TYPE_FILE_LIST, G_اولویت_بالا,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_مقدار_cb, drd);
    }
    وإلا إذا (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	ساكن ثابت حرف *m[] = {"text/html", عدم};

	drd = (Drop_Read_داده *)read_داده_new(READ_داده_DROP_داده, حجم(*drd));
	gdk_drop_read_async(drop, m, G_اولویت_بالا,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    وإلا إذا (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (Drop_Read_داده *)read_داده_new(READ_داده_DROP, حجم(*drd));
	gdk_drop_read_مقدار_async(drop, G_TYPE_STRING, G_اولویت_بالا,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_مقدار_cb, drd);
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
	dnd_timeout_id = timeout_جمع_کن(3000, drop_timeout_cb, عدم);

	أرجع TRUE;
    }
    أرجع FALSE;
}
#پایان_اگر

    ساكن فراغ
mainwin_destroy_cb(GObject *object استفادهنشده, gpointer داده استفادهنشده)
{
    gui.mainwin = عدم;
    gui.drawarea = عدم;
    إذا (!exiting)
	gui_shell_closed();
}

    ساكن gboolean
delete_event_cb(Gtk_پنجره *پنجره_2 استفادهنشده, gpointer داده استفادهنشده)
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
درون_داد_timer_cb(gpointer داده)
{
    صحيح *timed_بیرون = (صحيح *)داده;

    *timed_بیرون = TRUE;
    أرجع FALSE;	// don't call me again
}

    فراغ
gui_mch_update(فراغ)
{
    صحيح cnt = 0;

    طالما (g_main_context_pending(عدم) && !vim_is_درون_داد_buf_پر()
								&& ++cnt < 100)
	g_main_context_iteration(عدم, TRUE);
}

#اگر_تعریف_شده FEAT_کار_CHANNEL
    ساكن timeout_cb_type
channel_poll_cb(gpointer داده استفادهنشده)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    أرجع TRUE; // Keep repeating
}
#پایان_اگر

    صحيح
gui_mch_wait_for_chars(دراز wtime)
{
    صحيح		focus;
    guint	timer;
    ساكن صحيح	timed_بیرون;
    صحيح		retval = FAIL;
#اگر_تعریف_شده FEAT_کار_CHANNEL
    guint	channel_timer = 0;
#پایان_اگر

    timed_بیرون = FALSE;

    إذا (wtime >= 0)
	timer = timeout_جمع_کن(wtime == 0 ? 1L : wtime,
						   درون_داد_timer_cb, &timed_بیرون);
    وإلا
	timer = 0;

#اگر_تعریف_شده FEAT_کار_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    إذا (channel_any_keep_open())
	channel_timer = timeout_جمع_کن(20, channel_poll_cb, عدم);
#پایان_اگر

    focus = gui.درون_focus;

    افعل
    {
	// Stop or start blinking when focus changes
	إذا (gui.درون_focus != focus)
	{
	    إذا (gui.درون_focus)
		gui_mch_شروع_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.درون_focus;
	}

#اگر_تعریف_شده پیام_صف
# ifdef FEAT_TIMERS
	did_جمع_کن_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جمع_کن_timer)
	    اذهب theend;
# endif
#پایان_اگر

	إذا (gui.mainwin == عدم)
	    اذهب theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	إذا (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    اذهب theend;
	}

	إذا (!درون_داد_موجود())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(عدم, TRUE);
	    --gtk4_main_loop_level;
	}

	إذا (درون_داد_موجود())
	{
	    retval = OK;
	    اذهب theend;
	}
    } طالما (wtime < 0 || !timed_بیرون);

    gui_mch_update();

theend:
    إذا (timer != 0 && !timed_بیرون)
	timeout_remove(timer);
#اگر_تعریف_شده FEAT_کار_CHANNEL
    إذا (channel_timer != 0)
	timeout_remove(channel_timer);
#پایان_اگر

    أرجع retval;
}

    فراغ
gui_mch_flush(فراغ)
{
    // Ensure the offscreen surface content gets painted to the widget.
    إذا (gui.drawarea != عدم)
	gtk_widget_صف_draw(gui.drawarea);
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
    gtk_پنجره_minimize(GTK_پنجره(gui.mainwin));
}

    فراغ
gui_mch_مجموعه_foreground(فراغ)
{
    gtk_پنجره_present(GTK_پنجره(gui.mainwin));
}

    ساكن صحيح
query_اشاره_گر_pos(صحيح *x, صحيح *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *اشاره_گر;
    مزدوج	     sx, sy, nx, ny;
    graphene_نقطه_t src, dst;

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
    اشاره_گر = gdk_seat_get_اشاره_گر(seat);
    إذا (اشاره_گر == عدم)
	أرجع FALSE;

    إذا (!gdk_surface_get_device_position(surface, اشاره_گر, &sx, &sy, state))
	أرجع FALSE;

    إذا (x != عدم && y != عدم)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (عائم)(sx - nx);
	src.y = (عائم)(sy - ny);
	إذا (!gtk_widget_compute_نقطه(GTK_WIDGET(native), gui.drawarea,
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
    إذا (!query_اشاره_گر_pos(x, y, عدم))
    {
	*x = 0;
	*y = 0;
    }
}

    فراغ
gui_mch_setmouse(صحيح x استفادهنشده, صحيح y استفادهنشده)
{
    // GTK4/Wayland: cannot warp pointer
}

    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (gui.اشاره_گر_hidden == hide)
	أرجع;

    gui.اشاره_گر_hidden = hide;
    إذا (gui.blank_اشاره_گر != عدم)
    {
	إذا (hide)
	    gtk_widget_مجموعه_cursor(gui.drawarea, gui.blank_اشاره_گر);
	وإلا
#اگر_تعریف_شده FEAT_MOUSESHAPE
	    mch_مجموعه_mouse_shape(آخرین_shape);
#وإلا
	    gtk_widget_مجموعه_cursor(gui.drawarea, عدم);
#پایان_اگر
    }
}

    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_کلیدها[i].کلید_sym != 0; i++)
	إذا (name[0] == special_کلیدها[i].code0
		&& name[1] == special_کلیدها[i].code1)
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
gui_gtk4_update_scrollbar_اندازه(فراغ)
{
    win_T	*wp;
    صحيح		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_چپ].id;

	إذا (sbar == عدم || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_چپ]
		    && wp->w_scrollbars[SBAR_راست].id != عدم))
	    sbar = wp->w_scrollbars[SBAR_راست].id;

	إذا (sbar != عدم && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    صحيح		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_اندازه(sbar, &min, &nat);
	    sw = MAX(min.عرض, nat.عرض);
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

	gtk_widget_get_preferred_اندازه(sbar, &min, &nat);
	sh = MAX(min.ارتفاع, nat.ارتفاع);

	إذا (sh > 0)
	    h = sh;
    }

    إذا (w != -1)
	gui.scrollbar_عرض = w;
    إذا (h != -1)
	gui.scrollbar_ارتفاع = h;
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

	gtk_widget_مجموعه_visible(sb->id, flag);

	إذا (!was_visible && flag)
	    gui_gtk4_update_scrollbar_اندازه();
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
    gtk_widget_مجموعه_sensitive(menu->id, !grey);
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
    gtk_widget_مجموعه_visible(menu->id, !hidden);
    gui_mch_update();
}

    فراغ
gui_mch_draw_menubar(فراغ)
{
    // Just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#پایان_اگر

/*
 * ============================================================
 * Tabline
 * ============================================================
 */

#اگر_تعریف_شده FEAT_GUI_TABLINE
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    إذا (gui.tabline == عدم)
	أرجع;
    gtk_widget_مجموعه_visible(gui.tabline, showit);
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
	GtkEventController  *controller استفادهنشده,
	مزدوج		    x استفادهنشده,
	مزدوج		    y استفادهنشده,
	فراغ		    *udata)
{
    tabpage_hover = GPOINTER_مقصد_INT(udata);
}

    ساكن فراغ
tabline_enter_cb(
	GtkEventController  *controller استفادهنشده,
	مزدوج		    x استفادهنشده,
	مزدوج		    y استفادهنشده,
	فراغ		    *udata استفادهنشده)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    ساكن فراغ
tabline_جمع_کن_css(فراغ)
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
# إذا GTK_CHECK_نسخه(4, 12, 0)
    gtk_css_provider_بارگیری_from_string(provider, css);
# وإلا
    gtk_css_provider_بارگیری_from_داده(provider, css, -1);
# endif
    gtk_style_context_جمع_کن_provider_for_display(
	    gtk_widget_get_display(gui.tabline), GTK_STYLE_PROVIDER(provider),
	    GTK_STYLE_PROVIDER_اولویت_APPLICATION);
    g_object_unref(provider);
}

/*
 * Handle clicking the close button of a tab.
 */
    ساكن فراغ
tab_close_clicked_cb(GtkButton *button استفادهنشده, gpointer داده)
{
    صحيح idx = gtk_notebook_صفحه_رقم(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(داده));

    إذا (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    فراغ
gui_mch_update_tabline(فراغ)
{
    GtkWidget	*صفحه;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    صحيح		nr = 0;
    صحيح		tab_رقم;
    صحيح		curtabidx = 0;
    char_u	*labeltext;

    إذا (gui.tabline == عدم)
	أرجع;

    ignore_tabline_evt = TRUE;

    لكل (tp = اولی_tabpage; tp != عدم; tp = tp->tp_بعدی, ++nr)
    {
	Gtk_Notebook_صفحه	    *صفحه_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	إذا (tp == curtab)
	    curtabidx = nr;

	tab_رقم = nr + 1;

	صفحه = gtk_notebook_get_nth_صفحه(GTK_NOTEBOOK(gui.tabline), nr);
	إذا (صفحه == عدم)
	{
	    GtkWidget	*spacer;
	    GtkWidget	*close_button;

	    صفحه = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_widget_مجموعه_visible(صفحه, TRUE);
	    event_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_مجموعه_visible(event_box, TRUE);

	    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_جمع_کن_css_class(spacer, "vim-tab-spacer");
	    gtk_box_append(GTK_BOX(event_box), spacer);

	    label = gtk_label_new("-Empty-");
	    gtk_widget_مجموعه_hexpand(label, TRUE);
	    gtk_label_مجموعه_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_انتها);
	    gtk_box_append(GTK_BOX(event_box), label);
	    gtk_widget_مجموعه_visible(label, TRUE);
	    g_object_مجموعه_داده(G_OBJECT(event_box), "label", label);

	    close_button = gtk_button_new_from_icon_name("window-close-symbolic");
	    gtk_widget_جمع_کن_css_class(close_button, "flat");
	    gtk_widget_جمع_کن_css_class(close_button, "circular");
	    gtk_widget_مجموعه_valign(close_button, GTK_ALIGN_CENTER);
	    gtk_widget_مجموعه_focus_on_click(close_button, FALSE);
	    gtk_widget_مجموعه_tooltip_متن(close_button, _("Close Tab"));
	    g_signal_connect(close_button, "clicked",
		    G_CALLBACK(tab_close_clicked_cb), صفحه);
	    gtk_box_append(GTK_BOX(event_box), close_button);

	    gtk_notebook_درج_صفحه(GTK_NOTEBOOK(gui.tabline),
		    صفحه, event_box, nr);
	    gtk_notebook_مجموعه_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    صفحه, TRUE);
	    g_object_مجموعه(gtk_notebook_get_صفحه(GTK_NOTEBOOK(gui.tabline), صفحه),
		    "tab-expand", TRUE, عدم);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	صفحه_widget = gtk_notebook_get_صفحه(GTK_NOTEBOOK(gui.tabline), صفحه);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_مقصد_اشاره_گر(tab_رقم));
	gtk_event_controller_مجموعه_انتشار_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(صفحه_widget, "tab", &tab_widget, عدم);
	gtk_widget_جمع_کن_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), صفحه);
	label = g_object_get_داده(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_مقصد_UTF8(NameBuff);
	إذا (label != عدم && GTK_IS_LABEL(label))
	    gtk_label_مجموعه_متن(GTK_LABEL(label), (ثابت حرف *)labeltext);
	CONVERT_مقصد_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_مقصد_UTF8(NameBuff);
	gtk_widget_مجموعه_tooltip_متن(event_box, (ثابت gchar *)labeltext);
	CONVERT_مقصد_UTF8_FREE(labeltext);
    }

    طالما (gtk_notebook_get_nth_صفحه(GTK_NOTEBOOK(gui.tabline), nr) != عدم)
	gtk_notebook_remove_صفحه(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    إذا (nr == 1)
	gtk_widget_جمع_کن_css_class(gui.tabline, "single-tab");
    وإلا
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    إذا (gtk_notebook_get_current_صفحه(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_مجموعه_current_صفحه(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    فراغ
gui_mch_مجموعه_curtab(صحيح nr)
{
    إذا (gui.tabline != عدم)
	gtk_notebook_مجموعه_current_صفحه(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    ساكن فراغ
on_select_tab(
	GtkNotebook	*notebook استفادهنشده,
	gpointer	*صفحه استفادهنشده,
	gint		idx,
	gpointer	داده استفادهنشده)
{
    إذا (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    ساكن فراغ
on_tab_reordered(
	GtkNotebook	*notebook استفادهنشده,
	gpointer	*صفحه استفادهنشده,
	gint		idx,
	gpointer	داده استفادهنشده)
{
    إذا (ignore_tabline_evt)
	أرجع;

    إذا ((tabpage_شاخص(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    وإلا
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ساكن فراغ
tabline_menu_event_cb(Vim_Menu_قلم *قلم استفادهنشده, Vim_Menu_قلم_Event event, فراغ *udata)
{
    إذا (event == VIM_MENU_قلم_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_مقصد_INT(udata));
}

    ساكن فراغ
جمع_کن_tabline_menu_قلم(VimMenu *menu, ثابت حرف *name, صحيح resp)
{
    Vim_Menu_قلم *قلم = VIM_MENU_قلم(vim_menu_قلم_new(name,
		tabline_menu_event_cb, GINT_مقصد_اشاره_گر(resp)));

    vim_menu_درج_قلم(menu, قلم, -1);
}

/*
 * Create a menu for the tab line.
 */
    ساكن VimMenu *
create_tabline_popup_menu(فراغ)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    جمع_کن_tabline_menu_قلم(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    جمع_کن_tabline_menu_قلم(menu, _("New Tab"), TABLINE_MENU_NEW);
    جمع_کن_tabline_menu_قلم(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    أرجع menu;
}

    ساكن فراغ
tabline_menu_press_event(
	GtkGestureClick *gesture,
	صحيح		n_press استفادهنشده,
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
	rect.عرض = 1;
	rect.ارتفاع = 1;

	gtk_popover_مجموعه_pointing_مقصد(GTK_POPOVER(popover), &rect);
	gtk_popover_popup(GTK_POPOVER(popover));
    }
    وإلا إذا (but == GDK_BUTTON_PRIMARY)
    {
	إذا (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    وإلا إذا (but == GDK_BUTTON_میانه)
    {
	إذا (tabpage_hover != 0)
	    // Middle mouse click on tabpage label closes that tab.
	    send_tabline_menu_event(tabpage_hover, TABLINE_MENU_CLOSE);
    }
}
#پایان_اگر

/*
 * ============================================================
 * Sign support
 * ============================================================
 */

#إذا defined(FEAT_SIGN_ICONS)
# define SIGN_عرض  (2 * gui.char_عرض)
# define SIGN_ارتفاع (gui.char_ارتفاع)

    فراغ
gui_mch_drawsign(صحيح ردیف, صحيح ستون, صحيح typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    إذا (sign == عدم)
	أرجع;

    vim_draw_area_جمع_کن_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    ردیف, ستون, SIGN_عرض, SIGN_ارتفاع);

    gtk_widget_صف_draw(gui.drawarea);
}

    فراغ *
gui_mch_ثبات_sign(char_u *signfile)
{
    إذا (signfile[0] != NUL && signfile[0] != '-' && gui.درون_use)
    {
	GError	    *error = عدم;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ثابت حرف *)signfile,
							    &error);
	إذا (sign != عدم)
	    أرجع sign;

	semsg("E255: %s", error->پیام_2);
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
#پایان_اگر

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
gui_gtk_draw_string(صحيح ردیف, صحيح ستون, char_u *s, صحيح len, صحيح flags)
{
    صحيح cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    ردیف, ستون, s, len, flags);

    إذا (gui.drawarea != عدم)
	gtk_widget_صف_draw(gui.drawarea);

    أرجع cells;
}

    صحيح
gui_get_x11_windis(پنجره *win استفادهنشده, Display **dis استفادهنشده)
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
translate_mnemonic_برچسب(char_u *name, صحيح use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    صحيح	    n_underscores = 0;

    name = CONVERT_مقصد_UTF8(name);
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

    CONVERT_مقصد_UTF8_FREE(name);
    أرجع buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    فراغ
gui_gtk_مجموعه_mnemonics(صحيح enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	إذا (menu->id == عدم)
	    استمر;

	name = translate_mnemonic_برچسب(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	إذا (VIM_IS_MENU_BAR_قلم(menu->id))
	    vim_menu_bar_قلم_مجموعه_متن(VIM_MENU_BAR_قلم(menu->id),
		    (ثابت حرف *)name);
	vim_free(name);
    }
}

    ساكن فراغ
popupmenu_closed_cb(GtkWidget *popover, فراغ *udata استفادهنشده)
{
    gtk_widget_unparent(popover);
    إذا (gui.drawarea != عدم)
	gtk_widget_صف_draw(gui.drawarea);
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
    gtk_widget_مجموعه_والد(popover, gui.drawarea);

    rect.x = x;
    rect.y = y;
    rect.عرض = rect.ارتفاع = 1;

    // Make sure popover aligns down-and-to-the-right of the pointer.
    gtk_popover_مجموعه_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_widget_مجموعه_halign(popover, GTK_ALIGN_شروع);
    gtk_popover_مجموعه_pointing_مقصد(GTK_POPOVER(popover), &rect);

    g_signal_connect(GTK_POPOVER(popover), "closed",
	    G_CALLBACK(popupmenu_closed_cb), عدم);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    فراغ
gui_make_popup(char_u *مسیر_name, صحيح mouse_pos)
{
    vimmenu_T	*menu;
    صحيح		x, y;

    menu = gui_پیدا_کن_menu(مسیر_name);
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
#پایان_اگر // FEAT_MENU

    صحيح
get_menu_tool_عرض(فراغ)
{
    أرجع 0;
}

    صحيح
get_menu_tool_ارتفاع(فراغ)
{
    GtkWidget *widgets[] = {
#اگر_تعریف_شده FEAT_MENU
	gui.menubar,
#پایان_اگر
#اگر_تعریف_شده FEAT_TOOLBAR
	gui.toolbar,
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_TABLINE
	gui.tabline
#پایان_اگر
    };

    صحيح ارتفاع = 0;

    لكل (صحيح i = 0; i < (صحيح)ARRAY_درازی(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	صحيح		h;

	إذا (widgets[i] == عدم || !gtk_widget_get_visible(widgets[i]))
	    استمر;

	h = gtk_widget_get_ارتفاع(widgets[i]);

	إذا (h == 0)
	{
	    // Allocation hasn't been updated yet (widget just became visible).
	    // Query the preferred height so the caller gets a valid value
	    // before the layout pass runs.  Use the maximum of minimum and
	    // natural height: GTK may allocate min_h even when natural_h is
	    // smaller (e.g. GtkNotebook tab bar has min_h > natural_h due to
	    // CSS).
	    gtk_widget_get_preferred_اندازه(widgets[i], &min, &nat);
	    ارتفاع += MAX(min.ارتفاع, nat.ارتفاع);
	}
	وإلا
	    ارتفاع += h;
    }
    أرجع ارتفاع;
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
	gdk_content_provider_content_تغییریافته(gui.regular_provider);
    وإلا
	gdk_content_provider_content_تغییریافته(gui.primary_provider);
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
clip_read_cb(GdkClipboard *cb, GAsync_نتیجه *نتیجه, Clip_Read_داده *crd)
{
    GInput_جریان    *درون_جریان;
    ثابت حرف	    *mime_type;

    درون_جریان = gdk_clipboard_read_پایان(cb, نتیجه, &mime_type, عدم);
    إذا (درون_جریان == عدم)
    {
	read_داده_free(&crd->rd);
	أرجع;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_درون_داد_جریان_read_async(درون_جریان, crd->rd.buf, READDATA_BUFSIZE,
	    G_اولویت_بالا,
	    crd->rd.cancel, (GAsyncReadyCallback)read_داده_درون_داد_cb, crd);
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
    Clip_Read_داده *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, عدم);
    إذا (clipboard == عدم)
	أرجع;

    crd = (Clip_Read_داده *)read_داده_new(READ_داده_CLIP, حجم(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_اولویت_بالا, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t شروع;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	شروع = زمان(عدم);
	طالما (!g_cancellable_is_cancelled(cancel)
		&& زمان(عدم) < شروع + 3)
	    g_main_context_iteration(عدم, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

ساكن صحيح درون_clipboard_مجموعه = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    فراغ
clip_mch_مجموعه_selection(Clipboard_T *cbd استفادهنشده)
{
}

    ساكن فراغ
clipboard_تغییریافته_cb(GdkClipboard *clipboard, gpointer user_داده)
{
    Clipboard_T *cbd = (Clipboard_T *)user_داده;

    إذا (درون_clipboard_مجموعه)
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

    درون_clipboard_مجموعه = TRUE;
    ret = gdk_clipboard_مجموعه_content(cb, cp);
    درون_clipboard_مجموعه = FALSE;

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
	gdk_clipboard_مجموعه_content(clipboard, عدم);
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
ساكن ثابت حرف *mshape_css_نامها[] =
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
mch_مجموعه_mouse_shape(صحيح shape)
{
    GdkCursor	*c;
    ثابت حرف	*css_name = "default";

    إذا (gui.drawarea == عدم)
	أرجع;

    إذا (shape == MSHAPE_HIDE || gui.اشاره_گر_hidden)
	gtk_widget_مجموعه_cursor(gui.drawarea, gui.blank_اشاره_گر);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	وإلا إذا (shape < (صحيح)ARRAY_درازی(mshape_css_نامها))
	    css_name = mshape_css_نامها[shape];
	وإلا
	    أرجع;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, عدم);
	gtk_widget_مجموعه_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    إذا (shape != MSHAPE_HIDE)
	آخرین_shape = shape;
}

#پایان_اگر // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



ساكن صحيح آخرین_متن_area_w = 0;
ساكن صحيح آخرین_متن_area_h = 0;

/*
 * ============================================================
 * Menu functions
 * ============================================================
 */

#اگر_تعریف_شده FEAT_MENU

# ifdef FEAT_TOOLBAR
/*
 * Icon name table for toolbar buttons.
 * Must match toolbar_names[] in menu.c.
 */
ساكن ثابت حرف * ثابت toolbar_icon_نامها[] =
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
toolbar_button_clicked_cb(GtkWidget *widget استفادهنشده, gpointer داده)
{
    gui_menu_cb((vimmenu_T *)داده);
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
		gtk_widget_مجموعه_اندازه_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    إذا (image == عدم)
    {
	ثابت حرف *icon_name = "image-missing";
	صحيح n = (صحيح)ARRAY_درازی(toolbar_icon_نامها);

	إذا (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_نامها[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    أرجع image;
}
# endif

    ساكن فراغ
menu_button_clicked_cb(
	Vim_Menu_قلم	    *قلم استفادهنشده,
	Vim_Menu_قلم_Event    event,
	vimmenu_T	    *menu)
{
    إذا (event == VIM_MENU_قلم_CLICKED)
	gui_menu_cb(menu);
    وإلا إذا (event == VIM_MENU_قلم_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	ساكن gboolean did_پیام = FALSE;

	إذا (State & نما_CMDLINE)
	    أرجع;

	tooltip = CONVERT_مقصد_UTF8(menu->رشتهها[MENU_شاخص_TIP]);
	إذا (tooltip != عدم && utf_valid_string(tooltip, عدم))
	{
	    پیام((حرف *)tooltip);
	    did_پیام = TRUE;
	    setcursor();
	    بیرون_flush_cursor(TRUE, FALSE);
	}
	وإلا إذا (did_پیام)
	{
	    پیام("");
	    did_پیام = FALSE;
	    setcursor();
	    بیرون_flush_cursor(TRUE, FALSE);
	}
	CONVERT_مقصد_UTF8_FREE(tooltip);
    }
}

    فراغ
gui_mch_جمع_کن_menu(vimmenu_T *menu, صحيح idx)
{
    vimmenu_T	*والد;
    GtkWidget	*والد_widget;
    gboolean	use_mnemonic;
    char_u	*متن;

    إذا (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_مجموعه_والد(menu->submenu_id, gui.drawarea);
	أرجع;
    }

    والد = menu->والد;

    إذا ((والد != عدم && والد->submenu_id == عدم)
	    || !menu_is_menubar(menu->name))
	أرجع;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = والد != عدم || p_wak[0] != 'n';
    متن = translate_mnemonic_برچسب(menu->name, use_mnemonic);

    إذا (والد != عدم)
    {
	والد_widget = والد->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_قلم_new(
		    (ثابت حرف *)متن, عدم, عدم));

	vim_menu_قلم_مجموعه_submenu(VIM_MENU_قلم(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_درج_قلم(VIM_MENU(والد_widget),
		VIM_MENU_قلم(menu->id), idx);
    }
    وإلا
    {
	والد_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_قلم_new(
		    (ثابت حرف *)متن, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_درج_قلم(VIM_MENU_BAR(والد_widget),
		VIM_MENU_BAR_قلم(menu->id), idx);
    }

    vim_free(متن);
}

    فراغ
gui_mch_جمع_کن_menu_قلم(vimmenu_T *menu, صحيح idx)
{
    vimmenu_T	*والد = menu->والد;

# ifdef FEAT_TOOLBAR
    إذا (والد != عدم && menu_is_toolbar(والد->name))
    {
	إذا (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_درج_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	وإلا
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*متن;
	    char_u	*tooltip;

	    متن    = CONVERT_مقصد_UTF8(menu->dname);
	    tooltip = CONVERT_مقصد_UTF8(menu->رشتهها[MENU_شاخص_TIP]);
	    إذا (tooltip != عدم && !utf_valid_string(tooltip, عدم))
		CONVERT_مقصد_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_درج_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ثابت حرف *)متن, idx);
	    gtk_widget_مجموعه_tooltip_متن(btn, (ثابت gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_مقصد_UTF8_FREE(متن);
	    CONVERT_مقصد_UTF8_FREE(tooltip);
	}
	أرجع;
    }
# endif

    // Menu items (non-toolbar)
    إذا (والد == عدم || والد->submenu_id == عدم)
	أرجع;

    إذا (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_درج_separator(
		VIM_MENU(والد->submenu_id), idx));
    }
    وإلا
    {
	char_u	    *متن;
	char_u	    *accel_متن = عدم;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	متن = translate_mnemonic_برچسب(menu->name, use_mnemonic);

	إذا (menu->actext != عدم && menu->actext[0] != NUL)
	    accel_متن = CONVERT_مقصد_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_قلم_new((ثابت حرف *)متن,
		    (Vim_Menu_قلم_Func)menu_button_clicked_cb, menu));

	إذا (accel_متن != عدم)
	    vim_menu_قلم_مجموعه_accel(VIM_MENU_قلم(menu->id),
		    (ثابت حرف *)accel_متن);

	vim_menu_درج_قلم(VIM_MENU(والد->submenu_id),
		VIM_MENU_قلم(menu->id), idx);

	vim_free(متن);
	CONVERT_مقصد_UTF8_FREE(accel_متن);
    }
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable استفادهنشده)
{
    // GTK4: tearoff menus don't exist.
}

    فراغ
gui_mch_menu_مجموعه_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    إذا (menu->id == عدم)
	أرجع;

    tooltip = CONVERT_مقصد_UTF8(menu->رشتهها[MENU_شاخص_TIP]);
    إذا (tooltip != عدم && utf_valid_string(tooltip, عدم))
	gtk_widget_مجموعه_tooltip_متن(menu->id, (ثابت حرف *)tooltip);
    CONVERT_مقصد_UTF8_FREE(tooltip);
}

    فراغ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    إذا (menu->والد != عدم && menu_is_toolbar(menu->والد->name))
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
    وإلا إذا (menu->والد == عدم && menu->id != عدم)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    وإلا إذا (menu->id != عدم)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->والد->submenu_id), menu->id);

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
    menu = vim_menu_bar_مقصد_menu(VIM_MENU_BAR(gui.menubar));

    gtk_widget_مجموعه_والد(menu, gui.drawarea);
    gtk_popover_مجموعه_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    rect.x = 0;
    rect.y = 0;
    rect.عرض = 1;
    rect.ارتفاع = 1;
    gtk_popover_مجموعه_pointing_مقصد(GTK_POPOVER(menu), &rect);
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), عدم);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#پایان_اگر // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    فراغ
gui_mch_مجموعه_scrollbar_thumb(scrollbar_T *sb, دراز val, دراز اندازه, دراز max)
{
    GtkAdjustment *adj;

    إذا (sb->id == عدم)
	أرجع;
    إذا (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	أرجع;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_مجموعه_پایینی(adj, 0.0);
    gtk_adjustment_مجموعه_فوقانی(adj, (gdouble)max + 1);
    gtk_adjustment_مجموعه_مقدار(adj, (gdouble)val);
    gtk_adjustment_مجموعه_گام_increment(adj, 1.0);
    gtk_adjustment_مجموعه_صفحه_increment(adj, (gdouble)(اندازه > 2 ? اندازه - 2 : 1));
    gtk_adjustment_مجموعه_صفحه_اندازه(adj, (gdouble)اندازه);
}

    فراغ
gui_mch_مجموعه_scrollbar_pos(scrollbar_T *sb, صحيح x, صحيح y, صحيح w, صحيح h)
{
    إذا (sb->id != عدم)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    صحيح formwin_w = gtk_widget_get_عرض(gui.formwin);
    صحيح sbar_w = 0;
    صحيح xpad;

    إذا (gui.which_scrollbars[SBAR_چپ])
	sbar_w += gui.scrollbar_عرض;
    إذا (gui.which_scrollbars[SBAR_راست])
	sbar_w += gui.scrollbar_عرض;

    xpad = formwin_w - آخرین_متن_area_w - sbar_w;
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    صحيح formwin_h = gtk_widget_get_ارتفاع(gui.formwin);
    صحيح ypad;

    ypad = formwin_h - آخرین_متن_area_h;
    إذا (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_ارتفاع;

    أرجع (ypad < 0) ? 0 : ypad;
}

    ساكن فراغ
adjustment_مقدار_تغییریافته(GtkAdjustment *adj, gpointer داده استفادهنشده)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_داده(G_OBJECT(adj), "vim-sb");
    دراز مقدار = (دراز)gtk_adjustment_get_مقدار(adj);

    إذا (sb != عدم)
	gui_drag_scrollbar(sb, مقدار, FALSE);
}

    فراغ
gui_mch_create_scrollbar(scrollbar_T *sb, صحيح orient)
{
    GtkAdjustment *adj;
    إذا (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, عدم);
    وإلا
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, عدم);

    gtk_widget_جمع_کن_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_مجموعه_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_مجموعه_داده(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_مقدار_تغییریافته), عدم);
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
gui_mch_مجموعه_متن_area_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    إذا (w <= 0 || h <= 0)
	أرجع;
    آخرین_متن_area_w = w;
    آخرین_متن_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    فراغ
gui_gtk4_calculate_bleed(صحيح عرض, صحيح ارتفاع)
{
    صحيح old_راست = gui.bleed_راست;
    صحيح old_bot = gui.bleed_bot;

    gui.bleed_راست = عرض - آخرین_متن_area_w;
    gui.bleed_bot = ارتفاع - آخرین_متن_area_h;

    إذا (gui.which_scrollbars[SBAR_چپ])
	gui.bleed_راست -= gui.scrollbar_عرض;
    إذا (gui.which_scrollbars[SBAR_راست])
	gui.bleed_راست -= gui.scrollbar_عرض;
    إذا (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_ارتفاع;

    // Not sure if this can happen, but be safe...
    إذا (gui.bleed_راست < 0)
	gui.bleed_راست = 0;
    إذا (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    إذا (old_راست != gui.bleed_راست || old_bot != gui.bleed_bot)
	gtk_widget_صف_draw(GTK_WIDGET(gui.drawarea));
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
    GFile	*نتیجه;
    gboolean	انجامشده;
} File_Dialog_داده;

    ساكن فراغ
file_dialog_open_cb(GObject *منبع, GAsync_نتیجه *res, gpointer داده)
{
    File_Dialog_داده *fdd = (File_Dialog_داده *)داده;
    fdd->نتیجه = gtk_file_dialog_open_پایان(
		    GTK_FILE_DIALOG(منبع), res, عدم);
    fdd->انجامشده = TRUE;
}

    ساكن فراغ
file_dialog_save_cb(GObject *منبع, GAsync_نتیجه *res, gpointer داده)
{
    File_Dialog_داده *fdd = (File_Dialog_داده *)داده;
    fdd->نتیجه = gtk_file_dialog_save_پایان(
		    GTK_FILE_DIALOG(منبع), res, عدم);
    fdd->انجامشده = TRUE;
}

    ساكن فراغ
file_dialog_folder_cb(GObject *منبع, GAsync_نتیجه *res, gpointer داده)
{
    File_Dialog_داده *fdd = (File_Dialog_داده *)داده;
    fdd->نتیجه = gtk_file_dialog_select_folder_پایان(
		    GTK_FILE_DIALOG(منبع), res, عدم);
    fdd->انجامشده = TRUE;
}

    char_u *
gui_mch_browse(صحيح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext استفادهنشده,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    File_Dialog_داده  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *نتیجه = عدم;
    GListStore	    *filters;

    title = CONVERT_مقصد_UTF8(title);

    إذا (initdir == عدم || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    وإلا إذا (vim_پر_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    جمع_کن_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_مجموعه_modal(dlg, TRUE);
    إذا (title != عدم)
	gtk_file_dialog_مجموعه_title(dlg, (ثابت حرف *)title);

    {
	GFile *dir = g_file_new_for_مسیر((ثابت حرف *)dirbuf);
	gtk_file_dialog_مجموعه_initial_folder(dlg, dir);
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
			gtk_file_filter_مجموعه_name(gfilter, (gchar *)patt);
		    وإلا
		    {
			gtk_file_filter_جمع_کن_الگو(gfilter, (gchar *)patt);
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
    gtk_file_dialog_مجموعه_filters(dlg, G_LIST_MODEL(filters));
    g_object_unref(filters);

    إذا (saving && dflt != عدم && *dflt != NUL)
	gtk_file_dialog_مجموعه_initial_name(dlg, (ثابت حرف *)dflt);

    fdd.نتیجه = عدم;
    fdd.انجامشده = FALSE;

    إذا (saving)
	gtk_file_dialog_save(dlg, GTK_پنجره(gui.mainwin), عدم,
		file_dialog_save_cb, &fdd);
    وإلا
	gtk_file_dialog_open(dlg, GTK_پنجره(gui.mainwin), عدم,
		file_dialog_open_cb, &fdd);

    طالما (!fdd.انجامشده)
	g_main_context_iteration(عدم, TRUE);

    إذا (fdd.نتیجه != عدم)
    {
	حرف *مسیر = g_file_get_مسیر(fdd.نتیجه);
	إذا (مسیر != عدم)
	{
	    نتیجه = vim_strsave((char_u *)مسیر);
	    g_free(مسیر);
	}
	g_object_unref(fdd.نتیجه);
    }

    g_object_unref(dlg);
    CONVERT_مقصد_UTF8_FREE(title);

    أرجع نتیجه;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    File_Dialog_داده	fdd;
    char_u		*نتیجه = عدم;

    title = CONVERT_مقصد_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_مجموعه_modal(dlg, TRUE);
    إذا (title != عدم)
	gtk_file_dialog_مجموعه_title(dlg, (ثابت حرف *)title);

    إذا (initdir != عدم && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_مسیر((ثابت حرف *)initdir);
	gtk_file_dialog_مجموعه_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.نتیجه = عدم;
    fdd.انجامشده = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_پنجره(gui.mainwin), عدم,
	    file_dialog_folder_cb, &fdd);

    طالما (!fdd.انجامشده)
	g_main_context_iteration(عدم, TRUE);

    إذا (fdd.نتیجه != عدم)
    {
	حرف *مسیر = g_file_get_مسیر(fdd.نتیجه);
	إذا (مسیر != عدم)
	{
	    نتیجه = vim_strsave((char_u *)مسیر);
	    g_free(مسیر);
	}
	g_object_unref(fdd.نتیجه);
    }

    g_object_unref(dlg);
    CONVERT_مقصد_UTF8_FREE(title);

    أرجع نتیجه;
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
تقسیم_button_string(char_u *button_string, صحيح *n_buttons, حرف **tofree)
{
    حرف	    **array;
    char_u	    *p;
    بیعلامت صحيح    شمار = 1;

    button_string = (char_u *)g_strdup((ثابت حرف *)button_string);

    لكل (p = button_string; *p != NUL; ++p)
	إذا (*p == DLG_BUTTON_SEP)
	    ++شمار;

    array = g_malloc_n(شمار, حجم(حرف *));
    شمار = 0;

    إذا (array != عدم)
    {
	array[شمار++] = (حرف *)button_string;
	لكل (p = button_string; *p != NUL; )
	{
	    إذا (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[شمار++] = (حرف *)p;
	    }
	    وإلا إذا (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    وإلا
		MB_PTR_ADV(p);
	}
    }

    *tofree = (حرف *)button_string;
    *n_buttons = شمار;
    أرجع array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    ساكن ثابت حرف *
dialog_type_مقصد_icon(صحيح type)
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
    gboolean	*انجامشده;
} DialogButtonState;

تعريفنوع بنية
{
    GtkWidget	*win;
    gboolean	*انجامشده;
    gboolean	no_alt;
} DialogState;

    ساكن فراغ
dialog_button_clicked_cb(GtkButton *button استفادهنشده, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->انجامشده = TRUE;
}

    ساكن gboolean
dialog_کلید_pressed_cb(
	Gtk_Event_Controller_کلید	*controller استفادهنشده,
	guint			keyval,
	guint			keycode استفادهنشده,
	GdkModifierType		state,
	DialogState		*dstate)
{
    إذا (keyval == GDK_کلید_Escape)
    {
	*dstate->انجامشده = TRUE;
	أرجع TRUE;
    }

    إذا (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_ماسک()))
	أرجع gtk_widget_mnemonic_activate(dstate->win, FALSE);
    أرجع FALSE;
}

    ساكن gboolean
dialog_close_request_cb(Gtk_پنجره *win استفادهنشده, gboolean *win_closed)
{
    *win_closed = TRUE;
    أرجع FALSE;
}

    صحيح
gui_mch_dialog(
	صحيح	type,
	char_u	*title,
	char_u	*پیام_2,
	char_u	*buttons,
	صحيح	def_but,
	char_u	*textfield,
	صحيح	ex_cmd استفادهنشده)
{
    Gtk_پنجره		*win = GTK_پنجره(gtk_پنجره_new());
    GtkWidget		*vertbox;
    GtkWidget		*پیام_box;
    ثابت حرف		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*مدخل = عدم;
    char_u		*utf8_title;
    char_u		*utf8_پیام;
    GtkEventController *کلید_controller;
    DialogButtonState	*but_حالتها = عدم;
    حرف		*tofree = عدم;
    صحيح			response = -1;
    gboolean		انجامشده = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_مقصد_UTF8(title);
    إذا (utf8_title != عدم)
	gtk_پنجره_مجموعه_title(win, (ثابت حرف *)utf8_title);
    CONVERT_مقصد_UTF8_FREE(utf8_title);

    gtk_پنجره_مجموعه_transient_for(win, GTK_پنجره(gui.mainwin));
    gtk_پنجره_مجموعه_modal(win, TRUE);
    gtk_پنجره_مجموعه_default_اندازه(win, 300, -1);
    gtk_پنجره_مجموعه_destroy_with_والد(win, TRUE);
    g_signal_connect(win, "close-request",
	    G_CALLBACK(dialog_close_request_cb), &win_closed);

    // Create main vertical layout container
    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_پنجره_مجموعه_فرزند(win, vertbox);
    gtk_widget_مجموعه_margin_بالاترین(vertbox, 24);
    gtk_widget_مجموعه_margin_bottom(vertbox, 24);
    gtk_widget_مجموعه_margin_شروع(vertbox, 12);
    gtk_widget_مجموعه_margin_انتها(vertbox, 12);

    // Add the message label
    پیام_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_مجموعه_halign(پیام_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), پیام_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_مقصد_icon(type);
    إذا (icon_name != عدم)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_مجموعه_icon_اندازه(GTK_IMAGE(icon), GTK_ICON_اندازه_LARGE);
	gtk_box_append(GTK_BOX(پیام_box), icon);
    }

    utf8_پیام = CONVERT_مقصد_UTF8(پیام_2);
    label = gtk_label_new((ثابت حرف *)utf8_پیام);
    CONVERT_مقصد_UTF8_FREE(utf8_پیام);
    gtk_label_مجموعه_wrap(GTK_LABEL(label), TRUE);
    gtk_label_مجموعه_max_عرض_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(پیام_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    کلید_controller = gtk_event_controller_کلید_new();
    g_signal_connect(کلید_controller, "key-pressed",
	    G_CALLBACK(dialog_کلید_pressed_cb), &state);
    gtk_widget_جمع_کن_controller(GTK_WIDGET(win), کلید_controller);
    state.انجامشده = &انجامشده;
    state.win = GTK_WIDGET(win);

    إذا (textfield != عدم)
    {
	// Add text entry so user can enter text
	char_u *utf8_متن = CONVERT_مقصد_UTF8(textfield);

	مدخل = gtk_مدخل_new();

	إذا (utf8_متن != عدم)
	    gtk_editable_مجموعه_متن(GTK_EDITABLE(مدخل), (ثابت حرف *)utf8_متن);
	وإلا
	    gtk_editable_مجموعه_متن(GTK_EDITABLE(مدخل), "");
	CONVERT_مقصد_UTF8_FREE(utf8_متن);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_مدخل_مجموعه_activates_default(GTK_مدخل(مدخل), TRUE);
	gtk_box_append(GTK_BOX(vertbox), مدخل);
	state.no_alt = FALSE;
    }
    وإلا
    {
	GListModel  *controllers;
	صحيح	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_اقلام(controllers);
	لكل (صحيح i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_قلم(controllers, i);
	    إذا (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_مجموعه_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_پنجره_مجموعه_mnemonics_visible(win, TRUE);
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

	gtk_widget_مجموعه_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_مجموعه_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = تقسیم_button_string(buttons, &n_buttons, &tofree);

	but_حالتها = g_malloc_n(n_buttons, حجم(DialogButtonState));

	لكل (صحيح i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_حالتها + i;

	    but_label = CONVERT_مقصد_UTF8((char_u *)buttons_arr[i]);
	    إذا (but_label == عدم)
		استمر;

	    but = gtk_button_new_with_mnemonic((حرف *)but_label);
	    إذا (i == def_but - 1)
		gtk_پنجره_مجموعه_default_widget(win, but);
	    gtk_box_append(GTK_BOX(but_box), but);
	    CONVERT_مقصد_UTF8_FREE(but_label);

	    but_state->but_idx = i;
	    but_state->response = &response;
	    but_state->انجامشده = &انجامشده;

	    g_signal_connect(but, "clicked",
		    G_CALLBACK(dialog_button_clicked_cb), but_state);
	}
	g_free(buttons_arr);
    }

    gtk_پنجره_present(win);

    طالما (!انجامشده && !win_closed)
	g_main_context_iteration(عدم, TRUE);

    إذا (انجامشده)
    {
	إذا (textfield != عدم)
	{
	    // Get the text the user entered
	    char_u *متن;

	    متن = (char_u *)gtk_editable_get_متن(GTK_EDITABLE(مدخل));
	    متن = CONVERT_FROM_UTF8(متن);
	    vim_strncpy(textfield, متن, IOSIZE - 1);
	    CONVERT_FROM_UTF8_FREE(متن);
	}

	gtk_پنجره_destroy(win);
    }
    g_free(but_حالتها);
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
    GtkWidget *به_بالا;	    // Direction up radio
    GtkWidget *پایین;	    // Direction down radio
    GtkWidget *پیدا_کن;	    // 'Find Next' action button
    GtkWidget *replace;	    // 'Replace With' action button
    GtkWidget *all;	    // 'Replace All' action button
} Shared_پیدا_کن_Replace_2;

ساكن Shared_پیدا_کن_Replace_2 پیدا_کن_widgets = {0};
ساكن Shared_پیدا_کن_Replace_2 repl_widgets = {0};

    ساكن فراغ
پیدا_کن_replace_cb(GtkWidget *widget استفادهنشده, gpointer داده)
{
    صحيح			flags;
    char_u		*پیدا_کن_متن;
    char_u		*repl_متن;
    gboolean		direction_پایین;
    Shared_پیدا_کن_Replace_2	*sfr;

    flags = GPOINTER_مقصد_INT(داده);

    إذا (flags == FRD_FINDNEXT)
    {
	repl_متن = عدم;
	sfr = &پیدا_کن_widgets;
    }
    وإلا
    {
	repl_متن = (char_u *)gtk_editable_get_متن(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    پیدا_کن_متن = (char_u *)gtk_editable_get_متن(GTK_EDITABLE(sfr->what));
    direction_پایین = gtk_check_button_get_فعال(
	    GTK_CHECK_BUTTON(sfr->پایین));

    إذا (gtk_check_button_get_فعال(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_کامل_کلمه;
    إذا (gtk_check_button_get_فعال(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_تطابق_CASE;

    repl_متن = CONVERT_FROM_UTF8(repl_متن);
    پیدا_کن_متن = CONVERT_FROM_UTF8(پیدا_کن_متن);
    gui_do_findrepl(flags, پیدا_کن_متن, repl_متن, direction_پایین);
    CONVERT_FROM_UTF8_FREE(repl_متن);
    CONVERT_FROM_UTF8_FREE(پیدا_کن_متن);
}

    ساكن فراغ
dialog_destroyed_cb(GtkWidget *widget استفادهنشده, gpointer داده)
{
    *(GtkWidget **)داده = عدم;
}

    ساكن فراغ
مدخل_تغییریافته_cb(GtkWidget *مدخل, GtkWidget *dialog)
{
    ثابت gchar	*مدخل_متن;
    gboolean	nonempty;

    مدخل_متن = gtk_editable_get_متن(GTK_EDITABLE(مدخل));

    إذا (!مدخل_متن)
	أرجع; // Shouldn't happen

    nonempty = (مدخل_متن[0] != '\0');

    إذا (dialog == پیدا_کن_widgets.dialog)
	gtk_widget_مجموعه_sensitive(پیدا_کن_widgets.پیدا_کن, nonempty);

    إذا (dialog == repl_widgets.dialog)
    {
	gtk_widget_مجموعه_sensitive(repl_widgets.پیدا_کن, nonempty);
	gtk_widget_مجموعه_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_مجموعه_sensitive(repl_widgets.all, nonempty);
    }
}

    ساكن gboolean
پیدا_کن_کلید_pressed_cb(
	Gtk_Event_Controller_کلید	*controller استفادهنشده,
	guint			keyval,
	guint			keycode استفادهنشده,
	GdkModifierType		state,
	Shared_پیدا_کن_Replace_2	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    إذا (state & (GDK_CONTROL_ماسک | GDK_لغزش_ماسک))
	أرجع FALSE;

    // the Escape key synthesizes a cancellation action
    إذا (keyval == GDK_کلید_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_پنجره_destroy(GTK_پنجره(frdp->dialog));
	أرجع TRUE;
    }

    أرجع FALSE;
}

    ساكن فراغ
مدخل_activate_cb(GtkWidget *widget استفادهنشده, فراغ *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    ساكن فراغ
پیدا_کن_replace_dialog_create(char_u *arg, صحيح do_replace)
{
    Shared_پیدا_کن_Replace_2	*frdp;
    char_u		*مدخل_متن;
    صحيح			wword = FALSE;
    صحيح			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*کلید_controller;

    frdp = do_replace ? &repl_widgets : &پیدا_کن_widgets;
    مدخل_متن = get_پیدا_کن_dialog_متن(arg, &wword, &mcase);

    إذا (مدخل_متن != عدم && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = مدخل_متن;
	مدخل_متن = string_convert(&output_conv, مدخل_متن, عدم);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    إذا (frdp->dialog != عدم)
    {
	إذا (مدخل_متن != عدم)
	{
	    gtk_editable_مجموعه_متن(GTK_EDITABLE(frdp->what),
		    (حرف *)مدخل_متن);
	    gtk_check_button_مجموعه_فعال(GTK_CHECK_BUTTON(frdp->wword),
		    (gboolean)wword);
	    gtk_check_button_مجموعه_فعال(GTK_CHECK_BUTTON(frdp->mcase),
		    (gboolean)mcase);
	}
	gtk_پنجره_present(GTK_پنجره(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	إذا (do_replace && g_utf8_strlen(
		    gtk_editable_get_متن(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(مدخل_متن);
	أرجع;
    }

    // Create a new dialog window.
    frdp->dialog = gtk_پنجره_new();
    gtk_پنجره_مجموعه_transient_for(GTK_پنجره(frdp->dialog),
	    GTK_پنجره(gui.mainwin));
    gtk_پنجره_مجموعه_destroy_with_والد(GTK_پنجره(frdp->dialog), TRUE);
    gtk_پنجره_مجموعه_title(GTK_پنجره(frdp->dialog),
	    do_replace ? _("VIM - Search and Replace...")
	    : _("VIM - Search..."));
    gtk_پنجره_مجموعه_resizable(GTK_پنجره(frdp->dialog), FALSE);

    g_signal_connect(frdp->dialog, "destroy",
	    G_CALLBACK(dialog_destroyed_cb), &frdp->dialog);

    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_مجموعه_margin_شروع(vertbox, 12);
    gtk_widget_مجموعه_margin_انتها(vertbox, 12);
    gtk_widget_مجموعه_margin_بالاترین(vertbox, 12);
    gtk_widget_مجموعه_margin_bottom(vertbox, 12);
    gtk_پنجره_مجموعه_فرزند(GTK_پنجره(frdp->dialog), vertbox);

    // Grid for labels + entries
    grid = gtk_grid_new();
    gtk_grid_مجموعه_ردیف_spacing(GTK_GRID(grid), 6);
    gtk_grid_مجموعه_ستونی_spacing(GTK_GRID(grid), 6);
    gtk_box_append(GTK_BOX(vertbox), grid);

    // "Find what:" label + entry
    tmp = gtk_label_new(_("Find what:"));
    gtk_label_مجموعه_xalign(GTK_LABEL(tmp), 0.0);
    gtk_grid_attach(GTK_GRID(grid), tmp, 0, 0, 1, 1);

    frdp->what = gtk_مدخل_new();
    gtk_widget_مجموعه_hexpand(frdp->what, TRUE);
    sensitive = (مدخل_متن != عدم && مدخل_متن[0] != NUL);
    إذا (مدخل_متن != عدم)
	gtk_editable_مجموعه_متن(GTK_EDITABLE(frdp->what), (حرف *)مدخل_متن);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    إذا (do_replace)
    {
	// "Replace with:" label + entry
	tmp = gtk_label_new(_("Replace with:"));
	gtk_label_مجموعه_xalign(GTK_LABEL(tmp), 0.0);
	gtk_grid_attach(GTK_GRID(grid), tmp, 0, 1, 1, 1);

	frdp->with = gtk_مدخل_new();
	gtk_widget_مجموعه_hexpand(frdp->with, TRUE);
	gtk_grid_attach(GTK_GRID(grid), frdp->with, 1, 1, 1, 1);
	gtk_مدخل_مجموعه_activates_default(GTK_مدخل(frdp->with), TRUE);

	// Make the entry activation only change the input focus onto the
	// with item.
	gtk_مدخل_مجموعه_activates_default(GTK_مدخل(frdp->what), FALSE);
	g_signal_connect(G_OBJECT(frdp->what), "activate",
		G_CALLBACK(مدخل_activate_cb), frdp->with);
    }
    وإلا
	// Make the entry activation do the search.
	gtk_مدخل_مجموعه_activates_default(GTK_مدخل(frdp->what), TRUE);

    // Checkboxes
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    frdp->wword = gtk_check_button_new_with_label(_("Match whole word only"));
    gtk_check_button_مجموعه_فعال(GTK_CHECK_BUTTON(frdp->wword),
	    (gboolean)wword);
    gtk_box_append(GTK_BOX(hbox), frdp->wword);

    frdp->mcase = gtk_check_button_new_with_label(_("Match case"));
    gtk_check_button_مجموعه_فعال(GTK_CHECK_BUTTON(frdp->mcase),
	    (gboolean)mcase);
    gtk_box_append(GTK_BOX(hbox), frdp->mcase);

    // Direction radio buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    tmp = gtk_label_new(_("Direction:"));
    gtk_box_append(GTK_BOX(hbox), tmp);

    frdp->به_بالا = gtk_check_button_new_with_label(_("Up"));
    gtk_box_append(GTK_BOX(hbox), frdp->به_بالا);

    frdp->پایین = gtk_check_button_new_with_label(_("Down"));
    gtk_check_button_مجموعه_گروه(GTK_CHECK_BUTTON(frdp->پایین),
	    GTK_CHECK_BUTTON(frdp->به_بالا));
    gtk_check_button_مجموعه_فعال(GTK_CHECK_BUTTON(frdp->پایین), TRUE);
    gtk_box_append(GTK_BOX(hbox), frdp->پایین);

    // Action buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_مجموعه_halign(hbox, GTK_ALIGN_انتها);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    btn = gtk_button_new_with_label(_("Find Next"));
    gtk_widget_مجموعه_sensitive(btn, sensitive);
    frdp->پیدا_کن = btn;

    کلید_controller = gtk_event_controller_کلید_new();
    g_signal_connect(کلید_controller, "key-pressed",
	    G_CALLBACK(پیدا_کن_کلید_pressed_cb), frdp);
    gtk_widget_جمع_کن_controller(GTK_WIDGET(frdp->dialog), کلید_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(مدخل_تغییریافته_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_پنجره_مجموعه_default_widget(GTK_پنجره(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(پیدا_کن_replace_cb),
	    GINT_مقصد_اشاره_گر(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    إذا (do_replace)
    {
	btn = gtk_button_new_with_label(_("Replace"));
	gtk_widget_مجموعه_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(پیدا_کن_replace_cb),
		GINT_مقصد_اشاره_گر(FRD_REPLACE));
	frdp->replace = btn;
	gtk_box_append(GTK_BOX(hbox), btn);

	btn = gtk_button_new_with_label(_("Replace All"));
	gtk_widget_مجموعه_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(پیدا_کن_replace_cb),
		GINT_مقصد_اشاره_گر(FRD_REPLACEALL));
	frdp->all = btn;
	gtk_box_append(GTK_BOX(hbox), btn);
    }

    btn = gtk_button_new_with_label(_("Close"));
    g_signal_connect_swapped(btn, "clicked",
	    G_CALLBACK(gtk_پنجره_destroy), frdp->dialog);
    gtk_box_append(GTK_BOX(hbox), btn);

    gtk_پنجره_present(GTK_پنجره(frdp->dialog));
    gtk_widget_grab_focus(frdp->what);
    إذا (do_replace && مدخل_متن != عدم && مدخل_متن[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(مدخل_متن);
}

    فراغ
gui_mch_پیدا_کن_dialog(exarg_T *eap)
{
    إذا (gui.درون_use)
	پیدا_کن_replace_dialog_create(eap->arg, FALSE);
}

    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
    إذا (gui.درون_use)
	پیدا_کن_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    فراغ
ex_helpfind(exarg_T *eap استفادهنشده)
{
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

/*
 * ============================================================
 * Native print dialog
 * ============================================================
 */

#اگر_تعریف_شده USE_GTK4_PRINT_DIALOG
ساكن GtkPrintDialog *print_dialog = عدم;
ساكن GtkPrintSettings *print_settings = عدم;
ساكن Gtk_صفحه_راهاندازی *صفحه_راهاندازی = عدم;
ساكن Gtk_Print_راهاندازی *print_راهاندازی = عدم;
ساكن GFile *print_file;

    ساكن فراغ
print_dialog_cb(GtkPrintDialog *dialog, GAsync_نتیجه *نتیجه, gboolean *انجامشده)
{
    GError *error = عدم;
    print_راهاندازی = gtk_print_dialog_راهاندازی_پایان(dialog, نتیجه, &error);

    إذا (print_راهاندازی == عدم)
    {
	semsg(_(e_print_error_str), error->پیام_2);
	g_error_free(error);
    }
    *انجامشده = TRUE;
}

/*
 * Return file to write to on success (allocated string), or NULL on failure
 * (e.g. user cancelled)
 */
    char_u *
gui_gtk4_print_dialog(
	prt_settings_T	*psettings,
	char_u		*jobname,
	مزدوج		*صفحه_عرض,
	مزدوج		*صفحه_ارتفاع)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    انجامشده = FALSE;
    char_u	    *موقت;

    gtk_print_dialog_مجموعه_title(dialog, (ثابت حرف *)jobname);
    gtk_print_dialog_مجموعه_modal(dialog, TRUE);

    print_dialog = عدم;
    print_راهاندازی = عدم;

    إذا (print_settings == عدم)
    {
	option_جدول_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	إذا (opt->present)
	{
	    إذا (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_مجموعه_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    وإلا إذا (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_مجموعه_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    وإلا
		gtk_print_settings_مجموعه_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	إذا (opt->present)
	{
	    إذا (*opt->string == 'y')
		gtk_print_settings_مجموعه_orientation(print_settings,
			GTK_صفحه_ORIENTATION_PORTRAIT);
	    وإلا
		gtk_print_settings_مجموعه_orientation(print_settings,
			GTK_صفحه_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	إذا (opt->present)
	{
	    gtk_print_settings_مجموعه_paper_اندازه(print_settings,
		    gtk_paper_اندازه_new((ثابت حرف *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	إذا (opt->present)
	{
	    إذا (*opt->string == 'y')
		gtk_print_settings_مجموعه_collate(print_settings, TRUE);
	    وإلا
		gtk_print_settings_مجموعه_collate(print_settings, FALSE);
	}
    }

    إذا (صفحه_راهاندازی == عدم)
	صفحه_راهاندازی = gtk_صفحه_راهاندازی_new();

    gtk_print_dialog_مجموعه_print_settings(dialog, print_settings);
    gtk_print_dialog_مجموعه_صفحه_راهاندازی(dialog, صفحه_راهاندازی);

    gtk_print_dialog_راهاندازی(dialog, GTK_پنجره(gui.mainwin), عدم,
	    (GAsyncReadyCallback)print_dialog_cb, &انجامشده);

    طالما (!انجامشده)
	g_main_context_iteration(عدم, TRUE);

    إذا (print_راهاندازی == عدم)
    {
	g_object_unref(dialog);
	g_clear_اشاره_گر(&print_راهاندازی, gtk_print_راهاندازی_unref);

	psettings->user_abort_پیام = FALSE;
	أرجع عدم;
    }

    g_clear_object(&print_settings);
    g_clear_object(&صفحه_راهاندازی);
    print_settings = g_object_ref(
	    gtk_print_راهاندازی_get_print_settings(print_راهاندازی));
    صفحه_راهاندازی = g_object_ref(gtk_print_راهاندازی_get_صفحه_راهاندازی(print_راهاندازی));

    {
	GtkPrintDuplex duplex = gtk_print_settings_get_duplex(print_settings);
	psettings->duplex = duplex != GTK_PRINT_DUPLEX_SIMPLEX;
    }

    psettings->has_color = gtk_print_settings_get_use_color(print_settings);

    *صفحه_عرض = gtk_صفحه_راهاندازی_get_paper_عرض(صفحه_راهاندازی,
	    GTK_UNIT_POINTS);
    *صفحه_ارتفاع = gtk_صفحه_راهاندازی_get_paper_ارتفاع(صفحه_راهاندازی,
	    GTK_UNIT_POINTS);

    موقت  = vim_tempname('p', FALSE);
    إذا (موقت == عدم)
    {
	emsg(_(e_cant_پیدا_کن_موقت_file_for_writing));
	g_clear_object(&dialog);
	g_clear_اشاره_گر(&print_راهاندازی, gtk_print_راهاندازی_unref);

	psettings->user_abort_پیام = FALSE;
	أرجع عدم;
    }
    print_file = g_file_new_for_مسیر((حرف *)موقت);

    print_dialog = dialog;

    أرجع موقت;
}

    ساكن فراغ
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsync_نتیجه	*نتیجه,
	GFile		*file)
{
    حرف *مسیر = g_file_get_مسیر(file);

    (فراغ)gtk_print_dialog_print_file_پایان(dialog, نتیجه, عدم);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(مسیر);
    g_free(مسیر);
}


    فراغ
gui_gtk4_print_پایان(فراغ)
{
    إذا (print_dialog != عدم)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_پنجره(gui.mainwin),
		print_راهاندازی, print_file, عدم,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = عدم;
	g_clear_اشاره_گر(&print_راهاندازی, gtk_print_راهاندازی_unref);
    }
}
#پایان_اگر // USE_GTK4_PRINT_DIALOG

#اگر_تعریف_شده FEAT_IMAGE_GUI

تعريفنوع بنية
{
    GdkTexture *texture;
} image_gdk_T;

تعريفنوع بنية
{
    Gsk_Render_گره   **nodes; // Note that elements may be NULL
    صحيح		    n_nodes;
} image_placement_gdk_T;

    صحيح
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    Gdk_حافظه_Format fmt;
    size_t	    stride;
    GBytes	    *بایتها;
    size_t	    اندازه;
    صحيح		    w, h;

    إذا (ctx == عدم)
	أرجع FAIL;

    إذا (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_حافظه_R8_G8_B8_A8;
    وإلا
	fmt = GDK_حافظه_R8_G8_B8;

    image_get_dimensions(img, &w, &h);

    اندازه = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    بایتها = g_بایتها_new(img->داده, اندازه);
    ctx->texture = gdk_حافظه_texture_new(w, h, fmt, بایتها, stride);
    g_بایتها_unref(بایتها);

    img->backend_داده = ctx;
    أرجع OK;
}

    فراغ
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_داده;

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
	gsk_render_گره_unref(ctx->nodes[i]);
    }
    g_clear_اشاره_گر(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    صحيح
image_placement_gui_init(image_placement_T *قراربده)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    إذا (ctx == عدم)
	أرجع FAIL;

    قراربده->backend_داده = ctx;
    أرجع OK;
}

    فراغ
image_placement_gui_uninit(image_placement_T *قراربده)
{
    image_placement_gdk_T *ctx = قراربده->backend_داده;

    clear_nodes(ctx);
    vim_free(ctx);
}

    فراغ
image_placement_gui_draw(image_placement_T *قراربده)
{
    image_T		    *img = قراربده->img;
    image_placement_gdk_T   *ctx = قراربده->backend_داده;
    image_gdk_T		    *ictx = img->backend_داده;
    pixman_box32_t	    *rects;
    صحيح			    n_rects;
    صحيح			    iw, ih;
    Gsk_Render_گره	    *texture_گره;

    rects = pixman_region32_rectangles(&قراربده->visible, &n_rects);
    إذا (rects == عدم)
	أرجع;

    clear_nodes(ctx);
    إذا (n_rects == 0)
	أرجع;

    image_get_dimensions(img, &iw, &ih);
    texture_گره = gsk_texture_گره_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(قراربده->ستون) - FILL_X(قراربده->crop_box.x1),
		FILL_Y(قراربده->ردیف) - FILL_Y(قراربده->crop_box.y1)
		+ FILL_Y(قراربده->ردیف_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    إذا (texture_گره == عدم)
	// Not sure if this can happen...
	أرجع;

    ctx->nodes = g_malloc_n(n_rects, حجم(Gsk_Render_گره *));
    ctx->n_nodes = n_rects;

    لكل (صحيح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحيح		ردیف, ستون;
	صحيح		posx, posy;
	صحيح		x, y, w, h;
	Gsk_Render_گره	*گره;

	image_placement_subrect(قراربده, rect, &ردیف, &ستون, &x, &y, &w, &h, false);
	posx = FILL_X(ستون);
	posy = FILL_Y(ردیف);

	گره = gsk_clip_گره_new(texture_گره,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	إذا (گره != عدم)
	    vim_draw_area_جمع_کن_external(VIM_DRAW_AREA(gui.drawarea), گره);

	ctx->nodes[i] = گره;
    }
    gsk_render_گره_unref(texture_گره);
}

    فراغ
image_placement_gui_clear(image_placement_T *قراربده)
{
    image_placement_gdk_T   *ctx = قراربده->backend_داده;

    clear_nodes(ctx);
}

#پایان_اگر

#پایان_اگر // FEAT_GUI_GTK
