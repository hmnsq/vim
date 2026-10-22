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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK

#ਅਨ੍ਤਰ੍ਭਾਵ <gdk/gdkkeysyms.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gdk/gdk.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gtk/gtk.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "gui_gtk4_f.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "gui_gtk4_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "gui_gtk4_da.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
# include "gui_gtk4_tb.h"
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# include "gui_gtk4_menu.h"
#ਜੇ_ਅੰਤ

/*
 * Geometry string parser, replacing XParseGeometry to remove X11 dependency.
 * Format: [WIDTHxHEIGHT][{+-}XOFF{+-}YOFF]
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ No_ਮੁੱਲ		0x0000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਚੌੜਾਈ_ਮੁੱਲ	0x0004
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਉਚਾਈ_ਮੁੱਲ	0x0008

    ਸਥਿਰ ਪੂਰਨ
vim_parse_geometry(ਅਚਲ ਅੱਖਰ *str, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *ਚੌੜਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *ਉਚਾਈ)
{
    ਪੂਰਨ ਨਕਾਬ = No_ਮੁੱਲ;
    ਅੱਖਰ *ਸਿਰਾ;
    ਲੰਮਾ val;

    ਜੇ (str == ਨਿਰਾ || *str == NUL)
	ਵਾਪਸ ਨਕਾਬ;

    // Parse width
    ਜੇ (*str != '+' && *str != '-')
    {
	val = strtol(str, &ਸਿਰਾ, 10);
	ਜੇ (ਸਿਰਾ != str)
	{
	    *ਚੌੜਾਈ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)val;
	    ਨਕਾਬ |= ਚੌੜਾਈ_ਮੁੱਲ;
	    str = ਸਿਰਾ;
	}
    }

    // Parse 'x' or 'X' separator and height
    ਜੇ (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &ਸਿਰਾ, 10);
	ਜੇ (ਸਿਰਾ != str)
	{
	    *ਉਚਾਈ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)val;
	    ਨਕਾਬ |= ਉਚਾਈ_ਮੁੱਲ;
	    str = ਸਿਰਾ;
	}
    }

    ਵਾਪਸ ਨਕਾਬ;
}

#ਜੇ defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_shape = 0;
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
ਸਥਿਰ ਗਿਣਤੀ {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
ਸਥਿਰ ਪੂਰਨ gtk4_main_loop_level = 0;
ਸਥਿਰ ਪੂਰਨ gtk4_main_loop_quit = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
ਸਥਿਰ ਬਣਤਰ special_ਕੁੰਜੀ
{
    guint ਕੁੰਜੀ_sym;
    char_u code0;
    char_u code1;
}
ਅਚਲ special_ਕੁੰਜੀਆਂ[] =
{
    {GDK_ਕੁੰਜੀ_ਉੱਪਰ,	'k', 'u'},
    {GDK_ਕੁੰਜੀ_ਥੱਲੇ,	'k', 'd'},
    {GDK_ਕੁੰਜੀ_ਖੱਬਾ,	'k', 'l'},
    {GDK_ਕੁੰਜੀ_ਸੱਜਾ,	'k', 'r'},
    {GDK_ਕੁੰਜੀ_F1,	'k', '1'},
    {GDK_ਕੁੰਜੀ_F2,	'k', '2'},
    {GDK_ਕੁੰਜੀ_F3,	'k', '3'},
    {GDK_ਕੁੰਜੀ_F4,	'k', '4'},
    {GDK_ਕੁੰਜੀ_F5,	'k', '5'},
    {GDK_ਕੁੰਜੀ_F6,	'k', '6'},
    {GDK_ਕੁੰਜੀ_F7,	'k', '7'},
    {GDK_ਕੁੰਜੀ_F8,	'k', '8'},
    {GDK_ਕੁੰਜੀ_F9,	'k', '9'},
    {GDK_ਕੁੰਜੀ_F10,	'k', ';'},
    {GDK_ਕੁੰਜੀ_F11,	'F', '1'},
    {GDK_ਕੁੰਜੀ_F12,	'F', '2'},
    {GDK_ਕੁੰਜੀ_Help,	'%', '1'},
    {GDK_ਕੁੰਜੀ_Undo,	'&', '8'},
    {GDK_ਕੁੰਜੀ_ਪਿੱਠ_Space,	'k', 'b'},
    {GDK_ਕੁੰਜੀ_ਪਾਓ,	'k', 'I'},
    {GDK_ਕੁੰਜੀ_Delete,	'k', 'D'},
    {GDK_ਕੁੰਜੀ_Home,	'k', 'h'},
    {GDK_ਕੁੰਜੀ_ਸਿਰਾ,	'@', '7'},
    {GDK_ਕੁੰਜੀ_ਪੂਰਵ,	'k', 'P'},
    {GDK_ਕੁੰਜੀ_ਅਗਲਾ,	'k', 'N'},
    {GDK_ਕੁੰਜੀ_Print,	'%', '9'},
    {GDK_ਕੁੰਜੀ_KP_ਖੱਬਾ,	'k', 'l'},
    {GDK_ਕੁੰਜੀ_KP_ਸੱਜਾ,	'k', 'r'},
    {GDK_ਕੁੰਜੀ_KP_ਉੱਪਰ,	'k', 'u'},
    {GDK_ਕੁੰਜੀ_KP_ਥੱਲੇ,	'k', 'd'},
    {GDK_ਕੁੰਜੀ_KP_ਪਾਓ,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_ਕੁੰਜੀ_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_ਕੁੰਜੀ_KP_Home,	'K', '1'},
    {GDK_ਕੁੰਜੀ_KP_ਸਿਰਾ,	'K', '4'},
    {GDK_ਕੁੰਜੀ_KP_ਪੂਰਵ,	'K', '3'},
    {GDK_ਕੁੰਜੀ_KP_ਅਗਲਾ,	'K', '5'},
    {GDK_ਕੁੰਜੀ_KP_ਜੋੜੋ,	'K', '6'},
    {GDK_ਕੁੰਜੀ_KP_ਘਟਾਓ, 'K', '7'},
    {GDK_ਕੁੰਜੀ_KP_Divide,	'K', '8'},
    {GDK_ਕੁੰਜੀ_KP_Multiply, 'K', '9'},
    {GDK_ਕੁੰਜੀ_KP_Enter,	'K', 'A'},
    {GDK_ਕੁੰਜੀ_KP_Decimal,	'K', 'B'},
    {GDK_ਕੁੰਜੀ_KP_0,	'K', 'C'},
    {GDK_ਕੁੰਜੀ_KP_1,	'K', 'D'},
    {GDK_ਕੁੰਜੀ_KP_2,	'K', 'E'},
    {GDK_ਕੁੰਜੀ_KP_3,	'K', 'F'},
    {GDK_ਕੁੰਜੀ_KP_4,	'K', 'G'},
    {GDK_ਕੁੰਜੀ_KP_5,	'K', 'H'},
    {GDK_ਕੁੰਜੀ_KP_6,	'K', 'I'},
    {GDK_ਕੁੰਜੀ_KP_7,	'K', 'J'},
    {GDK_ਕੁੰਜੀ_KP_8,	'K', 'K'},
    {GDK_ਕੁੰਜੀ_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    ਸਥਿਰ ਪੂਰਨ
keyval_ਮੰਜ਼ਿਲ_string(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ keyval, char_u *string)
{
    ਪੂਰਨ		len;
    guint32	uc;

    uc = gdk_keyval_ਮੰਜ਼ਿਲ_unicode(keyval);
    ਜੇ (uc != 0)
    {
	len = utf_char2bytes((ਪੂਰਨ)uc, string);
    }
    ਨਹੀਂਤਾਂ
    {
	len = 1;
	ਚੋਣ (keyval)
	{
	    ਹਾਲਤ GDK_ਕੁੰਜੀ_Tab: ਹਾਲਤ GDK_ਕੁੰਜੀ_KP_Tab: ਹਾਲਤ GDK_ਕੁੰਜੀ_ISO_ਖੱਬਾ_Tab:
		string[0] = TAB;
		ਰੋਕੋ;
	    ਹਾਲਤ GDK_ਕੁੰਜੀ_Linefeed:
		string[0] = NL;
		ਰੋਕੋ;
	    ਹਾਲਤ GDK_ਕੁੰਜੀ_Return: ਹਾਲਤ GDK_ਕੁੰਜੀ_ISO_Enter: ਹਾਲਤ GDK_ਕੁੰਜੀ_3270_Enter:
		string[0] = CAR;
		ਰੋਕੋ;
	    ਹਾਲਤ GDK_ਕੁੰਜੀ_Escape:
		string[0] = ESC;
		ਰੋਕੋ;
	    ਮੂਲ:
		len = 0;
		ਰੋਕੋ;
	}
    }
    string[len] = NUL;
    ਵਾਪਸ len;
}

    ਸਥਿਰ ਪੂਰਨ
modifiers_gdk2vim(guint state)
{
    ਪੂਰਨ modifiers = 0;

    ਜੇ (state & GDK_ਖਿਸਕਾਓ_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
    ਜੇ (state & GDK_CONTROL_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_CTRL;
    ਜੇ (state & GDK_ALT_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_ALT;
    ਜੇ (state & GDK_META_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_META;
    ਜੇ (state & GDK_SUPER_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_CMD;

    ਵਾਪਸ modifiers;
}

ਸਥਿਰ GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
ਸਥਿਰ gboolean ਕੁੰਜੀ_press_event(Gtk_Event_Controller_ਕੁੰਜੀ *controller, guint keyval, guint keycode, GdkModifierType state, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ ਕੁੰਜੀ_release_event(Gtk_Event_Controller_ਕੁੰਜੀ *controller, guint keyval, guint keycode, GdkModifierType state, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ button_press_event(GtkGestureClick *gesture, ਪੂਰਨ n_press, ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ button_release_event(GtkGestureClick *gesture, ਪੂਰਨ n_press, ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ motion_notify_event(GtkEventControllerMotion *controller, ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ enter_notify_event(GtkEventControllerMotion *controller, ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, gpointer ਅੰਕੜੇ);
ਸਥਿਰ gboolean scroll_event(GtkEventControllerScroll *controller, ਦੁੱਗਣਾ dx, ਦੁੱਗਣਾ dy, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ focus_ਵਿੱਚ_event(GtkEventControllerFocus *controller, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ focus_ਬਾਹਰਲਾ_event(GtkEventControllerFocus *controller, gpointer ਅੰਕੜੇ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
ਸਥਿਰ gboolean drop_cb(Gtk_Drop_ਨਿਸ਼ਾਨਾ_Async *ਨਿਸ਼ਾਨਾ, GdkDrop *drop, ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, ਖਾਲੀ *ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
ਸਥਿਰ ਖਾਲੀ tabline_enter_cb(GtkEventController *controller, ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, ਖਾਲੀ *udata);
ਸਥਿਰ ਖਾਲੀ on_select_tab(GtkNotebook *notebook, gpointer *ਪੰਨਾ, gint idx, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ on_tab_reordered(GtkNotebook *notebook, gpointer *ਪੰਨਾ, gint idx, gpointer ਅੰਕੜੇ);
ਸਥਿਰ VimMenu *create_tabline_popup_menu(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ tabline_ਜੋੜੋ_css(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ tabline_menu_press_event(GtkGestureClick *gesture, ਪੂਰਨ n_press, ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, GtkWidget *popover);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ mainwin_destroy_cb(GObject *object, gpointer ਅੰਕੜੇ);
ਸਥਿਰ gboolean delete_event_cb(Gtk_ਖਿੜਕੀ *ਖਿੜਕੀ_2, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਪੂਰਨ query_ਸੂਚਕ_pos(ਪੂਰਨ *x, ਪੂਰਨ *y, GdkModifierType *state);
ਸਥਿਰ ਖਾਲੀ mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_form_ਮਾਪ(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ);
ਸਥਿਰ ਖਾਲੀ drawarea_realize_cb(GtkWidget *widget, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ drawarea_unrealize_cb(GtkWidget *widget, gpointer ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ clipboard_ਬਦਲਿਆ_cb(GdkClipboard *clipboard, gpointer user_ਅੰਕੜੇ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
ਸਥਿਰ ਖਾਲੀ show_menubar_popover(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *prgname = ਨਿਰਾ;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    ਸਥਿਰ gboolean
arg_ਮੇਲ(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ਮੁੱਲ)
{
    size_t  len = ਲੜੀ_ਲੰਬਾਈ(name);

    ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, name, len) != 0)
	ਵਾਪਸ FALSE;
    ਜੇ (s[len] == '=')
	*ਮੁੱਲ = (ਅੱਖਰ *)s + len + 1;
    ਨਹੀਂਤਾਂ ਜੇ (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ਖਾਲੀ
gui_mch_prepare(ਪੂਰਨ *argc, ਅੱਖਰ **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    ਪੂਰਨ	i = 0;

    ਜਦੋਂ (i < *argc)
    {
	ਅੱਖਰ	*s = argv[i];
	ਅੱਖਰ	*ਮੁੱਲ = ਨਿਰਾ;
	ਪੂਰਨ	has_inline_ਮੁੱਲ = FALSE;
	ਪੂਰਨ	n_strip;

	ਜੇ (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    ਜਾਰੀ;
	}

	ਜੇ (strchr(s, '=') != ਨਿਰਾ)
	    has_inline_ਮੁੱਲ = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	ਜੇ (!has_inline_ਮੁੱਲ)
	{
	    ਜੇ (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		ਮੁੱਲ = argv[i + 1];
	    ਨਹੀਂਤਾਂ
		ਮੁੱਲ = ਨਿਰਾ;
	}

	ਜੇ (arg_ਮੇਲ(s, "-fn", &ਮੁੱਲ) || arg_ਮੇਲ(s, "-font", &ਮੁੱਲ))
	    font_ਦਲੀਲ = ਮੁੱਲ;
	ਨਹੀਂਤਾਂ ਜੇ (arg_ਮੇਲ(s, "-geom", &ਮੁੱਲ)
		|| arg_ਮੇਲ(s, "-geometry", &ਮੁੱਲ))
	{
	    ਜੇ (ਮੁੱਲ != ਨਿਰਾ)
		gui.geom = vim_strsave((char_u *)ਮੁੱਲ);
	}
	ਨਹੀਂਤਾਂ ਜੇ (arg_ਮੇਲ(s, "-bg", &ਮੁੱਲ)
		|| arg_ਮੇਲ(s, "-background", &ਮੁੱਲ))
	    background_ਦਲੀਲ = ਮੁੱਲ;
	ਨਹੀਂਤਾਂ ਜੇ (arg_ਮੇਲ(s, "-fg", &ਮੁੱਲ)
		|| arg_ਮੇਲ(s, "-foreground", &ਮੁੱਲ))
	    foreground_ਦਲੀਲ = ਮੁੱਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_ਮੁੱਲ = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ ਜੇ (arg_ਮੇਲ(s, "--prg-name", &ਮੁੱਲ))
	    // GTK4 specific
	    prgname = ਮੁੱਲ;
	ਨਹੀਂਤਾਂ
	{
	    i++;
	    ਜਾਰੀ;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	ਜੇ (ਮੁੱਲ != ਨਿਰਾ && !has_inline_ਮੁੱਲ)
	    n_strip = 2;

	ਜੇ (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    ਜੇ (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * ਆਕਾਰ(ਅੱਖਰ *));
	    argv[*argc] = ਨਿਰਾ;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    ਖਾਲੀ
gui_mch_free_all(ਖਾਲੀ)
{
}

    ਸਥਿਰ guint
timeout_ਜੋੜੋ(ਪੂਰਨ ਸਮਾਂ, timeout_cb_type (*callback)(gpointer), ਪੂਰਨ *flagp)
{
    ਵਾਪਸ g_timeout_ਜੋੜੋ((guint)ਸਮਾਂ, (GSourceFunc)callback, flagp);
}

    ਸਥਿਰ ਖਾਲੀ
timeout_remove(guint timer)
{
    g_ਸਰੋਤ_remove(timer);
}

ਸਥਿਰ long_u blink_waittime = 700;
ਸਥਿਰ long_u blink_ontime = 400;
ਸਥਿਰ long_u blink_offtime = 250;
ਸਥਿਰ guint blink_timer = 0;

    ਸਥਿਰ timeout_cb_type
blink_cb(gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_ਜੋੜੋ(blink_offtime, blink_cb, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_ਜੋੜੋ(blink_ontime, blink_cb, ਨਿਰਾ);
    }
    ਵਾਪਸ FALSE;
}

    ਪੂਰਨ
gui_mch_is_blinking(ਖਾਲੀ)
{
    ਵਾਪਸ blink_state != BLINK_NONE;
}

    ਪੂਰਨ
gui_mch_is_blink_off(ਖਾਲੀ)
{
    ਵਾਪਸ blink_state == BLINK_OFF;
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_blinking(ਲੰਮਾ waittime, ਲੰਮਾ on, ਲੰਮਾ off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    ਖਾਲੀ
gui_mch_stop_blink(ਪੂਰਨ may_call_gui_update_cursor)
{
    ਜੇ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ਜੇ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ਖਾਲੀ
gui_mch_ਸ਼ੁਰੂ_blink(ਖਾਲੀ)
{
    ਜੇ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ਜੇ (blink_waittime && blink_ontime && blink_offtime && gui.ਵਿੱਚ_focus)
    {
	blink_timer = timeout_ਜੋੜੋ(blink_waittime, blink_cb, ਨਿਰਾ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    ਪੂਰਨ
gui_mch_early_init_check(ਪੂਰਨ give_ਸੁਨੇਹਾ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ OK;
}

    ਪੂਰਨ
gui_mch_init_check(ਖਾਲੀ)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_ਸੈੱਟ_prgname(prgname == ਨਿਰਾ ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    ਜੇ (g_getenv("EGL_LOG_LEVEL") == ਨਿਰਾ)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    ਵਾਪਸ OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ਪੂਰਨ
gui_mch_init(ਖਾਲੀ)
{
    // Allocate GdkRGBA color structs.
    gui.fgcolor = g_new0(GdkRGBA, 1);
    gui.bgcolor = g_new0(GdkRGBA, 1);
    gui.spcolor = g_new0(GdkRGBA, 1);

    gui.def_norm_pixel = 0x00000000;	// black
    gui.def_ਪਿੱਠ_pixel = 0x00ffffff;	// white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.ਪਿੱਠ_pixel = gui.def_ਪਿੱਠ_pixel;

    gui.scrollbar_ਚੌੜਾਈ = SB_DEFAULT_ਚੌੜਾਈ;
    gui.scrollbar_ਉਚਾਈ = SB_DEFAULT_ਚੌੜਾਈ;

    // Create the main window.
    gui.mainwin = gtk_ਖਿੜਕੀ_new();
    gtk_widget_ਸੈੱਟ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.ਪਾਠ_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_ਸੈੱਟ_ਅਧਾਰ_dir(gui.ਪਾਠ_context, PANGO_DIRECTION_LTR);

    g_signal_connect(G_OBJECT(gui.mainwin), "close-request",
		     G_CALLBACK(delete_event_cb), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), ਨਿਰਾ);

    // A vertical box holds the menubar, toolbar and main text window.
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_ਸੈੱਟ_homogeneous(GTK_BOX(vbox), FALSE);
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_ਬੱਚਾ(GTK_ਖਿੜਕੀ(gui.mainwin), vbox);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    gui.menubar = vim_menu_bar_new();
    gtk_widget_ਸੈੱਟ_name(gui.menubar, "vim-menubar");
    gtk_widget_ਸੈੱਟ_visible(gui.menubar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.menubar);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    gui.toolbar = vim_toolbar_new();
    gtk_widget_ਸੈੱਟ_name(gui.toolbar, "vim-toolbar");
    gtk_widget_ਸੈੱਟ_visible(gui.toolbar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.toolbar);
    vim_toolbar_ਸੈੱਟ_style(VIM_TOOLBAR(gui.toolbar), toolbar_flags, tbis_flags);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    gui.tabline = gtk_notebook_new();
    gtk_notebook_ਸੈੱਟ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_ਸੈੱਟ_show_tabs(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_notebook_ਸੈੱਟ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_widget_ਜੋੜੋ_css_class(gui.tabline, "vim-tabline");
    tabline_ਜੋੜੋ_css();
    gtk_widget_ਸੈੱਟ_visible(gui.tabline, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.tabline);

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ਨਿਰਾ);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), ਨਿਰਾ);

	// Make sure that tabline_enter_cb() is always called before
	// tabpage_enter_cb().
	gtk_event_controller_ਸੈੱਟ_ਪ੍ਰਸਾਰ_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	gtk_widget_ਜੋੜੋ_controller(gui.tabline, mcontroller);
    }

    // Create right click popup menu for tabline
    {
	GtkGesture	*click;
	VimMenu		*menu;

	click = gtk_gesture_click_new();
	menu = create_tabline_popup_menu();

	gtk_widget_ਸੈੱਟ_ਮਾਪਾ(GTK_WIDGET(menu), gui.tabline);
	g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(gui.tabline), "menu", menu);

	gtk_popover_ਸੈੱਟ_has_arrow(GTK_POPOVER(menu), FALSE);
	gtk_popover_ਸੈੱਟ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
	// Make popover start at top left corner
	gtk_widget_ਸੈੱਟ_halign(GTK_WIDGET(menu), GTK_ALIGN_ਸ਼ੁਰੂ);

	// Listen for anny mouse button
	gtk_gesture_single_ਸੈੱਟ_button(GTK_GESTURE_SINGLE(click), 0);

	g_signal_connect_object(click, "pressed",
		G_CALLBACK(tabline_menu_press_event),
		menu, G_CONNECT_DEFAULT);
	gtk_widget_ਜੋੜੋ_controller(gui.tabline, GTK_EVENT_CONTROLLER(click));
    }
#ਜੇ_ਅੰਤ

    // The form widget manages absolute positioning of scrollbars and the draw
    // area.
    gui.formwin = vim_form_new();
    gtk_widget_ਸੈੱਟ_name(gui.formwin, "vim-gtk-form");
    gtk_widget_ਸੈੱਟ_vexpand(gui.formwin, TRUE);
    gtk_widget_ਸੈੱਟ_hexpand(gui.formwin, TRUE);
    gtk_box_append(GTK_BOX(vbox), gui.formwin);

    // The drawing area for the editor content.
    gui.drawarea = vim_draw_area_new();
    gtk_widget_ਸੈੱਟ_focusable(gui.drawarea, TRUE);
    gtk_widget_ਸੈੱਟ_vexpand(gui.drawarea, TRUE);
    gtk_widget_ਸੈੱਟ_hexpand(gui.drawarea, TRUE);
    vim_form_put(VIM_FORM(gui.formwin), gui.drawarea, 0, 0);

    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ਨਿਰਾ);

    // Set up event controllers.
    {
	GtkEventController *ਕੁੰਜੀ_ctrl = gtk_event_controller_ਕੁੰਜੀ_new();
	g_signal_connect(ਕੁੰਜੀ_ctrl, "key-pressed",
			 G_CALLBACK(ਕੁੰਜੀ_press_event), ਨਿਰਾ);
	g_signal_connect(ਕੁੰਜੀ_ctrl, "key-released",
			 G_CALLBACK(ਕੁੰਜੀ_release_event), ਨਿਰਾ);
	gtk_widget_ਜੋੜੋ_controller(gui.drawarea, ਕੁੰਜੀ_ctrl);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
	xim_init();
#ਜੇ_ਅੰਤ
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_ਸੈੱਟ_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), ਨਿਰਾ);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), ਨਿਰਾ);
	gtk_widget_ਜੋੜੋ_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), ਨਿਰਾ);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), ਨਿਰਾ);
	gtk_widget_ਜੋੜੋ_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), ਨਿਰਾ);
	gtk_widget_ਜੋੜੋ_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_ਵਿੱਚ_event), ਨਿਰਾ);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_ਬਾਹਰਲਾ_event), ਨਿਰਾ);
	gtk_widget_ਜੋੜੋ_controller(gui.drawarea, focus);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_ਨਿਸ਼ਾਨਾ = gtk_drop_ਨਿਸ਼ਾਨਾ_async_new(ਨਿਰਾ,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_ਸੈੱਟ_dnd_targets();
	g_signal_connect(gui.drop_ਨਿਸ਼ਾਨਾ, "drop",
			 G_CALLBACK(drop_cb), ਨਿਰਾ);
	gtk_widget_ਜੋੜੋ_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_ਨਿਸ਼ਾਨਾ));
    }
#ਜੇ_ਅੰਤ

    gui.border_ਵਿੱਥ = gui.border_ਚੌੜਾਈ;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_ਸੂਚਕ = gdk_cursor_new_from_name("none", ਨਿਰਾ);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	ਜੇ (primary != ਨਿਰਾ)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_ਬਦਲਿਆ_cb), &clip_ਤਾਰਾ);
	ਜੇ (regular != ਨਿਰਾ)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_ਬਦਲਿਆ_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_ਤਾਰਾ);

    ਵਾਪਸ OK;
}

    ਖਾਲੀ
gui_mch_new_colors(ਖਾਲੀ)
{
    ਜੇ (gui.drawarea != ਨਿਰਾ && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ਪੂਰਨ
gui_mch_open(ਖਾਲੀ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_ਚੌੜਾਈ;
    guint pixel_ਉਚਾਈ;

    ਜੇ (gui.geom != ਨਿਰਾ)
    {
	ਪੂਰਨ		ਨਕਾਬ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	w, h;

	ਨਕਾਬ = vim_parse_geometry((ਅੱਖਰ *)gui.geom, &w, &h);

	ਜੇ (ਨਕਾਬ & ਚੌੜਾਈ_ਮੁੱਲ)
	    Columns = w;
	ਜੇ (ਨਕਾਬ & ਉਚਾਈ_ਮੁੱਲ)
	{
	    ਜੇ (p_ਖਿੜਕੀ > (ਲੰਮਾ)h - 1 || !option_was_ਸੈੱਟ((char_u *)"window"))
		p_ਖਿੜਕੀ = h - 1;
	    ਪੰਕਤੀਆਂ = h;
	}
	ਹੱਦਬੰਦੀ_screen_ਮਾਪ();

	VIM_CLEAR(gui.geom);
    }
    ਨਹੀਂਤਾਂ
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	ਜੇ (Columns > 80)
	    Columns = 80;
	ਜੇ (ਪੰਕਤੀਆਂ > 24)
	    ਪੰਕਤੀਆਂ = 24;
    }

    pixel_ਚੌੜਾਈ = (guint)(gui_get_ਅਧਾਰ_ਚੌੜਾਈ() + Columns * gui.char_ਚੌੜਾਈ);
    pixel_ਉਚਾਈ = (guint)(gui_get_ਅਧਾਰ_ਉਚਾਈ() + ਪੰਕਤੀਆਂ * gui.char_ਉਚਾਈ);

    pixel_ਚੌੜਾਈ  += get_menu_tool_ਚੌੜਾਈ();
    pixel_ਉਚਾਈ += get_menu_tool_ਉਚਾਈ();

    ਜੇ (foreground_ਦਲੀਲ != ਨਿਰਾ)
	fg_pixel = gui_get_color((char_u *)foreground_ਦਲੀਲ);
    ਜੇ (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    ਜੇ (background_ਦਲੀਲ != ਨਿਰਾ)
	bg_pixel = gui_get_color((char_u *)background_ਦਲੀਲ);
    ਜੇ (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    ਜੇ (ਮਿਲਿਆ_ਉਲਟ_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_ਪਿੱਠ_pixel = fg_pixel;
    }
    ਨਹੀਂਤਾਂ
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_ਪਿੱਠ_pixel = bg_pixel;
    }

    ਸੈੱਟ_ਸਾਧਾਰਨ_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ਨਿਰਾ);
    // Resize is handled by GtkForm's size_allocate callback.

    ਸੈੱਟ_form_ਮਾਪ((ਪੂਰਨ)pixel_ਚੌੜਾਈ - get_menu_tool_ਚੌੜਾਈ(),
	    (ਪੂਰਨ)pixel_ਉਚਾਈ - get_menu_tool_ਉਚਾਈ());

    gtk_ਖਿੜਕੀ_present(GTK_ਖਿੜਕੀ(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    ਵਾਪਸ OK;
}

    ਖਾਲੀ
gui_mch_exit(ਪੂਰਨ rc ਅਣਵਰਤਿਆ)
{
    ਜੇ (gui.mainwin != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	// Must unparent popover menu for tabline or we will get warning
	gtk_widget_unparent(g_object_get_ਅੰਕੜੇ(G_OBJECT(gui.tabline), "menu"));
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
	// Make sure to destroy popover used for balloon eval, or we will get a
	// warning from GTK that the draw area still has children left.
	gui_mch_destroy_beval_area(balloonEval);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	// Make sure to unparent any popover menus
	{
	    vimmenu_T *menu;

	    FOR_ALL_MENUS(menu)
	    {
		ਜੇ ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != ਨਿਰਾ)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#ਜੇ_ਅੰਤ
	gtk_ਖਿੜਕੀ_destroy(GTK_ਖਿੜਕੀ(gui.mainwin));
    }
}

    ਪੂਰਨ
gui_mch_get_winpos(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    ਵਾਪਸ FAIL;
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_winpos(ਪੂਰਨ x ਅਣਵਰਤਿਆ, ਪੂਰਨ y ਅਣਵਰਤਿਆ)
{
    // GTK4/Wayland: window positioning not available
}

    ਪੂਰਨ
gui_mch_maximized(ਖਾਲੀ)
{
    ਵਾਪਸ gtk_ਖਿੜਕੀ_is_maximized(GTK_ਖਿੜਕੀ(gui.mainwin));
}

    ਖਾਲੀ
gui_mch_unmaximize(ਖਾਲੀ)
{
    ਜੇ (gui.mainwin != ਨਿਰਾ)
	gtk_ਖਿੜਕੀ_unmaximize(GTK_ਖਿੜਕੀ(gui.mainwin));
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_fullscreen(ਪੂਰਨ flag)
{
    ਜੇ (gui.mainwin == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (flag)
	gtk_ਖਿੜਕੀ_fullscreen(GTK_ਖਿੜਕੀ(gui.mainwin));
    ਨਹੀਂਤਾਂ
	gtk_ਖਿੜਕੀ_unfullscreen(GTK_ਖਿੜਕੀ(gui.mainwin));
}

    ਸਥਿਰ ਖਾਲੀ
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec ਅਣਵਰਤਿਆ, gpointer user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    ਜੇ (gtk_ਖਿੜਕੀ_is_fullscreen(GTK_ਖਿੜਕੀ(obj)))
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
    ਖਾਲੀ
gui_mch_newfont(ਖਾਲੀ)
{
    ਪੂਰਨ w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_ਚੌੜਾਈ(gui.formwin);
    h = gtk_widget_get_ਉਚਾਈ(gui.formwin);
    gui_resize_shell(w, h);
}

    ਖਾਲੀ
gui_mch_settitle(char_u *title, char_u *icon ਅਣਵਰਤਿਆ)
{
    ਜੇ (title != ਨਿਰਾ && gui.mainwin != ਨਿਰਾ)
	gtk_ਖਿੜਕੀ_ਸੈੱਟ_title(GTK_ਖਿੜਕੀ(gui.mainwin), (ਅਚਲ ਅੱਖਰ *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    ਖਾਲੀ
gui_gtk_init_decor_ਉਚਾਈ(ਖਾਲੀ)
{
    ਪੂਰਨ h = gtk_widget_get_ਉਚਾਈ(gui.mainwin);

    ਜੇ (h == 0)
	ਵਾਪਸ;

    h -= get_menu_tool_ਉਚਾਈ();
    h -= gtk_widget_get_ਉਚਾਈ(gui.formwin);

    gui.decor_ਉਚਾਈ = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_form_ਮਾਪ(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    ਜੇ (gtk_widget_get_ਚੌੜਾਈ(gui.formwin) == ਚੌੜਾਈ
	    && gtk_widget_get_ਉਚਾਈ(gui.formwin) == ਉਚਾਈ)
	ਵਾਪਸ;

    // Remember the size the form widget is supposed to get. An allocation
    // that arrives before the compositor has answered this request still has
    // the previous size and must not be used.
    gui.pending_form_w = ਚੌੜਾਈ;
    gui.pending_form_h = ਉਚਾਈ;
    gui.pending_form_skip = 1;

    // GtkWindow default size also includes client side decorations, so must
    // include it also.  It also keeps the natural width of the toolbar from
    // deciding the width.
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_default_ਮਾਪ(GTK_ਖਿੜਕੀ(gui.mainwin),
	    ਚੌੜਾਈ + get_menu_tool_ਚੌੜਾਈ(),
	    ਉਚਾਈ + get_menu_tool_ਉਚਾਈ() + gui.decor_ਉਚਾਈ);

    // The window drops a request made while it is being presented, and
    // "gui.decor_height" is not known before that.  A size request on the form
    // widget is honoured then; it is dropped again once the size was given.
    gtk_widget_ਸੈੱਟ_ਮਾਪ_request(gui.formwin, ਚੌੜਾਈ, ਉਚਾਈ);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_shellsize(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ,
	ਪੂਰਨ min_ਚੌੜਾਈ ਅਣਵਰਤਿਆ, ਪੂਰਨ min_ਉਚਾਈ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ ਅਧਾਰ_ਚੌੜਾਈ ਅਣਵਰਤਿਆ, ਪੂਰਨ ਅਧਾਰ_ਉਚਾਈ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ direction ਅਣਵਰਤਿਆ)
{
    ਸੈੱਟ_form_ਮਾਪ(ਚੌੜਾਈ, ਉਚਾਈ);

    gui_mch_update();
}

    ਖਾਲੀ
gui_mch_get_screen_dimensions(ਪੂਰਨ *screen_w, ਪੂਰਨ *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    ਜੇ (surface != ਨਿਰਾ)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	ਜੇ (monitor != ਨਿਰਾ)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.ਚੌੜਾਈ;
	    *screen_h = geom.ਉਚਾਈ;
	    ਵਾਪਸ;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਖਾਲੀ
gui_mch_enable_menu(ਪੂਰਨ showit)
{
    ਜੇ (gui.menubar != ਨਿਰਾ)
    {
	gtk_widget_ਸੈੱਟ_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	ਜੇ (gui.drawarea != ਨਿਰਾ)
	    gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਖਾਲੀ
gui_mch_show_toolbar(ਪੂਰਨ showit)
{
    ਜੇ (gui.toolbar != ਨਿਰਾ)
    {
	gtk_widget_ਸੈੱਟ_visible(gui.toolbar, showit);
	ਜੇ (showit)
	    vim_toolbar_ਸੈੱਟ_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	ਜੇ (gui.drawarea != ਨਿਰਾ)
	    gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
    }
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_dark_theme(ਪੂਰਨ dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    ਜੇ (settings != ਨਿਰਾ)
	g_object_ਸੈੱਟ(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, ਨਿਰਾ);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    ਪੂਰਨ
gui_mch_adjust_charheight(ਖਾਲੀ)
{
    PangoFontMetrics *metrics;
    ਪੂਰਨ ascent;
    ਪੂਰਨ descent;

    metrics = pango_context_get_metrics(gui.ਪਾਠ_context, gui.norm_font,
			    pango_context_get_language(gui.ਪਾਠ_context));
    ascent = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);
    pango_font_metrics_unref(metrics);

    gui.char_ਉਚਾਈ = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_ਉਚਾਈ = MAX(gui.char_ਉਚਾਈ, gui.char_ascent + 1);

    ਵਾਪਸ OK;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    PangoFontDescription    *ਨਤੀਜਾ;
    gboolean		    ਪੂਰਾ;
} Font_Dialog_ਅੰਕੜੇ;

    ਸਥਿਰ ਖਾਲੀ
font_dialog_ਅੰਤ_cb(GObject *ਸਰੋਤ, GAsync_ਨਤੀਜਾ *res, gpointer ਅੰਕੜੇ)
{
    Font_Dialog_ਅੰਕੜੇ *fdd = (Font_Dialog_ਅੰਕੜੇ *)ਅੰਕੜੇ;
    fdd->ਨਤੀਜਾ = gtk_font_dialog_ਚੁਣੋ_font_ਅੰਤ(
		    GTK_FONT_DIALOG(ਸਰੋਤ), res, ਨਿਰਾ);
    fdd->ਪੂਰਾ = TRUE;
}

    ਸਥਿਰ gboolean
font_filter(gpointer ਵਸਤੂ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (PANGO_IS_FONT_FAMILY(ਵਸਤੂ))
	ਵਾਪਸ pango_font_family_is_monospace(PANGO_FONT_FAMILY(ਵਸਤੂ));
    ਜੇ (PANGO_IS_FONT_FACE(ਵਸਤੂ))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(ਵਸਤੂ));
	ਜੇ (family != ਨਿਰਾ)
	    ਵਾਪਸ pango_font_family_is_monospace(family);
    }
    ਵਾਪਸ TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = ਨਿਰਾ;
    char_u		*fontname = ਨਿਰਾ;
    Font_Dialog_ਅੰਕੜੇ	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_ਸੈੱਟ_modal(dlg, TRUE);
    gtk_font_dialog_ਸੈੱਟ_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, ਨਿਰਾ, ਨਿਰਾ)));

    ਜੇ (oldval != ਨਿਰਾ && oldval[0] != NUL)
    {
	char_u *oldname;

	ਜੇ (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    oldname = oldval;

	ਜੇ (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    ਜੇ (p != ਨਿਰਾ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		ਜੇ (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((ਅਚਲ ਅੱਖਰ *)oldname);
	ਜੇ (oldname != oldval)
	    vim_free(oldname);
    }
    ਨਹੀਂਤਾਂ
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.ਨਤੀਜਾ = ਨਿਰਾ;
    fdd.ਪੂਰਾ = FALSE;

    gtk_font_dialog_ਚੁਣੋ_font(dlg, GTK_ਖਿੜਕੀ(gui.mainwin),
	    initial, ਨਿਰਾ, font_dialog_ਅੰਤ_cb, &fdd);

    ਜਦੋਂ (!fdd.ਪੂਰਾ)
	g_main_context_iteration(ਨਿਰਾ, TRUE);

    ਜੇ (fdd.ਨਤੀਜਾ != ਨਿਰਾ)
    {
	ਅੱਖਰ *name = pango_font_description_ਮੰਜ਼ਿਲ_string(fdd.ਨਤੀਜਾ);
	ਜੇ (name != ਨਿਰਾ)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    ਜੇ (p != ਨਿਰਾ && ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&ਨਿਵੇਸ਼_conv, p, ਨਿਰਾ);
		vim_free(p);
	    }
	    ਨਹੀਂਤਾਂ
		fontname = p;
	}
	pango_font_description_free(fdd.ਨਤੀਜਾ);
    }

    ਜੇ (initial != ਨਿਰਾ)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    ਵਾਪਸ fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    ਸਥਿਰ ਖਾਲੀ
ascii_glyph_ਸਾਰਣੀ_init(ਖਾਲੀ)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *ਵਸਤੂ_list;
    ਪੂਰਨ		    i;

    ਜੇ (gui.ascii_glyphs != ਨਿਰਾ)
	pango_glyph_string_free(gui.ascii_glyphs);
    ਜੇ (gui.ascii_font != ਨਿਰਾ)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ਨਿਰਾ;
    gui.ascii_font   = ਨਿਰਾ;

    ਲਈ (i = 0; i < 128; ++i)
    {
	ਜੇ (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	ਨਹੀਂਤਾਂ
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    ਵਸਤੂ_list = pango_itemize(gui.ਪਾਠ_context, (ਅਚਲ ਅੱਖਰ *)ascii_chars,
			      0, ਆਕਾਰ(ascii_chars), attr_list, ਨਿਰਾ);

    ਜੇ (ਵਸਤੂ_list != ਨਿਰਾ && ਵਸਤੂ_list->ਅਗਲਾ == ਨਿਰਾ)
    {
	Pango_ਵਸਤੂ   *ਵਸਤੂ;
	ਪੂਰਨ	    ਚੌੜਾਈ;

	ਵਸਤੂ  = (Pango_ਵਸਤੂ *)ਵਸਤੂ_list->ਅੰਕੜੇ;
	ਚੌੜਾਈ = gui.char_ਚੌੜਾਈ * PANGO_SCALE;

	gui.ascii_font = ਵਸਤੂ->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ਅਚਲ ਅੱਖਰ *)ascii_chars, ਆਕਾਰ(ascii_chars),
		    &ਵਸਤੂ->analysis, gui.ascii_glyphs);

	ਜੇ (gui.ascii_glyphs->ਅੰਕ_glyphs == (ਪੂਰਨ)ਆਕਾਰ(ascii_chars))
	{
	    ਲਈ (i = 0; i < gui.ascii_glyphs->ਅੰਕ_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_ਵਿੱਥ += MAX(0, ਚੌੜਾਈ - geom->ਚੌੜਾਈ) / 2;
		geom->ਚੌੜਾਈ = ਚੌੜਾਈ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = ਨਿਰਾ;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = ਨਿਰਾ;
	}
    }

    g_list_foreach(ਵਸਤੂ_list, (GFunc)(ਖਾਲੀ *)&pango_ਵਸਤੂ_free, ਨਿਰਾ);
    g_list_free(ਵਸਤੂ_list);
    pango_attr_list_unref(attr_list);
}

    ਸਥਿਰ ਖਾਲੀ
get_styled_font_variants(ਖਾਲੀ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_ਲੱਦੋ_font(gui.ਪਾਠ_context, gui.norm_font);
    ਜੇ (plain_font == ਨਿਰਾ)
	ਵਾਪਸ;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_ਸੈੱਟ_ਭਾਰ(bold_font_desc, PANGO_ਭਾਰ_BOLD);

    bold_font = pango_context_ਲੱਦੋ_font(gui.ਪਾਠ_context, bold_font_desc);
    ਜੇ (bold_font != ਨਿਰਾ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    ਪੂਰਨ
gui_mch_init_font(char_u *font_name, ਪੂਰਨ fontset ਅਣਵਰਤਿਆ)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    ਪੂਰਨ			    ਚੌੜਾਈ;

    ਜੇ (font_name == ਨਿਰਾ)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    ਜੇ (font_desc == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_ਸੈੱਟ_font_description(gui.ਪਾਠ_context, font_desc);

    layout = pango_layout_new(gui.ਪਾਠ_context);
    pango_layout_ਸੈੱਟ_ਪਾਠ(layout, "MW", 2);
    pango_layout_get_ਮਾਪ(layout, &ਚੌੜਾਈ, ਨਿਰਾ);
    g_object_unref(layout);

    gui.char_ਚੌੜਾਈ = (ਚੌੜਾਈ / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    ਜੇ (gui.char_ਚੌੜਾਈ <= 0)
	gui.char_ਚੌੜਾਈ = 8;

    gui_mch_adjust_charheight();

    hl_ਸੈੱਟ_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_ਸਾਰਣੀ_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	ਪੂਰਨ im_ਪੰਕਤੀ = gui.ਪੰਕਤੀ < screen_ਪੰਕਤੀਆਂ ? gui.ਪੰਕਤੀ : screen_ਪੰਕਤੀਆਂ - 1;
	ਪੂਰਨ im_ਖਾਨਾ = gui.ਖਾਨਾ < screen_Columns ? gui.ਖਾਨਾ : screen_Columns - 1;
	im_ਸੈੱਟ_position(im_ਪੰਕਤੀ, im_ਖਾਨਾ);
    }
    ਵਾਪਸ OK;
}

    GuiFont
gui_mch_get_font(char_u *name, ਪੂਰਨ report_error)
{
    PangoFontDescription *font;

    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    font = pango_font_description_from_string((ਅਚਲ ਅੱਖਰ *)name);
    ਜੇ (font == ਨਿਰਾ)
    {
	ਜੇ (report_error)
	    semsg(_(e_unknown_font_str), name);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Ensure a size is set
    ਜੇ (pango_font_description_get_ਮਾਪ(font) <= 0)
	pango_font_description_ਸੈੱਟ_ਮਾਪ(font, 10 * PANGO_SCALE);

    ਵਾਪਸ font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name ਅਣਵਰਤਿਆ)
{
    ਜੇ (font != NOFONT)
    {
	ਅੱਖਰ *desc = pango_font_description_ਮੰਜ਼ਿਲ_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	ਵਾਪਸ ret;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

    ਖਾਲੀ
gui_mch_free_font(GuiFont font)
{
    ਜੇ (font != NOFONT)
	pango_font_description_free(font);
}

    ਖਾਲੀ
gui_mch_expand_font(
    optexpand_T	*ਦਲੀਲਾਂ,
    ਖਾਲੀ	*param,
    ਪੂਰਨ		(*ਜੋੜੋ_ਮੇਲ)(char_u *val))
{
    PangoFontFamily	**font_families = ਨਿਰਾ;
    ਪੂਰਨ			n_families = 0;
    ਪੂਰਨ			wide = *(ਪੂਰਨ *)param;

    ਜੇ (ਦਲੀਲਾਂ->oe_include_orig_val && *ਦਲੀਲਾਂ->oe_opt_ਮੁੱਲ == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	ਜੇ (ਜੋੜੋ_ਮੇਲ((char_u *)DEFAULT_FONT) != OK)
	    ਵਾਪਸ;
    }

    pango_context_list_families(
	    gui.ਪਾਠ_context,
	    &font_families,
	    &n_families);

    ਲਈ (ਪੂਰਨ i = 0; i < n_families; i++)
    {
	ਜੇ (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ਜਾਰੀ;

	ਅਚਲ ਅੱਖਰ *fam_name = pango_font_family_get_name(font_families[i]);
	ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&ਨਿਵੇਸ਼_conv,
		    (char_u *)fam_name, ਨਿਰਾ);
	    ਜੇ (buf != ਨਿਰਾ)
	    {
		ਜੇ (ਜੋੜੋ_ਮੇਲ(buf) != OK)
		{
		    vim_free(buf);
		    ਰੋਕੋ;
		}
		vim_free(buf);
	    }
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਜੋੜੋ_ਮੇਲ((char_u *)fam_name) != OK)
		ਰੋਕੋ;
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
    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ INVALCOLOR;

    ਜੇ (name != ਨਿਰਾ)
	ਵਾਪਸ gui_get_color_cmn(name);

    ਵਾਪਸ INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(ਪੂਰਨ r, ਪੂਰਨ g, ਪੂਰਨ b)
{
    ਵਾਪਸ gui_get_rgb_color_cmn(r, g, b);
}

    ਸਥਿਰ GdkRGBA
color_ਮੰਜ਼ਿਲ_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    ਵਾਪਸ rgba;
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_ਮੰਜ਼ਿਲ_rgba(color);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_ਮੰਜ਼ਿਲ_rgba(color);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_sp_color(guicolor_T color)
{
    *gui.spcolor = color_ਮੰਜ਼ਿਲ_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ਵਾਪਸ pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    ਖਾਲੀ
gui_gtk4_update_ਮਾਪ(ਖਾਲੀ)
{
    vim_draw_area_ਸੈੱਟ_ਮਾਪ(VIM_DRAW_AREA(gui.drawarea),
	    gui.ਅੰਕ_ਪੰਕਤੀਆਂ, gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    ਖਾਲੀ
gui_gtk4_ਜੋੜੋ_multisign(
	cairo_surface_t *surf,
	ਪੂਰਨ		ਪੰਕਤੀ,
	ਪੂਰਨ		ਖਾਨਾ,
	ਪੂਰਨ		ਚੌੜਾਈ,
	ਪੂਰਨ		ਉਚਾਈ)
{
    vim_draw_area_ਜੋੜੋ_multisign(VIM_DRAW_AREA(gui.drawarea), surf, ਪੰਕਤੀ, ਖਾਨਾ,
	    ਚੌੜਾਈ, ਉਚਾਈ);
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_clear_ਖੰਡ(ਪੂਰਨ row1, ਪੂਰਨ col1, ਪੂਰਨ row2, ਪੂਰਨ col2)
{
    vim_draw_area_clear_ਖੰਡ(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    ਜੇ (gui.drawarea != ਨਿਰਾ)
	gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
}

    ਖਾਲੀ
gui_mch_clear_all(ਖਾਲੀ)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    ਜੇ (gui.drawarea != ਨਿਰਾ)
	gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
}

    ਖਾਲੀ
gui_mch_delete_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
    vim_draw_area_move_ਖੰਡ(VIM_DRAW_AREA(gui.drawarea),
	    ਪੰਕਤੀ, ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ, gui.scroll_region_bot,
	    gui.scroll_region_ਖੱਬਾ, gui.scroll_region_ਸੱਜਾ);
    gui_clear_ਖੰਡ(
	    gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1, gui.scroll_region_ਖੱਬਾ,
	    gui.scroll_region_bot, gui.scroll_region_ਸੱਜਾ);

    gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
}

    ਖਾਲੀ
gui_mch_ਪਾਓ_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
    vim_draw_area_move_ਖੰਡ(VIM_DRAW_AREA(gui.drawarea),
	    ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ, ਪੰਕਤੀ, gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ,
	    gui.scroll_region_ਖੱਬਾ, gui.scroll_region_ਸੱਜਾ);
    gui_clear_ਖੰਡ(
	    ਪੰਕਤੀ, gui.scroll_region_ਖੱਬਾ,
	    ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ - 1, gui.scroll_region_ਸੱਜਾ);

    gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    ਖਾਲੀ
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, ਪੂਰਨ w, ਪੂਰਨ h)
{
    gui_mch_ਸੈੱਟ_bg_color(bg);
    gui_mch_ਸੈੱਟ_fg_color(fg);
    vim_draw_area_ਸੈੱਟ_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
}

    ਖਾਲੀ
gui_mch_flash(ਪੂਰਨ msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, (ਪੂਰਨ)Columns - 1);
    gui_mch_flush();
    ui_delay((ਲੰਮਾ)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, (ਪੂਰਨ)Columns - 1);
}

    ਖਾਲੀ
gui_mch_invert_rectangle(ਪੂਰਨ r, ਪੂਰਨ c, ਪੂਰਨ nr, ਪੂਰਨ nc)
{
    vim_draw_area_invert_ਖੰਡ(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    ਸਥਿਰ gboolean
ਕੁੰਜੀ_press_event(Gtk_Event_Controller_ਕੁੰਜੀ *controller ਅਣਵਰਤਿਆ,
	guint ਕੁੰਜੀ_sym, guint keycode ਅਣਵਰਤਿਆ,
	GdkModifierType state, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    char_u	string[32], string2[32];
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    ਪੂਰਨ		modifiers;
    ਪੂਰਨ		ਕੁੰਜੀ;
    char_u	*s, *d;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    ਜੇ (xic != ਨਿਰਾ)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	ਜੇ (event != ਨਿਰਾ && gtk_im_context_filter_keypress(xic, event))
	    ਵਾਪਸ TRUE;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਜੇ (ਕੁੰਜੀ_sym == GDK_ਕੁੰਜੀ_F10 && gui.menubar != ਨਿਰਾ)
    {
	ਸਥਿਰ char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	ਜੇ (check_map(k10, State, FALSE, TRUE, FALSE, ਨਿਰਾ, ਨਿਰਾ) == ਨਿਰਾ)
	{
	    show_menubar_popover();
	    ਵਾਪਸ TRUE;
	}
    }
#ਜੇ_ਅੰਤ

    len = keyval_ਮੰਜ਼ਿਲ_string(ਕੁੰਜੀ_sym, string2);

    ਜੇ (len > 1 && ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	len = convert_ਨਿਵੇਸ਼(string2, len, ਆਕਾਰ(string2));

    s = string2;
    d = string;
    ਲਈ (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	ਜੇ (d[-1] == CSI && d + 2 < string + ਆਕਾਰ(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (ਪੂਰਨ)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    ਜੇ (ਕੁੰਜੀ_sym == GDK_ਕੁੰਜੀ_ISO_ਖੱਬਾ_Tab)
    {
	ਕੁੰਜੀ_sym = GDK_ਕੁੰਜੀ_Tab;
	state |= GDK_ਖਿਸਕਾਓ_ਨਕਾਬ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    ਜੇ ((state & GDK_ALT_ਨਕਾਬ)
	    && gui.menu_is_ਸਰਗਰਮ
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ

    // Check for special keys
    ਜੇ (len == 0 || len == 1)
    {
	ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym != 0; i++)
	{
	    ਜੇ (special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym == ਕੁੰਜੀ_sym)
	    {
		string[0] = CSI;
		string[1] = special_ਕੁੰਜੀਆਂ[i].code0;
		string[2] = special_ਕੁੰਜੀਆਂ[i].code1;
		len = -3;
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (len == 0)
	ਵਾਪਸ TRUE;

    ਜੇ (len == -3)
	ਕੁੰਜੀ = ਮੰਜ਼ਿਲ_SPECIAL(string[1], string[2]);
    ਨਹੀਂਤਾਂ
    {
	string[len] = NUL;
	ਕੁੰਜੀ = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    ਕੁੰਜੀ = simplify_ਕੁੰਜੀ(ਕੁੰਜੀ, &modifiers);
    ਜੇ (ਕੁੰਜੀ == CSI)
	ਕੁੰਜੀ = K_CSI;
    ਜੇ (IS_SPECIAL(ਕੁੰਜੀ))
    {
	string[0] = CSI;
	string[1] = K_ਦੂਜਾ(ਕੁੰਜੀ);
	string[2] = K_ਤੀਜਾ(ਕੁੰਜੀ);
	len = 3;
    }
    ਨਹੀਂਤਾਂ
    {
	ਕੁੰਜੀ = may_adjust_ਕੁੰਜੀ_for_ctrl(modifiers, ਕੁੰਜੀ);
	modifiers = may_remove_ਖਿਸਕਾਓ_modifier(modifiers, ਕੁੰਜੀ);
	len = mb_char2bytes(ਕੁੰਜੀ, string);
    }

    ਜੇ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string2, 3);
    }

    {
	ਪੂਰਨ int_ch = check_for_ਰੁਕਾਵਟ(ਕੁੰਜੀ, modifiers);
	ਜੇ (int_ch != NUL)
	{
	    trash_ਨਿਵੇਸ਼_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);

    ਜੇ (p_mh)
	gui_mch_mousehide(TRUE);

    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
ਕੁੰਜੀ_release_event(Gtk_Event_Controller_ਕੁੰਜੀ *controller ਅਣਵਰਤਿਆ,
	guint keyval ਅਣਵਰਤਿਆ, guint keycode ਅਣਵਰਤਿਆ,
	GdkModifierType state ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
}

ਸਥਿਰ ਪੂਰਨ mouse_timed_ਬਾਹਰਲਾ = TRUE;
ਸਥਿਰ guint mouse_click_timer = 0;

    ਸਥਿਰ timeout_cb_type
mouse_click_timer_cb(gpointer ਅੰਕੜੇ)
{
    *(ਪੂਰਨ *)ਅੰਕੜੇ = TRUE;
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਪੂਰਨ
modifiers_gdk2mouse(guint state)
{
    ਪੂਰਨ modifiers = 0;

    ਜੇ (state & GDK_ਖਿਸਕਾਓ_ਨਕਾਬ)
	modifiers |= MOUSE_ਖਿਸਕਾਓ;
    ਜੇ (state & GDK_CONTROL_ਨਕਾਬ)
	modifiers |= MOUSE_CTRL;
    ਜੇ (state & GDK_ALT_ਨਕਾਬ)
	modifiers |= MOUSE_ALT;

    ਵਾਪਸ modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANY_BUTTON_ਨਕਾਬ (GDK_BUTTON1_ਨਕਾਬ | GDK_BUTTON2_ਨਕਾਬ \
	| GDK_BUTTON3_ਨਕਾਬ | GDK_BUTTON4_ਨਕਾਬ | GDK_BUTTON5_ਨਕਾਬ)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
ਸਥਿਰ ਪੂਰਨ mouse_pressed_button = -1;

ਸਥਿਰ guint motion_repeat_timer = 0;
ਸਥਿਰ gboolean motion_repeat_ਵਿੱਥ = FALSE;
ਸਥਿਰ ਪੂਰਨ mouse_press_x = -1, mouse_press_y = -1;

    ਸਥਿਰ ਖਾਲੀ
button_press_event(GtkGestureClick *gesture, ਪੂਰਨ n_press ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ button;
    ਪੂਰਨ repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    ਜੇ (!mouse_timed_ਬਾਹਰਲਾ && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_ਬਾਹਰਲਾ = FALSE;
    mouse_click_timer = timeout_ਜੋੜੋ(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_ਬਾਹਰਲਾ);

    ਚੋਣ (btn)
    {
	ਹਾਲਤ 1: button = MOUSE_ਖੱਬਾ; ਰੋਕੋ;
	ਹਾਲਤ 2: button = MOUSE_ਵਿਚਕਾਰ; ਰੋਕੋ;
	ਹਾਲਤ 3: button = MOUSE_ਸੱਜਾ; ਰੋਕੋ;
	ਹਾਲਤ 8: button = MOUSE_X1; ਰੋਕੋ;
	ਹਾਲਤ 9: button = MOUSE_X2; ਰੋਕੋ;
	ਮੂਲ: ਵਾਪਸ;
    }

    mouse_press_x = (ਪੂਰਨ)x;
    mouse_press_y = (ਪੂਰਨ)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (ਪੂਰਨ)x, (ਪੂਰਨ)y, repeated_click, vim_modifiers);
}

    ਸਥਿਰ ਖਾਲੀ
button_release_event(GtkGestureClick *gesture, ਪੂਰਨ n_press ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    ਜੇ (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (ਪੂਰਨ)x, (ਪੂਰਨ)y, FALSE, vim_modifiers);
}

ਸਥਿਰ ਦੁੱਗਣਾ prev_mouse_x = -1.0;
ਸਥਿਰ ਦੁੱਗਣਾ prev_mouse_y = -1.0;

ਸਥਿਰ GdkModifierType cur_state = 0;

    ਸਥਿਰ timeout_cb_type
mouse_repeat_timer_cb(gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ x, y;

    ਜੇ (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	ਵਾਪਸ G_ਸਰੋਤ_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    ਜੇ (vim_ਵਰਤਿਆ_ਵਿੱਚ_ਨਿਵੇਸ਼_buf() > 10)
	ਵਾਪਸ G_ਸਰੋਤ_CONTINUE;

    x = (ਪੂਰਨ)prev_mouse_x;
    y = (ਪੂਰਨ)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    ਜੇ (motion_repeat_ਵਿੱਥ)
	x += gui.char_ਚੌੜਾਈ;
    motion_repeat_ਵਿੱਥ = !motion_repeat_ਵਿੱਥ;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    ਵਾਪਸ G_ਸਰੋਤ_CONTINUE;
}

    ਸਥਿਰ ਖਾਲੀ
motion_notify_event(GtkEventControllerMotion *controller ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    ਜੇ (mouse_pressed_button >= 0 && event != ਨਿਰਾ
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_ਨਕਾਬ))
    {
	ਜੇ (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    ਜੇ (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	ਪੂਰਨ		w, h;

	ਜੇ (event != ਨਿਰਾ)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (ਪੂਰਨ)x, (ਪੂਰਨ)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_ਚੌੜਾਈ(gui.drawarea);
	h = gtk_widget_get_ਉਚਾਈ(gui.drawarea);

	ਜੇ (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	ਜੇ (x < 0 || y < 0 || x >= w || y >= h)
	{
	    ਪੂਰਨ	dx, dy;
	    ਪੂਰਨ offshoot;
	    ਪੂਰਨ	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    ਜੇ (offshoot > 127)
		delay = 5;
	    ਨਹੀਂਤਾਂ
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_ਜੋੜੋ(delay,
		    mouse_repeat_timer_cb, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	    motion_repeat_timer = 0;
    }
    ਨਹੀਂਤਾਂ
	gui_mouse_moved((ਪੂਰਨ)x, (ਪੂਰਨ)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    ਜੇ (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    ਸਥਿਰ ਖਾਲੀ
enter_notify_event(GtkEventControllerMotion *controller ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    ਸਥਿਰ gboolean
scroll_event(GtkEventControllerScroll *controller ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ dx ਅਣਵਰਤਿਆ, ਦੁੱਗਣਾ dy, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ button;
    int_u vim_modifiers;
    ਪੂਰਨ x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    ਜੇ (event == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    state = gdk_event_get_modifier_state(event);

    ਜੇ (dy < 0)
	button = MOUSE_4;	// scroll up
    ਨਹੀਂਤਾਂ ਜੇ (dy > 0)
	button = MOUSE_5;	// scroll down
    ਨਹੀਂਤਾਂ ਜੇ (dx < 0)
	button = MOUSE_7;	// scroll left
    ਨਹੀਂਤਾਂ ਜੇ (dx > 0)
	button = MOUSE_6;	// scroll right
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
focus_ਵਿੱਚ_event(GtkEventControllerFocus *controller ਅਣਵਰਤਿਆ,
	gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (gtk_ਖਿੜਕੀ_is_ਸਰਗਰਮ(GTK_ਖਿੜਕੀ(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	ਜੇ (blink_state == BLINK_NONE)
	    gui_mch_ਸ਼ੁਰੂ_blink();
    }
}

    ਸਥਿਰ ਖਾਲੀ
focus_ਬਾਹਰਲਾ_event(GtkEventControllerFocus *controller ਅਣਵਰਤਿਆ,
	gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!gtk_ਖਿੜਕੀ_is_ਸਰਗਰਮ(GTK_ਖਿੜਕੀ(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	ਜੇ (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#ਜੇ defined(FEAT_IMAGE)
    ਸਥਿਰ ਖਾਲੀ
scale_ਗੁਣਕ_cb(GdkSurface  *surface,
	GParamSpec	    *pspec ਅਣਵਰਤਿਆ,
	ਖਾਲੀ		    *udata ਅਣਵਰਤਿਆ)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
drawarea_realize_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_ਗੁਣਕ_cb), ਨਿਰਾ);
    redraw_all_later(UPD_VALID);
#ਜੇ_ਅੰਤ
    gui_mch_new_colors();
}

    ਸਥਿਰ ਖਾਲੀ
drawarea_unrealize_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    im_shutdown();
#ਜੇ_ਅੰਤ
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    READ_ਅੰਕੜੇ_CLIP,	    // Clipboard data
    READ_ਅੰਕੜੇ_DROP,	    // URI or text DND data
    READ_ਅੰਕੜੇ_DROP_ਅੰਕੜੇ	    // HTML DND data
} Read_ਅੰਕੜੇ_Type;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Read_ਅੰਕੜੇ_Type    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    ਅੱਖਰ	    *buf; // NULL when not needed
} Read_ਅੰਕੜੇ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Read_ਅੰਕੜੇ	rd;
    GdkDrop	*drop;
    ਪੂਰਨ		x;
    ਪੂਰਨ		y;
} Drop_Read_ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Read_ਅੰਕੜੇ	rd;
    Clipboard_T *cbd;
    ਅੱਖਰ	*mime_type;
} Clip_Read_ਅੰਕੜੇ;

    ਸਥਿਰ Read_ਅੰਕੜੇ *
read_ਅੰਕੜੇ_new(Read_ਅੰਕੜੇ_Type type, size_t sz)
{
    Read_ਅੰਕੜੇ *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    ਜੇ (type == READ_ਅੰਕੜੇ_CLIP || type == READ_ਅੰਕੜੇ_DROP_ਅੰਕੜੇ)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    ਵਾਪਸ rd;
}

    ਸਥਿਰ ਖਾਲੀ
read_ਅੰਕੜੇ_free(Read_ਅੰਕੜੇ *rd)
{
    g_cancellable_cancel(rd->cancel);

    ਜੇ (rd->type == READ_ਅੰਕੜੇ_CLIP)
	g_free(((Clip_Read_ਅੰਕੜੇ *)rd)->mime_type);
    ਨਹੀਂਤਾਂ ਜੇ (rd->type == READ_ਅੰਕੜੇ_DROP || rd->type == READ_ਅੰਕੜੇ_DROP_ਅੰਕੜੇ)
	g_object_unref(((Drop_Read_ਅੰਕੜੇ *)rd)->drop);
    g_object_unref(rd->cancel);
    ਜੇ (rd->arr != ਨਿਰਾ)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
ਸਥਿਰ ਖਾਲੀ drop_read_ਪਾਠ(GdkDrop *drop, char_u *ਪਾਠ);
#ਜੇ_ਅੰਤ

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    ਸਥਿਰ ਖਾਲੀ
read_ਅੰਕੜੇ_ਨਿਵੇਸ਼_cb(
	GInput_ਧਾਰਾ	*ਧਾਰਾ,
	GAsync_ਨਤੀਜਾ	*ਨਤੀਜਾ,
	Read_ਅੰਕੜੇ	*rd)
{
    ssize_t r = g_ਨਿਵੇਸ਼_ਧਾਰਾ_read_ਅੰਤ(ਧਾਰਾ, ਨਤੀਜਾ, ਨਿਰਾ);

    ਜੇ (r == -1)
    {
	// Error occurred
	Drop_Read_ਅੰਕੜੇ *drd = (Drop_Read_ਅੰਕੜੇ *)rd;

	ਜੇ (rd->type == READ_ਅੰਕੜੇ_DROP_ਅੰਕੜੇ)
	    gdk_drop_ਅੰਤ(drd->drop, 0);
    }
    ਨਹੀਂਤਾਂ ਜੇ (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	ਜੇ (rd->type == READ_ਅੰਕੜੇ_CLIP)
	{
	    Clip_Read_ਅੰਕੜੇ    *crd = (Clip_Read_ਅੰਕੜੇ *)rd;
	    ਲੰਮਾ	    len;
	    char_u	    *actual, *final;
	    ਪੂਰਨ		    motion_type = MAUTO;
	    char_u	    *tofree = ਨਿਰਾ;

	    len = (ਲੰਮਾ)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ਨਿਰਾ;

	    ਜੇ (clip_convert_ਅੰਕੜੇ(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
	ਨਹੀਂਤਾਂ ਜੇ (rd->type == READ_ਅੰਕੜੇ_DROP_ਅੰਕੜੇ)
	{
	    Drop_Read_ਅੰਕੜੇ    *drd = (Drop_Read_ਅੰਕੜੇ *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ਅਚਲ uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ਨਿਰਾ;
	    drop_read_ਪਾਠ(drd->drop, str);
	    g_free(str);
	    gdk_drop_ਅੰਤ(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ਅਚਲ uint8_t *)rd->buf, r);
	g_ਨਿਵੇਸ਼_ਧਾਰਾ_read_async(ਧਾਰਾ, rd->buf, READDATA_BUFSIZE,
		G_ਤਰਜੀਹ_ਉੱਚਾ,
		rd->cancel, (GAsyncReadyCallback)read_ਅੰਕੜੇ_ਨਿਵੇਸ਼_cb, rd);
	ਵਾਪਸ;
    }

    read_ਅੰਕੜੇ_free(rd);
    g_object_unref(ਧਾਰਾ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
/*
 * Set up for receiving DND items.
 */
    ਖਾਲੀ
