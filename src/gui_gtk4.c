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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK

#ಅನ್ತರ್ಭಾವ <gdk/gdkkeysyms.h>
#ಅನ್ತರ್ಭಾವ <gdk/gdk.h>
#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>
#ಅನ್ತರ್ಭಾವ "gui_gtk4_f.h"
#ಅನ್ತರ್ಭಾವ "gui_gtk4_cb.h"
#ಅನ್ತರ್ಭಾವ "gui_gtk4_da.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
# include "gui_gtk4_tb.h"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
# include "gui_gtk4_menu.h"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Geometry string parser, replacing XParseGeometry to remove X11 dependency.
 * Format: [WIDTHxHEIGHT][{+-}XOFF{+-}YOFF]
 */
#ವ್ಯಾಖ್ಯಾನಿಸು No_ಮೌಲ್ಯ		0x0000
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಗಲ_ಮೌಲ್ಯ	0x0004
#ವ್ಯಾಖ್ಯಾನಿಸು ಎತ್ತರ_ಮೌಲ್ಯ	0x0008

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim_parse_geometry(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *ಅಗಲ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *ಎತ್ತರ)
{
    ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ = No_ಮೌಲ್ಯ;
    ಅಕ್ಷರ *ಕೊನೆ;
    ದೀರ್ಘ val;

    ಆದರೆ (str == ಖಾಲಿ || *str == NUL)
	ಹಿಂತಿರುಗಿ ಮುಖವಾಡ;

    // Parse width
    ಆದರೆ (*str != '+' && *str != '-')
    {
	val = strtol(str, &ಕೊನೆ, 10);
	ಆದರೆ (ಕೊನೆ != str)
	{
	    *ಅಗಲ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)val;
	    ಮುಖವಾಡ |= ಅಗಲ_ಮೌಲ್ಯ;
	    str = ಕೊನೆ;
	}
    }

    // Parse 'x' or 'X' separator and height
    ಆದರೆ (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &ಕೊನೆ, 10);
	ಆದರೆ (ಕೊನೆ != str)
	{
	    *ಎತ್ತರ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)val;
	    ಮುಖವಾಡ |= ಎತ್ತರ_ಮೌಲ್ಯ;
	    str = ಕೊನೆ;
	}
    }

    ಹಿಂತಿರುಗಿ ಮುಖವಾಡ;
}

