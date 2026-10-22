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

#include "vim.h"

#ifdef FEAT_GUI_GTK

#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include "gui_gtk4_f.h"
#include "gui_gtk4_cb.h"
#include "gui_gtk4_da.h"
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
#define NoValue		0x0000
#define WidthValue	0x0004
#define HeightValue	0x0008

    static puur_naa_nka
vim_parse_geometry(const var_na *str, unsigned puur_naa_nka *width, unsigned puur_naa_nka *height)
{
    puur_naa_nka mask = NoValue;
    var_na *end;
    long val;

    ᰀᰧ (str == NULL || *str == NUL)
	ᰀᰧᰌ mask;

    // Parse width
    ᰀᰧ (*str != '+' && *str != '-')
    {
	val = strtol(str, &end, 10);
	ᰀᰧ (end != str)
	{
	    *width = (unsigned puur_naa_nka)val;
	    mask |= WidthValue;
	    str = end;
	}
    }

    // Parse 'x' or 'X' separator and height
    ᰀᰧ (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &end, 10);
	ᰀᰧ (end != str)
	{
	    *height = (unsigned puur_naa_nka)val;
	    mask |= HeightValue;
	    str = end;
	}
    }

    ᰀᰧᰌ mask;
}

#ᰀᰧ defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
static puur_naa_nka last_shape = 0;
#endif

#define DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
static ga_nanaa {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
static puur_naa_nka gtk4_main_loop_level = 0;
static puur_naa_nka gtk4_main_loop_quit = FALSE;

#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

typedef gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
static sa_nracanaa special_key
{
    guint key_sym;
    char_u code0;
    char_u code1;
}
const special_keys[] =
{
    {GDK_KEY_Up,	'k', 'u'},
    {GDK_KEY_Down,	'k', 'd'},
    {GDK_KEY_Left,	'k', 'l'},
    {GDK_KEY_Right,	'k', 'r'},
    {GDK_KEY_F1,	'k', '1'},
    {GDK_KEY_F2,	'k', '2'},
    {GDK_KEY_F3,	'k', '3'},
    {GDK_KEY_F4,	'k', '4'},
    {GDK_KEY_F5,	'k', '5'},
    {GDK_KEY_F6,	'k', '6'},
    {GDK_KEY_F7,	'k', '7'},
    {GDK_KEY_F8,	'k', '8'},
    {GDK_KEY_F9,	'k', '9'},
    {GDK_KEY_F10,	'k', ';'},
    {GDK_KEY_F11,	'F', '1'},
    {GDK_KEY_F12,	'F', '2'},
    {GDK_KEY_Help,	'%', '1'},
    {GDK_KEY_Undo,	'&', '8'},
    {GDK_KEY_BackSpace,	'k', 'b'},
    {GDK_KEY_Insert,	'k', 'I'},
    {GDK_KEY_Delete,	'k', 'D'},
    {GDK_KEY_Home,	'k', 'h'},
    {GDK_KEY_End,	'@', '7'},
    {GDK_KEY_Prior,	'k', 'P'},
    {GDK_KEY_Next,	'k', 'N'},
    {GDK_KEY_Print,	'%', '9'},
    {GDK_KEY_KP_Left,	'k', 'l'},
    {GDK_KEY_KP_Right,	'k', 'r'},
    {GDK_KEY_KP_Up,	'k', 'u'},
    {GDK_KEY_KP_Down,	'k', 'd'},
    {GDK_KEY_KP_Insert,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_KEY_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_KEY_KP_Home,	'K', '1'},
    {GDK_KEY_KP_End,	'K', '4'},
    {GDK_KEY_KP_Prior,	'K', '3'},
    {GDK_KEY_KP_Next,	'K', '5'},
    {GDK_KEY_KP_Add,	'K', '6'},
    {GDK_KEY_KP_Subtract, 'K', '7'},
    {GDK_KEY_KP_Divide,	'K', '8'},
    {GDK_KEY_KP_Multiply, 'K', '9'},
    {GDK_KEY_KP_Enter,	'K', 'A'},
    {GDK_KEY_KP_Decimal,	'K', 'B'},
    {GDK_KEY_KP_0,	'K', 'C'},
    {GDK_KEY_KP_1,	'K', 'D'},
    {GDK_KEY_KP_2,	'K', 'E'},
    {GDK_KEY_KP_3,	'K', 'F'},
    {GDK_KEY_KP_4,	'K', 'G'},
    {GDK_KEY_KP_5,	'K', 'H'},
    {GDK_KEY_KP_6,	'K', 'I'},
    {GDK_KEY_KP_7,	'K', 'J'},
    {GDK_KEY_KP_8,	'K', 'K'},
    {GDK_KEY_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    static puur_naa_nka
keyval_to_string(unsigned puur_naa_nka keyval, char_u *string)
{
    puur_naa_nka		len;
    guint32	uc;

    uc = gdk_keyval_to_unicode(keyval);
    ᰀᰧ (uc != 0)
    {
	len = utf_char2bytes((puur_naa_nka)uc, string);
    }
    ᰀᰧᰀ
    {
	len = 1;
	switch (keyval)
	{
	    pak_sa GDK_KEY_Tab: pak_sa GDK_KEY_KP_Tab: pak_sa GDK_KEY_ISO_Left_Tab:
		string[0] = TAB;
		bha_nga;
	    pak_sa GDK_KEY_Linefeed:
		string[0] = NL;
		bha_nga;
	    pak_sa GDK_KEY_Return: pak_sa GDK_KEY_ISO_Enter: pak_sa GDK_KEY_3270_Enter:
		string[0] = CAR;
		bha_nga;
	    pak_sa GDK_KEY_Escape:
		string[0] = ESC;
		bha_nga;
	    default:
		len = 0;
		bha_nga;
	}
    }
    string[len] = NUL;
    ᰀᰧᰌ len;
}

    static puur_naa_nka
modifiers_gdk2vim(guint state)
{
    puur_naa_nka modifiers = 0;

    ᰀᰧ (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    ᰀᰧ (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    ᰀᰧ (state & GDK_ALT_MASK)
	modifiers |= MOD_MASK_ALT;
    ᰀᰧ (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    ᰀᰧ (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;

    ᰀᰧᰌ modifiers;
}

static GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
static gboolean key_press_event(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
static void key_release_event(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
static void button_press_event(GtkGestureClick *gesture, puur_naa_nka n_press, dviguu_na x, dviguu_na y, gpointer data);
static void button_release_event(GtkGestureClick *gesture, puur_naa_nka n_press, dviguu_na x, dviguu_na y, gpointer data);
static void motion_notify_event(GtkEventControllerMotion *controller, dviguu_na x, dviguu_na y, gpointer data);
static void enter_notify_event(GtkEventControllerMotion *controller, dviguu_na x, dviguu_na y, gpointer data);
static gboolean scroll_event(GtkEventControllerScroll *controller, dviguu_na dx, dviguu_na dy, gpointer data);
static void focus_in_event(GtkEventControllerFocus *controller, gpointer data);
static void focus_out_event(GtkEventControllerFocus *controller, gpointer data);
#ifdef FEAT_DND
static gboolean drop_cb(GtkDropTargetAsync *target, GdkDrop *drop, dviguu_na x, dviguu_na y, void *data);
#endif
#ifdef FEAT_GUI_TABLINE
static void tabline_enter_cb(GtkEventController *controller, dviguu_na x, dviguu_na y, void *udata);
static void on_select_tab(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
static void on_tab_reordered(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
static VimMenu *create_tabline_popup_menu(void);
static void tabline_add_css(void);
static void tabline_menu_press_event(GtkGestureClick *gesture, puur_naa_nka n_press, dviguu_na x, dviguu_na y, GtkWidget *popover);
#endif
static void mainwin_destroy_cb(GObject *object, gpointer data);
static gboolean delete_event_cb(GtkWindow *window, gpointer data);
static puur_naa_nka query_pointer_pos(puur_naa_nka *x, puur_naa_nka *y, GdkModifierType *state);
static void mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_data);
static void set_form_size(puur_naa_nka width, puur_naa_nka height);
static void drawarea_realize_cb(GtkWidget *widget, gpointer data);
static void drawarea_unrealize_cb(GtkWidget *widget, gpointer data);
static void clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data);
#ifdef FEAT_MENU
static void show_menubar_popover(void);
#endif

static const var_na *prgname = NULL;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    static gboolean
arg_match(const var_na *s, const var_na *name, var_na **value)
{
    size_t  len = strlen(name);

    ᰀᰧ (strncmp(s, name, len) != 0)
	ᰀᰧᰌ FALSE;
    ᰀᰧ (s[len] == '=')
	*value = (var_na *)s + len + 1;
    ᰀᰧᰀ ᰀᰧ (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	ᰀᰧᰌ FALSE;
    ᰀᰧᰌ TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    void
gui_mch_prepare(puur_naa_nka *argc, var_na **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    puur_naa_nka	i = 0;

    ᰀᰬ (i < *argc)
    {
	var_na	*s = argv[i];
	var_na	*value = NULL;
	puur_naa_nka	has_inline_value = FALSE;
	puur_naa_nka	n_strip;

	ᰀᰧ (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    continue;
	}

	ᰀᰧ (strchr(s, '=') != NULL)
	    has_inline_value = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	ᰀᰧ (!has_inline_value)
	{
	    ᰀᰧ (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		value = argv[i + 1];
	    ᰀᰧᰀ
		value = NULL;
	}

	ᰀᰧ (arg_match(s, "-fn", &value) || arg_match(s, "-font", &value))
	    font_argument = value;
	ᰀᰧᰀ ᰀᰧ (arg_match(s, "-geom", &value)
		|| arg_match(s, "-geometry", &value))
	{
	    ᰀᰧ (value != NULL)
		gui.geom = vim_strsave((char_u *)value);
	}
	ᰀᰧᰀ ᰀᰧ (arg_match(s, "-bg", &value)
		|| arg_match(s, "-background", &value))
	    background_argument = value;
	ᰀᰧᰀ ᰀᰧ (arg_match(s, "-fg", &value)
		|| arg_match(s, "-foreground", &value))
	    foreground_argument = value;
#ifdef FEAT_NETBEANS_INTG
	ᰀᰧᰀ ᰀᰧ (strncmp(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_value = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#endif
	ᰀᰧᰀ ᰀᰧ (arg_match(s, "--prg-name", &value))
	    // GTK4 specific
	    prgname = value;
	ᰀᰧᰀ
	{
	    i++;
	    continue;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	ᰀᰧ (value != NULL && !has_inline_value)
	    n_strip = 2;

	ᰀᰧ (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    ᰀᰧ (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * parimaa_na(var_na *));
	    argv[*argc] = NULL;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    void
gui_mch_free_all(void)
{
}

    static guint
timeout_add(puur_naa_nka time, timeout_cb_type (*callback)(gpointer), puur_naa_nka *flagp)
{
    ᰀᰧᰌ g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    static void
timeout_remove(guint timer)
{
    g_source_remove(timer);
}

static long_u blink_waittime = 700;
static long_u blink_ontime = 400;
static long_u blink_offtime = 250;
static guint blink_timer = 0;

    static timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    ᰀᰧ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, NULL);
    }
    ᰀᰧᰀ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, NULL);
    }
    ᰀᰧᰌ FALSE;
}

    puur_naa_nka
gui_mch_is_blinking(void)
{
    ᰀᰧᰌ blink_state != BLINK_NONE;
}

    puur_naa_nka
gui_mch_is_blink_off(void)
{
    ᰀᰧᰌ blink_state == BLINK_OFF;
}

    void
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    void
gui_mch_stop_blink(puur_naa_nka may_call_gui_update_cursor)
{
    ᰀᰧ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ᰀᰧ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    void
gui_mch_start_blink(void)
{
    ᰀᰧ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ᰀᰧ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, NULL);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    puur_naa_nka
gui_mch_early_init_check(puur_naa_nka give_message UNUSED)
{
    ᰀᰧᰌ OK;
}

    puur_naa_nka
gui_mch_init_check(void)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_set_prgname(prgname == NULL ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    ᰀᰧ (g_getenv("EGL_LOG_LEVEL") == NULL)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    ᰀᰧᰌ OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    puur_naa_nka
gui_mch_init(void)
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
		     G_CALLBACK(delete_event_cb), NULL);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), NULL);

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
		     G_CALLBACK(on_select_tab), NULL);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), NULL);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), NULL);

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
		     G_CALLBACK(drawarea_realize_cb), NULL);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), NULL);

    // Set up event controllers.
    {
	GtkEventController *key_ctrl = gtk_event_controller_key_new();
	g_signal_connect(key_ctrl, "key-pressed",
			 G_CALLBACK(key_press_event), NULL);
	g_signal_connect(key_ctrl, "key-released",
			 G_CALLBACK(key_release_event), NULL);
	gtk_widget_add_controller(gui.drawarea, key_ctrl);
#ifdef FEAT_XIM
	xim_init();
#endif
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), NULL);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), NULL);
	gtk_widget_add_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), NULL);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), NULL);
	gtk_widget_add_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), NULL);
	gtk_widget_add_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_in_event), NULL);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_out_event), NULL);
	gtk_widget_add_controller(gui.drawarea, focus);
    }