gui_gtk_ਸੈੱਟ_dnd_targets(ਖਾਲੀ)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_ਜੋੜੋ_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_ਜੋੜੋ_gtype(builder, G_TYPE_STRING);
    ਜੇ (clip_html)
	gdk_content_formats_builder_ਜੋੜੋ_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_ਮੰਜ਼ਿਲ_formats(builder);
    gtk_drop_ਨਿਸ਼ਾਨਾ_async_ਸੈੱਟ_formats(gui.drop_ਨਿਸ਼ਾਨਾ, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    ਸਥਿਰ ਖਾਲੀ
drop_read_ਪਾਠ(GdkDrop *drop ਅਣਵਰਤਿਆ, char_u *ਪਾਠ)
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

    ਜੇ (ਪਾਠ == ਨਿਰਾ || *ਪਾਠ == NUL || !query_ਸੂਚਕ_pos(ਨਿਰਾ, ਨਿਰਾ, &state))
	ਵਾਪਸ;

    dnd_yank_drag_ਅੰਕੜੇ((char_u *)ਪਾਠ, (ਲੰਮਾ)STRLEN(ਪਾਠ));

    dropkey[2] = modifiers_gdk2vim(state);
    ਜੇ (dropkey[2] != 0)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(dropkey, (ਪੂਰਨ)ਆਕਾਰ(dropkey));
    ਨਹੀਂਤਾਂ
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(dropkey + 3, (ਪੂਰਨ)(ਆਕਾਰ(dropkey) - 3));
}

    ਸਥਿਰ ਖਾਲੀ