#ಆದರೆ defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_shape = 0;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
ಸ್ಥಿರ ಎಣಿಕೆ {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gtk4_main_loop_level = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gtk4_main_loop_quit = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
ಸ್ಥಿರ ರಚನೆ special_ಕೀಲಿ
{
    guint ಕೀಲಿ_sym;
    char_u code0;
    char_u code1;
}
ಸ್ಥಿರಾಂಕ special_ಕೀಲಿಗಳು[] =
{
    {GDK_ಕೀಲಿ_ಮೇಲೆ,	'k', 'u'},
    {GDK_ಕೀಲಿ_ಕೆಳಕ್ಕೆ,	'k', 'd'},
    {GDK_ಕೀಲಿ_ಎಡ,	'k', 'l'},
    {GDK_ಕೀಲಿ_ಬಲ,	'k', 'r'},
    {GDK_ಕೀಲಿ_F1,	'k', '1'},
    {GDK_ಕೀಲಿ_F2,	'k', '2'},
    {GDK_ಕೀಲಿ_F3,	'k', '3'},
    {GDK_ಕೀಲಿ_F4,	'k', '4'},
    {GDK_ಕೀಲಿ_F5,	'k', '5'},
    {GDK_ಕೀಲಿ_F6,	'k', '6'},
    {GDK_ಕೀಲಿ_F7,	'k', '7'},
    {GDK_ಕೀಲಿ_F8,	'k', '8'},
    {GDK_ಕೀಲಿ_F9,	'k', '9'},
    {GDK_ಕೀಲಿ_F10,	'k', ';'},
    {GDK_ಕೀಲಿ_F11,	'F', '1'},
    {GDK_ಕೀಲಿ_F12,	'F', '2'},
    {GDK_ಕೀಲಿ_Help,	'%', '1'},
    {GDK_ಕೀಲಿ_Undo,	'&', '8'},
    {GDK_ಕೀಲಿ_ಹಿಂಭಾಗ_Space,	'k', 'b'},
    {GDK_ಕೀಲಿ_ಸೇರಿಸು,	'k', 'I'},
    {GDK_ಕೀಲಿ_Delete,	'k', 'D'},
    {GDK_ಕೀಲಿ_Home,	'k', 'h'},
    {GDK_ಕೀಲಿ_ಕೊನೆ,	'@', '7'},
    {GDK_ಕೀಲಿ_ಪೂರ್ವ,	'k', 'P'},
    {GDK_ಕೀಲಿ_ಮುಂದಿನ,	'k', 'N'},
    {GDK_ಕೀಲಿ_Print,	'%', '9'},
    {GDK_ಕೀಲಿ_KP_ಎಡ,	'k', 'l'},
    {GDK_ಕೀಲಿ_KP_ಬಲ,	'k', 'r'},
    {GDK_ಕೀಲಿ_KP_ಮೇಲೆ,	'k', 'u'},
    {GDK_ಕೀಲಿ_KP_ಕೆಳಕ್ಕೆ,	'k', 'd'},
    {GDK_ಕೀಲಿ_KP_ಸೇರಿಸು,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_ಕೀಲಿ_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_ಕೀಲಿ_KP_Home,	'K', '1'},
    {GDK_ಕೀಲಿ_KP_ಕೊನೆ,	'K', '4'},
    {GDK_ಕೀಲಿ_KP_ಪೂರ್ವ,	'K', '3'},
    {GDK_ಕೀಲಿ_KP_ಮುಂದಿನ,	'K', '5'},
    {GDK_ಕೀಲಿ_KP_ಕೂಡಿಸು,	'K', '6'},
    {GDK_ಕೀಲಿ_KP_ಕಳೆ, 'K', '7'},
    {GDK_ಕೀಲಿ_KP_Divide,	'K', '8'},
    {GDK_ಕೀಲಿ_KP_Multiply, 'K', '9'},
    {GDK_ಕೀಲಿ_KP_Enter,	'K', 'A'},
    {GDK_ಕೀಲಿ_KP_Decimal,	'K', 'B'},
    {GDK_ಕೀಲಿ_KP_0,	'K', 'C'},
    {GDK_ಕೀಲಿ_KP_1,	'K', 'D'},
    {GDK_ಕೀಲಿ_KP_2,	'K', 'E'},
    {GDK_ಕೀಲಿ_KP_3,	'K', 'F'},
    {GDK_ಕೀಲಿ_KP_4,	'K', 'G'},
    {GDK_ಕೀಲಿ_KP_5,	'K', 'H'},
    {GDK_ಕೀಲಿ_KP_6,	'K', 'I'},
    {GDK_ಕೀಲಿ_KP_7,	'K', 'J'},
    {GDK_ಕೀಲಿ_KP_8,	'K', 'K'},
    {GDK_ಕೀಲಿ_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
keyval_ಗಮ್ಯ_string(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ keyval, char_u *string)
{
    ಪೂರ್ಣಾಂಕ		len;
    guint32	uc;

    uc = gdk_keyval_ಗಮ್ಯ_unicode(keyval);
    ಆದರೆ (uc != 0)
    {
	len = utf_char2bytes((ಪೂರ್ಣಾಂಕ)uc, string);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	len = 1;
	ಆಯ್ಕೆ (keyval)
	{
	    ಸ್ಥಿತಿ GDK_ಕೀಲಿ_Tab: ಸ್ಥಿತಿ GDK_ಕೀಲಿ_KP_Tab: ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ISO_ಎಡ_Tab:
		string[0] = TAB;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_ಕೀಲಿ_Linefeed:
		string[0] = NL;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_ಕೀಲಿ_Return: ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ISO_Enter: ಸ್ಥಿತಿ GDK_ಕೀಲಿ_3270_Enter:
		string[0] = CAR;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_ಕೀಲಿ_Escape:
		string[0] = ESC;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		len = 0;
		ನಿಲ್ಲಿಸು;
	}
    }
    string[len] = NUL;
    ಹಿಂತಿರುಗಿ len;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
modifiers_gdk2vim(guint state)
{
    ಪೂರ್ಣಾಂಕ modifiers = 0;

    ಆದರೆ (state & GDK_ಸರಿಸು_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
    ಆದರೆ (state & GDK_CONTROL_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_CTRL;
    ಆದರೆ (state & GDK_ALT_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_ALT;
    ಆದರೆ (state & GDK_META_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_META;
    ಆದರೆ (state & GDK_SUPER_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_CMD;

    ಹಿಂತಿರುಗಿ modifiers;
}

ಸ್ಥಿರ GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
ಸ್ಥಿರ gboolean ಕೀಲಿ_press_event(Gtk_Event_Controller_ಕೀಲಿ *controller, guint keyval, guint keycode, GdkModifierType state, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ ಕೀಲಿ_release_event(Gtk_Event_Controller_ಕೀಲಿ *controller, guint keyval, guint keycode, GdkModifierType state, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ button_press_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಂಕ n_press, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ button_release_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಂಕ n_press, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ motion_notify_event(GtkEventControllerMotion *controller, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ enter_notify_event(GtkEventControllerMotion *controller, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ gboolean scroll_event(GtkEventControllerScroll *controller, ದ್ವಿಗುಣ dx, ದ್ವಿಗುಣ dy, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ focus_ಒಳ_event(GtkEventControllerFocus *controller, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ focus_ಹೊರಹೋಗು_event(GtkEventControllerFocus *controller, gpointer ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
ಸ್ಥಿರ gboolean drop_cb(Gtk_Drop_ಗುರಿ_Async *ಗುರಿ, GdkDrop *drop, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, ಶೂನ್ಯ *ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
ಸ್ಥಿರ ಶೂನ್ಯ tabline_enter_cb(GtkEventController *controller, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, ಶೂನ್ಯ *udata);
ಸ್ಥಿರ ಶೂನ್ಯ on_select_tab(GtkNotebook *notebook, gpointer *ಪುಟ, gint idx, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ on_tab_reordered(GtkNotebook *notebook, gpointer *ಪುಟ, gint idx, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ VimMenu *create_tabline_popup_menu(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ tabline_ಕೂಡಿಸು_css(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ tabline_menu_press_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಂಕ n_press, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, GtkWidget *popover);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ mainwin_destroy_cb(GObject *object, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ gboolean delete_event_cb(Gtk_ಕಿಟಕಿ *ಕಿಟಕಿ_2, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ query_ಸೂಚಕ_pos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y, GdkModifierType *state);
ಸ್ಥಿರ ಶೂನ್ಯ mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_form_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ);
ಸ್ಥಿರ ಶೂನ್ಯ drawarea_realize_cb(GtkWidget *widget, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ drawarea_unrealize_cb(GtkWidget *widget, gpointer ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ clipboard_ಬದಲಾಯಿತು_cb(GdkClipboard *clipboard, gpointer user_ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
ಸ್ಥಿರ ಶೂನ್ಯ show_menubar_popover(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prgname = ಖಾಲಿ;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    ಸ್ಥಿರ gboolean
arg_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ಮೌಲ್ಯ)
{
    size_t  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);

    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, name, len) != 0)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (s[len] == '=')
	*ಮೌಲ್ಯ = (ಅಕ್ಷರ *)s + len + 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ಶೂನ್ಯ
gui_mch_prepare(ಪೂರ್ಣಾಂಕ *argc, ಅಕ್ಷರ **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    ಪೂರ್ಣಾಂಕ	i = 0;

    ಆಗ (i < *argc)
    {
	ಅಕ್ಷರ	*s = argv[i];
	ಅಕ್ಷರ	*ಮೌಲ್ಯ = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ	has_inline_ಮೌಲ್ಯ = FALSE;
	ಪೂರ್ಣಾಂಕ	n_strip;

	ಆದರೆ (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (strchr(s, '=') != ಖಾಲಿ)
	    has_inline_ಮೌಲ್ಯ = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	ಆದರೆ (!has_inline_ಮೌಲ್ಯ)
	{
	    ಆದರೆ (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		ಮೌಲ್ಯ = argv[i + 1];
	    ಇಲ್ಲದಿದ್ದರೆ
		ಮೌಲ್ಯ = ಖಾಲಿ;
	}

	ಆದರೆ (arg_ಹೊಂದಿಕೆ(s, "-fn", &ಮೌಲ್ಯ) || arg_ಹೊಂದಿಕೆ(s, "-font", &ಮೌಲ್ಯ))
	    font_ವಾದ = ಮೌಲ್ಯ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg_ಹೊಂದಿಕೆ(s, "-geom", &ಮೌಲ್ಯ)
		|| arg_ಹೊಂದಿಕೆ(s, "-geometry", &ಮೌಲ್ಯ))
	{
	    ಆದರೆ (ಮೌಲ್ಯ != ಖಾಲಿ)
		gui.geom = vim_strsave((char_u *)ಮೌಲ್ಯ);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg_ಹೊಂದಿಕೆ(s, "-bg", &ಮೌಲ್ಯ)
		|| arg_ಹೊಂದಿಕೆ(s, "-background", &ಮೌಲ್ಯ))
	    background_ವಾದ = ಮೌಲ್ಯ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg_ಹೊಂದಿಕೆ(s, "-fg", &ಮೌಲ್ಯ)
		|| arg_ಹೊಂದಿಕೆ(s, "-foreground", &ಮೌಲ್ಯ))
	    foreground_ವಾದ = ಮೌಲ್ಯ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_ಮೌಲ್ಯ = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg_ಹೊಂದಿಕೆ(s, "--prg-name", &ಮೌಲ್ಯ))
	    // GTK4 specific
	    prgname = ಮೌಲ್ಯ;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    i++;
	    ಮುಂದುವರಿ;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	ಆದರೆ (ಮೌಲ್ಯ != ಖಾಲಿ && !has_inline_ಮೌಲ್ಯ)
	    n_strip = 2;

	ಆದರೆ (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    ಆದರೆ (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * ಗಾತ್ರ(ಅಕ್ಷರ *));
	    argv[*argc] = ಖಾಲಿ;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    ಶೂನ್ಯ
gui_mch_free_all(ಶೂನ್ಯ)
{
}

    ಸ್ಥಿರ guint
timeout_ಕೂಡಿಸು(ಪೂರ್ಣಾಂಕ ಸಮಯ, timeout_cb_type (*callback)(gpointer), ಪೂರ್ಣಾಂಕ *flagp)
{
    ಹಿಂತಿರುಗಿ g_timeout_ಕೂಡಿಸು((guint)ಸಮಯ, (GSourceFunc)callback, flagp);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
timeout_remove(guint timer)
{
    g_ಮೂಲ_remove(timer);
}

ಸ್ಥಿರ long_u blink_waittime = 700;
ಸ್ಥಿರ long_u blink_ontime = 400;
ಸ್ಥಿರ long_u blink_offtime = 250;
ಸ್ಥಿರ guint blink_timer = 0;

    ಸ್ಥಿರ timeout_cb_type
blink_cb(gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_ಕೂಡಿಸು(blink_offtime, blink_cb, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_ಕೂಡಿಸು(blink_ontime, blink_cb, ಖಾಲಿ);
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_is_blinking(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ blink_state != BLINK_NONE;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ blink_state == BLINK_OFF;
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_blinking(ದೀರ್ಘ waittime, ದೀರ್ಘ on, ದೀರ್ಘ off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಂಕ may_call_gui_update_cursor)
{
    ಆದರೆ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ಆದರೆ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ಶೂನ್ಯ
gui_mch_ಆರಂಭ_blink(ಶೂನ್ಯ)
{
    ಆದರೆ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ಆದರೆ (blink_waittime && blink_ontime && blink_offtime && gui.ಒಳ_focus)
    {
	blink_timer = timeout_ಕೂಡಿಸು(blink_waittime, blink_cb, ಖಾಲಿ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    ಪೂರ್ಣಾಂಕ
gui_mch_early_init_check(ಪೂರ್ಣಾಂಕ give_ಸಂದೇಶ ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_init_check(ಶೂನ್ಯ)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_ಗಣ_prgname(prgname == ಖಾಲಿ ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    ಆದರೆ (g_getenv("EGL_LOG_LEVEL") == ಖಾಲಿ)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init(ಶೂನ್ಯ)
{
    // Allocate GdkRGBA color structs.
    gui.fgcolor = g_new0(GdkRGBA, 1);
    gui.bgcolor = g_new0(GdkRGBA, 1);
    gui.spcolor = g_new0(GdkRGBA, 1);

    gui.def_norm_pixel = 0x00000000;	// black
    gui.def_ಹಿಂಭಾಗ_pixel = 0x00ffffff;	// white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.ಹಿಂಭಾಗ_pixel = gui.def_ಹಿಂಭಾಗ_pixel;

    gui.scrollbar_ಅಗಲ = SB_DEFAULT_ಅಗಲ;
    gui.scrollbar_ಎತ್ತರ = SB_DEFAULT_ಅಗಲ;

    // Create the main window.
    gui.mainwin = gtk_ಕಿಟಕಿ_new();
    gtk_widget_ಗಣ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.ಪಠ್ಯ_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_ಗಣ_ತಳ_dir(gui.ಪಠ್ಯ_context, PANGO_DIRECTION_LTR);

    g_signal_connect(G_OBJECT(gui.mainwin), "close-request",
		     G_CALLBACK(delete_event_cb), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), ಖಾಲಿ);

    // A vertical box holds the menubar, toolbar and main text window.
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_ಗಣ_homogeneous(GTK_BOX(vbox), FALSE);
    gtk_ಕಿಟಕಿ_ಗಣ_ಮಗು(GTK_ಕಿಟಕಿ(gui.mainwin), vbox);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    gui.menubar = vim_menu_bar_new();
    gtk_widget_ಗಣ_name(gui.menubar, "vim-menubar");
    gtk_widget_ಗಣ_visible(gui.menubar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.menubar);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    gui.toolbar = vim_toolbar_new();
    gtk_widget_ಗಣ_name(gui.toolbar, "vim-toolbar");
    gtk_widget_ಗಣ_visible(gui.toolbar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.toolbar);
    vim_toolbar_ಗಣ_style(VIM_TOOLBAR(gui.toolbar), toolbar_flags, tbis_flags);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    gui.tabline = gtk_notebook_new();
    gtk_notebook_ಗಣ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_ಗಣ_show_tabs(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_notebook_ಗಣ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_widget_ಕೂಡಿಸು_css_class(gui.tabline, "vim-tabline");
    tabline_ಕೂಡಿಸು_css();
    gtk_widget_ಗಣ_visible(gui.tabline, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.tabline);

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ಖಾಲಿ);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), ಖಾಲಿ);

	// Make sure that tabline_enter_cb() is always called before
	// tabpage_enter_cb().
	gtk_event_controller_ಗಣ_ಪ್ರಸರಣ_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	gtk_widget_ಕೂಡಿಸು_controller(gui.tabline, mcontroller);
    }

    // Create right click popup menu for tabline
    {
	GtkGesture	*click;
	VimMenu		*menu;

	click = gtk_gesture_click_new();
	menu = create_tabline_popup_menu();

	gtk_widget_ಗಣ_ಪೋಷಕ(GTK_WIDGET(menu), gui.tabline);
	g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(gui.tabline), "menu", menu);

	gtk_popover_ಗಣ_has_arrow(GTK_POPOVER(menu), FALSE);
	gtk_popover_ಗಣ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
	// Make popover start at top left corner
	gtk_widget_ಗಣ_halign(GTK_WIDGET(menu), GTK_ALIGN_ಆರಂಭ);

	// Listen for anny mouse button
	gtk_gesture_single_ಗಣ_button(GTK_GESTURE_SINGLE(click), 0);

	g_signal_connect_object(click, "pressed",
		G_CALLBACK(tabline_menu_press_event),
		menu, G_CONNECT_DEFAULT);
	gtk_widget_ಕೂಡಿಸು_controller(gui.tabline, GTK_EVENT_CONTROLLER(click));
    }
#ಆದರೆ_ಅಂತ್ಯ

    // The form widget manages absolute positioning of scrollbars and the draw
    // area.
    gui.formwin = vim_form_new();
    gtk_widget_ಗಣ_name(gui.formwin, "vim-gtk-form");
    gtk_widget_ಗಣ_vexpand(gui.formwin, TRUE);
    gtk_widget_ಗಣ_hexpand(gui.formwin, TRUE);
    gtk_box_append(GTK_BOX(vbox), gui.formwin);

    // The drawing area for the editor content.
    gui.drawarea = vim_draw_area_new();
    gtk_widget_ಗಣ_focusable(gui.drawarea, TRUE);
    gtk_widget_ಗಣ_vexpand(gui.drawarea, TRUE);
    gtk_widget_ಗಣ_hexpand(gui.drawarea, TRUE);
    vim_form_put(VIM_FORM(gui.formwin), gui.drawarea, 0, 0);

    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ಖಾಲಿ);

    // Set up event controllers.
    {
	GtkEventController *ಕೀಲಿ_ctrl = gtk_event_controller_ಕೀಲಿ_new();
	g_signal_connect(ಕೀಲಿ_ctrl, "key-pressed",
			 G_CALLBACK(ಕೀಲಿ_press_event), ಖಾಲಿ);
	g_signal_connect(ಕೀಲಿ_ctrl, "key-released",
			 G_CALLBACK(ಕೀಲಿ_release_event), ಖಾಲಿ);
	gtk_widget_ಕೂಡಿಸು_controller(gui.drawarea, ಕೀಲಿ_ctrl);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
	xim_init();
#ಆದರೆ_ಅಂತ್ಯ
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_ಗಣ_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), ಖಾಲಿ);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), ಖಾಲಿ);
	gtk_widget_ಕೂಡಿಸು_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), ಖಾಲಿ);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), ಖಾಲಿ);
	gtk_widget_ಕೂಡಿಸು_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), ಖಾಲಿ);
	gtk_widget_ಕೂಡಿಸು_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_ಒಳ_event), ಖಾಲಿ);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_ಹೊರಹೋಗು_event), ಖಾಲಿ);
	gtk_widget_ಕೂಡಿಸು_controller(gui.drawarea, focus);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_ಗುರಿ = gtk_drop_ಗುರಿ_async_new(ಖಾಲಿ,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_ಗಣ_dnd_targets();
	g_signal_connect(gui.drop_ಗುರಿ, "drop",
			 G_CALLBACK(drop_cb), ಖಾಲಿ);
	gtk_widget_ಕೂಡಿಸು_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_ಗುರಿ));
    }
#ಆದರೆ_ಅಂತ್ಯ

    gui.border_ಸ್ಥಾನಾಂತರ = gui.border_ಅಗಲ;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_ಸೂಚಕ = gdk_cursor_new_from_name("none", ಖಾಲಿ);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	ಆದರೆ (primary != ಖಾಲಿ)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_ಬದಲಾಯಿತು_cb), &clip_ನಕ್ಷತ್ರ);
	ಆದರೆ (regular != ಖಾಲಿ)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_ಬದಲಾಯಿತು_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_ನಕ್ಷತ್ರ);

    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
gui_mch_new_colors(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.drawarea != ಖಾಲಿ && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_ಸರತಿ_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_open(ಶೂನ್ಯ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_ಅಗಲ;
    guint pixel_ಎತ್ತರ;

    ಆದರೆ (gui.geom != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ		ಮುಖವಾಡ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	w, h;

	ಮುಖವಾಡ = vim_parse_geometry((ಅಕ್ಷರ *)gui.geom, &w, &h);

	ಆದರೆ (ಮುಖವಾಡ & ಅಗಲ_ಮೌಲ್ಯ)
	    Columns = w;
	ಆದರೆ (ಮುಖವಾಡ & ಎತ್ತರ_ಮೌಲ್ಯ)
	{
	    ಆದರೆ (p_ಕಿಟಕಿ > (ದೀರ್ಘ)h - 1 || !option_was_ಗಣ((char_u *)"window"))
		p_ಕಿಟಕಿ = h - 1;
	    ಅಡ್ಡಸಾಲುಗಳು = h;
	}
	ಎಲ್ಲೆ_screen_ಗಾತ್ರ();

	VIM_CLEAR(gui.geom);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	ಆದರೆ (Columns > 80)
	    Columns = 80;
	ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು > 24)
	    ಅಡ್ಡಸಾಲುಗಳು = 24;
    }

    pixel_ಅಗಲ = (guint)(gui_get_ತಳ_ಅಗಲ() + Columns * gui.char_ಅಗಲ);
    pixel_ಎತ್ತರ = (guint)(gui_get_ತಳ_ಎತ್ತರ() + ಅಡ್ಡಸಾಲುಗಳು * gui.char_ಎತ್ತರ);

    pixel_ಅಗಲ  += get_menu_tool_ಅಗಲ();
    pixel_ಎತ್ತರ += get_menu_tool_ಎತ್ತರ();

    ಆದರೆ (foreground_ವಾದ != ಖಾಲಿ)
	fg_pixel = gui_get_color((char_u *)foreground_ವಾದ);
    ಆದರೆ (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    ಆದರೆ (background_ವಾದ != ಖಾಲಿ)
	bg_pixel = gui_get_color((char_u *)background_ವಾದ);
    ಆದರೆ (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    ಆದರೆ (ಸಿಕ್ಕಿತು_ಹಿಮ್ಮುಖ_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_ಹಿಂಭಾಗ_pixel = fg_pixel;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_ಹಿಂಭಾಗ_pixel = bg_pixel;
    }

    ಗಣ_ಸಾಮಾನ್ಯ_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ಖಾಲಿ);
    // Resize is handled by GtkForm's size_allocate callback.

    ಗಣ_form_ಗಾತ್ರ((ಪೂರ್ಣಾಂಕ)pixel_ಅಗಲ - get_menu_tool_ಅಗಲ(),
	    (ಪೂರ್ಣಾಂಕ)pixel_ಎತ್ತರ - get_menu_tool_ಎತ್ತರ());

    gtk_ಕಿಟಕಿ_present(GTK_ಕಿಟಕಿ(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಂಕ rc ಬಳಸದ)
{
    ಆದರೆ (gui.mainwin != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	// Must unparent popover menu for tabline or we will get warning
	gtk_widget_unparent(g_object_get_ದತ್ತಾಂಶ(G_OBJECT(gui.tabline), "menu"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
	// Make sure to destroy popover used for balloon eval, or we will get a
	// warning from GTK that the draw area still has children left.
	gui_mch_destroy_beval_area(balloonEval);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	// Make sure to unparent any popover menus
	{
	    vimmenu_T *menu;

	    FOR_ALL_MENUS(menu)
	    {
		ಆದರೆ ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != ಖಾಲಿ)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	gtk_ಕಿಟಕಿ_destroy(GTK_ಕಿಟಕಿ(gui.mainwin));
    }
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_winpos(ಪೂರ್ಣಾಂಕ x ಬಳಸದ, ಪೂರ್ಣಾಂಕ y ಬಳಸದ)
{
    // GTK4/Wayland: window positioning not available
}

    ಪೂರ್ಣಾಂಕ
gui_mch_maximized(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ gtk_ಕಿಟಕಿ_is_maximized(GTK_ಕಿಟಕಿ(gui.mainwin));
}

    ಶೂನ್ಯ
gui_mch_unmaximize(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.mainwin != ಖಾಲಿ)
	gtk_ಕಿಟಕಿ_unmaximize(GTK_ಕಿಟಕಿ(gui.mainwin));
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_fullscreen(ಪೂರ್ಣಾಂಕ flag)
{
    ಆದರೆ (gui.mainwin == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (flag)
	gtk_ಕಿಟಕಿ_fullscreen(GTK_ಕಿಟಕಿ(gui.mainwin));
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_ಕಿಟಕಿ_unfullscreen(GTK_ಕಿಟಕಿ(gui.mainwin));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec ಬಳಸದ, gpointer user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    ಆದರೆ (gtk_ಕಿಟಕಿ_is_fullscreen(GTK_ಕಿಟಕಿ(obj)))
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
    ಶೂನ್ಯ
gui_mch_newfont(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_ಅಗಲ(gui.formwin);
    h = gtk_widget_get_ಎತ್ತರ(gui.formwin);
    gui_resize_shell(w, h);
}

    ಶೂನ್ಯ
gui_mch_settitle(char_u *title, char_u *icon ಬಳಸದ)
{
    ಆದರೆ (title != ಖಾಲಿ && gui.mainwin != ಖಾಲಿ)
	gtk_ಕಿಟಕಿ_ಗಣ_title(GTK_ಕಿಟಕಿ(gui.mainwin), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    ಶೂನ್ಯ
gui_gtk_init_decor_ಎತ್ತರ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ h = gtk_widget_get_ಎತ್ತರ(gui.mainwin);

    ಆದರೆ (h == 0)
	ಹಿಂತಿರುಗಿ;

    h -= get_menu_tool_ಎತ್ತರ();
    h -= gtk_widget_get_ಎತ್ತರ(gui.formwin);

    gui.decor_ಎತ್ತರ = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_form_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    ಆದರೆ (gtk_widget_get_ಅಗಲ(gui.formwin) == ಅಗಲ
	    && gtk_widget_get_ಎತ್ತರ(gui.formwin) == ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ;

    // Remember the size the form widget is supposed to get. An allocation
    // that arrives before the compositor has answered this request still has
    // the previous size and must not be used.
    gui.pending_form_w = ಅಗಲ;
    gui.pending_form_h = ಎತ್ತರ;
    gui.pending_form_skip = 1;

    // GtkWindow default size also includes client side decorations, so must
    // include it also.  It also keeps the natural width of the toolbar from
    // deciding the width.
    gtk_ಕಿಟಕಿ_ಗಣ_default_ಗಾತ್ರ(GTK_ಕಿಟಕಿ(gui.mainwin),
	    ಅಗಲ + get_menu_tool_ಅಗಲ(),
	    ಎತ್ತರ + get_menu_tool_ಎತ್ತರ() + gui.decor_ಎತ್ತರ);

    // The window drops a request made while it is being presented, and
    // "gui.decor_height" is not known before that.  A size request on the form
    // widget is honoured then; it is dropped again once the size was given.
    gtk_widget_ಗಣ_ಗಾತ್ರ_request(gui.formwin, ಅಗಲ, ಎತ್ತರ);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_shellsize(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ min_ಅಗಲ ಬಳಸದ, ಪೂರ್ಣಾಂಕ min_ಎತ್ತರ ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ ತಳ_ಅಗಲ ಬಳಸದ, ಪೂರ್ಣಾಂಕ ತಳ_ಎತ್ತರ ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ direction ಬಳಸದ)
{
    ಗಣ_form_ಗಾತ್ರ(ಅಗಲ, ಎತ್ತರ);

    gui_mch_update();
}

    ಶೂನ್ಯ
gui_mch_get_screen_dimensions(ಪೂರ್ಣಾಂಕ *screen_w, ಪೂರ್ಣಾಂಕ *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    ಆದರೆ (surface != ಖಾಲಿ)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	ಆದರೆ (monitor != ಖಾಲಿ)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.ಅಗಲ;
	    *screen_h = geom.ಎತ್ತರ;
	    ಹಿಂತಿರುಗಿ;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಶೂನ್ಯ
gui_mch_enable_menu(ಪೂರ್ಣಾಂಕ showit)
{
    ಆದರೆ (gui.menubar != ಖಾಲಿ)
    {
	gtk_widget_ಗಣ_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	ಆದರೆ (gui.drawarea != ಖಾಲಿ)
	    gtk_widget_ಸರತಿ_draw(gui.drawarea);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಂಕ showit)
{
    ಆದರೆ (gui.toolbar != ಖಾಲಿ)
    {
	gtk_widget_ಗಣ_visible(gui.toolbar, showit);
	ಆದರೆ (showit)
	    vim_toolbar_ಗಣ_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	ಆದರೆ (gui.drawarea != ಖಾಲಿ)
	    gtk_widget_ಸರತಿ_draw(gui.drawarea);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_ಗಣ_dark_theme(ಪೂರ್ಣಾಂಕ dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    ಆದರೆ (settings != ಖಾಲಿ)
	g_object_ಗಣ(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, ಖಾಲಿ);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    ಪೂರ್ಣಾಂಕ
gui_mch_adjust_charheight(ಶೂನ್ಯ)
{
    PangoFontMetrics *metrics;
    ಪೂರ್ಣಾಂಕ ascent;
    ಪೂರ್ಣಾಂಕ descent;

    metrics = pango_context_get_metrics(gui.ಪಠ್ಯ_context, gui.norm_font,
			    pango_context_get_language(gui.ಪಠ್ಯ_context));
    ascent = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);
    pango_font_metrics_unref(metrics);

    gui.char_ಎತ್ತರ = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_ಎತ್ತರ = MAX(gui.char_ಎತ್ತರ, gui.char_ascent + 1);

    ಹಿಂತಿರುಗಿ OK;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    PangoFontDescription    *ಫಲಿತಾಂಶ;
    gboolean		    ಮುಗಿದ;
} Font_Dialog_ದತ್ತಾಂಶ;

    ಸ್ಥಿರ ಶೂನ್ಯ
font_dialog_ಮುಕ್ತಾಯ_cb(GObject *ಮೂಲ, GAsync_ಫಲಿತಾಂಶ *res, gpointer ದತ್ತಾಂಶ)
{
    Font_Dialog_ದತ್ತಾಂಶ *fdd = (Font_Dialog_ದತ್ತಾಂಶ *)ದತ್ತಾಂಶ;
    fdd->ಫಲಿತಾಂಶ = gtk_font_dialog_ಆರಿಸು_font_ಮುಕ್ತಾಯ(
		    GTK_FONT_DIALOG(ಮೂಲ), res, ಖಾಲಿ);
    fdd->ಮುಗಿದ = TRUE;
}

    ಸ್ಥಿರ gboolean
font_filter(gpointer ಅಂಶ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (PANGO_IS_FONT_FAMILY(ಅಂಶ))
	ಹಿಂತಿರುಗಿ pango_font_family_is_monospace(PANGO_FONT_FAMILY(ಅಂಶ));
    ಆದರೆ (PANGO_IS_FONT_FACE(ಅಂಶ))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(ಅಂಶ));
	ಆದರೆ (family != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ pango_font_family_is_monospace(family);
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = ಖಾಲಿ;
    char_u		*fontname = ಖಾಲಿ;
    Font_Dialog_ದತ್ತಾಂಶ	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_ಗಣ_modal(dlg, TRUE);
    gtk_font_dialog_ಗಣ_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, ಖಾಲಿ, ಖಾಲಿ)));

    ಆದರೆ (oldval != ಖಾಲಿ && oldval[0] != NUL)
    {
	char_u *oldname;

	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    oldname = oldval;

	ಆದರೆ (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		ಆದರೆ (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)oldname);
	ಆದರೆ (oldname != oldval)
	    vim_free(oldname);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.ಫಲಿತಾಂಶ = ಖಾಲಿ;
    fdd.ಮುಗಿದ = FALSE;

    gtk_font_dialog_ಆರಿಸು_font(dlg, GTK_ಕಿಟಕಿ(gui.mainwin),
	    initial, ಖಾಲಿ, font_dialog_ಮುಕ್ತಾಯ_cb, &fdd);

    ಆಗ (!fdd.ಮುಗಿದ)
	g_main_context_iteration(ಖಾಲಿ, TRUE);

    ಆದರೆ (fdd.ಫಲಿತಾಂಶ != ಖಾಲಿ)
    {
	ಅಕ್ಷರ *name = pango_font_description_ಗಮ್ಯ_string(fdd.ಫಲಿತಾಂಶ);
	ಆದರೆ (name != ಖಾಲಿ)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    ಆದರೆ (p != ಖಾಲಿ && ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&ಒಳಹರಿವು_conv, p, ಖಾಲಿ);
		vim_free(p);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		fontname = p;
	}
	pango_font_description_free(fdd.ಫಲಿತಾಂಶ);
    }

    ಆದರೆ (initial != ಖಾಲಿ)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    ಹಿಂತಿರುಗಿ fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ascii_glyph_ಕೋಷ್ಟಕ_init(ಶೂನ್ಯ)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *ಅಂಶ_list;
    ಪೂರ್ಣಾಂಕ		    i;

    ಆದರೆ (gui.ascii_glyphs != ಖಾಲಿ)
	pango_glyph_string_free(gui.ascii_glyphs);
    ಆದರೆ (gui.ascii_font != ಖಾಲಿ)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ಖಾಲಿ;
    gui.ascii_font   = ಖಾಲಿ;

    ಗಾಗಿ (i = 0; i < 128; ++i)
    {
	ಆದರೆ (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	ಇಲ್ಲದಿದ್ದರೆ
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    ಅಂಶ_list = pango_itemize(gui.ಪಠ್ಯ_context, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ascii_chars,
			      0, ಗಾತ್ರ(ascii_chars), attr_list, ಖಾಲಿ);

    ಆದರೆ (ಅಂಶ_list != ಖಾಲಿ && ಅಂಶ_list->ಮುಂದಿನ == ಖಾಲಿ)
    {
	Pango_ಅಂಶ   *ಅಂಶ;
	ಪೂರ್ಣಾಂಕ	    ಅಗಲ;

	ಅಂಶ  = (Pango_ಅಂಶ *)ಅಂಶ_list->ದತ್ತಾಂಶ;
	ಅಗಲ = gui.char_ಅಗಲ * PANGO_SCALE;

	gui.ascii_font = ಅಂಶ->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ascii_chars, ಗಾತ್ರ(ascii_chars),
		    &ಅಂಶ->analysis, gui.ascii_glyphs);

	ಆದರೆ (gui.ascii_glyphs->ಅಂಕೆ_glyphs == (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(ascii_chars))
	{
	    ಗಾಗಿ (i = 0; i < gui.ascii_glyphs->ಅಂಕೆ_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_ಸ್ಥಾನಾಂತರ += MAX(0, ಅಗಲ - geom->ಅಗಲ) / 2;
		geom->ಅಗಲ = ಅಗಲ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = ಖಾಲಿ;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = ಖಾಲಿ;
	}
    }

    g_list_foreach(ಅಂಶ_list, (GFunc)(ಶೂನ್ಯ *)&pango_ಅಂಶ_free, ಖಾಲಿ);
    g_list_free(ಅಂಶ_list);
    pango_attr_list_unref(attr_list);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
get_styled_font_variants(ಶೂನ್ಯ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_ತುಂಬು_font(gui.ಪಠ್ಯ_context, gui.norm_font);
    ಆದರೆ (plain_font == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_ಗಣ_ತೂಕ(bold_font_desc, PANGO_ತೂಕ_BOLD);

    bold_font = pango_context_ತುಂಬು_font(gui.ಪಠ್ಯ_context, bold_font_desc);
    ಆದರೆ (bold_font != ಖಾಲಿ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    ಪೂರ್ಣಾಂಕ
gui_mch_init_font(char_u *font_name, ಪೂರ್ಣಾಂಕ fontset ಬಳಸದ)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    ಪೂರ್ಣಾಂಕ			    ಅಗಲ;

    ಆದರೆ (font_name == ಖಾಲಿ)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    ಆದರೆ (font_desc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_ಗಣ_font_description(gui.ಪಠ್ಯ_context, font_desc);

    layout = pango_layout_new(gui.ಪಠ್ಯ_context);
    pango_layout_ಗಣ_ಪಠ್ಯ(layout, "MW", 2);
    pango_layout_get_ಗಾತ್ರ(layout, &ಅಗಲ, ಖಾಲಿ);
    g_object_unref(layout);

    gui.char_ಅಗಲ = (ಅಗಲ / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    ಆದರೆ (gui.char_ಅಗಲ <= 0)
	gui.char_ಅಗಲ = 8;

    gui_mch_adjust_charheight();

    hl_ಗಣ_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_ಕೋಷ್ಟಕ_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	ಪೂರ್ಣಾಂಕ im_ಅಡ್ಡಸಾಲು = gui.ಅಡ್ಡಸಾಲು < screen_ಅಡ್ಡಸಾಲುಗಳು ? gui.ಅಡ್ಡಸಾಲು : screen_ಅಡ್ಡಸಾಲುಗಳು - 1;
	ಪೂರ್ಣಾಂಕ im_ಲಂಬಸಾಲು = gui.ಲಂಬಸಾಲು < screen_Columns ? gui.ಲಂಬಸಾಲು : screen_Columns - 1;
	im_ಗಣ_position(im_ಅಡ್ಡಸಾಲು, im_ಲಂಬಸಾಲು);
    }
    ಹಿಂತಿರುಗಿ OK;
}

    GuiFont
gui_mch_get_font(char_u *name, ಪೂರ್ಣಾಂಕ report_error)
{
    PangoFontDescription *font;

    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    font = pango_font_description_from_string((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name);
    ಆದರೆ (font == ಖಾಲಿ)
    {
	ಆದರೆ (report_error)
	    semsg(_(e_unknown_font_str), name);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Ensure a size is set
    ಆದರೆ (pango_font_description_get_ಗಾತ್ರ(font) <= 0)
	pango_font_description_ಗಣ_ಗಾತ್ರ(font, 10 * PANGO_SCALE);

    ಹಿಂತಿರುಗಿ font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name ಬಳಸದ)
{
    ಆದರೆ (font != NOFONT)
    {
	ಅಕ್ಷರ *desc = pango_font_description_ಗಮ್ಯ_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	ಹಿಂತಿರುಗಿ ret;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಶೂನ್ಯ
gui_mch_free_font(GuiFont font)
{
    ಆದರೆ (font != NOFONT)
	pango_font_description_free(font);
}

    ಶೂನ್ಯ
gui_mch_expand_font(
    optexpand_T	*ವಾದಗಳು,
    ಶೂನ್ಯ	*param,
    ಪೂರ್ಣಾಂಕ		(*ಕೂಡಿಸು_ಹೊಂದಿಕೆ)(char_u *val))
{
    PangoFontFamily	**font_families = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			n_families = 0;
    ಪೂರ್ಣಾಂಕ			wide = *(ಪೂರ್ಣಾಂಕ *)param;

    ಆದರೆ (ವಾದಗಳು->oe_include_orig_val && *ವಾದಗಳು->oe_opt_ಮೌಲ್ಯ == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	ಆದರೆ (ಕೂಡಿಸು_ಹೊಂದಿಕೆ((char_u *)DEFAULT_FONT) != OK)
	    ಹಿಂತಿರುಗಿ;
    }

    pango_context_list_families(
	    gui.ಪಠ್ಯ_context,
	    &font_families,
	    &n_families);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_families; i++)
    {
	ಆದರೆ (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ಮುಂದುವರಿ;

	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fam_name = pango_font_family_get_name(font_families[i]);
	ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&ಒಳಹರಿವು_conv,
		    (char_u *)fam_name, ಖಾಲಿ);
	    ಆದರೆ (buf != ಖಾಲಿ)
	    {
		ಆದರೆ (ಕೂಡಿಸು_ಹೊಂದಿಕೆ(buf) != OK)
		{
		    vim_free(buf);
		    ನಿಲ್ಲಿಸು;
		}
		vim_free(buf);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಕೂಡಿಸು_ಹೊಂದಿಕೆ((char_u *)fam_name) != OK)
		ನಿಲ್ಲಿಸು;
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
    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ INVALCOLOR;

    ಆದರೆ (name != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ gui_get_color_cmn(name);

    ಹಿಂತಿರುಗಿ INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ g, ಪೂರ್ಣಾಂಕ b)
{
    ಹಿಂತಿರುಗಿ gui_get_rgb_color_cmn(r, g, b);
}

    ಸ್ಥಿರ GdkRGBA
color_ಗಮ್ಯ_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    ಹಿಂತಿರುಗಿ rgba;
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_ಗಮ್ಯ_rgba(color);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_ಗಮ್ಯ_rgba(color);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_sp_color(guicolor_T color)
{
    *gui.spcolor = color_ಗಮ್ಯ_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ಹಿಂತಿರುಗಿ pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    ಶೂನ್ಯ
gui_gtk4_update_ಗಾತ್ರ(ಶೂನ್ಯ)
{
    vim_draw_area_ಗಣ_ಗಾತ್ರ(VIM_DRAW_AREA(gui.drawarea),
	    gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು, gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    ಶೂನ್ಯ
gui_gtk4_ಕೂಡಿಸು_multisign(
	cairo_surface_t *surf,
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ		ಅಗಲ,
	ಪೂರ್ಣಾಂಕ		ಎತ್ತರ)
{
    vim_draw_area_ಕೂಡಿಸು_multisign(VIM_DRAW_AREA(gui.drawarea), surf, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು,
	    ಅಗಲ, ಎತ್ತರ);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_clear_ಖಂಡ(ಪೂರ್ಣಾಂಕ row1, ಪೂರ್ಣಾಂಕ col1, ಪೂರ್ಣಾಂಕ row2, ಪೂರ್ಣಾಂಕ col2)
{
    vim_draw_area_clear_ಖಂಡ(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    ಆದರೆ (gui.drawarea != ಖಾಲಿ)
	gtk_widget_ಸರತಿ_draw(gui.drawarea);
}

    ಶೂನ್ಯ
gui_mch_clear_all(ಶೂನ್ಯ)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    ಆದರೆ (gui.drawarea != ಖಾಲಿ)
	gtk_widget_ಸರತಿ_draw(gui.drawarea);
}

    ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    vim_draw_area_move_ಖಂಡ(VIM_DRAW_AREA(gui.drawarea),
	    ಅಡ್ಡಸಾಲು, ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು, gui.scroll_region_bot,
	    gui.scroll_region_ಎಡ, gui.scroll_region_ಬಲ);
    gui_clear_ಖಂಡ(
	    gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1, gui.scroll_region_ಎಡ,
	    gui.scroll_region_bot, gui.scroll_region_ಬಲ);

    gtk_widget_ಸರತಿ_draw(gui.drawarea);
}

    ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    vim_draw_area_move_ಖಂಡ(VIM_DRAW_AREA(gui.drawarea),
	    ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು, ಅಡ್ಡಸಾಲು, gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು,
	    gui.scroll_region_ಎಡ, gui.scroll_region_ಬಲ);
    gui_clear_ಖಂಡ(
	    ಅಡ್ಡಸಾಲು, gui.scroll_region_ಎಡ,
	    ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);

    gtk_widget_ಸರತಿ_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    ಶೂನ್ಯ
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    gui_mch_ಗಣ_bg_color(bg);
    gui_mch_ಗಣ_fg_color(fg);
    vim_draw_area_ಗಣ_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_ಸರತಿ_draw(gui.drawarea);
}

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಂಕ msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, (ಪೂರ್ಣಾಂಕ)Columns - 1);
    gui_mch_flush();
    ui_delay((ದೀರ್ಘ)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, (ಪೂರ್ಣಾಂಕ)Columns - 1);
}

    ಶೂನ್ಯ
gui_mch_invert_rectangle(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ nr, ಪೂರ್ಣಾಂಕ nc)
{
    vim_draw_area_invert_ಖಂಡ(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_ಸರತಿ_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    ಸ್ಥಿರ gboolean
ಕೀಲಿ_press_event(Gtk_Event_Controller_ಕೀಲಿ *controller ಬಳಸದ,
	guint ಕೀಲಿ_sym, guint keycode ಬಳಸದ,
	GdkModifierType state, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    char_u	string[32], string2[32];
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		modifiers;
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;
    char_u	*s, *d;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    ಆದರೆ (xic != ಖಾಲಿ)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	ಆದರೆ (event != ಖಾಲಿ && gtk_im_context_filter_keypress(xic, event))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಆದರೆ (ಕೀಲಿ_sym == GDK_ಕೀಲಿ_F10 && gui.menubar != ಖಾಲಿ)
    {
	ಸ್ಥಿರ char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	ಆದರೆ (check_map(k10, State, FALSE, TRUE, FALSE, ಖಾಲಿ, ಖಾಲಿ) == ಖಾಲಿ)
	{
	    show_menubar_popover();
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    len = keyval_ಗಮ್ಯ_string(ಕೀಲಿ_sym, string2);

    ಆದರೆ (len > 1 && ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	len = convert_ಒಳಹರಿವು(string2, len, ಗಾತ್ರ(string2));

    s = string2;
    d = string;
    ಗಾಗಿ (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	ಆದರೆ (d[-1] == CSI && d + 2 < string + ಗಾತ್ರ(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (ಪೂರ್ಣಾಂಕ)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    ಆದರೆ (ಕೀಲಿ_sym == GDK_ಕೀಲಿ_ISO_ಎಡ_Tab)
    {
	ಕೀಲಿ_sym = GDK_ಕೀಲಿ_Tab;
	state |= GDK_ಸರಿಸು_ಮುಖವಾಡ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    ಆದರೆ ((state & GDK_ALT_ಮುಖವಾಡ)
	    && gui.menu_is_ಸಕ್ರಿಯ
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // Check for special keys
    ಆದರೆ (len == 0 || len == 1)
    {
	ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	{
	    ಆದರೆ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == ಕೀಲಿ_sym)
	    {
		string[0] = CSI;
		string[1] = special_ಕೀಲಿಗಳು[i].code0;
		string[2] = special_ಕೀಲಿಗಳು[i].code1;
		len = -3;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (len == -3)
	ಕೀಲಿ = ಗಮ್ಯ_SPECIAL(string[1], string[2]);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	string[len] = NUL;
	ಕೀಲಿ = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);
    ಆದರೆ (ಕೀಲಿ == CSI)
	ಕೀಲಿ = K_CSI;
    ಆದರೆ (IS_SPECIAL(ಕೀಲಿ))
    {
	string[0] = CSI;
	string[1] = K_ಎರಡನೇ(ಕೀಲಿ);
	string[2] = K_ಮೂರನೇ(ಕೀಲಿ);
	len = 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಕೀಲಿ = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, ಕೀಲಿ);
	modifiers = may_remove_ಸರಿಸು_modifier(modifiers, ಕೀಲಿ);
	len = mb_char2bytes(ಕೀಲಿ, string);
    }

    ಆದರೆ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string2, 3);
    }

    {
	ಪೂರ್ಣಾಂಕ int_ch = check_for_ಅಡಚಣೆ(ಕೀಲಿ, modifiers);
	ಆದರೆ (int_ch != NUL)
	{
	    trash_ಒಳಹರಿವು_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);

    ಆದರೆ (p_mh)
	gui_mch_mousehide(TRUE);

    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೀಲಿ_release_event(Gtk_Event_Controller_ಕೀಲಿ *controller ಬಳಸದ,
	guint keyval ಬಳಸದ, guint keycode ಬಳಸದ,
	GdkModifierType state ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mouse_timed_ಹೊರಹೋಗು = TRUE;
ಸ್ಥಿರ guint mouse_click_timer = 0;

    ಸ್ಥಿರ timeout_cb_type
mouse_click_timer_cb(gpointer ದತ್ತಾಂಶ)
{
    *(ಪೂರ್ಣಾಂಕ *)ದತ್ತಾಂಶ = TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
modifiers_gdk2mouse(guint state)
{
    ಪೂರ್ಣಾಂಕ modifiers = 0;

    ಆದರೆ (state & GDK_ಸರಿಸು_ಮುಖವಾಡ)
	modifiers |= MOUSE_ಸರಿಸು;
    ಆದರೆ (state & GDK_CONTROL_ಮುಖವಾಡ)
	modifiers |= MOUSE_CTRL;
    ಆದರೆ (state & GDK_ALT_ಮುಖವಾಡ)
	modifiers |= MOUSE_ALT;

    ಹಿಂತಿರುಗಿ modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#ವ್ಯಾಖ್ಯಾನಿಸು ANY_BUTTON_ಮುಖವಾಡ (GDK_BUTTON1_ಮುಖವಾಡ | GDK_BUTTON2_ಮುಖವಾಡ \
	| GDK_BUTTON3_ಮುಖವಾಡ | GDK_BUTTON4_ಮುಖವಾಡ | GDK_BUTTON5_ಮುಖವಾಡ)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mouse_pressed_button = -1;

ಸ್ಥಿರ guint motion_repeat_timer = 0;
ಸ್ಥಿರ gboolean motion_repeat_ಸ್ಥಾನಾಂತರ = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mouse_press_x = -1, mouse_press_y = -1;

    ಸ್ಥಿರ ಶೂನ್ಯ
button_press_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಂಕ n_press ಬಳಸದ,
	ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ button;
    ಪೂರ್ಣಾಂಕ repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    ಆದರೆ (!mouse_timed_ಹೊರಹೋಗು && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_ಹೊರಹೋಗು = FALSE;
    mouse_click_timer = timeout_ಕೂಡಿಸು(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_ಹೊರಹೋಗು);

    ಆಯ್ಕೆ (btn)
    {
	ಸ್ಥಿತಿ 1: button = MOUSE_ಎಡ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2: button = MOUSE_ಮಧ್ಯ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 3: button = MOUSE_ಬಲ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 8: button = MOUSE_X1; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 9: button = MOUSE_X2; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ;
    }

    mouse_press_x = (ಪೂರ್ಣಾಂಕ)x;
    mouse_press_y = (ಪೂರ್ಣಾಂಕ)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (ಪೂರ್ಣಾಂಕ)x, (ಪೂರ್ಣಾಂಕ)y, repeated_click, vim_modifiers);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
button_release_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಂಕ n_press ಬಳಸದ,
	ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    ಆದರೆ (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (ಪೂರ್ಣಾಂಕ)x, (ಪೂರ್ಣಾಂಕ)y, FALSE, vim_modifiers);
}

ಸ್ಥಿರ ದ್ವಿಗುಣ prev_mouse_x = -1.0;
ಸ್ಥಿರ ದ್ವಿಗುಣ prev_mouse_y = -1.0;

ಸ್ಥಿರ GdkModifierType cur_state = 0;

    ಸ್ಥಿರ timeout_cb_type
mouse_repeat_timer_cb(gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ x, y;

    ಆದರೆ (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	ಹಿಂತಿರುಗಿ G_ಮೂಲ_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    ಆದರೆ (vim_ಬಳಸಿದ_ಒಳ_ಒಳಹರಿವು_buf() > 10)
	ಹಿಂತಿರುಗಿ G_ಮೂಲ_CONTINUE;

    x = (ಪೂರ್ಣಾಂಕ)prev_mouse_x;
    y = (ಪೂರ್ಣಾಂಕ)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    ಆದರೆ (motion_repeat_ಸ್ಥಾನಾಂತರ)
	x += gui.char_ಅಗಲ;
    motion_repeat_ಸ್ಥಾನಾಂತರ = !motion_repeat_ಸ್ಥಾನಾಂತರ;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    ಹಿಂತಿರುಗಿ G_ಮೂಲ_CONTINUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
motion_notify_event(GtkEventControllerMotion *controller ಬಳಸದ,
	ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    ಆದರೆ (mouse_pressed_button >= 0 && event != ಖಾಲಿ
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_ಮುಖವಾಡ))
    {
	ಆದರೆ (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    ಆದರೆ (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	ಪೂರ್ಣಾಂಕ		w, h;

	ಆದರೆ (event != ಖಾಲಿ)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (ಪೂರ್ಣಾಂಕ)x, (ಪೂರ್ಣಾಂಕ)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_ಅಗಲ(gui.drawarea);
	h = gtk_widget_get_ಎತ್ತರ(gui.drawarea);

	ಆದರೆ (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	ಆದರೆ (x < 0 || y < 0 || x >= w || y >= h)
	{
	    ಪೂರ್ಣಾಂಕ	dx, dy;
	    ಪೂರ್ಣಾಂಕ offshoot;
	    ಪೂರ್ಣಾಂಕ	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    ಆದರೆ (offshoot > 127)
		delay = 5;
	    ಇಲ್ಲದಿದ್ದರೆ
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_ಕೂಡಿಸು(delay,
		    mouse_repeat_timer_cb, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    motion_repeat_timer = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	gui_mouse_moved((ಪೂರ್ಣಾಂಕ)x, (ಪೂರ್ಣಾಂಕ)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    ಆದರೆ (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
enter_notify_event(GtkEventControllerMotion *controller ಬಳಸದ,
	ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    ಸ್ಥಿರ gboolean
scroll_event(GtkEventControllerScroll *controller ಬಳಸದ,
	ದ್ವಿಗುಣ dx ಬಳಸದ, ದ್ವಿಗುಣ dy, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ button;
    int_u vim_modifiers;
    ಪೂರ್ಣಾಂಕ x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    ಆದರೆ (event == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    state = gdk_event_get_modifier_state(event);

    ಆದರೆ (dy < 0)
	button = MOUSE_4;	// scroll up
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dy > 0)
	button = MOUSE_5;	// scroll down
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dx < 0)
	button = MOUSE_7;	// scroll left
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dx > 0)
	button = MOUSE_6;	// scroll right
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
focus_ಒಳ_event(GtkEventControllerFocus *controller ಬಳಸದ,
	gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (gtk_ಕಿಟಕಿ_is_ಸಕ್ರಿಯ(GTK_ಕಿಟಕಿ(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	ಆದರೆ (blink_state == BLINK_NONE)
	    gui_mch_ಆರಂಭ_blink();
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
focus_ಹೊರಹೋಗು_event(GtkEventControllerFocus *controller ಬಳಸದ,
	gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (!gtk_ಕಿಟಕಿ_is_ಸಕ್ರಿಯ(GTK_ಕಿಟಕಿ(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	ಆದರೆ (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#ಆದರೆ defined(FEAT_IMAGE)
    ಸ್ಥಿರ ಶೂನ್ಯ
scale_ಗುಣಕ_cb(GdkSurface  *surface,
	GParamSpec	    *pspec ಬಳಸದ,
	ಶೂನ್ಯ		    *udata ಬಳಸದ)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
drawarea_realize_cb(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_ಗುಣಕ_cb), ಖಾಲಿ);
    redraw_all_later(UPD_VALID);
#ಆದರೆ_ಅಂತ್ಯ
    gui_mch_new_colors();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drawarea_unrealize_cb(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    im_shutdown();
#ಆದರೆ_ಅಂತ್ಯ
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    READ_ದತ್ತಾಂಶ_CLIP,	    // Clipboard data
    READ_ದತ್ತಾಂಶ_DROP,	    // URI or text DND data
    READ_ದತ್ತಾಂಶ_DROP_ದತ್ತಾಂಶ	    // HTML DND data
} Read_ದತ್ತಾಂಶ_Type;

#ವ್ಯಾಖ್ಯಾನಿಸು READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Read_ದತ್ತಾಂಶ_Type    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    ಅಕ್ಷರ	    *buf; // NULL when not needed
} Read_ದತ್ತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Read_ದತ್ತಾಂಶ	rd;
    GdkDrop	*drop;
    ಪೂರ್ಣಾಂಕ		x;
    ಪೂರ್ಣಾಂಕ		y;
} Drop_Read_ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Read_ದತ್ತಾಂಶ	rd;
    Clipboard_T *cbd;
    ಅಕ್ಷರ	*mime_type;
} Clip_Read_ದತ್ತಾಂಶ;

    ಸ್ಥಿರ Read_ದತ್ತಾಂಶ *
read_ದತ್ತಾಂಶ_new(Read_ದತ್ತಾಂಶ_Type type, size_t sz)
{
    Read_ದತ್ತಾಂಶ *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    ಆದರೆ (type == READ_ದತ್ತಾಂಶ_CLIP || type == READ_ದತ್ತಾಂಶ_DROP_ದತ್ತಾಂಶ)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    ಹಿಂತಿರುಗಿ rd;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
read_ದತ್ತಾಂಶ_free(Read_ದತ್ತಾಂಶ *rd)
{
    g_cancellable_cancel(rd->cancel);

    ಆದರೆ (rd->type == READ_ದತ್ತಾಂಶ_CLIP)
	g_free(((Clip_Read_ದತ್ತಾಂಶ *)rd)->mime_type);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rd->type == READ_ದತ್ತಾಂಶ_DROP || rd->type == READ_ದತ್ತಾಂಶ_DROP_ದತ್ತಾಂಶ)
	g_object_unref(((Drop_Read_ದತ್ತಾಂಶ *)rd)->drop);
    g_object_unref(rd->cancel);
    ಆದರೆ (rd->arr != ಖಾಲಿ)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
ಸ್ಥಿರ ಶೂನ್ಯ drop_read_ಪಠ್ಯ(GdkDrop *drop, char_u *ಪಠ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
read_ದತ್ತಾಂಶ_ಒಳಹರಿವು_cb(
	GInput_ಹರಿವು	*ಹರಿವು,
	GAsync_ಫಲಿತಾಂಶ	*ಫಲಿತಾಂಶ,
	Read_ದತ್ತಾಂಶ	*rd)
{
    ssize_t r = g_ಒಳಹರಿವು_ಹರಿವು_read_ಮುಕ್ತಾಯ(ಹರಿವು, ಫಲಿತಾಂಶ, ಖಾಲಿ);

    ಆದರೆ (r == -1)
    {
	// Error occurred
	Drop_Read_ದತ್ತಾಂಶ *drd = (Drop_Read_ದತ್ತಾಂಶ *)rd;

	ಆದರೆ (rd->type == READ_ದತ್ತಾಂಶ_DROP_ದತ್ತಾಂಶ)
	    gdk_drop_ಮುಕ್ತಾಯ(drd->drop, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	ಆದರೆ (rd->type == READ_ದತ್ತಾಂಶ_CLIP)
	{
	    Clip_Read_ದತ್ತಾಂಶ    *crd = (Clip_Read_ದತ್ತಾಂಶ *)rd;
	    ದೀರ್ಘ	    len;
	    char_u	    *actual, *final;
	    ಪೂರ್ಣಾಂಕ		    motion_type = MAUTO;
	    char_u	    *tofree = ಖಾಲಿ;

	    len = (ದೀರ್ಘ)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ಖಾಲಿ;

	    ಆದರೆ (clip_convert_ದತ್ತಾಂಶ(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rd->type == READ_ದತ್ತಾಂಶ_DROP_ದತ್ತಾಂಶ)
	{
	    Drop_Read_ದತ್ತಾಂಶ    *drd = (Drop_Read_ದತ್ತಾಂಶ *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ಸ್ಥಿರಾಂಕ uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ಖಾಲಿ;
	    drop_read_ಪಠ್ಯ(drd->drop, str);
	    g_free(str);
	    gdk_drop_ಮುಕ್ತಾಯ(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ಸ್ಥಿರಾಂಕ uint8_t *)rd->buf, r);
	g_ಒಳಹರಿವು_ಹರಿವು_read_async(ಹರಿವು, rd->buf, READDATA_BUFSIZE,
		G_ಆದ್ಯತೆ_ಮೇಲಿನ,
		rd->cancel, (GAsyncReadyCallback)read_ದತ್ತಾಂಶ_ಒಳಹರಿವು_cb, rd);
	ಹಿಂತಿರುಗಿ;
    }

    read_ದತ್ತಾಂಶ_free(rd);
    g_object_unref(ಹರಿವು);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
/*
 * Set up for receiving DND items.
 */
    ಶೂನ್ಯ
gui_gtk_ಗಣ_dnd_targets(ಶೂನ್ಯ)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_ಕೂಡಿಸು_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_ಕೂಡಿಸು_gtype(builder, G_TYPE_STRING);
    ಆದರೆ (clip_html)
	gdk_content_formats_builder_ಕೂಡಿಸು_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_ಗಮ್ಯ_formats(builder);
    gtk_drop_ಗುರಿ_async_ಗಣ_formats(gui.drop_ಗುರಿ, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
drop_read_ಪಠ್ಯ(GdkDrop *drop ಬಳಸದ, char_u *ಪಠ್ಯ)
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

    ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ || *ಪಠ್ಯ == NUL || !query_ಸೂಚಕ_pos(ಖಾಲಿ, ಖಾಲಿ, &state))
	ಹಿಂತಿರುಗಿ;

    dnd_yank_drag_ದತ್ತಾಂಶ((char_u *)ಪಠ್ಯ, (ದೀರ್ಘ)STRLEN(ಪಠ್ಯ));

    dropkey[2] = modifiers_gdk2vim(state);
    ಆದರೆ (dropkey[2] != 0)
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(dropkey, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(dropkey));
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(dropkey + 3, (ಪೂರ್ಣಾಂಕ)(ಗಾತ್ರ(dropkey) - 3));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drop_read_ಮೌಲ್ಯ_cb(GdkDrop *drop, GAsync_ಫಲಿತಾಂಶ *ಫಲಿತಾಂಶ, Drop_Read_ದತ್ತಾಂಶ *drd)
{
    ಸ್ಥಿರಾಂಕ GValue    *ಮೌಲ್ಯ = gdk_drop_read_ಮೌಲ್ಯ_ಮುಕ್ತಾಯ(drop, ಫಲಿತಾಂಶ, ಖಾಲಿ);
    gboolean	    success = FALSE;

    ಆದರೆ (ಮೌಲ್ಯ == ಖಾಲಿ)
	ಹೋಗು ನಿರ್ಗಮಿಸು;

    ಆದರೆ (G_ಮೌಲ್ಯ_HOLDS(ಮೌಲ್ಯ, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_ಮೌಲ್ಯ_get_boxed(ಮೌಲ್ಯ);
	ಪೂರ್ಣಾಂಕ	nfiles = g_slist_ಉದ್ದಳತೆ(files);
	char_u	**fnames;
	ಪೂರ್ಣಾಂಕ	i;

	ಆದರೆ (nfiles <= 0)
	    ಹೋಗು ನಿರ್ಗಮಿಸು;

	fnames = ALLOC_MULT(char_u *, nfiles);
	ಆದರೆ (fnames == ಖಾಲಿ)
	    ಹೋಗು ನಿರ್ಗಮಿಸು;

	i = 0;
	ಗಾಗಿ (GSList *l = files; l != ಖಾಲಿ; l = l->ಮುಂದಿನ)
	{
	    GFile *file = l->ದತ್ತಾಂಶ;
	    ಅಕ್ಷರ *ಮಾರ್ಗ = g_file_get_ಮಾರ್ಗ(file);
	    ಆದರೆ (ಮಾರ್ಗ != ಖಾಲಿ)
		fnames[i++] = vim_strsave((char_u *)ಮಾರ್ಗ);
	    g_free(ಮಾರ್ಗ);
	}
	nfiles = i;

	ಆದರೆ (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(fnames);
	success = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (G_ಮೌಲ್ಯ_HOLDS(ಮೌಲ್ಯ, G_TYPE_STRING))
    {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ = g_ಮೌಲ್ಯ_get_string(ಮೌಲ್ಯ);

	ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ || *ಪಠ್ಯ == NUL)
	    ಹೋಗು ನಿರ್ಗಮಿಸು;

	drop_read_ಪಠ್ಯ(drop, (char_u *)ಪಠ್ಯ);
	success = TRUE;
    }

ನಿರ್ಗಮಿಸು:
    gdk_drop_ಮುಕ್ತಾಯ(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_ದತ್ತಾಂಶ_free(&drd->rd);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drop_read_cb(GdkDrop *drop, GAsync_ಫಲಿತಾಂಶ *ಫಲಿತಾಂಶ, Drop_Read_ದತ್ತಾಂಶ *drd)
{
    GInput_ಹರಿವು    *ಒಳ_ಹರಿವು;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	    *m;

    ಒಳ_ಹರಿವು = gdk_drop_read_ಮುಕ್ತಾಯ(drop, ಫಲಿತಾಂಶ, &m, ಖಾಲಿ);
    ಆದರೆ (ಒಳ_ಹರಿವು == ಖಾಲಿ || STRCMP(m, "text/html") != 0)
    {
	read_ದತ್ತಾಂಶ_free(&drd->rd);
	gdk_drop_ಮುಕ್ತಾಯ(drop, 0);
	ಹಿಂತಿರುಗಿ;
    }

    ಪ್ರತಿಜ್ಞಾ(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_ಒಳಹರಿವು_ಹರಿವು_read_async(ಒಳ_ಹರಿವು, drd->rd.buf, READDATA_BUFSIZE,
	    G_ಆದ್ಯತೆ_ಮೇಲಿನ,
	    drd->rd.cancel, (GAsyncReadyCallback)read_ದತ್ತಾಂಶ_ಒಳಹರಿವು_cb, drd);
}

// GCancellable of current DND operation, else NULL.
ಸ್ಥಿರ GCancellable *dnd_cancel = ಖಾಲಿ;
ಸ್ಥಿರ guint dnd_timeout_id = 0;

    ಸ್ಥಿರ gboolean
drop_timeout_cb(ಶೂನ್ಯ *udata ಬಳಸದ)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    ಹಿಂತಿರುಗಿ G_ಮೂಲ_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    ಸ್ಥಿರ gboolean
drop_cb(
	Gtk_Drop_ಗುರಿ_Async  *ಗುರಿ ಬಳಸದ,
	GdkDrop		    *drop,
	ದ್ವಿಗುಣ		    x,
	ದ್ವಿಗುಣ		    y,
	ಶೂನ್ಯ		    *udata ಬಳಸದ)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    Drop_Read_ದತ್ತಾಂಶ	*drd = ಖಾಲಿ;

    ಆದರೆ (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (Drop_Read_ದತ್ತಾಂಶ *)read_ದತ್ತಾಂಶ_new(READ_ದತ್ತಾಂಶ_DROP, ಗಾತ್ರ(*drd));
	gdk_drop_read_ಮೌಲ್ಯ_async(drop, GDK_TYPE_FILE_LIST, G_ಆದ್ಯತೆ_ಮೇಲಿನ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_ಮೌಲ್ಯ_cb, drd);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *m[] = {"text/html", ಖಾಲಿ};

	drd = (Drop_Read_ದತ್ತಾಂಶ *)read_ದತ್ತಾಂಶ_new(READ_ದತ್ತಾಂಶ_DROP_ದತ್ತಾಂಶ, ಗಾತ್ರ(*drd));
	gdk_drop_read_async(drop, m, G_ಆದ್ಯತೆ_ಮೇಲಿನ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (Drop_Read_ದತ್ತಾಂಶ *)read_ದತ್ತಾಂಶ_new(READ_ದತ್ತಾಂಶ_DROP, ಗಾತ್ರ(*drd));
	gdk_drop_read_ಮೌಲ್ಯ_async(drop, G_TYPE_STRING, G_ಆದ್ಯತೆ_ಮೇಲಿನ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_ಮೌಲ್ಯ_cb, drd);
    }

    ಆದರೆ (dnd_cancel != ಖಾಲಿ)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    ಆದರೆ (drd != ಖಾಲಿ)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (ಪೂರ್ಣಾಂಕ)x;
	drd->y = (ಪೂರ್ಣಾಂಕ)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_ಕೂಡಿಸು(3000, drop_timeout_cb, ಖಾಲಿ);

	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
mainwin_destroy_cb(GObject *object ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    gui.mainwin = ಖಾಲಿ;
    gui.drawarea = ಖಾಲಿ;
    ಆದರೆ (!exiting)
	gui_shell_closed();
}

    ಸ್ಥಿರ gboolean
delete_event_cb(Gtk_ಕಿಟಕಿ *ಕಿಟಕಿ_2 ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    gui_shell_closed();
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    ಸ್ಥಿರ timeout_cb_type
ಒಳಹರಿವು_timer_cb(gpointer ದತ್ತಾಂಶ)
{
    ಪೂರ್ಣಾಂಕ *timed_ಹೊರಹೋಗು = (ಪೂರ್ಣಾಂಕ *)ದತ್ತಾಂಶ;

    *timed_ಹೊರಹೋಗು = TRUE;
    ಹಿಂತಿರುಗಿ FALSE;	// don't call me again
}

    ಶೂನ್ಯ
gui_mch_update(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ cnt = 0;

    ಆಗ (g_main_context_pending(ಖಾಲಿ) && !vim_is_ಒಳಹರಿವು_buf_ತುಂಬಿದ()
								&& ++cnt < 100)
	g_main_context_iteration(ಖಾಲಿ, TRUE);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಸ್ಥಿರ timeout_cb_type
channel_poll_cb(gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    ಹಿಂತಿರುಗಿ TRUE; // Keep repeating
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
gui_mch_wait_for_chars(ದೀರ್ಘ wtime)
{
    ಪೂರ್ಣಾಂಕ		focus;
    guint	timer;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	timed_ಹೊರಹೋಗು;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    guint	channel_timer = 0;
#ಆದರೆ_ಅಂತ್ಯ

    timed_ಹೊರಹೋಗು = FALSE;

    ಆದರೆ (wtime >= 0)
	timer = timeout_ಕೂಡಿಸು(wtime == 0 ? 1L : wtime,
						   ಒಳಹರಿವು_timer_cb, &timed_ಹೊರಹೋಗು);
    ಇಲ್ಲದಿದ್ದರೆ
	timer = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ಆದರೆ (channel_any_keep_open())
	channel_timer = timeout_ಕೂಡಿಸು(20, channel_poll_cb, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    focus = gui.ಒಳ_focus;

    ಮಾಡು
    {
	// Stop or start blinking when focus changes
	ಆದರೆ (gui.ಒಳ_focus != focus)
	{
	    ಆದರೆ (gui.ಒಳ_focus)
		gui_mch_ಆರಂಭ_blink();
	    ಇಲ್ಲದಿದ್ದರೆ
		gui_mch_stop_blink(TRUE);
	    focus = gui.ಒಳ_focus;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
# ifdef FEAT_TIMERS
	did_ಕೂಡಿಸು_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ಆದರೆ (did_ಕೂಡಿಸು_timer)
	    ಹೋಗು theend;
# endif
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (gui.mainwin == ಖಾಲಿ)
	    ಹೋಗು theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	ಆದರೆ (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    ಹೋಗು theend;
	}

	ಆದರೆ (!ಒಳಹರಿವು_ಲಭ್ಯ())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(ಖಾಲಿ, TRUE);
	    --gtk4_main_loop_level;
	}

	ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ())
	{
	    retval = OK;
	    ಹೋಗು theend;
	}
    } ಆಗ (wtime < 0 || !timed_ಹೊರಹೋಗು);

    gui_mch_update();

theend:
    ಆದರೆ (timer != 0 && !timed_ಹೊರಹೋಗು)
	timeout_remove(timer);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (channel_timer != 0)
	timeout_remove(channel_timer);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ retval;
}

    ಶೂನ್ಯ
gui_mch_flush(ಶೂನ್ಯ)
{
    // Ensure the offscreen surface content gets painted to the widget.
    ಆದರೆ (gui.drawarea != ಖಾಲಿ)
	gtk_widget_ಸರತಿ_draw(gui.drawarea);
    ಆದರೆ (gui.mainwin != ಖಾಲಿ && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    ಶೂನ್ಯ
gui_mch_beep(ಶೂನ್ಯ)
{
    GdkDisplay *display;

    ಆದರೆ (gui.mainwin != ಖಾಲಿ && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	ಆದರೆ (display != ಖಾಲಿ)
	    gdk_display_beep(display);
    }
}

    ಶೂನ್ಯ *
gui_mch_get_display(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.mainwin != ಖಾಲಿ && gtk_widget_get_display(gui.mainwin))
	ಹಿಂತಿರುಗಿ gtk_widget_get_display(gui.mainwin);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಶೂನ್ಯ
gui_mch_iconify(ಶೂನ್ಯ)
{
    gtk_ಕಿಟಕಿ_minimize(GTK_ಕಿಟಕಿ(gui.mainwin));
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_foreground(ಶೂನ್ಯ)
{
    gtk_ಕಿಟಕಿ_present(GTK_ಕಿಟಕಿ(gui.mainwin));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
query_ಸೂಚಕ_pos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *ಸೂಚಕ;
    ದ್ವಿಗುಣ	     sx, sy, nx, ny;
    graphene_ಬಿಂದು_t src, dst;

    ಆದರೆ (gui.drawarea == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    ಆದರೆ (native == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    surface = gtk_native_get_surface(native);
    ಆದರೆ (surface == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    ಆದರೆ (display == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    seat = gdk_display_get_default_seat(display);
    ಆದರೆ (seat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಸೂಚಕ = gdk_seat_get_ಸೂಚಕ(seat);
    ಆದರೆ (ಸೂಚಕ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!gdk_surface_get_device_position(surface, ಸೂಚಕ, &sx, &sy, state))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (x != ಖಾಲಿ && y != ಖಾಲಿ)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (ದಶಮಾಂಶ)(sx - nx);
	src.y = (ದಶಮಾಂಶ)(sy - ny);
	ಆದರೆ (!gtk_widget_compute_ಬಿಂದು(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    ಹಿಂತಿರುಗಿ FALSE;

	*x = (ಪೂರ್ಣಾಂಕ)dst.x;
	*y = (ಪೂರ್ಣಾಂಕ)dst.y;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    ಆದರೆ (!query_ಸೂಚಕ_pos(x, y, ಖಾಲಿ))
    {
	*x = 0;
	*y = 0;
    }
}

    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಂಕ x ಬಳಸದ, ಪೂರ್ಣಾಂಕ y ಬಳಸದ)
{
    // GTK4/Wayland: cannot warp pointer
}

    ಶೂನ್ಯ
gui_mch_mousehide(ಪೂರ್ಣಾಂಕ hide)
{
    ಆದರೆ (gui.ಸೂಚಕ_hidden == hide)
	ಹಿಂತಿರುಗಿ;

    gui.ಸೂಚಕ_hidden = hide;
    ಆದರೆ (gui.blank_ಸೂಚಕ != ಖಾಲಿ)
    {
	ಆದರೆ (hide)
	    gtk_widget_ಗಣ_cursor(gui.drawarea, gui.blank_ಸೂಚಕ);
	ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
	    mch_ಗಣ_mouse_shape(ಕೊನೆಯ_shape);
#ಇಲ್ಲದಿದ್ದರೆ
	    gtk_widget_ಗಣ_cursor(gui.drawarea, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

    ಪೂರ್ಣಾಂಕ
gui_mch_haskey(char_u *name)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	ಆದರೆ (name[0] == special_ಕೀಲಿಗಳು[i].code0
		&& name[1] == special_ಕೀಲಿಗಳು[i].code1)
	    ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಶೂನ್ಯ
gui_mch_forked(ಶೂನ್ಯ)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_gtk4_update_scrollbar_ಗಾತ್ರ(ಶೂನ್ಯ)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_ಎಡ].id;

	ಆದರೆ (sbar == ಖಾಲಿ || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_ಎಡ]
		    && wp->w_scrollbars[SBAR_ಬಲ].id != ಖಾಲಿ))
	    sbar = wp->w_scrollbars[SBAR_ಬಲ].id;

	ಆದರೆ (sbar != ಖಾಲಿ && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    ಪೂರ್ಣಾಂಕ		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_ಗಾತ್ರ(sbar, &min, &nat);
	    sw = MAX(min.ಅಗಲ, nat.ಅಗಲ);
	    ಆದರೆ (sw > 0)
	    {
		w = sw;
		ನಿಲ್ಲಿಸು;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    ಆದರೆ (sbar != ಖಾಲಿ && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	ಪೂರ್ಣಾಂಕ		    sh;

	gtk_widget_get_preferred_ಗಾತ್ರ(sbar, &min, &nat);
	sh = MAX(min.ಎತ್ತರ, nat.ಎತ್ತರ);

	ಆದರೆ (sh > 0)
	    h = sh;
    }

    ಆದರೆ (w != -1)
	gui.scrollbar_ಅಗಲ = w;
    ಆದರೆ (h != -1)
	gui.scrollbar_ಎತ್ತರ = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    ಶೂನ್ಯ
gui_mch_enable_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ flag)
{
    ಆದರೆ (sb->id != ಖಾಲಿ)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_ಗಣ_visible(sb->id, flag);

	ಆದರೆ (!was_visible && flag)
	    gui_gtk4_update_scrollbar_ಗಾತ್ರ();
    }
}

#ಆದರೆ defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    ಶೂನ್ಯ
gui_mch_menu_grey(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ grey)
{
    ಆದರೆ (menu->id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    gtk_widget_ಗಣ_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    ಶೂನ್ಯ
gui_mch_menu_hidden(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ hidden)
{
    ಆದರೆ (menu->id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    gtk_widget_ಗಣ_visible(menu->id, !hidden);
    gui_mch_update();
}

    ಶೂನ್ಯ
gui_mch_draw_menubar(ಶೂನ್ಯ)
{
    // Just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ============================================================
 * Tabline
 * ============================================================
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    ಶೂನ್ಯ
gui_mch_show_tabline(ಪೂರ್ಣಾಂಕ showit)
{
    ಆದರೆ (gui.tabline == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    gtk_widget_ಗಣ_visible(gui.tabline, showit);
    gui_mch_update();
}

    ಪೂರ್ಣಾಂಕ
gui_mch_showing_tabline(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ gui.tabline != ಖಾಲಿ && gtk_widget_get_visible(gui.tabline);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tabpage_hover = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
tabpage_enter_cb(
	GtkEventController  *controller ಬಳಸದ,
	ದ್ವಿಗುಣ		    x ಬಳಸದ,
	ದ್ವಿಗುಣ		    y ಬಳಸದ,
	ಶೂನ್ಯ		    *udata)
{
    tabpage_hover = GPOINTER_ಗಮ್ಯ_INT(udata);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_enter_cb(
	GtkEventController  *controller ಬಳಸದ,
	ದ್ವಿಗುಣ		    x ಬಳಸದ,
	ದ್ವಿಗುಣ		    y ಬಳಸದ,
	ಶೂನ್ಯ		    *udata ಬಳಸದ)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_ಕೂಡಿಸು_css(ಶೂನ್ಯ)
{
    GtkCssProvider  *provider;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *css =
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
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(4, 12, 0)
    gtk_css_provider_ತುಂಬು_from_string(provider, css);
# ಇಲ್ಲದಿದ್ದರೆ
    gtk_css_provider_ತುಂಬು_from_ದತ್ತಾಂಶ(provider, css, -1);
# endif
    gtk_style_context_ಕೂಡಿಸು_provider_for_display(
	    gtk_widget_get_display(gui.tabline), GTK_STYLE_PROVIDER(provider),
	    GTK_STYLE_PROVIDER_ಆದ್ಯತೆ_APPLICATION);
    g_object_unref(provider);
}

/*
 * Handle clicking the close button of a tab.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tab_close_clicked_cb(GtkButton *button ಬಳಸದ, gpointer ದತ್ತಾಂಶ)
{
    ಪೂರ್ಣಾಂಕ idx = gtk_notebook_ಪುಟ_ಅಂಕೆ(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(ದತ್ತಾಂಶ));

    ಆದರೆ (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    ಶೂನ್ಯ
gui_mch_update_tabline(ಶೂನ್ಯ)
{
    GtkWidget	*ಪುಟ;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    ಪೂರ್ಣಾಂಕ		nr = 0;
    ಪೂರ್ಣಾಂಕ		tab_ಅಂಕೆ;
    ಪೂರ್ಣಾಂಕ		curtabidx = 0;
    char_u	*labeltext;

    ಆದರೆ (gui.tabline == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ignore_tabline_evt = TRUE;

    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ; tp = tp->tp_ಮುಂದಿನ, ++nr)
    {
	Gtk_Notebook_ಪುಟ	    *ಪುಟ_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	ಆದರೆ (tp == curtab)
	    curtabidx = nr;

	tab_ಅಂಕೆ = nr + 1;

	ಪುಟ = gtk_notebook_get_nth_ಪುಟ(GTK_NOTEBOOK(gui.tabline), nr);
	ಆದರೆ (ಪುಟ == ಖಾಲಿ)
	{
	    GtkWidget	*spacer;
	    GtkWidget	*close_button;

	    ಪುಟ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_widget_ಗಣ_visible(ಪುಟ, TRUE);
	    event_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_ಗಣ_visible(event_box, TRUE);

	    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_ಕೂಡಿಸು_css_class(spacer, "vim-tab-spacer");
	    gtk_box_append(GTK_BOX(event_box), spacer);

	    label = gtk_label_new("-Empty-");
	    gtk_widget_ಗಣ_hexpand(label, TRUE);
	    gtk_label_ಗಣ_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_ಕೊನೆ);
	    gtk_box_append(GTK_BOX(event_box), label);
	    gtk_widget_ಗಣ_visible(label, TRUE);
	    g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(event_box), "label", label);

	    close_button = gtk_button_new_from_icon_name("window-close-symbolic");
	    gtk_widget_ಕೂಡಿಸು_css_class(close_button, "flat");
	    gtk_widget_ಕೂಡಿಸು_css_class(close_button, "circular");
	    gtk_widget_ಗಣ_valign(close_button, GTK_ALIGN_CENTER);
	    gtk_widget_ಗಣ_focus_on_click(close_button, FALSE);
	    gtk_widget_ಗಣ_tooltip_ಪಠ್ಯ(close_button, _("Close Tab"));
	    g_signal_connect(close_button, "clicked",
		    G_CALLBACK(tab_close_clicked_cb), ಪುಟ);
	    gtk_box_append(GTK_BOX(event_box), close_button);

	    gtk_notebook_ಸೇರಿಸು_ಪುಟ(GTK_NOTEBOOK(gui.tabline),
		    ಪುಟ, event_box, nr);
	    gtk_notebook_ಗಣ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    ಪುಟ, TRUE);
	    g_object_ಗಣ(gtk_notebook_get_ಪುಟ(GTK_NOTEBOOK(gui.tabline), ಪುಟ),
		    "tab-expand", TRUE, ಖಾಲಿ);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	ಪುಟ_widget = gtk_notebook_get_ಪುಟ(GTK_NOTEBOOK(gui.tabline), ಪುಟ);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_ಗಮ್ಯ_ಸೂಚಕ(tab_ಅಂಕೆ));
	gtk_event_controller_ಗಣ_ಪ್ರಸರಣ_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(ಪುಟ_widget, "tab", &tab_widget, ಖಾಲಿ);
	gtk_widget_ಕೂಡಿಸು_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), ಪುಟ);
	label = g_object_get_ದತ್ತಾಂಶ(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_ಗಮ್ಯ_UTF8(NameBuff);
	ಆದರೆ (label != ಖಾಲಿ && GTK_IS_LABEL(label))
	    gtk_label_ಗಣ_ಪಠ್ಯ(GTK_LABEL(label), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)labeltext);
	CONVERT_ಗಮ್ಯ_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_ಗಮ್ಯ_UTF8(NameBuff);
	gtk_widget_ಗಣ_tooltip_ಪಠ್ಯ(event_box, (ಸ್ಥಿರಾಂಕ gchar *)labeltext);
	CONVERT_ಗಮ್ಯ_UTF8_FREE(labeltext);
    }

    ಆಗ (gtk_notebook_get_nth_ಪುಟ(GTK_NOTEBOOK(gui.tabline), nr) != ಖಾಲಿ)
	gtk_notebook_remove_ಪುಟ(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    ಆದರೆ (nr == 1)
	gtk_widget_ಕೂಡಿಸು_css_class(gui.tabline, "single-tab");
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    ಆದರೆ (gtk_notebook_get_current_ಪುಟ(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_ಗಣ_current_ಪುಟ(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_curtab(ಪೂರ್ಣಾಂಕ nr)
{
    ಆದರೆ (gui.tabline != ಖಾಲಿ)
	gtk_notebook_ಗಣ_current_ಪುಟ(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
on_select_tab(
	GtkNotebook	*notebook ಬಳಸದ,
	gpointer	*ಪುಟ ಬಳಸದ,
	gint		idx,
	gpointer	ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
on_tab_reordered(
	GtkNotebook	*notebook ಬಳಸದ,
	gpointer	*ಪುಟ ಬಳಸದ,
	gint		idx,
	gpointer	ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (ignore_tabline_evt)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((tabpage_ಸೂಚ್ಯಂಕ(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    ಇಲ್ಲದಿದ್ದರೆ
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_menu_event_cb(Vim_Menu_ಅಂಶ *ಅಂಶ ಬಳಸದ, Vim_Menu_ಅಂಶ_Event event, ಶೂನ್ಯ *udata)
{
    ಆದರೆ (event == VIM_MENU_ಅಂಶ_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_ಗಮ್ಯ_INT(udata));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_tabline_menu_ಅಂಶ(VimMenu *menu, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ resp)
{
    Vim_Menu_ಅಂಶ *ಅಂಶ = VIM_MENU_ಅಂಶ(vim_menu_ಅಂಶ_new(name,
		tabline_menu_event_cb, GINT_ಗಮ್ಯ_ಸೂಚಕ(resp)));

    vim_menu_ಸೇರಿಸು_ಅಂಶ(menu, ಅಂಶ, -1);
}

/*
 * Create a menu for the tab line.
 */
    ಸ್ಥಿರ VimMenu *
create_tabline_popup_menu(ಶೂನ್ಯ)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    ಕೂಡಿಸು_tabline_menu_ಅಂಶ(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    ಕೂಡಿಸು_tabline_menu_ಅಂಶ(menu, _("New Tab"), TABLINE_MENU_NEW);
    ಕೂಡಿಸು_tabline_menu_ಅಂಶ(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    ಹಿಂತಿರುಗಿ menu;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_menu_press_event(
	GtkGestureClick *gesture,
	ಪೂರ್ಣಾಂಕ		n_press ಬಳಸದ,
	ದ್ವಿಗುಣ		x,
	ದ್ವಿಗುಣ		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    ಆದರೆ (but == GDK_BUTTON_SECONDARY)
    {
	// Right mouse button pressed, popup menu
	GdkRectangle rect;
	rect.x = x;
	rect.y = y;
	rect.ಅಗಲ = 1;
	rect.ಎತ್ತರ = 1;

	gtk_popover_ಗಣ_pointing_ಗಮ್ಯ(GTK_POPOVER(popover), &rect);
	gtk_popover_popup(GTK_POPOVER(popover));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (but == GDK_BUTTON_PRIMARY)
    {
	ಆದರೆ (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (but == GDK_BUTTON_ಮಧ್ಯ)
    {
	ಆದರೆ (tabpage_hover != 0)
	    // Middle mouse click on tabpage label closes that tab.
	    send_tabline_menu_event(tabpage_hover, TABLINE_MENU_CLOSE);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ============================================================
 * Sign support
 * ============================================================
 */

#ಆದರೆ defined(FEAT_SIGN_ICONS)
# define SIGN_ಅಗಲ  (2 * gui.char_ಅಗಲ)
# define SIGN_ಎತ್ತರ (gui.char_ಎತ್ತರ)

    ಶೂನ್ಯ
gui_mch_drawsign(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    ಆದರೆ (sign == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_draw_area_ಕೂಡಿಸು_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, SIGN_ಅಗಲ, SIGN_ಎತ್ತರ);

    gtk_widget_ಸರತಿ_draw(gui.drawarea);
}

    ಶೂನ್ಯ *
gui_mch_ರಿಜಿಸ್ಟರ್_sign(char_u *signfile)
{
    ಆದರೆ (signfile[0] != NUL && signfile[0] != '-' && gui.ಒಳ_use)
    {
	GError	    *error = ಖಾಲಿ;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)signfile,
							    &error);
	ಆದರೆ (sign != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ sign;

	semsg("E255: %s", error->ಸಂದೇಶ_2);
	g_error_free(error);
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಶೂನ್ಯ
gui_mch_destroy_sign(ಶೂನ್ಯ *sign)
{
    ಆದರೆ (sign != ಖಾಲಿ)
	g_object_unref(sign);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
gui_gtk_draw_string(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, char_u *s, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, s, len, flags);

    ಆದರೆ (gui.drawarea != ಖಾಲಿ)
	gtk_widget_ಸರತಿ_draw(gui.drawarea);

    ಹಿಂತಿರುಗಿ cells;
}

    ಪೂರ್ಣಾಂಕ
gui_get_x11_windis(ಕಿಟಕಿ *win ಬಳಸದ, Display **dis ಬಳಸದ)
{
    // GTK4: not applicable
    ಹಿಂತಿರುಗಿ FAIL;
}

#ಆದರೆ defined(FEAT_MENU)
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
    ಸ್ಥಿರ char_u *
translate_mnemonic_ಟ್ಯಾಗ್(char_u *name, ಪೂರ್ಣಾಂಕ use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    ಪೂರ್ಣಾಂಕ	    n_underscores = 0;

    name = CONVERT_ಗಮ್ಯ_UTF8(name);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ಆದರೆ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ಆದರೆ (buf != ಖಾಲಿ)
    {
	pdest = buf;
	ಗಾಗಿ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ಆದರೆ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_ಗಮ್ಯ_UTF8_FREE(name);
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    ಶೂನ್ಯ
gui_gtk_ಗಣ_mnemonics(ಪೂರ್ಣಾಂಕ enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ಆದರೆ (menu->id == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	name = translate_mnemonic_ಟ್ಯಾಗ್(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	ಆದರೆ (VIM_IS_MENU_BAR_ಅಂಶ(menu->id))
	    vim_menu_bar_ಅಂಶ_ಗಣ_ಪಠ್ಯ(VIM_MENU_BAR_ಅಂಶ(menu->id),
		    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name);
	vim_free(name);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
popupmenu_closed_cb(GtkWidget *popover, ಶೂನ್ಯ *udata ಬಳಸದ)
{
    gtk_widget_unparent(popover);
    ಆದರೆ (gui.drawarea != ಖಾಲಿ)
	gtk_widget_ಸರತಿ_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_gtk_popup_at(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    ಆದರೆ (menu == ಖಾಲಿ || menu->submenu_id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    popover = vim_menu_copy(VIM_MENU(menu->submenu_id));
    gtk_widget_ಗಣ_ಪೋಷಕ(popover, gui.drawarea);

    rect.x = x;
    rect.y = y;
    rect.ಅಗಲ = rect.ಎತ್ತರ = 1;

    // Make sure popover aligns down-and-to-the-right of the pointer.
    gtk_popover_ಗಣ_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_widget_ಗಣ_halign(popover, GTK_ALIGN_ಆರಂಭ);
    gtk_popover_ಗಣ_pointing_ಗಮ್ಯ(GTK_POPOVER(popover), &rect);

    g_signal_connect(GTK_POPOVER(popover), "closed",
	    G_CALLBACK(popupmenu_closed_cb), ಖಾಲಿ);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    ಶೂನ್ಯ
gui_make_popup(char_u *ಮಾರ್ಗ_name, ಪೂರ್ಣಾಂಕ mouse_pos)
{
    vimmenu_T	*menu;
    ಪೂರ್ಣಾಂಕ		x, y;

    menu = gui_ಕಂಡುಹಿಡಿ_menu(ಮಾರ್ಗ_name);
    ಆದರೆ (menu == ಖಾಲಿ || menu->submenu_id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (mouse_pos)
	gui_mch_getmouse(&x, &y);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU

    ಪೂರ್ಣಾಂಕ
get_menu_tool_ಅಗಲ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ 0;
}

    ಪೂರ್ಣಾಂಕ
get_menu_tool_ಎತ್ತರ(ಶೂನ್ಯ)
{
    GtkWidget *widgets[] = {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	gui.menubar,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
	gui.toolbar,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	gui.tabline
#ಆದರೆ_ಅಂತ್ಯ
    };

    ಪೂರ್ಣಾಂಕ ಎತ್ತರ = 0;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	ಪೂರ್ಣಾಂಕ		h;

	ಆದರೆ (widgets[i] == ಖಾಲಿ || !gtk_widget_get_visible(widgets[i]))
	    ಮುಂದುವರಿ;

	h = gtk_widget_get_ಎತ್ತರ(widgets[i]);

	ಆದರೆ (h == 0)
	{
	    // Allocation hasn't been updated yet (widget just became visible).
	    // Query the preferred height so the caller gets a valid value
	    // before the layout pass runs.  Use the maximum of minimum and
	    // natural height: GTK may allocate min_h even when natural_h is
	    // smaller (e.g. GtkNotebook tab bar has min_h > natural_h due to
	    // CSS).
	    gtk_widget_get_preferred_ಗಾತ್ರ(widgets[i], &min, &nat);
	    ಎತ್ತರ += MAX(min.ಎತ್ತರ, nat.ಎತ್ತರ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಎತ್ತರ += h;
    }
    ಹಿಂತಿರುಗಿ ಎತ್ತರ;
}

/*
 * Update selection targets for regular and primary selections.
 */
    ಶೂನ್ಯ
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    ಆದರೆ (cbd == &clip_plus)
	gdk_content_provider_content_ಬದಲಾಯಿತು(gui.regular_provider);
    ಇಲ್ಲದಿದ್ದರೆ
	gdk_content_provider_content_ಬದಲಾಯಿತು(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    ಸ್ಥಿರ GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    ಆದರೆ (gui.mainwin == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    display = gtk_widget_get_display(gui.mainwin);
    ಆದರೆ (display == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (cbd == &clip_plus)
    {
	ಆದರೆ (provider != ಖಾಲಿ)
	    *provider = gui.regular_provider;
	ಹಿಂತಿರುಗಿ gdk_display_get_clipboard(display);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (provider != ಖಾಲಿ)
	    *provider = gui.primary_provider;
	ಹಿಂತಿರುಗಿ gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_read_cb(GdkClipboard *cb, GAsync_ಫಲಿತಾಂಶ *ಫಲಿತಾಂಶ, Clip_Read_ದತ್ತಾಂಶ *crd)
{
    GInput_ಹರಿವು    *ಒಳ_ಹರಿವು;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	    *mime_type;

    ಒಳ_ಹರಿವು = gdk_clipboard_read_ಮುಕ್ತಾಯ(cb, ಫಲಿತಾಂಶ, &mime_type, ಖಾಲಿ);
    ಆದರೆ (ಒಳ_ಹರಿವು == ಖಾಲಿ)
    {
	read_ದತ್ತಾಂಶ_free(&crd->rd);
	ಹಿಂತಿರುಗಿ;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_ಒಳಹರಿವು_ಹರಿವು_read_async(ಒಳ_ಹರಿವು, crd->rd.buf, READDATA_BUFSIZE,
	    G_ಆದ್ಯತೆ_ಮೇಲಿನ,
	    crd->rd.cancel, (GAsyncReadyCallback)read_ದತ್ತಾಂಶ_ಒಳಹರಿವು_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    ಶೂನ್ಯ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	ಖಾಲಿ
    };

    GdkClipboard *clipboard;
    Clip_Read_ದತ್ತಾಂಶ *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, ಖಾಲಿ);
    ಆದರೆ (clipboard == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    crd = (Clip_Read_ದತ್ತಾಂಶ *)read_ದತ್ತಾಂಶ_new(READ_ದತ್ತಾಂಶ_CLIP, ಗಾತ್ರ(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_ಆದ್ಯತೆ_ಮೇಲಿನ, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t ಆರಂಭ;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	ಆರಂಭ = ಸಮಯ(ಖಾಲಿ);
	ಆಗ (!g_cancellable_is_cancelled(cancel)
		&& ಸಮಯ(ಖಾಲಿ) < ಆರಂಭ + 3)
	    g_main_context_iteration(ಖಾಲಿ, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಒಳ_clipboard_ಗಣ = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    ಶೂನ್ಯ
clip_mch_ಗಣ_selection(Clipboard_T *cbd ಬಳಸದ)
{
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clipboard_ಬದಲಾಯಿತು_cb(GdkClipboard *clipboard, gpointer user_ದತ್ತಾಂಶ)
{
    Clipboard_T *cbd = (Clipboard_T *)user_ದತ್ತಾಂಶ;

    ಆದರೆ (ಒಳ_clipboard_ಗಣ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (gdk_clipboard_is_local(clipboard))
	ಹಿಂತಿರುಗಿ;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    ಪೂರ್ಣಾಂಕ
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    ಪೂರ್ಣಾಂಕ			ret;

    ಆದರೆ (cb == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಒಳ_clipboard_ಗಣ = TRUE;
    ret = gdk_clipboard_ಗಣ_content(cb, cp);
    ಒಳ_clipboard_ಗಣ = FALSE;

    ಹಿಂತಿರುಗಿ ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    ಶೂನ್ಯ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, ಖಾಲಿ);
    ಆದರೆ (clipboard == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    ಆದರೆ (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_ಗಣ_content(clipboard, ಖಾಲಿ);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ gtk4_main_loop_level;
}

    ಶೂನ್ಯ
gtk_main_quit(ಶೂನ್ಯ)
{
    gtk4_main_loop_quit = TRUE;
}

#ಆದರೆ defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mshape_css_ಹೆಸರುಗಳು[] =
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

    ಶೂನ್ಯ
mch_ಗಣ_mouse_shape(ಪೂರ್ಣಾಂಕ shape)
{
    GdkCursor	*c;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*css_name = "default";

    ಆದರೆ (gui.drawarea == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (shape == MSHAPE_HIDE || gui.ಸೂಚಕ_hidden)
	gtk_widget_ಗಣ_cursor(gui.drawarea, gui.blank_ಸೂಚಕ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shape < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(mshape_css_ಹೆಸರುಗಳು))
	    css_name = mshape_css_ಹೆಸರುಗಳು[shape];
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, ಖಾಲಿ);
	gtk_widget_ಗಣ_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    ಆದರೆ (shape != MSHAPE_HIDE)
	ಕೊನೆಯ_shape = shape;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_ಪಠ್ಯ_area_w = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_ಪಠ್ಯ_area_h = 0;

/*
 * ============================================================
 * Menu functions
 * ============================================================
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU

# ifdef FEAT_TOOLBAR
/*
 * Icon name table for toolbar buttons.
 * Must match toolbar_names[] in menu.c.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ toolbar_icon_ಹೆಸರುಗಳು[] =
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

    ಸ್ಥಿರ ಶೂನ್ಯ
toolbar_button_clicked_cb(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ)
{
    gui_menu_cb((vimmenu_T *)ದತ್ತಾಂಶ);
}

    ಸ್ಥಿರ GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = ಖಾಲಿ;

    // Try specified icon file first
    ಆದರೆ (menu->iconfile != ಖಾಲಿ)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	ಆದರೆ (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, ಖಾಲಿ);

	    ಆದರೆ (texture != ಖಾಲಿ)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_ಗಣ_ಗಾತ್ರ_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    ಆದರೆ (image == ಖಾಲಿ)
    {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *icon_name = "image-missing";
	ಪೂರ್ಣಾಂಕ n = (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(toolbar_icon_ಹೆಸರುಗಳು);

	ಆದರೆ (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_ಹೆಸರುಗಳು[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    ಹಿಂತಿರುಗಿ image;
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
menu_button_clicked_cb(
	Vim_Menu_ಅಂಶ	    *ಅಂಶ ಬಳಸದ,
	Vim_Menu_ಅಂಶ_Event    event,
	vimmenu_T	    *menu)
{
    ಆದರೆ (event == VIM_MENU_ಅಂಶ_CLICKED)
	gui_menu_cb(menu);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (event == VIM_MENU_ಅಂಶ_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	ಸ್ಥಿರ gboolean did_ಸಂದೇಶ = FALSE;

	ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	    ಹಿಂತಿರುಗಿ;

	tooltip = CONVERT_ಗಮ್ಯ_UTF8(menu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP]);
	ಆದರೆ (tooltip != ಖಾಲಿ && utf_valid_string(tooltip, ಖಾಲಿ))
	{
	    ಸಂದೇಶ((ಅಕ್ಷರ *)tooltip);
	    did_ಸಂದೇಶ = TRUE;
	    setcursor();
	    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (did_ಸಂದೇಶ)
	{
	    ಸಂದೇಶ("");
	    did_ಸಂದೇಶ = FALSE;
	    setcursor();
	    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
	}
	CONVERT_ಗಮ್ಯ_UTF8_FREE(tooltip);
    }
}

    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ idx)
{
    vimmenu_T	*ಪೋಷಕ;
    GtkWidget	*ಪೋಷಕ_widget;
    gboolean	use_mnemonic;
    char_u	*ಪಠ್ಯ;

    ಆದರೆ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_ಗಣ_ಪೋಷಕ(menu->submenu_id, gui.drawarea);
	ಹಿಂತಿರುಗಿ;
    }

    ಪೋಷಕ = menu->ಪೋಷಕ;

    ಆದರೆ ((ಪೋಷಕ != ಖಾಲಿ && ಪೋಷಕ->submenu_id == ಖಾಲಿ)
	    || !menu_is_menubar(menu->name))
	ಹಿಂತಿರುಗಿ;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = ಪೋಷಕ != ಖಾಲಿ || p_wak[0] != 'n';
    ಪಠ್ಯ = translate_mnemonic_ಟ್ಯಾಗ್(menu->name, use_mnemonic);

    ಆದರೆ (ಪೋಷಕ != ಖಾಲಿ)
    {
	ಪೋಷಕ_widget = ಪೋಷಕ->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_ಅಂಶ_new(
		    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ, ಖಾಲಿ, ಖಾಲಿ));

	vim_menu_ಅಂಶ_ಗಣ_submenu(VIM_MENU_ಅಂಶ(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_ಸೇರಿಸು_ಅಂಶ(VIM_MENU(ಪೋಷಕ_widget),
		VIM_MENU_ಅಂಶ(menu->id), idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೋಷಕ_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_ಅಂಶ_new(
		    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_ಸೇರಿಸು_ಅಂಶ(VIM_MENU_BAR(ಪೋಷಕ_widget),
		VIM_MENU_BAR_ಅಂಶ(menu->id), idx);
    }

    vim_free(ಪಠ್ಯ);
}

    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu_ಅಂಶ(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ idx)
{
    vimmenu_T	*ಪೋಷಕ = menu->ಪೋಷಕ;

# ifdef FEAT_TOOLBAR
    ಆದರೆ (ಪೋಷಕ != ಖಾಲಿ && menu_is_toolbar(ಪೋಷಕ->name))
    {
	ಆದರೆ (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_ಸೇರಿಸು_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*ಪಠ್ಯ;
	    char_u	*tooltip;

	    ಪಠ್ಯ    = CONVERT_ಗಮ್ಯ_UTF8(menu->dname);
	    tooltip = CONVERT_ಗಮ್ಯ_UTF8(menu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP]);
	    ಆದರೆ (tooltip != ಖಾಲಿ && !utf_valid_string(tooltip, ಖಾಲಿ))
		CONVERT_ಗಮ್ಯ_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_ಸೇರಿಸು_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ, idx);
	    gtk_widget_ಗಣ_tooltip_ಪಠ್ಯ(btn, (ಸ್ಥಿರಾಂಕ gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_ಗಮ್ಯ_UTF8_FREE(ಪಠ್ಯ);
	    CONVERT_ಗಮ್ಯ_UTF8_FREE(tooltip);
	}
	ಹಿಂತಿರುಗಿ;
    }
# endif

    // Menu items (non-toolbar)
    ಆದರೆ (ಪೋಷಕ == ಖಾಲಿ || ಪೋಷಕ->submenu_id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_ಸೇರಿಸು_separator(
		VIM_MENU(ಪೋಷಕ->submenu_id), idx));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	    *ಪಠ್ಯ;
	char_u	    *accel_ಪಠ್ಯ = ಖಾಲಿ;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	ಪಠ್ಯ = translate_mnemonic_ಟ್ಯಾಗ್(menu->name, use_mnemonic);

	ಆದರೆ (menu->actext != ಖಾಲಿ && menu->actext[0] != NUL)
	    accel_ಪಠ್ಯ = CONVERT_ಗಮ್ಯ_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_ಅಂಶ_new((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಪಠ್ಯ,
		    (Vim_Menu_ಅಂಶ_Func)menu_button_clicked_cb, menu));

	ಆದರೆ (accel_ಪಠ್ಯ != ಖಾಲಿ)
	    vim_menu_ಅಂಶ_ಗಣ_accel(VIM_MENU_ಅಂಶ(menu->id),
		    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)accel_ಪಠ್ಯ);

	vim_menu_ಸೇರಿಸು_ಅಂಶ(VIM_MENU(ಪೋಷಕ->submenu_id),
		VIM_MENU_ಅಂಶ(menu->id), idx);

	vim_free(ಪಠ್ಯ);
	CONVERT_ಗಮ್ಯ_UTF8_FREE(accel_ಪಠ್ಯ);
    }
}

    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಂಕ enable ಬಳಸದ)
{
    // GTK4: tearoff menus don't exist.
}

    ಶೂನ್ಯ
gui_mch_menu_ಗಣ_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    ಆದರೆ (menu->id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    tooltip = CONVERT_ಗಮ್ಯ_UTF8(menu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP]);
    ಆದರೆ (tooltip != ಖಾಲಿ && utf_valid_string(tooltip, ಖಾಲಿ))
	gtk_widget_ಗಣ_tooltip_ಪಠ್ಯ(menu->id, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)tooltip);
    CONVERT_ಗಮ್ಯ_UTF8_FREE(tooltip);
}

    ಶೂನ್ಯ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    ಆದರೆ (menu->ಪೋಷಕ != ಖಾಲಿ && menu_is_toolbar(menu->ಪೋಷಕ->name))
    {
	ಆದರೆ (menu->id != ಖಾಲಿ)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = ಖಾಲಿ;
	}
	ಹಿಂತಿರುಗಿ;
    }

    // For popup menus, unparent the menu as well
    ಆದರೆ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	ಆದರೆ (menu->submenu_id != ಖಾಲಿ)
	    gtk_widget_unparent(menu->submenu_id);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (menu->ಪೋಷಕ == ಖಾಲಿ && menu->id != ಖಾಲಿ)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (menu->id != ಖಾಲಿ)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->ಪೋಷಕ->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    ಶೂನ್ಯ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ಪೂರ್ಣಾಂಕ x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
show_menubar_popover(ಶೂನ್ಯ)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    ಆದರೆ (gui.menubar == ಖಾಲಿ || gui.drawarea == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ;
    }

    // Copy and convert the menubar into a menu popover
    menu = vim_menu_bar_ಗಮ್ಯ_menu(VIM_MENU_BAR(gui.menubar));

    gtk_widget_ಗಣ_ಪೋಷಕ(menu, gui.drawarea);
    gtk_popover_ಗಣ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    rect.x = 0;
    rect.y = 0;
    rect.ಅಗಲ = 1;
    rect.ಎತ್ತರ = 1;
    gtk_popover_ಗಣ_pointing_ಗಮ್ಯ(GTK_POPOVER(menu), &rect);
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), ಖಾಲಿ);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_thumb(scrollbar_T *sb, ದೀರ್ಘ val, ದೀರ್ಘ ಗಾತ್ರ_2, ದೀರ್ಘ max)
{
    GtkAdjustment *adj;

    ಆದರೆ (sb->id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	ಹಿಂತಿರುಗಿ;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_ಗಣ_ಅಧೋ(adj, 0.0);
    gtk_adjustment_ಗಣ_ಊರ್ಧ್ವ(adj, (gdouble)max + 1);
    gtk_adjustment_ಗಣ_ಮೌಲ್ಯ(adj, (gdouble)val);
    gtk_adjustment_ಗಣ_ಹೆಜ್ಜೆ_increment(adj, 1.0);
    gtk_adjustment_ಗಣ_ಪುಟ_increment(adj, (gdouble)(ಗಾತ್ರ_2 > 2 ? ಗಾತ್ರ_2 - 2 : 1));
    gtk_adjustment_ಗಣ_ಪುಟ_ಗಾತ್ರ(adj, (gdouble)ಗಾತ್ರ_2);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_pos(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    ಆದರೆ (sb->id != ಖಾಲಿ)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ formwin_w = gtk_widget_get_ಅಗಲ(gui.formwin);
    ಪೂರ್ಣಾಂಕ sbar_w = 0;
    ಪೂರ್ಣಾಂಕ xpad;

    ಆದರೆ (gui.which_scrollbars[SBAR_ಎಡ])
	sbar_w += gui.scrollbar_ಅಗಲ;
    ಆದರೆ (gui.which_scrollbars[SBAR_ಬಲ])
	sbar_w += gui.scrollbar_ಅಗಲ;

    xpad = formwin_w - ಕೊನೆಯ_ಪಠ್ಯ_area_w - sbar_w;
    ಹಿಂತಿರುಗಿ (xpad < 0) ? 0 : xpad;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ formwin_h = gtk_widget_get_ಎತ್ತರ(gui.formwin);
    ಪೂರ್ಣಾಂಕ ypad;

    ypad = formwin_h - ಕೊನೆಯ_ಪಠ್ಯ_area_h;
    ಆದರೆ (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_ಎತ್ತರ;

    ಹಿಂತಿರುಗಿ (ypad < 0) ? 0 : ypad;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
adjustment_ಮೌಲ್ಯ_ಬದಲಾಯಿತು(GtkAdjustment *adj, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_ದತ್ತಾಂಶ(G_OBJECT(adj), "vim-sb");
    ದೀರ್ಘ ಮೌಲ್ಯ = (ದೀರ್ಘ)gtk_adjustment_get_ಮೌಲ್ಯ(adj);

    ಆದರೆ (sb != ಖಾಲಿ)
	gui_drag_scrollbar(sb, ಮೌಲ್ಯ, FALSE);
}

    ಶೂನ್ಯ
gui_mch_create_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ orient)
{
    GtkAdjustment *adj;
    ಆದರೆ (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ಖಾಲಿ);

    gtk_widget_ಕೂಡಿಸು_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_ಗಣ_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_ಮೌಲ್ಯ_ಬದಲಾಯಿತು), ಖಾಲಿ);
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ಆದರೆ (sb->id != ಖಾಲಿ)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = ಖಾಲಿ;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    ಶೂನ್ಯ
gui_mch_ಗಣ_ಪಠ್ಯ_area_pos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    ಆದರೆ (w <= 0 || h <= 0)
	ಹಿಂತಿರುಗಿ;
    ಕೊನೆಯ_ಪಠ್ಯ_area_w = w;
    ಕೊನೆಯ_ಪಠ್ಯ_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    ಶೂನ್ಯ
gui_gtk4_calculate_bleed(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    ಪೂರ್ಣಾಂಕ old_ಬಲ = gui.bleed_ಬಲ;
    ಪೂರ್ಣಾಂಕ old_bot = gui.bleed_bot;

    gui.bleed_ಬಲ = ಅಗಲ - ಕೊನೆಯ_ಪಠ್ಯ_area_w;
    gui.bleed_bot = ಎತ್ತರ - ಕೊನೆಯ_ಪಠ್ಯ_area_h;

    ಆದರೆ (gui.which_scrollbars[SBAR_ಎಡ])
	gui.bleed_ಬಲ -= gui.scrollbar_ಅಗಲ;
    ಆದರೆ (gui.which_scrollbars[SBAR_ಬಲ])
	gui.bleed_ಬಲ -= gui.scrollbar_ಅಗಲ;
    ಆದರೆ (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_ಎತ್ತರ;

    // Not sure if this can happen, but be safe...
    ಆದರೆ (gui.bleed_ಬಲ < 0)
	gui.bleed_ಬಲ = 0;
    ಆದರೆ (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    ಆದರೆ (old_ಬಲ != gui.bleed_ಬಲ || old_bot != gui.bleed_bot)
	gtk_widget_ಸರತಿ_draw(GTK_WIDGET(gui.drawarea));
}

/*
 * ============================================================
 * Browse dialogs
 * ============================================================
 */

/*
 * Blocking helper: run a GtkFileDialog and wait for result.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    GFile	*ಫಲಿತಾಂಶ;
    gboolean	ಮುಗಿದ;
} File_Dialog_ದತ್ತಾಂಶ;

    ಸ್ಥಿರ ಶೂನ್ಯ
file_dialog_open_cb(GObject *ಮೂಲ, GAsync_ಫಲಿತಾಂಶ *res, gpointer ದತ್ತಾಂಶ)
{
    File_Dialog_ದತ್ತಾಂಶ *fdd = (File_Dialog_ದತ್ತಾಂಶ *)ದತ್ತಾಂಶ;
    fdd->ಫಲಿತಾಂಶ = gtk_file_dialog_open_ಮುಕ್ತಾಯ(
		    GTK_FILE_DIALOG(ಮೂಲ), res, ಖಾಲಿ);
    fdd->ಮುಗಿದ = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
file_dialog_save_cb(GObject *ಮೂಲ, GAsync_ಫಲಿತಾಂಶ *res, gpointer ದತ್ತಾಂಶ)
{
    File_Dialog_ದತ್ತಾಂಶ *fdd = (File_Dialog_ದತ್ತಾಂಶ *)ದತ್ತಾಂಶ;
    fdd->ಫಲಿತಾಂಶ = gtk_file_dialog_save_ಮುಕ್ತಾಯ(
		    GTK_FILE_DIALOG(ಮೂಲ), res, ಖಾಲಿ);
    fdd->ಮುಗಿದ = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
file_dialog_folder_cb(GObject *ಮೂಲ, GAsync_ಫಲಿತಾಂಶ *res, gpointer ದತ್ತಾಂಶ)
{
    File_Dialog_ದತ್ತಾಂಶ *fdd = (File_Dialog_ದತ್ತಾಂಶ *)ದತ್ತಾಂಶ;
    fdd->ಫಲಿತಾಂಶ = gtk_file_dialog_select_folder_ಮುಕ್ತಾಯ(
		    GTK_FILE_DIALOG(ಮೂಲ), res, ಖಾಲಿ);
    fdd->ಮುಗಿದ = TRUE;
}

    char_u *
gui_mch_browse(ಪೂರ್ಣಾಂಕ saving,
	char_u *title,
	char_u *dflt,
	char_u *ext ಬಳಸದ,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    File_Dialog_ದತ್ತಾಂಶ  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *ಫಲಿತಾಂಶ = ಖಾಲಿ;
    GListStore	    *filters;

    title = CONVERT_ಗಮ್ಯ_UTF8(title);

    ಆದರೆ (initdir == ಖಾಲಿ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_ತುಂಬಿದ_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    ಕೂಡಿಸು_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_ಗಣ_modal(dlg, TRUE);
    ಆದರೆ (title != ಖಾಲಿ)
	gtk_file_dialog_ಗಣ_title(dlg, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)title);

    {
	GFile *dir = g_file_new_for_ಮಾರ್ಗ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)dirbuf);
	gtk_file_dialog_ಗಣ_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    ಆದರೆ (filter != ಖಾಲಿ && *filter != NUL)
    {
	ಪೂರ್ಣಾಂಕ		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ಆದರೆ (patt != ಖಾಲಿ)
	{
	    ಆಗ (p != ಖಾಲಿ && *p != NUL)
	    {
		ಆದರೆ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ಆದರೆ (*p == '\t')
			gtk_file_filter_ಗಣ_name(gfilter, (gchar *)patt);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			gtk_file_filter_ಕೂಡಿಸು_ನಮೂನೆ(gfilter, (gchar *)patt);
			ಆದರೆ (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    ಆದರೆ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    gtk_file_dialog_ಗಣ_filters(dlg, G_LIST_MODEL(filters));
    g_object_unref(filters);

    ಆದರೆ (saving && dflt != ಖಾಲಿ && *dflt != NUL)
	gtk_file_dialog_ಗಣ_initial_name(dlg, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)dflt);

    fdd.ಫಲಿತಾಂಶ = ಖಾಲಿ;
    fdd.ಮುಗಿದ = FALSE;

    ಆದರೆ (saving)
	gtk_file_dialog_save(dlg, GTK_ಕಿಟಕಿ(gui.mainwin), ಖಾಲಿ,
		file_dialog_save_cb, &fdd);
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_file_dialog_open(dlg, GTK_ಕಿಟಕಿ(gui.mainwin), ಖಾಲಿ,
		file_dialog_open_cb, &fdd);

    ಆಗ (!fdd.ಮುಗಿದ)
	g_main_context_iteration(ಖಾಲಿ, TRUE);

    ಆದರೆ (fdd.ಫಲಿತಾಂಶ != ಖಾಲಿ)
    {
	ಅಕ್ಷರ *ಮಾರ್ಗ = g_file_get_ಮಾರ್ಗ(fdd.ಫಲಿತಾಂಶ);
	ಆದರೆ (ಮಾರ್ಗ != ಖಾಲಿ)
	{
	    ಫಲಿತಾಂಶ = vim_strsave((char_u *)ಮಾರ್ಗ);
	    g_free(ಮಾರ್ಗ);
	}
	g_object_unref(fdd.ಫಲಿತಾಂಶ);
    }

    g_object_unref(dlg);
    CONVERT_ಗಮ್ಯ_UTF8_FREE(title);

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    File_Dialog_ದತ್ತಾಂಶ	fdd;
    char_u		*ಫಲಿತಾಂಶ = ಖಾಲಿ;

    title = CONVERT_ಗಮ್ಯ_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_ಗಣ_modal(dlg, TRUE);
    ಆದರೆ (title != ಖಾಲಿ)
	gtk_file_dialog_ಗಣ_title(dlg, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)title);

    ಆದರೆ (initdir != ಖಾಲಿ && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_ಮಾರ್ಗ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)initdir);
	gtk_file_dialog_ಗಣ_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.ಫಲಿತಾಂಶ = ಖಾಲಿ;
    fdd.ಮುಗಿದ = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_ಕಿಟಕಿ(gui.mainwin), ಖಾಲಿ,
	    file_dialog_folder_cb, &fdd);

    ಆಗ (!fdd.ಮುಗಿದ)
	g_main_context_iteration(ಖಾಲಿ, TRUE);

    ಆದರೆ (fdd.ಫಲಿತಾಂಶ != ಖಾಲಿ)
    {
	ಅಕ್ಷರ *ಮಾರ್ಗ = g_file_get_ಮಾರ್ಗ(fdd.ಫಲಿತಾಂಶ);
	ಆದರೆ (ಮಾರ್ಗ != ಖಾಲಿ)
	{
	    ಫಲಿತಾಂಶ = vim_strsave((char_u *)ಮಾರ್ಗ);
	    g_free(ಮಾರ್ಗ);
	}
	g_object_unref(fdd.ಫಲಿತಾಂಶ);
    }

    g_object_unref(dlg);
    CONVERT_ಗಮ್ಯ_UTF8_FREE(title);

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
    ಸ್ಥಿರ ಅಕ್ಷರ **
ವಿಭಜಿಸು_button_string(char_u *button_string, ಪೂರ್ಣಾಂಕ *n_buttons, ಅಕ್ಷರ **tofree)
{
    ಅಕ್ಷರ	    **array;
    char_u	    *p;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    ಎಣಿಕೆ_2 = 1;

    button_string = (char_u *)g_strdup((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)button_string);

    ಗಾಗಿ (p = button_string; *p != NUL; ++p)
	ಆದರೆ (*p == DLG_BUTTON_SEP)
	    ++ಎಣಿಕೆ_2;

    array = g_malloc_n(ಎಣಿಕೆ_2, ಗಾತ್ರ(ಅಕ್ಷರ *));
    ಎಣಿಕೆ_2 = 0;

    ಆದರೆ (array != ಖಾಲಿ)
    {
	array[ಎಣಿಕೆ_2++] = (ಅಕ್ಷರ *)button_string;
	ಗಾಗಿ (p = button_string; *p != NUL; )
	{
	    ಆದರೆ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[ಎಣಿಕೆ_2++] = (ಅಕ್ಷರ *)p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ಇಲ್ಲದಿದ್ದರೆ
		MB_PTR_ADV(p);
	}
    }

    *tofree = (ಅಕ್ಷರ *)button_string;
    *n_buttons = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
dialog_type_ಗಮ್ಯ_icon(ಪೂರ್ಣಾಂಕ type)
{
    ಆಯ್ಕೆ (type)
    {
	ಸ್ಥಿತಿ VIM_ERROR:
	    ಹಿಂತಿರುಗಿ "dialog-error-symbolic";
	ಸ್ಥಿತಿ VIM_WARNING:
	    ಹಿಂತಿರುಗಿ "dialog-warning-symbolic";
	ಸ್ಥಿತಿ VIM_INFO:
	    ಹಿಂತಿರುಗಿ "dialog-information-symbolic";
	ಸ್ಥಿತಿ VIM_QUESTION:
	    ಹಿಂತಿರುಗಿ "dialog-question-symbolic";
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

// Data associated with each button in the dialog
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		but_idx;
    ಪೂರ್ಣಾಂಕ		*response;
    gboolean	*ಮುಗಿದ;
} DialogButtonState;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    GtkWidget	*win;
    gboolean	*ಮುಗಿದ;
    gboolean	no_alt;
} DialogState;

    ಸ್ಥಿರ ಶೂನ್ಯ
dialog_button_clicked_cb(GtkButton *button ಬಳಸದ, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->ಮುಗಿದ = TRUE;
}

    ಸ್ಥಿರ gboolean
dialog_ಕೀಲಿ_pressed_cb(
	Gtk_Event_Controller_ಕೀಲಿ	*controller ಬಳಸದ,
	guint			keyval,
	guint			keycode ಬಳಸದ,
	GdkModifierType		state,
	DialogState		*dstate)
{
    ಆದರೆ (keyval == GDK_ಕೀಲಿ_Escape)
    {
	*dstate->ಮುಗಿದ = TRUE;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_ಮುಖವಾಡ()))
	ಹಿಂತಿರುಗಿ gtk_widget_mnemonic_activate(dstate->win, FALSE);
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ gboolean
dialog_close_request_cb(Gtk_ಕಿಟಕಿ *win ಬಳಸದ, gboolean *win_closed)
{
    *win_closed = TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_dialog(
	ಪೂರ್ಣಾಂಕ	type,
	char_u	*title,
	char_u	*ಸಂದೇಶ_2,
	char_u	*buttons,
	ಪೂರ್ಣಾಂಕ	def_but,
	char_u	*textfield,
	ಪೂರ್ಣಾಂಕ	ex_cmd ಬಳಸದ)
{
    Gtk_ಕಿಟಕಿ		*win = GTK_ಕಿಟಕಿ(gtk_ಕಿಟಕಿ_new());
    GtkWidget		*vertbox;
    GtkWidget		*ಸಂದೇಶ_box;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*ನಮೂದು = ಖಾಲಿ;
    char_u		*utf8_title;
    char_u		*utf8_ಸಂದೇಶ;
    GtkEventController *ಕೀಲಿ_controller;
    DialogButtonState	*but_ಸ್ಥಿತಿಗಳು = ಖಾಲಿ;
    ಅಕ್ಷರ		*tofree = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			response = -1;
    gboolean		ಮುಗಿದ = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_ಗಮ್ಯ_UTF8(title);
    ಆದರೆ (utf8_title != ಖಾಲಿ)
	gtk_ಕಿಟಕಿ_ಗಣ_title(win, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)utf8_title);
    CONVERT_ಗಮ್ಯ_UTF8_FREE(utf8_title);

    gtk_ಕಿಟಕಿ_ಗಣ_transient_for(win, GTK_ಕಿಟಕಿ(gui.mainwin));
    gtk_ಕಿಟಕಿ_ಗಣ_modal(win, TRUE);
    gtk_ಕಿಟಕಿ_ಗಣ_default_ಗಾತ್ರ(win, 300, -1);
    gtk_ಕಿಟಕಿ_ಗಣ_destroy_with_ಪೋಷಕ(win, TRUE);
    g_signal_connect(win, "close-request",
	    G_CALLBACK(dialog_close_request_cb), &win_closed);

    // Create main vertical layout container
    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_ಕಿಟಕಿ_ಗಣ_ಮಗು(win, vertbox);
    gtk_widget_ಗಣ_margin_ತುದಿ(vertbox, 24);
    gtk_widget_ಗಣ_margin_bottom(vertbox, 24);
    gtk_widget_ಗಣ_margin_ಆರಂಭ(vertbox, 12);
    gtk_widget_ಗಣ_margin_ಕೊನೆ(vertbox, 12);

    // Add the message label
    ಸಂದೇಶ_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_ಗಣ_halign(ಸಂದೇಶ_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), ಸಂದೇಶ_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_ಗಮ್ಯ_icon(type);
    ಆದರೆ (icon_name != ಖಾಲಿ)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_ಗಣ_icon_ಗಾತ್ರ(GTK_IMAGE(icon), GTK_ICON_ಗಾತ್ರ_LARGE);
	gtk_box_append(GTK_BOX(ಸಂದೇಶ_box), icon);
    }

    utf8_ಸಂದೇಶ = CONVERT_ಗಮ್ಯ_UTF8(ಸಂದೇಶ_2);
    label = gtk_label_new((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)utf8_ಸಂದೇಶ);
    CONVERT_ಗಮ್ಯ_UTF8_FREE(utf8_ಸಂದೇಶ);
    gtk_label_ಗಣ_wrap(GTK_LABEL(label), TRUE);
    gtk_label_ಗಣ_max_ಅಗಲ_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(ಸಂದೇಶ_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    ಕೀಲಿ_controller = gtk_event_controller_ಕೀಲಿ_new();
    g_signal_connect(ಕೀಲಿ_controller, "key-pressed",
	    G_CALLBACK(dialog_ಕೀಲಿ_pressed_cb), &state);
    gtk_widget_ಕೂಡಿಸು_controller(GTK_WIDGET(win), ಕೀಲಿ_controller);
    state.ಮುಗಿದ = &ಮುಗಿದ;
    state.win = GTK_WIDGET(win);

    ಆದರೆ (textfield != ಖಾಲಿ)
    {
	// Add text entry so user can enter text
	char_u *utf8_ಪಠ್ಯ = CONVERT_ಗಮ್ಯ_UTF8(textfield);

	ನಮೂದು = gtk_ನಮೂದು_new();

	ಆದರೆ (utf8_ಪಠ್ಯ != ಖಾಲಿ)
	    gtk_editable_ಗಣ_ಪಠ್ಯ(GTK_EDITABLE(ನಮೂದು), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)utf8_ಪಠ್ಯ);
	ಇಲ್ಲದಿದ್ದರೆ
	    gtk_editable_ಗಣ_ಪಠ್ಯ(GTK_EDITABLE(ನಮೂದು), "");
	CONVERT_ಗಮ್ಯ_UTF8_FREE(utf8_ಪಠ್ಯ);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_ನಮೂದು_ಗಣ_activates_default(GTK_ನಮೂದು(ನಮೂದು), TRUE);
	gtk_box_append(GTK_BOX(vertbox), ನಮೂದು);
	state.no_alt = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	GListModel  *controllers;
	ಪೂರ್ಣಾಂಕ	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_ವಸ್ತುಗಳು(controllers);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_ಅಂಶ(controllers, i);
	    ಆದರೆ (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_ಗಣ_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_ಕಿಟಕಿ_ಗಣ_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    ಆದರೆ (buttons != ಖಾಲಿ)
    {
	GtkWidget   *but_box;
	ಅಕ್ಷರ	    **buttons_arr; // Note that array is allocated, not strings
	ಪೂರ್ಣಾಂಕ	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	ಆದರೆ (vim_strchr(p_go, GO_VERTICAL) != ಖಾಲಿ)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	ಇಲ್ಲದಿದ್ದರೆ
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_ಗಣ_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_ಗಣ_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = ವಿಭಜಿಸು_button_string(buttons, &n_buttons, &tofree);

	but_ಸ್ಥಿತಿಗಳು = g_malloc_n(n_buttons, ಗಾತ್ರ(DialogButtonState));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_ಸ್ಥಿತಿಗಳು + i;

	    but_label = CONVERT_ಗಮ್ಯ_UTF8((char_u *)buttons_arr[i]);
	    ಆದರೆ (but_label == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    but = gtk_button_new_with_mnemonic((ಅಕ್ಷರ *)but_label);
	    ಆದರೆ (i == def_but - 1)
		gtk_ಕಿಟಕಿ_ಗಣ_default_widget(win, but);
	    gtk_box_append(GTK_BOX(but_box), but);
	    CONVERT_ಗಮ್ಯ_UTF8_FREE(but_label);

	    but_state->but_idx = i;
	    but_state->response = &response;
	    but_state->ಮುಗಿದ = &ಮುಗಿದ;

	    g_signal_connect(but, "clicked",
		    G_CALLBACK(dialog_button_clicked_cb), but_state);
	}
	g_free(buttons_arr);
    }

    gtk_ಕಿಟಕಿ_present(win);

    ಆಗ (!ಮುಗಿದ && !win_closed)
	g_main_context_iteration(ಖಾಲಿ, TRUE);

    ಆದರೆ (ಮುಗಿದ)
    {
	ಆದರೆ (textfield != ಖಾಲಿ)
	{
	    // Get the text the user entered
	    char_u *ಪಠ್ಯ;

	    ಪಠ್ಯ = (char_u *)gtk_editable_get_ಪಠ್ಯ(GTK_EDITABLE(ನಮೂದು));
	    ಪಠ್ಯ = CONVERT_FROM_UTF8(ಪಠ್ಯ);
	    vim_strncpy(textfield, ಪಠ್ಯ, IOSIZE - 1);
	    CONVERT_FROM_UTF8_FREE(ಪಠ್ಯ);
	}

	gtk_ಕಿಟಕಿ_destroy(win);
    }
    g_free(but_ಸ್ಥಿತಿಗಳು);
    g_free(tofree);

    // Vim buttons are indexed starting from one.
    ಹಿಂತಿರುಗಿ response == -1 ? 0 : response + 1;
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    GtkWidget *dialog;
    GtkWidget *what;	    // Find what entry
    GtkWidget *with;	    // Replace with entry
    GtkWidget *wword;	    // Whole word check
    GtkWidget *mcase;	    // Match case check
    GtkWidget *ಮೇಲೆ;	    // Direction up radio
    GtkWidget *ಕೆಳಕ್ಕೆ;	    // Direction down radio
    GtkWidget *ಕಂಡುಹಿಡಿ;	    // 'Find Next' action button
    GtkWidget *replace;	    // 'Replace With' action button
    GtkWidget *all;	    // 'Replace All' action button
} Shared_ಕಂಡುಹಿಡಿ_Replace_2;

ಸ್ಥಿರ Shared_ಕಂಡುಹಿಡಿ_Replace_2 ಕಂಡುಹಿಡಿ_widgets = {0};
ಸ್ಥಿರ Shared_ಕಂಡುಹಿಡಿ_Replace_2 repl_widgets = {0};

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_replace_cb(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ)
{
    ಪೂರ್ಣಾಂಕ			flags;
    char_u		*ಕಂಡುಹಿಡಿ_ಪಠ್ಯ;
    char_u		*repl_ಪಠ್ಯ;
    gboolean		direction_ಕೆಳಕ್ಕೆ;
    Shared_ಕಂಡುಹಿಡಿ_Replace_2	*sfr;

    flags = GPOINTER_ಗಮ್ಯ_INT(ದತ್ತಾಂಶ);

    ಆದರೆ (flags == FRD_FINDNEXT)
    {
	repl_ಪಠ್ಯ = ಖಾಲಿ;
	sfr = &ಕಂಡುಹಿಡಿ_widgets;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	repl_ಪಠ್ಯ = (char_u *)gtk_editable_get_ಪಠ್ಯ(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    ಕಂಡುಹಿಡಿ_ಪಠ್ಯ = (char_u *)gtk_editable_get_ಪಠ್ಯ(GTK_EDITABLE(sfr->what));
    direction_ಕೆಳಕ್ಕೆ = gtk_check_button_get_ಸಕ್ರಿಯ(
	    GTK_CHECK_BUTTON(sfr->ಕೆಳಕ್ಕೆ));

    ಆದರೆ (gtk_check_button_get_ಸಕ್ರಿಯ(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_ಸಂಪೂರ್ಣ_ಶಬ್ದ;
    ಆದರೆ (gtk_check_button_get_ಸಕ್ರಿಯ(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_ಹೊಂದಿಕೆ_CASE;

    repl_ಪಠ್ಯ = CONVERT_FROM_UTF8(repl_ಪಠ್ಯ);
    ಕಂಡುಹಿಡಿ_ಪಠ್ಯ = CONVERT_FROM_UTF8(ಕಂಡುಹಿಡಿ_ಪಠ್ಯ);
    gui_do_findrepl(flags, ಕಂಡುಹಿಡಿ_ಪಠ್ಯ, repl_ಪಠ್ಯ, direction_ಕೆಳಕ್ಕೆ);
    CONVERT_FROM_UTF8_FREE(repl_ಪಠ್ಯ);
    CONVERT_FROM_UTF8_FREE(ಕಂಡುಹಿಡಿ_ಪಠ್ಯ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dialog_destroyed_cb(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ)
{
    *(GtkWidget **)ದತ್ತಾಂಶ = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ನಮೂದು_ಬದಲಾಯಿತು_cb(GtkWidget *ನಮೂದು, GtkWidget *dialog)
{
    ಸ್ಥಿರಾಂಕ gchar	*ನಮೂದು_ಪಠ್ಯ;
    gboolean	nonempty;

    ನಮೂದು_ಪಠ್ಯ = gtk_editable_get_ಪಠ್ಯ(GTK_EDITABLE(ನಮೂದು));

    ಆದರೆ (!ನಮೂದು_ಪಠ್ಯ)
	ಹಿಂತಿರುಗಿ; // Shouldn't happen

    nonempty = (ನಮೂದು_ಪಠ್ಯ[0] != '\0');

    ಆದರೆ (dialog == ಕಂಡುಹಿಡಿ_widgets.dialog)
	gtk_widget_ಗಣ_sensitive(ಕಂಡುಹಿಡಿ_widgets.ಕಂಡುಹಿಡಿ, nonempty);

    ಆದರೆ (dialog == repl_widgets.dialog)
    {
	gtk_widget_ಗಣ_sensitive(repl_widgets.ಕಂಡುಹಿಡಿ, nonempty);
	gtk_widget_ಗಣ_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_ಗಣ_sensitive(repl_widgets.all, nonempty);
    }
}

    ಸ್ಥಿರ gboolean
ಕಂಡುಹಿಡಿ_ಕೀಲಿ_pressed_cb(
	Gtk_Event_Controller_ಕೀಲಿ	*controller ಬಳಸದ,
	guint			keyval,
	guint			keycode ಬಳಸದ,
	GdkModifierType		state,
	Shared_ಕಂಡುಹಿಡಿ_Replace_2	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ಆದರೆ (state & (GDK_CONTROL_ಮುಖವಾಡ | GDK_ಸರಿಸು_ಮುಖವಾಡ))
	ಹಿಂತಿರುಗಿ FALSE;

    // the Escape key synthesizes a cancellation action
    ಆದರೆ (keyval == GDK_ಕೀಲಿ_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_ಕಿಟಕಿ_destroy(GTK_ಕಿಟಕಿ(frdp->dialog));
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ನಮೂದು_activate_cb(GtkWidget *widget ಬಳಸದ, ಶೂನ್ಯ *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_replace_dialog_create(char_u *arg, ಪೂರ್ಣಾಂಕ do_replace)
{
    Shared_ಕಂಡುಹಿಡಿ_Replace_2	*frdp;
    char_u		*ನಮೂದು_ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ			wword = FALSE;
    ಪೂರ್ಣಾಂಕ			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*ಕೀಲಿ_controller;

    frdp = do_replace ? &repl_widgets : &ಕಂಡುಹಿಡಿ_widgets;
    ನಮೂದು_ಪಠ್ಯ = get_ಕಂಡುಹಿಡಿ_dialog_ಪಠ್ಯ(arg, &wword, &mcase);

    ಆದರೆ (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = ನಮೂದು_ಪಠ್ಯ;
	ನಮೂದು_ಪಠ್ಯ = string_convert(&output_conv, ನಮೂದು_ಪಠ್ಯ, ಖಾಲಿ);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    ಆದರೆ (frdp->dialog != ಖಾಲಿ)
    {
	ಆದರೆ (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ)
	{
	    gtk_editable_ಗಣ_ಪಠ್ಯ(GTK_EDITABLE(frdp->what),
		    (ಅಕ್ಷರ *)ನಮೂದು_ಪಠ್ಯ);
	    gtk_check_button_ಗಣ_ಸಕ್ರಿಯ(GTK_CHECK_BUTTON(frdp->wword),
		    (gboolean)wword);
	    gtk_check_button_ಗಣ_ಸಕ್ರಿಯ(GTK_CHECK_BUTTON(frdp->mcase),
		    (gboolean)mcase);
	}
	gtk_ಕಿಟಕಿ_present(GTK_ಕಿಟಕಿ(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	ಆದರೆ (do_replace && g_utf8_strlen(
		    gtk_editable_get_ಪಠ್ಯ(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(ನಮೂದು_ಪಠ್ಯ);
	ಹಿಂತಿರುಗಿ;
    }

    // Create a new dialog window.
    frdp->dialog = gtk_ಕಿಟಕಿ_new();
    gtk_ಕಿಟಕಿ_ಗಣ_transient_for(GTK_ಕಿಟಕಿ(frdp->dialog),
	    GTK_ಕಿಟಕಿ(gui.mainwin));
    gtk_ಕಿಟಕಿ_ಗಣ_destroy_with_ಪೋಷಕ(GTK_ಕಿಟಕಿ(frdp->dialog), TRUE);
    gtk_ಕಿಟಕಿ_ಗಣ_title(GTK_ಕಿಟಕಿ(frdp->dialog),
	    do_replace ? _("VIM - Search and Replace...")
	    : _("VIM - Search..."));
    gtk_ಕಿಟಕಿ_ಗಣ_resizable(GTK_ಕಿಟಕಿ(frdp->dialog), FALSE);

    g_signal_connect(frdp->dialog, "destroy",
	    G_CALLBACK(dialog_destroyed_cb), &frdp->dialog);

    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_ಗಣ_margin_ಆರಂಭ(vertbox, 12);
    gtk_widget_ಗಣ_margin_ಕೊನೆ(vertbox, 12);
    gtk_widget_ಗಣ_margin_ತುದಿ(vertbox, 12);
    gtk_widget_ಗಣ_margin_bottom(vertbox, 12);
    gtk_ಕಿಟಕಿ_ಗಣ_ಮಗು(GTK_ಕಿಟಕಿ(frdp->dialog), vertbox);

    // Grid for labels + entries
    grid = gtk_grid_new();
    gtk_grid_ಗಣ_ಅಡ್ಡಸಾಲು_spacing(GTK_GRID(grid), 6);
    gtk_grid_ಗಣ_ಸ್ತಂಭ_spacing(GTK_GRID(grid), 6);
    gtk_box_append(GTK_BOX(vertbox), grid);

    // "Find what:" label + entry
    tmp = gtk_label_new(_("Find what:"));
    gtk_label_ಗಣ_xalign(GTK_LABEL(tmp), 0.0);
    gtk_grid_attach(GTK_GRID(grid), tmp, 0, 0, 1, 1);

    frdp->what = gtk_ನಮೂದು_new();
    gtk_widget_ಗಣ_hexpand(frdp->what, TRUE);
    sensitive = (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ && ನಮೂದು_ಪಠ್ಯ[0] != NUL);
    ಆದರೆ (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ)
	gtk_editable_ಗಣ_ಪಠ್ಯ(GTK_EDITABLE(frdp->what), (ಅಕ್ಷರ *)ನಮೂದು_ಪಠ್ಯ);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    ಆದರೆ (do_replace)
    {
	// "Replace with:" label + entry
	tmp = gtk_label_new(_("Replace with:"));
	gtk_label_ಗಣ_xalign(GTK_LABEL(tmp), 0.0);
	gtk_grid_attach(GTK_GRID(grid), tmp, 0, 1, 1, 1);

	frdp->with = gtk_ನಮೂದು_new();
	gtk_widget_ಗಣ_hexpand(frdp->with, TRUE);
	gtk_grid_attach(GTK_GRID(grid), frdp->with, 1, 1, 1, 1);
	gtk_ನಮೂದು_ಗಣ_activates_default(GTK_ನಮೂದು(frdp->with), TRUE);

	// Make the entry activation only change the input focus onto the
	// with item.
	gtk_ನಮೂದು_ಗಣ_activates_default(GTK_ನಮೂದು(frdp->what), FALSE);
	g_signal_connect(G_OBJECT(frdp->what), "activate",
		G_CALLBACK(ನಮೂದು_activate_cb), frdp->with);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Make the entry activation do the search.
	gtk_ನಮೂದು_ಗಣ_activates_default(GTK_ನಮೂದು(frdp->what), TRUE);

    // Checkboxes
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    frdp->wword = gtk_check_button_new_with_label(_("Match whole word only"));
    gtk_check_button_ಗಣ_ಸಕ್ರಿಯ(GTK_CHECK_BUTTON(frdp->wword),
	    (gboolean)wword);
    gtk_box_append(GTK_BOX(hbox), frdp->wword);

    frdp->mcase = gtk_check_button_new_with_label(_("Match case"));
    gtk_check_button_ಗಣ_ಸಕ್ರಿಯ(GTK_CHECK_BUTTON(frdp->mcase),
	    (gboolean)mcase);
    gtk_box_append(GTK_BOX(hbox), frdp->mcase);

    // Direction radio buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    tmp = gtk_label_new(_("Direction:"));
    gtk_box_append(GTK_BOX(hbox), tmp);

    frdp->ಮೇಲೆ = gtk_check_button_new_with_label(_("Up"));
    gtk_box_append(GTK_BOX(hbox), frdp->ಮೇಲೆ);

    frdp->ಕೆಳಕ್ಕೆ = gtk_check_button_new_with_label(_("Down"));
    gtk_check_button_ಗಣ_ಗುಂಪು(GTK_CHECK_BUTTON(frdp->ಕೆಳಕ್ಕೆ),
	    GTK_CHECK_BUTTON(frdp->ಮೇಲೆ));
    gtk_check_button_ಗಣ_ಸಕ್ರಿಯ(GTK_CHECK_BUTTON(frdp->ಕೆಳಕ್ಕೆ), TRUE);
    gtk_box_append(GTK_BOX(hbox), frdp->ಕೆಳಕ್ಕೆ);

    // Action buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_ಗಣ_halign(hbox, GTK_ALIGN_ಕೊನೆ);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    btn = gtk_button_new_with_label(_("Find Next"));
    gtk_widget_ಗಣ_sensitive(btn, sensitive);
    frdp->ಕಂಡುಹಿಡಿ = btn;

    ಕೀಲಿ_controller = gtk_event_controller_ಕೀಲಿ_new();
    g_signal_connect(ಕೀಲಿ_controller, "key-pressed",
	    G_CALLBACK(ಕಂಡುಹಿಡಿ_ಕೀಲಿ_pressed_cb), frdp);
    gtk_widget_ಕೂಡಿಸು_controller(GTK_WIDGET(frdp->dialog), ಕೀಲಿ_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(ನಮೂದು_ಬದಲಾಯಿತು_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_ಕಿಟಕಿ_ಗಣ_default_widget(GTK_ಕಿಟಕಿ(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(ಕಂಡುಹಿಡಿ_replace_cb),
	    GINT_ಗಮ್ಯ_ಸೂಚಕ(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    ಆದರೆ (do_replace)
    {
	btn = gtk_button_new_with_label(_("Replace"));
	gtk_widget_ಗಣ_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(ಕಂಡುಹಿಡಿ_replace_cb),
		GINT_ಗಮ್ಯ_ಸೂಚಕ(FRD_REPLACE));
	frdp->replace = btn;
	gtk_box_append(GTK_BOX(hbox), btn);

	btn = gtk_button_new_with_label(_("Replace All"));
	gtk_widget_ಗಣ_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(ಕಂಡುಹಿಡಿ_replace_cb),
		GINT_ಗಮ್ಯ_ಸೂಚಕ(FRD_REPLACEALL));
	frdp->all = btn;
	gtk_box_append(GTK_BOX(hbox), btn);
    }

    btn = gtk_button_new_with_label(_("Close"));
    g_signal_connect_swapped(btn, "clicked",
	    G_CALLBACK(gtk_ಕಿಟಕಿ_destroy), frdp->dialog);
    gtk_box_append(GTK_BOX(hbox), btn);

    gtk_ಕಿಟಕಿ_present(GTK_ಕಿಟಕಿ(frdp->dialog));
    gtk_widget_grab_focus(frdp->what);
    ಆದರೆ (do_replace && ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ && ನಮೂದು_ಪಠ್ಯ[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(ನಮೂದು_ಪಠ್ಯ);
}

    ಶೂನ್ಯ
gui_mch_ಕಂಡುಹಿಡಿ_dialog(exarg_T *eap)
{
    ಆದರೆ (gui.ಒಳ_use)
	ಕಂಡುಹಿಡಿ_replace_dialog_create(eap->arg, FALSE);
}

    ಶೂನ್ಯ
gui_mch_replace_dialog(exarg_T *eap)
{
    ಆದರೆ (gui.ಒಳ_use)
	ಕಂಡುಹಿಡಿ_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    ಶೂನ್ಯ
ex_helpfind(exarg_T *eap ಬಳಸದ)
{
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

/*
 * ============================================================
 * Native print dialog
 * ============================================================
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK4_PRINT_DIALOG
ಸ್ಥಿರ GtkPrintDialog *print_dialog = ಖಾಲಿ;
ಸ್ಥಿರ GtkPrintSettings *print_settings = ಖಾಲಿ;
ಸ್ಥಿರ Gtk_ಪುಟ_ಸಿದ್ಧತೆ *ಪುಟ_ಸಿದ್ಧತೆ = ಖಾಲಿ;
ಸ್ಥಿರ Gtk_Print_ಸಿದ್ಧತೆ *print_ಸಿದ್ಧತೆ = ಖಾಲಿ;
ಸ್ಥಿರ GFile *print_file;

    ಸ್ಥಿರ ಶೂನ್ಯ
print_dialog_cb(GtkPrintDialog *dialog, GAsync_ಫಲಿತಾಂಶ *ಫಲಿತಾಂಶ, gboolean *ಮುಗಿದ)
{
    GError *error = ಖಾಲಿ;
    print_ಸಿದ್ಧತೆ = gtk_print_dialog_ಸಿದ್ಧತೆ_ಮುಕ್ತಾಯ(dialog, ಫಲಿತಾಂಶ, &error);

    ಆದರೆ (print_ಸಿದ್ಧತೆ == ಖಾಲಿ)
    {
	semsg(_(e_print_error_str), error->ಸಂದೇಶ_2);
	g_error_free(error);
    }
    *ಮುಗಿದ = TRUE;
}

/*
 * Return file to write to on success (allocated string), or NULL on failure
 * (e.g. user cancelled)
 */
    char_u *
gui_gtk4_print_dialog(
	prt_settings_T	*psettings,
	char_u		*jobname,
	ದ್ವಿಗುಣ		*ಪುಟ_ಅಗಲ,
	ದ್ವಿಗುಣ		*ಪುಟ_ಎತ್ತರ)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    ಮುಗಿದ = FALSE;
    char_u	    *ತಾತ್ಕಾಲಿಕ;

    gtk_print_dialog_ಗಣ_title(dialog, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)jobname);
    gtk_print_dialog_ಗಣ_modal(dialog, TRUE);

    print_dialog = ಖಾಲಿ;
    print_ಸಿದ್ಧತೆ = ಖಾಲಿ;

    ಆದರೆ (print_settings == ಖಾಲಿ)
    {
	option_ಕೋಷ್ಟಕ_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	ಆದರೆ (opt->present)
	{
	    ಆದರೆ (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_ಗಣ_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_ಗಣ_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    ಇಲ್ಲದಿದ್ದರೆ
		gtk_print_settings_ಗಣ_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	ಆದರೆ (opt->present)
	{
	    ಆದರೆ (*opt->string == 'y')
		gtk_print_settings_ಗಣ_orientation(print_settings,
			GTK_ಪುಟ_ORIENTATION_PORTRAIT);
	    ಇಲ್ಲದಿದ್ದರೆ
		gtk_print_settings_ಗಣ_orientation(print_settings,
			GTK_ಪುಟ_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	ಆದರೆ (opt->present)
	{
	    gtk_print_settings_ಗಣ_paper_ಗಾತ್ರ(print_settings,
		    gtk_paper_ಗಾತ್ರ_new((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	ಆದರೆ (opt->present)
	{
	    ಆದರೆ (*opt->string == 'y')
		gtk_print_settings_ಗಣ_collate(print_settings, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ
		gtk_print_settings_ಗಣ_collate(print_settings, FALSE);
	}
    }

    ಆದರೆ (ಪುಟ_ಸಿದ್ಧತೆ == ಖಾಲಿ)
	ಪುಟ_ಸಿದ್ಧತೆ = gtk_ಪುಟ_ಸಿದ್ಧತೆ_new();

    gtk_print_dialog_ಗಣ_print_settings(dialog, print_settings);
    gtk_print_dialog_ಗಣ_ಪುಟ_ಸಿದ್ಧತೆ(dialog, ಪುಟ_ಸಿದ್ಧತೆ);

    gtk_print_dialog_ಸಿದ್ಧತೆ(dialog, GTK_ಕಿಟಕಿ(gui.mainwin), ಖಾಲಿ,
	    (GAsyncReadyCallback)print_dialog_cb, &ಮುಗಿದ);

    ಆಗ (!ಮುಗಿದ)
	g_main_context_iteration(ಖಾಲಿ, TRUE);

    ಆದರೆ (print_ಸಿದ್ಧತೆ == ಖಾಲಿ)
    {
	g_object_unref(dialog);
	g_clear_ಸೂಚಕ(&print_ಸಿದ್ಧತೆ, gtk_print_ಸಿದ್ಧತೆ_unref);

	psettings->user_abort_ಸಂದೇಶ = FALSE;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    g_clear_object(&print_settings);
    g_clear_object(&ಪುಟ_ಸಿದ್ಧತೆ);
    print_settings = g_object_ref(
	    gtk_print_ಸಿದ್ಧತೆ_get_print_settings(print_ಸಿದ್ಧತೆ));
    ಪುಟ_ಸಿದ್ಧತೆ = g_object_ref(gtk_print_ಸಿದ್ಧತೆ_get_ಪುಟ_ಸಿದ್ಧತೆ(print_ಸಿದ್ಧತೆ));

    {
	GtkPrintDuplex duplex = gtk_print_settings_get_duplex(print_settings);
	psettings->duplex = duplex != GTK_PRINT_DUPLEX_SIMPLEX;
    }

    psettings->has_color = gtk_print_settings_get_use_color(print_settings);

    *ಪುಟ_ಅಗಲ = gtk_ಪುಟ_ಸಿದ್ಧತೆ_get_paper_ಅಗಲ(ಪುಟ_ಸಿದ್ಧತೆ,
	    GTK_UNIT_POINTS);
    *ಪುಟ_ಎತ್ತರ = gtk_ಪುಟ_ಸಿದ್ಧತೆ_get_paper_ಎತ್ತರ(ಪುಟ_ಸಿದ್ಧತೆ,
	    GTK_UNIT_POINTS);

    ತಾತ್ಕಾಲಿಕ  = vim_tempname('p', FALSE);
    ಆದರೆ (ತಾತ್ಕಾಲಿಕ == ಖಾಲಿ)
    {
	emsg(_(e_cant_ಕಂಡುಹಿಡಿ_ತಾತ್ಕಾಲಿಕ_file_for_writing));
	g_clear_object(&dialog);
	g_clear_ಸೂಚಕ(&print_ಸಿದ್ಧತೆ, gtk_print_ಸಿದ್ಧತೆ_unref);

	psettings->user_abort_ಸಂದೇಶ = FALSE;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    print_file = g_file_new_for_ಮಾರ್ಗ((ಅಕ್ಷರ *)ತಾತ್ಕಾಲಿಕ);

    print_dialog = dialog;

    ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsync_ಫಲಿತಾಂಶ	*ಫಲಿತಾಂಶ,
	GFile		*file)
{
    ಅಕ್ಷರ *ಮಾರ್ಗ = g_file_get_ಮಾರ್ಗ(file);

    (ಶೂನ್ಯ)gtk_print_dialog_print_file_ಮುಕ್ತಾಯ(dialog, ಫಲಿತಾಂಶ, ಖಾಲಿ);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(ಮಾರ್ಗ);
    g_free(ಮಾರ್ಗ);
}


    ಶೂನ್ಯ
gui_gtk4_print_ಮುಕ್ತಾಯ(ಶೂನ್ಯ)
{
    ಆದರೆ (print_dialog != ಖಾಲಿ)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_ಕಿಟಕಿ(gui.mainwin),
		print_ಸಿದ್ಧತೆ, print_file, ಖಾಲಿ,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = ಖಾಲಿ;
	g_clear_ಸೂಚಕ(&print_ಸಿದ್ಧತೆ, gtk_print_ಸಿದ್ಧತೆ_unref);
    }
}
#ಆದರೆ_ಅಂತ್ಯ // USE_GTK4_PRINT_DIALOG

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE_GUI

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    GdkTexture *texture;
} image_gdk_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    Gsk_Render_ಗಂಟು   **nodes; // Note that elements may be NULL
    ಪೂರ್ಣಾಂಕ		    n_nodes;
} image_placement_gdk_T;

    ಪೂರ್ಣಾಂಕ
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    Gdk_ಸ್ಮರಣೆ_Format fmt;
    size_t	    stride;
    GBytes	    *ಬೈಟುಗಳು;
    size_t	    ಗಾತ್ರ_2;
    ಪೂರ್ಣಾಂಕ		    w, h;

    ಆದರೆ (ctx == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_ಸ್ಮರಣೆ_R8_G8_B8_A8;
    ಇಲ್ಲದಿದ್ದರೆ
	fmt = GDK_ಸ್ಮರಣೆ_R8_G8_B8;

    image_get_dimensions(img, &w, &h);

    ಗಾತ್ರ_2 = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    ಬೈಟುಗಳು = g_ಬೈಟುಗಳು_new(img->ದತ್ತಾಂಶ, ಗಾತ್ರ_2);
    ctx->texture = gdk_ಸ್ಮರಣೆ_texture_new(w, h, fmt, ಬೈಟುಗಳು, stride);
    g_ಬೈಟುಗಳು_unref(ಬೈಟುಗಳು);

    img->backend_ದತ್ತಾಂಶ = ctx;
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_ದತ್ತಾಂಶ;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_nodes(image_placement_gdk_T *ctx)
{
    ಆದರೆ (ctx->nodes == ಖಾಲಿ || gui.drawarea == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ctx->n_nodes; i++)
    {
	ಆದರೆ (ctx->nodes[i] == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_ಗಂಟು_unref(ctx->nodes[i]);
    }
    g_clear_ಸೂಚಕ(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    ಪೂರ್ಣಾಂಕ
image_placement_gui_init(image_placement_T *ಇರಿಸು)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    ಆದರೆ (ctx == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಇರಿಸು->backend_ದತ್ತಾಂಶ = ctx;
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
image_placement_gui_uninit(image_placement_T *ಇರಿಸು)
{
    image_placement_gdk_T *ctx = ಇರಿಸು->backend_ದತ್ತಾಂಶ;

    clear_nodes(ctx);
    vim_free(ctx);
}

    ಶೂನ್ಯ
image_placement_gui_draw(image_placement_T *ಇರಿಸು)
{
    image_T		    *img = ಇರಿಸು->img;
    image_placement_gdk_T   *ctx = ಇರಿಸು->backend_ದತ್ತಾಂಶ;
    image_gdk_T		    *ictx = img->backend_ದತ್ತಾಂಶ;
    pixman_box32_t	    *rects;
    ಪೂರ್ಣಾಂಕ			    n_rects;
    ಪೂರ್ಣಾಂಕ			    iw, ih;
    Gsk_Render_ಗಂಟು	    *texture_ಗಂಟು;

    rects = pixman_region32_rectangles(&ಇರಿಸು->visible, &n_rects);
    ಆದರೆ (rects == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    clear_nodes(ctx);
    ಆದರೆ (n_rects == 0)
	ಹಿಂತಿರುಗಿ;

    image_get_dimensions(img, &iw, &ih);
    texture_ಗಂಟು = gsk_texture_ಗಂಟು_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(ಇರಿಸು->ಲಂಬಸಾಲು) - FILL_X(ಇರಿಸು->crop_box.x1),
		FILL_Y(ಇರಿಸು->ಅಡ್ಡಸಾಲು) - FILL_Y(ಇರಿಸು->crop_box.y1)
		+ FILL_Y(ಇರಿಸು->ಅಡ್ಡಸಾಲು_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    ಆದರೆ (texture_ಗಂಟು == ಖಾಲಿ)
	// Not sure if this can happen...
	ಹಿಂತಿರುಗಿ;

    ctx->nodes = g_malloc_n(n_rects, ಗಾತ್ರ(Gsk_Render_ಗಂಟು *));
    ctx->n_nodes = n_rects;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ		posx, posy;
	ಪೂರ್ಣಾಂಕ		x, y, w, h;
	Gsk_Render_ಗಂಟು	*ಗಂಟು;

	image_placement_subrect(ಇರಿಸು, rect, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &x, &y, &w, &h, false);
	posx = FILL_X(ಲಂಬಸಾಲು);
	posy = FILL_Y(ಅಡ್ಡಸಾಲು);

	ಗಂಟು = gsk_clip_ಗಂಟು_new(texture_ಗಂಟು,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	ಆದರೆ (ಗಂಟು != ಖಾಲಿ)
	    vim_draw_area_ಕೂಡಿಸು_external(VIM_DRAW_AREA(gui.drawarea), ಗಂಟು);

	ctx->nodes[i] = ಗಂಟು;
    }
    gsk_render_ಗಂಟು_unref(texture_ಗಂಟು);
}

    ಶೂನ್ಯ
image_placement_gui_clear(image_placement_T *ಇರಿಸು)
{
    image_placement_gdk_T   *ctx = ಇರಿಸು->backend_ದತ್ತಾಂಶ;

    clear_nodes(ctx);
}

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_GTK