#ifdef FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_target = gtk_drop_target_async_new(NULL,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_set_dnd_targets();
	g_signal_connect(gui.drop_target, "drop",
			 G_CALLBACK(drop_cb), NULL);
	gtk_widget_add_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_target));
    }
#endif

    gui.border_offset = gui.border_width;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_pointer = gdk_cursor_new_from_name("none", NULL);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	ᰀᰧ (primary != NULL)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_star);
	ᰀᰧ (regular != NULL)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_star);

    ᰀᰧᰌ OK;
}

    void
gui_mch_new_colors(void)
{
    ᰀᰧ (gui.drawarea != NULL && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    puur_naa_nka
gui_mch_open(void)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_width;
    guint pixel_height;

    ᰀᰧ (gui.geom != NULL)
    {
	puur_naa_nka		mask;
	unsigned puur_naa_nka	w, h;

	mask = vim_parse_geometry((var_na *)gui.geom, &w, &h);

	ᰀᰧ (mask & WidthValue)
	    Columns = w;
	ᰀᰧ (mask & HeightValue)
	{
	    ᰀᰧ (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	VIM_CLEAR(gui.geom);
    }
    ᰀᰧᰀ
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	ᰀᰧ (Columns > 80)
	    Columns = 80;
	ᰀᰧ (Rows > 24)
	    Rows = 24;
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    pixel_width  += get_menu_tool_width();
    pixel_height += get_menu_tool_height();

    ᰀᰧ (foreground_argument != NULL)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    ᰀᰧ (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    ᰀᰧ (background_argument != NULL)
	bg_pixel = gui_get_color((char_u *)background_argument);
    ᰀᰧ (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    ᰀᰧ (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    ᰀᰧᰀ
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    set_normal_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), NULL);
    // Resize is handled by GtkForm's size_allocate callback.

    set_form_size((puur_naa_nka)pixel_width - get_menu_tool_width(),
	    (puur_naa_nka)pixel_height - get_menu_tool_height());

    gtk_window_present(GTK_WINDOW(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    ᰀᰧᰌ OK;
}

    void
gui_mch_exit(puur_naa_nka rc UNUSED)
{
    ᰀᰧ (gui.mainwin != NULL)
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
		ᰀᰧ ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != NULL)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#endif
	gtk_window_destroy(GTK_WINDOW(gui.mainwin));
    }
}

    puur_naa_nka
gui_mch_get_winpos(puur_naa_nka *x, puur_naa_nka *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    ᰀᰧᰌ FAIL;
}

    void
gui_mch_set_winpos(puur_naa_nka x UNUSED, puur_naa_nka y UNUSED)
{
    // GTK4/Wayland: window positioning not available
}

    puur_naa_nka
gui_mch_maximized(void)
{
    ᰀᰧᰌ gtk_window_is_maximized(GTK_WINDOW(gui.mainwin));
}

    void
gui_mch_unmaximize(void)
{
    ᰀᰧ (gui.mainwin != NULL)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

    void
gui_mch_set_fullscreen(puur_naa_nka flag)
{
    ᰀᰧ (gui.mainwin == NULL)
	ᰀᰧᰌ;
    ᰀᰧ (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    ᰀᰧᰀ
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}

    static void
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec UNUSED, gpointer user_data UNUSED)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    ᰀᰧ (gtk_window_is_fullscreen(GTK_WINDOW(obj)))
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
    void
gui_mch_newfont(void)
{
    puur_naa_nka w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_width(gui.formwin);
    h = gtk_widget_get_height(gui.formwin);
    gui_resize_shell(w, h);
}

    void
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    ᰀᰧ (title != NULL && gui.mainwin != NULL)
	gtk_window_set_title(GTK_WINDOW(gui.mainwin), (const var_na *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    void
gui_gtk_init_decor_height(void)
{
    puur_naa_nka h = gtk_widget_get_height(gui.mainwin);

    ᰀᰧ (h == 0)
	ᰀᰧᰌ;

    h -= get_menu_tool_height();
    h -= gtk_widget_get_height(gui.formwin);

    gui.decor_height = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    static void
set_form_size(puur_naa_nka width, puur_naa_nka height)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    ᰀᰧ (gtk_widget_get_width(gui.formwin) == width
	    && gtk_widget_get_height(gui.formwin) == height)
	ᰀᰧᰌ;

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

    void
gui_mch_set_shellsize(puur_naa_nka width, puur_naa_nka height,
	puur_naa_nka min_width UNUSED, puur_naa_nka min_height UNUSED,
	puur_naa_nka base_width UNUSED, puur_naa_nka base_height UNUSED,
	puur_naa_nka direction UNUSED)
{
    set_form_size(width, height);

    gui_mch_update();
}

    void
gui_mch_get_screen_dimensions(puur_naa_nka *screen_w, puur_naa_nka *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    ᰀᰧ (surface != NULL)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	ᰀᰧ (monitor != NULL)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.width;
	    *screen_h = geom.height;
	    ᰀᰧᰌ;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ifdef FEAT_MENU
    void
gui_mch_enable_menu(puur_naa_nka showit)
{
    ᰀᰧ (gui.menubar != NULL)
    {
	gtk_widget_set_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	ᰀᰧ (gui.drawarea != NULL)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

#ifdef FEAT_TOOLBAR
    void
gui_mch_show_toolbar(puur_naa_nka showit)
{
    ᰀᰧ (gui.toolbar != NULL)
    {
	gtk_widget_set_visible(gui.toolbar, showit);
	ᰀᰧ (showit)
	    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	ᰀᰧ (gui.drawarea != NULL)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

    void
gui_mch_set_dark_theme(puur_naa_nka dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    ᰀᰧ (settings != NULL)
	g_object_set(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, NULL);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    puur_naa_nka
gui_mch_adjust_charheight(void)
{
    PangoFontMetrics *metrics;
    puur_naa_nka ascent;
    puur_naa_nka descent;

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

    ᰀᰧᰌ OK;
}

typedef sa_nracanaa {
    PangoFontDescription    *result;
    gboolean		    done;
} FontDialogData;

    static void
font_dialog_finish_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FontDialogData *fdd = (FontDialogData *)data;
    fdd->result = gtk_font_dialog_choose_font_finish(
		    GTK_FONT_DIALOG(source), res, NULL);
    fdd->done = TRUE;
}

    static gboolean
font_filter(gpointer item, gpointer data UNUSED)
{
    ᰀᰧ (PANGO_IS_FONT_FAMILY(item))
	ᰀᰧᰌ pango_font_family_is_monospace(PANGO_FONT_FAMILY(item));
    ᰀᰧ (PANGO_IS_FONT_FACE(item))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(item));
	ᰀᰧ (family != NULL)
	    ᰀᰧᰌ pango_font_family_is_monospace(family);
    }
    ᰀᰧᰌ TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = NULL;
    char_u		*fontname = NULL;
    FontDialogData	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_set_modal(dlg, TRUE);
    gtk_font_dialog_set_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, NULL, NULL)));

    ᰀᰧ (oldval != NULL && oldval[0] != NUL)
    {
	char_u *oldname;

	ᰀᰧ (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, NULL);
	ᰀᰧᰀ
	    oldname = oldval;

	ᰀᰧ (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    ᰀᰧ (p != NULL)
	    {
		STRCPY(p + STRLEN(p), " 10");
		ᰀᰧ (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((const var_na *)oldname);
	ᰀᰧ (oldname != oldval)
	    vim_free(oldname);
    }
    ᰀᰧᰀ
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.result = NULL;
    fdd.done = FALSE;

    gtk_font_dialog_choose_font(dlg, GTK_WINDOW(gui.mainwin),
	    initial, NULL, font_dialog_finish_cb, &fdd);

    ᰀᰬ (!fdd.done)
	g_main_context_iteration(NULL, TRUE);

    ᰀᰧ (fdd.result != NULL)
    {
	var_na *name = pango_font_description_to_string(fdd.result);
	ᰀᰧ (name != NULL)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    ᰀᰧ (p != NULL && input_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&input_conv, p, NULL);
		vim_free(p);
	    }
	    ᰀᰧᰀ
		fontname = p;
	}
	pango_font_description_free(fdd.result);
    }

    ᰀᰧ (initial != NULL)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    ᰀᰧᰌ fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    static void
ascii_glyph_table_init(void)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    puur_naa_nka		    i;

    ᰀᰧ (gui.ascii_glyphs != NULL)
	pango_glyph_string_free(gui.ascii_glyphs);
    ᰀᰧ (gui.ascii_font != NULL)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = NULL;
    gui.ascii_font   = NULL;

    for (i = 0; i < 128; ++i)
    {
	ᰀᰧ (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	ᰀᰧᰀ
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (const var_na *)ascii_chars,
			      0, parimaa_na(ascii_chars), attr_list, NULL);

    ᰀᰧ (item_list != NULL && item_list->next == NULL)
    {
	PangoItem   *item;
	puur_naa_nka	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((const var_na *)ascii_chars, parimaa_na(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	ᰀᰧ (gui.ascii_glyphs->num_glyphs == (puur_naa_nka)parimaa_na(ascii_chars))
	{
	    for (i = 0; i < gui.ascii_glyphs->num_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_offset += MAX(0, width - geom->width) / 2;
		geom->width = width;
	    }
	}
	ᰀᰧᰀ
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = NULL;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = NULL;
	}
    }

    g_list_foreach(item_list, (GFunc)(void *)&pango_item_free, NULL);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    static void
get_styled_font_variants(void)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);
    ᰀᰧ (plain_font == NULL)
	ᰀᰧᰌ;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    ᰀᰧ (bold_font != NULL)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    puur_naa_nka
gui_mch_init_font(char_u *font_name, puur_naa_nka fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    puur_naa_nka			    width;

    ᰀᰧ (font_name == NULL)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    ᰀᰧ (font_desc == NULL)
	ᰀᰧᰌ FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, NULL);
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    ᰀᰧ (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	puur_naa_nka im_row = gui.row < screen_Rows ? gui.row : screen_Rows - 1;
	puur_naa_nka im_col = gui.col < screen_Columns ? gui.col : screen_Columns - 1;
	im_set_position(im_row, im_col);
    }
    ᰀᰧᰌ OK;
}

    GuiFont
gui_mch_get_font(char_u *name, puur_naa_nka report_error)
{
    PangoFontDescription *font;

    ᰀᰧ (name == NULL)
	ᰀᰧᰌ NULL;

    font = pango_font_description_from_string((const var_na *)name);
    ᰀᰧ (font == NULL)
    {
	ᰀᰧ (report_error)
	    semsg(_(e_unknown_font_str), name);
	ᰀᰧᰌ NULL;
    }

    // Ensure a size is set
    ᰀᰧ (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    ᰀᰧᰌ font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    ᰀᰧ (font != NOFONT)
    {
	var_na *desc = pango_font_description_to_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	ᰀᰧᰌ ret;
    }
    ᰀᰧᰌ NULL;
}

    void
gui_mch_free_font(GuiFont font)
{
    ᰀᰧ (font != NOFONT)
	pango_font_description_free(font);
}

    void
gui_mch_expand_font(
    optexpand_T	*args,
    void	*param,
    puur_naa_nka		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = NULL;
    puur_naa_nka			n_families = 0;
    puur_naa_nka			wide = *(puur_naa_nka *)param;

    ᰀᰧ (args->oe_include_orig_val && *args->oe_opt_value == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	ᰀᰧ (add_match((char_u *)DEFAULT_FONT) != OK)
	    ᰀᰧᰌ;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    for (puur_naa_nka i = 0; i < n_families; i++)
    {
	ᰀᰧ (!wide && !pango_font_family_is_monospace(font_families[i]))
	    continue;

	const var_na *fam_name = pango_font_family_get_name(font_families[i]);
	ᰀᰧ (input_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&input_conv,
		    (char_u *)fam_name, NULL);
	    ᰀᰧ (buf != NULL)
	    {
		ᰀᰧ (add_match(buf) != OK)
		{
		    vim_free(buf);
		    bha_nga;
		}
		vim_free(buf);
	    }
	    ᰀᰧᰀ
		bha_nga;
	}
	ᰀᰧᰀ
	{
	    ᰀᰧ (add_match((char_u *)fam_name) != OK)
		bha_nga;
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
    ᰀᰧ (!gui.in_use)
	ᰀᰧᰌ INVALCOLOR;

    ᰀᰧ (name != NULL)
	ᰀᰧᰌ gui_get_color_cmn(name);

    ᰀᰧᰌ INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(puur_naa_nka r, puur_naa_nka g, puur_naa_nka b)
{
    ᰀᰧᰌ gui_get_rgb_color_cmn(r, g, b);
}

    static GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    ᰀᰧᰌ rgba;
}

    void
gui_mch_set_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_to_rgba(color);
}

    void
gui_mch_set_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_to_rgba(color);
}

    void
gui_mch_set_sp_color(guicolor_T color)
{
    *gui.spcolor = color_to_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ᰀᰧᰌ pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    void
gui_gtk4_update_size(void)
{
    vim_draw_area_set_size(VIM_DRAW_AREA(gui.drawarea),
	    gui.num_rows, gui.num_cols);
}

#ifdef FEAT_NETBEANS_INTG
    void
gui_gtk4_add_multisign(
	cairo_surface_t *surf,
	puur_naa_nka		row,
	puur_naa_nka		col,
	puur_naa_nka		width,
	puur_naa_nka		height)
{
    vim_draw_area_add_multisign(VIM_DRAW_AREA(gui.drawarea), surf, row, col,
	    width, height);
}
#endif

    void
gui_mch_clear_block(puur_naa_nka row1, puur_naa_nka col1, puur_naa_nka row2, puur_naa_nka col2)
{
    vim_draw_area_clear_block(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    ᰀᰧ (gui.drawarea != NULL)
	gtk_widget_queue_draw(gui.drawarea);
}

    void
gui_mch_clear_all(void)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    ᰀᰧ (gui.drawarea != NULL)
	gtk_widget_queue_draw(gui.drawarea);
}

    void
gui_mch_delete_lines(puur_naa_nka row, puur_naa_nka num_lines)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row, row + num_lines, gui.scroll_region_bot,
	    gui.scroll_region_left, gui.scroll_region_right);
    gui_clear_block(
	    gui.scroll_region_bot - num_lines + 1, gui.scroll_region_left,
	    gui.scroll_region_bot, gui.scroll_region_right);

    gtk_widget_queue_draw(gui.drawarea);
}

    void
gui_mch_insert_lines(puur_naa_nka row, puur_naa_nka num_lines)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row + num_lines, row, gui.scroll_region_bot - num_lines,
	    gui.scroll_region_left, gui.scroll_region_right);
    gui_clear_block(
	    row, gui.scroll_region_left,
	    row + num_lines - 1, gui.scroll_region_right);

    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    void
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, puur_naa_nka w, puur_naa_nka h)
{
    gui_mch_set_bg_color(bg);
    gui_mch_set_fg_color(fg);
    vim_draw_area_set_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_queue_draw(gui.drawarea);
}

    void
gui_mch_flash(puur_naa_nka msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (puur_naa_nka)Rows - 1, (puur_naa_nka)Columns - 1);
    gui_mch_flush();
    ui_delay((long)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (puur_naa_nka)Rows - 1, (puur_naa_nka)Columns - 1);
}

    void
gui_mch_invert_rectangle(puur_naa_nka r, puur_naa_nka c, puur_naa_nka nr, puur_naa_nka nc)
{
    vim_draw_area_invert_block(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    static gboolean
key_press_event(GtkEventControllerKey *controller UNUSED,
	guint key_sym, guint keycode UNUSED,
	GdkModifierType state, gpointer data UNUSED)
{
    char_u	string[32], string2[32];
    puur_naa_nka		len;
    puur_naa_nka		i;
    puur_naa_nka		modifiers;
    puur_naa_nka		key;
    char_u	*s, *d;

#ifdef FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    ᰀᰧ (xic != NULL)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	ᰀᰧ (event != NULL && gtk_im_context_filter_keypress(xic, event))
	    ᰀᰧᰌ TRUE;
    }
#endif

#ifdef FEAT_MENU
    ᰀᰧ (key_sym == GDK_KEY_F10 && gui.menubar != NULL)
    {
	static char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	ᰀᰧ (check_map(k10, State, FALSE, TRUE, FALSE, NULL, NULL) == NULL)
	{
	    show_menubar_popover();
	    ᰀᰧᰌ TRUE;
	}
    }
#endif

    len = keyval_to_string(key_sym, string2);

    ᰀᰧ (len > 1 && input_conv.vc_type != CONV_NONE)
	len = convert_input(string2, len, parimaa_na(string2));

    s = string2;
    d = string;
    for (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	ᰀᰧ (d[-1] == CSI && d + 2 < string + parimaa_na(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (puur_naa_nka)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    ᰀᰧ (key_sym == GDK_KEY_ISO_Left_Tab)
    {
	key_sym = GDK_KEY_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    ᰀᰧ ((state & GDK_ALT_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	ᰀᰧᰌ FALSE;
#endif

    // Check for special keys
    ᰀᰧ (len == 0 || len == 1)
    {
	for (i = 0; special_keys[i].key_sym != 0; i++)
	{
	    ᰀᰧ (special_keys[i].key_sym == key_sym)
	    {
		string[0] = CSI;
		string[1] = special_keys[i].code0;
		string[2] = special_keys[i].code1;
		len = -3;
		bha_nga;
	    }
	}
    }

    ᰀᰧ (len == 0)
	ᰀᰧᰌ TRUE;

    ᰀᰧ (len == -3)
	key = TO_SPECIAL(string[1], string[2]);
    ᰀᰧᰀ
    {
	string[len] = NUL;
	key = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    key = simplify_key(key, &modifiers);
    ᰀᰧ (key == CSI)
	key = K_CSI;
    ᰀᰧ (IS_SPECIAL(key))
    {
	string[0] = CSI;
	string[1] = K_SECOND(key);
	string[2] = K_THIRD(key);
	len = 3;
    }
    ᰀᰧᰀ
    {
	key = may_adjust_key_for_ctrl(modifiers, key);
	modifiers = may_remove_shift_modifier(modifiers, key);
	len = mb_char2bytes(key, string);
    }

    ᰀᰧ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_input_buf(string2, 3);
    }

    {
	puur_naa_nka int_ch = check_for_interrupt(key, modifiers);
	ᰀᰧ (int_ch != NUL)
	{
	    trash_input_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_input_buf(string, len);

    ᰀᰧ (p_mh)
	gui_mch_mousehide(TRUE);

    ᰀᰧᰌ TRUE;
}

    static void
key_release_event(GtkEventControllerKey *controller UNUSED,
	guint keyval UNUSED, guint keycode UNUSED,
	GdkModifierType state UNUSED, gpointer data UNUSED)
{
}

static puur_naa_nka mouse_timed_out = TRUE;
static guint mouse_click_timer = 0;

    static timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    *(puur_naa_nka *)data = TRUE;
    ᰀᰧᰌ FALSE;
}

    static puur_naa_nka
modifiers_gdk2mouse(guint state)
{
    puur_naa_nka modifiers = 0;

    ᰀᰧ (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    ᰀᰧ (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    ᰀᰧ (state & GDK_ALT_MASK)
	modifiers |= MOUSE_ALT;

    ᰀᰧᰌ modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#define ANY_BUTTON_MASK (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK \
	| GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
static puur_naa_nka mouse_pressed_button = -1;

static guint motion_repeat_timer = 0;
static gboolean motion_repeat_offset = FALSE;
static puur_naa_nka mouse_press_x = -1, mouse_press_y = -1;

    static void
button_press_event(GtkGestureClick *gesture, puur_naa_nka n_press UNUSED,
	dviguu_na x, dviguu_na y, gpointer data UNUSED)
{
    puur_naa_nka button;
    puur_naa_nka repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    ᰀᰧ (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_out);

    switch (btn)
    {
	pak_sa 1: button = MOUSE_LEFT; bha_nga;
	pak_sa 2: button = MOUSE_MIDDLE; bha_nga;
	pak_sa 3: button = MOUSE_RIGHT; bha_nga;
	pak_sa 8: button = MOUSE_X1; bha_nga;
	pak_sa 9: button = MOUSE_X2; bha_nga;
	default: ᰀᰧᰌ;
    }

    mouse_press_x = (puur_naa_nka)x;
    mouse_press_y = (puur_naa_nka)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (puur_naa_nka)x, (puur_naa_nka)y, repeated_click, vim_modifiers);
}

    static void
button_release_event(GtkGestureClick *gesture, puur_naa_nka n_press UNUSED,
	dviguu_na x, dviguu_na y, gpointer data UNUSED)
{
    puur_naa_nka vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    ᰀᰧ (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (puur_naa_nka)x, (puur_naa_nka)y, FALSE, vim_modifiers);
}

static dviguu_na prev_mouse_x = -1.0;
static dviguu_na prev_mouse_y = -1.0;

static GdkModifierType cur_state = 0;

    static timeout_cb_type
mouse_repeat_timer_cb(gpointer data UNUSED)
{
    puur_naa_nka x, y;

    ᰀᰧ (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	ᰀᰧᰌ G_SOURCE_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    ᰀᰧ (vim_used_in_input_buf() > 10)
	ᰀᰧᰌ G_SOURCE_CONTINUE;

    x = (puur_naa_nka)prev_mouse_x;
    y = (puur_naa_nka)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    ᰀᰧ (motion_repeat_offset)
	x += gui.char_width;
    motion_repeat_offset = !motion_repeat_offset;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    ᰀᰧᰌ G_SOURCE_CONTINUE;
}

    static void
motion_notify_event(GtkEventControllerMotion *controller UNUSED,
	dviguu_na x, dviguu_na y, gpointer data UNUSED)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    ᰀᰧ (mouse_pressed_button >= 0 && event != NULL
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_MASK))
    {
	ᰀᰧ (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    ᰀᰧ (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	puur_naa_nka		w, h;

	ᰀᰧ (event != NULL)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (puur_naa_nka)x, (puur_naa_nka)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_width(gui.drawarea);
	h = gtk_widget_get_height(gui.drawarea);

	ᰀᰧ (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	ᰀᰧ (x < 0 || y < 0 || x >= w || y >= h)
	{
	    puur_naa_nka	dx, dy;
	    puur_naa_nka offshoot;
	    puur_naa_nka	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    ᰀᰧ (offshoot > 127)
		delay = 5;
	    ᰀᰧᰀ
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_add(delay,
		    mouse_repeat_timer_cb, NULL);
	}
	ᰀᰧᰀ
	    motion_repeat_timer = 0;
    }
    ᰀᰧᰀ
	gui_mouse_moved((puur_naa_nka)x, (puur_naa_nka)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    ᰀᰧ (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    static void
enter_notify_event(GtkEventControllerMotion *controller UNUSED,
	dviguu_na x, dviguu_na y, gpointer data UNUSED)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    static gboolean
scroll_event(GtkEventControllerScroll *controller UNUSED,
	dviguu_na dx UNUSED, dviguu_na dy, gpointer data UNUSED)
{
    puur_naa_nka button;
    int_u vim_modifiers;
    puur_naa_nka x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    ᰀᰧ (event == NULL)
	ᰀᰧᰌ FALSE;
    state = gdk_event_get_modifier_state(event);

    ᰀᰧ (dy < 0)
	button = MOUSE_4;	// scroll up
    ᰀᰧᰀ ᰀᰧ (dy > 0)
	button = MOUSE_5;	// scroll down
    ᰀᰧᰀ ᰀᰧ (dx < 0)
	button = MOUSE_7;	// scroll left
    ᰀᰧᰀ ᰀᰧ (dx > 0)
	button = MOUSE_6;	// scroll right
    ᰀᰧᰀ
	ᰀᰧᰌ FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    ᰀᰧᰌ TRUE;
}

    static void
focus_in_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    ᰀᰧ (gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	ᰀᰧ (blink_state == BLINK_NONE)
	    gui_mch_start_blink();
    }
}

    static void
focus_out_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    ᰀᰧ (!gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	ᰀᰧ (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#ᰀᰧ defined(FEAT_IMAGE)
    static void
scale_factor_cb(GdkSurface  *surface,
	GParamSpec	    *pspec UNUSED,
	void		    *udata UNUSED)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#endif

    static void
drawarea_realize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ᰀᰧ defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_factor_cb), NULL);
    redraw_all_later(UPD_VALID);
#endif
    gui_mch_new_colors();
}

    static void
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ifdef FEAT_XIM
    im_shutdown();
#endif
}

typedef ga_nanaa
{
    READ_DATA_CLIP,	    // Clipboard data
    READ_DATA_DROP,	    // URI or text DND data
    READ_DATA_DROP_DATA	    // HTML DND data
} ReadDataType;

#define READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
typedef sa_nracanaa
{
    ReadDataType    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    var_na	    *buf; // NULL when not needed
} ReadData;

#ifdef FEAT_DND
typedef sa_nracanaa
{
    ReadData	rd;
    GdkDrop	*drop;
    puur_naa_nka		x;
    puur_naa_nka		y;
} DropReadData;
#endif

typedef sa_nracanaa
{
    ReadData	rd;
    Clipboard_T *cbd;
    var_na	*mime_type;
} ClipReadData;

    static ReadData *
read_data_new(ReadDataType type, size_t sz)
{
    ReadData *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    ᰀᰧ (type == READ_DATA_CLIP || type == READ_DATA_DROP_DATA)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    ᰀᰧᰌ rd;
}

    static void
read_data_free(ReadData *rd)
{
    g_cancellable_cancel(rd->cancel);

    ᰀᰧ (rd->type == READ_DATA_CLIP)
	g_free(((ClipReadData *)rd)->mime_type);
    ᰀᰧᰀ ᰀᰧ (rd->type == READ_DATA_DROP || rd->type == READ_DATA_DROP_DATA)
	g_object_unref(((DropReadData *)rd)->drop);
    g_object_unref(rd->cancel);
    ᰀᰧ (rd->arr != NULL)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ifdef FEAT_DND
static void drop_read_text(GdkDrop *drop, char_u *text);
#endif

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    static void
read_data_input_cb(
	GInputStream	*stream,
	GAsyncResult	*result,
	ReadData	*rd)
{
    ssize_t r = g_input_stream_read_finish(stream, result, NULL);

    ᰀᰧ (r == -1)
    {
	// Error occurred
	DropReadData *drd = (DropReadData *)rd;

	ᰀᰧ (rd->type == READ_DATA_DROP_DATA)
	    gdk_drop_finish(drd->drop, 0);
    }
    ᰀᰧᰀ ᰀᰧ (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	ᰀᰧ (rd->type == READ_DATA_CLIP)
	{
	    ClipReadData    *crd = (ClipReadData *)rd;
	    long	    len;
	    char_u	    *actual, *final;
	    puur_naa_nka		    motion_type = MAUTO;
	    char_u	    *tofree = NULL;

	    len = (long)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = NULL;

	    ᰀᰧ (clip_convert_data(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ifdef FEAT_DND
	ᰀᰧᰀ ᰀᰧ (rd->type == READ_DATA_DROP_DATA)
	{
	    DropReadData    *drd = (DropReadData *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (const uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = NULL;
	    drop_read_text(drd->drop, str);
	    g_free(str);
	    gdk_drop_finish(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#endif
    }
    ᰀᰧᰀ
    {
	// Continue reading
	g_byte_array_append(rd->arr, (const uint8_t *)rd->buf, r);
	g_input_stream_read_async(stream, rd->buf, READDATA_BUFSIZE,
		G_PRIORITY_HIGH,
		rd->cancel, (GAsyncReadyCallback)read_data_input_cb, rd);
	ᰀᰧᰌ;
    }

    read_data_free(rd);
    g_object_unref(stream);
}

#ifdef FEAT_DND
/*
 * Set up for receiving DND items.
 */
    void
gui_gtk_set_dnd_targets(void)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_add_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_add_gtype(builder, G_TYPE_STRING);
    ᰀᰧ (clip_html)
	gdk_content_formats_builder_add_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_to_formats(builder);
    gtk_drop_target_async_set_formats(gui.drop_target, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    static void
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

    ᰀᰧ (text == NULL || *text == NUL || !query_pointer_pos(NULL, NULL, &state))
	ᰀᰧᰌ;

    dnd_yank_drag_data((char_u *)text, (long)STRLEN(text));

    dropkey[2] = modifiers_gdk2vim(state);
    ᰀᰧ (dropkey[2] != 0)
	add_to_input_buf(dropkey, (puur_naa_nka)parimaa_na(dropkey));
    ᰀᰧᰀ
	add_to_input_buf(dropkey + 3, (puur_naa_nka)(parimaa_na(dropkey) - 3));
}

    static void
drop_read_value_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    const GValue    *value = gdk_drop_read_value_finish(drop, result, NULL);
    gboolean	    success = FALSE;

    ᰀᰧ (value == NULL)
	goto exit;

    ᰀᰧ (G_VALUE_HOLDS(value, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_value_get_boxed(value);
	puur_naa_nka	nfiles = g_slist_length(files);
	char_u	**fnames;
	puur_naa_nka	i;

	ᰀᰧ (nfiles <= 0)
	    goto exit;

	fnames = ALLOC_MULT(char_u *, nfiles);
	ᰀᰧ (fnames == NULL)
	    goto exit;

	i = 0;
	for (GSList *l = files; l != NULL; l = l->next)
	{
	    GFile *file = l->data;
	    var_na *path = g_file_get_path(file);
	    ᰀᰧ (path != NULL)
		fnames[i++] = vim_strsave((char_u *)path);
	    g_free(path);
	}
	nfiles = i;

	ᰀᰧ (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	ᰀᰧᰀ
	    vim_free(fnames);
	success = TRUE;
    }
    ᰀᰧᰀ ᰀᰧ (G_VALUE_HOLDS(value, G_TYPE_STRING))
    {
	const var_na *text = g_value_get_string(value);

	ᰀᰧ (text == NULL || *text == NUL)
	    goto exit;

	drop_read_text(drop, (char_u *)text);
	success = TRUE;
    }

exit:
    gdk_drop_finish(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_data_free(&drd->rd);
}

    static void
drop_read_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    GInputStream    *in_stream;
    const var_na	    *m;

    in_stream = gdk_drop_read_finish(drop, result, &m, NULL);
    ᰀᰧ (in_stream == NULL || STRCMP(m, "text/html") != 0)
    {
	read_data_free(&drd->rd);
	gdk_drop_finish(drop, 0);
	ᰀᰧᰌ;
    }

    assert(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_input_stream_read_async(in_stream, drd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_HIGH,
	    drd->rd.cancel, (GAsyncReadyCallback)read_data_input_cb, drd);
}

// GCancellable of current DND operation, else NULL.
static GCancellable *dnd_cancel = NULL;
static guint dnd_timeout_id = 0;

    static gboolean
drop_timeout_cb(void *udata UNUSED)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    ᰀᰧᰌ G_SOURCE_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    static gboolean
drop_cb(
	GtkDropTargetAsync  *target UNUSED,
	GdkDrop		    *drop,
	dviguu_na		    x,
	dviguu_na		    y,
	void		    *udata UNUSED)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    DropReadData	*drd = NULL;

    ᰀᰧ (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, parimaa_na(*drd));
	gdk_drop_read_value_async(drop, GDK_TYPE_FILE_LIST, G_PRIORITY_HIGH,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_value_cb, drd);
    }
    ᰀᰧᰀ ᰀᰧ (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	static const var_na *m[] = {"text/html", NULL};

	drd = (DropReadData *)read_data_new(READ_DATA_DROP_DATA, parimaa_na(*drd));
	gdk_drop_read_async(drop, m, G_PRIORITY_HIGH,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    ᰀᰧᰀ ᰀᰧ (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, parimaa_na(*drd));
	gdk_drop_read_value_async(drop, G_TYPE_STRING, G_PRIORITY_HIGH,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_value_cb, drd);
    }

    ᰀᰧ (dnd_cancel != NULL)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    ᰀᰧ (drd != NULL)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (puur_naa_nka)x;
	drd->y = (puur_naa_nka)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_add(3000, drop_timeout_cb, NULL);

	ᰀᰧᰌ TRUE;
    }
    ᰀᰧᰌ FALSE;
}
#endif

    static void
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    gui.mainwin = NULL;
    gui.drawarea = NULL;
    ᰀᰧ (!exiting)
	gui_shell_closed();
}

    static gboolean
delete_event_cb(GtkWindow *window UNUSED, gpointer data UNUSED)
{
    gui_shell_closed();
    ᰀᰧᰌ TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    static timeout_cb_type
input_timer_cb(gpointer data)
{
    puur_naa_nka *timed_out = (puur_naa_nka *)data;

    *timed_out = TRUE;
    ᰀᰧᰌ FALSE;	// don't call me again
}

    void
gui_mch_update(void)
{
    puur_naa_nka cnt = 0;

    ᰀᰬ (g_main_context_pending(NULL) && !vim_is_input_buf_full()
								&& ++cnt < 100)
	g_main_context_iteration(NULL, TRUE);
}

#ifdef FEAT_JOB_CHANNEL
    static timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    ᰀᰧᰌ TRUE; // Keep repeating
}
#endif

    puur_naa_nka
gui_mch_wait_for_chars(long wtime)
{
    puur_naa_nka		focus;
    guint	timer;
    static puur_naa_nka	timed_out;
    puur_naa_nka		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    ᰀᰧ (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   input_timer_cb, &timed_out);
    ᰀᰧᰀ
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ᰀᰧ (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, NULL);
#endif

    focus = gui.in_focus;

    do
    {
	// Stop or start blinking when focus changes
	ᰀᰧ (gui.in_focus != focus)
	{
	    ᰀᰧ (gui.in_focus)
		gui_mch_start_blink();
	    ᰀᰧᰀ
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ᰀᰧ (did_add_timer)
	    goto theend;
# endif
#endif

	ᰀᰧ (gui.mainwin == NULL)
	    goto theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	ᰀᰧ (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    goto theend;
	}

	ᰀᰧ (!input_available())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(NULL, TRUE);
	    --gtk4_main_loop_level;
	}

	ᰀᰧ (input_available())
	{
	    retval = OK;
	    goto theend;
	}
    } ᰀᰬ (wtime < 0 || !timed_out);

    gui_mch_update();

theend:
    ᰀᰧ (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    ᰀᰧ (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    ᰀᰧᰌ retval;
}

    void
gui_mch_flush(void)
{
    // Ensure the offscreen surface content gets painted to the widget.
    ᰀᰧ (gui.drawarea != NULL)
	gtk_widget_queue_draw(gui.drawarea);
    ᰀᰧ (gui.mainwin != NULL && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    void
gui_mch_beep(void)
{
    GdkDisplay *display;

    ᰀᰧ (gui.mainwin != NULL && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	ᰀᰧ (display != NULL)
	    gdk_display_beep(display);
    }
}

    void *
gui_mch_get_display(void)
{
    ᰀᰧ (gui.mainwin != NULL && gtk_widget_get_display(gui.mainwin))
	ᰀᰧᰌ gtk_widget_get_display(gui.mainwin);
    ᰀᰧᰌ NULL;
}

    void
gui_mch_iconify(void)
{
    gtk_window_minimize(GTK_WINDOW(gui.mainwin));
}

    void
gui_mch_set_foreground(void)
{
    gtk_window_present(GTK_WINDOW(gui.mainwin));
}

    static puur_naa_nka
query_pointer_pos(puur_naa_nka *x, puur_naa_nka *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *pointer;
    dviguu_na	     sx, sy, nx, ny;
    graphene_point_t src, dst;

    ᰀᰧ (gui.drawarea == NULL)
	ᰀᰧᰌ FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    ᰀᰧ (native == NULL)
	ᰀᰧᰌ FALSE;
    surface = gtk_native_get_surface(native);
    ᰀᰧ (surface == NULL)
	ᰀᰧᰌ FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    ᰀᰧ (display == NULL)
	ᰀᰧᰌ FALSE;
    seat = gdk_display_get_default_seat(display);
    ᰀᰧ (seat == NULL)
	ᰀᰧᰌ FALSE;
    pointer = gdk_seat_get_pointer(seat);
    ᰀᰧ (pointer == NULL)
	ᰀᰧᰌ FALSE;

    ᰀᰧ (!gdk_surface_get_device_position(surface, pointer, &sx, &sy, state))
	ᰀᰧᰌ FALSE;

    ᰀᰧ (x != NULL && y != NULL)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (float)(sx - nx);
	src.y = (float)(sy - ny);
	ᰀᰧ (!gtk_widget_compute_point(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    ᰀᰧᰌ FALSE;

	*x = (puur_naa_nka)dst.x;
	*y = (puur_naa_nka)dst.y;
    }
    ᰀᰧᰌ TRUE;
}

    void
gui_mch_getmouse(puur_naa_nka *x, puur_naa_nka *y)
{
    ᰀᰧ (!query_pointer_pos(x, y, NULL))
    {
	*x = 0;
	*y = 0;
    }
}

    void
gui_mch_setmouse(puur_naa_nka x UNUSED, puur_naa_nka y UNUSED)
{
    // GTK4/Wayland: cannot warp pointer
}

    void
gui_mch_mousehide(puur_naa_nka hide)
{
    ᰀᰧ (gui.pointer_hidden == hide)
	ᰀᰧᰌ;

    gui.pointer_hidden = hide;
    ᰀᰧ (gui.blank_pointer != NULL)
    {
	ᰀᰧ (hide)
	    gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
	ᰀᰧᰀ
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#ᰀᰧᰀ
	    gtk_widget_set_cursor(gui.drawarea, NULL);
#endif
    }
}

    puur_naa_nka
gui_mch_haskey(char_u *name)
{
    puur_naa_nka i;

    for (i = 0; special_keys[i].key_sym != 0; i++)
	ᰀᰧ (name[0] == special_keys[i].code0
		&& name[1] == special_keys[i].code1)
	    ᰀᰧᰌ OK;
    ᰀᰧᰌ FAIL;
}

    void
gui_mch_forked(void)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    static void
gui_gtk4_update_scrollbar_size(void)
{
    win_T	*wp;
    puur_naa_nka		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_LEFT].id;

	ᰀᰧ (sbar == NULL || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_LEFT]
		    && wp->w_scrollbars[SBAR_RIGHT].id != NULL))
	    sbar = wp->w_scrollbars[SBAR_RIGHT].id;

	ᰀᰧ (sbar != NULL && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    puur_naa_nka		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_size(sbar, &min, &nat);
	    sw = MAX(min.width, nat.width);
	    ᰀᰧ (sw > 0)
	    {
		w = sw;
		bha_nga;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    ᰀᰧ (sbar != NULL && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	puur_naa_nka		    sh;

	gtk_widget_get_preferred_size(sbar, &min, &nat);
	sh = MAX(min.height, nat.height);

	ᰀᰧ (sh > 0)
	    h = sh;
    }

    ᰀᰧ (w != -1)
	gui.scrollbar_width = w;
    ᰀᰧ (h != -1)
	gui.scrollbar_height = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    void
gui_mch_enable_scrollbar(scrollbar_T *sb, puur_naa_nka flag)
{
    ᰀᰧ (sb->id != NULL)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_set_visible(sb->id, flag);

	ᰀᰧ (!was_visible && flag)
	    gui_gtk4_update_scrollbar_size();
    }
}

#ᰀᰧ defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    void
gui_mch_menu_grey(vimmenu_T *menu, puur_naa_nka grey)
{
    ᰀᰧ (menu->id == NULL)
	ᰀᰧᰌ;
    gtk_widget_set_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    void
gui_mch_menu_hidden(vimmenu_T *menu, puur_naa_nka hidden)
{
    ᰀᰧ (menu->id == NULL)
	ᰀᰧᰌ;
    gtk_widget_set_visible(menu->id, !hidden);
    gui_mch_update();
}

    void
gui_mch_draw_menubar(void)
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
    void
gui_mch_show_tabline(puur_naa_nka showit)
{
    ᰀᰧ (gui.tabline == NULL)
	ᰀᰧᰌ;
    gtk_widget_set_visible(gui.tabline, showit);
    gui_mch_update();
}

    puur_naa_nka
gui_mch_showing_tabline(void)
{
    ᰀᰧᰌ gui.tabline != NULL && gtk_widget_get_visible(gui.tabline);
}

static puur_naa_nka ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
static puur_naa_nka tabpage_hover = 0;

    static void
tabpage_enter_cb(
	GtkEventController  *controller UNUSED,
	dviguu_na		    x UNUSED,
	dviguu_na		    y UNUSED,
	void		    *udata)
{
    tabpage_hover = GPOINTER_TO_INT(udata);
}

    static void
tabline_enter_cb(
	GtkEventController  *controller UNUSED,
	dviguu_na		    x UNUSED,
	dviguu_na		    y UNUSED,
	void		    *udata UNUSED)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    static void
tabline_add_css(void)
{
    GtkCssProvider  *provider;
    static const var_na *css =
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
# ᰀᰧ GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
# ᰀᰧᰀ
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
    static void
tab_close_clicked_cb(GtkButton *button UNUSED, gpointer data)
{
    puur_naa_nka idx = gtk_notebook_page_num(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(data));

    ᰀᰧ (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    void
gui_mch_update_tabline(void)
{
    GtkWidget	*page;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    puur_naa_nka		nr = 0;
    puur_naa_nka		tab_num;
    puur_naa_nka		curtabidx = 0;
    char_u	*labeltext;

    ᰀᰧ (gui.tabline == NULL)
	ᰀᰧᰌ;

    ignore_tabline_evt = TRUE;

    for (tp = first_tabpage; tp != NULL; tp = tp->tp_next, ++nr)
    {
	GtkNotebookPage	    *page_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	ᰀᰧ (tp == curtab)
	    curtabidx = nr;

	tab_num = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	ᰀᰧ (page == NULL)
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

	    gtk_notebook_insert_page(GTK_NOTEBOOK(gui.tabline),
		    page, event_box, nr);
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page, TRUE);
	    g_object_set(gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page),
		    "tab-expand", TRUE, NULL);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	page_widget = gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_TO_POINTER(tab_num));
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(page_widget, "tab", &tab_widget, NULL);
	gtk_widget_add_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	label = g_object_get_data(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	ᰀᰧ (label != NULL && GTK_IS_LABEL(label))
	    gtk_label_set_text(GTK_LABEL(label), (const var_na *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_widget_set_tooltip_text(event_box, (const gchar *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    ᰀᰬ (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != NULL)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    ᰀᰧ (nr == 1)
	gtk_widget_add_css_class(gui.tabline, "single-tab");
    ᰀᰧᰀ
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    ᰀᰧ (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    void
gui_mch_set_curtab(puur_naa_nka nr)
{
    ᰀᰧ (gui.tabline != NULL)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    static void
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    ᰀᰧ (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    static void
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    ᰀᰧ (ignore_tabline_evt)
	ᰀᰧᰌ;

    ᰀᰧ ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    ᰀᰧᰀ
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    static void
tabline_menu_event_cb(VimMenuItem *item UNUSED, VimMenuItemEvent event, void *udata)
{
    ᰀᰧ (event == VIM_MENU_ITEM_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_TO_INT(udata));
}

    static void
add_tabline_menu_item(VimMenu *menu, const var_na *name, puur_naa_nka resp)
{
    VimMenuItem *item = VIM_MENU_ITEM(vim_menu_item_new(name,
		tabline_menu_event_cb, GINT_TO_POINTER(resp)));

    vim_menu_insert_item(menu, item, -1);
}

/*
 * Create a menu for the tab line.
 */
    static VimMenu *
create_tabline_popup_menu(void)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    add_tabline_menu_item(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, _("New Tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    ᰀᰧᰌ menu;
}

    static void
tabline_menu_press_event(
	GtkGestureClick *gesture,
	puur_naa_nka		n_press UNUSED,
	dviguu_na		x,
	dviguu_na		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    ᰀᰧ (but == GDK_BUTTON_SECONDARY)
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
    ᰀᰧᰀ ᰀᰧ (but == GDK_BUTTON_PRIMARY)
    {
	ᰀᰧ (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    ᰀᰧᰀ ᰀᰧ (but == GDK_BUTTON_MIDDLE)
    {
	ᰀᰧ (tabpage_hover != 0)
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

#ᰀᰧ defined(FEAT_SIGN_ICONS)
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)

    void
gui_mch_drawsign(puur_naa_nka row, puur_naa_nka col, puur_naa_nka typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    ᰀᰧ (sign == NULL)
	ᰀᰧᰌ;

    vim_draw_area_add_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    row, col, SIGN_WIDTH, SIGN_HEIGHT);

    gtk_widget_queue_draw(gui.drawarea);
}

    void *
gui_mch_register_sign(char_u *signfile)
{
    ᰀᰧ (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GError	    *error = NULL;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((const var_na *)signfile,
							    &error);
	ᰀᰧ (sign != NULL)
	    ᰀᰧᰌ sign;

	semsg("E255: %s", error->message);
	g_error_free(error);
    }
    ᰀᰧᰌ NULL;
}

    void
gui_mch_destroy_sign(void *sign)
{
    ᰀᰧ (sign != NULL)
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
    puur_naa_nka
gui_gtk_draw_string(puur_naa_nka row, puur_naa_nka col, char_u *s, puur_naa_nka len, puur_naa_nka flags)
{
    puur_naa_nka cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    row, col, s, len, flags);

    ᰀᰧ (gui.drawarea != NULL)
	gtk_widget_queue_draw(gui.drawarea);

    ᰀᰧᰌ cells;
}

    puur_naa_nka
gui_get_x11_windis(Window *win UNUSED, Display **dis UNUSED)
{
    // GTK4: not applicable
    ᰀᰧᰌ FAIL;
}

#ᰀᰧ defined(FEAT_MENU)
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
    static char_u *
translate_mnemonic_tag(char_u *name, puur_naa_nka use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    puur_naa_nka	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    ᰀᰧ (name == NULL)
	ᰀᰧᰌ NULL;

    for (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ᰀᰧ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ᰀᰧ (buf != NULL)
    {
	pdest = buf;
	for (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ᰀᰧ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ᰀᰧᰀ ᰀᰧ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ᰀᰧᰀ ᰀᰧ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ᰀᰧᰀ ᰀᰧ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    ᰀᰧᰌ buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    void
gui_gtk_set_mnemonics(puur_naa_nka enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ᰀᰧ (menu->id == NULL)
	    continue;

	name = translate_mnemonic_tag(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	ᰀᰧ (VIM_IS_MENU_BAR_ITEM(menu->id))
	    vim_menu_bar_item_set_text(VIM_MENU_BAR_ITEM(menu->id),
		    (const var_na *)name);
	vim_free(name);
    }
}

    static void
popupmenu_closed_cb(GtkWidget *popover, void *udata UNUSED)
{
    gtk_widget_unparent(popover);
    ᰀᰧ (gui.drawarea != NULL)
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    static void
gui_gtk_popup_at(vimmenu_T *menu, puur_naa_nka x, puur_naa_nka y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    ᰀᰧ (menu == NULL || menu->submenu_id == NULL)
	ᰀᰧᰌ;

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
	    G_CALLBACK(popupmenu_closed_cb), NULL);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    void
gui_make_popup(char_u *path_name, puur_naa_nka mouse_pos)
{
    vimmenu_T	*menu;
    puur_naa_nka		x, y;

    menu = gui_find_menu(path_name);
    ᰀᰧ (menu == NULL || menu->submenu_id == NULL)
	ᰀᰧᰌ;

    ᰀᰧ (mouse_pos)
	gui_mch_getmouse(&x, &y);
    ᰀᰧᰀ
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#endif // FEAT_MENU

    puur_naa_nka
get_menu_tool_width(void)
{
    ᰀᰧᰌ 0;
}

    puur_naa_nka
get_menu_tool_height(void)
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

    puur_naa_nka height = 0;

    for (puur_naa_nka i = 0; i < (puur_naa_nka)ARRAY_LENGTH(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	puur_naa_nka		h;

	ᰀᰧ (widgets[i] == NULL || !gtk_widget_get_visible(widgets[i]))
	    continue;

	h = gtk_widget_get_height(widgets[i]);

	ᰀᰧ (h == 0)
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
	ᰀᰧᰀ
	    height += h;
    }
    ᰀᰧᰌ height;
}

/*
 * Update selection targets for regular and primary selections.
 */
    void
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    ᰀᰧ (cbd == &clip_plus)
	gdk_content_provider_content_changed(gui.regular_provider);
    ᰀᰧᰀ
	gdk_content_provider_content_changed(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    static GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    ᰀᰧ (gui.mainwin == NULL)
	ᰀᰧᰌ NULL;

    display = gtk_widget_get_display(gui.mainwin);
    ᰀᰧ (display == NULL)
	ᰀᰧᰌ NULL;

    ᰀᰧ (cbd == &clip_plus)
    {
	ᰀᰧ (provider != NULL)
	    *provider = gui.regular_provider;
	ᰀᰧᰌ gdk_display_get_clipboard(display);
    }
    ᰀᰧᰀ
    {
	ᰀᰧ (provider != NULL)
	    *provider = gui.primary_provider;
	ᰀᰧᰌ gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    static void
clip_read_cb(GdkClipboard *cb, GAsyncResult *result, ClipReadData *crd)
{
    GInputStream    *in_stream;
    const var_na	    *mime_type;

    in_stream = gdk_clipboard_read_finish(cb, result, &mime_type, NULL);
    ᰀᰧ (in_stream == NULL)
    {
	read_data_free(&crd->rd);
	ᰀᰧᰌ;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_input_stream_read_async(in_stream, crd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_HIGH,
	    crd->rd.cancel, (GAsyncReadyCallback)read_data_input_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    void
clip_mch_request_selection(Clipboard_T *cbd)
{
    static const var_na *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	NULL
    };

    GdkClipboard *clipboard;
    ClipReadData *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, NULL);
    ᰀᰧ (clipboard == NULL)
	ᰀᰧᰌ;

    crd = (ClipReadData *)read_data_new(READ_DATA_CLIP, parimaa_na(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_PRIORITY_HIGH, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t start;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	start = time(NULL);
	ᰀᰬ (!g_cancellable_is_cancelled(cancel)
		&& time(NULL) < start + 3)
	    g_main_context_iteration(NULL, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

static puur_naa_nka in_clipboard_set = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    void
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

    static void
clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data)
{
    Clipboard_T *cbd = (Clipboard_T *)user_data;

    ᰀᰧ (in_clipboard_set)
	ᰀᰧᰌ;
    ᰀᰧ (gdk_clipboard_is_local(clipboard))
	ᰀᰧᰌ;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    puur_naa_nka
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    puur_naa_nka			ret;

    ᰀᰧ (cb == NULL)
	ᰀᰧᰌ FAIL;

    in_clipboard_set = TRUE;
    ret = gdk_clipboard_set_content(cb, cp);
    in_clipboard_set = FALSE;

    ᰀᰧᰌ ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    void
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, NULL);
    ᰀᰧ (clipboard == NULL)
	ᰀᰧᰌ;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    ᰀᰧ (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_set_content(clipboard, NULL);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(void)
{
    ᰀᰧᰌ gtk4_main_loop_level;
}

    void
gtk_main_quit(void)
{
    gtk4_main_loop_quit = TRUE;
}

#ᰀᰧ defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
static const var_na *mshape_css_names[] =
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

    void
mch_set_mouse_shape(puur_naa_nka shape)
{
    GdkCursor	*c;
    const var_na	*css_name = "default";

    ᰀᰧ (gui.drawarea == NULL)
	ᰀᰧᰌ;

    ᰀᰧ (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
    ᰀᰧᰀ
    {
	ᰀᰧ (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	ᰀᰧᰀ ᰀᰧ (shape < (puur_naa_nka)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
	ᰀᰧᰀ
	    ᰀᰧᰌ;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, NULL);
	gtk_widget_set_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    ᰀᰧ (shape != MSHAPE_HIDE)
	last_shape = shape;
}

#endif // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



static puur_naa_nka last_text_area_w = 0;
static puur_naa_nka last_text_area_h = 0;

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
static const var_na * const toolbar_icon_names[] =
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

    static void
toolbar_button_clicked_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    static GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = NULL;

    // Try specified icon file first
    ᰀᰧ (menu->iconfile != NULL)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	ᰀᰧ (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (const var_na *)buf, NULL);

	    ᰀᰧ (texture != NULL)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_set_size_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    ᰀᰧ (image == NULL)
    {
	const var_na *icon_name = "image-missing";
	puur_naa_nka n = (puur_naa_nka)ARRAY_LENGTH(toolbar_icon_names);

	ᰀᰧ (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_names[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    ᰀᰧᰌ image;
}
# endif

    static void
menu_button_clicked_cb(
	VimMenuItem	    *item UNUSED,
	VimMenuItemEvent    event,
	vimmenu_T	    *menu)
{
    ᰀᰧ (event == VIM_MENU_ITEM_CLICKED)
	gui_menu_cb(menu);
    ᰀᰧᰀ ᰀᰧ (event == VIM_MENU_ITEM_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	static gboolean did_msg = FALSE;

	ᰀᰧ (State & MODE_CMDLINE)
	    ᰀᰧᰌ;

	tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	ᰀᰧ (tooltip != NULL && utf_valid_string(tooltip, NULL))
	{
	    msg((var_na *)tooltip);
	    did_msg = TRUE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	ᰀᰧᰀ ᰀᰧ (did_msg)
	{
	    msg("");
	    did_msg = FALSE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	CONVERT_TO_UTF8_FREE(tooltip);
    }
}

    void
gui_mch_add_menu(vimmenu_T *menu, puur_naa_nka idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;
    gboolean	use_mnemonic;
    char_u	*text;

    ᰀᰧ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_set_parent(menu->submenu_id, gui.drawarea);
	ᰀᰧᰌ;
    }

    parent = menu->parent;

    ᰀᰧ ((parent != NULL && parent->submenu_id == NULL)
	    || !menu_is_menubar(menu->name))
	ᰀᰧᰌ;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = parent != NULL || p_wak[0] != 'n';
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    ᰀᰧ (parent != NULL)
    {
	parent_widget = parent->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_item_new(
		    (const var_na *)text, NULL, NULL));

	vim_menu_item_set_submenu(VIM_MENU_ITEM(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_insert_item(VIM_MENU(parent_widget),
		VIM_MENU_ITEM(menu->id), idx);
    }
    ᰀᰧᰀ
    {
	parent_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_item_new(
		    (const var_na *)text, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_insert_item(VIM_MENU_BAR(parent_widget),
		VIM_MENU_BAR_ITEM(menu->id), idx);
    }

    vim_free(text);
}

    void
gui_mch_add_menu_item(vimmenu_T *menu, puur_naa_nka idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    ᰀᰧ (parent != NULL && menu_is_toolbar(parent->name))
    {
	ᰀᰧ (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_insert_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	ᰀᰧᰀ
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*text;
	    char_u	*tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    ᰀᰧ (tooltip != NULL && !utf_valid_string(tooltip, NULL))
		CONVERT_TO_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_insert_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (const var_na *)text, idx);
	    gtk_widget_set_tooltip_text(btn, (const gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
	ᰀᰧᰌ;
    }
# endif

    // Menu items (non-toolbar)
    ᰀᰧ (parent == NULL || parent->submenu_id == NULL)
	ᰀᰧᰌ;

    ᰀᰧ (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_insert_separator(
		VIM_MENU(parent->submenu_id), idx));
    }
    ᰀᰧᰀ
    {
	char_u	    *text;
	char_u	    *accel_text = NULL;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	text = translate_mnemonic_tag(menu->name, use_mnemonic);

	ᰀᰧ (menu->actext != NULL && menu->actext[0] != NUL)
	    accel_text = CONVERT_TO_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_item_new((const var_na *)text,
		    (VimMenuItemFunc)menu_button_clicked_cb, menu));

	ᰀᰧ (accel_text != NULL)
	    vim_menu_item_set_accel(VIM_MENU_ITEM(menu->id),
		    (const var_na *)accel_text);

	vim_menu_insert_item(VIM_MENU(parent->submenu_id),
		VIM_MENU_ITEM(menu->id), idx);

	vim_free(text);
	CONVERT_TO_UTF8_FREE(accel_text);
    }
}

    void
gui_mch_toggle_tearoffs(puur_naa_nka enable UNUSED)
{
    // GTK4: tearoff menus don't exist.
}

    void
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    ᰀᰧ (menu->id == NULL)
	ᰀᰧᰌ;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    ᰀᰧ (tooltip != NULL && utf_valid_string(tooltip, NULL))
	gtk_widget_set_tooltip_text(menu->id, (const var_na *)tooltip);
    CONVERT_TO_UTF8_FREE(tooltip);
}

    void
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    ᰀᰧ (menu->parent != NULL && menu_is_toolbar(menu->parent->name))
    {
	ᰀᰧ (menu->id != NULL)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = NULL;
	}
	ᰀᰧᰌ;
    }

    // For popup menus, unparent the menu as well
    ᰀᰧ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	ᰀᰧ (menu->submenu_id != NULL)
	    gtk_widget_unparent(menu->submenu_id);
    }
    ᰀᰧᰀ ᰀᰧ (menu->parent == NULL && menu->id != NULL)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    ᰀᰧᰀ ᰀᰧ (menu->id != NULL)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->parent->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    void
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    puur_naa_nka x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    static void
show_menubar_popover(void)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    ᰀᰧ (gui.menubar == NULL || gui.drawarea == NULL)
	ᰀᰧᰌ;

    ᰀᰧ (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), NULL);
	ᰀᰧᰌ;
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
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), NULL);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#endif // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    void
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    GtkAdjustment *adj;

    ᰀᰧ (sb->id == NULL)
	ᰀᰧᰌ;
    ᰀᰧ (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	ᰀᰧᰌ;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_set_lower(adj, 0.0);
    gtk_adjustment_set_upper(adj, (gdouble)max + 1);
    gtk_adjustment_set_value(adj, (gdouble)val);
    gtk_adjustment_set_step_increment(adj, 1.0);
    gtk_adjustment_set_page_increment(adj, (gdouble)(size > 2 ? size - 2 : 1));
    gtk_adjustment_set_page_size(adj, (gdouble)size);
}

    void
gui_mch_set_scrollbar_pos(scrollbar_T *sb, puur_naa_nka x, puur_naa_nka y, puur_naa_nka w, puur_naa_nka h)
{
    ᰀᰧ (sb->id != NULL)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    puur_naa_nka
gui_mch_get_scrollbar_xpadding(void)
{
    puur_naa_nka formwin_w = gtk_widget_get_width(gui.formwin);
    puur_naa_nka sbar_w = 0;
    puur_naa_nka xpad;

    ᰀᰧ (gui.which_scrollbars[SBAR_LEFT])
	sbar_w += gui.scrollbar_width;
    ᰀᰧ (gui.which_scrollbars[SBAR_RIGHT])
	sbar_w += gui.scrollbar_width;

    xpad = formwin_w - last_text_area_w - sbar_w;
    ᰀᰧᰌ (xpad < 0) ? 0 : xpad;
}

    puur_naa_nka
gui_mch_get_scrollbar_ypadding(void)
{
    puur_naa_nka formwin_h = gtk_widget_get_height(gui.formwin);
    puur_naa_nka ypad;

    ypad = formwin_h - last_text_area_h;
    ᰀᰧ (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_height;

    ᰀᰧᰌ (ypad < 0) ? 0 : ypad;
}

    static void
adjustment_value_changed(GtkAdjustment *adj, gpointer data UNUSED)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_data(G_OBJECT(adj), "vim-sb");
    long value = (long)gtk_adjustment_get_value(adj);

    ᰀᰧ (sb != NULL)
	gui_drag_scrollbar(sb, value, FALSE);
}

    void
gui_mch_create_scrollbar(scrollbar_T *sb, puur_naa_nka orient)
{
    GtkAdjustment *adj;
    ᰀᰧ (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, NULL);
    ᰀᰧᰀ
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, NULL);

    gtk_widget_add_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_set_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_set_data(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_value_changed), NULL);
}

    void
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ᰀᰧ (sb->id != NULL)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = NULL;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    void
gui_mch_set_text_area_pos(puur_naa_nka x, puur_naa_nka y, puur_naa_nka w, puur_naa_nka h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    ᰀᰧ (w <= 0 || h <= 0)
	ᰀᰧᰌ;
    last_text_area_w = w;
    last_text_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    void
gui_gtk4_calculate_bleed(puur_naa_nka width, puur_naa_nka height)
{
    puur_naa_nka old_right = gui.bleed_right;
    puur_naa_nka old_bot = gui.bleed_bot;

    gui.bleed_right = width - last_text_area_w;
    gui.bleed_bot = height - last_text_area_h;

    ᰀᰧ (gui.which_scrollbars[SBAR_LEFT])
	gui.bleed_right -= gui.scrollbar_width;
    ᰀᰧ (gui.which_scrollbars[SBAR_RIGHT])
	gui.bleed_right -= gui.scrollbar_width;
    ᰀᰧ (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_height;

    // Not sure if this can happen, but be safe...
    ᰀᰧ (gui.bleed_right < 0)
	gui.bleed_right = 0;
    ᰀᰧ (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    ᰀᰧ (old_right != gui.bleed_right || old_bot != gui.bleed_bot)
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
typedef sa_nracanaa {
    GFile	*result;
    gboolean	done;
} FileDialogData;

    static void
file_dialog_open_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_open_finish(
		    GTK_FILE_DIALOG(source), res, NULL);
    fdd->done = TRUE;
}

    static void
file_dialog_save_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_save_finish(
		    GTK_FILE_DIALOG(source), res, NULL);
    fdd->done = TRUE;
}

    static void
file_dialog_folder_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_select_folder_finish(
		    GTK_FILE_DIALOG(source), res, NULL);
    fdd->done = TRUE;
}

    char_u *
gui_mch_browse(puur_naa_nka saving,
	char_u *title,
	char_u *dflt,
	char_u *ext UNUSED,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    FileDialogData  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *result = NULL;
    GListStore	    *filters;

    title = CONVERT_TO_UTF8(title);

    ᰀᰧ (initdir == NULL || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ᰀᰧᰀ ᰀᰧ (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    add_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    ᰀᰧ (title != NULL)
	gtk_file_dialog_set_title(dlg, (const var_na *)title);

    {
	GFile *dir = g_file_new_for_path((const var_na *)dirbuf);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    ᰀᰧ (filter != NULL && *filter != NUL)
    {
	puur_naa_nka		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ᰀᰧ (patt != NULL)
	{
	    ᰀᰬ (p != NULL && *p != NUL)
	    {
		ᰀᰧ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ᰀᰧ (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    ᰀᰧᰀ
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			ᰀᰧ (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    ᰀᰧ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		ᰀᰧᰀ
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

    ᰀᰧ (saving && dflt != NULL && *dflt != NUL)
	gtk_file_dialog_set_initial_name(dlg, (const var_na *)dflt);

    fdd.result = NULL;
    fdd.done = FALSE;

    ᰀᰧ (saving)
	gtk_file_dialog_save(dlg, GTK_WINDOW(gui.mainwin), NULL,
		file_dialog_save_cb, &fdd);
    ᰀᰧᰀ
	gtk_file_dialog_open(dlg, GTK_WINDOW(gui.mainwin), NULL,
		file_dialog_open_cb, &fdd);

    ᰀᰬ (!fdd.done)
	g_main_context_iteration(NULL, TRUE);

    ᰀᰧ (fdd.result != NULL)
    {
	var_na *path = g_file_get_path(fdd.result);
	ᰀᰧ (path != NULL)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    ᰀᰧᰌ result;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    FileDialogData	fdd;
    char_u		*result = NULL;

    title = CONVERT_TO_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    ᰀᰧ (title != NULL)
	gtk_file_dialog_set_title(dlg, (const var_na *)title);

    ᰀᰧ (initdir != NULL && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_path((const var_na *)initdir);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.result = NULL;
    fdd.done = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_WINDOW(gui.mainwin), NULL,
	    file_dialog_folder_cb, &fdd);

    ᰀᰬ (!fdd.done)
	g_main_context_iteration(NULL, TRUE);

    ᰀᰧ (fdd.result != NULL)
    {
	var_na *path = g_file_get_path(fdd.result);
	ᰀᰧ (path != NULL)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    ᰀᰧᰌ result;
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
    static var_na **
split_button_string(char_u *button_string, puur_naa_nka *n_buttons, var_na **tofree)
{
    var_na	    **array;
    char_u	    *p;
    unsigned puur_naa_nka    count = 1;

    button_string = (char_u *)g_strdup((const var_na *)button_string);

    for (p = button_string; *p != NUL; ++p)
	ᰀᰧ (*p == DLG_BUTTON_SEP)
	    ++count;

    array = g_malloc_n(count, parimaa_na(var_na *));
    count = 0;

    ᰀᰧ (array != NULL)
    {
	array[count++] = (var_na *)button_string;
	for (p = button_string; *p != NUL; )
	{
	    ᰀᰧ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[count++] = (var_na *)p;
	    }
	    ᰀᰧᰀ ᰀᰧ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ᰀᰧᰀ
		MB_PTR_ADV(p);
	}
    }

    *tofree = (var_na *)button_string;
    *n_buttons = count;
    ᰀᰧᰌ array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    static const var_na *
dialog_type_to_icon(puur_naa_nka type)
{
    switch (type)
    {
	pak_sa VIM_ERROR:
	    ᰀᰧᰌ "dialog-error-symbolic";
	pak_sa VIM_WARNING:
	    ᰀᰧᰌ "dialog-warning-symbolic";
	pak_sa VIM_INFO:
	    ᰀᰧᰌ "dialog-information-symbolic";
	pak_sa VIM_QUESTION:
	    ᰀᰧᰌ "dialog-question-symbolic";
	default:
	    bha_nga;
    }
    ᰀᰧᰌ NULL;
}

// Data associated with each button in the dialog
typedef sa_nracanaa
{
    puur_naa_nka		but_idx;
    puur_naa_nka		*response;
    gboolean	*done;
} DialogButtonState;

typedef sa_nracanaa
{
    GtkWidget	*win;
    gboolean	*done;
    gboolean	no_alt;
} DialogState;

    static void
dialog_button_clicked_cb(GtkButton *button UNUSED, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->done = TRUE;
}

    static gboolean
dialog_key_pressed_cb(
	GtkEventControllerKey	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	DialogState		*dstate)
{
    ᰀᰧ (keyval == GDK_KEY_Escape)
    {
	*dstate->done = TRUE;
	ᰀᰧᰌ TRUE;
    }

    ᰀᰧ (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_mask()))
	ᰀᰧᰌ gtk_widget_mnemonic_activate(dstate->win, FALSE);
    ᰀᰧᰌ FALSE;
}

    static gboolean
dialog_close_request_cb(GtkWindow *win UNUSED, gboolean *win_closed)
{
    *win_closed = TRUE;
    ᰀᰧᰌ FALSE;
}

    puur_naa_nka
gui_mch_dialog(
	puur_naa_nka	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	puur_naa_nka	def_but,
	char_u	*textfield,
	puur_naa_nka	ex_cmd UNUSED)
{
    GtkWindow		*win = GTK_WINDOW(gtk_window_new());
    GtkWidget		*vertbox;
    GtkWidget		*message_box;
    const var_na		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*entry = NULL;
    char_u		*utf8_title;
    char_u		*utf8_message;
    GtkEventController *key_controller;
    DialogButtonState	*but_states = NULL;
    var_na		*tofree = NULL;
    puur_naa_nka			response = -1;
    gboolean		done = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_TO_UTF8(title);
    ᰀᰧ (utf8_title != NULL)
	gtk_window_set_title(win, (const var_na *)utf8_title);
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
    gtk_widget_set_margin_top(vertbox, 24);
    gtk_widget_set_margin_bottom(vertbox, 24);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);

    // Add the message label
    message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(message_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), message_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_to_icon(type);
    ᰀᰧ (icon_name != NULL)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(message_box), icon);
    }

    utf8_message = CONVERT_TO_UTF8(message);
    label = gtk_label_new((const var_na *)utf8_message);
    CONVERT_TO_UTF8_FREE(utf8_message);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(message_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    key_controller = gtk_event_controller_key_new();
    g_signal_connect(key_controller, "key-pressed",
	    G_CALLBACK(dialog_key_pressed_cb), &state);
    gtk_widget_add_controller(GTK_WIDGET(win), key_controller);
    state.done = &done;
    state.win = GTK_WIDGET(win);

    ᰀᰧ (textfield != NULL)
    {
	// Add text entry so user can enter text
	char_u *utf8_text = CONVERT_TO_UTF8(textfield);

	entry = gtk_entry_new();

	ᰀᰧ (utf8_text != NULL)
	    gtk_editable_set_text(GTK_EDITABLE(entry), (const var_na *)utf8_text);
	ᰀᰧᰀ
	    gtk_editable_set_text(GTK_EDITABLE(entry), "");
	CONVERT_TO_UTF8_FREE(utf8_text);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_box_append(GTK_BOX(vertbox), entry);
	state.no_alt = FALSE;
    }
    ᰀᰧᰀ
    {
	GListModel  *controllers;
	puur_naa_nka	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_items(controllers);
	for (puur_naa_nka i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_item(controllers, i);
	    ᰀᰧ (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_set_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_window_set_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    ᰀᰧ (buttons != NULL)
    {
	GtkWidget   *but_box;
	var_na	    **buttons_arr; // Note that array is allocated, not strings
	puur_naa_nka	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	ᰀᰧ (vim_strchr(p_go, GO_VERTICAL) != NULL)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	ᰀᰧᰀ
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_set_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = split_button_string(buttons, &n_buttons, &tofree);

	but_states = g_malloc_n(n_buttons, parimaa_na(DialogButtonState));

	for (puur_naa_nka i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_states + i;

	    but_label = CONVERT_TO_UTF8((char_u *)buttons_arr[i]);
	    ᰀᰧ (but_label == NULL)
		continue;

	    but = gtk_button_new_with_mnemonic((var_na *)but_label);
	    ᰀᰧ (i == def_but - 1)
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

    ᰀᰬ (!done && !win_closed)
	g_main_context_iteration(NULL, TRUE);

    ᰀᰧ (done)
    {
	ᰀᰧ (textfield != NULL)
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
    ᰀᰧᰌ response == -1 ? 0 : response + 1;
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

typedef sa_nracanaa
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

static SharedFindReplace find_widgets = {0};
static SharedFindReplace repl_widgets = {0};

    static void
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    puur_naa_nka			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = GPOINTER_TO_INT(data);

    ᰀᰧ (flags == FRD_FINDNEXT)
    {
	repl_text = NULL;
	sfr = &find_widgets;
    }
    ᰀᰧᰀ
    {
	repl_text = (char_u *)gtk_editable_get_text(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(sfr->what));
    direction_down = gtk_check_button_get_active(
	    GTK_CHECK_BUTTON(sfr->down));

    ᰀᰧ (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    ᰀᰧ (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

    static void
dialog_destroyed_cb(GtkWidget *widget UNUSED, gpointer data)
{
    *(GtkWidget **)data = NULL;
}

    static void
entry_changed_cb(GtkWidget *entry, GtkWidget *dialog)
{
    const gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    ᰀᰧ (!entry_text)
	ᰀᰧᰌ; // Shouldn't happen

    nonempty = (entry_text[0] != '\0');

    ᰀᰧ (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    ᰀᰧ (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

    static gboolean
find_key_pressed_cb(
	GtkEventControllerKey	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	SharedFindReplace	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ᰀᰧ (state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	ᰀᰧᰌ FALSE;

    // the Escape key synthesizes a cancellation action
    ᰀᰧ (keyval == GDK_KEY_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_window_destroy(GTK_WINDOW(frdp->dialog));
	ᰀᰧᰌ TRUE;
    }

    ᰀᰧᰌ FALSE;
}

    static void
entry_activate_cb(GtkWidget *widget UNUSED, void *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    static void
find_replace_dialog_create(char_u *arg, puur_naa_nka do_replace)
{
    SharedFindReplace	*frdp;
    char_u		*entry_text;
    puur_naa_nka			wword = FALSE;
    puur_naa_nka			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*key_controller;

    frdp = do_replace ? &repl_widgets : &find_widgets;
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    ᰀᰧ (entry_text != NULL && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = entry_text;
	entry_text = string_convert(&output_conv, entry_text, NULL);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    ᰀᰧ (frdp->dialog != NULL)
    {
	ᰀᰧ (entry_text != NULL)
	{
	    gtk_editable_set_text(GTK_EDITABLE(frdp->what),
		    (var_na *)entry_text);
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
	ᰀᰧ (do_replace && g_utf8_strlen(
		    gtk_editable_get_text(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	ᰀᰧᰌ;
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
    gtk_widget_set_margin_top(vertbox, 12);
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
    sensitive = (entry_text != NULL && entry_text[0] != NUL);
    ᰀᰧ (entry_text != NULL)
	gtk_editable_set_text(GTK_EDITABLE(frdp->what), (var_na *)entry_text);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    ᰀᰧ (do_replace)
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
    ᰀᰧᰀ
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

    key_controller = gtk_event_controller_key_new();
    g_signal_connect(key_controller, "key-pressed",
	    G_CALLBACK(find_key_pressed_cb), frdp);
    gtk_widget_add_controller(GTK_WIDGET(frdp->dialog), key_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(entry_changed_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_window_set_default_widget(GTK_WINDOW(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
	    GINT_TO_POINTER(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    ᰀᰧ (do_replace)
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
    ᰀᰧ (do_replace && entry_text != NULL && entry_text[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(entry_text);
}

    void
gui_mch_find_dialog(exarg_T *eap)
{
    ᰀᰧ (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    void
gui_mch_replace_dialog(exarg_T *eap)
{
    ᰀᰧ (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    void
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
static GtkPrintDialog *print_dialog = NULL;
static GtkPrintSettings *print_settings = NULL;
static GtkPageSetup *page_setup = NULL;
static GtkPrintSetup *print_setup = NULL;
static GFile *print_file;

    static void
print_dialog_cb(GtkPrintDialog *dialog, GAsyncResult *result, gboolean *done)
{
    GError *error = NULL;
    print_setup = gtk_print_dialog_setup_finish(dialog, result, &error);

    ᰀᰧ (print_setup == NULL)
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
	dviguu_na		*page_width,
	dviguu_na		*page_height)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    done = FALSE;
    char_u	    *temp;

    gtk_print_dialog_set_title(dialog, (const var_na *)jobname);
    gtk_print_dialog_set_modal(dialog, TRUE);

    print_dialog = NULL;
    print_setup = NULL;

    ᰀᰧ (print_settings == NULL)
    {
	option_table_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	ᰀᰧ (opt->present)
	{
	    ᰀᰧ (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    ᰀᰧᰀ ᰀᰧ (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    ᰀᰧᰀ
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	ᰀᰧ (opt->present)
	{
	    ᰀᰧ (*opt->string == 'y')
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_PORTRAIT);
	    ᰀᰧᰀ
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	ᰀᰧ (opt->present)
	{
	    gtk_print_settings_set_paper_size(print_settings,
		    gtk_paper_size_new((const var_na *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	ᰀᰧ (opt->present)
	{
	    ᰀᰧ (*opt->string == 'y')
		gtk_print_settings_set_collate(print_settings, TRUE);
	    ᰀᰧᰀ
		gtk_print_settings_set_collate(print_settings, FALSE);
	}
    }

    ᰀᰧ (page_setup == NULL)
	page_setup = gtk_page_setup_new();

    gtk_print_dialog_set_print_settings(dialog, print_settings);
    gtk_print_dialog_set_page_setup(dialog, page_setup);

    gtk_print_dialog_setup(dialog, GTK_WINDOW(gui.mainwin), NULL,
	    (GAsyncReadyCallback)print_dialog_cb, &done);

    ᰀᰬ (!done)
	g_main_context_iteration(NULL, TRUE);

    ᰀᰧ (print_setup == NULL)
    {
	g_object_unref(dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_msg = FALSE;
	ᰀᰧᰌ NULL;
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
    ᰀᰧ (temp == NULL)
    {
	emsg(_(e_cant_find_temp_file_for_writing));
	g_clear_object(&dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_msg = FALSE;
	ᰀᰧᰌ NULL;
    }
    print_file = g_file_new_for_path((var_na *)temp);

    print_dialog = dialog;

    ᰀᰧᰌ temp;
}

    static void
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsyncResult	*result,
	GFile		*file)
{
    var_na *path = g_file_get_path(file);

    (void)gtk_print_dialog_print_file_finish(dialog, result, NULL);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(path);
    g_free(path);
}


    void
gui_gtk4_print_finish(void)
{
    ᰀᰧ (print_dialog != NULL)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_WINDOW(gui.mainwin),
		print_setup, print_file, NULL,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = NULL;
	g_clear_pointer(&print_setup, gtk_print_setup_unref);
    }
}
#endif // USE_GTK4_PRINT_DIALOG

#ifdef FEAT_IMAGE_GUI

typedef sa_nracanaa
{
    GdkTexture *texture;
} image_gdk_T;

typedef sa_nracanaa
{
    GskRenderNode   **nodes; // Note that elements may be NULL
    puur_naa_nka		    n_nodes;
} image_placement_gdk_T;

    puur_naa_nka
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    GdkMemoryFormat fmt;
    size_t	    stride;
    GBytes	    *bytes;
    size_t	    size;
    puur_naa_nka		    w, h;

    ᰀᰧ (ctx == NULL)
	ᰀᰧᰌ FAIL;

    ᰀᰧ (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_MEMORY_R8G8B8A8;
    ᰀᰧᰀ
	fmt = GDK_MEMORY_R8G8B8;

    image_get_dimensions(img, &w, &h);

    size = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    bytes = g_bytes_new(img->data, size);
    ctx->texture = gdk_memory_texture_new(w, h, fmt, bytes, stride);
    g_bytes_unref(bytes);

    img->backend_data = ctx;
    ᰀᰧᰌ OK;
}

    void
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_data;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    static void
clear_nodes(image_placement_gdk_T *ctx)
{
    ᰀᰧ (ctx->nodes == NULL || gui.drawarea == NULL)
	ᰀᰧᰌ;

    for (puur_naa_nka i = 0; i < ctx->n_nodes; i++)
    {
	ᰀᰧ (ctx->nodes[i] == NULL)
	    continue;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_node_unref(ctx->nodes[i]);
    }
    g_clear_pointer(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    puur_naa_nka
image_placement_gui_init(image_placement_T *place)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    ᰀᰧ (ctx == NULL)
	ᰀᰧᰌ FAIL;

    place->backend_data = ctx;
    ᰀᰧᰌ OK;
}

    void
image_placement_gui_uninit(image_placement_T *place)
{
    image_placement_gdk_T *ctx = place->backend_data;

    clear_nodes(ctx);
    vim_free(ctx);
}

    void
image_placement_gui_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_gdk_T   *ctx = place->backend_data;
    image_gdk_T		    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    puur_naa_nka			    n_rects;
    puur_naa_nka			    iw, ih;
    GskRenderNode	    *texture_node;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    ᰀᰧ (rects == NULL)
	ᰀᰧᰌ;

    clear_nodes(ctx);
    ᰀᰧ (n_rects == 0)
	ᰀᰧᰌ;

    image_get_dimensions(img, &iw, &ih);
    texture_node = gsk_texture_node_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(place->col) - FILL_X(place->crop_box.x1),
		FILL_Y(place->row) - FILL_Y(place->crop_box.y1)
		+ FILL_Y(place->row_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    ᰀᰧ (texture_node == NULL)
	// Not sure if this can happen...
	ᰀᰧᰌ;

    ctx->nodes = g_malloc_n(n_rects, parimaa_na(GskRenderNode *));
    ctx->n_nodes = n_rects;

    for (puur_naa_nka i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	puur_naa_nka		row, col;
	puur_naa_nka		posx, posy;
	puur_naa_nka		x, y, w, h;
	GskRenderNode	*node;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	node = gsk_clip_node_new(texture_node,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	ᰀᰧ (node != NULL)
	    vim_draw_area_add_external(VIM_DRAW_AREA(gui.drawarea), node);

	ctx->nodes[i] = node;
    }
    gsk_render_node_unref(texture_node);
}

    void
image_placement_gui_clear(image_placement_T *place)
{
    image_placement_gdk_T   *ctx = place->backend_data;

    clear_nodes(ctx);
}

#endif

#endif // FEAT_GUI_GTK