drop_read_ਮੁੱਲ_cb(GdkDrop *drop, GAsync_ਨਤੀਜਾ *ਨਤੀਜਾ, Drop_Read_ਅੰਕੜੇ *drd)
{
    ਅਚਲ GValue    *ਮੁੱਲ = gdk_drop_read_ਮੁੱਲ_ਅੰਤ(drop, ਨਤੀਜਾ, ਨਿਰਾ);
    gboolean	    success = FALSE;

    ਜੇ (ਮੁੱਲ == ਨਿਰਾ)
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

    ਜੇ (G_ਮੁੱਲ_HOLDS(ਮੁੱਲ, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_ਮੁੱਲ_get_boxed(ਮੁੱਲ);
	ਪੂਰਨ	nfiles = g_slist_ਲੰਮਾਈ(files);
	char_u	**fnames;
	ਪੂਰਨ	i;

	ਜੇ (nfiles <= 0)
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	fnames = ALLOC_MULT(char_u *, nfiles);
	ਜੇ (fnames == ਨਿਰਾ)
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	i = 0;
	ਲਈ (GSList *l = files; l != ਨਿਰਾ; l = l->ਅਗਲਾ)
	{
	    GFile *file = l->ਅੰਕੜੇ;
	    ਅੱਖਰ *ਰਸਤਾ = g_file_get_ਰਸਤਾ(file);
	    ਜੇ (ਰਸਤਾ != ਨਿਰਾ)
		fnames[i++] = vim_strsave((char_u *)ਰਸਤਾ);
	    g_free(ਰਸਤਾ);
	}
	nfiles = i;

	ਜੇ (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	ਨਹੀਂਤਾਂ
	    vim_free(fnames);
	success = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (G_ਮੁੱਲ_HOLDS(ਮੁੱਲ, G_TYPE_STRING))
    {
	ਅਚਲ ਅੱਖਰ *ਪਾਠ = g_ਮੁੱਲ_get_string(ਮੁੱਲ);

	ਜੇ (ਪਾਠ == ਨਿਰਾ || *ਪਾਠ == NUL)
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	drop_read_ਪਾਠ(drop, (char_u *)ਪਾਠ);
	success = TRUE;
    }

ਬਾਹਰ_ਨਿਕਲੋ:
    gdk_drop_ਅੰਤ(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_ਅੰਕੜੇ_free(&drd->rd);
}

    ਸਥਿਰ ਖਾਲੀ
drop_read_cb(GdkDrop *drop, GAsync_ਨਤੀਜਾ *ਨਤੀਜਾ, Drop_Read_ਅੰਕੜੇ *drd)
{
    GInput_ਧਾਰਾ    *ਵਿੱਚ_ਧਾਰਾ;
    ਅਚਲ ਅੱਖਰ	    *m;

    ਵਿੱਚ_ਧਾਰਾ = gdk_drop_read_ਅੰਤ(drop, ਨਤੀਜਾ, &m, ਨਿਰਾ);
    ਜੇ (ਵਿੱਚ_ਧਾਰਾ == ਨਿਰਾ || STRCMP(m, "text/html") != 0)
    {
	read_ਅੰਕੜੇ_free(&drd->rd);
	gdk_drop_ਅੰਤ(drop, 0);
	ਵਾਪਸ;
    }

    ਪ੍ਰਤਿਜ੍ਞਾ(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_ਨਿਵੇਸ਼_ਧਾਰਾ_read_async(ਵਿੱਚ_ਧਾਰਾ, drd->rd.buf, READDATA_BUFSIZE,
	    G_ਤਰਜੀਹ_ਉੱਚਾ,
	    drd->rd.cancel, (GAsyncReadyCallback)read_ਅੰਕੜੇ_ਨਿਵੇਸ਼_cb, drd);
}

// GCancellable of current DND operation, else NULL.
ਸਥਿਰ GCancellable *dnd_cancel = ਨਿਰਾ;
ਸਥਿਰ guint dnd_timeout_id = 0;

    ਸਥਿਰ gboolean
drop_timeout_cb(ਖਾਲੀ *udata ਅਣਵਰਤਿਆ)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    ਵਾਪਸ G_ਸਰੋਤ_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    ਸਥਿਰ gboolean
drop_cb(
	Gtk_Drop_ਨਿਸ਼ਾਨਾ_Async  *ਨਿਸ਼ਾਨਾ ਅਣਵਰਤਿਆ,
	GdkDrop		    *drop,
	ਦੁੱਗਣਾ		    x,
	ਦੁੱਗਣਾ		    y,
	ਖਾਲੀ		    *udata ਅਣਵਰਤਿਆ)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    Drop_Read_ਅੰਕੜੇ	*drd = ਨਿਰਾ;

    ਜੇ (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (Drop_Read_ਅੰਕੜੇ *)read_ਅੰਕੜੇ_new(READ_ਅੰਕੜੇ_DROP, ਆਕਾਰ(*drd));
	gdk_drop_read_ਮੁੱਲ_async(drop, GDK_TYPE_FILE_LIST, G_ਤਰਜੀਹ_ਉੱਚਾ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_ਮੁੱਲ_cb, drd);
    }
    ਨਹੀਂਤਾਂ ਜੇ (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	ਸਥਿਰ ਅਚਲ ਅੱਖਰ *m[] = {"text/html", ਨਿਰਾ};

	drd = (Drop_Read_ਅੰਕੜੇ *)read_ਅੰਕੜੇ_new(READ_ਅੰਕੜੇ_DROP_ਅੰਕੜੇ, ਆਕਾਰ(*drd));
	gdk_drop_read_async(drop, m, G_ਤਰਜੀਹ_ਉੱਚਾ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    ਨਹੀਂਤਾਂ ਜੇ (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (Drop_Read_ਅੰਕੜੇ *)read_ਅੰਕੜੇ_new(READ_ਅੰਕੜੇ_DROP, ਆਕਾਰ(*drd));
	gdk_drop_read_ਮੁੱਲ_async(drop, G_TYPE_STRING, G_ਤਰਜੀਹ_ਉੱਚਾ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_ਮੁੱਲ_cb, drd);
    }

    ਜੇ (dnd_cancel != ਨਿਰਾ)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    ਜੇ (drd != ਨਿਰਾ)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (ਪੂਰਨ)x;
	drd->y = (ਪੂਰਨ)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_ਜੋੜੋ(3000, drop_timeout_cb, ਨਿਰਾ);

	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
mainwin_destroy_cb(GObject *object ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    gui.mainwin = ਨਿਰਾ;
    gui.drawarea = ਨਿਰਾ;
    ਜੇ (!exiting)
	gui_shell_closed();
}

    ਸਥਿਰ gboolean
delete_event_cb(Gtk_ਖਿੜਕੀ *ਖਿੜਕੀ_2 ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    gui_shell_closed();
    ਵਾਪਸ TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    ਸਥਿਰ timeout_cb_type
ਨਿਵੇਸ਼_timer_cb(gpointer ਅੰਕੜੇ)
{
    ਪੂਰਨ *timed_ਬਾਹਰਲਾ = (ਪੂਰਨ *)ਅੰਕੜੇ;

    *timed_ਬਾਹਰਲਾ = TRUE;
    ਵਾਪਸ FALSE;	// don't call me again
}

    ਖਾਲੀ
gui_mch_update(ਖਾਲੀ)
{
    ਪੂਰਨ cnt = 0;

    ਜਦੋਂ (g_main_context_pending(ਨਿਰਾ) && !vim_is_ਨਿਵੇਸ਼_buf_ਭਰਿਆ()
								&& ++cnt < 100)
	g_main_context_iteration(ਨਿਰਾ, TRUE);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਸਥਿਰ timeout_cb_type
channel_poll_cb(gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    ਵਾਪਸ TRUE; // Keep repeating
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
gui_mch_wait_for_chars(ਲੰਮਾ wtime)
{
    ਪੂਰਨ		focus;
    guint	timer;
    ਸਥਿਰ ਪੂਰਨ	timed_ਬਾਹਰਲਾ;
    ਪੂਰਨ		retval = FAIL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    guint	channel_timer = 0;
#ਜੇ_ਅੰਤ

    timed_ਬਾਹਰਲਾ = FALSE;

    ਜੇ (wtime >= 0)
	timer = timeout_ਜੋੜੋ(wtime == 0 ? 1L : wtime,
						   ਨਿਵੇਸ਼_timer_cb, &timed_ਬਾਹਰਲਾ);
    ਨਹੀਂਤਾਂ
	timer = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ਜੇ (channel_any_keep_open())
	channel_timer = timeout_ਜੋੜੋ(20, channel_poll_cb, ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    focus = gui.ਵਿੱਚ_focus;

    ਕਰੋ
    {
	// Stop or start blinking when focus changes
	ਜੇ (gui.ਵਿੱਚ_focus != focus)
	{
	    ਜੇ (gui.ਵਿੱਚ_focus)
		gui_mch_ਸ਼ੁਰੂ_blink();
	    ਨਹੀਂਤਾਂ
		gui_mch_stop_blink(TRUE);
	    focus = gui.ਵਿੱਚ_focus;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
# ifdef FEAT_TIMERS
	did_ਜੋੜੋ_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ਜੇ (did_ਜੋੜੋ_timer)
	    ਜਾਓ theend;
# endif
#ਜੇ_ਅੰਤ

	ਜੇ (gui.mainwin == ਨਿਰਾ)
	    ਜਾਓ theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	ਜੇ (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    ਜਾਓ theend;
	}

	ਜੇ (!ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(ਨਿਰਾ, TRUE);
	    --gtk4_main_loop_level;
	}

	ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	{
	    retval = OK;
	    ਜਾਓ theend;
	}
    } ਜਦੋਂ (wtime < 0 || !timed_ਬਾਹਰਲਾ);

    gui_mch_update();

theend:
    ਜੇ (timer != 0 && !timed_ਬਾਹਰਲਾ)
	timeout_remove(timer);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (channel_timer != 0)
	timeout_remove(channel_timer);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ retval;
}

    ਖਾਲੀ
gui_mch_flush(ਖਾਲੀ)
{
    // Ensure the offscreen surface content gets painted to the widget.
    ਜੇ (gui.drawarea != ਨਿਰਾ)
	gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
    ਜੇ (gui.mainwin != ਨਿਰਾ && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    ਖਾਲੀ
gui_mch_beep(ਖਾਲੀ)
{
    GdkDisplay *display;

    ਜੇ (gui.mainwin != ਨਿਰਾ && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	ਜੇ (display != ਨਿਰਾ)
	    gdk_display_beep(display);
    }
}

    ਖਾਲੀ *
gui_mch_get_display(ਖਾਲੀ)
{
    ਜੇ (gui.mainwin != ਨਿਰਾ && gtk_widget_get_display(gui.mainwin))
	ਵਾਪਸ gtk_widget_get_display(gui.mainwin);
    ਵਾਪਸ ਨਿਰਾ;
}

    ਖਾਲੀ
gui_mch_iconify(ਖਾਲੀ)
{
    gtk_ਖਿੜਕੀ_minimize(GTK_ਖਿੜਕੀ(gui.mainwin));
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_foreground(ਖਾਲੀ)
{
    gtk_ਖਿੜਕੀ_present(GTK_ਖਿੜਕੀ(gui.mainwin));
}

    ਸਥਿਰ ਪੂਰਨ
query_ਸੂਚਕ_pos(ਪੂਰਨ *x, ਪੂਰਨ *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *ਸੂਚਕ;
    ਦੁੱਗਣਾ	     sx, sy, nx, ny;
    graphene_ਬਿੰਦੂ_t src, dst;

    ਜੇ (gui.drawarea == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    ਜੇ (native == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    surface = gtk_native_get_surface(native);
    ਜੇ (surface == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    ਜੇ (display == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    seat = gdk_display_get_default_seat(display);
    ਜੇ (seat == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਸੂਚਕ = gdk_seat_get_ਸੂਚਕ(seat);
    ਜੇ (ਸੂਚਕ == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਜੇ (!gdk_surface_get_device_position(surface, ਸੂਚਕ, &sx, &sy, state))
	ਵਾਪਸ FALSE;

    ਜੇ (x != ਨਿਰਾ && y != ਨਿਰਾ)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (ਦਸ਼ਮਲਵ)(sx - nx);
	src.y = (ਦਸ਼ਮਲਵ)(sy - ny);
	ਜੇ (!gtk_widget_compute_ਬਿੰਦੂ(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    ਵਾਪਸ FALSE;

	*x = (ਪੂਰਨ)dst.x;
	*y = (ਪੂਰਨ)dst.y;
    }
    ਵਾਪਸ TRUE;
}

    ਖਾਲੀ
gui_mch_getmouse(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    ਜੇ (!query_ਸੂਚਕ_pos(x, y, ਨਿਰਾ))
    {
	*x = 0;
	*y = 0;
    }
}

    ਖਾਲੀ
gui_mch_setmouse(ਪੂਰਨ x ਅਣਵਰਤਿਆ, ਪੂਰਨ y ਅਣਵਰਤਿਆ)
{
    // GTK4/Wayland: cannot warp pointer
}

    ਖਾਲੀ
gui_mch_mousehide(ਪੂਰਨ hide)
{
    ਜੇ (gui.ਸੂਚਕ_hidden == hide)
	ਵਾਪਸ;

    gui.ਸੂਚਕ_hidden = hide;
    ਜੇ (gui.blank_ਸੂਚਕ != ਨਿਰਾ)
    {
	ਜੇ (hide)
	    gtk_widget_ਸੈੱਟ_cursor(gui.drawarea, gui.blank_ਸੂਚਕ);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
	    mch_ਸੈੱਟ_mouse_shape(ਆਖਰੀ_shape);
#ਨਹੀਂਤਾਂ
	    gtk_widget_ਸੈੱਟ_cursor(gui.drawarea, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    }
}

    ਪੂਰਨ
gui_mch_haskey(char_u *name)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym != 0; i++)
	ਜੇ (name[0] == special_ਕੁੰਜੀਆਂ[i].code0
		&& name[1] == special_ਕੁੰਜੀਆਂ[i].code1)
	    ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

    ਖਾਲੀ
gui_mch_forked(ਖਾਲੀ)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_gtk4_update_scrollbar_ਮਾਪ(ਖਾਲੀ)
{
    win_T	*wp;
    ਪੂਰਨ		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_ਖੱਬਾ].id;

	ਜੇ (sbar == ਨਿਰਾ || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_ਖੱਬਾ]
		    && wp->w_scrollbars[SBAR_ਸੱਜਾ].id != ਨਿਰਾ))
	    sbar = wp->w_scrollbars[SBAR_ਸੱਜਾ].id;

	ਜੇ (sbar != ਨਿਰਾ && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    ਪੂਰਨ		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_ਮਾਪ(sbar, &min, &nat);
	    sw = MAX(min.ਚੌੜਾਈ, nat.ਚੌੜਾਈ);
	    ਜੇ (sw > 0)
	    {
		w = sw;
		ਰੋਕੋ;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    ਜੇ (sbar != ਨਿਰਾ && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	ਪੂਰਨ		    sh;

	gtk_widget_get_preferred_ਮਾਪ(sbar, &min, &nat);
	sh = MAX(min.ਉਚਾਈ, nat.ਉਚਾਈ);

	ਜੇ (sh > 0)
	    h = sh;
    }

    ਜੇ (w != -1)
	gui.scrollbar_ਚੌੜਾਈ = w;
    ਜੇ (h != -1)
	gui.scrollbar_ਉਚਾਈ = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    ਖਾਲੀ
gui_mch_enable_scrollbar(scrollbar_T *sb, ਪੂਰਨ flag)
{
    ਜੇ (sb->id != ਨਿਰਾ)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_ਸੈੱਟ_visible(sb->id, flag);

	ਜੇ (!was_visible && flag)
	    gui_gtk4_update_scrollbar_ਮਾਪ();
    }
}

#ਜੇ defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    ਖਾਲੀ
gui_mch_menu_grey(vimmenu_T *menu, ਪੂਰਨ grey)
{
    ਜੇ (menu->id == ਨਿਰਾ)
	ਵਾਪਸ;
    gtk_widget_ਸੈੱਟ_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    ਖਾਲੀ
gui_mch_menu_hidden(vimmenu_T *menu, ਪੂਰਨ hidden)
{
    ਜੇ (menu->id == ਨਿਰਾ)
	ਵਾਪਸ;
    gtk_widget_ਸੈੱਟ_visible(menu->id, !hidden);
    gui_mch_update();
}

    ਖਾਲੀ
gui_mch_draw_menubar(ਖਾਲੀ)
{
    // Just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#ਜੇ_ਅੰਤ

/*
 * ============================================================
 * Tabline
 * ============================================================
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    ਖਾਲੀ
gui_mch_show_tabline(ਪੂਰਨ showit)
{
    ਜੇ (gui.tabline == ਨਿਰਾ)
	ਵਾਪਸ;
    gtk_widget_ਸੈੱਟ_visible(gui.tabline, showit);
    gui_mch_update();
}

    ਪੂਰਨ
gui_mch_showing_tabline(ਖਾਲੀ)
{
    ਵਾਪਸ gui.tabline != ਨਿਰਾ && gtk_widget_get_visible(gui.tabline);
}

ਸਥਿਰ ਪੂਰਨ ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
ਸਥਿਰ ਪੂਰਨ tabpage_hover = 0;

    ਸਥਿਰ ਖਾਲੀ
tabpage_enter_cb(
	GtkEventController  *controller ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ		    x ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ		    y ਅਣਵਰਤਿਆ,
	ਖਾਲੀ		    *udata)
{
    tabpage_hover = GPOINTER_ਮੰਜ਼ਿਲ_INT(udata);
}

    ਸਥਿਰ ਖਾਲੀ
tabline_enter_cb(
	GtkEventController  *controller ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ		    x ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ		    y ਅਣਵਰਤਿਆ,
	ਖਾਲੀ		    *udata ਅਣਵਰਤਿਆ)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    ਸਥਿਰ ਖਾਲੀ
tabline_ਜੋੜੋ_css(ਖਾਲੀ)
{
    GtkCssProvider  *provider;
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ *css =
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
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(4, 12, 0)
    gtk_css_provider_ਲੱਦੋ_from_string(provider, css);
# ਨਹੀਂਤਾਂ
    gtk_css_provider_ਲੱਦੋ_from_ਅੰਕੜੇ(provider, css, -1);
# endif
    gtk_style_context_ਜੋੜੋ_provider_for_display(
	    gtk_widget_get_display(gui.tabline), GTK_STYLE_PROVIDER(provider),
	    GTK_STYLE_PROVIDER_ਤਰਜੀਹ_APPLICATION);
    g_object_unref(provider);
}

/*
 * Handle clicking the close button of a tab.
 */
    ਸਥਿਰ ਖਾਲੀ
tab_close_clicked_cb(GtkButton *button ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ)
{
    ਪੂਰਨ idx = gtk_notebook_ਪੰਨਾ_ਅੰਕ(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(ਅੰਕੜੇ));

    ਜੇ (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    ਖਾਲੀ
gui_mch_update_tabline(ਖਾਲੀ)
{
    GtkWidget	*ਪੰਨਾ;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    ਪੂਰਨ		nr = 0;
    ਪੂਰਨ		tab_ਅੰਕ;
    ਪੂਰਨ		curtabidx = 0;
    char_u	*labeltext;

    ਜੇ (gui.tabline == ਨਿਰਾ)
	ਵਾਪਸ;

    ignore_tabline_evt = TRUE;

    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ; tp = tp->tp_ਅਗਲਾ, ++nr)
    {
	Gtk_Notebook_ਪੰਨਾ	    *ਪੰਨਾ_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	ਜੇ (tp == curtab)
	    curtabidx = nr;

	tab_ਅੰਕ = nr + 1;

	ਪੰਨਾ = gtk_notebook_get_nth_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), nr);
	ਜੇ (ਪੰਨਾ == ਨਿਰਾ)
	{
	    GtkWidget	*spacer;
	    GtkWidget	*close_button;

	    ਪੰਨਾ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_widget_ਸੈੱਟ_visible(ਪੰਨਾ, TRUE);
	    event_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_ਸੈੱਟ_visible(event_box, TRUE);

	    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_ਜੋੜੋ_css_class(spacer, "vim-tab-spacer");
	    gtk_box_append(GTK_BOX(event_box), spacer);

	    label = gtk_label_new("-Empty-");
	    gtk_widget_ਸੈੱਟ_hexpand(label, TRUE);
	    gtk_label_ਸੈੱਟ_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_ਸਿਰਾ);
	    gtk_box_append(GTK_BOX(event_box), label);
	    gtk_widget_ਸੈੱਟ_visible(label, TRUE);
	    g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(event_box), "label", label);

	    close_button = gtk_button_new_from_icon_name("window-close-symbolic");
	    gtk_widget_ਜੋੜੋ_css_class(close_button, "flat");
	    gtk_widget_ਜੋੜੋ_css_class(close_button, "circular");
	    gtk_widget_ਸੈੱਟ_valign(close_button, GTK_ALIGN_CENTER);
	    gtk_widget_ਸੈੱਟ_focus_on_click(close_button, FALSE);
	    gtk_widget_ਸੈੱਟ_tooltip_ਪਾਠ(close_button, _("Close Tab"));
	    g_signal_connect(close_button, "clicked",
		    G_CALLBACK(tab_close_clicked_cb), ਪੰਨਾ);
	    gtk_box_append(GTK_BOX(event_box), close_button);

	    gtk_notebook_ਪਾਓ_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline),
		    ਪੰਨਾ, event_box, nr);
	    gtk_notebook_ਸੈੱਟ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    ਪੰਨਾ, TRUE);
	    g_object_ਸੈੱਟ(gtk_notebook_get_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), ਪੰਨਾ),
		    "tab-expand", TRUE, ਨਿਰਾ);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	ਪੰਨਾ_widget = gtk_notebook_get_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), ਪੰਨਾ);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(tab_ਅੰਕ));
	gtk_event_controller_ਸੈੱਟ_ਪ੍ਰਸਾਰ_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(ਪੰਨਾ_widget, "tab", &tab_widget, ਨਿਰਾ);
	gtk_widget_ਜੋੜੋ_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), ਪੰਨਾ);
	label = g_object_get_ਅੰਕੜੇ(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_ਮੰਜ਼ਿਲ_UTF8(NameBuff);
	ਜੇ (label != ਨਿਰਾ && GTK_IS_LABEL(label))
	    gtk_label_ਸੈੱਟ_ਪਾਠ(GTK_LABEL(label), (ਅਚਲ ਅੱਖਰ *)labeltext);
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_ਮੰਜ਼ਿਲ_UTF8(NameBuff);
	gtk_widget_ਸੈੱਟ_tooltip_ਪਾਠ(event_box, (ਅਚਲ gchar *)labeltext);
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(labeltext);
    }

    ਜਦੋਂ (gtk_notebook_get_nth_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), nr) != ਨਿਰਾ)
	gtk_notebook_remove_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    ਜੇ (nr == 1)
	gtk_widget_ਜੋੜੋ_css_class(gui.tabline, "single-tab");
    ਨਹੀਂਤਾਂ
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    ਜੇ (gtk_notebook_get_current_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_ਸੈੱਟ_current_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_curtab(ਪੂਰਨ nr)
{
    ਜੇ (gui.tabline != ਨਿਰਾ)
	gtk_notebook_ਸੈੱਟ_current_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    ਸਥਿਰ ਖਾਲੀ
on_select_tab(
	GtkNotebook	*notebook ਅਣਵਰਤਿਆ,
	gpointer	*ਪੰਨਾ ਅਣਵਰਤਿਆ,
	gint		idx,
	gpointer	ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    ਸਥਿਰ ਖਾਲੀ
on_tab_reordered(
	GtkNotebook	*notebook ਅਣਵਰਤਿਆ,
	gpointer	*ਪੰਨਾ ਅਣਵਰਤਿਆ,
	gint		idx,
	gpointer	ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (ignore_tabline_evt)
	ਵਾਪਸ;

    ਜੇ ((tabpage_ਸੂਚਕਾਂਕ(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    ਨਹੀਂਤਾਂ
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ਸਥਿਰ ਖਾਲੀ
tabline_menu_event_cb(Vim_Menu_ਵਸਤੂ *ਵਸਤੂ ਅਣਵਰਤਿਆ, Vim_Menu_ਵਸਤੂ_Event event, ਖਾਲੀ *udata)
{
    ਜੇ (event == VIM_MENU_ਵਸਤੂ_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_ਮੰਜ਼ਿਲ_INT(udata));
}

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_tabline_menu_ਵਸਤੂ(VimMenu *menu, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ resp)
{
    Vim_Menu_ਵਸਤੂ *ਵਸਤੂ = VIM_MENU_ਵਸਤੂ(vim_menu_ਵਸਤੂ_new(name,
		tabline_menu_event_cb, GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(resp)));

    vim_menu_ਪਾਓ_ਵਸਤੂ(menu, ਵਸਤੂ, -1);
}

/*
 * Create a menu for the tab line.
 */
    ਸਥਿਰ VimMenu *
create_tabline_popup_menu(ਖਾਲੀ)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    ਜੋੜੋ_tabline_menu_ਵਸਤੂ(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    ਜੋੜੋ_tabline_menu_ਵਸਤੂ(menu, _("New Tab"), TABLINE_MENU_NEW);
    ਜੋੜੋ_tabline_menu_ਵਸਤੂ(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    ਵਾਪਸ menu;
}

    ਸਥਿਰ ਖਾਲੀ
tabline_menu_press_event(
	GtkGestureClick *gesture,
	ਪੂਰਨ		n_press ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ		x,
	ਦੁੱਗਣਾ		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    ਜੇ (but == GDK_BUTTON_SECONDARY)
    {
	// Right mouse button pressed, popup menu
	GdkRectangle rect;
	rect.x = x;
	rect.y = y;
	rect.ਚੌੜਾਈ = 1;
	rect.ਉਚਾਈ = 1;

	gtk_popover_ਸੈੱਟ_pointing_ਮੰਜ਼ਿਲ(GTK_POPOVER(popover), &rect);
	gtk_popover_popup(GTK_POPOVER(popover));
    }
    ਨਹੀਂਤਾਂ ਜੇ (but == GDK_BUTTON_PRIMARY)
    {
	ਜੇ (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    ਨਹੀਂਤਾਂ ਜੇ (but == GDK_BUTTON_ਵਿਚਕਾਰ)
    {
	ਜੇ (tabpage_hover != 0)
	    // Middle mouse click on tabpage label closes that tab.
	    send_tabline_menu_event(tabpage_hover, TABLINE_MENU_CLOSE);
    }
}
#ਜੇ_ਅੰਤ

/*
 * ============================================================
 * Sign support
 * ============================================================
 */

#ਜੇ defined(FEAT_SIGN_ICONS)
# define SIGN_ਚੌੜਾਈ  (2 * gui.char_ਚੌੜਾਈ)
# define SIGN_ਉਚਾਈ (gui.char_ਉਚਾਈ)

    ਖਾਲੀ
gui_mch_drawsign(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    ਜੇ (sign == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_draw_area_ਜੋੜੋ_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    ਪੰਕਤੀ, ਖਾਨਾ, SIGN_ਚੌੜਾਈ, SIGN_ਉਚਾਈ);

    gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
}

    ਖਾਲੀ *
gui_mch_ਰਜਿਸਟਰ_sign(char_u *signfile)
{
    ਜੇ (signfile[0] != NUL && signfile[0] != '-' && gui.ਵਿੱਚ_use)
    {
	GError	    *error = ਨਿਰਾ;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ਅਚਲ ਅੱਖਰ *)signfile,
							    &error);
	ਜੇ (sign != ਨਿਰਾ)
	    ਵਾਪਸ sign;

	semsg("E255: %s", error->ਸੁਨੇਹਾ_2);
	g_error_free(error);
    }
    ਵਾਪਸ ਨਿਰਾ;
}

    ਖਾਲੀ
gui_mch_destroy_sign(ਖਾਲੀ *sign)
{
    ਜੇ (sign != ਨਿਰਾ)
	g_object_unref(sign);
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
gui_gtk_draw_string(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, char_u *s, ਪੂਰਨ len, ਪੂਰਨ flags)
{
    ਪੂਰਨ cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    ਪੰਕਤੀ, ਖਾਨਾ, s, len, flags);

    ਜੇ (gui.drawarea != ਨਿਰਾ)
	gtk_widget_ਕਤਾਰ_draw(gui.drawarea);

    ਵਾਪਸ cells;
}

    ਪੂਰਨ
gui_get_x11_windis(ਖਿੜਕੀ *win ਅਣਵਰਤਿਆ, Display **dis ਅਣਵਰਤਿਆ)
{
    // GTK4: not applicable
    ਵਾਪਸ FAIL;
}

#ਜੇ defined(FEAT_MENU)
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
    ਸਥਿਰ char_u *
translate_mnemonic_ਟੈਗ(char_u *name, ਪੂਰਨ use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    ਪੂਰਨ	    n_underscores = 0;

    name = CONVERT_ਮੰਜ਼ਿਲ_UTF8(name);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ਜੇ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ਜੇ (buf != ਨਿਰਾ)
    {
	pdest = buf;
	ਲਈ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ਜੇ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(name);
    ਵਾਪਸ buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    ਖਾਲੀ
gui_gtk_ਸੈੱਟ_mnemonics(ਪੂਰਨ enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ਜੇ (menu->id == ਨਿਰਾ)
	    ਜਾਰੀ;

	name = translate_mnemonic_ਟੈਗ(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	ਜੇ (VIM_IS_MENU_BAR_ਵਸਤੂ(menu->id))
	    vim_menu_bar_ਵਸਤੂ_ਸੈੱਟ_ਪਾਠ(VIM_MENU_BAR_ਵਸਤੂ(menu->id),
		    (ਅਚਲ ਅੱਖਰ *)name);
	vim_free(name);
    }
}

    ਸਥਿਰ ਖਾਲੀ
popupmenu_closed_cb(GtkWidget *popover, ਖਾਲੀ *udata ਅਣਵਰਤਿਆ)
{
    gtk_widget_unparent(popover);
    ਜੇ (gui.drawarea != ਨਿਰਾ)
	gtk_widget_ਕਤਾਰ_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    ਸਥਿਰ ਖਾਲੀ
gui_gtk_popup_at(vimmenu_T *menu, ਪੂਰਨ x, ਪੂਰਨ y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    ਜੇ (menu == ਨਿਰਾ || menu->submenu_id == ਨਿਰਾ)
	ਵਾਪਸ;

    popover = vim_menu_copy(VIM_MENU(menu->submenu_id));
    gtk_widget_ਸੈੱਟ_ਮਾਪਾ(popover, gui.drawarea);

    rect.x = x;
    rect.y = y;
    rect.ਚੌੜਾਈ = rect.ਉਚਾਈ = 1;

    // Make sure popover aligns down-and-to-the-right of the pointer.
    gtk_popover_ਸੈੱਟ_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_widget_ਸੈੱਟ_halign(popover, GTK_ALIGN_ਸ਼ੁਰੂ);
    gtk_popover_ਸੈੱਟ_pointing_ਮੰਜ਼ਿਲ(GTK_POPOVER(popover), &rect);

    g_signal_connect(GTK_POPOVER(popover), "closed",
	    G_CALLBACK(popupmenu_closed_cb), ਨਿਰਾ);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    ਖਾਲੀ
gui_make_popup(char_u *ਰਸਤਾ_name, ਪੂਰਨ mouse_pos)
{
    vimmenu_T	*menu;
    ਪੂਰਨ		x, y;

    menu = gui_ਲੱਭੋ_menu(ਰਸਤਾ_name);
    ਜੇ (menu == ਨਿਰਾ || menu->submenu_id == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (mouse_pos)
	gui_mch_getmouse(&x, &y);
    ਨਹੀਂਤਾਂ
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#ਜੇ_ਅੰਤ // FEAT_MENU

    ਪੂਰਨ
get_menu_tool_ਚੌੜਾਈ(ਖਾਲੀ)
{
    ਵਾਪਸ 0;
}

    ਪੂਰਨ
get_menu_tool_ਉਚਾਈ(ਖਾਲੀ)
{
    GtkWidget *widgets[] = {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	gui.menubar,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
	gui.toolbar,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	gui.tabline
#ਜੇ_ਅੰਤ
    };

    ਪੂਰਨ ਉਚਾਈ = 0;

    ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	ਪੂਰਨ		h;

	ਜੇ (widgets[i] == ਨਿਰਾ || !gtk_widget_get_visible(widgets[i]))
	    ਜਾਰੀ;

	h = gtk_widget_get_ਉਚਾਈ(widgets[i]);

	ਜੇ (h == 0)
	{
	    // Allocation hasn't been updated yet (widget just became visible).
	    // Query the preferred height so the caller gets a valid value
	    // before the layout pass runs.  Use the maximum of minimum and
	    // natural height: GTK may allocate min_h even when natural_h is
	    // smaller (e.g. GtkNotebook tab bar has min_h > natural_h due to
	    // CSS).
	    gtk_widget_get_preferred_ਮਾਪ(widgets[i], &min, &nat);
	    ਉਚਾਈ += MAX(min.ਉਚਾਈ, nat.ਉਚਾਈ);
	}
	ਨਹੀਂਤਾਂ
	    ਉਚਾਈ += h;
    }
    ਵਾਪਸ ਉਚਾਈ;
}

/*
 * Update selection targets for regular and primary selections.
 */
    ਖਾਲੀ
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    ਜੇ (cbd == &clip_plus)
	gdk_content_provider_content_ਬਦਲਿਆ(gui.regular_provider);
    ਨਹੀਂਤਾਂ
	gdk_content_provider_content_ਬਦਲਿਆ(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    ਸਥਿਰ GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    ਜੇ (gui.mainwin == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    display = gtk_widget_get_display(gui.mainwin);
    ਜੇ (display == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (cbd == &clip_plus)
    {
	ਜੇ (provider != ਨਿਰਾ)
	    *provider = gui.regular_provider;
	ਵਾਪਸ gdk_display_get_clipboard(display);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (provider != ਨਿਰਾ)
	    *provider = gui.primary_provider;
	ਵਾਪਸ gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    ਸਥਿਰ ਖਾਲੀ
clip_read_cb(GdkClipboard *cb, GAsync_ਨਤੀਜਾ *ਨਤੀਜਾ, Clip_Read_ਅੰਕੜੇ *crd)
{
    GInput_ਧਾਰਾ    *ਵਿੱਚ_ਧਾਰਾ;
    ਅਚਲ ਅੱਖਰ	    *mime_type;

    ਵਿੱਚ_ਧਾਰਾ = gdk_clipboard_read_ਅੰਤ(cb, ਨਤੀਜਾ, &mime_type, ਨਿਰਾ);
    ਜੇ (ਵਿੱਚ_ਧਾਰਾ == ਨਿਰਾ)
    {
	read_ਅੰਕੜੇ_free(&crd->rd);
	ਵਾਪਸ;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_ਨਿਵੇਸ਼_ਧਾਰਾ_read_async(ਵਿੱਚ_ਧਾਰਾ, crd->rd.buf, READDATA_BUFSIZE,
	    G_ਤਰਜੀਹ_ਉੱਚਾ,
	    crd->rd.cancel, (GAsyncReadyCallback)read_ਅੰਕੜੇ_ਨਿਵੇਸ਼_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    ਖਾਲੀ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	ਨਿਰਾ
    };

    GdkClipboard *clipboard;
    Clip_Read_ਅੰਕੜੇ *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, ਨਿਰਾ);
    ਜੇ (clipboard == ਨਿਰਾ)
	ਵਾਪਸ;

    crd = (Clip_Read_ਅੰਕੜੇ *)read_ਅੰਕੜੇ_new(READ_ਅੰਕੜੇ_CLIP, ਆਕਾਰ(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_ਤਰਜੀਹ_ਉੱਚਾ, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t ਸ਼ੁਰੂ;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	ਸ਼ੁਰੂ = ਸਮਾਂ(ਨਿਰਾ);
	ਜਦੋਂ (!g_cancellable_is_cancelled(cancel)
		&& ਸਮਾਂ(ਨਿਰਾ) < ਸ਼ੁਰੂ + 3)
	    g_main_context_iteration(ਨਿਰਾ, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

ਸਥਿਰ ਪੂਰਨ ਵਿੱਚ_clipboard_ਸੈੱਟ = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    ਖਾਲੀ
clip_mch_ਸੈੱਟ_selection(Clipboard_T *cbd ਅਣਵਰਤਿਆ)
{
}

    ਸਥਿਰ ਖਾਲੀ
clipboard_ਬਦਲਿਆ_cb(GdkClipboard *clipboard, gpointer user_ਅੰਕੜੇ)
{
    Clipboard_T *cbd = (Clipboard_T *)user_ਅੰਕੜੇ;

    ਜੇ (ਵਿੱਚ_clipboard_ਸੈੱਟ)
	ਵਾਪਸ;
    ਜੇ (gdk_clipboard_is_local(clipboard))
	ਵਾਪਸ;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    ਪੂਰਨ
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    ਪੂਰਨ			ret;

    ਜੇ (cb == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਵਿੱਚ_clipboard_ਸੈੱਟ = TRUE;
    ret = gdk_clipboard_ਸੈੱਟ_content(cb, cp);
    ਵਿੱਚ_clipboard_ਸੈੱਟ = FALSE;

    ਵਾਪਸ ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    ਖਾਲੀ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, ਨਿਰਾ);
    ਜੇ (clipboard == ਨਿਰਾ)
	ਵਾਪਸ;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    ਜੇ (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_ਸੈੱਟ_content(clipboard, ਨਿਰਾ);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(ਖਾਲੀ)
{
    ਵਾਪਸ gtk4_main_loop_level;
}

    ਖਾਲੀ
gtk_main_quit(ਖਾਲੀ)
{
    gtk4_main_loop_quit = TRUE;
}

#ਜੇ defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *mshape_css_ਨਾਂ[] =
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

    ਖਾਲੀ
mch_ਸੈੱਟ_mouse_shape(ਪੂਰਨ shape)
{
    GdkCursor	*c;
    ਅਚਲ ਅੱਖਰ	*css_name = "default";

    ਜੇ (gui.drawarea == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (shape == MSHAPE_HIDE || gui.ਸੂਚਕ_hidden)
	gtk_widget_ਸੈੱਟ_cursor(gui.drawarea, gui.blank_ਸੂਚਕ);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	ਨਹੀਂਤਾਂ ਜੇ (shape < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(mshape_css_ਨਾਂ))
	    css_name = mshape_css_ਨਾਂ[shape];
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, ਨਿਰਾ);
	gtk_widget_ਸੈੱਟ_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    ਜੇ (shape != MSHAPE_HIDE)
	ਆਖਰੀ_shape = shape;
}

#ਜੇ_ਅੰਤ // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_ਪਾਠ_area_w = 0;
ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_ਪਾਠ_area_h = 0;

/*
 * ============================================================
 * Menu functions
 * ============================================================
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU

# ifdef FEAT_TOOLBAR
/*
 * Icon name table for toolbar buttons.
 * Must match toolbar_names[] in menu.c.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ toolbar_icon_ਨਾਂ[] =
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

    ਸਥਿਰ ਖਾਲੀ
toolbar_button_clicked_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ)
{
    gui_menu_cb((vimmenu_T *)ਅੰਕੜੇ);
}

    ਸਥਿਰ GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = ਨਿਰਾ;

    // Try specified icon file first
    ਜੇ (menu->iconfile != ਨਿਰਾ)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	ਜੇ (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (ਅਚਲ ਅੱਖਰ *)buf, ਨਿਰਾ);

	    ਜੇ (texture != ਨਿਰਾ)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_ਸੈੱਟ_ਮਾਪ_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    ਜੇ (image == ਨਿਰਾ)
    {
	ਅਚਲ ਅੱਖਰ *icon_name = "image-missing";
	ਪੂਰਨ n = (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(toolbar_icon_ਨਾਂ);

	ਜੇ (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_ਨਾਂ[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    ਵਾਪਸ image;
}
# endif

    ਸਥਿਰ ਖਾਲੀ
menu_button_clicked_cb(
	Vim_Menu_ਵਸਤੂ	    *ਵਸਤੂ ਅਣਵਰਤਿਆ,
	Vim_Menu_ਵਸਤੂ_Event    event,
	vimmenu_T	    *menu)
{
    ਜੇ (event == VIM_MENU_ਵਸਤੂ_CLICKED)
	gui_menu_cb(menu);
    ਨਹੀਂਤਾਂ ਜੇ (event == VIM_MENU_ਵਸਤੂ_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	ਸਥਿਰ gboolean did_ਸੁਨੇਹਾ = FALSE;

	ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	    ਵਾਪਸ;

	tooltip = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP]);
	ਜੇ (tooltip != ਨਿਰਾ && utf_valid_string(tooltip, ਨਿਰਾ))
	{
	    ਸੁਨੇਹਾ((ਅੱਖਰ *)tooltip);
	    did_ਸੁਨੇਹਾ = TRUE;
	    setcursor();
	    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
	}
	ਨਹੀਂਤਾਂ ਜੇ (did_ਸੁਨੇਹਾ)
	{
	    ਸੁਨੇਹਾ("");
	    did_ਸੁਨੇਹਾ = FALSE;
	    setcursor();
	    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
	}
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(tooltip);
    }
}

    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu(vimmenu_T *menu, ਪੂਰਨ idx)
{
    vimmenu_T	*ਮਾਪਾ;
    GtkWidget	*ਮਾਪਾ_widget;
    gboolean	use_mnemonic;
    char_u	*ਪਾਠ;

    ਜੇ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_ਸੈੱਟ_ਮਾਪਾ(menu->submenu_id, gui.drawarea);
	ਵਾਪਸ;
    }

    ਮਾਪਾ = menu->ਮਾਪਾ;

    ਜੇ ((ਮਾਪਾ != ਨਿਰਾ && ਮਾਪਾ->submenu_id == ਨਿਰਾ)
	    || !menu_is_menubar(menu->name))
	ਵਾਪਸ;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = ਮਾਪਾ != ਨਿਰਾ || p_wak[0] != 'n';
    ਪਾਠ = translate_mnemonic_ਟੈਗ(menu->name, use_mnemonic);

    ਜੇ (ਮਾਪਾ != ਨਿਰਾ)
    {
	ਮਾਪਾ_widget = ਮਾਪਾ->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_ਵਸਤੂ_new(
		    (ਅਚਲ ਅੱਖਰ *)ਪਾਠ, ਨਿਰਾ, ਨਿਰਾ));

	vim_menu_ਵਸਤੂ_ਸੈੱਟ_submenu(VIM_MENU_ਵਸਤੂ(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_ਪਾਓ_ਵਸਤੂ(VIM_MENU(ਮਾਪਾ_widget),
		VIM_MENU_ਵਸਤੂ(menu->id), idx);
    }
    ਨਹੀਂਤਾਂ
    {
	ਮਾਪਾ_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_ਵਸਤੂ_new(
		    (ਅਚਲ ਅੱਖਰ *)ਪਾਠ, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_ਪਾਓ_ਵਸਤੂ(VIM_MENU_BAR(ਮਾਪਾ_widget),
		VIM_MENU_BAR_ਵਸਤੂ(menu->id), idx);
    }

    vim_free(ਪਾਠ);
}

    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu_ਵਸਤੂ(vimmenu_T *menu, ਪੂਰਨ idx)
{
    vimmenu_T	*ਮਾਪਾ = menu->ਮਾਪਾ;

# ifdef FEAT_TOOLBAR
    ਜੇ (ਮਾਪਾ != ਨਿਰਾ && menu_is_toolbar(ਮਾਪਾ->name))
    {
	ਜੇ (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_ਪਾਓ_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	ਨਹੀਂਤਾਂ
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*ਪਾਠ;
	    char_u	*tooltip;

	    ਪਾਠ    = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->dname);
	    tooltip = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP]);
	    ਜੇ (tooltip != ਨਿਰਾ && !utf_valid_string(tooltip, ਨਿਰਾ))
		CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_ਪਾਓ_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ਅਚਲ ਅੱਖਰ *)ਪਾਠ, idx);
	    gtk_widget_ਸੈੱਟ_tooltip_ਪਾਠ(btn, (ਅਚਲ gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(ਪਾਠ);
	    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(tooltip);
	}
	ਵਾਪਸ;
    }
# endif

    // Menu items (non-toolbar)
    ਜੇ (ਮਾਪਾ == ਨਿਰਾ || ਮਾਪਾ->submenu_id == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_ਪਾਓ_separator(
		VIM_MENU(ਮਾਪਾ->submenu_id), idx));
    }
    ਨਹੀਂਤਾਂ
    {
	char_u	    *ਪਾਠ;
	char_u	    *accel_ਪਾਠ = ਨਿਰਾ;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	ਪਾਠ = translate_mnemonic_ਟੈਗ(menu->name, use_mnemonic);

	ਜੇ (menu->actext != ਨਿਰਾ && menu->actext[0] != NUL)
	    accel_ਪਾਠ = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_ਵਸਤੂ_new((ਅਚਲ ਅੱਖਰ *)ਪਾਠ,
		    (Vim_Menu_ਵਸਤੂ_Func)menu_button_clicked_cb, menu));

	ਜੇ (accel_ਪਾਠ != ਨਿਰਾ)
	    vim_menu_ਵਸਤੂ_ਸੈੱਟ_accel(VIM_MENU_ਵਸਤੂ(menu->id),
		    (ਅਚਲ ਅੱਖਰ *)accel_ਪਾਠ);

	vim_menu_ਪਾਓ_ਵਸਤੂ(VIM_MENU(ਮਾਪਾ->submenu_id),
		VIM_MENU_ਵਸਤੂ(menu->id), idx);

	vim_free(ਪਾਠ);
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(accel_ਪਾਠ);
    }
}

    ਖਾਲੀ
gui_mch_toggle_tearoffs(ਪੂਰਨ enable ਅਣਵਰਤਿਆ)
{
    // GTK4: tearoff menus don't exist.
}

    ਖਾਲੀ
gui_mch_menu_ਸੈੱਟ_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    ਜੇ (menu->id == ਨਿਰਾ)
	ਵਾਪਸ;

    tooltip = CONVERT_ਮੰਜ਼ਿਲ_UTF8(menu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP]);
    ਜੇ (tooltip != ਨਿਰਾ && utf_valid_string(tooltip, ਨਿਰਾ))
	gtk_widget_ਸੈੱਟ_tooltip_ਪਾਠ(menu->id, (ਅਚਲ ਅੱਖਰ *)tooltip);
    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(tooltip);
}

    ਖਾਲੀ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    ਜੇ (menu->ਮਾਪਾ != ਨਿਰਾ && menu_is_toolbar(menu->ਮਾਪਾ->name))
    {
	ਜੇ (menu->id != ਨਿਰਾ)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = ਨਿਰਾ;
	}
	ਵਾਪਸ;
    }

    // For popup menus, unparent the menu as well
    ਜੇ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	ਜੇ (menu->submenu_id != ਨਿਰਾ)
	    gtk_widget_unparent(menu->submenu_id);
    }
    ਨਹੀਂਤਾਂ ਜੇ (menu->ਮਾਪਾ == ਨਿਰਾ && menu->id != ਨਿਰਾ)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    ਨਹੀਂਤਾਂ ਜੇ (menu->id != ਨਿਰਾ)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->ਮਾਪਾ->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    ਖਾਲੀ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ਪੂਰਨ x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    ਸਥਿਰ ਖਾਲੀ
show_menubar_popover(ਖਾਲੀ)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    ਜੇ (gui.menubar == ਨਿਰਾ || gui.drawarea == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), ਨਿਰਾ);
	ਵਾਪਸ;
    }

    // Copy and convert the menubar into a menu popover
    menu = vim_menu_bar_ਮੰਜ਼ਿਲ_menu(VIM_MENU_BAR(gui.menubar));

    gtk_widget_ਸੈੱਟ_ਮਾਪਾ(menu, gui.drawarea);
    gtk_popover_ਸੈੱਟ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    rect.x = 0;
    rect.y = 0;
    rect.ਚੌੜਾਈ = 1;
    rect.ਉਚਾਈ = 1;
    gtk_popover_ਸੈੱਟ_pointing_ਮੰਜ਼ਿਲ(GTK_POPOVER(menu), &rect);
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), ਨਿਰਾ);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#ਜੇ_ਅੰਤ // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_thumb(scrollbar_T *sb, ਲੰਮਾ val, ਲੰਮਾ ਮਾਪ, ਲੰਮਾ max)
{
    GtkAdjustment *adj;

    ਜੇ (sb->id == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	ਵਾਪਸ;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_ਸੈੱਟ_ਹੇਠਲਾ(adj, 0.0);
    gtk_adjustment_ਸੈੱਟ_ਉੱਤਲਾ(adj, (gdouble)max + 1);
    gtk_adjustment_ਸੈੱਟ_ਮੁੱਲ(adj, (gdouble)val);
    gtk_adjustment_ਸੈੱਟ_ਕਦਮ_increment(adj, 1.0);
    gtk_adjustment_ਸੈੱਟ_ਪੰਨਾ_increment(adj, (gdouble)(ਮਾਪ > 2 ? ਮਾਪ - 2 : 1));
    gtk_adjustment_ਸੈੱਟ_ਪੰਨਾ_ਮਾਪ(adj, (gdouble)ਮਾਪ);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_pos(scrollbar_T *sb, ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    ਜੇ (sb->id != ਨਿਰਾ)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    ਪੂਰਨ
gui_mch_get_scrollbar_xpadding(ਖਾਲੀ)
{
    ਪੂਰਨ formwin_w = gtk_widget_get_ਚੌੜਾਈ(gui.formwin);
    ਪੂਰਨ sbar_w = 0;
    ਪੂਰਨ xpad;

    ਜੇ (gui.which_scrollbars[SBAR_ਖੱਬਾ])
	sbar_w += gui.scrollbar_ਚੌੜਾਈ;
    ਜੇ (gui.which_scrollbars[SBAR_ਸੱਜਾ])
	sbar_w += gui.scrollbar_ਚੌੜਾਈ;

    xpad = formwin_w - ਆਖਰੀ_ਪਾਠ_area_w - sbar_w;
    ਵਾਪਸ (xpad < 0) ? 0 : xpad;
}

    ਪੂਰਨ
gui_mch_get_scrollbar_ypadding(ਖਾਲੀ)
{
    ਪੂਰਨ formwin_h = gtk_widget_get_ਉਚਾਈ(gui.formwin);
    ਪੂਰਨ ypad;

    ypad = formwin_h - ਆਖਰੀ_ਪਾਠ_area_h;
    ਜੇ (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_ਉਚਾਈ;

    ਵਾਪਸ (ypad < 0) ? 0 : ypad;
}

    ਸਥਿਰ ਖਾਲੀ
adjustment_ਮੁੱਲ_ਬਦਲਿਆ(GtkAdjustment *adj, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_ਅੰਕੜੇ(G_OBJECT(adj), "vim-sb");
    ਲੰਮਾ ਮੁੱਲ = (ਲੰਮਾ)gtk_adjustment_get_ਮੁੱਲ(adj);

    ਜੇ (sb != ਨਿਰਾ)
	gui_drag_scrollbar(sb, ਮੁੱਲ, FALSE);
}

    ਖਾਲੀ
gui_mch_create_scrollbar(scrollbar_T *sb, ਪੂਰਨ orient)
{
    GtkAdjustment *adj;
    ਜੇ (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ਨਿਰਾ);

    gtk_widget_ਜੋੜੋ_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_ਸੈੱਟ_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_ਮੁੱਲ_ਬਦਲਿਆ), ਨਿਰਾ);
}

    ਖਾਲੀ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ਜੇ (sb->id != ਨਿਰਾ)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = ਨਿਰਾ;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_ਪਾਠ_area_pos(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    ਜੇ (w <= 0 || h <= 0)
	ਵਾਪਸ;
    ਆਖਰੀ_ਪਾਠ_area_w = w;
    ਆਖਰੀ_ਪਾਠ_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    ਖਾਲੀ
gui_gtk4_calculate_bleed(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ)
{
    ਪੂਰਨ old_ਸੱਜਾ = gui.bleed_ਸੱਜਾ;
    ਪੂਰਨ old_bot = gui.bleed_bot;

    gui.bleed_ਸੱਜਾ = ਚੌੜਾਈ - ਆਖਰੀ_ਪਾਠ_area_w;
    gui.bleed_bot = ਉਚਾਈ - ਆਖਰੀ_ਪਾਠ_area_h;

    ਜੇ (gui.which_scrollbars[SBAR_ਖੱਬਾ])
	gui.bleed_ਸੱਜਾ -= gui.scrollbar_ਚੌੜਾਈ;
    ਜੇ (gui.which_scrollbars[SBAR_ਸੱਜਾ])
	gui.bleed_ਸੱਜਾ -= gui.scrollbar_ਚੌੜਾਈ;
    ਜੇ (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_ਉਚਾਈ;

    // Not sure if this can happen, but be safe...
    ਜੇ (gui.bleed_ਸੱਜਾ < 0)
	gui.bleed_ਸੱਜਾ = 0;
    ਜੇ (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    ਜੇ (old_ਸੱਜਾ != gui.bleed_ਸੱਜਾ || old_bot != gui.bleed_bot)
	gtk_widget_ਕਤਾਰ_draw(GTK_WIDGET(gui.drawarea));
}

/*
 * ============================================================
 * Browse dialogs
 * ============================================================
 */

/*
 * Blocking helper: run a GtkFileDialog and wait for result.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    GFile	*ਨਤੀਜਾ;
    gboolean	ਪੂਰਾ;
} File_Dialog_ਅੰਕੜੇ;

    ਸਥਿਰ ਖਾਲੀ
file_dialog_open_cb(GObject *ਸਰੋਤ, GAsync_ਨਤੀਜਾ *res, gpointer ਅੰਕੜੇ)
{
    File_Dialog_ਅੰਕੜੇ *fdd = (File_Dialog_ਅੰਕੜੇ *)ਅੰਕੜੇ;
    fdd->ਨਤੀਜਾ = gtk_file_dialog_open_ਅੰਤ(
		    GTK_FILE_DIALOG(ਸਰੋਤ), res, ਨਿਰਾ);
    fdd->ਪੂਰਾ = TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
file_dialog_save_cb(GObject *ਸਰੋਤ, GAsync_ਨਤੀਜਾ *res, gpointer ਅੰਕੜੇ)
{
    File_Dialog_ਅੰਕੜੇ *fdd = (File_Dialog_ਅੰਕੜੇ *)ਅੰਕੜੇ;
    fdd->ਨਤੀਜਾ = gtk_file_dialog_save_ਅੰਤ(
		    GTK_FILE_DIALOG(ਸਰੋਤ), res, ਨਿਰਾ);
    fdd->ਪੂਰਾ = TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
file_dialog_folder_cb(GObject *ਸਰੋਤ, GAsync_ਨਤੀਜਾ *res, gpointer ਅੰਕੜੇ)
{
    File_Dialog_ਅੰਕੜੇ *fdd = (File_Dialog_ਅੰਕੜੇ *)ਅੰਕੜੇ;
    fdd->ਨਤੀਜਾ = gtk_file_dialog_select_folder_ਅੰਤ(
		    GTK_FILE_DIALOG(ਸਰੋਤ), res, ਨਿਰਾ);
    fdd->ਪੂਰਾ = TRUE;
}

    char_u *
gui_mch_browse(ਪੂਰਨ saving,
	char_u *title,
	char_u *dflt,
	char_u *ext ਅਣਵਰਤਿਆ,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    File_Dialog_ਅੰਕੜੇ  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *ਨਤੀਜਾ = ਨਿਰਾ;
    GListStore	    *filters;

    title = CONVERT_ਮੰਜ਼ਿਲ_UTF8(title);

    ਜੇ (initdir == ਨਿਰਾ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ਨਹੀਂਤਾਂ ਜੇ (vim_ਭਰਿਆ_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    ਜੋੜੋ_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_ਸੈੱਟ_modal(dlg, TRUE);
    ਜੇ (title != ਨਿਰਾ)
	gtk_file_dialog_ਸੈੱਟ_title(dlg, (ਅਚਲ ਅੱਖਰ *)title);

    {
	GFile *dir = g_file_new_for_ਰਸਤਾ((ਅਚਲ ਅੱਖਰ *)dirbuf);
	gtk_file_dialog_ਸੈੱਟ_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    ਜੇ (filter != ਨਿਰਾ && *filter != NUL)
    {
	ਪੂਰਨ		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ਜੇ (patt != ਨਿਰਾ)
	{
	    ਜਦੋਂ (p != ਨਿਰਾ && *p != NUL)
	    {
		ਜੇ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ਜੇ (*p == '\t')
			gtk_file_filter_ਸੈੱਟ_name(gfilter, (gchar *)patt);
		    ਨਹੀਂਤਾਂ
		    {
			gtk_file_filter_ਜੋੜੋ_ਨਮੂਨਾ(gfilter, (gchar *)patt);
			ਜੇ (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    ਜੇ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		ਨਹੀਂਤਾਂ
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    gtk_file_dialog_ਸੈੱਟ_filters(dlg, G_LIST_MODEL(filters));
    g_object_unref(filters);

    ਜੇ (saving && dflt != ਨਿਰਾ && *dflt != NUL)
	gtk_file_dialog_ਸੈੱਟ_initial_name(dlg, (ਅਚਲ ਅੱਖਰ *)dflt);

    fdd.ਨਤੀਜਾ = ਨਿਰਾ;
    fdd.ਪੂਰਾ = FALSE;

    ਜੇ (saving)
	gtk_file_dialog_save(dlg, GTK_ਖਿੜਕੀ(gui.mainwin), ਨਿਰਾ,
		file_dialog_save_cb, &fdd);
    ਨਹੀਂਤਾਂ
	gtk_file_dialog_open(dlg, GTK_ਖਿੜਕੀ(gui.mainwin), ਨਿਰਾ,
		file_dialog_open_cb, &fdd);

    ਜਦੋਂ (!fdd.ਪੂਰਾ)
	g_main_context_iteration(ਨਿਰਾ, TRUE);

    ਜੇ (fdd.ਨਤੀਜਾ != ਨਿਰਾ)
    {
	ਅੱਖਰ *ਰਸਤਾ = g_file_get_ਰਸਤਾ(fdd.ਨਤੀਜਾ);
	ਜੇ (ਰਸਤਾ != ਨਿਰਾ)
	{
	    ਨਤੀਜਾ = vim_strsave((char_u *)ਰਸਤਾ);
	    g_free(ਰਸਤਾ);
	}
	g_object_unref(fdd.ਨਤੀਜਾ);
    }

    g_object_unref(dlg);
    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(title);

    ਵਾਪਸ ਨਤੀਜਾ;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    File_Dialog_ਅੰਕੜੇ	fdd;
    char_u		*ਨਤੀਜਾ = ਨਿਰਾ;

    title = CONVERT_ਮੰਜ਼ਿਲ_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_ਸੈੱਟ_modal(dlg, TRUE);
    ਜੇ (title != ਨਿਰਾ)
	gtk_file_dialog_ਸੈੱਟ_title(dlg, (ਅਚਲ ਅੱਖਰ *)title);

    ਜੇ (initdir != ਨਿਰਾ && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_ਰਸਤਾ((ਅਚਲ ਅੱਖਰ *)initdir);
	gtk_file_dialog_ਸੈੱਟ_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.ਨਤੀਜਾ = ਨਿਰਾ;
    fdd.ਪੂਰਾ = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_ਖਿੜਕੀ(gui.mainwin), ਨਿਰਾ,
	    file_dialog_folder_cb, &fdd);

    ਜਦੋਂ (!fdd.ਪੂਰਾ)
	g_main_context_iteration(ਨਿਰਾ, TRUE);

    ਜੇ (fdd.ਨਤੀਜਾ != ਨਿਰਾ)
    {
	ਅੱਖਰ *ਰਸਤਾ = g_file_get_ਰਸਤਾ(fdd.ਨਤੀਜਾ);
	ਜੇ (ਰਸਤਾ != ਨਿਰਾ)
	{
	    ਨਤੀਜਾ = vim_strsave((char_u *)ਰਸਤਾ);
	    g_free(ਰਸਤਾ);
	}
	g_object_unref(fdd.ਨਤੀਜਾ);
    }

    g_object_unref(dlg);
    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(title);

    ਵਾਪਸ ਨਤੀਜਾ;
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
    ਸਥਿਰ ਅੱਖਰ **
ਵੰਡੋ_button_string(char_u *button_string, ਪੂਰਨ *n_buttons, ਅੱਖਰ **tofree)
{
    ਅੱਖਰ	    **array;
    char_u	    *p;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    ਗਿਣਤੀ_2 = 1;

    button_string = (char_u *)g_strdup((ਅਚਲ ਅੱਖਰ *)button_string);

    ਲਈ (p = button_string; *p != NUL; ++p)
	ਜੇ (*p == DLG_BUTTON_SEP)
	    ++ਗਿਣਤੀ_2;

    array = g_malloc_n(ਗਿਣਤੀ_2, ਆਕਾਰ(ਅੱਖਰ *));
    ਗਿਣਤੀ_2 = 0;

    ਜੇ (array != ਨਿਰਾ)
    {
	array[ਗਿਣਤੀ_2++] = (ਅੱਖਰ *)button_string;
	ਲਈ (p = button_string; *p != NUL; )
	{
	    ਜੇ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[ਗਿਣਤੀ_2++] = (ਅੱਖਰ *)p;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ਨਹੀਂਤਾਂ
		MB_PTR_ADV(p);
	}
    }

    *tofree = (ਅੱਖਰ *)button_string;
    *n_buttons = ਗਿਣਤੀ_2;
    ਵਾਪਸ array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ *
dialog_type_ਮੰਜ਼ਿਲ_icon(ਪੂਰਨ type)
{
    ਚੋਣ (type)
    {
	ਹਾਲਤ VIM_ERROR:
	    ਵਾਪਸ "dialog-error-symbolic";
	ਹਾਲਤ VIM_WARNING:
	    ਵਾਪਸ "dialog-warning-symbolic";
	ਹਾਲਤ VIM_INFO:
	    ਵਾਪਸ "dialog-information-symbolic";
	ਹਾਲਤ VIM_QUESTION:
	    ਵਾਪਸ "dialog-question-symbolic";
	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

// Data associated with each button in the dialog
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		but_idx;
    ਪੂਰਨ		*response;
    gboolean	*ਪੂਰਾ;
} DialogButtonState;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    GtkWidget	*win;
    gboolean	*ਪੂਰਾ;
    gboolean	no_alt;
} DialogState;

    ਸਥਿਰ ਖਾਲੀ
dialog_button_clicked_cb(GtkButton *button ਅਣਵਰਤਿਆ, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->ਪੂਰਾ = TRUE;
}

    ਸਥਿਰ gboolean
dialog_ਕੁੰਜੀ_pressed_cb(
	Gtk_Event_Controller_ਕੁੰਜੀ	*controller ਅਣਵਰਤਿਆ,
	guint			keyval,
	guint			keycode ਅਣਵਰਤਿਆ,
	GdkModifierType		state,
	DialogState		*dstate)
{
    ਜੇ (keyval == GDK_ਕੁੰਜੀ_Escape)
    {
	*dstate->ਪੂਰਾ = TRUE;
	ਵਾਪਸ TRUE;
    }

    ਜੇ (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_ਨਕਾਬ()))
	ਵਾਪਸ gtk_widget_mnemonic_activate(dstate->win, FALSE);
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ gboolean
dialog_close_request_cb(Gtk_ਖਿੜਕੀ *win ਅਣਵਰਤਿਆ, gboolean *win_closed)
{
    *win_closed = TRUE;
    ਵਾਪਸ FALSE;
}

    ਪੂਰਨ
gui_mch_dialog(
	ਪੂਰਨ	type,
	char_u	*title,
	char_u	*ਸੁਨੇਹਾ_2,
	char_u	*buttons,
	ਪੂਰਨ	def_but,
	char_u	*textfield,
	ਪੂਰਨ	ex_cmd ਅਣਵਰਤਿਆ)
{
    Gtk_ਖਿੜਕੀ		*win = GTK_ਖਿੜਕੀ(gtk_ਖਿੜਕੀ_new());
    GtkWidget		*vertbox;
    GtkWidget		*ਸੁਨੇਹਾ_box;
    ਅਚਲ ਅੱਖਰ		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*ਇੰਦਰਾਜ = ਨਿਰਾ;
    char_u		*utf8_title;
    char_u		*utf8_ਸੁਨੇਹਾ;
    GtkEventController *ਕੁੰਜੀ_controller;
    DialogButtonState	*but_ਹਾਲਤਾਂ = ਨਿਰਾ;
    ਅੱਖਰ		*tofree = ਨਿਰਾ;
    ਪੂਰਨ			response = -1;
    gboolean		ਪੂਰਾ = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_ਮੰਜ਼ਿਲ_UTF8(title);
    ਜੇ (utf8_title != ਨਿਰਾ)
	gtk_ਖਿੜਕੀ_ਸੈੱਟ_title(win, (ਅਚਲ ਅੱਖਰ *)utf8_title);
    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(utf8_title);

    gtk_ਖਿੜਕੀ_ਸੈੱਟ_transient_for(win, GTK_ਖਿੜਕੀ(gui.mainwin));
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_modal(win, TRUE);
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_default_ਮਾਪ(win, 300, -1);
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_destroy_with_ਮਾਪਾ(win, TRUE);
    g_signal_connect(win, "close-request",
	    G_CALLBACK(dialog_close_request_cb), &win_closed);

    // Create main vertical layout container
    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_ਬੱਚਾ(win, vertbox);
    gtk_widget_ਸੈੱਟ_margin_ਸਿਖਰ(vertbox, 24);
    gtk_widget_ਸੈੱਟ_margin_bottom(vertbox, 24);
    gtk_widget_ਸੈੱਟ_margin_ਸ਼ੁਰੂ(vertbox, 12);
    gtk_widget_ਸੈੱਟ_margin_ਸਿਰਾ(vertbox, 12);

    // Add the message label
    ਸੁਨੇਹਾ_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_ਸੈੱਟ_halign(ਸੁਨੇਹਾ_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), ਸੁਨੇਹਾ_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_ਮੰਜ਼ਿਲ_icon(type);
    ਜੇ (icon_name != ਨਿਰਾ)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_ਸੈੱਟ_icon_ਮਾਪ(GTK_IMAGE(icon), GTK_ICON_ਮਾਪ_LARGE);
	gtk_box_append(GTK_BOX(ਸੁਨੇਹਾ_box), icon);
    }

    utf8_ਸੁਨੇਹਾ = CONVERT_ਮੰਜ਼ਿਲ_UTF8(ਸੁਨੇਹਾ_2);
    label = gtk_label_new((ਅਚਲ ਅੱਖਰ *)utf8_ਸੁਨੇਹਾ);
    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(utf8_ਸੁਨੇਹਾ);
    gtk_label_ਸੈੱਟ_wrap(GTK_LABEL(label), TRUE);
    gtk_label_ਸੈੱਟ_max_ਚੌੜਾਈ_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(ਸੁਨੇਹਾ_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    ਕੁੰਜੀ_controller = gtk_event_controller_ਕੁੰਜੀ_new();
    g_signal_connect(ਕੁੰਜੀ_controller, "key-pressed",
	    G_CALLBACK(dialog_ਕੁੰਜੀ_pressed_cb), &state);
    gtk_widget_ਜੋੜੋ_controller(GTK_WIDGET(win), ਕੁੰਜੀ_controller);
    state.ਪੂਰਾ = &ਪੂਰਾ;
    state.win = GTK_WIDGET(win);

    ਜੇ (textfield != ਨਿਰਾ)
    {
	// Add text entry so user can enter text
	char_u *utf8_ਪਾਠ = CONVERT_ਮੰਜ਼ਿਲ_UTF8(textfield);

	ਇੰਦਰਾਜ = gtk_ਇੰਦਰਾਜ_new();

	ਜੇ (utf8_ਪਾਠ != ਨਿਰਾ)
	    gtk_editable_ਸੈੱਟ_ਪਾਠ(GTK_EDITABLE(ਇੰਦਰਾਜ), (ਅਚਲ ਅੱਖਰ *)utf8_ਪਾਠ);
	ਨਹੀਂਤਾਂ
	    gtk_editable_ਸੈੱਟ_ਪਾਠ(GTK_EDITABLE(ਇੰਦਰਾਜ), "");
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(utf8_ਪਾਠ);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_ਇੰਦਰਾਜ_ਸੈੱਟ_activates_default(GTK_ਇੰਦਰਾਜ(ਇੰਦਰਾਜ), TRUE);
	gtk_box_append(GTK_BOX(vertbox), ਇੰਦਰਾਜ);
	state.no_alt = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	GListModel  *controllers;
	ਪੂਰਨ	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_ਚੀਜ਼ਾਂ(controllers);
	ਲਈ (ਪੂਰਨ i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_ਵਸਤੂ(controllers, i);
	    ਜੇ (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_ਸੈੱਟ_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_ਖਿੜਕੀ_ਸੈੱਟ_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    ਜੇ (buttons != ਨਿਰਾ)
    {
	GtkWidget   *but_box;
	ਅੱਖਰ	    **buttons_arr; // Note that array is allocated, not strings
	ਪੂਰਨ	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	ਜੇ (vim_strchr(p_go, GO_VERTICAL) != ਨਿਰਾ)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	ਨਹੀਂਤਾਂ
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_ਸੈੱਟ_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_ਸੈੱਟ_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = ਵੰਡੋ_button_string(buttons, &n_buttons, &tofree);

	but_ਹਾਲਤਾਂ = g_malloc_n(n_buttons, ਆਕਾਰ(DialogButtonState));

	ਲਈ (ਪੂਰਨ i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_ਹਾਲਤਾਂ + i;

	    but_label = CONVERT_ਮੰਜ਼ਿਲ_UTF8((char_u *)buttons_arr[i]);
	    ਜੇ (but_label == ਨਿਰਾ)
		ਜਾਰੀ;

	    but = gtk_button_new_with_mnemonic((ਅੱਖਰ *)but_label);
	    ਜੇ (i == def_but - 1)
		gtk_ਖਿੜਕੀ_ਸੈੱਟ_default_widget(win, but);
	    gtk_box_append(GTK_BOX(but_box), but);
	    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(but_label);

	    but_state->but_idx = i;
	    but_state->response = &response;
	    but_state->ਪੂਰਾ = &ਪੂਰਾ;

	    g_signal_connect(but, "clicked",
		    G_CALLBACK(dialog_button_clicked_cb), but_state);
	}
	g_free(buttons_arr);
    }

    gtk_ਖਿੜਕੀ_present(win);

    ਜਦੋਂ (!ਪੂਰਾ && !win_closed)
	g_main_context_iteration(ਨਿਰਾ, TRUE);

    ਜੇ (ਪੂਰਾ)
    {
	ਜੇ (textfield != ਨਿਰਾ)
	{
	    // Get the text the user entered
	    char_u *ਪਾਠ;

	    ਪਾਠ = (char_u *)gtk_editable_get_ਪਾਠ(GTK_EDITABLE(ਇੰਦਰਾਜ));
	    ਪਾਠ = CONVERT_FROM_UTF8(ਪਾਠ);
	    vim_strncpy(textfield, ਪਾਠ, IOSIZE - 1);
	    CONVERT_FROM_UTF8_FREE(ਪਾਠ);
	}

	gtk_ਖਿੜਕੀ_destroy(win);
    }
    g_free(but_ਹਾਲਤਾਂ);
    g_free(tofree);

    // Vim buttons are indexed starting from one.
    ਵਾਪਸ response == -1 ? 0 : response + 1;
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    GtkWidget *dialog;
    GtkWidget *what;	    // Find what entry
    GtkWidget *with;	    // Replace with entry
    GtkWidget *wword;	    // Whole word check
    GtkWidget *mcase;	    // Match case check
    GtkWidget *ਉੱਪਰ;	    // Direction up radio
    GtkWidget *ਥੱਲੇ;	    // Direction down radio
    GtkWidget *ਲੱਭੋ;	    // 'Find Next' action button
    GtkWidget *replace;	    // 'Replace With' action button
    GtkWidget *all;	    // 'Replace All' action button
} Shared_ਲੱਭੋ_Replace_2;

ਸਥਿਰ Shared_ਲੱਭੋ_Replace_2 ਲੱਭੋ_widgets = {0};
ਸਥਿਰ Shared_ਲੱਭੋ_Replace_2 repl_widgets = {0};

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_replace_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ)
{
    ਪੂਰਨ			flags;
    char_u		*ਲੱਭੋ_ਪਾਠ;
    char_u		*repl_ਪਾਠ;
    gboolean		direction_ਥੱਲੇ;
    Shared_ਲੱਭੋ_Replace_2	*sfr;

    flags = GPOINTER_ਮੰਜ਼ਿਲ_INT(ਅੰਕੜੇ);

    ਜੇ (flags == FRD_FINDNEXT)
    {
	repl_ਪਾਠ = ਨਿਰਾ;
	sfr = &ਲੱਭੋ_widgets;
    }
    ਨਹੀਂਤਾਂ
    {
	repl_ਪਾਠ = (char_u *)gtk_editable_get_ਪਾਠ(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    ਲੱਭੋ_ਪਾਠ = (char_u *)gtk_editable_get_ਪਾਠ(GTK_EDITABLE(sfr->what));
    direction_ਥੱਲੇ = gtk_check_button_get_ਸਰਗਰਮ(
	    GTK_CHECK_BUTTON(sfr->ਥੱਲੇ));

    ਜੇ (gtk_check_button_get_ਸਰਗਰਮ(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_ਸਾਰਾ_ਸ਼ਬਦ;
    ਜੇ (gtk_check_button_get_ਸਰਗਰਮ(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_ਮੇਲ_CASE;

    repl_ਪਾਠ = CONVERT_FROM_UTF8(repl_ਪਾਠ);
    ਲੱਭੋ_ਪਾਠ = CONVERT_FROM_UTF8(ਲੱਭੋ_ਪਾਠ);
    gui_do_findrepl(flags, ਲੱਭੋ_ਪਾਠ, repl_ਪਾਠ, direction_ਥੱਲੇ);
    CONVERT_FROM_UTF8_FREE(repl_ਪਾਠ);
    CONVERT_FROM_UTF8_FREE(ਲੱਭੋ_ਪਾਠ);
}

    ਸਥਿਰ ਖਾਲੀ
dialog_destroyed_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ)
{
    *(GtkWidget **)ਅੰਕੜੇ = ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
ਇੰਦਰਾਜ_ਬਦਲਿਆ_cb(GtkWidget *ਇੰਦਰਾਜ, GtkWidget *dialog)
{
    ਅਚਲ gchar	*ਇੰਦਰਾਜ_ਪਾਠ;
    gboolean	nonempty;

    ਇੰਦਰਾਜ_ਪਾਠ = gtk_editable_get_ਪਾਠ(GTK_EDITABLE(ਇੰਦਰਾਜ));

    ਜੇ (!ਇੰਦਰਾਜ_ਪਾਠ)
	ਵਾਪਸ; // Shouldn't happen

    nonempty = (ਇੰਦਰਾਜ_ਪਾਠ[0] != '\0');

    ਜੇ (dialog == ਲੱਭੋ_widgets.dialog)
	gtk_widget_ਸੈੱਟ_sensitive(ਲੱਭੋ_widgets.ਲੱਭੋ, nonempty);

    ਜੇ (dialog == repl_widgets.dialog)
    {
	gtk_widget_ਸੈੱਟ_sensitive(repl_widgets.ਲੱਭੋ, nonempty);
	gtk_widget_ਸੈੱਟ_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_ਸੈੱਟ_sensitive(repl_widgets.all, nonempty);
    }
}

    ਸਥਿਰ gboolean
ਲੱਭੋ_ਕੁੰਜੀ_pressed_cb(
	Gtk_Event_Controller_ਕੁੰਜੀ	*controller ਅਣਵਰਤਿਆ,
	guint			keyval,
	guint			keycode ਅਣਵਰਤਿਆ,
	GdkModifierType		state,
	Shared_ਲੱਭੋ_Replace_2	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ਜੇ (state & (GDK_CONTROL_ਨਕਾਬ | GDK_ਖਿਸਕਾਓ_ਨਕਾਬ))
	ਵਾਪਸ FALSE;

    // the Escape key synthesizes a cancellation action
    ਜੇ (keyval == GDK_ਕੁੰਜੀ_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_ਖਿੜਕੀ_destroy(GTK_ਖਿੜਕੀ(frdp->dialog));
	ਵਾਪਸ TRUE;
    }

    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
ਇੰਦਰਾਜ_activate_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, ਖਾਲੀ *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_replace_dialog_create(char_u *arg, ਪੂਰਨ do_replace)
{
    Shared_ਲੱਭੋ_Replace_2	*frdp;
    char_u		*ਇੰਦਰਾਜ_ਪਾਠ;
    ਪੂਰਨ			wword = FALSE;
    ਪੂਰਨ			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*ਕੁੰਜੀ_controller;

    frdp = do_replace ? &repl_widgets : &ਲੱਭੋ_widgets;
    ਇੰਦਰਾਜ_ਪਾਠ = get_ਲੱਭੋ_dialog_ਪਾਠ(arg, &wword, &mcase);

    ਜੇ (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = ਇੰਦਰਾਜ_ਪਾਠ;
	ਇੰਦਰਾਜ_ਪਾਠ = string_convert(&output_conv, ਇੰਦਰਾਜ_ਪਾਠ, ਨਿਰਾ);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    ਜੇ (frdp->dialog != ਨਿਰਾ)
    {
	ਜੇ (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ)
	{
	    gtk_editable_ਸੈੱਟ_ਪਾਠ(GTK_EDITABLE(frdp->what),
		    (ਅੱਖਰ *)ਇੰਦਰਾਜ_ਪਾਠ);
	    gtk_check_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_CHECK_BUTTON(frdp->wword),
		    (gboolean)wword);
	    gtk_check_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_CHECK_BUTTON(frdp->mcase),
		    (gboolean)mcase);
	}
	gtk_ਖਿੜਕੀ_present(GTK_ਖਿੜਕੀ(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	ਜੇ (do_replace && g_utf8_strlen(
		    gtk_editable_get_ਪਾਠ(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(ਇੰਦਰਾਜ_ਪਾਠ);
	ਵਾਪਸ;
    }

    // Create a new dialog window.
    frdp->dialog = gtk_ਖਿੜਕੀ_new();
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_transient_for(GTK_ਖਿੜਕੀ(frdp->dialog),
	    GTK_ਖਿੜਕੀ(gui.mainwin));
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_destroy_with_ਮਾਪਾ(GTK_ਖਿੜਕੀ(frdp->dialog), TRUE);
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_title(GTK_ਖਿੜਕੀ(frdp->dialog),
	    do_replace ? _("VIM - Search and Replace...")
	    : _("VIM - Search..."));
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_resizable(GTK_ਖਿੜਕੀ(frdp->dialog), FALSE);

    g_signal_connect(frdp->dialog, "destroy",
	    G_CALLBACK(dialog_destroyed_cb), &frdp->dialog);

    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_ਸੈੱਟ_margin_ਸ਼ੁਰੂ(vertbox, 12);
    gtk_widget_ਸੈੱਟ_margin_ਸਿਰਾ(vertbox, 12);
    gtk_widget_ਸੈੱਟ_margin_ਸਿਖਰ(vertbox, 12);
    gtk_widget_ਸੈੱਟ_margin_bottom(vertbox, 12);
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_ਬੱਚਾ(GTK_ਖਿੜਕੀ(frdp->dialog), vertbox);

    // Grid for labels + entries
    grid = gtk_grid_new();
    gtk_grid_ਸੈੱਟ_ਪੰਕਤੀ_spacing(GTK_GRID(grid), 6);
    gtk_grid_ਸੈੱਟ_ਕਾਲਮ_spacing(GTK_GRID(grid), 6);
    gtk_box_append(GTK_BOX(vertbox), grid);

    // "Find what:" label + entry
    tmp = gtk_label_new(_("Find what:"));
    gtk_label_ਸੈੱਟ_xalign(GTK_LABEL(tmp), 0.0);
    gtk_grid_attach(GTK_GRID(grid), tmp, 0, 0, 1, 1);

    frdp->what = gtk_ਇੰਦਰਾਜ_new();
    gtk_widget_ਸੈੱਟ_hexpand(frdp->what, TRUE);
    sensitive = (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ && ਇੰਦਰਾਜ_ਪਾਠ[0] != NUL);
    ਜੇ (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ)
	gtk_editable_ਸੈੱਟ_ਪਾਠ(GTK_EDITABLE(frdp->what), (ਅੱਖਰ *)ਇੰਦਰਾਜ_ਪਾਠ);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    ਜੇ (do_replace)
    {
	// "Replace with:" label + entry
	tmp = gtk_label_new(_("Replace with:"));
	gtk_label_ਸੈੱਟ_xalign(GTK_LABEL(tmp), 0.0);
	gtk_grid_attach(GTK_GRID(grid), tmp, 0, 1, 1, 1);

	frdp->with = gtk_ਇੰਦਰਾਜ_new();
	gtk_widget_ਸੈੱਟ_hexpand(frdp->with, TRUE);
	gtk_grid_attach(GTK_GRID(grid), frdp->with, 1, 1, 1, 1);
	gtk_ਇੰਦਰਾਜ_ਸੈੱਟ_activates_default(GTK_ਇੰਦਰਾਜ(frdp->with), TRUE);

	// Make the entry activation only change the input focus onto the
	// with item.
	gtk_ਇੰਦਰਾਜ_ਸੈੱਟ_activates_default(GTK_ਇੰਦਰਾਜ(frdp->what), FALSE);
	g_signal_connect(G_OBJECT(frdp->what), "activate",
		G_CALLBACK(ਇੰਦਰਾਜ_activate_cb), frdp->with);
    }
    ਨਹੀਂਤਾਂ
	// Make the entry activation do the search.
	gtk_ਇੰਦਰਾਜ_ਸੈੱਟ_activates_default(GTK_ਇੰਦਰਾਜ(frdp->what), TRUE);

    // Checkboxes
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    frdp->wword = gtk_check_button_new_with_label(_("Match whole word only"));
    gtk_check_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_CHECK_BUTTON(frdp->wword),
	    (gboolean)wword);
    gtk_box_append(GTK_BOX(hbox), frdp->wword);

    frdp->mcase = gtk_check_button_new_with_label(_("Match case"));
    gtk_check_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_CHECK_BUTTON(frdp->mcase),
	    (gboolean)mcase);
    gtk_box_append(GTK_BOX(hbox), frdp->mcase);

    // Direction radio buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    tmp = gtk_label_new(_("Direction:"));
    gtk_box_append(GTK_BOX(hbox), tmp);

    frdp->ਉੱਪਰ = gtk_check_button_new_with_label(_("Up"));
    gtk_box_append(GTK_BOX(hbox), frdp->ਉੱਪਰ);

    frdp->ਥੱਲੇ = gtk_check_button_new_with_label(_("Down"));
    gtk_check_button_ਸੈੱਟ_ਸਮੂਹ(GTK_CHECK_BUTTON(frdp->ਥੱਲੇ),
	    GTK_CHECK_BUTTON(frdp->ਉੱਪਰ));
    gtk_check_button_ਸੈੱਟ_ਸਰਗਰਮ(GTK_CHECK_BUTTON(frdp->ਥੱਲੇ), TRUE);
    gtk_box_append(GTK_BOX(hbox), frdp->ਥੱਲੇ);

    // Action buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_ਸੈੱਟ_halign(hbox, GTK_ALIGN_ਸਿਰਾ);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    btn = gtk_button_new_with_label(_("Find Next"));
    gtk_widget_ਸੈੱਟ_sensitive(btn, sensitive);
    frdp->ਲੱਭੋ = btn;

    ਕੁੰਜੀ_controller = gtk_event_controller_ਕੁੰਜੀ_new();
    g_signal_connect(ਕੁੰਜੀ_controller, "key-pressed",
	    G_CALLBACK(ਲੱਭੋ_ਕੁੰਜੀ_pressed_cb), frdp);
    gtk_widget_ਜੋੜੋ_controller(GTK_WIDGET(frdp->dialog), ਕੁੰਜੀ_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(ਇੰਦਰਾਜ_ਬਦਲਿਆ_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_default_widget(GTK_ਖਿੜਕੀ(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(ਲੱਭੋ_replace_cb),
	    GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    ਜੇ (do_replace)
    {
	btn = gtk_button_new_with_label(_("Replace"));
	gtk_widget_ਸੈੱਟ_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(ਲੱਭੋ_replace_cb),
		GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(FRD_REPLACE));
	frdp->replace = btn;
	gtk_box_append(GTK_BOX(hbox), btn);

	btn = gtk_button_new_with_label(_("Replace All"));
	gtk_widget_ਸੈੱਟ_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(ਲੱਭੋ_replace_cb),
		GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(FRD_REPLACEALL));
	frdp->all = btn;
	gtk_box_append(GTK_BOX(hbox), btn);
    }

    btn = gtk_button_new_with_label(_("Close"));
    g_signal_connect_swapped(btn, "clicked",
	    G_CALLBACK(gtk_ਖਿੜਕੀ_destroy), frdp->dialog);
    gtk_box_append(GTK_BOX(hbox), btn);

    gtk_ਖਿੜਕੀ_present(GTK_ਖਿੜਕੀ(frdp->dialog));
    gtk_widget_grab_focus(frdp->what);
    ਜੇ (do_replace && ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ && ਇੰਦਰਾਜ_ਪਾਠ[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(ਇੰਦਰਾਜ_ਪਾਠ);
}

    ਖਾਲੀ
gui_mch_ਲੱਭੋ_dialog(exarg_T *eap)
{
    ਜੇ (gui.ਵਿੱਚ_use)
	ਲੱਭੋ_replace_dialog_create(eap->arg, FALSE);
}

    ਖਾਲੀ
gui_mch_replace_dialog(exarg_T *eap)
{
    ਜੇ (gui.ਵਿੱਚ_use)
	ਲੱਭੋ_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    ਖਾਲੀ
ex_helpfind(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

/*
 * ============================================================
 * Native print dialog
 * ============================================================
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK4_PRINT_DIALOG
ਸਥਿਰ GtkPrintDialog *print_dialog = ਨਿਰਾ;
ਸਥਿਰ GtkPrintSettings *print_settings = ਨਿਰਾ;
ਸਥਿਰ Gtk_ਪੰਨਾ_ਤਿਆਰੀ *ਪੰਨਾ_ਤਿਆਰੀ = ਨਿਰਾ;
ਸਥਿਰ Gtk_Print_ਤਿਆਰੀ *print_ਤਿਆਰੀ = ਨਿਰਾ;
ਸਥਿਰ GFile *print_file;

    ਸਥਿਰ ਖਾਲੀ
print_dialog_cb(GtkPrintDialog *dialog, GAsync_ਨਤੀਜਾ *ਨਤੀਜਾ, gboolean *ਪੂਰਾ)
{
    GError *error = ਨਿਰਾ;
    print_ਤਿਆਰੀ = gtk_print_dialog_ਤਿਆਰੀ_ਅੰਤ(dialog, ਨਤੀਜਾ, &error);

    ਜੇ (print_ਤਿਆਰੀ == ਨਿਰਾ)
    {
	semsg(_(e_print_error_str), error->ਸੁਨੇਹਾ_2);
	g_error_free(error);
    }
    *ਪੂਰਾ = TRUE;
}

/*
 * Return file to write to on success (allocated string), or NULL on failure
 * (e.g. user cancelled)
 */
    char_u *
gui_gtk4_print_dialog(
	prt_settings_T	*psettings,
	char_u		*jobname,
	ਦੁੱਗਣਾ		*ਪੰਨਾ_ਚੌੜਾਈ,
	ਦੁੱਗਣਾ		*ਪੰਨਾ_ਉਚਾਈ)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    ਪੂਰਾ = FALSE;
    char_u	    *ਆਰਜ਼ੀ;

    gtk_print_dialog_ਸੈੱਟ_title(dialog, (ਅਚਲ ਅੱਖਰ *)jobname);
    gtk_print_dialog_ਸੈੱਟ_modal(dialog, TRUE);

    print_dialog = ਨਿਰਾ;
    print_ਤਿਆਰੀ = ਨਿਰਾ;

    ਜੇ (print_settings == ਨਿਰਾ)
    {
	option_ਸਾਰਣੀ_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	ਜੇ (opt->present)
	{
	    ਜੇ (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_ਸੈੱਟ_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_ਸੈੱਟ_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    ਨਹੀਂਤਾਂ
		gtk_print_settings_ਸੈੱਟ_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	ਜੇ (opt->present)
	{
	    ਜੇ (*opt->string == 'y')
		gtk_print_settings_ਸੈੱਟ_orientation(print_settings,
			GTK_ਪੰਨਾ_ORIENTATION_PORTRAIT);
	    ਨਹੀਂਤਾਂ
		gtk_print_settings_ਸੈੱਟ_orientation(print_settings,
			GTK_ਪੰਨਾ_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	ਜੇ (opt->present)
	{
	    gtk_print_settings_ਸੈੱਟ_paper_ਮਾਪ(print_settings,
		    gtk_paper_ਮਾਪ_new((ਅਚਲ ਅੱਖਰ *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	ਜੇ (opt->present)
	{
	    ਜੇ (*opt->string == 'y')
		gtk_print_settings_ਸੈੱਟ_collate(print_settings, TRUE);
	    ਨਹੀਂਤਾਂ
		gtk_print_settings_ਸੈੱਟ_collate(print_settings, FALSE);
	}
    }

    ਜੇ (ਪੰਨਾ_ਤਿਆਰੀ == ਨਿਰਾ)
	ਪੰਨਾ_ਤਿਆਰੀ = gtk_ਪੰਨਾ_ਤਿਆਰੀ_new();

    gtk_print_dialog_ਸੈੱਟ_print_settings(dialog, print_settings);
    gtk_print_dialog_ਸੈੱਟ_ਪੰਨਾ_ਤਿਆਰੀ(dialog, ਪੰਨਾ_ਤਿਆਰੀ);

    gtk_print_dialog_ਤਿਆਰੀ(dialog, GTK_ਖਿੜਕੀ(gui.mainwin), ਨਿਰਾ,
	    (GAsyncReadyCallback)print_dialog_cb, &ਪੂਰਾ);

    ਜਦੋਂ (!ਪੂਰਾ)
	g_main_context_iteration(ਨਿਰਾ, TRUE);

    ਜੇ (print_ਤਿਆਰੀ == ਨਿਰਾ)
    {
	g_object_unref(dialog);
	g_clear_ਸੂਚਕ(&print_ਤਿਆਰੀ, gtk_print_ਤਿਆਰੀ_unref);

	psettings->user_abort_ਸੁਨੇਹਾ = FALSE;
	ਵਾਪਸ ਨਿਰਾ;
    }

    g_clear_object(&print_settings);
    g_clear_object(&ਪੰਨਾ_ਤਿਆਰੀ);
    print_settings = g_object_ref(
	    gtk_print_ਤਿਆਰੀ_get_print_settings(print_ਤਿਆਰੀ));
    ਪੰਨਾ_ਤਿਆਰੀ = g_object_ref(gtk_print_ਤਿਆਰੀ_get_ਪੰਨਾ_ਤਿਆਰੀ(print_ਤਿਆਰੀ));

    {
	GtkPrintDuplex duplex = gtk_print_settings_get_duplex(print_settings);
	psettings->duplex = duplex != GTK_PRINT_DUPLEX_SIMPLEX;
    }

    psettings->has_color = gtk_print_settings_get_use_color(print_settings);

    *ਪੰਨਾ_ਚੌੜਾਈ = gtk_ਪੰਨਾ_ਤਿਆਰੀ_get_paper_ਚੌੜਾਈ(ਪੰਨਾ_ਤਿਆਰੀ,
	    GTK_UNIT_POINTS);
    *ਪੰਨਾ_ਉਚਾਈ = gtk_ਪੰਨਾ_ਤਿਆਰੀ_get_paper_ਉਚਾਈ(ਪੰਨਾ_ਤਿਆਰੀ,
	    GTK_UNIT_POINTS);

    ਆਰਜ਼ੀ  = vim_tempname('p', FALSE);
    ਜੇ (ਆਰਜ਼ੀ == ਨਿਰਾ)
    {
	emsg(_(e_cant_ਲੱਭੋ_ਆਰਜ਼ੀ_file_for_writing));
	g_clear_object(&dialog);
	g_clear_ਸੂਚਕ(&print_ਤਿਆਰੀ, gtk_print_ਤਿਆਰੀ_unref);

	psettings->user_abort_ਸੁਨੇਹਾ = FALSE;
	ਵਾਪਸ ਨਿਰਾ;
    }
    print_file = g_file_new_for_ਰਸਤਾ((ਅੱਖਰ *)ਆਰਜ਼ੀ);

    print_dialog = dialog;

    ਵਾਪਸ ਆਰਜ਼ੀ;
}

    ਸਥਿਰ ਖਾਲੀ
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsync_ਨਤੀਜਾ	*ਨਤੀਜਾ,
	GFile		*file)
{
    ਅੱਖਰ *ਰਸਤਾ = g_file_get_ਰਸਤਾ(file);

    (ਖਾਲੀ)gtk_print_dialog_print_file_ਅੰਤ(dialog, ਨਤੀਜਾ, ਨਿਰਾ);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(ਰਸਤਾ);
    g_free(ਰਸਤਾ);
}


    ਖਾਲੀ
gui_gtk4_print_ਅੰਤ(ਖਾਲੀ)
{
    ਜੇ (print_dialog != ਨਿਰਾ)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_ਖਿੜਕੀ(gui.mainwin),
		print_ਤਿਆਰੀ, print_file, ਨਿਰਾ,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = ਨਿਰਾ;
	g_clear_ਸੂਚਕ(&print_ਤਿਆਰੀ, gtk_print_ਤਿਆਰੀ_unref);
    }
}
#ਜੇ_ਅੰਤ // USE_GTK4_PRINT_DIALOG

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE_GUI

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    GdkTexture *texture;
} image_gdk_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    Gsk_Render_ਗੰਢ   **nodes; // Note that elements may be NULL
    ਪੂਰਨ		    n_nodes;
} image_placement_gdk_T;

    ਪੂਰਨ
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    Gdk_ਸਮ੍ਰਿਤੀ_Format fmt;
    size_t	    stride;
    GBytes	    *ਬਾਈਟਾਂ;
    size_t	    ਮਾਪ;
    ਪੂਰਨ		    w, h;

    ਜੇ (ctx == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_ਸਮ੍ਰਿਤੀ_R8_G8_B8_A8;
    ਨਹੀਂਤਾਂ
	fmt = GDK_ਸਮ੍ਰਿਤੀ_R8_G8_B8;

    image_get_dimensions(img, &w, &h);

    ਮਾਪ = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    ਬਾਈਟਾਂ = g_ਬਾਈਟਾਂ_new(img->ਅੰਕੜੇ, ਮਾਪ);
    ctx->texture = gdk_ਸਮ੍ਰਿਤੀ_texture_new(w, h, fmt, ਬਾਈਟਾਂ, stride);
    g_ਬਾਈਟਾਂ_unref(ਬਾਈਟਾਂ);

    img->backend_ਅੰਕੜੇ = ctx;
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_ਅੰਕੜੇ;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    ਸਥਿਰ ਖਾਲੀ
clear_nodes(image_placement_gdk_T *ctx)
{
    ਜੇ (ctx->nodes == ਨਿਰਾ || gui.drawarea == ਨਿਰਾ)
	ਵਾਪਸ;

    ਲਈ (ਪੂਰਨ i = 0; i < ctx->n_nodes; i++)
    {
	ਜੇ (ctx->nodes[i] == ਨਿਰਾ)
	    ਜਾਰੀ;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_ਗੰਢ_unref(ctx->nodes[i]);
    }
    g_clear_ਸੂਚਕ(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    ਪੂਰਨ
image_placement_gui_init(image_placement_T *ਰੱਖੋ)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    ਜੇ (ctx == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਰੱਖੋ->backend_ਅੰਕੜੇ = ctx;
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
image_placement_gui_uninit(image_placement_T *ਰੱਖੋ)
{
    image_placement_gdk_T *ctx = ਰੱਖੋ->backend_ਅੰਕੜੇ;

    clear_nodes(ctx);
    vim_free(ctx);
}

    ਖਾਲੀ
image_placement_gui_draw(image_placement_T *ਰੱਖੋ)
{
    image_T		    *img = ਰੱਖੋ->img;
    image_placement_gdk_T   *ctx = ਰੱਖੋ->backend_ਅੰਕੜੇ;
    image_gdk_T		    *ictx = img->backend_ਅੰਕੜੇ;
    pixman_box32_t	    *rects;
    ਪੂਰਨ			    n_rects;
    ਪੂਰਨ			    iw, ih;
    Gsk_Render_ਗੰਢ	    *texture_ਗੰਢ;

    rects = pixman_region32_rectangles(&ਰੱਖੋ->visible, &n_rects);
    ਜੇ (rects == ਨਿਰਾ)
	ਵਾਪਸ;

    clear_nodes(ctx);
    ਜੇ (n_rects == 0)
	ਵਾਪਸ;

    image_get_dimensions(img, &iw, &ih);
    texture_ਗੰਢ = gsk_texture_ਗੰਢ_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(ਰੱਖੋ->ਖਾਨਾ) - FILL_X(ਰੱਖੋ->crop_box.x1),
		FILL_Y(ਰੱਖੋ->ਪੰਕਤੀ) - FILL_Y(ਰੱਖੋ->crop_box.y1)
		+ FILL_Y(ਰੱਖੋ->ਪੰਕਤੀ_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    ਜੇ (texture_ਗੰਢ == ਨਿਰਾ)
	// Not sure if this can happen...
	ਵਾਪਸ;

    ctx->nodes = g_malloc_n(n_rects, ਆਕਾਰ(Gsk_Render_ਗੰਢ *));
    ctx->n_nodes = n_rects;

    ਲਈ (ਪੂਰਨ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ਪੂਰਨ		ਪੰਕਤੀ, ਖਾਨਾ;
	ਪੂਰਨ		posx, posy;
	ਪੂਰਨ		x, y, w, h;
	Gsk_Render_ਗੰਢ	*ਗੰਢ;

	image_placement_subrect(ਰੱਖੋ, rect, &ਪੰਕਤੀ, &ਖਾਨਾ, &x, &y, &w, &h, false);
	posx = FILL_X(ਖਾਨਾ);
	posy = FILL_Y(ਪੰਕਤੀ);

	ਗੰਢ = gsk_clip_ਗੰਢ_new(texture_ਗੰਢ,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	ਜੇ (ਗੰਢ != ਨਿਰਾ)
	    vim_draw_area_ਜੋੜੋ_external(VIM_DRAW_AREA(gui.drawarea), ਗੰਢ);

	ctx->nodes[i] = ਗੰਢ;
    }
    gsk_render_ਗੰਢ_unref(texture_ਗੰਢ);
}

    ਖਾਲੀ
image_placement_gui_clear(image_placement_T *ਰੱਖੋ)
{
    image_placement_gdk_T   *ctx = ਰੱਖੋ->backend_ਅੰਕੜੇ;

    clear_nodes(ctx);
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ // FEAT_GUI_GTK
